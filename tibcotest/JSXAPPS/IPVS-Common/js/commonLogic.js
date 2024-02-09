// setup log handle
var log = null;

//create Global topic and event
var tApp = "Application";
var tAppDoc = null;
var eventApp = null;
var eventHandlerArr = null;

//app. starting components
var cmpApp = null;

// set this to test animation only without any request/response
var AnimationMode = false;
// set this to test with simulated response XMLs
var SimulationMode = false;

// set this to pop up an alert before/after every applet requestresponse
var AppletRequestAlertMode = true;
var AppletResponseAlertMode = true;

// set this to pop up an alert before every player call
var PlayerAlertMode = false;
/***
 * define and change player TimeLength from 1Hr to 4Hr 
 * changes in clsComponent.cmpSlider,clsCtrlAgent,cmpblkCollarboration
 ***/
var DefaultTimeLength = 4 * 3600000;

// hard coded default values  
var DefaultLoginDomain = "";
var DefaultLoginServer = "";
var DefaultLoginPort = "5222";
var DefaultLoginResource = "";
var DefaultServiceDomain = "default";
var DefaultAutoJoin = "";
var DefaultLoginInterfaceMacAddress = "";
var DefaultLoginResourcePassword = "";

//var g_TreeRootNodeImage = "images/icons/RootNode.gif";
//var g_TreeNodeImage = "images/icons/Node.gif";

//for new applet block 
clsApplet = null;
clsAppletPubSub = null;

//browser type
var browserType = null;
//client type
var clientType = null;
//client relative start path
var giStartPath = null;
//firefox version
var firefoxVersion = -1;
var firefoxVerFlag = true;

var clientLocation = null;
var tagOfGitak = false;

if( SimulationMode == true ) {
  var DefaultGroupTagName = "string";
}


//common variables for classes
var IpvsLogin = null;
//var RetryLogin = null;
//initialise common classes
function common_InitClasses() {
  if( jsx3.util.strEmpty(IpvsLogin) ) {
    IpvsLogin = new com.ipvs.api.Login(tApp);
  }
  else {
    IpvsLogin.updateTopicName(tApp);
  }
}

function common_DestroyClasses() {
  IpvsLogin.unSubscribeAll();
}

//initial logger for the application
function common_InitLog() {
  //create log message handle
  if (log == null) {
    log = handleLogMessage();
    initMemoryHandler(PNameSpace);
    initHtmlRenderHandler();
    if (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI" || PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") {
      initAOPHandler();
    }
  }

}
//initial tApp for the application
function common_InitApplicationTopic() {
  //create tApp,tAppDoc and eventApp, eventHandlerArr
  if (eventApp == null) {
    // initialize the "Application" topic
    tApp = tApp + "_" + jsx3.xml.CDF.getKey();
    tAppDoc = topic_Create(tApp);
    // setup a global app event handler
    eventApp = new EventListner(tApp);
    eventHandlerArr = new Object;
    eventHandlerArr[tApp] = eventApp;
  }
}

//a helper function to detect Firefox version 
function getFirefoxVersion(usrAgtStr) {
  var index = usrAgtStr.indexOf('firefox');
  return parseFloat(usrAgtStr.substring(index + 'firefox'.length + 1));
}
/***
 * get client browser type (IE/FF), client type (web/local)
 * and client start relative path
 ***/
function common_InitClientEnvironment() {
  //check browser type and set the type to tApp
  var usrAgt = navigator.userAgent.toLowerCase();
  //currently only support MSIE and FIREFOX treat others as UNKOWN
  browserType = usrAgt.indexOf("msie") >= 0 ? "MSIE" : usrAgt.indexOf('firefox') >= 0 ? "FIREFOX" : "UNKNOWN";
  //assign it in local make Logoff-re-Login correct
  clientLocation = window.location.href;
  var dirPrefix = ""; 

  //get GI path from clientLocation
  var nameSpace = PNameSpace.getEnv("NAMESPACE");
  if (nameSpace == "STXClientUI") {
    clientLocation = clientLocation.substring(0, clientLocation.lastIndexOf("JSXAPPS") - 1);
    dirPrefix = "../../";
  }
  else {
    clientLocation = clientLocation.substring(0, clientLocation.lastIndexOf("/"));
  }

  //localize the following parameters for debug
  var objNode = PNameSpace.loadInclude(dirPrefix+"debug/debugProperty.xml", null, "xml");
  var debugNode = objNode?objNode.selectSingleNode("/Debug"):null;
  var DEBUGMODE_GISTARTPATH = debugNode ? debugNode.selectSingleNode(".//giStartPath").getValue():"";
  var DEBUGMODE_CLIENTTYPE = debugNode ? debugNode.selectSingleNode(".//clientType").getValue():"";
  var DEBUGMODE_LOGINSERVER = debugNode ? debugNode.selectSingleNode(".//managementServerIP").getValue():"";

  //If browser is Firefox,detect and set version
  if (browserType=="FIREFOX") {
    firefoxVersion = getFirefoxVersion(usrAgt);
  }
  //get clientType and valid giStartPath 
  if (clientLocation.indexOf("file:")==0) {
    clientType = "INSTALLED";

    giStartPath = clientLocation.substring(8);
    //process 3 cases for GITAK (GI-Builder launch,GI-Builder record and manual script)
    if (giStartPath.match("shell.html") != null && giStartPath.match("gitak/selenium/core") != null) {
      //[1]'Launch in GITAK',i.e.:"C:/ms3_14//shell.html?jsxapppath=file:///C:/ms3_14/gitak/selenium/core/../../../JSXAPPS/IPVS_AdminUI"
      var gitakIndex = giStartPath.indexOf("gitak/selenium/core");
      var jsxapppathIndex = giStartPath.indexOf("jsxapppath") + "jsxapppath=file:///".length;
      giStartPath = giStartPath.substring(jsxapppathIndex, gitakIndex - 1);
      tagOfGitak = true;
    }
    else if (giStartPath.indexOf("GI_Builder/plugins/jsx3.ide.recorder") > 0) {
      //[2]'Launch Record', i.e.:"C:/ms3_14/GI_Builder/plugins/jsx3.ide.recorder/recorder.html"
      var girecordIndex = giStartPath.indexOf("GI_Builder/plugins/jsx3.ide.recorder");
      giStartPath = giStartPath.substring(0, girecordIndex - 1);
      tagOfGitak = true;
    }
    else if (giStartPath.match("jsxapppath=file:///") != null) {
      //[3]tibco gitak using path like jsxapppath=file:/// in testSetup.html for automation
      giStartPath = giStartPath.substring(0, giStartPath.lastIndexOf("GI/") + 2);
    }
  }
  else if (clientLocation.indexOf("http://localhost") != -1) {
    clientType = DEBUGMODE_CLIENTTYPE;
    if (clientType == "WEB"){
      giStartPath = clientLocation.substring(0);
    }
    else {
      giStartPath = DEBUGMODE_GISTARTPATH;
    }
  }
  else {
    clientType = "WEB";
    giStartPath = clientLocation.substring(0);
  }
  
  topic_Set(tApp,"clientType",clientType);

  log.info("commonLogic::common_InitClientEnvironment:giStartPath=" + giStartPath);
  //giStartPath = "C://svn-branch-work//3.8.1-0325";
  
  //set WEB client login server to topic
  if (clientType == "WEB"){
    //set login server for 'WEB' client
    var webClientLoginServer = window.location.href.substring(7);
    webClientLoginServer = webClientLoginServer.substring(0, webClientLoginServer.indexOf('/'));
    //for 'WEB' client debug
    if (webClientLoginServer.indexOf('localhost') != -1) {
      webClientLoginServer = DEBUGMODE_LOGINSERVER;
    }
    topic_Set(tApp, "WebClientLoginServer", webClientLoginServer);
  }

  if (nameSpace != "IPVSSettingsUI"){
    //check and upgrade client data based on migration clientdata.xsl
    commonClientDataUpgrade();
  }
}

/***
* upgrade client data based on mode/migration/../clientdata.xsl
* (currently only Preferences.xml) 
***/
function commonClientDataUpgrade() {
  giStartPath = giStartPath.replace(/%20/g, " ");
  var ServiceVersionFilePath = giStartPath + "/JSXAPPS/IPVS-Common/model/objectXml/service.version";
  var ServiceVersionXMLNode = loadXMLFile(ServiceVersionFilePath);
  if (ServiceVersionXMLNode == null) return;
  //put hack here for now as per Firefox fail to get 'Ver' attribute (.selectSingleNode("//Service"))
  var serviceVersion = ServiceVersionXMLNode.getAttribute("Ver"); 
  if (serviceVersion == null) {
    log.error("commonLogic::commonClientDataUpgrade:No 'serviceVersion' in service.version.");
    return;
  }
  //convert a major.minor.revision to an integer number
  serviceVersion = getVerNumberByStr(serviceVersion);
  if (clientType == "WEB") {
    //upgrade preferences based on preferences from cookie 
    var preferencesXMLNode = loadPreferencesXML_fromCookie();
    chkForClientDataUpgrade_Web(serviceVersion, preferencesXMLNode);
  }
  else if (clientType == "INSTALLED") {
    if (browserType == "MSIE") {
      //upgrade preferences based on Preferences.xml from local drive
      var clientDataFilePath = giStartPath + "/JSXAPPS/IPVS-Common/xml/defaults/Preferences.xml";
      var clientDataXMLNode = loadXMLFile(clientDataFilePath);
      if (clientDataXMLNode == null) {
        log.error("commonLogic::commonClientDataUpgrade:Fail to load Preferences.xml");
        return;
      }
      chkForClientDataUpgrade_Msie(serviceVersion);
      log.info("commonLogic::commonClientDataUpgrade:upgrade client data from local drive for IE");
    }
    else if (browserType == "FIREFOX") {
      //upgrade preferences based on preferences from cookie
      var preferencesXMLNode = loadPreferencesXML_fromCookie();
      //treat FF as a kind of WEB client
      chkForClientDataUpgrade_Firefox(serviceVersion, preferencesXMLNode);
      log.info("commonLogic::commonClientDataUpgrade:upgrade client data for Firefox client");
    }
  }
}
/***
* help function of commonClientDataUpgrade
***/
function chkForClientDataUpgrade_Web(serviceVersion, clientDataXMLNode, preferensXML) {
  //WEB client(including INSTALLED FF) upgrade and save the Preferences to cookie
  var rootNode = clientDataXMLNode.getRootNode(); 
  var clientDataVersionStr = rootNode.selectSingleNode("//Preferences").getAttribute("serviceVer");
  //migration directory start from "1.0"
  clientDataVersionStr = jsx3.util.strEmpty(clientDataVersionStr) ? "1.0" : clientDataVersionStr;
  var clientDataVersion = getVerNumberByStr(clientDataVersionStr);

  if (serviceVersion == clientDataVersion) {
    //service verison equal client version
    if (!jsx3.util.strEmpty(preferensXML)) {
      createCookie(prefCookieName, preferensXML);
    }
    return;
  }
  else if (serviceVersion < clientDataVersion) {
      //bug fix for 25510: when user forget delete existing cookie force to delete it
      //directory of 'migration' like 1.4,1.5,2.1,2.2..2.9,2.10,2.19,2.21 etc.
      eraseCookie(prefCookieName);
      var initPreferencesCookie = loadPreferencesXML_fromCookie();
      chkForClientDataUpgrade_Web(serviceVersion, initPreferencesCookie, preferensXML);
      return;
  }
  
  var XSLBasePath = giStartPath + "/JSXAPPS/IPVS-Common/model/migration/";
  var XSLFilePath = XSLBasePath + clientDataVersionStr + "/client/clientdata.xsl";
  var XSLFile = PNameSpace.loadInclude(XSLFilePath, "upgradeXML", "xsl");
  if (XSLFile == null) {
    log.error("Fail to load clientdata.xsl to upgrade Preferences.xml");
    return;
  }
  var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
  var NewXML = XSLDoc.transform(rootNode);
  var preferensXML = NewXML.replace(/[\r\n\t]/g, "");
  var prefXMLNode = (new jsx3.xml.Document()).loadXML(preferensXML);

  //check migration xsl data valid (number keep growing)
  var nextClientVersionStr = prefXMLNode.getRootNode().selectSingleNode("//Preferences").getAttribute("serviceVer"); ;
  var nextClientVersion = getVerNumberByStr(nextClientVersionStr);
  //migration data validation check, it should keep increase
  if (nextClientVersion <= clientDataVersion ) {
    log.error("commonLogic:chkForClientDataUpgrade_Web::  Bad version number in migration directory '" + nextClientVersionStr + "'.");
    throw new jsx3.Exception("commonLogic:chkForClientDataUpgrade_Web: Bad version number in migration directory '" + nextClientVersionStr + "'.");
  }

  //recusive call to move forward version number
  chkForClientDataUpgrade_Web(serviceVersion, prefXMLNode, preferensXML);
}


// get version number by version string (major.minor.revision)
function getVerNumberByStr(versionStr) {
  //split major,minor and revision to an array  
  var verArr = versionStr.split('.');
  //if no revision number, convert it to 0. ie. '2.5' -->'2.5.0'
  verArr[2] = verArr[2] == undefined ? 0 : verArr[2];
  var verNumber = verArr[0] * 1000000 + verArr[1] * 1000 + parseInt(verArr[2]);
  return verNumber;
}

function common_InitDefaultsLoad() {
  //initialize preferences
  preferences_Init();

  //set DefaultAutoJoin to tApp
  DefaultAutoJoin = getLocalPreferences(PNameSpace, ".//AutoJoin", "false");
  topic_Set(tApp, "DefaultAutoJoin", DefaultAutoJoin);
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI" ) {
    var bAutoLogin = getLocalPreferences(PNameSpace, ".//Login/AutoLogin", "false");
    //'autoLogin' is global variable which is 'null' initially and setting as 'LOGOUT' in case of logout.
    //auto logging only when client lauched
    if( bAutoLogin == "true" && autoLogin != "LOGOUT" ) {
      topic_Set(tApp, "PCClientAutoLogin", "ENABLED");
    }
    else {
      topic_Set(tApp, "PCClientAutoLogin", "DISABLED");
    }
  }
}

function common_InitStartApp() {
  // initialize Cache
  commonCache_Init();
  //initialize the agent cache
  commonAgentCache_Init();
  common_InitClasses();

  cmpCommonErrorCode_init();
  cmpReq_Create();

  // create the App components
  cmpApp = cmpApp_Create();
  // initialize the App components
  cmpApp.init();

  //subscribe logout state
  app_SubscribeLogoutState();
  // start things off
  cmpApp.show();

  //set login status "Loading..."
  if (clsApplet && clsApplet.appletHandle) {
    topic_Publish(tApp, "LoginStatus", "");
    //simulate applet LOADED while re-login
    topic_Set(tApp, "ServiceVersion", clsApplet.getServiceVersion());
    topic_Publish(tApp, "AppletStatus", 'LOADED');

  }
  else {
    topic_Publish(tApp, "LoginStatus", "Loading...");
  }
}

//unload for IPVSClient,STX-Client,AdminUI
function common_FinalOnUnLoad() {

  common_BasicFinalOnUnLoad();
  
  //used for IPVSClient,STX-Client,Admin
  cmpCommonErrorCode_Destroy();
  cmpReq_Destroy();
  cmpApp_Destroy();
  common_DestroyClasses();
  // tCache
  commonCache_Final();
  //commonAgentCache_Final();

  preferences_Destory();
}

//basic unload for all projects
function common_BasicFinalOnUnLoad() {
  // destroy all topics
  // destroy each topic in eventhandlerarr 
  for (var topic in eventHandlerArr) {
    eventHandlerArr[topic].destroy();
    delete eventHandlerArr[topic];
  }
  eventApp.destroy();
  eventApp = null;

  cmpApp.destroy();
  cmpApp = null;

  topic_Destroy(tApp);
  tApp = "Application";
  tAppDoc = null;
}
