#include <stdlib.h>
#include <string.h>

#define CUSTOM_MENU_DEFS

#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "vlog.h"
#include "vutils.h"
#include "system_utils.h"
#include "hal.h"
#include "device_detection.h"
#include "get_license_pin.h"
#include "verify_license.h"
#include "vlog.h"

//#include "blm.h"
//#include "blmlibProcs.h"


#define BARCO_LICENSE_FILE_PATH  "/mnt/cf/barcoLicense.txt"
#define TMP_LICENSE_PATH   "/mnt/cf/barcolic.txt"

static int LicenseValid[10];
static int DEBUG =  1;


static int PrintContentofLicenseFile(void) {

    FILE *fp;
    char linebuffer[1024];



    fp = fopen(BARCO_LICENSE_FILE_PATH, "r");
    if (fp == NULL)
        return 0;

    printf("\n");
    //printf("Below is the content of the License file \n");
    printf("Current Licenses \n");
    printf("\n");
    while (fgets(linebuffer, sizeof(linebuffer), fp)) {
        printf("%s", linebuffer);

    }
    fclose(fp);
    return 1;
}

static int checkpinvalidation(char *pincode) {

    int getpin = get_license_pin();
    char cmd[16]; 
    char pin[8];
    int idx = 0;
    char *cp;
    int len = 0;

    len = strlen(pincode);
    strcpy(pin, pincode + 8);

    sprintf(cmd, "%d", getpin);

    if (!strcmp(pin, cmd))
        return 1;
    else 
        return 0;

}

static int  CheckForLicenseType(char product[10][20], char *typetocheck, int index) {

    int i = 0;
    char *cp; 
    int baselicensefound = 0;
    int  isvalidpin = 0;


    int hardware_profile = get_hardware_profile();
    if (hardware_profile == HARDWARE_PROFILE_DMS1550) {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, typetocheck) && LicenseValid[i]) {
                baselicensefound = 1; 
            }
        }

    } else if (hardware_profile == HARDWARE_PROFILE_DMS150) {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, typetocheck) &&  LicenseValid[i])  {
                baselicensefound = 1;

            }
        }


    } else if (hardware_profile == HARDWARE_PROFILE_MS150) {

        for (i = 0 ; i < index; i ++) {
            cp = product[i];

            if (!strcmp(cp, typetocheck) &&  LicenseValid[i]) {
                baselicensefound = 1;

            }

        }
    } else {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, typetocheck) &&  LicenseValid[i])  {
                baselicensefound = 1;

            }
        }
    }

    return 1;
    /*if (baselicensefound) 
        return 1;
    else 
        return 0;*/
}

//static int  CheckLicenseAgainstValidation(char product[10][20], int index) {
static int  CheckForBaseLicense(char product[10][20], int index) {

    int i = 0;
    char *cp; 
    int baselicensefound = 0;
    int  isvalidpin = 0;


    int hardware_profile = get_hardware_profile();
    if (hardware_profile == HARDWARE_PROFILE_DMS1550) {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, "DMS_1550_Base") && LicenseValid[i]) {
                baselicensefound = 1; 
            }
        }

    } else if (hardware_profile == HARDWARE_PROFILE_DMS150) {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, "DMS_150_Base") &&  LicenseValid[i])  {
                baselicensefound = 1;

            }
        }


    } else if (hardware_profile == HARDWARE_PROFILE_MS150) {

        for (i = 0 ; i < index; i ++) {
            cp = product[i];

            if (!strcmp(cp, "MS_150_Base") &&  LicenseValid[i]) {
                baselicensefound = 1;

            }

        }
    } else {
        for (i = 0 ; i < index; i ++) {
            cp = product[i];

            if (!strcmp(cp, "DMS_150_Base") &&  LicenseValid[i]) {
                baselicensefound = 1;

            }

        }

    }


    return 1;
    /*if (baselicensefound) 
        return 1;
    else 
        return 0;*/
}


static int  CheckLicenseAgainstHw(char product[10][20], char pincode[10][512], int index) {

    int i = 0;
    char *cp; 
    int baselicensefound = 0;
    int  isvalidpin = 0;


    int hardware_profile = get_hardware_profile();
    if (hardware_profile == HARDWARE_PROFILE_DMS1550) {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, "DMS_1550_Base")) {
                baselicensefound = 1; 
                isvalidpin = checkpinvalidation(pincode[i] ); 
            }
        }

    } else if (hardware_profile == HARDWARE_PROFILE_DMS150) {

        for (i = 0 ; i <  index; i++) {
            cp = product[i];

            if (!strcmp(cp, "DMS_150_Base")) {
                baselicensefound = 1;
                isvalidpin = checkpinvalidation(pincode[i] );

            }
        }


    } else if (hardware_profile == HARDWARE_PROFILE_MS150) {

        for (i = 0 ; i < index; i ++) {
            cp = product[i];

            if (!strcmp(cp, "MS_150_Base")) {
                LicenseValid[i] = 1;
                baselicensefound = 1;
                isvalidpin = checkpinvalidation(pincode[i] );

            }

        }
    } else {
        for (i = 0 ; i < index; i ++) {
            cp = product[i];

            if (!strcmp(cp, "DMS_150_Base")) {
                baselicensefound = 1;
                isvalidpin = checkpinvalidation(pincode[i] );

            }
        }

    }

    return 1;
    /*if (baselicensefound && isvalidpin) 
        return 1;
    else 
        return 0;*/

}


static void clean_up_tmp_files(void) {

    struct stat sbuf;

    if (stat(TMP_LICENSE_PATH, &sbuf) == 0)
        remove(TMP_LICENSE_PATH);

}


static void setup_license_file_location(void) {

    /* Set the license file location the one which has only one line at a time*/ 
    
    setenv("BLM_LICENSE_FILE", TMP_LICENSE_PATH, 0);

}

static int init_license(char * product) {

    char * division = "BSMD";
    char * version = "1.0";
    char * err_string = NULL;
    int result = 0;

//    result = blmLicenseSAInit(product, division, version, NULL, 
//            &err_string, NULL);
    return (result == 0) ? 0 : -1;

}


static int check_feature(char * product, char * feature, char * pincode) {
    char * err_string = NULL;


    return 0;
    //if (blmLicenseCheck(product, feature, pincode, &err_string) == 0)
    //    return 0;

    //return -1;
}

static void deinit_license(char * product) {
    //blmLicenseEnd(product);
}


static void get_license_info(char *cp, char *buff, int index) {

    int cnt = 0;

    if (cp == NULL)
        return;

    while(*cp != ' ' || cp == NULL) {
        *buff++ = *cp++;
        cnt++;
    }

    *buff = '\0';
}

static void get_line_custom_info(char *cp, char *buff, int index) {

    int cnt = 0;

    if (cp == NULL)
        return;

    while(*cp != ' ' || cp == NULL) {
        *buff++ = *cp++;
        cnt++;
    }

    *buff = '\0';

}



static int ValidateLicense(char product[10][20], char pincode[10][512], char feature[10][20], int ind) {


    FILE *fp;
    FILE *lpf;
    int i = 0;
    char linebuffer[1024];
    int index = 0;
    int cmdbuf[32];


    setup_license_file_location();

    fp = fopen(BARCO_LICENSE_FILE_PATH, "r");
    if (fp == NULL)
        return 0;

    while (fgets(linebuffer, sizeof(linebuffer), fp)) {
        lpf = fopen(TMP_LICENSE_PATH, "w");
        if (lpf == NULL) {
            printf("Error in opening license tmp file \n");
            return 0;
        }

        fprintf(lpf, "%s", linebuffer);
        fclose(lpf);


        init_license(product[index]);
        if (check_feature(product[index], feature[index], pincode[index]) == 0)
            LicenseValid[index] = 1;
        else 
            LicenseValid[index] = 0; 
        index++;

    }
    fclose(fp);


    return 1;
}


static int CheckAndValidateLicense(char product[10][20],  char pincode[10][512], char feature[10][20], int index) {

    int i = 0;

    if (CheckLicenseAgainstHw(product, pincode, index))  {
        ValidateLicense(product, pincode, feature, index);

        if (!CheckForBaseLicense(product, index)) {
            if (DEBUG)
                printf("Valid license on this device is not found \n");
        }
        else {
            for (i = 0; i < index; i++) {
                if (LicenseValid[i] )
                    ;//if (DEBUG)
                     //   printf("License Type=%-15s \n", product[i]); 
            }
        }
    } else {
        if (DEBUG)
            printf("Valid license is not found on this device \n");
    }

    if (DEBUG)
        PrintContentofLicenseFile();

    clean_up_tmp_files();


    return 1;


}


static int ReadContentsFromFile(char product[10][20], char pincode[10][512], char feature[10][20], int *items) {


    FILE *fp;
    int linenumber = 0;
    char linebuffer[1024];
    char *cp;
    int index = 0;
    char cmdbuf[512];

    fp =fopen(BARCO_LICENSE_FILE_PATH, "r");
    if (fp == NULL)
        return 0;


    while(fgets(linebuffer, sizeof(linebuffer), fp)) {

        linenumber++;


        cp = strstr(linebuffer, "PRODUCT");
        if(cp != NULL) {
            cp = cp + 8;
            get_license_info(cp, &product[index][0], index);
        }

        if (linenumber == 1) {

            cp = strstr(linebuffer, "CUSTOM");
            if (cp != NULL) {
                cp = cp + 7 ;
                get_line_custom_info(cp, &pincode[index][0], index);
                sprintf(cmdbuf, "%s", pincode[index]);
            }


        } else {
            cp = strstr(linebuffer, "Pincode=");

            if (cp != NULL) {
                cp = cp + 8;
                get_license_info(cp, cmdbuf, index);
                sprintf(pincode[index], "Pincode=%s", cmdbuf);
            }
        }

        cp = strstr(linebuffer, "FEATURE");

        if (cp != NULL) {
            cp = cp + 8;
            get_license_info(cp, &feature[index][0], index);
        }


        index++;

    }
    fclose(fp);

    *items = index;

    return 1;

}

static int CheckLicenseFileExist(void) {

 
    FILE *lpf;
    struct stat sbuf;
    int rc = 0;



    if (stat(BARCO_LICENSE_FILE_PATH, &sbuf) != 0) {
        printf("No License found on this Device \n");
        return 0;
    }


    return 1;

}


static int EvaluateLicenseType(char product[10][20], char pincode[10][512], char feature[10][20], int *index) {

    int rc = -1;
    system_command("/bin/cfunmount");
    rc = system_command("/bin/cfmount");
    if (rc != 0) {
        printf("Error mounting compact flash \n");
        return 0;
    }


    if (CheckLicenseFileExist())
        rc = ReadContentsFromFile(product, pincode, feature, index);

    if (!rc) {
        rc = system_command("/bin/cfunmount");
        return 0;
    }


    CheckAndValidateLicense(product, pincode, feature, *index);


    rc = system_command("/bin/cfunmount");

    return 1;


}

int verify_license_main(int argc, char **argv) {

    char pincode[10][512];
    char product[10][20];
    char feature[10][20];
    int index;
    int hardware_profile;

    hardware_profile = get_hardware_profile();


    if (((hardware_profile == HARDWARE_PROFILE_MS25 ) || (hardware_profile == HARDWARE_PROFILE_MS) ||  (hardware_profile == HARDWARE_PROFILE_XPI)))
        return 1;
    


    EvaluateLicenseType(product, pincode, feature, &index);

    return 1;
}



int isHavingBaseLicense(int debug) {

    char pincode[10][512];
    char product[10][20];
    char feature[10][20];
    int index;
    int hardware_profile;
    int rc = 0;

    DEBUG = debug;

    hardware_profile = get_hardware_profile();

    if ((hardware_profile != HARDWARE_PROFILE_DMS1550 &&  hardware_profile != HARDWARE_PROFILE_DMS150 &&
            hardware_profile != HARDWARE_PROFILE_MS150) ||  (hardware_profile == HARDWARE_PROFILE_XPI)) 
        return 1;
    

   rc =  EvaluateLicenseType(product, pincode, feature, &index);

   if (!rc)
       return 0;


    if (hardware_profile == HARDWARE_PROFILE_DMS1550)
        rc = CheckForLicenseType(product, "DMS_1550_Base", index);
    else if (hardware_profile == HARDWARE_PROFILE_DMS150)
        rc = CheckForLicenseType(product, "DMS_150_Base", index);
    else if (hardware_profile == HARDWARE_PROFILE_MS150)
        rc = CheckForLicenseType(product, "MS_150_Base", index);


    //return rc;
    return 1;

}

int isHavingNASLicense() {


    char pincode[10][512];
    char product[10][20];
    char feature[10][20];
    int index;
    int hardware_profile;
    int rc = 0;

    DEBUG = 0;
    hardware_profile = get_hardware_profile();

    if ((hardware_profile == HARDWARE_PROFILE_XPI) || (hardware_profile == HARDWARE_PROFILE_MS) ||
            (hardware_profile == HARDWARE_PROFILE_MS25))
        return 1;

    rc = EvaluateLicenseType(product, pincode, feature, &index);
    if (!rc)
        return 0;

    hardware_profile = get_hardware_profile();
    rc = CheckForLicenseType(product, "DMS_NAS", index);


    //return rc;
    return 1;

}



int isHavingDMSMSLicense(int debug) {


    char pincode[10][512];
    char product[10][20];
    char feature[10][20];
    int index;
    int hardware_profile;
    int rc = 0;

    DEBUG = debug;
    hardware_profile = get_hardware_profile();

    if ((hardware_profile != HARDWARE_PROFILE_DMS1550 &&  hardware_profile != HARDWARE_PROFILE_DMS150) 
            || (hardware_profile == HARDWARE_PROFILE_XPI)) 
        return 1;


    rc =  EvaluateLicenseType(product, pincode, feature, &index);
    if (!rc)
        return 0;

    rc = CheckForLicenseType(product, "DMS_MS", index);

    //return rc;
    return 1;

}


int isHavingPerfLicense(int debug) {


    char pincode[10][512];
    char product[10][20];
    char feature[10][20];
    int index;
    int hardware_profile;
    int rc = 0;

    DEBUG = debug;
    hardware_profile = get_hardware_profile();

    if ((hardware_profile != HARDWARE_PROFILE_DMS1550 &&  hardware_profile != HARDWARE_PROFILE_DMS150 &&
            hardware_profile != HARDWARE_PROFILE_MS150) ||  (hardware_profile == HARDWARE_PROFILE_XPI)) 
        return 1;

    rc = EvaluateLicenseType(product, pincode, feature, &index);
    if (!rc)
        return 0;

    if (hardware_profile == HARDWARE_PROFILE_DMS1550) 
        rc = CheckForLicenseType(product, "DMS_1550_Perf", index);
    else if (hardware_profile == HARDWARE_PROFILE_DMS150)
        rc = CheckForLicenseType(product, "DMS_150_Perf", index);


    //return rc;
    return 1;

}
