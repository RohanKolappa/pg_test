#include <stdlib.h>
#include <string.h>
#include <sys/time.h>
#include <unistd.h>

#include "fpga.h"
#include "step_timer.h"


// Initialize the step_timer object
void step_timer_init(struct step_timer * step_timer, int duration) {

    memset(step_timer, 0x00, sizeof(struct step_timer));
    
    step_timer->duration = duration;

    // Determine end time
    gettimeofday(&step_timer->start_time, (struct timezone *) 0);
    step_timer->end_time.tv_sec = step_timer->start_time.tv_sec + 
        (step_timer->duration/1000) ;
    step_timer->end_time.tv_usec = 
        step_timer->start_time.tv_usec + 
        ((step_timer->duration % 1000) * 1000);
    if (step_timer->end_time.tv_usec > 1000000) {
        step_timer->end_time.tv_usec -= 1000000;
        step_timer->end_time.tv_sec++;
    }
 
    // Determine how long we need to sleep every round to have
    // aproximately 100 intervals
    step_timer->sleep_time =  (1.0 * duration * 10);
    if (step_timer->sleep_time < 1000)
        step_timer->sleep_time = 1000;
}

// Check the time and returns true if the duration has elapsed
// Computes scale as a factor of the duration that has elapsed
int step_timer_check(struct step_timer *step_timer) {


    if (step_timer->done)
        return 0;

    // Have we past the end time??
    struct timeval now;
    gettimeofday(&now, (struct timezone *) 0);

    if (now.tv_sec > step_timer->end_time.tv_sec) {
        step_timer->done = 1;
    }
    if ((now.tv_sec == step_timer->end_time.tv_sec) &&
        (now.tv_usec > step_timer->end_time.tv_usec)) {
        step_timer->done = 1;
    }

    if (step_timer->done) {
        step_timer->scale = 1.0;
        return 1;
    }
    
    // Compute scale
    int timediff = TIMEVAL_DIFF(now, step_timer->start_time); // in usec
    timediff = timediff/1000; // in msec
    step_timer->scale = (double)(1.0 * timediff)/step_timer->duration;

    // Sleep before returning
    usleep(step_timer->sleep_time);
    
    return 1;
}

double step_timer_get_scale(struct step_timer *step_timer) {
    return step_timer->scale;
}
