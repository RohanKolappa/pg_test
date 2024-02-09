var LastLoginState = "NONE";
function cmpBlkLogin_Create(BlkName, 
                            TxtUser, TxtPassword, TxtDomain, 
                            BlkAutoLogin, ChkAutoLogin,
                            BlkLoginResource, TxtLoginResource,
                            BlkLoginResourcePassword, RadioAutoPassword, RadioManualPassword, TxtLoginResourcePassword,
                            BlkInterface, DropListLoginInterface, TxtLoginInterfaceIP, BtnAdvanceSettings, BlkAdvanceSettings,
                            BlkLoginServer, TxtLoginServer,
                            TxtMessage,
                            BtnLogin, BtnCancelRetryLogin, userNameDropList, blkCancel) {

  // Trigger Login Action for EnterKey
  subscribeEnterKeyAction();  
  // this presence subscription is to check for duplicate login
  // if the same user login with the diff media client then the 1st
  // login(user) will be logged out. This subscription is set in 
  //commonNotificationfnc.js function handleNotification_XMPPEvent(notificationData)
  // logout will be published when user are same and presence state is true
  eventApp.subscribe("PRESENCE", null, this,
                      function(EventVar, EventVal) {
                        var presence = EventVal;
                        var roomNID = presence.RoomNID;
                        if( roomNID == "NONE" ) {
                          var PresenceState = presence.State;
                          var PresenceFrom = presence.JID;
                          var UserFromName = PresenceFrom.substring(0, PresenceFrom.indexOf("@"));
                          var ResourceFrom = PresenceFrom.substring(PresenceFrom.indexOf("/")+1);
                          var PresenceTo = topic_Get(tApp, "LoginUserJID");
                          var UserToName = PresenceTo.substring(0, PresenceTo.indexOf("@"));
                          var ResourceTo = PresenceTo.substring(PresenceTo.indexOf("/")+1);
                          if( (PresenceState) && (UserFromName == UserToName) ) {
                            log.warn("PRESENCE: Duplicate Login: LOGOUT PUBLISH");
                            IpvsLogin.doLogout();                          
                            
                            /*jsx3.sleep( (function(){
                                          log.warn("PRESENCE: Duplicate Login: LOGOUT PUBLISH");
                                          topic_Publish(tApp, "LoginState", "LOGOUT");
                                         }
                                        ),
                                        null, this );
                            */            
                            jsx3.sleep( (function(){
                                            log.warn("PRESENCE: Duplicate Login: ALERT MSG");
                                            alert("Session Terminated: Duplicate Login\n" +
                                                  "User: '"+ UserFromName +"'\n" +
                                                  "Current Client Name: '"+ ResourceTo +"'\n" +
                                                  "Remote Client Name: '"+ ResourceFrom + "'\n"
                                                 );
                                            return; 
                                         }
                                        ),
                                        null, this );                                               
                          }
                        }
                      });

  function handleAssetPrivateData() {
    var homepageURL = getLocalPreferences(PNameSpace, ".//HomepageURL");
    if (!jsx3.util.strEmpty(homepageURL) ) {
      topic_Set(tApp, "PassPortURL", homepageURL);
    }
    var bannerImageURL = getLocalPreferences(PNameSpace, ".//BannerURL");
    if (!jsx3.util.strEmpty(bannerImageURL )) {
      topic_Publish(tApp, "BannerImage", bannerImageURL);
    }

    //Check for Fps, UseHTTP and UseMMS in Assetprivate data.
    //if available then update in tApp
    var fpsValue = getLocalPreferences(PNameSpace, ".//Fps");
    if (!jsx3.util.strEmpty(fpsValue)) {
      topic_Set(tApp, "IPVSPlayerDefaultFps", fpsValue);
    }

    var useHttpValue = getLocalPreferences(PNameSpace, ".//UseHTTP");
    if(!jsx3.util.strEmpty(useHttpValue)) {
      topic_Set(tApp, "IPVSPlayerDefaultUseHTTP", useHttpValue);
    }

    var useMMSValue = getLocalPreferences(PNameSpace, ".//UseMMS");
    if (!jsx3.util.strEmpty(useMMSValue)) {
      topic_Set(tApp, "IPVSPlayerDefaultUseMMS", useMMSValue);
    }

    //Check for inactivity timeout
    var inActivityTimeoutPrefs = getLocalPreferences(PNameSpace, ".//InActivityTimeout");
    if (!jsx3.util.strEmpty(inActivityTimeoutPrefs)) {
      var inActivityTimeout = inActivityTimeoutPrefs * 1000; //milliseconds
      topic_Set(tApp, "InActivityTimeout", inActivityTimeout);

      var inActivityWarningTimeout = 30000;
      var inActivityWarningTimeoutPrefs = getLocalPreferences(PNameSpace,".//InActivityWarningTimeout");
      if (!jsx3.util.strEmpty(inActivityWarningTimeoutPrefs)) {
        inActivityWarningTimeout = inActivityWarningTimeoutPrefs * 1000;
      }
      topic_Set(tApp, "InActivityWarningTimeout", inActivityWarningTimeout);

      initInActivityTimeout();
    }
  }                      

  //get all the local network interfaces and show them in the drop down
  function getInterfaces() {
    var strXMLCacheId = cmpSelLoginInterface.BlkJSX.getXMLId();
    var objXML11 = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (!jsx3.util.strEmpty(objXML11)) {
      objXML11.removeChildren();
    }
    // add the default name
    var o = new Object();
    o.jsxid = "DefaultInterface";
    o.jsxtext = "Select Interface";
    o.jsxIPAddress = "DefaultIPAddress";
    cmpSelLoginInterface.BlkJSX.insertRecord(o, null, false);
    var strRecordid = "DefaultInterface";    
    var interfaceObjArr = getHWInfo_InterfaceList();
    for (var interfaceObj in interfaceObjArr) {
      if (!jsx3.util.strEmpty(interfaceObjArr[interfaceObj].MACADDRESS) && !jsx3.util.strEmpty(interfaceObjArr[interfaceObj].DESCRIPTION) && !jsx3.util.strEmpty(interfaceObjArr[interfaceObj].IPADDRESS)) {
        var o = new Object();
        o.jsxid = interfaceObjArr[interfaceObj].MACADDRESS;
        o.jsxtext = interfaceObjArr[interfaceObj].DESCRIPTION;
        o.jsxIPAddress = interfaceObjArr[interfaceObj].IPADDRESS;
        cmpSelLoginInterface.BlkJSX.insertRecord(o, null, false);
      }
    }
    
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    log.info("cmpBlkLogin::getLocalInterfaces:: cmpSelLoginInterface::XML:" + objXML);    

    var interfaceMACAddress = topic_Get(tApp, "LoginInterfaceMacAddress");
    if (!jsx3.util.strEmpty(interfaceMACAddress)) {
      if (!jsx3.util.strEmpty(objXML)) {
        var record = objXML.selectSingleNode("//record[@jsxid='" + interfaceMACAddress + "']");
        //if the record exisit then the network connection is up else connection is down
        if (!jsx3.util.strEmpty(record)) {
          strRecordid = interfaceMACAddress;
          //get the ip address
          var ipaddress = getHWInfo_IPAddressOfMacAddress(strRecordid);
          cmptxtLoginInterfaceIP.BlkJSX.setValue(ipaddress, true);
        }
      }
    }
    //set the select
    cmpSelLoginInterface.BlkJSX.setValue(strRecordid);
    cmpSelLoginInterface.BlkJSX.repaint();
    topic_Set(tApp, "InitPrefInterface", "TRUE");
  }
  
  eventApp.subscribe("AppletStatus", null, this,
                      function(EventVar, EventVal) {
                        var appletStatus = EventVal;
                        if (appletStatus == "LOADED") {
                          topic_Publish(tApp, "LoginStatus", "Loaded ");
                          handleFieldsState(jsx3.gui.Form.STATEENABLED);
                          IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_SERVICEVERSION, topic_Get(tApp, "ServiceVersion"));
                          IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_SOFTWAREVERSION, topic_Get(tApp, "BuildVersion"));
                          IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_CLIENTIPADDRESS, topic_Get(tApp, "ClientIPAddress"));
                          var bAutoLogin = getLocalPreferences(PNameSpace, ".//Login/AutoLogin", "false");
                          cmpChkAutoLogin.BlkJSX.setValue(bAutoLogin);                           
                          if (topic_Get(tApp, "AutoLogin") == "TRUE" || topic_Get(tApp, "PCClientAutoLogin") == "ENABLED") {
                            cmpTxtLoginPwd.BlkJSX.setValue(IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_PASSWORD));
                            cmpBtnLogin.onSelect.call(cmpBtnLogin);
                          }
                          else if (topic_Get(tApp, "AutoLogin") == "FALSE") {
                            alert("'Auto Login' need more parameters.");
                          }

                         jsx3.sleep(function () {
                            topic_Publish(tApp, "AppletLoaded", "DONE");
                            if(IpvsLogin.getStateParam(com.ipvs.api.Login.RETRYLOGINSTATE) != com.ipvs.api.Login.RETRYLOGIN_INPROGRESS) {
                              topic_Publish(tApp, "LoginStatus", " ");
                            }
                            if(IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) ==  com.ipvs.api.Login.LOGIN_NONE && IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_ERROR) {
                              if(!jsx3.util.strEmpty(cmpLoginUser.BlkJSX.getValue())){
                                setTimeout(function() { cmpTxtLoginPwd.BlkJSX.focus(); }, 1800);
                              }
                              else{
                                cmpLoginUser.BlkJSX.focus();
                              }
                              return;
                            }
                          }, null, this);                            
                        }
                        else{  
                          topic_Publish(tApp, "LoginStatus", EventVal);
                        }
                      }
                    );     
                                          
  // setup login params
  //topic_Set(tApp, "LoginState", LastLoginState);

//  topic_Set(tApp, "LoginBtnState", "VALID");
//  var refreshedFlag = getSessionPreferences(PNameSpace, "REFRESHED");
//  if( refreshedFlag ) {
//    alert("  Refresh is not supported.\n\nPlease exit the application and restart.");
//    unsubscribeEnterKeyAction();
//    topic_Set(tApp, "LoginBtnState", "NONE");
//  }
  
  // init JID to blank  
  topic_Set(tApp, "LoginUserJID", "");
  topic_Set(tApp, "LoginUserDomain", "");
  topic_Set(tApp, "InitPrefInterface", "");
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_USERJID, "");
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_USERDOMAIN, "");
  
  // show the advance block by default
  topic_Set(tApp, "LoginAdvanceSettingsBlock", "OPEN");
  // LoginPreferredInterface mac address
  var loginInterfaceMacAddress = getLocalPreferences(PNameSpace, ".//Login/InterfaceMacAddress", DefaultLoginInterfaceMacAddress);
  topic_Set(tApp, "LoginInterfaceMacAddress", loginInterfaceMacAddress);  
  
  //try to get login info from settings
  // LoginUser info
  var loginUname = getLocalPreferences(PNameSpace, ".//Login/user", "");
  if(loginUname == "") {
    // if not seee if there is a last user name
    loginUname = getLocalPreferences(PNameSpace, ".//LastLoginUsername");
    if(! loginUname) {
      // if not just blank it out
      loginUname = "";
    }
  }
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_USER, loginUname);
  // LoginPassword info
  var loginPassword = getLocalPreferences(PNameSpace, ".//Login/password", "");
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_PASSWORD, loginPassword);
  // LoginDomain info
  var loginDomain = getLocalPreferences(PNameSpace, ".//Login/domain", DefaultLoginDomain);
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_DOMAIN, loginDomain);
  // LoginServer info
  var loginServer = getLocalPreferences(PNameSpace, ".//Login/LastLoginServer", DefaultLoginServer);
  if(loginServer == ""){
    loginServer = getLocalPreferencesNode(PNameSpace, ".//Login/ServerList/Server", DefaultLoginServer).getAttribute("Value");
  }
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_SERVER, loginServer);
  // make sure there is either a valid server IP or UI to get it from user
  if( BlkLoginServer == null ) {
    if( loginServer == null || loginServer == "" ) {
      alert("Invalid or missing System Settings\n\nPlease run the Settings Tool and try again");
      browserClose();
      return null;
    }
  }
  // LoginPort info
  var loginPort = getLocalPreferences(PNameSpace, ".//Login/port", DefaultLoginPort);
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_PORT, loginPort);
  // ServiceDomain info
  var serviceDomain = getLocalPreferences(PNameSpace, ".//Service/servicedomain", DefaultServiceDomain);
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_SERVICEDOMAIN, serviceDomain); 

  if(clientType != "WEB"){
    // LoginResource info
    var AssetTemplateTitle = null;
    var AssetResourceTitle = getLocalPreferences(PNameSpace, ".//AssetResourceTitle", "");

    if( AssetResourceTitle == "" ) {
      AssetResourceTitle = getLocalPreferences(PNameSpace, ".//Login/resource", DefaultLoginResource);
    }
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_CLIENTNAME, AssetResourceTitle);    
    // if not found - use default resource name as template name
    if( AssetResourceTitle == "") {
      topic_Set(tApp, "LoginAdvanceSettingsBlock", "OPEN");
      AssetTemplateTitle = getLocalPreferences(PNameSpace, ".//Login/resource", DefaultLoginResource);
      //if no UI to ask user for resource name
      if( BlkLoginResource == null ) {
        //generate a new dynamic resource name from template name
        AssetResourceTitle = AssetTemplateTitle + "-" + getHWInfo_ComputerName();
        //AssetResourceTitle = AssetTemplateTitle + "-" + getUUID();
      }
    }
    else {
      topic_Set(tApp, "LoginAdvanceSettingsBlock", "CLOSE");
    }
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE, AssetResourceTitle);
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_ASSETTEMPLATETITLE, AssetTemplateTitle);
  }
  else {
    //set cmpBlkLoginResource visible for WEB client
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_CLIENTNAME, '');
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_WEBCLIENTSERVER, topic_Get(tApp, "WebClientLoginServer"));
    var loginResource = getLocalPreferences(PNameSpace, ".//Login/resource", DefaultLoginResource);
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE, loginResource);
  }
  // generate asset private key - from system hw info
  var AssetPrivateKeyScheme = getLocalPreferences(PNameSpace, ".//AssetPrivateKeyScheme", "SERVER");
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_ASSETPRIVATEKEYSCHEME, AssetPrivateKeyScheme);
  var AssetPrivateKeyData = "";
  var autoGeneratepassword = "true";
  if(clientType != "WEB"){
    // TBD: ignore key scheme for now
  //  if( AssetPrivateKeyScheme == "MACADDRESS" ) {
  //    AssetPrivateKeyData = getHWInfo_MACAddress();
  //  }
  //  else if( AssetPrivateKeyScheme == "DRIVESERIAL" ) {
  //    AssetPrivateKeyData = getHWInfo_HDVolumeSerial();
  //  }
  //  else if (AssetPrivateKeyScheme == "SERVER") {
  //after middle ware add 'SERVER' scheme using:    AssetPrivateKeyData = getLocalPreferences(PNameSpace, ".//AssetPrivateKeyData", "");
  //  }
    
    //find out the value of AutoGenerateResourcePassword if its true then generate it other leave it it blank user will prompted to enter
    
    autoGeneratepassword = getLocalPreferences(PNameSpace, ".//Login/AutoGenerateResourcePassword", "true");
    if( autoGeneratepassword == "true"){
      AssetPrivateKeyData = getHWInfo_HDVolumeSerial();
    }
    else {
      AssetPrivateKeyData = getLocalPreferences(PNameSpace, ".//Asset/AssetPrivateKeyData", "");
      if( jsx3.util.strEmpty(AssetPrivateKeyData) ){
        AssetPrivateKeyData = getLocalPreferences(PNameSpace, ".//Login/resourcePassword", DefaultLoginResourcePassword);
      }     
    }
  }
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_AUTOGENERATEPASSWORD, autoGeneratepassword);
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_ASSETPRIVATEKEYDATA, AssetPrivateKeyData);
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE_PASSWORD, AssetPrivateKeyData);
  
  // get asset private data from local prefs
  var AssetPrivateData = getLocalPreferences(PNameSpace, ".//AssetPrivateData", "");
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_ASSETPRIVATEDATA, AssetPrivateData);
  
  //Set App Name Based on PNameSpace (to Check for Max Client Logins)
  var AppName = "";
  if(PNameSpace.getEnv("NAMESPACE")== "IPVSAdminUI"){
    AppName = "IPVSAdmin";
  }
  else  if(PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI"){
    AppName = "IPVSClient";
  }
  else if(PNameSpace.getEnv("NAMESPACE") == "OdysseyClientUI"){
    AppName = "OdysseyClient";
  }
  IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_APPNAME, AppName);
  
  // --------------- login components -----------
  // new parameter userNameDropList to check whether username is dropdown or textbox
  if( userNameDropList == "DROPLIST" ) {
    var namesObjectList = new Array();
    var i = 1;
    var userName = null;
    while( userName = getLocalPreferences(PNameSpace, ".//RecentUsersList/UserName["+i+"]") ) {
      namesObjectList[i-1] = userName;
      i++;
    }
    var cmpLoginUser = new CmpList( TxtUser,
                                    namesObjectList,
                                    {"jsxid":"username", "jsxtext":"username", "jsxvalue":"username"},
                                    null,
                                    {persist:{col:"jsxtext",initval:true}} );
  }
  else {
    var loginUser = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USER);
    var cmpLoginUser = new CmpTxtBoxWithBlur(TxtUser, loginUser, null,
                                                                        { action: "FNCCALL",
                                                                            context: this,
                                                                            fnc: function () {
                                                                                if (cmpLoginUser.BlkJSX.getValue() == "superuser-default") {
                                                                                    alert("The administrative login name 'superuser-default' has been changed to 'admin'");
                                                                                    cmpLoginUser.BlkJSX.focus();
                                                                                }
                                                                            }
                                                                        });
  }
  var cmpTxtLoginPwd = new CmpTxtBox(TxtPassword, null, null, null, 
                                    {action:"FNCCALL", 
                                     context:this, 
                                     fnc:function() {
                                           var LoginState = IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE);
                                           if( LoginState  == com.ipvs.api.Login.LOGIN_ERROR ){
                                             topic_Publish(tApp, "LoginStatus", '');
                                           }
                                         }
                                    });

  var cmpSelLoginDomain = new CmpList(TxtDomain,
                                      [ {showtxt:loginDomain}
                                      ],
                                      {"jsxtext":"showtxt"},
                                      null,
                                      {persist:{col:"jsxtext",initval:true}} );

  var cmpBlkAutoLogin = null;
  var cmpChkAutoLogin = null;
  if( BlkAutoLogin != null ){
    cmpBlkAutoLogin = new Component(BlkAutoLogin);
    
    cmpChkAutoLogin = new CmpCheckBox(ChkAutoLogin,
                                      null,
                                      {action:"FNCCALL", 
                                       context:null, 
                                       fnc:function() {
                                          
                                       }
                                      }
                                     );                                     
  }
      
  var cmpBlkLoginResource = null;
  var cmpTxtLoginResource = null;
  if( BlkLoginResource != null ) {
    cmpBlkLoginResource = new Component(BlkLoginResource);
    var loginResource = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE);
    cmpTxtLoginResource = new CmpTxtBox(TxtLoginResource, loginResource, null, null, null);
  }
  
  var cmpBlkLoginResourcePassword = null;
  var cmpBtnRadioAutoPassword = null;
  var cmpBtnRadioManualPassword = null;
  var cmpTxtLoginResourcePassword = null;
  if( BlkLoginResourcePassword != null ){
    cmpBlkLoginResourcePassword = new Component(BlkLoginResourcePassword);
    
    cmpTxtLoginResourcePassword = new Component(TxtLoginResourcePassword);
    
    cmpBtnRadioAutoPassword = new CmpRadioButton(RadioAutoPassword, null,
                                                             { action: "FNCCALL",
                                                               context: null,
                                                               fnc: function(){
                                                                  cmpTxtLoginResourcePassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                                  cmpTxtLoginResourcePassword.BlkJSX.setValue("");
                                                               }
                                                             });
                                                 
    cmpBtnRadioManualPassword = new CmpRadioButton(RadioManualPassword, null,
                                                             { action: "FNCCALL",
                                                               context: null,
                                                               fnc: function(){
                                                                  cmpTxtLoginResourcePassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                                  // commenting bcoz of bug# 34192
//                                                                  var loginResourcePassword = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_ASSETPRIVATEKEYDATA);
//                                                                  loginResourcePassword = getLocalPreferences(PNameSpace, ".//Login/resourcePassword", DefaultLoginResourcePassword);                                                                  
//                                                                  cmpTxtLoginResourcePassword.BlkJSX.setValue(loginResourcePassword);
                                                                  cmpTxtLoginResourcePassword.BlkJSX.setValue("");
                                                                  cmpTxtLoginResourcePassword.BlkJSX.focus();
                                                               }
                                                             });    
    
    
    
    
    var loginResourcePassword = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_ASSETPRIVATEKEYDATA);
    if( jsx3.util.strEmpty(loginResourcePassword) ){
      loginResourcePassword = getLocalPreferences(PNameSpace, ".//Login/resourcePassword", DefaultLoginResourcePassword);
    }    

    
    var autoGeneratePassword = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_AUTOGENERATEPASSWORD);
    if( autoGeneratePassword == "true" ){
      cmpBtnRadioAutoPassword.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
      cmpTxtLoginResourcePassword.BlkJSX.setValue("");
    }
    else {
      cmpBtnRadioManualPassword.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
      cmpTxtLoginResourcePassword.BlkJSX.setValue(loginResourcePassword);
      cmpTxtLoginResourcePassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  var cmpBlkInterface = null;
  if( BlkInterface != null ){
    cmpBlkInterface = new Component(BlkInterface);
  }

  var cmpSelLoginInterface = null;
  if( DropListLoginInterface != null ){
    cmpSelLoginInterface = new CmpSelect(DropListLoginInterface, null,
                                        { action: "FNCCALL",
                                          context: this,
                                          fnc: function() {
                                            if (topic_Get(tApp, "InitPrefInterface") != "TRUE") {
                                              log.info("cmpBlkUI::click Preferred Interface button:: calling getInterfaces():");
                                              getInterfaces();
                                            }
                                            jsx3.sleep(function () {
                                              var selVal = cmpSelLoginInterface.BlkJSX.getValue();
                                              if (jsx3.util.strEmpty(selVal)) return;
                                              var strXMLCacheId = cmpSelLoginInterface.BlkJSX.getXMLId();
                                              var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                              if (jsx3.util.strEmpty(objXML)) return;
                                              var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
                                              var ipaddress = record.getAttribute("jsxIPAddress");
                                              if (ipaddress == "DefaultIPAddress") {
                                                ipaddress = "";
                                              }
                                              cmptxtLoginInterfaceIP.BlkJSX.setValue(ipaddress, true);
                                              //save the new values
                                              // topic_Set(tApp, "ClientIPAddress", ipaddress)
                                              //IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_CLIENTIPADDRESS, ipaddress);

                                              log.info("cmpBlkUI::ONSELECT:: cmpSelLoginInterface::Selected Record:" + record);
                                            }, null, this);
                                            
                                          }
                                        });
  }
  
  var cmptxtLoginInterfaceIP = null;
  if( TxtLoginInterfaceIP != null ){
    cmptxtLoginInterfaceIP = new CmpTxtBox(TxtLoginInterfaceIP, null, null, null, null);
  }

  var cmpBtnAdvanceSettings = null;
  if (BtnAdvanceSettings != null) {
    cmpBtnAdvanceSettings = new CmpImageButton(BtnAdvanceSettings,
                                    null,
                                   { action: "FNCCALL",
                                     context: this,
                                     fnc: function() {
                                            var objBlkState = cmpBtnAdvanceSettings.BlkJSX.getState();                            
                                            if( objBlkState == "1" ) {
                                                topic_Publish(tApp, "LoginAdvanceSettingsBlock", "OPEN");
                                            }
                                            else if( objBlkState == "0" ) {
                                                topic_Publish(tApp, "LoginAdvanceSettingsBlock", "CLOSE");
                                            }
                                          }
                                   });
    var blkState = topic_Get(tApp, "LoginAdvanceSettingsBlock");
    if( blkState == "OPEN" ){
      cmpBtnAdvanceSettings.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
    }
    else {
      cmpBtnAdvanceSettings.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
    }
    
  }
  
  var cmpBlkLoginServer = null;
  var cmpCmbLoginServer = null;
  if (BlkLoginServer != null) {
    cmpBlkLoginServer = new Component(BlkLoginServer);
    var loginServer = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_SERVER);
    cmpCmbLoginServer = new Component(TxtLoginServer);
    var serverList = getLocalPreferencesNode(PNameSpace, ".//ServerList");
    if( serverList != null ) {
      var iterServers = serverList.selectNodeIterator(".//Server");
      while( iterServers.hasNext() ) {
        var nodeServer = iterServers.next();
        var serverName = nodeServer.getAttribute("Name");
        var serverValue = nodeServer.getAttribute("Value");
        if(jsx3.util.strEmpty(serverName)){
         serverName = serverValue;
        }
        var record = {jsxid:serverValue, jsxtext:serverName, jsxvalue:serverValue};
       cmpCmbLoginServer.BlkJSX.insertRecord(record);
      }
      if( jsx3.util.strEmpty(loginServer) ) {
        var firstServerNode = serverList.getFirstChild();
        loginServer =firstServerNode.getAttribute("Value");
      }
      if(!jsx3.util.strEmpty(loginServer)){
        cmpCmbLoginServer.BlkJSX.setValue(loginServer);
      }
    }
  }
  
  var cmpTxtLoginError = new CmpLabel(TxtMessage, null, 
                                      [{action:"APPEVENT", eventvar:"LoginStatus", eventval:null}], 
                                        null, "");

  var cmpLogin = PNameSpace.getJSXByName(BtnLogin);
  var cmpBtnLogin = null;
  if( cmpLogin.doExecute ) {
    cmpBtnLogin = new CmpButton(BtnLogin,
                                  null,
                                 { action: "FNCCALL",
                                     context: this,
                                     fnc: function() {
                                         IpvsLogin.cancelRetryLogin();
                                         doLogin();
                                     }
                                 });

  }
  else if( cmpLogin.doValidate ) {
    cmpBtnLogin = new CmpImageButton(BtnLogin,
                                  null,
                                 { action: "FNCCALL",
                                   context: this,
                                   fnc: function()
                                   {
                                     IpvsLogin.cancelRetryLogin();
                                     doLogin();
                                   }
                                 });


  }
  else {
    cmpBtnLogin = new CmpBlockButton(BtnLogin,
                                null,
                               { action: "FNCCALL",
                                 context: this,
                                 fnc: function()
                                 {
                                   IpvsLogin.cancelRetryLogin();
                                   doLogin();
                                 }
                               });
  }

  //Log Off
  if( BtnCancelRetryLogin != null ) {
    var cmpLogoff = PNameSpace.getJSXByName(BtnCancelRetryLogin);
    var cmpBtnCancelRetryLogin = null;

    if( cmpLogoff.doExecute ) {
      cmpBtnCancelRetryLogin = new CmpButton(BtnCancelRetryLogin, 
                                null, { action: "FNCCALL",
                                context:this, 
                                fnc:function() {
                                      IpvsLogin.cancelRetryLogin();
                                    }
                               });

    }
    else if( cmpLogoff.doValidate ) {
      cmpBtnCancelRetryLogin = new CmpImageButton(BtnCancelRetryLogin, 
                                null, { action: "FNCCALL",
                                context: this,
                                fnc: function()
                                {
                                  IpvsLogin.cancelRetryLogin();
                                }
                                });


    }
    else {
      cmpBtnCancelRetryLogin = new CmpBlockButton(BtnCancelRetryLogin, 
                                null, { action: "FNCCALL",
                                context: this,
                                fnc: function()
                                {
                                  IpvsLogin.cancelRetryLogin();
                                }
                                });
    }
  }
  
  function clearCacheServiceRoot() {
    var cacheServiceRoot = topic_GetNode(tCache, "//ServiceRoot");
    if( cacheServiceRoot != null ) {
      cacheServiceRoot.removeChildren();
    }
    // clear out local login prefs
    setLocalPreferences(PNameSpace, ".//AssetResourceTitle",'');
    setLocalPreferences(PNameSpace, ".//AssetPrivateKeyScheme", '');
    setLocalPreferences(PNameSpace, ".//AssetPrivateKeyData", '');
    setLocalPreferences(PNameSpace, ".//AssetPrivateData",'');
  }

  function handleFieldsState(fldState) {
    if(!jsx3.util.strEmpty(cmpLoginUser.BlkJSX)) cmpLoginUser.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpTxtLoginPwd.BlkJSX)) cmpTxtLoginPwd.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpSelLoginDomain.BlkJSX)) cmpSelLoginDomain.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpChkAutoLogin.BlkJSX)) cmpChkAutoLogin.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpTxtLoginResource.BlkJSX)) cmpTxtLoginResource.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpCmbLoginServer.BlkJSX)) cmpCmbLoginServer.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpBtnRadioManualPassword.BlkJSX)) cmpBtnRadioManualPassword.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpBtnAdvanceSettings.BlkJSX)) cmpBtnAdvanceSettings.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
    if(!jsx3.util.strEmpty(cmpSelLoginInterface.BlkJSX)) cmpSelLoginInterface.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmptxtLoginInterfaceIP.BlkJSX)) cmptxtLoginInterfaceIP.BlkJSX.setEnabled(fldState, true);
    if(!jsx3.util.strEmpty(cmpBtnLogin.BlkJSX)) cmpBtnLogin.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
  }

  function doLogin() {
    if (!clsApplet || !clsApplet.appletHandle ) {
      alert("Initializing .. Please wait and retry")
      return false;
    }
    var valLogin = cmpLoginUser.BlkJSX.getValue().toLowerCase();
    var valPassword = cmpTxtLoginPwd.BlkJSX.getValue();
    var valSelDomain = cmpSelLoginDomain.selRecord.jsxtext;
    var valLoginResource = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE);
    var valAutoLogin = cmpChkAutoLogin.BlkJSX.getChecked();
    if( valAutoLogin == jsx3.gui.CheckBox.CHECKED){
      valAutoLogin = "true";
    }
    else {
      valAutoLogin = "false";
    }
    
    if( BlkLoginResource != null ) {
      valLoginResource = cmpTxtLoginResource.BlkJSX.getValue();
    }
    var valLoginServer = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_SERVER);
    if (BlkLoginServer != null) {
      if (clientType == "WEB") {
        valLoginServer = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_WEBCLIENTSERVER);
      }
      else{
        valLoginServer = cmpCmbLoginServer.BlkJSX.getValue();
      }
    }
    
    var loginResourcePassword = cmpTxtLoginResourcePassword.BlkJSX.getValue();
    if( cmpBtnRadioAutoPassword.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED ){
      loginResourcePassword = getHWInfo_HDVolumeSerial();
    }

    var valLoginInterfaceMacAddress = cmpSelLoginInterface.BlkJSX.getValue();
    if (valLoginInterfaceMacAddress == "DefaultInterface" || jsx3.util.strEmpty(valLoginInterfaceMacAddress)) {
      var savedLoginInterfaceMacAddress = topic_Get(tApp, "LoginInterfaceMacAddress");
      var LoginIPAddress = null;
      if (savedLoginInterfaceMacAddress == "DefaultInterface" || jsx3.util.strEmpty(savedLoginInterfaceMacAddress)) {
        LoginIPAddress = getHWInfo_IPAddress();
        valLoginInterfaceMacAddress = "DefaultInterface";
      }
      else {
        valLoginInterfaceMacAddress = savedLoginInterfaceMacAddress;
        LoginIPAddress = getHWInfo_IPAddressOfMacAddress(savedLoginInterfaceMacAddress);
      }
    }
    else {
      LoginIPAddress = getHWInfo_IPAddressOfMacAddress(valLoginInterfaceMacAddress);
    }
    topic_Set(tApp, "ClientIPAddress", LoginIPAddress);
    IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_CLIENTIPADDRESS, LoginIPAddress);    
    
    if(valLogin == ""){
      alert("Please enter a Username");
      cmpLoginUser.BlkJSX.focus();
      return false;
    } 
    else if(valPassword == "") {
      alert("Please enter a Password");
      cmpTxtLoginPwd.BlkJSX.focus();
      return false;
    }
    else if(valLoginResource == "") {
      alert("Please enter a Client Name");
      cmpTxtLoginResource.BlkJSX.focus();
      return false;
    }
    else if(jsx3.util.strEmpty(valLoginServer)) {
      alert("Please enter a Server IP");
      cmpCmbLoginServer.BlkJSX.focus();
      return false;
    }
    else if(valSelDomain == null) {
      alert("Please select a Domain");
      return false;
    }
    else if( cmpBtnRadioManualPassword.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED  && jsx3.util.strEmpty(loginResourcePassword) ){
      alert("Please select/enter a Client Password");
      cmpTxtLoginResourcePassword.BlkJSX.focus();
      return false;    
    }
    else if( valAutoLogin == "true" && valPassword == "") {
      alert("Please enter a Password");
      cmpTxtLoginPwd.BlkJSX.focus();
      return false;    
    }
    else {
      IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_USER, valLogin);
      IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_PASSWORD, valPassword);
      IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_DOMAIN, valSelDomain);

      setLocalPreferences(PNameSpace, ".//LastLoginUsername", valLogin);
      setLocalPreferences(PNameSpace, ".//LastLoginServer", valLoginServer);
      setLocalPreferences(PNameSpace, ".//Login/user", valLogin);
      setLocalPreferences(PNameSpace, ".//Login/domain", valSelDomain);
      setLocalPreferences(PNameSpace, ".//Login/resource", valLoginResource);
      setLocalPreferences(PNameSpace, ".//Asset/AssetResourceTitle", valLoginResource);
      setLocalPreferences(PNameSpace, ".//Login/AutoLogin", valAutoLogin);
      setLocalPreferences("GLOBAL", ".//Login/AutoLogin", valAutoLogin);

      if (valAutoLogin == "true") {
        setLocalPreferences(PNameSpace, ".//Login/password", valPassword);
        setLocalPreferences("GLOBAL", ".//Login/password", valPassword);
      }
      else {
        setLocalPreferences(PNameSpace, ".//Login/password", "");
        setLocalPreferences("GLOBAL", ".//Login/password", "");
      }

      if (cmpBtnRadioManualPassword.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED) {
        IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_AUTOGENERATEPASSWORD, "false");
        setLocalPreferences(PNameSpace, ".//Login/AutoGenerateResourcePassword", "false");
      }
      else {
        IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_AUTOGENERATEPASSWORD, "true");
        setLocalPreferences(PNameSpace, ".//Login/AutoGenerateResourcePassword", "true");
      }
      IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_ASSETPRIVATEKEYDATA, loginResourcePassword);
      IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE_PASSWORD, loginResourcePassword);
      setLocalPreferences(PNameSpace, ".//Login/resourcePassword", loginResourcePassword);
      setLocalPreferences(PNameSpace, ".//Asset/AssetPrivateKeyData", loginResourcePassword);

      topic_Set(tApp, "LoginInterfaceMacAddress", valLoginInterfaceMacAddress);
      //set to Preferences.xml
      setLocalPreferences("GLOBAL", ".//Login/InterfaceMacAddress", valLoginInterfaceMacAddress);

      if (clientType == "WEB") {
        valLoginServer = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_WEBCLIENTSERVER);
        //add for WEB client log in
        IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_ASSETTEMPLATETITLE, valLoginResource);
        //set login resource assetResourceTitle_username
        IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE, valLoginResource + "_" + valLogin);
      }
      else {
        IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE, valLoginResource);
      }
      IpvsLogin.setLoginParam(com.ipvs.api.Login.LOGIN_SERVER, valLoginServer);
      //Update the Server list if new server ip is encountered
      var newServerListNode = getLocalPreferencesNode(PNameSpace, ".//Login/ServerList").cloneNode(true);
      var loginServerName = valLoginServer;
      if (!jsx3.util.strEmpty(newServerListNode)) {
        var serverNode = newServerListNode.selectSingleNode("./Server[@Value ='" + valLoginServer + "']");
        if (serverNode != null) {
          loginServerName = serverNode.getAttribute("Name");
          newServerListNode.removeChild(serverNode);
        }
        else {
          var record = { jsxid: valLoginServer, jsxtext: valLoginServer, jsxvalue: valLoginServer };
          cmpCmbLoginServer.BlkJSX.insertRecord(record);
        }
        cmpCmbLoginServer.BlkJSX.setValue(valLoginServer);
        var newServerNode = newServerListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Server").setValue("");
        newServerNode.setAttribute("Name", loginServerName);
        newServerNode.setAttribute("Value", valLoginServer);
        var firstServerNode = newServerListNode.getFirstChild();
        if (firstServerNode) {
          if (jsx3.util.strEmpty(firstServerNode.getAttribute("Value"))) {
            newServerListNode.replaceNode(newServerNode, firstServerNode);
          }
          else {
            newServerListNode.insertBefore(newServerNode, firstServerNode);
          }
        }
        else {
          newServerListNode.appendChild(newServerNode);
        }
        var lastServerNode = null;
        if (newServerListNode.getChildNodes().size() > 10) {
          newServerListNode.removeChild(newServerListNode.getLastChild());
        }
        setLocalPreferencesNode(PNameSpace, ".//Login/ServerList", newServerListNode);
      }
    }
    //topic_Publish(tApp, "LoginState", 'PROCESSING');
    topic_Publish(tApp, "LoginStatus", 'Logging in...');
    window.setTimeout(delayRequest, 1);    
    return true;
  }
  
  function delayRequest() {
    IpvsLogin.doLogin()
  }
  
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this, onLoginStateChange);
  
  function onLoginStateChange(objEvent) {
    if( objEvent.stateVar != com.ipvs.api.Login.LOGINSTATE ) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.api.Login.LOGIN_SUCCESS:
        //Handle F5 Key,Back,ReLoad Icon
        subscribeBrowserClose();
        unsubscribeEnterKeyAction();
        var UserJID = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
        topic_Publish(tApp, com.ipvs.api.Login.LOGIN_USERJID, UserJID);
        var userName = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USER);
        var clientName = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE);
        if (clientType == "WEB") {
          clientName = clientName.split("_")[0];
        }
        var welcomeText = userName + "." + clientName; 
        topic_Publish(tApp, com.ipvs.api.Login.LOGIN_USER, welcomeText);
        topic_Publish(tApp, "LoginStatus", '');
        handleAssetPrivateData();
        // check for app server jid - is returned by set service domain
        var serviceContactJID = topic_Get(tApp,"AppServerAgentJID");
        if( serviceContactJID != null && serviceContactJID != '') {
          log.info("ServiceLogin: App Server JID: " + serviceContactJID);
          return;
        }
        break;
      case com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS:
        unsubscribeEnterKeyAction();
        LastLoginState = "NONE";
        clearSessionPreferences(PNameSpace, "REFRESHED");
        topic_Publish(tApp, "LoginStatus", '');
        destroyInActivityTimeout();
        //clean Dialog if there is
        if( jsx3.gui.Dialog ) {
          try {
            var DialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
            for (var i = 0; i < DialogBoxArr.length; i++) {
              var BlkJSX = DialogBoxArr[i];
              BlkJSX.doClose();
            }
          }catch(e) {
            log.warn("cmpBlkLogin:LoginState - LOGOUT:clean Dialog:" + e.description);
          }
        }
        //clean Window if there is
        if (jsx3.gui.Window) {
          try {
            var arrWindow = PNameSpace.WINDOWS.getChildren();
            jsx3.$A(arrWindow).each(function(x) {
              //give a 20 ms delay for trigger each window close
              setTimeout(function(){x.close();},20);
            });
          } catch (e) {
            log.warn("cmpBlkLogin:LoginState - LOGOUT:clean Window:" + e.description);
          }
        }
        // clean up the topics from the cacahe
        for (var i = 0; i < cacheDocNamesArr.length; i++) {
          var topicName = cacheDocNamesArr[i];
          topic_Destroy(topicName);
        }
        break;     
      default:
        return;
    }
  }
  
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGIN_ERROR, this, onLoginStateError);
  
  function onLoginStateError(objEvent) {
    if( objEvent.stateVar != com.ipvs.api.Login.LOGINSTATE ) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.api.Login.LOGIN_XMPP_DUPLICATE:
        handleServiceStateErrors("Duplicate Login is not supported.\n\nPlease exit the application and restart.");
        break;
      case com.ipvs.api.Login.LOGIN_XMPP_ERROR:
        handleServiceStateErrors("Login failed, please try again.");
        break;
      case com.ipvs.api.Login.LOGIN_SETSERVICE_VERSION_ERROR:
        // logout the user
        handleServiceStateErrors("Service Version mismatch.\n\nPlease click OK to download the latest version of client software from the server.",stateVal);
        break;
      case com.ipvs.api.Login.LOGIN_SETSERVICE_CLIENTINUSE_ERROR:
        clearCacheServiceRoot();
        handleServiceStateErrors("Client password incorrect.");
        break;
      case com.ipvs.api.Login.LOGIN_SETSERVICE_CLIENTLOGGEDIN_ERROR:
        clearCacheServiceRoot();
        handleServiceStateErrors("Client name is already in use.");
        break;
      case com.ipvs.api.Login.LOGIN_SETSERVICE_CLIENTINVALID_ERROR:  //1026
        clearCacheServiceRoot();
        handleServiceStateErrors("Invalid Client Name.",stateVal);
        break;
      case com.ipvs.api.Login.LOGIN_SETSERVICE_INVALIDCLIENT_ERROR: //1037
        clearCacheServiceRoot();
        handleServiceStateErrors("Cannot login using this Client Name.");
        break;
      case com.ipvs.api.Login.LOGIN_INVALID_SERVER_IPADDRESS: //1040
        clearCacheServiceRoot();
        handleServiceStateErrors("Unable to connect to server:" + cmpCmbLoginServer.BlkJSX.getValue());
        break;
      case com.ipvs.api.Login.LOGIN_SETSERVICE_APPSERVERNOTFOUND_ERROR: //APPSERVER_JID_NOT_FOUND
        handleServiceStateErrors("App server is not ready, please try again.");                       
        break;
      case com.ipvs.api.Login.LOGIN_GETSERVERTIME_ERROR:
        clearCacheServiceRoot();
        handleServiceStateErrors("Get ServerTime failed.");
        break;
      case com.ipvs.api.Login.LOGIN_MAX_CLIENTS_EXCEEDED_ERROR: //MAX_CLIENTS_EXCEEDED
        handleServiceStateErrors("The maximum number of concurrent user licenses has been exceeded. Please contact your network administrator.");                       
        break;
      default:
        handleServiceStateErrors("Unknown error. Error Code:" + stateVal + ". Please try again.");
        return;
    }
  }
  
  function handleServiceStateErrors(alertMsg,stateVal) {
    //logout incase of  service state error and popup the msg
    LastLoginState = "ERROR";
    topic_Publish(tApp, "LoginStatus", 'Login failed ...');
    //IpvsLogin.doLogout();
    cmpTxtLoginPwd.BlkJSX.setValue("");
    cmpTxtLoginPwd.BlkJSX.focus();
    if( !jsx3.util.strEmpty(alertMsg) ) {
      alert(alertMsg);
      //For UI if Service Version mismatch happens then point the user to new available software download
      if (!jsx3.util.strEmpty(stateVal) && stateVal == com.ipvs.api.Login.LOGIN_SETSERVICE_VERSION_ERROR) {
        IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this, closeWindowAndDownloadApp);
      }
      //lets just show the advance settings block: Client Inavlid, Client Inavlid password, Server ip wrong(this case it saus login inavlid)
      else {
      //else if(!jsx3.util.strEmpty(stateVal) && stateVal == com.ipvs.api.Login.LOGIN_SETSERVICE_CLIENTINVALID_ERROR) {
        IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this, openLoginSettingsPage);
      }
    }
  }

  function closeWindowAndDownloadApp(objEvent) {
    if (objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS) {
      IpvsLogin.unsubscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, this);
      unloadApplet();
      jsx3.sleep(function(){window.close();},null,this);// this cause mshta process left behind taskmanager, if manul click window "X" not  
      var loginServer = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_SERVER)
      window.open("http://" + loginServer + "/", "_blank", "fullscreen=no,titlebar=yes,toolbar=no,statusbar=no,menubar=no", true);
    }
  }
  
  function openLoginSettingsPage(objEvent) {
    if (objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS) {
      jsx3.sleep(function(){
        topic_Publish(tApp, "LoginAdvanceSettingsBlock", "OPEN");
      },null,this);
    }
  }

  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGIN_RETRY, this, doRelogin);

  function doRelogin(objEvent) {
    if( objEvent.stateVar != com.ipvs.api.Login.RETRYLOGINSTATE ) return;
    var stateVal = objEvent.stateVal;
    if( stateVal == com.ipvs.api.Login.LOGIN_NONE ) {
      topic_Publish(tApp, "LoginStatus", " ");
    }
    else if( stateVal == com.ipvs.api.Login.RETRYLOGIN_START ) {
      cmpLoginUser.BlkJSX.setValue(IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RETRYUSER));
      cmpTxtLoginPwd.BlkJSX.setValue(IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RETRYPASSWORD));
      topic_Publish(tApp, "LoginStatus", "Session terminated. Retrying Login ... (30)");
    }
    else if( stateVal == com.ipvs.api.Login.RETRYLOGIN_END ) {
      topic_Publish(tApp, "LoginStatus", " ");
      doLogin();
    }
    else {
      topic_Publish(tApp, "LoginStatus", "Session terminated. Retrying Login ... (" + stateVal + ")");
    }     
  }

  function subscribeEnterKeyAction() {
    jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, eval(handleEnterKeyAction)); 
  }

  function unsubscribeEnterKeyAction() {
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, eval(handleEnterKeyAction)); 
  }

  function handleEnterKeyAction(objEvent) {
    if(objEvent.event.enterKey()) {    
      doLogin();
    }
  }
  //auto-login:fill value to the 'Password' field and 'Login'
//  eventApp.subscribe("AutoLogin", null, this,
//                     function() {
//                        cmpTxtLoginPwd.BlkJSX.setValue(topic_Get(tApp, "LoginPassword"));
//                        cmpBtnLogin.onSelect.call(cmpBtnLogin); 
//                     });

  var cmpBlkCancel = null;
  if( blkCancel != null){
    cmpBlkCancel = new CmpContainer([BlkName, blkCancel],
                                        [ {cmp:cmpBtnCancelRetryLogin}
                                        ],
                                        null);
  }
  else{
    cmpBlkCancel = cmpBtnCancelRetryLogin;
  }


  var cmpBlkAdvanceSettings = null;
  if( BlkAdvanceSettings != null ) {
    cmpBlkAdvanceSettings = new CmpContainer(BlkAdvanceSettings,
                                            [ {cmp:cmpBlkLoginResource},
                                               //sel:"IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_CLIENTNAME) == ''"},
                                              {cmp:cmpTxtLoginResource},
			                                        {cmp: cmpBlkLoginServer,
                                                sel:"clientType != 'WEB'"},
                                              {cmp: cmpBlkLoginResourcePassword,
                                                //sel:"clientType != 'WEB' && IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_CLIENTNAME) == ''"}, 
                                                sel:"clientType != 'WEB'"}, 
                                              {cmp:cmpBlkInterface,
                                                sel:"clientType != 'WEB'"},
                                              {cmp:cmpBtnRadioAutoPassword},
                                              {cmp:cmpBtnRadioManualPassword},
                                              {cmp:cmpTxtLoginResourcePassword}, 
                                              {cmp:cmpCmbLoginServer},
                                              {cmp:cmpSelLoginInterface},
                                              {cmp:cmptxtLoginInterfaceIP}
                                            ],
                                            null
                                            );
                                            }

 handleFieldsState(jsx3.gui.Form.STATEDISABLED);
 var cmpBlkLogin = new CmpContainer(BlkName,
                                    [ {cmp:cmpLoginUser},
							                        {cmp:cmpTxtLoginPwd},
							                        {cmp:cmpSelLoginDomain},
							                        {cmp:cmpBtnAdvanceSettings,
							                          sel: "topic_Get('" + tApp + "', 'AppletLoaded') == 'DONE'"
                                      },							                        
                                      {cmp:cmpBlkAdvanceSettings,
                                        sel:"topic_Get('" + tApp + "', 'LoginAdvanceSettingsBlock') == 'OPEN'"},
							                        {cmp:cmpTxtLoginError,
                                       sel:"IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_SUCCESS"},
                                      {cmp:cmpBtnLogin,
                                        sel: "topic_Get('" + tApp + "', 'AppletLoaded') == 'DONE' && " +
                                           "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_INPROGRESS && " +
                                           "IpvsLogin.getStateParam(com.ipvs.api.Login.LOGINSTATE) != com.ipvs.api.Login.LOGIN_SUCCESS"},
                                      {cmp:cmpBlkCancel,
                                       sel:"IpvsLogin.getStateParam(com.ipvs.api.Login.RETRYLOGINSTATE) == com.ipvs.api.Login.RETRYLOGIN_INPROGRESS"},
                                      {cmp:cmpBlkAutoLogin,
                                       sel:"clientType != 'WEB' && IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_APPNAME) == 'IPVSClient' "},
                                      {cmp:cmpChkAutoLogin}    
                                    ],
                                    [{ action: "CLSEVENT", 
                                       eventcls:IpvsLogin, 
                                       eventsub:com.ipvs.api.Login.ON_LOGIN_RETRY, 
                                       eventvar:com.ipvs.api.Login.RETRYLOGINSTATE, 
                                       eventval: null },
                                     { action: "TOPICEVENT", topic: tApp, eventvar: "LoginAdvanceSettingsBlock", eventval: null },
                                     { action: "TOPICEVENT", topic: tApp, eventvar: "AppletLoaded", eventval: null }
                                    ]);


  return cmpBlkLogin;
}
