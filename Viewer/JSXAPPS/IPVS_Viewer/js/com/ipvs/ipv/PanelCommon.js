/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Base class that provides common functions for Viewer and Archive.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.PanelCommon", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(PanelCommon, PanelCommon_prototype) {

  /** @private @jsxobf-clobber */
  PanelCommon._LOG = jsx3.util.Logger.getLogger(PanelCommon.jsxclass.getName());

  /**
  * The common init code
  */
  PanelCommon_prototype.onAfterAttach = function() {
    // do up front to make sure they exist
    this.objCollabInfo = new Object();
    this.objPlayerInfo = new Object();

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

  PanelCommon_prototype._onAppInit = function() {
    // init the Layout
    this._init_Layout();

    //init DisplayPanel
    this._init_DisplayPanel();

    //init Controller
    this._init_Controller();

    // subscribe to Menu change
    this.clsApp.subscribe(com.ipvs.app.App.MENU_SELECT, this, this._onMenuSelect);

    // subscribe to Login state
    this.clsApp.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
    //this.clsApp.subscribe(com.ipvs.app.App.WINDOW_DESTROY, this, this._destroy);

  }

  //===================== Init methods ======================================

  /** @private @jsxobf-clobber */
  PanelCommon_prototype._init_Layout = function() {
    this.lytVSplit = this.getDescendantOfName("lytLeftRight");
    this.lytVSplit.setCols('450,*', true);
  }

  /** @private @jsxobf-clobber */
  PanelCommon_prototype._init_DisplayPanel = function() {
    // init the display and panel Ids
    this.displayId = this.DISPLAYID;
    this.panelId = this.PANELID;
    this.canvasId = "";
    this.windowId = "";

    // get handle to actual displaypanel
    this.clsDisplayPanel = this.getDescendantsOfType("com.ipvs.gui.DisplayPanel")[0];
    this.clsDisplayPanel.init();
    // save it to player info obj
    this.objPlayerInfo.clsDisplayPanel = this.clsDisplayPanel;
  }

  /** @private @jsxobf-clobber */
  PanelCommon_prototype._init_Controller = function() {
    // init the session and conn Ids
    this.sessionId = "";
    this.connId = "";
    // get a handle to the controller and init it
    this.controller = this.getDescendantsOfType("com.ipvs.ipv.Controller")[0];
    
    // subscribe to fullscreen
    this.controller.subscribe(com.ipvs.gui.Controller.MSG_FULLSCREEN, this, this._onFullScreen);
    this.controller.subscribe(com.ipvs.gui.Controller.MSG_ZOOM, this, this._onZoom);
  }

  //===================== Event Handlers ======================================

  /** @private @jsxobf-clobber */
  /* Login State change
  * - TBD - add comment
  */
  PanelCommon_prototype._onLoginStateChange = function(objEvent) {
    if(objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        // set a new sessionId
        this.sessionId = "Session_" + getUUID();

        // set the session info to the controller
        this.objCollabInfo.sessionId = this.sessionId;
        this.objCollabInfo.isSourceSelected = false;
        this.controller.setCollaborationInfo(this.objCollabInfo);

        // - get a new default display config with new canvas and window id
        this.canvasId = "Canvas_" + getUUID();
        this.windowId = "Window_" + getUUID();

        // set the display config to the displaypanel
        var nodeDisplayConfig = this.clsApp.clsDisplayAgent.getDefaultDisplayConfig(this.displayId, this.panelId, this.canvasId, this.windowId);
        // - set this in the display agent
        this.clsApp.clsDisplayAgent.setDisplayConfig(this.displayId, nodeDisplayConfig);
        // - finally set the display config id to the displaypanel
        this.clsDisplayPanel.setDisplayPanelId(this.displayId, this.panelId);

        // - set the windowId and display refernece to the controller
        this.objPlayerInfo.windowId = this.windowId;
        this.controller.setPlayerInfo(this.objPlayerInfo);

        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:

        this.clsDisplayPanel.removeDisplayPanelId(this.displayId, this.panelId);
        // delete displayconfig
        this.clsApp.clsDisplayAgent.deleteDisplayConfig(this.displayId);

        // reset window Ids and player info
        this.canvasId = "";
        this.windowId = "";
        this.objPlayerInfo.windowId = this.windowId;
        this.controller.setPlayerInfo(this.objPlayerInfo);

        // reset session Ids and collab info
        this.sessionId = "";
        this.objCollabInfo.sessionId = this.sessionId;
        this.objCollabInfo.isSourceSelected = false;
        this.controller.setCollaborationInfo(this.objCollabInfo);

        break;
      default:
        return;
    }
  }

  /** @private @jsxobf-clobber */
  /* On Menu selected 
  * * HACK - Setting the Layout on Show/Hide because the if Full screen is done when a block is hidden
  *   then the layout of the hidden block becomes stale and forced recaculation of box profile is required by the browser.
  */
  PanelCommon_prototype._onMenuSelect = function(objEvent) {
    if(this.clsApp.getMenuSelection() == this.SHOW_ON_MENU_SELECTION) {
      this._LOG.info("_onMenuSelect: Show: " + this.SHOW_ON_MENU_SELECTION);
      this.cmpShow();
    }
    else {
      this.cmpHide();
      this._LOG.info("_onMenuSelect: Hide:" + this.SHOW_ON_MENU_SELECTION);
    }
  }

  /* _onFullScreen 
  * - adjust blkViewer layout for fullscreen
  * @param centerLayout {Model}: center layout which including left list and right videoMessage + blkVieo;
  */
  PanelCommon_prototype._onFullScreen = function(objFullscreenEvent) {
    //adjust layout for fullscreen
    if(objFullscreenEvent.context.objFULLSCREEN.isFullScreen) {
      this.lytVSplit.setCols('0,*', true);
    }
    else {
      this.lytVSplit.setCols('450,*', true);
    }
  }
});