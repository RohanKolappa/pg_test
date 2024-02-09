#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

# Run common functions
ensure_generic_device

if [ x$1 = "x" ] ; then
    echo "ERROR: Please provide Serial Number of device."
    exit 1
fi
SERIAL_NUMBER="$1"

REPORT_FILE="/tmp/testreport-${SERIAL_NUMBER}.txt"

RESULTS_FILE_LIST="check_device_results.txt network_io_results.txt disk_io_results.txt" 
FILE_LIST="${RESULTS_FILE_LIST} network_info.txt disk_info.txt check_device.log" 


# Delete previous report file
/bin/rm -f ${REPORT_FILE}

# Write Header
echo -n "Report Generated on " >> ${REPORT_FILE}
/bin/date >> ${REPORT_FILE}
echo >> ${REPORT_FILE}
echo "Report for Device Serial Number: ${SERIAL_NUMBER}" >> ${REPORT_FILE}

# Parse for failures
failures_detected=0
for file_name in ${RESULTS_FILE_LIST} ; do 
    if [ -f /tmp/${file_name} ] ; then
        grep "FAIL" /tmp/${file_name} > /dev/null
        if [ x$? = "x0" ] ; then
            echo "WARNING: Some failures detected in ${file_name}."
            failures_detected=1
        fi
    fi
done

# Concatinate all the files
missing_files=0
for file_name in ${FILE_LIST} ; do 
    if [ ! -f /tmp/${file_name} ] ; then
        missing_files=1
        echo "ERROR: Missing file ${file_name}."
        continue
    fi

    echo >> ${REPORT_FILE}   
    echo "************ ${file_name} ************" >> ${REPORT_FILE}   
    echo >> ${REPORT_FILE}   
    cat /tmp/${file_name} >> ${REPORT_FILE}
done

echo
if [ x$missing_files = "x1" ] ; then
    echo "WARNING: Some tests have not been run. Report is incomplete."
fi
if [ x$failures_detected = "x1" ] ; then
    echo "WARNING: Report includes some failed tests."
fi
echo "Report ready in file ${REPORT_FILE}"
echo

