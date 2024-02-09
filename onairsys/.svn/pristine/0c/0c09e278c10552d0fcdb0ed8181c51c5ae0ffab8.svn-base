#ifndef	SMCLCD_H
#define	SMCLCD_H

#define	SL_SUCCESS	0
#define	SL_FAIL		-1

#define	SL_NO_WAIT	50

/* sl_errno */
#define	SLE_NOERROR	0
#define	SLE_USBFS	1
#define	SLE_OPEN	2
#define	SLE_READ	3
#define	SLE_FTW		4
#define	SLE_ALREADYINIT	5
#define	SLE_MEMORY	6
#define	SLE_IOCTL	7
#define	SLE_CLAIM	8
#define	SLE_NOTINIT	9
#define	SLE_PROTOCOL	10
#define	SLE_NOTFOUND	11

typedef struct {
	int	initialized;
	char	*name;
	int	intepin;
	int	intepout;
	int	interface;
	int	fd;
} SLHandle;

typedef enum {
	SLCT_OFF	= 0,	/* hide cursor */
	SLCT_UNDERLINE	= 2,	/* display underline cursor */
	SLCT_BLINK	= 3,	/* display blinking cursor */
} SLCursorType;

typedef enum {
	SLLT_OFF	= 0,	/* turn off the LED */
	SLLT_GREEN	= 1,	/* turn on the LED in green */
	SLLT_YELLOW	= 2,	/* turn on the LED in yellow */
} SLLedType;

typedef enum {
	SL_OFF		= 0,	/* turn the backlight off */
	SL_ON		= 1,	/* turn the backlight on */
} SLOnOff;

typedef enum {
	SLK_ERROR	= -1,	/* error, sl_errno is set appropriately. */
	SLK_NOKEY	= -2,	/* no key */
    SLK_UP      = 0,    /* key 1 pressed */
    SLK_RIGHT   = 1,    /* key 2 pressed */
    SLK_LEFT    = 2,    /* key 3 pressed */
    SLK_DOWN    = 3,    /* key 4 pressed */
    SLK_ENTER   = 4,    /* key 5 pressed */
    SLK_CANCEL  = 5,    /* key 6 pressed */
} SLKey;

extern int sl_errno;

SLHandle *sl_init(void);
void  sl_close(SLHandle *slh);
int sl_clear(SLHandle *slh);
int sl_home (SLHandle *slh);
int sl_set_cursor (SLHandle *slh, SLCursorType slct);
int sl_get_key(SLHandle *slh, int timeout);
int sl_set_led (SLHandle *slh, SLLedType sllt);
int sl_set_backlight (SLHandle *slh, SLOnOff mode);
int sl_move_cursor (SLHandle *slh, int x, int y);
int sl_putch (SLHandle *slh, int ch);
int sl_printf (SLHandle *slh, char *fmt, ...);
int sl_mvprintf (SLHandle *slh, int x, int y, char *fmt, ...);
const char *sl_strerror(int sl_errno);
int sl_getch(SLHandle *slh, unsigned char DDRam, unsigned char *c);
int sl_getline(SLHandle *slh, unsigned char LineNum, unsigned char *InBuf);
int sl_getrevision(SLHandle *slh, unsigned int *Revision);

#endif	/* SMCLCD_H */
