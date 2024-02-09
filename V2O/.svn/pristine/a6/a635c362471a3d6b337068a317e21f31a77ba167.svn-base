#ifndef __OSD_CONTROL_H__
#define __OSD_CONTROL_H__

#define OSD1                    0       /* OSD # 1 */
#define OSD2                    1       /* OSD # 2 */
#define DEFAULT_ICON_OSD        OSD2    /* OSD for keyboard & guideruler icon */

#define DEFAULT_KEYBOARD_ICON_FILE      "/V2O/config/keyboard.bmp"
#define DEFAULT_KEYBOARD_ICON_COLOR     "Red"
#define DEFAULT_KEYBOARD_ICON_STARTX    4
#define DEFAULT_KEYBOARD_ICON_STARTY    4
#define DEFAULT_KEYBOARD_ICON_ENDX      4
#define DEFAULT_KEYBOARD_ICON_ENDY      4
#define DEFAULT_KEYBOARD_ICON_SIZE      48
#define DEFAULT_GUIDERULER_ICON_FILE    "/V2O/config/guideruler.bmp"
#define DEFAULT_GUIDERULER_ICON_COLOR   "Red"
#define DEFAULT_GUIDERULER_ICON_STARTX  0
#define DEFAULT_GUIDERULER_ICON_STARTY  0
#define DEFAULT_GUIDERULER_ICON_SIZEX   104
#define DEFAULT_GUIDERULER_ICON_SIZEY   72
#define DEFAULT_STRING_ICON_COLOR       "Blue"
#define DEFAULT_STRING_ICON_WIDTH       256
#define DEFAULT_STRING_ICON_HEIGHT      32
#define DEFAULT_STRING_ICON_STARTX      0
#define DEFAULT_STRING_ICON_STARTY      0
#define DEFAULT_STRING_ICON_DELTA       5

#define MAX_MULTIMOUSE          16      /* Max supported multimouse OSDs */
#define MOUSECURSOR_NUM         0       /* Local mouse cursor number */
#define PIX1CURSOR_NUM          1       /* Pixel cursor number 1 */
#define PIX2CURSOR_NUM          2       /* Pixel cursor number 2 */
#define PIX3CURSOR_NUM          3       /* Pixel cursor number 3 */
#define MOUSECURSOR_SIZE        5       /* Local mouse cursor size in pixels */
#define MOUSECURSOR_CENTER      2       /* Local mouse cursor center in pixels */
#define PIXCURSOR_SIZE          15      /* Pixel cursor size in pixels */
#define PIXCURSOR_CENTER        7       /* Pixel cursor center in pixels */
#define REMOTECURSOR_FLAG       0x8000  /* Remote cursor flag */
#define MOUSECURSOR             (REMOTECURSOR_FLAG | MOUSECURSOR_NUM)
#define LOCALPIX1CURSOR         PIX1CURSOR_NUM
#define LOCALPIX2CURSOR         PIX2CURSOR_NUM
#define LOCALPIX3CURSOR         PIX3CURSOR_NUM
#define REMOTEPIX1CURSOR        (REMOTECURSOR_FLAG | PIX1CURSOR_NUM)
#define REMOTEPIX2CURSOR        (REMOTECURSOR_FLAG | PIX2CURSOR_NUM)
#define REMOTEPIX3CURSOR        (REMOTECURSOR_FLAG | PIX3CURSOR_NUM)
#define DEFAULTCURSOR_SIZE      9
#define DEFAULTCURSOR_CENTER    4

#ifdef __cplusplus
extern "C" {
#endif
    void hal_move_icon_xpos(HAL *hp, unsigned short x);
    void hal_move_icon_ypos(HAL *hp, unsigned short y);
    void hal_set_icon_xpos(HAL *hp, unsigned short x, unsigned short sizex);
    void hal_set_icon_ypos(HAL *hp, unsigned short y, unsigned short sizey);
    void hal_set_icon_color(HAL *hp, unsigned short color);
    void hal_set_icon_on(HAL *hp, 
                         int color, 
                         int x, 
                         int y, 
                         int sizex, 
                         int sizey);
    void hal_set_icon_off(HAL *hp);
    int hal_init_keyboard_icon(HAL *hp, int state, int color);
    int hal_init_guideruler_icon(HAL *hp, int state, int color);
    int hal_init_string_icon(HAL *hp, int state, int color, char *str);
    void hal_init_mouse_cursor(HAL *hp, 
                               unsigned short x,
                               unsigned short y,
                               unsigned short maxx,
                               unsigned short maxy);
    void hal_set_mouse_cursor_pos(HAL *hp, 
                                  unsigned short x, 
                                  unsigned short y,
                                  unsigned short maxx,
                                  unsigned short maxy);
    void hal_set_mouse_cursor_status(HAL *hp, int status);
    void hal_init_pix_cursor(HAL *hp, 
                             unsigned short x,
                             unsigned short y,
                             unsigned short maxx,
                             unsigned short maxy,
                             int cursor);
    void hal_set_pix_cursor_status(HAL *hp, int status, int cursor);
    void hal_init_multimouse_cursor(HAL *hp, 
                                    unsigned short x,
                                    unsigned short y,
                                    unsigned short maxx,
                                    unsigned short maxy,
                                    int cursor);
    void hal_set_multimouse_cursor_pos(HAL *hp, 
                                       unsigned short x, 
                                       unsigned short y,
                                       unsigned short maxx,
                                       unsigned short maxy,
                                       int cursor);
    void hal_set_multimouse_cursor_status(HAL *hp, int status, int cursor);

#ifdef __cplusplus
}
#endif

#endif  /* __OSD_CONTROL_H__ */
