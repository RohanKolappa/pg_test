// ---------------------- Date time FNC --------------------
function getDateTimeFromTimeCode(DateTimeCode, DateFormat) {
  if(jsx3.util.strEmpty(DateFormat)) {
    DateFormat = "MM/dd/yyyy HH:mm";
  }
  var dateTimeFormat = new jsx3.util.DateFormat(DateFormat);
  return dateTimeFormat.format(DateTimeCode);
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
    var objNode = PNameSpace.loadInclude(XMLFilePath, null, "xml", true);
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
  unloadApplet();
}

function browserClose() {
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
  var playLength = parseInt(playLength);
  var OffsetSeconds = parseInt(playLength / 1000);
  var OffsetHr = parseInt(OffsetSeconds / 3600);
  if (OffsetHr < 10)
    OffsetHr = "0" + OffsetHr;
  var OffsetMin = parseInt(OffsetSeconds / 60);
  OffsetMin = OffsetMin % 60;
  if (OffsetMin < 10)
    OffsetMin = "0" + OffsetMin;
  var OffsetSec = OffsetSeconds % 60;
  if (OffsetSec < 10)
    OffsetSec = "0" + OffsetSec;

  playLengthStr = OffsetHr + ":" + OffsetMin + ":" + OffsetSec;
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
function initMemoryHandler(objPNameSpace, strcacheDocNames) {
// all topics are dynamically added to this arrary now
// no need for strcacheDocNames here - if used need to eval before insert in array
//  cacheDocNamesArr = strcacheDocNames.split(",");
  var blkMain = objPNameSpace.getJSXByName("JSXBODY");
  var intKey = jsx3.gui.HotKey.keyDownCharToCode("d"); 
  blkMain.registerHotKey(promptUserToGenLog, intKey, false, true, true);
  var intXKey = jsx3.gui.HotKey.keyDownCharToCode("x"); 
  blkMain.registerHotKey(disableMemoryHandler, intXKey, false, true, true);
  blkMain.repaint();
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
function handleMemoryHandler(browserType) {
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
function getHWInfo_MACAddress(browserType) {
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
function getHWInfo_ComputerName(browserType) {
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
function getHWInfo_HDVolumeSerial(browserType) {
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

function getHWInfo_IPAddress(browserType) {
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

function getHWInfo_IPAddressOfMacAddress(loginInterfaceMacAddress, browserType) {
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

function getHWInfo_InterfaceList(browserType) {
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

function cmdRun(cmd, intWinStyle, bWaitOnReturn, browserType) {
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
function cmdExec(cmd, browserType) {
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
function cmdTerminate(oExec, browserType) {
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
function fullScreen(browserType) {
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

function utilRuleGetOutboundDoc(RuleName) {
  try {
    var objService = PNameSpace.loadResource(RuleName + "_xml");
    objService.setOperation("");
    objService.caller = this;
    objService.requestHandle = this;
    var ruleDoc = objService.getServiceMessage();
    return ruleDoc;
  }
  catch(e) {
    alert( "Internal Error: Outbound Map Rule: " + RuleName + "\n" );
    return null;
  }    
}