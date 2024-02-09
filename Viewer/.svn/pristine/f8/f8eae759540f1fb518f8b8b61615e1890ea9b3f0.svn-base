/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Header class provides UI for Header.
* <p/>
* This class uses com.ipvs.agent.ServiceAgent.
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.ipv.Header", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(Header, Header_prototype) {

  /** @private @jsxobf-clobber */
  Header_prototype._LOG = jsx3.util.Logger.getLogger(Header.jsxclass.getName());

  /**
  * The instance initializer.
  */
  Header_prototype.onAfterAttach = function() {
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

  Header_prototype._onAppInit = function() {
    this.lblWelcome = this.getDescendantOfName("txtWelcome");
    this.btnLogout = this.getDescendantOfName("btnLogout");
    this.lytHeader = this.clsApp.getDescendantOfName("lytHeaderBody");
    // logout - click
    this.btnLogout.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Logout);
    this.clsApp.subscribe(com.ipvs.app.App.MENU_SELECT, this, this._toggle);

    this.Controller = this.clsApp.getDescendantsOfType("com.ipvs.ipv.Controller")[0];
    this.Controller.subscribe(com.ipvs.gui.Controller.MSG_FULLSCREEN, this, this.onFullscreen);
  }

  /** @private @jsxobf-clobber */
  /* inner function to adjust blkHeaderMenu.xml layout for fullscreen */
  Header_prototype.onFullscreen = function(objFullscreenEvent) {
    if(objFullscreenEvent.context.objFULLSCREEN.isFullScreen) {
      this.lytHeader.setRows('0,*', true);
    }
    else {
      this.lytHeader.setRows('51,*', true);
    }
  }

  /** @private @jsxobf-clobber */
  Header_prototype._toggle = function(objEvent) {
    if(this.clsApp.getMenuSelection() == "NONE") {
      this.lblWelcome.setText("", true);
      this.cmpHide();
      this._LOG.info("Header: Hide");
    }
    else {
      this.cmpShow();
      var userName = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_USER);
      var clientName = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_RESOURCE);
      var welcomeText = userName + "." + clientName; 
      this.lblWelcome.setText(welcomeText, true);
      this._LOG.info("Header: Show");
    }
  }

  /** @private @jsxobf-clobber */
  Header_prototype._onClick_Logout = function(objEvent) {
    this.clsApp.clsServiceAgent.doLogout();
  }  
});