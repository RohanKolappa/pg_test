
#include "strmsrv_compatibility.h"
#include "V2DUtils.hh"
#include "imagemanip.h"

// Populate SCAP with default values
int
V2DUtils::CreateDefaultScap(ScapData_t *scap)
{

    memset(scap, 0x00, sizeof(ScapData_t));
    scap->m_videoNameSet = 1;
    scap->m_hResSet = 1;
    scap->m_vResSet = 1;
    scap->m_videoRefreshSet = 1;
    scap->m_origHresSet = 1;
    scap->m_origVresSet = 1;
    scap->m_videoPcrSet = 1;
    scap->m_videoHfpSet = 1;
    scap->m_videoHbpSet = 1;
    scap->m_videoHsyncSet = 1;
    scap->m_videoVfpSet = 1;
    scap->m_videoVbpSet = 1;
    scap->m_videoVsyncSet = 1;
    scap->m_videoStereoSet = 1;
    scap->m_sliceSizeSet = 1;
    scap->m_videoCompmodeSet = 1;
    scap->m_videoMotionCompmodeSet = 1;
    scap->m_videoCodecVersionSet  = 1;

    strcpy(scap->m_videoName,"Rx Splash Frame");
    scap->m_hRes = 1280;
    scap->m_vRes = 1024;
    scap->m_videoPcr = 107.35;
    scap->m_videoHfp = 40;
    scap->m_videoHbp = 240;
    scap->m_videoHsync = 120;
    scap->m_videoVfp = 3;
    scap->m_videoVbp = 35;
    scap->m_videoVsync = 3;
    scap->m_videoRefresh = 59.999;
    scap->m_videoStereo = 0;
    scap->m_origHres = 1280;
    scap->m_origVres = 1024;
    scap->m_sliceSize = 32;
    scap->m_videoCompmode = 0;
    scap->m_videoMotionCompmode = 0;
    scap->m_videoCodecVersion = 1;
    scap->m_videoPortSet = 1;
    scap->m_videoPort = VIDEO_PORT;

    scap->m_audioPortSet = 1;
    scap->m_audioPort = scap->m_videoPort + 1;


    return 0;
}


/**********************************************************
 * Populates the LUT_struct  with the values in SCAP      *
***********************************************************/
int
V2DUtils::PopulateLutFromScap(LUT_struct *lutp, ScapData_t *scap) {

    if (scap == NULL)
        return -1;
    if (lutp == NULL)
        return -1;

    memset(lutp, 0x00, sizeof(LUT_struct));

    if (scap->m_videoNameSet) {
        strncpy(lutp->comment, scap->m_videoName, 64);
    }
    if (scap->m_videoIdSet) {
        lutp->a_alutreg =  scap->m_videoId;
    }
    if (scap->m_hResSet) {
        if(scap->m_hRes > 0) {
            lutp->a_hres = scap->m_hRes;
        }
    }
    if (scap->m_vResSet) {
        if(scap->m_vRes > 0) {
            lutp->a_vres = scap->m_vRes;
        }
    }
    if (scap->m_videoPcrSet) {
        if(scap->m_videoPcr > 0) {
            lutp->a_pcr = scap->m_videoPcr;
        }
    }
    if (scap->m_videoHfpSet) {
        if(scap->m_videoHfp > 0) {
            lutp->a_hfp = scap->m_videoHfp;
        }
    }
    if (scap->m_videoHbpSet) {
        if(scap->m_videoHbp > 0) {
            lutp->a_hbp = scap->m_videoHbp;
        }
    }
    if (scap->m_videoHsyncSet) {
        if(scap->m_videoHsync > 0) {
            lutp->a_hsync = scap->m_videoHsync;
        }
    }
    if (scap->m_videoVfpSet) {
        if(scap->m_videoVfp > 0) {
            lutp->a_vfp = scap->m_videoVfp;
        }
    }
    if (scap->m_videoVbpSet) {
        if(scap->m_videoVbp > 0) {
            lutp->a_vbp = scap->m_videoVbp;
        }
    }
    if (scap->m_videoVsyncSet) {
        if(scap->m_videoVsync > 0) {
            lutp->a_vsync = scap->m_videoVsync;
        }
    }

    if (scap->m_videoRefreshSet) {
        if(scap->m_videoRefresh > 0) {
            lutp->a_refresh = scap->m_videoRefresh;
        }
    }
    if (scap->m_videoStereoSet) {
        if (scap->m_videoStereo > 0) {
            lutp->a_stereo = scap->m_videoStereo;
        }
    }
    if (scap->m_origHresSet) {
        lutp->a_orig_hres = (scap->m_origHres == 0) ? lutp->a_hres : scap->m_origHres;
    }
    if (scap->m_origVresSet) {
        lutp->a_orig_vres = (scap->m_origVres == 0) ? lutp->a_vres : scap->m_origVres;
    }
   return 0;
}


void
V2DUtils::CreateLutEntry(int orig_width, int orig_height, LUT_struct * pLUT) {

    int encoded_width = 0;
    int encoded_height = 0;

    int slices_per_frame;
    int slice_size;
    V2DLib::computeSliceSize(orig_width, orig_height, slice_size,
            encoded_width, encoded_height, slices_per_frame);

    memset(pLUT, 0x00, sizeof(LUT_struct));
    // 800 x 600
    if (encoded_width == 800 && encoded_height == 600) {
        pLUT->a_hres = 800;
        pLUT->a_vres = 600;
        pLUT->a_refresh = 85.06;
        pLUT->a_pcr = 56.25;
        pLUT->a_hfp = 32;
        pLUT->a_hbp = 152;
        pLUT->a_hsync = 64;
        pLUT->a_vfp = 1;
        pLUT->a_vbp = 27;
        pLUT->a_vsync = 3;
        pLUT->a_alutreg = 11;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "VESA %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    // 1024 x 768
    if (encoded_width == 1024 && encoded_height == 768) {
        pLUT->a_hres = 1024;
        pLUT->a_vres = 768;
#ifdef HI_REFRESH
        pLUT->a_refresh = 85.0;
        pLUT->a_pcr = 94.5;
        pLUT->a_hfp = 48;
        pLUT->a_hbp = 208;
        pLUT->a_hsync = 96;
        pLUT->a_vfp = 1;
        pLUT->a_vbp = 36;
        pLUT->a_vsync = 3;
        pLUT->a_alutreg = 15;
#else
        pLUT->a_refresh = 60.0;
        pLUT->a_pcr = 65.0;
        pLUT->a_hfp = 24;
        pLUT->a_hbp = 160;
        pLUT->a_hsync = 136;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 29;
        pLUT->a_vsync = 6;
        pLUT->a_alutreg = 12;
#endif
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "VESA %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    // 1280 x 1024
    if (encoded_width == 1280 && encoded_height == 1024) {
        pLUT->a_hres = 1280;
        pLUT->a_vres = 1024;
#ifdef HI_REFRESH
        pLUT->a_refresh = 75.03;
        pLUT->a_pcr = 135.00;
        pLUT->a_hfp = 16;
        pLUT->a_hbp = 248;
        pLUT->a_hsync = 144;
        pLUT->a_vfp = 1;
        pLUT->a_vbp = 38;
        pLUT->a_vsync = 3;
        pLUT->a_alutreg = 19;
#else
        pLUT->a_refresh = 60.02;
        pLUT->a_pcr = 108.00;
        pLUT->a_hfp = 48;
        pLUT->a_hbp = 248;
        pLUT->a_hsync = 112;
        pLUT->a_vfp = 1;
        pLUT->a_vbp = 38;
        pLUT->a_vsync = 3;
        pLUT->a_alutreg = 18;
#endif
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "VESA %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    // 1600 x 1200
    if (encoded_width == 1600 && encoded_height == 1200) {
        pLUT->a_hres = 1600;
        pLUT->a_vres = 1200;
        pLUT->a_refresh = 60.00;
        pLUT->a_pcr = 162.00;
        pLUT->a_hfp = 64;
        pLUT->a_hbp = 304;
        pLUT->a_hsync = 192;
        pLUT->a_vfp = 1;
        pLUT->a_vbp = 46;
        pLUT->a_vsync = 3;
        pLUT->a_alutreg = 21;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "VESA %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    // 1920 x 1080
    if (encoded_width == 1920 && encoded_height == 1080) {
        pLUT->a_hres = 1920;
        pLUT->a_vres = 1080;
        pLUT->a_refresh = 60.00;
        pLUT->a_pcr = 148.50;
        pLUT->a_hfp = 88;
        pLUT->a_hbp = 144;
        pLUT->a_hsync = 48;
        pLUT->a_vfp = 4;
        pLUT->a_vbp = 36;
        pLUT->a_vsync = 5;
        pLUT->a_alutreg = 22;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "CEA-861 %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    if (encoded_width == 1920 && encoded_height == 1200) {
        pLUT->a_hres = 1920;
        pLUT->a_vres = 1200;
        pLUT->a_refresh = 59.95;
        pLUT->a_pcr = 154.00;
        pLUT->a_hfp = 48;
        pLUT->a_hbp = 80;
        pLUT->a_hsync = 32;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 26;
        pLUT->a_vsync = 6;
        pLUT->a_alutreg = 23;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "CVT-RB %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    if (encoded_width == 2560 && encoded_height == 1600) {
        pLUT->a_hres = 2560;
        pLUT->a_vres = 1600;
        pLUT->a_refresh = 59.972;
        pLUT->a_pcr = 268.50;
        pLUT->a_hfp = 48;
        pLUT->a_hbp = 80;
        pLUT->a_hsync = 32;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 37;
        pLUT->a_vsync = 6;
        pLUT->a_alutreg = 24;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "CVT-RB %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    if (encoded_width == 3840 && encoded_height == 1080) {
        pLUT->a_hres = 3840;
        pLUT->a_vres = 1080;
        pLUT->a_refresh = 59.93;
        pLUT->a_pcr = 277.00;
        pLUT->a_hfp = 64;
        pLUT->a_hbp = 192;
        pLUT->a_hsync = 64;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 19;
        pLUT->a_vsync = 9;
        pLUT->a_alutreg = 25;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "MTX0511 %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    if (encoded_width == 3840 && encoded_height == 1200) {
        pLUT->a_hres = 3840;
        pLUT->a_vres = 1200;
        pLUT->a_refresh = 59.95;
        pLUT->a_pcr = 308.00;
        pLUT->a_hfp = 64;
        pLUT->a_hbp = 192;
        pLUT->a_hsync = 64;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 22;
        pLUT->a_vsync = 10;
        pLUT->a_alutreg = 26;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "MTX0511 %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    if (encoded_width == 3360 && encoded_height == 1056) {
        pLUT->a_hres = 3360;
        pLUT->a_vres = 1056;
        pLUT->a_refresh = 59.88;
        pLUT->a_pcr = 238.00;
        pLUT->a_hfp = 64;
        pLUT->a_hbp = 192;
        pLUT->a_hsync = 64;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 19;
        pLUT->a_vsync = 8;
        pLUT->a_alutreg = 27;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "MTX0511 %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }

    if (encoded_width == 2048 && encoded_height == 1152) {
        pLUT->a_hres = 2048;
        pLUT->a_vres = 1152;
        pLUT->a_refresh = 59.909;
        pLUT->a_pcr = 156.75;
        pLUT->a_hfp = 48;
        pLUT->a_hbp = 80;
        pLUT->a_hsync = 32;
        pLUT->a_vfp = 3;
        pLUT->a_vbp = 25;
        pLUT->a_vsync = 5;
        pLUT->a_alutreg = 28;
        pLUT->a_orig_hres = orig_width;
        pLUT->a_orig_vres = orig_height;
        sprintf(pLUT->comment, "MAC %dx%d@%4.1fHz",
                orig_width, orig_height, pLUT->a_refresh);
        return;
    }


    // If we reach here, it is not one our pre-configured resolutions,
    // so we have to compute an LUT
    float pixel_clock, refresh_rate;
    int hfp, hbp, hsync;
    int vfp, vbp, vsync;
    V2DLib::computeVideoTimings(encoded_width, encoded_height,
            refresh_rate, pixel_clock,
            hfp, hbp, hsync,
            vfp, vbp, vsync);

    pLUT->a_hres = encoded_width;
    pLUT->a_vres = encoded_height;
    pLUT->a_refresh = refresh_rate;
    pLUT->a_pcr = pixel_clock;
    pLUT->a_hfp = hfp;
    pLUT->a_hbp = hbp;
    pLUT->a_hsync = hsync;
    pLUT->a_vfp = vfp;
    pLUT->a_vbp = vbp;
    pLUT->a_vsync = vsync;
    pLUT->a_alutreg = 40;
    pLUT->a_orig_hres = orig_width;
    pLUT->a_orig_vres = orig_height;
    sprintf(pLUT->comment, "CVT %dx%d@%4.1fHz",
            orig_width, orig_height, pLUT->a_refresh);

}
