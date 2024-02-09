#ifndef __VUTILS_H_INCLUDED__
#define __VUTILS_H_INCLUDED__

#define INSTALL_PATH  "/V2O"



/* Admin user is defined with gid 100 in /etc/passd file */
#define ADMIN_USER_GROUP     100

typedef int (*util_entry_point)(int argc, char** argv);

struct v_util {
        const   char*   name;
        int     (*main)(int argc, char** argv);
};

extern const struct v_util util[];


/******************************************************
 * Prototypes for main entry points for all utilities *
 ******************************************************/
int tester_main(int argc, char **argv);
int getuserinput_main(int argc, char **argv);
int dhcpc_event_main(int argc, char **argv);
int loadlut_main(int argc, char **argv);
int modlut_main(int argc, char **argv);
int loadnet_main(int argc, char **argv);
int loadxmpp_main(int argc, char **argv);
int menu_main(int argc, char **argv);
int cli_main(int argc, char **argv);
int dreg_main(int argc, char **argv);
int vlog_main(int argc, char **argv);
int mtdcp_main(int argc, char **argv);
int vserver_main(int argc, char **argv);
int dserver_main(int argc, char **argv);
int vclient_main(int argc, char **argv);
int dclient_main(int argc, char **argv);
int mkudirs_main(int argc, char **argv);
int vwhois_main(int argc, char **argv);
int vwhoisd_main(int argc, char **argv);
int vpasswd_main(int argc, char **argv);
int get_tx_conn_parm_main(int argc, char **argv);
int set_tx_conn_parm_main(int argc, char **argv);
int get_auth_table_main(int argc, char **argv);
int describe_auth_table_main(int argc, char **argv);
int add_auth_entry_main(int argc, char **argv);
int del_auth_entry_main(int argc, char **argv);
int mod_auth_entry_main(int argc, char **argv);
int get_tx_vid_parm_main(int argc, char **argv);
int set_tx_vid_parm_main(int argc, char **argv);
int add_conn_entry_main(int argc, char **argv);
int del_conn_entry_main(int argc, char **argv);
int mod_conn_entry_main(int argc, char **argv);
int get_conn_table_main(int argc, char **argv);
int describe_conn_table_main(int argc, char **argv);
int make_conn_main(int argc, char **argv);
int break_conn_main(int argc, char **argv);
int get_tx_vid_parm_main(int argc, char **argv);
int set_tx_vid_parm_main(int argc, char **argv);
int get_rx_vid_parm_main(int argc, char **argv);
int set_rx_vid_parm_main(int argc, char **argv);
int get_eth_if_main(int argc, char **argv);
int set_eth_if_main(int argc, char **argv);
int add_route_entry_main(int argc, char **argv);
int del_route_entry_main(int argc, char **argv);
int mod_route_entry_main(int argc, char **argv);
int get_route_table_main(int argc, char **argv);
int describe_route_table_main(int argc, char **argv);
int save_config_main(int argc, char **argv);
int save_video_params_to_lut_main(int argc, char **argv);
int revert_config_main(int argc, char **argv);
int get_ssh_main(int argc, char **argv);
int set_ssh_main(int argc, char **argv);
int get_tx_status_main(int argc, char **argv);
int get_rx_status_main(int argc, char **argv);
int get_version_main(int argc, char **argv);
int set_diag_main(int argc, char **argv);
int get_diag_main(int argc, char **argv);
int add_user_entry_main(int argc, char **argv);
int del_user_entry_main(int argc, char **argv);
int mod_user_entry_main(int argc, char **argv);
int get_user_table_main(int argc, char **argv);
int auto_phase_adjust_main(int argc, char **argv);
int get_splash_parm_main(int argc, char **argv);
int set_splash_parm_main(int argc, char **argv);
int get_colorgain_offset_parm_main(int argc, char **argv);
int set_colorgain_offset_parm_main(int argc, char **argv);
int get_edid_main(int argc, char **argv);
int get_edid_table_main(int argc, char **argv);
int set_edid_main(int argc, char **argv);
int add_edid_entry_main(int argc, char **argv);
int del_edid_entry_main(int argc, char **argv);
int mod_edid_entry_main(int argc, char **argv);
int set_edid_main(int argc, char **argv);
int set_edid_with_lut_main(int argc, char **argv);
int clear_edid_list_main(int argc, char **argv);
int get_rulelist_table_main(int argc, char **argv);
int add_rulelist_entry_main(int argc, char **argv);
int del_rulelist_entry_main(int argc, char **argv);
int mod_rulelist_entry_main(int argc, char **argv);
int auto_adjust_quality_parms_main(int argc, char **argv);
int sync_cp_main(int argc, char **argv);
int kmtest_main(int argc, char **argv);
int audiotest_main(int argc, char **argv);
int videotest_main(int argc, char **argv);
int get_xmppparams_main(int argc, char **argv);
int set_xmppparams_main(int argc, char **argv);
int get_xmppserver_main(int argc, char **argv);
int set_xmppserver_main(int argc, char **argv);
int dump_config_main(int argc, char **argv);
int upgrade_device_main(int argc, char **argv);
int reboot_system_main(int argc, char **argv);
int shutdown_system_main(int argc, char **argv);
int ha_takeover_main(int argc, char **argv);
int ha_standby_main(int argc, char **argv);
int set_time_main(int argc, char **argv);
int colortest_main(int argc, char **argv);
int load_fpga_main(int argc, char **argv);
int dumpsram_main(int argc, char **argv);
int export_database_main(int argc, char **argv);
int import_database_main(int argc, char **argv);
int save_database_main(int argc, char **argv);
int restore_database_main(int argc, char **argv);
int delete_database_main(int argc, char **argv);
int dump_main(int argc, char **argv);
int standalone_main(int argc, char **argv);
int send_notification_main(int argc, char **argv);
int lseventdev_main(int argc, char **argv);
int lsttyusb_main(int argc, char **argv);
int usbhotplug_main(int argc, char **argv);
int describe_override_list_main(int argc, char **argv);
int get_override_list_main(int argc, char **argv);
int get_chromakey_parm_main(int argc, char **argv);
int set_chromakey_parm_main(int argc, char **argv);
int get_chromakey_color_main(int argc, char **argv);
int set_chromakey_color_main(int argc, char **argv);
int apply_chromakey_config_main(int argc, char **argv);
int get_chromakey_status_main(int argc, char **argv);
int check_device_main(int argc, char **argv);
int load_edid_eeprom_main(int argc, char **argv);
int read_edid_eeprom_main(int argc, char **argv);
int configure_banner_login_main(int argc, char **argv);
int show_banner_login_main(int argc, char **argv);
int urlencoder_main(int argc, char **argv);
int urldecoder_main(int argc, char **argv);
int get_license_pin_main(int argc, char **argv);
int set_license_file_main(int argc, char **argv);
int verify_license_main(int argc, char **argv);

// Utility functions for finding cli command
util_entry_point find_cli_util(const char *util_name);

#endif /* __VUTILS_H_INCLUDED__ */

