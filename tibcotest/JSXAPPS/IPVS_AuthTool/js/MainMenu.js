/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* MainMenu class handles menu naviagation it will publish MenuSelected event.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.MainMenu", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(MainMenu, MainMenu_prototype) {
  
  /** @private @jsxobf-clobber */
  MainMenu_prototype._LOG = jsx3.util.Logger.getLogger(MainMenu.jsxclass.getName());
  
  /**
   * {String} Menu type published during navigation.
   * @final @jsxobf-final
   */
  MainMenu.MENU_MAINMENU = "MAINMENU";
  MainMenu.MENU_NEWPROJECT = "NEWPROJECT";
  MainMenu.MENU_EDITPROJECT = "EDITPROJECT";
  MainMenu.MENU_PROCESSPROJECT = "PROCESSPROJECT";
  MainMenu.MENU_VIEWPROJECTS = "VIEWPROJECTS";
  MainMenu.MENU_SETTINGS = "SETTINGS";

  /**
   * The instance initializer.
   */
  MainMenu_prototype.init = function() {
    //Get handle of child components and subscribe as required
    this.btnCreateNewProject = this.getDescendantOfName("btnNewProject");
    this.btnViewProjects = this.getDescendantOfName("btnViewProjects");
    this.btnSettings = this.getDescendantOfName("btnViewSettings");
    this.btnExit = this.getDescendantOfName("btnExit");
    this.btnCreateNewProject.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_CreateNewProject);
    this.btnViewProjects.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_ViewProjects);
    this.btnSettings.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Settings);
    this.btnExit.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Exit);

    //get the handle of MainApp class
    this.blkApp = this.getAncestorOfType("com.ipvs.authtool.App");
    this.blkApp.subscribe(com.ipvs.authtool.App.MENU_SELECT, this, this._toggle);
    this.blkApp.subscribe(com.ipvs.authtool.App.WINDOW_DESTROY, this, this._destroy);
    this._LOG.info("MainMenu: Initiated");
  }

  /** @private @jsxobf-clobber */
  MainMenu_prototype._toggle = function(objEvent) {
    if( this.blkApp.getMenuSelection() == MainMenu.MENU_MAINMENU ) {
      this.cmpShow();
      this._LOG.info("MainMenu: Show");
    }
    else {
      this.cmpHide();
      this._LOG.info("MainMenu: Hide");
    }
  }
  
  /**
   * Destroy all child components 
   */
  MainMenu_prototype.destroy = function() {
    this.btnCreateNewProject = null;
    this.btnViewProjects = null;
    this.btnSettings = null;
  }
  
  /** @private @jsxobf-clobber */
  MainMenu_prototype._onClick_CreateNewProject = function(objEvent) {
    this._LOG.info("MainMenu: Menu selected: " + MainMenu.MENU_NEWPROJECT);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_NEWPROJECT);
  }

  /** @private @jsxobf-clobber */
  MainMenu_prototype._onClick_ViewProjects = function(objEvent) {
    this._LOG.info("MainMenu: Menu selected: " + MainMenu.MENU_VIEWPROJECTS);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_VIEWPROJECTS);
  }

  /** @private @jsxobf-clobber */
  MainMenu_prototype._onClick_Settings = function(objEvent) {
    this._LOG.info("MainMenu: Menu selected: " + MainMenu.MENU_SETTINGS);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_SETTINGS);
  }

  /** @private @jsxobf-clobber */
  MainMenu_prototype._onClick_Exit = function(objEvent) {
    this._LOG.info("MainMenu: Exit");
    window.close();
  }
});