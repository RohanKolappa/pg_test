/***********************************************************************
Separate the following browser sensitive functions from commonUtilFnc.js
************************************************************************/

//---------------------- MEMORY HANDLER FNC --------------------

function memoryLogWriteHandler_Firefox(dateTime, recString) {
  log.info(" call handleMemoryHandler_Firefox");
  return null;
}

//-------------------- HW INFO FNC ------------------------------
//---- MAC Address
function getHWInfo_MACAddress_Firefox() {
  log.info(" call getHWInfo_MACAddress_Firefox");
  return null;
}
//--- ComputerName
function getHWInfo_ComputerName_Firefox() {
  log.info(" call getHWInfo_ComputerName_Firefox");
  return null;
}
//--- HDD Volume Serial number
function getHWInfo_HDVolumeSerial_Firefox() {
  log.info(" call getHWInfo_HDVolumeSerial_Firefox");
  return null;
}

function getHWInfo_IPAddress_Firefox() {
  log.info(" call getHWInfo_IPAddress_Firefox");
  return null;
}

function getHWInfo_IPAddressOfMacAddress_Firefox(loginInterfaceMacAddress) {
  log.info(" call getHWInfo_IPAddressOfMacAddress_Firefox");
  return null;
}

function getHWInfo_InterfaceList_Firefox() {
  log.info(" call getHWInfo_InterfaceList_Firefox");
  return null;
}

//-------------------cmd Run/Exec/Terminate -------------------------

function cmdRun_Firefox(cmd, intWinStyle, bWaitOnReturn) {
  log.info(" call cmdRun_Firefox");
  return null;
}
function cmdExec_Firefox(cmd) {
  log.info(" call cmdExec_Firefox");
  return null;
}
function cmdTerminate_Firefox(oExec) {
  log.info(" call cmdTerminate_Firefox");
  return null;
}
//simulate to press F11 for a media player
function fullScreen_Firefox() {
  log.info(" call fullScreen_Firefox");
}

//write preferences
function prefFileWrite_Firefox(objPNameSpace, preferencesFilePath, preferencesXMLNode, prefCookieName) {
  //write preference to cookie if the browser is FF
  var preferensXML = (preferencesXMLNode.getXML()).replace(/[\r\n\t]/g, "");
  createCookie(prefCookieName, preferensXML);
  log.info(" prefFileWrite_Firefox,create cookie: "+ prefCookieName);
}

function chkForClientDataUpgrade_Firefox(ServiceVersion, ClientDataXMLNode) {
  log.info(" chkForClientDataUpgrade_Firefox ServiceVersion: " + ServiceVersion);
  chkForClientDataUpgrade_Web(ServiceVersion, ClientDataXMLNode);
}

//In Firefox 2.0,GI components are only displayed after a window resize
function firefox20WindowResize(firefoxVersion, firefoxVerFlag, messageXML) {
  if (firefoxVersion > 0 && firefoxVersion < 3) {
    if (messageXML && (messageXML.match("<requestname>LoginRequest") || messageXML.match("<requestname>LogoutRequest"))) {
      return firefoxVerFlag;
    }
    var val = firefoxVerFlag ? 1 : -1;
    window.resizeBy(val, 0);
    log.warn("FirefoxVersion:" + firefoxVersion + ":WindowResize:" + val + ":firefoxVerFlag:" + firefoxVerFlag);
    return !firefoxVerFlag;
  }
}