#!/bin/sh

nicecommand="nice -n 20 ionice -c2 -n7 "

# Try lightweight make
${nicecommand} make Update
${nicecommand} make clean
${nicecommand} make


# If build fails, try a really clean make 
if [ x$? != "x0" ] ; then

    echo "Build failed!!"
    echo
    echo "Please Wait... Will retry a cleaner build in 5 seconds..."
    echo

    dobuild=0

    read -n 1 -t 5 -p "Press Q to abort or any other key to continue..." -t  5 key
    echo

    if [ x$? != "x0" ] ; then
        echo "Timed out... Continuing with clean build."
        dobuild=1
    else 
        if [ x$key = "xq" -o x$key = "xQ" ] ; then
            echo "Aborting clean build."
            dobuild=0
        else
            echo "Continuing with clean build..."
            dobuild=1
        fi
    fi

    if [ $dobuild = 1 ] ; then
        ${nicecommand} make Checkout
        ${nicecommand} make buildclean
        ${nicecommand} make
    fi
fi
