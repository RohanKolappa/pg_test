//-----------------------------------------------------------------------
//load global preferences from IPVS-Common/xml/defaults/Preferences.xml
//application preferences from IPVS_ClientUI/xml/defaults/Preferences.xml
//-----------------------------------------------------------------------
jsx3.require("jsx3.app.UserSettings");
jsx3.Class.defineClass("com.ipvs.util.Preferences", null, null, function(Preferences, Preferences_prototype) {
  
  /** @private @jsxobf-clobber */
  Preferences_prototype._LOG = jsx3.util.Logger.getLogger(Preferences.jsxclass.getName());
  
  Preferences_prototype.init = function() {
    // get a handle to the App
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    
    // do init
    this._preferencesFile = "Preferences.xml";
    this._globalPreferencesFilePath = "JSXAPPS/IPVS-Common/xml/defaults/";
    var appPath = PNameSpace.getEnv("APPPATH");
    this._preferencesFilePath = appPath.substring(appPath.indexOf('JSXAPPS/')) + "/xml/defaults/";
    this._globalPreferencesXMLNode = null;
    this._preferencesXMLNode = null;
    this._prefCookieName = "IPVSPREFERENCES";
    this._clientType = this.clsApp.getClientType();
    this._browserType = this.clsApp.getBrowserType();
    this._giStartPath = this.clsApp.getGIStartPath();
    this._nodePreferences = this._preferencesFileRead();
    //update preferences node by URL input in case of AutoLogin
    //if (autoLogin != "LOGOUT") nodePrefs = updatePrefNodeByURL(preferencesXMLNode);  //TBD
    
    // setup for app init
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    } 
  }
  Preferences_prototype._onAppInit = function() {
  }
  
  Preferences_prototype.getLocalPreferencesNode = function(objName) {
    var objNode = this._nodePreferences.selectSingleNode(".//" + objName);
    return objNode;
  }
  
  Preferences_prototype.setLocalPreferencesNode = function(objName, newObjNode) {
    var objNode = this._nodePreferences.selectSingleNode(".//" + objName);
    if(! objNode ) return false;
    var parentNode = objNode.getParent();
    parentNode.replaceNode(newObjNode, objNode);
     // also put back to the prefs file
    this._preferencesFileWrite();
    return true;
  }
  
  Preferences_prototype.getLocalPreferences = function(objName, defaultValue) {
    var objValue = null;
    // get from tPrefs first
    var objNode = this._nodePreferences.selectSingleNode(".//" + objName);
    if( objNode ) objValue = objNode.getValue();
    // if still blank - return default value
    if ( jsx3.util.strEmpty(objValue) || objValue == '\n' ) objValue = defaultValue;
   
    return objValue;
  }

  Preferences_prototype.setLocalPreferences = function(objName, objValue) {
    // get from tPrefs first
    var objNode = this._nodePreferences.selectSingleNode(".//" + objName);
    if( ! objNode ) return false;
    // set the value to the node
    objNode.setValue(objValue);
    // also put back to the prefs file
    this._preferencesFileWrite();
    return true;
  }

  Preferences_prototype.clearLocalPreferences = function(objName) {
    // get from tPrefs first
    var objNode = this._nodePreferences.selectSingleNode(".//" + objName);
    if( !objNode ) return false;
    // set "" to the node
    objNode.setValue("");
    return true;
  }

  Preferences_prototype.setSessionPreferences = function(objPNameSpace, objName, objValue) {
    var objCookie = new jsx3.app.UserSettings(objPNameSpace,jsx3.app.UserSettings.PERSIST_SESSION);
    objCookie.set(objName,objValue);
    objCookie.save();
  }

  Preferences_prototype.getSessionPreferences = function(objPNameSpace, objName){    
    var objCookie = new jsx3.app.UserSettings(objPNameSpace, jsx3.app.UserSettings.PERSIST_SESSION);
    var objCookieValue = objCookie.get(objName);
    return objCookieValue; 
  }

  Preferences_prototype.clearSessionPreferences = function(objPNameSpace, objName) {
    var objCookie = new jsx3.app.UserSettings(objPNameSpace, jsx3.app.UserSettings.PERSIST_SESSION);
    objCookie.remove(objName);
    objCookie.save();
  }
  
  Preferences_prototype._preferencesFileRead = function(objPNameSpace) {
    var preferencesXMLNode = null;
    if( this._clientType == "WEB" ) {
      preferencesXMLNode = this._loadPreferencesXMLFromCookie();
    }
    else if( this._clientType == "INSTALLED" ) {
      if( this._browserType == "MSIE" ) {
        //load Preferences.xml from local drive 
        preferencesXMLNode = this._loadPreferencesXMLFromFile(objPNameSpace);
      }
      else if( this._browserType == "FIREFOX" ) {
        //treat FF as a kind of WEB client
        preferencesXMLNode = this._loadPreferencesXMLFromCookie();
      }
    }
    return preferencesXMLNode;
  }
  
  Preferences_prototype._loadPreferencesXMLFromFile = function(objPNameSpace) {
    //global Preferences.xml
    var preferencesFilePath = objPNameSpace == "GLOBAL" ? this._globalPreferencesFilePath : this._preferencesFilePath;
    return loadXMLFile(preferencesFilePath + this._preferencesFile);
  }

  Preferences_prototype._loadPreferencesXMLFromCookie = function(objPNameSpace) {
    var preferencesCookie = readCookie(this._prefCookieName);
    var cookiePreferencesXMLNode = null;
    if (jsx3.util.strEmpty(preferencesCookie)) {
      //if no cookie load default Preferences.xml
      cookiePreferencesXMLNode = this._loadPreferencesXMLFromFile("GLOBAL");

      //write this default preferences to a cookie
      createCookie(this._prefCookieName, (cookiePreferencesXMLNode.getXML()).replace(/[\r\n\t]/g, ""), 0);
    }
    else {
      //have a 'preferences' cookie,load it in as current preferences
      cookiePreferencesXMLNode = (new jsx3.xml.Document()).loadXML(preferencesCookie);
    }
    return cookiePreferencesXMLNode;
  }
  
  Preferences_prototype._preferencesFileWrite = function(objPNameSpace) {
    if( this._clientType == "WEB" ) {
      //WEB client:update cookie
      var preferensXML = (this._nodePreferences.getXML()).replace(/[\r\n\t]/g, "");
      createCookie(this._prefCookieName, preferensXML);
    }
    else if( this._clientType == "INSTALLED" ) {
      //INSTALLED client:write to Preferences.xml
      //if 'GLOBAL' preferencesFilePath = globalPreferencesFilePath
      var preferencesFilePath = objPNameSpace == "GLOBAL" ? this._globalPreferencesFilePath : this._preferencesFilePath;
      if( this._browserType == "MSIE" ) {
        //wirte Preferences.xml to local drive using ActiveXObject("Scripting.FileSystemObject")
        prefFileWrite_Msie(objPNameSpace, this._giStartPath + "\\" + preferencesFilePath + this._preferencesFile, this._nodePreferences);
      }
      else if( this._browserType == "FIREFOX" ) {
      //wirte Preferences.xml to cookie as a kind of WEB client for now 
        prefFileWrite_Firefox(objPNameSpace, this._giStartPath + "\\" + preferencesFilePath + this._preferencesFile, this._nodePreferences, prefCookieName);
      }
    }
  }
});


//this can't be replaced by tApp,beacuse tApp is renew by common_InitDefaultsLoad()  
//var autoLogin = null;
// ---------------  LOCAL PREFERENCE FNC -----------------------------


/***
 * using auto login URL parameters to update Preferences
 ***/
//function updatePrefNodeByURL(preferencesXMLNode) {
//  if (clientLocation.indexOf("?user") > 0) {
//    //trigger only by preferences_Init,not by ClientDataUpgrade
//    var user = getPrefFromURLInput(clientLocation, "user");
//    var pwd = getPrefFromURLInput(clientLocation, "password");
//    var resrc = getPrefFromURLInput(clientLocation, "resource");
//    var ip = getPrefFromURLInput(clientLocation, "ip");
//    preferencesXMLNode.selectSingleNode(".//Login/user").setValue(user);
//    preferencesXMLNode.selectSingleNode(".//Login/password").setValue(pwd);
//    preferencesXMLNode.selectSingleNode(".//Login/resource").setValue(resrc);
//    preferencesXMLNode.selectSingleNode(".//Login/domain").setValue(ip);
//    preferencesXMLNode.selectSingleNode(".//Login/server").setValue(ip);
//    //avoid Logoff - AutoLogin loop 
//    if (!jsx3.util.strEmpty(user) && !jsx3.util.strEmpty(pwd)
//        && !jsx3.util.strEmpty(resrc) && !jsx3.util.strEmpty(ip)) {
//      topic_Set(tApp,"AutoLogin","TRUE");
//    }
//    else {
//      topic_Set(tApp, "AutoLogin", "FALSE");
//    }
//  }
//  return preferencesXMLNode;
//}

/***
 * help function to grap preferences from URL parameters
 ***/
//function getPrefFromURLInput(clientLocation, name) {//clientLocation="http://localhost:2636/3.8.1-0325/IPVSClient-Index.htm?user=sunuser3&password=Sunuser123&resource=sunpc3&ip=10.1.5.178"
//  name = name.replace(/[\[]/, "\\\[").replace(/[\]]/, "\\\]");
//  var regexS = "[\\?&]" + name + "=([^&#]*)";
//  var regex = new RegExp(regexS);
//  var results = regex.exec(clientLocation);
//  if (results == null)
//    return "";
//  else
//    return results[1];
//}
