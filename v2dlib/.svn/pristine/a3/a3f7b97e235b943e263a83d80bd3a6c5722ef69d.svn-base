/*
 * splash.c
 *
 *  Created on: Oct 17, 2009
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include <math.h>

#include "v2d.h"
#include "v2dcommon.h"


#ifndef WINDOWS
#define DO_PNG
#endif

#ifdef DO_PNG
#include "png.h"
#endif

#include "comp_utils.h"

static void compress_frame(struct splash_frame *sf, int mquant, int dcquant,
        int slicesize, int frame_number)
{

    char *buffer, *tmpbuffer, *cp, *bp;
    int i, j;
    int len;

    LOG_SYS(V2DLIB_INFO, "Compressing Splash Screen.");

    buffer = (char *) malloc(sf->sf_hres * sf->sf_vres * 4 + 32);
    tmpbuffer = (char *) malloc(sf->sf_hres * sf->sf_vres * 4 + 32);

    if (buffer == NULL || tmpbuffer == NULL) {
        LOG_SYS(V2DLIB_ERROR, "Could not compress frame");
        return;
    }

    memset(buffer, 0x00, sf->sf_hres * sf->sf_vres * 4 + 32);
    memset(tmpbuffer, 0x00, sf->sf_hres * sf->sf_vres * 4 + 32);

    LOG_SYS(V2DLIB_VERBOSE, "Allocated memory for Splash Screen.");

    // Change RGB to BGRA
    bp = (char *) sf->sf_buffer;
    cp = tmpbuffer;
    for (i = 0; i < sf->sf_vres; i++) {
        for (j = 0; j < sf->sf_hres; j++) {

            *cp++ = *(bp + 2);
            *cp++ = *(bp + 1);
            *cp++ = *bp;
            cp++;
            bp += 3;
        }
    }

    LOG_SYS(V2DLIB_VERBOSE, "Converted raw frame.");

    len = sf->sf_len;
    initFrameParams(sf->sf_hres, sf->sf_vres, mquant, dcquant, slicesize);
    len = encodeOneFrameToFile((unsigned char *) tmpbuffer, buffer, 0);

    if (len < 0) {
        LOG_SYS(V2DLIB_ERROR, "Unable to compress frame.");
        free(buffer);
        return;
    }

    LOG_SYS(V2DLIB_VERBOSE,
            "Compressed frame from size %d to size %d", sf->sf_len, len);

    /* Round up splash screen size to nearest 1 K for fuller packets */
    len += 1024 - (len % 1024);
    LOG_SYS(V2DLIB_VERBOSE, "Rounding up compressed frame to size %d", len);

    free(sf->sf_buffer);
    free(tmpbuffer);
    sf->sf_buffer = (unsigned char*) buffer;
    sf->sf_len = len;


    memcpy(sf->sf_splash_buffer + sf->sf_splash_len, buffer, sf->sf_len);
    sf->sf_splash_len += sf->sf_len;

    LOG_SYS(V2DLIB_INFO, "Compressed Splash Screen #%d", frame_number);

}

static void finalize_frame(struct splash_frame *sf)
{
    if (sf->sf_buffer != NULL)
        free(sf->sf_buffer);
    sf->sf_buffer = sf->sf_splash_buffer;
    sf->sf_len = sf->sf_splash_len;

    /* Reduce memory usage to just what the
      compressed image needs */
    sf->sf_buffer = (unsigned char *) realloc(sf->sf_buffer, sf->sf_len + 1024);
}

#ifdef  DO_PNG
static int overlay_png(struct splash_frame *sf, FILE *pngfp, int rightjustify)
{

    png_byte png_buff[4096];
    png_structp png_ptr;
    png_infop info_ptr, end_info;
#ifdef __XPI__
    png_uint_32 width, height;
#else
    unsigned long width, height;
#endif
    int bit_depth, color_type;
    int interlace_type, compression_type, filter_method;
    png_bytep *row_pointers;
    unsigned char *rp, *cp;
    int i, j, h_offset;

    fread(png_buff, 1, 8, pngfp);

    /*********************************
     * Do nothing if file is not png *
     *********************************/
    if (png_sig_cmp(png_buff, 0, 8))
        return -1;

    png_ptr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL,
            NULL, NULL);
    if (!png_ptr)
        return -1;

    info_ptr = png_create_info_struct(png_ptr);
    if (!info_ptr) {
        png_destroy_read_struct(&png_ptr, (png_infopp) NULL, (png_infopp) NULL);
        return -1;
    }

    end_info = png_create_info_struct(png_ptr);
    if (!end_info) {
        png_destroy_read_struct(&png_ptr, &info_ptr, (png_infopp) NULL);
        return -1;
    }

    if (setjmp(png_jmpbuf(png_ptr))) {
        /*         LOG_WARNING_S("ERROR within PNG library."); */
        png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);
        return -1;
    }

    png_init_io(png_ptr, pngfp);

    png_set_sig_bytes(png_ptr, 8);

    png_read_png(png_ptr, info_ptr, PNG_TRANSFORM_EXPAND, NULL);

    png_get_IHDR(png_ptr, info_ptr, &width, &height, &bit_depth, &color_type,
            &interlace_type, &compression_type, &filter_method);

    row_pointers = png_get_rows(png_ptr, info_ptr);
#ifdef __XPI__
    LOG_SYS(V2DLIB_VERBOSE, "PNG width = %d.", (int) width);
    LOG_SYS(V2DLIB_VERBOSE, "PNG height = %d.", (int) height);
#else
    LOG_SYS(V2DLIB_VERBOSE, "PNG width = %ld.", width);
    LOG_SYS(V2DLIB_VERBOSE, "PNG height = %ld.", height);
#endif
    LOG_SYS(V2DLIB_VERBOSE, "PNG depth = %d.", bit_depth);
    LOG_SYS(V2DLIB_VERBOSE, "PNG color = %d.", color_type);
    LOG_SYS(V2DLIB_VERBOSE, "PNG interlace = %d.", interlace_type);
    LOG_SYS(V2DLIB_VERBOSE, "PNG compression = %d.", compression_type);
    LOG_SYS(V2DLIB_VERBOSE, "PNG filter = %d.", filter_method);
#ifdef __XPI__
    LOG_SYS(V2DLIB_VERBOSE, "PNG rowbytes = %d.", (int) png_get_rowbytes(png_ptr, info_ptr) );
#else
    LOG_SYS(V2DLIB_VERBOSE, "PNG rowbytes = %ld.", png_get_rowbytes(png_ptr, info_ptr) );
#endif
    /************************************
     * Overlay png file on splash frame *
     ************************************/
#define V_OFFSET    50

    h_offset = (rightjustify) ? (sf->sf_hres - (width + 50)) : 50;

    cp = sf->sf_buffer;
    for (i = 0; i < sf->sf_vres; i++) {
        if (i >= (int) V_OFFSET && i < (int) (height + V_OFFSET))
            rp = row_pointers[i - V_OFFSET];
        else
            rp = NULL;
        for (j = 0; j < sf->sf_hres; j++) {
            if (rp != NULL && j >= (int) h_offset && j < (int) (width
                    + h_offset)) {
                unsigned char rrn, ggn, bbn;
                if (color_type == 2) { /* true color */
                    rrn = *rp;
                    ggn = *(rp + 1);
                    bbn = *(rp + 2);
                    *cp++ = bbn;
                    *cp++ = ggn;
                    *cp++ = rrn;

                    rp += 3;
                }
                else if (color_type == 6) { /* true color + alpha */
                    double alpha = 1.0 - (((*(rp + 3)) * 1.0) / 255.0);
                    unsigned char rr, gg, bb;
                    //alpha = 1.0;
                    rrn = *(rp);
                    ggn = *(rp + 1);
                    bbn = *(rp + 2);
                    rr = *cp;
                    gg = *(cp + 1);
                    bb = *(cp + 2);

                    *cp++ = (unsigned char) round((rr * alpha) + rrn * (1.0
                            - alpha));
                    *cp++ = (unsigned char) round((gg * alpha) + ggn * (1.0
                            - alpha));
                    *cp++ = (unsigned char) round((bb * alpha) + bbn * (1.0
                            - alpha));

                    rp += 4;
                }
            }
            else {
                cp += 3;
            }
        }
    }

    /* Free up PNG resources */
    png_destroy_read_struct(&png_ptr, &info_ptr, &end_info);

    return 0;
}
#endif

static void add_logo(struct splash_frame *sf, int type, int frame_number)
{

    FILE *logofp = NULL, *ipvfp = NULL;

    /************************
     * Overlay IPV png file *
     ************************/
    if ((ipvfp = fopen("/V2O/config/ipv.png", "rb")) == NULL) {
        LOG_SYS(V2DLIB_WARN, "Could not open ipv png file");
    }
    else {
#ifndef WINDOWS
        overlay_png(sf, ipvfp, 0);
#endif
    }
    if (ipvfp)
        fclose(ipvfp);

    /*****************************
     * Overlay TX or RX png file *
     *****************************/
    if (type & SPLASH_TYPE_TX)
        logofp = fopen("/V2O/config/txlogo.png", "rb");
    else
        logofp = fopen("/V2O/config/rxlogo.png", "rb");

    if (!logofp) {
        LOG_SYS(V2DLIB_WARN, "Could not open logo png file.");
    }
    else {
        // Blink logo
#ifndef WINDOWS
        if (frame_number % 2 == 0)
            overlay_png(sf, logofp, 1);
#endif
    }
    if (logofp)
        fclose(logofp);

}

static void create_splash_pattern(unsigned char * splash_buffer, int vres, int hres,
        int type, int frame_number)
{
    int i, j, l;
    unsigned char k;
    unsigned char *cp;

    /*****************************************************
     * Simple bitmaps of all blue and alternating pixels *
     *****************************************************/
    cp = splash_buffer;
    for (i = 0; i < vres; i++) {
        k = 0;
        for (j = 0; j < hres; j++) {

            if (type & SPLASH_TYPE_BLUE) {
                *cp++ = 0;
                *cp++ = 0;
                *cp++ = 255;
            }
            else if (type & SPLASH_TYPE_BLACK) {
                *cp++ = 0;
                *cp++ = 0;
                *cp++ = 0;
            }
            else if (type & SPLASH_TYPE_WHITE) {
                *cp++ = 255;
                *cp++ = 255;
                *cp++ = 255;
            }
            else if (type & SPLASH_TYPE_CBAR) {
                if (j < hres / 8) {
                    if (i < vres / 2) {
                        *cp++ = 255;
                        *cp++ = 255;
                        *cp++ = 255;
                    }
                    else {
                        *cp++ = 31;
                        *cp++ = 31;
                        *cp++ = 31;
                    }
                }
                else if (j < hres / 4) {
                    if (i < vres / 2) {
                        *cp++ = 255;
                        *cp++ = 0;
                        *cp++ = 0;
                    }
                    else {
                        *cp++ = 63;
                        *cp++ = 63;
                        *cp++ = 63;
                    }
                }
                else if (j < hres / 8 * 3) {
                    if (i < vres / 2) {
                        *cp++ = 255;
                        *cp++ = 255;
                        *cp++ = 0;
                    }
                    else {
                        *cp++ = 95;
                        *cp++ = 95;
                        *cp++ = 95;
                    }
                }
                else if (j < hres / 2) {
                    if (i < vres / 2) {
                        *cp++ = 0;
                        *cp++ = 255;
                        *cp++ = 0;
                    }
                    else {
                        *cp++ = 127;
                        *cp++ = 127;
                        *cp++ = 127;
                    }
                }
                else if (j < hres / 8 * 5) {
                    if (i < vres / 2) {
                        *cp++ = 0;
                        *cp++ = 255;
                        *cp++ = 255;
                    }
                    else {
                        *cp++ = 159;
                        *cp++ = 159;
                        *cp++ = 159;
                    }
                }
                else if (j < hres / 4 * 3) {
                    if (i < vres / 2) {
                        *cp++ = 0;
                        *cp++ = 0;
                        *cp++ = 255;
                    }
                    else {
                        *cp++ = 191;
                        *cp++ = 191;
                        *cp++ = 191;
                    }
                }
                else if (j < hres / 8 * 7) {
                    if (i < vres / 2) {
                        *cp++ = 255;
                        *cp++ = 0;
                        *cp++ = 255;
                    }
                    else {
                        *cp++ = 223;
                        *cp++ = 223;
                        *cp++ = 223;
                    }
                }
                else {
                    if (i < vres / 2) {
                        *cp++ = 0;
                        *cp++ = 0;
                        *cp++ = 0;
                    }
                    else {
                        *cp++ = 255;
                        *cp++ = 255;
                        *cp++ = 255;
                    }
                }
            }
            else {
                if (i < (vres * 1 / 4))
                    l = 3;
                else if (i < (vres * 2 / 4))
                    l = 2;
                else if (i < (vres * 3 / 4))
                    l = 1;
                else
                    l = 0;

                l = l + frame_number; /* change pattern for each succeeding frame */

                l = l % 4;

                l = 1 << l;

                if (j % l == 0)
                    k = ~k;

                if (k) {
                    *cp++ = 255;
                    *cp++ = 255;
                    *cp++ = 255;
                }
                else {
                    *cp++ = 0;
                    *cp++ = 0;
                    *cp++ = 0;
                }
            }
        }
    }

    LOG_SYS(V2DLIB_VERBOSE, "Created bitmap of size %d.", cp - splash_buffer);

}

struct splash_frame *v2dCreateSplashFrame(int hres, int vres, int mquant,
        int dcquant, int slicesize, int type, int number_of_frames)
{

    struct splash_frame *sf;
    int frame_number;

    // Allocate memory for the splash frame
    sf = (struct splash_frame *) malloc(sizeof(struct splash_frame));

    if (sf == NULL) {
        LOG_SYS(V2DLIB_ERROR, "Out of memory creating splash_frame struct.");
        return NULL;
    }



    memset(sf, 0x00, sizeof(struct splash_frame));

    if (number_of_frames == 0)
        number_of_frames = 1;
    sf->sf_number_of_frames = number_of_frames;

    sf->sf_len = hres * vres * 3;
    sf->sf_buffer = (unsigned char*) malloc(sf->sf_len * 2);
    if (sf->sf_buffer == NULL) {
        LOG_SYS(V2DLIB_ERROR,
                "Out of memory creating splash bitmap of size %d.",
                sf->sf_len);
        free(sf);
        return NULL;
    }
    sf->sf_splash_buffer = (unsigned char*) malloc(sf->sf_len * 2 * number_of_frames);
    if (sf->sf_splash_buffer == NULL) {
        LOG_SYS(V2DLIB_ERROR,
                "Out of memory creating splash bitmap of size %d.",
                sf->sf_len);
        free(sf->sf_buffer);
        free(sf);
        return NULL;
    }

    sf->sf_hres = hres;
    sf->sf_vres = vres;
    sf->sf_mquant = mquant;
    sf->sf_dcquant = dcquant;
    sf->sf_slicesize = slicesize;
    sf->sf_type = type;

    LOG_SYS(V2DLIB_INFO, "Creating Splash Screen  [%dx%d].", hres, vres);


    for (frame_number = 0; frame_number < number_of_frames; frame_number++) {
        // Create the splash pattern
        create_splash_pattern(sf->sf_buffer, hres, vres, type, frame_number);

        // Add Logo to the pattern
        add_logo(sf, type, frame_number);

        // Compress (encode) the pattern buffer
        compress_frame(sf, mquant, dcquant, slicesize, frame_number);
    }
    finalize_frame(sf);

    LOG_SYS(V2DLIB_INFO, "Created  Splash Screen.");

    return sf;

}

void v2dDestroySplashFrame(struct splash_frame * sf)
{
    if (sf != NULL) {
        if (sf->sf_buffer != NULL)
            free(sf->sf_buffer);
        free(sf);
    }
}

