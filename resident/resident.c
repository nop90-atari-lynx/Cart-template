/*
 * The code and data defined here is placed in resident RAM
 */
#include <lynx.h>
#include <stdlib.h>
#include <string.h>
#include <conio.h>
#include <tgi.h>
#include <joystick.h>
#include <6502.h>
#include "LynxSD.h"

#define EEPROM_MAGIC "GAME" // Change this to your own string. 3-5 chars are fine.
#define ISEMULATOR 0xfd97 // Fake register used to know if running on emlator (Handy based). Handy returns 42 insted of 0xFF 
#define IODIR ((volatile unsigned char *) 0xFD8A)

signed char SDCheck = -1; // tracks if there is a SD cart with saves enabled. -1 = No check yet, 0 = no SD saves, 1 = SD saves possible 
const char SDSavePath[] = "/saves/game.sav"; //rename game.sav to yourprogramname.sav, i.e. the same name of your lnx file
unsigned int saveBuf[64]; // Buffer for storing SaveData in memory. Only the first 6 bytes are used for checking if data is valid, for the others there is no predefined data structure. Use it as you want

unsigned char halted = 0; // for pause event
unsigned char reset = 0; // for reset event

// For reading writing EEPROM. The data is always 2 bytes, the address range 0..63
extern int __fastcall__ lnx_eeprom_read(unsigned char pos);
extern void __fastcall__ lnx_eeprom_write(unsigned char pos, int val);

void lynx_snd_init ();
void lynx_snd_pause ();
void lynx_snd_continue ();
void __fastcall__ lynx_snd_play (unsigned char channel, unsigned char *music);
void lynx_snd_stop ();

typedef struct {
    unsigned char *music0;
    unsigned char *music1;
    unsigned char *music2;
    unsigned char *music3;
} song_t;

extern song_t musicptr;

extern int intro();
extern int game();

extern int INTRO_FILENR;
extern int GAME_FILENR;
extern int TUNE0_FILENR;

unsigned char backupPal[32]; // stores the palette before graying the screen in pause mode
unsigned char grayPal[32]; // stores the grayed palette for the pause mode derived from the active palette

void eewrite(unsigned char pos, unsigned int val)
{
	unsigned int check;
	check = lnx_eeprom_read(pos);
	if(check!=val)
		lnx_eeprom_write(pos,val);
}

void writeSaveData(void)
{
	unsigned char i;

	if(SDCheck==1) // use SD
	{
		if (LynxSD_OpenFile(SDSavePath) == FR_OK) {
			LynxSD_WriteFile((void *)saveBuf, 128);
			LynxSD_CloseFile();
		}	
	}
	else if(SDCheck==0) // Try to use EEPROM
	{
		for(i=0;i<64;i++)
		{
			eewrite(i,saveBuf[i]);
		}
	}
}

void readSaveData(void)
{
	unsigned char i;
	unsigned char * isEmulator = ISEMULATOR;

	FRESULT res;

	if(SDCheck==-1) // SD not tested yet
	{
		if(*isEmulator!=0xFF)
		{
			// Handy returns 42 reading this fake register. If the code is run in Handy we don't use SD because Handy old code hangs.
			// New emulators based on handy seems to have this fixed
			SDCheck=0;
		}
		else
		{
			LynxSD_Init();
			res = LynxSD_OpenFileTimeout(SDSavePath); 
			if(res == FR_OK)
				SDCheck=2; // SD savefile is ok and next SD access doesn't need to open the file (already opened)
			else if(res==FR_DISK_ERR) // SD read Timeout -> NO SD
			{
				SDCheck=0; // Try to use EEPROM, if there isn't an EEPROM the functions calls are safe because they don't freeze the code 
				*IODIR = 0xFF; // LynxSD_Init() sets IODIR to 0 that blocks emulated eeprom on emulators, so we set it to 0xFF again before reading the eeprom.
			}
			else	
			{
				SDCheck=-2; // The SD answers, but there is no sav file, or some other problem occoured accessing the file -> don't try to use SD and EEPROM anymore (EEPROM calls on SD frezes the code)
			}
		}
	}

	if (SDCheck==0)	// Read the EEPROM	
	{ 
		for(i=0;i<64;i++)
		{
			saveBuf[i]= lnx_eeprom_read(i);
		}
	}
	else if(SDCheck>0) // the value can be 1 or 2
	{
		if(SDCheck==1) //SD sav file enabled
		{
			if (LynxSD_OpenFile(SDSavePath) == FR_OK)
			{
				LynxSD_ReadFile((void *)saveBuf, 128);
				LynxSD_CloseFile();	
			}
		}
		else // //SD sav file enabled and sav file already opened
		{
			SDCheck=1;
			LynxSD_ReadFile((void *)saveBuf, 128);
			LynxSD_CloseFile();	
		}
	}
}

void resetSaveData(void)
{
    int i;
    saveBuf[3]=0;
    strcpy((char*)saveBuf,EEPROM_MAGIC);
	for(i=14;i<64;i++)
	{
		saveBuf[i]=0; //instead of this you could initialize the savedata with some starting values, like a predefined highscores table.
	}
	writeSaveData();
}



int main(void)
{
    tgi_install(&tgi_static_stddrv);
    tgi_init();
    joy_install(&joy_static_stddrv);
    lynx_snd_init();
    CLI();
	
	readSaveData();
	if(strcmp((char*)saveBuf,EEPROM_MAGIC)!=0)
		resetSaveData();

    lynx_load((int)&TUNE0_FILENR);
#if 1
    lynx_snd_pause();
    lynx_snd_play(0, musicptr.music0);
    lynx_snd_play(1, musicptr.music1);
    lynx_snd_play(2, musicptr.music2);
    lynx_snd_play(3, musicptr.music3);
    lynx_snd_continue();
#endif

    while (1) {

		reset = 0;
		halted = 0;

        lynx_load((int)&INTRO_FILENR);
        intro();
        lynx_load((int)&GAME_FILENR);
        game();
    }
    return 0;
}

unsigned char checkInput(void)
{
	unsigned int col;
	unsigned char i;
	const unsigned char* pal;
	 
	if(!reset)
	do
	{
		if (kbhit()) 
		{
			switch (cgetc()) 
			{
			case 'F':
				tgi_flip();
				break;
			case 'P':
				if(halted)
				{
					halted = 0;
					tgi_setpalette(backupPal); // restore normal palette. No need to wait vsync; data in the screenbuffer are valid
					lynx_snd_continue ();					
				}
				else
				{
					pal = tgi_getpalette(); // let's backup the palette
					for (i=0;i<	16;i++) 
// A simple grayed palette is obtained setting the new r,g, b to the g*0,5 + r*0,25 + b*0,25 . 
// This is an approximate formula that gives a good result considering that there are only 16 shades of gray.					
					{
						backupPal[i] = pal[i];
 						backupPal[i+16] = pal[i+16];
						col = ((backupPal[i]&0xf)*2 + (backupPal[i+16]&0xf) + (backupPal[i+16]>>4)) >> 2;
						grayPal[i] = col;
						grayPal[i+16] = col | (col<<4);
					}	
					tgi_setpalette(grayPal); // set gray palette
					
					halted = 1;
					lynx_snd_pause ();
				}
				break;
			case 'R':
				if(halted)
				{
					halted = 0;
					tgi_setpalette(backupPal); //restore normal palette
					lynx_snd_continue ();					
				}
				lynx_snd_stop();
				reset=1;
				break;
  
			case '1': 
				break;
			case '2':
				break;

			case '3':// used to clear saves on eeprom pressing Opt1 + Opt2 while game is paused
				if(halted)
				{
					resetSaveData();					
				}
				break;

			case '?': 
				break;

			default:
				break;
			}
		}
	} while(halted && !reset);
	
	return joy_read(JOY_1);
}
