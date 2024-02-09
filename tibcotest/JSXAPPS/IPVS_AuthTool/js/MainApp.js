/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* App class handles global app events.
* This class provides get/set methods to handle global variables.
* <p/>
* This class uses jsx3.gui.Interactive, jsx3.util.EventDispatcher.
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.authtool.App", jsx3.gui.Block, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(App, App_prototype) {

  /** @private @jsxobf-clobber */
  App_prototype._LOG = jsx3.util.Logger.getLogger(App.jsxclass.getName());

  // the following are Authtool events.
  /** {String}
   * @final @jsxobf-final
   * Authtool Menu Selection
   */
  App.MENU_SELECT = "MENUSELECTION";
  
  /** {String}
   * @final @jsxobf-final
   * Authtool Project Selection
   */
  App.PROJECT_SELECT = "PROJECTSELECTED";

  /** {String}
   * @final @jsxobf-final
   * Authtool Project Add/Update
   */
  App.PROJECT_ADDEDIT = "PROJECTADDEDIT";
  
  /** {String}
   * @final @jsxobf-final
   * Authtool Project Process Cancel
   */
  App.PROJECT_PROCESSCANCEL = "PROJECTPROCESSCANCEL";
  
   /** {String}
   * @final @jsxobf-final
   * Authtool Project Process Cancel
   */
  App.WINDOW_DESTROY = "WINDOWDESTROY";

  /**
   * The instance initializer.
   */
  App_prototype.init = function() {
    this.jsxsuper();
    this.MenuSelected = null;
    this.ProjectSelected = null;
    ThisClass = this;
    window.attachEvent("onbeforeunload",this._onWindow_Destroy);
  }
  
  /**
   * Returns the selected Project name.
   * @return {String} name of project selected.
   */
  App_prototype.getProjectSelected = function() {
    return this.ProjectSelected;
  }

  /**
   * Sets the project selection value.;
   * @param objEvent {jsx3.gui.Event}
   * @param projectSelected {String}
   * @param bPublish {boolean} or null
   * Publishes a model event using doEvent of jsx3.gui.Interactive
   */
  App_prototype.setProjectSelected = function(objEvent, projectSelected, bPublish) {
    this.ProjectSelected = projectSelected;
    if (bPublish != false) this.publish({ subject: App.PROJECT_SELECT, context: { objEVENT: objEvent} });
  }

  /**
   * Sets the project Added/Updated selection value.;
   * @param objEvent {jsx3.gui.Event}
   * @param project Added/Edited {String}
   * @param bPublish {boolean} or null
   * Publishes a model event using doEvent of jsx3.gui.Interactive
   */
  App_prototype.setAddEditProject = function(objEvent, projectName, bPublish) {
    this.ProjectSelected = projectName;
    if (bPublish != false) this.publish({ subject: App.PROJECT_ADDEDIT, context: { objEVENT: objEvent} });
  }
  
  /**
   * Sets the project Added/Updated selection value.;
   * @param objEvent {jsx3.gui.Event}
   * @param project Added/Edited {String}
   * @param bPublish {boolean} or null
   * Publishes a model event using doEvent of jsx3.gui.Interactive
   */
  App_prototype.setCancelProcessProject = function(objEvent, projectName, bPublish) {
    this.ProjectSelected = projectName;
    if (bPublish != false) this.publish({ subject: App.PROJECT_PROCESSCANCEL, context: { objEVENT: objEvent} });
  }

  /**
   * Returns the selected menu.
   * @return {String} menu selected.
   */
  App_prototype.getMenuSelection = function() {
    return this.MenuSelected;
  }

  /**
   * Sets the menu selection value.;
   * @param objEvent {jsx3.gui.Event}
   * @param menuSelected {String}
   * @param bPublish {boolean} or null
   * Publishes a model event using doEvent of jsx3.gui.Interactive
   */
  App_prototype.setMenuSelection = function(objEvent, menuSelected, bPublish) {
    this.MenuSelected = menuSelected;
    if (bPublish != false) {
      this.publish({subject:App.MENU_SELECT, context:{ objEVENT: objEvent }});
    }
  }

  App_prototype._onWindow_Destroy = function(objEvent) {
    ThisClass.doEvent(App.WINDOW_DESTROY, {objEVENT:objEvent});
  }
});