#include<system.h>
#include<types.h>
#include<portio.h>

/* --- static variables --- */
int tty_w=80, tty_h=25; /* terminal dimensions (see: video bios mode 3) */
int tty_color=0x7;      /* dark white color (see: vga palette) */
int tty_x=0, tty_y=0;   /* starting cursor position */

unsigned short int* tty_scr=(unsigned short int*)0xb8000;
                        /* identity mapping and color card assumed */
                        /* for mono, use 0xb0000; see: tty_setup() */

/* --- the only function, that needs direct i/o port (hw. driver) access --- */
void tty_setcursor(int x, int y, int w, int h)
{
 //vga_setcursor(x+y*w);               /* the hw. uses linear cursor offsets */

		unsigned short position=(x*80) + y;

		// cursor LOW port to vga INDEX register
		outportb(0x3D4, 0x0F);
		outportb(0x3D5, (unsigned char)(position&0xFF));
		// cursor HIGH port to vga INDEX register
		outportb(0x3D4, 0x0E);
		outportb(0x3D5, (unsigned char)((position>>8)&0xFF));
}

/* --- clear the screen by filling it with spaces --- */
void tty_cls(void)
{
 int t;
 for(t=0; t < tty_w*tty_h; t++)      /* clear the screen */
  tty_scr[t]=' ' | (tty_color<<8);
 tty_x=0; tty_y=0;                   /* set cursor to home location */
 tty_setcursor(0, 0, tty_w, tty_h);  /* call the hw. cursor setup */
}

/* --- scroll the screen up one line --- */
void tty_scrollup(void)
{
 int t;
 for(t=0; t < tty_w*(tty_h-1); t++)  /* scroll every line up */
  tty_scr[t]=tty_scr[t+tty_w];
 for(; t < tty_w*tty_h; t++)         /* clear the bottom line */
  tty_scr[t]=' ' | (tty_color<<8);
}

/* --- print one character on the screen --- */
void tty_putc(char c)
{
 int t;
 switch(c)
 {
  case '\r':                         /* -> carriage return */
    tty_x=0;
    break;

  case '\n':                         /* -> newline (with implicit cr) */
    tty_x=0;
    tty_y++;
    break;

  case 8:                            /* -> backspace */
    t=tty_x+tty_y*tty_w;             /* get linear address */
    if(t>0) t--;                     /* if not in home position, step back */
    tty_x=t%tty_w;
    tty_y=t/tty_w;
    tty_scr[t]=' ' | (tty_color<<8); /* put space under the cursor */
    break;

  default:                           /* -> all other characters */
    if(c<' ') break;                 /* ignore non printable ascii chars */
    tty_scr[tty_x+tty_y*tty_w]=c | (tty_color<<8);
    tty_x++;                         /* step cursor one character */
    if(tty_x == tty_w)               /* to next line if required */
    {
     tty_x=0;
     tty_y++;
    }
    break;
 }

 if(tty_y == tty_h)                  /* the cursor moved off of the screen? */
 {
  tty_scrollup();                    /* scroll the screen up */
  tty_y--;                           /* and move the cursor back */
 }
                                     /* and finally, set the cursor */
 tty_setcursor(tty_x, tty_y, tty_w, tty_h);
}

/* some misc. functions */

void tty_gotoxy(int x, int y)
{
 tty_x=x % tty_w;
 tty_y=y % tty_h;
 tty_setcursor(tty_x, tty_y, tty_w, tty_h);
}

void tty_setcolor(unsigned char color)
{
 tty_color=color;                    /* see: ega/vga palette (text mode) */
}

void tty_write(char* str)
{
 while((*str) !=0 ) tty_putc(*(str++));
}

/* --- optional setup, if the code above is used with a different mode --- */
void tty_setup(int w, int h, void* screen)
{
 tty_w=w; tty_h=h;
 tty_x=0; tty_y=0;
 /* this is the screen base address as seen by kernel */
 tty_scr=(unsigned short int*)screen;
 tty_cls();
}
