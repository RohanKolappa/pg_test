#ifndef __SYSTEM_UTILS_H__
#define __SYSTEM_UTILS_H__

#include <netinet/in.h>

#define HWMON_TEMP_MODE0        "/sys/class/hwmon/hwmon0/device/temp_mode0"
#define HWMON_TEMP_MODE1        "/sys/class/hwmon/hwmon0/device/temp_mode1"
#define OMNITEK_PHY0_STATUS     "/sys/class/net/eth0/phy0_status"
#define OMNITEK_PHY1_STATUS     "/sys/class/net/eth0/phy1_status"
#define OMNITEK_LINK_INDEX      "/sys/class/net/eth0/link_index"
#define OMNITEK_LINK0_SPEED     "/sys/class/net/eth0/link0_speed"
#define OMNITEK_LINK1_SPEED     "/sys/class/net/eth0/link1_speed"
#define TEMPERATURE_PROC_FILE   "/proc/acpi/thermal_zone/THRM/temperature"
#define DEFAULT_DST_MAC         "aa:bb:cc:dd:ee:ff"
#define DEFAULT_DST_IP          "10.51.49.55"
#define DEFAULT_SRC_PORT        8080
#define DEFAULT_DST_PORT        8080
#define DEFAULT_IF_NAME         "eth0"

#ifdef __cplusplus
extern "C" {
#endif
extern char *get_cpu_usage(char *buf, void *usrptr, int ident);
extern int get_persistent_int(char *name);
extern void set_persistent_int(char *name, int val);
extern int find_proc_by_name(char *pidName);
extern int get_if_info(char *filename, struct in_addr *ap, struct in_addr *mp);
extern int get_interface_info(char *ifname,  char *ipaddress, char *netmask);
extern int system_command(char *command);
extern int system_direct_command(char *command);
extern char * get_command_buffer(char *command);
extern int read_hex_from_file(FILE *file, unsigned char dat[], int maxlen, 
        int longformat);
extern int get_sysfs_attribute(const char *filename, const char *scanfmt);
#ifdef __cplusplus
}
#endif

#endif      /* __TEXT_UTILS_H__ */


