var gTimerID = null;
var objAlert = null;
var timerCount = null;
var gPreviousClientX = 0;
var gPreviousClientY = 0;

function initInActivityTimeout() {
  jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, eval(handleKeyMouseAction)); 
  jsx3.gui.Event.subscribe(jsx3.gui.Event.MOUSEMOVE, eval(handleKeyMouseAction));
  timerCount = 0;
  gTimerID = setInterval("handleInActivityTimeout()", 10000);
}

function handleInActivityTimeout() {
  timerCount += 10000;
  var inActivityTimeoutSetting = topic_Get(tApp, "InActivityTimeout");
  if( timerCount >= inActivityTimeoutSetting ) {
    if( gTimerID != null ) {
      clearInterval(gTimerID);
      gTimerID = null;
    }
    var warningTimeout = topic_Get(tApp, "InActivityWarningTimeout");
    var alertMsg = "Application will be logged out due to inactivity.\nTo prevent the logout click Cancel within  " + warningTimeout/1000 + " seconds";
    objAlert = PNameSpace.alert("Warning", alertMsg, closeAlert, "Cancel");
    objAlert.setLeft(10);
    objAlert.repaint();
    gTimerID = setTimeout("closeAlert('LOGOUT')", warningTimeout);
    timerCount = 0;
  }  
}

function closeAlert(actionMode) {
  if( objAlert != null ) {
    objAlert.doClose();
    objAlert = null;
  }
  if( gTimerID != null ) {
    clearTimeout(gTimerID);
    gTimerID = null;
  }
  timerCount = 0;
  if( actionMode == "LOGOUT" ) {
    IpvsLogin.doLogout();
  }
  else {
    gTimerID = setInterval("handleInActivityTimeout()", 10000);
  }
}

function handleKeyMouseAction(objEvent) {
  //WORKAROUND somehow/somewhere mouse move event is generating even though X and Y are same.
  if( objEvent.event.clientX() == gPreviousClientX && objEvent.event.clientY() == gPreviousClientY ) return;
  timerCount = 0;
  gPreviousClientX = objEvent.event.clientX();
  gPreviousClientY = objEvent.event.clientY();
}

function destroyInActivityTimeout() {
  if( gTimerID != null ) {
    clearInterval(gTimerID);
    gTimerID = null;
  }
  jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(handleKeyMouseAction)); 
  jsx3.gui.Event.unsubscribe(jsx3.gui.Event.MOUSEMOVE, eval(handleKeyMouseAction));
  gPreviousClientX = 0;
  gPreviousClientY = 0;
}