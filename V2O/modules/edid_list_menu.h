#ifndef __EDID_LIST_MENU_H_
#define __EDID_LIST_MENU_H_
#include <stdio.h>
#include "mylist.h"

#define MAX_EDID_DESC_COUNT                   3

#define MAX_EDID_LUT_COUNT                    8

#define FIELD_EDID_LIST_ALUTREG_LENGTH        4
#define FIELD_EDID_LIST_EDID_DESC_LENGTH      2
#define FIELD_EDID_LIST_EDID_HPOL_LENGTH      4
#define FIELD_EDID_LIST_EDID_VPOL_LENGTH      4
#define FIELD_EDID_LIST_NAME_LENGTH           30
#define FIELD_EDID_LIST_STEREO_LENGTH         4 
#define FIELD_EDID_LIST_HRES_LENGTH           5
#define FIELD_EDID_LIST_VRES_LENGTH           5
#define FIELD_EDID_LIST_REFRESH_LENGTH        5
#define FIELD_EDID_LIST_PCR_LENGTH            6
#define FIELD_EDID_LIST_HFP_LENGTH            5
#define FIELD_EDID_LIST_HSYNC_LENGTH          5
#define FIELD_EDID_LIST_HBP_LENGTH            5
#define FIELD_EDID_LIST_VFP_LENGTH            5
#define FIELD_EDID_LIST_VSYNC_LENGTH          5
#define FIELD_EDID_LIST_VBP_LENGTH            5

/* Client implement this function to register all customization hooks */
void EdidDescriptorListInit(void);

enum edid_list_edid_hpol {
    EDID_LIST_EDID_HPOL_POS,
    EDID_LIST_EDID_HPOL_NEG,
};

enum edid_list_edid_vpol {
    EDID_LIST_EDID_VPOL_POS,
    EDID_LIST_EDID_VPOL_NEG,
};

struct edid_list_params {
    int edid_list_id;

/* Clients use these members to access the parameter values */
    int edid_alutreg_value;
    int edid_desc_value;
    enum edid_list_edid_hpol edid_hpol_value;
    enum edid_list_edid_vpol edid_vpol_value;

/* Clients use these members to check if a parameter value has been set */
    int edid_alutreg_set;
    int edid_desc_set;
    int edid_hpol_set;
    int edid_vpol_set;

/* Generic user pointer used for various purposes (e.g. to pass HAL ptr) */
    void *userptr;

/* Internal memembers not to be used by client */
    char edid_alutreg[FIELD_EDID_LIST_ALUTREG_LENGTH + 1];
    char edid_desc[FIELD_EDID_LIST_EDID_DESC_LENGTH + 1];
    char edid_hpol[FIELD_EDID_LIST_EDID_HPOL_LENGTH + 1];
    char edid_vpol[FIELD_EDID_LIST_EDID_VPOL_LENGTH + 1];

/* The next pointer for linked list */
    struct edid_list_params *next;
};

/* List iterator helpers */
extern struct edid_list_params *first_edid_list_entry(void);
extern struct edid_list_params *next_edid_list_entry(struct
                                                     edid_list_params
                                                     *params);
#define foreach_edid_list_entry(params) \
            for ((params) = first_edid_list_entry(); \
                     (params) != NULL; (params) = next_edid_list_entry((params)))

#ifdef CUSTOM_MENU_DEFS
#include "custmenu.h"
        /* Prototype for configureEdidDescriptorList command */
extern int configureEdidDescriptorList(MENUINFO * minfo);
extern int configureEdidVideoInput(MENUINFO * minfo);
extern int configureEdidFromLoopbackMonitor(MENUINFO * minfo);
#endif

/* Custom Validator function prototype*/
typedef void EdidDescriptorListSavedFunction(int req_code);

extern void
registerEdidDescriptorListSavedFunction(EdidDescriptorListSavedFunction *
                                        funcPtr);

/* Prototype for applyEdidDescriptorList function */
extern int validateEdidDescriptorList(int req_code,
                                      struct edid_list_params *params,
                                      struct edid_list_params *orig_params,
                                      char *error_string);

        /* Prototype for applyEdidDescriptorList function */
extern int applyEdidDescriptorList(int req_code,
                                   struct edid_list_params *params,
                                   struct edid_list_params *orig_params);

/* Prototype for retrieveEdidDescriptorListValues function */
extern int retrieveEdidDescriptorListValues(HAL * hp);

/* Prototype for constructRowEdidDescriptorList function */
extern char *constructRowEdidDescriptorList(char *buffer,
                                            struct edid_list_params
                                            *params);

/* Prototype for constructHeaderEdidDescriptorList function */
extern char *constructHeaderEdidDescriptorList(char *buffer);

/* Prototype for loadEdidDescriptorList function. */
extern int loadEdidDescriptorList(void);

/* Prototypes for CLI access entry point functions */
extern int describe_edid_list_main(int argc, char **argv);
extern int get_edid_list_main(int argc, char **argv);
extern int load_edid_list_main(int argc, char **argv);
extern int add_edid_list_main(int argc, char **argv);
extern int del_edid_list_main(int argc, char **argv);
extern int mod_edid_list_main(int argc, char **argv);

/* Prototypes for dumpEdidDescriptorList function */
extern int dumpEdidDescriptorList(void);

#endif                          /* __EDID_LIST_MENU_H_ */
