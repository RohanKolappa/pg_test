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
jsx3.Class.defineClass("com.ipvs.api.Login", null, [jsx3.util.EventDispatcher], function (IpvsLogin, IpvsLogin_prototype) {

  /** @private @jsxobf-clobber */
  IpvsLogin._LOG = jsx3.util.Logger.getLogger("com.ipvs.api.Login");

  //LOGINSTATE:holds actual login state
  IpvsLogin.LOGINSTATE = "LoginState";
  IpvsLogin.LOGINERRORCODE = "LoginErrorCode";

  /**
  * {String} Event type published when Login/Logout successed .
  * @final @jsxobf-final
  */
  IpvsLogin.ON_LOGINSTATE_CHANGE = "OnLoginStateChange";
  /**
  * {String} Event type published when an error occers during login process.
  * @final @jsxobf-final
  */
  IpvsLogin.ON_LOGIN_ERROR = "OnLoginError";
  /**
  * {String} Event type published when an error occers during logout process.
  * @final @jsxobf-final
  */
  IpvsLogin.ON_LOGOUT_ERROR = "OnLogoutError";
  /**
  * {String} Event type published during retry login process.
  * @final @jsxobf-final
  */
  IpvsLogin.ON_LOGIN_RETRY = "OnLoginRetry";
  IpvsLogin.RETRYLOGINSTATE = "RetryLoginState";

  IpvsLogin.LOGIN_NONE = "NONE";
  IpvsLogin.LOGIN_SUCCESS = "VALID";
  IpvsLogin.LOGIN_ERROR = "ERROR";
  IpvsLogin.LOGIN_INPROGRESS = "InProgress";
  IpvsLogin.LOGIN_XMPP_SUCCESS = "XMPP_LOGIN_VALID";

  IpvsLogin.LOGIN_XMPP_ERROR = "1003";
  IpvsLogin.LOGIN_XMPP_DUPLICATE = "1004";
  IpvsLogin.LOGIN_SETSERVICE_VERSION_ERROR = "1200";
  IpvsLogin.LOGIN_SETSERVICE_CLIENTINVALID_ERROR = "1026";
  IpvsLogin.LOGIN_SETSERVICE_CLIENTINUSE_ERROR = "1027";
  IpvsLogin.LOGIN_SETSERVICE_CLIENTLOGGEDIN_ERROR = "1303";
  IpvsLogin.LOGIN_SETSERVICE_INVALIDCLIENT_ERROR = "1037";
  IpvsLogin.LOGIN_SETSERVICE_APPSERVERNOTFOUND_ERROR = "2800";
  IpvsLogin.LOGIN_MAX_CLIENTS_EXCEEDED_ERROR = "1302";
  IpvsLogin.LOGIN_INVALID_SERVER_IPADDRESS = "1040";

  IpvsLogin.LOGIN_SETSERVICE_SUCCESS = "VERSION_SUCCESS";
  IpvsLogin.LOGIN_GETSERVERTIME_ERROR = "GETSERVERTIME_ERROR";
  IpvsLogin.LOGIN_GETSERVICECLIENT_ERROR = "GETSERVICECLIENT_ERROR";
  IpvsLogin.LOGIN_GETSERVICECLIENT_SUCCESS = "GETSERVICECLIENT_SUCCESS";

  IpvsLogin.LOGIN_LOGOUT_SUCCESS = "LOGOUT_SUCCESS";
  IpvsLogin.LOGIN_LOGOUT_ERROR = "LOGOUT_ERROR";

  IpvsLogin.RETRYLOGIN_START = "RetryLoginStart";
  IpvsLogin.RETRYLOGIN_INPROGRESS = "RetryLoginInProgress";
  IpvsLogin.RETRYLOGIN_END = "RetryLoginEnd";

  //login params
  IpvsLogin.LOGIN_USER = "LoginUser";
  IpvsLogin.LOGIN_PASSWORD = "LoginPassword";
  IpvsLogin.LOGIN_DOMAIN = "LoginDomain";
  IpvsLogin.LOGIN_CLIENTNAME = "ClientName";
  IpvsLogin.LOGIN_RESOURCE = "LoginResource";
  IpvsLogin.LOGIN_RESOURCE_PASSWORD = "LoginResourcePassword";
  IpvsLogin.LOGIN_SERVER = "LoginServer";
  IpvsLogin.LOGIN_PORT = "LoginPort";
  IpvsLogin.LOGIN_USERJID = "LoginUserJID";
  IpvsLogin.LOGIN_APPSERVERAGENTJID = "AppServerAgentJID";
  IpvsLogin.LOGIN_USERDOMAIN = "LoginUserDomain";
  IpvsLogin.LOGIN_SERVICEDOMAIN = "ServiceDomain";
  IpvsLogin.LOGIN_ASSETTEMPLATETITLE = "AssetTemplateTitle";
  IpvsLogin.LOGIN_ASSETPRIVATEKEYSCHEME = "AssetPrivateKeyScheme";
  IpvsLogin.LOGIN_ASSETPRIVATEKEYDATA = "AssetPrivateKeyData";
  IpvsLogin.LOGIN_ASSETPRIVATEDATA = "AssetPrivateData";
  IpvsLogin.LOGIN_SERVICEVERSION = "ServiceVersion";
  IpvsLogin.LOGIN_SOFTWAREVERSION = "SoftwareVersion";
  IpvsLogin.LOGIN_CLIENTIPADDRESS = "ClientIPAddress";
  IpvsLogin.LOGIN_APPNAME = "AppName";
  IpvsLogin.LOGIN_WEBCLIENTSERVER = "WebClientLoginServer";
  //retry login params
  IpvsLogin.LOGIN_RETRYUSER = "LastLoginUser";
  IpvsLogin.LOGIN_RETRYPASSWORD = "LastLoginPassword";
  IpvsLogin.LOGIN_AUTOGENERATEPASSWORD = "AutoGeneratePassword";

  /**
  * The instance initializer.
  * @param topicName {String} topic name which holds global variables.
  */
  IpvsLogin_prototype.init = function (topicName) {
    this.tName = topicName;
    this._loginParams = [];
    this._loginStateParams = [];
    this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_NONE);
    this._setStateParam(IpvsLogin.LOGINERRORCODE, IpvsLogin.LOGIN_NONE);
    this._setStateParam(IpvsLogin.RETRYLOGINSTATE, IpvsLogin.LOGIN_NONE);
    this._retryLogin = null;
    this._retryTimerID = null;
    this._retryStartTime = null;
  };

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._setStateParam = function (paramName, paramValue) {
    this._loginStateParams[paramName] = paramValue;
  };

  /**
  * Returns the login state.
  * @param paramName {String}
  * @return {String} login state.
  */
  IpvsLogin_prototype.getStateParam = function (paramName) {
    return this._loginStateParams[paramName];
  };

  /**
  * Sets the login parameter value.;
  * @param paramName {String} 
  * @param paramValue {String}
  */
  IpvsLogin_prototype.setLoginParam = function (paramName, paramValue) {
    this._loginParams[paramName] = paramValue;
  };

  /**
  * Returns the login parameter value.
  * @param paramName {String}
  * @return {String} login parameter value.
  */
  IpvsLogin_prototype.getLoginParam = function (paramName) {
    return this._loginParams[paramName];
  };

  /**
  * Sets the value for multiple login parameters;
  * @param objLoginParams {Array};
  */
  IpvsLogin_prototype.setLoginParams = function (objLoginParams) {
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
  IpvsLogin_prototype.getLoginParams = function () {
    return this._loginParams;
  };

  /**
  * Returns the login status;
  * @return {boolean} true/false.
  */
  IpvsLogin_prototype.isLoggedIn = function () {
    var loginState = this.getStateParam(IpvsLogin.LOGINSTATE);
    if (loginState == IpvsLogin.LOGIN_SUCCESS || loginState == IpvsLogin.LOGIN_LOGOUT_ERROR) {
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
  IpvsLogin_prototype.doLogin = function () {
    //check if login is already in progress
    if (this.getStateParam(IpvsLogin.LOGINSTATE) == IpvsLogin.LOGIN_INPROGRESS) {
      IpvsLogin._LOG.info("Login process is in progress ....");
      return;
    }
    this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_INPROGRESS);
    //###########################
    //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code ###
    //Once MW completes then uncomment the following code
    // #########################
    //if(!this._retryLogin) {
      this.publish({ subject: IpvsLogin.ON_LOGINSTATE_CHANGE,
        stateVar: IpvsLogin.LOGINSTATE,
        stateVal: IpvsLogin.LOGIN_INPROGRESS
      });
   // }
    var urlStr = 'ipvs:///LoginRequest?ServiceName=xmpp' +
                 '&OpType=Operation' +
                 '&User=' + escape(this.getLoginParam(IpvsLogin.LOGIN_USER)) +
                 '&Password=' + escape(this.getLoginParam(IpvsLogin.LOGIN_PASSWORD)) +
                 '&Domain=' + escape(this.getLoginParam(IpvsLogin.LOGIN_DOMAIN)) +
                 '&Resource=' + escape(this.getLoginParam(IpvsLogin.LOGIN_RESOURCE)) +
                 '&Server=' + this.getLoginParam(IpvsLogin.LOGIN_SERVER) +
                 '&Port=' + this.getLoginParam(IpvsLogin.LOGIN_PORT) +
                 '&ObjectType=Login';
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(urlStr));
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        respStatus = IpvsLogin.LOGIN_XMPP_SUCCESS;
        var respVal = req.getResponseText();
        this.setLoginParam(IpvsLogin.LOGIN_USERJID, respVal);
        this._setTopicVar(IpvsLogin.LOGIN_USERJID, respVal); //TBD
        var LoginUserDomain = respVal.substring(respVal.indexOf("@") + 1, respVal.indexOf("/"));
        this.setLoginParam(IpvsLogin.LOGIN_USERDOMAIN, LoginUserDomain);
      }
      delete serviceAppletReq;

      if (respStatus == IpvsLogin.LOGIN_XMPP_SUCCESS) {
        IpvsLogin._LOG.info("Xmpp Login Success.");
        this._setServiceDomain();
      }
      else {
        IpvsLogin._LOG.info("Xmpp Login error. Error code:" + respStatus);
        this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_ERROR);
        this._setStateParam(IpvsLogin.LOGINERRORCODE, respStatus);
        //###########################
        //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code ###
        //Once MW completes then uncomment the following code
        // #########################
        //if(!this._retryLogin) {
          this.publish({ subject: IpvsLogin.ON_LOGIN_ERROR,
            stateVar: IpvsLogin.LOGINSTATE,
            stateVal: respStatus
          });
       // }
        if (this._retryLogin) {
          this._initRetryLogin();
        }
      }
    }
    req.subscribe("*", this, onRequestEvent);
    IpvsLogin._LOG.info("Sending Login request ....");
    req.send();
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._setServiceDomain = function () {
    var urlStr = 'ipvs:///SetServiceDomain?ServiceName=systemservice' +
                 '&OpType=Operation' +
                 '&ServiceDomain=' + escape(this.getLoginParam(IpvsLogin.LOGIN_SERVICEDOMAIN)) +
                 '&AssetTemplateTitle=' + escape(this.getLoginParam(IpvsLogin.LOGIN_ASSETTEMPLATETITLE)) +
                 '&AssetPrivateKeyScheme=' + escape(this.getLoginParam(IpvsLogin.LOGIN_ASSETPRIVATEKEYSCHEME)) +
                 '&AssetPrivateKeyData=' + escape(this.getLoginParam(IpvsLogin.LOGIN_ASSETPRIVATEKEYDATA)) +
                 '&AssetPrivateData=' + escape(this.getLoginParam(IpvsLogin.LOGIN_ASSETPRIVATEDATA)) +
                 '&ServiceVersion=' + this.getLoginParam(IpvsLogin.LOGIN_SERVICEVERSION) +
                 '&SoftwareVersion=' + this.getLoginParam(IpvsLogin.LOGIN_SOFTWAREVERSION) +
                 '&IPAddress=' + this.getLoginParam(IpvsLogin.LOGIN_CLIENTIPADDRESS) +
                 '&AppName=' + escape(this.getLoginParam(IpvsLogin.LOGIN_APPNAME)) +
                 '&ObjectType=SetServiceDomain';

    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(urlStr));
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        respStatus = IpvsLogin.LOGIN_SETSERVICE_SUCCESS;
        var response = req.getResponseXML();
        var respServiceRoot = response.selectSingleNode("//ServiceRoot");
        if (respServiceRoot != null) {
          var cacheServiceRoot = topic_GetNode(tCache, "//ServiceRoot");
          if (cacheServiceRoot == null) {
            cacheServiceRoot = topic_Set(tCache, "ServiceRoot", null);
          }
          cacheServiceRoot.getParent().replaceNode(respServiceRoot.cloneNode(true), cacheServiceRoot);
        }

        var respAppServerAgentJID = response.selectSingleNode("//appServerAgentJID");
        if (respAppServerAgentJID != null) {
          var AppServerAgentJID = respAppServerAgentJID.getValue();
          this.setLoginParam(IpvsLogin.LOGIN_APPSERVERAGENTJID, AppServerAgentJID);
          this._setTopicVar("AppServerAgentJID", AppServerAgentJID); //TBD
          var msIP = AppServerAgentJID.substring(AppServerAgentJID.indexOf("@") + 1, AppServerAgentJID.indexOf("/"));
          if (msIP == "") {
            msIP = AppServerAgentJID.substring(AppServerAgentJID.indexOf(".") + 1);
          }
          this._setTopicVar("MSServerIP", msIP); //TBD
        }

        //get the MSSoftwraeversion from the response.
        var softwareVersion = response.selectSingleNode("//softwareVersion");
        var mSSoftwareVersion = "";
        if (!jsx3.util.strEmpty(softwareVersion)) {
          mSSoftwareVersion = softwareVersion.getValue();
        }
        this._setTopicVar("MSSoftwareVersion", mSSoftwareVersion);

        //get the DeviceSoftwareVersion from the response.
        var deviceSoftwareVersion = response.selectSingleNode("//DeviceSoftwareVersion");
        var msDeviceSoftwareVersion = "";
        if (!jsx3.util.strEmpty(deviceSoftwareVersion)) {
          msDeviceSoftwareVersion = deviceSoftwareVersion.getValue();
        }
        this._setTopicVar("MsDeviceSoftwareVersion", msDeviceSoftwareVersion);

        var respAssetResource = response.selectSingleNode("//AssetResource");
        if (respAssetResource != null) {
          var nodeAssetResourceTitle = respAssetResource.selectSingleNode(".//Info/Title");
          if (nodeAssetResourceTitle != null) {
            setLocalPreferences(PNameSpace, ".//AssetResourceTitle", nodeAssetResourceTitle.getValue()); //TBD
          }
          var nodeAssetPrivateKeyScheme = respAssetResource.selectSingleNode(".//AssetPrivateKey/KeyScheme");
          if (nodeAssetPrivateKeyScheme != null) {
            var assetPrivateKeyScheme = nodeAssetPrivateKeyScheme.getValue();
            setLocalPreferences(PNameSpace, ".//AssetPrivateKeyScheme", assetPrivateKeyScheme); //TBD
            if (assetPrivateKeyScheme == "SERVER") {
              var nodeAssetPrivateKeyData = respAssetResource.selectSingleNode(".//AssetPrivateKey/KeyData");
              if (nodeAssetPrivateKeyData != null) {
                setLocalPreferences(PNameSpace, ".//AssetPrivateKeyData", nodeAssetPrivateKeyData.getValue()); //TBD
              }
            }
          }
          var nodeAssetPrivateData = respAssetResource.selectSingleNode(".//AssetPrivateData");
          if (nodeAssetPrivateData != null) {
            //Following change is a workaround to handle stripping of XML tags in AssetPrivateData.
            //And needs to be reverted when the middleware is fixed.
            var assetPrivateDataParentNode = topic_GetNode(tPrefs, ".//Asset"); //TBD
            var assetPrivateDataNode = topic_GetNode(tPrefs, ".//AssetPrivateData"); //TBD
            assetPrivateDataParentNode.replaceNode(nodeAssetPrivateData, assetPrivateDataNode);
            for (var iter = nodeAssetPrivateData.getChildNodes().iterator(); iter.hasNext(); ) {
              var childNode = iter.next();
              var objValue = childNode.getValue();
              var objName = ".//" + childNode.getNodeName();
              setLocalPreferences(PNameSpace, objName, objValue); //TBD
            }
          }
        }
      }
      delete serviceAppletReq;

      if (respStatus == IpvsLogin.LOGIN_SETSERVICE_SUCCESS) {
        IpvsLogin._LOG.info("SetServiceDomain Success.");
        //###########################
        //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code ###
        //Once MW completes then uncomment the following code
        // #########################
        //if(this._retryLogin){
        //  this.cancelRetryLogin();
        //  topic_Publish(tApp, "SCStarted", true);
        //  return;
        //}
        this.cancelRetryLogin();
        this._getServiceClient();
      }
      else {
        IpvsLogin._LOG.info("SetServiceDomain error. Error code:" + respStatus);
        this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_ERROR);
        this._setStateParam(IpvsLogin.LOGINERRORCODE, respStatus);
        if (respStatus != IpvsLogin.LOGIN_SETSERVICE_APPSERVERNOTFOUND_ERROR) {
          this.cancelRetryLogin();
        }
        //###########################
        //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code ###
        //Once MW completes then uncomment the following code
        // #########################
        //if(!this._retryLogin){
          this.publish({ subject: IpvsLogin.ON_LOGIN_ERROR,
            stateVar: IpvsLogin.LOGINSTATE,
            stateVal:respStatus});
        //}
        this._logout();
      }
    }
    req.subscribe("*", this, onRequestEvent);
    IpvsLogin._LOG.info("Sending SetServiceDomain request...");
    req.send();
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._getServerTime = function () {
    var urlStr = 'ipvs:///getSrerverTime?ServiceName=systemservice' +
                 '&OpType=Operation' +
                 '&ObjectType=GetServerTime';

    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(urlStr));
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();

      var serverTime = "";
      var deltaTime = "";
      var localTime = new Date().getTime();
      if (respStatus == Request.STATUS_OK) {
        var eventVal = IpvsLogin.LOGIN_SUCCESS;
        var eventSub = IpvsLogin.ON_LOGINSTATE_CHANGE;
        this._setStateParam(IpvsLogin.LOGINSTATE, eventVal);
        var response = req.getResponseXML();
        var respServerTime = response.selectSingleNode("//serverTime");
        if (respServerTime != null) {
          serverTime = respServerTime.getValue();
          deltaTime = parseInt(localTime) - parseInt(serverTime);
        }
        IpvsLogin._LOG.info("getSrerverTime success.");
      }
      else {
        IpvsLogin._LOG.info("getSrerverTime error. Error code:" + respStatus);
        this._setStateParam(IpvsLogin.LOGINERRORCODE, respStatus);
        this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_ERROR);
        var eventVal = IpvsLogin.LOGIN_GETSERVERTIME_ERROR;
        var eventSub = IpvsLogin.ON_LOGIN_ERROR;
      }
      delete serviceAppletReq;

      this._setTopicVar("ServerTime", serverTime);
      this._setTopicVar("DeltaServerLocalTime", deltaTime);
      this._setTopicVar("LocalTime", localTime);

      this.publish({ subject: eventSub,
        stateVar: IpvsLogin.LOGINSTATE,
        stateVal: eventVal
      });
      if (eventVal != IpvsLogin.LOGIN_SUCCESS) {
        this._logout();
      }
      else {
        this.auditEvent("Login");
      }
    }
    req.subscribe("*", this, onRequestEvent);
    req.send();
  }


  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._getServiceClient = function () {
    var urlStr = 'ipvs:///getServiceClient?ServiceName=sessionservice' +
                 '&OpType=Operation' +
                 '&ObjectList_SelectFormat=EXTENDED' +
                 '&RequestName=GetServiceClientRequest' +
                 '&iq_type=set ' +
                 '&iq_from=' + escape(this.getLoginParam(IpvsLogin.LOGIN_USERJID)) +
                 '&iq_to=' + escape(this._getTopicVar("AppServerAgentJID"));
    '&ObjectType=GetServiceClient';

    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(urlStr));
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();

      if (respStatus == Request.STATUS_OK) {
        respStatus = IpvsLogin.LOGIN_GETSERVICECLIENT_SUCCESS;
        var response = req.getResponseXML();
        //set the permissions
        var permissionNode = response.selectSingleNode(".//Permissions");
        if (jsx3.util.strEmpty(permissionNode)) {
          var permissionsStr = '<Permissions><User add="true" delete="true" modify="true" read="true"/><Port add="true" delete="true" modify="true" read="true" viewfrom="true" streamto="true"/><Device add="true" delete="true" modify="true" read="true"/><Dir delete="true" modify="true" read="true" import="true" record="true"/><File delete="true" modify="true" read="true" export="true" playback="true"/><Profile add="true" delete="true" modify="true" read="true"/><SessionConfig add="true" delete="true" modify="true" read="true"/><Policy add="true" delete="true" modify="true" read="true"/><System upgrade="true" restore="true" backup="true" read="true" modify="true" sessionmanagement="true"/><Report add="true" delete="true" modify="true" read="true"/></Permissions>';
          permissionNode = new jsx3.xml.Document().loadXML(permissionsStr);
        }
        clsUserPermission.setPermission(permissionNode);

        //fix the UserPrivateData Node its a string covert to xml                          
        var userPrivateDataNode = response.selectSingleNode(".//UserPrivateData");
        if (!jsx3.util.strEmpty(userPrivateDataNode) && !jsx3.util.strEmpty(userPrivateDataNode.getValue())) {
          var userPrivateDataNodeVal = userPrivateDataNode.getValue().replace(/\s+ /g, '');
          var userPrivateDataDoc = new jsx3.xml.Document().loadXML(userPrivateDataNodeVal);
          var userPrivateDataParent = response.selectSingleNode(".//UserPrivateData").getParent();
          userPrivateDataParent.replaceNode(userPrivateDataDoc, userPrivateDataNode);
        }
        //fix the AssestPrivateData Node its a string covert to xml                          
        var assestPrivateDataNode = response.selectSingleNode(".//AssetPrivateData");
        if (!jsx3.util.strEmpty(assestPrivateDataNode) && !jsx3.util.strEmpty(assestPrivateDataNode.getValue())) {
          var assestPrivateDataNodeVal = assestPrivateDataNode.getValue().replace(/\s+ /g, '');
          var assestPrivateDataDoc = new jsx3.xml.Document().loadXML(assestPrivateDataNodeVal);
          var assestPrivateDataParent = response.selectSingleNode(".//AssetPrivateData").getParent();
          assestPrivateDataParent.replaceNode(assestPrivateDataDoc, assestPrivateDataNode);
        }
        //store client data to tApp
        var serviceClientData = topic_Set(tApp, "ServiceClientData", null);
        serviceClientData.appendChild(response);
      }
      else {
        IpvsLogin._LOG.info("getSrerverTime error. Error code:" + respStatus);
        this._setStateParam(IpvsLogin.LOGINERRORCODE, respStatus);
        this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_ERROR);
        var eventVal = IpvsLogin.LOGIN_GETSERVICECLIENT_ERROR;
        var eventSub = IpvsLogin.ON_LOGIN_ERROR;
      }
      delete serviceAppletReq;

      if (respStatus == IpvsLogin.LOGIN_GETSERVICECLIENT_SUCCESS) {
        IpvsLogin._LOG.info("GetServiceClient Success. Calling GetServerTime");
        this._getServerTime();
      }
      else {
        IpvsLogin._LOG.info("GetServiceClient error. Error code:" + respStatus);
        this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_ERROR);
        this._setStateParam(IpvsLogin.LOGINERRORCODE, respStatus);
        this.publish({ subject: IpvsLogin.ON_LOGIN_ERROR,
          stateVar: IpvsLogin.LOGINSTATE,
          stateVal: respStatus
        });
        this._logout();
      }
    }

    req.subscribe("*", this, onRequestEvent);
    req.send();
  }


  /**
  * Sets info for retry login process.
  * @param clientType {string} INSTALLED/WEB may be null;
  */
  IpvsLogin_prototype.retryLogin = function (clientType) {
    this._retryLogin = true;
    this.setLoginParam(IpvsLogin.LOGIN_RETRYUSER, this.getLoginParam(IpvsLogin.LOGIN_USER));
    this.setLoginParam(IpvsLogin.LOGIN_RETRYPASSWORD, this.getLoginParam(IpvsLogin.LOGIN_PASSWORD));
    this.doLogout();
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._initRetryLogin = function () {
    this._setStateParam(IpvsLogin.RETRYLOGINSTATE, IpvsLogin.RETRYLOGIN_INPROGRESS);
    this._retryStartTime = new Date().getTime();
    this.publish({ subject: IpvsLogin.ON_LOGIN_RETRY,
      stateVar: IpvsLogin.RETRYLOGINSTATE,
      stateVal: IpvsLogin.RETRYLOGIN_START
    });
    var ThisClass = this;
    this._retryTimerID = setInterval(function () { IpvsLogin_prototype._processRetryLogin(ThisClass); }, 1000);
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._processRetryLogin = function (context) {
    var currTime = new Date().getTime();
    var waitTime = parseInt(30000 - (new Date().getTime() - context._retryStartTime));
    if (waitTime <= 0) {
      clearInterval(context._retryTimerID);
      context._retryTimerID = null;
      context._retryStartTime = null;
      waitTime = IpvsLogin.RETRYLOGIN_END;
      context._setStateParam(IpvsLogin.RETRYLOGINSTATE, IpvsLogin.LOGIN_NONE);
    }
    else {
      waitTime = parseInt(waitTime / 1000);
    }
    context.publish({ subject: IpvsLogin.ON_LOGIN_RETRY,
      stateVar: IpvsLogin.RETRYLOGINSTATE,
      stateVal: waitTime
    });
  }

  /**
  * Stops retry login process.
  */
  IpvsLogin_prototype.cancelRetryLogin = function () {
    if (this._retryTimerID != null) {
      clearInterval(this._retryTimerID);
    }
    this._retryLogin = null;
    this._retryTimerID = null;
    this._retryStartTime = null;
    this._setStateParam(IpvsLogin.RETRYLOGINSTATE, IpvsLogin.LOGIN_NONE);
    this.publish({ subject: IpvsLogin.ON_LOGIN_RETRY,
      stateVar: IpvsLogin.RETRYLOGINSTATE,
      stateVal: IpvsLogin.LOGIN_NONE
    });
  }

  /**
  * Initiates logout process and sends logout request.
  * Publish the events ON_LOGINSTATE_CHANGE / ON_LOGOUT_ERROR;
  * @param clientType {string} INSTALLED/WEB may be null;
  */
  IpvsLogin_prototype.doLogout = function () {
    this.auditEvent("Logout");
    this._logout();
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._logout = function () {
    var urlStr = 'ipvs:///LogoutRequest?ServiceName=xmpp' +
                 '&OpType=Operation' +
                 '&userJID=' + escape(this.getLoginParam(IpvsLogin.LOGIN_USERJID)) +
                 '&ObjectType=Logout';

    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(urlStr));
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        var eventVal = IpvsLogin.LOGIN_LOGOUT_SUCCESS;
        var eventSub = IpvsLogin.ON_LOGINSTATE_CHANGE;
        IpvsLogin._LOG.info("Logout success");
        this._setStateParam(IpvsLogin.LOGINSTATE, IpvsLogin.LOGIN_NONE);
      }
      else {
        var eventVal = IpvsLogin.LOGIN_LOGOUT_ERROR;
        var eventSub = IpvsLogin.ON_LOGOUT_ERROR;
        IpvsLogin._LOG.info("Logout request error. Error code:" + respStatus);
      }
      //###########################
      //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code ###
      //Once MW completes then uncomment the following code
      // #########################
      //if (!this._retryLogin) {
        this.publish({ subject: eventSub,
          stateVar: IpvsLogin.LOGINSTATE,
          stateVal: eventVal
        });
     // }
      delete serviceAppletReq;
      if (eventVal == IpvsLogin.LOGIN_LOGOUT_SUCCESS) {
        if (this._retryLogin) {
          this._initRetryLogin();
        }
      }
    }
    req.subscribe("*", this, onRequestEvent);
    IpvsLogin._LOG.info("Sending Logout request...");
    req.send();
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype.auditEvent = function (eventName, dataNode) {
    //check if login state is valid
    var loginState = this.getStateParam(IpvsLogin.LOGINSTATE);
    if (loginState != IpvsLogin.LOGIN_SUCCESS) {
      return;
    }

    var type = "custom";
    var date = "";
    var time = "";
    var level = "";
    var name = eventName;

    var descriptionNode = new jsx3.xml.Document().createDocumentElement("Description");
    var serviceClientData = topic_GetNode(tApp, "ServiceClientData").cloneNode(true);
    var eventUser = serviceClientData.selectSingleNode(".//User");

    var eventUserNode = descriptionNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "EventUser");
    if (!jsx3.util.strEmpty(eventUser)) {
      eventUserNode.appendChild(eventUser);
    }
    descriptionNode.appendChild(eventUserNode);
    var eventAsset = serviceClientData.selectSingleNode(".//Asset");
    var eventAssetNode = descriptionNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "EventAsset");
    if (!jsx3.util.strEmpty(eventAsset)) {
      eventAssetNode.appendChild(eventAsset);
    }

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
      case "Deleted":
        if (!jsx3.util.strEmpty(dataNode)) {
          descriptionNode.appendChild(dataNode);
        }
        break;
      default:
        break;
    }
    var urlStr = 'ipvs:///auditEvent?ServiceName=sessionservice' +
                 '&OpType=Operation' +
                 '&ObjectList_type=' + type +
                 '&ObjectList_date=' + date +
                 '&ObjectList_time=' + time +
                 '&ObjectList_level=' + level +
                 '&ObjectList_name=' + name +
                 '&RequestName=AuditEventRequest' +
                 '&iq_type=set ' +
                 '&iq_from=' + escape(this.getLoginParam(IpvsLogin.LOGIN_USERJID)) +
                 '&iq_to=' + escape(this._getTopicVar("AppServerAgentJID"));
    '&ObjectType=AuditEvent';

    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(urlStr));
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    serviceAppletReq.RequestDataObjectArr = new Object;
    serviceAppletReq.RequestDataObjectArr["Description"] = descriptionNode;
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        IpvsLogin._LOG.info("AuditEvent:Success");
      }
      else {
        IpvsLogin._LOG.info("AuditEvent:Failed");
      }
    }
    req.subscribe("*", this, onRequestEvent);
    req.send();
  }

  /**
  * Unsubscribes all events
  */
  IpvsLogin_prototype.unSubscribeAll = function () {
    this.unsubscribeAllFromAll();
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._setTopicVar = function (varName, varVal) {
    topic_Set(this.tName, varName, varVal);
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._getTopicVar = function (varName) {
    return topic_Get(this.tName, varName);
  }

  /** @private @jsxobf-clobber */
  IpvsLogin_prototype._topicPublish = function (stateVar, stateVal) {
    topic_Publish(this.tName, stateVar, stateVal);
  }

  /**
  * Updates global topic name
  * @param topicName {String} topic name which holds global variables.
  */
  IpvsLogin_prototype.updateTopicName = function (topicName) {
    this.tName = topicName;
  }
});