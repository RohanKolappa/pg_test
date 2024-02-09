/*
 * png.cpp
 *
 *  Created on: Nov 8, 2009
 *      Author: rkale
 */

#include "v2dlib_compatibility.h"
#include <math.h>

#include "png.hh"

CPng::CPng(const char *a_FileName)
{
#ifndef WINDOWS
    png_byte png_buff[4096];

    // Open png file
    m_pFP = fopen(a_FileName, "rb");
    m_bError = false;
    m_nWidth = 0;
    m_nHeight = 0;

    if (m_pFP == NULL) {
        printf("Could not open png file %s:\n",
                a_FileName);
        m_bError = true;
        return;
    }


    fread(png_buff, 1, 8, m_pFP);

    // Do nothing if not png
    if (png_sig_cmp(png_buff, 0, 8)) {
        m_bError = true;
        return;
    }

    m_pPngPtr = png_create_read_struct(PNG_LIBPNG_VER_STRING, (png_voidp) NULL,
            NULL, NULL);
    if (m_pPngPtr == NULL) {
        m_bError = true;
        return;
    }

    m_pInfoPtr = png_create_info_struct(m_pPngPtr);
    if (!m_pInfoPtr) {
        png_destroy_read_struct(&m_pPngPtr, (png_infopp) NULL,
                (png_infopp) NULL);
        m_bError = true;
        return;
    }

    m_pEndInfo = png_create_info_struct(m_pPngPtr);
    if (!m_pEndInfo) {
        png_destroy_read_struct(&m_pPngPtr, &m_pInfoPtr, (png_infopp) NULL);
        m_bError = true;
        return;
    }

    if (setjmp(png_jmpbuf(m_pPngPtr))) {
        /*         LOG_WARNING_S("ERROR within PNG library."); */
        png_destroy_read_struct(&m_pPngPtr, &m_pInfoPtr, &m_pEndInfo);
        m_bError = true;
        return;
    }


    png_init_io(m_pPngPtr, m_pFP);

    png_set_sig_bytes(m_pPngPtr, 8);

    png_read_png(m_pPngPtr, m_pInfoPtr, PNG_TRANSFORM_EXPAND, NULL);

    png_get_IHDR(m_pPngPtr, m_pInfoPtr, &m_nWidth, &m_nHeight, &m_nBitDepth,
            &m_nColorType, &m_nInterlaceType, &m_nCompressionType,
            &m_nFilterMethod);

#else
    m_bError = true;
#endif
}

CPng::~CPng()
{
#ifndef WINDOWS
    if (!m_bError)
        png_destroy_read_struct(&m_pPngPtr, &m_pInfoPtr, &m_pEndInfo);

    if (m_pFP != NULL)
        fclose(m_pFP);
#endif
}

void CPng::DisplayInfo()
{
#ifndef WINDOWS 
#ifdef __XPI__
    printf("PNG width = %d.\n", (int) m_nWidth);
    printf("PNG height = %d.\n", (int) m_nHeight);
#else
    printf("PNG width = %ld.\n", m_nWidth);
    printf("PNG height = %ld.\n", m_nHeight);
#endif
    printf("PNG depth = %d.\n", m_nBitDepth);
    printf("PNG color = %d.\n", m_nColorType);
    printf("PNG interlace = %d.\n", m_nInterlaceType);
    printf("PNG compression = %d.\n", m_nCompressionType);
    printf("PNG filter = %d.\n", m_nFilterMethod);
#ifdef __XPI__ 
    printf("PNG rowbytes = %d.\n", (int) png_get_rowbytes(m_pPngPtr, m_pInfoPtr));
#else
    printf("PNG rowbytes = %ld.\n", png_get_rowbytes(m_pPngPtr, m_pInfoPtr));
#endif
#endif
}

int CPng::Position(unsigned char *inBuff, int frame_width, int frame_height,
        int v_offset, int v_extent, int h_offset, int h_extent)
{
    if (m_bError)
        return -1;
#ifndef WINDOWS

    int bytes_per_pixel;
    if (m_nBitDepth == 16)
        bytes_per_pixel = 2;
    else if (m_nBitDepth == 8)
        bytes_per_pixel = 1;
    else
        return -1;

    png_bytep *row_pointers = png_get_rows(m_pPngPtr, m_pInfoPtr);
    unsigned char * cp = inBuff;
    unsigned char * rp;
    for (int i = 0; i < frame_height; i++) {
        if (i >= v_offset && i < v_extent)
            rp = row_pointers[i - v_offset];
        else
            rp = NULL;
        for (int j = 0; j < frame_width; j++) {
            if (rp != NULL && j >= h_offset && j < h_extent) {
                // printf("Adding png pixels at %d %d for color_type %d\n", i, j, color_type);
                unsigned char rrn, ggn, bbn;
                if (m_nColorType == 2) { /* true color */
                    rrn = *rp;
                    ggn = *(rp + (1 * bytes_per_pixel));
                    bbn = *(rp + (2 * bytes_per_pixel));
                    *cp++ = bbn;
                    *cp++ = ggn;
                    *cp++ = rrn;
                    cp++;
                    rp += 3 * bytes_per_pixel;
                }
                else if (m_nColorType == 6) { /* true color + alpha */
                    double transparency = 1.0
                            - (((*(rp + (3 * bytes_per_pixel))) * 1.0) / 255.0);
                    unsigned char rr, gg, bb;
                    //alpha = 0.0;
                    //printf("alpha = %f", alpha);
                    rrn = *(rp);
                    ggn = *(rp + (1 * bytes_per_pixel));
                    bbn = *(rp + (2 * bytes_per_pixel));
                    rr = *cp;
                    gg = *(cp + 1);
                    bb = *(cp + 2);

                    *cp++ = (unsigned char) round((bb * transparency) + bbn
                            * (1.0 - transparency));
                    *cp++ = (unsigned char) round((gg * transparency) + ggn
                            * (1.0 - transparency));
                    *cp++ = (unsigned char) round((rr * transparency) + rrn
                            * (1.0 - transparency));
                    cp++;

                    rp += 4 * bytes_per_pixel;
                }

            }
            else {
                cp += 4;
            }
        }
    }
#endif
    return 0;
}

int CPng::Overlay(unsigned char *inBuff, int frame_width, int frame_height)
{
    if (m_bError)
        return -1;

    // Center logo on image
    int h_offset = frame_width / 2 - m_nWidth / 2;
    int h_extent = frame_width / 2 + m_nWidth / 2;
    int v_offset = frame_height / 2 - m_nHeight / 2;
    int v_extent = frame_height / 2 + m_nHeight / 2;

    Position(inBuff, frame_width,frame_height,
             v_offset, v_extent, h_offset, h_extent);

    return 0;
}

int CPng::Tile(unsigned char *inBuff, int frame_width, int frame_height)
{
    if (m_bError)
        return -1;
#ifndef WINDOWS

    // Tile logo on image
    for (int v_offset = 0; v_offset < frame_height; v_offset += m_nHeight) {
        int v_extent = v_offset + m_nHeight;
        for (int h_offset = 0; h_offset < frame_width; h_offset += m_nWidth) {
            int h_extent = h_offset + m_nWidth;

            Position(inBuff, frame_width,frame_height,
                    v_offset, v_extent, h_offset, h_extent);
        }
    }
#endif
    return 0;
}

int CPng::GetBGRA(unsigned char *inBuff)
{
    if (m_bError)
        return -1;
#ifndef WINDOWS

    int bytes_per_pixel;
    if (m_nBitDepth == 16)
        bytes_per_pixel = 2;
    else if (m_nBitDepth == 8)
        bytes_per_pixel = 1;
    else
        return -1;

    png_bytep *row_pointers = png_get_rows(m_pPngPtr, m_pInfoPtr);

    unsigned char * cp = inBuff;
    unsigned char * rp;
    for (int j = 0; j < (int) m_nHeight; j++) {
        rp = row_pointers[j];
        for (int i = 0; i < (int) m_nWidth; i++) {

            unsigned char rrn, ggn, bbn;
            if (m_nColorType == 2) { /* true color */
                rrn = *rp;
                ggn = *(rp + (1 * bytes_per_pixel));
                bbn = *(rp + (2 * bytes_per_pixel));
                *cp++ = bbn;
                *cp++ = ggn;
                *cp++ = rrn;
                cp++;
                rp += 3 * bytes_per_pixel;
            }
            else if (m_nColorType == 6) { /* true color + alpha */
                double transparency = 1.0 - (((*(rp + (3 * bytes_per_pixel)))
                        * 1.0) / 255.0);
                unsigned char rr, gg, bb;

                rrn = *(rp);
                ggn = *(rp + (1 * bytes_per_pixel));
                bbn = *(rp + (2 * bytes_per_pixel));
                bb = *cp;
                gg = *(cp + 1);
                rr = *(cp + 2);

                *cp++ = (unsigned char) round((bb * transparency) + bbn * (1.0
                        - transparency));
                *cp++ = (unsigned char) round((gg * transparency) + ggn * (1.0
                        - transparency));
                *cp++ = (unsigned char) round((rr * transparency) + rrn * (1.0
                        - transparency));
                cp++;

                rp += 4 * bytes_per_pixel;
            }

        }

    }
#endif
    return 0;
}
