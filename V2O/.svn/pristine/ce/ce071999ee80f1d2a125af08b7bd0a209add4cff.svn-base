



proc create_config_cli {root base_name} {


    set title [get_text_node_value $root "/Menu/Title"]
    set description [get_text_node_value $root "/Menu/Description"]
    set access [get_text_node_value $root "/Menu/Access"]
    set upper_access [string toupper $access]
    set config_file [get_text_node_value $root "/Menu/ConfigFile"]
    set config_file "/V2O/config/${config_file}"
    set config_type [get_text_node_value $root "/Menu/Type"]
    set vertical_offset 1
    if [check_node_exists $root "/Menu/VerticalOffset"] {
       set vertical_offset [get_text_node_value $root "/Menu/VerticalOffset"]
    }


    if [check_node_exists $root "/Menu/SubType"] {
        set config_subtype [get_text_node_value $root "/Menu/SubType"]
    } else {
        set config_subtype "normal"
    }

    set board_access 0
    if [check_node_exists $root "/Menu/BoardAccess"] {
       set value [get_text_node_value $root "/Menu/BoardAccess"]
        if [string equal $value "true"] {
            set board_access 1
        } elseif [string equal $value "false"] {
            set board_access 0
        } else {
            Error "Value \"$value\" not supported for BoardAccess. Should be true or false"
        }
    }

    set cli_command_name ${access}
    if [check_node_exists $root "/Menu/CliCommandName"] {
        set cli_command_name [get_text_node_value $root "/Menu/CliCommandName"]
    }

    set publish_remote 1
    if [check_node_exists $root "/Menu/PublishRemote"] {
       set value [get_text_node_value $root "/Menu/PublishRemote"]
        if [string equal $value "true"] {
            set publish_remote 1
        } elseif [string equal $value "false"] {
            set publish_remote 0
        } else {
            Error "Value \"$value\" not supported for PublishRemote. Should be true or false"
        }
    }

    set remote_config_suffix ""
    if { $publish_remote == 0 } {
        set remote_config_suffix "_dummy"
    }

    Debug "Title: $title"
    Debug "Description: $description"
    Debug "Access: $access"
    Debug "File: $config_file"
    Debug "Type: $config_type"
    Debug "SubType: $config_subtype"

    # Get count of the number of parameter items
    set item_count 0
    set nodes [$root selectNodes "/Menu/Items/Item" type]
    if { $type == "nodes" } {
        set item_count [llength $nodes]
    }

    # Create list of indexes
    set index_list {}
    for {set idx 1} {$idx < $item_count + 1} {incr idx} {
        lappend index_list $idx
    }

    # Retrieve all needed parameter (Item) data
    foreach {idx} $index_list {
        set item($idx,name) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Name"]
        set item($idx,type) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Type"]
        set item($idx,label) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Label"]
        set item($idx,fieldlength) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/FieldLength"]
        set item($idx,default) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/DefaultValue"]
        set item($idx,help) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Help"]
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Align"] {
            set item($idx,align) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Align"]
        } else {
            set item($idx,align) "normal"
        }

        if [string equal $config_subtype "immediate"] {
            if [string equal $item($idx,type) "integer"] {

            } elseif [string equal $item($idx,type) "boolean"] {

            } elseif [string equal $item($idx,type) "string"] {

            } elseif [string equal $item($idx,type) "float"] {

            } elseif [string equal $item($idx,type) "bandwidth"] {

            } else {
                Error "Node type \"$item($idx,type)\" not supported for config type immediate"
            }

        } elseif [string equal $config_subtype "normal"] {

        } else {
            Error "SubType  \"$config_subtype\" is invalid, should be \"normal\" or \"immediate\"."
        }

        if [string equal $item($idx,align) "normal"] {

        } elseif [string equal $item($idx,align) "follow"] {

        } else {
            Error "Align tag value \"$item($idx,align)\" is invalid, should be \"normal\" or \"follow\"."
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Validation"] {
            set item($idx,validation) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Type"]
            if [string equal $config_subtype "immediate"] {
                if [string equal $item($idx,validation) "ipaddress"] {
                    Error "Do not support \"$item($idx,validation)\" validation type while generating above _gen.c file"
                } elseif [string equal $item($idx,validation) "netmask"] {
                    Error "Do not support \"$item($idx,validation)\" validation type while generating above _gen.c file"
                }
            }
            if [string equal $item($idx,validation) "range"] {
                set item($idx,validation_min) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Min"]
                set item($idx,validation_max) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Max"]
            } elseif [string equal $item($idx,validation) "regexp"] {
                set item($idx,validation_value) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Value"]
            } elseif [string equal $item($idx,validation) "enumeration"] {
                set item($idx,validation_values) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Values"]
                set field_validation_values [split $item($idx,validation_values) ","]
                set value_list {}
                set value_list_csv {}
                foreach value $field_validation_values {
                    set val [string trim ${value} " \""]
                    lappend value_list $val
                    lappend value_list_csv $val
                    lappend value_list_csv ","
                }
                set item($idx,validation_value_list) $value_list
                set item($idx,validation_value_list_csv) $value_list_csv
            } else {

            }
        } else {
            set item($idx,validation) "none"
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Disable"] {
            if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Disable/Item"] {
                set dis_item_count 0
                # Get count of the number of parameter Disable items
                set dis_nodes [$root selectNodes "/Menu/Items/Item\[$idx\]/Disable/Item"]
                set dis_item_count [llength $dis_nodes]

                set item($idx,disable_item_count) $dis_item_count

                for {set dis_idx 1} {$dis_idx < $dis_item_count + 1} {incr dis_idx} {
                    set item($idx,disable,$dis_idx,name) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Disable/Item\[$dis_idx\]/Name"]
                    set item($idx,disable,$dis_idx,value) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Disable/Item\[$dis_idx\]/Values"]
                    set field_disable_value [split $item($idx,disable,$dis_idx,value) ","]
                    set dis_value_list {}
                    foreach {disablevalue} $field_disable_value {
                        set disvalue [string trim ${disablevalue} " \""]
                        lappend dis_value_list $disvalue
                    }
                    set item($idx,disable,$dis_idx,disable_value_list) $dis_value_list
                }
            } else {
                set item($idx,disable_item_count) 0
            }
        } else {
            set item($idx,disable_item_count) 0
        }

        Debug "Parameter: $item($idx,name)"
    }


    ##################################
    # Create header file
    set upper_base_name [string toupper $base_name]
    emit_header {
        #ifndef __${upper_base_name}_H_
        #define __${upper_base_name}_H_
    }

    # Field Length hash defs
    emit_header_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_length $item($idx,fieldlength)
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"
        emit_header {
            #define ${field_length_name}      ${field_length}
        }
    }

    # Init function prototype
    emit_header_newline
    emit_header {
        /* Client implement this function to register all customization hooks */
        void ${title}Init(void);
    }

    # Define enums needed for enumerated strings
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type $item($idx,type)
        set field_validation "$item($idx,validation)"
        if {[string equal $field_type "string"]} {
            if {[string equal $field_validation "enumeration"]} {
                emit_header_newline
                emit_header {
                    /* ENUMs for $name */
                }
                emit_header {
                    enum ${access}_${name} \{
                }
                foreach value $item($idx,validation_value_list) {
                    set val [string toupper $value]
                    set val [string map {/ "_BY_"} $val]
                    set val [string map {" " "_"} $val]
                    set val [string map {":" "_COLON_"} $val]
                    set val [string map {"." "_DOT_"} $val]
                    emit_header {
                        ${upper_access}_${upper_name}_${val},
                    }
                }
                emit_header {
                    \};
                }
            }
        }
    }

    # Parameter access struct definition
    set access_struct "${access}_params"
    emit_header_newline
    emit_header {
        struct ${access_struct} \{
    }

    emit_header_newline
    emit_header {
        /* Clients use these members to access the parameter values */
    }

    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type "$item($idx,type)"
        set field_validation "$item($idx,validation)"
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"

        if {[string equal $field_type "boolean"] || [string equal $field_type "integer"] } {
            emit_header {
                int      ${name}_value;
            }
        } elseif {[string equal $field_type "float"] } {
            emit_header {
                double    ${name}_value;
            }
        } elseif {[string equal $field_type "string"] && [string equal $field_validation "enumeration"] } {
            emit_header {
                enum ${access}_${name} ${name}_value;
            }
        } elseif {[string equal $field_type "string"] && [string equal $field_validation "hexadecimal"] } {
            emit_header {
                int     ${name}_value;
            }
        } elseif {[string equal $field_type "string"] && [string equal $field_validation "onlyhexadecimal"] } {
            emit_header {
                int     ${name}_value;
            }
        } elseif {[string equal $field_type "bandwidth"] } {
            emit_header {
                int    ${name}_value;
            }
        } else {
            emit_header {
                char     ${name}_value[$field_length_name + 1];
            }
        }
    }

    emit_header_newline
    emit_header {
        /* Clients use these members to check if a parameter value has been set */
    }
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit_header {
            int      ${name}_set;
        }
    }
    emit_header_newline
    emit_header {
        /* Generic user pointer used for various purposes (e.g. to pass HAL ptr) */
        void * userptr;
    }
    emit_header_newline
    emit_header {
        /* Internal members not to be used by client */
    }
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"

        emit_header {
            char     ${name}[$field_length_name + 1];
        }
    }

    emit_header {
        \};
    }

    emit_header_newline
    emit_header {
        #ifdef CUSTOM_MENU_DEFS
        #include "custmenu.h"
        /* Prototype for configure${title} command */
        extern int configure${title}(MENUINFO *minfo);
        #endif
    }

    emit_header_newline
    if {$board_access} {
        emit_header {
            typedef void ${title}SavedFunction(void * userptr);
        }
    } else {
        emit_header {
            typedef void ${title}SavedFunction(void);
        }
    }

    emit_header {
        extern void register${title}SavedFunction(${title}SavedFunction *funcPtr);
    }

    emit_header_newline
    if {$board_access} {
        emit_header {
            typedef void ${title}MenuInitFunction(void * userptr);
        }
    } else {
        emit_header {
            typedef void ${title}MenuInitFunction(void);
        }
    }

    emit_header {
        extern void register${title}MenuInitFunction(${title}MenuInitFunction *funcPtr);
    }

    emit_header_newline
    emit_header {
        /* Custom Validator function prototype*/
        typedef int ${title}ValidateFunction(struct ${access_struct} *params,
                                              struct ${access_struct} *orig_params,
                                              char *error_string);

        extern void register${title}ValidateFunction(${title}ValidateFunction *funcPtr);

    }

  emit_header_newline
    if {$board_access} {
        emit_header {
            typedef void ${title}MenuExitFunction(void * userptr);
        }
    } else {
        emit_header {
            typedef void ${title}MenuExitFunction(void);
        }
    }

    emit_header {
        extern void register${title}MenuExitFunction(${title}MenuExitFunction *funcPtr);
    }

    emit_header_newline
    emit_header {
        /* Custom parameter initialization function prototype */
        typedef void ${title}ParamsInitFunction(struct ${access_struct} *params);

        extern void register${title}ParamsInitFunction(${title}ParamsInitFunction *funcPtr);

    }

    emit_header_newline
    emit_header {
        /* Prototype for apply${title} function */
        extern int apply${title}(struct ${access_struct} *params,
                                 struct ${access_struct} *orig_params);

    }

    emit_header_newline
    emit_header {
        /* Prototype for retrieve${title}Values function */
        extern void retrieve${title}Values(struct ${access_struct} *params);
    }

    emit_header_newline
    emit_header {
        /* Prototype for retrieveConfigured${title}Values function */
        extern void retrieveConfigured${title}Values(struct ${access_struct} *params);

    }

    emit_header_newline
    emit_header {
        /* Prototype for unsetConfigured${title}Values function */
        extern void unmask${title}Values(struct ${access_struct} *params);

    }


    emit_header_newline
    emit_header {
        /* Prototype for load${title} function. */
        extern int load${title}(void);
    }

    emit_header_newline
    emit_header {
        /* Prototypes for CLI access entry point functions */
        extern int set_${access}_main(int argc, char **argv);
        extern int get_${access}_main(int argc, char **argv);
        extern int load_${access}_main(int argc, char **argv);
	extern int describe_${access}_main(int argc, char **argv);

    }

    if {![string equal $cli_command_name "undefined"]} {
        emit_header_newline
        emit_header {
        /* These set up CLI access name mappings that get plugged into vmain.c */
        /*
        CLI_COMMAND_MAP    {"set_${cli_command_name}", set_${access}_main},
        CLI_COMMAND_MAP    {"get_${cli_command_name}", get_${access}_main},
        CLI_COMMAND_MAP    {"load_${cli_command_name}", load_${access}_main},
        CLI_COMMAND_MAP    {"describe_${cli_command_name}", describe_${access}_main},
        */
        }
    }
    emit_header_newline
    emit_header {
        /* Prototypes for dump${title} function */
        extern int dump${title}(void);
    }


    emit_header_newline

    emit_header {
        #endif /* __${upper_base_name}_H_ */
    }

    ###############################
    # Write out the code in C file

    # Write the include directives
    emit {

        #include <stdlib.h>
        #include <string.h>
        #include <getopt.h>

        #include <netinet/in.h>

        #define CUSTOM_MENU_DEFS

        #include "hal.h"
        #include "custmenu.h"
        #include "menuutils.h"
        #include "configureSystem.h"
        #include "configureVideo.h"
        #include "loadnet.h"
        #include "commonutils.h"
        #include "cli_utils.h"
        #include "configfile_parser.h"
        #include "validationroutines.h"
        #include "text_utils.h"
        #include "vlog.h"
        #include "remoteConfig.h"
        #include "${base_name}.h"

    }

    ##################################
    # Field hash defs
    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        emit {
            \#define FIELD_${upper_name}      $idx
        }
    }



    emit {
        #ifndef LIBONLY
    }

    emit_newline
    emit {
        static char *get_${access} = "Get $description";
        static char *set_${access} = "Set $description";
        static char *${access}_params = "$title";
    }


    ##################################
    # Parameter option list array
    set access_options "${access}_options"

    emit_newline
    emit {
        static struct option ${access_options}[] = \{
    }

    foreach {idx} $index_list {
        set name $item($idx,name)

        emit {
            {"$name", 1, 0, 0},}
    }
    emit {
        {"clientData", 1, 0, 0},
    }

    set channel_help ""
    if {$board_access} {
        emit {
            {"channelNumber", 1, 0, 0},
        }
        set channel_index [expr $item_count + 1 ]
        set channel_help "-channelNumber Number"
    }
    emit {
        {0, 0, 0, 0}
        \};
    }

    emit {
        #endif /* LIBONLY */
    }


    ##################################
    # Misc structures that may be needed
    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set field_type $item($idx,type)
        set field_validation "$item($idx,validation)"

        if {[string equal $field_type "string"]} {
            if {[string equal $field_validation "enumeration"]} {
                emit {
                    static char* ${name}_values[] = \{$item($idx,validation_values), NULL\};
                }
            }
        }
    }


    ##################################
    # The four access functions for each parameter item

    emit_newline
    if ($board_access) {
        emit {
            static int board_number = 0;
        }
    }
    emit_newline
    emit {
        static char *get_config_file_name(void) \{
    }
    if ($board_access) {
        emit {
            static char buf[128];
            sprintf(buf, "${config_file}_%d.txt", board_number);
            return buf;
        }
    } else {
        emit {
            return "${config_file}.txt";
        }
    }
    emit {
        \}
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name "[string toupper $name]"
        set field_name_str "\"$upper_name\""
        set file_name "\"$config_file\""
        set field_length "FIELD_${upper_access}_${upper_name}_LENGTH"
        set default_value "\"$item($idx,default)\""
        set field_type $item($idx,type)
        set field_validation "$item($idx,validation)"

        emit {

            static int get_${name}_handler(char *name, char *value, void *ptr) \{
            char *buf = ptr;
            if (strcasecmp(name, $field_name_str) == 0) \{
        }

        emit {
            if (strlen(value) > $field_length)
            return 0;
        }

        if {[string equal $field_type "string"]} {
            if {[string equal $field_validation "ipaddress"]} {
                emit {
                    if (check_ip(value) != RC_NORMAL)
                    return 0;
                }

            } elseif {[string equal $field_validation "ipaddressorblank"]} {
                emit {
                    if (check_ipaddorblank(value) != RC_NORMAL)
                    return 0;
                }

            } elseif {[string equal $field_validation "hostnameorblank"]} {
                emit {
                    if (check_hostnameorblank(value) != RC_NORMAL)
                    return 0;
                }

            } elseif {[string equal $field_validation "inetname"]} {
                emit {
                    if (check_inetname(value) != RC_NORMAL)
                    return 0;
                }
            } elseif {[string equal $field_validation "domainname"]} {
                emit {
                    if (check_domainname(value) != RC_NORMAL)
                    return 0;
                }
            } elseif {[string equal $field_validation "netmask"]} {
                emit {
                    if (check_netmask(value) != RC_NORMAL)
                    return 0;
                }
            } elseif {[string equal $field_validation "multicast"]} {
                emit {
                    if (check_multicastip(value) != RC_NORMAL)
                    return 0;
                }
            } elseif {[string equal $field_validation "regexp"]} {
                emit {
                    if (check_regex(value, regexp_$item($idx,validation_value)) != RC_NORMAL)
                    return 0;
                }
            } elseif {[string equal $field_validation "enumeration"]} {
                emit {
                    if (check_enum(value, ${name}_values) != RC_NORMAL)
                    return 0;

                }
            } elseif {[string equal $field_validation "hexadecimal"]} {
                emit {
                    if (check_hexadecimal(value) != RC_NORMAL)
                    return 0;

                }
            } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                emit {
                    if (check_onlyhexadecimal(value) != RC_NORMAL)
                    return 0;

                }
            } else {
                Error "Unrecognized validation type ${field_validation} in ${name}"
            }
        } elseif {[string equal $field_type "boolean"]} {
            emit {
                if (check_enum(value, yesnochecklist) != RC_NORMAL)
                return 0;
            }
        } elseif {[string equal $field_type "password"]} {
            emit {
                // Not checking password value found in file
            }
        } elseif {[string equal $field_type "integer"]} {
            emit {
                if ((atoi(value)) > $item($idx,validation_max) || \
                        atoi(value) < $item($idx,validation_min))
                return 0;
            }

        } elseif {[string equal $field_type "float"]} {
            emit {
                if (atof(value) > $item($idx,validation_max) || \
                             atof(value) < $item($idx,validation_min))
                return 0;
            }

        } elseif {[string equal $field_type "bandwidth"]} {
            emit {
                if (check_regex(value, regexp_bandwidth) != RC_NORMAL)
                return 0;
                // Cleanup the bitrate string
                int val = bits_to_bytes(value);
                bytes_to_bits(val, value, $field_length);
            }
        }

        emit {
            if (buf != NULL)
            strncpy(buf, value, $field_length + 1);
            return 1;
            \}
            return 0;
            \}

            static char *get_${name}(char *buf, void *usrptr, int ident) \{
            char buffer[$field_length + 1];
            strncpy(buf, $default_value, $field_length + 1);
            strncpy(buffer, $default_value, $field_length + 1);
            if (parse_configuration_file(get_config_file_name(),
                                          get_${name}_handler, buffer) < 0) \{
            VLOG_WARNING("Could not load configuration file %s", get_config_file_name());
            \}
            else \{
            strncpy(buf, buffer, $field_length + 1);
            \}

            if (usrptr != NULL) \{
            struct ${access_struct} *params = (struct ${access_struct} *)usrptr;
            if (params->${name}_set == 1)
            strncpy(buf, params->${name}, $field_length + 1);
            \}
            return buf;
            \}

            #ifndef LIBONLY
            static int write_${name}_handler(char *name, char *value, void *ptr, char flag) \{
            char *buf = ptr;
            if (flag) \{
            if (buf != NULL && strlen(buf) <= $field_length) \{
            if (strcasecmp(name, $field_name_str) == 0) \{
            sprintf(value, "%s", buf);
            return 1;
            \}
            \}
            \}
            else \{
            strcpy(name, $field_name_str);
            sprintf(value, "%s", buf);
            \}
            return 0;
            \}

            static int set_${name}(char *buf) \{
            if (rewrite_configuration_file(get_config_file_name(),
                                           write_${name}_handler, buf) < 0) \{
            VLOG_WARNING("Could not rewrite configuration file %s", get_config_file_name());
            return RC_ERROR;
            \}
            return RC_NORMAL;
            \}

            #endif /* LIBONLY */
        }

    }

    ##################################
    # Parameter value setting function
    emit_newline
    emit {
        static void setup_parameter_values(struct ${access_struct} *params) \{
    }
    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type "$item($idx,type)"
        set field_validation "$item($idx,validation)"
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"
        if {[string equal $field_type "integer"] } {
            emit {
                params->${name}_value = atoi(params->${name});
            }
        } elseif {[string equal $field_type "float"] } {
            emit {
                params->${name}_value = strtod(params->${name}, NULL);
            }
        } elseif {[string equal $field_type "boolean"] } {
            emit {
                if (strcasecmp(params->${name}, "Yes") == 0)
                params->${name}_value = 1;
                else
                params->${name}_value = 0;
            }
        } elseif {[string equal $field_type "bandwidth"] } {
            emit {
                params->${name}_value = bits_to_bytes(params->${name});
            }
        } elseif {[string equal $field_type "string"] } {
            if {[string equal $field_validation "enumeration"]} {
                emit {
                    do \{
                }
                foreach value $item($idx,validation_value_list) {
                    set upper_value [string toupper $value]
                    set upper_value [string map {/ "_BY_"} $upper_value]
                    set upper_value [string map {" " "_"} $upper_value]
                    set upper_value [string map {":" "_COLON_"} $upper_value]
                    set upper_value [string map {"." "_DOT_"} $upper_value]
                    emit {
                        if (strcasecmp(params->${name}, "${value}") == 0) \{
                        params->${name}_value = ${upper_access}_${upper_name}_${upper_value};
                        break;
                        \}
                    }
                }
                emit {
                    \} while (0);
                }
            } elseif {[string equal $field_validation "hexadecimal"]} {
                emit {
                    params->${name}_value = get_hexadecimal(params->${name});
                }
            } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                emit {
                    params->${name}_value = get_onlyhexadecimal(params->${name});
                }
            } else {
                emit {
                    strncpy(params->${name}_value, params->${name},
                            ${field_length_name} + 1);
                }
            }
        } else {
            # Should not match this catchall
            emit {
                strncpy(params->${name}_value, params->${name},
                        ${field_length_name});
            }
        }
    }
    emit {
        \}
    }

    ##################################
    # Parameter string setting function
    emit_newline
    emit {
        static void setup_parameter_strings(struct ${access_struct} *params) \{
    }
    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type "$item($idx,type)"
        set field_validation "$item($idx,validation)"
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"
        emit {
            if(params->${name}_set == 1) \{
        }
        if {[string equal $field_type "integer"] } {
            emit {
                sprintf(params->${name}, "%d", params->${name}_value);
            }
        } elseif {[string equal $field_type "float"] } {
            emit {
                sprintf(params->${name}, "%.3f", params->${name}_value);
            }
        } elseif {[string equal $field_type "boolean"] } {
            emit {
                if (params->${name}_value == 1)
                strcpy(params->${name}, "Yes");
                else
                strcpy(params->${name}, "No");
            }
        } elseif {[string equal $field_type "bandwidth"] } {
            emit {
                bytes_to_bits(params->${name}_value, params->${name}, $field_length_name);
            }
        } elseif {[string equal $field_type "string"] } {
            if {[string equal $field_validation "enumeration"]} {
                emit {
                    switch (params->${name}_value) \{
                }
                foreach value $item($idx,validation_value_list) {
                    set upper_value [string toupper $value]
                    set upper_value [string map {/ "_BY_"} $upper_value]
                    set upper_value [string map {" " "_"} $upper_value]
                    set upper_value [string map {":" "_COLON_"} $upper_value]
                    set upper_value [string map {"." "_DOT_"} $upper_value]
                    emit {
                        case ${upper_access}_${upper_name}_${upper_value}:
                        strcpy(params->${name}, "$value");
                        break;
                    }
                }
                emit {
                    \}
                }
            } elseif {[string equal $field_validation "hexadecimal"]} {
                emit {
                    sprintf(params->${name}, "%d", params->${name}_value);
                }
            } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                emit {
                    sprintf(params->${name}, "%d", params->${name}_value);
                }
            } else {
                emit {
                    strncpy(params->${name}, params->${name}_value,
                            ${field_length_name} + 1);
                }
            }
        } else {
            # Should not match this catchall
            emit {
                strncpy(params->${name}, params->${name}_value,
                        ${field_length_name} + 1);
            }
        }
        emit {
           \}
        }
    }
    emit {
        \}
    }


    ##################################
    # Custom hooks register functions
    emit_newline
    emit {
        static ${title}ValidateFunction *validateHook = NULL;
        extern void register${title}ValidateFunction(${title}ValidateFunction *funcPtr) \{
        if (funcPtr != NULL) validateHook = funcPtr;
        \}
    }

    emit_newline
    emit {
        static ${title}ParamsInitFunction *paramsInitHook = NULL;
        extern void register${title}ParamsInitFunction(${title}ParamsInitFunction *funcPtr) \{
        if (funcPtr != NULL) paramsInitHook = funcPtr;
        \}
    }

    emit_newline
    emit {
        static ${title}SavedFunction *savedHook = NULL;
        extern void register${title}SavedFunction(${title}SavedFunction *funcPtr) \{
        if (funcPtr != NULL) savedHook = funcPtr;
        \}
    }
    emit_newline
    emit {
        static ${title}MenuInitFunction *menuInitHook = NULL;
        extern void register${title}MenuInitFunction(${title}MenuInitFunction *funcPtr) \{
        if (funcPtr != NULL) menuInitHook = funcPtr;
        \}
    }

    emit_newline
    emit {
        static ${title}MenuExitFunction *menuExitHook = NULL;
        extern void register${title}MenuExitFunction(${title}MenuExitFunction *funcPtr) \{
        if (funcPtr != NULL) menuExitHook = funcPtr;
        \}
    }

    ##################################
    # Retrieve parameters function
    emit_newline
    emit {
        void retrieve${title}Values(struct ${access_struct} *params) \{

        struct ${access_struct} init_params;
        void * init_param_ptr = NULL;
    }

    emit_newline
    emit {
        memset(&init_params, 0x00, sizeof(init_params));
        if (paramsInitHook != NULL) \{
        init_params.userptr = params->userptr;
        (*paramsInitHook)(&init_params);
        setup_parameter_strings(&init_params);
        init_param_ptr = &init_params;
        \}
    }

    if ($board_access) {
        emit_newline
        emit {
            if (params->userptr != NULL) \{
            board_number = hal_get_board_number((HAL *) params->userptr);
            \}
        }
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            get_${name}(params->${name}, init_param_ptr, 0);
            params->${name}_set = 1;
        }
    }

    emit_newline
    emit {
        setup_parameter_values(params);
        \}
    }

    emit_newline
    emit {
        void retrieveConfigured${title}Values(struct ${access_struct} *params) \{
        ${title}ParamsInitFunction *funcPtr;

        funcPtr = paramsInitHook;
        paramsInitHook = NULL;
        retrieve${title}Values(params);
        paramsInitHook = funcPtr;
        \}
    }

    ##################################
    # Unmask parameters function
    emit_newline
    emit {
        void unmask${title}Values(struct ${access_struct} *params) \{
    }
    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            params->${name}_set = 0;
        }
    }

    emit_newline
    emit {
        \}
    }


    emit {
        #ifndef LIBONLY
    }


    ##################################
    # The FORMITEMDEF Item definition array

    emit_newline
    emit {
        static FORMITEMDEF itemdefs[] = \{
    }


    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name "[string toupper $name]"
        set field_length "FIELD_${upper_access}_${upper_name}_LENGTH"
        set field_label "\"$item($idx,label)\""
        set field_help "\"$item($idx,help)\""
        set field_type $item($idx,type)
        set field_validation "$item($idx,validation)"

        set validation_type "FTYPE_NONE"
        set validation_function "NULL"
        set validation_min "0"
        set validation_max "0"
        set options "O_NULLOK"
        set flags "0"

        if [string equal $item($idx,align) "normal"] {
            set flags "0"
        } else {
            if {$idx==1} then {
                Error "The first field cannot be of follow align type."
            } elseif {$item($idx,fieldlength) >= 14} then {
                Error " \"$item($idx,name)\" field has length > 13, hence cant make this field \"follow\" align type."
            } else {
                # Check the kind and length of previous field
                set pidx [expr {$idx - 1}]
                if {[string equal $item($pidx,align) "follow"]} {
                    Error "The previous field to \"$item($idx,name)\" is of follow align type, hence cant make this \"follow\" type."
                } elseif {$item($pidx,fieldlength) >= 14} then {
                    Error "The previous field to \"$item($idx,name)\" has length > 13, hence cant make this field \"follow\" align type."
                } else {
                    set flags FFLAG_FOLLOW
                }
            }
        }

        if [string equal $config_subtype "immediate"] {
            if {[string equal $field_type "string"]} {
                if {[string equal $field_validation "regexp"]} {
                    set validation_type "FTYPE_REGEXP_IMMEDIATE"
                    set validation_min "(int) regexp_$item($idx,validation_value)"
                } elseif {[string equal $field_validation "enumeration"]} {
                    set validation_type "FTYPE_ENUM_IMMEDIATE"
                    set validation_min "(int) ${name}_values"
                } elseif {[string equal $field_validation "hexadecimal"]} {
                    set validation_type "FTYPE_NONE"
                    set validation_function "check_hexadecimal"
                } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                    set validation_type "FTYPE_NONE"
                    set validation_function "check_onlyhexadecimal"
                } else {
                    Error "Unsupported Validation \"${validation_type}\" at node \"${name}\""
                }
            } elseif {[string equal $field_type "boolean"]} {
                set validation_type "FTYPE_ENUM_IMMEDIATE"
                set validation_min "(int) yesnochecklist"
            } elseif {[string equal $field_type "integer"]} {
                set validation_type "FTYPE_INTEGER_IMMEDIATE"
                set validation_min "$item($idx,validation_min)"
                set validation_max "$item($idx,validation_max)"
            } elseif {[string equal $field_type "float"]} {
                set validation_type "FTYPE_NUMERIC_IMMEDIATE"
                set validation_min "$item($idx,validation_min)"
                set validation_max "$item($idx,validation_max)"
            } elseif {[string equal $field_type "bandwidth"]} {
                set validation_type "FTYPE_REGEXP_IMMEDIATE"
                set validation_min "(int) regexp_bandwidth"
            } else {
                Error "Unsupported Type \"${field_type}\" at node \"${name}\""
            }

        } elseif [string equal $config_subtype "normal"] {
            if {[string equal $field_type "string"]} {
                if {[string equal $field_validation "ipaddress"]} {
                    set validation_function "check_ip"
                } elseif {[string equal $field_validation "inetname"]} {
                    set validation_function "check_inetname"
                } elseif {[string equal $field_validation "ipaddressorblank"]} {
                    set validation_function "check_ipaddorblank"
                } elseif {[string equal $field_validation "hostnameorblank"]} {
                    set validation_function "check_hostnameorblank"
                } elseif {[string equal $field_validation "domainname"]} {
                    set validation_function "check_domainname"
                } elseif {[string equal $field_validation "netmask"]} {
                    set validation_function "check_netmask"
                } elseif {[string equal $field_validation "multicast"]} {
                    set validation_function "check_multicastip"
                } elseif {[string equal $field_validation "regexp"]} {
                    set validation_type "FTYPE_REGEXP"
                    set validation_min "(int) regexp_$item($idx,validation_value)"
                } elseif {[string equal $field_validation "enumeration"]} {
                    set validation_type "FTYPE_ENUM_RANGE"
                    set validation_min "(int) ${name}_values"
                } elseif {[string equal $field_validation "hexadecimal"]} {
                    set validation_type "FTYPE_NONE"
                    set validation_function "check_hexadecimal"
                } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                    set validation_type "FTYPE_NONE"
                    set validation_function "check_onlyhexadecimal"
                } else {
                    Error "Unsupported Validation \"${validation_type}\" at node \"${name}\""
                }
            } elseif {[string equal $field_type "boolean"]} {
                set validation_type "FTYPE_ENUM_RANGE"
                set validation_min "(int) yesnochecklist"
            } elseif {[string equal $field_type "password"]} {
                set validation_type "FTYPE_REGEXP"
                set validation_min "(int) regexp_password"
                set options "O_PUBLIC"
            } elseif {[string equal $field_type "integer"]} {
                set validation_type "FTYPE_INTEGER"
                set validation_min "$item($idx,validation_min)"
                set validation_max "$item($idx,validation_max)"
            } elseif {[string equal $field_type "float"]} {
                set validation_type "FTYPE_NUMERIC"
                set validation_min "$item($idx,validation_min)"
                set validation_max "$item($idx,validation_max)"
            } elseif {[string equal $field_type "bandwidth"]} {
                set validation_type "FTYPE_REGEXP"
                set validation_min "(int) regexp_bandwidth"
            } else {
                Error "Unsupported Type \"${field_type}\" at node \"${name}\""
            }

        }


        emit {

            {FIELD_${upper_name}, $field_label,
                $field_help,
                $field_length, get_${name}, $validation_function,
                $validation_type, $validation_min, $validation_max, $options, $flags},
        }

        if {[string equal $field_type "password"]} {
            emit {

                {FIELD_${upper_name}+100, "Retype Password",
                    $field_help,
                    $field_length, get_${name}, $validation_function,
                    $validation_type, $validation_min, $validation_max, $options, $flags},
            }
        }
    }

    emit {

        {0, NULL, NULL,
            0, NULL, NULL,
            0, 0, 0, 0, 0}
        \};
    }

    ##################################
    # Disable Hook function

    emit_newline
    emit {
        static void disable_fields(FORM * form) \{
    }

    # item_buf_count value determines if buffer for this item is already
    # defined.
    set item_buf_count 0
    set item_buf_list {}
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name "[string toupper $name]"
        set field_length "FIELD_${upper_access}_${upper_name}_LENGTH"
        set disable_item_count $item($idx,disable_item_count)

        if {$disable_item_count == 0} {

        } else {
            for {set dis_idx 1} {$dis_idx < $disable_item_count + 1} {incr dis_idx} {
                set disable_item $item($idx,disable,$dis_idx,name)
                foreach {index_dis} $index_list {
                    set name_dis $item($index_dis,name)
                    set upper_name_dis "[string toupper $name_dis]"
                    set field_length_dis "FIELD_${upper_access}_${upper_name_dis}_LENGTH"
                        if {[string equal $name_dis "${disable_item}"]} {
                            foreach {item_buf_val} $item_buf_list {
                                if {[string equal $item_buf_val "${disable_item}"]} {
                                    set item_buf_count 1
                                    break
                                } else {
                                    set item_buf_count 0
                                }
                            }

                            if { $item_buf_count == 0 } {
                            emit {
                                char ${name_dis}buf[$field_length_dis + 1];
                                get_field_buffer(form, FIELD_${upper_name_dis}, ${name_dis}buf);
                            }
                            lappend item_buf_list $disable_item
                            } else {

                            }
                        } else {

                        }
                }
            }
            # if_case value determines a if statement or continue by || (OR)ing
            #it. 0 means write a if statement.
            set if_case 0
            for {set dis_idx 1} {$dis_idx < $disable_item_count + 1} {incr dis_idx} {
                foreach value $item($idx,disable,$dis_idx,disable_value_list) {
                set disable_item $item($idx,disable,$dis_idx,name)
                    if { $if_case == 0 } {
                        set if_case 1
                        emit {
                            if((strcasecmp (${disable_item}buf, "${value}") == 0)
                        }
                    } else {
                        emit {
                            || (strcasecmp (${disable_item}buf, "${value}") == 0)
                        }
                    }
                }
            }
            emit {
                ) \{
                    set_formitem_options(form,FIELD_${upper_name},1);
                \}
                else \{
                    set_formitem_options(form,FIELD_${upper_name},0);
                \}
            }
        }
    }
    if { [llength $item_buf_list] > 0 } {
        emit {
            refresh_form_disable(form);
        }
    }
    emit {
        \}
    }


    ##################################
    # Menu access function

    emit_newline
    emit_newline
    emit {

        int configure${title}(MENUINFO *minfo) \{

        FORM                *form = NULL;
        int                 rc = E_OK;
        int                 retval = RC_NORMAL;
        char                error_string[128];
        struct ${access_struct} params;
        struct ${access_struct} orig_params;
        struct ${access_struct} init_params;
        void *              init_param_ptr = NULL;;
    }

    if ($board_access) {
        emit_newline
        emit {
            \{
            HAL *hp;
            hp = minfo->minfo_userptr;
            if (hp != NULL) \{
            board_number = hal_get_board_number(hp);
            \}
            \}
        }
    }

    emit_newline
    emit {
        show_title(minfo, NULL);

        memset(&init_params, 0x00, sizeof(init_params));
        if (paramsInitHook != NULL) \{
        init_params.userptr = minfo->minfo_userptr;
        (*paramsInitHook)(&init_params);
        setup_parameter_strings(&init_params);
        init_param_ptr = &init_params;
        \}

        form = create_form(itemdefs, ${vertical_offset}, 0, init_param_ptr);

        set_field_init(form, disable_fields);
        set_field_term(form, disable_fields);

        set_form_windows(minfo, form);

        post_form(form);
    }
    emit_newline
    if ($board_access) {
        emit {
            if (menuInitHook != NULL)
            (*menuInitHook)(minfo->minfo_userptr);
        }
    } else {
        emit {
            if (menuInitHook != NULL)
            (*menuInitHook)();
        }
    }
    emit_newline
    emit {
        while(1) \{

        rc = process_form(minfo, form, formmesg, 0);

        if (rc == RC_EXIT) \{
        retval = RC_EXIT;
        break;
        \}

        if(rc == RC_QUIT) \{
        retval = RC_NORMAL;
        break;
        \}

    }
    emit_newline
    if [string equal $config_subtype "immediate"] {
        emit {
            FIELD       *curr_field;
            FORMITEMDEF     *fitem;
            char tmpbuf[64];

            if (rc == RC_EXCEPT_FIELD) \{
            rc = RC_NORMAL;
            curr_field = current_field(form);
            fitem = field_userptr(curr_field);
            char * buf;
            buf = field_buffer(curr_field, 0);
            stripWhiteSpace(vBOTH, buf);
            sprintf(tmpbuf, "Set %s to %s?" YESNO_STR,
                fitem->formitem_name, buf);
            show_info(minfo, tmpbuf);
            if (get_yesno(minfo)) \{
            memset(&params, 0x00, sizeof(params));
            memset(&orig_params, 0x00, sizeof(orig_params));
        }
        emit_newline
        foreach {idx} $index_list {
            set name $item($idx,name)
            emit {
                get_${name}(orig_params.${name}, init_param_ptr, 0);
                orig_params.${name}_set = 1;
            }
        }

        emit {
            switch(fitem->formitem_ident) \{
        }
        emit_newline
        foreach {idx} $index_list {
            set name $item($idx,name)
            set upper_name [string toupper $name]
            emit {
                case FIELD_$upper_name:
                get_field_buffer(form, FIELD_${upper_name}, params.${name});
                params.${name}_set = 1;
                break;
            }
        }
        emit {
            \}
            setup_parameter_values(&orig_params);
            setup_parameter_values(&params);
            params.userptr = minfo->minfo_userptr;
            /* Call custom validator */
            if (validateHook != NULL)
            rc = (*validateHook)(&params, &orig_params, error_string);
            if (rc == RC_WARNING)
                rc = RC_NORMAL;
            if (rc == RC_NORMAL) \{
            rc = acquire_config_lock();
            if (rc == RC_NORMAL) \{

            rc = apply${title}(&params, &orig_params);
            release_config_lock();

            if (rc == RC_NORMAL) \{
        }

        emit_newline
        emit {
            remote_config_start${remote_config_suffix}();
            remote_config_set_config_name("$title");
            remote_config_set_command_name("set_${access}");
            remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
            remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);

            switch(fitem->formitem_ident) \{
        }
        emit_newline
        foreach {idx} $index_list {
            set name $item($idx,name)
            set upper_name [string toupper $name]
            set index [expr $idx - 1]
            emit {
                case FIELD_$upper_name:
                set_${name}(params.${name});
                remote_config_add_parameter(${access_options}[$index].name,
                                            params.${name});
                break;
            }
        }

        emit {
            \}
        }
        if ($board_access) {
            emit_newline
            emit {
                remote_config_set_board_number(board_number);
                remote_config_set_channel_number(board_number+1);
            }
        }
        emit_newline
        emit {
            params.userptr = minfo->minfo_userptr;
            remote_config_send();
        }
        emit_newline
        if ($board_access) {
            emit {
                if (savedHook != NULL)
                (*savedHook)(minfo->minfo_userptr);
            }
        } else {
            emit {
                if (savedHook != NULL)
                (*savedHook)();
            }
        }

        emit_newline
        emit {
            \}
            else \{
                show_info(minfo, "Failed: System Error.\\n"
                          PRESSANYKEY_STR);
                retval = RC_ERROR;
                get_anykey(minfo);
                break;
            \}
            \}
            else \{
            if(rc == RC_LOCKED_OUT)
                show_info(minfo,
                        "Cannot Update: System busy with other config change.\\n"
                        PRESSANYKEY_STR);
            else
                show_info(minfo,
                        "Cannot Update: System busy.\\n"
                        PRESSANYKEY_STR);
            get_anykey(minfo);
            retval = RC_ERROR;
            break;
            \}
            \}
            else \{
                char info_string[256];
                sprintf(info_string, "Failed: %s\\n%s", error_string,
                    PRESSANYKEY_STR);
                show_info(minfo, info_string);
                retval = RC_ERROR;
                get_anykey(minfo);
                break;
            \}
        }

        emit {
            \}
            \}
        }
    }

    emit {

        else \{

        if(get_form_status(form) == TRUE) \{
        show_info(minfo, "Update $description Info?" YESNO_STR);
        if (get_yesno(minfo)) \{
        rc = RC_NORMAL;
        memset(&params, 0x00, sizeof(params));
        memset(&orig_params, 0x00, sizeof(orig_params));
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            get_${name}(orig_params.${name}, init_param_ptr, 0);
            orig_params.${name}_set = 1;
        }
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type $item($idx,type)
        emit {
            get_field_buffer(form, FIELD_${upper_name}, params.${name});
            params.${name}_set = 1;
        }
        if {[string equal $field_type "password"]} {
            emit {
                get_field_buffer(form, FIELD_${upper_name}+100, error_string);
                if (strcmp(params.${name}, error_string) != 0) \{
                rc = RC_INVALID_VALUE;
                strcpy(error_string, "Password Mismatch.");
                \}

            }
        }
    }

    emit_newline
    emit {
        setup_parameter_values(&orig_params);
        setup_parameter_values(&params);
        params.userptr = minfo->minfo_userptr;

        /* Call custom validator */
        if (rc == RC_NORMAL) \{
        sprintf(error_string, "Invalid Data");
        if (validateHook != NULL) \{
        rc = (*validateHook)(&params, &orig_params, error_string);

        if (rc == RC_WARNING) \{


        char info_string[256];
        sprintf(info_string, "WARNING: %s\\nProceed anyway? %s", error_string,
                YESNO_STR);
        show_info(minfo, info_string);

        if (get_yesno(minfo)) \{
        rc = RC_NORMAL;
        \} else \{
        show_info(minfo, "Changes Aborted.\\n" PRESSANYKEY_STR);
        get_anykey(minfo);
        break;
        \}
        \}
        \}

        \}
        if( rc == RC_NORMAL) \{
        rc = acquire_config_lock();
        if (rc == RC_NORMAL) \{
        rc = apply${title}(&params, &orig_params);
        release_config_lock();

        if (rc == RC_NORMAL) \{
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        emit {
            set_${name}(params.${name});
        }
    }

    emit_newline
    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(minfo->minfo_userptr);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)();
        }
    }

    emit_newline
    emit {
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name("$title");
        remote_config_set_command_name("set_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);

    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set index [expr $idx - 1]
        emit {
            remote_config_add_parameter(${access_options}[$index].name,
                                        params.${name});
        }
    }

    if ($board_access) {
        emit_newline
        emit {
            remote_config_set_board_number(board_number);
            remote_config_set_channel_number(board_number+1);
        }
    }


    emit_newline
    emit {
        remote_config_send();

        if (paramsInitHook != NULL) \{
            init_params.userptr = minfo->minfo_userptr;
            (*paramsInitHook) (&init_params);
            setup_parameter_strings(&init_params);
        \}

        refresh_form(form);
        show_info(minfo, "$description Info Updated.\\n"
                  PRESSANYKEY_STR);
        \}
        else \{
            show_info(minfo, "Failed: System Error.\\n"
                      PRESSANYKEY_STR);
            retval = RC_ERROR;
        \}
        \}
        else \{
        if(rc == RC_SYSTEM_BUSY)
            show_info(minfo,
                    "Cannot Update: System busy.\\n"
                    PRESSANYKEY_STR);
        else if(rc == RC_LOCKED_OUT)
            show_info(minfo,
                    "Cannot Update: System busy with other config change.\\n"
                    PRESSANYKEY_STR);
        else \{
            show_info(minfo, "Failed: System Error.\\n"
                      PRESSANYKEY_STR);
        \}
        retval = RC_ERROR;
        \}
        \}
        else \{
            char info_string[256];
            sprintf(info_string, "Failed: %s\\n%s", error_string,
                PRESSANYKEY_STR);
            show_info(minfo, info_string);
            retval = RC_ERROR;
        \}
        get_anykey(minfo);
        break;
        \}
        \}
        else \{
        show_info(minfo, "No change in data. Quit?" YESNO_STR);
        if (get_yesno(minfo))
        break;
        \}

        \}

        \}
    }
    emit_newline
    if ($board_access) {
        emit {
            if (menuExitHook != NULL)
            (*menuExitHook)(minfo->minfo_userptr);
        }
    } else {
        emit {
            if (menuExitHook != NULL)
            (*menuExitHook)();
        }
    }
    emit_newline
    emit {

    /* cleanup */
    unpost_form(form);
    unset_form_windows(form);
    delete_form(form);

    return retval;

        \}

    }

    ##################################
    # The DESCRIBE CLI function
    emit_newline
    emit_newline
    emit {

	int describe_${access}_main(int argc, char **argv) \{

	int rc = RC_NORMAL;
	char buf[8192];
	int ctr=0;
    }

    emit {
        printf(XML_MENUDESCR_ROOTBEGIN);
        printf("\\n");
    }

    emit {
        printf("   " XML_MENUDESCR_ITEMTYPE, "config");
        printf("   " XML_MENUDESCR_TITLE, "${title}");
        printf("   " XML_MENUDESCR_DESCRIPTION, "${description}");
        printf("   " XML_MENUDESCR_BOARDACCESS, ${board_access} ? "true" : "false");
    	printf("   " XML_MENUDESCR_CLICOMMANDNAME, "${cli_command_name}");
    	printf("   " XML_MENUDESCR_CONFIGFILENAME, "${config_file}");
        }


    emit {
        printf("\\n");
        printf("   " XML_MENUDESCR_ITEMSROOTBEGIN);
    }
    foreach {idx} $index_list {
	set name $item($idx,name)
	emit {
            printf("      " XML_FIELD_START);
	}
	emit {
            printf(XML_FIELD, 
            "$item($idx,name)" , "$item($idx,label)" , "$item($idx,type)",
            "$item($idx,fieldlength)" , "$item($idx,default)" );
        }
	if [string equal $item($idx,validation) "netmask"] {
		emit {
			printf (XML_FIELD_NETMASK,
			"$item($idx,validation)");
		}
	}
	if [string equal $item($idx,validation) "enumeration"] {
		emit {
			printf (XML_FIELD_ENUMERATION,
			"$item($idx,validation)",
			"$item($idx,validation_value_list_csv)");
		}
	}
	if [string equal $item($idx,validation) "ipaddress"] {
		emit {
			printf (XML_FIELD_IPADDRESS,
			"$item($idx,validation)");
		}
	}
	if [string equal $item($idx,validation) "range"] {
		emit {
			printf (XML_FIELD_RANGE,
			"$item($idx,validation)", 
			"$item($idx,validation_min)", 
			"$item($idx,validation_max)");
		}
	}
	emit {
            printf(XML_FIELD_END);
	}

    }

    emit {
         printf("\\n");
         printf("   " XML_MENUDESCR_ITEMSROOTEND);
    }

    emit {
         printf("\\n");
         printf(XML_MENUDESCR_ROOTEND);
    }


    emit {
	return rc;
	\}
    }


    ##################################
    # The GET CLI function
    emit_newline
    emit_newline
    emit {

        int get_${access}_main(int argc, char **argv) \{

        int opt_idx = 1;
        int rc = RC_NORMAL;
        char buf[8192];
        int ctr=0;
        struct ${access_struct} init_params;
        void * init_param_ptr = NULL;
        HAL *hp = NULL;
    }

    if ($board_access) {
        emit {
            int c, opt_index = 0;

            while(1) \{
            c = getopt_long_only(argc, argv, "h", ${access_options}, &opt_index);
            if (c == -1) \{
            break;
            \}
            else if (c == 0) \{

            switch(opt_index) \{
            case $channel_index:
            board_number = atoi(optarg) - 1;
            break;
            \}
            \}
            else \{

            rc = RC_ERROR;
            break;

            \}
            \}
            opt_idx = optind;
        }
    }


    if ($board_access) {
        emit_newline
        emit {
            hp = hal_acquire(board_number);
            if (hp == NULL) {
                print_cli_failure(get_${access}, CLI_REASON_WRONG_HW);
                return RC_ERROR;
            }
        }
    }

    emit_newline
    emit {
        memset(&init_params, 0x00, sizeof(init_params));
        if (paramsInitHook != NULL) \{
        init_params.userptr = hp;
        (*paramsInitHook)(&init_params);
        setup_parameter_strings(&init_params);
        init_param_ptr = &init_params;
        \}

    }

    emit {

        if (opt_idx == argc) \{

        printf(XML_GROUP, get_${access});
        print_cli_success(NULL);
        printf("  " XML_OBJENTRY, ${access}_params);

    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            get_${name}(buf, init_param_ptr, 0);
            printf("      " XML_ATTRSTR, ${access_options}[ctr++].name, buf);
        }
    }

    emit_newline
    emit {
        printf("  " XML_OBJEND);
        printf(XML_GROUPEND);
        \}
        else \{
        rc = RC_ERROR;
        print_cli_failure(get_${access}, CLI_REASON_BAD_USAGE);
        printf("\\nUsage: get_${access} ${channel_help}\\n");
        printf("Get $description\\n");
        \}
    }

    if ($board_access) {
        emit_newline
        emit {
            if (hp != NULL)
            hal_release(hp);
        }
    }
    emit {
        return rc;
        \}
    }


    ##################################
    # The SET CLI function
    emit_newline
    emit_newline
    emit {
        int set_${access}_main(int argc, char **argv) \{

        int c, opt_index = 0;
        int rc = RC_NORMAL;
        int value;
        char error_string[128];
        struct ${access_struct} params;
        struct ${access_struct} orig_params;
        struct ${access_struct} init_params;
        void *init_param_ptr = NULL;
        char * bad_value_string;
        char buf[64];
        HAL *hp = NULL;
    }

    emit_newline
    emit {
        value = 0;
        invoked_via_cli = 1;
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_params);
        remote_config_set_command_name("set_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

        memset(&params, 0x00, sizeof(params));
        memset(&orig_params, 0x00, sizeof(orig_params));

        if (view_only() != 0) \{
        print_cli_failure(set_${access}, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to set $description parameters\\n");
        return RC_ERROR;
        \}

        sprintf(buf, "%s:", CLI_REASON_BAD_VALUE);
        bad_value_string = strdup("\\n");

        while(1) \{
        c = getopt_long_only(argc, argv, "h", ${access_options}, &opt_index);
        if (c == -1) \{
        break;
        \}
        else if (c == 0) \{

        switch(opt_index) \{
    }

    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type $item($idx,type)
        set field_validation "$item($idx,validation)"
        set field_length "FIELD_${upper_access}_${upper_name}_LENGTH"

        set index [expr $idx - 1]

        emit {
            case $index:
        }
        emit {
            if (strlen(optarg) > $field_length)  \{
            rc = RC_INVALID_VALUE;
            sprintf(buf, "Field \'${name}\' length exceeds %d\\n", $field_length);
            bad_value_string = strgrowcat(bad_value_string, buf);
            \}
        }
        if {[string equal $field_type "string"]} {
            if {[string equal $field_validation "ipaddress"]} {
                emit {
                    else if (check_ip(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed ipaddress validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }

            } elseif {[string equal $field_validation "ipaddressorblank"]} {
                emit {
                    else if (check_ipaddorblank(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed ipaddressorblank validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "inetname"]} {
                emit {
                    else if (check_inetname(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed inetname validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }

            } elseif {[string equal $field_validation "hostnameorblank"]} {
                emit {
                    else if (check_hostnameorblank(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed hostname validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }

            } elseif {[string equal $field_validation "domainname"]} {
                emit {
                    else if (check_domainname(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed domainname validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "netmask"]} {
                emit {
                    else if (check_netmask(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed netmask validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "multicast"]} {
                emit {
                    else if (check_multicastip(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed multicast validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "regexp"]} {
                emit {
                    else if (check_regex(optarg, regexp_$item($idx,validation_value)) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "enumeration"]} {
                emit {
                    else if (check_enum(optarg, ${name}_values) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed to match enumerated value\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "hexadecimal"]} {
                emit {
                    else if (check_hexadecimal(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed hexadecimal validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                emit {
                    else if (check_onlyhexadecimal(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    sprintf(buf, "Field \'${name}\' failed onlyhexadecimal validation\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                    \}
                }
            } else {
                Warning "No implementation for validation type $field_validation in $name??"
                emit {
                    else if (1)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                    else \{
                    rc = RC_INVALID_VALUE;
                    \}
                }
            }
        } elseif {[string equal $field_type "boolean"]} {
            emit {
                else if (check_enum(optarg, yesnochecklist) == RC_NORMAL)  \{
                params.${name}_set = 1;
                strncpy(params.${name}, optarg,
                        $field_length + 1);
                \}
                else {
                    sprintf(buf, "Field \'${name}\' should be Yes/No\\n");
                    bad_value_string = strgrowcat(bad_value_string, buf);
                    rc = RC_INVALID_VALUE;
                }
            }
        } elseif {[string equal $field_type "password"]} {
            emit {
                else if (check_regex(optarg, regexp_password) == RC_NORMAL)  \{
                params.${name}_set = 1;
                strncpy(params.${name}, optarg,
                        $field_length + 1);
                \}
                else \{
                sprintf(buf, "Field \'${name}\' failed password validation\\n");
                bad_value_string = strgrowcat(bad_value_string, buf);
                rc = RC_INVALID_VALUE;
                \}
            }
        } elseif {[string equal $field_type "integer"]} {
            emit {
                else if ((value = atoi(optarg)) <= $item($idx,validation_max) && \
                             value >= $item($idx,validation_min))  \{
                params.${name}_set = 1;
                snprintf(params.${name}, $field_length + 1,
                "%d", atoi(optarg));
                \}
                else \{
                sprintf(buf, "Field \'${name}\' should be between %d and %d\\n",
                        $item($idx,validation_min), $item($idx,validation_max));
                bad_value_string = strgrowcat(bad_value_string, buf);
                rc = RC_INVALID_VALUE;
                \}
            }

        } elseif {[string equal $field_type "float"]} {
            emit {
                else if (atof(optarg) <= $item($idx,validation_max) && \
                             atof(optarg) >= $item($idx,validation_min))  \{
                params.${name}_set = 1;
                strncpy(params.${name}, optarg,
                        $field_length + 1);
                \}
                else \{
                sprintf(buf, "Field \'${name}\' should be between %f and %f\\n",
                        $item($idx,validation_min), $item($idx,validation_max));
                bad_value_string = strgrowcat(bad_value_string, buf);
                rc = RC_INVALID_VALUE;
                \}
            }

        } elseif {[string equal $field_type "bandwidth"]} {
            emit {
                else if (check_regex(optarg, regexp_bandwidth) == RC_NORMAL)  \{
                params.${name}_set = 1;
                strncpy(params.${name}, optarg,
                        $field_length + 1);
                \}
                else \{
                sprintf(buf, "Field \'${name}\' failed bandwidth validation\\n");
                bad_value_string = strgrowcat(bad_value_string, buf);
                rc = RC_INVALID_VALUE;
                \}
            }
        }

        emit {
            break;
        }

    }

    emit {
        case $item_count:
        remote_config_set_client_data(optarg);
        break;
    }

    if ($board_access) {
        emit {
            case $channel_index:
            board_number = atoi(optarg) - 1;
            break;
        }
    }
    emit {
        \}
        \}
        else \{
        rc = RC_ERROR;
        break;
        \}
        \}
    }

    if ($board_access) {
        emit_newline
        emit {
            hp = hal_acquire(board_number);
            remote_config_set_board_number(board_number);
            remote_config_set_channel_number(board_number+1);
            if (hp == NULL) \{
            print_cli_failure(set_${access}, CLI_REASON_WRONG_HW);
            remote_config_send_error(CLI_REASON_WRONG_HW);
            return RC_ERROR;
            \}
        }
    }

    emit_newline
    emit {
        params.userptr = hp;
        orig_params.userptr = hp;
    }

    emit_newline
    emit {
        memset(&init_params, 0x00, sizeof(init_params));
        if (paramsInitHook != NULL) \{
        init_params.userptr = hp;
        (*paramsInitHook)(&init_params);
        setup_parameter_strings(&init_params);
        init_param_ptr = &init_params;
        \}
    }

    emit_newline
    emit {
        if (optind == argc && rc == RC_NORMAL) \{
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            get_${name}(orig_params.${name}, init_param_ptr, 0);
            orig_params.${name}_set = 1;
        }
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set index [expr $idx - 1]
        set upper_name [string toupper $name]
        set field_length "FIELD_${upper_access}_${upper_name}_LENGTH"
        emit {
            if (params.${name}_set == 1) \{
            remote_config_add_parameter(${access_options}[${index}].name,
                                        params.${name});
            \}
            else \{
            strncpy(params.${name}, orig_params.${name},
                    $field_length + 1);
            \}
        }
    }

    emit_newline
    emit {
        setup_parameter_values(&orig_params);
        setup_parameter_values(&params);
        sprintf(error_string, "Invalid Data");


        if (validateHook != NULL)
        rc = (*validateHook)(&params, &orig_params, error_string);
        if (rc == RC_WARNING)
        rc = RC_NORMAL;

        if (rc != RC_NORMAL) \{
        print_cli_failure(set_${access}, error_string);
        remote_config_send_error(error_string);
        return rc;
        \}
        rc = acquire_config_lock();
        if (rc != RC_NORMAL) \{
        char system_busy_string [64];
        if (rc == RC_LOCKED_OUT)
        sprintf(system_busy_string, "\\n Cannot Update: System busy with other config change.\\n");
        else
        sprintf(system_busy_string, "\\n Cannot Update: System busy.\\n");
        print_cli_failure_with_detail(set_${access}, CLI_REASON_SYSTEM_BUSY, system_busy_string);
        remote_config_send_error(CLI_REASON_SYSTEM_BUSY);
        return RC_ERROR;
        \}
    }



    emit_newline
    emit {
        rc = apply${title}(&params, &orig_params);
        release_config_lock();
        if (rc == RC_NORMAL) \{
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            if (params.${name}_set == 1) \{
            set_${name}(params.${name});
            \}
        }
    }

    emit_newline
    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(hp);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)();
        }
    }

    emit {
            print_cli_success(set_${access});
            remote_config_send();
        \}
        else \{
            print_cli_failure(set_${access}, CLI_REASON_SYSTEM_ERR);
            remote_config_send_error(CLI_REASON_SYSTEM_ERR);
            rc = RC_ERROR;
        \}
        \}
        else if (rc == RC_INVALID_VALUE) \{
        print_cli_failure_with_detail(set_${access}, CLI_REASON_BAD_VALUE,
                                      bad_value_string);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
        \}
        else \{
        print_cli_failure(set_${access}, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        printf("\\nUsage: set_${access} [OPTIONS]\\n");
        printf("Set $description \\n");
        printf("Options:\\n");
    }

    foreach {idx} $index_list {
        set name $item($idx,name)
        set field_label "$item($idx,label)"
        set field_help "$item($idx,help)"
        set field_type $item($idx,type)
        set field_validation "$item($idx,validation)"
        set value "Value"
        if {[string equal $field_type "string"]} {
            if {[string equal $field_validation "ipaddress"]} {
                set value "IpAddr"
            } elseif {[string equal $field_validation "netmask"]} {
                set value "NetMask"
            } elseif {[string equal $field_validation "enumeration"]} {
                set list [join $item($idx,validation_value_list) ", "]
                append field_help ": Choice of $list "
            }
        } elseif {[string equal $field_type "boolean"]} {
            set value "Yes/No"
        } elseif {[string equal $field_type "password"]} {
            set value "Password"
        } elseif {[string equal $field_type "integer"]} {
            set value "Number"
        }
        set spaces [string repeat " " [expr 25 - [string length "${name} ${value}"]]]
        set safety 0
        while {$safety < 5} {
            if {[string length $field_help] == 0} {
                break
            } elseif {[string length $field_help] > 40} {
                set index [string last " " $field_help 40]
                if {$index == 0} {
                    set index [string last "," $field_help 40]
                }
                set help_string [string range $field_help 0 $index]
                set field_help [string range $field_help [expr $index + 1] end]
            } else {
                set help_string $field_help
                set field_help ""
            }

            if {$safety == 0} {
                emit {
                    printf("        -${name} $value ${spaces} ${help_string} \\n");
                }
            } else {
                emit {
                    printf("                                    ${help_string} \\n");
                }
            }
            incr safety
        }
    }

    if ($board_access) {
        emit {
            printf("        ${channel_help} \\n");
        }
    }
    emit {
        \}
    }

    if ($board_access) {
        emit_newline
        emit {
            if (hp != NULL)
            hal_release(hp);
        }
    }

    emit {
        return rc;
        \}
    }

    emit_newline
    emit_newline
    emit {
        int load_${access}_main(int argc, char **argv) \{

        verbose_errors = 1;
        load${title}();
        return 0;
        \}

    }

    ##################################
    # The  load function
    emit_newline
    emit_newline
    emit {
        int load${title}(void) \{
        struct ${access_struct} params;
        struct ${access_struct} orig_params;
        HAL *hp = NULL;
        int rc = RC_NORMAL;
    }

    if ($board_access) {
        emit_newline
        emit {
            int number_of_boards = 1;
            int board;
            hp = hal_acquire(0);
            if (hp == NULL)
            return 0;
            number_of_boards = hal_get_number_of_boards(hp);
            hal_release(hp);

            for (board = 0; board < number_of_boards; board++) \{
            board_number = board;
            hp = hal_acquire(board);
            if (hp == NULL)
            continue;

        }
    }

    emit_newline
    emit {
        memset(&params, 0x00, sizeof(params));
        memset(&orig_params, 0x00, sizeof(params));
        params.userptr = hp;

        /* Retrieve the params but leave the orig_params zeroed out so
        that the apply handler is always forced to apply */
        retrieve${title}Values(&params);

        /* Call the apply handler */
        rc = acquire_config_lock();
        if (rc == RC_NORMAL) \{
        rc = apply${title}(&params, &orig_params);
        release_config_lock();
        if (rc != RC_NORMAL)
           VLOG_DEBUG("Failed: System Error.");
        \}
        else if (rc == RC_LOCKED_OUT)
        VLOG_DEBUG("Cannot Update: System busy with other config change.");
        else
        VLOG_DEBUG("Cannot Update: System busy.");
    }

    if ($board_access) {
        emit_newline
        emit {
            if (hp != NULL)
            hal_release(hp);
            \}
        }

    }
    emit_newline
    emit {
        return 0;
        \}
    }

    ##################################
    # The  Dump function
    emit_newline
    emit_newline
    emit {

        int dump${title}(void) \{

        char buf[8192];
        int ctr=0;
        struct ${access_struct} init_params;
        void *init_param_ptr = NULL;
        HAL *hp = NULL;
    }

    if ($board_access) {
        emit_newline
        emit {
            int number_of_boards = 1;
            int board;
            hp = hal_acquire(0);
            if (hp == NULL)
            return 0;
            number_of_boards = hal_get_number_of_boards(hp);
            hal_release(hp);

            for (board = 0; board < number_of_boards; board++) \{
            board_number = board;
            hp = hal_acquire(board);
            if (hp == NULL)
            continue;
            ctr = 0;

        }
    }

    emit_newline
    emit {
        memset(&init_params, 0x00, sizeof(init_params));
        if (paramsInitHook != NULL) \{
        init_params.userptr = hp;
        (*paramsInitHook)(&init_params);
        setup_parameter_strings(&init_params);
        init_param_ptr = &init_params;
        \}
    }

    emit_newline
    emit {
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_params);
        remote_config_set_command_name("set_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            get_${name}(buf, init_param_ptr, 0);
            remote_config_add_parameter(${access_options}[ctr++].name, buf);
        }
    }

    if ($board_access) {
        emit_newline
        emit {
            remote_config_set_board_number(board_number);
            remote_config_set_channel_number(board_number+1);
        }
    }

    emit_newline
    emit {
        remote_config_send();
    }

    if ($board_access) {
        emit_newline
        emit {
            if (hp != NULL)
            hal_release(hp);
            \}
        }

    }
    emit_newline
    emit {
        return 0;
        \}
    }

    emit {
        #endif /* LIBONLY */
    }
}


proc create_config_doc {root base_name} {


    set title [get_text_node_value $root "/Menu/Title"]
    set description [get_text_node_value $root "/Menu/Description"]
    set access [get_text_node_value $root "/Menu/Access"]
    set upper_access [string toupper $access]
    set config_file [get_text_node_value $root "/Menu/ConfigFile"]
    set config_file "/V2O/config/${config_file}"
    set config_type [get_text_node_value $root "/Menu/Type"]
    set vertical_offset 1
    if [check_node_exists $root "/Menu/VerticalOffset"] {
       set vertical_offset [get_text_node_value $root "/Menu/VerticalOffset"]
    }


    if [check_node_exists $root "/Menu/SubType"] {
        set config_subtype [get_text_node_value $root "/Menu/SubType"]
    } else {
        set config_subtype "normal"
    }

        # Get count of the number of parameter items
    set item_count 0
    set nodes [$root selectNodes "/Menu/Items/Item" type]
    if { $type == "nodes" } {
        set item_count [llength $nodes]
    }

    set board_access 0
    if [check_node_exists $root "/Menu/BoardAccess"] {
       set value [get_text_node_value $root "/Menu/BoardAccess"]
        if [string equal $value "true"] {
            set board_access 1
        } elseif [string equal $value "false"] {
            set board_access 0
        } else {
            Error "Value \"$value\" not supported for BoardAccess. Should be true or false"
        }
    }
    set skip_doc 0
    if [check_node_exists $root "/Menu/SkipDocs"] {
       set value [get_text_node_value $root "/Menu/SkipDocs"]
        if [string equal $value "true"] {
            set skip_doc 1
        } elseif [string equal $value "false"] {
            set skip_doc 0
        } else {
            Error "Value \"$value\" not supported for SkipDoc. Should be true or false"
        }
    }


    # Create list of indexes
    set index_list {}
    for {set idx 1} {$idx < $item_count + 1} {incr idx} {
        lappend index_list $idx
    }

      # Retrieve all needed parameter (Item) data
    foreach {idx} $index_list {
        set item($idx,name) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Name"]
        set item($idx,type) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Type"]
        set item($idx,label) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Label"]
        set item($idx,fieldlength) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/FieldLength"]
        set item($idx,default) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/DefaultValue"]
        set item($idx,help) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Help"]
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Align"] {
            set item($idx,align) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Align"]
        } else {
            set item($idx,align) "normal"
        }

        if [string equal $config_subtype "immediate"] {
            if [string equal $item($idx,type) "integer"] {

            } elseif [string equal $item($idx,type) "boolean"] {

            } elseif [string equal $item($idx,type) "string"] {

            } elseif [string equal $item($idx,type) "float"] {

            } elseif [string equal $item($idx,type) "bandwidth"] {

            } else {
                Error "Node type \"$item($idx,type)\" not supported for config type immediate"
            }

        } elseif [string equal $config_subtype "normal"] {

        } else {
            Error "SubType  \"$config_subtype\" is invalid, should be \"normal\" or \"immediate\"."
        }

        if [string equal $item($idx,align) "normal"] {

        } elseif [string equal $item($idx,align) "follow"] {

        } else {
            Error "Align tag value \"$item($idx,align)\" is invalid, should be \"normal\" or \"follow\"."
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Validation"] {
            set item($idx,validation) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Type"]
            if [string equal $config_subtype "immediate"] {
                if [string equal $item($idx,validation) "ipaddress"] {
                    Error "Do not support \"$item($idx,validation)\" validation type while generating above _gen.c file"
                } elseif [string equal $item($idx,validation) "netmask"] {
                    Error "Do not support \"$item($idx,validation)\" validation type while generating above _gen.c file"
                    }
            }
            if [string equal $item($idx,validation) "range"] {
                set item($idx,validation_min) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Min"]
                set item($idx,validation_max) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Max"]
            } elseif [string equal $item($idx,validation) "regexp"] {
                set item($idx,validation_value) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Value"]
            } elseif [string equal $item($idx,validation) "enumeration"] {
                set item($idx,validation_values) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Values"]
                set field_validation_values [split $item($idx,validation_values) ","]
                set value_list {}
                foreach value $field_validation_values {
                    set val [string trim ${value} " \""]
                    lappend value_list $val
                }
                set item($idx,validation_value_list) $value_list
            } else {

            }
        } else {
            set item($idx,validation) "none"
        }
     }
     set channel_help ""
     if {$board_access} {
        set channel_index [expr $item_count + 1 ]
        set channel_help "-channelNumber Number"
     }

     if {$skip_doc == 0} {

       emit_newline_doc
       emit_doc {
          == $title
       }

       emit_newline_doc
       emit_doc {

Type:: Configuration
Category:: $title
Description:: $description
       }

       emit_newline_doc
       emit_doc {
=== Get Configuration

Command Name:: get_${access}
Description:: Gets the current configiration for $title
       }

       if ($board_access) {
            emit_doc {

Options:: ${channel_help}
            }
       } else {
           emit_doc {

Options:: None
           }
       }

       emit_newline_doc
       emit_doc {
=== Set Configuration

Command Name:: set_${access} Options[]
Description:: Sets the configuration for $title
       }

       if ($board_access) {
            emit_doc {

Options:: ${channel_help}
            }
       } else {
           emit_doc {

Options:: -
           }
       }

       emit_doc {
         [cols="1,1,3",options="header"]
       }

       emit_doc {
           |===============================================================================
	        	 | NAME        | VALUE            |HELP STRING
       }

       foreach {idx} $index_list {
           set name $item($idx,name)
           set field_label "$item($idx,label)"
           set field_help "$item($idx,help)"
           set field_type $item($idx,type)
           set field_validation "$item($idx,validation)"
           set value "Value"
           if {[string equal $field_type "string"]} {
               if {[string equal $field_validation "ipaddress"]} {
                   set value "IpAddr"
               } elseif {[string equal $field_validation "netmask"]} {
                   set value "NetMask"
               } elseif {[string equal $field_validation "enumeration"]} {
                   set list [join $item($idx,validation_value_list) ", "]
                   append field_help ": Choice of $list "
               }
           } elseif {[string equal $field_type "boolean"]} {
               set value "Yes/No"
           } elseif {[string equal $field_type "password"]} {
               set value "Password"
           } elseif {[string equal $field_type "integer"]} {
               set value "Number"
           }
           set help_string $field_help

           emit_newline_doc
           emit_doc {

                  | -${name} |$value | ${help_string}
           }
       }
       emit_doc {
          |================================================================================
       }
   }
}

