Organization of code
====================
SourceStreamAgent 
-----------------
- translates messages to SourceSM requests or StreamUtils requests
StreamUtils 
-----------
- handles all info and util requests(does not change the state of the sources/streams)
SourceSM
--------
- handles all TimeLine update requests and maintains the state of the sources and translates to StreamRequests
StreamTable
-----------
- maintains a table of the streams and handles requests to setup/teardown streams

DestStreamAgent
---------------
- translates messages to DestSM transitions
DestSM
------
- handles all Stream setup/teardown/transitions

Handling of TimelineUpdate(Pause/Resume)
========================================

stopOList(Now/Later) {
  // Stop All Outputs
  setOutputState(StopPending)
  stopSyncOList(Now/Later)[ live, local file ]
  stop remaining outputs
}
handleOutputStopped(output) {
  if(outputState != StopPending) // Stale
     return
  // Stop the Input
  if(input.countStarted() == 0) {
    if(relay)
      propogateStop
    else {
      setOutputState(Stopped)
      stopInput
    }
  }
}

boolean switchInputOnRelay() {
  return (relay && (newOutputId == oldOutputId))
}

startOList() {
  // Stop all outputs
  stop all outputs
  
  // Switch to new inputs
  for each output {
    if(switchInputOnRelay)
    	addTo relayInputList 
    else
    	switchInput
    	addTo localSyncInputList or addTo localInputList
  }
  
  // Start all outputs
  setOutputState(Started)
  start all outputs
  
  // Start all inputs
  start/propogate relayInputList
  start localSyncInputList(if Stopped)
  start localInputList(if Stopped)
}

Note: The relayInput gets started on creation is never stopped for its lifetime
    

