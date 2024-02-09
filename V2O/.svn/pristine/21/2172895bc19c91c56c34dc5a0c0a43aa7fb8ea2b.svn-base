
#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <netinet/in.h>
#include <signal.h>
#include <time.h>
#include <getopt.h>
#include <sys/types.h>
#include <regex.h>
#include <strings.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "hal_LED.h"
#include "custmenu.h"
#include "menuutils.h"
#include "displayAudioVideoStatistics.h"
#include "lut_table.h"
#include "commonutils.h"
#include "system_utils.h"


/*****************************
 * Print TX Video Statistics *
 *****************************/
static char *vstathelp =
    "The System Information is continuously getting updated.\n"
    "Press </U>R<!U> to toggle counts display, </U>P<!U> to toggle compression\n"
    "ratio display, </U>C<!U> to clear error counts or any other key to quit.";


static struct kbm_param_io kbmpio;
static int read_diff = 0;
static int read_percent = 0;
static int timdiff = 1;
static struct LUT_struct current_alut;
static int current_nobpslice = 128;
static unsigned long slice_diff, frame_diff;
static unsigned long byte_diff = 0;
static unsigned long audio_byte_diff = 0;

#define FIELD_CPU_USAGE                 1
#define FIELD_FC_RATE                   2
#define FIELD_FC_LIMIT                  3
#define FIELD_FC_BURST                  4
#define FIELD_FC_BACKLOG                5
#define FIELD_FC_MAXBACKLOG             6
#define FIELD_FC_ENQUEUE_CNT            7
#define FIELD_FC_DEQUEUE_CNT            8
#define FIELD_FC_PACKETS                9
#define FIELD_FC_TOKENS                 10
#define FIELD_FC_THROTTLED              11
#define FIELD_FC_ERRORS                 12
#define FIELD_FC_DELIVERED              13
#define FIELD_FC_CLIENT                 14
#define FIELD_FC_STATUS                 15

#define CPU_USAGE_FIELD_LEN             34
#define COMPRESSION_FIELD_LEN           34
#define FC_INT_FIELD_LEN                11
#define CLIENT_FIELD_LEN                32
#define SERVER_STATUS_FIELD_LEN         32

static struct fc_param_io fcpio;

static void incrementRate(HAL * hp, int howmuch)
{

    hal_get_fc_params(hp, &fcpio);

    fcpio.setmask = FC_SET_PARAM_RATE;
    fcpio.fc_rate += howmuch;
    hal_set_fc_params(hp, &fcpio);
}

static void decrementRate(HAL * hp, int howmuch)
{

    hal_get_fc_params(hp, &fcpio);

    fcpio.setmask = FC_SET_PARAM_RATE;
    if (fcpio.fc_rate > howmuch) {
        fcpio.fc_rate -= howmuch;
        hal_set_fc_params(hp, &fcpio);
    }
}


static char *get_fc_rate(char *buf, void *usrptr, int ident)
{
    double rate;
    int diff = 512 * 1024;
    HAL *hp = (HAL *) usrptr;

    /* Get data from kernel only once */
    hal_get_fc_params(hp, &fcpio);

    hal_get_addside_lut_k(hp, &current_alut);
    hal_get_addside_nobpslice(hp, &current_nobpslice);
    if (current_nobpslice <= 0)
        current_nobpslice = 128;

    if (fcpio.fc_flags & FC_FLAGS_VIDEO_ONLY)
        diff = 0;
    if (fcpio.fc_flags & FC_FLAGS_AUDIO_MONO)
        diff /= 2;
    if (fcpio.fc_flags & FC_FLAGS_AUDIO_8BIT)
        diff /= 2;

    rate = (double) fcpio.fc_rate * 8 - (double) (512 * 1024 - diff);

    if (rate < 1024)
        sprintf(buf, "%.2f B", rate);
    else if (rate < 1024 * 1024)
        sprintf(buf, "%.2f K", rate / 1024);
    else
        sprintf(buf, "%.2f M", rate / (1024 * 1024));

    return buf;
}

static char *get_fc_limit(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.fc_limit);
    return buf;
}

static char *get_fc_burst(char *buf, void *usrptr, int ident)
{

    if (!read_diff) {
        double rate =
            ((double) fcpio.fc_peakrate * fcpio.fc_rate * 8.0) / 100.0;
        if ((rate / (1024 * 1024)) > 500.0)
            rate = 500.0 * 1024 * 1024;

        if (rate < 1024)
            sprintf(buf, "%.1fB %lu", rate, fcpio.fc_burst);
        else if (rate < 1024 * 1024)
            sprintf(buf, "%.1fK %lu", rate / 1024, fcpio.fc_burst);
        else
            sprintf(buf, "%.1fM %lu", rate / (1024 * 1024),
                    fcpio.fc_burst);
    }
    else
        sprintf(buf, "%lu", fcpio.fc_buffer);
    return buf;
}

static char *get_video_backlog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%ld", fcpio.video_backlog);
    return buf;
}

static char *get_video_maxbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.video_maxbacklog);
    return buf;
}

static char *get_video_enqueue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_enqueue_cnt = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.video_enqueue_cnt - last_enqueue_cnt) *
                 1000000) / (timdiff));
        last_enqueue_cnt = fcpio.video_enqueue_cnt;
    }
    else
        sprintf(buf, "%lu", fcpio.video_enqueue_cnt);
    return buf;
}

static char *get_video_dequeue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_dequeue_cnt = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.video_dequeue_cnt - last_dequeue_cnt) *
                 1000000) / (timdiff));
        last_dequeue_cnt = fcpio.video_dequeue_cnt;
    }
    else
        sprintf(buf, "%lu", fcpio.video_dequeue_cnt);
    return buf;
}

static char *get_video_packets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.video_packets);
    return buf;
}

static char *get_video_errors(char *buf, void *usrptr, int ident)
{
    static unsigned long last_errors = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.video_errors - last_errors) * 1000000) /
                (timdiff));
        last_errors = fcpio.video_errors;
    }
    else
        sprintf(buf, "%lu", fcpio.video_errors);
    return buf;
}

static char *get_fc_tokens(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.fc_tokens);
    return buf;
}

static char *get_video_throttled(char *buf, void *usrptr, int ident)
{
    static unsigned long last_throttled = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.video_throttled - last_throttled) *
                 1000000) / (timdiff));
        last_throttled = fcpio.video_throttled;
    }
    else
        sprintf(buf, "%lu", fcpio.video_throttled);
    return buf;
}

static char *get_video_delivered(char *buf, void *usrptr, int ident)
{
    if (read_diff) {
        sprintf(buf, "%0.2f",
                ((double) (byte_diff) * 1000000) / (timdiff));
    }
    else
        sprintf(buf, "%lu", fcpio.video_delivered);

    return buf;
}

static char *get_video_avgbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.video_avgbacklog);
    return buf;
}

static char *get_video_avgpackets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.video_avgpackets);
    return buf;
}

static char *get_video_frame_cnt(char *buf, void *usrptr, int ident)
{
    if (read_diff) {
        sprintf(buf, "%0.2f",
                (((double) frame_diff) / timdiff) * 1000000.0);
    }
    else
        sprintf(buf, "%lu", fcpio.frame_cnt);
    return buf;
}

static char *get_video_slice_cnt(char *buf, void *usrptr, int ident)
{
    if (read_diff) {
        sprintf(buf, "%0.2f",
                (((double) slice_diff) / timdiff) * 1000000.0);
    }
    else
        sprintf(buf, "%lu", fcpio.slice_cnt);
    return buf;
}

static char mybuf[40];

static char *get_video_compression(char *buf, void *usrptr, int ident)
{
    double xval, wval, tval, fval, pval, sval, cval;

    xval = (double) ((current_alut.a_orig_hres +
                      current_alut.a_hfp +
                      current_alut.a_hbp +
                      current_alut.a_hsync) *
                     (current_alut.a_orig_vres +
                      current_alut.a_vfp +
                      current_alut.a_vbp + current_alut.a_vsync) * 3);

    wval =
        (double) (current_alut.a_orig_hres * current_alut.a_orig_vres * 3);


    if (byte_diff > 0 && wval > 0) {
        pval = xval / wval;
        fval =
            current_alut.a_refresh / ((((double) frame_diff) / timdiff) *
                                      1000000.0);
        sval =
            ((wval / (double) (64 * current_nobpslice * 3)) * frame_diff) /
            slice_diff;
        cval =
            (((double) (64 * current_nobpslice * 3)) / byte_diff) *
            slice_diff;

        tval =
            (xval / byte_diff) * ((double) timdiff / 1000000.0) *
            current_alut.a_refresh;

        if (read_percent) {
            pval = ((double) 1.0 - (double) 1.0 / pval) * 100.0;
            fval = ((double) 1.0 - (double) 1.0 / fval) * 100.0;
            sval = ((double) 1.0 - (double) 1.0 / sval) * 100.0;
            cval = ((double) 1.0 - (double) 1.0 / cval) * 100.0;
            tval = ((double) 1.0 - (double) 1.0 / tval) * 100.0;
            sprintf(buf, "Pch %5.2f%% Frm %5.2f%% Slc %5.2f%%", pval, fval,
                    sval);
            sprintf(mybuf, "Real %5.2f%%     Total %.2f %%", cval, tval);

        }
        else {
            sprintf(buf, "Pch %5.1f  Frm %5.1f  Slc %5.1f", pval, fval,
                    sval);
            sprintf(mybuf, "Real %5.1f     Total %.1f", cval, tval);
        }
    }
    else {
        sprintf(buf, "????");
        sprintf(mybuf, "????");
    }

    return buf;
}

static char *get_video_compression2(char *buf, void *usrptr, int ident)
{

    strcpy(buf, mybuf);

    return buf;
}

static char *get_video_rate(char *buf, void *usrptr, int ident)
{
    static unsigned long last_delivered = 0;
    static struct timeval last_time;
    static unsigned long last_slice_cnt = 0;
    static unsigned long last_frame_cnt = 0;
    struct timeval now;
    int timediff;
    double rate;

    gettimeofday(&now, (struct timezone *) 0);

    timediff = TIMEVAL_DIFF(now, last_time);
    if (timediff > 1000)
        timdiff = timediff;

    frame_diff = fcpio.frame_cnt - last_frame_cnt;
    slice_diff = fcpio.slice_cnt - last_slice_cnt;
    byte_diff = fcpio.video_delivered - last_delivered;

    /* (8 * 1000 * 1000)/(1024 * 1024) = 7.62939453 */
    rate = ((double) byte_diff * 1000000 * 8) / (timediff);
    if (rate < 1024)
        sprintf(buf, "%.2f B", rate);
    else if (rate < 1024 * 1024)
        sprintf(buf, "%.2f K", rate / 1024);
    else
        sprintf(buf, "%.2f M", rate / (1024 * 1024));

    last_slice_cnt = fcpio.slice_cnt;
    last_frame_cnt = fcpio.frame_cnt;
    last_delivered = fcpio.video_delivered;

    if (fcpio.fc_flags & FC_FLAGS_AUDIO_ONLY)
        sprintf(buf, "0.0 B");

    last_time = now;
    return buf;
}

static char *get_kb_bytes(char *buf, void *usrptr, int ident)
{

    /* Get data from kernel only once */
    hal_get_kbm_params((HAL *) usrptr, &kbmpio);

    sprintf(buf, "%lu", kbmpio.kb_rbytes);
    return buf;
}

static char *get_ms_bytes(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", kbmpio.ms_rbytes);
    return buf;
}

static char *get_kb_dropped(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", kbmpio.kb_rdropped);
    return buf;
}

static char *get_ms_dropped(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", kbmpio.ms_rdropped);
    return buf;
}

static FORMITEMDEF vstatfitemdefs[] = {
    {FIELD_CPU_USAGE, "Cpu Usage:",
     "No Help Yet.",
     CPU_USAGE_FIELD_LEN, get_cpu_usage, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_RATE, "BW:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_rate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Video BW:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_rate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_LIMIT, "Limit:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_limit, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_BURST, "Burst:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_burst, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_BACKLOG, "Backlog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_backlog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_MAXBACKLOG, "MaxBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_maxbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_ENQUEUE_CNT, "Enqueued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_enqueue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DEQUEUE_CNT, "Dequeued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_dequeue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_PACKETS, "Packets:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_packets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_TOKENS, "Tokens:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_tokens, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_THROTTLED, "Throttled:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_throttled, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_ERRORS, "Errors:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_errors, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "Frames:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_frame_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Slices:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_slice_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "Compression:",
     "No Help Yet.",
     COMPRESSION_FIELD_LEN, get_video_compression, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Compression:",
     "No Help Yet.",
     COMPRESSION_FIELD_LEN, get_video_compression2, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

static FORMITEMDEF v2statfitemdefs[] = {

    {FIELD_CPU_USAGE, "Cpu Usage:",
     "No Help Yet.",
     CPU_USAGE_FIELD_LEN, get_cpu_usage, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_RATE, "Rate:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_rate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Video Rate:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_rate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "AvgPckts:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_avgpackets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "AvgBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_avgbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "Delivered:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_delivered, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Keyboard:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_kb_bytes, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Mouse:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_ms_bytes, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "KB Dropped:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_kb_dropped, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "MS Dropped:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_ms_dropped, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

int displayVideoStatistics(MENUINFO * minfo)
{

    FORM *form = NULL;
    int rc = E_OK;
    int retval = RC_NORMAL;
    int form_number = 0;
    FORMITEMDEF *current_form;
    HAL *hp = NULL;

    /********************
     * Print Menu title *
     ********************/
  again:
    show_title(minfo, NULL);

    if (hp == NULL)
        hp = hal_acquire(0);
    if (hp == NULL) {
        return show_fault_dialog(minfo, "Missing Hardware!\n");
    }


    /*******************
     * Create the form *
     *******************/
    if (form_number == 0)
        current_form = vstatfitemdefs;
    else
        current_form = v2statfitemdefs;

    form = create_form(current_form, 0, 0, hp);

    /**********************************
     * Associate windows for the form *
     **********************************/
    set_form_windows(minfo, form);

    /*****************
     * Post the form *
     *****************/
    post_form(form);

    /*************************
     * Process user requests *
     *************************/
    while (1) {
        rc = process_form(minfo, form, vstathelp,
                          FORM_DISPLAY_ONLY | FORM_AUTO_REFRESH);

        if (rc == 'i') {
            incrementRate(hp, 10240 / 8);
            continue;
        }

        else if (rc == 'I') {
            incrementRate(hp, 1024 * 1024 / 8);
            continue;
        }

        else if (rc == 0x09) {  /* CTRL-I shows as a tab */
            incrementRate(hp, 102400 / 8);
            continue;
        }

        else if (rc == 'd') {
            decrementRate(hp, 10240 / 8);
            continue;
        }

        else if (rc == 'D') {
            decrementRate(hp, 1024 * 1024 / 8);
            continue;
        }

        else if (rc == '') {
            decrementRate(hp, 102400 / 8);
            continue;
        }

        else if (rc == 'r') {
            if (read_diff)
                read_diff = 0;
            else
                read_diff = 1;
            continue;
        }
        else if (rc == 'p') {
            if (read_percent)
                read_percent = 0;
            else
                read_percent = 1;
            continue;
        }
        else if (rc == 'm') {

            if (form_number == 0)
                form_number = 1;
            else
                form_number = 0;

            unpost_form(form);
            unset_form_windows(form);
            delete_form(form);
            goto again;
        }
        else if (rc == 'c') {
            /* Signal dclient to reset error counts in shared memory */
            system_command("/usr/bin/killall -USR1 dclient");
            continue;
        }

        if (rc == RC_EXIT)
            retval = RC_EXIT;
        else
            retval = RC_NORMAL;
        break;
    }

    /************
     * clean-up *
     ************/
    read_diff = read_percent = 0;
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    hal_release(hp);
    return retval;
}

/**************************
 * Print Audio Statistics *
 **************************/
static char *astathelp =
    "The System Information is continuously getting updated.\n"
    "Press </U>R<!U> to toggle counts display, </U>C<!C> to clear\n"
    "error counts or any other key to quit.";

static int audio_diff = 0;
static int audiotimdiff = 1;

static char *get_audio_rate(char *buf, void *usrptr, int ident)
{
    static unsigned long last_delivered = 0;
    static struct timeval last_time;
    struct timeval now;
    int timediff;
    double rate;
    HAL *hp = (HAL *) usrptr;

    /* Get data from kernel only once */
    hal_get_fc_params(hp, &fcpio);

    gettimeofday(&now, (struct timezone *) 0);

    timediff = TIMEVAL_DIFF(now, last_time);
    if (timediff > 1000)
        audiotimdiff = timediff;

    audio_byte_diff = fcpio.audio_delivered - last_delivered;

    rate = ((double) audio_byte_diff * 1000000 * 8) / (timediff);
    if (rate < 1024)
        sprintf(buf, "%.2f B", rate);
    else if (rate < 1024 * 1024)
        sprintf(buf, "%.2f K", rate / 1024);
    else
        sprintf(buf, "%.2f M", rate / (1024 * 1024));

    last_delivered = fcpio.audio_delivered;

    last_time = now;
    return buf;
}

static char *get_audio_backlog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%ld", fcpio.audio_backlog);
    return buf;
}

static char *get_audio_maxbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.audio_maxbacklog);
    return buf;
}

static char *get_audio_enqueue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_enqueue_cnt = 0;
    if (audio_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.audio_enqueue_cnt - last_enqueue_cnt) *
                 1000000) / (audiotimdiff));
        last_enqueue_cnt = fcpio.audio_enqueue_cnt;
    }
    else
        sprintf(buf, "%lu", fcpio.audio_enqueue_cnt);
    return buf;
}

static char *get_audio_dequeue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_dequeue_cnt = 0;
    if (audio_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.audio_dequeue_cnt - last_dequeue_cnt) *
                 1000000) / (audiotimdiff));
        last_dequeue_cnt = fcpio.audio_dequeue_cnt;
    }
    else
        sprintf(buf, "%lu", fcpio.audio_dequeue_cnt);
    return buf;
}

static char *get_audio_packets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.audio_packets);
    return buf;
}

static char *get_audio_errors(char *buf, void *usrptr, int ident)
{
    static unsigned long last_errors = 0;
    if (audio_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.audio_errors - last_errors) * 1000000) /
                (audiotimdiff));
        last_errors = fcpio.audio_errors;
    }
    else
        sprintf(buf, "%lu", fcpio.audio_errors);
    return buf;
}

static char *get_audio_throttled(char *buf, void *usrptr, int ident)
{
    static unsigned long last_throttled = 0;
    if (audio_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcpio.audio_throttled - last_throttled) *
                 1000000) / (audiotimdiff));
        last_throttled = fcpio.audio_throttled;
    }
    else
        sprintf(buf, "%lu", fcpio.audio_throttled);
    return buf;
}

static char *get_audio_avgbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.audio_avgbacklog);
    return buf;
}

static char *get_audio_avgpackets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcpio.audio_avgpackets);
    return buf;
}

static FORMITEMDEF astatfitemdefs[] = {

    {FIELD_CPU_USAGE, "Cpu Usage",
     "No Help Yet.",
     CPU_USAGE_FIELD_LEN, get_cpu_usage, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_RATE, "BW:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_rate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Audio BW",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_rate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_BACKLOG, "Backlog",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_backlog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_MAXBACKLOG, "MaxBckLog",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_maxbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_ENQUEUE_CNT, "Enqueued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_enqueue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DEQUEUE_CNT, "Dequeued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_dequeue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_PACKETS, "Packets:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_packets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_TOKENS, "Tokens:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_fc_tokens, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_THROTTLED, "Throttled:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_throttled, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_ERRORS, "Errors:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_errors, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "AvgPckts:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_avgpackets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "AvgBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_avgbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

int displayAudioStatistics(MENUINFO * minfo)
{

    FORM *form = NULL;
    int rc = E_OK;
    int retval = RC_NORMAL;
    HAL *hp;

    /********************
     * Print Menu title *
     ********************/
    show_title(minfo, NULL);

    hp = hal_acquire(0);
    if (hp == NULL) {
        return show_fault_dialog(minfo, "Missing Hardware!\n");
    }

    /*******************
     * Create the form *
     *******************/
    form = create_form(astatfitemdefs, 0, 0, hp);

    /**********************************
     * Associate windows for the form *
     **********************************/
    set_form_windows(minfo, form);

    /*****************
     * Post the form *
     *****************/
    post_form(form);

    /*************************
     * Process user requests *
     *************************/
    while (1) {
        rc = process_form(minfo, form, astathelp,
                          FORM_DISPLAY_ONLY | FORM_AUTO_REFRESH);

        if (rc == 'i') {
            incrementRate(hp, 10240 / 8);
            continue;
        }

        else if (rc == 'I') {
            incrementRate(hp, 1024 * 1024 / 8);
            continue;
        }

        else if (rc == 0x09) {  /* CTRL-I shows as a tab */
            incrementRate(hp, 102400 / 8);
            continue;
        }

        else if (rc == 'd') {
            decrementRate(hp, 10240 / 8);
            continue;
        }

        else if (rc == 'D') {
            decrementRate(hp, 1024 * 1024 / 8);
            continue;
        }

        else if (rc == '') {
            decrementRate(hp, 102400 / 8);
            continue;
        }

        else if (rc == 'r') {
            if (audio_diff)
                audio_diff = 0;
            else
                audio_diff = 1;
            continue;
        }
        else if (rc == 'c') {
            /* Signal dclient to reset error counts in shared memory */
            system_command("/usr/bin/killall -USR1 dclient");
            continue;
        }

        if (rc == RC_EXIT)
            retval = RC_EXIT;
        else
            retval = RC_NORMAL;
        break;
    }

    /************
     * clean-up *
     ************/
    audio_diff = 0;
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    hal_release(hp);
    return retval;
}
