#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <stdbool.h>
#include <signal.h>
#include <fcntl.h>
#include <sys/ioctl.h>
#include "vlog.h"
#ifdef __XPI__
#include <linux/videodev2.h>
#include "netvizxpi/netvizxpi.h"
#endif
#include "xpi_gpio_control.h"

#define XPI_GPIO_BASE                "/sys/class/gpio/"
#define XPI_GPIO_EXPORT              "/sys/class/gpio/export"
#define XPI_GPIO_UNEXPORT            "/sys/class/gpio/unexport"

#ifdef __XPI__
static int export_gpio_pin(int gpio_pin)
{
    int file, rc = 0;
    char buf[32];

    if ((file = open(XPI_GPIO_EXPORT, O_WRONLY)) < 0) {
        VLOG_INFO("Failed to open file %s: %m", XPI_GPIO_EXPORT);
        return -1;
    }
    sprintf(buf, "%d", gpio_pin);
    if (write(file, buf, strlen(buf)) != strlen(buf)) {
        VLOG_INFO("Failed to write to file %s: %m", XPI_GPIO_EXPORT);
        rc = -1;
    }
    close(file);
    return rc;
}

static int unexport_gpio_pin(int gpio_pin)
{
    int file, rc = 0;
    char buf[32];

    if ((file = open(XPI_GPIO_UNEXPORT, O_WRONLY)) < 0) {
        VLOG_INFO("Failed to open file %s: %m", XPI_GPIO_UNEXPORT);
        return -1;
    }
    sprintf(buf, "%d", gpio_pin);
    if (write(file, buf, strlen(buf)) != strlen(buf)) {
        VLOG_INFO("Failed to write to file %s: %m", XPI_GPIO_UNEXPORT);
        rc = -1;
    }
    close(file);
    return rc;
}

static int set_gpio_pin_direction(int gpio_pin, bool in)
{
    int file, rc = 0;
    char filename[32], buf[32];

    sprintf(filename, XPI_GPIO_BASE "gpio%d/direction", gpio_pin);
    if ((file = open(filename, O_RDWR)) < 0) {
        VLOG_INFO("Failed to open file %s: %m", filename);
        return -1;
    }
    strcpy(buf, (in) ? "in" : "out");
    if (write(file, buf, strlen(buf)) != strlen(buf)) {
        VLOG_INFO("Failed to write to file %s: %m", filename);
        rc = -1;
    }
    close(file);
    return rc;
}

static int set_gpio_pin_level(int gpio_pin, int level)
{
    int file, rc = 0;
    char filename[32], buf[32];
    
    sprintf(filename, XPI_GPIO_BASE "gpio%d/value", gpio_pin);
    if ((file = open(filename, O_RDWR)) < 0) {
        VLOG_INFO("Failed to open file %s: %m", filename);
        return -1;
    }
    strcpy(buf, (level) ? "1\n" : "0\n");
    if (write(file, buf, strlen(buf)) != strlen(buf)) {
        VLOG_INFO("Failed to write to file %s: %m", filename);
        rc = -1;
    }
    close(file);
    return rc;
}

static int get_gpio_pin_level(int gpio_pin, int *level)
{
    int file, len, rc = 0, i;
    char filename[32], buf[32];

    sprintf(filename, XPI_GPIO_BASE "gpio%d/value", gpio_pin);
    if ((file = open(filename, O_RDWR)) < 0) {
        VLOG_INFO("Failed to open file %s: %m", filename);
        return -1;
    }
    len = read(file, buf, sizeof(buf) - 1);
    if (len == -1) {
        VLOG_INFO("Failed to read from file %s: %m", filename);
        rc = -1;
    }
    else {
        for (i = 0; i < sizeof(buf); i++) {
            if (buf[i] == '\n' || buf[i] == '\r') {
                buf[i] = '\0';
                break;
            }
        }
        *level = atoi(buf);
    }
    close(file);
    return rc;
}
#endif

int xpi_get_gpio_pin_value(int gpio_pin, int *val)
{
    int rc = -1;

#ifdef __XPI__
    if (export_gpio_pin(gpio_pin) < 0) {
        return -1;
    }
    else {
        int value;
        if (get_gpio_pin_level(gpio_pin, &value) == 0) {
            *val = (value) ? 1 : 0;
            rc = 0;
        }
    }
    if (unexport_gpio_pin(gpio_pin) < 0) 
        return -1;
#endif
    return rc;
}

int xpi_set_gpio_pin_value(int gpio_pin, int val)
{
    int rc = -1;

#ifdef __XPI__
    if (export_gpio_pin(gpio_pin) < 0) {
        return -1;
    }
    else if (set_gpio_pin_direction(gpio_pin, false) == 0) {
        if (set_gpio_pin_level (gpio_pin, (val) ? 1 : 0) == 0)
            rc = 0; 
    }
    if (unexport_gpio_pin(gpio_pin) < 0)
        return -1;
#endif
    return rc;
}

int xpi_enable_video_input_to_pc(void)
{
    int rc = -1, value;
#ifdef __XPI__
    if (xpi_get_gpio_pin_value(GPIO_HPD_NOR_PROC, &value) < 0)
        return -1;

    rc = xpi_set_gpio_pin_value(GPIO_HPD_CTRL, !value);
#endif
    return rc;
}

int xpi_disable_video_input_to_pc(void)
{
    int rc = -1, value;
#ifdef __XPI__

    if (xpi_get_gpio_pin_value(GPIO_HPD_NOR_PROC, &value) < 0)
        return -1;

    rc = xpi_set_gpio_pin_value(GPIO_HPD_CTRL, value);
#endif
    return rc;
}
