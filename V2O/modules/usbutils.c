#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdint.h>
#include <unistd.h>
#include <netinet/in.h>
#include <ctype.h>
#include <fcntl.h>
#include <dirent.h>
#include <sys/stat.h>
#include <linux/input.h>
#ifndef __XPI__
#include <usb.h>
#endif
#include <errno.h>

#include "hal.h"
#include "hal_hwinfo.h"
#include "usbutils.h"
#include "vutils.h"
#include "vlog.h"
#include "fpga.h"
#include "commonutils.h"
#include "device_detection.h"
#include "strmsrv_comm.h"

#define USBFLG_DELETED              1
#define USBFLG_NEW                  2

#define FTDI_TYPE_ERROR             -1
#define FTDI_TYPE_NONE              0
#define FTDI_TYPE_BUILTIN           1
#define FTDI_TYPE_EXTERNAL          2

#define    FTDI_VENDOR_ID                0x0403
#define    PHILIPS_VENDOR_ID            0x04cc
#define    FTDI2232C_PRODUCT_ID        0x6010
#define    ISP1520_PRODUCT_ID            0x1520
#define BOARD_NUMBER_STRING_INDEX    3
#define BOARD_NUMBER_OFFSET            11
#define BUILTIN_FTDI_USB_LEVEL        2
#define BUILTIN_FTDI_USB_PORT        2
#define BUILTIN_FTDI_PORT            1

#define SYSFS_PATH_MAX              256

#define SYSFS_IDPRODUCT                "idProduct"
#define SYSFS_IDVENDOR                "idVendor"
#define SYSFS_DEVNUM                "devnum"
#define SYSFS_BUSNUM                "busnum"

#define SYSFS_EVENT                 "event"
#define SYSFS_TTYUSB                "ttyUSB"
#define SYSFS_DEVICE                "device"
#define SYSFS_PCI                   "pci"
#define SYSFS_USB                   "usb"

#define SYSBUSUSBSERDEV             "/sys/bus/usb-serial/devices"
#define SYSCLASSINPUT               "/sys/class/input"
#define SYSDEVICES                  "/sys/devices"
#define PROCBUSUSB                  "/proc/bus/usb/devices"

#define HOTPLUG_ACTION_ADD          "add"
#define HOTPLUG_ACTION_REMOVE       "remove"
#define HOTPLUG_MDEV_EVENT          "event"
#define HOTPLUG_MDEV_TTY            "ttyUSB"
#define HOTPLUG_MDEV_EVENT_FORMAT   "event%d"
#define HOTPLUG_MDEV_TTY_FORMAT     "ttyUSB%d"

#define FLAG_MDEV_EVENT             0x0100
#define FLAG_MDEV_TTY               0x0200

#define IS_ISP1520(x)    ((x)->descriptor.idVendor == PHILIPS_VENDOR_ID && \
             (x)->descriptor.idProduct == ISP1520_PRODUCT_ID)

#define IS_FTDI2232C(x)    ((x)->descriptor.idVendor == FTDI_VENDOR_ID && \
             (x)->descriptor.idProduct == FTDI2232C_PRODUCT_ID)

#define USB_DEBUG_USB               0x0001
#define USB_DEBUG_SYSFS             0x0002
#define USB_DEBUG_FTDI              0x0004
#define USB_DEBUG_ISP1520           0x0008
#define USB_DEBUG_EVENT             0x0010
#define USB_DEBUG_TTYUSB            0x0020
#define USB_DEBUG_HOTPLUG           0x0040

#ifdef DEBUG
#define debug_log(f, str...)  if (usb_debug & f) VLOG_INFO_CAT(VLOG_USB, str)
#else
#define debug_log(f, str...)  /* nothing */
#endif

static int usb_debug = (USB_DEBUG_EVENT|USB_DEBUG_TTYUSB|USB_DEBUG_HOTPLUG);

struct usbbusnode {
    struct list_head list;
    struct list_head childlist;
    unsigned int flags;
    unsigned int busnum;
};

struct usbdevnode {
    struct list_head list;
    struct list_head childlist;
    unsigned int flags;

    struct usbbusnode *bus;
    struct usbdevnode *parent;

    unsigned int devnum;
    unsigned int level;
    unsigned int port;
    unsigned int vendorid;
    unsigned int productid;
};

/*******************************************************************
 * USB utilities mostly copied from usbutils-0.72                  *
 *******************************************************************/
LIST_HEAD(usbbuslist);

static void xp200_usb_init(int debug) {
#ifndef __XPI__
    static int already_initialized = 0;
    usb_set_debug(debug);

    if (already_initialized)
        return;

    already_initialized = 1;

    usb_init();
    usb_find_busses();
    usb_find_devices();
#endif
}

static void markdel(struct list_head *list)
{
    struct usbdevnode *dev;
    struct list_head *list2;

    for (list2 = list->next; list2 != list; list2 = list2->next) {
        dev = list_entry(list2, struct usbdevnode, list);
        dev->flags |= USBFLG_DELETED;
        markdel(&dev->childlist);
    }
}

static void markdeleted(void)
{
    struct usbbusnode *bus;
    struct list_head *list;

    for (list = usbbuslist.next; list != &usbbuslist; list = list->next) {
        bus = list_entry(list, struct usbbusnode, list);
        markdel(&bus->childlist);
    }
}

static struct usbbusnode *findbusnode(unsigned int busn)
{
    struct usbbusnode *bus;
    struct list_head *list;

    for (list = usbbuslist.next; list != &usbbuslist; list = list->next) {
        bus = list_entry(list, struct usbbusnode, list);
        if (bus->busnum == busn) {
            return bus;
        }
    }
    return NULL;
}

static struct usbdevnode *findsubdevnode(struct list_head *list,
                                         unsigned int devn)
{
    struct usbdevnode *dev, *dev2;
    struct list_head *list2;

    for (list2 = list->next; list2 != list; list2 = list2->next) {
        dev = list_entry(list2, struct usbdevnode, list);
        if (dev->devnum == devn) {
            return dev;
        }
        dev2 = findsubdevnode(&dev->childlist, devn);
        if (dev2) {
            return dev2;
        }
    }
    return NULL;
}

static struct usbdevnode *finddevnode(struct usbbusnode *bus,
                                      unsigned int devn) {
    return findsubdevnode(&bus->childlist, devn);
}

static void parsedevfile(int fd) {
    char buf[16384];
    char *start, *end, *lineend, *cp;
    int ret;
    unsigned int devnum = 0, busnum = 0, parentdevnum = 0, level = 0, port = 0;
    unsigned int class = 0xff, vendor = 0xffff, prodid = 0xffff, speed = 0;
    struct usbbusnode *bus;
    struct usbdevnode *dev, *dev2;

    markdeleted();
    if (lseek(fd, 0, SEEK_SET) == (off_t)-1)
        VLOG_WARNING_CAT(VLOG_USB, "%s. lseek: %m (%d)", __FUNCTION__, errno);
    ret = read(fd, buf, sizeof(buf)-1);
    if (ret == -1)
        VLOG_WARNING_CAT(VLOG_USB, "%s. read: %m (%d)", __FUNCTION__, errno);
    end = buf + ret;
    *end = 0;
    start = buf;
    while (start < end) {
        lineend = strchr(start, '\n');
        if (!lineend)
            break;
        *lineend = 0;
        switch (start[0]) {
        case 'T':  /* topology line */
            if ((cp = strstr(start, "Dev#=")))
               devnum = strtoul(cp + 5, NULL, 10);
            else
                devnum = 0;
            if ((cp = strstr(start, "Bus=")))
                busnum = strtoul(cp + 4, NULL, 10);
            else
                busnum = 0;
            if ((cp = strstr(start, "Lev=")))
                level = strtoul(cp + 4, NULL, 10);
            else
                level = 0;
            if ((cp = strstr(start, "Prnt=")))
                parentdevnum = strtoul(cp + 5, NULL, 10);
            else
                parentdevnum = 0;
            if ((cp = strstr(start, "Port=")))
                port = strtoul(cp + 5, NULL, 10);
            else
                port = 0;
            if (strstr(start, "Spd=1.5"))
                speed = 1;
            else if (strstr(start, "Spd=12"))
                speed = 2;
            else
                speed = 0;
            break;

        case 'D':
            if ((cp = strstr(start, "Cls=")))
                class = strtoul(cp + 4, NULL, 16);
            else
                class = 0xff;
            break;

        case 'P':
            if ((cp = strstr(start, "Vendor=")))
                vendor = strtoul(cp + 7, NULL, 16);
            else
                vendor = 0xffff;
            if ((cp = strstr(start, "ProdID=")))
                prodid = strtoul(cp + 7, NULL, 16);
            else
                prodid = 0xffff;

            if (!(bus = findbusnode(busnum))) {
                if (!(bus = malloc(sizeof(struct usbbusnode))))
                    VLOG_WARNING_CAT(VLOG_USB, "%s. out of memory",
                            __FUNCTION__);
                bus->busnum = busnum;
                bus->flags = USBFLG_NEW;
                INIT_LIST_HEAD(&bus->childlist);
                list_add_tail(&bus->list, &usbbuslist);
            }
            else {
                bus->flags &= ~USBFLG_DELETED;
            }
            if (!(dev = finddevnode(bus, devnum)) ||
                    dev->vendorid != vendor || dev->productid != prodid) {
                if (!(dev = malloc(sizeof(struct usbdevnode))))
                    VLOG_WARNING_CAT(VLOG_USB, "%s. out of memory",
                            __FUNCTION__);
                dev->devnum = devnum;
                dev->flags = USBFLG_NEW;
                dev->bus = bus;
                dev->level = level;
                dev->port = port;
                dev->vendorid = vendor;
                dev->productid = prodid;

                INIT_LIST_HEAD(&dev->childlist);
                if (level == 0 && parentdevnum == 0) {
                    list_add_tail(&dev->list, &bus->childlist);
                    dev->parent = NULL;
                }
                else {
                    if (!(dev2 = finddevnode(bus, parentdevnum))) {
                        VLOG_INFO_CAT(VLOG_USB,
                                "%s. Bus %d Dev %d Prnt %d not found",
                                __FUNCTION__, busnum, devnum, parentdevnum);
                        break;
                    }
                    else {
                        dev->parent = dev2;
                        list_add_tail(&dev->list, &dev2->childlist);
                    }
                }
            }
            else {
                dev->flags &= ~USBFLG_DELETED;
            }
            break;
        default:
            break;
        }
        start = lineend + 1;
    }
}

static void deletetree(struct list_head *list, unsigned int force)
{
    struct usbdevnode *dev;
    struct list_head *list2;

    for (list2 = list->next; list2 != list;) {
        dev = list_entry(list2, struct usbdevnode, list);
        list2 = list2->next;
        deletetree(&dev->childlist, force || dev->flags & USBFLG_DELETED);
        if (!force && !(dev->flags & USBFLG_DELETED))
            continue;
        list_del(&dev->list);
        INIT_LIST_HEAD(&dev->list);
        free(dev);
    }
}

static void release_parseddevfile(void)
{
    struct list_head *list;
    struct usbbusnode *bus;

    for (list = usbbuslist.next; list != &usbbuslist;) {
        bus = list_entry(list, struct usbbusnode, list);
        list = list->next;
        deletetree(&bus->childlist, bus->flags & USBFLG_DELETED);
        if (!(bus->flags & USBFLG_DELETED))
            continue;
        list_del(&bus->list);
        INIT_LIST_HEAD(&bus->list);
        free(bus);
    }
}

/*******************************************************************
 * Original copyright notice for the usbutils routines above       *
 *******************************************************************/
/*
 *      Copyright (C) 1999-2001, 2003
 *        Thomas Sailer (t.sailer@alumni.ethz.ch)
 *      Copyright (C) 2003-2005 David Brownell
 *
 *      This program is free software; you can redistribute it and/or modify
 *      it under the terms of the GNU General Public License as published by
 *      the Free Software Foundation; either version 2 of the License, or
 *      (at your option) any later version.
 *
 *      This program is distributed in the hope that it will be useful,
 *      but WITHOUT ANY WARRANTY; without even the implied warranty of
 *      MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
 *      GNU General Public License for more details.
 *
 *      You should have received a copy of the GNU General Public License
 *      along with this program; if not, write to the Free Software
 *      Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.
 *
 */

/********************************************************************
 * Routines to find out which board the device /dev/ttyUSB is on    *
 ********************************************************************/
static struct usb_device *find_usb_device(int busnum, int devnum) {
#ifndef __XPI__
    struct usb_bus *busses, *bus;
    struct usb_device *dev;

    xp200_usb_init((usb_debug & USB_DEBUG_USB) ? 1 : 0);

    busses = usb_get_busses();

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            if (dev->bus && dev->bus->dirname && dev->filename &&
                    atoi(dev->bus->dirname) == busnum &&
                    atoi(dev->filename) == devnum) {
                 debug_log(USB_DEBUG_USB,
                        "%s(%d, %d) returning: %s/%s %04X/%04x",
                        __FUNCTION__, busnum, devnum,
                        bus->dirname, dev->filename, dev->descriptor.idVendor,
                        dev->descriptor.idProduct);
                return dev;
            }
        }
    }
#endif
    return NULL;
}

/******************************************************
 * Routines to find device information from sysfs     *
 ******************************************************/
static void remove_end_newline(char *value) {
    char *p = value + (strlen(value) - 1);

    if (p != NULL && *p == '\n')
        *p = '\0';
}

static int sysfs_path_is_file(const char *path) {
    struct stat astats;

    if (!path) {
        VLOG_WARNING_CAT(VLOG_USB, "%s failed. path == NULL", __FUNCTION__);
        return 0;
    }
    if ((lstat(path, &astats)) != 0) {
        return 0;
    }
    if (S_ISREG(astats.st_mode))
        return 1;

    return 0;
}

static int sysfs_get_link(const char *path, char *target, int len) {
    char devdir[SYSFS_PATH_MAX];
    char linkpath[SYSFS_PATH_MAX];
    char temp_path[SYSFS_PATH_MAX];
    char *d = NULL, *s = NULL;
    int slashes = 0, count = 0;

    if (!path || !target || len == 0) {
        VLOG_WARNING_CAT(VLOG_USB, "%s failed. %s. len = %d", __FUNCTION__,
            (path == NULL) ? "path == NULL" :
            ((target == NULL) ? "target == NULL" : ""), len);
        return -1;
    }

    debug_log(USB_DEBUG_SYSFS, "%s called. path %s, len %d",
            __FUNCTION__, path, len);

    memset(devdir, 0, SYSFS_PATH_MAX);
    memset(linkpath, 0, SYSFS_PATH_MAX);
    memset(temp_path, 0, SYSFS_PATH_MAX);
    strncpy(devdir, path, SYSFS_PATH_MAX - 1);

    if ((readlink(path, linkpath, SYSFS_PATH_MAX)) < 0) {
        VLOG_WARNING_CAT(VLOG_USB, "%s: readlink failed. path = %s, errno %d",
                __FUNCTION__, path, errno);
        return -1;
    }

    /********************************************************
     * Three cases here: 1. relative path => format ../..,  *
     * 2. absolute path => format /abcd/efgh, 3. relative   *
     * path _from_ this dir => format abcd/efgh             *
     ********************************************************/
    d = linkpath;
    switch (*d) {
    case '.':
        /* handle the case where link is of type ./abcd/xxx */
        strncpy(temp_path, devdir, SYSFS_PATH_MAX - 1);
        if (*(d+1) == '/')
           d += 2;
        else if (*(d+1) == '.') {
            /*  relative path, getting rid of leading "../.." */
            while (*d == '/' || *d == '.') {
                if (*d == '/')
                    slashes++;
                d++;
            }
            d--;
            s = &devdir[strlen(devdir)-1];
            while (s != NULL && count != (slashes+1)) {
                s--;
                if (*s == '/')
                    count++;
            }
            strncpy(s, d, SYSFS_PATH_MAX - strlen(devdir));
            strncpy(target, devdir, len - 1);
        }
        else {
            s = strrchr(temp_path, '/');
            if (s != NULL) {
                *(s+1) = '\0';
                strncat(temp_path, d, SYSFS_PATH_MAX - strlen(temp_path) - 1);
            }
            else {
                strncpy(temp_path, d, SYSFS_PATH_MAX - 1);
                strncpy(temp_path, devdir, SYSFS_PATH_MAX - 1);
            }
            strncpy(target, temp_path, len - 1);
        }
        break;
    case '/':
        /* absolute path - copy as is */
        strncpy(target, linkpath, len - 1);
        break;
    default:
        /* relative path from this directory */
        strncpy(temp_path, devdir, SYSFS_PATH_MAX - 1);
        s = strrchr(temp_path, '/');
        if (s != NULL) {
            *(s+1) = '\0';
            strncat(temp_path, linkpath, SYSFS_PATH_MAX - strlen(temp_path) - 1);
        }
        else {
            strncpy(temp_path, linkpath, SYSFS_PATH_MAX - 1);
        }
        strncpy(target, temp_path, len - 1);
    }

    debug_log(USB_DEBUG_SYSFS, "%s returning 0. target %s", __FUNCTION__, target);

    return 0;
}

static int sysfs_read_attr(char *path, char *name, char *val, int len) {
    int fd, rc;
    char attr[SYSFS_PATH_MAX];

    if (!path || !name) {
        VLOG_WARNING_CAT(VLOG_USB, "%s failed. %s.", __FUNCTION__,
            (path == NULL) ? "path == NULL" : "target == NULL");
        return -1;
    }

    if (strlen(path) + strlen(name) >= SYSFS_PATH_MAX - 1) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s failed. path = %s, name = %s", __FUNCTION__,
            path, name);
        return -1;
    }

    debug_log(USB_DEBUG_SYSFS, "%s called.  path %s, name %s, len %d",
            __FUNCTION__, path, name, len);

    strcpy(attr, path);
    strcat(attr, "/");
    strcat(attr, name);

    if (!sysfs_path_is_file(attr)) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s failed. path %s is a file", __FUNCTION__, attr);
        return -1;
    }
    if ((fd = open(attr, O_RDONLY)) < 0) {
        VLOG_WARNING_CAT(VLOG_USB,
                "Error reading attribute %s. errno %d", attr, errno);
        return -1;
    }
    rc = read(fd, val, len);
    if (rc < 0) {
        VLOG_WARNING_CAT(VLOG_USB,
                "Error reading from attribute %s. errno %d", attr, errno);
        close(fd);
        return -1;
    }
    close(fd);
    remove_end_newline(val);

    debug_log(USB_DEBUG_SYSFS, "%s returning %d. val %d", __FUNCTION__, rc, len);

    return rc;
}

static int sysfs_open_device(const char *device, char *path, int psize) {
     /********************************************************
      * We are at /sys/bus/"bus_name"/devices/"device" which *
      * is a link.  Read the link to get to the device.      *
      ********************************************************/
    if (sysfs_get_link(device, path, psize) < 0) {
        VLOG_WARNING_CAT(VLOG_USB, "Error getting to device %s", device);
        return -1;
    }
    return 0;
}

static int sysfs_get_device_parent(const char *device, char *parent) {
    char *tmp, ppath[SYSFS_PATH_MAX];

    if (strlen(device) >= SYSFS_PATH_MAX) {
        VLOG_WARNING_CAT(VLOG_USB, "%s. Invalid path to device %s",
                __FUNCTION__, device);
        return -1;
    }

    debug_log(USB_DEBUG_SYSFS, "%s called.  device %s", __FUNCTION__, device);

    strcpy(ppath, device);
    tmp = strrchr(ppath, '/');
    if (!tmp) {
        VLOG_WARNING_CAT(VLOG_USB, "%s. Invalid path to device %s",
                __FUNCTION__, ppath);
        return -1;
    }
    if (*(tmp + 1) == '\0') {
        *tmp = '\0';
        tmp = strrchr(tmp, '/');
        if (tmp == NULL) {
            VLOG_WARNING_CAT(VLOG_USB, "%s. Invalid path to device %s",
                    __FUNCTION__, ppath);
            return -1;
        }
    }
    *tmp = '\0';

    if (strcmp(ppath, SYSDEVICES) == 0) {
        /* Device has no parent */
        return -1;
    }

    strcpy(parent, ppath);

    debug_log(USB_DEBUG_SYSFS, "%s returning 0.  parent %s", __FUNCTION__, parent);

    return 0;
}

static int get_ftdi_type(char *linkpath) {
    int rc = FTDI_TYPE_EXTERNAL;
#ifndef __XPI__
    char ppath[SYSFS_PATH_MAX], p2path[SYSFS_PATH_MAX];
    char product[MAXDEVLEN], vendor[MAXDEVLEN];
    char devnum[MAXDEVLEN], busnum[MAXDEVLEN];
    int fd, bus, dev, pport;
    struct usbdevnode *devnode;
    struct usbbusnode *busnode;
    struct usb_device *usbdev;

    if ((fd = open(PROCBUSUSB, O_RDONLY)) == -1) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s: cannot open %s. %m", __FUNCTION__, PROCBUSUSB);
        return FTDI_TYPE_ERROR;
    }
    parsedevfile(fd);
    close(fd);

    if (sysfs_get_device_parent(linkpath, ppath) < 0) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s: %s has no parent", __FUNCTION__, linkpath);
        return FTDI_TYPE_ERROR;
    }

    debug_log(USB_DEBUG_FTDI, "%s(%s).", __FUNCTION__, linkpath);
    debug_log(USB_DEBUG_FTDI, "%s parent is %s", linkpath, ppath);

    if (sysfs_get_device_parent(ppath, p2path) < 0) {
        debug_log(USB_DEBUG_FTDI, "%s has no parent", ppath);
        return rc;
    }

    pport = ppath[strlen(ppath) - 1] - '0';

    vendor[0] = product[0] = devnum[0] = busnum[0] = '\0';

    if (sysfs_read_attr(p2path, SYSFS_IDVENDOR, vendor,
            SYSFS_PATH_MAX) < 0 ||
            strtoul(vendor, NULL, 16) != FTDI_VENDOR_ID) {
        debug_log(USB_DEBUG_FTDI, "Prnt lev 2 %s vendor not FTDI", ppath);
    }
    else if (sysfs_read_attr(p2path, SYSFS_IDPRODUCT, product,
            SYSFS_PATH_MAX) < 0 ||
            strtoul(product, NULL, 16) != FTDI2232C_PRODUCT_ID) {
        debug_log(USB_DEBUG_FTDI, "Prnt lev 2 %s product not 2232C", ppath);
    }
    else if (sysfs_read_attr(p2path, SYSFS_DEVNUM, devnum,
                SYSFS_PATH_MAX) < 0) {
        debug_log(USB_DEBUG_FTDI, "Prnt lev 2 %s no devnum", ppath);
    }
    else if (sysfs_read_attr(p2path, SYSFS_BUSNUM, busnum,
            SYSFS_PATH_MAX) < 0) {
        debug_log(USB_DEBUG_FTDI, "Prnt lev 2 %s no busnum", ppath);
    }
    else {
        bus = strtoul(busnum, NULL, 10);
        dev = strtoul(devnum, NULL, 10);

        usbdev = find_usb_device(bus, dev);

        if (usbdev) {
            if (IS_FTDI2232C(usbdev)) {
                debug_log(USB_DEBUG_FTDI,
                    "Prnt lev 2 %s is FTDI2232C, bus %d dev %d", ppath,
                    bus, dev);
                busnode = findbusnode(atoi(usbdev->bus->dirname));
                if (busnode == NULL) {
                    VLOG_WARNING_CAT(VLOG_USB, "%s error. NULL busnode for %s",
                                __FUNCTION__, usbdev->bus->dirname);
                    return rc;
                }
                devnode = finddevnode(busnode, atoi(usbdev->filename));
                if (devnode == NULL) {
                    VLOG_WARNING_CAT(VLOG_USB, "%s error. NULL devnode for %s",
                                __FUNCTION__, usbdev->filename);
                    return rc;
                }
                if (devnode->level == BUILTIN_FTDI_USB_LEVEL &&
                        devnode->port == BUILTIN_FTDI_USB_PORT) {
                    rc = (pport == BUILTIN_FTDI_PORT) ? FTDI_TYPE_BUILTIN :
                                        FTDI_TYPE_NONE;
                }
            }
        }
    }
    release_parseddevfile();

    debug_log(USB_DEBUG_FTDI, "%s returning %d.", __FUNCTION__, rc);
#endif
    return rc;
}

/***************************************************************
 * Returns board number for ISP1520 device 'busnum',  'devnum' *
 ***************************************************************/
static int get_isp1520_hub_board_number(int busnum, int devnum) {
#ifndef __XPI__
    int ret;
    struct usb_bus *busses, *bus;
    struct usb_device *dev;
    char string[SYSFS_PATH_MAX];

    xp200_usb_init((usb_debug & USB_DEBUG_ISP1520) ? 1 : 0);

    busses = usb_get_busses();

    for (bus = usb_busses; bus; bus = bus->next) {
        for (dev = bus->devices; dev; dev = dev->next) {
            usb_dev_handle *udev;
            if (IS_ISP1520(dev)) {
                if (busnum != atoi(bus->dirname) ||
                        devnum != atoi(dev->filename))
                    continue;
                udev = usb_open(dev);
                if (udev) {
                    ret = usb_get_string_simple(udev,
                                BOARD_NUMBER_STRING_INDEX,
                                string,
                                sizeof(string));
                    if (ret > 0) {
                        debug_log(USB_DEBUG_ISP1520,
                                "%s: String %s, Board Number %d",
                                __FUNCTION__, string, string[11] - '0');
                        usb_close(udev);
                        return (string[11] - '0');
                    }
                    else {
                        VLOG_INFO_CAT(VLOG_USB,
                                "%s. Unable to fetch serial number string",
                                __FUNCTION__);
                    }
                }
                usb_close(udev);
            }
            else {
                debug_log(USB_DEBUG_ISP1520,
                        "%s/%s %04X/%04x does not match ISP1520",
                        bus->dirname, dev->filename, dev->descriptor.idVendor,
                        dev->descriptor.idProduct);
            }
        }
    }
#endif
    return -1;
}

static int check_event_present(struct list_head *head,
                               const char *name) {
    struct list_head *tmp;
    struct event_s *evententryp;
    int rc = 0;

    if (head == NULL || list_empty(head)) {
        return rc;
    }

    list_for_each(tmp, head) {
        evententryp = list_entry(tmp, struct event_s, event_list);
        if (!strcasecmp(evententryp->event_name, name)) {
            rc = 1;
            break;
        }
    }
    return rc;
}

/******************************************************
 * Returns board number for input event device 'name' *
 ******************************************************/
static int usb_get_event_device_board_number(const char *name) {
#ifndef __XPI__
    int rc = -1;
    int plev, bus, dev, hwprofile;
    char devpath[SYSFS_PATH_MAX], linkpath[SYSFS_PATH_MAX];
    char parentpath[SYSFS_PATH_MAX], *slash;
    char product[MAXDEVLEN], vendor[MAXDEVLEN];
    char devnum[MAXDEVLEN], busnum[MAXDEVLEN];
    struct usb_device *usbdev;

    sprintf(devpath, "%s/%s", SYSCLASSINPUT, name);
    if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s: cannot open device %s", __FUNCTION__, devpath);
        return rc;
    }

    if ((slash = strrchr(linkpath, '/')) == NULL) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s: bad link %s for device %s",
                __FUNCTION__, linkpath, devpath);
        return rc;
    }
    *slash = '\0';

    sprintf(devpath, "%s/device", linkpath);
    if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s: cannot open device %s", __FUNCTION__, devpath);
        return rc;
    }

    if (strstr(linkpath, SYSFS_USB) == NULL) {
        debug_log(USB_DEBUG_EVENT, "%s: device %s not on USB bus linkpath %s",
                __FUNCTION__, devpath, linkpath);
        return rc;
    }

    /* Assign all USB devices on single board systems to Board 0 */
    if ((hwprofile = get_hardware_profile()) == HARDWARE_PROFILE_XP100 ||
            hwprofile == HARDWARE_PROFILE_XP200S ||
            hwprofile == HARDWARE_PROFILE_XP220S)
        return 0;

    plev = 1;
    while (1) {
        if (sysfs_get_device_parent(linkpath, parentpath) < 0) {
            debug_log(USB_DEBUG_EVENT, "%s: Prnt lev %d has no parent",
                    __FUNCTION__, plev);
            break;
        }
        vendor[0] = product[0] = devnum[0] = busnum[0] = '\0';
        if (sysfs_read_attr(parentpath, SYSFS_IDVENDOR, vendor,
                    SYSFS_PATH_MAX) < 0 ||
                    strtoul(vendor, NULL, 16) != PHILIPS_VENDOR_ID) {
            debug_log(USB_DEBUG_EVENT, "%s: Prnt lev %d vendor not Philips",
                    __FUNCTION__, plev);
        }
        else if (sysfs_read_attr(parentpath, SYSFS_IDPRODUCT,
                    product, SYSFS_PATH_MAX) < 0 ||
                    strtoul(product, NULL, 16) != ISP1520_PRODUCT_ID) {
            debug_log(USB_DEBUG_EVENT, "%s: Prnt lev %d not 1520",
                    __FUNCTION__, plev);
        }
        else if (sysfs_read_attr(parentpath, SYSFS_DEVNUM, devnum,
                    SYSFS_PATH_MAX) < 0) {
            debug_log(USB_DEBUG_EVENT, "%s: Prnt lev %d no devnum",
                    __FUNCTION__, plev);
        }
        else if (sysfs_read_attr(parentpath, SYSFS_BUSNUM, busnum,
                    SYSFS_PATH_MAX) < 0) {
            debug_log(USB_DEBUG_EVENT, "%s: Prnt lev %d no busnum",
                    __FUNCTION__, plev);
        }
        else {
            bus = strtoul(busnum, NULL, 10);
            dev = strtoul(devnum, NULL, 10);
            debug_log(USB_DEBUG_EVENT,
                    "%s: Prnt lev %d is ISP1520, bus %d dev %d",
                    __FUNCTION__, plev, bus, dev);

            usbdev = find_usb_device(bus, dev);

            if (usbdev) {
                if (IS_ISP1520(usbdev)) {
                    rc = get_isp1520_hub_board_number(bus, dev);
                    debug_log(USB_DEBUG_EVENT,
                            "%s: ISP1520 Hub (%d, %d) is on Board %d",
                            __FUNCTION__, bus, dev, rc);
                    return rc;
                 }
            }
            else {
                debug_log(USB_DEBUG_EVENT,
                    "%s: failed to find device bus %d, device %d ",
                    __FUNCTION__, bus, dev);
            }
            break;
        }
        strcpy(linkpath, parentpath);
        plev++;
    }
    return rc;
#else
    return 0;
#endif
}

#ifdef DEBUG
static void dump_events(struct list_head *head) {
    struct list_head *tmp;
    struct event_s *evententryp;
    int count = 0;

    if (head == NULL || list_empty(head)) {
        VLOG_INFO_CAT(VLOG_USB, "dump_events: no events to dump");
    }
    else {
        list_for_each(tmp, head) {
            evententryp = list_entry(tmp, struct event_s, event_list);
            VLOG_INFO_CAT(VLOG_USB, "Event %d: Name %s, FD %d", count++,
                    evententryp->event_name, evententryp->event_fd);
        }
    }
}
#else
static void dump_events(struct list_head *head) {}
#endif

/***************************************************
 * Populates list of input event devices for board *
 * based on what's in sysfs.                       *
 ***************************************************/
int usb_initialize_event_devices(HAL *hp) {
    DIR *dir;
    struct dirent *entry;
    char devpath[SYSFS_PATH_MAX], linkpath[SYSFS_PATH_MAX];
    int rc = 0, bno = hal_get_board_number(hp);

    INIT_LIST_HEAD(&(hp->event_head));

    dir = opendir(SYSCLASSINPUT);
    if (!dir) {
        closedir(dir);
        return -1;
    }

    for (;;) {
        struct event_s evententry, *evententryp;

        if ((entry = readdir(dir)) == NULL)
            break;
        if (strncmp(entry->d_name, SYSFS_EVENT, sizeof(SYSFS_EVENT) - 1) != 0)
            continue;
        sprintf(devpath, "%s/%s", SYSCLASSINPUT, entry->d_name);
        if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0)
            continue;
        else if (strstr(linkpath, SYSFS_USB) == NULL)
            continue;

        if (bno != usb_get_event_device_board_number(entry->d_name))
           continue;

        memset(&evententry, 0, sizeof(struct event_s));
        snprintf(evententry.event_name, MAXDEVLEN-1, "/dev/%s", entry->d_name);
        evententryp = malloc(sizeof(struct event_s));

        if (evententryp) {
            memcpy(evententryp, &evententry, sizeof(struct event_s));
            evententryp->event_fd = open(evententryp->event_name, O_RDONLY);
            if (evententryp->event_fd < 0) {
                VLOG_WARNING_CAT(VLOG_USB,
                        "Channel %d. Failed to open Device %s. %m",
                        bno+1, evententryp->event_name);
                free(evententryp);
                rc = -1; break;
            }
            list_add_tail(&(evententryp->event_list), &(hp->event_head));
            rc++;
        }
    }

    closedir(dir);

    debug_log(USB_DEBUG_EVENT,
            "Channel %d. usb_initialize_event_devices. returning %d",
            bno+1, rc);
    dump_events(&(hp->event_head));

    return rc;
}

void usb_release_event_devices(HAL *hp) {
    struct list_head *tmp;
    struct event_s *evententryp;

    list_for_each(tmp, &(hp->event_head)) {
        evententryp = list_entry(tmp, struct event_s, event_list);
        if (evententryp->event_fd > 0)
            close(evententryp->event_fd);
        list_del(&(evententryp->event_list));
        free(evententryp);
    }
}

int usb_update_event_device(HAL *hp, char *device, int flag) {
    int cno = hal_get_board_number(hp) + 1, rc = 0;
    struct list_head *tmp;
    struct event_s evententry, *evententryp;

    debug_log(USB_DEBUG_EVENT,
            "Channel %d. usb_update_event_device. device %s, flag %x",
            cno, device, flag);
    dump_events(&(hp->event_head));

    if (flag & USB_UPDATE_FLAG_ADD) {
        if (check_event_present(&(hp->event_head), device) > 0) {
            VLOG_WARNING_CAT(VLOG_USB,
                    "Channel %d. Device %s already present.",
                    cno, device);
            return -1;
        }

        memset(&evententry, 0, sizeof(struct event_s));
        strncpy(evententry.event_name, device, MAXDEVLEN - 1);

        evententryp = malloc(sizeof(struct event_s));
        if (evententryp) {
            memcpy(evententryp, &evententry, sizeof(struct event_s));
            evententryp->event_fd = open(evententryp->event_name, O_RDWR);
            if (evententryp->event_fd < 0) {
                VLOG_WARNING_CAT(VLOG_USB,
                        "Channel %d. Failed to open Device %s. %m",
                        cno, evententryp->event_name);
                free(evententryp);
                return -1;
            }
            list_add_tail(&(evententryp->event_list), &(hp->event_head));
            rc++;
        }
        else {
            VLOG_WARNING_CAT(VLOG_USB,
                    "Channel %d. Failed malloc for Device %s", cno,
                    evententryp->event_name);
            return -1;
        }
    }
    else if (flag & USB_UPDATE_FLAG_REMOVE) {
        if (check_event_present(&(hp->event_head), device) == 0) {
            VLOG_WARNING_CAT(VLOG_USB,
                    "Channel %d. Device %s not present.",
                    cno, device);
            return 0;
        }
        if (!list_empty(&(hp->event_head))) {
            list_for_each(tmp, &(hp->event_head)) {
                evententryp = list_entry(tmp, struct event_s, event_list);
                if (!strcasecmp(evententryp->event_name, device)) {
                    if (evententryp->event_fd > 0)
                        close(evententryp->event_fd);
                    list_del(&(evententryp->event_list));
                    free(evententryp);
                    rc++;
                }
            }
        }
    }

    debug_log(USB_DEBUG_EVENT,
            "Channel %d. %s returning %d", cno, __FUNCTION__, rc);
    dump_events(&(hp->event_head));

    return rc;
}

/*****************************************************************
 * Returns 1 with 'name' set to that for the USB serial on Board *
 * 'bno'. If 'builtin' is set, 'name' is the device name for the *
 * builtin USB serial, otherwise it's the first non-builtin USB  *
 * serial we find. Returns 0 if no USB serial can be found.      *
 *****************************************************************/
int usb_find_tty_device_name_for_board(int bno,
                                       char name[],
                                       int len,
                                       int builtin) {
    int rc = -1;
#ifndef __XPI__
    int plev, bus, dev, board, ftdi_type, hwprofile;
    char devpath[SYSFS_PATH_MAX], linkpath[SYSFS_PATH_MAX];
    char parentpath[SYSFS_PATH_MAX], *dname;
    char product[MAXDEVLEN], vendor[MAXDEVLEN];
    char devnum[MAXDEVLEN], busnum[MAXDEVLEN];
    struct usb_device *usbdev;
    DIR *dir = NULL;
    struct dirent *entry;

    if (!dir) {
        dir = opendir(SYSBUSUSBSERDEV);
        if (!dir) {
            closedir(dir);
            VLOG_WARNING_CAT(VLOG_USB,
                    "%s: failed to open %s", __FUNCTION__, SYSBUSUSBSERDEV);
            return rc;
        }
    }

    hwprofile = get_hardware_profile();

    for (;;) {
        if ((entry = readdir(dir)) == NULL) {
            break;
        }
        dname = entry->d_name;
        if (strncmp(dname, SYSFS_TTYUSB, sizeof(SYSFS_TTYUSB) - 1) != 0)
            continue;

        sprintf(devpath, "%s/%s", SYSBUSUSBSERDEV, dname);
        if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0) {
            VLOG_WARNING_CAT(VLOG_USB,
                    "%s: cannot open device %s", __FUNCTION__, dname);
            continue;
        }

        if ((ftdi_type = get_ftdi_type(linkpath)) == FTDI_TYPE_ERROR) {
            closedir(dir);
            VLOG_WARNING_CAT(VLOG_USB,
                    "%s: get_ftdi_type for %s returns error.",
                    __FUNCTION__, linkpath);
            return rc;
        }

        if ((builtin && (ftdi_type != FTDI_TYPE_BUILTIN)) ||
                (!builtin && (ftdi_type != FTDI_TYPE_EXTERNAL)))
            continue;

        if (hwprofile == HARDWARE_PROFILE_XP100 ||
                hwprofile == HARDWARE_PROFILE_XP220S) {
            closedir(dir);
            snprintf(name, len - 1, "/dev/%s", dname);
            name[len - 1] = '\0';
            return 1;
        }

        plev = 1;
        while (1) {
            if (sysfs_get_device_parent(linkpath, parentpath) < 0) {
                debug_log(USB_DEBUG_TTYUSB,
                    "%s: Prnt lev %d has no parent",__FUNCTION__, plev);
                break;
            }
            vendor[0] = product[0] = devnum[0] = busnum[0] = '\0';
            if (sysfs_read_attr(parentpath, SYSFS_IDVENDOR, vendor,
                    SYSFS_PATH_MAX) < 0 ||
                    strtoul(vendor, NULL, 16) != PHILIPS_VENDOR_ID) {
                debug_log(USB_DEBUG_TTYUSB,
                    "%s: Prnt lev %d vendor not Philips",
                    __FUNCTION__, plev);
            }
            else if (sysfs_read_attr(parentpath, SYSFS_IDPRODUCT,
                    product, SYSFS_PATH_MAX) < 0 ||
                    strtoul(product, NULL, 16) != ISP1520_PRODUCT_ID) {
                debug_log(USB_DEBUG_TTYUSB, "%s: Prnt level %d not 1520",
                    __FUNCTION__, plev);
            }
            else if (sysfs_read_attr(parentpath, SYSFS_DEVNUM, devnum,
                    SYSFS_PATH_MAX) < 0) {
                debug_log(USB_DEBUG_TTYUSB, "%s: Prnt lev %d no devnum",
                    __FUNCTION__, plev);
            }
            else if (sysfs_read_attr(parentpath, SYSFS_BUSNUM, busnum,
                    SYSFS_PATH_MAX) < 0) {
                debug_log(USB_DEBUG_TTYUSB, "%s: Prnt lev %d no busnum",
                    __FUNCTION__, plev);
            }
            else {
                bus = strtoul(busnum, NULL, 10);
                dev = strtoul(devnum, NULL, 10);

                debug_log(USB_DEBUG_TTYUSB,
                        "%s: Prnt lev %d is ISP1520, bus %d dev %d",
                        __FUNCTION__, plev, bus, dev);

                usbdev = find_usb_device(bus, dev);

                if (usbdev) {
                    if (IS_ISP1520(usbdev)) {
                        board = get_isp1520_hub_board_number(bus, dev);
                        if (board == bno) {
                            snprintf(name, len - 1, "/dev/%s", dname);
                            name[len - 1] = '\0';
                            rc = 1;
                            debug_log(USB_DEBUG_TTYUSB,
                                "%s: Returning %d. name = %s",
                                __FUNCTION__, rc, name);
                            break;
                        }
                    }
                }
                else {
                    debug_log(USB_DEBUG_TTYUSB,
                        "%s: failed to find device bus %d, device %d ",
                        __FUNCTION__, bus, dev);
                }
                break;
            }
            strcpy(linkpath, parentpath);
            plev++;
        }
    }
    closedir(dir);
#endif
    return rc;
}

/*************************************************
 * Returns board number for ttyUSB device 'name' *
 *************************************************/
int usb_get_tty_device_board_number(const char *name) {
#ifndef __XPI__
    int rc = -1;
    int plev, bus, dev, hwprofile;
    char devpath[SYSFS_PATH_MAX], linkpath[SYSFS_PATH_MAX];
    char parentpath[SYSFS_PATH_MAX];
    char product[MAXDEVLEN], vendor[MAXDEVLEN];
    char devnum[MAXDEVLEN], busnum[MAXDEVLEN];
    struct usb_device *usbdev;
    sprintf(devpath, "%s/%s", SYSBUSUSBSERDEV, name);

    if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0) {
        VLOG_WARNING_CAT(VLOG_USB,
                "%s: cannot open device path %s", __FUNCTION__, name);
        return rc;
    }

    if (strstr(linkpath, SYSFS_USB) == NULL)
        return rc;

    /* Assign all USB devices on single board systems to Board 0 */
    if ((hwprofile = get_hardware_profile()) == HARDWARE_PROFILE_XP100 ||
            hwprofile == HARDWARE_PROFILE_XP200S ||
            hwprofile == HARDWARE_PROFILE_XP220S)
        return 0;

    plev = 1;

    while (1) {
        if (sysfs_get_device_parent(linkpath, parentpath) < 0) {
            debug_log(USB_DEBUG_TTYUSB,
                    "%s: Prnt lev %d has no parent", __FUNCTION__, plev);
            break;
        }
        vendor[0] = product[0] = devnum[0] = busnum[0] = '\0';
        if (sysfs_read_attr(parentpath, SYSFS_IDVENDOR, vendor,
                    SYSFS_PATH_MAX) < 0 ||
                    strtoul(vendor, NULL, 16) != PHILIPS_VENDOR_ID) {
            debug_log(USB_DEBUG_TTYUSB, "%s: Prnt lev %d vendor not Philips",
                __FUNCTION__, plev);
        }
        else if (sysfs_read_attr(parentpath, SYSFS_IDPRODUCT,
                    product, SYSFS_PATH_MAX) < 0 ||
                    strtoul(product, NULL, 16) != ISP1520_PRODUCT_ID) {
            debug_log(USB_DEBUG_TTYUSB, "%s: Prnt lev %d not 1520",
                __FUNCTION__, plev);
        }
        else if (sysfs_read_attr(parentpath, SYSFS_DEVNUM, devnum,
                    SYSFS_PATH_MAX) < 0) {
            debug_log(USB_DEBUG_TTYUSB, "%s: Prnt lev %d no devnum",
                __FUNCTION__, plev);
        }
        else if (sysfs_read_attr(parentpath, SYSFS_BUSNUM, busnum,
                    SYSFS_PATH_MAX) < 0) {
            debug_log(USB_DEBUG_TTYUSB, "%s: Prnt lev %d no busnum",
                __FUNCTION__, plev);
        }
        else {
            bus = strtoul(busnum, NULL, 10);
            dev = strtoul(devnum, NULL, 10);

            debug_log(USB_DEBUG_TTYUSB,
                "%s: Prnt lev %d is ISP1520, bus %d dev %d",
                __FUNCTION__, plev, bus, dev);

            usbdev = find_usb_device(bus, dev);

            if (usbdev) {
                if (IS_ISP1520(usbdev)) {
                    rc = get_isp1520_hub_board_number(bus, dev);
                    debug_log(USB_DEBUG_TTYUSB,
                        "%s: ISP1520 Hub (%d, %d) is on Board %d",
                        __FUNCTION__, bus, dev, rc);
                    return rc;
                 }
            }
            else {
                debug_log(USB_DEBUG_TTYUSB,
                    "%s: failed to find device bus %d, device %d ",
                    __FUNCTION__, bus, dev);
            }
            break;
        }
        strcpy(linkpath, parentpath);
        plev++;
    }
    return rc;
#else
    return 0;
#endif
}

/******************************************
 * Test program for event device routines *
 ******************************************/
static void print_lseventdev_help(void) {
    printf("\nUsage: lseventdev [-d]\n");
    printf("Display /dev/input/eventX information.\n");
    printf("The -d switch turns on debug\n");
}

static int usb_scan_event_devices(int verbose) {
    DIR *dir = NULL;
    struct dirent *entry;
    char devpath[SYSFS_PATH_MAX], linkpath[SYSFS_PATH_MAX];
    char *dname;
    int count = 0, bno = -1;

    if (!dir) {
        dir = opendir(SYSCLASSINPUT);
        if (!dir) {
            closedir(dir);
            printf("%s: failed to open %s\n", __FUNCTION__, SYSCLASSINPUT);
            return -1;
        }
    }

    for (;;) {
        if ((entry = readdir(dir)) == NULL) {
            break;
        }
        dname = entry->d_name;

        if (verbose) printf("Processing %s\n", dname);

        if (strncmp(dname, SYSFS_EVENT, sizeof(SYSFS_EVENT) - 1) != 0) {
            if (verbose)
                printf("\"%s\" is not an event device\n", dname);
            continue;
        }

        sprintf(devpath, "%s/%s", SYSCLASSINPUT, dname);
        if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0) {
            if (verbose)
                printf("Error: cannot open device path %s\n", dname);
            continue;
        }
        else if (strstr(linkpath, SYSFS_USB) == NULL) {
            if (verbose)
                printf("%s is not a USB device\n", dname);
            continue;
        }
        else {
            bno = usb_get_event_device_board_number(dname);
            if (bno < 0)
                printf("\"%s\" is not a USB Event device\n", dname);
            else
                printf("\"%s\" is on Channel %d\n", dname, bno + 1);
            count++;
        }
    }
    closedir(dir);
    return count;
}

int lseventdev_main(int argc, char **argv) {
    int opt, count, debug = 0;

    while ((opt = getopt(argc, argv, "dh")) > 0) {
        switch (opt) {
        case 'd':
            debug = 1;
            break;
        case '?':
        case 'h':
            print_lseventdev_help();;
            return -1;
        }
    }
    if (optind != argc) {
        print_lseventdev_help();
        return -1;
    }
    if ((count = usb_scan_event_devices(debug)) <= 0) {
        printf("Failed to find event devices\n");
    }
    return 0;
}

/*******************************************
 * Test program for ttyusb device routines *
 *******************************************/
static void print_lsttyusb_help(void) {
    printf("Display /dev/ttyUSBx information.\n");
    printf("The -d switch turns on debug\n");
}

static int usb_scan_tty_devices(int verbose) {
    DIR *dir = NULL;
    struct dirent *entry;
    char devpath[SYSFS_PATH_MAX], linkpath[SYSFS_PATH_MAX], *dname;
    int count = 0, bno = -1, rc = -1;

    if (!dir) {
        dir = opendir(SYSBUSUSBSERDEV);
        if (!dir) {
            closedir(dir);
            printf("%s: failed to open %s\n", __FUNCTION__, SYSBUSUSBSERDEV);
            return rc;
        }
    }

    for (;;) {
        if ((entry = readdir(dir)) == NULL) {
            break;
        }
        dname = entry->d_name;
        if (strncmp(dname, SYSFS_TTYUSB, sizeof(SYSFS_TTYUSB) - 1) != 0)
            continue;

        if (verbose) printf("Processing %s\n", dname);

        if (strncmp(dname, SYSFS_TTYUSB, sizeof(SYSFS_TTYUSB) - 1) != 0) {
            if (verbose)
                printf("\"%s\" is not a TTY USB device\n", dname);
            continue;
        }

        sprintf(devpath, "%s/%s", SYSBUSUSBSERDEV, dname);
        if (sysfs_open_device(devpath, linkpath, SYSFS_PATH_MAX) < 0) {
            if (verbose)
                printf("%s: cannot open device path %s\n", __FUNCTION__, dname);
            continue;
        }
        else if (strstr(linkpath, SYSFS_USB) == NULL) {
            if (verbose)
                printf("%s is not a USB device\n", dname);
            continue;
        }
        else {
            bno = usb_get_tty_device_board_number(dname);
            if (bno < 0 && verbose) {
                printf("\"%s\" is not a TTY USB device\n", dname);
            }
            else if (bno >= 0) {
                printf("\"%s\" is on Channel %d\n", dname, bno + 1);
            }
            count++;
        }
    }
    closedir(dir);
    return count;
}

int lsttyusb_main(int argc, char **argv) {
    int opt, i, count = 0, debug = 0;
    char devname[MAXDEVLEN];

    while ((opt = getopt(argc, argv, "c:b:dh")) > 0) {
        switch (opt) {
        case 'd':
            debug = 1;
            break;
        case '?':
        case 'h':
            print_lsttyusb_help();;
            return -1;
        }
    }

    if (optind != argc) {
        print_lsttyusb_help();
        return -1;
    }

    if ((count = usb_scan_tty_devices(debug)) <= 0)
        printf("Failed to find ttyUSB devices\n");

    if (get_hardware_profile() == HARDWARE_PROFILE_XP100)
        return 0;

    for (i = 0; i < MAX_FPGA_BOARDS; i++) {
        if (usb_find_tty_device_name_for_board(i, devname, MAXDEVLEN, 1) < 0)
            printf("Channel %d has no builtin USB serial\n", i+1);
        else
            printf("Builtin USB serial for Channel %d is %s\n", i+1, devname);

        if (usb_find_tty_device_name_for_board(i, devname, MAXDEVLEN, 0) < 0)
            printf("Channel %d has no external USB serial\n", i+1);
        else
            printf("First external USB serial for Channel %d is %s\n",
                    i+1, devname);
    }

    return 0;
}


/********************************************************************
 * Routine called by mdev on USB device Add/Remove hotplug actions. *
 * Since when we get called by mdev on "Remove", the device has     *
 * already disappeared from SYSFS & PROCFS we can't find the board  *
 * the device was on so we send "update" to all boards for removal. *
 ********************************************************************/
int usbhotplug_main(int argc, char **argv) {
    int devnum, bno = -1, flag = 0, rc = 0;
    char *mdev, *action;
    struct strmsrv_hotplug_params params;

    /****************************************************************
     * This is a fake socket call added as work-around to a bug in  *
     * busybox mdev (at least we think so) where the 1st socket()   *
     * call in the program mdev invokes always fails w/errno 2 or 25*
     ****************************************************************/
    int s = socket(AF_INET, SOCK_STREAM, 0);
    if (s <= 0)
        VLOG_DEBUG_CAT(VLOG_USB, "%s: socket failed. %m. errno %d",
                __FUNCTION__, errno);
    else
        VLOG_DEBUG_CAT(VLOG_USB, "%s: socket %d. success.", __FUNCTION__, s);
    if (s > 0)
        close(s);

    /****************************************************************
     * This is where the actual hotplug code starts                 *
     ****************************************************************/
    memset(&params, 0, sizeof(struct strmsrv_hotplug_params));
    action = getenv("ACTION");
    mdev = getenv("MDEV");

    if (mdev == NULL || action == NULL) {
        debug_log(USB_DEBUG_HOTPLUG,
                "%s: MDEV or ACTION not set.", __FUNCTION__);
        return 0;
    }

    VLOG_DEBUG_CAT(VLOG_USB, "Device = %s, Action = %s.", mdev, action);

    if (!strncmp(mdev, HOTPLUG_MDEV_EVENT, sizeof(HOTPLUG_MDEV_EVENT) - 1))
        flag |= FLAG_MDEV_EVENT;
    else if (!strncmp(mdev, HOTPLUG_MDEV_TTY, sizeof(HOTPLUG_MDEV_TTY) - 1))
        flag |= FLAG_MDEV_TTY;

    if (strcmp(action, HOTPLUG_ACTION_ADD) == 0) {
        if (flag & FLAG_MDEV_EVENT) {
            sscanf(mdev, HOTPLUG_MDEV_EVENT_FORMAT, &devnum);
            bno = usb_get_event_device_board_number(mdev);
            params.add_event_device_set = 1;
            params.add_event_device = devnum;
        }
        else if (flag & FLAG_MDEV_TTY) {
            sscanf(mdev, HOTPLUG_MDEV_TTY_FORMAT, &devnum);
            bno = usb_get_tty_device_board_number(mdev);
            params.add_serial_device_set = 1;
            params.add_serial_device = devnum;
        }
        if (bno < 0) {
            VLOG_WARNING_CAT(VLOG_USB,
                    "Failed to find board number for %s", mdev);
            rc = -1;
        }
        else {
            strmsrv_set_hotplug_params(bno + 1, &params);
        }
    }
    else if (strcmp(action, HOTPLUG_ACTION_REMOVE) == 0) {
        int hardware_profile = get_hardware_profile();
        if (flag & FLAG_MDEV_EVENT) {
            sscanf(mdev, HOTPLUG_MDEV_EVENT_FORMAT, &devnum);
            params.remove_event_device_set = 1;
            params.remove_event_device = devnum;
        }
        else if (flag & FLAG_MDEV_TTY) {
            sscanf(mdev, HOTPLUG_MDEV_TTY_FORMAT, &devnum);
            params.remove_serial_device_set = 1;
            params.remove_serial_device = devnum;
        }
        if (hardware_profile == HARDWARE_PROFILE_XP100 ||
                hardware_profile == HARDWARE_PROFILE_XP200S ||
                hardware_profile == HARDWARE_PROFILE_XP220S ||
                hardware_profile == HARDWARE_PROFILE_XPI) {
            strmsrv_set_hotplug_params(1, &params);
        }
        else {
            for (bno = 0; bno < MAX_FPGA_BOARDS; bno++) {
                strmsrv_set_hotplug_params(bno + 1, &params);
            }
        }
    }
    return 0;
}
