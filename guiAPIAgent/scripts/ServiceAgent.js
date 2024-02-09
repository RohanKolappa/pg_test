/*
* Copyright (c) Barco Inc.
* Use, modification, and distribution subject to terms of license.
*/
ServiceAgent = function () {
  this.cls = "ServiceAgent";
  BarcoUtil.logger(bDEBUG, "", this.cls, "", "constructor", "");
  this.init();
}

jQuery.extend(ServiceAgent.prototype, {
  LOGINSTATE: "LoginState",
  LOGINERRORCODE: "LoginErrorCode",
  /**
  * {String} Event type published when Login/Logout successed .
  */
  ON_LOGINSTATE_CHANGE: "OnLoginStateChange",
  /**
  * {String} Event type published when an error occers during login process.
  */
  ON_LOGIN_ERROR: "OnLoginError",
  /**
  * {String} Event type published when an error occers during logout process.
  */
  ON_LOGOUT_ERROR: "OnLogoutError",
  /**
  * {String} Event type published during retry login process.
  */
  ON_LOGIN_RETRY: "OnLoginRetry",

  //Login state param
  LOGIN_NONE: "NONE",
  LOGIN_SUCCESS: "VALID",
  LOGIN_ERROR: "ERROR",
  LOGIN_LOGOUT_SUCCESS: "LOGOUT_SUCCESS",
  LOGIN_LOGOUT_ERROR: "LOGOUT_ERROR",

  LOGIN_XMPP_DUPLICATE: "1004",
  LOGIN_INVALID_SERVER_IPADDRESS: "1040",
  LOGIN_SETSERVICE_VERSION_ERROR: "1200",

  LOGIN_INPROGRESS: "InProgress",
  RETRYLOGINSTATE: "RetryLoginState",
  RETRYLOGIN_START: "RetryLoginStart",
  RETRYLOGIN_INPROGRESS: "RetryLoginInProgress",
  RETRYLOGIN_END: "RetryLoginEnd",
  RETRYLOGIN_FAILED: "RetryLoginFailed",

  //login params
  LOGIN_USER: "LoginUser",
  LOGIN_PASSWORD: "LoginPassword",
  LOGIN_CLIENTNAME: "ClientName",
  LOGIN_SERVER: "LoginServer",

  /* Static constant variables of message types of Notification */
  MSG_PRESENCE: "EntityPresence",
  MSG_SSCMD: "SSCmd",
  MSG_SESSIONSTATEEVENT: "SessionState",
  MSG_CONNSTATEEVENT: "ConnState",
  MSG_TIMELINEEVENT: "Timeline",
  MSG_SESSIONPLAYBACKIQNODE: "SessionPlaybackIQNode",
  MSG_IMPORTFILEEVENT: "IMPORTFILEEVENT",
  //MSG_ROOMEVENT: "ROOMEVENT",
  MSG_AGENTEVENT: "AGENTEVENT",
  MSG_KICKEDOUT: "KickedOut",
  MSG_PORTCONNECTIONUPDATE: "PortConnectionUpdate",
  MSG_PORTCONNECTIONSTATEUPDATE: "PortConnectionStateUpdate",
  MSG_PORTUPDATE: "PortUpdate",
  MSG_NOTIFICATIONERROR: "NotificationError",
  MSG_PORTDISPLAYCONFIGUPDATE: "PortDisplayConfigUpdate",
  MSG_PORTDISPLAYSTATUSUPDATE: "PortDisplayStatusUpdate",
  PORT_LONGPULLING: 8880,

  /**
  * The instance initializer.
  */
  init: function () {
    BarcoUtil.logger(bDEBUG, "", this.cls, "init", "call", "");
    this._loginParams = [];
    this._loginStateParams = [];
    this._setStateParam(this.LOGINSTATE, this.LOGIN_NONE);
    this._setStateParam(this.LOGINERRORCODE, this.LOGIN_NONE);
    this._setStateParam(this.RETRYLOGINSTATE, this.LOGIN_NONE);
    this._retryLogin = null;
    this._retryTimerID = null;
    this._retryStartTime = null;
    this._DeltaServerLocalTime = 0;
  },

  /**
  * Sets the login parameter value.;
  * @param paramName {String} 
  * @param paramValue {String}
  */
  setLoginParam: function (paramName, paramValue) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "setLoginParam", "call",
          paramName + ', ' + paramValue);
    this._loginParams[paramName] = paramValue;
  },

  /**
  * Returns the login parameter value.
  * @param paramName {String}
  * @return {String} login parameter value.
  */
  getLoginParam: function (paramName) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getLoginParam", "call",
          paramName);
    return this._loginParams[paramName];
  },

  /**
  *Sets Login state values
  */
  _setStateParam: function (paramName, paramValue) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "_setStateParam", "call",
          paramName + ', ' + paramValue);
    this._loginStateParams[paramName] = paramValue;
  },

  /*
  *Returns value for Login states
  * @param paramName {String} 
  * @param paramValue {String}
  */
  getStateParam: function (paramName) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getStateParam", "call",
          paramName);
    return this._loginStateParams[paramName];
  },

  /**
  * Sets the value for DeltaServerLocalTime. Time different between ServerTime and LocalTime;
  * @param deltaServerLocalTime {String} in Milliseconds.
  */
  _setDeltaServerLocalTime: function (deltaServerLocalTime) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "_setDeltaServerLocalTime", "call",
          deltaServerLocalTime);
    this._DeltaServerLocalTime = deltaServerLocalTime;
  },
  /**
  * Returns DeltaServerLocalTime;
  * @return {String} in Milliseconds.
  */
  getDeltaServerLocalTime: function () {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getDeltaServerLocalTime", "call", "");
    return this._DeltaServerLocalTime;
  },
  /**
  * Returns Current ServerTime;
  * @return {String} in Milliseconds.
  */
  getCurrentServerTime: function () {
    // BarcoUtil.logger(bDEBUG, "", this.cls, "getCurrentServerTime", "call", "");
    var CurrTime = (new Date()).getTime();
    var CurrServerTime = parseInt(CurrTime) - parseInt(this._DeltaServerLocalTime);
    return CurrServerTime;
  },
  /**
  * Returns the Login status;
  * @return {boolean} true/false.
  */
  isLoggedIn: function () {
    BarcoUtil.logger(bDEBUG, "", this.cls, "isLoggedIn", "call", "");
    var loginState = this.getStateParam(this.LOGINSTATE);
    if (loginState == this.LOGIN_SUCCESS || loginState == this.LOGIN_LOGOUT_ERROR) {
      return true;
    }
    else {
      return false;
    }
  },

  /**
  * Initiates login process and sends Login requests.
  * Calls callback function after recieved response/in case of login error with error code.
  * @param userName {String} 
  * @param protocol {String}
  * @param password {String}
  * @param ipAddress {String} 
  * @param clientId {String}
  * @param serviceVerion {String} 
  * @param callback {Object}
  */
  doLogin: function (userName, password, protocol, ipAddress, clientId, serviceVerion, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "doLogin", "call",
          clientId + ', ' + serviceVerion);
    //check if login is already in progress
    if (this.getStateParam(this.LOGINSTATE) == this.LOGIN_INPROGRESS) {
      callback({ State: "ERROR",
        Error: { Code: this.LOGIN_INPROGRESS,
          Desc: this.LOGIN_INPROGRESS
        }
      });
    }
    else {
      //Set Login state as inprogress
      this._setStateParam(this.LOGINSTATE, this.LOGIN_INPROGRESS);
      //Set Login params
      this.setLoginParam(this.LOGIN_USER, userName);
      this.setLoginParam(this.LOGIN_PASSWORD, password);
      this.setLoginParam(this.LOGIN_RESOURCE, clientId);
      this.setLoginParam(this.LOGIN_SERVER, ipAddress);
      //Send Login
      Request.Login(userName, password, protocol, ipAddress, clientId, serviceVerion, jQuery.proxy(function (objResponse, textStatus, objResponseData) {
        if (objResponse.status == 200) {
          var notificationURL = objResponse.getResponseHeader('x-barco-notification-channel');
          //long pulling use 8880 to fix iPad safari load image blocked problem
          if ((navigator.platform.indexOf("iPhone") != -1) || (navigator.platform.indexOf("iPad") != -1)) {
            var strArr = notificationURL.split('/controller/');
            notificationURL = strArr[0] + ':' + this.PORT_LONGPULLING + '/controller/' + strArr[1];
          }
          this._createLongPollingChannel(notificationURL);
          this._setStateParam(this.LOGINERRORCODE, this.LOGIN_NONE);
          this._setStateParam(this.LOGINSTATE, this.LOGIN_SUCCESS);
          var retObj = { State: "SUCCESS", Error: { Code: "", Desc: ""} }
        }
        else {
          this._setStateParam(this.LOGINSTATE, this.LOGIN_ERROR);
          this._setStateParam(this.LOGINERRORCODE, objResponse.status);
          //401:Unauthorized;409:User Already Loggedin;500:invalid client name;502/503:server not available;
          var errorMsg = objResponse.responseText;
          if(errorMsg && errorMsg.indexOf('Authentication failed') >=0 ){
            errorMsg = 'The username and password did not match, please try again.';
          }
          else if(objResponse.status == '401') {
            errorMsg = 'Unauthorized';
          }
          else if(objResponse.status == '409') {
            errorMsg = 'User Already Loggedin';
          }          
          else if (objResponse.status == '500' && (errorMsg.indexOf('<Code>1026</Code>') >= 0 || errorMsg.indexOf("code='1026'") >= 0)) {
            //4.3 errorMsg: "<BarcoError code='1026'><Description>ads</Description></BarcoError>"
            var clientId = errorMsg.substring(errorMsg.indexOf('<Description>') + '<Description>'.length, errorMsg.indexOf('</Description>'));
            errorMsg = 'Invalid client name'+':'+clientId;
          }
          else if (objResponse.status == '502' || objResponse.status == '503'){
            errorMsg = 'Server not available';
          }
          else {
            if (errorMsg && errorMsg.indexOf('</Description>') >= 0) {
               errorMsg = errorMsg.substring(errorMsg.indexOf('<Description>') + '<Description>'.length, errorMsg.indexOf('</Description>'));
            }
          }
          var retObj = { State: "ERROR", Error: { Code: objResponse.status, Desc: errorMsg} };
        }
        callback(retObj);
      }, this))
    }
  },

  /**
  * Create long pulling channel
  * @param urlStr {String} 
  */
  _createLongPollingChannel: function (urlStr) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "_createLongPollingChannel", "call", urlStr);
    var thisObj = this;
    Request.doGET(urlStr, function (response, respStatus) {
      if (respStatus && respStatus == "success") {
        if (response.responseText) {
          if (response.responseText.indexOf('LoggedOut') < 0) {
            //handle notifications if not empty, otherwise expired got: "{"NotificationList": ""}"
            var responseObj = JSON.parse(response.responseText);
            if (responseObj.NotificationList || (responseObj.NotificationList == "")) {
              // Send long polling request when 
              // [1] have a notification but not KickedOut or LoggedOut; {"NotificationList": {"KickedOut": ""}}, {"NotificationList": {"LoggedOut": ""}}
              // [2] notification expired
              thisObj._handleNotification(responseObj.NotificationList);
              if (response.responseText.indexOf('KickedOut') < 0)
                thisObj._createLongPollingChannel(urlStr);
            }
          }
        }
        else {
          //if no responseText then publish error
          jQuery.publish("NotificationError", { responseText: "Empty Notification" });
        }
      }
      else {
        //if not success then publish error
        jQuery.publish("NotificationError", { respStatus: respStatus });
      }
    });
  },

  /**
  * Handle notification with NotificationList
  * @param NotificaitonList {JSON} 
  */
  _handleNotification: function (NotificationList) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "_handleNotification", "call", "");
    if (NotificationList && NotificationList != "") {
      var NotificationArray = null;
      if ((NotificationList.Notification) instanceof Array) {
        NotificationArray = NotificationList.Notification;
      }
      else if ((NotificationList.x) instanceof Array) {
        NotificationArray = NotificationList.x;
      }
      //If there are more than 1 notification then it is an array,
      if (NotificationArray) {
        jQuery.each(NotificationArray, function (key, notificationNode) {
          for (propertyName in notificationNode) {
            jQuery.publish(propertyName, notificationNode[propertyName]);
          }
        });
      }
      else {
        //single notification, special cases: {"NotificationList": {"KickedOut": ""}} {"NotificationList": {"LoggedOut": ""}}
        var notificationNode = NotificationList.Notification ? NotificationList.Notification : NotificationList;
        for (propertyName in notificationNode) {
          jQuery.publish(propertyName, notificationNode[propertyName]);
        }
      }
    }
  },

  /**
  * Initiates logout process and sends logout request.
  */
  doLogout: function (callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "doLogout", "call", "");
    Request.Logout(jQuery.proxy(function (objResponse, textStatus) {
      if (objResponse.status == 200) {
        this._setStateParam(this.LOGINSTATE, this.LOGIN_NONE);
        var retObj = { State: "SUCCESS", Error: { Code: "", Desc: ""} }
      }
      else {
        //TBD error handling based on error code/description
        var retObj = { State: "ERROR", Error: { Code: objResponse.status, Desc: objResponse.responseText, Timedout: objResponse.timedout, Aborted: objResponse.aborted, KickOut: objResponse.kickout} };
      }
      callback(retObj);
    }, this))
  },
  
  /**
  * Initiates kickout process and sends kickout request.
  */
  kickout: function (callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "kickout", "call", "");
    Request.Kickout(jQuery.proxy(function (objResponse, textStatus) {
      if (objResponse.status == 200) {
        this._setStateParam(this.LOGINSTATE, this.LOGIN_NONE);
        var retObj = { State: "SUCCESS", Error: { Code: "", Desc: ""} }
      }
      else {
        //TBD error handling based on error code/description
        var retObj = { State: "ERROR", Error: { Code: objResponse.status, Desc: objResponse.responseText, Timedout: objResponse.timedout, Aborted: objResponse.aborted, KickOut: objResponse.kickout} };
      }
      callback(retObj);
    }, this))
  }

})