/*
 * IPVSMediaPlugin.cpp
 *
 *  Created on: Sep 16, 2011
 *      Author: rkale
 */

#include "IPVSTranscode_compatibility.h"

#include <string>

#include "avisynth_c.h"
#include "V2DFrameServer.hh"
#include "V2DAudioFrameServer.hh"

struct IPVSPluginData {
    CV2DFrameServer *m_pV2DFrameServer;
    CV2DAudioFrameServer *m_pV2DAudioFrameServer;
};


static AVS_VideoFrame * AVSC_CC GetFrame(AVS_FilterInfo * p, int n)
{
    AVS_VideoFrame * dest;
    int row_size, height, dest_pitch;
    BYTE * dest_data;
    int y, x;

    dest = avs_new_video_frame(p->env, &p->vi);

    dest_data = avs_get_write_ptr(dest);
    dest_pitch = avs_get_pitch(dest);

    row_size = avs_get_row_size(dest);
    height = avs_get_height(dest);

    IPVSPluginData *pd = static_cast<IPVSPluginData *>(p->user_data);
    int src_pitch = pd->m_pV2DFrameServer->GetFrameWidth() * 4;
    unsigned char * video_frame_data = pd->m_pV2DFrameServer->GetVideoFrame(n);
    if (video_frame_data != NULL) {
        avs_bit_blt(p->env,
                dest_data, dest_pitch,
                video_frame_data + (src_pitch * (height - 1)), -src_pitch,
                row_size, height);


    }
    else {
        // Create a black frame.
        for (y = 0; y < height; y++) {
            for (x = 0; x < row_size; x++) {
                dest_data[x] = 0;
            }
            dest_data += dest_pitch;
        }
    }
    return dest;
}

static void AVSC_CC FreeFilter(AVS_FilterInfo * p)
{
    // Free our resources
    IPVSPluginData *pd = static_cast<IPVSPluginData *>(p->user_data);
    delete pd->m_pV2DFrameServer;
    delete pd->m_pV2DAudioFrameServer;
    delete pd;
}

static void AVSC_CC Destroy_Plugin_Instance(void * unused, AVS_ScriptEnvironment * env)
{
    // No need to do anything here yet.
}

static int AVSC_CC GetAudioFrame (AVS_FilterInfo *p, void * buf,
                                  INT64 start, INT64 count)
{
	IPVSPluginData *pd = static_cast<IPVSPluginData *>(p->user_data);

	pd->m_pV2DAudioFrameServer->GetSamples((uint8_t*)buf, start, count);

    return 0;
}

static AVS_Value AVSC_CC Create_Plugin_Instance(AVS_ScriptEnvironment * env, AVS_Value args,
              void * unused)
{
    AVS_Value v;
    AVS_FilterInfo * fi;
    AVS_Clip * new_clip = avs_new_c_filter(env, &fi, avs_void, 0);

    //v = avs_new_value_error("IPVS Media Source not yet implemented");
    avs_set_field_based(&fi->vi, 0);
    fi->vi.width = 0;
    fi->vi.height = 0;
    fi->vi.num_frames = 0;
    fi->vi.pixel_type = AVS_CS_BGR32;

    fi->get_frame = GetFrame;
    fi->free_filter = FreeFilter;

    // Initialize plugin's opaque user_data with our call back structure
    IPVSPluginData *pd = new IPVSPluginData;
    fi->user_data = pd;

    //std::string sourcePath("v2d_1hrclock_media");
    std::string sourcePath(avs_as_string(avs_array_elt(args, 0)));
    int fps = 30;
    if (avs_defined(avs_array_elt(args, 1))) {
        fps = avs_as_int(avs_array_elt(args, 1));
    }
    if (fps <= 0 || fps > 120) {
        v = avs_new_value_error("FPS out of range (should be between 1 and 120)");
        avs_release_clip(new_clip);
        return v;
    }
    avs_set_fps(&fi->vi, fps, 1);

    int forceWidth = 0;
    int forceHeight = 0;
    int bForceDimension = 0;
    int bDebugVideo = 0;
    if (avs_defined(avs_array_elt(args, 2)) && avs_defined(avs_array_elt(args, 3))) {
        bForceDimension = 1;
        forceWidth = avs_as_int(avs_array_elt(args, 2));
        forceHeight = avs_as_int(avs_array_elt(args, 3));
        if (forceWidth <= 0 || forceWidth > 8192 ||
                forceHeight <= 0 || forceHeight > 8192) {
            v = avs_new_value_error("Incorrect width/height specified");
            avs_release_clip(new_clip);
            return v;
        }
    }
    if (avs_defined(avs_array_elt(args, 4))) {
        bDebugVideo = avs_as_int(avs_array_elt(args, 4));
    }
    pd->m_pV2DFrameServer =
            new CV2DFrameServer(sourcePath, fps);
    if (bDebugVideo) {
        pd->m_pV2DFrameServer->EnableDebug();
    }
    int rc;
    if (bForceDimension)
        rc = pd->m_pV2DFrameServer->Initialize(forceWidth, forceHeight);
    else
        rc = pd->m_pV2DFrameServer->Initialize();
    if (rc != FS_SUCCESS) {
        v = avs_new_value_error(pd->m_pV2DFrameServer->GetErrorString().c_str());
        avs_release_clip(new_clip);
        return v;
    }
    fi->vi.width = pd->m_pV2DFrameServer->GetFrameWidth();
    fi->vi.height = pd->m_pV2DFrameServer->GetFrameHeight();
    fi->vi.num_frames = pd->m_pV2DFrameServer->GetVideoFrameCount();

    pd->m_pV2DAudioFrameServer =
              new CV2DAudioFrameServer(sourcePath);
    rc = pd->m_pV2DAudioFrameServer->Initialize();
    if (rc != FS_SUCCESS) {
        v = avs_new_value_error(pd->m_pV2DAudioFrameServer->GetErrorString().c_str());
        avs_release_clip(new_clip);
        return v;
    }
    if (pd->m_pV2DAudioFrameServer->HasAudio())
    {
        fi->get_audio = GetAudioFrame;

        int nAudioBitsPerSample = pd->m_pV2DAudioFrameServer->GetBitsPerSample();
        if (nAudioBitsPerSample == 16)
            fi->vi.sample_type =AVS_SAMPLE_INT16 ;
        else if (nAudioBitsPerSample == 8 )
            fi->vi.sample_type =AVS_SAMPLE_INT8 ;

        fi->vi.audio_samples_per_second = pd->m_pV2DAudioFrameServer->GetFrequency();
        fi->vi.nchannels = pd->m_pV2DAudioFrameServer->GetChannels();
        fi->vi.num_audio_samples = pd->m_pV2DAudioFrameServer->GetSampleCount();
    }
    else
    	fi->vi.audio_samples_per_second = 0;

    // Return a reference to the clip and release our clip's handle
    v = avs_new_value_clip(new_clip);
    avs_release_clip(new_clip);

    return v;
}

const char * avisynth_c_plugin_init(AVS_ScriptEnvironment * env)
{
    avs_add_function(env, "IPVSMediaSource", "s[fps]i[width]i[height]i[debugvideo]i", Create_Plugin_Instance, 0);
    avs_at_exit(env, Destroy_Plugin_Instance, 0);
    return "IPVS Media Source Plugin";
}



