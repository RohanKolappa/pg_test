#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <unistd.h>
#include <ctype.h>
#include <sys/types.h>
#include <sys/stat.h>
#include <fcntl.h>
#include <time.h>
#include <syslog.h>

#include "hal.h"
#include "vlog.h"
#include "system_utils.h"
#include "commonutils.h"
#include "device_detection.h"
#include "temperature_control.h"
#ifdef __XPI__
#include "xpi_i2c_control.h"
#else
#include "xp_i2c_control.h"
#endif

/****************************************************
 * Routine to read LM63 registers for XP100 and XPi *
 ****************************************************/
static unsigned char get_lm63_val_k(HAL *hp, int reg)
{
    unsigned char val = 0;
#ifdef __XPI__
    if (hal_check_expansion_lm63(hp)) 
        xpi_get_expansion_i2c_val(hp, EXPANSIONLM63_I2C_PORT, LM63_I2C_ADDR, 
                reg, &val);
    else if (hal_check_main_lm63(hp))
        xpi_get_main_i2c_val(hp, LM63_I2C_PORT, LM63_I2C_ADDR, reg, &val);
#else
    hal_read_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, LM63_I2C_ADDR, reg, 1, &val);
#endif
    return val;
}

static void set_lm63_val_k(HAL *hp, int reg, unsigned char val)
{
#ifdef __XPI__
    if (hal_check_expansion_lm63(hp)) 
        xpi_set_expansion_i2c_val(hp, EXPANSIONLM63_I2C_PORT, LM63_I2C_ADDR, 
                reg, val);
    else if (hal_check_main_lm63(hp)) 
        xpi_set_main_i2c_val(hp, LM63_I2C_PORT, LM63_I2C_ADDR, reg, val);
#else
    hal_write_i2c_val_k(hp, I2CIO_TYPE_MAIN_READ, LM63_I2C_ADDR, reg, 1, &val);
#endif
}

/*********************************************
 * Routine to read MAX6650 registers for XPi *
 *********************************************/
static unsigned char get_max6650_val_k(HAL *hp, int reg)
{
    unsigned char val = 0;
#ifdef __XPI__
    if (hal_check_max6650(hp))
        xpi_get_expansion_i2c_val(hp, EXPANSIONMAX6650_I2C_PORT, 
                MAX6650_I2C_ADDR, reg, &val);
#endif
    return val;
}

static void set_max6650_val_k(HAL *hp, int reg, unsigned char val)
{
#ifdef __XPI__
    if (hal_check_max6650(hp)) 
        xpi_set_expansion_i2c_val(hp, EXPANSIONMAX6650_I2C_PORT, 
                MAX6650_I2C_ADDR, reg, val);
#endif
}

/************************************
 * Routines to read XPi tempertures *
 ************************************/
#define OMNITEK_SYSMONDE_REG        12
#define OMNITEK_SYSMONDATAOUT_REG   15

int hal_get_xpi_fpga_temperature(HAL *hp, float *temp) 
{
    int fpgaADCValue;

    *temp = (float) 0;

#ifdef __XPI__
    /* Read tempearture from FPGA core */
    hal_set_omnitek_val_k(hp, OMNITEK_SYSMONDE_REG, 0x01);
    hal_set_omnitek_val_k(hp, OMNITEK_SYSMONDE_REG, 0x00);

    /* 10 bit ADC value MSB alisnged */
    fpgaADCValue = hal_get_omnitek_val_k(hp, OMNITEK_SYSMONDATAOUT_REG);
    fpgaADCValue = (fpgaADCValue >> 6) & 0x3ff;
    *temp = ((double) fpgaADCValue * 503.975) / (double) 1024 - (double) 273;
#endif

    return 0;
}

int hal_get_xpi_phy_temperature(HAL *hp, float *temp)
{
    unsigned long mdioval, pageval;

#ifdef __XPI__
    /* Read temperature from main PHY page 6, register 26 */
    if (hal_get_mdio_val_k(hp, 0, 6, 26, &mdioval) < 0) {
        VLOG_INFO("%s: failed to read MDIO phyad 1 reg 0x1a\n", __func__);
        return -1;
    }
    mdioval &= 0x1f;
    *temp = ((float) mdioval * (float) 5) - (float) 25;
#endif

    return 0;
}

/* Returns CPU temperature in degrees centigrade */
int hal_get_xpi_cpu_temperature(HAL *hp, float *temp) 
{
    int rc = get_sysfs_attribute(HWMON_TEMP_MODE1, "%d\n");

    if (rc < 0) 
        *temp = (float) 0;
    *temp = (float) rc / (float) 1000; 
    return (rc < 0) ? -1 : 0;
}

/*************************************
 * Routine to read board temperature *
 *************************************/
int hal_get_board_temperature(HAL *hp, float *ltemp, float *rtemp) {
    int local = 0, remote = 0;
    unsigned char reglt, regrthb, regrtlb;

    *ltemp = *rtemp = 0; 

    reglt = get_lm63_val_k(hp, LM63REG_LT);
    regrthb = get_lm63_val_k(hp, LM63REG_RTHB);
    regrtlb = get_lm63_val_k(hp, LM63REG_RTLB);

    local = (reglt & 0x80) ? ((int) reglt - 0x100) : (int) reglt;
    remote = ((int) (regrthb & 0x7f) << 3) | ((int) regrtlb >> 5);
    remote = (regrthb & 0x80) ? (remote - 0x100) : remote;
    *ltemp = (float) local;
    *rtemp = (float) remote / (float) 8;

    return 0;
}

/***********************************************************
 * Routine to get and set XP100 and XPi fan speed. The XPi *
 * code is based on kernel/drivers/hwmon/max6650.c         *
 ***********************************************************/
#define FAN_FVALUE                      1

/* LM63 PWM register values */

#define LM63_PWM_VALUE_LOW              0x18
#define LM63_PWM_VALUE_MEDIUM           0x24
#define LM63_PWM_VALUE_HIGH             0x30

/* MAX6650 Config register bits */

#define MAX6650_CFG_PRESCALER_MASK      0x07
#define MAX6650_CFG_PRESCALER_1         0x00
#define MAX6650_CFG_PRESCALER_2         0x01
#define MAX6650_CFG_PRESCALER_4         0x02
#define MAX6650_CFG_PRESCALER_8         0x03
#define MAX6650_CFG_PRESCALER_16        0x04
#define MAX6650_CFG_V12                 0x08
#define MAX6650_CFG_MODE_MASK           0x30
#define MAX6650_CFG_MODE_ON             0x00
#define MAX6650_CFG_MODE_OFF            0x10
#define MAX6650_CFG_MODE_CLOSED_LOOP    0x20
#define MAX6650_CFG_MODE_OPEN_LOOP      0x30
#define MAX6650_COUNT_MASK              0x03

/* MAX6650 fan speed values */

#define MAX6650_SPEED_VALUE_LOW         0xf8
#define MAX6650_SPEED_VALUE_MEDIUM      0x20
#define MAX6650_SPEED_VALUE_HIGH        0x00

#define MAX6650_RPM_VALUE_HIGH         20000
#define MAX6650_RPM_VALUE_MEDIUM        1900

void hal_init_fan_speed(HAL *hp, int speed) {
    unsigned char val;
    int profile = get_hardware_profile();

    if (profile == HARDWARE_PROFILE_XP100) {
        VLOG_INFO_CAT(VLOG_VLIB, "Setting Fan Speed to %s", 
            (speed == FAN_SPEED_LOW) ? "Low" : 
            ((speed == FAN_SPEED_MEDIUM) ? "Medium" : "High"));
        /* Set LM63 PWM and RPM Reg to 0x28 */
        set_lm63_val_k(hp, LM63REG_PWM_RPM, 0x2a);
        /* Set LM63 Fan Spin Up Config to 0x00 */
        set_lm63_val_k(hp, LM63REG_SPINUP_CFG, 0x00);
        /* Set LM63 PWM Freq to 0x17 */
        set_lm63_val_k(hp, LM63REG_PWM_FREQ, 0x17);
        /* Set LM63 PWM Value to 0x17 */
        set_lm63_val_k(hp, LM63REG_PWM_VAL, 0x17);
        /* Write 0x86 to LM63 Configuration */
        set_lm63_val_k(hp, LM63REG_CFG, 0x86);
        /* Write a '1' bit to T_Crit_Limit_Override in LM63 Configuration */
        val = get_lm63_val_k(hp, LM63REG_CFG);
        set_lm63_val_k(hp, LM63REG_CFG, val | 0x02);
        /* Set LM63 Remote T-Crit_Setpoint to 0x7f */
        set_lm63_val_k(hp, LM63REG_RMTCRI_SETPOINT, 0x7f);
        /* Set initial fan speed */
        val = (speed == FAN_SPEED_LOW) ? LM63_PWM_VALUE_LOW : 
           ((speed == FAN_SPEED_MEDIUM) ? LM63_PWM_VALUE_MEDIUM : 
            LM63_PWM_VALUE_HIGH);
        set_lm63_val_k(hp, LM63REG_PWM_VAL, val);
    }
#ifdef __XPI
    else if (profile == HARDWARE_PROFILE_XPI && hal_check_max6650(hp)) {
        /* Initialize fan configuration to 12v, closed-loop operation */
        /* prescaler division 1, speed to maximum (500 rps/30000 rpm) */  
        VLOG_INFO_CAT(VLOG_VLIB, "Initializing MAX6650 registers..."); 
        set_max6650_val_k(hp, MAX6650REG_CONFIG, MAX6650_CFG_V12 |
                MAX6650_CFG_MODE_CLOSED_LOOP | MAX6650_CFG_PRESCALER_1 | 
                MAX6650_CFG_MODE_ON);
        VLOG_INFO_CAT(VLOG_VLIB, "Setting Fan Speed to %s", 
            (speed == FAN_SPEED_LOW) ? "Low" : 
            ((speed == FAN_SPEED_MEDIUM) ? "Medium" : "High"));
        set_max6650_val_k(hp, MAX6650REG_SPEED,
                (speed == FAN_SPEED_LOW) ? MAX6650_SPEED_VALUE_LOW :
                ((speed == FAN_SPEED_MEDIUM) ? MAX6650_SPEED_VALUE_MEDIUM :
                 MAX6650_SPEED_VALUE_HIGH));
    }
#endif
}

int hal_get_fan_speed(HAL *hp) {
    unsigned char val;
    int profile = get_hardware_profile();

    if (profile == HARDWARE_PROFILE_XP100) {
        val = get_lm63_val_k(hp, LM63REG_PWM_RPM);
        if (val & 0x10)
            return FAN_SPEED_OFF;
        val = get_lm63_val_k(hp, LM63REG_PWM_VAL);
        if (val >= LM63_PWM_VALUE_HIGH)
            return FAN_SPEED_HIGH;
        else if (val >= LM63_PWM_VALUE_MEDIUM)
            return FAN_SPEED_MEDIUM;
        else
            return FAN_SPEED_LOW;
    }
#ifdef __XPI__
    else if (profile == HARDWARE_PROFILE_XPI && hal_check_max6650(hp)) {
        if (get_max6650_val_k(hp, MAX6650REG_CONFIG) & MAX6650_CFG_MODE_OFF)
            return FAN_SPEED_OFF;
        val = (get_max6650_val_k(hp, MAX6650REG_TACH0) * 120) /
                (1 << (get_max6650_val_k(hp, MAX6650REG_COUNT) & 7));
#if 0
        //FIXME: val is a char, can never be greater than 20000 or 1900!
        if (val >= MAX6650_RPM_VALUE_HIGH)
            return FAN_SPEED_HIGH;
        else if (val >= MAX6650_RPM_VALUE_MEDIUM)
            return FAN_SPEED_MEDIUM;
        else
#endif
            return FAN_SPEED_LOW;
    }
#endif
    return 0;
}

void hal_set_fan_speed(HAL *hp, int speed) {
    unsigned char val;
    int profile = get_hardware_profile();

    if (profile == HARDWARE_PROFILE_XP100) {
        if (speed == FAN_SPEED_OFF) 
            set_lm63_val_k(hp, LM63REG_PWM_RPM, 0x3a);
        else {
            set_lm63_val_k(hp, LM63REG_PWM_RPM, 0x2a);
            if (speed == FAN_SPEED_LOW)
                val = LM63_PWM_VALUE_LOW;
            else if (speed == FAN_SPEED_MEDIUM)
                val = LM63_PWM_VALUE_MEDIUM;
            else
                val = LM63_PWM_VALUE_HIGH;
            set_lm63_val_k(hp, LM63REG_PWM_VAL, val);
        }
    }
#ifdef __XPI__
    else if (profile == HARDWARE_PROFILE_XPI && hal_check_max6650(hp)) {
        if (speed == FAN_SPEED_OFF) 
            set_max6650_val_k(hp, MAX6650REG_CONFIG, MAX6650_CFG_V12 |
                MAX6650_CFG_MODE_CLOSED_LOOP | MAX6650_CFG_PRESCALER_1 | 
                MAX6650_CFG_MODE_OFF);
        if (speed == FAN_SPEED_LOW)
            set_max6650_val_k(hp, MAX6650REG_SPEED, MAX6650_SPEED_VALUE_LOW);
        else if (speed == FAN_SPEED_MEDIUM)
            set_max6650_val_k(hp, MAX6650REG_SPEED, MAX6650_SPEED_VALUE_MEDIUM);
        else 
            set_max6650_val_k(hp, MAX6650REG_SPEED, MAX6650_SPEED_VALUE_HIGH);
    }
#endif
}

int hal_get_fan_rpm(HAL *hp) {
    unsigned short cnt;
    unsigned char val;
    static unsigned int firsttime, lastrpm; 
    static struct timeval lasttime;
    struct timeval now;
    int profile = get_hardware_profile();

    gettimeofday(&now, (struct timezone *) 0);
    if (!firsttime) {
        int timediff = TIMEVAL_DIFF(now, lasttime);
        if (timediff < 5000000) 
            return lastrpm;
    }
    else {
        firsttime = 0;
    }

    lasttime = now;

    if (profile == HARDWARE_PROFILE_XP100) {
        val = get_lm63_val_k(hp, LM63REG_TACHCNT_LOW);
        cnt = val | (get_lm63_val_k(hp, LM63REG_TACHCNT_HIGH) << 8);
        if (cnt == 0xffff)
            lastrpm = 0;
        else
            lastrpm = ((double) FAN_FVALUE * (double) 5400000 / (double) cnt);
        return lastrpm;
    }
#ifdef __XPI__
    else if (profile == HARDWARE_PROFILE_XPI && hal_check_max6650(hp)) {
        /* Each tachometer counts over an interval given by "count" */
        /* register (0.25, 0.50, 1 or 2 seconds). We assume that    */
        /* the fans produce two pulses per revolution               */
        if (hal_get_fan_speed(hp) == FAN_SPEED_OFF)
            lastrpm = 0; 
        else {
            val = get_max6650_val_k(hp, MAX6650REG_TACH0);
            lastrpm = (val * 120) / 
                (1 << (get_max6650_val_k(hp, MAX6650REG_COUNT) & 7));
        }
        return lastrpm;
    }
#endif
    return -1;
}

/************************************************
 * Routine to monitor XP100 and XPi temperature *
 ************************************************/
#define MAX_TEMP_INDEX      5
#define MAX_TEMP_SAMPLES    5

int hal_monitor_temperature(HAL *hp, int print) 
{
    static float temp[MAX_TEMP_SAMPLES][MAX_TEMP_INDEX];
    static int samples = 0;
    int i, j, profile = get_hardware_profile();
    float sum[MAX_TEMP_INDEX], ctemp, localtemp, remotetemp;

    if ((profile != HARDWARE_PROFILE_XP100 && profile != HARDWARE_PROFILE_XPI))
        return -1;

    if (samples == 0) {
        for (i = 0; i < MAX_TEMP_INDEX; i++) {
            sum[i] = (float) 0;
            for (j = 0; j < MAX_TEMP_SAMPLES; j++)
                temp[j][i] = 0;
        }
    }

    if (hal_get_board_temperature(hp, &localtemp, &remotetemp) < 0)
        return -1;

#ifndef __XPI__
    ctemp = get_sysfs_attribute(TEMPERATURE_PROC_FILE, "temperature: %d C\n");
    temp[samples % MAX_TEMP_SAMPLES][0] = ctemp;
    temp[samples % MAX_TEMP_SAMPLES][1] = localtemp;
    temp[samples % MAX_TEMP_SAMPLES][2] = remotetemp;
    temp[samples % MAX_TEMP_SAMPLES][3] = 0;
    temp[samples % MAX_TEMP_SAMPLES][4] = 0;
    if (print) {
        printf("  %5.2f    %5.2f    %5.2f\n", ctemp, localtemp, remotetemp);
    }
#else
    hal_get_xpi_cpu_temperature(hp, &ctemp);
    temp[samples % MAX_TEMP_SAMPLES][0] = ctemp;
    temp[samples % MAX_TEMP_SAMPLES][1] = localtemp;
    temp[samples % MAX_TEMP_SAMPLES][2] = remotetemp;
    hal_get_xpi_fpga_temperature(hp, &ctemp);
    temp[samples % MAX_TEMP_SAMPLES][3] = ctemp;
    hal_get_xpi_phy_temperature(hp, &ctemp);
    temp[samples % MAX_TEMP_SAMPLES][4] = ctemp;
    if (print) {
        printf(" %5.2f  %5.2f  %5.2f  %5.2f  %5.2f\n", 
                temp[samples % MAX_TEMP_SAMPLES][0], localtemp, remotetemp,
                temp[samples % MAX_TEMP_SAMPLES][3], ctemp);
    }
#endif
    samples++;

    for (i = 0; i < MAX_TEMP_INDEX; i++) {
        for (j = 0; j < MAX_TEMP_SAMPLES; j++) 
            sum[i] += temp[j][i];
        if (samples >= MAX_TEMP_SAMPLES) 
            sum[i] /= (float) MAX_TEMP_SAMPLES;
    }
    
    for (i = 0; i < MAX_TEMP_INDEX; i++) { 
        if (sum[i] >= TEMP_SETPOINT_HIGH)
            return TEMP_RANGE_HIGH;
        if (sum[i] >= TEMP_SETPOINT_MEDIUM)
            return TEMP_RANGE_MEDIUM_2;
        if (sum[i] >= TEMP_SETPOINT_LOW)
            return TEMP_RANGE_MEDIUM_1;
        if (sum[i] >= TEMP_SETPOINT_VERY_LOW)
            return TEMP_RANGE_LOW_2;
    }
    return TEMP_RANGE_LOW_1;
}

