/*********************************************************************
  Separate the following ActiveXObject functions from commonUtilFnc.js
 *********************************************************************/

//---------------------- MEMORY HANDLER FNC --------------------

function memoryLogWriteHandler_Msie(dateTime, recString) {
  try {
    var WshShell = new ActiveXObject("WScript.Shell");
    var strDesktop = WshShell.SpecialFolders("Desktop");
    var logFileName = "IPVSLog-" + dateTime + ".txt";
    var FSO = new ActiveXObject("Scripting.FileSystemObject");
    var theFile = FSO.OpenTextFile(strDesktop + '\\' + logFileName, 2, true);
    theFile.write(recString);
    theFile.close();
    alert("Logfile was created: " + strDesktop + "\\" + logFileName);
  }
  catch (e) {
    promptUserSettings(e.number, "Writing Logfile failed : " + e.description);
  }
}

//-------------------- REGISTRY FNC ---------------------------------
var key = "HKEY_LOCAL_MACHINE\\SOFTWARE\\IpVideoSystems\\";
var errorCode = 0;
function registry_Write(name, value) {
  try {
    Sh = new ActiveXObject("WScript.Shell"); 
    try {      
      Sh.RegWrite(key + name, value);
      return true;
    }
    catch(err) {
      log.warn("Registry WRITE:" + err.description);
      return false;
    }
  }
  catch(e) {
    log.warn("Registry WRITE:" + e.description);
    promptUserSettings(e.number);
    return false;
  }
}

function registry_Read(name) {
  try {
    Sh = new ActiveXObject("WScript.Shell");
    //very first time there is no registry keys to read
    // it will return error if we try to read those keys
    try {
      var value = Sh.RegRead(key + name);
      return value;
    }
    catch(er) {
      log.warn("Registry READ:" + er.description);
      return null;
    }
  }
  catch(e) {
    log.warn("Registry READ:" + e.description);
    promptUserSettings(e.number);
    return null;
  } 
}

function registry_Delete(name) {
  try {
    Sh = new ActiveXObject("WScript.Shell");
    //very first time there is no registry keys to delete
    // it will return error if we try to delete those keys
    try {      
      Sh.RegDelete(key + name);
      return true;
    }
    catch(err) {
      log.warn("Registry DELETE:" + err.description);
      return false;
    }
  }
  catch(e) {
    log.warn("Registry DELETE:" + e.description);
    promptUserSettings(e.number);
    return false;
  }
}

//-------------------- HW INFO FNC ------------------------------
//---- MAC Address
function getHWInfo_MACAddress_Msie() {
  
  try {
    var valMAC = null;
    var locator = new ActiveXObject("WbemScripting.SWbemLocator");
    var service = locator.ConnectServer(".");
    var properties = service.ExecQuery("SELECT * FROM Win32_NetworkAdapterConfiguration");
    var e = new Enumerator (properties);
    for (;!e.atEnd();e.moveNext())
    {
      var p = e.item();
      if( p.MACAddress != null ) {
        valMAC = p.MACAddress;
        break;
      }   
    }
    return valMAC;
  }
  catch(e) {
    log.warn("GetHWInfo - MACAddress:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}
//--- ComputerName
function getHWInfo_ComputerName_Msie() {
  try {
    Nw = new ActiveXObject("WScript.Network"); 
    var name = Nw.ComputerName;
    return name;
  }
  catch(e) {
    log.warn("GetHWInfo - ComputerName:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}
//--- HDD Volume Serial number
function getHWInfo_HDVolumeSerial_Msie() {
  try {
    var fso = new ActiveXObject("Scripting.FileSystemObject");
    // d = fso.GetDrive(fso.GetDriveName(fso.GetAbsolutePathName(drvpath)));
    d = fso.GetDrive("C");
    return d.SerialNumber;
  }
  catch(e) {
    log.warn("GetHWInfo - HD Volume Serial:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}

function getHWInfo_IPAddress_Msie() {
  try {
    var ip = "";
    //var wmi = GetObject("winmgmts:!\\\\.\\root\\cimv2");
    //var service = GetObject("winmgmts:{impersonationLevel=Impersonate}!\\\\.\\root\\cimv2");
    
    var locator = new ActiveXObject("WbemScripting.SWbemLocator");
    var service = locator.ConnectServer(".");
    
    var nac = service.ExecQuery("Select * from Win32_NetworkAdapterConfiguration Where IPEnabled = True");
    for (nac = new Enumerator(nac); !nac.atEnd(); nac.moveNext()) {
      ip = nac.item().ipAddress(0);
      // exit after the first found ip
      if( ip != "" && ip != "0.0.0.0" ) break;
    }  
    return ip;
  }
  catch(e) {
    log.warn("GetHWInfo - IPAddress:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}

//--- get the list of interfaces along with the MAC address
function getHWInfo_InterfaceList_Msie() {
  try {
    var interfaceObjArr = new Object;

    //getting the vpn connection, wireless adapters and Ethernet adapters if any.
    //parsing the ipconfig /all command output and looking for PPP Adapter(vpn), Ethernet adapter (lan wired), Wireless LAN adapter(wireless adapters)
    // mac address under Vista and win 7 is empty for vpn connections so we will put 00:00:00:00:00:00
    var interfaceObjArr = getHWInfo_InterfacesInfo_Msie();

    return interfaceObjArr;
  }
  catch (e) {
    log.warn("GetHWInfo - InterfaceList:" + e.description);
    return null;
  }
}

//--- get the IP address of the given MAC address
function getHWInfo_IPAddressOfMacAddress_Msie(macAddress) {
  try {
    var ip = "";
    var interfaceObjArr = getHWInfo_InterfacesInfo_Msie();
    for (var interfaceObj in interfaceObjArr) {
      if (macAddress == interfaceObjArr[interfaceObj].MACADDRESS) {
        ip = interfaceObjArr[interfaceObj].IPADDRESS;
        break;
      }
    }

    //if by chance the interface is down then select the first available interface
    if (jsx3.util.strEmpty(ip) || ip == "0.0.0.0") {
      ip = getHWInfo_IPAddress_Msie();
    } 
    return ip;
  }
  catch (e) {
    log.warn("GetHWInfo - IPAddressOfMacAddress:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}

function getHWInfo_InterfacesInfo_Msie() {
    try {
        var interfacesObjArr = getHWInfo_InterfacesInfo_Msie_en(),
            emptyProperties = true;
        for (var obj in interfacesObjArr) {
            emptyProperties = false;
            break;
        }
        if (emptyProperties) {
            interfacesObjArr = getIpMacAddressByWmi();
        }
        return interfacesObjArr;
    }
    catch (e) {
        log.error("error occurs when getHWInfo_InterfacesInfo_Msie:" + e.description);
    }
    
}

function getIpMacAddressByWmi() {
    try {
        var valMac = '', valIp = '', interfacesObjArr = {};
        var locator = new ActiveXObject("WbemScripting.SWbemLocator");
        var service = locator.ConnectServer(".");
        var properties = service.ExecQuery("Select * from Win32_NetworkAdapterConfiguration Where IPEnabled = True");
        var e = new Enumerator(properties);
        for (; !e.atEnd(); e.moveNext()) {
            var p = e.item();
            if (p.MACAddress) {
                valIp = p.ipAddress(0);
                valMac = p.MACAddress;
                interfacesObjArr[valMac] = { "MACADDRESS": valMac,
                                             "DESCRIPTION": valIp,
                                             "IPADDRESS": valIp
                                           };
            }
        }
       
        return interfacesObjArr;
    }
    catch (e) {
        log.error("error occurs when getIpMacAddressByWMI:" + e.description);
    }
}

//get the Interfaces Info
function getHWInfo_InterfacesInfo_Msie_en() {
  
  //helper function to extract the IP address
  function extractIPAddrress(ipStrLine) {
    var Line = ipStrLine.replace(/[\r\n\t]/g, "");
    var ipAddress = Line.substring(Line.indexOf(":") + 1);
    //in case of vista and win 7 ipaddress is follwoed by "(Preferred)" string
    if (ipAddress.indexOf("(") != -1) {
      ipAddress = ipAddress.substring(0, ipAddress.indexOf("("));
    }
    return ipAddress;
  } // end extractIP

  //helper function to extract the mac address
  function extractMac(macStrLine) {
    var Line = macStrLine.replace(/[\r\n\t]/g, "");
    var mac = Line.substring(Line.indexOf(":") + 1);
    if (jsx3.util.strEmpty(mac) || mac == " ") {
      mac = "00:00:00:00:00:00";
    }
    else {
      mac = mac.replace(/-/g, ":");
    }
    return mac;
  } // end extractMac

  //helper function to extract the desc
  function extractDesc(descStrLine) {
    var Line = descStrLine.replace(/[\r\n\t]/g, "");
    var desc = "";
    if (strLine.search("PPP adapter") != -1) {
      desc = Line.substring(12, Line.indexOf(":"));
    }
    else if (strLine.search("Ethernet adapter") != -1) {
    desc = Line.substring(17, Line.indexOf(":"));
    }
    else if (strLine.search("Wireless LAN adapter") != -1) {
    desc = Line.substring(21, Line.indexOf(":"));
  }

    return desc;
  } // end extractDesc

  // main code
  var interfacesObjArr = new Object;
  try {
    var FSO = new ActiveXObject("Scripting.FileSystemObject");
    var ProxyWshShell = new ActiveXObject("WScript.Shell");
    var path = ProxyWshShell.ExpandEnvironmentStrings("%TEMP%\\ipvs.txt");
    var cmd = "%comspec% /c ipconfig /all";
    var cmdLine = cmd + " > " + path;
    var cmdExecResult = cmdRun_Msie(cmdLine, 0, true);
    
    var objStdOutFile = FSO.OpenTextFile(path, 1);
    while (!objStdOutFile.AtEndOfStream) {
      var strLine = objStdOutFile.ReadLine();
      if (strLine.search("PPP adapter") != -1) {
        var descLine = strLine;
        var mac = null;
        var ip = null;
        //for the next 9 lines extract the IP address and macaddress
        for (var i = 0; i < 8; i++) {
          var subStrLine = objStdOutFile.ReadLine();
          if (subStrLine.search("Media disconnected") != -1) {
            //read th next 2 lines
            subStrLine = objStdOutFile.ReadLine();
            subStrLine = objStdOutFile.ReadLine();
            i = 9;
          }
          //for XP
          if (subStrLine.search("IP Address") != -1) {
            ip = extractIPAddrress(subStrLine);
          }
          //for Vista and Win7
          if (subStrLine.search("IPv4 Address") != -1) {
            ip = extractIPAddrress(subStrLine);
          }
          //for macaddress
          if (subStrLine.search("Physical Address") != -1) {
            mac = extractMac(subStrLine);
          }
        }
        //extract the description from the line
        var desc = extractDesc(descLine);
        if(!jsx3.util.strEmpty(ip) || ip != " ") {
          interfacesObjArr[mac] = { "MACADDRESS": mac,
                                   "DESCRIPTION": desc,
                                   "IPADDRESS": ip
                                 };
        }
      } //end if PPP Addapter

      // for the Ethernet adapters. In XP wireless adapters are also ethernet
      if (strLine.search("Ethernet adapter") != -1) {
        var descLine = strLine;
        var mac = null;  //
        var ip = null;
        //for the next 8 lines extract the IP address and macaddress
        for (var i = 0; i < 8; i++) {
          var subStrLine = objStdOutFile.ReadLine();
          if (subStrLine.search("Media disconnected") != -1) {
            //read th next 2 lines
            subStrLine = objStdOutFile.ReadLine();
            subStrLine = objStdOutFile.ReadLine();
            i = 9;
          }          
          //for XP
          if (subStrLine.search("IP Address") != -1) {
            ip = extractIPAddrress(subStrLine);
          }
          //for Vista and Win7
          if (subStrLine.search("IPv4 Address") != -1) {
            ip = extractIPAddrress(subStrLine);
          }
          //for macaddress
          if (subStrLine.search("Physical Address") != -1) {
            mac = extractMac(subStrLine);
          }
        }
        //extract the description from the line
        var desc = extractDesc(descLine);
        if (!jsx3.util.strEmpty(ip) && ip != " ") {
          interfacesObjArr[mac] = { "MACADDRESS": mac,
                                    "DESCRIPTION": desc,
                                    "IPADDRESS": ip
                                  };
        }
      } // end if Ethernet adapter

      // for the Wireless adapters for vista and win7
      if (strLine.search("Wireless LAN adapter") != -1) {
        var descLine = strLine;
        var mac = null;  //
        var ip = null;
        //for the next 8 lines extract the IP address and macaddress
        for (var i = 0; i < 8; i++) {
          var subStrLine = objStdOutFile.ReadLine();
          if (subStrLine.search("Media disconnected") != -1) {
            //read th next 2 lines
            subStrLine = objStdOutFile.ReadLine();
            subStrLine = objStdOutFile.ReadLine();
            i = 9;
          } 
          //for XP
          if (subStrLine.search("IP Address") != -1) {
            ip = extractIPAddrress(subStrLine);
          }
          //for Vista and Win7
          if (subStrLine.search("IPv4 Address") != -1) {
            ip = extractIPAddrress(subStrLine);
          }
          //for macaddress
          if (subStrLine.search("Physical Address") != -1) {
            mac = extractMac(subStrLine);
          }
        }
        //extract the description from the line
        var desc = extractDesc(descLine);
        if (!jsx3.util.strEmpty(ip) || ip != " ") {
          interfacesObjArr[mac] = { "MACADDRESS": mac,
                                    "DESCRIPTION": desc,
                                    "IPADDRESS": ip
                                  };
        }
      }
    } // end if wireless adapter
    objStdOutFile.Close();
  }
  catch(e){
  
  }

  return interfacesObjArr;
}

//------------------- UUID FNC -------------------------------------
// Not used currently handeled by getUUID() for both MSIE and FF
function getUUID_Msie() {
  try {
    var guid = new ActiveXObject("Scriptlet.TypeLib").guid.substring(1, 37);
    return guid;
  }
  catch(e) {
    log.warn("GetUUID:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}

//-------------------cmd Run/Exec/Terminate -------------------------

function cmdRun_Msie(cmd, intWinStyle, bWaitOnReturn) {
  try
  {
    //create a Windows Shell handler
    var ProxyWshShell = new ActiveXObject("WScript.Shell");
    var oRun = ProxyWshShell.Run(cmd, intWinStyle, bWaitOnReturn);
    ProxyWshShell.Quit;
    return oRun;
  }
  catch (e) {
    log.warn("cmd Run:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}
function cmdExec_Msie(cmd) {
  try
  {
    //create a Windows Shell handler
    var ProxyWshShell = new ActiveXObject("WScript.Shell");
    var oExec = ProxyWshShell.Exec(cmd);
    ProxyWshShell.Quit;
    return oExec;
  }
  catch (e) {
    log.warn("cmd Exec:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}
function cmdTerminate_Msie(oExec) {
  try
  {
    //create a Windows Shell handler
    oExec.Terminate();
    return true;
  }
  catch (e) {
    log.warn("cmd Exec Terminate:" + e.description);
    promptUserSettings(e.number);
    return null;
  }
}
//simulate to press F11 for a media player
function fullScreen_Msie() {
  var WshShell = new ActiveXObject("WScript.Shell");
  WshShell.SendKeys("{F11}");
  log.info("commonUtilFnc::fullScreen:SendKeys F11");
}

//write preferences file to local drive
function prefFileWrite_Msie(objPNameSpace, preferencesFilePath, preferencesXMLNode) {
  var theFile = null;
  try {
    var passContentStr = preferencesXMLNode.toString();
    var FSO = new ActiveXObject("Scripting.FileSystemObject");
    //pass giStartPath 
    giStartPath = giStartPath.replace(/%20/g, " ");
    if (objPNameSpace == "GLOBAL") {
      var theFilePath = giStartPath + "\\" + preferencesFilePath + preferencesFile;
      theFile = FSO.OpenTextFile(theFilePath, 2, true);
    }
    theFile.write(passContentStr);
    theFile.close();
  } catch (e) {
    promptUserSettings(e.number, "Preferences.xml failed to write to " + giStartPath + ": " + e.description );
  }
}

//using clientdata.xsl to upgrade Preferences.xml
function chkForClientDataUpgrade_Msie(serviceVersion, fileSysObj) {

  var ClientDataFilePath = giStartPath + "/JSXAPPS/IPVS-Common/xml/defaults/Preferences.xml";
  var ClientDataXMLNode = loadXMLFile(ClientDataFilePath);
  if (ClientDataXMLNode == null) return;
  var clientDataVersionStr = ClientDataXMLNode.selectSingleNode("//Preferences").getAttribute("serviceVer");
  //migration directory start from "1.0"
  clientDataVersionStr = jsx3.util.strEmpty(clientDataVersionStr) ? "1.0" : clientDataVersionStr; 
  var clientDataVersion = getVerNumberByStr(clientDataVersionStr);

  var FSO = fileSysObj;
  if (serviceVersion == clientDataVersion) {
    if (FSO != null) {
      FSO.DeleteFile(ClientDataFilePath + ".tmp", true);
      FSO = null;
    }
    return;
  }

  //first time FSO will be null
  //creat FSO and creat backup for Preferences
  if (FSO == null) {
    try {
      FSO = new ActiveXObject("Scripting.FileSystemObject");
      FSO.CopyFile(ClientDataFilePath, ClientDataFilePath + ".tmp", true);
    }
    catch (er) {
      log.error("INSTALLED/IE upgrade Preferences.xml failed: " + er.number + ":" + er.description);
      return;
    }
  }

  var XSLBasePath = giStartPath + "/JSXAPPS/IPVS-Common/model/migration/";
  var XSLFilePath = XSLBasePath + clientDataVersionStr + "/client/clientdata.xsl";
  var XSLFile = PNameSpace.loadInclude(XSLFilePath, "upgradeXML", "xsl");
  if (XSLFile == null) {
    log.error("INSTALLED/IE upgrade Preferences.xml failed at client version " + clientDataVersion);
    return;
  }
  var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
  var NewXML = XSLDoc.transform(ClientDataXMLNode);
 
  //get client version after upgrade 
  var prefXMLNode = (new jsx3.xml.Document()).loadXML(NewXML);
  var nextClientVersionStr = prefXMLNode.selectSingleNode("//Preferences").getAttribute("serviceVer");
  var nextClientVersion = getVerNumberByStr(nextClientVersionStr);
  
  //check migration xsl data valid (number keep growing)
  if (nextClientVersion <= clientDataVersion) {
    log.error("commonLogic:chkForClientDataUpgrade_Msie:: Bad version number in migration directory '" + nextClientVersionStr + "'.");
    throw new jsx3.Exception("commonLogic: chkForClientDataUpgrade_Msie:: Bad version number in migration directory '" + nextClientVersionStr + "'.");
  }

  try {
    var theFile = FSO.OpenTextFile(ClientDataFilePath, 2, true);
    theFile.write(NewXML.toString());
    theFile.close();
    theFile = null;
  }
  catch (er) {
    log.error("INSTALLED/IE upgrade Preferences.xml failed at : " + er.number + ":" + er.description);
    FSO.CopyFile(ClientDataFilePath + ".tmp", ClientDataFilePath, true);
    FSO.DeleteFile(ClientDataFilePath + ".tmp", true);
    return;
  }

  chkForClientDataUpgrade_Msie(serviceVersion, FSO);
}

//fix bug 38694 -- incorrect DPI setting for UMC
function readDPISetting() {
  try {
    Sh = new ActiveXObject("WScript.Shell");
    try {
      var value = Sh.RegRead("HKCU\\Control Panel\\Desktop\\WindowMetrics\\AppliedDPI");
      return value;
    }
    catch (er) {
      log.warn("Can't read registry HKCU\\Control Panel\\Desktop\\WindowMetrics\\AppliedDPI: " + er.description);
      return null;
    }
  }
  catch (e) {
    log.warn("Can't create WScript.Shell, " + e.description);
    return null;
  }
}