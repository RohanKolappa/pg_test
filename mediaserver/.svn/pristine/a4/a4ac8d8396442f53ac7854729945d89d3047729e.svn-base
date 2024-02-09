There are 2 class files to be used for ctl implementation

1. cmdSingle cmd ctl : 1 command to start 1 process 
   - use cmdctlbase.sh
   - see clusterctl.sh for sample code
   - need to set SERVER_NAME, SERVER_CTL_FILE_PREFIX, SERVER_STATUS_FILE_PREFIX, THIS_SCRIPT_PATH
   - need to implement start_command() and update PID
   - cmdctlbase extends ctlbase

2. Other : Multiple commands to start multiple processes
   - use ctlbase.sh
   - need to set SERVER_NAME, SERVER_CTL_FILE_PREFIX, SERVER_STATUS_FILE_PREFIX, THIS_SCRIPT_PATH
   - need to implement start_implementation, stop_implementation, status_implementation, reset_implementation
