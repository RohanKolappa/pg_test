#include <stdlib.h>
#include <stdio.h>
#include <string.h>

#ifdef __XPI__

#include "blm.h"
#include "blmlibProcs.h"

#include "license_verifier.h"

static int init_license(char * product) {
    char * division = "BSMD";
    char * version = "1.0";
    char * err_string = NULL;
    int result;

    result = blmLicenseSAInit(product, division, version, NULL, 
                              &err_string, NULL);
    return (result == 0) ? 0 : -1;

}

static int check_feature(char * product, char * feature, char * serial_number) {
    char * err_string = NULL;

    if (blmLicenseCheck(product, feature, serial_number, &err_string) == 0)
        return 0;
    return -1;
}

static void deinit_license(char * product) {
    blmLicenseEnd(product);
}

static void setup_license_file_location(void) {
    /* Set the license file location to the one in compact flash if not 
       already provided by the environment variable (don't overwrite) */
    setenv("BLM_LICENSE_FILE", "/mnt/cf/barcoLicense.txt", 0);
}

static int verify_license(char * product, char * feature, char * serial_number)
{
    int result = -1;

    if (serial_number == NULL || strlen(serial_number) == 0)
        return -1;

    setup_license_file_location();


    if (init_license(product) == 0) {
        if (check_feature(product, feature, serial_number) == 0)
            result = 0;
    }
    deinit_license(product);
    return result;
}

static int read_firstline_from_file(const char *filename, 
                                  char * buffer, int buffer_len) {

    FILE * fp = fopen(filename, "r");
    if (fp == NULL)
        return -1;

    strcpy(buffer, "");

    if (fgets(buffer, buffer_len, fp) == NULL) {
        fclose(fp);
        return -1;
    }

    /* take off the new line at the end of the string */
    buffer[strlen(buffer) - 1] = '\0';

    fclose(fp);
    return 0;
}

static char * get_sd_card_serial_number(void) {
    /* Read the SD card serial number from the mmc block device CID */
    static char buffer[40];
    if (read_firstline_from_file("/sys/block/mmcblk0/device/cid", buffer, 40) == -1)
        return NULL;
    return buffer;
}


static char * get_product_string(void) {
    return "NGS-D200";
}

static int have_feature_license(char * feature) {
    if (verify_license(get_product_string(), 
                       feature, get_sd_card_serial_number()) == 0)
        return 1;
    return 0;
}

int have_basic_license() {
    return have_feature_license("basic");
}

int have_duallink_license() {
    return have_feature_license("duallink");
}

int have_stereo_license() {
    return have_feature_license("stereo");
}

#endif      /* __XPI__ */
