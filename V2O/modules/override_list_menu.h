#ifndef __OVERRIDE_LIST_MENU_GEN_H_
#define __OVERRIDE_LIST_MENU_GEN_H_
#include <stdio.h>
#include "mylist.h"

#define FIELD_OVERRIDE_LIST_ALUTREG_LENGTH      4
#define FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH   5
#define FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH    4
#define FIELD_OVERRIDE_LIST_OV_HRES_LENGTH      5
#define FIELD_OVERRIDE_LIST_OV_VRES_LENGTH      5
#define FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH   5
#define FIELD_OVERRIDE_LIST_OV_PCR_LENGTH       6
#define FIELD_OVERRIDE_LIST_NAME_LENGTH         30
#define FIELD_OVERRIDE_LIST_HFP_LENGTH          5
#define FIELD_OVERRIDE_LIST_HBP_LENGTH          5
#define FIELD_OVERRIDE_LIST_HSYNC_LENGTH        5
#define FIELD_OVERRIDE_LIST_VFP_LENGTH          5
#define FIELD_OVERRIDE_LIST_VBP_LENGTH          5
#define FIELD_OVERRIDE_LIST_VSYNC_LENGTH        5

/* Client implement this function to register all customization hooks */
void OverrideListInit(void);

enum override_list_ov_stereo {
    OVERRIDE_LIST_OV_STEREO_NO,
    OVERRIDE_LIST_OV_STEREO_YES,
    OVERRIDE_LIST_OV_STEREO_ANY,
};

/* enum for ov_stereo */

#define OVERRIDE_LIST_OV_HRES_ANY                0xffff
#define OVERRIDE_LIST_OV_VRES_ANY                0xffff
#define OVERRIDE_LIST_OV_ALUTREG_ANY             0xffff
#define OVERRIDE_LIST_OV_REFRESH_ANY             (double) -1
#define OVERRIDE_LIST_OV_PCR_ANY                 (double) -1

struct override_list_params {
    int override_list_id;

/* Clients use these members to access the parameter values */
    int alutreg_value;
    int ov_alutreg_value;
    enum override_list_ov_stereo ov_stereo_value;
    int ov_hres_value;
    int ov_vres_value;
    double ov_refresh_value;
    double ov_pcr_value;

/* Clients use these members to check if a parameter value has been set */
    int alutreg_set;
    int ov_alutreg_set;
    int ov_stereo_set;
    int ov_hres_set;
    int ov_vres_set;
    int ov_refresh_set;
    int ov_pcr_set;

/* Generic user pointer used for various purposes (e.g. to pass HAL ptr) */
    void *userptr;

/* Internal memembers not to be used by client */
    char alutreg[FIELD_OVERRIDE_LIST_ALUTREG_LENGTH + 1];
    char ov_alutreg[FIELD_OVERRIDE_LIST_OV_ALUTREG_LENGTH + 1];
    char ov_stereo[FIELD_OVERRIDE_LIST_OV_STEREO_LENGTH + 1];
    char ov_hres[FIELD_OVERRIDE_LIST_OV_HRES_LENGTH + 1];
    char ov_vres[FIELD_OVERRIDE_LIST_OV_VRES_LENGTH + 1];
    char ov_refresh[FIELD_OVERRIDE_LIST_OV_REFRESH_LENGTH + 1];
    char ov_pcr[FIELD_OVERRIDE_LIST_OV_PCR_LENGTH + 1];

/* The next pointer for linked list */
    struct override_list_params *next;
};

/* List iterator helpers */
extern struct override_list_params *first_override_list_entry(void);
extern struct override_list_params *next_override_list_entry(struct
                                                             override_list_params
                                                             *params);
#define foreach_override_list_entry(params) \
            for ((params) = first_override_list_entry(); \
                     (params) != NULL; (params) = next_override_list_entry((params)))

#ifdef CUSTOM_MENU_DEFS
#include "custmenu.h"
        /* Prototype for configureOverrideList command */
extern int configureOverrideList(MENUINFO * minfo);
#endif

typedef void OverrideListSavedFunction(int req_code, void *userptr);
extern void registerOverrideListSavedFunction(OverrideListSavedFunction *
                                              funcPtr);

/* Custom Validator function prototype*/
extern int validateOverrideList(int req_code,
                                struct override_list_params *params,
                                struct override_list_params *orig_params,
                                char *error_string);

        /* Prototype for applyOverrideList function */
extern int applyOverrideList(int req_code,
                             struct override_list_params *params,
                             struct override_list_params *orig_params);

/* Prototype for retrieveOverrideListValues function */
extern int retrieveOverrideListValues(HAL * hp);

/* Prototype for loadOverrideList function. */
extern int loadOverrideList(void);

/* Prototypes for CLI access entry point functions */
extern int describe_override_list_main(int argc, char **argv);
extern int get_override_list_main(int argc, char **argv);
extern int load_override_list_main(int argc, char **argv);
extern int add_override_list_main(int argc, char **argv);
extern int del_override_list_main(int argc, char **argv);
extern int mod_override_list_main(int argc, char **argv);

/* Prototypes for dumpOverrideList function */
extern int dumpOverrideList(void);
#endif                          /* __OVERRIDE_LIST_MENU_GEN_H_ */
