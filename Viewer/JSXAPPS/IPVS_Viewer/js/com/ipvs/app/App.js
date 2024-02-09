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
jsx3.Class.defineClass("com.ipvs.app.App", jsx3.gui.Block, [com.ipvs.app.AppI, jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(App, App_prototype) {

  /** @private @jsxobf-clobber */
  App_prototype._LOG = jsx3.util.Logger.getLogger(App.jsxclass.getName());

  // the following are App events.
  /** {String}
  * @final @jsxobf-final
  * App Init
  */
  App.APP_INIT = "APPINIT";

  /** {String}
  * @final @jsxobf-final
  * Menu Selection
  */
  App.MENU_SELECT = "MENUSELECTION";

  // Values
  App.MENU_MAINMENU = "MAINMENU";
  App.MENU_LIVESESSIONS = "LIVESESSIONS";
  App.MENU_BROWSESESSIONS = "BROWSESESSIONS";
  App.MENU_MEDIALIST = "MEDIALIST";
  App.MENU_NEWSESSION = "NEWSESSION";
  App.MENU_PREFERENCES = "PREFERENCES";
  App.MENU_MAINPANEL = "MAINPANEL"

  /** {String}
  * @final @jsxobf-final
  * OnSettingsChange
  */
  App.ON_SETTINGSCHANGE = "ONSETTINGSCHANGE";

  /** {String}
  * @final @jsxobf-final
  * OnError
  */
  App.ON_ERROR = "ONERROR";

  /** {String}
  * @final @jsxobf-final
  * Authtool Project Process Cancel
  */
  App.WINDOW_DESTROY = "WINDOWDESTROY";

  // Init State vars
  // - handle to self (instance)
  App.clsApp = null;
  // - for anyone that missed App init
  App.INITED = false;

  App_prototype.onAfterAttach = function() {
    App.clsApp = this;
    this.jsxsuper();
    this.jsxsupermix();
    // local init
    this._MenuSelected = "NONE";
    this._buildVersion = null;

    // attach to window close
    //this line cause loading problem in IE sometime, comment it by now
    /*window.attachEvent("onbeforeunload", this._onWindow_Destroy);*/
    // do app init event and save for future lookup
    this.publish({ subject: com.ipvs.app.App.APP_INIT, context: this });
    App.INITED = true;


    // subscribe to Login state
    this.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
    this.setMenuSelection("NONE");
  }

  /**
  * Returns the selected menu.
  * @return {String} menu selected.
  */
  App_prototype.getMenuSelection = function() {
    return this._MenuSelected;
  }

  /**
  * Sets the menu selection value.;
  * @param objEvent {jsx3.gui.Event}
  * @param menuSelected {String}
  * @param bPublish {boolean} or null
  * Publishes a model event using doEvent of jsx3.gui.Interactive
  */
  App_prototype.setMenuSelection = function(menuSelected, bPublish) {
    this._MenuSelected = menuSelected;
    if(bPublish != false) this.publish({ subject: App.MENU_SELECT, context: this });
  }

  //  /**
  //  *Notifies Settings change to App
  //  */
  //  App_prototype.pubSettingsChange = function() {
  //    this.publish({subject:App.ON_SETTINGSCHANGE, context:this});
  //  }

  /* 
  * Login State change
  * Manage show hide of app pages
  */
  App_prototype._onLoginStateChange = function(objEvent) {
    if(objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        this.setMenuSelection(App.MENU_MAINPANEL);
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        this.setMenuSelection("NONE");
        break;
      default:
        return;
    }
  }

  /**
  * Returns version from version.xml;
  */
  App_prototype.getVersion = function() {
    if(!jsx3.util.strEmpty(this._buildVersion)) return this._buildVersion;
    this._buildVersion = "0.0.r0.b0";
    var appPath = PNameSpace.getEnv("APPPATH");
    var nodeVersionData = PNameSpace.loadInclude(appPath.substring(appPath.indexOf('JSXAPPS/')) + "/xml/version.xml", "Version_xml", "xml", true);
    var nodeVersion = nodeVersionData.selectSingleNode(".//version");
    if(nodeVersion != null) {
      this._buildVersion = nodeVersion.getValue();
    }
    return this._buildVersion;
  }

  App_prototype.setError = function (objErrorInfo, errorName) {
    this.jsxsupermix(objErrorInfo, errorName);
    this.publish({ subject: App.ON_ERROR, context: this });
  }
});