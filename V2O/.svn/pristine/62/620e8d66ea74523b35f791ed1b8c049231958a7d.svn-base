#ifndef __CONFIGURE_VIDEO_H__
#define __CONFIGURE_VIDEO_H__



#ifdef CUSTOM_MENU_DEFS
extern int configureVideoParameters(MENUINFO *minfo);
extern int legacy_configureRxVideoParameters(MENUINFO *minfo);
extern int configureConnectionParameters(MENUINFO *minfo);
extern int configureAuthList(MENUINFO *minfo);
extern int disconnectClients(MENUINFO *minfo);
#endif

extern void initVideoCompression(HAL *hp);
extern void initPhaseShift(HAL *hp);
extern void initFrameDivider(HAL *hp);
extern void initHorizPos(HAL *hp);
extern void initVertPos(HAL *hp);
extern void initCoarseNoise(HAL *hp);
extern void initFineNoise(HAL *hp);
extern void initPixelAdjust(HAL *hp);
extern void initDropPhaseAdjust(HAL *hp);
extern void initGenlockMode(HAL *hp);
extern int getLocalLUT(void);

extern int dump_auth_table(void);
extern int dump_tx_vid_parm(void);
#endif  /* __CONFIGURE_VIDEO_H */

