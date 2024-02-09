#ifndef __STEP_TIMER_H_
#define __STEP_TIMER_H_

#include <sys/time.h>


struct step_timer {
    struct timeval start_time;
    struct timeval end_time;
    int duration;
    int done;
    int sleep_time;
    double scale;
};


extern void step_timer_init(struct step_timer * step_timer, int duration);
extern int step_timer_check(struct step_timer *step_timer);
extern double step_timer_get_scale(struct step_timer *step_timer);


#endif
