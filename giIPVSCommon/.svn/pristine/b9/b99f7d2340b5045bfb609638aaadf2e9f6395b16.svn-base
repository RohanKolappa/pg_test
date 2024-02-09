/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Login class provides UI for login screen.
* <p/>
* This class uses com.ipvs.agent.ServiceAgent.
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.Login", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(Login, Login_prototype) {

  Login.SERVICEAGENT = com.ipvs.agent.ServiceAgent;
  /** @private @jsxobf-clobber */
  Login_prototype._LOG = jsx3.util.Logger.getLogger(Login.jsxclass.getName());

  /**
  * The instance initializer.
  */
  Login_prototype.onAfterAttach = function() {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if(!this.clsApp) return;
    if(this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  Login_prototype._onAppInit = function() {
    this.cmbLogin = this.getDescendantOfName("cmbLogin");
    this.txtPassword = this.getDescendantOfName("txtPassword");
    this.selDomain = this.getDescendantOfName("selDomain");
    this.blkLoginResource = this.getDescendantOfName("blkLoginResource");
    this.txtLoginResource = this.getDescendantOfName("txtLoginResource");
    this.blkLoginServer = this.getDescendantOfName("blkLoginServer");
    this.cmbLoginServer = this.getDescendantOfName("cmbLoginServer");
    this.lblMessage = this.getDescendantOfName("lblMessage");
    this.btnLogin = this.getDescendantOfName("btnLogin");
    this.btnEndSession = this.getDescendantOfName("btnEndSession");
    this.btnLogin.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Login);
    this.btnEndSession.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_EndSession);

    this._initLoginParams();

    this.subscribe("LoginStatus", this, this._handleLabelMsg);
    this.txtPassword.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
    this.txtPassword.setEvent("this._onFocus_Password();", jsx3.gui.Interactive.JSXFOCUS);
    this.txtPassword._onFocus_Password = this._onFocus_Password;
    this._viewState = false;
    this._LOG.info("Login: Initiated");
  }

  Login_prototype._initLoginParams = function() {
    if(!this._viewState) {
      this.clsApp.subscribe(com.ipvs.app.App.MENU_SELECT, this, this._toggle);
      this.clsApp.clsApplet.subscribe("AppletStatus", this, this._appletStatus);
      this.clsApp.clsServiceAgent.subscribe(Login.SERVICEAGENT.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
      this.clsApp.clsServiceAgent.subscribe(Login.SERVICEAGENT.ON_LOGIN_ERROR, this, this._onLoginStateError);
    }

    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USERJID, "");
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USERDOMAIN, "");
    var loginUname = this.clsApp.clsPreferences.getLocalPreferences("Login/user", "");
    if(loginUname == "") {
      // if not seee if there is a last user name
      loginUname = this.clsApp.clsPreferences.getLocalPreferences("LastLoginUsername", "");
    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USER, loginUname);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PASSWORD, this.clsApp.clsPreferences.getLocalPreferences("Login/password", ""));

    var loginDomain = this.clsApp.clsPreferences.getLocalPreferences("Login/domain", DefaultLoginDomain);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_DOMAIN, loginDomain);

    var loginServer = this.clsApp.clsPreferences.getLocalPreferencesNode("Login/ServerList/Server", DefaultLoginServer).getAttribute("Value");
    if(loginServer == "") {
      loginServer = this.clsApp.clsPreferences.getLocalPreferences("LastLoginServer", "");
    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVER, loginServer);
    //    if( loginServer == "" ) {
    //      this.blkLoginServer.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    //    }
    //    else {
    //      this.blkLoginServer.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    //    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PORT, this.clsApp.clsPreferences.getLocalPreferences("Login/port", DefaultLoginPort));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVICEDOMAIN, this.clsApp.clsPreferences.getLocalPreferences("Login/servicedomain", DefaultServiceDomain));
    // Setting the App Name to be sent with Set Service Doamin Request
    var AppName = ""
    if(PNameSpace.getEnv("NAMESPACE") == "OdysseyClientUI") {
      AppName = "OdysseyClient";
    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_APPNAME, AppName);

    //-------???? need to confirm following logic
    var AssetTemplateTitle = null;
    var AssetResourceTitle = this.clsApp.clsPreferences.getLocalPreferences("AssetResourceTitle", "");
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_CLIENTNAME, AssetResourceTitle);
    if(AssetResourceTitle == "") {
      AssetResourceTitle = this.clsApp.clsPreferences.getLocalPreferences("Login/resource", DefaultLoginResource);
      this.blkLoginResource.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
    else {
      this.blkLoginResource.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    // if not found - use default resource name as template name
    if(AssetResourceTitle == "") {
      AssetTemplateTitle = this.clsApp.clsPreferences.getLocalPreferences("Login/resource", DefaultLoginResource);
      //      AssetResourceTitle = AssetTemplateTitle + "-" + getHWInfo_ComputerName(this.clsApp.getBrowserType());
      //      //if no UI to ask user for resource name
      //      if( BlkLoginResource == null ) {
      //        //generate a new dynamic resource name from template name
      //        AssetResourceTitle = AssetTemplateTitle + "-" + getHWInfo_ComputerName();
      //        //AssetResourceTitle = AssetTemplateTitle + "-" + getUUID();
      //      }
    }
    //-----

    var resourcePassword = getHWInfo_HDVolumeSerial(this.clsApp.getBrowserType());
    var valAutoGeneratePassword = this.clsApp.clsPreferences.getLocalPreferences("Login/AutoGenerateResourcePassword", "true");
    if(valAutoGeneratePassword == "false") {
      resourcePassword = this.clsApp.clsPreferences.getLocalPreferences("Login/resourcePassword", "");
      if(resourcePassword == "") {
        resourcePassword = this.clsApp.clsPreferences.getLocalPreferences("Asset/AssetPrivateKeydata", "");
      }
    }

    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE, AssetResourceTitle);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE_PASSWORD, resourcePassword);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETTEMPLATETITLE, AssetTemplateTitle);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEKEYSCHEME, this.clsApp.clsPreferences.getLocalPreferences("AssetPrivateKeyScheme", "SERVER"));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEKEYDATA, resourcePassword);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEDATA, this.clsApp.clsPreferences.getLocalPreferences("AssetPrivateData", ""));
    var recentUsersList = this.clsApp.clsPreferences.getLocalPreferencesNode("RecentUsersList");
    if(recentUsersList != null) {
      var iterUsers = recentUsersList.selectNodeIterator(".//UserName");
      while(iterUsers.hasNext()) {
        var nodeUser = iterUsers.next();
        var userName = nodeUser.getValue();
        var record = { jsxid: userName, jsxtext: userName, jsxvalue: userName };
        this.cmbLogin.insertRecord(record);
      }
      var firstUserNode = recentUsersList.getFirstChild();
      var defaultUser = firstUserNode.getValue();
      if(!jsx3.util.strEmpty(defaultUser)) {
        this.cmbLogin.setValue(defaultUser);
      }
    }
    var selDomainRec = { jsxid: loginDomain, jsxtext: loginDomain, jsxvalue: loginDomain };
    this.selDomain.insertRecord(selDomainRec);
    this.selDomain.setValue(loginDomain);
    this.txtLoginResource.setValue(AssetResourceTitle);
    var serverList = this.clsApp.clsPreferences.getLocalPreferencesNode("ServerList");
    if(serverList != null) {
      var iterServers = serverList.selectNodeIterator(".//Server");
      while(iterServers.hasNext()) {
        var nodeServer = iterServers.next();
        var serverName = nodeServer.getAttribute("Name");
        var serverValue = nodeServer.getAttribute("Value");
        if(jsx3.util.strEmpty(serverName)) {
          serverName = serverValue;
        }
        var record = { jsxid: serverValue, jsxtext: serverName, jsxvalue: serverValue };
        this.cmbLoginServer.insertRecord(record);
      }
      var firstServerNode = serverList.getFirstChild();
      var defaultServer = firstServerNode.getAttribute("Value");
      if(!jsx3.util.strEmpty(defaultServer)) {
        this.cmbLoginServer.setValue(defaultServer);
      }
    }
    this._viewState = true;
    this._subscribeEnterKeyAction();
  }

  Login_prototype._handleBlkLoginResource = function() {
    if(this.clsApp.clsPreferences.getLocalPreferences("AssetResourceTitle", "") == "") {
      this.blkLoginResource.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    }
    else {
      this.blkLoginResource.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    }
    this.blkLoginResource.repaint();
  }

  /** @private @jsxobf-clobber */
  Login_prototype._toggle = function(objEvent) {
    if(this.clsApp.getMenuSelection() == "NONE") {
      this.cmpShow();
      this._LOG.info("Login: Show");
    }
    else {
      this.cmpHide();
      this._LOG.info("Login: Hide");
    }
  }

  Login_prototype._onLoginStateChange = function(objEvent) {
    if(objEvent.stateVar != Login.SERVICEAGENT.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case Login.SERVICEAGENT.LOGIN_SUCCESS:
        //Handle F5 Key,Back,ReLoad Icon
        this._unsubscribeEnterKeyAction();
        this.publish({ subject: "LoginStatus", eventVal: "" });
        this._clearCacheServiceRoot();
        this._handleAssetResource(this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_ASSETRESOURCE));
        // check for app server jid - is returned by set service domain
        var serviceContactJID = this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_APPSERVERAGENTJID);
        if(serviceContactJID != null && serviceContactJID != '') {
          this._LOG.info("ServiceLogin: App Server JID: " + serviceContactJID);
          return;
        }
        break;
      case Login.SERVICEAGENT.LOGIN_LOGOUT_SUCCESS:
        if(this.clsApp.clsServiceAgent.getStateParam(Login.SERVICEAGENT.LOGINERRORCODE) == "NONE") {
          this.publish({ subject: "LoginStatus", eventVal: "" });
        }
        this.txtPassword.setValue("");
        this._handleBlkLoginResource();
        this._subscribeEnterKeyAction();
        //destroyInActivityTimeout();
        //clean Dialog if there is
        if(jsx3.gui.Dialog) {
          try {
            var DialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
            for(var i = 0;i < DialogBoxArr.length;i++) {
              var BlkJSX = DialogBoxArr[i];
              BlkJSX.doClose();
            }
          } catch(e) {
            this._LOG.warn("Login:LoginState - LOGOUT:clean Dialog:" + e.description);
          }
        }
        //clean Window if there is
        if(jsx3.gui.Window) {
          try {
            var arrWindow = PNameSpace.WINDOWS.getChildren();
            jsx3.$A(arrWindow).each(function(x) {
              //give a 20 ms delay for trigger each window close
              setTimeout(function() { x.close(); }, 20);
            });
          } catch(e) {
            this._LOG.warn("Login:LoginState - LOGOUT:clean Window:" + e.description);
          }
        }
        break;
      default:
        return;
    }
  }

  Login_prototype._handleAssetResource = function(assetResource) {
    var respAssetResource = assetResource.selectSingleNode("//AssetResource");
    if(respAssetResource != null) {
      var nodeAssetResourceTitle = respAssetResource.selectSingleNode(".//Info/Title");
      if(nodeAssetResourceTitle != null) {
        this.clsApp.clsPreferences.setLocalPreferences("AssetResourceTitle", nodeAssetResourceTitle.getValue());
      }
      var nodeAssetPrivateKeyScheme = respAssetResource.selectSingleNode(".//AssetPrivateKey/KeyScheme");
      if(nodeAssetPrivateKeyScheme != null) {
        var assetPrivateKeyScheme = nodeAssetPrivateKeyScheme.getValue();
        this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateKeyScheme", assetPrivateKeyScheme);
        if(assetPrivateKeyScheme == "SERVER") {
          var nodeAssetPrivateKeyData = respAssetResource.selectSingleNode(".//AssetPrivateKey/KeyData");
          if(nodeAssetPrivateKeyData != null) {
            this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateKeyData", nodeAssetPrivateKeyData.getValue());
          }
        }
      }
      var nodeAssetPrivateData = respAssetResource.selectSingleNode(".//AssetPrivateData");
      if(nodeAssetPrivateData != null) {
        var strAssetPrivateData = nodeAssetPrivateData.toString();
        strAssetPrivateData = strAssetPrivateData.replace(/\&lt;/g, '<');
        strAssetPrivateData = strAssetPrivateData.replace(/\&gt;/g, '>');
        var nodeAssetPrivateDataXML = (new jsx3.xml.Document()).loadXML(strAssetPrivateData.replace(/[\n\r]/g, " "));
        for(var iter = nodeAssetPrivateDataXML.getChildNodes().iterator();iter.hasNext();) {
          var childNode = iter.next();
          var objValue = childNode.getValue();
          var objName = childNode.getNodeName();
          this.clsApp.clsPreferences.setLocalPreferences(objName, objValue);
        }
      }
    }
    return;
  }

  Login_prototype._onLoginStateError = function(objEvent) {
    if(objEvent.stateVar != Login.SERVICEAGENT.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case Login.SERVICEAGENT.LOGIN_XMPP_DUPLICATE:
        this._handleServiceStateErrors("Duplicate Login is not supported.\n\nPlease exit the application and restart.");
        break;
      case Login.SERVICEAGENT.LOGIN_XMPP_ERROR:
        this._handleServiceStateErrors("Login failed, please try again.");
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_VERSION_ERROR:
        // logout the user
        this._handleServiceStateErrors("Service Version mismatch.\n\nPlease download the latest version of the client software.", Login.SERVICEAGENT.LOGIN_SETSERVICE_VERSION_ERROR);
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_CLIENTINUSE_ERROR:
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Client Name already in use.");
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_CLIENTINVALID_ERROR:  //1026
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Invalid Client Name.");
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_INVALIDCLIENT_ERROR: //1037
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Cannot login using this Client Name.");
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_DEVICEINUSE_ERROR:
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Device is already in use.");
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_APPSERVERNOTFOUND_ERROR: //APPSERVER_JID_NOT_FOUND
        this._handleServiceStateErrors("App server is not ready, please try again.");
        break;
      case Login.SERVICEAGENT.LOGIN_GETSERVERTIME_ERROR:
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Get ServerTime failed.");
        break;
      case Login.SERVICEAGENT.LOGIN_MAX_CLIENTS_EXCEEDED_ERROR: //MAX_CLIENTS_EXCEEDED
        this.publish({ subject: "LoginStatus", eventVal: "The maximum number of concurrent user licenses has been exceeded. &nbsp;&nbsp;Please contact your network administrator." });
        break;
      default:
        this._handleServiceStateErrors("Unknown error. Error Code:" + stateVal + ". Please try again.");
        return;
    }
  }

  Login_prototype._handleServiceStateErrors = function(alertMsg, stateVal) {
    //logout incase of  service state error and popup the msg
    this.publish({ subject: "LoginStatus", eventVal: "Login failed ..." });
    //ServiceAgent.doLogout();
    this.txtPassword.setValue("");
    if(!jsx3.util.strEmpty(alertMsg)) {
      jsx3.sleep(function() {
		    alert(alertMsg);
      }, null, this);
    }
  }

  Login_prototype._clearCacheServiceRoot = function() {
    // clear out local login prefs
    this.clsApp.clsPreferences.setLocalPreferences("AssetResourceTitle", '');
    this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateKeyScheme", '');
    this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateKeyData", '');
    this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateData", '');
    //Clear previous AssetPrivateData values in prferences.
    this.clsApp.clsPreferences.setLocalPreferences("InActivityTimeout", "");
    this.clsApp.clsPreferences.setLocalPreferences("InActivityWarningTimeout", "");
    this.clsApp.clsPreferences.setLocalPreferences("HomepageURL", "");
    this.clsApp.clsPreferences.setLocalPreferences("BannerURL", "");
  }

  Login_prototype._onClick_Login = function() {
    if(this.clsApp.clsApplet.getAppletStatus() != "LOADED") {
      alert("Initializing .. Please wait and retry")
      return false;
    }
    var valLogin = this.cmbLogin.getValue();
    var valPassword = this.txtPassword.getValue();
    var valSelDomain = this.selDomain.getValue();
    var valLoginResource = this.txtLoginResource.getValue();
    var valLoginServer = this.cmbLoginServer.getValue();
    if(valLogin == "") {
      alert("Please enter a Username");
      this.cmbLogin.focus();
      return false;
    }
    else if(valPassword == "") {
      alert("Please enter a Password");
      this.txtPassword.focus();
      return false;
    }
    else if(valLoginResource == "") {
      alert("Please enter a Client Name");
      this.txtLoginResource.focus();
      return false;
    }
    else if(valLoginServer == "") {
      alert("Please enter a Server IP");
      return false;
    }
    else if(valSelDomain == null) {
      alert("Please select a Domain");
      return false;
    }
    else {
      valLogin = valLogin.toLowerCase();
      this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USER, valLogin);
      this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PASSWORD, valPassword);
      this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_DOMAIN, valSelDomain);
      this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE, valLoginResource);
      this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVER, valLoginServer);

      this.clsApp.clsPreferences.setLocalPreferences("LastLoginUsername", valLogin);
      this.clsApp.clsPreferences.setLocalPreferences("LastLoginServer", valLoginServer);
      this.clsApp.clsPreferences.setLocalPreferences("Login/user", valLogin);
      this.clsApp.clsPreferences.setLocalPreferences("Login/domain", valSelDomain);
      this.clsApp.clsPreferences.setLocalPreferences("Login/resource", valLoginResource);

      //Update the Recent UserList if a new user is encountered
      var newRecentUsersListNode = this.clsApp.clsPreferences.getLocalPreferencesNode("RecentUsersList").cloneNode(true);
      if(!jsx3.util.strEmpty(newRecentUsersListNode)) {
        var userNode = newRecentUsersListNode.selectSingleNode("./UserName[.='" + valLogin + "']");
        if(userNode != null) {
          newRecentUsersListNode.removeChild(userNode);
        }
        else {
          var record = { jsxid: valLogin, jsxtext: valLogin, jsxvalue: valLogin };
          this.cmbLogin.insertRecord(record);
        }
        this.cmbLogin.setValue(valLogin);
        var newUserNode = newRecentUsersListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "UserName").setValue(valLogin);
        var firstUserNode = newRecentUsersListNode.getFirstChild();
        if(firstUserNode) {
          if(jsx3.util.strEmpty(firstUserNode.getValue())) {
            newRecentUsersListNode.replaceNode(newUserNode, firstUserNode);
          }
          else {
            newRecentUsersListNode.insertBefore(newUserNode, firstUserNode);
          }
        }
        else {
          newRecentUsersListNode.appendChild(newUserNode);
        }
        var lastUserNode = null;
        if(newRecentUsersListNode.getChildNodes().size() > 10) {
          newRecentUsersListNode.removeChild(newRecentUsersListNode.getLastChild());
        }
        this.clsApp.clsPreferences.setLocalPreferencesNode("Login/RecentUsersList", newRecentUsersListNode);
      }
      //Update the Server list if new server ip is encountered
      var newServerListNode = this.clsApp.clsPreferences.getLocalPreferencesNode("ServerList").cloneNode(true);
      if(!jsx3.util.strEmpty(newServerListNode)) {
        var serverNode = newServerListNode.selectSingleNode("./Server[@Value ='" + valLoginServer + "']");
        var serverName = "";
        if(serverNode != null) {
          serverName = serverNode.getAttribute("Name");
          newServerListNode.removeChild(serverNode);
        }
        else {
          var record = { jsxid: valLoginServer, jsxtext: valLoginServer, jsxvalue: valLoginServer };
          this.cmbLoginServer.insertRecord(record);
        }
        this.cmbLoginServer.setValue(valLoginServer);
        var newServerNode = newServerListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Server").setValue("");
        newServerNode.setAttribute("Name", serverName);
        newServerNode.setAttribute("Value", valLoginServer);
        var firstServerNode = newServerListNode.getFirstChild();
        if(firstServerNode) {
          if(jsx3.util.strEmpty(firstServerNode.getAttribute("Value"))) {
            newServerListNode.replaceNode(newServerNode, firstServerNode);
          }
          else {
            newServerListNode.insertBefore(newServerNode, firstServerNode);
          }
        }
        else {
          newServerListNode.appendChild(newServerNode);
        }
        this.clsApp.clsPreferences.setLocalPreferencesNode("Login/ServerList", newServerListNode);
      }
      this.publish({ subject: "LoginStatus", eventVal: "Logging in..." });

      jsx3.sleep((function() {
        this.clsApp.clsServiceAgent.doLogin();
      }), null, this);
      return true;
    }
  }

  Login_prototype._appletStatus = function(objEvent) {
    if(objEvent.subject == "AppletStatus") {
      var appletStatus = this.clsApp.clsApplet.getAppletStatus();
      var loginStatus = appletStatus;
      if(appletStatus == "LOADING") {
        loginStatus = "Loading...";
      }
      else if(appletStatus == "LOADED") {
        this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVICEVERSION, this.clsApp.clsApplet.getServiceVersion());
        this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SOFTWAREVERSION, "");
        var clientIPAdd = getHWInfo_IPAddress(this.clsApp.getBrowserType());
        this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_CLIENTIPADDRESS, clientIPAdd);
        loginStatus = " ";
      }
      this.publish({ subject: "LoginStatus", eventVal: loginStatus });
    }
  }

  Login_prototype._onFocus_Password = function() {
    var ThisClass = this.getAncestorOfType("com.ipvs.stx.Login");
    var LoginState = ThisClass.clsApp.clsServiceAgent.getStateParam(Login.SERVICEAGENT.LOGINSTATE);
    var LoginError = ThisClass.clsApp.clsServiceAgent.getStateParam(Login.SERVICEAGENT.LOGINERRORCODE);
    if(LoginState != "NONE" || LoginError != "NONE") {
      ThisClass.lblMessage.setText("", true);
    }
  }

  Login_prototype._handleLabelMsg = function(objEvent) {
    if(objEvent.subject == "LoginStatus") {
      this.lblMessage.setText(objEvent.eventVal, true);
    }
  }

  Login_prototype._subscribeEnterKeyAction = function() {
    jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, this, this._handleEnterKeyAction);
  }

  Login_prototype._unsubscribeEnterKeyAction = function() {
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, this, this._handleEnterKeyAction);
  }

  Login_prototype._handleEnterKeyAction = function(objEvent) {
    if(objEvent.event.enterKey()) {
      this._onClick_Login();
    }
  }

  Login_prototype._onClick_EndSession = function() {
    browserClose();
  }
});