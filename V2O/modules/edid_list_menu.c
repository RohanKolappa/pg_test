#include <stdlib.h>
#include <string.h>
#include <getopt.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_edid.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "configfile_parser.h"
#include "vlog.h"
#include "remoteConfig.h"
#include "lut_utils.h"
#include "edid_list_menu.h"
#include "edid_utils.h"
#include "system_utils.h"
#include "cli_utils.h"

#define FIELD_DUMMY          0
#define FIELD_ALUTREG        1
#define FIELD_EDID_DESC      2
#define FIELD_EDID_HPOL      3
#define FIELD_EDID_VPOL      4
#define FIELD_NAME           5
#define FIELD_STEREO         6
#define FIELD_HRES           7
#define FIELD_VRES           8
#define FIELD_REFRESH        9
#define FIELD_PCR            10

#ifndef LIBONLY

static char *describe_edid_list = "Describe EDID Descriptors Table";
static char *get_edid_list = "Get List of EDID Descriptors Table";
static char *add_des_edid_list = "Add List of EDID Descriptors Entry";
static char *mod_des_edid_list = "Modify List of EDID Descriptors Entry";
static char *del_des_edid_list = "Delete List of EDID Descriptors Entry";
static char *edid_list_title = "EdidDescriptorList";

static int update_edid_list_dialog(MENUINFO * minfo, int idx,
                                   int req_code);
static int delete_edid_list(int idx, int *edid_list_idp);
static int update_edid_list_entry(int idx,
                                  struct edid_list_params *params);
static char *edid_list_help =
    "Press </U>A<!U> to Add, </U>D<!U> to Delete, </U>M<!U> to Modify,\n</U>Enter<!U> to Browse or </U>Q<!U> to Quit. ";
static char *add_edid_list_dialog_help =
    "Press </U>Enter<!U> to Select Entry for EDID List or </U>Q<!U> to Quit. ";

static struct option edid_list_options[] = {
    {"alutreg", 1, 0, 0},
    {"descr", 1, 0, 0},
    {"hpol", 1, 0, 0},
    {"vpol", 1, 0, 0},
    {"clientData", 1, 0, 0},
    {"channelNumber", 1, 0, 0},
    {0, 0, 0, 0}
};

static char *hpol_values[] = { "Pos", "Neg", NULL };
static char *vpol_values[] = { "Pos", "Neg", NULL };

static int lutlistcount = 0;

#endif                          /* LIBONLY */

static struct edid_list_params *edid_list_table_head = NULL;

struct edid_list_params *first_edid_list_entry(void)
{
    return edid_list_table_head;
}

struct edid_list_params *next_edid_list_entry(struct edid_list_params
                                              *params)
{
    return params->next;
}

static int board_number = 0;
static struct lut_list_params lutlistparams;

int validateEdidDescriptorList(int req_code,
                               struct edid_list_params *params,
                               struct edid_list_params *orig_params,
                               char * error_string) {

    struct edid_list_params *paramsp;

    // Validate add request
    if (req_code == ADD_REQUEST) {
        int count = 0;
        foreach_edid_list_entry(paramsp) {
            count++;
            if (paramsp->edid_desc_value == params->edid_desc_value) {
                sprintf(error_string, "Duplicate: Desc# \'%d\' conflicts.",
                         params->edid_desc_value);
                return RC_DUPLICATE;
            }
        }
        if (count > MAX_EDID_DESC_COUNT) {
             sprintf(error_string, "Too Many EDID descriptors");
            return RC_ERROR;
        }
    }

    // Should not allow change of LUTID during modify
    if (req_code == MOD_REQUEST) {
        if ((params->edid_alutreg_set == 1) &&
            (params->edid_alutreg_value != orig_params->edid_alutreg_value)) {
                 sprintf(error_string, "Cannot modify LUTID");
                 return RC_ERROR;
        }
    }
    return RC_NORMAL;
}

// Apply request to edid list
int applyEdidDescriptorList(int req_code,
                            struct edid_list_params *params,
                            struct edid_list_params *orig_params) {
    HAL *hp;

    if (req_code == DEL_REQUEST)
        hp = orig_params->userptr;
    else
        hp = params->userptr;

    if (hp == NULL) 
        return RC_ERROR;

    return RC_NORMAL;
}

static char *get_config_file_name(void)
{
    static char buf[128];
    sprintf(buf, "/V2O/config/edidlistconf_%d.txt", board_number);
        return buf;
}

#ifndef LIBONLY
static char *get_lutname(char *buf, void *usrptr, int ident) 
{
    strcpy(buf, "???");
    
    /* get LUT list parameters only once */
    if (usrptr != NULL) {
        struct edid_list_params *params = (struct edid_list_params *) usrptr;
        if (params->edid_alutreg_set == 1) {
            if (get_lut_list_params_by_lutid(&lutlistparams, 
                        params->edid_alutreg_value) != 0) {
                strncpy(buf, lutlistparams.name_value, 
                        FIELD_EDID_LIST_NAME_LENGTH + 1);
            }
            else 
                memset(&lutlistparams, '\0', sizeof(struct lut_list_params));
        }
    }
    else {
        strncpy(buf, lutlistparams.name_value, 
                FIELD_EDID_LIST_NAME_LENGTH + 1);
    }
    return buf;
}

#endif                          /* LIBONLY */

static char *get_alutreg(char *buf, void *usrptr, int ident)
{
    if (usrptr != NULL) {
        struct edid_list_params *params =
            (struct edid_list_params *) usrptr;
        if (params->edid_alutreg_set == 1) 
            strncpy(buf, params->edid_alutreg,
                    FIELD_EDID_LIST_ALUTREG_LENGTH + 1);
    }
    else {
        snprintf(buf, FIELD_EDID_LIST_ALUTREG_LENGTH, "%d", 
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
    snprintf(buf, FIELD_EDID_LIST_HRES_LENGTH + 1, "%d", 
           lutlistparams.orig_hres_value); 
    return buf;
}

static char *get_vres(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_EDID_LIST_VRES_LENGTH + 1, "%d", 
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
    snprintf(buf, FIELD_EDID_LIST_HFP_LENGTH + 1, "%d", 
               lutlistparams.hfp_value); 
    return buf;
}

static char *get_vfp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_EDID_LIST_VFP_LENGTH + 1, "%d", 
               lutlistparams.vfp_value); 
    return buf;
}

static char *get_hsync(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_EDID_LIST_HSYNC_LENGTH + 1, "%d", 
               lutlistparams.hsync_value); 
    return buf;
}

static char *get_vsync(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_EDID_LIST_VSYNC_LENGTH + 1, "%d", 
               lutlistparams.vsync_value); 
    return buf;
}

static char *get_hbp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_EDID_LIST_HBP_LENGTH + 1, "%d", 
               lutlistparams.hbp_value); 
    return buf;
}

static char *get_vbp(char *buf, void *usrptr, int ident) 
{
    snprintf(buf, FIELD_EDID_LIST_VBP_LENGTH + 1, "%d", 
               lutlistparams.vbp_value); 
    return buf;
}

#endif                          /* LIBONLY */

static int get_unique_edid_desc(void)
{
    struct edid_list_params *paramsp;
    int unique_desc = 1;

  again:
    paramsp = edid_list_table_head;
    while (paramsp != NULL) {
        if (paramsp->edid_desc_value == unique_desc) {
            unique_desc++;
            goto again;
        }
        paramsp = paramsp->next;
    }
    return unique_desc;
}

static char *get_edid_desc(char *buf, void *usrptr, int ident)
{
    strncpy(buf, "1", FIELD_EDID_LIST_EDID_DESC_LENGTH + 1);

    if (usrptr != NULL) {
        struct edid_list_params *params =
            (struct edid_list_params *) usrptr;
        if (params->edid_desc_set == 1)
            strncpy(buf, params->edid_desc,
                    FIELD_EDID_LIST_EDID_DESC_LENGTH + 1);
    }
    else {
        int edid_desc = get_unique_edid_desc();
        snprintf(buf, FIELD_EDID_LIST_ALUTREG_LENGTH, "%d", edid_desc);
    }
    return buf;
}

static char *get_edid_hpol(char *buf, void *usrptr, int ident) {
    strncpy(buf, "Pos", FIELD_EDID_LIST_EDID_HPOL_LENGTH + 1);
    if (usrptr != NULL) {
        struct edid_list_params *params =
            (struct edid_list_params *) usrptr;
        if (params->edid_hpol_set == 1) 
            strncpy(buf, params->edid_hpol,
                    FIELD_EDID_LIST_EDID_HPOL_LENGTH + 1);
    }
    return buf;
}

static char *get_edid_vpol(char *buf, void *usrptr, int ident) {
    strncpy(buf, "Pos", FIELD_EDID_LIST_EDID_VPOL_LENGTH + 1);
    if (usrptr != NULL) {
        struct edid_list_params *params =
            (struct edid_list_params *) usrptr;
        if (params->edid_vpol_set == 1) 
            strncpy(buf, params->edid_vpol,
                    FIELD_EDID_LIST_EDID_VPOL_LENGTH + 1);
    }
    return buf;
}

static void setup_parameter_values(struct edid_list_params *params)
{
    params->edid_alutreg_value = atoi(params->edid_alutreg);
    params->edid_desc_value = atoi(params->edid_desc);
    do {
        if (strcasecmp(params->edid_hpol, "Pos") == 0) {
            params->edid_hpol_value = EDID_LIST_EDID_HPOL_POS;
            break;
        }
        if (strcasecmp(params->edid_hpol, "Neg") == 0) {
            params->edid_hpol_value = EDID_LIST_EDID_HPOL_NEG;
            break;
        }
    } while (0);
    do {
        if (strcasecmp(params->edid_vpol, "Pos") == 0) {
            params->edid_vpol_value = EDID_LIST_EDID_VPOL_POS;
            break;
        }
        if (strcasecmp(params->edid_vpol, "Neg") == 0) {
            params->edid_vpol_value = EDID_LIST_EDID_VPOL_NEG;
            break;
        }
    } while (0);
}

#ifndef LIBONLY
static struct edid_list_params *get_edid_list_entry_by_index(int index) 
{
    struct edid_list_params *params_iter;
    int count = 0;

    params_iter = edid_list_table_head;
    while (params_iter != NULL) {
        if (count++ == index)
            return params_iter;
        params_iter = params_iter->next;
    }

    return NULL;
}

static char *edid_list_selector_cb(int item)
{
    struct edid_list_params *paramsp;

    paramsp = get_edid_list_entry_by_index(item);

    sprintf(tempbuf, "????");
    if (paramsp != NULL) {
        char lutname[22];
        struct lut_list_params lutparams;

        if (!get_lut_list_params_by_lutid(&lutparams, 
                    paramsp->edid_alutreg_value)) {
            return tempbuf;
        }

        setup_parameter_values(paramsp);

        memset(lutname, '\0', sizeof(lutname));
        strncpy(lutname, lutparams.name_value, sizeof(lutname) - 1);

        sprintf(tempbuf, " %-2d    %-3d  %-22s %-5d %-5d",
                paramsp->edid_desc_value,
                paramsp->edid_alutreg_value,
                lutname,
                lutparams.orig_hres_value,
                lutparams.orig_vres_value);
    }
    return tempbuf;
}
#endif                          /* LIBONLY */

static int count_edid_list(void)
{
    struct edid_list_params *params_iter;
    int count = 0;

    params_iter = edid_list_table_head;
    while (params_iter != NULL) {
        count++;
        params_iter = params_iter->next;
    }
    return count;
}

static void free_edid_list(void)
{
    struct edid_list_params *params_iter, *tmp;
    params_iter = edid_list_table_head;
    while (params_iter != NULL) {
        tmp = params_iter;
        params_iter = params_iter->next;
        free(tmp);
    }
    edid_list_table_head = NULL;

}
static void print_edid_list(struct edid_list_params *params, int count,
                            FILE * fp)
{

    int len;

    fprintf(fp,
            "################################################################\n");
    len = fprintf(fp, "%s.%d", "EDID_LIST_ID", count);
    fprintf(fp, "%*s %d\n", 25 - len, " ", params->edid_list_id);
    len = fprintf(fp, "%s.%d", "ALUTREG", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->edid_alutreg);
    len = fprintf(fp, "%s.%d", "EDID_DESC", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", params->edid_desc);
    len = fprintf(fp, "%s.%d", "EDID_HPOL", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", 
            (params->edid_hpol == EDID_LIST_EDID_HPOL_POS) ? "Pos" : "Neg");
    len = fprintf(fp, "%s.%d", "EDID_VPOL", count);
    fprintf(fp, "%*s %s\n", 25 - len, " ", 
            (params->edid_vpol == EDID_LIST_EDID_VPOL_POS) ? "Pos" : "Neg");
}

static int save_edid_list(void)
{

    struct edid_list_params *params_iter;
    int count = 0;
    FILE *fp;

    if ((fp = fopen(get_config_file_name(), "w+")) == NULL) {
        return -1;
    }

    params_iter = edid_list_table_head;
    while (params_iter != NULL) {
        count++;
        print_edid_list(params_iter, count, fp);
        params_iter = params_iter->next;
    }

    fclose(fp);

    return count;
}

static int get_unique_edid_list_id(void)
{
    struct edid_list_params *paramsp;
    int unique_id = 1;

  again:
    paramsp = edid_list_table_head;
    while (paramsp != NULL) {
        if (paramsp->edid_list_id == unique_id) {
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

    struct edid_list_params *params_iter, *tmp;
    params_iter = edid_list_table_head;
    if (params_iter == NULL)
        return;

    if (params_iter->edid_list_id == list_id) {
        tmp = params_iter;
        edid_list_table_head = params_iter->next;
        free(tmp);
    }
    else {
        while (params_iter->next != NULL) {
            if (params_iter->next->edid_list_id == list_id) {
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

static void add_to_list(struct edid_list_params *params)
{

    struct edid_list_params *params_iter;
    params->next = NULL;
    params_iter = edid_list_table_head;
    if (params_iter == NULL) {
        edid_list_table_head = params;
    }
    else {
        while (params_iter->next != NULL) {
            params_iter = params_iter->next;
        }
        params_iter->next = params;
    }
}

static int edid_list_config_handler(char *name, char *value, void *ptr,
                                    int idx)
{
    static struct edid_list_params params;
    struct edid_list_params *paramsp;
    static int lastidx = 0;


    if (idx != lastidx) {
        if (lastidx == 0 && (idx > 1 || idx == 0))
            return -1;
        if (lastidx == 0 && idx == 1) {
            // prepare to populate new entry
            memset(&params, 0x00, sizeof(struct edid_list_params));
            lastidx = 1;
        }
        else {
            /* Add the struct built up so far to the linked list */
            paramsp = malloc(sizeof(struct edid_list_params));
            if (paramsp == NULL)
                return -1;
            memcpy(paramsp, &params, sizeof(struct edid_list_params));
            if (ptr != NULL)
                paramsp->userptr = ptr;

            // for any values not found, fill in defaults
            if (paramsp->edid_alutreg_set == 0) {
                get_alutreg(paramsp->edid_alutreg, NULL, 0);
                paramsp->edid_alutreg_set = 1;
            }
            if (paramsp->edid_desc_set == 0) {
                get_edid_desc(paramsp->edid_desc, NULL, 0);
                paramsp->edid_desc_set = 1;
            }
            if (paramsp->edid_hpol_set == 0) {
                get_edid_hpol(paramsp->edid_hpol, NULL, 0);
                paramsp->edid_hpol_set = 1;
            }
            if (paramsp->edid_vpol_set == 0) {
                get_edid_vpol(paramsp->edid_vpol, NULL, 0);
                paramsp->edid_vpol_set = 1;
            }
            add_to_list(paramsp);

            // prepare to populate new entry
            memset(&params, 0x00, sizeof(struct edid_list_params));
            lastidx = idx;
        }
    }
    if (strcasecmp(name, "EDID_LIST_ID") == 0) {
        params.edid_list_id = atoi(value);
    }
    else if (strcasecmp(name, "ALUTREG") == 0) {
        params.edid_alutreg_set = 1;
        strncpy(params.edid_alutreg, value, FIELD_EDID_LIST_ALUTREG_LENGTH + 1);
    }
    else if (strcasecmp(name, "EDID_DESC") == 0) {
        params.edid_desc_set = 1;
        strncpy(params.edid_desc, value,
                FIELD_EDID_LIST_EDID_DESC_LENGTH + 1);
    }
    else if (strcasecmp(name, "EDID_HPOL") == 0) {
        params.edid_hpol_set = 1;
        strncpy(params.edid_hpol, value,
                FIELD_EDID_LIST_EDID_HPOL_LENGTH + 1);
    }
    else if (strcasecmp(name, "EDID_VPOL") == 0) {
        params.edid_vpol_set = 1;
        strncpy(params.edid_vpol, value,
                FIELD_EDID_LIST_EDID_VPOL_LENGTH + 1);
    }

    return 0;
}

static int parse_edid_list(void *ptr)
{
    struct edid_list_params *params;
    int count = 0;
    int need_to_save = 0;

    free_edid_list();

    if (parse_list_config_file(get_config_file_name(),
                               edid_list_config_handler, ptr) < 0) {
        VLOG_WARNING("Could not load  configuration file %s",
                      get_config_file_name());
    }
    else {
        // Setup parameter values and fix id values if necessary
        foreach_edid_list_entry(params) {
            setup_parameter_values(params);
            if (params->edid_list_id <= 0) {
                params->edid_list_id = get_unique_edid_list_id();
                need_to_save = 1;
            }
        }
    }
    if (need_to_save)
        save_edid_list();
    count = count_edid_list();
    return count;
}

static EdidDescriptorListSavedFunction *savedHook = NULL;
extern void
registerEdidDescriptorListSavedFunction(EdidDescriptorListSavedFunction *
                                        funcPtr)
{
    if (funcPtr != NULL)
        savedHook = funcPtr;
}

int retrieveEdidDescriptorListValues(HAL * hp)
{
    // Just parse the current config file and foreach_edid_list_entry() 
    // will provide the rest

    if (hp != NULL)
        board_number = hal_get_board_number(hp);
    return parse_edid_list(hp);
}

#ifndef LIBONLY

static FORMITEMDEF itemdefs[] = {
    {FIELD_DUMMY, "EDID Descriptor", "",
     1, NULL, NULL,
     0, 0, 0, O_NULLOK | O_ACTIVE, 0},
    {FIELD_EDID_DESC, "Descriptor#",
     "Descriptor#",
     FIELD_EDID_LIST_EDID_DESC_LENGTH, get_edid_desc, NULL,
     FTYPE_INTEGER, 1, 3, O_NULLOK, 0},
    {FIELD_EDID_HPOL, "Horiz Pol.",
     "EDID Descriptor Horizontal Polarity",
     FIELD_EDID_LIST_EDID_HPOL_LENGTH, get_edid_hpol, NULL,
     FTYPE_ENUM_RANGE, (int) hpol_values, 0, O_NULLOK, 0},
    {FIELD_ALUTREG, "Vert Pol.",
     "EDID Descriptor Vertical Polarity",
     FIELD_EDID_LIST_EDID_VPOL_LENGTH, get_edid_vpol, NULL,
     FTYPE_ENUM_RANGE, (int) vpol_values, 0, O_NULLOK, FFLAG_FOLLOW},
    {FIELD_DUMMY, "", "",
     1, NULL, NULL,
     0, 0, 0, O_NULLOK | O_ACTIVE, 0},
    {FIELD_DUMMY, "LUT for EDID", "",
     1, NULL, NULL,
     0, 0, 0, O_NULLOK | O_ACTIVE, 0},
    {FIELD_NAME, "Name",
     "Name of LUT",
     FIELD_EDID_LIST_NAME_LENGTH, get_lutname, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_ALUTREG, "LUT ID",
     "EDID List LUT ID",
     FIELD_EDID_LIST_ALUTREG_LENGTH, get_alutreg, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_STEREO, "Stereo?",
     "Stereo Setting of LUT",
     FIELD_EDID_LIST_STEREO_LENGTH, get_stereo, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HRES, "Horiz. Resolution",
     "Horizontal Resolution of LUT",
     FIELD_EDID_LIST_HRES_LENGTH, get_hres, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VRES, "Vert. Resolution",
     "Vertical Resolution of LUT",
     FIELD_EDID_LIST_VRES_LENGTH, get_vres, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_REFRESH, "Refresh Rate",
     "Refresh Rate of LUT",
     FIELD_EDID_LIST_REFRESH_LENGTH, get_refresh, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VRES, "Pixel Clock Rate",
     "Pixel Clock Rate of LUT",
     FIELD_EDID_LIST_PCR_LENGTH, get_pcr, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HRES, "Horiz. Front Porch",
     "Horizontal Front Porch of LUT",
     FIELD_EDID_LIST_HFP_LENGTH, get_hfp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VRES, "Vert. Front Porch",
     "Vertical Front Porch of LUT",
     FIELD_EDID_LIST_VFP_LENGTH, get_vfp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HRES, "Horiz. Sync",
     "Horizontal Sync Width of LUT",
     FIELD_EDID_LIST_HSYNC_LENGTH, get_hsync, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VRES, "Vert. Sync",
     "Vertical Sync Width of LUT",
     FIELD_EDID_LIST_VSYNC_LENGTH, get_vsync, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {FIELD_HRES, "Horiz. Back Porch",
     "Horizontal Back Porch of LUT",
     FIELD_EDID_LIST_HBP_LENGTH, get_hbp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},
    {FIELD_VRES, "Vert. Back Porch",
     "Vertical Back Porch of LUT",
     FIELD_EDID_LIST_VBP_LENGTH, get_vbp, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},
    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

/***********************************
 * Configures EDID descriptor list *
 ***********************************/
int configureEdidDescriptorList(MENUINFO * minfo)
{
    MENU *selector;
    DIALOG *dialog;
    int rc;
    int idx;
    int number_of_items;
    char keylist[12];
    char titlebuf[120];
    int retval = RC_NORMAL;
    int edid_list_id = 0;
    struct edid_list_params *orig_param_ptr;
    unsigned char edidbuf[EDID_DATA_LEN];
    char error_string[128];
    HAL *hp = NULL;

    {
        hp = minfo->minfo_userptr;
        if (hp != NULL) { 
            board_number = hal_get_board_number(hp);
        }
    }

    /****************************************************
     * Set EDID descriptor list to that from the EEPROM *
     ****************************************************/
    if (hal_read_edid_eeprom(hp, edidbuf, sizeof(edidbuf)) < 0) {
        VLOG_INFO("Failed to read EDID EEPROM");
        return -1;
    }

    update_lut_from_buffer(hp, edidbuf);

    update_edid_list_from_buffer(hp, edidbuf);

    /*******************************************************
     * Retreive and get count of lut list params only once *
     *******************************************************/
    lutlistcount = retrieveLUTListValues(hp);

    number_of_items = parse_edid_list(hp);

    if (number_of_items == 0) {
        dialog = show_dialog(minfo, 8, 10,
                "</K/U>Empty List of EDID Descriptors Table!!<!K!U>");

        show_info(minfo, "Add new List of EDID Descriptors?" YESNO_STR);
        if (get_yesno(minfo)) {
            remove_dialog(dialog);
            rc = update_edid_list_dialog(minfo, 0, ADD_REQUEST);
            if (rc == RC_EXIT) {
                free_edid_list();
                return RC_EXIT;
            }
            if (rc != RC_NORMAL) {
                free_edid_list();
                return retval;
            }
            number_of_items = count_edid_list();
            if (number_of_items == 0) {
                free_edid_list();
                return retval;
            }
            show_title(minfo, titlebuf);
        }
        else {
            remove_dialog(dialog);
            free_edid_list();
            return retval;
        }
    }

    /********************
     * Print Menu title *
     ********************/
    sprintf(titlebuf, "%s\n  \n</U>%s", minfo->minfo_title,
                      "   Descr LutId       LUT Name         Hres  Vres");
    show_title(minfo, titlebuf);

    selector = create_selector(number_of_items, edid_list_selector_cb);

    set_menu_windows(minfo, selector);

    post_menu(selector);

    show_info(minfo, edid_list_help);

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
                    "Delete List of EDID Descriptors at Index %d ?"
                    YESNO_STR, idx);
            show_info(minfo, tempbuf);
            if (get_yesno(minfo)) {
                orig_param_ptr = get_edid_list_entry_by_index(rc);
                if (orig_param_ptr == NULL) {
                    return RC_ERROR;
                }
                orig_param_ptr->userptr = minfo->minfo_userptr;
                setup_parameter_values(orig_param_ptr);

                /* Call custom validator */
                sprintf(error_string, "Invalid Data");
                rc = validateEdidDescriptorList(DEL_REQUEST, NULL,
                                                orig_param_ptr,
                                                error_string);

                if (rc == RC_NORMAL) {

                    rc = applyEdidDescriptorList(DEL_REQUEST, NULL,
                                                 orig_param_ptr);

                    if (rc == RC_NORMAL) {
   
                        remote_config_start();
                        remote_config_set_config_name(edid_list_title);
                        remote_config_set_command_name("del_edid_list");
                        remote_config_set_operation
                            (REMOTE_CONFIG_OPERATION_DELETE);
                        remote_config_set_source
                            (REMOTE_CONFIG_SOURCE_MENU);

                        delete_edid_list(idx - 1, &edid_list_id);

                        remote_config_set_index(edid_list_id);
                        remote_config_send();

                        number_of_items = count_edid_list();
                        save_edid_list();

                        if (savedHook != NULL)
                            (*savedHook) (DEL_REQUEST);

                        if (number_of_items == 0) {
                            show_info(minfo,
                                      "Deleted last EDID Descriptor. "
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
                                 edid_list_selector_cb);
                post_menu(selector);
            }

            show_info(minfo, edid_list_help);
            break;

        case 'a':
        case 'A':
            rc = -1;
        case 'm':
        case 'M':
            unpost_menu(selector);
            idx = rc + 1;
            rc = update_edid_list_dialog(minfo, 
                    idx, 
                    (keylist[0] == 'a' || keylist[0] == 'A') ? ADD_REQUEST :
                    MOD_REQUEST);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            if (rc == RC_NORMAL) {
                number_of_items = count_edid_list();
                refresh_selector(selector, number_of_items,
                                 edid_list_selector_cb);
                if (idx > 0)
                    set_selector_item(selector, idx - 1);
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, edid_list_help);
            break;


        case '\0':
            unpost_menu(selector);
            idx = rc + 1;
            rc = update_edid_list_dialog(minfo, idx, 1);
            if (rc == RC_EXIT) {
                retval = RC_EXIT;
                goto QUIT_LABEL;
            }
            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, edid_list_help);
            break;
        }
    }

  QUIT_LABEL:

    /***********************************
     * Check if should write to EEPROM *
     ***********************************/
    sprintf(tempbuf, "Write EDID List to EEPROM? " YESNO_STR);
    show_info(minfo, tempbuf);

   if (get_yesno(minfo)) {
       unsigned char edidbuf[EDID_DATA_LEN];
       int rc, video_input = 0x80, board_type = hal_get_board_type(hp);

       /************************
        * Show warning message *
        ************************/
       if (board_type == BOARDTYPE_TX) {
           dialog = show_dialog(minfo, 3, 0,
               "Writing to EDID EEPROM with DVI cable connected\n"
               "to a host computer may corrupt the EEPROM.\n"
               "</K></U>Warning:<!U><!K> Make sure the DVI cable is disconnected\n"
               "before proceeding.\n"
               "Note: If problem in reading or writing EDID\n"
               "EEPROM persists, you may need to power cycle\n"
               "the V2D to recover.");
       }
       else {
           dialog = show_dialog(minfo, 3, 0,
               "Writing to EDID EEPROM with DVI cable connected\n"
               "on local input port may corrupt the EEPROM.\n"
               "</K></U>Warning:<!U><!K> Make sure the DVI cable is disconnected\n"
               "before proceeding.\n"
               "Note: If problem in reading or writing EDID\n"
               "EEPROM persists, you may need to power cycle\n"
               "the V2D to recover.");
       }
       show_info(minfo, PRESSANYKEY_STR);
       get_anykey(minfo);
       remove_dialog(dialog);

       /********************************
        * Get video input from EEPROM  * 
        ********************************/
       if (hal_read_edid_eeprom(hp, edidbuf, sizeof(edidbuf)) == 0) 
           video_input = edidbuf[20];

       /********************************
        * Write EDID to EEPROM or FPGA * 
        ********************************/
       if ((rc = load_edid_eeprom_with_descriptor(hp, video_input) < 0)) {
           show_info(minfo, "Failed to write EDID EEPROM. " PRESSANYKEY_STR);
           retval = RC_ERROR;
       }
       else
           show_info(minfo, "EDID EEPROM Updated. " PRESSANYKEY_STR);
       get_anykey(minfo);
    }

    free_edid_list();
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

static int add_edid_list_dialog(MENUINFO * minfo, 
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
                             "</K/U>Empty List of Video LUTs!!<!K!U>");
        show_info(minfo, PRESSANYKEY_STR);
        get_anykey(minfo);
        remove_dialog(dialog);
        return RC_ERROR;
    }

    selector = create_selector(lutlistcount, lut_list_selector_cb);
    set_menu_windows(minfo, selector);
    post_menu(selector);
    show_info(minfo, add_edid_list_dialog_help);

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

static int update_edid_list_dialog(MENUINFO * minfo, int idx, int req_code)
{

    FORM *form = NULL;
    int rc = E_OK;
    int retval = RC_NORMAL;
    char error_string[128];
    struct edid_list_params params;
    struct edid_list_params *orig_param_ptr = NULL;
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

    number_of_items = count_edid_list();

  redraw:
    if (req_code == DISPLAY_REQUEST) {
        sprintf(titlebuf, "Display Entry at Index %d", idx);
        show_title(minfo, titlebuf);
    }
    else if (req_code == ADD_REQUEST) {
        if (number_of_items >= MAX_EDID_DESC_COUNT) {
            return RC_EXIT;
        }
        show_title(minfo, "Add New Entry");
        rc = add_edid_list_dialog(minfo, &lutparams);
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
        orig_param_ptr = get_edid_list_entry_by_index(idx - 1);
    }

    show_title(minfo, NULL);

    form = create_form(itemdefs, 0, 0, orig_param_ptr);

    set_form_windows(minfo, form);

    post_form(form);

    if (req_code == DISPLAY_REQUEST) {        // Display request only
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
            if (req_code == ADD_REQUEST ||get_form_status(form) == TRUE) {
                if (req_code == ADD_REQUEST)
                    show_info(minfo, "Add this Entry?" YESNO_STR);
                else {
                    show_info(minfo, "Update this Entry?" YESNO_STR);
                    memset(&params, 0x00, sizeof(params));
                }
                if (get_yesno(minfo)) {
                    rc = RC_NORMAL;
                    memset(&params, 0x00, sizeof(params));

                    get_field_buffer(form, FIELD_ALUTREG, params.edid_alutreg);
                    params.edid_alutreg_set = 1;
                    get_field_buffer(form, FIELD_EDID_DESC, params.edid_desc);
                    params.edid_desc_set = 1;

                    setup_parameter_values(&params);
                    params.userptr = minfo->minfo_userptr;
                    // orig_params only available in case of modify
                    if (orig_param_ptr != NULL) {
                        setup_parameter_values(orig_param_ptr);
                        params.edid_list_id = orig_param_ptr->edid_list_id;
                        orig_param_ptr->userptr = minfo->minfo_userptr;
                    }
                    else {
                        params.edid_list_id = get_unique_edid_list_id();
                    }

                    /* Call custom validator */
                    sprintf(error_string, "Invalid Data");
                    rc = validateEdidDescriptorList(idx ==
                                                    0 ? ADD_REQUEST :
                                                    MOD_REQUEST, &params,
                                                    orig_param_ptr,
                                                    error_string);

                    if (rc == RC_NORMAL) {
                        remote_config_start();
                        remote_config_set_config_name(edid_list_title);
                        if (idx == 0) {
                            remote_config_set_command_name
                                ("add_edid_list");
                            remote_config_set_operation
                                (REMOTE_CONFIG_OPERATION_ADD);
                        }
                        else {
                            remote_config_set_command_name
                                ("mod_edid_list");
                            remote_config_set_operation
                                (REMOTE_CONFIG_OPERATION_MODIFY);
                        }
                        remote_config_set_source
                            (REMOTE_CONFIG_SOURCE_MENU);
                        remote_config_add_parameter(edid_list_options[0].
                                                    name, params.edid_alutreg);
                        remote_config_add_parameter(edid_list_options[1].
                                                    name,
                                                    params.edid_desc);

                        rc = applyEdidDescriptorList(idx ==
                                                         0 ? ADD_REQUEST :
                                                         MOD_REQUEST,
                                                         &params,
                                                         orig_param_ptr);

                        if (rc == RC_NORMAL) 
                            rc = update_edid_list_entry(idx, &params);

                        remote_config_set_index(params.edid_list_id);
                        if (rc == RC_NORMAL) {
                            save_edid_list();
                            if (savedHook != NULL)
                                (*savedHook) (idx ==
                                              0 ? ADD_REQUEST :
                                              MOD_REQUEST);

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


static int update_edid_list_entry(int idx, struct edid_list_params *params)
{

    struct edid_list_params *paramsp;

    if (idx == 0) {             // add request
        paramsp = malloc(sizeof(struct edid_list_params));
        if (paramsp != NULL) {
            memcpy(paramsp, params, sizeof(struct edid_list_params));
            add_to_list(paramsp);
        }
        else {
            return RC_ERROR;
        }
    }
    else {                      // modify request
        /* Copy values into existing struct in list, retaining needed vaules */
        paramsp = get_edid_list_entry_by_index(idx - 1);
        params->edid_list_id = paramsp->edid_list_id;
        params->userptr = paramsp->userptr;
        params->next = paramsp->next;
        memcpy(paramsp, params, sizeof(struct edid_list_params));
    }

    return RC_NORMAL;
}


static int delete_edid_list(int idx, int *edid_list_idp)
{
    int number_of_items;
    struct edid_list_params *paramsp;

    number_of_items = count_edid_list();

    if (idx >= number_of_items)
        return RC_ERROR;

    paramsp = get_edid_list_entry_by_index(idx);
    if (paramsp == NULL)
        return RC_ERROR;

    *edid_list_idp = paramsp->edid_list_id;

    delete_from_list(paramsp->edid_list_id);

    return RC_NORMAL;
}
int describe_edid_list_main(int argc, char **argv)
{
    int rc = RC_NORMAL;

    printf ("<MenuDescription>\n");
    printf ("\t<Type>configlist</Type>\n");
    printf ("\t<Title>EDIDList</Title>\n");
    printf ("\t<Description>List of Preferred Timings in EDID to Source</Description>\n");
    printf ("\t<BoardAccess>true</BoardAccess>\n");
    printf ("\t<CliCommandName>edid_list</CliCommandName>\n");
    printf ("\t<ConfigFile>/V2O/config/edidlistconf</ConfigFile>\n");
    printf ("\t<MenuItems>\n");
    printf ("\t\t<Attribute name=\"alutreg\" label=\"LUT Id\" type=\"string\" size=\"4\" defaultvalue=\"1\"  validation=\"enumeration\" values=\"lut_list_alutreg ,\"/>\n");
    printf ("\t\t<Attribute name=\"descr\" label=\"Descriptor Id\" type=\"string\" size=\"4\" defaultvalue=\"1\"  validation=\"enumeration\" values=\"1 , 2 , 3 ,\"/>\n");
    printf ("\t\t<Attribute name=\"hpol\" label=\"Horizontal Polarity \" type=\"string\" size=\"4\" defaultvalue=\"Neg\"  validation=\"enumeration\" values=\"Pos , Neg ,\"/>\n");
    printf ("\t\t<Attribute name=\"vpol\" label=\"Vertical Polarity \" type=\"string\" size=\"4\" defaultvalue=\"Neg\"  validation=\"enumeration\" values=\"Pos , Neg ,\"/>\n");
    printf ("\t</MenuItems>\n");
    printf ("</MenuDescription>\n");
    return rc;
}


int get_edid_list_main(int argc, char **argv)
{
    int opt_idx = 1;
    int rc = RC_NORMAL;
    int ctr = 0;
    HAL *hp = NULL;
    struct edid_list_params *params;
    int c, opt_index = 0;

    while (1) {
        c = getopt_long_only(argc, argv, "h", edid_list_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {
            switch (opt_index) {
            case 5:
                board_number = atoi(optarg) - 1;
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
        print_cli_failure(get_edid_list, CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }
    if (opt_idx == argc) {
        parse_edid_list(hp);
        printf(XML_GROUP, get_edid_list);
        print_cli_success(NULL);
        printf("  " XML_TABLE, edid_list_title);
        foreach_edid_list_entry(params) {
            ctr = 0;
            printf("    " XML_OBJINDEX, params->edid_list_id,
                   "List of EDID Descriptors Entry");
            printf("      " XML_ATTRSTR, edid_list_options[ctr++].name,
                   params->edid_alutreg);
            printf("      " XML_ATTRSTR, edid_list_options[ctr++].name,
                   params->edid_desc);
            printf("      " XML_ATTRSTR, edid_list_options[ctr++].name,
                    params->edid_hpol);
            printf("      " XML_ATTRSTR, edid_list_options[ctr++].name,
                   params->edid_vpol);
            printf("  " XML_OBJEND);
        }
        printf("  " XML_TABEND);
        printf(XML_GROUPEND);
        free_edid_list();
    }
    else {
        rc = RC_ERROR;
        print_cli_failure(get_edid_list, CLI_REASON_BAD_USAGE);
        printf("\nUsage: get_edid_list -boardNumber Number\n");
        printf("Get List of EDID Descriptors\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}


static int update_edid_list_cli(int argc, char **argv, int req_code)
{

    int c, opt_index = 0;
    int rc = RC_NORMAL;
    int value;
    char error_string[128];
    struct edid_list_params params, *paramsp;
    struct edid_list_params *orig_param_ptr = NULL;
    int access_id;
    HAL *hp = NULL;
    char *des_string;
    int index = 0;

    value = 0;
    remote_config_start();
    remote_config_set_config_name(edid_list_title);
    if (req_code == ADD_REQUEST) {
        remote_config_set_command_name("add_edid_list");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        des_string = add_des_edid_list;
    }
    else {
        remote_config_set_command_name("mod_edid_list");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        des_string = mod_des_edid_list;
    }
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

    memset(&params, 0x00, sizeof(params));

    if (view_only() != 0) {
        print_cli_failure(des_string, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf
            ("You must be root to set List of EDID Descriptors parameters\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", edid_list_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 0:
                if (strlen(optarg) > FIELD_EDID_LIST_ALUTREG_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if ((value = atoi(optarg)) <= 169 && value >= 1) {
                    params.edid_alutreg_set = 1;
                    strncpy(params.edid_alutreg, optarg,
                            FIELD_EDID_LIST_ALUTREG_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 1:
                if (strlen(optarg) > FIELD_EDID_LIST_EDID_DESC_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if ((value = atoi(optarg)) <= 3 && value >= 1) {
                    params.edid_desc_set = 1;
                    strncpy(params.edid_desc, optarg,
                            FIELD_EDID_LIST_EDID_DESC_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 2:
                if (strlen(optarg) > FIELD_EDID_LIST_EDID_HPOL_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (!strcasecmp(optarg, "Pos") || !strcasecmp(optarg, "Neg")) {
                    params.edid_hpol_set = 1;
                    strncpy(params.edid_hpol, optarg,
                            FIELD_EDID_LIST_EDID_HPOL_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 3:
                if (strlen(optarg) > FIELD_EDID_LIST_EDID_VPOL_LENGTH) {
                    rc = RC_INVALID_VALUE;
                }
                else if (!strcasecmp(optarg, "Pos") || !strcasecmp(optarg, "Neg")) {
                    params.edid_vpol_set = 1;
                    strncpy(params.edid_vpol, optarg,
                            FIELD_EDID_LIST_EDID_VPOL_LENGTH + 1);
                }
                else {
                    rc = RC_INVALID_VALUE;
                }
                break;
            case 4:
                remote_config_set_client_data(optarg);
                break;
            case 5:
                board_number = atoi(optarg) - 1;
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

        (void) parse_edid_list(hp);

        // For modify, find the entry and the index it is at
        if (req_code == MOD_REQUEST) {
            index = 0;
            foreach_edid_list_entry(paramsp) {
                index++;
                if (paramsp->edid_list_id == access_id) {
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
            params.edid_list_id = orig_param_ptr->edid_list_id;
        }
        else {
            params.edid_list_id = get_unique_edid_list_id();
        }

        // For params that did not come from command line, pick from defaults
        // (for add) or from orig_param_ptr (for modify)

        if (params.edid_alutreg_set == 1) {
            remote_config_add_parameter(edid_list_options[0].name,
                                        params.edid_alutreg);
        }
        else {
            get_alutreg(params.edid_alutreg, orig_param_ptr, 0);
        }
        if (params.edid_desc_set == 1) {
            remote_config_add_parameter(edid_list_options[1].name,
                                        params.edid_desc);
        }
        else {
            get_edid_desc(params.edid_desc, orig_param_ptr, 0);
        }
        if (params.edid_hpol_set == 1) {
            remote_config_add_parameter(edid_list_options[2].name,
                                        params.edid_hpol);
        }
        else {
            get_edid_hpol(params.edid_hpol, orig_param_ptr, 0);
        }
        if (params.edid_vpol_set == 1) {
            remote_config_add_parameter(edid_list_options[3].name,
                                        params.edid_vpol);
        }
        else {
            get_edid_vpol(params.edid_vpol, orig_param_ptr, 0);
        }

        setup_parameter_values(&params);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validateEdidDescriptorList(req_code, &params, orig_param_ptr,
                                        error_string);

        if (rc != RC_NORMAL) {
            print_cli_failure(des_string, error_string);
            remote_config_send_error(error_string);
            return rc;
        }

        if (rc == RC_NORMAL) {
            rc = applyEdidDescriptorList(req_code, &params,
                                         orig_param_ptr);

            if (rc == RC_NORMAL) {
                rc = update_edid_list_entry(index, &params);
            }
            remote_config_set_index(params.edid_list_id);

            if (rc == RC_NORMAL) {
                print_cli_success(des_string);
                remote_config_send();
                save_edid_list();
                if (savedHook != NULL)
                    (*savedHook) (req_code);
            }
        }
        if (rc != RC_NORMAL) {
            print_cli_failure(des_string, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        }
        free_edid_list();
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(des_string, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(des_string, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        if (req_code == ADD_REQUEST)
            printf("\nUsage: add_edid_list [OPTIONS]\n");
        else
            printf("\nUsage: mod_edid_list [OPTIONS] ID\n");
        printf("%s \n", des_string);
        printf("Options:\n");
        printf("        -alutreg Number             EDID List LUT ID\n");
        printf("        -desc Number                EDID Descriptor #\n");
        printf("        -channelNumber Number\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}

int add_edid_list_main(int argc, char **argv)
{
    return update_edid_list_cli(argc, argv, ADD_REQUEST);
}

int mod_edid_list_main(int argc, char **argv)
{
    return update_edid_list_cli(argc, argv, MOD_REQUEST);
}

int del_edid_list_main(int argc, char **argv)
{

    int c, opt_index = 0;
    int rc = RC_NORMAL;
    char error_string[128];
    struct edid_list_params *paramsp;
    struct edid_list_params *orig_param_ptr = NULL;
    int access_id;
    HAL *hp = NULL;
    int index = 0;

    hp = NULL;
    remote_config_start();
    remote_config_set_config_name(edid_list_title);
    remote_config_set_command_name("del_edid_list");
    remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
    remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);


    if (view_only() != 0) {
        print_cli_failure(del_des_edid_list, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf
            ("You must be root to set List of EDID Descriptors parameters\n");
        return RC_ERROR;
    }

    while (1) {
        c = getopt_long_only(argc, argv, "h", edid_list_options,
                             &opt_index);
        if (c == -1) {
            break;
        }
        else if (c == 0) {

            switch (opt_index) {
            case 2:
                remote_config_set_client_data(optarg);
                break;
            case 3:
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
        print_cli_failure(del_des_edid_list, CLI_REASON_WRONG_HW);
        remote_config_send_error(CLI_REASON_WRONG_HW);
        return RC_ERROR;
    }

    if ((rc == RC_NORMAL) && (optind == (argc - 1))) {

        access_id = atoi(argv[optind]);
        remote_config_set_index(access_id);

        (void) parse_edid_list(hp);

        // Find the entry and the index it is at
        index = 0;
        foreach_edid_list_entry(paramsp) {
            index++;
            if (paramsp->edid_list_id == access_id) {
                orig_param_ptr = paramsp;
                break;
            }
        }

        if (orig_param_ptr == NULL) {
            print_cli_failure(del_des_edid_list, CLI_REASON_BAD_INDEX);
            remote_config_send_error(CLI_REASON_BAD_INDEX);
            return RC_ERROR;
        }
        setup_parameter_values(orig_param_ptr);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validateEdidDescriptorList(DEL_REQUEST, NULL, orig_param_ptr,
                                        error_string);

        if (rc != RC_NORMAL) {
            print_cli_failure(del_des_edid_list, error_string);
            remote_config_send_error(error_string);
            return rc;
        }

        if (rc == RC_NORMAL) {
            rc = applyEdidDescriptorList(DEL_REQUEST, NULL,
                                         orig_param_ptr);

            if (rc == RC_NORMAL) {
                delete_edid_list(index - 1, &access_id);
                print_cli_success(del_des_edid_list);
                remote_config_send();
                save_edid_list();
                if (savedHook != NULL)
                    (*savedHook) (DEL_REQUEST);
            }
        }
        if (rc != RC_NORMAL) {
            print_cli_failure(del_des_edid_list, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        }
        free_edid_list();
    }
    else if (rc == RC_INVALID_VALUE) {
        print_cli_failure(del_des_edid_list, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
    }
    else {
        print_cli_failure(del_des_edid_list, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);

        printf("\nUsage: del_edid_list [OPTIONS] ID\n");
        printf("%s \n", del_des_edid_list);
        printf("Options:\n");
        printf("        -boardNumber Number\n");
    }

    if (hp != NULL)
        hal_release(hp);
    return rc;
}


int load_edid_list_main(int argc, char **argv)
{

    verbose_errors = 1;
    loadEdidDescriptorList();
    return 0;
}


int loadEdidDescriptorList(void)
{
    struct edid_list_params *paramsp;
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
        retrieveEdidDescriptorListValues(hp);   // Retrieve current list

        foreach_edid_list_entry(paramsp) {
            applyEdidDescriptorList(ADD_REQUEST, paramsp, NULL);
        }

        if (savedHook != NULL)
            (*savedHook) (ADD_REQUEST);

        if (hp != NULL)
            hal_release(hp);
    }
    return 0;
}


int dumpEdidDescriptorList(void)
{

    int ctr = 0;
    struct edid_list_params *paramsp;
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

        retrieveEdidDescriptorListValues(hp);

        foreach_edid_list_entry(paramsp) {

            ctr = 0;
            remote_config_start();
            remote_config_set_config_name(edid_list_title);
            remote_config_set_command_name("add_edid_list");
            remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
            remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);
            remote_config_set_index(paramsp->edid_list_id);

            remote_config_add_parameter(edid_list_options[ctr++].name,
                                        paramsp->edid_alutreg);
            remote_config_add_parameter(edid_list_options[ctr++].name,
                                        paramsp->edid_desc);

            remote_config_set_board_number(board_number);

            remote_config_send();
        }

        if (hp != NULL)
            hal_release(hp);
    }

    return 0;
}

#endif                          /* LIBONLY */
