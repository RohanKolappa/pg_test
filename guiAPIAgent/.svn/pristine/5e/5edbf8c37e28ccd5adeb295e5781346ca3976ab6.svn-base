BarcoAPI = function () {
  this.cls = "BarcoAPI";
  BarcoUtil.logger(bDEBUG, "", this.cls, "", "constructor", "");
  this.init();  
}

jQuery.extend(BarcoAPI.prototype, {

  init: function () {
    BarcoUtil.logger(bDEBUG, "", this.cls, "init", "call", "");
    this.clsServiceAgent = new ServiceAgent();
    this.clsSessionAPI = new SessionAPI();
  },

  /**
  *This will set callback function to send notifications back
  * @param notificationCallback {Object}
  */
  registerCallback: function (notificationCallback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "registerCallback", "call", "");
    this.notificationCallback = notificationCallback;
    jQuery.subscribe(this.clsServiceAgent.MSG_PORTCONNECTIONUPDATE, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsServiceAgent.MSG_PORTCONNECTIONSTATEUPDATE, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsServiceAgent.MSG_PORTUPDATE, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsServiceAgent.MSG_NOTIFICATIONERROR, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsServiceAgent.MSG_PORTDISPLAYCONFIGUPDATE, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsServiceAgent.MSG_PORTDISPLAYSTATUSUPDATE, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsServiceAgent.MSG_KICKEDOUT, jQuery.proxy(this._handleNotifications, this));
    jQuery.subscribe(this.clsSessionAPI.MSG_SESSIONUPDATE, jQuery.proxy(this._handleNotifications, this));
  },

  _handleNotifications: function (eventType, eventData) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "_handleNotifications", "call", eventType);
    this.notificationCallback(eventType.type, eventData);
  },

  /**
  * Initiates login process.
  * Calls callback function after recieved response.
  * @param userName {String} 
  * @param password {String}
  * @param protocol {String}
  * @param ipAddress {String} 
  * @param clientId {String}
  * @param serviceVerion {String} 
  * @param callback {Object}
  */
  doLogin: function (username, password, protocol, ipaddress, clientid, serviceVerion, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "doLogin", "call",
      clientid + ', ' + serviceVerion);
    this.clsServiceAgent.doLogin(username, password, protocol, ipaddress, clientid, serviceVerion, function (response) {
      if (jQuery.isFunction(callback)) callback(response);
    });
  },

  /**
  * Initiates getServiceClient process.
  * @param callback {Object}
  */
  getServiceClient: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getServiceClient", "call", url);
    Request.doGET(url, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    });
  },


  /**
  * Initiates logout process.
  * @param callback {Object}
  */
  doLogout: function (callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "doLogout", "call", "");
      this.clsServiceAgent.doLogout(function (response) {
          if (jQuery.isFunction(callback)) callback(response);
      });
  },


  /**
  * Sends getport request via Request
  * @param url {String}
  * @param callback {Object}
  */
  getPortList: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getPortList", "call", url);
    if (url.indexOf("http://") != 0 && url.indexOf("https://") != 0) url = Request.baseURL + '/Resource/Port?' + url;
    Request.doGET(url, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
 * getAuthInfo method will return authentication token.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param userName {String} 
 * @param password {String}
 * @param protocol {String} 
 * @param ipAddress {String}
 * @param clientId {String}
 * @return Object {JSON}.
 */
  getAuthInfo: function (username, password, protocol, ipaddress, clientid) {
    return Request.setAuthInfo(username, password, protocol, ipaddress, clientid);
  },

  /**
  * Sends getport request via Request
  * @param url {String}
  * @param callback {Object}
  */
  getPortListWithAuthToken: function (queryStr, authtoken, protocol, ipaddress, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getPortListWithAuthToken", "call", queryStr);
    if (jQuery.isBlank(protocol)) protocol = "http:";
    var url = Request.baseURL + '/Resource/Port';
    if (!jQuery.isBlank(ipaddress)) {
      url = protocol + '//' + ipAddress + '/controller/Resource/Port';
    }
    var queryParams = "expireTime=INFINITE";
    //Check queryString for expiry if there use it or else use infinte
    if (!jQuery.isBlank(queryStr)) {
      if (queryStr.indexOf("expireTime=") == -1) {
        queryParams = queryStr + "&" + queryParams;
      }
      else {
        queryParams = queryStr;
      }
    }
    url = url + "?" + queryParams;
    Request.doGETCustomHeader(url, authtoken, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
  * Sends getDeviceList request via Request
  * @param url {String}
  * @param callback {Object}
  */
  getDeviceList: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getDeviceList", "call", url);
    Request.doGET(url, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
  * addDevice request via Request
  * @param url {String}
  * @param objData {JSON}
  * @param callback {Object}
  */
  addDevice: function (url, objData, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "addDevice", "call", url);
    Request.doPOST(url, objData, 
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * deleteDevice request via Request
  * @param url {String}
  * @param callback {Object}
  */
  deleteDevice: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "deleteDevice", "call", url);
    Request.doDELETE(url, function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * updatePortInfo request via Request
  * @param url {String}
  * @param objData {JSON}
  * @param callback {Object}
  */
  updatePortInfo: function (url, objData, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "updatePortInfo", "call", url);
    var customhdr = { "Accept": "application/vnd.barco.nimbus.Port-Info+json" };
    Request.doPOSTCustomHeader(url, customhdr, objData, 
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * Sends Add/Update Portgroup request via Request
  * @param url {String}
  * @param callback {Object}
  */
  addUpdatePortGroup: function (url, objData, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "addUpdatePortGroup", "call", url);
    var customhdr = { "Accept": "application/vnd.barco.nimbus.PortGroup+json" };
    Request.doPOSTCustomHeader(url, customhdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },
  /**
  * Sends getFile request via Request
  * @param url {String}
  * @param callback {Object}
  */
  getFileList: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getFileList", "call", url);
    Request.doGET(url, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  
  /**
  * Sends getPortTags request via Request
  * @param url {String}
  * @param callback {Object}
  */
  getPortTags: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getPortTags", "call", url);
    if (url.indexOf("http://") != 0 && url.indexOf("https://") != 0) url = Request.baseURL + '/Resource/Port?' + url;
    var customhdr = { "Accept": "application/vnd.barco.nimbus.Port-Tags+json" };
    Request.doGETCustomHeader(url, customhdr, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },


  /**
  * Sends getDisplayConfig request via Request
  * @param url {String}
  * @param callback {Function}
  */
  getDisplayConfig: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getDisplayConfig", "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.Port-DisplayConfig+json" };
    Request.doGETCustomHeader(url, hdr, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
          retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
  * Sends getDisplayStatus request via Request
  * @param url {String}
  * @param callback {Function}
  */
  getDisplayStatus: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getDisplayStatus", "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.Port-DisplayStatus+json" };
    Request.doGETCustomHeader(url, hdr, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
          retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },
  /**
    * Sends getuser request via Request
    * @param url {String}
    * @param callback {Function}
    */
  getUserList: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getUserList", "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.User+json" };
    Request.doGETCustomHeader(url, hdr, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },
  /**
    * Sends add user request via Request
    * @param url {String}
    * @param userProps {Object}
    * @param tags {String/JSON}
    * @param callback {Function}
    */
  addUserRequest: function (url, userProps, groupsTags, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "addUserRequest",
      "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.User+json" };
    var jsonStr =
      '{' +
      '  "AddUser": {';
    jsonStr += groupsTags;
    if (userProps.email == null) userProps.email = "";
    jsonStr += 
      '    "@fullName": "' + userProps.fullName + '",' +
      '    "@loginName": "' + userProps.username + '",' +
      '    "@password": "' + userProps.password + '",' +
      '    "@email": "' + userProps.email + '",' +
      '    "UserPrivateData": ""' +
      '  }' +
      '}';
    var objData = JSON.parse(jsonStr);
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },
  /**
    * Sends add user request via Request
    * @param url {String}
    * @param userProps {Object}
    * @param tags {Array}
    * @param callback {Function}
    */
  changeUserInfoRequest: function (url, userProps, groupsTags, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "changeUserInfo",
      "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.User-Info+json" };
    var jsonStr =
      '{' +
      '  "UserInfo": {';
    jsonStr += groupsTags;
    if (userProps.email == null) userProps.email = "";
    jsonStr += 
      '    "@fullName": "' + userProps.fullName + '",' +
      '    "@email": "' + userProps.email + '"' +
      '  }' +
      '}';
    var objData = JSON.parse(jsonStr);
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },
  /**
    * Sends change password request via Request
    * @param url {String}
    * @param oldPassword {String}
    * @param userProps {Object}
    * @param callback {Function}
    */
  changeUserPasswordRequest: function (url, oldPassword, newPassword, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "changeUserPassword",
      "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.User-Password+json" };
    var jsonStr =
      '{' +
      '  "UserPassword": {' +
      '    "@oldPassword": "' + oldPassword + '",' +
      '    "@newPassword": "' + newPassword + '"' +
      '  }' +
      '}';
    var objData = JSON.parse(jsonStr);
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },
  /**
    * Sends getuser request via Request
    * @param url {String}
    * @param callback {Object}
    */
  deleteUserRequest: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "deleteUser", "call", url);
    Request.doDELETE(url, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
  * Sends setPortWindowPropertiesRequest request via Request
  * @param url {String}
  * @param dstWinId {String}
  * @param key {String}
  * @param key {String}
  * @param callback {Function}
  */
  setPortWindowPropertiesRequest: function(url, dstWinId, key, value, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "sendSetPortWindowPropertiesRequest",
      "call", url + ', ' + dstWinId + ', ' + key + ', ' + value);
    if (dstWinId && dstWinId != "") {
      var winProphdr = {"Accept": "application/vnd.barco.nimbus.Port-WindowProperties+json"};
      var jsonStr = '{"PortWindowProperties": {' +
        '"@byWindowId":"' + dstWinId + '",' +
        '"WindowProperties": {' +
          '"' + key + '": "' + value + '"}}}';
      BarcoUtil.logger(bDEBUG, "", this.cls, "sendSetPortWindowPropertiesRequest",
        "block", jsonStr);
      var objData = JSON.parse(jsonStr);
      Request.doPOSTCustomHeader(url, winProphdr, objData,
        function (response, status) {
          var retData = response.responseText;
          if (status == "success") {
              retData = JSON.parse(response.responseText);
          }
          if (jQuery.isFunction(callback)) callback(status, retData);
        });
    }
  },

  /**
  * Sends updateFileInfoRequest request via Request
  * @param url {String}
  * @param newTitle {String}
  * @param desc {String}
  * @param callback {Function}
  */
  updateFileInfoRequest: function(url, newTitle, desc, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "updateFileInfoRequest",
      "call", url + ', ' + newTitle + ', ' + desc);
    var hdr = {"Accept": "application/vnd.barco.nimbus.File-Info+json"};
    var jsonStr = '{"FileInfo": {' +
      '"@title":"' + newTitle + '",' +
      '"@description":"' + desc + '"}}';
    var objData = JSON.parse(jsonStr);
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
            retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * Sends updatePortInfoRequest request via Request
  * @param url {String}
  * @param newTitle {String}
  * @param desc {String}
  * @param callback {Function}
  */
  updatePortInfoRequest: function (url, newTitle, desc, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "updatePortInfoRequest",
      "call", url + ', ' + newTitle + ', ' + desc);
    var hdr = { "Accept": "application/vnd.barco.nimbus.Port-Info+json" };
    var jsonStr = '{"PortInfo": {' +
      '"@title":"' + newTitle + '",' +
      '"@description":"' + desc + '"}}';
    var objData = JSON.parse(jsonStr);
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * updatePortStreamURLRequest
  * @param url {String}
  * @param strSDP {String}
  * @param callback {Function}
  */
  updatePortStreamURLRequest: function (url, strSDP, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "updatePortStreamURLRequest", "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.Port-StreamURL+json" };
    var objData = {"PortStreamURL": {
                      "@streamURL": strSDP
                  }};
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * Sends getDir request via Request
  * @param url {String}
  * @param callback {Function}
  */
  getDir: function (url, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getDisplayConfig", "call", url);
    var hdr = { "Accept": "application/vnd.barco.nimbus.Dir+json" };
    Request.doGETCustomHeader(url, hdr, function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
            retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
  * Sends PortPVRInfo request via Request
  * @param dirNID {String} 
  * @param trimlength {String} 
  * @param callback {Function}
  */
  setPortPVRInfo: function (url, dirNID, trimlength, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "setPortPVRInfo", "call", url + ', ' + dirNID + ', ' + trimlength);
    var hdr = { "Accept": "application/vnd.barco.nimbus.Port-PVRInfo+json" };
    var objData = {"PortPVRInfo": {
                      "@mediaStreamDirResourceNID": dirNID,
                      "@trimLength":trimlength,
                      "@enable": "true"
                  }};
    Request.doPOSTCustomHeader(url, hdr, objData,
      function (response, status) {
        var retData = response.responseText;
        if (status == "success") {
          retData = JSON.parse(response.responseText);
        }
        if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * Sends GetSession request via Request
  * @param url {String}
  * @param callback {Object}
  */
  getSessionList: function (url, objData, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionList", "call", url);
    Request.doPOST(url, objData, function (response, status) {
      var retData = response.responseText;
      if (status == "success") {
        retData = JSON.parse(response.responseText);
      }
      if (jQuery.isFunction(callback)) callback(status, retData);
    })
  },

  /**
  * Sends View/Playback request via Request
  * @param mode {string}
  * @param srcTitle {string}
  * @param callback {Function}
  */
  /*   
  play: function (mode,srcTitle,srcNID,callback) {
  var reqParams = new Object();
  if(mode == "LIVE"){
  //reqParams.pvrEnable = "true";
  reqParams.srcPortTitle = srcTitle;
  reqParams.srcPortNID = srcNID;
  }
  else{
  reqParams.fileTitle = srcTitle;
  reqParams.fileNID = srcNID;
  }
  reqParams.dstPortTitle = this.clsServiceAgent.getLoginParam(this.clsServiceAgent.LOGIN_RESOURCE) + "_" + this.clsServiceAgent.getLoginParam(this.clsServiceAgent.LOGIN_USER);
  reqParams.dstPortIP = this.clsServiceAgent.getLoginParam(this.clsServiceAgent.LOGIN_SERVER);
  // assign a sessionId;restart session with existing session id
  reqParams.sessionId = "Session_A1B2C3D4E5F6G7H8";
  reqParams.RestartSession = true;

  this.clsSessionAgent.View(reqParams, function (status, response) {
  var retData = response;
  if (status == "success") {
  retData = JSON.parse(retData);
  }
  if (jQuery.isFunction(callback)) callback(status, retData);
  });
  },
  */

  /**
  * Sends View request via SessionAgent
  * @param jsonObj {JSON}
  * @param callback {Object}
  */
  view: function (jsonObj, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "view", "call", "");
      /*
      var reqParams = new Object();
      reqParams.ReqType = "View";
      reqParams.pvrEnable = "true";
      // TBD - need to find out if NID is really used
      reqParams.SrcPortNID = "";
      reqParams.SrcPortTitle = jsonObj.srcTitle;

      // - assign conn dest info
      reqParams.DstPortTitle = jsonObj.destTitle;
      reqParams.DstEntityId = "";
      reqParams.DstPortNID = "";

      // assign sessionId
      // restart session with existing session id
      reqParams.SessionId = jsonObj.sessionId;
      reqParams.RestartSession = true;

      // assign connId- using the same UUID with the SessionID
      // TBD - need to check if this is right - will end up with same connId all the time
      reqParams.ConnId = reqParams.SessionId.replace(/Session_/, "Conn_");
      reqParams.ConnName = jsonObj.connName;

      // assign canvasId, windowId - reuse existing
      reqParams.CanvasId = jsonObj.CanvaseId;
      reqParams.WindowId = jsonObj.WindowId ;
      */
      this.clsSessionAPI.doView(jsonObj, function (status, response) {
          var retData = response;
          if (status == "success" || status == "") {
              retData = JSON.parse(retData);
          }
          if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },

  /**
  * Sends Playback request via SessionAgent
  * @param jsonObj {JSON}
  * @param callback {Object}
  */
  playback: function (jsonObj, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "playback", "call", "");
      this.clsSessionAPI.doPlayback(jsonObj, function (status, response) {
          var retData = response;
          if (status == "success" || status == "") {
              retData = JSON.parse(retData);
          }
          if (jQuery.isFunction(callback)) callback(status, retData);
      });
  },


  /**
  * Sends StopSession request via SessionAgent
  * @param jsonData {JSON}
  * @param callback {Object}
  */
  stop: function (sessionId, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "stop", "call", sessionId);
      this.clsSessionAPI.doStop(sessionId, function (status, response) {
          if (jQuery.isFunction(callback)) callback(status, response);
      });
  },
  /**
  * Sends Pause request via SessionAgent
  * @param jsonData {JSON}
  * @param callback {Object}
  */
  pause: function (sessionId, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "pause", "call", sessionId);
      this.clsSessionAPI.doPause(sessionId, function (status, response) {
          if (jQuery.isFunction(callback)) callback(status, response);
      });
  },
  /**
  * Sends resume request via SessionAgent
  * @param jsonData {JSON}
  * @param callback {Object}
  */
  resume: function (sessionId, timeScale, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "resume", "call", sessionId);
      this.clsSessionAPI.doResume(sessionId, timeScale, function (status, response) {
          if (jQuery.isFunction(callback)) callback(status, response);
      });
  },
  /**
  * Sends catchup request via SessionAgent
  * @param jsonData {JSON}
  * @param callback {Object}
  */
  catchup: function (sessionId, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "catchup", "call", sessionId);
      this.clsSessionAPI.doCatchup(sessionId, function (status, response) {
          if (jQuery.isFunction(callback)) callback(status, response);
      });
  },
  /**
  * Sends skipback request via SessionAgent
  * @param jsonData {JSON}
  * @param callback {Object}
  */
  skipback: function (sessionId, timeScale, skipOffset, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "skipback", "call", sessionId);
      this.clsSessionAPI.doSkipBack(sessionId, timeScale, actionData, function (status, response) {
          if (jQuery.isFunction(callback)) callback(status, response);
      });
  },

  /**
  * Sends skipfwd request via SessionAgent
  * @param jsonData {JSON}
  * @param callback {Object}
  */
  skipfwd: function (sessionId, timeScale, skipOffset, callback) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "skipfwd", "call", sessionId);
      this.clsSessionAPI.doSkipFwd(sessionId, timeScale, actionData, function (status, response) {
          if (jQuery.isFunction(callback)) callback(status, response);
      });
  },

  startRecording: function (url, reqParams, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "startRecording", "call", "");
    var objData = {
      "Record": {
        "@resetTimeline": "",
        "@name": "",
        "DstFile": {
          "@action": "",
          "@key": "",
          "MetaData": { "Tag": { "@Name": "" } },
          "@keyMetaDataTagName": "",
          "@fileTitle": "",
          "@dirId": "",
          "@grpFileId": "",
          "@id": "",
          "@dirNID": ""
        },
        "GrpFile": {
          "@fileTitle": "",
          "@id": ""
        },
        "@srcEntityId": "",
        "@profileId": "",
        "Profile": {
          "@profileTitle": "",
          "@profileNID": "",
          "@id": ""
        },
        "@sessionId": reqParams.sessionId || "",
        "@id": "",
        "GrpSrcPort": {
          "@portGrpNID": reqParams.portGrpNID || "",
          "@portGrpTitle": reqParams.portGrpTitle || "",
          "Canvas": "",
          "@id": ""
        },
        "@sessionName": "",
        "SrcPort": {
          "@portNID": "",
          "@portTitle": "",
          "@id": ""
        }
      }
    };
    Request.doPOST(url, objData, jQuery.proxy(function (response, status) {
      var sessionObj = null;
      if (status === "success") {
        BarcoUtil.logger(bDEBUG, "startRecording", "", "StartRecord Success fileTitle:" + reqParams.portGrpTitle);
        if (response.responseJSON && response.responseJSON.Record) {
          sessionObj = response.responseJSON.Record.Session;
        }
      }
      if (jQuery.isFunction(callback)) callback(status, sessionObj);
    }, this));
  },

  stopRecording: function (url, sessionId, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "stopRecording", "call", "");
    var objData = { "StopSession": { "@sessionId": sessionId } };
    Request.doPOST(url, objData, jQuery.proxy(function (response, status) {
      var sessionId = null;
      if (status == "success") {
        BarcoUtil.logger(bDEBUG, "stopRecording", "", "StopRecord Success sessionId:" + sessionId);
      }
      if (jQuery.isFunction(callback)) callback(status, sessionId);
    }, this));
  },

  stopSession: function (url, sessionId, callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "stopSession", "call", "");
    var objData = { "StopSession": { "@sessionId": sessionId } };
    Request.doPOST(url, objData, jQuery.proxy(function (response, status) {
      if (jQuery.isFunction(callback)) callback(status, sessionId);
    }, this));
  },

    
  /**
  * Initiates kickout process.
  * @param callback {Object}
  */
  kickout: function (callback) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "kickoutAndLogin", "call", "");
    this.clsServiceAgent.kickout(function (response) {
        if (jQuery.isFunction(callback)) callback(response);
    });
  },

});