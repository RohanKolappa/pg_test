#ifndef __HALD_H_INCLUDED__
#define __HALD_H_INCLUDED__

typedef struct kb_icon_s {
    int color                   ; /* icon color */
    int position                ; /* icon position */
    int duration                ; /* icon duration */
} KB_ICON;

int hald_main(int argc, char **argv);
#endif /* __HALD_H_INCLUDED__ */
