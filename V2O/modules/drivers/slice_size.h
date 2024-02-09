#ifndef __SLICE_SIZE_H__
#define __SLICE_SIZE_H__

#ifdef __cplusplus
extern "C" {
#endif
extern int compute_slice_size(int width, int height, int * slice_size,
        int * encoded_width, int * encoded_height);
extern void compute_svc_slice_size(int width, int height, int *slice_size, int *slice_width,
        int *slice_height, int * encoded_width, int * encoded_height);
extern int compute_i50_slice_size(int width, int height, int * slice_size,
        int * encoded_width, int * encoded_height);
#ifdef __cplusplus
}
#endif

#endif
