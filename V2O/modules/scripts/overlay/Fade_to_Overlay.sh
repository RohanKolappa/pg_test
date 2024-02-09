#!/bin/sh
#
# This script fades out the remote window, execute $1, fade in to target alpha $2, all on $3 board
#

# Grab command line parameters
script=$1
target_alpha=$2
bNumber=$3

# Examples:

# To position overlay with alpha 0 on board 1:
# fade_to_Overlay "/V2O/bin/set_overlay_parm -videosrc overlay -hpos Left -vpos 0 -hscale FixRatio -vscale FixRatio -hres 1280 -vres 1200" 0 1

# To set to local (alpha 100) on board 2
# fade_to_Overlay "/V2O/bin/set_overlay_parm -videosrc overlay -hpos Left -vpos 0 -hscale FixRatio -vscale FixRatio -hres 1280 -vres 1200" 100 2

# To set overlay with alpha 50 on board 1
# fade_to_Overlay "/V2O/bin/set_overlay_parm -videosrc overlay -hpos Left -vpos 0 -hscale FixRatio -vscale FixRatio -hres 1280 -vres 1200" 50 1


# Functions to echo and/or print to syslog
logprint() {
    # logger "$1"
    echo "$1"
}

logdebug() {
    echo "$1"
}


# Get current transperency
current_alpha=$(/V2O/bin/get_display_parm -boardNumber $bNumber | grep -i transparency | sed 's/.*value=\"\(.*\)\".*/\1/')
logprint "Current transparency: $current_alpha"

alpha_diff=$(( 100 - $target_alpha ))
logprint "Target Difference: $alpha_diff"

intermediate_alpha=$(( 100 - $alpha_diff * 2 / 4 ))
logprint "Intermediate Target: $intermediate_alpha"

final_alpha=$target_alpha
logprint "Final Target: $final_alpha"


#
# FADE OUT
#
logprint "FADE OUT BOARD $bNumber"

a2=$current_alpha
logprint "Fade Out start transparency: $a2 on $bNumber"
while [ "$a2" -le "100" ]
do
    logdebug "Setting transparency to $a2 on $bNumber"
    /V2O/bin/set_display_parm -transparency $a2 -boardNumber $bNumber > /dev/null
    a2=$(( $a2 + 2 ))
    usleep 5000 
done 
a2=$(( $a2 - 2 ))
logprint "Fade Out end transparency: $a2 on $bNumber"

#
# SET NEW PARAMETERS: Execute command in \"$script\" on board $bNumber
#
logprint "Running command $script on board $bNumber"
$script -boardNumber $bNumber > /dev/null


#
# FADE IN Phase 1
#

logprint "Fade IN start transparency: $a2 on $bNumber"
a2=100
while [ "$a2" -ge "$intermediate_alpha" ]
do
    logdebug "Setting transparency to $a2 on $bNumber"
    /V2O/bin/set_display_parm -transparency $a2 -boardNumber $bNumber > /dev/null
    a2=$(( $a2 - 1 ))
    usleep 25000 
done

#
# FADE IN Phase 2
#

logprint "Fade In Intermediate -> Final alpha: $a2 on $bNumber"
while [ "$a2" -ge "$final_alpha" ]
do
    logdebug "Setting transparency to $a2 on $bNumber"
    /V2O/bin/set_display_parm -transparency $a2 -boardNumber $bNumber > /dev/null
    a2=$(( $a2 - 1 ))
    usleep 5000 
done
a2=$(( $a2 + 1 ))
logprint "Fade In end alpha: $a2 on $bNumber"
logprint "Done!"
