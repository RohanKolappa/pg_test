#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#include <sys/socket.h>
#include <arpa/inet.h>
#include <netinet/in.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "custmenu.h"
#include "menuutils.h"
#include "configureSystem.h"
#include "configureVideo.h"
#include "loadnet.h"
#include "commonutils.h"
#include "validationroutines.h"
#include "configfile_parser.h"
#include "vlog.h"
#include "remoteConfig.h"
#include "lut_utils.h"
#include "override_config.h"
#include "override_list_menu.h"
#include "strmsrv_comm.h"
#include "cli_utils.h"

#define FIELD_DUMMY        0
#define FIELD_ALUTREG      1
#define FIELD_OV_ALUTREG   2
#define FIELD_OV_STEREO    3
#define FIELD_OV_HRES      4
#define FIELD_OV_VRES      5
#define FIELD_OV_REFRESH   6
#define FIELD_OV_PCR       7
#define FIELD_NAME         8
#define FIELD_HRES         9
#define FIELD_VRES         10
#define FIELD_REFRESH      11
#define FIELD_PCR          12
#define FIELD_HFP          13
#define FIELD_VFP          14
#define FIELD_HSYNC        15
#define FIELD_VSYNC        16
#define FIELD_HBP          17
#define FIELD_VBP          18
#define FIELD_STEREO       19
#ifndef LIBONLY

static char *describe_override_list = "Describe Override LUT Table";
static char *get_override_list = "Get List of Override LUTs Table";
static char *add_des_override_list = "Add List of Override LUTs Entry";
static char *mod_des_override_list = "Modify List of Override LUTs Entry";
static char *del_des_override_list = "Delete List of Override LUTs Entry";
static char *override_list_title = "OverrideList";

static int update_override_list_dialog(MENUINFO * minfo, int idx,
                                       int req_code);
static int delete_override_list(int idx, int *override_list_idp);
static int update_override_list_entry(int idx,
                                      struct override_list_params *params);
static char *override_list_help =
    "Press </U>A<!U> to Add, </U>D<!U> to Delete, </U>M<!U> to Modify,\n</U>Enter<!U> to Browse or </U>Q<!U> to Quit. ";
static char *add_override_list_dialog_help =
    "Press </U>Enter<!U> to Select Entry for Override List or </U>Q<!U> to Quit.";

static struct option override_list_options[] = {
    {"alutreg", 1, 0, 0},
    {"ov_alutreg", 1, 0, 0},
    {"ov_stereo", 1, 0, 0},
    {"ov_hres", 1, 0, 0},
    {"ov_vres", 1, 0, 0},
    {"ov_refresh", 1, 0, 0},
    {"ov_pcr", 1, 0, 0},
    {"clientData", 1, 0, 0},
    {"boardNumber", 1, 0, 0},
    {0, 0, 0, 0}
};

static char *ov_stereo_values[] = { "No", "Yes", "Any", NULL };

static int lutlistcount = 0;

static int hardware_type = PLX_DEVICE;

#endif                          /* LIBONLY */

void ApplyOverrideListToDisplay(int req_code, HAL *hp);

struct override_list_params *override_list_table_head = NULL;

struct override_list_params *first_override_list_entry(void)
{
    return override_list_table_head;
}

struct override_list_params *next_override_list_entry(struct
                                                      override_list_params
                                                      *params)
{
    return params->next;
}

static int board_number = 0;
static struct lut_list_params lutlistparams;

int validateOverrideList(int req_code,
                         struct override_list_params *params,
                         struct override_list_params *orig_params,
                         char * error_string) {
                                                                                    
    struct override_list_params *paramsp;

    // Validate add request
    if (req_code == ADD_REQUEST) {
        foreach_override_list_entry(paramsp) {
            if (paramsp->alutreg_value == params->alutreg_value) {
                sprintf(error_string, "Duplicate: LUTID \'%d\' conflicts.",
                                       params->alutreg_value);
                return RC_DUPLICATE;
            }
        }
    }

    // Should not allow change of LUTID during modify
   if (req_code == MOD_REQUEST) {
       if ((params->alutreg_set == 1) &&
           (params->alutreg_value != orig_params->alutreg_value)) {
           sprintf(error_string, "Cannot modify LUTID");
           return RC_ERROR;
       }
   }
   return RC_NORMAL;
}

// Apply request to LUT list
int applyOverrideList(int req_code,
                      struct override_list_params *params,
                      struct override_list_params *orig_params) {
    HAL *hp;
                                                                                     if (req_code == DEL_REQUEST)
        hp = orig_params->userptr;
    else
        hp = params->userptr;

    if (hp == NULL) {
        return RC_ERROR;
    }

    return RC_NORMAL; 
}

void ApplyOverrideListToDisplay(int req_code, HAL *hp) {
    struct override_config override_cfg;
    struct strmsrv_rx_clnt_params clnt_params;

    if (hp == NULL) 
        return;

    if (get_override_config(hp, &override_cfg) < 0) 
        return;

    // Update display LUT if override option is Auto
    if (hal_get_hardware_type(hp) == V2DI50_DEVICE &&
            override_cfg.ov_option == OV_OPTION_AUTO) {
        memset(&clnt_params, 0x00, sizeof(clnt_params));
        clnt_params.ov_option_set = 1;
        clnt_params.ov_option = OV_OPTION_AUTO;
        strmsrv_set_rx_clnt_params(hal_get_channel_number(hp), &clnt_params);
    }
}

void OverrideListInit(void) {
    registerOverrideListSavedFunction(
            (OverrideListSavedFunction *) ApplyOverrideListToDisplay);
}

static char *get_config_file_name(void)
{
    static char buf[128];
    sprintf(buf, "/V2O/config/overridelistconf_%d.txt", board_number);
    return buf;
}

#ifndef LIBONLY
static char *get_lutname(char *buf, void *usrptr, int ident) 
{
    strcpy(buf, "???");
    
    /* get LUT list parameters only once */
    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->alutreg_set == 1) {
            if (get_lut_list_params_by_lutid(&lutlistparams, 
                        params->alutreg_value) != 0) {
                strncpy(buf, lutlistparams.name_value, 
                        FIELD_OVERRIDE_LIST_NAME_LENGTH + 1);
            }
            else 
                memset(&lutlistparams, '\0', sizeof(struct lut_list_params));
        }
    }
    else {
        strncpy(buf, lutlistparams.name_value, 
                FIELD_OVERRIDE_LIST_NAME_LENGTH + 1);
    }
    return buf;
}
#endif                          /* LIBONLY */

static char *get_alutreg(char *buf, void *usrptr, int ident)
{
    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->alutreg_set == 1)
            strncpy(buf, params->alutreg,
                    FIELD_OVERRIDE_LIST_ALUTREG_LENGTH + 1);
    }
    else {
        snprintf(buf, FIELD_OVERRIDE_LIST_ALUTREG_LENGTH, "%d",
                lutlistparams.alutreg_value);
    }

    return buf;
}

#ifndef LIBONLY
static char *get_stereo(char *buf, void *usrptr, int ident) 
{
    if (lutlistparams.stereo_value == LUT_LIST_STEREO_NO)
        strcpy(buf, "No");
    else if (lutlistparams.stereo_value == LUT_LIST_STEREO_YES)
        strcpy(buf, "Yes");
    else
        strcpy(buf, "Both");
    return buf;
}

static char *get_hres(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_OV_HRES_LENGTH + 1, "%d", 
           lutlistparams.orig_hres_value); 
    return buf;
}

static char *get_vres(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_OV_VRES_LENGTH + 1, "%d", 
               lutlistparams.orig_vres_value); 
    return buf;
}

static char *get_refresh(char *buf, void *usrptr, int ident) 
{
    sprintf(buf, "%-6.2f", lutlistparams.refresh_value); 
    return buf;
}

static char *get_pcr(char *buf, void *usrptr, int ident) 
{
    sprintf(buf, "%-6.2f", lutlistparams.pcr_value); 
    return buf;
}

static char *get_hfp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_HFP_LENGTH + 1, "%d", 
               lutlistparams.hfp_value); 
    return buf;
}

static char *get_vfp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_VFP_LENGTH + 1, "%d", 
               lutlistparams.vfp_value); 
    return buf;
}

static char *get_hsync(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_HSYNC_LENGTH + 1, "%d", 
               lutlistparams.hsync_value); 
    return buf;
}

static char *get_vsync(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_VSYNC_LENGTH + 1, "%d", 
               lutlistparams.vsync_value); 
    return buf;
}

static char *get_hbp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_HBP_LENGTH + 1, "%d", 
               lutlistparams.hbp_value); 
    return buf;
}

static char *get_vbp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_OVERRIDE_LIST_VBP_LENGTH + 1, "%d", 
               lutlistparams.vbp_value); 
    return buf;
}
#endif                          /* LIBONLY */

static char *get_ov_alutreg(char *buf, void *usrptr, int ident)
{
    strncpy(buf, "Any", FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH + 1);

    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->ov_alutreg_set == 1)
            strncpy(buf, params->ov_alutreg,
                    FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH + 1);
    }
    return buf;
}

static char *get_ov_stereo(char *buf, void *usrptr, int ident)
{
    strncpy(buf, "Any", FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH + 1);

    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->ov_stereo_set == 1)
            strncpy(buf, params->ov_stereo,
                    FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH + 1);
    }
    return buf;
}

static char *get_ov_hres(char *buf, void *usrptr, int ident)
{

    strncpy(buf, "Any", FIELD_OVERRIDE_LIST_OV_HRES_LENGTH + 1);

    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->ov_hres_set == 1)
            strncpy(buf, params->ov_hres,
                    FIELD_OVERRIDE_LIST_OV_HRES_LENGTH + 1);
    }
    return buf;
}

static char *get_ov_vres(char *buf, void *usrptr, int ident)
{
    strncpy(buf, "Any", FIELD_OVERRIDE_LIST_OV_VRES_LENGTH + 1);

    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->ov_vres_set == 1)
            strncpy(buf, params->ov_vres,
                    FIELD_OVERRIDE_LIST_OV_VRES_LENGTH + 1);
    }
    return buf;
}

static char *get_ov_refresh(char *buf, void *usrptr, int ident)
{
    strncpy(buf, "Any", FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH + 1);

    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->ov_refresh_set == 1)
            strncpy(buf, params->ov_refresh,
                    FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH + 1);
    }
    return buf;
}

static char *get_ov_pcr(char *buf, void *usrptr, int ident)
{
    strncpy(buf, "Any", FIELD_OVERRIDE_LIST_OV_PCR_LENGTH + 1);

    if (usrptr != NULL) {
        struct override_list_params *params =
            (struct override_list_params *) usrptr;
        if (params->ov_pcr_set == 1)
            strncpy(buf, params->ov_pcr,
                    FIELD_OVERRIDE_LIST_OV_PCR_LENGTH + 1);
    }
    return buf;
}

static void setup_parameter_values(struct override_list_params *params)
{

    params->alutreg_value = atoi(params->alutreg);
    if (strcasecmp(params->ov_alutreg, "Any") == 0)
        params->ov_alutreg_value = OVERRIDE_LIST_OV_ALUTREG_ANY;
    else
        params->ov_alutreg_value = atoi(params->ov_alutreg);
    do {
        if (strcasecmp(params->ov_stereo, "No") == 0) {
            params->ov_stereo_value = OVERRIDE_LIST_OV_STEREO_NO;
            break;
        }
        if (strcasecmp(params->ov_stereo, "Yes") == 0) {
            params->ov_stereo_value = OVERRIDE_LIST_OV_STEREO_YES;
            break;
        }
        if (strcasecmp(params->ov_stereo, "Any") == 0) {
            params->ov_stereo_value = OVERRIDE_LIST_OV_STEREO_ANY;
            break;
        }
    } while (0);
    if (strcasecmp(params->ov_hres, "Any") == 0)
        params->ov_hres_value = OVERRIDE_LIST_OV_HRES_ANY;
    else
        params->ov_hres_value = atoi(params->ov_hres);
    if (strcasecmp(params->ov_vres, "Any") == 0)
        params->ov_vres_value = OVERRIDE_LIST_OV_VRES_ANY;
    else
        params->ov_vres_value = atoi(params->ov_vres);
    if (strcasecmp(params->ov_refresh, "Any") == 0)
        params->ov_refresh_value = OVERRIDE_LIST_OV_REFRESH_ANY;
    else 
        params->ov_refresh_value = strtod(params->ov_refresh, NULL);
    if (strcasecmp(params->ov_pcr, "Any") == 0)
        params->ov_pcr_value = OVERRIDE_LIST_OV_PCR_ANY;
    else 
        params->ov_pcr_value = strtod(params->ov_pcr, NULL);
}

#ifndef LIBONLY
static struct override_list_params *get_override_list_entry_by_index(int
                                                                     index)
{
    struct override_list_params *params_iter;
    int count = 0;

    params_iter = override_list_table_head;
    while (params_iter != NULL) {
        if (count++ == index) {
            return params_iter;
        }
        params_iter = params_iter->next;
    }
    return NULL;
}

static char *override_list_selector_cb(int item)
{
    struct override_list_params *paramsp;

    paramsp = get_override_list_entry_by_index(item);

    sprintf(tempbuf, "????");
    
    if (paramsp != NULL) {
        char lutname[17];
        char hresbuf[6], vresbuf[6], lutidbuf[6];
        struct lut_list_params lutparams;

        if (!get_lut_list_params_by_lutid(&lutparams, paramsp->alutreg_value)) {
            return tempbuf;
        }

        setup_parameter_values(paramsp);

        memset(lutname, '\0', sizeof(lutname));
        strncpy(lutname, lutparams.name_value, sizeof(lutname) - 1);
        if (paramsp->ov_hres_value == OVERRIDE_LIST_OV_HRES_ANY)
           sprintf(hresbuf, "%-5s", "Any");
        else
           sprintf(hresbuf, "%-5d", paramsp->ov_hres_value);
        if (paramsp->ov_vres_value == OVERRIDE_LIST_OV_VRES_ANY)
           sprintf(vresbuf, "%-5s", "Any");
        else
           sprintf(vresbuf, "%-5d", paramsp->ov_vres_value);
        if (paramsp->ov_alutreg_value == OVERRIDE_LIST_OV_ALUTREG_ANY)
           sprintf(lutidbuf, "%-5s", "Any");
        else
           sprintf(lutidbuf, "%-5d", paramsp->ov_alutreg_value);

        sprintf(tempbuf, " %s  %s  %s  %-17s  %-5d %-5d",
            hresbuf,
            vresbuf,
            lutidbuf,
            lutname,
            lutparams.orig_hres_value,
            lutparams.orig_vres_value);
    }

    return tempbuf;
}
#endif                          /* LIBONLY */

static int count_override_list(void)
{
    struct override_list_params *params_iter;
    int count = 0;

    params_iter = override_list_table_head;
    while (params_iter != NULL) {
        count++;
        params_iter = params_iter->next;
    }
    return count;
}

static void free_override_list(void)
{
    struct override_list_params *params_iter, *tmp;
    params_iter = override_list_table_head;
    while (params_iter != NULL) {
        tmp = params_iter;
        params_iter = params_iter->next;
        free(tmp);
    }
    override_list_table_head = NULL;

}
static void print_override_list(struct override_list_params *params,
                                int count, FILE * fp)
{

    int len;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", "OVERRIDE_LIST_ID", count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", params->override_list_id);
    len = fprintf(fp, "%s.%d", "ALUTREG", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->alutreg);
    len = fprintf(fp, "%s.%d", "OV_ALUTREG", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->ov_alutreg);
    len = fprintf(fp, "%s.%d", "OV_STEREO", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->ov_stereo);
    len = fprintf(fp, "%s.%d", "OV_HRES", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->ov_hres);
    len = fprintf(fp, "%s.%d", "OV_VRES", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->ov_vres);
    len = fprintf(fp, "%s.%d", "OV_REFRESH", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->ov_refresh);
    len = fprintf(fp, "%s.%d", "OV_PCR", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->ov_pcr);
}

static int save_override_list(void)
{

    struct override_list_params *params_iter;
    int count = 0;
    FILE *fp;

    if ((fp = fopen(get_config_file_name(), "w+")) == NULL) {
        return -1;
    }

    params_iter = override_list_table_head;
    while (params_iter != NULL) {
        count++;
        print_override_list(params_iter, count, fp);
        params_iter = params_iter->next;
    }

    fclose(fp);

    return count;
}

static int get_unique_override_list_id(void)
{
    struct override_list_params *paramsp;
    int unique_id = 1;

  again:
    paramsp = override_list_table_head;
    while (paramsp != NULL) {
        if (paramsp->override_list_id == unique_id) {
            unique_id++;
            goto again;
        }
        paramsp = paramsp->next;
    }
    return unique_id;
}

#ifndef LIBONLY
static void delete_from_list(int list_id)
{

    struct override_list_params *params_iter, *tmp;
    params_iter = override_list_table_head;
    if (params_iter == NULL)
        return;

    if (params_iter->override_list_id == list_id) {
        tmp = params_iter;
        override_list_table_head = params_iter->next;
        free(tmp);
    }
    else {
        while (params_iter->next != NULL) {
            if (params_iter->next->override_list_id == list_id) {
                tmp = params_iter->next;
                params_iter->next = params_iter->next->next;
                free(tmp);
                break;
            }
            params_iter = params_iter->next;

        }
    }
}
#endif                          /* LIBONLY */

static void add_to_list(struct override_list_params *params)
{

    struct override_list_params *params_iter;
    params->next = NULL;
    params_iter = override_list_table_head;
    if (params_iter == NULL) {
        override_list_table_head = params;
    }
    else {
        while (params_iter->next != NULL) {
            params_iter = params_iter->next;
        }
        params_iter->next = params;
    }
}

static int override_list_config_handler(char *name, char *value, void *ptr,
                                        int idx)
{
    static struct override_list_params params;
    struct override_list_params *paramsp;
    static int lastidx = 0;


    if (idx != lastidx) {
        if (lastidx == 0 && (idx > 1 || idx == 0))
            return -1;
        if (lastidx == 0 && idx == 1) {
            // prepare to populate new entry
            memset(&params, 0x00, sizeof(struct override_list_params));
            lastidx = 1;
        }
        else {
            /* Add the struct built up so far to the linked list */
            paramsp = malloc(sizeof(struct override_list_params));
            if (paramsp == NULL)
                return -1;
            memcpy(paramsp, &params, sizeof(struct override_list_params));
            if (ptr != NULL)
                paramsp->userptr = ptr;

            // for any values not found, fill in defaults
            if (paramsp->alutreg_set == 0) {
                get_alutreg(paramsp->alutreg, NULL, 0);
                paramsp->alutreg_set = 1;
            }
            if (paramsp->ov_alutreg_set == 0) {
                get_ov_alutreg(paramsp->ov_alutreg, NULL, 0);
                paramsp->ov_alutreg_set = 1;
            }
            if (paramsp->ov_stereo_set == 0) {
                get_ov_stereo(paramsp->ov_stereo, NULL, 0);
                paramsp->ov_stereo_set = 1;
            }
            if (paramsp->ov_hres_set == 0) {
                get_ov_hres(paramsp->ov_hres, NULL, 0);
                paramsp->ov_hres_set = 1;
            }
            if (paramsp->ov_vres_set == 0) {
                get_ov_vres(paramsp->ov_vres, NULL, 0);
                paramsp->ov_vres_set = 1;
            }
            if (paramsp->ov_refresh_set == 0) {
                get_ov_refresh(paramsp->ov_refresh, NULL, 0);
                paramsp->ov_refresh_set = 1;
            }
            if (paramsp->ov_pcr_set == 0) {
                get_ov_pcr(paramsp->ov_pcr, NULL, 0);
                paramsp->ov_pcr_set = 1;
            }
            add_to_list(paramsp);

            // prepare to populate new entry
            memset(&params, 0x00, sizeof(struct override_list_params));
            lastidx = idx;
        }
    }
    if (strcasecmp(name, "OVERRIDE_LIST_ID") == 0) {
        params.override_list_id = atoi(value);
    }
    else if (strcasecmp(name, "ALUTREG") == 0) {
        params.alutreg_set = 1;
        strncpy(params.alutreg, value,
                FIELD_OVERRIDE_LIST_ALUTREG_LENGTH + 1);
    }
    else if (strcasecmp(name, "OV_ALUTREG") == 0) {
        params.ov_alutreg_set = 1;
        strncpy(params.ov_alutreg, value,
                FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH + 1);
    }
    else if (strcasecmp(name, "OV_STEREO") == 0) {
        params.ov_stereo_set = 1;
        strncpy(params.ov_stereo, value,
                FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH + 1);
    }
    else if (strcasecmp(name, "OV_HRES") == 0) {
        params.ov_hres_set = 1;
        strncpy(params.ov_hres, value,
                FIELD_OVERRIDE_LIST_OV_HRES_LENGTH + 1);
    }
    else if (strcasecmp(name, "OV_VRES") == 0) {
        params.ov_vres_set = 1;
        strncpy(params.ov_vres, value,
                FIELD_OVERRIDE_LIST_OV_VRES_LENGTH + 1);
    }
    else if (strcasecmp(name, "OV_REFRESH") == 0) {
        params.ov_refresh_set = 1;
        strncpy(params.ov_refresh, value,
                FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH + 1);
    }
    else if (strcasecmp(name, "OV_PCR") == 0) {
        params.ov_pcr_set = 1;
        strncpy(params.ov_pcr, value,
                FIELD_OVERRIDE_LIST_OV_PCR_LENGTH + 1);
    }

    return 0;
}

static int parse_override_list(void *ptr)
{
    struct override_list_params *params;
    int count = 0;
    int need_to_save = 0;

    free_override_list();

    if (parse_list_config_file(get_config_file_name(),
                               override_list_config_handler, ptr) < 0) {
        VLOG_WARNING("Could not load  configuration file %s",
                      get_config_file_name());
    }
    else {
        // Setup parameter values and fix id values if necessary
        foreach_override_list_entry(params) {
            setup_parameter_values(params);
            if (params->override_list_id <= 0) {
                params->override_list_id = get_unique_override_list_id();
                need_to_save = 1;
            }
        }
    }
    if (need_to_save)
        save_override_list();
    count = count_override_list();
    return count;
}

static OverrideListSavedFunction *savedHook = NULL;
extern void registerOverrideListSavedFunction(OverrideListSavedFunction *
                                              funcPtr)
{
    if (funcPtr != NULL)
        savedHook = funcPtr;
}

int retrieveOverrideListValues(HAL * hp)
{
    // Just parse the current config file and foreach_override_list_entry() 
    // will provide the rest

    if (hp != NULL)
        board_number = hal_get_board_number(hp);
    return parse_override_list(hp);
}

#ifndef LIBONLY

static FORMITEMDEF itemdefs[] = {
    {FIELD_DUMMY, "Override Criteria", "",
     1, NULL, NULL,
     0, 0, 0, O_NULLOK | O_ACTIVE, 0},
    {FIELD_OV_ALUTREG, "LUT ID",
     "LUT ID to Override With",
     FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH, get_ov_alutreg, NULL,
     FTYPE_REGEXP, (int) regexp_anynonempty, 0, O_NULLOK, 0},
    {FIELD_OV_STEREO, "Stereo?",
     "Stereo Setting to be Overridden",
     FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH, get_ov_stereo, NULL,
     FTYPE_ENUM_RANGE, (int) ov_stereo_values, 0, O_NULLOK, FFLAG_FOLLOW},
    {FIELD_OV_HRES, "Horiz. Resolution",
     "Horizontal Resolution to be Overridden",
     FIELD_OVERRIDE_LIST_OV_HRES_LENGTH, get_ov_hres, NULL,
     FTYPE_REGEXP, (int) regexp_anynonempty, 0, O_NULLOK, 0},
    {FIELD_OV_VRES, "Vert. Resolution",
     "Vertical Resolution to be Overridden",
     FIELD_OVERRIDE_LIST_OV_VRES_LENGTH, get_ov_vres, NULL,
     FTYPE_REGEXP, (int) regexp_anynonempty, 0, O_NULLOK, FFLAG_FOLLOW},
    {FIELD_OV_REFRESH, "Refresh Rate",
     "Refresh Rate to be Overridden",
     FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH, get_ov_refresh, NULL,
     FTYPE_REGEXP, (int) regexp_anynonempty, 0, O_NULLOK, 0},
    {FIELD_OV_PCR, "Pixel Clock Rate",
     "Pixel Clock Rate to be Overridden",
     FIELD_OVERRIDE_LIST_OV_PCR_LENGTH, get_ov_pcr, NULL,
     FTYPE_REGEXP, (int) regexp_anynonempty, 0, O_NULLOK, FFLAG_FOLLOW},
    {FIELD_DUMMY, "", "",
     1, NULL, NULL,
     0, 0, 0, O_NULLOK | O_ACTIVE, 0},
    {FIELD_DUMMY, "Overriding LUT", "",
     1, NULL, NULL,
     0, 0, 0, O_NULLOK | O_ACTIVE, 0},
    {FIELD_NAME, "Name",
     "Name of Overriding LUT",
     FIELD_OVERRIDE_LIST_NAME_LENGTH, get_lutname, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_ALUTREG, "LUTID",
     "LUTID of Overriding LUT",
     FIELD_OVERRIDE_LIST_ALUTREG_LENGTH, get_alutreg, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_STEREO, "Stereo?",
     "Stereo Setting of Overriding LUT",
     FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH, get_stereo, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HRES, "Horiz. Resolution",
     "Horizontal Resolution of Overriding LUT",
     FIELD_OVERRIDE_LIST_OV_HRES_LENGTH, get_hres, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VRES, "Vert. Resolution",
     "Vertical Resolution of Overriding LUT",
     FIELD_OVERRIDE_LIST_OV_VRES_LENGTH, get_vres, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_REFRESH, "Refresh Rate",
     "Refresh Rate of Overriding LUT",
     FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH, get_refresh, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_PCR, "Pixel Clock Rate",
     "Pixel Clock Rate of Overriding LUT",
     FIELD_OVERRIDE_LIST_OV_PCR_LENGTH, get_pcr, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HFP, "Horiz. Front Porch",
     "Horizontal Front Porch of Overriding LUT",
     FIELD_OVERRIDE_LIST_HFP_LENGTH, get_hfp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VFP, "Vert. Front Porch",
     "Vertical Front Porch of Overriding LUT",
     FIELD_OVERRIDE_LIST_VFP_LENGTH, get_vfp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HSYNC, "Horiz. Sync",
     "Horizontal Synch of Overriding LUT",
     FIELD_OVERRIDE_LIST_HSYNC_LENGTH, get_hsync, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VSYNC, "Vert. Sync",
     "Vertical Synch of Overriding LUT",
     FIELD_OVERRIDE_LIST_VSYNC_LENGTH, get_vsync, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HBP, "Horiz. Back Porch",
     "Horizontal Back Porch of Overriding LUT",
     FIELD_OVERRIDE_LIST_HBP_LENGTH, get_hbp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VBP, "Vert. Back Porch",
     "Vertical Back Porch of Overriding LUT",
     FIELD_OVERRIDE_LIST_VBP_LENGTH, get_vbp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};


int configureOverrideList(MENUINFO * minfo)
{

    MENU *selector;
    DIALOG *dialog;
    int rc;
    int idx;
    int number_of_items;
    char keylist[12];
    char titlebuf[120];
    int retval = RC_NORMAL;
    int override_list_id = 0;
    struct override_list_params *orig_param_ptr;
    char error_string[128];
    HAL *hp = NULL;

    {
        hp = minfo->minfo_userptr;
        if (hp != NULL) {
            board_number = hal_get_board_number(hp);
            hardware_type = hal_get_hardware_type(hp);
        }
    }

    /*******************************************************
     * Retreive and get count of lut list params only once *
     *******************************************************/
    lutlistcount = retrieveLUTListValues(hp);

    number_of_items = parse_override_list(hp);

    if (number_of_items == 0) {
        dialog = show_dialog(minfo, 8, 10, 
                "</K/U>Empty List of Override LUTs Table!!<!K!U>");

        show_info(minfo, "Add new List of Override LUTs?" YESNO_STR);
        if (get_yesno(minfo)) {
            remove_dialog(dialog);
            rc = update_override_list_dialog(minfo, 0, ADD_REQUEST);
            if (rc == RC_EXIT) {
                free_override_list();
                return RC_EXIT;
            }
            if (rc != RC_NORMAL) {
                free_override_list();
                return retval;
            }
            number_of_items = count_override_list();
            if (number_of_items == 0) {
                free_override_list();
                return retval;
            }
            show_title(minfo, titlebuf);
        }
        else {
            remove_dialog(dialog);
            free_override_list();
            return retval;
        }
    }

    /********************
     * Print Menu title *
     ********************/
    sprintf(titlebuf, "%s\n  \n</U>%s %s %s %-15s    %s  %s",
            minfo->minfo_title,
            "   OvHres", "OvVres", "OvLutId", "     LUT Name", "Hres", "Vres");

    show_title(minfo, titlebuf);

    selector = create_selector(number_of_items, override_list_selector_cb);

    set_menu_windows(minfo, selector);

    post_menu(selector);

    show_info(minfo, override_list_help);

    while (1) {
        strcpy(keylist, "dDaAmM");
        rc = process_selector(minfo, selector, keylist);

        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }
        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            break;
        }

        switch (keylist[0]) {

        case 'd':
        case 'D':

            idx = rc + 1;
            sprintf(tempbuf,
                    "Delete Override LUT at Index %d ?" YESNO_STR,
                    idx);
            show_info(minfo, tempbuf);
            if (get_yesno(minfo)) {

                orig_param_ptr = get_override_list_entry_by_index(rc);

                if (orig_param_ptr == NULL) {
                    return RC_ERROR;
                }
                orig_param_ptr->userptr = minfo->minfo_userptr;
                setup_parameter_values(orig_param_ptr);

                /* Call custom validator */
                sprintf(error_string, "Invalid Data");
                rc = validateOverrideList(DEL_REQUEST, NULL,
                                          orig_param_ptr, error_string);

                if (rc == RC_NORMAL) {

                    rc = applyOverrideList(DEL_REQUEST, NULL,
                                           orig_param_ptr);

                    if (rc == RC_NORMAL) {

                        remote_config_start();
                        remote_config_set_config_name(override_list_title);
                        remote_config_set_command_name
                            ("del_override_list");
                        remote_config_set_operation
                            (REMOTE_CONFIG_OPERATION_DELETE);
                        remote_config_set_source
                            (REMOTE_CONFIG_SOURCE_MENU);

                        delete_override_list(idx - 1, &override_list_id);

                        remote_config_set_index(override_list_id);
                        remote_config_send();

                        number_of_items = count_override_list();
                        save_override_list();

                        if (savedHook != NULL)
                            (*savedHook) (DEL_REQUEST,
                                          minfo->minfo_userptr);

                        if (number_of_items == 0) {
                            show_info(minfo,
                                      "Deleted last Override LUT. "
                                      PRESSANYKEY_STR);
                            get_anykey(minfo);
                            goto QUIT_LABEL;
                        }
                    }
                    else {
                        show_info(minfo, "Failed: Sytem Error.\n"
                                  PRESSANYKEY_STR);
                        get_anykey(minfo);
                    }
                }
                else {
                    char info_string[256];
                    sprintf(info_string, "Failed: %s\n%s", error_string,
                            PRESSANYKEY_STR);
                    show_info(minfo, info_string);
                    get_anykey(minfo);
                }

                unpost_menu(selector);
                refresh_selector(selector, number_of_items,
                                 override_list_selector_cb);
                post_menu(selector);
            }

            show_info(minfo, override_list_help);
            break;

        case 'a':
        case 'A':
            rc = -1;
        case 'm':
        case 'M':
            unpost_menu(selector);
            idx = rc + 1;
            rc = update_override_list_dialog(minfo, 
                    idx, 
                    (keylist[0] == 'a' || keylist[0] == 'A') ? ADD_REQUEST :
                    MOD_REQUEST);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            if (rc == RC_NORMAL) {
                number_of_items = count_override_list();
                refresh_selector(selector, number_of_items,
                                 override_list_selector_cb);
                if (idx > 0)
                    set_selector_item(selector, idx - 1);
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, override_list_help);
            break;


        case '\0':
            unpost_menu(selector);
            idx = rc + 1;
            rc = update_override_list_dialog(minfo, idx, DISPLAY_REQUEST);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, override_list_help);
            break;

        }
    }

  QUIT_LABEL:

    free_override_list();
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    return retval;

}

static char *lut_list_selector_cb(int item)
{
    struct lut_list_params params;

    sprintf(tempbuf, "????");

    if (get_lut_list_params_by_index(&params, item) > 0) {
        constructRowLUTList(tempbuf, &params);
    }

    return tempbuf;
}

static int add_override_list_dialog(MENUINFO * minfo, 
                                    struct lut_list_params *paramsp) {
    MENU *selector;
    DIALOG *dialog;
    char titlebuf[120];
    char keylist[12];
    HAL *hp;
    int rc, retval = RC_NORMAL;

    hp = minfo->minfo_userptr;
    if (hp != NULL) {
        board_number = hal_get_board_number(hp);
    }

    /* Initialize returned lut list params */
    memset(paramsp, '\0', sizeof(struct lut_list_params));

    sprintf(titlebuf, "%s\n  \n   </U>", minfo->minfo_title);
    constructHeaderLUTList(titlebuf + strlen(titlebuf));
    strcat(titlebuf, "<!U>");
    show_title(minfo, titlebuf);

    if (lutlistcount == 0) {
        dialog = show_dialog(minfo, 8, 15,
                             "</K/U>Empty List of Video LUTs Table!!<!K!U>");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return RC_ERROR;
    }

    selector = create_selector(lutlistcount, lut_list_selector_cb);
    set_menu_windows(minfo, selector);
    post_menu(selector);
    show_info(minfo, add_override_list_dialog_help);

    while (1) {
        keylist[0] = '\0';
        rc = process_selector(minfo, selector, keylist);
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            goto QUIT_LABEL;
        }
        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            goto QUIT_LABEL;
        }
        if (keylist[0] == '\0') {
            if (get_lut_list_params_by_index(paramsp, rc) == 1) {
                break;
            }
            else {
                VLOG_INFO("Failed to get LUT list params for Index %d", rc);
            }
        }
    }

QUIT_LABEL:
    unpost_menu(selector);
    unset_menu_windows(selector);
    delete_selector(selector);
    return retval;
}

static int update_override_list_dialog(MENUINFO * minfo, int idx, int req_code)
{
    FORM *form = NULL;
    int rc = E_OK;
    int retval = RC_NORMAL;
    char error_string[128];
    struct override_list_params params;
    struct override_list_params *orig_param_ptr = NULL;
    struct lut_list_params lutparams;
    char titlebuf[80];
    int number_of_items;

    {
        HAL *hp;
        hp = minfo->minfo_userptr;
        if (hp != NULL) {
            board_number = hal_get_board_number(hp);
        }
    }

    number_of_items = count_override_list();

  redraw:
    if (req_code == DISPLAY_REQUEST) {
        sprintf(titlebuf, "Display Entry at Index %d", idx);
        show_title(minfo, titlebuf);
    }
    else if (req_code == ADD_REQUEST) {
        show_title(minfo, "Add New Entry");
        rc = add_override_list_dialog(minfo, &lutparams);
        if (rc == RC_NORMAL) {
            if (lutparams.alutreg_value == 0) {
                VLOG_WARNING("Invalid LUT ID 0 for Entry at Index %d.", 
                        lutparams.lut_list_id);
                return RC_EXIT;
            }
            /* Copy lut list params to global variable */
            memcpy(&lutlistparams, &lutparams, sizeof(struct lut_list_params));
        }
        else {
            return rc;
        }
    }
    else {
        show_title(minfo, "Modify Entry");
    }

    if (idx > 0) {
        orig_param_ptr = get_override_list_entry_by_index(idx - 1);
    }

    show_title(minfo, NULL);

    form = create_form(itemdefs, 0, 0, orig_param_ptr);

    set_form_windows(minfo, form);

    post_form(form);

    if (req_code == DISPLAY_REQUEST) {
        wrefresh(minfo->minfo_menuscreen->window);
        show_info(minfo,
                  "Press </U>Space<!U> or </U>N<!U> for next entry, </U>P<!U> for previous entry\n Or </U>Q<!U> to Quit.");
        rc = get_anykey(minfo);

        unpost_form(form);
        unset_form_windows(form);
        delete_form(form);

        if (rc == '') {
            return RC_EXIT;
        }
        else if (rc == 'Q' || rc == 'q') {
            return RC_ERROR;    /* Nothing wrong really */
        }
        else if (rc == ' ' || rc == 'N' || rc == 'n' || rc == KEY_DOWN) {
            if (idx < number_of_items)
                idx = idx + 1;
        }
        else if (rc == 'P' || rc == 'p' || rc == KEY_UP) {
            if (idx > 1)
                idx = idx - 1;
        }

        goto redraw;
    }

    while (1) {
        rc = process_form(minfo, form, formmesg, 0);
        if (rc == RC_EXIT) {
            retval = RC_EXIT;
            break;
        }
        if (rc == RC_QUIT) {
            retval = RC_NORMAL;
            break;
        }
        else {
            if (req_code == ADD_REQUEST || get_form_status(form) == TRUE) {
                if (req_code == ADD_REQUEST) { 
                    show_info(minfo, "Add this Entry?" YESNO_STR);
                }
                else {
                    show_info(minfo, "Update this Entry?" YESNO_STR);
                    memset(&params, 0x00, sizeof(params));
                }
                if (get_yesno(minfo)) {
                    rc = RC_NORMAL;

                    get_field_buffer(form, FIELD_ALUTREG, params.alutreg);
                    params.alutreg_set = 1;
                    get_field_buffer(form, FIELD_OV_ALUTREG,
                                     params.ov_alutreg);
                    params.ov_alutreg_set = 1;
                    get_field_buffer(form, FIELD_OV_STEREO,
                                     params.ov_stereo);
                    params.ov_stereo_set = 1;
                    get_field_buffer(form, FIELD_OV_HRES, params.ov_hres);
                    params.ov_hres_set = 1;
                    get_field_buffer(form, FIELD_OV_VRES, params.ov_vres);
                    params.ov_vres_set = 1;
                    get_field_buffer(form, FIELD_OV_REFRESH,
                                     params.ov_refresh);
                    params.ov_refresh_set = 1;
                    get_field_buffer(form, FIELD_OV_PCR, params.ov_pcr);
                    params.ov_pcr_set = 1;

                    setup_parameter_values(&params);
                    params.userptr = minfo->minfo_userptr;
                    // orig_params only available in case of modify
                    if (orig_param_ptr != NULL) {
                        setup_parameter_values(orig_param_ptr);
                        params.override_list_id =
                            orig_param_ptr->override_list_id;
                        orig_param_ptr->userptr = minfo->minfo_userptr;
                    }
                    else {
                        params.override_list_id =
                            get_unique_override_list_id();
                    }

                    /* Call custom validator */
                    sprintf(error_string, "Invalid Data");
                    rc = validateOverrideList(idx ==
                                              0 ? ADD_REQUEST :
                                              MOD_REQUEST, &params,
                                              orig_param_ptr,
                                              error_string);

                    if (rc == RC_NORMAL) {
                        remote_config_start();
                        remote_config_set_config_name(override_list_title);
                        if (idx == 0) {
                            remote_config_set_command_name
                                ("add_override_list");
                            remote_config_set_operation
                                (REMOTE_CONFIG_OPERATION_ADD);
                        }
                        else {
                            remote_config_set_command_name
                                ("mod_override_list");
                            remote_config_set_operation
                                (REMOTE_CONFIG_OPERATION_MODIFY);
                        }
                        remote_config_set_source
                            (REMOTE_CONFIG_SOURCE_MENU);
                        remote_config_add_parameter(override_list_options
                                                    [0].name,
                                                    params.alutreg);
                        remote_config_add_parameter(override_list_options
                                                    [1].name,
                                                    params.ov_alutreg);
                        remote_config_add_parameter(override_list_options
                                                    [2].name,
                                                    params.ov_stereo);
                        remote_config_add_parameter(override_list_options
                                                    [3].name,
                                                    params.ov_hres);
                        remote_config_add_parameter(override_list_options
                                                    [4].name,
                                                    params.ov_vres);
                        remote_config_add_parameter(override_list_options
                                                    [5].name,
                                                    params.ov_refresh);
                        remote_config_add_parameter(override_list_options
                                                    [6].name,
                                                    params.ov_pcr);

                        if (rc == RC_NORMAL) {
                            rc = applyOverrideList(idx ==
                                                   0 ? ADD_REQUEST :
                                                   MOD_REQUEST, &params,
                                                   orig_param_ptr);
                        }

                        if (rc == RC_NORMAL) {
                            rc = update_override_list_entry(idx, &params);
                        }
                        remote_config_set_index(params.override_list_id);
                        if (rc == RC_NORMAL) {
                            save_override_list();
                            if (savedHook != NULL)
                                (*savedHook) (idx ==
                                              0 ? ADD_REQUEST :
                                              MOD_REQUEST,
                                              minfo->minfo_userptr);

                            remote_config_send();
                            if (idx == 0) {
                                show_info(minfo, "Entry added.\n"
                                          PRESSANYKEY_STR);
                            }
                            else {
                                show_info(minfo, "Entry updated.\n"
                                          PRESSANYKEY_STR);
                            }
                        }
                        else {
                            show_info(minfo, "Failed: Sytem Error.\n"
                                      PRESSANYKEY_STR);
                            retval = RC_ERROR;
                        }
                    }
                    else {
                        char info_string[256];
                        sprintf(info_string, "Failed: %s\n%s",
                                error_string, PRESSANYKEY_STR);
                        show_info(minfo, info_string);
                        retval = RC_ERROR;
                    }

                    get_anykey(minfo);
                    break;
                }
                else {
                    show_info(minfo, "Continue Editing?" YESNO_STR);
                    if (get_yesno(minfo)) {
                        /* continue */
                    }
                    else {
                        retval = RC_ERROR;
                        break;
                    }
                }
            }
            else {
                show_info(minfo, "No change in data. Quit?" YESNO_STR);
                if (get_yesno(minfo)) {
                    retval = RC_ERROR;
                    break;
                }
            }
        }
    }
    /* cleanup */
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    return retval;

}


static int update_override_list_entry(int idx,
                                      struct override_list_params *params)
{

    struct override_list_params *paramsp;

    if (idx == 0) {             // add request
        paramsp = malloc(sizeof(struct override_list_params));
        if (paramsp != NULL) {
            memcpy(paramsp, params, sizeof(struct override_list_params));
            add_to_list(paramsp);
        }
        else {
            return RC_ERROR;
        }
    }
    else {                      // modify request
        /* Copy values into existing struct in list, retaining needed vaules */
        paramsp = get_override_list_entry_by_index(idx - 1);
        params->override_list_id = paramsp->override_list_id;
        params->userptr = paramsp->userptr;
        params->next = paramsp->next;
        memcpy(paramsp, params, sizeof(struct override_list_params));
    }

    return RC_NORMAL;
}


static int delete_override_list(int idx, int *override_list_idp)
{
    int number_of_items;
    struct override_list_params *paramsp;

    number_of_items = count_override_list();

    if (idx >= number_of_items)
        return RC_ERROR;

    paramsp = get_override_list_entry_by_index(idx);
    if (paramsp == NULL)
        return RC_ERROR;

    *override_list_idp = paramsp->override_list_id;

    delete_from_list(paramsp->override_list_id);

    return RC_NORMAL;
}
int describe_override_list_main(int argc, char **argv)
{
    int rc = RC_NORMAL;

    printf ("<MenuDescription>\n");
    printf ("\t<Type>configlist</Type>\n");
    printf ("\t<Title>OverrideList</Title>\n");
    printf ("\t<Description>List of Override Descriptors</Description>\n");
    printf ("\t<BoardAccess>true</BoardAccess>\n");
    printf ("\t<CliCommandName>override_list</CliCommandName>\n");
    printf ("\t<ConfigFile>/V2O/config/overridelistconf</ConfigFile>\n");
    printf ("\t<MenuItems>\n");
    printf ("\t\t<Attribute name=\"alutreg\" label=\"LUT Id\" type=\"string\" size=\"4\" defaultvalue=\"1\"  validation=\"enumeration\" values=\"lut_list_alutreg ,\"/>\n");
    printf("\t\t<Attribute name=\"alutreg\" label=\"LUT ID\" type=\"integer\" size=\"4\" defaultvalue=\"1\"  validation=\"range\" min=\"1\" max=\"169\" />\n");
    printf("\t\t<Attribute name=\"ov_hres\" label=\"Horiz. Resolution\" type=\"integer\" size=\"5\" defaultvalue=\"1280\"  validation=\"range\" min=\"256\" max=\"8192\" />\n");
    printf("\t\t<Attribute name=\"ov_vres\" label=\"Vert. Resolution\" type=\"integer\" size=\"5\" defaultvalue=\"1024\"  validation=\"range\" min=\"256\" max=\"4096\" />\n");
    printf("\t\t<Attribute name=\"ov_refresh\" label=\"Refresh Rate\" type=\"float\" size=\"13\" defaultvalue=\"60.0\"  validation=\"range\" min=\"1.0\" max=\"500.0\" />\n");
    printf("\t\t<Attribute name=\"ov_pcr\" label=\"Pixel Clock Rate\" type=\"float\" size=\"12\" defaultvalue=\"100.0\"  validation=\"range\" min=\"20.0\" max=\"330.0\" />\n");

    printf ("\t</MenuItems>\n");
    printf ("</MenuDescription>\n");
    return rc;
}


int get_override_list_main(int argc, char **argv)
{

    int opt_idx = 1;
    int rc = RC_NORMAL;
    int ctr = 0;
    HAL *hp = NULL;
    struct override_list_params *params;
    int c, opt_index = 0;

    while (1) {
        c = getopt_long_only(argc, argv, "h", override_list_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 8:
                board_number = atoi(optarg);
                break;
            }
        }
        else {

            rc = RC_ERROR;
            break;

        }
    }
    opt_idx = optind;

    hp = hal_acquire(board_number);
    if (hp == NULL) {
        print_cli_failure(get_override_list, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }
    if (opt_idx == argc) {
        parse_override_list(hp);

        printf(XML_GROUP, get_override_list);
        print_cli_success(NULL);
        printf("  " XML_TABLE, override_list_title);

        foreach_override_list_entry(params) {
            ctr = 0;
            printf("    " XML_OBJINDEX, params->override_list_id,
                   "List of Override LUTs Entry");

            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->alutreg);
            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->ov_alutreg);
            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->ov_stereo);
            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->ov_hres);
            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->ov_vres);
            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->ov_refresh);
            printf("      " XML_ATTRSTR, override_list_options[ctr++].name,
                   params->ov_pcr);

            printf("  " XML_OBJEND);
        }
        printf("  " XML_TABEND);
        printf(XML_GROUPEND);
        free_override_list();
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_override_list, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_override_list -boardNumber Number\n");
        printf("Get List of Override LUTs\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}


static int update_override_list_cli(int argc, char **argv, int req_code)
{

    int c, opt_index = 0;
    int rc = RC_NORMAL;
    //int number_of_items;
    int value;
    char error_string[128];
    struct override_list_params params, *paramsp;
    struct override_list_params *orig_param_ptr = NULL;
    int access_id;
    HAL *hp = NULL;
    char *des_string;
    int index = 0;

    value = 0;
    remote_config_start();
    remote_config_set_config_name(override_list_title);
    if (req_code == ADD_REQUEST) {
        remote_config_set_command_name("add_override_list");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        des_string = add_des_override_list;
    }
    else {
        remote_config_set_command_name("mod_override_list");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        des_string = mod_des_override_list;
    }
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    memset(&params, 0x00, sizeof(params));

    if (view_only() != 0) {
        print_cli_failure(des_string, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf
            ("You must be root to set List of Override LUTs parameters\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", override_list_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 0:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_ALUTREG_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if ((value = atoi(optarg)) <= 169 && value >= 1) {
                    params.alutreg_set = 1;
                    strncpy(params.alutreg, optarg,
                            FIELD_OVERRIDE_LIST_ALUTREG_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 1:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (check_regex(optarg, regexp_anynonempty) ==
                         RC_NORMAL) {
                    params.ov_alutreg_set = 1;
                    strncpy(params.ov_alutreg, optarg,
                            FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 2:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (check_enum(optarg, ov_stereo_values) == RC_NORMAL) {
                    params.ov_stereo_set = 1;
                    strncpy(params.ov_stereo, optarg,
                            FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 3:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_OV_HRES_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (check_regex(optarg, regexp_anynonempty) ==
                         RC_NORMAL) {
                    params.ov_hres_set = 1;
                    strncpy(params.ov_hres, optarg,
                            FIELD_OVERRIDE_LIST_OV_HRES_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 4:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_OV_VRES_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (check_regex(optarg, regexp_anynonempty) ==
                         RC_NORMAL) {
                    params.ov_vres_set = 1;
                    strncpy(params.ov_vres, optarg,
                            FIELD_OVERRIDE_LIST_OV_VRES_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 5:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (check_regex(optarg, regexp_anynonempty) ==
                         RC_NORMAL) {
                    params.ov_refresh_set = 1;
                    strncpy(params.ov_refresh, optarg,
                            FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 6:
                if (strlen(optarg) > FIELD_OVERRIDE_LIST_OV_PCR_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (check_regex(optarg, regexp_anynonempty) ==
                         RC_NORMAL) {
                    params.ov_pcr_set = 1;
                    strncpy(params.ov_pcr, optarg,
                            FIELD_OVERRIDE_LIST_OV_PCR_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 7:
                remote_config_set_client_data(optarg);
                break;
            case 8:
                board_number = atoi(optarg);
                break;
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }

    hp = hal_acquire(board_number);
    remote_config_set_board_number(board_number);
    if (hp == NULL) {
        print_cli_failure(des_string, CLI_REASON_WRONG_HW);
        remote_config_send_error(CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    params.userptr = hp;

    if ((rc == RC_NORMAL) &&
        ((req_code == ADD_REQUEST && optind == argc) ||
         (req_code == MOD_REQUEST && optind == (argc - 1)))) {

        if (req_code == MOD_REQUEST) {
            access_id = atoi(argv[optind]);
            remote_config_set_index(access_id);
        }

        //number_of_items = parse_override_list(hp);
        (void) parse_override_list(hp);

        // For modify, find the entry and the index it is at
        if (req_code == MOD_REQUEST) {
            index = 0;
            foreach_override_list_entry(paramsp) {
                index++;
                if (paramsp->override_list_id == access_id) {
                    orig_param_ptr = paramsp;
                    break;
                }
            }

            if (orig_param_ptr == NULL) {
                print_cli_failure(des_string, CLI_REASON_BAD_INDEX);
                remote_config_send_error(CLI_REASON_BAD_INDEX);
                return RC_ERROR;
            }
            setup_parameter_values(orig_param_ptr);
            params.override_list_id = orig_param_ptr->override_list_id;
        }
        else {
            params.override_list_id = get_unique_override_list_id();
        }

        // For params that did not come from command line, pick from defaults
        // (for add) or from orig_param_ptr (for modify)

        if (params.alutreg_set == 1) {
            remote_config_add_parameter(override_list_options[0].name,
                                        params.alutreg);
        }
        else {
            get_alutreg(params.alutreg, orig_param_ptr, 0);
        }
        if (params.ov_alutreg_set == 1) {
            remote_config_add_parameter(override_list_options[1].name,
                                        params.ov_alutreg);
        }
        else {
            get_ov_alutreg(params.ov_alutreg, orig_param_ptr, 0);
        }
        if (params.ov_stereo_set == 1) {
            remote_config_add_parameter(override_list_options[2].name,
                                        params.ov_stereo);
        }
        else {
            get_ov_stereo(params.ov_stereo, orig_param_ptr, 0);
        }
        if (params.ov_hres_set == 1) {
            remote_config_add_parameter(override_list_options[3].name,
                                        params.ov_hres);
        }
        else {
            get_ov_hres(params.ov_hres, orig_param_ptr, 0);
        }
        if (params.ov_vres_set == 1) {
            remote_config_add_parameter(override_list_options[4].name,
                                        params.ov_vres);
        }
        else {
            get_ov_vres(params.ov_vres, orig_param_ptr, 0);
        }
        if (params.ov_refresh_set == 1) {
            remote_config_add_parameter(override_list_options[5].name,
                                        params.ov_refresh);
        }
        else {
            get_ov_refresh(params.ov_refresh, orig_param_ptr, 0);
        }
        if (params.ov_pcr_set == 1) {
            remote_config_add_parameter(override_list_options[6].name,
                                        params.ov_pcr);
        }
        else {
            get_ov_pcr(params.ov_pcr, orig_param_ptr, 0);
        }

        setup_parameter_values(&params);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validateOverrideList(req_code, &params, orig_param_ptr,
                                  error_string);

        if (rc != RC_NORMAL) {
            print_cli_failure(des_string, error_string);
            remote_config_send_error(error_string);
            return rc;
        }

        if (rc == RC_NORMAL) {
            rc = applyOverrideList(req_code, &params, orig_param_ptr);

            if (rc == RC_NORMAL) {
                rc = update_override_list_entry(index, &params);
            }
            remote_config_set_index(params.override_list_id);

            if (rc == RC_NORMAL) {
                print_cli_success(des_string);
                remote_config_send();
                save_override_list();
                if (savedHook != NULL)
                    (*savedHook) (req_code, hp);
            }
        }
        if (rc != RC_NORMAL) {
            print_cli_failure(des_string, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        }
        free_override_list();
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(des_string, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(des_string, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        if (req_code == ADD_REQUEST)
            printf("\nUsage: add_override_list [OPTIONS]\n");
        else
            printf("\nUsage: mod_override_list [OPTIONS] ID\n");
        printf("%s \n", des_string);
        printf("Options:\n");
        printf("        -alutreg Number             Override LUT ID\n");
        printf
            ("        -ov_alutreg Value           LUT ID to Be Overridden\n");
        printf
            ("        -ov_stereo Value            Stereo Setting to be Overridden: Choice \n");
        printf("                                    of No, Yes, Any \n");
        printf
            ("        -ov_hres Value              Horizontal Resolution to be Overridden\n");
        printf
            ("        -ov_vres Value              Vertical Resolution to be Overridden\n");
        printf
            ("        -ov_refresh Value           Refresh Rate to be Overridden\n");
        printf
            ("        -ov_pcr Value               Pixel Clock Rate to be Overridden\n");
        printf("        -boardNumber Number\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}

int add_override_list_main(int argc, char **argv)
{
    return update_override_list_cli(argc, argv, ADD_REQUEST);
}

int mod_override_list_main(int argc, char **argv)
{
    return update_override_list_cli(argc, argv, MOD_REQUEST);
}


int del_override_list_main(int argc, char **argv)
{

    int c, opt_index = 0;
    int rc = RC_NORMAL;
    //int number_of_items;
    //int value;
    char error_string[128];
    struct override_list_params *paramsp;
    struct override_list_params *orig_param_ptr = NULL;
    int access_id;
    HAL *hp = NULL;
    int index = 0;

    hp = NULL;
    //value = 0;
    remote_config_start();
    remote_config_set_config_name(override_list_title);
    remote_config_set_command_name("del_override_list");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);


    if (view_only() != 0) {
        print_cli_failure(del_des_override_list, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf
            ("You must be root to set List of Override LUTs parameters\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", override_list_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 7:
                remote_config_set_client_data(optarg);
                break;
            case 8:
                board_number = atoi(optarg);
                break;
            }
        }
        else {
            rc = RC_ERROR;
            break;
        }
    }

    hp = hal_acquire(board_number);
    remote_config_set_board_number(board_number);
    if (hp == NULL) {
        print_cli_failure(del_des_override_list, CLI_REASON_WRONG_HW);
        remote_config_send_error(CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    if ((rc == RC_NORMAL) && (optind == (argc - 1))) {

        access_id = atoi(argv[optind]);
        remote_config_set_index(access_id);

        //number_of_items = parse_override_list(hp);
        (void) parse_override_list(hp);

        // Find the entry and the index it is at
        index = 0;
        foreach_override_list_entry(paramsp) {
            index++;
            if (paramsp->override_list_id == access_id) {
                orig_param_ptr = paramsp;
                break;
            }
        }

        if (orig_param_ptr == NULL) {
            print_cli_failure(del_des_override_list, CLI_REASON_BAD_INDEX);
            remote_config_send_error(CLI_REASON_BAD_INDEX);
            return RC_ERROR;
        }
        setup_parameter_values(orig_param_ptr);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validateOverrideList(DEL_REQUEST, NULL, orig_param_ptr,
                                  error_string);


        if (rc != RC_NORMAL) {
            print_cli_failure(del_des_override_list, error_string);
            remote_config_send_error(error_string);
            return rc;
        }

        if (rc == RC_NORMAL) {
            rc = applyOverrideList(DEL_REQUEST, NULL, orig_param_ptr);

            if (rc == RC_NORMAL) {
                delete_override_list(index - 1, &access_id);
                print_cli_success(del_des_override_list);
                remote_config_send();
                save_override_list();
                if (savedHook != NULL)
                    (*savedHook) (DEL_REQUEST, hp);
            }
        }
        if (rc != RC_NORMAL) {
            print_cli_failure(del_des_override_list,
                              CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        }
        free_override_list();
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(del_des_override_list, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(del_des_override_list, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);

        printf("\nUsage: del_override_list [OPTIONS] ID\n");
        printf("%s \n", del_des_override_list);
        printf("Options:\n");
        printf("        -boardNumber Number\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}


int load_override_list_main(int argc, char **argv)
{

    verbose_errors = 1;
    loadOverrideList();
    return 0;
}


int loadOverrideList(void)
{
    struct override_list_params *paramsp;
    HAL *hp;
    hp = NULL;

    int number_of_boards = 1;
    int board;
    hp = hal_acquire(0);
    if (hp == NULL)
        return 0;

    number_of_boards = hal_get_number_of_boards(hp);
    hal_release(hp);

    for (board = 0; board < number_of_boards; board++) {
        board_number = board;
        hp = hal_acquire(board);
        if (hp == NULL)
            continue;
        retrieveOverrideListValues(hp); // Retrieve current list

        foreach_override_list_entry(paramsp) {
            applyOverrideList(ADD_REQUEST, paramsp, NULL);
        }

        if (savedHook != NULL)
            (*savedHook) (ADD_REQUEST, hp);

        if (hp != NULL)
            hal_release(hp);
    }
    return 0;
}


int dumpOverrideList(void)
{

    int ctr = 0;
    struct override_list_params *paramsp;
    HAL *hp;
    hp = NULL;

    int number_of_boards = 1;
    int board;
    hp = hal_acquire(0);
    if (hp == NULL)
        return 0;
    number_of_boards = hal_get_number_of_boards(hp);
    hal_release(hp);

    for (board = 0; board < number_of_boards; board++) {
        board_number = board;
        hp = hal_acquire(board);
        if (hp == NULL)
            continue;

        retrieveOverrideListValues(hp);

        foreach_override_list_entry(paramsp) {

            ctr = 0;
            remote_config_start();
            remote_config_set_config_name(override_list_title);
            remote_config_set_command_name("add_override_list");
            remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
            remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);
            remote_config_set_index(paramsp->override_list_id);

            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->alutreg);
            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->ov_alutreg);
            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->ov_stereo);
            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->ov_hres);
            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->ov_vres);
            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->ov_refresh);
            remote_config_add_parameter(override_list_options[ctr++].name,
                                        paramsp->ov_pcr);

            remote_config_set_board_number(board_number);

            remote_config_send();

            if (hp != NULL)
                hal_release(hp);
        }

    }
    return 0;
}
#endif                          /* LIBONLY */
