/*
  our app is in a frame called "main"
  the applet is dynamically loaded in a DIV "applet" and has it's own JS "init" and "handleMessage" functions
  - upon init the applet calls it's JS init routine which sets the global "appletHandle"
  - once we see that is valid we call the applet's JS "setCallback" to setup the function we want messages forwarded to
  - at run time the applet calls it's own JS "handleMessage" function which then calls the function we setCallback to

var appletDIV = null;
var appletTimerID = null;
var appletInitXML = null;
var appletServiceVersion = null;

function applet_Init() {
  if( AnimationMode == true || SimulationMode == true ) {
    return true;
  }
  if (appletDIV == null) {
    appletDIV = eval("document.getElementById('applet')");
  }
  if (appletDIV == null || window.appletHandle == null) {
    return false;
  }
  else {
    appletDIV = window;
    appletDIV.appletHandle = window.appletHandle;
    appletInitXML = setCallback("applet_handleResponse");
  }

  if (appletInitXML != null) {
    // get service version from applet Init response XML
    var appletInitDoc = new jsx3.xml.Document();
    appletInitDoc.loadXML(appletInitXML);
    var nodeVersion = appletInitDoc.selectSingleNode("/Version");
    if (nodeVersion != null) {
      appletServiceVersion = nodeVersion.getValue();
    }
  }

  if (appletServiceVersion != null) {
    topic_Set(tApp, "ServiceVersion", appletServiceVersion);
    log.info("commonAppletFnc::applet_Init:: ServiceVersion:" + topic_Get(tApp, "ServiceVersion") + " \n");
  }
  else {
    log.warn("commonAppletFnc::applet_Init:: ServiceVersion:" + topic_Get(tApp, "ServiceVersion") + " \n");
    return false;
  }

  return true;
}

function checkForAppletLoad() {
  if (topic_Get(tApp, "AppletStatus") == null) {
    topic_Publish(tApp, "AppletStatus", 'LOADING');
  }
  if (applet_Init()) {
    clearAppletTimer();
    topic_Publish(tApp, "AppletStatus", 'LOADED');
  }
  else if (appletTimerID == null) {
    appletTimerID = setInterval("checkForAppletLoad()", 500);
  }
}

function clearAppletTimer() {
  if( appletTimerID != null ) {
    clearInterval(appletTimerID);
    appletTimerID = null;
  }
}

function applet_sendRequest(userJID, messageXML) {
  try {
    var ret = appletDIV.appletHandle.sendRequest(userJID, messageXML);
   
  }
  catch (e1) {
    var htlmSrc = " NOT AVAILABLE ";
    var errStr = "";
    try {
      var blkBody = PNameSpace.getJSXByName("JSXBODY");
      htlmSrc = blkBody.getRendered().innerHTML; 
      errStr =  "Applet Handle HTML source: " + htlmSrc + "\n" +
                "Description: " + e1.description + "\n";
    }
    catch (e2){
     errStr =  "Applet Handle HTML source: " + htlmSrc + "\n" +
                "Description: " + e1.description + "\n" +
                "Description: " + e2.description + "\n";
    }
    finally {
      handleFatalError("Internal Error: Calling applet sendRequest\n" +
            "Applet Frame: " + appletDIV + "\n" +
            "Applet Handle: " + appletDIV.appletHandle + "\n" + errStr);
      return null;
    }    
  }
  return ret;
}

function applet_handleResponse(messageXML) {
  handleAppletXML(messageXML);
  //HACK: resize window for Firefox2.0 to display GI components
//  firefoxVerFlag = firefox20WindowResize(firefoxVersion, firefoxVerFlag, messageXML);
}

function fncSimulateAppletResponse(SimResponseXML, TopicName) {
  return( handleAppletXML(SimResponseXML, TopicName) );
}

function fncSimulateAppletResponseByName(ResponseName, TopicName) {
  var SimResponseFileName = "jsxuser:///../IPVS-Common/xml/" + ResponseName + ".xml";
  var simResponseDoc = PNameSpace.loadInclude(SimResponseFileName, "Simulated_" + ResponseName, "xml", true);
  var SimResponseXML = simResponseDoc.toString();
  return( fncSimulateAppletResponse(SimResponseXML, TopicName) );
}

function fncSimulateAppletNotificationByName(NotificationName) {
  var SimNotificationFile = "jsxuser:///../IPVS-Common/xml/Notification_" + NotificationName + ".xml";
  try {
    var simResponseDoc = PNameSpace.loadInclude(SimNotificationFile, "SimulatedNotification_" + NotificationName, "xml", true);
  }
  catch(e) {
    handleFatalError( "Internal Error: Loading Simulated Notification: " + SimNotificationFile + "\n" +
            "Description: " + e.description + "\n" );
    return false;
  }  
  var SimResponseXML = simResponseDoc.toString();
  return( fncSimulateAppletResponse(SimResponseXML, null) );
}

function handleInit(messageXML) {
  //alert('call handleInit');
  appletHandle = document.getElementById("ServiceApplet");
  appletHandle.setCallback("handleMessage");
  appletInitXML = messageXML;
}

var appletCallbackFunction = null;
function setCallback(CallbackFunctionStr) {
  appletCallbackFunction = eval(CallbackFunctionStr);
  return appletInitXML;
}

function handleMessage(messageXML) {
  if (appletCallbackFunction != null) {
    appletCallbackFunction(messageXML);
  }
}

function setAppletInDIV(applet_JarCodeStr, codeBaseStr) {
  var appletHtml = "<div id=\"applet\" style=\"position: absolute; top: 0px; left: 0px; height: 0px; width: 0px;\"> " +
	                  "<applet id=\"ServiceApplet\"" +
                      "style=\"width: 1px; height: 1px; float: left;\"" +
                      "classloader_cache=\"false\"" + codeBaseStr + applet_JarCodeStr +
	                  "</applet></div>";
  //load applet to a pre-defined block 
  var blkApplet = getJsxByName("blkApplet");
  blkApplet.setText(appletHtml, true);
}

function unloadApplet() {
  var blkBody = getJsxByName("blkUI"); 
  var divApplet = blkBody.getChild("blkApplet");
  blkBody.removeChild(divApplet);
}
*/