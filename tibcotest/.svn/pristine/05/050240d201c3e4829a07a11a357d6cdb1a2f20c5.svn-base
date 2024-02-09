//-----------------------------------------------------------------------
//load global preferences from IPVS-Common/xml/defaults/Preferences.xml
//application preferences from IPVS_ClientUI/xml/defaults/Preferences.xml
//-----------------------------------------------------------------------
// a global doc for prefs
var tPrefs = "Preferences";
var tPrefsDoc = null;

var preferencesFile = "Preferences.xml";
var preferencesFilePath;
var globalPreferencesFilePath;
var globalFilePrefix;
var filePrefix;
var globalPreferencesXMLNode = null;
var preferencesXMLNode = null;
var NAMESPACE = PNameSpace.getEnv("NAMESPACE");
var prefCookieName = "IPVSPREFERENCES";
//this can't be replaced by tApp,beacuse tApp is renew by common_InitDefaultsLoad()  
var autoLogin = null;
// ---------------  LOCAL PREFERENCE FNC -----------------------------
jsx3.require("jsx3.app.UserSettings");

function preferences_Init(){
  // create the "Preferences" topic
  tPrefs = tPrefs + "_" + jsx3.xml.CDF.getKey();
  tPrefsDoc = topic_Create(tPrefs);
  // initialize it from the gloabl prefs file
  // read the entire global prefs file as a node
  var nodePrefs = preferencesFile_Read("GLOBAL");
  //update preferences node by URL input in case of AutoLogin
  if (autoLogin != "LOGOUT") nodePrefs = updatePrefNodeByURL(preferencesXMLNode);
  // get a handle to the prefs topic root node
  var nodePrefsRoot = topic_GetRootNode(tPrefs);
  // append prefs node to topic root
  nodePrefsRoot.appendChild(nodePrefs);
}

//used for Server List
function getLocalPreferencesNode(objPNameSpace, objName){
  // get from tPrefs first
  var preferenceNode = topic_GetNode(tPrefs, "Preferences");
  var objNode = preferenceNode.selectSingleNode(objName);
  return objNode;
}

//using for Server List or setting up a entire element in prefrences
function setLocalPreferencesNode(objPNameSpace, objName, newObjNode){
  // get from tPrefs first
  var preferenceNode = topic_GetNode(tPrefs, "Preferences");
  var objNode = preferenceNode.selectSingleNode(objName);
  if(! objNode ) return false;
  var parentNode = objNode.getParent();
  parentNode.replaceNode(newObjNode, objNode);
    // also put back to the prefs file
  preferencesFile_Write("GLOBAL");
  return true;
}

function getLocalPreferences(objPNameSpace, objName, defaultValue){
  var objValue = null;
  // get from tPrefs first
  var objNode = topic_GetNode(tPrefs, objName);
  if( objNode ) objValue = objNode.getValue();
  // if still blank - return default value
  if ( jsx3.util.strEmpty(objValue) || objValue == '\n' ) objValue = defaultValue;
 
  return objValue;
}

function setLocalPreferences(objPNameSpace, objName, objValue){
  // get from tPrefs first
  var objNode = topic_GetNode(tPrefs, objName);
  if( ! objNode ) return false;
  // set the value to the node
  objNode.setValue(objValue);
  // also put back to the prefs file
  preferencesFile_Write("GLOBAL");
  
  return true;
}

function addAttributeLocalPreferences(objPNameSpace, objName, objAttribute, objValue) {
  // get from tPrefs first
  var objNode = topic_GetNode(tPrefs, objName);
  if (!objNode) return false;
  // set the value to the node
  objNode.setAttribute(objAttribute, objValue);
  // also put back to the prefs file
  preferencesFile_Write("GLOBAL");

  return true;
}

function clearLocalPreferences(objPNameSpace, objName){    
  // get from tPrefs first
  var objNode = topic_GetNode(tPrefs, objName);
  if( ! objNode ) return false;
  // set "" to the node
  objNode.setValue("");
  return true;
}

//------------------ TEMP(SESSION) PREFERENCE FNC -----------------------------

function setSessionPreferences(objPNameSpace, objName, objValue) {
  var objCookie = new jsx3.app.UserSettings(objPNameSpace,jsx3.app.UserSettings.PERSIST_SESSION);
  objCookie.set(objName,objValue);
  objCookie.save();
}

function getSessionPreferences(objPNameSpace, objName){    
  var objCookie = new jsx3.app.UserSettings(objPNameSpace, jsx3.app.UserSettings.PERSIST_SESSION);
  var objCookieValue = objCookie.get(objName);
  return objCookieValue; 
}

function clearSessionPreferences(objPNameSpace, objName) {
  var objCookie = new jsx3.app.UserSettings(objPNameSpace, jsx3.app.UserSettings.PERSIST_SESSION);
  objCookie.remove(objName);
  objCookie.save();
}

function preferencesFile_Read(objPNameSpace) {
  if (clientType == "WEB") {
    preferencesXMLNode = loadPreferencesXML_fromCookie();
  }
  else if (clientType == "INSTALLED") {
    if (browserType == "MSIE") {
      //load Preferences.xml from local drive 
      preferencesXMLNode = loadPreferencesXML_fromFile(objPNameSpace);
    }
    else if (browserType == "FIREFOX") {
      //treat FF as a kind of WEB client
      preferencesXMLNode = loadPreferencesXML_fromCookie();
    }
  }
    return preferencesXMLNode;
}

/***
 * help function to load file Preferences.xml
 * from either INSTALLED/drive or WEB/server 
 ***/
function loadPreferencesXML_fromFile(objPNameSpace) {
  //INSTALLED client read preferences from local XML file
  if( NAMESPACE == "STXClientUI" ) {
    globalFilePrefix = "../";
    filePrefix = "";
  }
  else {
    globalFilePrefix = "JSXAPPS/";
    filePrefix = PNameSpace.getEnv("APPPATH") + "/";
  }
  //global Preferences.xml
  globalPreferencesFilePath = globalFilePrefix + "IPVS-Common/xml/defaults/";
  globalPreferencesXMLNode = null;
  globalPreferencesXMLNode = loadXMLFile(globalPreferencesFilePath + preferencesFile);
  //application Preferences.xml
  preferencesFilePath = filePrefix + "xml/defaults/";
  var lcPreferencesXMLNode = loadXMLFile(preferencesFilePath + preferencesFile);
  if (objPNameSpace == "GLOBAL") {
    lcPreferencesXMLNode = globalPreferencesXMLNode;
  }
  return lcPreferencesXMLNode;
}

/***
 * help function to load Preferences from cookie
 ***/
function loadPreferencesXML_fromCookie() {
  //WEB client read preferences from either server(no cookie) or cookie
  var preferencesCookie = readCookie(prefCookieName);
  var cookiePreferencesXMLNode = null;
  if (jsx3.util.strEmpty(preferencesCookie)) {
    //if no cookie load default Preferences.xml
    globalPreferencesXMLNode = loadPreferencesXML_fromFile("GLOBAL");
    cookiePreferencesXMLNode = globalPreferencesXMLNode;

    //write this default preferences to a cookie
    var preferensXML = (cookiePreferencesXMLNode.getXML()).replace(/[\r\n\t]/g, "");
    createCookie(prefCookieName, "",0);
  }
  else {
    //have a 'preferences' cookie,load it in as current preferences
    cookiePreferencesXMLNode = (new jsx3.xml.Document()).loadXML(preferencesCookie);
  }
  return cookiePreferencesXMLNode;
}

/***
 * using auto login URL parameters to update Preferences
 ***/
function updatePrefNodeByURL(preferencesXMLNode) {
  if (clientLocation.indexOf("?user") > 0) {
    //trigger only by preferences_Init,not by ClientDataUpgrade
    var user = getPrefFromURLInput(clientLocation, "user");
    var pwd = getPrefFromURLInput(clientLocation, "password");
    var resrc = getPrefFromURLInput(clientLocation, "resource");
    var ip = getPrefFromURLInput(clientLocation, "ip");
    preferencesXMLNode.selectSingleNode(".//Login/user").setValue(user);
    preferencesXMLNode.selectSingleNode(".//Login/password").setValue(pwd);
    preferencesXMLNode.selectSingleNode(".//Login/resource").setValue(resrc);
    preferencesXMLNode.selectSingleNode(".//Login/domain").setValue(ip);
    preferencesXMLNode.selectSingleNode(".//Login/server").setValue(ip);
    //avoid Logoff - AutoLogin loop 
    if (!jsx3.util.strEmpty(user) && !jsx3.util.strEmpty(pwd)
        && !jsx3.util.strEmpty(resrc) && !jsx3.util.strEmpty(ip)) {
      topic_Set(tApp,"AutoLogin","TRUE");
    }
    else {
      topic_Set(tApp, "AutoLogin", "FALSE");
    }
  }
  return preferencesXMLNode;
}

/***
 * help function to grap preferences from URL parameters
 ***/
function getPrefFromURLInput(clientLocation, name) {//clientLocation="http://localhost:2636/3.8.1-0325/IPVSClient-Index.htm?user=sunuser3&password=Sunuser123&resource=sunpc3&ip=10.1.5.178"
  name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
  var regexS = "[\\?&]" + name + "=([^&#]*)";
  var regex = new RegExp(regexS);
  var results = regex.exec(clientLocation);
  if (results == null)
    return "";
  else
    return results[1];
}


/*** ----------------------------------------------------------------------*
 * set back to global preferences to IPVS-Common/xml/defaults/Preferences.xml
 * OR set application preferences to 
 *(STX_ClientUI,IPVS_SettingsUI,IPVS_ClientUI)/ xml/defaults/Preferences.xml
 ***-----------------------------------------------------------------------*/
function preferencesFile_Write(objPNameSpace) {
  preferencesXMLNode = topic_GetNode(tPrefs, "Preferences");
  if (clientType == "WEB") {
    //WEB client:update cookie
    var preferensXML = (preferencesXMLNode.getXML()).replace(/[\r\n\t]/g, "");
    createCookie(prefCookieName, preferensXML);
  }
  else if (clientType == "INSTALLED") {
    //INSTALLED client:write to Preferences.xml
    //if 'GLOBAL' preferencesFilePath = globalPreferencesFilePath
    preferencesFilePath = objPNameSpace == "GLOBAL" ? globalPreferencesFilePath : preferencesFilePath;
    if (browserType == "MSIE") {
      //wirte Preferences.xml to local drive using ActiveXObject("Scripting.FileSystemObject")
      prefFileWrite_Msie(objPNameSpace, preferencesFilePath, preferencesXMLNode);
    }
    else if (browserType == "FIREFOX") {
    //wirte Preferences.xml to cookie as a kind of WEB client for now 
      prefFileWrite_Firefox(objPNameSpace, preferencesFilePath, preferencesXMLNode, prefCookieName);
    }
  }
}
//unload for all Preferences
function preferences_Destory(){
  topic_Destroy(tPrefs);
  tPrefs = "Preferences";
  tPrefsDoc = null;
}
