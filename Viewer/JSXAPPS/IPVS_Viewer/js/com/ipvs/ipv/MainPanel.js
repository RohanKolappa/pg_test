/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* MainPanel class.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.MainPanel", com.ipvs.ipv.PanelCommon, null, function (MainPanel, MainPanel_prototype) {

  /** @private @jsxobf-clobber */
  MainPanel_prototype._LOG = jsx3.util.Logger.getLogger(MainPanel.jsxclass.getName());

  MainPanel.MENU_SELECT = "MENUSELECTION";
  MainPanel.MENU_LIVEPANEL = "LIVEPANEL";
  MainPanel.MENU_RECPANEL = "RECPANEL";

  //property for MENU_SELECTION
  MainPanel_prototype.SHOW_ON_MENU_SELECTION = com.ipvs.app.App.MENU_MAINPANEL;
  //properties for PanelCommon
  MainPanel_prototype.DISPLAYID = "MAIN";
  MainPanel_prototype.PANELID = "MAIN_PANEL1";

  MainPanel_prototype.SHOWCONTROLLER = false;
  /**
  * Extended initializer.
  */
  MainPanel_prototype._onAppInit = function () {
    // do common init
    this.jsxsuper();
    // init MainPanel collab info
    this.objCollabInfo.enablePVR = true;
    this.objCollabInfo.enableRecord = true;
    this.objCollabInfo.enableViewBookmarks = false;
    this.objCollabInfo.enableAddBookmarks = true;
    this.objCollabInfo.confirmOnStop = true;
    this.objCollabInfo.isSourceSelected = "NONE";
    this._MenuSelected = "NONE";
    this._panelMode = null;
    this.blkControllerContainer = this.getDescendantOfName("blkControllerContainer");
    this.blkMsConnectionMask = this.getDescendantOfName("blkMsConnectionMask");
    this.blkMsConnectionOK = this.getDescendantOfName("blkMsOk");
    this.blkMsConnectionMask.setEvent("this.onMaskClick();", jsx3.gui.Interactive.JSXCLICK);
    this.blkMsConnectionMask.onMaskClick = this.onMaskClick;
    //attach ConnError place holder to controller
    this.getDescendantsOfType("com.ipvs.ipv.Controller")[0].clsConnError = this.getDescendantsOfType("com.ipvs.ipv.ConnError")[0];
    this.getDescendantsOfType("com.ipvs.ipv.Controller")[0].clsZoomPanel = this.getDescendantsOfType("com.ipvs.ipv.ZoomPanel")[0];
    this.getDescendantsOfType("com.ipvs.ipv.Controller")[0].setZoomEvents();
    this.clsImagePanel = this.getDescendantsOfType("com.ipvs.ipv.ImagePanel")[0];
    this._LOG.info("MainPanel: Initiated");
  }

  /**
  * Returns the selected menu.
  * @return {String} menu selected.
  */
  MainPanel_prototype.getMenuSelection = function () {
    return this._MenuSelected;
  }

  /**
  * Sets the menu selection value.;
  * @param objEvent {jsx3.gui.Event}
  * @param menuSelected {String}
  * @param bPublish {boolean} or null
  * Publishes a model event using doEvent of jsx3.gui.Interactive
  */
  MainPanel_prototype.setMenuSelection = function (menuSelected, bPublish) {
    this._MenuSelected = menuSelected;
    if (bPublish != false) this.publish({ subject: MainPanel.MENU_SELECT, context: this });
  }

  //gets the selected live src for recording
  MainPanel_prototype.getSelectedSrc = function () {
    return this.getDescendantsOfType("com.ipvs.ipv.LivePanel")[0].getSelectedSrc();
  }

  //gets the selected live src for recording
  MainPanel_prototype.onSrcSelected = function (activePanel) {
    if (activePanel == com.ipvs.ipv.MainPanel.MENU_LIVEPANEL) {
      this.getDescendantsOfType("com.ipvs.ipv.RecPanel")[0].deSelectSrc();
    }
    else {
      this.getDescendantsOfType("com.ipvs.ipv.LivePanel")[0].deSelectSrc();
    }
    if (!this.SHOWCONTROLLER) {
      this.blkControllerContainer.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.SHOWCONTROLLER = true;
    }

  }

  /**
  * Show/Hide Image/Video panels;
  * @param panelMode {String} IMAGE/VIDEO
  */
  MainPanel_prototype.switchPanel = function (panelMode) {
    //do nothing if prev panel mode is same
    if (this._panelMode == panelMode) return;
    this._panelMode = panelMode;
    if (this._panelMode == "VIDEO") {
      this.clsImagePanel.resetSnapShot();
      this.clsImagePanel.getParent().setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.clsDisplayPanel.getParent().setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
    else {
      this.clsDisplayPanel.getParent().setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.clsImagePanel.getParent().setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
  }

  /* _onZoom 
  * - adjust image panel layout as per zoom
  * @param objZoom {Object};
  */
  MainPanel_prototype._onZoom = function (objZoom) {
    if (this._panelMode == "IMAGE") {
      this.clsImagePanel.doZoom(objZoom.context.ZoomSize);
    }
  }

  /** @private @jsxobf-clobber */
  /* Login State change
  */
  MainPanel_prototype._onLoginStateChange = function (objEvent) {
    this.jsxsuper(objEvent);
    if (objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch (stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        this.blkControllerContainer.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.switchPanel("VIDEO");
        this.SHOWCONTROLLER = false;
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        this.blkControllerContainer.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.SHOWCONTROLLER = false;
        this.setMenuSelection("NONE");
        break;
      default:
        return;
    }
  }

  MainPanel_prototype.showConnectionMask = function (connectionMask, errorMask) {
    if(connectionMask == true){
      if(errorMask == true){
        this.blkMsConnectionOK.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.blkMsConnectionMask.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
      else{
        this.blkMsConnectionOK.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
    }
    else{
      this.blkMsConnectionMask.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.blkMsConnectionMask.unsubscribe(jsx3.gui.Interactive.JSXCLICK, this);
    }
  }

  MainPanel_prototype.onMaskClick = function () {
    var ThisClass = this.getAncestorOfType("com.ipvs.ipv.MainPanel");

    if(ThisClass.blkMsConnectionOK.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK){
      ThisClass.showConnectionMask(false, false);
    }
  }

});
