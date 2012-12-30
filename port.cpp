#include <stdlib.h>
#include <unistd.h>
#include <strings.h>
#include <stdarg.h>
#include <locale.h>
#include <ncurses.h>

#include "colours.h"
#include "port.h"

static WINDOW *win = NULL;
static int begin_x;
static int begin_y;

static unsigned short cp437[256] =
{
    0x0000, 0x263a, 0x263b, 0x2665, 0x2666, 0x2663, 0x2660, 0x2022,
    0x25d8, 0x25cb, 0x25d9, 0x2642, 0x2640, 0x266a, 0x266b, 0x263c,
    0x25b6, 0x25c0, 0x2195, 0x203c, 0x00b6, 0x00a7, 0x25ac, 0x21a8,
    0x2191, 0x2193, 0x2192, 0x2190, 0x221f, 0x2194, 0x25b2, 0x25bc,
    0x0020, 0x0021, 0x0022, 0x0023, 0x0024, 0x0025, 0x0026, 0x0027,
    0x0028, 0x0029, 0x002a, 0x002b, 0x002c, 0x002d, 0x002e, 0x002f,
    0x0030, 0x0031, 0x0032, 0x0033, 0x0034, 0x0035, 0x0036, 0x0037,
    0x0038, 0x0039, 0x003a, 0x003b, 0x003c, 0x003d, 0x003e, 0x003f,
    0x0040, 0x0041, 0x0042, 0x0043, 0x0044, 0x0045, 0x0046, 0x0047,
    0x0048, 0x0049, 0x004a, 0x004b, 0x004c, 0x004d, 0x004e, 0x004f,
    0x0050, 0x0051, 0x0052, 0x0053, 0x0054, 0x0055, 0x0056, 0x0057,
    0x0058, 0x0059, 0x005a, 0x005b, 0x005c, 0x005d, 0x005e, 0x005f,
    0x0060, 0x0061, 0x0062, 0x0063, 0x0064, 0x0065, 0x0066, 0x0067,
    0x0068, 0x0069, 0x006a, 0x006b, 0x006c, 0x006d, 0x006e, 0x006f,
    0x0070, 0x0071, 0x0072, 0x0073, 0x0074, 0x0075, 0x0076, 0x0077,
    0x0078, 0x0079, 0x007a, 0x007b, 0x007c, 0x007d, 0x007e, 0x2302,
    0x00c7, 0x00fc, 0x00e9, 0x00e2, 0x00e4, 0x00e0, 0x00e5, 0x00e7,
    0x00ea, 0x00eb, 0x00e8, 0x00ef, 0x00ee, 0x00ec, 0x00c4, 0x00c5,
    0x00c9, 0x00e6, 0x00c6, 0x00f4, 0x00f6, 0x00f2, 0x00fb, 0x00f9,
    0x00ff, 0x00d6, 0x00dc, 0x00a2, 0x00a3, 0x00a5, 0x20a7, 0x0192,
    0x00e1, 0x00ed, 0x00f3, 0x00fa, 0x00f1, 0x00d1, 0x00aa, 0x00ba,
    0x00bf, 0x2310, 0x00ac, 0x00bd, 0x00bc, 0x00a1, 0x00ab, 0x00bb,
    0x2591, 0x2592, 0x2593, 0x2502, 0x2524, 0x2561, 0x2562, 0x2556,
    0x2555, 0x2563, 0x2551, 0x2557, 0x255d, 0x255c, 0x255b, 0x2510,
    0x2514, 0x2534, 0x252c, 0x251c, 0x2500, 0x253c, 0x255e, 0x255f,
    0x255a, 0x2554, 0x2569, 0x2566, 0x2560, 0x2550, 0x256c, 0x2567,
    0x2568, 0x2564, 0x2565, 0x2559, 0x2558, 0x2552, 0x2553, 0x256b,
    0x256a, 0x2518, 0x250c, 0x2588, 0x2584, 0x258c, 0x2590, 0x2580,
    0x03b1, 0x00df, 0x0393, 0x03c0, 0x03a3, 0x03c3, 0x00b5, 0x03c4,
    0x03a6, 0x0398, 0x03a9, 0x03b4, 0x221e, 0x03c6, 0x03b5, 0x2229,
    0x2261, 0x00b1, 0x2265, 0x2264, 0x2320, 0x2321, 0x00f7, 0x2248,
    0x00b0, 0x2219, 0x00b7, 0x221a, 0x207f, 0x00b2, 0x25a0, 0x00a0,
};

char *itoa(int n, char *s, int radix)
{
    if (n == 0)
    {
        *s++ = '0';
        *s++ = 0;
        return s;
    }

    if (n < 0)
    {
        *s++ = '-';
        n = -n;
    }

    for (int tmp = 10; tmp <= n; tmp *= radix)
        s++;

    *++s = 0;

    while (n)
    {
        int d = n % radix;
        n /= 10;
        *--s = '0' + d;
    }

    return s;
}

int stricmp(char *s1, char *s2)
{
    return strcasecmp(s1, s2);
}

void startup()
{
    setlocale(LC_CTYPE, "");
    win = initscr();
    atexit(endwin);
    cbreak();
    noecho();
    nonl();
    intrflush(win, FALSE);
    meta(win, TRUE);
}

int kbhit(void)
{
    int c;

    nodelay(win, TRUE);
    wtimeout(win, 0);
    c = wgetch(win);
    nodelay(win, FALSE);

    if (c == -1)
        c = 0;
    else
        ungetch(c);

    return c;
}

int putch(unsigned char c)
{
    wchar_t w = cp437[c];
    return waddnwstr(win, &w, 1);
}

int getche(void)
{
    int c = wgetch(win);
    waddch(win, c);
    wrefresh(win);
    return c;
}

int cprintf(const char *fmt, ...)
{
    int ret;
    char buf[2048];
    va_list ap;
    va_start(ap, fmt);
    vsnprintf(buf, sizeof buf, fmt, ap);
    va_end(ap);
    // FIXME: This should probably do CP437 translation
    ret = waddstr(win, buf);
    wrefresh(win);
    return ret;
}

// TODO: Should these use screen or window-relative coordinates?
// (probably the latter)

void gotoxy(int x, int y)
{
    //wmove(win, begin_y + y - 1, begin_x + x - 1);
    wmove(win, y-1, x-1);
}

int wherex()
{
    int x, y;
    getyx(win, y, x);
    return begin_x + x + 1;
}

int wherey()
{
    int x, y;
    getyx(win, y, x);
    return begin_y + y + 1;
}

void window(int left, int top, int right, int bottom)
{
    if (win != NULL && win != stdscr)
        delwin(win);
    begin_y = top - 1;
    begin_x = left - 1;
    int lines = bottom - top + 1;
    int columns = right - left + 1;
    win = subwin(stdscr, lines, columns, begin_y, begin_x);
}

void gettext(int left, int top, int right, int bottom, unsigned char *dest)
{
    int orig_y, orig_x;
    getyx(stdscr, orig_y, orig_x);
    for (int row = top - 1; row <= bottom - 1; row++)
    {
        for (int col = left - 1; col <= right - 1; col++)
        {
            // FIXME make this not crap
            // FIXME make it handle attributes too
            wchar_t w;
            mvinnwstr(row, col, &w, 1);
            for (unsigned int i = 0; i < 256; i++)
            {
                if (cp437[i] == w)
                {
                    *dest++ = w;
                }
            }
            //*dest == 'q';
            //*dest++ = mvinch(row, col) & A_CHARTEXT;
            *dest++ = 0;
        }
    }
    move(orig_y, orig_x);
}

void puttext(int left, int top, int right, int bottom, unsigned char *str)
{
    int orig_y, orig_x;
    getyx(stdscr, orig_y, orig_x);
    for (int y = top - 1; y <= bottom - 1; y++)
    {
        for (int x = left - 1; x <= right - 1; x++)
        {
            unsigned char chr = *str++;
            unsigned char col = *str++;
            wchar_t w = cp437[chr];
            if (w == 0) w = (wchar_t)' ';
            textcolor(col);
            mvaddnwstr(y, x, &w, 1);
        }
    }
    move(orig_y, orig_x);
}

static int curses_colour(int col)
{
    switch (col)
    {
    case BLACK:        return COLOR_BLACK;
    case BLUE:         return COLOR_BLUE;
    case GREEN:        return COLOR_GREEN;
    case CYAN:         return COLOR_CYAN;
    case RED:          return COLOR_RED;
    case MAGENTA:      return COLOR_MAGENTA;
    case BROWN:        return COLOR_YELLOW;
    case LIGHTGRAY:    return COLOR_WHITE;
    case DARKGRAY:     return (COLOR_BLACK   | A_BOLD);
    case LIGHTBLUE:    return (COLOR_BLUE    | A_BOLD);
    case LIGHTGREEN:   return (COLOR_GREEN   | A_BOLD);
    case LIGHTCYAN:    return (COLOR_CYAN    | A_BOLD);
    case LIGHTRED:     return (COLOR_RED     | A_BOLD);
    case LIGHTMAGENTA: return (COLOR_MAGENTA | A_BOLD);
    case YELLOW:       return (COLOR_YELLOW  | A_BOLD);
    case WHITE:        return (COLOR_WHITE   | A_BOLD);
    }
}

void textcolor(int col)
{
    int ccol = curses_colour(col);
    int pair = ccol == 0 ? 63 : ccol;
    wattrset(win, COLOR_PAIR(pair));
}

void textbackground(int col)
{
    int ccol = curses_colour(col);
    int pair = ccol == 0 ? 63 : ccol * 8;
    wattrset(win, COLOR_PAIR(pair));
}

void clrscr()
{
    textcolor(LIGHTGRAY);
    textbackground(BLACK);
    wclear(win);
}

void delay(int time)
{
    usleep(time * 1000);
}

void _setcursortype(int cursortype)
{
    curs_set(cursortype);
}
