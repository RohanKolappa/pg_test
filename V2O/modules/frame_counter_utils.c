#include <stdio.h>
#include <string.h>
#include <stdlib.h>
#include <stdarg.h>
#include <unistd.h>
#include <termios.h>
#include <fcntl.h>
#include <sys/types.h>
#include <sys/ioctl.h>
#include <sys/stat.h>
#include <ctype.h>
#include <errno.h>
#include <time.h>
#include <syslog.h>
    
#include "plx9056pciif.h"
#include "hal.h"
#include "hal_hwinfo.h"
#include "hal_LED.h"
#include "vlog.h"
#include "commonutils.h"
#include "vlib.h"
#include "tx_kbm_config.h"
#include "frame_counter_utils.h"
#include "rx_kbm_config.h"
#include "led_control.h"
#include "device_detection.h"
#include "display_config.h"

/***************************
 * Read Frame Counter FIFO *
 ***************************/
int hal_read_framecnt_fifo(HAL *hp, 
                           unsigned char *buf, 
                           int sampling_interval) {
    int i, fifolevel=0;
    unsigned short cnt[8];

    if (hp ==NULL || hp->board_number >= MAX_FPGA_BOARDS || 
            sampling_interval < 0)
        return -1;

    /* Reset frame counter FIFO */
    hal_or_in_fpga_val_k(hp, FPGA_FRMCNT_REG, FRMCNT_RESET_BIT);

    /* Enable the FPGA to dump data to FIFO */
    hal_or_in_fpga_val_k(hp, FPGA_FRMCNT_REG, FRMCNT_DUMP_BIT);

    /* Remove reset */
    hal_and_in_fpga_val_k(hp, FPGA_FRMCNT_REG, ~FRMCNT_RESET_BIT);

    /* Get frame counter FIFO level  */
    for (i = 0; i < sampling_interval; i++) {
        sleep(1);
        fifolevel = hal_get_fpga_val_k(hp, FPGA_FRMCNT_LVL);
    }

    /* Read frame counter FIFO  */
    VLOG_INFO_CAT(VLOG_VLIB, "Frame counter FIFO level = %d", fifolevel);
    for (i = 0; i < fifolevel; i++) {
        hal_or_in_fpga_val_k(hp, FPGA_FRMCNT_REG, FRMCNT_READ_BIT);
        *((unsigned long *) &buf[4*i]) = 
            hal_get_fpga_val_k(hp, FPGA_FRMCNT_REG);
        cnt[i % 8] = (*((unsigned long *) &buf[4*i]) & 0x000ffff0) >> 4;
        if (i % 8 == 7) {
            VLOG_INFO_CAT(VLOG_VLIB, 
                    " %05d %05d %05d %05d %05d %05d %05d %05d",
                    cnt[0], cnt[1], cnt[2], cnt[3], 
                    cnt[4], cnt[5], cnt[6], cnt[7]);
        } 
        hal_and_in_fpga_val_k(hp, FPGA_FRMCNT_REG, ~FRMCNT_READ_BIT);
    }

    if (fifolevel % 8 == 1)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d", cnt[0]);
    else if (fifolevel % 8 == 2)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d %05d", cnt[0], cnt[1]);
    else if (fifolevel % 8 == 3)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d %05d %05d", cnt[0], cnt[1], cnt[2]);
    else if (fifolevel % 8 == 4)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d %05d %05d %05d", 
                cnt[0], cnt[1], cnt[2], cnt[3]);
    else if (fifolevel % 8 == 5)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d %05d %05d %05d %05d", 
                cnt[0], cnt[1], cnt[2], cnt[3], cnt[4]);
    else if (fifolevel % 8 == 6)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d %05d %05d %05d %05d %05d", 
                cnt[0], cnt[1], cnt[2], cnt[3], cnt[4], cnt[5]);
    else if (fifolevel % 8 == 7)
        VLOG_INFO_CAT(VLOG_VLIB, " %05d %05d %05d %05d %05d %05d %05d", 
                cnt[0], cnt[1], cnt[2], cnt[3], cnt[4], cnt[5], cnt[6]);

    return fifolevel;
}


#define FRAMECNT_STAT_SIZE        50
#define FRAMECNT_BLOCKS           (FRAMECNT_TEXT_SIZE / 14)
#define FRAMECNT_BLKSIZE          12

#define TX_TITLE    "Frame Processed   # Skipped\n"
#define RX_TITLE    "Frame Processed   Displayed\n"
#define TX_SUMMARY  "  # Skipped     # Times\n"
#define RX_SUMMARY  "  # Displayed   # Times\n"

struct frmcnt_stat {
    int value;
    int repeat;
};

static struct frmcnt_stat   frmcnt[FRAMECNT_STAT_SIZE];
static int                  num_frmcnt;

static void update_frmcnt_stat(int value) {
    if (num_frmcnt > 0) {
        int i;
        for (i = 0; i < num_frmcnt; i++) {
            if (frmcnt[i].value == value) {
                frmcnt[i].repeat++;
                return;
            }
        }
    }
    
    if (num_frmcnt < FRAMECNT_STAT_SIZE - 1) {
        frmcnt[num_frmcnt].value = value;
        frmcnt[num_frmcnt++].repeat = 1;
    }
    else {
        VLOG_INFO_CAT(VLOG_VLIB, 
                "ERROR: too many entries in frame counter statstics");
    }
}

static int compare(const void *s, const void *t) {
    if (*(int *) s > *(int *) t) return 1;
    else if (*(int *) s == *(int *) t) return 0;
    else return -1;
}

static int print_title(char *txt, char *title) {
    int len;
    char linebuf[72];
    
    len = sprintf(linebuf, title);
    strcat(txt, linebuf);
    len += sprintf(linebuf, "----------------------------\n");
    strcat(txt, linebuf);
    return len;
}

int print_framecnt_to_buffer(unsigned char *buf, 
                             char *txt,
                             char *pager,
                             int level,
                             int tm,
                             int type) {
    int i, len = 0, lines = 0, pages = -1;
    int total = 0, num_processed = 0, num_skip = 0, num_repeat = 1;
    unsigned long curr, prev = 0;
    char linebuf[72];

    if (level <= 0 || level >= FRAMECNT_FIFO_SIZE)
        return -1;

    num_frmcnt = 0;
    memset(frmcnt, 0, sizeof(struct frmcnt_stat) * FRAMECNT_STAT_SIZE);

    txt[0] = 0; 

    len += print_title(txt, (type == BOARDTYPE_TX) ? TX_TITLE : RX_TITLE);
    lines = 2;

    for (i = 0; i < level; i++) {

        curr = (*((unsigned long *) &buf[4 * i]) & 0x000ffff0) >> 4;

        if (i > 0) {
            //if (curr < prev) curr += 0x10000;
            if (prev > curr) {
                VLOG_INFO_CAT(VLOG_VLIB, 
                    "Error displaying frame count statistics: %ld follows %ld.",
                    curr, prev);
                return -1; 
            } 
            if (type == BOARDTYPE_TX) {
                /* invalid frame count */
                if (curr < prev) 
                    return -1;

                /* ignore repeated frames */
                if (curr == prev)    
                    continue;

                num_skip = curr - prev - 1;
                update_frmcnt_stat(num_skip);
                sprintf(linebuf, "     %05d      %5d\n", 
            (unsigned short) (curr & 0xffff), num_skip);
                len += sprintf(linebuf, "     %05d      %5d\n", 
            (unsigned short) (curr & 0xffff), num_skip);
                if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
                   return -1;
                } else {
                    strcat(txt, linebuf);
                }
                if (++lines % FRAMECNT_BLKSIZE == 0) {
                    if (pages >= (FRAMECNT_BLOCKS - 2)) break; 
                    ++pages;
                    if (pager != NULL) {
                        len += sprintf(txt + len, "\n%s\n", pager);
                    }
                    len += print_title(txt, TX_TITLE);
                    lines = 2;
                }
            }
            else {
                /* count repeated frames, print it when we see the next */
                /* frame (last frame is never in the printed list).     */
                if (prev == curr) {
                    ++num_repeat; continue;
                }

                update_frmcnt_stat(num_repeat);
                len += sprintf(linebuf, "     %05d          %d\n",
                           (unsigned short) (prev & 0xffff), num_repeat);
                if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
                   return -1;
                } else {
                   strcat(txt, linebuf);
                }
                if (++lines % FRAMECNT_BLKSIZE == 0) {
                    if (pages >= (FRAMECNT_BLOCKS - 2)) break; 
                    ++pages;
                    if (pager != NULL) {
                        len += sprintf(txt + len, "\n%s\n", pager);
                    }
                    len += print_title(txt, RX_TITLE);
                    lines = 2;
                }

                if (prev < curr - 1) {
                    /* fill in missing frames */
                    while (++prev < curr) {
                        num_skip++;
                        len += sprintf(linebuf, "     %05d          0\n",
                                    (unsigned short) (prev & 0xffff));
                        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
                           return -1;
                        } else {
                            strcat(txt, linebuf);
                        }
                        if (++lines % FRAMECNT_BLKSIZE == 0) {
                            if (pages >= (FRAMECNT_BLOCKS - 2)) break; 
                            ++pages;
                            if (pager != NULL) {
                                len += sprintf(txt + len, "\n%s\n", pager);
                            }
                            len += print_title(txt, RX_TITLE);
                            lines = 2;
                        }
                    }
                }
                num_repeat = 1;
            }
        } 
        else if (type == BOARDTYPE_TX) {
            len += sprintf(linebuf, "     %05d\n",
                    (unsigned short) (curr & 0xffff));
            if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
                return -1;
            } else {
               strcat(txt, linebuf);
            }
             
            if (++lines % FRAMECNT_BLKSIZE == 0) {
                if (pages >= (FRAMECNT_BLOCKS - 2)) break;
                ++pages;
                if (pager != NULL) {
                    len += sprintf(txt + len, "\n%s\n", pager);
                }
                len += print_title(txt, TX_TITLE);
                lines = 2;
            }
        }
        prev = curr;
    }

    ++pages;

    if (pager != NULL) {
        len += sprintf(txt + len, "\n%s\n", pager);
    }
    if (num_frmcnt > 0)
        qsort((void *) frmcnt, num_frmcnt, sizeof(struct frmcnt_stat), compare);

    if (type == BOARDTYPE_TX) {
        prev = (*((unsigned long *) &buf[0]) & 0x000ffff0) >> 4;
        curr = (*((unsigned long *) &buf[4 * level - 4]) & 0x000ffff0) >> 4;
        if (curr < prev) curr += 0x10000;
        total = curr - prev + 1;
    }
    else if (num_frmcnt > 0) {
        for (total = 0, i = 0; i < num_frmcnt; i++) 
            total += frmcnt[i].value * frmcnt[i].repeat;
    }

    for (num_processed = 0, i = 0; i < num_frmcnt; i++) 
        num_processed += frmcnt[i].repeat;

    len += sprintf(linebuf, "Summary Frame Count Statistics\n\n");
    if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
       return -1;
    } else {
        strcat(txt, linebuf);
    }
    if (type == BOARDTYPE_TX) {
        num_processed++;        /* compensate for first frame */
        len += sprintf(linebuf, "Total frames %d in %d secs.\n", total, tm);
        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
           return -1;
        } else {
            strcat(txt, linebuf);
        }
        len += sprintf(linebuf, "%d frames processed, %d frames skipped.\n\n", 
        num_processed, total - num_processed);
        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
           return -1;
        } else {
            strcat(txt, linebuf);
        }
    }
    else {
        len += sprintf(linebuf, "Total frames %d displayed in %d secs.\n", 
        total, tm);
        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
           return -1;
        } else {
            strcat(txt, linebuf);
        }
        len += sprintf(linebuf, "%d frames processed, %d frames repeated.\n\n",
        num_processed, total - num_processed);
        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
            return -1;
        } else {
            strcat(txt, linebuf);
        }
    }

    
    len += print_title(txt, (type==BOARDTYPE_TX) ? TX_SUMMARY : RX_SUMMARY);
    lines = 5;

    if (num_skip > 0 && type == BOARDTYPE_RX) {
        total += num_skip;
        len += sprintf(linebuf, "      0        %5d\n", num_skip);
        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
            return -1;
        } else {
            strcat(txt, linebuf);
        }
        lines++;
    }
                                                                                
    for (i = 0; i < num_frmcnt; i++) {
        len += sprintf(linebuf, "  %5d        %5d\n",
                        frmcnt[i].value, frmcnt[i].repeat);
        if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
            return -1;
        } else {
            strcat(txt, linebuf);
        }
        if ((++lines % FRAMECNT_BLKSIZE) == 0) {
            if (pages >= (FRAMECNT_BLOCKS - 2)) break; 
                ++pages;
                if (pager != NULL) {
                    len += sprintf(txt + len, "\n%s\n", pager);
                }
            lines = 0;
        }
    }

    strcat(txt, "\n"), len++;
    if ((++lines % FRAMECNT_BLKSIZE) == 0) {
        if (pages < (FRAMECNT_BLOCKS - 2)) {
            ++pages;
            if (pager != NULL) {
                len += sprintf(txt + len, "\n%s\n", pager);
            }
            lines = 0;
        }
    }

    if (total == 0) return 0;

    if (type == BOARDTYPE_TX) {
        if (total == 0) return 0;
        len += sprintf(linebuf, 
              "Min skipped %d, Max skipped %d, %% skipped %4.1f.",
            frmcnt[0].value, frmcnt[num_frmcnt - 1].value,
            100.0 * (1.0 - (double) num_processed / (double) total));
                            printf("%s", linebuf);
    }
    else { 
        if (num_processed == 0) return 0;
        len += sprintf(linebuf, 
            "Min repeat %d, Max repeat %d, Avg repeat %4.1f.",
            (num_skip > 0) ? 0 : frmcnt[0].value, 
            frmcnt[num_frmcnt - 1].value,
            (double) total / (double) num_processed);
    }
    if(strlen(txt) + strlen(linebuf) > FRAMECNT_TEXT_SIZE) {
        return -1;
    } else {
        strcat(txt, linebuf);
    }
    txt[len - 1] = '\0'; 
    return pages;
}
