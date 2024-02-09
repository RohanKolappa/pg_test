/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Applet global functions
*/
var handleInit, setCallback, handleMessage, unloadApplet = null;

/**
* Applet class handles applet loading.
* This class provides methods to send/recieve applet request/response.
*/
jsx3.Class.defineClass("com.ipvs.util.Applet", null, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(Applet, Applet_prototype) {

  /** @private @jsxobf-clobber */
  Applet_prototype._LOG = jsx3.util.Logger.getLogger(Applet.jsxclass.getName());

  /**
   * The instance initializer.
   */
  Applet_prototype.init = function() {
    this.appletHandle = null;
    this._AppletStatus = null;
    this._ServiceVersion = null;
    this._AppletInitXML = null;
    this._appletTimerID = null;

    // get a handle to the App - if already inited - finish init else subscribe to appinit
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
    this.clsApp.subscribe('APPLETLOADED', this, this._applet_Init);
    if (!handleInit) {
      handleInit = this.handleInit;
      setCallback = this.setCallback;
      handleMessage = this.handleMessage;
      unloadApplet = this.unloadApplet;
    }
  }
  /* 
   * onAppInit - set applet DIV to paint
  */
  Applet_prototype._onAppInit = function() {
    this.loadApplet();
    this._setAppletStatus("LOADING");
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~ Applet init function ~~~~~~~~~~~~~~~~~~~~~~~~  
   
  Applet_prototype._applet_Init = function(objEventVal) {
    if (!objEventVal) {
      this._LOG.error("applet isn't loaded successful.");
      return;
    }

    this.appletHandle = objEventVal.context.applet;
    this._AppletInitXML = objEventVal.context.initXML;

    var handleResponse = this.applet_handleResponse;
    setCallback(handleResponse);
    var appletServiceVersion = null;
    if( this._AppletInitXML != null ) {
      // get service version from applet Init response XML
      var appletInitDoc = new jsx3.xml.Document();
      appletInitDoc.loadXML(this._AppletInitXML);
      var nodeVersion = appletInitDoc.selectSingleNode("/Version");
      if( nodeVersion != null ) {
        appletServiceVersion = nodeVersion.getValue();
      }
    }
    if(appletServiceVersion != null){
      this._setServiceVersion(appletServiceVersion);
      this._setAppletStatus("LOADED");
      this._LOG.info("Applet::applet_Init:: ServiceVersion:" + appletServiceVersion + " \n" );
    }
    else {
      this._LOG.warn("Applet::applet_Init:: ServiceVersion:" + appletServiceVersion + " \n" );    
    }
  }


  //~~~~~~~~~~~~~~~~~~~~~~~~ Applet status / service version functions ~~~~~~~~~~~~~~~~~~~~~~

  Applet_prototype._setAppletStatus = function(statusVal) {
    this._AppletStatus = statusVal;
    this.publish({ subject: "AppletStatus",context:this});
  }
  
  Applet_prototype.getAppletStatus = function() {
    return this._AppletStatus;
  }
  
  Applet_prototype._setServiceVersion = function(serviceVersion) {
    this._ServiceVersion = serviceVersion;
  }

  Applet_prototype.getServiceVersion = function() {
    return this._ServiceVersion;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~ Applet send request / handle response functions ~~~~~~~~~~~~~~~~~~~~~

  Applet_prototype.applet_sendRequest = function(userJID, messageXML) {
    try {
      var ret = this.appletHandle.sendRequest(userJID, messageXML);
    }
    catch(e) {
      alert( "Internal Error: Calling applet sendRequest\n" +
              "Applet Handle: " + this.appletHandle + "\n" +
              "Description: " + e.description + "\n" );
      return null;
    }
    return ret;
  }

  // NOTE: this method runs in the context of Applet class
  Applet_prototype.applet_handleResponse = function(messageXML) {
    com.ipvs.app.App.clsApp.clsNotification.handleAppletXML(messageXML);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~ load applet DIV ~~~~~~~~~~~~~~~~~~~~~~~~

  /* 
  * loadApplet
  * dynamic create applet DIV and load
  */
  Applet_prototype.loadApplet = function() {

    //prepare applet DIV for loading
    var applet_JarCodeStr = null;
    if (this.clsApp.getClientType() == "WEB") {
      applet_JarCodeStr = "archive=\"smack-1.4.jar, smackx-1.4.jar, su-smack-1.2.jar, commons-logging.jar, plugin.jar, dom4j.jar, jaxen.jar, ipvsclient.jar, xpp3.jar\"" +
                              "code=\"com.ipvs.app.IPVSClientApplet\">";
    }
    else {
      applet_JarCodeStr = "archive=\"commons-beanutils.jar, commons-collections-3.2.jar, commons-lang-2.3.jar, commons-logging-1.1.jar, commons-logging-adapters-1.1.jar, commons-logging-api-1.1.jar, log4j-1.2.15.jar, mediaroom.jar, smack.jar, smackx.jar, su-smack-latest.jar, xpp3.jar\"" +
                              "code=\"com.ipvs.app.ServiceApplet\">";
    }
    var startURL = window.location.href;
    //'true':launch app. from local htm file,in this case have to put mediaroom jar files to the same dir of local htm file;
    var htmlFlag = (startURL.indexOf('file:///') >= 0) && (startURL.indexOf('.hta') < 0);
    var codebase = htmlFlag ? "" : "codebase=\"addins/mediaroom/\"";
    var appletHtml = "<div id=\"applet\" style=\"position: absolute; top: 0px; left: 0px; height: 0px; width: 0px;\"> " +
	                  "<applet id=\"ServiceApplet\"" +
                      "style=\"width: 0px; height: 0px; float: left;\"" +
                      "classloader_cache=\"false\"" + codebase + applet_JarCodeStr +
	                  "</applet></div>";
    //Using 'blkApplet' which is a sibling block of blkLogin and blkContainer 
    var blkBody = PNameSpace.getJSXByName("blkApplet"); 
    blkBody.setText(appletHtml, true);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~ unload applet DIV ~~~~~~~~~~~~~~~~~~~~~~~~

  /* 
  * unloadApplet: unload applet;
  */
  Applet_prototype.unloadApplet = function() {
    try {
      var blkBody = PNameSpace.getJSXByName("blkLogin"); 
      blkBody.setText("", true);
    }
    catch (e) {
      alert("Fail to unload applet error,this may leave mshta process in task manager.");
    }
  }
  //~~~~~~~~~~~~~~~~~~~~~~~~ local mapping functions  ~~~~~~~~~~~~~~~~~~~~~~~~
  Applet_prototype.handleInit = function(messageXML) {
    appletHandle = document.getElementById("ServiceApplet");
    appletHandle.setCallback("handleMessage");
    var objEvent = { initXML: messageXML, applet: appletHandle };
    com.ipvs.app.App.clsApp.publish({ subject: 'APPLETLOADED', context: objEvent });
  }

  Applet_prototype.setCallback = function(CallbackFunctionStr) {
    appletCallbackFunction = eval(CallbackFunctionStr);
  }

  Applet_prototype.handleMessage = function(messageXML) {
    if (appletCallbackFunction != null) {
      appletCallbackFunction(messageXML);
    }
  }
});


