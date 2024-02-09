#ifndef __COMP_UTILS_H__
#define __COMP_UTILS_H__

#ifdef V2DLIBRARY_EXPORTS
  #define DLLEXPORT __declspec(dllexport)
#else
  #define DLLEXPORT
#endif

#ifdef __cplusplus
extern "C" {
#endif
DLLEXPORT void initFrameParams(int a_frameWidth, int a_frameHeight, int mquant, int dcquant, int sclicesize);
DLLEXPORT int encodeOneFrameToFile(unsigned char *a_bitmapArray,
			   char *a_outFp, int a_frameNo);
DLLEXPORT int  encodeOneSlice(unsigned char * a_bitmapArray,
                    char * a_outFp);
#ifdef __cplusplus
}
#endif

#endif
