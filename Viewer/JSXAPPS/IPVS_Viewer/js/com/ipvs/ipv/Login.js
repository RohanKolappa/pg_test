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
jsx3.Class.defineClass("com.ipvs.ipv.Login", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function (Login, Login_prototype) {

  Login.SERVICEAGENT = com.ipvs.agent.ServiceAgent;
  /** @private @jsxobf-clobber */
  Login_prototype._LOG = jsx3.util.Logger.getLogger(Login.jsxclass.getName());

  /**
  * The instance initializer.
  */
  Login_prototype.onAfterAttach = function () {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if (!this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  Login_prototype._onAppInit = function () {
    this.txtLogin = this.getDescendantOfName("txtLogin");
    this.txtPassword = this.getDescendantOfName("txtPassword");
    this.txtLoginResource = this.getDescendantOfName("txtClientName");
    this.cmbLoginServer = this.getDescendantOfName("cmbLoginServer");
    this.chkAutoLogin = this.getDescendantOfName("chkAutoLogin");
    this.btnChkAutoLogin = this.getDescendantOfName("btnChkAutoLogin");
    this.lblMessage = this.getDescendantOfName("lblMessage");
    this.blkMask = this.getDescendantOfName("blkMask");
    this.chkAutoPassword = this.getDescendantOfName("chkAutoPassword");
    this.btnChkAutoPassword = this.getDescendantOfName("btnChkAutoPassword");
    this.selLoginInterface = this.getDescendantOfName("selLoginInterface");
    this.txtLoginInterfaceIP = this.getDescendantOfName("txtLoginInterfaceIP");
    this.txtClientPassword = this.getDescendantOfName("txtClientPassword");
    this.blkSettingsCnt = this.getDescendantOfName("blkSettingsCnt");
    this.btnSettings = this.getDescendantOfName("btnSettings");
    this.btnLoginDsbl = this.getDescendantOfName("btnLoginDsbl");
    this.btnLogin = this.getDescendantOfName("btnLogin");
    this.btnCancelRetryLogin = this.getDescendantOfName("btnCancelRetryLogin");
    this.lblVersion = this.getDescendantOfName("lblVersion");

    this.txtLogin.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
    this.txtLogin.setEvent("this._onFocus_Login();", jsx3.gui.Interactive.JSXFOCUS);
    this.txtLogin._onFocus_Login = jsx3.$F(this._onFocus_Login).bind(this);
    this.txtPassword.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
    this.txtPassword.setEvent("this._onFocus_Password();", jsx3.gui.Interactive.JSXFOCUS);
    this.txtPassword._onFocus_Password = jsx3.$F(this._onFocus_Password).bind(this);
    this.txtLoginResource.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
    this.txtLoginResource.setEvent("this._onFocus_LoginResource();", jsx3.gui.Interactive.JSXFOCUS);
    this.txtLoginResource._onFocus_LoginResource = jsx3.$F(this._onFocus_LoginResource).bind(this);
    this.txtClientPassword.removeEvent(jsx3.gui.Interactive.JSXFOCUS);
    this.txtClientPassword.setEvent("this._onFocus_ClientPassword();", jsx3.gui.Interactive.JSXFOCUS);
    this.txtClientPassword._onFocus_ClientPassword = jsx3.$F(this._onFocus_ClientPassword).bind(this);
    this.cmbLoginServer.subscribe(jsx3.gui.Interactive.SELECT, this, this._onSelect_ServerIP);
    this.cmbLoginServer.subscribe(jsx3.gui.Interactive.JSXKEYDOWN, this, this._onSelect_ServerIP);

    this.subscribe("LoginStatus", this, this._handleLabelMsg);
    this.btnLogin.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Login);
    this.btnLogin.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.btnLoginDsbl.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Login);
    this.btnLoginDsbl.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.btnChkAutoLogin.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_AutoLogin);
    this.btnChkAutoLogin.setState(jsx3.gui.ImageButton.STATE_OFF);
    this.btnChkAutoPassword.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_AutoPassword);
    this.btnChkAutoPassword.setState(jsx3.gui.ImageButton.STATE_OFF);
    this.selLoginInterface.subscribe(jsx3.gui.Interactive.SELECT, this, this._onSelect_LoginInterface);
    this.btnSettings.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_AdvancedSettings);
    this.btnSettings.setState(jsx3.gui.ImageButton.STATE_OFF);
    this.btnSettings.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkSettingsCnt.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.btnCancelRetryLogin.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_CancelRetryLogin);
    this.btnCancelRetryLogin.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.lblVersion.setText(this.clsApp.getVersion(), true);
    this.blkMask.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.errorObjName = null;
    this._initLoginParams();

    this._viewState = false;
    this._LOG.info("Login: Initiated");
  }

  Login_prototype._initLoginParams = function () {
    if (!this._viewState) {
      this.clsApp.subscribe(com.ipvs.app.App.MENU_SELECT, this, this._toggle);
      this.clsApp.clsApplet.subscribe("AppletStatus", this, this._appletStatus);
      this.clsApp.clsServiceAgent.subscribe(Login.SERVICEAGENT.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);
      this.clsApp.clsServiceAgent.subscribe(Login.SERVICEAGENT.ON_LOGIN_ERROR, this, this._onLoginStateError);
      this.clsApp.clsServiceAgent.subscribe(Login.SERVICEAGENT.ON_LOGIN_RETRY, this, this._onRetryLogin);
    }

    // Setting the App Name to be sent with Set Service Doamin Request
    var AppName = ""
    if (PNameSpace.getEnv("NAMESPACE") == "IPVSViewer") {
      AppName = "IPVSClient";
    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_APPNAME, AppName);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USERJID, "");
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USERDOMAIN, "");

    var loginUname = this.clsApp.clsPreferences.getLocalPreferences("Login/user", "");
    if (loginUname == "") {
      // if not see if there is a last user name
      loginUname = this.clsApp.clsPreferences.getLocalPreferences("LastLoginUsername", "");
    }
    this.txtLogin.setValue(loginUname);
    this.txtPassword.setValue("");

    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USER, loginUname);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PASSWORD, this.clsApp.clsPreferences.getLocalPreferences("Login/password", ""));
    var loginDomain = this.clsApp.clsPreferences.getLocalPreferences("Login/domain", DefaultLoginDomain);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_DOMAIN, loginDomain);

    var AssetTemplateTitle = null;
    var AssetResourceTitle = this.clsApp.clsPreferences.getLocalPreferences("AssetResourceTitle", "");
    if (AssetResourceTitle == "") {
      AssetResourceTitle = this.clsApp.clsPreferences.getLocalPreferences("Login/resource", DefaultLoginResource);
    }
    // if not found - use default resource name as template name
    if (AssetResourceTitle == "") {
      AssetTemplateTitle = AssetResourceTitle;
    }
    this.txtLoginResource.setValue(AssetResourceTitle);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_CLIENTNAME, AssetResourceTitle);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE, AssetResourceTitle);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETTEMPLATETITLE, AssetTemplateTitle);

    var resourcePassword = null;
    var valAutoGeneratePassword = this.clsApp.clsPreferences.getLocalPreferences("Login/AutoGenerateResourcePassword", "true");
    if (valAutoGeneratePassword == "false") {
      resourcePassword = this.clsApp.clsPreferences.getLocalPreferences("Login/resourcePassword", "");
      if (resourcePassword == "") {
        resourcePassword = this.clsApp.clsPreferences.getLocalPreferences("Asset/AssetPrivateKeydata", "");
      }

      this.chkAutoPassword.setChecked(jsx3.gui.CheckBox.UNCHECKED);
      this.txtClientPassword.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.txtClientPassword.setValue(resourcePassword);
      this.btnChkAutoPassword.setState(jsx3.gui.ImageButton.STATE_OFF);
    }
    else {
      resourcePassword = getHWInfo_HDVolumeSerial(this.clsApp.getBrowserType());
      this.chkAutoPassword.setChecked(jsx3.gui.CheckBox.CHECKED);
      this.txtClientPassword.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.txtClientPassword.setValue("");
      this.btnChkAutoPassword.setState(jsx3.gui.ImageButton.STATE_ON);
    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_AUTOGENERATEPASSWORD, valAutoGeneratePassword);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE_PASSWORD, resourcePassword);


    var loginServer = this.clsApp.clsPreferences.getLocalPreferences("LastLoginServer", "");
    if (loginServer == "") {
      loginServer = this.clsApp.clsPreferences.getLocalPreferencesNode("Login/ServerList/Server", DefaultLoginServer).getAttribute("Value");
    }
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVER, loginServer);
    var serverList = this.clsApp.clsPreferences.getLocalPreferencesNode("ServerList");
    if (serverList != null) {
      var iterServers = serverList.selectNodeIterator(".//Server");
      while (iterServers.hasNext()) {
        var nodeServer = iterServers.next();
        var serverName = nodeServer.getAttribute("Name");
        var serverValue = nodeServer.getAttribute("Value");
        if (jsx3.util.strEmpty(serverName)) {
          serverName = serverValue;
        }
        var record = { jsxid: serverValue, jsxtext: serverName, jsxvalue: serverValue };
        this.cmbLoginServer.insertRecord(record);
      }
      var firstServerNode = serverList.getFirstChild();
      var defaultServer = firstServerNode.getAttribute("Value");
      if (!jsx3.util.strEmpty(defaultServer)) {
        this.cmbLoginServer.setValue(defaultServer);
      }
    }

    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PORT, this.clsApp.clsPreferences.getLocalPreferences("Login/port", DefaultLoginPort));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVICEDOMAIN, this.clsApp.clsPreferences.getLocalPreferences("Login/servicedomain", DefaultServiceDomain));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEKEYSCHEME, this.clsApp.clsPreferences.getLocalPreferences("AssetPrivateKeyScheme", "SERVER"));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEKEYDATA, resourcePassword);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEDATA, this.clsApp.clsPreferences.getLocalPreferences("AssetPrivateData", ""));

    if (!jsx3.util.strEmpty(loginUname) && !jsx3.util.strEmpty(AssetResourceTitle) && !jsx3.util.strEmpty(loginServer) && (valAutoGeneratePassword == "true" || (valAutoGeneratePassword == "false" && !jsx3.util.strEmpty(this.txtClientPassword.getValue())))) {
      //Disabled the advance setting block for subsequent login
      this._handleBlock_AdvanceSettings(false);
    }
    this._viewState = true;
    this._subscribeEnterKeyAction();
  }

  /** @private @jsxobf-clobber */
  Login_prototype._toggle = function (objEvent) {
    if (this.clsApp.getMenuSelection() == "NONE") {
      this.cmpShow();
      this._LOG.info("Login: Show");
    }
    else {
      this.cmpHide();
      this._LOG.info("Login: Hide");
    }
  }

  Login_prototype._onLoginStateChange = function (objEvent) {
    if (objEvent.stateVar != Login.SERVICEAGENT.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch (stateVal) {
      case Login.SERVICEAGENT.LOGIN_SUCCESS:
        //Handle F5 Key,Back,ReLoad Icon
        this._unsubscribeEnterKeyAction();
        this.publish({ subject: "LoginStatus", eventVal: "" });
        this._clearCacheServiceRoot();
        this._handleAssetResource(this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_ASSETRESOURCE));
        // check for app server jid - is returned by set service domain
        var serviceContactJID = this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_APPSERVERAGENTJID);
        if (serviceContactJID != null && serviceContactJID != '') {
          this._LOG.info("ServiceLogin: App Server JID: " + serviceContactJID);
          return;
        }
        break;
      case Login.SERVICEAGENT.LOGIN_LOGOUT_SUCCESS:
        if (this.clsApp.clsServiceAgent.getStateParam(Login.SERVICEAGENT.LOGINERRORCODE) == "NONE") {
          this.publish({ subject: "LoginStatus", eventVal: "" });
          this._handleLoginButton("Disable");
          this._handleBlock_AdvanceSettings(false);
          this._subscribeEnterKeyAction();
        }
        this.txtPassword.setValue("");
        //destroyInActivityTimeout();
        //clean Dialog if there is
        if (jsx3.gui.Dialog) {
          try {
            var DialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
            for (var i = 0; i < DialogBoxArr.length; i++) {
              var BlkJSX = DialogBoxArr[i];
              BlkJSX.doClose();
            }
          } catch (e) {
            this._LOG.warn("Login:LoginState - LOGOUT:clean Dialog:" + e.description);
          }
        }
        //clean Window if there is
        if (jsx3.gui.Window) {
          try {
            var arrWindow = PNameSpace.WINDOWS.getChildren();
            jsx3.$A(arrWindow).each(function (x) {
              //give a 20 ms delay for trigger each window close
              setTimeout(function () { x.close(); }, 20);
            });
          } catch (e) {
            this._LOG.warn("Login:LoginState - LOGOUT:clean Window:" + e.description);
          }
        }
        break;
      default:
        return;
    }
  }

  Login_prototype._handleAssetResource = function (assetResource) {
    var respAssetResource = assetResource.selectSingleNode("//AssetResource");
    if (respAssetResource != null) {
      var nodeAssetResourceTitle = respAssetResource.selectSingleNode(".//Info/Title");
      if (nodeAssetResourceTitle != null) {
        this.clsApp.clsPreferences.setLocalPreferences("AssetResourceTitle", nodeAssetResourceTitle.getValue());
      }
      var nodeAssetPrivateKeyScheme = respAssetResource.selectSingleNode(".//AssetPrivateKey/KeyScheme");
      if (nodeAssetPrivateKeyScheme != null) {
        var assetPrivateKeyScheme = nodeAssetPrivateKeyScheme.getValue();
        this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateKeyScheme", assetPrivateKeyScheme);
        if (assetPrivateKeyScheme == "SERVER") {
          var nodeAssetPrivateKeyData = respAssetResource.selectSingleNode(".//AssetPrivateKey/KeyData");
          if (nodeAssetPrivateKeyData != null) {
            this.clsApp.clsPreferences.setLocalPreferences("AssetPrivateKeyData", nodeAssetPrivateKeyData.getValue());
          }
        }
      }
      var nodeAssetPrivateData = respAssetResource.selectSingleNode(".//AssetPrivateData");
      if (nodeAssetPrivateData != null) {
        var strAssetPrivateData = nodeAssetPrivateData.toString();
        strAssetPrivateData = strAssetPrivateData.replace(/\&lt;/g, '<');
        strAssetPrivateData = strAssetPrivateData.replace(/\&gt;/g, '>');
        var nodeAssetPrivateDataXML = (new jsx3.xml.Document()).loadXML(strAssetPrivateData.replace(/[\n\r]/g, " "));
        for (var iter = nodeAssetPrivateDataXML.getChildNodes().iterator(); iter.hasNext(); ) {
          var childNode = iter.next();
          var objValue = childNode.getValue();
          var objName = childNode.getNodeName();
          this.clsApp.clsPreferences.setLocalPreferences(objName, objValue);
        }
      }
    }
    return;
  }

  Login_prototype._onLoginStateError = function (objEvent) {
    if (objEvent.stateVar != Login.SERVICEAGENT.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch (stateVal) {
      case Login.SERVICEAGENT.LOGIN_XMPP_DUPLICATE:
        this._handleServiceStateErrors("Duplicate Login is not supported.\n\nPlease exit the application and restart.");
        break;
      case Login.SERVICEAGENT.LOGIN_XMPP_ERROR:
        this._handleServiceStateErrors("The user name and password you entered did not match.", this.txtLogin);
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_VERSION_ERROR:
        // logout the user
        this._handleServiceStateErrors("Service Version mismatch.\n\nPlease download the latest version of the client software.");
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_CLIENTINUSE_ERROR:
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Invalid client password or client is already in use.", this.txtClientPassword);
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_CLIENTLOGGEDIN_ERROR:
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Client is already in use.", this.txtLoginResource);
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_CLIENTINVALID_ERROR:  //1026
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Invalid client name.", this.txtLoginResource);
        break;
      case Login.SERVICEAGENT.LOGIN_SETSERVICE_INVALIDCLIENT_ERROR: //1037
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Cannot login using this client name.", this.txtLoginResource);
        break;
      case Login.SERVICEAGENT.LOGIN_INVALID_SERVER_IPADDRESS: //1040
        this._clearCacheServiceRoot();
        this._handleServiceStateErrors("Unable to connect to server:" + this.cmbLoginServer.getValue(), this.cmbLoginServer);
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
        this._handleServiceStateErrors("The maximum number of concurrent user licenses has been exceeded.Please contact your network administrator.");
        break;
      case Login.SERVICEAGENT.RETRYLOGIN_FAILED: //Login failed due to any reasong while retrying login
        this._handleServiceStateErrors("Retry Login Failed.");
        break;
      default:
        this._handleServiceStateErrors("Unknown error. Error Code:" + stateVal + ". Please try again.");
        return;
    }
  }

  Login_prototype._handleServiceStateErrors = function (alertMsg, objTextBox) {
    this.txtPassword.setValue("");
    if (!jsx3.util.strEmpty(alertMsg)) {
      this.btnLoginDsbl.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.btnLogin.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      if (!jsx3.util.strEmpty(objTextBox)) {
        if (objTextBox.getName() == "txtClientName" || objTextBox.getName() == "txtClientPassword" || objTextBox.getName() == "cmbLoginServer") {
          if (objTextBox.getName() == "txtClientPassword") {
            this.btnChkAutoPassword.setState(jsx3.gui.ImageButton.STATE_OFF);
            this._onClick_AutoPassword();
          }
          this._handleBlock_AdvanceSettings(true);
        }
        else {
          this._handleBlock_AdvanceSettings(false);
        }
      }
      this._onError_HandleSetError(alertMsg, objTextBox);
    }
  }

  Login_prototype._handleBlock_AdvanceSettings = function (expandState) {
    this.btnSettings.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    if (expandState) {
      this.blkSettingsCnt.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.btnSettings.setState(jsx3.gui.ImageButton.STATE_ON);
      this.setWidth("550");
    }
    else {
      this.blkSettingsCnt.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.btnSettings.setState(jsx3.gui.ImageButton.STATE_OFF);
      this.setWidth("275");
    }
    this.repaint();
    this.blkMask.setWidth("100%");
    this.blkMask.repaint();
  }

  Login_prototype._onClick_AdvancedSettings = function () {
    var blkExpandState = (this.btnSettings.getState() == jsx3.gui.ImageButton.STATE_ON) ? true : false;
    this._handleBlock_AdvanceSettings(blkExpandState);
  }

  Login_prototype._onError_HandleSetError = function (erMsg, objTextBox) {
    //Disabled login button when an error occurs and highlighted the lable message with red border
    this._handleLoginButton("Disable");
    this.lblMessage.setText(erMsg, true);
    this.lblMessage.setDynamicProperty("jsxcolor", "errorText");
    this.lblMessage.repaint();
    if (!jsx3.util.strEmpty(objTextBox)) {
      this.errorObjName = objTextBox.getName();
      //cleared content in textbox and highlighted with red border
      objTextBox.setValue("");
      objTextBox.setDynamicProperty("jsxborder", "textfieldError");
      //Before setting border, we need to set dynamic property for jsxborder since setBorder does not work without it.
      objTextBox.setBorder("solid 1px #cc3333;solid 1px #cc3333;solid 1px #cc3333;solid 1px #cc3333", true);
    }
  }

  Login_prototype._onError_HandleResetError = function (objTextBox) {
    //reset label color and text
    if (jsx3.util.strEmpty(objTextBox) || jsx3.util.strEmpty(this.errorObjName) || (!jsx3.util.strEmpty(objTextBox) && objTextBox.getName() == this.errorObjName)) {
      this.lblMessage.setText("");
      this.lblMessage.setDynamicProperty("jsxcolor", "highlightText");
      this.lblMessage.repaint();
    }
    //reset the border if this is called from the same object which has error.
    if (!jsx3.util.strEmpty(objTextBox) && objTextBox.getName() == this.errorObjName) {
      this.errorObjName = null;
      objTextBox.setDynamicProperty("jsxborder", "textfield");
      objTextBox.setBorder("solid 1px #777777;solid 1px #777777;solid 1px #777777;solid 1px #777777", true);
    }
  }

  Login_prototype._clearCacheServiceRoot = function () {
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

  Login_prototype._onClick_AutoLogin = function () {
    var isChecked = (this.btnChkAutoLogin.getState() == jsx3.gui.ImageButton.STATE_ON) ? jsx3.gui.CheckBox.CHECKED : jsx3.gui.CheckBox.UNCHECKED;
    this.chkAutoLogin.setChecked(isChecked);
  }

  Login_prototype._onClick_CancelRetryLogin = function () {
    this._cancelRetryLogin();
    this.clsApp.clsServiceAgent.cancelRetryLogin();
  }

  Login_prototype._cancelRetryLogin = function () {
    this._onError_HandleResetError(null);
    this.btnCancelRetryLogin.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkMask.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  Login_prototype._onRetryLogin = function (objEvent) {
    if (objEvent.stateVar != Login.SERVICEAGENT.RETRYLOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    if (stateVal == Login.SERVICEAGENT.LOGIN_NONE) {
      this._cancelRetryLogin();
    }
    else if (stateVal == Login.SERVICEAGENT.RETRYLOGIN_START) {
      this._onError_HandleSetError("Session terminated. Retrying Login ... (30)");
      this.txtLogin.setValue(this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_RETRYUSER));
      this.txtPassword.setValue(this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_RETRYPASSWORD));
      this.blkMask.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.btnCancelRetryLogin.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
    else if (stateVal == Login.SERVICEAGENT.RETRYLOGIN_END) {
      this._cancelRetryLogin();
      this._onClick_Login();
    }
    else {
      this._onError_HandleSetError("Session terminated. Retrying Login ... (" + stateVal + ")");
    }
  }

  Login_prototype._validateOnLogin = function () {
    var errMsg = null;
    var objComp = null;
    //check if login is initiated without resetting previous error
    if (!jsx3.util.strEmpty(this.errorObjName)) {
      if (this.errorObjName == "txtClientName" || this.errorObjName == "txtClientPassword" || this.errorObjName == "cmbLoginServer") {
        this._handleBlock_AdvanceSettings(true);
      }
      return false;
    }

    // if the ip address value is not there then get it
    if (jsx3.util.strEmpty(this.txtLoginInterfaceIP.getValue())) {
      var loginInterfaceMacAddress = this.selLoginInterface.getValue();
      if (jsx3.util.strEmpty(loginInterfaceMacAddress) || loginInterfaceMacAddress == "DefaultInterface") {
        var savedLoginInterfaceMacAddress = this.clsApp.clsPreferences.getLocalPreferences("Login/InterfaceMacAddress", DefaultLoginInterfaceMacAddress);
        if (savedLoginInterfaceMacAddress == "DefaultInterface" || jsx3.util.strEmpty(savedLoginInterfaceMacAddress)) {
          this._getLocalInterfaces();
        }
      }
    }

    if (jsx3.util.strEmpty(this.cmbLoginServer.getValue())) {
      errMsg = "Please enter a Server IP";
      objComp = this.cmbLoginServer;
      this._handleBlock_AdvanceSettings(true);
    }
    else if (jsx3.util.strEmpty(this.txtLogin.getValue())) {
      errMsg = "Please enter a Username";
      objComp = this.txtLogin;
    }
    else if (jsx3.util.strEmpty(this.txtPassword.getValue())) {
      errMsg = "Please enter a Password";
      objComp = this.txtPassword;
    }
    else if (jsx3.util.strEmpty(this.txtLoginResource.getValue())) {
      errMsg = "Please enter a Client Name";
      objComp = this.txtLoginResource;
      this._handleBlock_AdvanceSettings(true);
    }
    else if (this.chkAutoPassword.getChecked() == jsx3.gui.CheckBox.UNCHECKED && jsx3.util.strEmpty(this.txtClientPassword.getValue())) {
      errMsg = "Please enter a Client Password";
      objComp = this.txtClientPassword;
      this._handleBlock_AdvanceSettings(true);
    }
    else if (jsx3.util.strEmpty(this.txtLoginInterfaceIP.getValue())) {
      errMsg = "Please select default network from the drop down";
      objComp = this.selLoginInterface;
      this._handleBlock_AdvanceSettings(true);
    }
    if (!jsx3.util.strEmpty(errMsg)) {
      this._onError_HandleSetError(errMsg, objComp);
      return false;
    }
    else {
      //To fix Multiple errors like Max media clients exceeded and Service version mismatch ...
      this._onError_HandleResetError(null);
    }
    return true;
  }

  Login_prototype._onClick_Login = function () {
    if (this.clsApp.clsApplet.getAppletStatus() != "LOADED") {
      this.lblMessage.setText("Initializing .. Please wait and retry", true)
      return false;
    }

    //check if retry login is in progerss.
    if (this.clsApp.clsServiceAgent.getStateParam(Login.SERVICEAGENT.RETRYLOGINSTATE) == Login.SERVICEAGENT.RETRYLOGIN_INPROGRESS) return;

    if (this._validateOnLogin() != true) return;
    var valLogin = this.txtLogin.getValue();
    var valPassword = this.txtPassword.getValue();
    var valLoginResource = this.txtLoginResource.getValue();
    var valLoginServer = this.cmbLoginServer.getValue();
    var valResourcePassword = this.txtClientPassword.getValue();
    if (this.chkAutoPassword.getChecked() == jsx3.gui.CheckBox.CHECKED) {
      valResourcePassword = getHWInfo_HDVolumeSerial(this.clsApp.getBrowserType());
    }
    valLogin = valLogin.toLowerCase();
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_USER, valLogin);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PASSWORD, valPassword);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_DOMAIN, this.clsApp.clsPreferences.getLocalPreferences("Login/domain", DefaultLoginDomain));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE, valLoginResource);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVER, valLoginServer);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_PORT, this.clsApp.clsPreferences.getLocalPreferences("Login/port", DefaultLoginPort));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVICEDOMAIN, this.clsApp.clsPreferences.getLocalPreferences("Login/servicedomain", DefaultServiceDomain));
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_ASSETPRIVATEKEYDATA, valResourcePassword);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_RESOURCE_PASSWORD, valResourcePassword);

    var autoLogin = "false";
    if (this.chkAutoLogin.getChecked() == jsx3.gui.CheckBox.CHECKED) {
      autoLogin = "true";
      this.clsApp.clsPreferences.setLocalPreferences("Login/password", valPassword);
    }
    else {
      this.clsApp.clsPreferences.setLocalPreferences("Login/password", "");
    }
    var autoPassword = "false";
    if (this.chkAutoPassword.getChecked() == jsx3.gui.CheckBox.CHECKED) {
      autoPassword = "true";
    }
    var loginInterfaceMacAddress = this.selLoginInterface.getValue();
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_AUTOGENERATEPASSWORD, autoPassword);
    this.clsApp.clsPreferences.setLocalPreferences("Login/AutoGenerateResourcePassword", autoPassword);
    this.clsApp.clsPreferences.setLocalPreferences("Login/resourcePassword", valResourcePassword);
    this.clsApp.clsPreferences.setLocalPreferences("Asset/AssetPrivateKeyData", valResourcePassword);
    this.clsApp.clsPreferences.setLocalPreferences("Login/InterfaceMacAddress", loginInterfaceMacAddress);
    this.clsApp.clsPreferences.setLocalPreferences("Login/AutoLogin", autoLogin);
    this.clsApp.clsPreferences.setLocalPreferences("LastLoginUsername", valLogin);
    this.clsApp.clsPreferences.setLocalPreferences("LastLoginServer", valLoginServer);
    this.clsApp.clsPreferences.setLocalPreferences("Login/user", valLogin);
    this.clsApp.clsPreferences.setLocalPreferences("Login/resource", valLoginResource);

    //Update the Server list if new server ip is encountered
    var newServerListNode = this.clsApp.clsPreferences.getLocalPreferencesNode("ServerList").cloneNode(true);
    if (!jsx3.util.strEmpty(newServerListNode)) {
      var serverNode = newServerListNode.selectSingleNode("./Server[@Value ='" + valLoginServer + "']");
      var serverName = "";
      if (serverNode != null) {
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
      this.clsApp.clsPreferences.setLocalPreferencesNode("Login/ServerList", newServerListNode);
    }
    this.publish({ subject: "LoginStatus", eventVal: "Logging in..." });

    jsx3.sleep((function () {
      this.clsApp.clsServiceAgent.doLogin();
    }), null, this);
    return true;
  }

  Login_prototype._appletStatus = function (objEvent) {
    if (objEvent.subject == "AppletStatus") {
      var appletStatus = this.clsApp.clsApplet.getAppletStatus();
      var loginStatus = appletStatus;
      if (appletStatus == "LOADING") {
        loginStatus = "Loading...";
      }
      else if (appletStatus == "LOADED") {
        //To avoid white blank screen before login page being loaded
        this._getLocalInterfaces();
        this.blkMask.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SERVICEVERSION, this.clsApp.clsApplet.getServiceVersion());
        this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_SOFTWAREVERSION, this.clsApp.getVersion());
        loginStatus = " ";
        var autoLogin = this.clsApp.clsPreferences.getLocalPreferences("Login/AutoLogin", "false");
        if (autoLogin == "true") {
          this.chkAutoLogin.setChecked(jsx3.gui.CheckBox.CHECKED);
          this.btnChkAutoLogin.setState(jsx3.gui.ImageButton.STATE_ON);
          this.txtPassword.setValue(this.clsApp.clsServiceAgent.getLoginParam(Login.SERVICEAGENT.LOGIN_PASSWORD));
          this._onClick_Login();
          return;
        }
        else {
          this.chkAutoLogin.setChecked(jsx3.gui.CheckBox.UNCHECKED);
          this.btnChkAutoLogin.setState(jsx3.gui.ImageButton.STATE_OFF);
        }
      }
      this.publish({ subject: "LoginStatus", eventVal: loginStatus });
    }
  }

  Login_prototype._handleLoginButton = function (objState) {
    if (objState == "Enable") {
      this.btnLogin.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.btnLoginDsbl.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    else {
      this.btnLoginDsbl.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.btnLogin.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
  }

  Login_prototype._onFocus_HandleLoginButton = function (objTextBox) {
    this._onError_HandleResetError(objTextBox);
    var loginBtnState = "Enable";
    if (objTextBox.getName() != "txtLogin" && jsx3.util.strEmpty(this.txtLogin.getValue())) {
      loginBtnState = "Disable";
    }
    else if (objTextBox.getName() != "txtPassword" && jsx3.util.strEmpty(this.txtPassword.getValue())) {
      loginBtnState = "Disable";
    }
    else if (objTextBox.getName() != "txtClientName" && jsx3.util.strEmpty(this.txtLoginResource.getValue())) {
      loginBtnState = "Disable";
    }
    else if (objTextBox.getName() != "txtClientPassword" && jsx3.util.strEmpty(this.txtClientPassword.getValue()) && this.chkAutoPassword.getChecked() == jsx3.gui.CheckBox.UNCHECKED) {
      loginBtnState = "Disable";
    }
    else if (objTextBox.getName() != "cmbLoginServer" && jsx3.util.strEmpty(this.cmbLoginServer.getValue())) {
      loginBtnState = "Disable";
    }
    else if (objTextBox.getName() != "selLoginInterface" && jsx3.util.strEmpty(this.txtLoginInterfaceIP.getValue())) {
      loginBtnState = "Disable";
    }
    
    this._handleLoginButton(loginBtnState);
  }

  Login_prototype._onFocus_Login = function () {
    //validating login button state
    this._onFocus_HandleLoginButton(this.txtLogin);
  }

  Login_prototype._onFocus_Password = function () {
    //validating login button state
    this._onFocus_HandleLoginButton(this.txtPassword);
  }

  Login_prototype._onFocus_LoginResource = function () {
    //validating login button state
    this._onFocus_HandleLoginButton(this.txtLoginResource);
  }

  Login_prototype._onFocus_ClientPassword = function () {
    //validating login button state
    this._onFocus_HandleLoginButton(this.txtClientPassword);
  }

  Login_prototype._onSelect_ServerIP = function () {
    //validating login button state
    this._onFocus_HandleLoginButton(this.cmbLoginServer);
  }

  Login_prototype._handleLabelMsg = function (objEvent) {
    if (objEvent.subject == "LoginStatus") {
      this.lblMessage.setText(objEvent.eventVal, true);
    }
  }

  Login_prototype._subscribeEnterKeyAction = function () {
    jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, this, this._handleEnterKeyAction);
  }

  Login_prototype._unsubscribeEnterKeyAction = function () {
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, this, this._handleEnterKeyAction);
  }

  Login_prototype._handleEnterKeyAction = function (objEvent) {
    if (objEvent.event.enterKey()) {
      this._onClick_Login();
    }
  }

  Login_prototype._onClick_AutoPassword = function () {
    if (this.btnChkAutoPassword.getState() == jsx3.gui.ImageButton.STATE_ON) {
      this.chkAutoPassword.setChecked(jsx3.gui.CheckBox.CHECKED);
      this.txtClientPassword.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.txtClientPassword.setValue("");
      this._onFocus_HandleLoginButton(this.txtClientPassword);
    }
    else {
      this.chkAutoPassword.setChecked(jsx3.gui.CheckBox.UNCHECKED);
      this.txtClientPassword.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  Login_prototype._onSelect_LoginInterface = function () {
    var selInterface = this.selLoginInterface.getValue();
    if (jsx3.util.strEmpty(selInterface)) return;
    var strXMLCacheId = this.selLoginInterface.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (jsx3.util.strEmpty(objXML)) return;
    var record = objXML.selectSingleNode("//record[@jsxid='" + selInterface + "']");
    var ipaddress = record.getAttribute("jsxIPAddress");
    if (ipaddress == "DefaultIPAddress") {
      ipaddress = "";
    }
    this.txtLoginInterfaceIP.setValue(ipaddress, true);
    this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_CLIENTIPADDRESS, ipaddress);
    //validating login button state
    this._onFocus_HandleLoginButton(this.selLoginInterface);
  }

  Login_prototype._getLocalInterfaces = function () {
    var strXMLCacheId = this.selLoginInterface.getXMLId();
    var objXML11 = PNameSpace.getCache().getDocument(strXMLCacheId);
    this._LOG.info("Login::getLocalInterfaces:: BEFORE REMOVE CHILDREN::XML:" + objXML11);
    objXML11.removeChildren();
    var strRecordid = "DefaultInterface";
    var interfaceObjArr = getHWInfo_InterfaceList(this.clsApp.getBrowserType());
    for (var interfaceObj in interfaceObjArr) {
      if (!jsx3.util.strEmpty(interfaceObjArr[interfaceObj].MACADDRESS) && !jsx3.util.strEmpty(interfaceObjArr[interfaceObj].DESCRIPTION) && !jsx3.util.strEmpty(interfaceObjArr[interfaceObj].IPADDRESS)) {
        var o = new Object();
        o.jsxid = interfaceObjArr[interfaceObj].MACADDRESS;
        o.jsxtext = interfaceObjArr[interfaceObj].DESCRIPTION;
        o.jsxIPAddress = interfaceObjArr[interfaceObj].IPADDRESS;
        this._LOG.info("Login::getLocalInterfaces:: VALID ENTRY::" + " MACADDRESS::" + o.jsxid + " DESCRIPTION::" + o.jsxtext + " IPADDRESS::" + o.jsxIPAddress);
        this.selLoginInterface.insertRecord(o, null, false);
      }
    }

    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    this._LOG.info("Login::getLocalInterfaces:: cmpSelLoginInterface:: NEW DATA XML:" + objXML);

    var interfaceMACAddress = this.clsApp.clsPreferences.getLocalPreferences("Login/InterfaceMacAddress", DefaultLoginInterfaceMacAddress);
    this._LOG.info("Login::getLocalInterfaces:: interfaceMACAddress:" + interfaceMACAddress);
    if (!jsx3.util.strEmpty(interfaceMACAddress)) {
      if (!jsx3.util.strEmpty(objXML)) {
        var firstServerNode = objXML.selectSingleNode("//record[@jsxid='" + interfaceMACAddress + "']");
        if(jsx3.util.strEmpty(firstServerNode)) firstServerNode = objXML.getFirstChild();
        this._LOG.info("Login::getLocalInterfaces:: firstServerNode:::" + firstServerNode);
        if (jsx3.util.strEmpty(firstServerNode)) return;
        var defaultInterface = firstServerNode.getAttribute("jsxid");
        this._LOG.info("Login::getLocalInterfaces:: defaultInterface:::" + defaultInterface);
        if (!jsx3.util.strEmpty(defaultInterface)) {
          strRecordid = defaultInterface;
          //get the ip address
          //var ipaddress = getHWInfo_IPAddressOfMacAddress(strRecordid, this.clsApp.getBrowserType());
          var ipaddress = firstServerNode.getAttribute("jsxIPAddress");
          this._LOG.info("Login::getLocalInterfaces:: ipaddress:::" + ipaddress);
          if (jsx3.util.strEmpty(ipaddress) || ipaddress == "0.0.0.0") {
            ipaddress = getHWInfo_IPAddress(this.clsApp.getBrowserType());
          }
          this._LOG.info("Login::getLocalInterfaces:: SET ipaddress:::" + ipaddress);
          this.clsApp.clsServiceAgent.setLoginParam(Login.SERVICEAGENT.LOGIN_CLIENTIPADDRESS, ipaddress);
          this.txtLoginInterfaceIP.setValue(ipaddress, true);
        }
      }
    }
    this._LOG.info("Login::getLocalInterfaces:: BEFORE SET VALuE to selLoginInterface:: strRecordid:" + strRecordid);
    this.selLoginInterface.setValue(strRecordid);
    this.selLoginInterface.repaint();
  }
});