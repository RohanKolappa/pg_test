#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

# Run common functions
ensure_generic_device

get_device_type "$1"

load_hardware_profile

# Get netserver IP address
NETSERVER_IP="$2"
if [ x${NETSERVER_IP} = "x" ] ; then
    echo "ERROR: Need to specify IP address of Netperf server."
    exit 1
fi

echo "Using Netperf Server on ${NETSERVER_IP}"

# Setup test parameters
TCP_TEST_NAMES="TCP_STREAM TCP_MAERTS"
TCP_BLOCK_SIZES="1024 2048 4096 16384"
UDP_TEST_NAMES="UDP_STREAM"
UDP_BLOCK_SIZES="256 512 1024 2048"
TEST_NUMBERS="0 1 2 3 4"
TEST_DURATION=10

# Overide for quick tests
if [ x${QUICK_TESTS} = "x1" ] ; then
    TCP_TEST_NAMES="TCP_STREAM"
    TCP_BLOCK_SIZES="1024 2048"
    UDP_TEST_NAMES="UDP_STREAM"
    UDP_BLOCK_SIZES="256 1024 2048"
    TEST_NUMBERS="0 1"
    TEST_DURATION=5
fi

NETPERF=/bin/netperf
COMMONARGS="-H ${NETSERVER_IP} -l ${TEST_DURATION} -a 4096"
COMMONTESTARGS="-s 131072 -S 131072"
WORKDIR=/tmp/networkiotest
UDHCPC=/sbin/udhcpc
IFCONFIG=/sbin/ifconfig
TEST_RESULTS_FILE=/tmp/network_io_results.txt


# Create work dir
/bin/mkdir -p ${WORKDIR}
/bin/rm -rf ${WORKDIR}/*

# Procedure to acquire IP address for an interface
enable_interface() {
   local interface=$1
   echo -n "Acquiring IP Address for interface ${interface}... "
   ${UDHCPC} -i ${interface} -R -p /var/run/${interface}.pid -n -t 15 > /dev/null 2>&1
   if [ x$? != "x0" ] ; then
       echo
       echo "ERROR: Failed to acquire IP address for interface ${interface}"
       echo "Aborting..."
   fi
   ACQUIRED_IP=`${IFCONFIG} ${interface} | head -2 | tail -1 | sed -e 's/.*addr:\([.0-9]*\) *B.*/\1/`
   echo "IP Address: ${ACQUIRED_IP}"
}

# Procedure to release IP address for an interface
disable_interface() {
   local interface=$1
   echo -n "Disabling Interface ${interface}... "
   # Is interface up?
   ${IFCONFIG} ${interface} | grep UP | grep RUNNING > /dev/null 2>&1
   if [ x$? != "x0" ] ; then
       echo "Already disabled."
       return
   fi
   # Ask udhcpc to realease interface
   if [ -f /var/run/${interface}.pid ] ; then
       pid=`cat /var/run/${interface}.pid`
       kill ${pid}
       /bin/rm -f /var/run/${interface}.pid
       sleep 2
   fi
   # Bring down the interface
   ${IFCONFIG} ${interface} down
   echo "Disabled."
}

# Procedure to perform a single netperf test
netperf_test() {
    local TestName=$1
    local BlockSize=$2
    local Interface=$3
    local TestNumber=$4
 
    echo -n "${TestName} Test #${TestNumber} Size = $BlockSize on ${Interface}... "
    
    timer_start
    ${NETPERF} ${COMMONARGS} -t ${TestName} -- ${COMMONTESTARGS} \
        -m ${BlockSize} \
        > ${WORKDIR}/result_${Interface}_${TestName}_${BlockSize}_${TestNumber}.txt

    exit_on_error "netperf utility failed to run"
    timer_stop
    echo "Done. [${TIME_TAKEN} secs]"
}

# Procedure to compute and display result for a single test case
compute_result() {
    local TestName=$1
    local BlockSize=$2
    local Interface=$3

    local bw=0
    local total_bw=0
    local loop_count=0
    for test_number in ${TEST_NUMBERS} ; do
        if [ ${TestName} = "UDP_STREAM" ] ; then
            bw=`tail -2 ${WORKDIR}/result_${Interface}_${TestName}_${BlockSize}_${test_number}.txt | head -1 | sed -r -e 's/[ \t]+/ /g'  | cut -f 4 -d ' ' | cut -f 1 -d '.'`
        else 
            bw=`cat ${WORKDIR}/result_${Interface}_${TestName}_${BlockSize}_${test_number}.txt | tail -1 | sed -r -e 's/[ \t]+/ /g' | cut -f 5 -d ' ' | cut -f 1 -d '.'`
        fi
        total_bw=`expr $total_bw + $bw`
        loop_count=`expr $loop_count + 1`
    done
    bw=`expr $total_bw / $loop_count`

    result_string="${TestName} ${Interface} Size = ${BlockSize} Rate = ${bw} Mbits/sec"

    eval local expected_bw=\$NETWORK_IO_${TestName}_${BlockSize}
    if [ x$expected_bw = "x" ] ; then
        echo "No standard profile found. Please Define variable \$NETWORK_IO_${TestName}_${BlockSize}"
        continue
    fi
    bw_diff_str=""
    test_failed=0
    if [ $bw -lt $expected_bw ] ; then
        bw_diff=$(( \( $expected_bw - $bw \) * 100 / $expected_bw ))
        if [ $bw_diff -gt 1 ] ; then
            bw_diff_str="(- ${bw_diff} %%)"
        fi
        if [ $bw_diff -gt 10 ] ; then
            test_failed=1
        fi
    fi
    
    result_string="${result_string} ${bw_diff_str}"
    if [ x$test_failed = "x0" ] ; then
        print_passed_result_string ${result_string}
        TESTS_PASSED_COUNT=`expr ${TESTS_PASSED_COUNT} + 1`
    else
        print_failed_result_string ${result_string}
        TESTS_FAILED_COUNT=`expr ${TESTS_FAILED_COUNT} + 1`
    fi
    TESTS_TOTAL_COUNT=`expr ${TESTS_TOTAL_COUNT} + 1`

}

# Prepare for testing
prepare_tests

# Determine list of interfaces to process
INTERFACE_LIST=""
for interface in 0 1 2 ; do
    eval local exists=\$ETH${interface}_EXISTS
    if [ x$exists = "x1" ] ; then
        INTERFACE_LIST="${INTERFACE_LIST} eth${interface}"
    fi
done

# First disable all interfaces
killall udhcpc > /dev/null 2>&1
for interface in ${INTERFACE_LIST} ; do
    disable_interface ${interface}
done

# Ensure all interfaces have connectivity
for interface in ${INTERFACE_LIST} ; do
    echo  "Checking connectivity for interface ${interface}..."
    enable_interface ${interface}
    /bin/ping ${NETSERVER_IP} -c 1 -I ${interface} > /dev/null 2>&1
    exit_on_error "Could not contact ${NETSERVER_IP} via interface ${interface}"
    echo "Interface ${interface} OK"
    disable_interface ${interface}
done

echo

# Perform netperf tests
for interface in ${INTERFACE_LIST} ; do

    enable_interface ${interface}

    for test_name in ${TCP_TEST_NAMES} ; do
        for block_size in ${TCP_BLOCK_SIZES} ; do
            for test_number in ${TEST_NUMBERS} ; do
                netperf_test ${test_name} ${block_size} ${interface} ${test_number}
            done
        done
    done
    
    for test_name in ${UDP_TEST_NAMES} ; do
        for block_size in ${UDP_BLOCK_SIZES} ; do
            for test_number in ${TEST_NUMBERS} ; do
                netperf_test ${test_name} ${block_size} ${interface} ${test_number}
            done
        done
    done

    disable_interface ${interface}

done

# Show Results
for interface in ${INTERFACE_LIST} ; do
    display_banner "Results ${interface}"
    for test_name in ${TCP_TEST_NAMES} ; do
        for block_size in ${TCP_BLOCK_SIZES} ; do
            compute_result ${test_name} ${block_size} ${interface}
        done
    done
    
    for test_name in ${UDP_TEST_NAMES} ; do
        for block_size in ${UDP_BLOCK_SIZES} ; do
            compute_result ${test_name} ${block_size} ${interface}
        done
    done
done


# Print test summary
print_test_summary

