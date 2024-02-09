/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Controller class.
* Base class Deals with common contols - most are direct descendants. some are external blocks.
* - Rew,Play,Pause,Stop,FF
* - 1:1,FS,SnapShot
* - Bookmarks
* - Status (external block)
* Extend to match the final block
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.Controller", jsx3.gui.Block, [jsx3.gui.Interactive, com.ipvs.gui.SessionControllerI, com.ipvs.gui.WindowControllerI], function(Controller, Controller_prototype) {

  /** @private @jsxobf-clobber */
  Controller_prototype._LOG = jsx3.util.Logger.getLogger(Controller.jsxclass.getName());

  /** notification  fullscreen */
  Controller.MSG_FULLSCREEN = "ONFULLSCREEN";
  Controller.MSG_ZOOM = "ONZOOM";

  /* init 
  * - get handles to the controls  and subscribe to events
  */
  Controller_prototype.onAfterAttach = function() {
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
  Controller_prototype._onAppInit = function() {
    // init all session controls
    this.initSessionControls();

    // init all window controls
    this.initWindowControls();

    // subscribe to Login state
    this.clsApp.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
  }

  /** @private @jsxobf-clobber */
  /* Login State change
  * - TBD - add comment
  */
  Controller_prototype._onLoginStateChange = function(objEvent) {
    if(objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        // reset window and session controls
        this.resetWindowControls();
        this.resetSessionControls();

        // reset player and collab info
        this.resetPlayerInfo();
        this.resetCollaborationInfo();
        this._recordPermission = this.clsApp.clsUserPermission.getPermission("Dir", "record");
        // update controller with reset state
        this.applyCollaborationInfo();
        this.applyPlayerInfo();

        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        // reset player and collab info
        this.resetPlayerInfo();
        this.resetCollaborationInfo();
        this._recordPermission = "NONE";
        // update controller with reset state
        this.applyCollaborationInfo();
        this.applyPlayerInfo();

        // reset window and session controls
        this.resetSessionControls();
        this.resetWindowControls();
        
        break;
      default:
        return;
    }
  }

  /**
  * getPlayerHtmlSrc to get html source of player 
  */
  Controller_prototype.getPlayerHtmlSrc = function() {
    //get html source for view
    var cmpSrcBlkJSX = PNameSpace.getJSXByName('lytHeader'); 
    var cmpSrc = cmpSrcBlkJSX.getRendered();
    var renderedHtml = cmpSrc ? cmpSrc.innerHTML : "EMPTY";
    this._LOG.error("::HTML Source:://n" + renderedHtml);
  }

});
