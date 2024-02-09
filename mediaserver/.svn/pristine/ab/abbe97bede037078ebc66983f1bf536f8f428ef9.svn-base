#!/bin/sh

# Source common library
. /etc/scripts/device_check_common.rc

# Run common functions
ensure_generic_device

get_device_type "$1"

load_hardware_profile


# Setup test parameters
FILE_SIZE=1024000
BLOCK_SIZES="512 1024 2048 4096"
TEST_NUMBERS="0 1 2 3 4 5 6 7 8 9"

# Overide for quick tests
if [ x${QUICK_TESTS} = "x1" ] ; then
    FILE_SIZE=10240
    BLOCK_SIZES="512 1024"
    TEST_NUMBERS="0 1"
fi

IOZONE=/bin/iozone
COMMONARGS="-o -+r -w"
TEST_RESULTS_FILE=/tmp/disk_io_results.txt


# Setup File Systems to test
FILE_SYSTEMS=""
if [ x${FILE_SYSTEM_COUNT} = "x1" ] ; then
    FILE_SYSTEMS="/data1"
elif [ x${FILE_SYSTEM_COUNT} = "x2" ] ; then
    FILE_SYSTEMS="/data1 /data2"
else
    echo "No file systems to test"
    exit
fi

# Map of test id to test name
TEST_NAME_0="WRITE"
TEST_NAME_1="READ"

# Procedure to perform a single iozone test
iozone_test() {
    local BlockSize=$1
    local TestId=$2
    local TestNumber=$3
    local FileSystem=$4

    local WorkDir="${FileSystem}/iotest"
    eval local test_name=\$TEST_NAME_${TestId}
    echo -n "${test_name} Test #${TestNumber} Size = $BlockSize in ${FileSystem}... "
    timer_start
    sync
    echo 3 > /proc/sys/vm/drop_caches
    usleep 1000
    ${IOZONE} ${COMMONARGS} -r ${BlockSize} -s ${FILE_SIZE} \
        -f ${WorkDir}/file_${BlockSize}_${TestNumber}.tmp -i ${TestId} \
        > ${WorkDir}/result_${TestId}_${BlockSize}_${TestNumber}.txt
    exit_on_error "iozone utility failed to run"
    timer_stop
    echo "Done. [${TIME_TAKEN} secs]"
}

# Procedure to compute and display result for a single test case
compute_result() {
    local BlockSize=$1
    local TestId=$2
    local FileSystem=$3

    local WorkDir="${FileSystem}/iotest"
    eval local test_name=\$TEST_NAME_${TestId}
    local bw=0
    local total_bw=0
    local loop_count=0
    for test_number in ${TEST_NUMBERS} ; do
        bw=`tail -3 ${WorkDir}/result_${TestId}_${BlockSize}_${test_number}.txt | head -1 | sed -r -e 's/[ \t]+/ /g' | cut -f 4 -d " "`
        total_bw=`expr $total_bw + $bw`
        loop_count=`expr $loop_count + 1`
    done
    bw=`expr $total_bw / $loop_count`

    result_string="${test_name} ${FileSystem} Size = ${BlockSize} Rate = ${bw} Kbytes/sec"    
    eval local expected_bw=\$FILE_SYSTEM_IO_${test_name}_${BlockSize}
    if [ x$expected_bw = "x" ] ; then
        echo "No standard profile found. Please Define variable FILE_SYSTEM_IO_${test_name}_${BlockSize}"
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

# Procedure to delete a test file
delete_test_file() {
    local BlockSize=$1
    local TestNumber=$2
    local FileSystem=$3

    local WorkDir="${FileSystem}/iotest"
    echo -n "Deleting file #${TestNumber} for Size = $BlockSize in ${FileSystem}... "
    timer_start
    /bin/rm -f ${WorkDir}/file_${BlockSize}_${TestNumber}.tmp
    sync
    timer_stop
    echo "Done. [${TIME_TAKEN} secs]"
}

# Prepare for testing
prepare_tests

# Ensure file systems exist
for fs in ${FILE_SYSTEMS} ; do
    /bin/df ${fs} | grep ${fs} > /dev/null 2>&1
    exit_on_error "File system ${fs} not found"
done

# Create work directories
for fs in ${FILE_SYSTEMS} ; do
    /bin/mkdir -p ${fs}/iotest
    /bin/rm -rf ${fs}/iotest/*
done

# Perform iozone tests
for fs in ${FILE_SYSTEMS} ; do
    for test_id in 0 1 ; do
        eval local test_name=\$TEST_NAME_${test_id}
        for block_size in ${BLOCK_SIZES} ; do
            for test_number in ${TEST_NUMBERS} ; do
                iozone_test ${block_size} ${test_id} ${test_number} ${fs}
            done
        done
    done
done

# Delete test files
for fs in ${FILE_SYSTEMS} ; do
    for block_size in ${BLOCK_SIZES} ; do
        for test_number in ${TEST_NUMBERS} ; do
            delete_test_file ${block_size} ${test_number} ${fs}
        done
    done
done

# Show results
for fs in ${FILE_SYSTEMS} ; do
    display_banner "Results ${fs}"
    for test_id in 0 1; do
        for block_size in ${BLOCK_SIZES} ; do
            compute_result ${block_size} ${test_id} ${fs}
        done
    done
done

# Save off intermediate results in a retrievable place
for fs in ${FILE_SYSTEMS} ; do
    /bin/mkdir -p /tmp/iotest/${fs}
    /bin/cp ${fs}/iotest/*.txt /tmp/iotest/${fs}
done

# Final cleanup
for fs in ${FILE_SYSTEMS} ; do
    /bin/rm -rf ${fs}/iotest
done

# Print test summary
print_test_summary
