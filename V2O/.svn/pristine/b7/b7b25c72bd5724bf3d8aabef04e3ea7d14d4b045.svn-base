#include <sys/time.h>

#define CUSTOM_MENU_DEFS

#include "hal.h"
#include "custmenu.h"
#include "menuutils.h"
#include "commonutils.h"
#include "system_utils.h"
#include "displayWriteAudioVideoStatistics.h"


static struct fc_wparam_io fcwpio;
static int read_diff = 0;
static int timdiff = 1;
static struct fc_wparam_io fcwpio;

/*****************************
 * Print RX Video Statistics *
 *****************************/
static char *wvstathelp =
    "The Video Statistics are continuously getting updated.\n"
    "Press any  key to quit.";

#define FIELD_CPU_USAGE                 1
#define FIELD_FC_BACKLOG                5
#define FIELD_FC_MAXBACKLOG             6
#define FIELD_FC_ENQUEUE_CNT            7
#define FIELD_FC_DEQUEUE_CNT            8
#define FIELD_FC_PACKETS                9
#define FIELD_FC_THROTTLED              11
#define FIELD_FC_ERRORS                 12
#define FIELD_FC_DELIVERED              13

#define CPU_USAGE_FIELD_LEN             34
#define FC_INT_FIELD_LEN                11

static char *get_video_wbacklog(char *buf, void *usrptr, int ident)
{

    /* Get data from kernel only once */
    hal_get_fc_wparams((HAL *) usrptr, &fcwpio);

    sprintf(buf, "%ld", fcwpio.video_wbacklog);
    return buf;
}

static char *get_video_wmaxbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.video_wmaxbacklog);
    return buf;
}

static char *get_video_wenqueue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wenqueue_cnt = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.video_wenqueue_cnt - last_wenqueue_cnt) *
                 1000000) / (timdiff));
        last_wenqueue_cnt = fcwpio.video_wenqueue_cnt;
    }
    else
        sprintf(buf, "%lu", fcwpio.video_wenqueue_cnt);
    return buf;
}

static char *get_video_wdequeue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wdequeue_cnt = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.video_wdequeue_cnt - last_wdequeue_cnt) *
                 1000000) / (timdiff));
        last_wdequeue_cnt = fcwpio.video_wdequeue_cnt;
    }
    else
        sprintf(buf, "%lu", fcwpio.video_wdequeue_cnt);
    return buf;
}

static char *get_video_wpackets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.video_wpackets);
    return buf;
}

static char *get_video_werrors(char *buf, void *usrptr, int ident)
{
    static unsigned long last_werrors = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.video_werrors - last_werrors) *
                 1000000) / (timdiff));
        last_werrors = fcwpio.video_werrors;
    }
    else
        sprintf(buf, "%lu", fcwpio.video_werrors);
    return buf;
}


static char *get_video_wthrottled(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wthrottled = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.video_wthrottled - last_wthrottled) *
                 1000000) / (timdiff));
        last_wthrottled = fcwpio.video_wthrottled;
    }
    else
        sprintf(buf, "%lu", fcwpio.video_wthrottled);
    return buf;
}

static char *get_video_wdelivered(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wdelivered = 0;
    if (read_diff) {
        sprintf(buf, "%0.2f",
                ((double) (fcwpio.video_wdelivered - last_wdelivered) *
                 1000000) / (timdiff));
        last_wdelivered = fcwpio.video_wdelivered;
    }
    else
        sprintf(buf, "%lu", fcwpio.video_wdelivered);
    return buf;
}

static char *get_video_wavgbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.video_wavgbacklog);
    return buf;
}

static char *get_video_wavgpackets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.video_wavgpackets);
    return buf;
}

static char *get_video_wrate(char *buf, void *usrptr, int ident)
{
    static unsigned long last_delivered = 0;
    static struct timeval last_time;
    struct timeval now;
    int timediff;
    double rate;

    gettimeofday(&now, (struct timezone *) 0);

    timediff = TIMEVAL_DIFF(now, last_time);
    if (timediff > 1000)
        timdiff = timediff;

    rate =
        ((double) (fcwpio.video_wdelivered - last_delivered) * 8000000) /
        (timediff);

    if (rate < 1024)
        sprintf(buf, "%.2f B", rate);
    else if (rate < 1024 * 1024)
        sprintf(buf, "%.2f K", rate / 1024);
    else
        sprintf(buf, "%.2f M", rate / (1024 * 1024));

    last_time = now;
    last_delivered = fcwpio.video_wdelivered;
    return buf;
}

static FORMITEMDEF wvstatfitemdefs[] = {

    {FIELD_CPU_USAGE, "Cpu Usage:",
     "No Help Yet.",
     CPU_USAGE_FIELD_LEN, get_cpu_usage, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Curr Rate:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wrate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_BACKLOG, "Backlog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_MAXBACKLOG, "MaxBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wmaxbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_ENQUEUE_CNT, "Enqueued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wenqueue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DEQUEUE_CNT, "Dequeued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wdequeue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_PACKETS, "Packets:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wpackets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_THROTTLED, "Throttled:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wthrottled, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "Delivered:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wdelivered, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_ERRORS, "Errors:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_werrors, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "AvgPckts:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wavgpackets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "AvgBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_video_wavgbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

int displayWriteVideoStatistics(MENUINFO * minfo)
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
    form = create_form(wvstatfitemdefs, 0, 0, hp);

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
        rc = process_form(minfo, form, wvstathelp,
                          FORM_DISPLAY_ONLY | FORM_AUTO_REFRESH);

        if (rc == 'r') {
            if (read_diff)
                read_diff = 0;
            else
                read_diff = 1;
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
    read_diff = 0;
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    hal_release(hp);
    return retval;
}

/********************************
 * Print Write Audio Statistics *
 ********************************/
static char *wastathelp =
    "The Audio Statistics are continuously getting updated.\n"
    "Press any  key to quit.";

static int audiotimdiff = 1;

static char *get_audio_wbacklog(char *buf, void *usrptr, int ident)
{

    /* Get data from kernel only once */
    hal_get_fc_wparams((HAL *) usrptr, &fcwpio);

    sprintf(buf, "%ld", fcwpio.audio_wbacklog);
    return buf;
}

static char *get_audio_wmaxbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.audio_wmaxbacklog);
    return buf;
}

static char *get_audio_wenqueue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wenqueue_cnt = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.audio_wenqueue_cnt - last_wenqueue_cnt) *
                 1000000) / (audiotimdiff));
        last_wenqueue_cnt = fcwpio.audio_wenqueue_cnt;
    }
    else
        sprintf(buf, "%lu", fcwpio.audio_wenqueue_cnt);
    return buf;
}

static char *get_audio_wdequeue_cnt(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wdequeue_cnt = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.audio_wdequeue_cnt - last_wdequeue_cnt) *
                 1000000) / (audiotimdiff));
        last_wdequeue_cnt = fcwpio.audio_wdequeue_cnt;
    }
    else
        sprintf(buf, "%lu", fcwpio.audio_wdequeue_cnt);
    return buf;
}

static char *get_audio_wpackets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.audio_wpackets);
    return buf;
}

static char *get_audio_werrors(char *buf, void *usrptr, int ident)
{
    static unsigned long last_werrors = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.audio_werrors - last_werrors) *
                 1000000) / (audiotimdiff));
        last_werrors = fcwpio.audio_werrors;
    }
    else
        sprintf(buf, "%lu", fcwpio.audio_werrors);
    return buf;
}


static char *get_audio_wthrottled(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wthrottled = 0;
    if (read_diff) {
        sprintf(buf, "%0.1f",
                ((double) (fcwpio.audio_wthrottled - last_wthrottled) *
                 1000000) / (audiotimdiff));
        last_wthrottled = fcwpio.audio_wthrottled;
    }
    else
        sprintf(buf, "%lu", fcwpio.audio_wthrottled);
    return buf;
}

static char *get_audio_wdelivered(char *buf, void *usrptr, int ident)
{
    static unsigned long last_wdelivered = 0;
    if (read_diff) {
        sprintf(buf, "%0.2f",
                ((double) (fcwpio.audio_wdelivered - last_wdelivered) *
                 1000000) / (audiotimdiff));
        last_wdelivered = fcwpio.audio_wdelivered;
    }
    else
        sprintf(buf, "%lu", fcwpio.audio_wdelivered);
    return buf;
}

static char *get_audio_wavgbacklog(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.audio_wavgbacklog);
    return buf;
}

static char *get_audio_wavgpackets(char *buf, void *usrptr, int ident)
{

    sprintf(buf, "%lu", fcwpio.audio_wavgpackets);
    return buf;
}

static char *get_audio_wrate(char *buf, void *usrptr, int ident)
{
    static unsigned long last_delivered = 0;
    static struct timeval last_time;
    struct timeval now;
    int timediff;
    double rate;

    gettimeofday(&now, (struct timezone *) 0);

    timediff = TIMEVAL_DIFF(now, last_time);
    if (timediff > 1000)
        audiotimdiff = timediff;

    rate =
        ((double) (fcwpio.audio_wdelivered - last_delivered) * 8000000) /
        (timediff);

    if (rate < 1024)
        sprintf(buf, "%.2f B", rate);
    else if (rate < 1024 * 1024)
        sprintf(buf, "%.2f K", rate / 1024);
    else
        sprintf(buf, "%.2f M", rate / (1024 * 1024));

    last_time = now;
    last_delivered = fcwpio.audio_wdelivered;
    return buf;
}

static FORMITEMDEF wastatfitemdefs[] = {

    {FIELD_CPU_USAGE, "Cpu Usage:",
     "No Help Yet.",
     CPU_USAGE_FIELD_LEN, get_cpu_usage, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "Audio Rate:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wrate, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_BACKLOG, "Backlog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_MAXBACKLOG, "MaxBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wmaxbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_ENQUEUE_CNT, "Enqueued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wenqueue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DEQUEUE_CNT, "Dequeued:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wdequeue_cnt, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_PACKETS, "Packets:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wpackets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_THROTTLED, "Throttled:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wthrottled, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "Delivered:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wdelivered, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_ERRORS, "Errors:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_werrors, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {FIELD_FC_DELIVERED, "AvgPckts:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wavgpackets, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, 0},

    {FIELD_FC_DELIVERED, "AvgBckLog:",
     "No Help Yet.",
     FC_INT_FIELD_LEN, get_audio_wavgbacklog, NULL,
     FTYPE_NONE, 0, 0, O_EDIT, FFLAG_FOLLOW},

    {0, NULL, NULL,
     0, NULL, NULL,
     0, 0, 0, 0, 0}
};

int displayWriteAudioStatistics(MENUINFO * minfo)
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
    form = create_form(wastatfitemdefs, 0, 0, hp);

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
        rc = process_form(minfo, form, wastathelp,
                          FORM_DISPLAY_ONLY | FORM_AUTO_REFRESH);

        if (rc == 'r') {
            if (read_diff)
                read_diff = 0;
            else
                read_diff = 1;
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
    read_diff = 0;
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    hal_release(hp);
    return retval;
}
