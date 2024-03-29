#ifndef __LYNX_SD__
#define __LYNX_SD__

typedef enum {
	FR_OK = 0,			/* 0 */
	FR_DISK_ERR,		/* 1 */
	FR_NOT_READY,		/* 2 */
	FR_NO_FILE,			/* 3 */
	FR_NOT_OPENED,		/* 4 */
	FR_NOT_ENABLED,		/* 5 */
	FR_NO_FILESYSTEM	/* 6 */
} FRESULT;

typedef struct {
	unsigned long 	fsize;		/* File size */
	unsigned int 	fdate;		/* Last modified date */
	unsigned int	ftime;		/* Last modified time */
	unsigned char	fattrib;	/* Attribute */
	char	fname[13];	/* File name */
} SFileInfo;

#define	AM_RDO	0x01	/* Read only */
#define	AM_HID	0x02	/* Hidden */
#define	AM_SYS	0x04	/* System */
#define	AM_VOL	0x08	/* Volume label */
#define AM_LFN	0x0F	/* LFN entry */
#define AM_DIR	0x10	/* Directory */
#define AM_ARC	0x20	/* Archive */
#define AM_MASK	0x3F	/* Mask of defined bits */

void LynxSD_Init();
void LynxSD_LowPowerMode();
FRESULT __fastcall__ LynxSD_OpenDir(const char *pDir);
FRESULT __fastcall__ LynxSD_ReadDir(SFileInfo *pInfo);
FRESULT __fastcall__ LynxSD_OpenFile(const char *pFile);
FRESULT __fastcall__ LynxSD_ReadFile(void *pBuffer, unsigned int nSize);
FRESULT __fastcall__ LynxSD_WriteFile(void *pBuffer, unsigned int nSize);
FRESULT LynxSD_CloseFile();
FRESULT __fastcall__ LynxSD_SeekFile(unsigned long nSeekPos);
unsigned long LynxSD_GetFileSize();
FRESULT __fastcall__ LynxSD_ProgramROMFromFile(unsigned int nStartBlock, unsigned char nBlockSize, unsigned int nBlockCount, unsigned char b512BlockCard);
FRESULT __fastcall__ LynxSD_ClearROMBlocks(unsigned int nStartBlock, unsigned int nBlocks, unsigned char b512BlockCard);
FRESULT __fastcall__ LynxSD_OpenFileTimeout(void *pBuffer);

#endif // __LYNX_SD__
