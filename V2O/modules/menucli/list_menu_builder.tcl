



proc create_list_cli {root base_name} {


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

    set select_item_callback 0
    if [check_node_exists $root "/Menu/SelectItemCallback"] {
       set value [get_text_node_value $root "/Menu/SelectItemCallback"]
        if [string equal $value "true"] {
            set select_item_callback 1
        } elseif [string equal $value "false"] {
            set select_item_callback 0
        } else {
            Error "Value \"$value\" not supported for SelectItemCallback. Should be true or false"
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
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/ViewList"] {
            set item($idx,viewlist) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/ViewList"]
        } else {
            set item($idx,viewlist) "yes"
        }
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Align"] {
            set item($idx,align) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Align"]
        } else {
            set item($idx,align) "normal"
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/DisplayOnly"] {
            set item($idx,displayonly) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/DisplayOnly"]
        } else {
            set item($idx,displayonly) "false"
        }

        if [string equal $item($idx,align) "normal"] {

        } elseif [string equal $item($idx,align) "follow"] {

        } else {
            Error "Align tag value \"$item($idx,align)\" is invalid, should be \"normal\" or \"follow\"."
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Validation"] {
            set item($idx,validation) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Type"]
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

        Debug "Parameter: $item($idx,name)"
    }


    ##################################
    # Create header file
    set upper_base_name [string toupper $base_name]
    emit_header {
        #ifndef __${upper_base_name}_H_
        #define __${upper_base_name}_H_
    }
    emit_header {
        #include <stdio.h>
        #include "mylist.h"
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
                    enum ${access}_${name} \{
                }
                foreach value $item($idx,validation_value_list) {
                    set val [string toupper $value]
                    set val [string map {/ "_BY_"} $val]
                    set val [string map {" " "_"} $val]
                    set val [string map {":" "_COLON_"} $val]
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
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type "$item($idx,type)"
        set field_validation "$item($idx,validation)"
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"

        if {[string equal $field_type "string"] && [string equal $field_validation "enumeration"] } {
            emit_header_newline
            emit_header {
                /* enum for $name */
            }
        }
    }


    # Parameter access struct definition
    set access_struct "${access}_params"

    emit_header_newline
    emit_header {
        struct ${access_struct} \{
        int     ${access}_id;
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
        } elseif {[string equal $field_type "integerorany"] } {
            emit_header {
                int     ${name}_value;
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

    emit_header_newline
    emit_header {
        /* The next pointer for linked list */
        struct ${access_struct} * next;
    }

    emit_header {
        \};
    }

    emit_header_newline
    emit_header {
        /* List iterator helpers */
        extern struct ${access_struct} * first_${access}_entry(void);
        extern struct ${access_struct} * next_${access}_entry(struct ${access_struct} * params);
        #define foreach_${access}_entry(params) \\
            for ((params) = first_${access}_entry(); \\
                     (params) != NULL; (params) = next_${access}_entry((params)))
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
            typedef void ${title}SavedFunction(int req_code, void * userptr);
        }
    } else {
        emit_header {
            typedef void ${title}SavedFunction(int req_code);
        }
    }

    emit_header {
        extern void register${title}SavedFunction(${title}SavedFunction *funcPtr);
    }
    emit_header_newline
    emit_header {
        /* Custom paramater default initialization function prototype */
        typedef void ${title}ParamsDefaultsInitFunction(struct ${access_struct} *params);

        extern void register${title}ParamsDefaultsInitFunction(${title}ParamsDefaultsInitFunction *funcPtr);

    }
    emit_header_newline
    emit_header {
        /* Prototype for apply${title} function */
        extern int validate${title}(int req_code,
                                    struct ${access_struct} *params,
                                    struct ${access_struct} *orig_params,
                                    char * error_string);

        /* Prototype for apply${title} function */
        extern int apply${title}(int req_code,
                                 struct ${access_struct} *params,
                                 struct ${access_struct} *orig_params);

    }

    if ($select_item_callback) {
        emit_header_newline
        emit_header {
        /* Prototype for select${title} function */
        #ifdef CUSTOM_MENU_DEFS
        #include "custmenu.h"
        extern int select${title}(MENUINFO *minfo, struct ${access_struct} *params,
                                  char * error_string);
        #endif
       }
    }
    emit_header_newline
    if {$board_access} {
        emit_header {
            /* Prototype for retrieve${title}Values function */
            extern int retrieve${title}Values(HAL *hp);
        }
    } else {
        emit_header {
            /* Prototype for retrieve${title}Values function */
            extern int retrieve${title}Values(void);
        }

    }
    emit_header_newline
    emit_header {
        /* Prototype for count${title}Values function */
        extern int count${title}Values(void);
    }

    emit_header_newline
    emit_header {
        /* Prototype for constructRow${title} function */
        extern char * constructRow${title}(char *buffer, struct ${access_struct} *params);
    }

    emit_header_newline
    emit_header {
        /* Prototype for constructHeader${title} function */
        extern char * constructHeader${title}(char *buffer);
    }


    emit_header_newline
    emit_header {
        /* Prototype for load${title} function. */
        extern int load${title}(void);
    }

    emit_header_newline
    emit_header {
        /* Prototypes for CLI access entry point functions */
        extern int get_${access}_main(int argc, char **argv);
        extern int load_${access}_main(int argc, char **argv);
        extern int add_${access}_main(int argc, char **argv);
        extern int del_${access}_main(int argc, char **argv);
        extern int mod_${access}_main(int argc, char **argv);
        extern int describe_${access}_main(int argc, char **argv);

    }

    if {![string equal $cli_command_name "undefined"]} {
        emit_header_newline
        emit_header {
        /* These set up CLI access name mappings that get plugged into vmain.c */
        /*
        CLI_COMMAND_MAP    {"get_${cli_command_name}", get_${access}_main},
        CLI_COMMAND_MAP    {"load_${cli_command_name}", load_${access}_main},
        CLI_COMMAND_MAP    {"add_${cli_command_name}", add_${access}_main},
        CLI_COMMAND_MAP    {"del_${cli_command_name}", del_${access}_main},
        CLI_COMMAND_MAP    {"mod_${cli_command_name}", mod_${access}_main},
        CLI_COMMAND_MAP    {"describe_${cli_command_name}", describe_${access}_main},
        */
        }
    }

    emit_header_newline
    emit_header {
        /* Prototypes for dump${title} function */
        extern int dump${title}(void);
    }

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

        #include <sys/socket.h>
        #include <arpa/inet.h>
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
        #include "validationroutines.h"
        #include "text_utils.h"
        #include "configfile_parser.h"
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
        static char *get_${access} = "Get $description Table";
        static char *add_des_${access} = "Add $description Entry";
        static char *mod_des_${access} = "Modify $description Entry";
        static char *del_des_${access} = "Delete $description Entry";
        static char *${access}_title = "$title";

        static int update_${access}_dialog(MENUINFO *minfo, int idx, int req_code);
        static int delete_${access}(int idx, int *${access}_idp);
        static int update_${access}_entry(int idx, struct ${access_struct} *params);
    }

    emit {
        static char *${access}_help = "Press </U>A<!U> to Add, </U>D<!U> to Delete, </U>M<!U> to Modify,\\n</U>Enter<!U> to Browse or </U>Q<!U> to Quit. ";

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

    set access_delete_options "${access}_delete_options"

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

    emit {
        #endif /* LIBONLY */
    }

    emit_newline
    emit {
        static void setup_parameter_strings(struct ${access_struct} *params);
    }

    ##################################
    # Custom hooks register functions
    emit_newline
    emit {
        static ${title}ParamsDefaultsInitFunction *paramsDefaultsInitHook = NULL;
        extern void register${title}ParamsDefaultsInitFunction(${title}ParamsDefaultsInitFunction *funcPtr) \{
        if (funcPtr != NULL) paramsDefaultsInitHook = funcPtr;
        \}
    }

    ##################################
    # List head pointer
    emit {
        static struct ${access_struct} *${access}_table_head = NULL;
    }

    ##################################
    # List iterators
    emit_newline
    emit {
        struct ${access_struct} * first_${access}_entry(void) {
            return ${access}_table_head;
        }
    }
    emit_newline
    emit {
        struct ${access_struct} * next_${access}_entry(struct ${access_struct} * params) {
            return params->next;
        }
    }



    ##################################
    # The access function for each parameter item

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


    ##################################
    # The access function for each parameter item
    emit_newline
    emit {
     static struct ${access_struct} default_params;
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


        emit_newline
        emit {
            static char *get_${name}(char *buf, void *usrptr, int ident) \{
            strncpy(buf, $default_value, $field_length + 1);

            if (usrptr != NULL) \{
            struct ${access_struct} *params = (struct ${access_struct} *)usrptr;
            if (params->${name}_set == 1)
            strncpy(buf, params->${name}, $field_length + 1);
            \}
            else \{
        }
        emit {
            if (paramsDefaultsInitHook != NULL) \{
            if (default_params.${name}_set == 1)
            strncpy(buf, default_params.${name}, $field_length + 1);
            \}
        }

        emit {
            \}
            return buf;
            \}
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
         } elseif {[string equal $field_type "integerorany"]} {
                emit {
                  if (strcasecmp(params->${name}, "Any") == 0)
                  params->${name}_value = 0X7FFFFFF;
                  else
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
                        ${field_length_name} + 1);
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
        } elseif {[string equal $field_type "integerorany"] } {
            emit {
                if (params->${name}_value == 0X7FFFFFF)
                strcpy(params->${name}, "Any");
                else
                sprintf(params->${name}, "%d", params->${name}_value);
            }
        } elseif {[string equal $field_type "float"] } {
            emit {
                sprintf(params->${name}, "%.2f", params->${name}_value);
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

    emit {
        #ifndef LIBONLY
    }

    ##################################
    # Get entry by index function
    emit_newline
    emit {
        static struct ${access_struct} *get_${access}_entry_by_index(int index) \{
        struct ${access_struct} *params_iter;
        int count = 0;

        params_iter = ${access}_table_head;
        while (params_iter != NULL) \{
        if (count++ == index)
        return params_iter;
        params_iter = params_iter->next;
        \}

        return NULL;
        \}
    }

    ##################################
    # Selector call back function
    # TODO
    emit_newline
    emit {
        static char * ${access}_selector_cb(int item) \{
        struct ${access_struct} *paramsp;

        paramsp = get_${access}_entry_by_index(item);

        sprintf(tempbuf, "????");
        if (paramsp != NULL) \{
        setup_parameter_values(paramsp);
        constructRow${title}(tempbuf, paramsp);
        \}
        return tempbuf;
        \}
    }

    emit {
        #endif /* LIBONLY */
    }

    ##################################
    # Count list function
    emit_newline
    emit {
        static int count_${access}(void) \{
        struct ${access_struct} *params_iter;
        int count = 0;

        params_iter = ${access}_table_head;
        while (params_iter != NULL) {
            count++;
            params_iter = params_iter->next;
        }
        return count;
        \}
    }


    ##################################
    # Free list entry function
    emit_newline
    emit {
        static void free_${access}(void) \{
        struct ${access_struct} *params_iter, *tmp;
        params_iter = ${access}_table_head;
        while (params_iter != NULL) {
            tmp = params_iter;
            params_iter = params_iter->next;
            free(tmp);
        }
        ${access}_table_head = NULL;

        \}
    }


    ##################################
    # Print list function
    emit {
        static void print_${access}(struct ${access_struct} *params, int count, FILE * fp) \{

            int len;

            fprintf(fp,
                    "\################################################################\\n");
    }
    set upper_id [string toupper ${access}_id]
    set field_id "\"$upper_id\""
    emit {
        len = fprintf(fp, "%s.%d", ${field_id}, count);
        fprintf(fp, "%*s %d\\n", 25 - len, " ", params->${access}_id);
    }
    foreach {idx} $index_list {
        set name $item($idx,name)
        set field_type $item($idx,type)
        set upper_name [string toupper $name]
        set field_name_str "\"$upper_name\""
        emit {
            len = fprintf(fp, "%s.%d", ${field_name_str}, count);
            fprintf(fp, "%*s %s\\n", 25 - len, " ", params->${name});
        }
    }
    emit {
        \}
    }

    ##################################
    # Save list function
    emit_newline
    emit {
        static int save_${access}(void) \{

        struct ${access_struct} *params_iter;
        int count = 0;
        FILE *fp;

        if ((fp = fopen(get_config_file_name(), "w+")) == NULL) \{
        return -1;
        \}

        params_iter = ${access}_table_head;
        while (params_iter != NULL) {
            count++;
            print_${access}(params_iter, count, fp);
            params_iter = params_iter->next;
        }

        fclose(fp);

        return count;
        \}
    }


    ##################################
    # Get unique id function
    emit_newline
    emit {
        static int get_unique_${access}_id(void) \{
        struct ${access_struct} *paramsp;
        int unique_id = 1;

        again:
        paramsp = ${access}_table_head;
        while (paramsp != NULL) {
            if (paramsp->${access}_id == unique_id) {
                unique_id++;
                goto again;
            }
            paramsp = paramsp->next;
        }
        return unique_id;
        \}
    }

    emit {
        #ifndef LIBONLY
    }
    ##################################
    # Delete from list function
    emit_newline
    emit {
        static void delete_from_list(int list_id) \{

        struct ${access_struct}  *params_iter, *tmp;
        params_iter = ${access}_table_head;
        if (params_iter == NULL)
        return;

        if (params_iter->${access}_id == list_id) \{
        tmp = params_iter;
        ${access}_table_head = params_iter->next;
        free(tmp);
        \}
        else \{
        while (params_iter->next != NULL) \{
        if (params_iter->next->${access}_id == list_id) \{
        tmp = params_iter->next;
        params_iter->next = params_iter->next->next;
        free(tmp);
        break;
        \}
        params_iter = params_iter->next;

        \}
        \}
        \}
    }
    emit {
        #endif /* LIBONLY */
    }

    ##################################
    # Add to list function
    emit_newline
    emit {
        static void add_to_list(struct ${access_struct} *params) \{

        struct ${access_struct}  *params_iter;
        params->next = NULL;
        params_iter = ${access}_table_head;
        if (params_iter == NULL) \{
        ${access}_table_head = params;
        \}
        else \{
        while (params_iter->next != NULL) \{
        params_iter = params_iter->next;
        \}
        params_iter->next = params;
        \}
        \}
    }

    ##################################
    # Config file handler function
    emit_newline
    emit {
        static int ${access}_config_handler(char *name, char *value, void *ptr,
                                            int idx) \{
        static struct ${access_struct} params;
        struct ${access_struct} *paramsp;
        static int lastidx = 0;


        if (idx != lastidx) \{
        if (lastidx == 0 && (idx > 1 || idx == 0))
        return -1;
        if (lastidx == 0 && idx == 1) \{
        // prepare to populate new entry
        memset(&params, 0x00, sizeof(struct ${access_struct}));
        lastidx = 1;
        \}
        else \{
        /* Add the struct built up so far to the linked list */
        paramsp = malloc(sizeof(struct ${access_struct}));
        if (paramsp == NULL)
        return -1;
        memcpy(paramsp, &params, sizeof(struct ${access_struct}));
        if (ptr != NULL)
        paramsp->userptr = ptr;

        // for any values not found, fill in defaults
    }

    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            if (paramsp->${name}_set == 0) \{
            get_${name}(paramsp->${name}, NULL, 0);
            paramsp->${name}_set = 1;
            \}
        }
    }

    emit {
        add_to_list(paramsp);

        // prepare to populate new entry
        memset(&params, 0x00, sizeof(struct ${access_struct}));
        lastidx = idx;
        \}
        \}
    }
    set upper_id [string toupper ${access}_id]
    set field_id "\"$upper_id\""
    emit {
        if (strcasecmp(name, ${field_id}) == 0) \{
        params.${access}_id = atoi(value);
        \}
    }
    foreach {index} $index_list {
        set name $item($index,name)
        set upper_name [string toupper $name]
        set field_name_str "\"$upper_name\""
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"
        emit {
            else if (strcasecmp(name, ${field_name_str}) == 0) \{
            params.${name}_set = 1;
            strncpy(params.${name}, value, ${field_length_name} + 1);
            \}
        }
    }
    emit_newline
    emit {
        return 0;
        \}
    }


    ##################################
    # Parse config file function

    emit_newline
    emit {
        static int parse_${access}(void *ptr) \{
        struct ${access_struct} *params;
        int count = 0;
        int need_to_save = 0;

        if (0) \{
        // avoid static function warning
        setup_parameter_strings(params);
        \}
        free_${access}();

        if (parse_list_config_file(get_config_file_name(),
                                   ${access}_config_handler,
                                   ptr) < 0) \{
        VLOG_WARNING("Could not load  configuration file %s",
                      get_config_file_name());
        \}
        else \{
        // Setup parameter values and fix id values if necessary
        foreach_${access}_entry(params) \{
        setup_parameter_values(params);
        if (params->${access}_id <= 0) \{
        params->${access}_id = get_unique_${access}_id();
        need_to_save = 1;
        \}
        \}
        \}
        if (need_to_save)
        save_${access}();
        count = count_${access}();
        return count;
        \}
    }

    ##################################
    # Custom hooks register functions
    emit_newline
    emit {
        static ${title}SavedFunction *savedHook = NULL;
        extern void register${title}SavedFunction(${title}SavedFunction *funcPtr) \{
        if (funcPtr != NULL) savedHook = funcPtr;
        \}
    }


    ##################################
    # Retrieve parameters function
    if {$board_access} {
        emit_newline
        emit {
            int  retrieve${title}Values(HAL *hp) \{
            // Just parse the current config file and foreach_${access}_entry()
            // will provide the rest
            if (hp == NULL)
            return 0;
            board_number = hal_get_board_number(hp);
            return parse_${access}(hp);
            \}
        }
    } else {
        emit_newline
        emit {
            int  retrieve${title}Values() \{
            // Just parse the current config file and foreach_${access}_entry()
            // will provide the rest
            return parse_${access}(NULL);
            \}
        }
    }

    emit_newline
    emit {
        int  count${title}Values() \{
        return count_${access}();
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

        if {[string equal $item($idx,displayonly) "true"] && [string equal $item($idx,align) "follow"]} {
            set flags "FFLAG_DISPLAYONLY | FFLAG_FOLLOW"
        }

        if [string equal $item($idx,displayonly) "true"] {
            set flags "FFLAG_DISPLAYONLY"
        }


        if {[string equal $field_type "string"]} {

            if {[string equal $field_validation "ipaddress"]} {
                set validation_function "check_ip"
            } elseif {[string equal $field_validation "inetname"]} {
                set validation_function "check_inetname"
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
                Error "Unsupported Validation \"${field_validation}\" at node \"${name}\""
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
        } elseif {[string equal $field_type "integerorany"]} {
            set validation_function "check_integerorany"
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

        ##################################################
        #if the item is display only, then disable editing
        #################################################
        if [string equal $item($idx,displayonly) "true"] {
            emit {

                {FIELD_${upper_name}, $field_label,
                $field_help,
                $field_length, get_${name}, NULL,
                FTYPE_NONE, 0, 0, O_EDIT, $flags},
            }
        } else {
            emit {

                {FIELD_${upper_name}, $field_label,
                $field_help,
                $field_length, get_${name}, $validation_function,
                $validation_type, $validation_min, $validation_max, $options, $flags},
            }
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
    # Menu access function

    emit_newline
    emit_newline
    emit {

        int configure${title}(MENUINFO *minfo) \{

        MENU *selector;
        DIALOG *dialog;
        int rc;
        int idx;
        int number_of_items;
        char keylist[12];
        char titlebuf[120];
        int retval = RC_NORMAL;
        int ${access}_id = 0;
        struct ${access_struct} *orig_param_ptr;
        char error_string[128];
        HAL *hp = NULL;
        int toprow;
    }

    if ($board_access) {
        emit_newline
        emit {
            \{
            hp = minfo->minfo_userptr;
            if (hp != NULL) \{
            board_number = hal_get_board_number(hp);
            \}
            \}
        }
    }

    emit_newline
    emit {
        sprintf(titlebuf, "%s\\n  \\n   </U>", minfo->minfo_title);
        constructHeader${title}(titlebuf + strlen(titlebuf));
        strcat(titlebuf, "<!U>");

        show_title(minfo, titlebuf);

        number_of_items = parse_${access}(hp);
        if (number_of_items == 0) \{
        dialog = show_dialog(minfo, 8, 15,
                             "</K/U>Empty ${description} Table!!<!K!U>");


        show_info(minfo, "Add new ${description}?" YESNO_STR);
        if (get_yesno(minfo)) \{
        remove_dialog(dialog);
        rc = update_${access}_dialog(minfo, 0, 0);
        if (rc == RC_EXIT) \{
        free_${access}();
        return RC_EXIT;
        \}
        if (rc != RC_NORMAL) \{
        free_${access}();
        return retval;
        \}
        number_of_items = count_${access}();
        if (number_of_items == 0) \{
        free_${access}();
        return retval;
        \}
        show_title(minfo, titlebuf);
        \}
        else \{
        remove_dialog(dialog);
        free_${access}();
        return retval;
        \}
        \}
        selector = create_selector(number_of_items, ${access}_selector_cb);

        set_menu_windows(minfo, selector);

        post_menu(selector);

        show_info(minfo, ${access}_help);

        while (1) \{
        strcpy(keylist, "dDaAmMsS");
        rc = process_selector(minfo, selector, keylist);

        if (rc == RC_EXIT) \{
        retval = RC_EXIT;
        break;
        \}
        if (rc == RC_QUIT) \{
        retval = RC_NORMAL;
        break;
        \}


        switch (keylist[0]) \{

        case 'd':
        case 'D':

        idx = rc + 1;
        sprintf(tempbuf, "Delete ${description} at Index %d ?" YESNO_STR, idx);
        show_info(minfo, tempbuf);
        if (get_yesno(minfo)) \{

        orig_param_ptr = get_${access}_entry_by_index(rc);

        if (orig_param_ptr == NULL) \{
        return RC_ERROR;
        \}
        orig_param_ptr->userptr = minfo->minfo_userptr;
        setup_parameter_values(orig_param_ptr);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validate${title}(DEL_REQUEST, NULL, orig_param_ptr, error_string);

        if (rc == RC_WARNING) \{


        char info_string[256];
        sprintf(info_string, "WARNING: %s\\nDelete entry anyway? %s", error_string,
                YESNO_STR);
        show_info(minfo, info_string);

        if (get_yesno(minfo)) \{
        rc = RC_NORMAL;
        \} else \{
        show_info(minfo, "Delete Aborted.\\n" PRESSANYKEY_STR);
        get_anykey(minfo);
        \}
        \}
        if (rc == RC_NORMAL) \{
        rc = acquire_config_lock();
        if (rc == RC_NORMAL) \{

        rc = apply${title}(DEL_REQUEST, NULL, orig_param_ptr);
        release_config_lock();

        if (rc == RC_NORMAL) \{

        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_title);
        remote_config_set_command_name("del_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);
    }
    if ($board_access) {
        emit {
            remote_config_set_board_number(board_number);
            remote_config_set_channel_number(board_number+1);
        }
    }
    emit {
        delete_${access}(idx - 1, &${access}_id);

        remote_config_set_index(${access}_id);
        remote_config_send();

        number_of_items = count_${access}();
        save_${access}();

    }

    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(DEL_REQUEST, minfo->minfo_userptr);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)(DEL_REQUEST);
        }
    }

    emit {
        if (number_of_items == 0) \{
        show_info(minfo, "Deleted last ${description}. " PRESSANYKEY_STR);
        get_anykey(minfo);
        goto QUIT_LABEL;
        \}
        \}
        else \{
        show_info(minfo, "Failed: Sytem Error.\\n"
                  PRESSANYKEY_STR);
        get_anykey(minfo);
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
        get_anykey(minfo);
        \}
        \}
        else \{
            char info_string[256];
            sprintf(info_string, "Failed: %s\\n%s", error_string,
                    PRESSANYKEY_STR);
            show_info(minfo, info_string);
            retval = RC_ERROR;
            get_anykey(minfo);
        \}
        unpost_menu(selector);
        refresh_selector(selector, number_of_items, ${access}_selector_cb);
        post_menu(selector);
        \}

        show_info(minfo, ${access}_help);
        break;

        case 'a':
        case 'A':
        unpost_menu(selector);
        rc = update_${access}_dialog(minfo, 0, 0);
        if (rc == RC_EXIT) \{
        retval = RC_EXIT;
        goto QUIT_LABEL;
        \}
        if (rc == RC_NORMAL) \{
        number_of_items = count_${access}();
        refresh_selector(selector, number_of_items, ${access}_selector_cb);
        \}
        show_title(minfo, titlebuf);
        post_menu(selector);
        show_info(minfo, ${access}_help);
        break;

        case 'm':
        case 'M':
        toprow = top_row(selector);
        unpost_menu(selector);
        idx = rc + 1;
        rc = update_${access}_dialog(minfo, idx, 0);
        if (rc == RC_EXIT) \{
        retval = RC_EXIT;
        goto QUIT_LABEL;
        \}
        if (rc == RC_NORMAL) \{
        number_of_items = count_${access}();
        refresh_selector(selector, number_of_items, ${access}_selector_cb);
        \}
        if (toprow != 0) \{
        set_top_row(selector, toprow);
        \}
        set_selector_item(selector, idx - 1);
        show_title(minfo, titlebuf);
        post_menu(selector);
        show_info(minfo, ${access}_help);
        break;
    }

    if ($select_item_callback) {
       emit {
           case '\\0':
	   unpost_menu(selector);
           idx = rc + 1;
           orig_param_ptr = get_${access}_entry_by_index(rc);
           if (orig_param_ptr == NULL) \{
           return RC_ERROR;
           \}
           orig_param_ptr->userptr = minfo->minfo_userptr;
           setup_parameter_values(orig_param_ptr);
           /* Call custom selector */
           sprintf(error_string, "Invalid Data");
           rc = select${title}(minfo, orig_param_ptr, error_string);
           if (rc == RC_EXIT) \{
           retval = RC_EXIT;
           goto QUIT_LABEL;
           \}
           show_title(minfo, titlebuf);
           post_menu(selector);
           show_info(minfo, ${access}_help);
           break;
       }
    } else {
       emit {
           case '\\0':
       }
    }
    emit {
	case 's':
        case 'S':
        unpost_menu(selector);
        idx = rc + 1;
        rc = update_${access}_dialog(minfo, idx, 1);
        if (rc == RC_EXIT) \{
        retval = RC_EXIT;
        goto QUIT_LABEL;
        \}
        show_title(minfo, titlebuf);
        post_menu(selector);
        show_info(minfo, ${access}_help);
        break;

        \}
        \}

        QUIT_LABEL:

        free_${access}();
        unpost_menu(selector);
        unset_menu_windows(selector);
        delete_selector(selector);
        return retval;

        \}

    }

    ##################################
    # Update dialog function

    emit_newline
    emit_newline
    emit {

        static int update_${access}_dialog(MENUINFO *minfo, int idx, int req_code) \{

        FORM *form = NULL;
        int rc = E_OK;
        int retval = RC_NORMAL;
        char error_string[128];
        struct ${access_struct} params;
        struct ${access_struct} *orig_param_ptr = NULL;
        char titlebuf[80];
        int number_of_items;
        HAL *hp = NULL;
    }

    if ($board_access) {
        emit_newline
        emit {
            \{
            hp = minfo->minfo_userptr;
            if (hp != NULL) \{
            board_number = hal_get_board_number(hp);
            \}
            \}
        }
    }
    emit_newline
    emit {
        // Call hook if we are adding a new entry
        if (paramsDefaultsInitHook != NULL && req_code != 1 && idx == 0) \{
        memset(&default_params, 0x0, sizeof(struct ${access_struct}));
        default_params.userptr = hp; // could be null
        (*paramsDefaultsInitHook)(&default_params);
        setup_parameter_strings(&default_params);
        \}
    }
    emit_newline
    emit {
        number_of_items = count_${access}();

        redraw:
        if (req_code == 1) \{
            sprintf(titlebuf, "Display Entry at Index %d", idx);
            show_title(minfo, titlebuf);
        \}
        else if (idx == 0) \{
        show_title(minfo, "Add New Entry");
        \}
        else \{
        show_title(minfo, "Modify Entry");
        \}

        if (idx > 0) \{
        orig_param_ptr = get_${access}_entry_by_index(idx - 1);
        \}

        show_title(minfo, NULL);
        form = create_form(itemdefs, ${vertical_offset}, 0, orig_param_ptr);

        set_form_windows(minfo, form);

        post_form(form);

        if (req_code == 1) \{ // Display request only
        wrefresh(minfo->minfo_menuscreen->window);
        show_info(minfo,
                  "Press </U>Space<!U> or </U>N<!U> for next entry, </U>P<!U> for previous entry\\n Or </U>Q<!U> to Quit.");
        rc = get_anykey(minfo);
        unpost_form(form);
        unset_form_windows(form);
        delete_form(form);

        if (rc == '') \{
        return RC_EXIT;
        \}
        else if (rc == 'Q' || rc == 'q') \{
        return RC_ERROR;    /* Nothing wrong really */
        \}
        else if (rc == ' ' || rc == 'N' || rc == 'n' || rc == KEY_DOWN) \{
        if (idx < number_of_items)
        idx = idx + 1;
        \}
        else if (rc == 'P' || rc == 'p' || rc == KEY_UP) \{
        if (idx > 1)
        idx = idx - 1;
        \}

        goto redraw;
        \}

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

    emit {

        else \{

        if(get_form_status(form) == TRUE) \{
        if (idx == 0)
        show_info(minfo, "Add this Entry?" YESNO_STR);
        else
        show_info(minfo, "Update this Entry?" YESNO_STR);
        if (get_yesno(minfo)) \{
        rc = RC_NORMAL;
        memset(&params, 0x00, sizeof(params));
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
        setup_parameter_values(&params);
        params.userptr = minfo->minfo_userptr;
        // orig_params only available in case of modify
        if (orig_param_ptr != NULL) \{
        setup_parameter_values(orig_param_ptr);
        params.${access}_id = orig_param_ptr->${access}_id;
        orig_param_ptr->userptr = minfo->minfo_userptr;
        \}
        else \{
        params.${access}_id = get_unique_${access}_id();
        \}
    }
    emit {
        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validate${title}(idx == 0 ? ADD_REQUEST : MOD_REQUEST,
                              &params, orig_param_ptr, error_string);
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set field_type "$item($idx,type)"
        set field_validation "$item($idx,validation)"

        if {[string equal $field_type "integerorany" ]} {
            emit {
                if (check_integeroranyrange(params.${name}, $item($idx,validation_min), $item($idx,validation_max)) != RC_NORMAL) \{
                rc = RC_INVALID_VALUE;
                sprintf(error_string, "Invalid value");
                \}
            }
        }
    }

    emit {

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
        if (rc == RC_NORMAL) \{
        rc = acquire_config_lock();
        if (rc == RC_NORMAL) \{
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_title);
        if (idx == 0) \{
        remote_config_set_command_name("add_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        \}
        else \{
        remote_config_set_command_name("mod_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        \}
        remote_config_set_source(REMOTE_CONFIG_SOURCE_MENU);

    }
    foreach {idx} $index_list {
        set name $item($idx,name)
        set index [expr $idx - 1]
        emit {
            remote_config_add_parameter(${access_options}[${index}].name,
                                        params.${name});
        }
    }
    emit_newline
    emit {

        if (rc == RC_NORMAL) \{
        rc = apply${title}(idx == 0 ? ADD_REQUEST : MOD_REQUEST,
                           &params, orig_param_ptr);
        release_config_lock();
        \}

        if (rc == RC_NORMAL) \{
        rc = update_${access}_entry(idx, &params);
        \}
        remote_config_set_index(params.${access}_id);
        if (rc == RC_NORMAL) \{
        save_${access}();
    }
    if ($board_access) {
        emit {
            remote_config_set_board_number(board_number);
            remote_config_set_channel_number(board_number+1);
        }
    }
    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(idx == 0 ? ADD_REQUEST : MOD_REQUEST, minfo->minfo_userptr);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)(idx == 0 ? ADD_REQUEST : MOD_REQUEST);
        }
    }

    emit {
        remote_config_send();

        if (idx == 0) \{
        show_info(minfo, "Entry added.\\n"
                  PRESSANYKEY_STR);
        \}
        else \{
        show_info(minfo, "Entry updated.\\n"
                  PRESSANYKEY_STR);
        \}
        \}
        else \{
        show_info(minfo, "Failed: Sytem Error.\\n"
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
        else \{
            show_info(minfo, "Continue Editing?"
                    YESNO_STR);
            if (get_yesno(minfo)) \{
                /* continue */
            \}
            else \{
                retval = RC_ERROR;
                break;
            \}
        \}
        \}
        else \{
        show_info(minfo, "No change in data. Quit?" YESNO_STR);
        if (get_yesno(minfo))\{
            retval = RC_ERROR;
            break;
        \}
        \}

        \}

        \}
        /* cleanup */
        unpost_form(form);
        unset_form_windows(form);
        delete_form(form);

        return retval;

        \}

    }

    ##################################
    # Update to list function
    emit_newline
    emit_newline
    emit {
        static int update_${access}_entry(int idx, struct ${access_struct} *params) \{

        struct ${access_struct}  *paramsp;
    }
    emit_newline
    emit {
        if (idx == 0) \{ // add request
        paramsp = malloc(sizeof(struct ${access_struct}));
        if (paramsp != NULL) \{
        memcpy(paramsp, params, sizeof(struct ${access_struct}));
        add_to_list(paramsp);
        \}
        else \{
        return RC_ERROR;
        \}
        \}
        else \{ // modify request
        /* Copy values into existing struct in list, retaining needed vaules */
        paramsp = get_${access}_entry_by_index(idx - 1);
        params->${access}_id = paramsp->${access}_id;
        params->userptr = paramsp->userptr;
        params->next = paramsp->next;
        memcpy(paramsp, params, sizeof(struct ${access_struct}));
        \}

    }
    emit_newline
    emit {
        return RC_NORMAL;
        \}
    }

    emit_newline
    emit_newline
    emit {
        static int delete_${access}(int idx, int *${access}_idp) \{
        int number_of_items;
        struct ${access_struct} *paramsp;

        number_of_items = count_${access}();

        if (idx >= number_of_items)
        return RC_ERROR;

        paramsp = get_${access}_entry_by_index(idx);
        if (paramsp == NULL)
        return RC_ERROR;

        *${access}_idp = paramsp->${access}_id;

        delete_from_list(paramsp->${access}_id);

        return RC_NORMAL;
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
        printf("   " XML_MENUDESCR_ITEMTYPE, "configlist");
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
	if [string equal $item($idx,validation) "range"] {
		emit {
			printf (XML_FIELD_RANGE,
			"$item($idx,validation)", 
			"$item($idx,validation_min)", 
			"$item($idx,validation_max)");
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
	if [string equal $item($idx,validation) "netmask"] {
		emit {
			printf (XML_FIELD_NETMASK,
			"$item($idx,validation)");
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
        int ctr=0;
        HAL *hp = NULL;
        struct ${access_struct} *params;
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

    emit {

        if (opt_idx == argc) \{
        parse_${access}(hp);

        printf(XML_GROUP, get_${access});
        print_cli_success(NULL);
        printf("  " XML_TABLE, ${access}_title);

        foreach_${access}_entry(params) \{
        ctr = 0;
        printf("    " XML_OBJINDEX, params->${access}_id, "$description Entry");

    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            printf("      " XML_ATTRSTR, ${access_options}[ctr++].name, params->$name);
        }
    }

    emit_newline
    emit {
        printf("  " XML_OBJEND);
        \}
        printf("  " XML_TABEND);
        printf(XML_GROUPEND);
        free_${access}();
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
    # The Update CLI function (Add/Modify)
    emit_newline
    emit_newline
    emit {
        static int update_${access}_cli(int argc, char **argv, int req_code) \{

        int c, opt_index = 0;
        int rc = RC_NORMAL;
        int number_of_items;
        int value;
        char error_string[128];
        struct ${access_struct} params, *paramsp;
        struct ${access_struct} *orig_param_ptr = NULL;
        int access_id;
        HAL *hp = NULL;
        char * des_string;
        int index = 0;
    }

    emit_newline
    emit {
        value = 0;
        invoked_via_cli = 1;
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_title);
        if (req_code == ADD_REQUEST) \{
        remote_config_set_command_name("add_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        des_string = add_des_${access};
        \}
        else \{
        remote_config_set_command_name("mod_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_MODIFY);
        des_string = mod_des_${access};
        \}
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);

        memset(&params, 0x00, sizeof(params));

        if (view_only() != 0) \{
        print_cli_failure(des_string, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to set $description parameters\\n");
        return RC_ERROR;
        \}

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
                }
            } elseif {[string equal $field_validation "inetname"]} {
                emit {
                    else if (check_inetname(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "domainname"]} {
                emit {
                    else if (check_domain(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "netmask"]} {
                emit {
                    else if (check_netmask(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "multicast"]} {
                emit {
                    else if (check_multicastip(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "regexp"]} {
                emit {
                    else if (check_regex(optarg, regexp_$item($idx,validation_value)) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "enumeration"]} {
                emit {
                    else if (check_enum(optarg, ${name}_values) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "hexadecimal"]} {
                emit {
                    else if (check_hexadecimal(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } elseif {[string equal $field_validation "onlyhexadecimal"]} {
                emit {
                    else if (check_onlyhexadecimal(optarg) == RC_NORMAL)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
                    \}
                }
            } else {
                emit {
                    else if (1)  \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                            $field_length + 1);
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
            }
        } elseif {[string equal $field_type "password"]} {
            emit {
                else if (check_regex(optarg, regexp_password) == RC_NORMAL)  \{
                params.${name}_set = 1;
                strncpy(params.${name}, optarg,
                        $field_length + 1);
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
            }
        } elseif {[string equal $field_type "integerorany"]} {
                 emit {
                    else if (check_integeroranyrange(optarg,$item($idx,validation_min),$item($idx,validation_max)) == RC_NORMAL) \{
                    params.${name}_set = 1;
                    strncpy(params.${name}, optarg,
                           $field_length + 1);
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
            }

        } elseif {[string equal $field_type "bandwidth"]} {
            emit {
                else if (check_regex(optarg, regexp_bandwidth) == RC_NORMAL)  \{
                params.${name}_set = 1;
                strncpy(params.${name}, optarg,
                        $field_length + 1);
                \}
            }
        }

        emit {
            else {
                rc = RC_INVALID_VALUE;
            }
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
            if (hp == NULL) {
                print_cli_failure(des_string, CLI_REASON_WRONG_HW);
                remote_config_send_error(CLI_REASON_WRONG_HW);
                return RC_ERROR;
            }
        }
    }

    emit_newline
    emit {
        params.userptr = hp;
    }

    emit_newline
    emit {
        if ((rc == RC_NORMAL) &&
            ((req_code == ADD_REQUEST && optind == argc) ||
            (req_code == MOD_REQUEST && optind == (argc - 1)))) \{

        if (req_code == MOD_REQUEST) \{
        access_id = atoi(argv[optind]);
        remote_config_set_index(access_id);
        \}
        else \{
        // For add request we may need to invoke default init hook
        if (paramsDefaultsInitHook != NULL) \{
        memset(&default_params, 0x0, sizeof(struct ${access_struct}));
        default_params.userptr = hp; // could be null
        (*paramsDefaultsInitHook)(&default_params);
        setup_parameter_strings(&default_params);
        \}
        \}


        number_of_items = parse_${access}(hp);

        // For modify, find the entry and the index it is at
        if (req_code == MOD_REQUEST) \{
        index = 0;
        foreach_${access}_entry(paramsp) \{
        index++;
        if (paramsp->${access}_id == access_id) \{
        orig_param_ptr = paramsp;
        break;
        \}
        \}

        if (orig_param_ptr == NULL) \{
        print_cli_failure(des_string, CLI_REASON_BAD_INDEX);
        remote_config_send_error(CLI_REASON_BAD_INDEX);
        return RC_ERROR;
        \}
        setup_parameter_values(orig_param_ptr);
        params.${access}_id = orig_param_ptr->${access}_id;
        \}
        else \{
        params.${access}_id = get_unique_${access}_id();
        \}

        // For params that did not come from command line, pick from defaults
        // (for add) or from orig_param_ptr (for modify)
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
            get_${name}(params.${name}, orig_param_ptr, 0);
            \}
        }
    }
    emit {

        setup_parameter_values(&params);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validate${title}(req_code, &params, orig_param_ptr, error_string);
        if (rc == RC_WARNING)
        rc = RC_NORMAL;

        if (rc != RC_NORMAL) \{
        print_cli_failure(des_string, error_string);
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
            print_cli_failure_with_detail(des_string, CLI_REASON_SYSTEM_BUSY, system_busy_string);
            remote_config_send_error(CLI_REASON_SYSTEM_BUSY);
            return RC_ERROR;
        \}
    }
    emit_newline
    emit {

        if (rc == RC_NORMAL) \{
        rc = apply${title}(req_code, &params, orig_param_ptr);
        release_config_lock();

        if (rc == RC_NORMAL) \{
        rc = update_${access}_entry(index, &params);
        \}
        if (req_code != MOD_REQUEST) \{
        remote_config_set_index(params.${access}_id);
        \}
        if (rc == RC_NORMAL) \{
        print_cli_success(des_string);
        remote_config_send();
        save_${access}();
    }
    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(req_code, hp);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)(req_code);
        }
    }

    emit {
        \}
        \}
        if (rc != RC_NORMAL) \{
        print_cli_failure(des_string, CLI_REASON_SYSTEM_ERR);
        remote_config_send_error(CLI_REASON_SYSTEM_ERR);
        rc = RC_ERROR;
        \}
        free_${access}();
        \}
        else if (rc == RC_INVALID_VALUE) \{
        print_cli_failure(des_string, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
        \}
        else \{
        print_cli_failure(des_string, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);
        if (req_code == ADD_REQUEST)
        printf("\\nUsage: add_${access} [OPTIONS]\\n");
        else
        printf("\\nUsage: mod_${access} [OPTIONS] ID\\n");
        printf("%s \\n", des_string);
        printf("Options:\\n");
    }


    foreach {idx} $index_list {
        set name $item($idx,name)
        set field_label "$item($idx,label)"
        set field_help "$item($idx,help)"
        set field_type $item($idx,type)
        set field_displayonly $item($idx,displayonly)
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
            if {[string equal $field_displayonly "false"]} {
                if {$safety == 0} {
                    emit {
                        printf("        -${name} $value ${spaces} ${help_string}\\n");
                    }
                } else {
                    emit {
                        printf("                                    ${help_string}\\n");
                    }
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


    ##################################
    # The Add CLI function
    emit_newline
    emit {

        int add_${access}_main(int argc, char **argv) \{
        return update_${access}_cli(argc, argv, ADD_REQUEST);
        \}
    }

    ##################################
    # The Modify CLI function
    emit_newline
    emit {

        int mod_${access}_main(int argc, char **argv) \{
        return update_${access}_cli(argc, argv, MOD_REQUEST);
        \}
    }

    ##################################
    # The Delete CLI function
    emit_newline
    emit_newline
    emit {
        int del_${access}_main(int argc, char **argv) \{

        int c, opt_index = 0;
        int rc = RC_NORMAL;
        char error_string[128];
        struct ${access_struct} *paramsp;
        struct ${access_struct} *orig_param_ptr = NULL;
        int access_id;
        HAL *hp = NULL;
        int index = 0;
    }

    emit_newline
    emit {
        hp = NULL;
        invoked_via_cli = 1;
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_title);
        remote_config_set_command_name("del_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_DELETE);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);


        if (view_only() != 0) \{
        print_cli_failure(del_des_${access}, CLI_REASON_NO_PERMISSION);
        remote_config_send_error(CLI_REASON_NO_PERMISSION);
        printf("You must be root to set $description parameters\\n");
        return RC_ERROR;
        \}

        while(1) \{
        c = getopt_long_only(argc, argv, "h", ${access_options}, &opt_index);
        if (c == -1) \{
        break;
        \}
        else if (c == 0) \{

        switch(opt_index) \{

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
            if (hp == NULL) {
                print_cli_failure(del_des_${access}, CLI_REASON_WRONG_HW);
                remote_config_send_error(CLI_REASON_WRONG_HW);
                return RC_ERROR;
            }
        }
    }



    emit_newline
    emit {
        if ((rc == RC_NORMAL) && (optind == (argc - 1))) \{

        access_id = atoi(argv[optind]);
        remote_config_set_index(access_id);

        parse_${access}(hp);

        // Find the entry and the index it is at
        index = 0;
        foreach_${access}_entry(paramsp) \{
        index++;
        if (paramsp->${access}_id == access_id) \{
        orig_param_ptr = paramsp;
        break;
        \}
        \}

        if (orig_param_ptr == NULL) \{
        print_cli_failure(del_des_${access}, CLI_REASON_BAD_INDEX);
        remote_config_send_error(CLI_REASON_BAD_INDEX);
        return RC_ERROR;
        \}
        setup_parameter_values(orig_param_ptr);

        /* Call custom validator */
        sprintf(error_string, "Invalid Data");
        rc = validate${title}(DEL_REQUEST, NULL, orig_param_ptr, error_string);
        if (rc == RC_WARNING)
        rc = RC_NORMAL;


        if (rc != RC_NORMAL) \{
        print_cli_failure(del_des_${access}, error_string);
        remote_config_send_error(error_string);
        return rc;
        \}
        rc = acquire_config_lock();
        if (rc != RC_NORMAL) \{
            char system_busy_string [64];
            if (rc == RC_LOCKED_OUT)
                sprintf(system_busy_string, "\\n Cannot Delete: System busy with other config change.\\n");
            else
                sprintf(system_busy_string, "\\n Cannot Delete: System busy.\\n");
            print_cli_failure_with_detail(del_des_${access}, CLI_REASON_SYSTEM_BUSY, system_busy_string);
            remote_config_send_error(CLI_REASON_SYSTEM_BUSY);
            return RC_ERROR;
        \}
    }


    emit_newline
    emit {

        if (rc == RC_NORMAL) \{
        rc = apply${title}(DEL_REQUEST, NULL, orig_param_ptr);
        release_config_lock();

        if (rc == RC_NORMAL) \{
        delete_${access}(index - 1, &access_id);
        print_cli_success(del_des_${access});
        remote_config_send();
        save_${access}();

    }

    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(DEL_REQUEST, hp);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)(DEL_REQUEST);
        }
    }

    emit {
        \}
        \}
        if (rc != RC_NORMAL) \{
        print_cli_failure(del_des_${access}, CLI_REASON_SYSTEM_ERR);
        remote_config_send_error(CLI_REASON_SYSTEM_ERR);
        rc = RC_ERROR;
        \}
        free_${access}();
        \}
        else if (rc == RC_INVALID_VALUE) \{
        print_cli_failure(del_des_${access}, CLI_REASON_BAD_VALUE);
        remote_config_send_error(CLI_REASON_BAD_VALUE);
        \}
        else \{
        print_cli_failure(del_des_${access}, CLI_REASON_BAD_USAGE);
        remote_config_send_error(CLI_REASON_BAD_USAGE);

        printf("\\nUsage: del_${access} [OPTIONS] ID\\n");
        printf("%s \\n", del_des_${access});
        printf("Options:\\n");
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

    ##################################
    # The Load CLI function
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
        struct ${access_struct} *paramsp;
        HAL *hp;
        hp = NULL;
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
            retrieve${title}Values(hp);         // Retrieve current list

        }
    } else {
        emit {
            retrieve${title}Values();         // Retrieve current list
        }
    }

    emit_newline
    emit {
        rc = acquire_config_lock();
        if (rc == RC_NORMAL) \{
            foreach_${access}_entry(paramsp) \{
            apply${title}(ADD_REQUEST, paramsp, NULL);
            \}
            release_config_lock();
        \}
        else if (rc == RC_LOCKED_OUT)
            VLOG_DEBUG("Cannot Update: System busy with other config change.");
        else
            VLOG_DEBUG("Cannot Update: System busy");

    }

    if ($board_access) {
        emit {
            if (savedHook != NULL)
            (*savedHook)(ADD_REQUEST, hp);
        }
    } else {
        emit {
            if (savedHook != NULL)
            (*savedHook)(ADD_REQUEST);
        }
    }

    if ($board_access) {
        emit_newline
        emit {
            if (hp != NULL)
            hal_release(hp);
            \}
        }
    }

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

        int ctr=0;
        struct ${access_struct} *paramsp;
        HAL *hp;
        hp = NULL;

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

            retrieve${title}Values(hp);

        }
    } else {
        emit {
            retrieve${title}Values();         // Retrieve current list
        }
    }

    emit_newline
    emit {

        foreach_${access}_entry(paramsp) \{

    }
    emit_newline
    emit {
        ctr = 0;
        remote_config_start${remote_config_suffix}();
        remote_config_set_config_name(${access}_title);
        remote_config_set_command_name("add_${access}");
        remote_config_set_operation(REMOTE_CONFIG_OPERATION_ADD);
        remote_config_set_source(REMOTE_CONFIG_SOURCE_CLI);
        remote_config_set_index(paramsp->${access}_id);
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        emit {
            remote_config_add_parameter(${access_options}[ctr++].name, paramsp->${name});
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
        \}
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

proc create_list_doc {root base_name} {

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

    set channel_help ""
    if {$board_access} {
        set channel_index [expr $item_count + 1 ]
        set channel_help "-channelNumber Number"
    }

 # Retrieve all needed parameter (Item) data
    foreach {idx} $index_list {
        set item($idx,name) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Name"]
        set item($idx,type) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Type"]
        set item($idx,label) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Label"]
        set item($idx,fieldlength) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/FieldLength"]
        set item($idx,default) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/DefaultValue"]
        set item($idx,help) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Help"]
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/ViewList"] {
            set item($idx,viewlist) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/ViewList"]
        } else {
            set item($idx,viewlist) "yes"
        }
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Align"] {
            set item($idx,align) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Align"]
        } else {
            set item($idx,align) "normal"
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/DisplayOnly"] {
            set item($idx,displayonly) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/DisplayOnly"]
        } else {
            set item($idx,displayonly) "false"
        }

        if [string equal $item($idx,align) "normal"] {

        } elseif [string equal $item($idx,align) "follow"] {

        } else {
            Error "Align tag value \"$item($idx,align)\" is invalid, should be \"normal\" or \"follow\"."
        }

        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Validation"] {
            set item($idx,validation) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Validation/Type"]
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

        Debug "Parameter: $item($idx,name)"
    }

    if {$skip_doc == 0} {

      emit_newline_doc
      emit_doc {
           == $title

      }

      emit_newline_doc
      emit_doc {
Type:: List
Category:: $title
Description:: $description
      }

      emit_newline_doc
      emit_doc {
=== Get List Configuration

Command Name:: get_${access}
Description:: Gets the current list for $title

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
=== Delete List Configuration Item

Command Name:: del_${access} [Options] ID
Description:: Deletes the current list of $title

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
=== Add List Configuration Item

Command Name:: add_${access} [Options]
Description:: Add a list for $title

=== Modify List Configuration Item

Command Name:: mod_${access} [Options] ID
Description:: Modify the list for $title

Options common for add_${access} and mod_${access}:
      }

      emit_doc {
         [cols="1,1,3",options="header"]
      }

      emit_doc {
          |================================================================================
                    |NAME        |VALUE              |HELP STRING
      }

      foreach {idx} $index_list {
          set name $item($idx,name)
          set field_label "$item($idx,label)"
          set field_help "$item($idx,help)"
          set field_type $item($idx,type)
          set field_displayonly $item($idx,displayonly)
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

          if {[string equal $field_displayonly "false"]} {
              emit_newline_doc
              emit_doc {
                      |${name} |$value  |${help_string}
              }
          }
      }
      emit_doc {
          |================================================================================
       }
   }
}
