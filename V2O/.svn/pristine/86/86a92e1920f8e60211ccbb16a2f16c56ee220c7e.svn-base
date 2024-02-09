#include "custmenu.h"
#include "commonutils.h"
#include "system_utils.h"
#include "vlog.h"
#include "vutils.h"




int set_license_file_main(int argc, char **argv) {


    int rc = -1;

    system_command("/bin/cfunmount");
    rc = system_command("/bin/cfmount");
    if (rc != 0) {
        printf("Failed in mounting /mnt/cf \n");
        return -1;
    }
    rc = system_command("/bin/mv /tmp/barcoLicense.txt  /mnt/cf");
    if (rc != 0) {
        printf("Upload file failed \n");
        return -1;
    }

    rc = system_command("/bin/cfunmount");

    printf("Upload file success \n");
    return 1;

}

