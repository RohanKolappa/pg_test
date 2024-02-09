/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Mixin interface that provides methods to handle App components.
*/

jsx3.Class.defineInterface("com.ipvs.app.AppI", null, function(AppI, AppI_prototype) {

  /** @private @jsxobf-clobber */
  AppI_prototype._LOG = jsx3.util.Logger.getLogger(AppI.jsxclass.getName());

  /*
  * onAfterAttach
  * - hook into GI DOM building to do init
  * - code before 'jsxsuper' - pre-init - executes during top-down traverse
  * - code after 'jsxsuper' - post-init - executes on the way back
  */
  AppI_prototype.onAfterAttach = function() {
    // -post-init - do the real init and publish APP_INIT
    // init client env
    this._giStartPath = "";
    this._clientType = "";
    this._browserType = "";
    this._objErrorInfo = null;
    this._initClientEnvironment();
    subscribeBrowserClose();
    // init preferences    
    if(jsx3.util.strEmpty(this.clsPreferences)) {
      this.clsPreferences = new com.ipvs.util.Preferences();
    }
    // init Applet and notification handling
    if(jsx3.util.strEmpty(this.clsApplet)) {
      this.clsApplet = new com.ipvs.util.Applet();
    }
    if(jsx3.util.strEmpty(this.clsNotification)) {
      this.clsNotification = new com.ipvs.api.Notification();
    }
    // init Agents
    if(jsx3.util.strEmpty(this.clsSessionAgent)) {
      this.clsSessionAgent = new com.ipvs.agent.SessionAgent();
    }
    if(jsx3.util.strEmpty(this.clsDisplayAgent)) {
      this.clsDisplayAgent = new com.ipvs.agent.DisplayAgent();
    }
    if(jsx3.util.strEmpty(this.clsSSAgent)) {
      this.clsSSAgent = new com.ipvs.agent.SSAgent();
    }
    // service agent and permissions
    if(jsx3.util.strEmpty(this.clsServiceAgent)) {
      this.clsServiceAgent = new com.ipvs.agent.ServiceAgent();
    }
    if(jsx3.util.strEmpty(this.clsUserPermission)) {
      this.clsUserPermission = new com.ipvs.util.UserPermission();
    }
    // init inactivity and log
    if(jsx3.util.strEmpty(this.clsHandleInactivity)) {
      this.clsHandleInactivity = new com.ipvs.util.HandleInActivity();
    }
    this.jsxBody = this.getAncestorOfName("JSXBODY");
    if(jsx3.util.strEmpty(this.clsHandleMemory)) {
      this.clsHandleMemory = new com.ipvs.util.HandleMemory();
    }
  }

  /*
  * onAfterPaint
  * - hide all toolbars of breowser
  */
  AppI_prototype.onAfterPaint = function(objGUI) {
    //Hide the toolbars in IE
    if(jsx3.CLASS_LOADER.IE) {
      //get handle to IPVSUtil activeX block
      var appHTML = objGUI;
      if(appHTML) {
        var objIPVSUtil = appHTML.ownerDocument.getElementById("IPVSUtilIE");
        if(objIPVSUtil) {
          try {
            objIPVSUtil.HideToolbars();
          }
          catch(e) {
            this._LOG.warn("APP::IPVSUtilIE is not registered");
          }
        }
      }
    }
    if(jsx3.CLASS_LOADER.FX) {
      //for Firefox
    }
    if(jsx3.CLASS_LOADER.SAF) {
      //for safari
    }
  }

  /**
  * Returns Application start path;
  * @return {String} giStartPath.
  */
  AppI_prototype.getGIStartPath = function() {
    return this._giStartPath;
  }

  /**
  * Returns Application type;
  * @return {String} INSTALLED/WEB.
  */
  AppI_prototype.getClientType = function() {
    return this._clientType;
  }

  /**
  * Returns BrowserType;
  * @return {String} MSIE/FIREFOX.
  */
  AppI_prototype.getBrowserType = function() {
    return this._browserType;
  }

  /** @private @jsxobf-clobber */
  AppI_prototype._initClientEnvironment = function() {
    //check browser type and set the type to tApp
    var usrAgt = navigator.userAgent.toLowerCase();
    //currently only support MSIE and FIREFOX treat others as UNKOWN
    this._browserType = usrAgt.indexOf("msie") >= 0 ? "MSIE" : usrAgt.indexOf('firefox') >= 0 ? "FIREFOX" : "UNKNOWN";
    //assign it in local make Logoff-re-Login correct
    var clientLocation = window.location.href; 
    clientLocation = clientLocation.substring(0, clientLocation.lastIndexOf("/"));
    //localize the following parameters for debug
    var objNode = PNameSpace.loadInclude("debug/debugProperty.xml", null, "xml");
    var debugNode = objNode ? objNode.selectSingleNode("/Debug") : null;
    var DEBUGMODE_GISTARTPATH = debugNode ? debugNode.selectSingleNode(".//giStartPath").getValue() : "";
    var DEBUGMODE_CLIENTTYPE = debugNode ? debugNode.selectSingleNode(".//clientType").getValue() : "";
    var DEBUGMODE_LOGINSERVER = debugNode ? debugNode.selectSingleNode(".//managementServerIP").getValue() : "";

    var giStartPath = "";
    //get clientType and valid giStartPath 
    if(clientLocation.indexOf("file:") != -1) {
      this._clientType = "INSTALLED";
      //strip off file:///
      giStartPath = clientLocation.substring(8);
      if (giStartPath.match("/shell.html")) {
        //tibco gitak using path like jsxapppath=file:/// in testSetup.html for automation
        giStartPath = giStartPath.substring(0, giStartPath.indexOf("//shell.html"));
      }
      else if (giStartPath.match("/GI_Builder")) {
        //launch in recorder using path like:file:///C:/giOdysseyClient/GI_Builder/pugins/jsx3.ide.recorder/recorder.html
        giStartPath = giStartPath.substring(0, giStartPath.indexOf("/GI_Builder"));;
      }
    }
    else if(clientLocation.indexOf("http://localhost") != -1) {
      this._clientType = DEBUGMODE_CLIENTTYPE;
      if(this._clientType == "WEB") {
        giStartPath = clientLocation.substring(0);
      }
      else {
        giStartPath = DEBUGMODE_GISTARTPATH;
      }
    }
    else {
      this._clientType = "WEB";
      giStartPath = clientLocation.substring(0);
    }

    this._LOG.info("AppI::InitClientEnvironment:giStartPath=" + giStartPath);

    //set WEB client login server to topic
    if(this._clientType == "WEB") {
      //set login server for 'WEB' client
      var webClientLoginServer = window.location.href.substring(7);
      webClientLoginServer = webClientLoginServer.substring(0, webClientLoginServer.indexOf('/'));
      //for 'WEB' client debug
      if(webClientLoginServer.indexOf('localhost') != -1) {
        webClientLoginServer = DEBUGMODE_LOGINSERVER;
      }
    }
    this._giStartPath = giStartPath.replace(/%20/g, " ");
  }

  AppI_prototype.setError = function (objErrorInfo, errorName) {
    this._objErrorInfo = objErrorInfo;
    if (!jsx3.util.strEmpty(objErrorInfo)) this._objErrorInfo.errorName = errorName;
  }

  AppI_prototype.getError = function () {
    return this._objErrorInfo;
  }
});