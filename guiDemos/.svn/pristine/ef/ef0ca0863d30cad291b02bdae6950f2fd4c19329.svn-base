#!/bin/sh


# This script enables/disables the demo applications on the device
# To enable or disable, use the "setup" or "teardown" argument
# If enabled, it sets up the directories needed for the demo applications to run
# Use the "check" command to determine if demos are enabled

# This script is isntalled in /etc/scripts and is used by the 
# device menu/cli to enable/disable demos

DEMODIR="/data/demos"

setup() {
    # Create the demo directory
    /bin/mkdir -p ${DEMODIR}
    /bin/mkdir -p ${DEMODIR}/sap
    /bin/mkdir -p ${DEMODIR}/zc

    # Recursivly change ownership of demo directory to admin user
    /bin/chown -R admin ${DEMODIR}
}

teardown() {
    /bin/rm -rf ${DEMODIR}
}

check() {
    if [ ! -d ${DEMODIR} ] ; then
        return 1
    fi
    return 0
}

RC=0

case "$1" in
  setup)
        setup
        ;;

  teardown)
        teardown
        ;;

  check)
        check
        RC=$?
        ;;

  *)
        echo "Usage: $0 [setup|teardown|check]"
        exit 1

esac

exit $RC
