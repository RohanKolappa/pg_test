//---------------------- SERVER TIME FNC --------------------
function newServerDate() {
  var CurrDate = new Date();
  var deltaTime = topic_Get(tApp, "DeltaServerLocalTime");
  if( deltaTime == null) {
    deltaTime = 0;
  }
  var CurrServerTime = parseInt(CurrDate.getTime()) - parseInt(deltaTime);
  CurrDate.setTime(CurrServerTime);
  return CurrDate;
}

function getCurrServerTime() {
  var CurrTime = (new Date()).getTime();
  var deltaTime = topic_Get(tApp, "DeltaServerLocalTime");
  if( deltaTime == null) {
    deltaTime = 0;
  }  
  var CurrServerTime = parseInt(CurrTime) - parseInt(deltaTime);
  return CurrServerTime;
}

// ---------------------- Date time FNC --------------------
function getDateTimeFromTimeCode(DateTimeCode) {
  var CurrDate = new Date();
  var TimeCode = DateTimeCode.match(/\d{1,}/);
  CurrDate.setTime(TimeCode);
  CurrDate = formatDate(CurrDate, "MM/dd/yyyy HH:mm");
  return CurrDate;
}

function getDateTimeHHMMFromTimeCode(DateTimeCode) {
  var CurrDate = new Date();
  var TimeCode = DateTimeCode.match(/\d{1,}/);
  CurrDate.setTime(TimeCode);
  CurrDate = formatDate(CurrDate, "hh:mm a MM/dd/yyyy");
  return CurrDate;
}

function getTimeHHMMFromTimeCode(DateTimeCode) {
  var CurrDate = new Date();
  var TimeCode = DateTimeCode.match(/\d{1,}/);
  CurrDate.setTime(TimeCode);
  CurrDate = formatDate(CurrDate, "hh:mm a");
  return CurrDate;
}

function getTimeZone() {
  var d = new Date();
  var offTz = d.getTimezoneOffset();
  var gmtSign = (offTz > 0 ? '-' : '+');
  var offTz = offTz < 0 ? offTz * -1 : offTz;
  var gmtHours = ('0' + parseInt(offTz / 60)).replace(/.*(\d\d)$/, '$1');
  var gmtMin = ('0' + offTz % 60).replace(/.*(\d\d)$/, '$1');
  var gmtTz = gmtSign + gmtHours + ":" + gmtMin;
  return gmtTz;
}

// ---------------------- Load XML File --------------------
function loadXMLFile(XMLFile) {
  var XMLFilePath = null;
  if (XMLFile.indexOf("http://")!=-1) {
    XMLFilePath = XMLFile;
  }
  else {
    XMLFilePath = "jsxuser:///" + XMLFile;
  }
  try {
    var objNode = PNameSpace.loadInclude(XMLFilePath, XMLFile , "xml", true);
    return objNode;
  }
  catch(e) {
    handleFatalError("Internal Error: Loading File: " + XMLFilePath + "\n" + 
          "Result XML: " + objNode);
    return null;
  }
}

//------------handle fatal error---------------

function handleFatalError(strError) {
  log.error(strError);
  //alert("Fatal Internal Error: Please shutdown and restart the application");
  dlgMsg_Create("FatalError");
}

//---------------------- BROWSER CLOSE FNC --------------------
function subscribeBrowserClose(){
  jsx3.gui.Event.subscribe(jsx3.gui.Event.BEFOREUNLOAD, eval(handleBrowserClose)); 
}

function unSubscribeBrowserClose(){
  jsx3.gui.Event.unsubscribe(jsx3.gui.Event.BEFOREUNLOAD, eval(handleBrowserClose)); 
}

function handleBrowserClose(objEvent){
  var nameSpace = PNameSpace.getEnv("NAMESPACE");
  if(nameSpace != "IPVSAdminUI" && nameSpace != "IPVSClientUI"){
    objEvent.returnValue = "This will end your Cinema Collaboration Session";
  }
  setSessionPreferences(PNameSpace, "REFRESHED", "TRUE");
}

function browserClose(){
  unSubscribeBrowserClose();
  parent.window.close();
}

function getJsxByName(BlkName) {
  var BlkJSX = null;
  if (BlkName != null) {
    if (BlkName == null) BlkName = BlkName;
    // get JSX handle from blkname
    if (BlkJSX == null) {
      // allow an array to specify hierarchy
      // CAVEAT: Not all javascript strings are 'String'
      // when in doubt - caller should use array form of input
      if (typeof BlkName == 'string') {
        BlkJSX = PNameSpace.getJSXByName(BlkName);
      }
      else {
        var blkNameArr = BlkName;
        for (var i = 0; i < blkNameArr.length; i++) {
          if (typeof blkNameArr[i] != 'string') {
            BlkJSX = blkNameArr[i];
          }
          else {
            if (BlkJSX == null) {
              BlkJSX = PNameSpace.getJSXByName(blkNameArr[i]);
            }
            else {
              BlkJSX = BlkJSX.getDescendantOfName(blkNameArr[i], false, false);
            }
          }
        }
      }
    }
    return BlkJSX;
  }
}

/*** 
 checks if the node is non null - if so returns its value
 else returns the second param (which if not present will cause a null return)
 ***/
function getNodeValue(node, nullValue) {
  if (node) return node.getValue();
  return nullValue;
}


//------------------ COOKIE FNC --------------------------------------
function createCookie(name, value, days) {
  if(jsx3.util.strEmpty(days)) days = 1000;
  if (days > 0) {
    var date = new Date();
    date.setTime(date.getTime() + (days * 24 * 60 * 60 * 1000));
    var expires = "; expires=" + date.toGMTString();
  }
  else {
    var expires = "";
  }
  document.cookie = name + "=" + value + expires + "; path=/";
}

function readCookie(name) {
  var nameEQ = name + "="; 
  var ca = document.cookie.split(';');
  for (var i = 0; i < ca.length; i++) {
    var c = ca[i];
    while (c.charAt(0) == ' ') c = c.substring(1, c.length);
    if (c.indexOf(nameEQ) == 0) return c.substring(nameEQ.length, c.length);
  }
  return null;
}

function eraseCookie(name) {
  createCookie(name, "", 0);
}

//---------- help function -------------------
function promptUserSettings(varNum, varMsg) {
  var promptMsg = "";
  if (varMsg != null) {
    promptMsg = varMsg + "\n";
  }
  if (errorCode != varNum) {
    errorCode = varNum;
    alert(promptMsg + "Make sure that the following IE setting \n\"Tools->Intranet Options->Security->Custom Level->Initialize and script ActiveX controls not marked as safe\"\nmust be \"Enable\".");
    browserClose();
  }
}

//--------- associative helper function ----------
function getAssociativeArrayLength(array) {
  var length = 0;
  for (var object in array) {
    length++;
  }
  return length;
}
function getAssociativeArrLastElement(array) {
  var arrLen = getAssociativeArrayLength(array);
  var count = 0;
  var retObj = null;
  for (var object in array) {
    count++;
    if (count == arrLen) {
      retObj = object;
      break;
    }
  }
  return retObj;
}
function getAssociativeArrayFirstElement(array) {
  var arrLen = getAssociativeArrayLength(array);
  var count = 0;
  var retObj = null;
  for (var object in array) {
    if (count++ == 0) {
      retObj = object;
      break;
    }
  }
  return retObj;
}

function getAssociativeArrayByIndex(array,index) {
  var arrLen = getAssociativeArrayLength(array);
  var count = 0;
  var retObj = null;
  for (var object in array) {
    if (count++ == index) {
      retObj = object;
      break;
    }
  }
  return retObj;
}
//---------get "com.ipvs" log -------------
function handleLogMessage() {
  var logger = jsx3.util.Logger.getLogger("com.ipvs");
  return logger;
}
//-----------------------------------------
function doBrowseForFile(fieldName) {
  jsx3.require("jsx3.net.Form");
  var strFieldName = fieldName;
  var objFile = null;
  if (!objFile) {
    objFile = new jsx3.net.Form();
    objFile.addFileUploadField(strFieldName);
    objFile.subscribe(jsx3.net.Form.EVENT_FILE_SELECTED, setFieldOnFileSelected);
  }
  objFile.promptForFile(strFieldName);
}

// fills the text box with the selected file
function setFieldOnFileSelected(objEvent) {
  PNameSpace.getJSXByName(objEvent.field).setValue(objEvent.value);
}

function sec2HMS(playLength) {
  var playLengthStr = "";
  var playLength = parseInt(playLength,10);
  var OffsetSeconds = parseInt(playLength / 1000, 10);
  var OffsetHr = parseInt(OffsetSeconds / 3600, 10);
  if (OffsetHr < 10)
    OffsetHr = "0" + OffsetHr;
  var OffsetMin = parseInt(OffsetSeconds / 60, 10);
  OffsetMin = OffsetMin % 60;
  if (OffsetMin < 10)
    OffsetMin = "0" + OffsetMin;
  var OffsetSec = OffsetSeconds % 60;
  if (OffsetSec < 10)
    OffsetSec = "0" + OffsetSec;

  playLengthStr = OffsetHr + ":" + OffsetMin + ":" + OffsetSec;
  return playLengthStr;
}

function convertMMtoHrMinText(trimLength) {
  var convertedVal = "0hrs 0min";
  var trimLengthMin = parseInt(trimLength, 10) / (60 * 1000);
  if (trimLengthMin < 60) {
    convertedVal = "0hrs " + parseInt(trimLengthMin, 10) + "min";
  }
  else {
    var trimLengthHr = parseInt(trimLength, 10) / (60 * 60 * 1000);
    trimLengthHr = Math.floor(trimLengthHr)
    if (trimLengthHr >= 1 && trimLengthHr <= 24) {
      trimLengthMin = parseInt(trimLength, 10) - (parseInt(trimLengthHr, 10) * (60 * 60 * 1000));
      trimLengthMin = parseInt(trimLengthMin, 10) / (60 * 1000);

      convertedVal = parseInt(trimLengthHr, 10) + "hrs " + parseInt(trimLengthMin, 10) + "min";
    }
  }
  return convertedVal;
}

function convertMMtoHrMin(playLength) {
  var playLengthStr = "";
  var playLength = parseInt(playLength, 10);
  var OffsetSeconds = parseInt(playLength / 1000, 10);
  var OffsetHr = parseInt(OffsetSeconds / 3600, 10);
  if (OffsetHr < 10)
    OffsetHr = "0" + OffsetHr;
  var OffsetMin = parseInt(OffsetSeconds / 60, 10);
  OffsetMin = OffsetMin % 60;
  if (OffsetMin < 10)
    OffsetMin = "0" + OffsetMin;
  var OffsetSec = OffsetSeconds % 60;
  if (OffsetSec < 10)
    OffsetSec = "0" + OffsetSec;

  playLengthStr = OffsetHr + ":" + OffsetMin;
  return playLengthStr;
}

function truncatToShortForm(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = truncatFnc(matrix, cdfkey);
}

function truncatFnc(matrix, cdfkey) {
  var selRecord = matrix.getRecord(cdfkey);
  var cfgNID = selRecord.jsxCfgSessionCreated;
  cfgNID = cfgNID.substring(cfgNID.lastIndexOf(".") + 1);

  return cfgNID;
}

//---------------- MEMORY HANDLER FNC -----------------

//assumed that strcacheDocNames is comma separated string
function initMemoryHandler(objPNameSpace) {
  var blkMain = objPNameSpace.getJSXByName("blkIndex"); //replace "JSXBODY" to "blkIndex"
  var intKey = jsx3.gui.HotKey.keyDownCharToCode("d"); 
  blkMain.registerHotKey(promptUserToGenLog, intKey, false, true, true);
  var intXKey = jsx3.gui.HotKey.keyDownCharToCode("x");
  blkMain.registerHotKey(disableMemoryHandler, intXKey, false, true, true);
  /**
   * bug #40267 fix -- remove blkMain.repaint(), it will be painted naturally after index.xml loading
   * otherwise cause applet loading fail without any error for version jre 1.7.0_01 above
   **/
  // blkMain.repaint();
  }

function disableMemoryHandler() {
  var logManager = jsx3.util.Logger.Manager.getManager();
  var memoryHandler = logManager.getHandler("memory");
  memoryHandler.setLevel(jsx3.util.Logger.OFF);
}

function promptUserToGenLog() {
  dlgMsg_Create("Prompt");
}

//---------------- Get Rendered GI Component HTML Source Code-----------------
//Shift,Control,Alt
function initHtmlRenderHandler() {
  var blkMain = PNameSpace.getJSXByName("JSXBODY");
  var intKey = jsx3.gui.HotKey.keyDownCharToCode("=");
  blkMain.registerHotKey(promptUserToHtmlSrcLog, intKey, false, true, true);
}

function promptUserToHtmlSrcLog() {
  dlgMsg_Create("RenderHtml");
}
//---------------- register AOP handler -----------------

function initAOPHandler() {
  var blkMain = PNameSpace.getJSXByName("JSXBODY");
  var intKey = jsx3.gui.HotKey.keyDownCharToCode("a");
  var aopHandler = new com.ipvs.util.AOPHandler();
  blkMain.registerHotKey(aopHandler.handleAdvice, intKey, true, true, true);
}

//--------- browser sensitive functions -----------
function handleMemoryHandler() {
  var logManager = jsx3.util.Logger.Manager.getManager();
  var memoryHandler = logManager.getHandler("memory");
  var memoryHandlerRecords = memoryHandler.getRecords();
  var memoryRecordCount = memoryHandlerRecords.length;
  var recString = "---------- MEMORY LOG -----------\n\n";
  for (var recCount = 0; recCount < memoryRecordCount; recCount++) {
    recString += memoryHandlerRecords[recCount].getDate();
    recString += " - ";
    //recString += memoryHandlerRecords[recCount].getError();
    //recString += " - ";
    recString += memoryHandlerRecords[recCount].getLoggerName();
    recString += " - ";
    recString += jsx3.util.Logger.levelAsString(memoryHandlerRecords[recCount].getLevel());
    recString += " - ";
    recString += memoryHandlerRecords[recCount].getMessage().replace(/\&lt;/g, '<').replace(/\&gt;/g, '>');
    recString += "\n";
  }
  recString += "\n";

  var cacheDocCnt = cacheDocNamesArr.length;
  for (var docCnt = 0; docCnt < cacheDocCnt; docCnt++) {
    var cacheDocName = cacheDocNamesArr[docCnt];
    recString += "---------- " + cacheDocName + " -----------\n\n";
    var rootCacheNode = topic_GetRootNode(cacheDocName);
    //rootCacheNode normally shouldn't be null but set it as a defensive way 
    var topicNodeStr = !rootCacheNode ? "EMPTY\n\n" : rootCacheNode.toString().replace(/<\?.*\?>/, "").replace(/[\n\r]/g, " ").replace(/>\s+</g, "><") + "\n\n";
    recString += topicNodeStr;
  }
  var dateTimeFormat = new jsx3.util.DateFormat("MM-dd-yyyy-HH-mm-ss");
  var dateTime = dateTimeFormat.format(new Date());
  //call different browser based function  
  if (browserType == "MSIE") {
    memoryLogWriteHandler_Msie(dateTime, recString);
  }
  else if (browserType == "FIREFOX") {
    memoryLogWriteHandler_Firefox(dateTime, recString);
  }
  else {
    log.error("Current browser is not supported.");
  }
}

//-------------------- HW INFO FNC ------------------------------
//---- MAC Address
function getHWInfo_MACAddress() {
  if (browserType == "MSIE") {
    return getHWInfo_MACAddress_Msie();
  }
  else if (browserType == "FIREFOX") {
    return getHWInfo_MACAddress_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
    return null;
  }
}
//--- ComputerName
function getHWInfo_ComputerName() {
  if (browserType == "MSIE") {
    return getHWInfo_ComputerName_Msie();
  }
  else if (browserType == "FIREFOX") {
    return getHWInfo_ComputerName_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
    return null;
  }
}
//--- HDD Volume Serial number
function getHWInfo_HDVolumeSerial() {
  if (browserType == "MSIE") {
    return getHWInfo_HDVolumeSerial_Msie();
  }
  else if (browserType == "FIREFOX") {
    return getHWInfo_HDVolumeSerial_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
    return  null;
  }
}

function getHWInfo_IPAddress() {
  if (browserType == "MSIE") {
    return getHWInfo_IPAddress_Msie();
  }
  else if (browserType == "FIREFOX") {
    return getHWInfo_IPAddress_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
    return null;
  }
}

function getHWInfo_IPAddressOfMacAddress(loginInterfaceMacAddress) {
  if (browserType == "MSIE") {
    return getHWInfo_IPAddressOfMacAddress_Msie(loginInterfaceMacAddress);
  }
  else if (browserType == "FIREFOX") {
    return getHWInfo_IPAddressOfMacAddress_Firefox(loginInterfaceMacAddress);
  }
  else {
    log.error("Current browser is not supported.");
    return null;
  }
}

function getHWInfo_InterfaceList() {
  if (browserType == "MSIE") {
    return getHWInfo_InterfaceList_Msie();
  }
  else if (browserType == "FIREFOX") {
    return getHWInfo_InterfaceList_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
    return null;
  }
}
//------------------- UUID FNC -------------------------------------
Math.randRange = function(min, max) { 
    return Math.max(Math.min(Math.round(Math.random() * max), max), min);
}
function getUUID() {

  var getIntegerBits = function(val, start, end) {
      var quadArray = val.toString(16).toUpperCase().split('');
      var quadString = '';
      for(var i = Math.floor(start / 4); i <= Math.floor(end / 4); i++) {
          if(!quadArray[i] || quadArray[i] == '') quadString += '0';
          else quadString += quadArray[i];
      }
      return quadString;
  }

  var dg = new Date(1582, 10, 15, 0, 0, 0, 0).getTime();
  var dc = new Date().getTime();
  var t = (dg < 0) ? Math.abs(dg) + dc : dc - dg;
  var h = '-';
  var tl = getIntegerBits(t, 0, 31);
  var tm = getIntegerBits(t, 32, 47);
  var thv = getIntegerBits(t, 48, 59) + '1'; // version 1, security version is 2
  var csar = getIntegerBits(Math.randRange(0, 4095), 0, 7);
  var csl = getIntegerBits(Math.randRange(0, 4095), 0, 7);

  /* if nic or at least an IP can be obtained reliably, that should be put in here instead. */
  var n = getIntegerBits(Math.randRange(0, 8191), 0, 7) +
          getIntegerBits(Math.randRange(0, 8191), 8, 15) +
          getIntegerBits(Math.randRange(0, 8191), 0, 7) +
          getIntegerBits(Math.randRange(0, 8191), 8, 15) +
          getIntegerBits(Math.randRange(0, 8191), 0, 15); // this last number is two octets long
  var id = tl + h + tm + h + thv + h + csar + csl + n;
  
  return id;
}

//-------------------cmd Run/Exec/Terminate -------------------------

function cmdRun(cmd, intWinStyle, bWaitOnReturn) {
  if (browserType == "MSIE") {
    return cmdRun_Msie(cmd, intWinStyle, bWaitOnReturn);
  }
  else if (browserType == "FIREFOX") {
    return cmdRun_Firefox(cmd, intWinStyle, bWaitOnReturn);
  }
  else {
    log.error("Current browser is not supported.");
  }
}
function cmdExec(cmd) {
  if (browserType == "MSIE") {
    cmdExec_Msie();
  }
  else if (browserType == "FIREFOX") {
    cmdExec_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
  }
}
function cmdTerminate(oExec) {
  if (browserType == "MSIE") {
    cmdTerminate_Msie();
  }
  else if (browserType == "FIREFOX") {
    cmdTerminate_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
  }
}

//--- simulate to press F11 for a media player ---
function fullScreen() {
  if (browserType == "MSIE") {
    fullScreen_Msie();
  }
  else if (browserType == "FIREFOX") {
    fullScreen_Firefox();
  }
  else {
    log.error("Current browser is not supported.");
  }
}

function verifyIPAddress(IPvalue, showAlert) {
  errorString = "";
  theName = "IPaddress";

  var ipPattern = /^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/;
  var ipArray = IPvalue.match(ipPattern);

  if (IPvalue == "0.0.0.0")
    errorString = errorString + theName + ': ' + IPvalue + ' is a special IP address and cannot be used here.';
  else if (IPvalue == "255.255.255.255")
    errorString = errorString + theName + ': ' + IPvalue + ' is a special IP address and cannot be used here.';
  if (ipArray == null)
    errorString = errorString + theName + ': ' + IPvalue + ' is not a valid IP address.';
  else {
    for (i = 0; i < 4; i++) {
      thisSegment = ipArray[i];
      if (thisSegment > 255) {
        errorString = errorString + theName + ': ' + IPvalue + ' is not a valid IP address.';
        i = 4;
      }
      if ((i == 0) && (thisSegment > 255)) {
        errorString = errorString + theName + ': ' + IPvalue + ' is a special IP address and cannot be used here.';
        i = 4;
      }
    }
  }
  extensionLength = 3;
  if (errorString == "") {
    return true;
  }
  else {
    if( showAlert){
      alert(errorString);
    }
  }
  return false;
}

function validateNetmask(mask, showAlert) {
  //m[0] can be 128, 192, 224, 240, 248, 252, 254, 255
  //m[1] can be 128, 192, 224, 240, 248, 252, 254, 255 if m[0] is 255, else m[1] must be 0
  //m[2] can be 128, 192, 224, 240, 248, 252, 254, 255 if m[1] is 255, else m[2] must be 0
  //m[3] can be 128, 192, 224, 240, 248, 252, 254, 255 if m[2] is 255, else m[3] must be 0

  var flag = true;
  var correct_range = { 128: 1, 192: 1, 224: 1, 240: 1, 248: 1, 252: 1, 254: 1, 255: 1, 0: 1 };
  var m = mask.split('.');

  for (var i = 0; i <= 3; i++) {
    if (!(m[i] in correct_range)) {
      flag = false;
      break;
    }
  }

  if ((m[0] == 0) || (m[0] != 255 && m[1] != 0) || (m[1] != 255 && m[2] != 0) || (m[2] != 255 && m[3] != 0)) {
    flag = false;
  }

  if (!flag && showAlert) {
    alert("Netmask:" + mask + "is not a valid NetMask.");
    return false;
  }
  return flag;
}

function verifyMulticastIPAddress(IPvalue){
   var ipPattern = /^(\d{1,3})\.(\d{1,3})\.(\d{1,3})\.(\d{1,3})$/;
   var ipArray = IPvalue.match(ipPattern);
   var errorString = "";
   var ip = 0;
   var thisSegment = 0;
   var MulticastFromAddress = "3758096384";//224.0.0.0 is used to Multicast from address
   var MulticastToAddress = "4026531839";//239.255.255.255 is used to Multicast to address
   var reservedIPAddresses = new Array();
   reservedIPAddresses[0] = "3758096385";//224.0.0.1 is used to all Hosts multicast group that contains all systems on the same network segment
   reservedIPAddresses[1] = "3758096386";//224.0.0.2 is used to all Routers multicast group that contains all routers on the same network segment
   reservedIPAddresses[2] = "3758096389";//224.0.0.5 is used to Open Shortest Path First (OSPF) AllSPFRouters address. Used to send Hello packets to all OSPF routers on a network segment
   reservedIPAddresses[3] = "3758096390";//224.0.0.6 is used to send OSPF routing information to OSPF designated routers on a network segment
   reservedIPAddresses[4] = "3758096393";//224.0.0.9 is used to send routing information using the RIP protocol to all RIP v2-aware routers on a network segment
   reservedIPAddresses[5] = "3758096394";//224.0.0.10 is used to send EIGRP routing information to all EIGRP routers on a network segment
   reservedIPAddresses[6] = "3758096397";//224.0.0.13 is used to Protocol Independent Multicast(PIM) Version 2
   reservedIPAddresses[7] = "3758096402";//224.0.0.18 is used to Virtual Router Redundancy Protocol
   reservedIPAddresses[8] = "3758096403";//224.0.0.19 is used to IS-IS over IP
   reservedIPAddresses[9] = "3758096404";//224.0.0.20 is used to IS-IS over IP
   reservedIPAddresses[10] = "3758096405";//224.0.0.21 is used to IS-IS over IP
   reservedIPAddresses[11] = "3758096406";//224.0.0.22 is used to Internet Group Management Protocol(IGMP) Version 3
   reservedIPAddresses[12] = "3758096486";//224.0.0.102 is used to Hot Standby Router Protocol Version 2
   reservedIPAddresses[13] = "3758096635";//224.0.0.251 is used to Multicast DNS address
   reservedIPAddresses[14] = "3758096636";//224.0.0.252 is used to Link-local Multicast Name Resolution address
   reservedIPAddresses[15] = "3758096641";//224.0.1.1 is used to Network Time Protocol address
   reservedIPAddresses[16] = "3758096679";//224.0.1.39 is used to Cisco Auto-RP-Announce address
   reservedIPAddresses[17] = "3758096680";//224.0.1.40 is used to Cisco Auto-RP-Discovery address
   reservedIPAddresses[18] = "3758096681";//224.0.1.41 is used to H.323 Gatekeeper discovery address
   if (ipArray != null) {
     for (i = 0; i < 5; i++) {
       thisSegment = ipArray[i];
       //ipArray[0] value is xxx.xxx.xxx.xxx
       if( (i == 0) && (thisSegment > 255)){
         errorString = 'IPaddress' + ': '+IPvalue+' is not a valid IP address.';
         i = 4;
       }
       else if(i >= 1){
         if (thisSegment <= 255) {
           ip = (ip * 256) + (thisSegment*1);
         }
         else{
           errorString = 'IPaddress' + ': '+IPvalue+' is not a valid IP address.';
           i = 4;
         }
       }
     }
     if( ip < MulticastFromAddress || ip > MulticastToAddress ){
       errorString = 'IPaddress' + ': '+IPvalue+' is not a valid IP address.';
     }
     for(j = 0; j < reservedIPAddresses.length; j++){
      if(ip == reservedIPAddresses[j]){
        errorString = 'IPaddress' + ': '+IPvalue+' is a reserved IP address and cannot be used here.';
      }
     }
   }
   else{
      errorString = 'IPaddress' + ': '+IPvalue+' is not a valid IP address.';
   }
   if (errorString == ""){
     return true;
   }
   else{
     alert(errorString);
     return false;
   }
}

function ipAddress2num(ipaddress) {
  var d = ipaddress.split('.');
  return ((((((+d[0])*256)+(+d[1]))*256)+(+d[2]))*256)+(+d[3]);
}

function windowAlert(blkJSX, message){
  if(blkJSX != null){
    if(!jsx3.util.strEmpty(jsx3.gui.Window)){
      //check if this is calling from child window(native code) with same window context
      if(jsx3.util.strEmpty(blkJSX.getAncestorOfType(jsx3.gui.Window))){
        return blkJSX._jsxwindow.alert(message);
      }
      else{
        //check if this is calling from parent window(common code) with child window context
        return blkJSX.getAncestorOfType(jsx3.gui.Window)._jsxwindow.alert(message);
      }
    }
    else{
      return alert(message);
    }
  }
}

function windowConfirm(blkJSX, message){
  if(blkJSX != null){
    if(!jsx3.util.strEmpty(jsx3.gui.Window)){
      if(jsx3.util.strEmpty(blkJSX.getAncestorOfType(jsx3.gui.Window))){
        return blkJSX._jsxwindow.confirm(message);
      }
      else{
        return blkJSX.getAncestorOfType(jsx3.gui.Window)._jsxwindow.confirm(message);
      }
    }
    else{
      return confirm(message);
    }
  }
}


function bitsToSize(bits) {
  var sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
  if (bits == 0) return '0';
  var bytes = parseInt(bits/8, 10);
  var i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)), 10);
  if (i == 0) return bytes + ' ' + sizes[i];
  return (bytes / Math.pow(1024, i)).toFixed(2) + ' ' + sizes[i];
}

function bytesToSize(bytes) {
  var sizes = ['Bytes', 'KB', 'MB', 'GB', 'TB'];
  if (bytes == 0) return '0';
  var i = parseInt(Math.floor(Math.log(bytes) / Math.log(1024)), 10);
  if (i == 0) return bytes + ' ' + sizes[i];
  return (bytes / Math.pow(1024, i)).toFixed(2) + ' ' + sizes[i];
}



function unSerializeTwoDotXMedia_GetLength(strSerialized) {
  strSerialized = strSerialized.substring(2);
  var length = Number(strSerialized.substr(0, strSerialized.indexOf(':')));
  return length;
}


function unSerializeTwoDotXMedia(serializedStr) {
  var length = 0;
  switch (serializedStr.charAt(0)) {
    /**
    * Array
    */ 
    case 'a':
      length = unSerializeTwoDotXMedia_GetLength(serializedStr);
      serializedStr = serializedStr.substr(String(length).length + 4);

      var arr = new Array();
      var key = null;
      var value = null;

      for (var i = 0; i < length; ++i) {
        key = unSerializeTwoDotXMedia(serializedStr);
        serializedStr = key[1];

        value = unSerializeTwoDotXMedia(serializedStr);
        serializedStr = value[1];

        arr[key[0]] = value[0];
      }

      serializedStr = serializedStr.substr(1);
      return [arr, serializedStr];
      break;

    /**
    * Objects
    */ 
    case 'O':
      length = unSerializeTwoDotXMedia_GetLength(serializedStr);
      var classname = String(serializedStr.substr(String(length).length + 4, length));

      serializedStr = serializedStr.substr(String(length).length + 6 + length);
      var numProperties = Number(serializedStr.substring(0, serializedStr.indexOf(':')))
      serializedStr = serializedStr.substr(String(numProperties).length + 2);

      var obj = new Object();
      var property = null;
      var value = null;

      for (var i = 0; i < numProperties; ++i) {
        key = unSerializeTwoDotXMedia(serializedStr);
        serializedStr = key[1];

        // Handle private/protected
        key[0] = key[0].replace(new RegExp('^\x00' + classname + '\x00'), '');
        key[0] = key[0].replace(new RegExp('^\x00\\*\x00'), '');

        value = unSerializeTwoDotXMedia(serializedStr);
        serializedStr = value[1];

        obj[key[0]] = value[0];
      }

      serializedStr = serializedStr.substr(1);
      return [obj, serializedStr];
      break;

    /**
    * Strings
    */ 
    case 's':
      length = unSerializeTwoDotXMedia_GetLength(serializedStr);
      return [String(serializedStr.substr(String(length).length + 4, length)), serializedStr.substr(String(length).length + 6 + length)];
      break;

    /**
    * Integers and doubles
    */ 
    case 'i':
    case 'd':
      var num = Number(serializedStr.substring(2, serializedStr.indexOf(';')));
      return [num, serializedStr.substr(String(num).length + 3)];
      break;

    /**
    * Booleans
    */ 
    case 'b':
      var bool = (serializedStr.substr(2, 1) == 1);
      return [bool, serializedStr.substr(4)];
      break;

    /**
    * Null
    */ 
    case 'N':
      return [null, serializedStr.substr(2)];
      break;

    /**
    * Unsupported
    */ 
    case 'o':
    case 'r':
    case 'C':
    case 'R':
    case 'U':
      alert('Error: Unsupported PHP data type found!');

      /**
      * Error
      */
    default:
      return [null, null];
      break;
  }
}