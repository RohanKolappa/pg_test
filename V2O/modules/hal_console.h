#ifndef __HAL_CONSOLE_H__
#define __HAL_CONSOLE_H__

#ifdef __cplusplus
extern "C" {
#endif

    extern int hal_get_console_fd(HAL * hp);
    extern int hal_kbm_switchin(HAL *hp);
    extern int hal_kbm_switchout(HAL *hp);
    extern int hal_update_virtual_console(HAL *hp);
    extern int hal_get_current_kbmode(HAL *hp);
    extern void hal_initialize_console(HAL *hp);
    extern void hal_close_console(HAL *hp);
#ifdef __cplusplus
}
#endif

#endif                          // __HAL_CONSOLE_H__
