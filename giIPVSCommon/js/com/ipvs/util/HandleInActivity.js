/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/
jsx3.Class.defineClass("com.ipvs.util.HandleInActivity", null, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(HandleInActivity, HandleInActivity_prototype) {

  /** @private @jsxobf-clobber */
  HandleInActivity_prototype._LOG = jsx3.util.Logger.getLogger("com.ipvs.util.HandleInActivity");

  /** * The instance initializer.  */
  HandleInActivity_prototype.init = function() {
    this._gTimerID = null;
    this._objAlert = null;
    this._timerCount = 0;
    this._gPreviousClientX = 0;
    this._gPreviousClientY = 0;
    this._inActivityTimeout = null;
    this._warningTimeout = null;
    
    // get a handle to App and wait for app init
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    } 
  }
  HandleInActivity_prototype._onAppInit = function() {
    this.clsApp.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
  }
  
  
  /** @private @jsxobf-clobber */
  //Subscribe to LoginState to start/Stop InActivity timer
  HandleInActivity_prototype._onLoginStateChange = function(objEvent) {
    if(objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        this.startInActivityTimer();
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        this.destroyInActivityTimer();
        break;
      default:
        return;
    }
  }

  /**
    *Starts the Inactivity timer if the InActivityTimeout is configured in AssetPrivateData
    */
  HandleInActivity_prototype.startInActivityTimer = function() {
    this._inActivityTimeout = this.clsApp.clsPreferences.getLocalPreferences("InActivityTimeout", null);
    if(!jsx3.util.strEmpty(this._inActivityTimeout)) {
      this._inActivityTimeout = this._inActivityTimeout * 1000; //milliseconds
      this._warningTimeout = this.clsApp.clsPreferences.getLocalPreferences("InActivityWarningTimeout", 30);
      this._warningTimeout = this._warningTimeout * 1000; //milliseconds
      this._timerCount = 0;
      var ThisClass = this;
      jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, this, function(objEvent) { HandleInActivity_prototype._handleKeyMouseAction(ThisClass, objEvent) });
      jsx3.gui.Event.subscribe(jsx3.gui.Event.MOUSEMOVE, this, function(objEvent) { HandleInActivity_prototype._handleKeyMouseAction(ThisClass, objEvent) });
      this._gTimerID = setInterval(function() { HandleInActivity_prototype._handleInActivityTimeout(ThisClass); }, 10000);
      this._LOG.info("HandleInActivity: Inactivity timer started");
    }
  }

  /** @private @jsxobf-clobber */
  //If there is no activity more than configured time then It will popup warning.
  HandleInActivity_prototype._handleInActivityTimeout = function(ThisClass) {
    ThisClass._timerCount += 10000;
    if(ThisClass._timerCount >= ThisClass._inActivityTimeout) {
      if(ThisClass._gTimerID != null) {
        clearInterval(ThisClass._gTimerID);
        ThisClass._gTimerID = null;
        ThisClass._LOG.info("HandleInActivity: Inactivity Timer cleared _gTimerID: " + ThisClass._gTimerID);
      }
      var alertMsg = "Application will be logged out due to inactivity.\nTo prevent the logout click Cancel within  " + ThisClass._warningTimeout / 1000 + " seconds";
      ThisClass._objAlert = PNameSpace.alert("Warning", alertMsg, function() { HandleInActivity_prototype._closeAlert(ThisClass); }, "Cancel");
      ThisClass._objAlert.setLeft(10);
      ThisClass._objAlert.repaint();
      ThisClass._gTimerID = setTimeout(function() { HandleInActivity_prototype._closeAlert(ThisClass, 'LOGOUT'); }, ThisClass._warningTimeout);
      ThisClass._timerCount = 0;
      ThisClass._LOG.info("HandleInActivity: Inactivity Timeout Warning: " + ThisClass._gTimerID);
    }
  }

  /** @private @jsxobf-clobber */
  //If user cancel the warning then Inactivity time will be restart
  //Otherwise User will be logged out after configured InActivityWarningTimeout period.
  HandleInActivity_prototype._closeAlert = function(ThisClass, actionMode) {
    if(ThisClass._objAlert != null) {
      ThisClass._objAlert.doClose();
      ThisClass._objAlert = null;
    }
    if(ThisClass._gTimerID != null) {
      clearTimeout(ThisClass._gTimerID);
      ThisClass._gTimerID = null;
    }
    ThisClass._timerCount = 0;
    if(actionMode == "LOGOUT") {
      ThisClass._LOG.info("HandleInActivity: Logout due to Inactivity");
      ThisClass.clsApp.clsServiceAgent.doLogout();
    }
    else {
      ThisClass._gTimerID = setInterval(function() { HandleInActivity_prototype._handleInActivityTimeout(ThisClass); }, 10000);
      ThisClass._LOG.info("HandleInActivity: Inactivity Timer restarted after warning closed: " + ThisClass._gTimerID);
    }
  }

  /** @private @jsxobf-clobber */
  //For every Move/Click action it will reset timercount to 0
  HandleInActivity_prototype._handleKeyMouseAction = function(context, objEvent) {
    if(objEvent.event.clientX() == context._gPreviousClientX && objEvent.event.clientY() == context._gPreviousClientY) return;
    context._timerCount = 0;
    context._gPreviousClientX = objEvent.event.clientX();
    context._gPreviousClientY = objEvent.event.clientY();
  }

  /**
  *Stops the Inactivity timer.
  */
  HandleInActivity_prototype.destroyInActivityTimer = function() {
    if(this._gTimerID != null) {
      clearInterval(this._gTimerID);
      this._gTimerID = null;
    }
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, this);
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.MOUSEMOVE, this);
    this._gPreviousClientX = 0;
    this._gPreviousClientY = 0;
    this._LOG.info("HandleInActivity: Inactivity Timer Stopped");
  }
});