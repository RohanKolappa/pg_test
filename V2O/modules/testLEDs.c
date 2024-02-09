
#define CUSTOM_MENU_DEFS
#include "custmenu.h"
#include "led_control.h"
#include "system_utils.h"
#include "testLEDs.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_LED.h"

int testLEDs(MENUINFO * minfo)
{
    DIALOG *dialog;
    int retval = RC_NORMAL;
    int hardware_type, conn_led = 0, video_led = 0;
    int i, expansion_led[MAX_EXPANSION_LED + 1];
    int main_rj45_green = 0, main_rj45_yellow = 0, main_rj45_orange = 0;
    HAL *hp;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    hp = hal_acquire(0);
    if (hp == NULL) {
        return show_fault_dialog(minfo, "Wrong Hardware!\n");
    }

    hardware_type = hal_get_hardware_type(hp);

    /* Get current state of the LEDs */
    if (hardware_type == PLX_DEVICE) {
        int avflag = hal_getAudioVideoFlag(hp);
        conn_led = (avflag & AVFLAG_CONNECTIONLED ? 1 : 0);
        video_led = (avflag & AVFLAG_VIDEOLED) ? 1 : 0;
    }
    else if (hardware_type == XPI_DEVICE) {
        for (i = MIN_EXPANSION_LED; i <= MAX_EXPANSION_LED; i++)
            expansion_led[i] = hal_LED_status(hp, i);
        main_rj45_green = hal_LED_status(hp, MAIN_RJ45_GREEN);
        main_rj45_yellow = hal_LED_status(hp, MAIN_RJ45_YELLOW);
        main_rj45_orange = hal_LED_status(hp, MAIN_RJ45_ORANGE);
    }

    dialog = show_dialog(minfo, 4, 0,
        "</K></U>Warning:<!U><!K> This test will temporarily disable\n"
        "the normal functionality of front panel LEDs.");

    show_info(minfo, "Start LED Test?" YESNO_STR);

    if (get_yesno(minfo)) {
        system_command("/bin/touch /tmp/.testLED");
        remove_dialog(dialog);
        /* Turn all LEDs on */
        hal_LED_all_on(hp);
        dialog = show_dialog(minfo, 4, 0,
                             "Verify that all front panel LEDs are ON.");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        /* Turn all LEDs off */
        hal_LED_all_off(hp);
        dialog = show_dialog(minfo, 4, 0,
                             "Verify only the Power LED is on");
        show_info(minfo, " ");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        /* Restore the LEDs to original state */
        if (hardware_type == PLX_DEVICE) {
            if (conn_led)
                hal_LED_on(hp, CONNECTION_LED);
            else
                hal_LED_off(hp, CONNECTION_LED);
            if (video_led)
                hal_LED_on(hp, VIDEO_LED);
            else
                hal_LED_off(hp, VIDEO_LED);
        }
        else if (hardware_type == XPI_DEVICE) {
            for (i = MIN_EXPANSION_LED; i <= MAX_EXPANSION_LED; i++) {
                if (expansion_led[i] == LED_ON)
                    hal_LED_on(hp, i);
                else if (expansion_led[i] == LED_BLINK)
                    hal_LED_blink(hp, i, 1);
                else if (expansion_led[i] == LED_OFF)
                    hal_LED_off(hp, i);
            }
            if (main_rj45_green == LED_ON)
                hal_LED_on(hp, MAIN_RJ45_GREEN);
            else 
                hal_LED_off(hp, MAIN_RJ45_GREEN);
            if (main_rj45_yellow == LED_ON)
                hal_LED_on(hp, MAIN_RJ45_YELLOW);
            else 
                hal_LED_off(hp, MAIN_RJ45_YELLOW);
            if (main_rj45_orange == LED_ON)
                hal_LED_on(hp, MAIN_RJ45_ORANGE);
            else 
                hal_LED_off(hp, MAIN_RJ45_ORANGE);
        }
        else {
            hal_LED_all_normal(hp);
        }
        system_command("/bin/rm /tmp/.testLED");
    }
    else
        remove_dialog(dialog);

    hal_release(hp);

    return retval;
}
