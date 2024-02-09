#!/usr/bin/tclsh

package require tdom

# Source the builder libraries
source "menucli/config_menu_builder.tcl"
source "menucli/list_menu_builder.tcl"
source "menucli/status_menu_builder.tcl"

proc parse_xml_file {xml_file} {

    
    if [catch {set fid [open $xml_file]}] {
        Error "Could not open XML file \"${xml_file}\""
    }

    if [catch {set xml [read $fid]} ] {
        Error "Could not read XML file \"${xml_file}\""
    }

    close $fid

    if [catch {set document [dom parse $xml]} ] {
        Error "Could not parse XML file \"${xml_file}\""
    }

    return $document
} 

proc Warning {message} {

    puts "Warning: $message"
}

proc Debug {message} {

#    puts "$message"
}

proc Error {message} {
    puts "Error: $message"
    exit 1
}

proc get_text_node_value {node xpath} {

    set retval ""

    # Special case: if xpath does not have any "/" in it, prepend a "//"
    if {[string first "/" $xpath] == -1} {
        set xpath "//${xpath}"
    }
    # Make sure the xpath resolves to exactly one element with or without a 
    # single child text node
    set nodes [$node selectNodes "$xpath" type]
    if [string equal $type  "nodes"] {
        if { [llength $nodes] == 1 } {
            set childNodes [$nodes childNodes]
            if { [llength $childNodes] == 1 } {
                if {[$childNodes nodeType] == "TEXT_NODE"} {
                    # get the value
                    set retval [concat [$childNodes nodeValue]]
                } else {
                    # Does not seem to be a pure text node"
                    Warning "\"$xpath\": Not a pure text node"
                }
            } elseif { [llength $childNodes] == 0 } {
                # Empty text node, so return empty string
                set retval ""
            } else {
                # Does not seem to be a pure text node
                Warning "\"$xpath\": Not a text node"
            }
        } else {
            # Matched multiple nodes
            Warning "\"$xpath\": Matched multiple nodes"
        }
    } elseif [string equal $type  "attrnodes"] {
        if { [llength $nodes] == 1 } {
            set retval [string trim [lindex [lindex $nodes 0] 1]]
        } elseif { [llength $nodes] == 0 } {
            Warning "\"$xpath\": No attributes matched"            
        } else {
            Warning "\"$xpath\": Matched  multiple attributes $nodes"
        }

    } else {
        Warning "\"$xpath\": Did not match anything: $type"
    }
    
    return $retval

}

proc check_node_exists {node xpath} {

    set retval ""

    # Special case: if xpath does not have any "/" in it, prepend a "//"
    if {[string first "/" $xpath] == -1} {
        set xpath "//${xpath}"
    }

    # Make sure the xpath resolves to exactly one element with or without a 
    # single child text node
    set nodes [$node selectNodes "$xpath" type]
    return ![string equal $type  "empty"]

}

proc get_text_node_values {node xpath} {

    set retval {}

    # Special case: if xpath does not have any "/" in it, prepend a "//"
    if {[string first "/" $xpath] == -1} {
        set xpath "//${xpath}"
    }
    # Make sure the xpath resolves to a set (zero or more) of elements
    # each of which is a pure text node
    set nodes [$node selectNodes "$xpath" type]
    if { $type == "nodes" } {
        foreach node $nodes {
            set childNodes [$node childNodes]
            if { [llength $childNodes] == 1 } {
                if {[$childNodes nodeType] == "TEXT_NODE"} {
                    # get the value
                    lappend retval [$childNodes nodeValue]
                } else {
                    # Does not seem to be a pure text node"
                    Warning "\"$xpath\": Not a pure text node"
                }
            } elseif { [llength $childNodes] == 0 } {
                # Empty text node, so append empty string
                lappend retval ""
            } else {
                # Does not seem to be a pure text node
                Warning "\"$xpath\": Not a text node"
            }
        }
    } else {
        Warning "\"$xpath\": Did not match anything"
    }
    
    return $retval

}


# Procedure to emit code into oputput file
proc emit {buf} {
    global fd
    puts $fd [string trim [uplevel 1 subst -nocommands [list $buf] ]]
} 

proc emit_newline {} {
    global fd
    puts $fd ""
} 

proc emit_header {buf} {
    global hd
    puts $hd [string trim [uplevel 1 subst -nocommands [list $buf] ]]
} 

proc emit_header_newline {} {
    global hd
    puts $hd ""
} 


proc emit_doc {buf} {
    global td
    puts $td [string trim [uplevel 1 subst -nocommands [list $buf] ]]
}

proc emit_newline_doc {} {
    global td
    puts $td ""
}

# Check input file
if {$argc < 1 } {
    puts "Need XML file name to process"
    return 1
}

set xml_file [lindex $argv 0]

set index  [string last ".xml" $xml_file]
if {$index == -1} {
    puts "File $xml_file needs to end with \".xml\""
    return 1
}
incr index -1

if {$argc < 2 } {

    puts "Need directory name for autogenerating the files"
    return 1
}

set dirs [lindex $argv 1]

set base_name "[string range $xml_file 0 $index]_gen"

set create_doc_value 0 

if {$argc == 3} {

    set c_doc [lindex $argv 2]

    if [string equal $c_doc "create_doc"] {
       set create_doc_value 1
    } else {
       puts "If there are 3 arguments, the last argument should be create_doc for generating the doc files."
       return 1
    }
}

if {$create_doc_value == 1} {

    set doc_file "${dirs}/${base_name}.txt"
    set td [open $doc_file "w"]
  } else {
    set code_file "${dirs}/${base_name}.c"
    set fd [open $code_file "w"]
    set header_file "${dirs}/${base_name}.h"
    set hd [open $header_file "w"]
}

# Parse the input xml file
set doc [parse_xml_file $xml_file]
set root [$doc documentElement]


# Get header info
set cli_type [get_text_node_value $root "/Menu/Type"]

if [string equal $cli_type "config"] {
   
  if {$create_doc_value == 1 } {
     create_config_doc $root $base_name
  } else {
     create_config_cli $root $base_name
  }

}

if [string equal $cli_type "status"] {
 
   if {$create_doc_value == 1} {
     create_status_doc $root $base_name 0 0
   } else {
    create_status_cli $root $base_name 0 0
   }
}

if [string equal $cli_type "statuspager"] {
   if {$create_doc_value == 1} {
     create_status_doc $root $base_name 1 0
   } else {  
    create_status_cli $root $base_name 1 0
   }
}

if [string equal $cli_type "statuslist"] {
   if {$create_doc_value == 1} {
     create_status_doc $root $base_name 0 1
   } else { 
    create_status_cli $root $base_name 0 1
   }  
}

if [string equal $cli_type "list"] {

   if {$create_doc_value == 1} {
       create_list_doc $root $base_name
   } else {
       create_list_cli $root $base_name
   }
}
