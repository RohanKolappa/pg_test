#include <stdio.h>
#include <string.h>

#include "hal.h"
#include "hal_event_device.h"
#include "hal_hwinfo.h"
#include "vutils.h"
#include "commonutils.h"
#include "device_detection.h"
#include "usbutils.h"
#include "kbms_utils.h"

#include "vlog.h"

/* Open the input event devices for the board */
struct list_head *hal_get_event_fd_list(HAL * hp)
{
    int cno = hal_get_board_number(hp) + 1;
    int hw_type = get_hardware_category();

    /* Do nothing if hardware not XP200 family */
    if (hw_type == HARDWARE_CATEGORY_XP || hw_type == HARDWARE_CATEGORY_XPI )
    {
      /* Clean up list if exists */
      if (!list_empty(&(hp->event_head)))
      usb_release_event_devices(hp);

      /* Get all the input event devices for the board */
      if (usb_initialize_event_devices(hp) < 0)
         VLOG_INFO_CAT(VLOG_HAL,
               "Channel %d. Failed to initialize event devices. %m", cno);

      return &(hp->event_head);
    }
    else {
        return NULL;
    }
}

/* Add the event device 'device' to the board */
int hal_add_event_device(HAL *hp, char *device) {
    int rc = 0, cno = hal_get_board_number(hp) + 1;
    int hardware_category = get_hardware_category();

    if (hardware_category == HARDWARE_CATEGORY_XPI || hardware_category == HARDWARE_CATEGORY_XP) {
        rc = usb_update_event_device(hp, device, USB_UPDATE_FLAG_ADD);
        if (rc < 0)
            VLOG_WARNING_CAT(VLOG_HAL,
                "Channel %d. Failed to add event device %s.", cno, device);
        else if (rc > 0)
            VLOG_INFO_CAT(VLOG_HAL,
                "Channel %d. Added event device %s", cno, device);
    }
    return rc;
}

/* Remove the event device 'device' if it's on the board */
int hal_remove_event_device(HAL *hp, char *device) {
    int rc = 0, cno = hal_get_board_number(hp) + 1;
    int hardware_category = get_hardware_category();

    if (hardware_category == HARDWARE_CATEGORY_XPI || hardware_category == HARDWARE_CATEGORY_XP) {
        char name[MAXDEVLEN];
        strncpy(name, device, sizeof(name) - 1);
        name[MAXDEVLEN - 1] = 0;
        rc = usb_update_event_device(hp, device, USB_UPDATE_FLAG_REMOVE);
        if (rc < 0)
            VLOG_WARNING_CAT(VLOG_HAL,
                "Channel %d. Failed to remove event device %s", cno, name);
        else if (rc > 0)
            VLOG_INFO_CAT(VLOG_HAL,
                "Channel %d. Removed event device %s", cno, name);
    }
    return rc;
}                          
