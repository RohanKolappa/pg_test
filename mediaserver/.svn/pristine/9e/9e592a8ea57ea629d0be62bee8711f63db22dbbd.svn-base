#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

LOG_FILE=/tmp/check_device.log
TEST_RESULTS_FILE=/tmp/check_device_results.txt
DISK_INFO_FILE=/tmp/disk_info.txt
NETWORK_INFO_FILE=/tmp/network_info.txt

# Variable that gets set to 1 if a test passes
TEST_RESULT=0
LAST_RESULT=0

# Value of next test case
TEST_STRING="Fix Me"


# Convenience Functions

# Announce a test has passed
test_passed() {
    TEST_RESULT=1
}

# Test return value of last command executed to see if a test passed
check_return_code() {
    local retcode=$?
    TEST_RESULT=0
    if [ x$retcode = "x0" ] ; then
        TEST_RESULT=1
    fi
}


# Show the next test case
set_test_string() {
    TEST_STRING=$1
    echo -n "${TEST_STRING} "
    echo  >> ${LOG_FILE}
    echo "**** ${TEST_STRING} ****" >> ${LOG_FILE}
}

# Log the expected match string
set_match_string() {
    local match_string="$1"
    echo "Expected: ${match_string}" >> ${LOG_FILE}
}

# Compare two strings and pass test if string 2 exactly equals string 1
match_and_show_result() {
    set_match_string "$2"
    echo Result: "$1" >> ${LOG_FILE}
    if [ x$1 = x${2} ] ; then
        test_passed
    fi
    show_result
}

# Compare two strings and pass test if string 2 is contained in string 1
compare_and_show_result() {
    set_match_string "$2"
    echo Result: "$1" >> ${LOG_FILE}
    echo "$1" | grep "$2" >> ${LOG_FILE} 2>&1
    check_return_code
    show_result
}

# Compare two strings and set success if string 2 exactly equals string 1
match_result() {
    set_match_string "$2"
    echo Result: "$1" >> ${LOG_FILE}
    TEST_RESULT=0
    if [ x$1 = x${2} ] ; then
        TEST_RESULT=1 
    fi
}

# Compare two strings and set success if string 2 is contained in string 1
compare_result() {
    set_match_string "$2"
    echo Result: "$1" >> ${LOG_FILE}
    echo "$1" | grep "$2" >> ${LOG_FILE} 2>&1
    check_return_code
}

# Log result string
log_result() {
    echo Result: "$1" >> ${LOG_FILE}
}

# Pretty print a test result
show_result() {
    $MOVE_TO_COL
    if [ x$TEST_RESULT = "x1" ] ; then
        $SETCOLOR_SUCCESS
        echo -n "PASSED"
        echo "Test Case: ${TEST_STRING}: PASSED" >> ${LOG_FILE}
        printf "PASSED\t\t${TEST_STRING}\n" >> ${TEST_RESULTS_FILE}
        TESTS_PASSED_COUNT=`expr ${TESTS_PASSED_COUNT} + 1`
    else
        $SETCOLOR_FAILURE
        echo -n "FAILED"
        echo "Test Case: ${TEST_STRING}: FAILED" >> ${LOG_FILE}
        printf "FAILED\t\t${TEST_STRING}\n" >> ${TEST_RESULTS_FILE}
        TESTS_FAILED_COUNT=`expr ${TESTS_FAILED_COUNT} + 1`
    fi
    TESTS_TOTAL_COUNT=`expr ${TESTS_TOTAL_COUNT} + 1`
    $SETCOLOR_NORMAL

    echo

    # Reset for next round
    LAST_RESULT=${TEST_RESULT}
    TEST_RESULT=0
    TEST_STRING="Fix Me"
}


# Find out what device we are
DEVICE=xxx
if [ -f /etc/config/hw_profile_v2dtx.txt ] ; then
    DEVICE=V2D-Tx
elif [ -f /etc/config/hw_profile_v2drx.txt ] ; then
    DEVICE=V2D-Rx
elif [ -f /etc/config/hw_profile_v2dxp200.txt ] ; then
    DEVICE=`cat /etc/config/hw_profile_v2dxp200.txt | sed s/-Device//`
elif [ -f /etc/config/hw_profile_dms.txt ] ; then
    DEVICE=`cat /etc/config/hw_profile_dms.txt | sed s/-Device//`
elif [ -f /etc/config/hw_profile_ms.txt ] ; then
    DEVICE=MS
elif [ -f /etc/config/hw_profile_orb.txt ] ; then
    DEVICE=`cat /etc/config/hw_profile_orb.txt | sed s/-Device//`
elif [ -f /etc/config/hw_profile_generic.txt ] ; then
    DEVICE=Generic
fi

if [ x${DEVICE} = "xxxx" ] ; then
    echo "Error: Could not determine device type."
    exit 1
fi

# Device name needs to come from command line in case of generic device
if [ x${DEVICE} = "xGeneric" ]; then
    if [ x$1 = "x" ] ; then
        echo "Provide device type as an argument."
        echo 
        echo "List of supported device types:"
        display_dms_device_list
        display_v2d_device_list
        display_orb_device_list
        exit 1
    fi
fi


# If command line provides hardware  revision (R2, R3, etc.),
# append revison to the device name we already have.
# Else, override device name from command line.
if [ x$1 != "x" ] ; then
    echo $1 | grep -E '^R[0-9]+$'
    if [ x$? = "x0" ] ; then
        DEVICE=${DEVICE}-${1}
    else
        DEVICE=$1
    fi
fi

# Prepare for testing
prepare_tests

# Initialize variables to (in)sane values
CPU_MODEL_NAME="XXX"
CPU_CACHE="XXX"
MEMORY_SIZE="XXX"
BIOS_VENDOR="XXX"
BIOS_VERSION="XXX"
BIOS_RELEASE_DATE="XXX"


# Load hardware profile file
local hwprofile=/etc/config/hwprofiles/${DEVICE}.profile
if [ -f ${hwprofile} ] ; then
    . ${hwprofile}
else 
    echo "Error: Did not find hardware profile file ${hwprofile}"
    exit 1
fi

/bin/rm -f ${LOG_FILE}

echo "Scanning hardware for device type ${DEVICE}..."

# Check info supplied by BIOS 

set_test_string "BIOS Vendor"
compare_and_show_result "`/usr/sbin/dmidecode -s bios-vendor`" "${BIOS_VENDOR}"

set_test_string "BIOS Version"
compare_and_show_result "`/usr/sbin/dmidecode -s bios-version`" "${BIOS_VERSION}"

set_test_string "BIOS Release Date"
compare_and_show_result "`/usr/sbin/dmidecode -s bios-release-date`" "${BIOS_RELEASE_DATE}"

if [ "x${SYSTEM_MANUFACTURER}" != "x" ] ; then
    set_test_string "System Manufacturer"
    compare_and_show_result "`/usr/sbin/dmidecode -s system-manufacturer`" "${SYSTEM_MANUFACTURER}"
fi

if [ "x${SYSTEM_PRODUCT_NAME}" != "x" ] ; then
    set_test_string "System Product Name"
    compare_and_show_result "`/usr/sbin/dmidecode -s system-product-name`" "${SYSTEM_PRODUCT_NAME}"
fi

# We allow multiple sets of allowable CPU parameters
echo  >> ${LOG_FILE}
echo "**** CPU Model Tests ****" >> ${LOG_FILE}


# Loop through all listed CPU models and find first match
if [ x${CPU_MODEL_COUNT} = "x" ] ; then
    CPU_MODEL_COUNT=0
    CPU_COUNT_1=${CPU_COUNT}
    CPU_MODEL_NAME_1=${CPU_MODEL_NAME}
    CPU_CACHE_1=${CPU_CACHE}
    MEMORY_SIZE_1=${MEMORY_SIZE}
fi
local cpu_model_index=1
local cpu_count_result=0
local cpu_model_name_result=0
local cpu_cache_result=0
while true; do

    eval local cpu_count=\$CPU_COUNT_${cpu_model_index}
    eval local cpu_model_name=\$CPU_MODEL_NAME_${cpu_model_index}
    eval local cpu_cache=\$CPU_CACHE_${cpu_model_index}

    local all_passed=0
    # Check Number of CPUs
    match_result "`cat /proc/cpuinfo | grep processor | wc -l`" "${cpu_count}"
    cpu_count_result=${TEST_RESULT}

    # Check CPU model name
    compare_result "`cat /proc/cpuinfo | grep \"model name\" | head -1`" "${cpu_model_name}"
    cpu_model_name_result=${TEST_RESULT}

    # Check CPU cache size
    compare_result "`cat /proc/cpuinfo | grep \"cache size\" | head -1`" "${cpu_cache}"
    cpu_cache_result=${TEST_RESULT}
    
    # Break if all tests pass
    if [ $cpu_count_result = 1 -a $cpu_model_name_result = 1 -a $cpu_cache_result = 1 ] ; then
        break
    fi

    # Break after last model index
    cpu_model_index=`expr ${cpu_model_index} + 1`
    if [ $cpu_model_index -gt ${CPU_MODEL_COUNT} ]; then
        break
    fi
done


set_test_string "CPU Count"
TEST_RESULT=${cpu_count_result}
show_result

set_test_string "CPU Model Name"
TEST_RESULT=${cpu_model_name_result}
show_result

set_test_string "CPU Cache Size"
TEST_RESULT=${cpu_cache_result}
show_result

# Loop through all listed memory indexes and find first match
if [ x${MEMORY_COUNT} = "x" ] ; then
    MEMORY_COUNT=0
    MEMORY_SIZE_1=${MEMORY_SIZE}
fi
local memory_index=1
local memory_size_result=0
while true; do

    eval local memory_size=\$MEMORY_SIZE_${memory_index}
    # Check memory 
    if [ x${PRE_TEST} != "x1" ] ; then 
        compare_result "`cat /proc/meminfo | grep MemTotal`" "${memory_size}"
        memory_size_result=${TEST_RESULT}
    else
        memory_size_result=1
    fi

    # Break if test passes
    if [ $memory_size_result = 1 ] ; then
        break
    fi

    # Break after last model index
    memory_index=`expr ${memory_index} + 1`
    if [ $memory_index -gt ${MEMORY_COUNT} ]; then
        break
    fi
done



if [ x${PRE_TEST} != "x1" ] ; then 
    set_test_string "System Memory"
    TEST_RESULT=${memory_size_result}
    show_result
fi

# Check Network cards
/bin/rm -f ${NETWORK_INFO_FILE}
/bin/touch ${NETWORK_INFO_FILE}
for interface in 0 1 2 ; do
    eval local exists=\$ETH${interface}_EXISTS
    if [ x$exists = "x1" ] ; then 
        set_test_string "Ethernet Interface ${interface}"
        eval local slot=\$ETH${interface}_PCI_SLOT
        eval local vendor=\$ETH${interface}_PCI_VENDOR
        eval local device=\$ETH${interface}_PCI_DEVICE
        set_match_string "${slot}"
        set_match_string "${vendor}"
        set_match_string "${device}"
        if [ -z "${slot}" -o -z "${vendor}" -o -z "${device}" ] ; then
            echo "Error: Some parameters not defined for Ethernet Interface ${interface}"
            exit 1
        fi
        local result=`/sbin/lspci -vmm -nn -s $slot 2>&1`
        log_result "${result}"
        echo ${result} | grep "${vendor}" >> ${LOG_FILE}
        if [ x$? = "x0" ] ; then
            echo ${result} | grep "${device}" >> ${LOG_FILE}
            check_return_code
        fi
        show_result

        # Save off mac address into a scpecial file
        local mac_address=`/sbin/ifconfig eth${interface} | grep HWaddr | sed -e 's/.*HWaddr \(.*\) */\1/'`
        echo "Interface ${interface} Mac Address = ${mac_address}" >> ${NETWORK_INFO_FILE}
    fi
done

# Check RAID device
if [ x$RAID_CONTROLLER_EXISTS = "x1" ] ; then 
    set_test_string "RAID Controller PCI"
    set_match_string "${RAID_CONTROLLER_PCI_SLOT}"
    set_match_string "${RAID_CONTROLLER_PCI_VENDOR}"
    set_match_string "${RAID_CONTROLLER_PCI_DEVICE}"
    local result=`/sbin/lspci -vmm -nn -s ${RAID_CONTROLLER_PCI_SLOT} 2>&1`
    log_result "${result}"
    echo ${result} | grep "${RAID_CONTROLLER_PCI_VENDOR}" >> ${LOG_FILE}
    if [ x$? = "x0" ] ; then
        echo ${result} | grep "${RAID_CONTROLLER_PCI_DEVICE}" >> ${LOG_FILE}
        check_return_code
    fi
    show_result 

    set_test_string "RAID Controller Model"
    compare_and_show_result "`tw_cli info c0 model 2>&1`" "${RAID_CONTROLLER_MODEL}"

    if [ x${PRE_TEST} != "x1" ] ; then 
        set_test_string "RAID Controller Bios"
        compare_and_show_result "`tw_cli info c0 bios 2>&1`" "${RAID_CONTROLLER_BIOS}"

        set_test_string "RAID Controller Firmware"
        compare_and_show_result "`tw_cli info c0 firmware 2>&1`" "${RAID_CONTROLLER_FIRMWARE}"

        set_test_string "RAID Controller Auto Rebuild Policy"
        compare_and_show_result "`tw_cli info c0 autorebuild 2>&1`" "Auto-Rebuild Policy = off"

        if [ x${RAID_CONTROLLER_BBU_EXISTS} = "x1" ] ; then
            set_test_string "RAID Controller BBU Status"
            compare_and_show_result "`tw_cli /c0/bbu show status 2>&1`" "= OK"
        fi

        unit_list="u0"
        if [ x${RAID_CONTROLLER_NUMB_UNITS} = "x2" ] ; then
            unit_list="u0 u1"
        fi
        for unit in ${unit_list} ; do 
            set_test_string "RAID Controller ${unit} Auto Verify Policy"
            compare_and_show_result "`tw_cli info c0 ${unit} autoverify 2>&1`" "Auto Verify Policy = off"
        
            set_test_string "RAID Controller ${unit} Cache Policy"
            compare_and_show_result "`tw_cli info c0 ${unit} cache 2>&1`" "Write Cache = ${RAID_CONTROLLER_WRITE_CACHE}"

            set_test_string "RAID Controller ${unit} Raid Type"
            match_and_show_result "`tw_cli info c0 unitstatus | grep \"${unit} \" | sed -r -e 's/[ \t]+/ /g' | cut -f 2 -d ' '  2>&1`" "${RAID_CONTROLLER_RAIDTYPE}"
        done
    fi
fi

# Check hard drives
/bin/rm -f ${DISK_INFO_FILE}
/bin/touch ${DISK_INFO_FILE}
if [ x$RAID_CONTROLLER_EXISTS = "x1" ] ; then 

    # Number of hard drives
    if [ x${HARD_DRIVE_COUNT} = "x" ] ; then
        HARD_DRIVE_COUNT=0
    fi
    set_test_string "Hard Drive Count"
    compare_and_show_result "`tw_cli info c0 numdrives 2>&1`" "Number of Drives = ${HARD_DRIVE_COUNT}"
    found_all_drives=${LAST_RESULT}

    # Check model/capacity for each drive
    # Currently all drives are expected to be exactly alike
    drive=0
    drive_mismatch=0
    first_model=x
    if [ x${HARD_DRIVE_MODEL_COUNT} = "x" ] ; then
        HARD_DRIVE_MODEL_COUNT=0
    fi
    while true; do
        set_test_string "Hard Drive ${drive} Detect"
        compare_and_show_result "`tw_cli info c0 p${drive} status 2>&1`" "Status = OK"
        # There can be multiple allowed hard drive models
        set_test_string "Hard Drive ${drive} Model/Capacity"
        model_count=1
        while true; do

            eval local model=\$HARD_DRIVE_MODEL_${model_count}
            eval local capacity=\$HARD_DRIVE_CAPACITY_${model_count}

            set_match_string "${model}"
            tw_cli info c0 p${drive} model 2>&1 | grep "Model = ${model}" >> ${LOG_FILE}
            if [ x$? = "x0" ] ; then
                set_match_string "${capacity}"
                tw_cli info c0 p${drive} capacity 2>&1 | grep "Capacity = ${capacity}" >> ${LOG_FILE}
                if [ x$? = "x0" ] ; then
                    test_passed
                    # Check for drive mismatch
                    if [ x$first_model = "xx" ] ; then
                        first_model=${model_count}
                    else
                        if [ x$first_model != x${model_count} ] ; then
                            drive_mismatch=1
                        fi
                    fi
                    local serial_number=`tw_cli info c0 p${drive} serial 2>&1 | grep Serial | cut -f 4 -d ' '`
                    echo "Port ${drive} SerialNumber = ${serial_number}" >> ${DISK_INFO_FILE}
                    break
                fi
            fi
            model_count=`expr ${model_count} + 1`
            if [ $model_count -gt ${HARD_DRIVE_MODEL_COUNT} ]; then
                break
            fi
            
        done
        show_result

        drive=`expr ${drive} + 1`
        if [ $drive -ge ${HARD_DRIVE_COUNT} ]; then
            break
        fi
        if [ $drive -ge 16 ]; then
            break
        fi
    done
    
    if [ x${STRICT_CHECK} != "x0" ] ; then 
        set_test_string "Hard Drives Similar"
        if [ x$found_all_drives = "x1" -a x$drive_mismatch = "x0" ] ; then
            test_passed
        fi
        show_result
    fi

fi

# Check File Systems
if [ x${FILE_SYSTEM_COUNT} = "x" ] ; then
    FILE_SYSTEM_COUNT=0
fi
fs_count=0
while true; do
    fs_count=`expr ${fs_count} + 1`
    if [ $fs_count -gt ${FILE_SYSTEM_COUNT} -o $fs_count -gt 8 ]; then
        break
    fi
    # FS Exists
    if [ x${PRE_TEST} != "x1" ] ; then 
        set_test_string "File System ${fs_count} Detect"
        result=`/bin/df  /data${fs_count} 2>&1 | tail -1`
        compare_and_show_result "${result}" "/data${fs_count}"
        
        # FS Size
        set_test_string "File System ${fs_count} Size"
        eval local fs_size=\$FILE_SYSTEM_${fs_count}_SIZE
        compare_and_show_result "${result}" "${fs_size}"
    fi
done


# Check front panel 
if [ x$FRONT_PANEL_EXISTS = "x1" ] ; then 
    # Detect the device
    set_test_string "Front Panel Detected"
    if [ x$FRONT_PANEL_DEVICE_TYPE = "xUSB" ] ; then 
        set_match_string "${FRONT_PANEL_USB_VENDOR}"
        set_match_string "${FRONT_PANEL_USB_PRODUCT}"
        /sbin/lsusb -d ${FRONT_PANEL_USB_VENDOR}:${FRONT_PANEL_USB_PRODUCT} >> ${LOG_FILE} 2>&1
        check_return_code
    fi
    show_result

    # If frontpanel starts successfully we expect a special 
    # file in /tmp
    set_test_string "Front Panel Started"
    set_match_string "File /tmp/frontpanel_started.txt exists"
    /bin/ls /tmp/frontpanel_started.txt >> ${LOG_FILE} 2>&1
    check_return_code
    show_result

    # Visual test: show "TEST" code on LCD panel
    # Do this only if currently in "Ready" status
    grep "8" /tmp/status.led >> ${LOG_FILE} 2>&1
    if [ x$? = "x0" ] ; then
        /bin/echo 9 > /tmp/status.led
    fi
fi

# Test Encoder boards
local numb_boards=0
for board in 1 2 ; do
    eval local exists=\$FPGA_BOARD${board}_EXISTS
    if [ x$exists = "x1" ] ; then 
        set_test_string "FPGA Board ${board}"
        eval local slot=\$FPGA_BOARD${board}_PCI_SLOT
        eval local vendor=\$FPGA_BOARD${board}_PCI_VENDOR
        eval local device=\$FPGA_BOARD${board}_PCI_DEVICE
        if [ -z "${slot}" -o -z "${vendor}" -o -z "${device}" ] ; then
            echo "Error: Some parameters not defined for FPGA board ${board}"
            exit 1
        fi
        set_match_string "${slot}"
        set_match_string "${vendor}"
        set_match_string "${device}"
        /sbin/lspci -vmm -nn -s $slot | grep "${vendor}" >> ${LOG_FILE} 2>&1
        if [ x$? = "x0" ] ; then
            /sbin/lspci -vmm -nn -s $slot | grep "${device}" >> ${LOG_FILE} 2>&1
            check_return_code
        fi
        show_result 
        numb_boards=`expr ${numb_boards} + 1`
    fi
done

# For XP boards test presense of on board USB devices
if [ x${FPGA_BOARD_FTD_EXISTS} = "x1" ] ; then
    if [ x$numb_boards != "x0" ] ; then
        set_test_string "FPGA USB FTD (${numb_boards})"
        set_match_string "0403:6010"
        count=`/sbin/lsusb -d  0403:6010 | wc -l`
        if [ x${count} = x${numb_boards} ] ; then
            test_passed
        fi
        show_result
    fi
fi

if [ x${FPGA_BOARD_PHILIPS_EXISTS} = "x1" ] ; then
    if [ x$numb_boards != "x0" ] ; then
        set_test_string "FPGA USB Philips (${numb_boards})"
        set_match_string "04cc:1520"
        count=`/sbin/lsusb -d  04cc:1520 | wc -l`
        if [ x${count} = x${numb_boards} ] ; then
            test_passed
        fi
        show_result
    fi
fi

if [ x${PRE_TEST} = "x1" ] ; then 
    echo ""
    echo "WARNING: Some tests ignored."
fi

# Print test summary
print_test_summary

