#include <stdlib.h>
#include <string.h>
//#include <sys/sysinfo.h>

#define CUSTOM_MENU_DEFS
#include "custmenu.h"

#include "memory_status_menu_gen.h"


int getMemoryStatus(struct memory_status_params *params) 
{

    char buff[128];

    /* Calculate memory info */
    FILE *fp;
    char *line;
    unsigned long long memtotal = 0;
    unsigned long long memfree = 0;
    unsigned long long buffers = 0;
    unsigned long long cached = 0;

    fp = fopen("/proc/meminfo", "r");
    if (fp != NULL) {
        while (1) {
            line = fgets(buff, 30, fp);
            if (line == NULL)
                break;

            if (strncmp(line, "MemTotal:", 9) == 0) {
                sscanf(line + 10, "%llu kB", &memtotal);
                memtotal *= 1024;
            }
            else if (strncmp(line, "MemFree:", 8) == 0) {
                sscanf(line + 9, "%llu kB", &memfree);
                memfree *= 1024;
            }
            else if (strncmp(line, "Buffers:", 8) == 0) {
                sscanf(line + 9, "%llu kB", &buffers);
                buffers *= 1024;
            }
            else if (strncmp(line, "Cached:", 7) == 0) {
                sscanf(line + 8, "%llu kB", &cached);
                cached *= 1024;
            }
        }
        fclose(fp);
    }

    if (memtotal > 0) {
        params->total_value = memtotal;
        params->free_value = memfree;
        params->cached_value = buffers + cached;
        params->usable_value = memfree + buffers + cached;

    }


    return RC_NORMAL;
}
