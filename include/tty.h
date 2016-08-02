void tty_setcursor(int x, int y, int w, int h);
void tty_cls(void);
void tty_scrollup(void);
void tty_putc(char c);
void tty_gotoxy(int x, int y);
void tty_setcolor(unsigned char color);
void tty_write(char* str);
void tty_setup(int w, int h, void* screen);
