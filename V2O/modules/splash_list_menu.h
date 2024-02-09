#ifndef __SPLASH_LIST_MENU_GEN_H_
#define __SPLASH_LIST_MENU_GEN_H_
#include <stdio.h>
#include "mylist.h"

#define FIELD_SPLASH_LIST_NAME_LENGTH      32
#define FIELD_SPLASH_LIST_ALUTREG_LENGTH      4
#define FIELD_SPLASH_LIST_STEREO_LENGTH      4
#define FIELD_SPLASH_LIST_ORIG_HRES_LENGTH      5
#define FIELD_SPLASH_LIST_ORIG_VRES_LENGTH      5
#define FIELD_SPLASH_LIST_REFRESH_LENGTH      13
#define FIELD_SPLASH_LIST_PCR_LENGTH      12
#define FIELD_SPLASH_LIST_HFP_LENGTH      5
#define FIELD_SPLASH_LIST_VFP_LENGTH      5
#define FIELD_SPLASH_LIST_HBP_LENGTH      5
#define FIELD_SPLASH_LIST_VBP_LENGTH      5
#define FIELD_SPLASH_LIST_HSYNC_LENGTH      5
#define FIELD_SPLASH_LIST_VSYNC_LENGTH      5
#define FIELD_SPLASH_LIST_PHASE_LENGTH      4
#define FIELD_SPLASH_LIST_VMARGIN_LENGTH      2
#define FIELD_SPLASH_LIST_HSHIFT_LENGTH      5
#define FIELD_SPLASH_LIST_VSHIFT_LENGTH      4
#define FIELD_SPLASH_LIST_CNOISE_LENGTH      4
#define FIELD_SPLASH_LIST_FNOISE_LENGTH      5
#define FIELD_SPLASH_LIST_CSPACE_LENGTH      6
#define FIELD_SPLASH_LIST_CPMP_LENGTH      5
#define FIELD_SPLASH_LIST_RGAIN_LENGTH      4
#define FIELD_SPLASH_LIST_ROFFSET_LENGTH      4
#define FIELD_SPLASH_LIST_GGAIN_LENGTH      4
#define FIELD_SPLASH_LIST_GOFFSET_LENGTH      4
#define FIELD_SPLASH_LIST_BGAIN_LENGTH      4
#define FIELD_SPLASH_LIST_BOFFSET_LENGTH      4

/* Client implement this function to register all customization hooks */
void SplashListInit(void);

enum splash_list_stereo {
    SPLASH_LIST_STEREO_YES,
    SPLASH_LIST_STEREO_NO,
    SPLASH_LIST_STEREO_BOTH,
};

enum splash_list_cspace {
    SPLASH_LIST_CSPACE_RGB,
    SPLASH_LIST_CSPACE_YPRPB,
};

enum splash_list_cpmp {
    SPLASH_LIST_CPMP_AUTO,
    SPLASH_LIST_CPMP_50,
    SPLASH_LIST_CPMP_100,
    SPLASH_LIST_CPMP_150,
    SPLASH_LIST_CPMP_250,
    SPLASH_LIST_CPMP_350,
    SPLASH_LIST_CPMP_500,
    SPLASH_LIST_CPMP_750,
    SPLASH_LIST_CPMP_1500,
};

/* enum for stereo */

/* enum for cspace */

/* enum for cpmp */

struct splash_list_params {
    int splash_list_id;

/* Clients use these members to access the parameter values */
    char name_value[FIELD_SPLASH_LIST_NAME_LENGTH + 1];
    int alutreg_value;
    enum splash_list_stereo stereo_value;
    int orig_hres_value;
    int orig_vres_value;
    double refresh_value;
    double pcr_value;
    int hfp_value;
    int vfp_value;
    int hbp_value;
    int vbp_value;
    int hsync_value;
    int vsync_value;
    int phase_value;
    int vmargin_value;
    int hshift_value;
    int vshift_value;
    int cnoise_value;
    int fnoise_value;
    enum splash_list_cspace cspace_value;
    enum splash_list_cpmp cpmp_value;
    int rgain_value;
    int roffset_value;
    int ggain_value;
    int goffset_value;
    int bgain_value;
    int boffset_value;

/* Clients use these members to check if a parameter value has been set */
    int name_set;
    int alutreg_set;
    int stereo_set;
    int orig_hres_set;
    int orig_vres_set;
    int refresh_set;
    int pcr_set;
    int hfp_set;
    int vfp_set;
    int hbp_set;
    int vbp_set;
    int hsync_set;
    int vsync_set;
    int phase_set;
    int vmargin_set;
    int hshift_set;
    int vshift_set;
    int cnoise_set;
    int fnoise_set;
    int cspace_set;
    int cpmp_set;
    int rgain_set;
    int roffset_set;
    int ggain_set;
    int goffset_set;
    int bgain_set;
    int boffset_set;

/* Generic user pointer used for various purposes (e.g. to pass HAL ptr) */
    void *userptr;

/* Internal members not to be used by client */
    char name[FIELD_SPLASH_LIST_NAME_LENGTH + 1];
    char alutreg[FIELD_SPLASH_LIST_ALUTREG_LENGTH + 1];
    char stereo[FIELD_SPLASH_LIST_STEREO_LENGTH + 1];
    char orig_hres[FIELD_SPLASH_LIST_ORIG_HRES_LENGTH + 1];
    char orig_vres[FIELD_SPLASH_LIST_ORIG_VRES_LENGTH + 1];
    char refresh[FIELD_SPLASH_LIST_REFRESH_LENGTH + 1];
    char pcr[FIELD_SPLASH_LIST_PCR_LENGTH + 1];
    char hfp[FIELD_SPLASH_LIST_HFP_LENGTH + 1];
    char vfp[FIELD_SPLASH_LIST_VFP_LENGTH + 1];
    char hbp[FIELD_SPLASH_LIST_HBP_LENGTH + 1];
    char vbp[FIELD_SPLASH_LIST_VBP_LENGTH + 1];
    char hsync[FIELD_SPLASH_LIST_HSYNC_LENGTH + 1];
    char vsync[FIELD_SPLASH_LIST_VSYNC_LENGTH + 1];
    char phase[FIELD_SPLASH_LIST_PHASE_LENGTH + 1];
    char vmargin[FIELD_SPLASH_LIST_VMARGIN_LENGTH + 1];
    char hshift[FIELD_SPLASH_LIST_HSHIFT_LENGTH + 1];
    char vshift[FIELD_SPLASH_LIST_VSHIFT_LENGTH + 1];
    char cnoise[FIELD_SPLASH_LIST_CNOISE_LENGTH + 1];
    char fnoise[FIELD_SPLASH_LIST_FNOISE_LENGTH + 1];
    char cspace[FIELD_SPLASH_LIST_CSPACE_LENGTH + 1];
    char cpmp[FIELD_SPLASH_LIST_CPMP_LENGTH + 1];
    char rgain[FIELD_SPLASH_LIST_RGAIN_LENGTH + 1];
    char roffset[FIELD_SPLASH_LIST_ROFFSET_LENGTH + 1];
    char ggain[FIELD_SPLASH_LIST_GGAIN_LENGTH + 1];
    char goffset[FIELD_SPLASH_LIST_GOFFSET_LENGTH + 1];
    char bgain[FIELD_SPLASH_LIST_BGAIN_LENGTH + 1];
    char boffset[FIELD_SPLASH_LIST_BOFFSET_LENGTH + 1];

/* The next pointer for linked list */
    struct splash_list_params *next;
};

/* List iterator helpers */
extern struct splash_list_params *first_splash_list_entry(void);
extern struct splash_list_params *next_splash_list_entry(struct
                                                         splash_list_params
                                                         *params);
#define foreach_splash_list_entry(params) \
            for ((params) = first_splash_list_entry(); \
                     (params) != NULL; (params) = next_splash_list_entry((params)))

#ifdef CUSTOM_MENU_DEFS
#include "custmenu.h"
        /* Prototype for configureSplashList command */
extern int configureSplashList(MENUINFO * minfo);
#endif

typedef void SplashListSavedFunction(int req_code, void *userptr);
extern void registerSplashListSavedFunction(SplashListSavedFunction *
                                            funcPtr);

/* Custom paramater default initialization function prototype */
typedef void SplashListParamsDefaultsInitFunction(struct splash_list_params
                                                  *params);

extern void
registerSplashListParamsDefaultsInitFunction
(SplashListParamsDefaultsInitFunction * funcPtr);

/* Prototype for applySplashList function */
extern int validateSplashList(int req_code,
                              struct splash_list_params *params,
                              struct splash_list_params *orig_params,
                              char *error_string);

        /* Prototype for applySplashList function */
extern int applySplashList(int req_code,
                           struct splash_list_params *params,
                           struct splash_list_params *orig_params);

/* Prototype for retrieveSplashListValues function */
extern int retrieveSplashListValues(HAL * hp);

/* Prototype for countSplashListValues function */
extern int countSplashListValues(void);

/* Prototype for constructRowSplashList function */
extern char *constructRowSplashList(char *buffer,
                                    struct splash_list_params *params);

/* Prototype for constructHeaderSplashList function */
extern char *constructHeaderSplashList(char *buffer);

/* Prototype for loadSplashList function. */
extern int loadSplashList(void);

/* Prototypes for CLI access entry point functions */
extern int get_splash_list_main(int argc, char **argv);
extern int load_splash_list_main(int argc, char **argv);
extern int add_splash_list_main(int argc, char **argv);
extern int del_splash_list_main(int argc, char **argv);
extern int mod_splash_list_main(int argc, char **argv);

/* Prototypes for dumpSplashList function */
extern int dumpSplashList(void);
#endif                          /* __SPLASH_LIST_MENU_GEN_H_ */
