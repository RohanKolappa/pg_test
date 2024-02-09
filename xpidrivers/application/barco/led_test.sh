#!/bin/sh
setvar() {
        eval $1=\$2;
}

green_on() {
    echo "Turning all green LEDs ON.."
    for lednum in 0 1 2 4 7 9
    do
        echo "Turning on LED $lednum"
        /V2O/bin/maint testled -l $lednum -n
    done
}

green_off() {
    echo "Turning all green LEDs OFF.."
    for lednum in 0 1 2 4 7 9
    do
        echo "Turning on LED $lednum"
        /V2O/bin/maint testled -l $lednum -f
    done
}

red_on() {
    echo "Turning all red LEDs ON.."
    for lednum in 3 5
    do
        echo "Turning on LED $lednum"
        /V2O/bin/maint testled -l $lednum -n
    done
}

red_off() {
    for lednum in 3 5
    do
        echo "Turning off LED $lednum"
        /V2O/bin/maint testled -l $lednum -f
    done
}

orange_on() {
    echo "Turning all orange LEDs ON.."
    for lednum in 2 3 4 5 6 11
    do
        echo "Turning on LED $lednum"
        /V2O/bin/maint testled -l $lednum -n
    done
}

orange_off() {
    for lednum in 2 3 4 5 6 11
    do
        echo "Turning off LED $lednum"
        /V2O/bin/maint testled -l $lednum -f
    done
}

yellow_on() {
    echo "Turning all yellow LEDs ON.."
    for lednum in 8 10
    do
        echo "Turning on LED $lednum"
        /V2O/bin/maint testled -l $lednum -n
    done
}

yellow_off() {
    for lednum in 8 10
    do
        echo "Turning off LED $lednum"
        /V2O/bin/maint testled -l $lednum -f
    done
}

echo "Starting LED test..."
maint testled -s

echo "Saving current LED status..."
for i in `seq 0 11`
do
    status=`/V2O/bin/maint testled -l $i -p`
    setvar ledstatus$i $status
    eval status="\$ledstatus$i"
done

green_on
sleep 5
green_off
sleep 1
red_on
sleep 5
red_off
sleep 1
orange_on
sleep 5
orange_off
sleep 1
yellow_on
sleep 5
yellow_off

echo "Restoring LED status..."
for i in `seq 0 11`
do
    eval status="\$ledstatus$i"
    echo "LED $i status = $status"
    if [ $status == "OFF" ]; then
        /V2O/bin/maint testled -l $i -f
    elif [ $status == "ON" ]; then
        /V2O/bin/maint testled -l $i -n
    elif [ $status == "BLINK" ]; then
        /V2O/bin/maint testled -l $i -b
    fi
done

echo "Stopping LED test..."
maint testled -S

exit 0
