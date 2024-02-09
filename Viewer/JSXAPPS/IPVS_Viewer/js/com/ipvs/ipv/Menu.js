/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Menu class provides UI for Menu.
* <p/>
* This class uses com.ipvs.agent.ServiceAgent.
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.ipv.Menu", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(Menu, Menu_prototype) {

  /** @private @jsxobf-clobber */
  Menu_prototype._LOG = jsx3.util.Logger.getLogger(Menu.jsxclass.getName());

  /**
  * The instance initializer.
  */
  Menu_prototype.onAfterAttach = function() {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if(!this.clsApp) return;
    if(this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }

  /** @private @jsxobf-clobber */
  Menu_prototype._onAppInit = function() {
    this.btnLive = this.getDescendantOfName("btnLive");
    this.btnRecordings = this.getDescendantOfName("btnRecordings");
    this.btnLive.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Live);
    this.btnRecordings.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Recordings);
    // subscribe to Login state
    this.clsApp.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
  }

  /** @private @jsxobf-clobber */
  Menu_prototype._onClick_Live = function() {
    this.btnRecordings.setState(jsx3.gui.ImageButton.STATE_OFF);
    this.btnLive.setState(jsx3.gui.ImageButton.STATE_ON);
    this.clsApp.getDescendantsOfType("com.ipvs.ipv.MainPanel")[0].setMenuSelection(com.ipvs.ipv.MainPanel.MENU_LIVEPANEL);
  }

  /** @private @jsxobf-clobber */
  Menu_prototype._onClick_Recordings = function() {
    this.btnLive.setState(jsx3.gui.ImageButton.STATE_OFF);
    this.btnRecordings.setState(jsx3.gui.ImageButton.STATE_ON);
    this.clsApp.getDescendantsOfType("com.ipvs.ipv.MainPanel")[0].setMenuSelection(com.ipvs.ipv.MainPanel.MENU_RECPANEL);
  }

  /** @private @jsxobf-clobber */
  /* Login State change
  */
  Menu_prototype._onLoginStateChange = function(objEvent) {
    if(objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        this._onClick_Live();
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        break;
      default:
        return;
    }
  }

});