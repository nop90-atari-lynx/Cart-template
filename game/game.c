#include <lynx.h>
#include <conio.h>
#include <joystick.h>
#include <tgi.h>
#include <stdlib.h>

unsigned char checkInput(void);
extern unsigned char reset; 

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

extern unsigned char tree[];

static SCB_REHV_PAL Stree = {
    BPP_4 | TYPE_NORMAL,
    0x10, 0x20,
    0,
    tree,
    0, 0,
    0x0100, 0x100,
    {0x01, 0x23, 0x45, 0x67, 0x89, 0xab, 0xcd, 0xef}
};


/* ----------------- Road -------------------- */
unsigned char pixel[] = { 3, 0x84, 0, 0 };

static SCB_REHVST_PAL Spixel = {
    BPP_1 | TYPE_BACKGROUND,
    0x30, 0x20,
    0,
    pixel,
    50, 50,
    0x2800, 0x800,
    0x00, 0x100,
    {0x68}
};

void polygon(int x1, int y1, int w1, int x2, int y2, int w2, unsigned char color)
{
    Spixel.hpos = x1;
    Spixel.vpos = y1;
    Spixel.hsize = w1 << 8;
    Spixel.vsize = (y2 - y1 + 1) << 8;
    Spixel.tilt = (x2 - x1) * 256 / (y2 - y1);
    Spixel.stretch = (w2 - w1) * 256 / (y2 - y1);
    Spixel.penpal[0] = color;
    tgi_sprite(&Spixel);
}

void
drawsegment(int lanes, int x1, int y1, int w1, int x2, int y2, int w2)
{
    int l1;
    int l2;
    int r1;
    int r2;
    int lanew1;
    int lanew2;
    int lanex1;
    int lanex2;
    int lane;
    l1 = w1 >> (lanes + 2);
    l2 = w2 >> (lanes + 2);
    r1 = w1 >> (lanes + 1);
    r2 = w2 >> (lanes + 1);
    // Draw left yellow rumble line
    polygon(x1 - w1 - r1 + 1, y1, r1, x2 - w2 - r2 + 1, y2, r2, COLOR_YELLOW);
    // Draw road
    polygon(x1 - w1, y1, w1, x2 - w2, y2, w2, COLOR_GREY);
    lanew1 = w1 * 2 / lanes;
    lanew2 = w2 * 2 / lanes;
    lanex1 = x1 - w1 + lanew1;
    lanex2 = x2 - w2 + lanew2;
    for (lane = 1; lane < lanes; lanex1 += lanew1, lanex2 += lanew2, lane++) {
        polygon(lanex1 - l1 / 2, y1, l1, lanex2 - l2 / 2, y2, l2, COLOR_WHITE);
    }
    polygon(x1 - 1 + l1 / 2, y1, w1, x2 - 1 + l2 / 2, y2, w2, COLOR_GREY);
    // Draw right yellow rumble line
    polygon(x1 + w1, y1, r1, x2 + w2, y2, r2, COLOR_YELLOW);
    // Draw right cleanup
    polygon(x1 + w1 + r1 - 1, y1, r1, x2 + w2 + r2 - 1, y2, r2, COLOR_DARKGREY);
}

static void draw_objects(int x, int y, int scale)
{
    Stree.vpos = y;
    Stree.hpos = x + 110 * scale / 256;
    Stree.vsize = scale/5;
    Stree.hsize = scale/5;
    tgi_sprite(&Stree);
    Stree.hpos = x - 110 * scale / 256;
    tgi_sprite(&Stree);
}

void
drawscreen(int x, int y, int turn)
{
    int y2;
    int scale1, scale2;
    scale1 = 3 * y + 8;
    y2 = y + ((20 * scale1) >> 8);
    scale2 = 3 * y2 + 8;
    tgi_setcolor(COLOR_DARKGREY);
    tgi_bar(0, 0, 159, 101);
    drawsegment(2, x, y, (80 * scale1) >> 8, x + turn, y2, (80 * scale2) >> 8);
    draw_objects(x + turn, y2, scale2);
}

void game()
{
    int x = 80;
    int y = 50;
    int turn = 0;

    while (!reset) { // never use while(1). Every loop needs to be halted by the reset event
        if (!tgi_busy()) {
            unsigned char joy;
            
// Instead of calling joy_read() directly, use checkInput() defined in resident.c . 
// It will return the joy state, but will handle pause and reset events too
            
//            joy = joy_read(JOY_1);
            joy = checkInput();
            if (JOY_BTN_UP(joy)) {
                y--;
                if (y < 0) y = 0;
                lynx_snd_play(1, musicptr.music1);
            }
            if (JOY_BTN_DOWN(joy)) {
                y++;
                if (y > 101) y = 101;
                lynx_snd_play(1, musicptr.music2);
            }
            if (JOY_BTN_RIGHT(joy)) {
                x++;
            }
            if (JOY_BTN_LEFT(joy)) {
                x--;
                lynx_snd_play(1, musicptr.music3);
            }
            if (JOY_BTN_FIRE(joy)) {
                turn++;
            }
            if (JOY_BTN_FIRE2(joy)) {
                turn--;
            }
            drawscreen(x, y, turn);
            tgi_updatedisplay();
        }
    }
}



