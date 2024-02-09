

#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "commonutils.h"
#include "device_detection.h"
#include "software_image_status_menu_gen.h"
#include "license_verifier.h"



int getSoftwareImageStatus(struct software_image_status_params *params) 
{

    get_software_version(params->version_value, 
                         FIELD_SOFTWARE_IMAGE_STATUS_VERSION_LENGTH);

    get_software_serviceversion(params->serviceversion_value, 
                         FIELD_SOFTWARE_IMAGE_STATUS_VERSION_LENGTH);

    get_software_date(params->date_value, 
                      FIELD_SOFTWARE_IMAGE_STATUS_DATE_LENGTH);

    get_device_serial_number(params->serial_number_value, 
                      FIELD_SOFTWARE_IMAGE_STATUS_SERIAL_NUMBER_LENGTH);


#ifdef __XPI__
    if (have_stereo_license())
        sprintf(params->model_value, "Pro3D");
    else if (have_duallink_license())
        sprintf(params->model_value, "Pro");
    else 
        sprintf(params->model_value, "Lite");


    get_sd_card_id(params->license_id_value, 
                      FIELD_SOFTWARE_IMAGE_STATUS_LICENSE_ID_LENGTH);

#endif

    return RC_NORMAL;
}

