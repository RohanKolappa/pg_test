
proc create_status_cli {root base_name isPager isList} {


    set title [get_text_node_value $root "/Menu/Title"]
    set description [get_text_node_value $root "/Menu/Description"]
    set access [get_text_node_value $root "/Menu/Access"]
    set upper_access [string toupper $access]

    set board_access 0
    if [check_node_exists $root "/Menu/BoardAccess"] {
       set value [get_text_node_value $root "/Menu/BoardAccess"]
        if [string equal $value "true"] {
            set board_access 1
        } elseif [string equal $value "false"] {
            set board_access 0
        } else {
            Error "Value \"$value\" not supported for BoardAccess. Should be \"true\" or \"false\""
        }
    }

    set cli_command_name ${access}
    if [check_node_exists $root "/Menu/CliCommandName"] {
        set cli_command_name [get_text_node_value $root "/Menu/CliCommandName"]
    }

    # Get count of the number of parameter items
    set item_count 0
    set nodes [$root selectNodes "/Menu/Items/Item" type]
    if { $type == "nodes" } {
        set item_count [llength $nodes]
    }
    if { $isPager } {
        if { $item_count != 1 } {
            Error "Total number of Items has to be 1 for statuspager type, it is \"$item_count\"."
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
        if { $isPager } {
            if [ string equal $item($idx,name) "textarea" ] {

            } else {
                Error "Item name has to be \"textarea\" instead of \"$item($idx,name)\"."
            }
        }

        set item($idx,type) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Type"]
        if { $isPager } {
            if [ string equal $item($idx,type) "string" ] {

            } else {
                Error "Item name has to be \"string\" instead of \"$item($idx,type)\"."
            }

            if [check_node_exists $root "/Menu/Items/Item\[$idx\]/TotalPages"] {
                set item($idx,totalpages) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/TotalPages"]
            } else {
                set item($idx,totalpages) 20
            }

            if [check_node_exists $root "/Menu/Items/Item\[$idx\]/LinesPerPage"] {
                set item($idx,linesperpage) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/LinesPerPage"]
                if {$item($idx,linesperpage) > 13} then {
                    set item($idx,linesperpage) 13
                }
            } else {
                set item($idx,linesperpage) 13
            }
            if [check_node_exists $root "/Menu/Items/Item\[$idx\]/PageBreaker"] {
                set item($idx,pagebreaker) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/PageBreaker"]
            } else {
                set item($idx,pagebreaker) "NULL"
            }

        }
        set item($idx,label) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Label"]
        set item($idx,fieldlength) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/FieldLength"]
        set item($idx,help) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Help"]
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Format"] {
            set item($idx,format) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Format"]
        } else {
            set item($idx,format) "normal"
        }
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/ForceFormat"] {
            set item($idx,forceformat) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/ForceFormat"]
        } else {
            set item($idx,forceformat) "no"
        }
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Align"] {
            set item($idx,align) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Align"]
        } else {
            set item($idx,align) "normal"
        }
        if [string equal $item($idx,align) "normal"] {

        } elseif [string equal $item($idx,align) "follow"] {

        } else {
            Error "Align tag value \"$item($idx,align)\" is invalid, should be \"normal\" or \"follow\"."
        }

        set item($idx,subtype) ""
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/SubType"] {
            set item($idx,subtype) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/SubType"]
        }
        set item($idx,units) "NULL"
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Units"] {
            set units [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Units"]
            set item($idx,units) "\"${units}\""
        }
        set item($idx,source) "NULL"
        if {[string equal $item($idx,type) "string"]} {
            if {[string equal $item($idx,subtype) "dynamic"]} {
                set item($idx,source) "\"dynamic\""
            }
        }
        if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Source"] {
            set source [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Source"]
            set item($idx,source) "\"${source}\""
        }

        set item($idx,type_type) "char *"
        if {[string equal $item($idx,type) "string"]} {
            set item($idx,type_type) "char"
        } elseif {[string equal $item($idx,type) "integer"]} {
            set item($idx,type_type) "int"
        } elseif {[string equal $item($idx,type) "float"]} {
            set item($idx,type_type) "double"
            set item($idx,precision) 3
            if [check_node_exists $root "/Menu/Items/Item\[$idx\]/Precision"] {
                set item($idx,precision) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/Precision"]
            }
        } elseif {[string equal $item($idx,type) "boolean"]} {
            set item($idx,type_type) "int"
        } elseif {[string equal $item($idx,type) "long"]} {
            set item($idx,type_type) "long long"
        } elseif {[string equal $item($idx,type) "unsigned_integer"]} {
            set item($idx,type_type) "unsigned long"
            set item($idx,rate_precision) 2
            if [check_node_exists $root "/Menu/Items/Item\[$idx\]/RateDisplayPrecision"] {
                set item($idx,rate_precision) [get_text_node_value $root "/Menu/Items/Item\[$idx\]/RateDisplayPrecision"]
            }
        } elseif {[string equal $item($idx,type) "unsigned_long"]} {
            set item($idx,type_type) "unsigned long long"
        }
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
            #define ${field_length_name}     ${field_length}
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
        /* Client use these members to access the parameter values */
    }

    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type "$item($idx,type)"
        set field_subtype "$item($idx,subtype)"
        set type "$item($idx,type_type)"
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"


        if {[string equal $field_type "string"]} {
            if {[string equal $field_subtype "dynamic"]} {
                emit_header {
                    char *    ${name}_value;
                }
            } else {
                emit_header {
                    char     ${name}_value[$field_length_name + 1];
                }
            }
        } else {
            emit_header {
                $type      ${name}_value;
            }
        }
    }


    emit_header_newline
    emit_header {

        /* Generic user pointer used for various purposes */
        void * userptr;

        /* This is used in case of list type status */
        struct ${access_struct} * next;
        \};
    }

    emit_header_newline
    emit_header {
        #ifdef CUSTOM_MENU_DEFS
        #include "custmenu.h"
        /* Prototype for display${title} command */
        extern int display${title}(MENUINFO *minfo);
        #endif
    }


    emit_header_newline
    emit_header {
        /* Prototype for get${title} function */
        extern int get${title}(struct ${access_struct} *params);
    }

    if {$board_access} {
        emit_header_newline
        emit_header {
            /* Prototype for retrieve${title}Values function */
            extern struct ${access_struct} * retrieve${title}Values(HAL *hp);
            extern void destroy${title}Values(struct ${access_struct} *params);
        }
    } else {
        emit_header_newline
        emit_header {
            /* Prototype for retrieve${title}Values function */
            extern struct ${access_struct} * retrieve${title}Values(void);
            extern void destroy${title}Values(struct ${access_struct} *params);
        }
    }

    if (${isList}) {
        emit_header_newline
        emit_header {
            /* List iterator helpers */
            #define foreach_${access}_entry(params, params_head) \\
            for ((params) = (params_head)->next; \\
                 (params) != NULL; (params) = (params)->next)
        }

    }

    emit_header_newline
    emit_header {
        /* Prototype for CLI access entry point function */
        extern int get_${access}_main(int argc, char **argv);
    }

    if {![string equal $cli_command_name "undefined"]} {
        emit_header_newline
        emit_header {
        /* Thus sets up CLI access name mapping that gets plugged into vmain.c */
        /*
        CLI_COMMAND_MAP    {"get_${cli_command_name}", get_${access}_main},
        */
       }
    }

    emit_header_newline
    emit_header {
        /* This function is used by lists to return a new allocated param object */
        extern struct ${access_struct} *
        get${title}ParamsStruct(struct ${access_struct} *params);
    }

    if (${isList}) {
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
    }

    emit_header_newline
    emit_header {
        #endif /* __${upper_base_name}_H_ */
    }


    ##################################
    # Write out the C section

    # Write the include directives
    emit {

        #include <stdlib.h>
        #include <string.h>
        #include <getopt.h>

        #include <netinet/in.h>

        #define CUSTOM_MENU_DEFS

        #include "hal.h"
        #include "hal_console.h"
        #include "custmenu.h"
        #include "menuutils.h"
        #include "commonutils.h"
        #include "text_utils.h"
        #include "vlog.h"
        #include "remoteStatus.h"
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

    ##################################
    # Global static declarations
    emit_newline
    if (!$isPager) {
        emit {
            static char *helpstr = "The status is continuously getting updated.\\n"
            "Press any  key to quit.";
        }
    } else {
        emit {
            #define HEADER_LENGTH         44
        }
    }

    if (${isList}) {
        emit {
            static char *${access}_help = "Press </U>Enter<!U> on item to get detailed status\\nor </U>Q<!U> to Quit. ";
            static struct ${access_struct} *${access}_table_head = NULL;
            static int ${access}_current_index = 1;
        }
    }

    emit_newline
    if (!$isPager) {
        emit {
            static int read_rates = 0;

            static int clear_counter = 0;

            static unsigned char show_decimal = 0;

            static int timdiff;

            static struct timeval last_time = {0,0};
        }
    }

    emit {
            static struct ${access_struct} last_params;
        }


        if ($board_access) {
            emit_newline
            emit {
                static int board_number = 0;
            }

            set access_options "${access}_options"

            emit {
                static struct option ${access_options}[] = \{
                {"channelNumber", 1, 0, 0},
                {"publish", 0, 0, 0},
                {0, 0, 0, 0}
                \};
            }
        } else {
            set access_options "${access}_options"

            emit {
                static struct option ${access_options}[] = \{
                {"publish", 0, 0, 0},
                {0, 0, 0, 0}
                \};
            }
        }


        ##################################
        # The Free params function
        emit_newline
        emit_newline
        emit {
            static void free_params(struct ${access_struct} *params) \{

            struct ${access_struct} *tmp_params;
            if (params == NULL)
            return;

            while (1) \{
            tmp_params = params->next;
    }

    foreach {idx} $index_list {
        set name $item($idx,name)
        set field_type "$item($idx,type)"
        set field_subtype "$item($idx,subtype)"

        if {[string equal $field_type "string"]} {
            if {[string equal $field_subtype "dynamic"]} {
                emit {
                    if (params->${name}_value != NULL)
                    free(params->${name}_value);
                }
            }
        }
    }

    emit {
        free(params);
        if (tmp_params == NULL)
        break;
        params = tmp_params;
        \}

        \}

    }

    ##################################
    # The Count params function
    emit_newline
    emit_newline
    if (${isList}) {
        emit {
            static int count_params(struct ${access_struct} *params) \{

            int count = 0;
            if (params == NULL)
            return 0;

            while (params != NULL) \{
            count++;
            params = params->next;
            \}
            return count - 1;
            \}
        }
    }

    ##################################
    # Get entry by index function
    if (${isList}) {
        emit_newline
        emit {
            static struct ${access_struct} *get_${access}_entry_by_index(int index) \{
            struct ${access_struct} *params_iter;
            int count = -1;

            params_iter = ${access}_table_head;
            while (params_iter != NULL) \{
            if (++count == index)
            return params_iter;
            params_iter = params_iter->next;
            \}

            return NULL;
            \}
        }
    }

    ##################################
    # Selector call back function
    if (${isList}) {
        emit_newline
        emit {
            static char * ${access}_selector_cb(int item) \{
            struct ${access_struct} *paramsp;

            paramsp = get_${access}_entry_by_index(item + 1);

            sprintf(tempbuf, "????");
            if (paramsp != NULL) \{
            constructRow${title}(tempbuf, paramsp);
            \}
            return tempbuf;
            \}
        }
    }


    ##################################
    # For Lists, this function returns a new allocated param stucture
    emit_newline
    emit_newline
    emit {
        struct ${access_struct} *
        get${title}ParamsStruct(struct ${access_struct} *params) \{

        struct ${access_struct} *tmp_params;
        if (params == NULL)
        return NULL;

        tmp_params = malloc(sizeof(*tmp_params));

        if (tmp_params == NULL)
        return NULL;

        params->next = tmp_params;

        memset(tmp_params, 0x00, sizeof(*tmp_params));
        return tmp_params;

        \}

    }



    ##################################
    # The access function for each parameter item
    if (!$isPager) {
    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name "[string toupper $name]"
        set field_name_str "\"$upper_name\""
        set field_length "FIELD_${upper_access}_${upper_name}_LENGTH"
        set field_type $item($idx,type)
        set field_subtype "$item($idx,subtype)"
        set field_units $item($idx,units)
        set field_source $item($idx,source)


        if {[string equal $field_type "string"]} {
            if {[string equal $field_subtype "dynamic"]} {
                continue
            }
        }

        if {[string equal $field_source "\"counter\""]} {
            if {[string equal $field_units "\"bytes\""] ||
               [string equal $field_units "\"number\""]} {
                if {[string equal $field_type "unsigned_integer"]} {
                    emit {
                        static unsigned long last_${name}_value = 0;
                    }
                } elseif {[string equal $field_type "unsigned_long"]} {
                    emit {
                        static unsigned long long last_${name}_value = 0;
                    }
                }
                emit {
                    static int counter_${name}_cnt = 0;
                }
            }
        }

        emit {

            static char *get_${name}(char *buf, void *usrptr, int ident) \{
            struct ${access_struct} *params = *((struct ${access_struct} **)usrptr);

        }
        if {$idx == 1} {
            if {$isList} {
                emit {
                    struct ${access_struct} *old_params;
                    old_params = get_${access}_entry_by_index(${access}_current_index);
                    memcpy(&last_params, old_params, sizeof(struct ${access_struct}));
                    free_params(params);
                    if ((params = malloc(sizeof(*params))) == NULL) \{
                    exit(1);
                    \}
                    memset(params, 0x00, sizeof(*params));
                    *((struct ${access_struct} **)usrptr) = params;
                    ${access}_table_head = params;
                    timdiff = get_time_diff(&last_time);
                }
            } else {
                emit {
                    void *ptr = params->userptr;
                    memcpy(&last_params, params, sizeof(struct ${access_struct}));
                    memset(params, 0x00, sizeof(*params));
                    params->userptr = ptr;
                    timdiff = get_time_diff(&last_time);

                }
            }
            emit {
                get${title}(params);
            }

            if {$isList} {
                emit {
                    if (${access}_current_index > count_params(params))
                    ${access}_current_index = count_params(params);
                }
            }
        }


        if {$isList} {
            emit {
                params = get_${access}_entry_by_index(${access}_current_index);
            }
        }


        if {[string equal $field_type "string"]} {
            emit {
                strncpy(buf, params->${name}_value, $field_length);
            }
        } elseif {[string equal $field_type "float"]} {
            emit {
                snprintf(buf, $field_length, "%.*f", $item($idx,precision), params->${name}_value);
            }
        } elseif {[string equal $field_type "boolean"]} {

            if {[string equal $field_source "\"truth\""]} {
                emit {
                    snprintf(buf, $field_length, "%s", params->${name}_value ? "Yes":"No");
                }
            } elseif {[string equal $field_source "\"health\""]} {
                emit {
                    snprintf(buf, $field_length, "%s", params->${name}_value ? "Up":"Down");
                }
            } elseif {[string equal $field_source "\"configuration\""]} {
                emit {
                    snprintf(buf, $field_length, "%s", params->${name}_value ? "Enabled":"Disabled");
                }
            } else {
                Error "Unknown Source for Type=boolean in ${name}"

            }
        } elseif {[string equal $field_type "integer"]} {
            set field_format $item($idx,format)
            if {[string equal $field_format "hex4"]} {
                set force_format $item($idx,forceformat)
                if {[string equal $force_format "no"]} {
                    emit {
                        if (show_decimal == 1)
                            snprintf(buf, $field_length, "%d", params->${name}_value);
                        else
                    }
                }
                emit {
                    snprintf(buf, $field_length, "0x%04x", params->${name}_value);
                }
            } elseif {[string equal $field_format "hex8"]} {
                set force_format $item($idx,forceformat)
                if {[string equal $force_format "no"]} {
                    emit {
                        if (show_decimal == 1)
                            snprintf(buf, $field_length, "%d", params->${name}_value);
                        else
                    }
                }
                emit {
                    snprintf(buf, $field_length, "0x%08x", params->${name}_value);
                }
            } elseif {[string equal $field_format "hex2"]} {
                emit {
                    snprintf(buf, $field_length, "0x%02x", params->${name}_value);
                }
            } else {
                emit {
                    snprintf(buf, $field_length, "%d", params->${name}_value);
                }
            }
        } elseif {[string equal $field_type "unsigned_integer"] || [string equal $field_type "unsigned_long"]} {
            if {[string equal $field_type "unsigned_integer"]} {
                set data_type "unsigned long"
                set print_format "%lu"
                set conversion_function "bytes_to_string"
            } else {
                set data_type "unsigned long long"
                set print_format "%llu"
                set conversion_function "bytes_l_to_string"
            }
            if {[string equal $field_source "\"counter\""]} {
                if {[string equal $field_units "\"bytes\""]} {
                    emit {
                        if (clear_counter == 1) \{
                            if(counter_${name}_cnt == 0) \{
                                last_${name}_value = last_params.${name}_value;
                                counter_${name}_cnt = 1;
                            \}
                            params->${name}_value = params->${name}_value - last_${name}_value;
                        \}
                        else \{
                            counter_${name}_cnt = 0;
                        \}
                        if (read_rates == 0) \{
                        snprintf(buf, $field_length, "${print_format}", params->${name}_value);
                        \} else \{
                            ${data_type} ${name}_diff = params->${name}_value - last_params.${name}_value;
                            double value  = (((double) ${name}_diff) / timdiff) * 1000000.0;
                            bytes_to_bits((int)value, buf, $field_length);
                        \}
                    }
                } elseif {[string equal $field_units "\"number\""]} {
                    emit {
                        if (clear_counter == 1) \{
                            if(counter_${name}_cnt == 0) \{
                                last_${name}_value = last_params.${name}_value;
                                counter_${name}_cnt = 1;
                            \}
                            params->${name}_value = params->${name}_value - last_${name}_value;
                        \}
                        else \{
                            counter_${name}_cnt = 0;
                        \}
                        if (read_rates == 0) \{
                            snprintf(buf, $field_length, "${print_format}", params->${name}_value);
                        \} else \{
                            ${data_type} ${name}_diff = params->${name}_value - last_params.${name}_value;
                            double value  = (((double) ${name}_diff) / timdiff) * 1000000.0;
                            snprintf(buf, $field_length, "%.*f",$item($idx,rate_precision), value);
                        \}
                    }
                } else {
                    emit {
                        snprintf(buf, $field_length, "${print_format}", params->${name}_value);
                    }
                }
            } else {
                if {[string equal $field_units "\"bytes\""]} {
                    emit {
                        if (show_decimal == 1) \{
                            snprintf(buf, $field_length, "${print_format}", params->${name}_value);
                        \} else \{
                        ${conversion_function}(buf, params->${name}_value);
                        \}
                    }
                } else {
                    emit {
                        snprintf(buf, $field_length, "%lu", params->${name}_value);
                    }
                }
            }
        } elseif {[string equal $field_type "long"]} {
            emit {
                snprintf(buf, $field_length, "%ll", params->${name}_value);
            }
        }
        emit {

                return buf;
            \}

        }
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
        set field_subtype "$item($idx,subtype)"
        set flags "FFLAG_DISPLAYONLY"

        if [string equal $item($idx,align) "normal"] {
            set flags "FFLAG_DISPLAYONLY"
        } else {
            if {$idx==1} then {
                Error "The first field cannot be of follow align type."
            } else {
                # Check the kind and length of previous field
                set pidx [expr {$idx - 1}]
                if {[string equal $item($pidx,align) "follow"]} {
                    Error "The previous field to \"$item($idx,name)\" is of follow align type, hence cant make this \"follow\" type."
                } else {
                    set flags "FFLAG_DISPLAYONLY | FFLAG_FOLLOW"
                }
            }
        }

        if {[string equal $field_type "string"]} {
            if {[string equal $field_subtype "dynamic"]} {
                continue
            }
        }
        if {[string equal $field_subtype "CliOnly"]} {
            continue
        }
        emit_newline
        emit {

            {FIELD_${upper_name}, $field_label,
                $field_help,
                $field_length, get_${name}, NULL,
                FTYPE_NONE, 0, 0, O_EDIT, $flags},
        }

    }

    emit {

        {0, NULL, NULL,
            0, NULL, NULL,
            0, 0, 0, 0, 0}
        \};
    }

    }
    ##################################
    # The Status Display menu function
    emit_newline
    emit_newline
    if (${isPager}) {
    emit {
        typedef struct _PageInfo \{
            char* beginAddress;
            int length;
        \} PageInfo;

        static const int maxNumberOfPages = $item($idx,totalpages);


        static int pageInEdid(struct ${access_struct} *params, PageInfo  pageInfoArray []) \{
            char * tmpPtr = params->textarea_value;
            char * prevTmpPtr = params->textarea_value;
            char * prevPageEnd = params->textarea_value;
            const int linesPerPage = $item($idx,linesperpage);
            int lineCount = 0;
            int pageCount = 0;
            int pageBreaker = 0;
            char * matchingString = \"$item($idx,pagebreaker)\";
            int done = 0;
            while (1) \{
                tmpPtr = strpbrk(tmpPtr, "\\n");

                if (tmpPtr == NULL) \{
                        tmpPtr = params->textarea_value + strlen(params->textarea_value);
                        done = 1;
                \}
                else \{
                    ++lineCount;
                    // Go past the last matching character (newline)
                    ++tmpPtr;
                \}

                //Check if we have a matching string to page break
                if((tmpPtr != prevTmpPtr) && (matchingString != NULL)) \{
                    int stringLength = (tmpPtr - prevTmpPtr);
                    char *bptr = malloc(stringLength);
                    memcpy(bptr,prevTmpPtr,stringLength);
                    *(bptr + (stringLength - 1)) = '\\0';
                    if (strcmp(matchingString,bptr) == 0) \{
                        pageBreaker = 1;
                    \}
                    else \{
                        prevTmpPtr = tmpPtr;
                    \}
                    free(bptr);
                \}
                if ((lineCount > linesPerPage - 1)
                    || (pageBreaker == 1) || (done == 1))\{

                    // Reached end of page. Store page info
                    pageInfoArray[pageCount].beginAddress = prevPageEnd;
                    // If pageBreaker skip the string
                        pageInfoArray[pageCount].length = (prevTmpPtr - prevPageEnd);
                    prevPageEnd = tmpPtr;
                    if (pageInfoArray[pageCount].length != 0)
                        ++pageCount;
                    if ((pageCount >= maxNumberOfPages) || (done == 1)) \{
                        break;
                    \}

                    // Reset line counter , page Breaker
                    lineCount = 0;
                    pageBreaker = 0;

                \}
                //set prevTmpPtr and proceed to look for next page breaker
                prevTmpPtr = tmpPtr;
            \}
            return pageCount;
        \}

        int display${title}(MENUINFO *minfo) \{
        DIALOG *dialog;
        int     rc, ans;
        char titlebuf[120];
        char pagebuf[20];

        // TODO Allocate on heap if number of pages increase.


        memset(&last_params, 0x00, sizeof(struct ${access_struct}));
        struct ${access_struct} *params = NULL;

        if ((params = malloc(sizeof(*params))) == NULL) \{
        return RC_ERROR;
        \}
        memset(params, 0x00, sizeof(*params));
    }
    if ($board_access) {
        emit_newline
        emit {
            \{
                HAL *hp;
                hp = minfo->minfo_userptr;
                if (hp != NULL) \{
                    board_number = hal_get_board_number(hp);
                    params->userptr = minfo->minfo_userptr;
                \}
            \}
        }
    }
    emit {
        rc = get${title}(params);

        if (rc != RC_NORMAL) \{
        free_params(params);
        return RC_ERROR;
        \}
        PageInfo  pageInfoArray[maxNumberOfPages];
        int pageCount = pageInEdid(params, pageInfoArray);
        int curPage = 0;
        while (1) \{
            int titlelen = strlen(minfo->minfo_title);
            sprintf(titlebuf, "%s", minfo->minfo_title);
            int tabcount = 0;
            if (titlelen <= HEADER_LENGTH - 8) \{
                tabcount = (HEADER_LENGTH - titlelen) / 8;
            \} else \{
                memcpy(titlebuf, minfo->minfo_title, HEADER_LENGTH -1);
                strcat(titlebuf, ":");
            \}
            int i;
            for (i = 0; i < tabcount; ++i) \{
                strcat(titlebuf, "\\t");
            \}
            sprintf(pagebuf, "Page %d of %d", curPage+1, pageCount);
            strcat(titlebuf, pagebuf);
            show_title(minfo, titlebuf);
            char *bufptr = malloc(pageInfoArray[curPage].length+128);
            memcpy(bufptr,pageInfoArray[curPage].beginAddress,pageInfoArray[curPage].length);
            *(bufptr + pageInfoArray[curPage].length -1) = '\\0';
            dialog = show_dialog(minfo, 2, 0, bufptr);
            show_info (minfo, "Previous, Next or Quit? ");
            ans = get_prevnextquit(minfo);
            remove_dialog(dialog);
            free(bufptr);

            if(ans == 'n')
                curPage = (curPage < pageCount-1) ? (curPage + 1) : 0;
            else if (ans == 'p')
                curPage = (curPage > 0) ? (curPage - 1) : pageCount - 1;
            else
                break;
        \}
        free_params(params);
        return 0;
    \}
    }

    } else {

    emit {

        int display${title}(MENUINFO *minfo) \{
        FORM    *form = NULL;
        int     rc;
        int     retval = RC_NORMAL;
        read_rates = 0;
        clear_counter = 0;
        show_decimal = 0;
        memset(&last_params, 0x00, sizeof(struct ${access_struct}));
        timdiff = get_time_diff(&last_time);
    }
    if (${isList}) {
        emit  {
            MENU *selector;
            int number_of_items;
            char titlebuf[120];
            char keylist[12];
            int idx;
        }
    }
    emit {
        struct ${access_struct} *params = NULL;

        if ((params = malloc(sizeof(*params))) == NULL) \{
        return RC_ERROR;
        \}
        memset(params, 0x00, sizeof(*params));
    }

    if (${isList}) {
        emit  {
            ${access}_table_head = params;
        }
    }

    if ($board_access) {
        emit_newline
        emit {
            \{
            HAL *hp;
            hp = minfo->minfo_userptr;
            if (hp != NULL) \{
            board_number = hal_get_board_number(hp);
            params->userptr = minfo->minfo_userptr;
            \}
            \}
        }
    }


    emit {
        rc = get${title}(params);

        if (rc != RC_NORMAL) \{
        free_params(params);
        return RC_ERROR;
        \}
    }

    if (${isList}) {
        emit_newline
        emit {
            sprintf(titlebuf, "%s\\n  \\n   </U>", minfo->minfo_title);
            constructHeader${title}(titlebuf + strlen(titlebuf));
            strcat(titlebuf, "<!U>");

            show_title(minfo, titlebuf);

            number_of_items = count_params(params);

            selector = create_selector(number_of_items, ${access}_selector_cb);

            set_menu_windows(minfo, selector);

            post_menu(selector);

            show_info(minfo, ${access}_help);

            while (1) \{
            strcpy(keylist, "T");
            rc = process_selector(minfo, selector, keylist);

            if (rc == RC_EXIT) \{
            retval = RC_EXIT;
            goto QUIT_LABEL;
            break;
            \}
            if (rc == RC_QUIT) \{
            retval = RC_NORMAL;
            goto QUIT_LABEL;
            break;
            \}

            switch (keylist[0]) \{

            case 'T':
            // Timedout
            idx = rc + 1;
            free_params(params);
            if ((params = malloc(sizeof(*params))) == NULL) \{
            goto QUIT_LABEL;
            \}
            memset(params, 0x00, sizeof(*params));
            ${access}_table_head = params;
            rc = get${title}(params);
            if (rc != RC_NORMAL) \{
            goto QUIT_LABEL;
            \}
            number_of_items = count_params(params);

            clear_selector_init(selector);
            unpost_menu(selector);
            refresh_selector(selector, number_of_items, ${access}_selector_cb);
            if (idx > 0 && idx <= number_of_items)
            set_selector_item(selector, idx - 1);
            else
            set_selector_item(selector, number_of_items - 1);
            post_menu(selector);

            break;

            case '\\0':
            set_selector_init(selector);
            unpost_menu(selector);
            ${access}_current_index = rc + 1;
        }
    }

    emit_newline
    emit {

        show_title(minfo, NULL);
        show_decimal = 0;

        form = create_form(itemdefs, 0, 0, &params);

        set_form_windows(minfo, form);

        post_form(form);

        while (1) \{
        rc = process_form(minfo, form, helpstr,
                         FORM_DISPLAY_ONLY | FORM_AUTO_REFRESH);

        if (rc == 'd' || rc == 'D') \{
            show_decimal = 1 - show_decimal;
            continue;
        \}

        if (rc == 'r') \{
        if (read_rates == 1)
        read_rates = 0;
        else
        read_rates = 1;
        continue;
        \}

        if (rc == 'c') \{
        if (clear_counter == 1)
        clear_counter = 0;
        else
        clear_counter = 1;
        continue;
        \}


        if (rc == RC_EXIT) \{
        retval = RC_EXIT;
        goto QUIT_LABEL;
        \}
        else \{
        retval = RC_NORMAL;
        break;
        \}
        \}
    }
    if (${isList}) {
        emit {

            unpost_form(form);
            unset_form_windows(form);
            delete_form(form);

            show_title(minfo, titlebuf);
            post_menu(selector);
            show_info(minfo, ${access}_help);
            break;


            \}
            \}
        }
    }

    if (${isList}) {
        emit {
            QUIT_LABEL:
            unpost_menu(selector);
            unset_menu_windows(selector);
            delete_selector(selector);
        }
    } else {
        emit {
            QUIT_LABEL:
            unpost_form(form);
            unset_form_windows(form);
            delete_form(form);
        }
    }

    emit {
        free_params(params);

        return retval;
        \}
    }
    }


    ##################################
    # The Retrieve function
    emit_newline
    emit_newline
    if ($board_access) {
        emit {
            struct ${access_struct} * retrieve${title}Values(HAL *hp) \{
        }
    } else {
        emit {
            struct ${access_struct} * retrieve${title}Values() \{
        }
    }
    emit_newline
    emit {
        int rc;

        struct ${access_struct} *params;
        if ((params = malloc(sizeof(*params))) == NULL) \{
        return NULL;
        \}
        memset(params, 0x00, sizeof(*params));

    }

    if ($board_access) {
        emit_newline
        emit {
            params->userptr = hp;
        }
    }
    emit_newline
    emit {
        /* Call the custom hook to get the current values of the parameters */
        rc = get${title}(params);

        if (rc != RC_NORMAL) \{
        return NULL;
        \}
    }

    emit {
        return params;
        \}
    }

    ##################################
    # The Destroy function
    emit_newline
    emit {
        void destroy${title}Values(struct ${access_struct} *params_head) \{
        if (params_head != NULL) \{
        free_params(params_head);
        \}
        \}
    }

    ##################################
    # The GET CLI function
    emit_newline
    emit_newline
    emit {

        int get_${access}_main(int argc, char **argv) \{

        int rc;
        int publish = 0;

        struct ${access_struct} *params;
        struct ${access_struct} *params_head;


        remote_status_start(${isList});
        remote_status_set_name("$title");
        remote_status_set_command_name("get_${access}");

        if ((params = malloc(sizeof(*params))) == NULL) \{
        remote_status_print_error();
        return -1;
        \}
        params_head = params;
        memset(params, 0x00, sizeof(*params));

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
            case 0:
            board_number = atoi(optarg) - 1;
            break;
            case 1:
            publish = 1;
            break;
            \}
            \}
            else \{

            rc = RC_ERROR;
            break;

            \}
            \}
        }
    } else {
        emit {
            int c, opt_index = 0;

            while(1) \{
            c = getopt_long_only(argc, argv, "h", ${access_options}, &opt_index);
            if (c == -1) \{
            break;
            \}
            else if (c == 0) \{

            switch(opt_index) \{
            case 0:
            publish = 1;
            break;
            \}
            \}
            else \{

            rc = RC_ERROR;
            break;

            \}
            \}
        }

    }

    if ($board_access) {
        emit_newline
        emit {
            HAL *hp = hal_acquire(board_number);
            if (hp == NULL) {
                free_params(params_head);
                remote_status_print_error();
                return -1;
            }
            params->userptr = hp;
            remote_status_set_channel_number(board_number + 1);
        }
    }

    emit_newline
    emit {
        /* Call the custom hook to get the current values of the parameters */
        rc = get${title}(params);

        if (rc != RC_NORMAL) \{
        free_params(params_head);
        remote_status_print_error();
        return -1;
        \}
    }

    if {$isList} {
        emit {
            params = params->next;
        }
    }

    emit_newline
    emit {
        /* Print all parameters in XML form */
        while (params != NULL) \{
        remote_status_new_parameters();
    }

    emit_newline
    foreach {idx} $index_list {
        set name $item($idx,name)
        set upper_name [string toupper $name]
        set field_type $item($idx,type)
        set field_units $item($idx,units)
        set field_source $item($idx,source)
        set field_subtype "$item($idx,subtype)"
        set field_label "\"$item($idx,label)\""
        set field_length_name "FIELD_${upper_access}_${upper_name}_LENGTH"
        set index [expr $idx - 1]

        if {[string equal $field_subtype "MenuOnly"]} {
            continue
        }

        if {[string equal $field_type "float"]} {
            emit {
                remote_status_add_${field_type}_parameter($field_label, params->${name}_value, $field_units, $field_source, $item($idx,precision));
            }
        } else {
            if (!$isPager) {
                emit {
                    remote_status_add_${field_type}_parameter($field_label, params->${name}_value, $field_units, $field_source);
                }
            } else {
                emit {
                    PageInfo  pageInfoArray[maxNumberOfPages];
                   int pageCount = pageInEdid(params, pageInfoArray);
                   int i=0;
                    char *bufptr = (char*) malloc(${field_length_name});
                    char *buftext = bufptr;
                    bzero(bufptr, ${field_length_name});
                    for (i=0; i < pageCount; ++i ) \{
                        memcpy(bufptr,pageInfoArray[i].beginAddress,pageInfoArray[i].length);
                        bufptr += pageInfoArray[i].length;
                    \}
                    remote_status_add_${field_type}_parameter($field_label, buftext, $field_units, $field_source);
                    free(buftext);
                }
            }
        }
    }

    emit_newline
    emit {

        params = params->next;

        \}

        if (publish == 0)
        remote_status_print();
        else
        remote_status_print_notify();

        free_params(params_head);

        return 0;
        \}
    }

}

proc create_status_doc {root base_name isPager isList} {


    set title [get_text_node_value $root "/Menu/Title"]
    set description [get_text_node_value $root "/Menu/Description"]
    set access [get_text_node_value $root "/Menu/Access"]
    set upper_access [string toupper $access]

    set board_access 0
    if [check_node_exists $root "/Menu/BoardAccess"] {
       set value [get_text_node_value $root "/Menu/BoardAccess"]
        if [string equal $value "true"] {
            set board_access 1
        } elseif [string equal $value "false"] {
            set board_access 0
        } else {
            Error "Value \"$value\" not supported for BoardAccess. Should be \"true\" or \"false\""
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

    if {$skip_doc == 0} {

      emit_newline_doc
      emit_doc {

          == $title
      }

      emit_newline_doc
      emit_doc {
Type:: Status
Category:: $title
Description:: $description
      }

      emit_newline_doc
      emit_doc {
=== Get Status Configuration
Command Name:: get_${access}
Description:: Displays the $title
Options:: None
      }
    }
}
