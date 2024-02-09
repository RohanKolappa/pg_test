/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* LiveRecCommon class.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.LiveRecCommon", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(LiveRecCommon, LiveRecCommon_prototype) {

  /** @private @jsxobf-clobber */
  LiveRecCommon_prototype._LOG = jsx3.util.Logger.getLogger(LiveRecCommon.jsxclass.getName());
  //property for MENU_SELECTION

  /**
  * The common init code
  */
  LiveRecCommon_prototype.onAfterAttach = function() {
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
 
  /**
  * Extended initializer.
  */
  LiveRecCommon_prototype._onAppInit = function() {
    // subscribe to Login state
    this.clsApp.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
    this.clsParent = this.clsApp.getDescendantsOfType("com.ipvs.ipv.MainPanel")[0];
    // subscribe to Menu change
    this.clsParent.subscribe(com.ipvs.ipv.MainPanel.MENU_SELECT, this, this._onMenuSelect);
    this._LOG.info("LiveRecCommon: Initiated");
  }

  /** @private @jsxobf-clobber */
  /* On Menu selected 
  * * HACK - Setting the Layout on Show/Hide because the if Full screen is done when a block is hidden
  *   then the layout of the hidden block becomes stale and forced recaculation of box profile is required by the browser.
  */
  LiveRecCommon_prototype._onMenuSelect = function(objEvent) {
    if(this.clsParent.getMenuSelection() == this.SHOW_ON_MENU_SELECTION) {
      this._LOG.info("_onMenuSelect: Show: " + this.SHOW_ON_MENU_SELECTION);
      this.cmpShow();
    }
    else {
      this.cmpHide();
      this._LOG.info("_onMenuSelect: Hide:" + this.SHOW_ON_MENU_SELECTION);
    }
  }
});
