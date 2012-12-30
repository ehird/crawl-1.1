char *itoa(int value, char *string, int radix);
int stricmp(char *s1, char *s2);
void startup();
int kbhit(void);
int putch(unsigned char c);
int getche(void);
int cprintf(const char *fmt, ...);
void gotoxy(int x, int y);
int wherex();
int wherey();
void window(int left, int top, int right, int bottom);
void gettext(int left, int top, int right, int bottom, unsigned char *dest);
void puttext(int left, int top, int right, int bottom, unsigned char *str); // maybe wrong, see http://www.codeforge.com/read/47922/PUTTEXT.C__html
void textcolor(int col);
void textbackground(int col);
void clrscr(void);
void delay(int time);
void _setcursortype(int cursor_type);
#define _NOCURSOR 0
#define _NORMALCURSOR 1
#define O_BINARY 0
