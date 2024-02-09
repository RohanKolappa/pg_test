/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Ipvs class which handles Login process;
* <p/>
* This class uses com.ipvs.api.Request;
* <p/>
* login class publish the ON_LOGINSTATE_CHANGE / ON_LOGIN_ERROR / ON_LOGOUT_ERROR events;
*/
jsx3.Class.defineClass("com.ipvs.agent.ServiceAgent", null, [jsx3.util.EventDispatcher], function (ServiceAgent, ServiceAgent_prototype) {

  /** @private @jsxobf-clobber */
  ServiceAgent._LOG = jsx3.util.Logger.getLogger("com.ipvs.agent.ServiceAgent");

  //LOGINSTATE:holds actual login state
  ServiceAgent.LOGINSTATE = "LoginState";
  ServiceAgent.LOGINERRORCODE = "LoginErrorCode";

  /**
  * {String} Event type published when Login/Logout successed .
  * @final @jsxobf-final
  */
  ServiceAgent.ON_LOGINSTATE_CHANGE = "OnLoginStateChange";
  /**
  * {String} Event type published when an error occers during login process.
  * @final @jsxobf-final
  */
  ServiceAgent.ON_LOGIN_ERROR = "OnLoginError";
  /**
  * {String} Event type published when an error occers during logout process.
  * @final @jsxobf-final
  */
  ServiceAgent.ON_LOGOUT_ERROR = "OnLogoutError";
  /**
  * {String} Event type published during retry login process.
  * @final @jsxobf-final
  */
  ServiceAgent.ON_LOGIN_RETRY = "OnLoginRetry";
  ServiceAgent.RETRYLOGINSTATE = "RetryLoginState";

  ServiceAgent.LOGIN_NONE = "NONE";
  ServiceAgent.LOGIN_SUCCESS = "VALID";
  ServiceAgent.LOGIN_ERROR = "ERROR";
  ServiceAgent.LOGIN_INPROGRESS = "InProgress";
  ServiceAgent.LOGIN_XMPP_SUCCESS = "XMPP_LOGIN_VALID";

  ServiceAgent.LOGIN_XMPP_ERROR = "1003";
  ServiceAgent.LOGIN_XMPP_DUPLICATE = "1004";
  ServiceAgent.LOGIN_INVALID_SERVER_IPADDRESS = "1040";
  ServiceAgent.LOGIN_SETSERVICE_VERSION_ERROR = "1200";
  ServiceAgent.LOGIN_SETSERVICE_CLIENTINVALID_ERROR = "1026";
  ServiceAgent.LOGIN_SETSERVICE_CLIENTINUSE_ERROR = "1027";
  ServiceAgent.LOGIN_SETSERVICE_INVALIDCLIENT_ERROR = "1037";
  ServiceAgent.LOGIN_SETSERVICE_APPSERVERNOTFOUND_ERROR = "2800";
  ServiceAgent.LOGIN_MAX_CLIENTS_EXCEEDED_ERROR = "1302";
  ServiceAgent.LOGIN_SETSERVICE_CLIENTLOGGEDIN_ERROR = "1303";

  ServiceAgent.LOGIN_SETSERVICE_SUCCESS = "VERSION_SUCCESS";
  ServiceAgent.LOGIN_GETSERVERTIME_ERROR = "GETSERVERTIME_ERROR";
  ServiceAgent.LOGIN_GETSERVICECLIENT_ERROR = "GETSERVICECLIENT_ERROR";
  ServiceAgent.LOGIN_GETSERVICECLIENT_SUCCESS = "GETSERVICECLIENT_SUCCESS";
  ServiceAgent.LOGIN_SERVICECLIENTDATA = "GetServiceClientData";

  ServiceAgent.LOGIN_LOGOUT_SUCCESS = "LOGOUT_SUCCESS";
  ServiceAgent.LOGIN_LOGOUT_ERROR = "LOGOUT_ERROR";

  ServiceAgent.RETRYLOGIN_START = "RetryLoginStart";
  ServiceAgent.RETRYLOGIN_INPROGRESS = "RetryLoginInProgress";
  ServiceAgent.RETRYLOGIN_END = "RetryLoginEnd";
  ServiceAgent.RETRYLOGIN_FAILED = "RetryLoginFailed";

  //login params
  ServiceAgent.LOGIN_USER = "LoginUser";
  ServiceAgent.LOGIN_PASSWORD = "LoginPassword";
  ServiceAgent.LOGIN_DOMAIN = "LoginDomain";
  ServiceAgent.LOGIN_CLIENTNAME = "ClientName";
  ServiceAgent.LOGIN_RESOURCE = "LoginResource";
  ServiceAgent.LOGIN_RESOURCE_PASSWORD = "LoginResourcePassowrd";
  ServiceAgent.LOGIN_SERVER = "LoginServer";
  ServiceAgent.LOGIN_PORT = "LoginPort";
  ServiceAgent.LOGIN_USERJID = "LoginUserJID";
  ServiceAgent.LOGIN_USERDOMAIN = "LoginUserDomain";
  ServiceAgent.LOGIN_SERVICEDOMAIN = "ServiceDomain";
  ServiceAgent.LOGIN_ASSETTEMPLATETITLE = "AssetTemplateTitle";
  ServiceAgent.LOGIN_ASSETPRIVATEKEYSCHEME = "AssetPrivateKeyScheme";
  ServiceAgent.LOGIN_ASSETPRIVATEKEYDATA = "AssetPrivateKeyData";
  ServiceAgent.LOGIN_ASSETPRIVATEDATA = "AssetPrivateData";
  ServiceAgent.LOGIN_SERVICEVERSION = "ServiceVersion";
  ServiceAgent.LOGIN_SOFTWAREVERSION = "SoftwareVersion";
  ServiceAgent.LOGIN_CLIENTIPADDRESS = "ClientIPAddress";
  ServiceAgent.LOGIN_APPNAME = "AppName";
  ServiceAgent.LOGIN_WEBCLIENTSERVER = "WebClientLoginServer";
  ServiceAgent.LOGIN_APPSERVERAGENTJID = "AppServerAgentJID";
  ServiceAgent.LOGIN_MSSERVERIP = "MSServerIP";
  //  ServiceAgent.LOGIN_ASSETRESOURCEXML = "AssetResource";
  ServiceAgent.LOGIN_ASSETRESOURCE = "AssetResource";
  ServiceAgent.LOGIN_PORTTITLE = "PortTitle";
  ServiceAgent.LOGIN_PORTSTREAMTYPE = "PortStreamType";
  //retry login params
  ServiceAgent.LOGIN_RETRYUSER = "LastLoginUser";
  ServiceAgent.LOGIN_RETRYPASSWORD = "LastLoginPassword";
  //DestNID for start a session
  ServiceAgent.LOGIN_ASSETRESOURCENID = "DestNID";

  /**
  * The instance initializer.
  * @param topicName {String} topic name which holds global variables.
  */
  ServiceAgent_prototype.init = function() {
    this._loginParams = [];
    this._loginStateParams = [];
    this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_NONE);
    this._setStateParam(ServiceAgent.LOGINERRORCODE, ServiceAgent.LOGIN_NONE);
    this._setStateParam(ServiceAgent.RETRYLOGINSTATE, ServiceAgent.LOGIN_NONE);
    this.setLoginParam(ServiceAgent.LOGIN_USERJID, ServiceAgent.LOGIN_NONE);
    this._retryLogin = null;
    this._retryTimerID = null;
    this._retryStartTime = null;
    this._DeltaServerLocalTime = 0;
    
    // get a handle to App and wait for app init
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    } 
  }
  ServiceAgent_prototype._onAppInit = function() {
    this.clsApp.clsNotification.subscribe(com.ipvs.api.Notification.MSG_PRESENCE, this, "_handlePresenceNotification");
  };

  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._setStateParam = function (paramName, paramValue) {
    this._loginStateParams[paramName] = paramValue;
  };

  /**
  * Returns the login state.
  * @param paramName {String}
  * @return {String} login state.
  */
  ServiceAgent_prototype.getStateParam = function (paramName) {
    return this._loginStateParams[paramName];
  };

  /**
  * Sets the login parameter value.;
  * @param paramName {String} 
  * @param paramValue {String}
  */
  ServiceAgent_prototype.setLoginParam = function (paramName, paramValue) {
    this._loginParams[paramName] = paramValue;
  };

  /**
  * Returns the login parameter value.
  * @param paramName {String}
  * @return {String} login parameter value.
  */
  ServiceAgent_prototype.getLoginParam = function (paramName) {
    return this._loginParams[paramName];
  };

  /**
  * Sets the value for multiple login parameters;
  * @param objLoginParams {Array};
  */
  ServiceAgent_prototype.setLoginParams = function (objLoginParams) {
    if (!jsx3.util.strEmpty(objLoginParams)) {
      for (var lpName in objLoginParams) {
        this.setLoginParam(lpName, objLoginParams[lpName]);
      }
    }
  };

  /**
  * Returns all login parameters;
  * @return {Array} Login parameters array.
  */
  ServiceAgent_prototype.getLoginParams = function () {
    return this._loginParams;
  };

  /**
  * Sets the value for DeltaServerLocalTime. Time different between ServerTime and LocalTime;
  * @param deltaServerLocalTime {String} in Milliseconds.
  */
  ServiceAgent_prototype._setDeltaServerLocalTime = function (deltaServerLocalTime) {
    this._DeltaServerLocalTime = deltaServerLocalTime;
  }

  /**
  * Returns DeltaServerLocalTime;
  * @return {String} in Milliseconds.
  */
  ServiceAgent_prototype.getDeltaServerLocalTime = function () {
    return this._DeltaServerLocalTime;
  }

  /**
  * Returns Current ServerTime;
  * @return {String} in Milliseconds.
  */
  ServiceAgent_prototype.getCurrentServerTime = function () {
    var CurrTime = (new Date()).getTime();
    var CurrServerTime = parseInt(CurrTime) - parseInt(this._DeltaServerLocalTime);
    return CurrServerTime;
  }

  /**
  * Returns the login status;
  * @return {boolean} true/false.
  */
  ServiceAgent_prototype.isLoggedIn = function () {
    var loginState = this.getStateParam(ServiceAgent.LOGINSTATE);
    if (loginState == ServiceAgent.LOGIN_SUCCESS || loginState == ServiceAgent.LOGIN_LOGOUT_ERROR) {
      return true;
    }
    else {
      return false;
    }
  }

  /**
  * Initiates login process and sends Login/SetServiceDomain/getServerTime/logout requests.
  * Publish event ON_LOGIN_ERROR in case of login error with error code.
  * Need to set all login parameters before calling this method.
  */
  ServiceAgent_prototype.doLogin = function () {
    //check if login is already in progress
    if (this.getStateParam(ServiceAgent.LOGINSTATE) == ServiceAgent.LOGIN_INPROGRESS) {
      ServiceAgent._LOG.info("Login process is in progress ....");
      return;
    }
    this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_INPROGRESS);
    this.publish({ subject: ServiceAgent.ON_LOGINSTATE_CHANGE,
      stateVar: ServiceAgent.LOGINSTATE,
                  stateVal:ServiceAgent.LOGIN_INPROGRESS});

    var User = this.getLoginParam(ServiceAgent.LOGIN_USER);
    var Password = this.getLoginParam(ServiceAgent.LOGIN_PASSWORD);
    var Domain = this.getLoginParam(ServiceAgent.LOGIN_DOMAIN);
    var Resource = this.getLoginParam(ServiceAgent.LOGIN_RESOURCE);
    var Server = this.getLoginParam(ServiceAgent.LOGIN_SERVER);
    var Port = this.getLoginParam(ServiceAgent.LOGIN_PORT);
    com.ipvs.api.API.Login(User, Password, Domain, Resource, Server, Port).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        respStatus = ServiceAgent.LOGIN_XMPP_SUCCESS;
        this.setLoginParam(ServiceAgent.LOGIN_USERJID, respVal);
        var LoginUserDomain = respVal.substring(respVal.indexOf("@") + 1, respVal.indexOf("/"));
        this.setLoginParam(ServiceAgent.LOGIN_USERDOMAIN, LoginUserDomain);
      }

      if (respStatus == ServiceAgent.LOGIN_XMPP_SUCCESS) {
        ServiceAgent._LOG.info("Xmpp Login Success.");
        this._setStateParam(ServiceAgent.LOGINERRORCODE, ServiceAgent.LOGIN_NONE);
        this._setServiceDomain();
      }
      else {
        ServiceAgent._LOG.info("Xmpp Login error. Error code:" + respStatus);
        this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_ERROR);
        this._setStateParam(ServiceAgent.LOGINERRORCODE, respStatus);
        if (this._retryLogin) respStatus = ServiceAgent.RETRYLOGIN_FAILED;
        this.publish({ subject: ServiceAgent.ON_LOGIN_ERROR,
          stateVar: ServiceAgent.LOGINSTATE,
                      stateVal:respStatus});
        if (this._retryLogin) {
          this._initRetryLogin();
        }
      }
    }).bind(this));
  }

  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._setServiceDomain = function () {

    var ServiceDomain = this.getLoginParam(ServiceAgent.LOGIN_SERVICEDOMAIN);
    var AssetTemplateTitle = this.getLoginParam(ServiceAgent.LOGIN_ASSETTEMPLATETITLE);
    var AssetPrivateKeyScheme = this.getLoginParam(ServiceAgent.LOGIN_ASSETPRIVATEKEYSCHEME);
    var AssetPrivateKeyData = this.getLoginParam(ServiceAgent.LOGIN_ASSETPRIVATEKEYDATA);
    var AssetPrivateData = this.getLoginParam(ServiceAgent.LOGIN_ASSETPRIVATEDATA);
    var ServiceVersion = this.getLoginParam(ServiceAgent.LOGIN_SERVICEVERSION);
    var SoftwareVersion = this.getLoginParam(ServiceAgent.LOGIN_SOFTWAREVERSION);
    var IPAddress = this.getLoginParam(ServiceAgent.LOGIN_CLIENTIPADDRESS);
    var AppName = this.getLoginParam(ServiceAgent.LOGIN_APPNAME);

    com.ipvs.api.API.SetServiceDomain(ServiceDomain, AssetTemplateTitle, AssetPrivateKeyScheme, AssetPrivateKeyData, AssetPrivateData, ServiceVersion, SoftwareVersion, IPAddress, AppName).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        respStatus = ServiceAgent.LOGIN_SETSERVICE_SUCCESS;
        var respAppServerAgentJID = response.selectSingleNode("//appServerAgentJID");
        if (respAppServerAgentJID != null) {
          var AppServerAgentJID = respAppServerAgentJID.getValue();
          this.setLoginParam(ServiceAgent.LOGIN_APPSERVERAGENTJID, AppServerAgentJID);
          var msIP = AppServerAgentJID.substring(AppServerAgentJID.indexOf("@") + 1, AppServerAgentJID.indexOf("/"));
          if (msIP == "") {
            msIP = AppServerAgentJID.substring(AppServerAgentJID.indexOf(".") + 1);
          }
          this.setLoginParam(ServiceAgent.LOGIN_MSSERVERIP, msIP);
        }
        var respAssetResource = response.selectSingleNode("//AssetResource");
        this.setLoginParam(ServiceAgent.LOGIN_ASSETRESOURCE, respAssetResource);
        //set AssetResourceNID as DestNID
        var respAssetResourceNID = respAssetResource.getAttribute('NID');
        this.setLoginParam(ServiceAgent.LOGIN_ASSETRESOURCENID, respAssetResourceNID);
      }

      if (respStatus == ServiceAgent.LOGIN_SETSERVICE_SUCCESS) {
        ServiceAgent._LOG.info("SetServiceDomain Success.");
        //###########################
        //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code
        //Once MW completes then uncomment the following code
        // #########################
//        if(this._retryLogin){
//          this.cancelRetryLogin();
//          this.clsApp.getDescendantsOfType("com.ipvs.ipv.MainPanel")[0].showConnectionMask(true, false);
//          return;
//        }
        this.cancelRetryLogin();
        this._getServiceClient();
      }
      else {
        ServiceAgent._LOG.info("SetServiceDomain error. Error code:" + respStatus);
        this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_ERROR);
        this._setStateParam(ServiceAgent.LOGINERRORCODE, respStatus);
        //check if retry login is in progress and error is '2800-AppServer not found' then continue retry login otherwise cancel retry process
        //Bug#43447
        if (this._retryLogin && respStatus == ServiceAgent.LOGIN_SETSERVICE_APPSERVERNOTFOUND_ERROR) {
          respStatus = ServiceAgent.RETRYLOGIN_FAILED
        }
        else {
          this.cancelRetryLogin();
        }
        this.publish({ subject: ServiceAgent.ON_LOGIN_ERROR,
          stateVar: ServiceAgent.LOGINSTATE,
                      stateVal:respStatus});
        this._logout();
      }
    }).bind(this));
  }



  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._getServiceClient = function () {
    var appServerAgentJID = this.getLoginParam(ServiceAgent.LOGIN_APPSERVERAGENTJID);
    var loginUserJid = this.getLoginParam(ServiceAgent.LOGIN_USERJID);
    com.ipvs.api.API.GetServiceClient(loginUserJid, appServerAgentJID).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        respStatus = ServiceAgent.LOGIN_GETSERVICECLIENT_SUCCESS;
        //set the permissions
        var permissionNode = response.selectSingleNode(".//Permissions");
        if (jsx3.util.strEmpty(permissionNode)) {
          var permissionsStr = '<Permissions><User add="true" delete="true" modify="true" read="true"/><Port add="true" delete="true" modify="true" read="true" viewfrom="true" streamto="true"/><Device add="true" delete="true" modify="true" read="true"/><Dir delete="true" modify="true" read="true" import="true" record="true"/><File delete="true" modify="true" read="true" export="true" playback="true"/><Profile add="true" delete="true" modify="true" read="true"/><SessionConfig add="true" delete="true" modify="true" read="true"/><Policy add="true" delete="true" modify="true" read="true"/><System upgrade="true" restore="true" backup="true" read="true" modify="true" sessionmanagement="true"/><Report add="true" delete="true" modify="true" read="true"/></Permissions>';
          permissionNode = new jsx3.xml.Document().loadXML(permissionsStr);
        }
        this.clsApp.clsUserPermission.setPermission(permissionNode);

        //fix the UserPrivateData Node its a string covert to xml
        var userPrivateDataParent = response.selectSingleNode(".//UserPrivateData").getParent();
        var userPrivateDataNode = response.selectSingleNode(".//UserPrivateData");
        if (!jsx3.util.strEmpty(userPrivateDataNode) && !jsx3.util.strEmpty(userPrivateDataNode.getValue())) {
          var userPrivateDataNodeVal = userPrivateDataNode.getValue().replace(/\s+ /g, '');
          var userPrivateDataDoc = new jsx3.xml.Document().loadXML(userPrivateDataNodeVal);
          userPrivateDataParent.replaceNode(userPrivateDataDoc, userPrivateDataNode);
        }
        //fix the AssestPrivateData Node its a string covert to xml
        var assestPrivateDataParent = response.selectSingleNode(".//AssetPrivateData").getParent();
        var assestPrivateDataNode = response.selectSingleNode(".//AssetPrivateData");
        if (!jsx3.util.strEmpty(assestPrivateDataNode) && !jsx3.util.strEmpty(assestPrivateDataNode.getValue())) {
          var assestPrivateDataNodeVal = assestPrivateDataNode.getValue().replace(/\s+ /g, '');
          var assestPrivateDataDoc = new jsx3.xml.Document().loadXML(assestPrivateDataNodeVal);
          assestPrivateDataParent.replaceNode(assestPrivateDataDoc, assestPrivateDataNode);
        }

        var portNode = response.selectSingleNode(".//PortList/Port");
        this.setLoginParam(ServiceAgent.LOGIN_PORTTITLE, portNode.getAttribute("title"));
        this.setLoginParam(ServiceAgent.LOGIN_PORTSTREAMTYPE, portNode.getAttribute("streamType"));
        //store client data to tApp
        this.setLoginParam(ServiceAgent.LOGIN_SERVICECLIENTDATA, response);
      }

      if (respStatus == ServiceAgent.LOGIN_GETSERVICECLIENT_SUCCESS) {
        ServiceAgent._LOG.info("GetServiceClient Success.");
        this._getServerTime();
      }
      else {
        ServiceAgent._LOG.info("GetServiceClient error. Error code:" + respStatus);
        this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_ERROR);
        this._setStateParam(ServiceAgent.LOGINERRORCODE, respStatus);
        this.publish({ subject: ServiceAgent.ON_LOGIN_ERROR,
          stateVar: ServiceAgent.LOGINSTATE,
                      stateVal:respStatus});
        this._logout();
      }
    }).bind(this));
  }



  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._getServerTime = function () {
    com.ipvs.api.API.GetServerTime().when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      var localTime = new Date().getTime();
      var serverTime = "";
      var deltaTime = "";
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        var eventVal = ServiceAgent.LOGIN_SUCCESS;
        var eventSub = ServiceAgent.ON_LOGINSTATE_CHANGE;
        this._setStateParam(ServiceAgent.LOGINSTATE, eventVal);
        var respServerTime = response.selectSingleNode("//serverTime");
        if (respServerTime != null) {
          serverTime = respServerTime.getValue();
          deltaTime = parseInt(localTime) - parseInt(serverTime);
        }
        ServiceAgent._LOG.info("getSrerverTime success.");
      }
      else {
        ServiceAgent._LOG.info("getSrerverTime error. Error code:" + respStatus);
        this._setStateParam(ServiceAgent.LOGINERRORCODE, respStatus);
        this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_ERROR);
        var eventVal = ServiceAgent.LOGIN_GETSERVERTIME_ERROR;
        var eventSub = ServiceAgent.ON_LOGIN_ERROR;
      }
      this._setDeltaServerLocalTime(deltaTime);

      this.publish({ subject: eventSub,
        stateVar: ServiceAgent.LOGINSTATE,
                   stateVal:eventVal});
      if (eventVal == ServiceAgent.LOGIN_SUCCESS) {
        this.auditEvent("Login");
      }
      else {
        this._logout();
      }
    }).bind(this));
  }

  /**
  * Sets info for retry login process.
  * @param clientType {string} INSTALLED/WEB may be null;
  */
  ServiceAgent_prototype.retryLogin = function (clientType) {
    this._retryLogin = true;
    this.setLoginParam(ServiceAgent.LOGIN_RETRYUSER, this.getLoginParam(ServiceAgent.LOGIN_USER));
    this.setLoginParam(ServiceAgent.LOGIN_RETRYPASSWORD, this.getLoginParam(ServiceAgent.LOGIN_PASSWORD));
    this.doLogout(clientType);
  }

  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._initRetryLogin = function () {
    this._setStateParam(ServiceAgent.RETRYLOGINSTATE, ServiceAgent.RETRYLOGIN_INPROGRESS);
    this._retryStartTime = new Date().getTime();
    this.publish({ subject: ServiceAgent.ON_LOGIN_RETRY,
      stateVar: ServiceAgent.RETRYLOGINSTATE,
                  stateVal:ServiceAgent.RETRYLOGIN_START});
    var ThisClass = this;
    this._retryTimerID = setInterval(function () { ServiceAgent_prototype._processRetryLogin(ThisClass); }, 1000);
  }

  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._processRetryLogin = function (context) {
    var currTime = new Date().getTime();
    var waitTime = parseInt(30000 - (new Date().getTime() - context._retryStartTime));
    if (waitTime <= 0) {
      clearInterval(context._retryTimerID);
      context._retryTimerID = null;
      context._retryStartTime = null;
      waitTime = ServiceAgent.RETRYLOGIN_END;
      context._setStateParam(ServiceAgent.RETRYLOGINSTATE, ServiceAgent.LOGIN_NONE);
    }
    else {
      waitTime = parseInt(waitTime / 1000);
    }
    context.publish({ subject: ServiceAgent.ON_LOGIN_RETRY,
      stateVar: ServiceAgent.RETRYLOGINSTATE,
                  stateVal:waitTime});
  }

  /**
  * Stops retry login process.
  */
  ServiceAgent_prototype.cancelRetryLogin = function () {
    if (this._retryTimerID != null) {
      clearInterval(this._retryTimerID);
    }
    this._retryLogin = null;
    this._retryTimerID = null;
    this._retryStartTime = null;
    this._setStateParam(ServiceAgent.RETRYLOGINSTATE, ServiceAgent.LOGIN_NONE);
    this.publish({ subject: ServiceAgent.ON_LOGIN_RETRY,
      stateVar: ServiceAgent.RETRYLOGINSTATE,
                  stateVal:ServiceAgent.LOGIN_NONE});   
  }

  /**
  * Initiates logout process and sends logout request.
  * Publish the events ON_LOGINSTATE_CHANGE / ON_LOGOUT_ERROR;
  * @param clientType {string} INSTALLED/WEB may be null;
  */
  ServiceAgent_prototype.doLogout = function (clientType) {
    this.auditEvent("Logout");
    if (clientType == "WEB") {
      ServiceAgent._LOG.info("WEB logout..");
      this.publish({ subject: ServiceAgent.ON_LOGINSTATE_CHANGE,
        stateVar: ServiceAgent.LOGINSTATE,
                    stateVal:ServiceAgent.LOGIN_LOGOUT_SUCCESS});
      this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_NONE);
      if (this._retryLogin) {
        this._initRetryLogin();
      }
    }
    else {
      this._logout();
    }
  }

  /** @private @jsxobf-clobber */
  ServiceAgent_prototype._logout = function () {
    com.ipvs.api.API.Logout(this.getLoginParam(ServiceAgent.LOGIN_USERJID)).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        var eventVal = ServiceAgent.LOGIN_LOGOUT_SUCCESS;
        var eventSub = ServiceAgent.ON_LOGINSTATE_CHANGE;
        ServiceAgent._LOG.info("Logout success");
        this._setStateParam(ServiceAgent.LOGINSTATE, ServiceAgent.LOGIN_NONE);
      }
      else {
        var eventVal = ServiceAgent.LOGIN_LOGOUT_ERROR;
        var eventSub = ServiceAgent.ON_LOGOUT_ERROR;
        ServiceAgent._LOG.info("Logout request error. Error code:" + respStatus);
      }
      //###########################
      //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code
      //Once MW completes then uncomment the following code
      // #########################
      //if(!this._retryLogin){
        this.publish({ subject: eventSub,
          stateVar: ServiceAgent.LOGINSTATE,
                     stateVal:eventVal});
     // }
      if (eventVal == ServiceAgent.LOGIN_LOGOUT_SUCCESS) {
        if (this._retryLogin) {
          this._initRetryLogin();
        }
      }
    }).bind(this));
  }

  ServiceAgent_prototype.auditEvent = function (eventName, dataNode) {
    //check if login state is valid  
    var loginState = this.getStateParam(ServiceAgent.LOGINSTATE);
    if (loginState != ServiceAgent.LOGIN_SUCCESS) {
      return;
    }
      
    var type = "custom";
    var date = "";
    var time = "";
    var level = "";
    var name = eventName;

    var descriptionNode = new jsx3.xml.Document().createDocumentElement("Description");
    var serviceClientData = this.getLoginParam(ServiceAgent.LOGIN_SERVICECLIENTDATA).cloneNode(true);
    var eventUser = serviceClientData.selectSingleNode(".//User");
    var eventUserNode = descriptionNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "EventUser");
    eventUserNode.appendChild(eventUser);
    descriptionNode.appendChild(eventUserNode);
    var eventAsset = serviceClientData.selectSingleNode(".//Asset");
    var eventAssetNode = descriptionNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "EventAsset");
    eventAssetNode.appendChild(eventAsset);
    descriptionNode.appendChild(eventAssetNode);

    switch (eventName) {
      case "Login":
        descriptionNode.setAttribute("text", "Login Success");
        break;
      case "Logout":
        descriptionNode.setAttribute("text", "Logout");
        break;
      case "Searched":
        descriptionNode.setAttribute("text", dataNode);
        break;
      case "Viewed":
      case "Modified":
        if (!jsx3.util.strEmpty(dataNode)) {
          descriptionNode.appendChild(dataNode);
        }
        break;
      default:
        break;
    }    

    com.ipvs.api.API.AuditEvent(type, date, time, level, name, descriptionNode).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        ServiceAgent._LOG.info("AuditEvent:Success");
      }
      else {
        ServiceAgent._LOG.info("AuditEvent:Failed");
      }
    }).bind(this));
  }

  ServiceAgent_prototype._handlePresenceNotification = function (objPresenceEvent) {
    var PresenceState = objPresenceEvent.State;
    var PresenceFrom = objPresenceEvent.JID;
    var UserFromName = PresenceFrom.substring(0, PresenceFrom.indexOf("@"));
    var ResourceFrom = PresenceFrom.substring(PresenceFrom.indexOf("/") + 1);
    var PresenceTo = this.getLoginParam(ServiceAgent.LOGIN_USERJID);
    var UserToName = PresenceTo.substring(0, PresenceTo.indexOf("@"));
    var ResourceTo = PresenceTo.substring(PresenceTo.indexOf("/") + 1);
    if ((PresenceState) && (UserFromName == UserToName)) {
      ServiceAgent._LOG.warn("PRESENCE: Duplicate Login: LOGOUT PUBLISH");
      this.doLogout();
      jsx3.sleep((function () {
        ServiceAgent._LOG.warn("PRESENCE: Duplicate Login: ALERT MSG");
        alert("Session Terminated: Duplicate Login\n" +
              "User: '" + UserFromName + "'\n" +
              "Current Client Name: '" + ResourceTo + "'\n" +
              "Remote Client Name: '" + ResourceFrom + "'\n"
              );
        return;
      }), null, this);
    }
  }

  /**
  * Unsubscribes all events
  */
  ServiceAgent_prototype.unSubscribeAll = function () {
    this.unsubscribeAllFromAll();
  }
});