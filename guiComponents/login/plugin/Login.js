/* ============================================================================
 * Login jQuery plugin
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */

var Login = Class.extend({
  /**
  * _defaults options 
  */
  _defaults: {
    //autologin: need clearify this user case  
    autoLogin: false,
    //required in inital
    barcoAPI: null,
    //default web clientId
    defaultWebClientId: 'web-client',
    //enforce kickout while login 
    enforceKickout: false,
    ipAddress: '',
    protocol: 'http:',
    //required in inital
    serviceVersion: null,
    //login call back data
    loginDataInfo: { state: 'NONE', data: '', errorCode: '', errorDesc: '' },
    //authentication params
    userAuthParams: { user: '', password: '', server: '', protocol: '', clientId: '', serviceVersion: '' },
    //mode  "Client|Network"
    mode: "Client"
  },

  /**
  * init function 
  * @param options {JSON}, 
  * @param elem {Object}
  */
  init: function (options, elem) {
    //initialize instance variables
    this.txtUsername = null;
    this.txtPassword = null;
    this.txtClientId = null;
    this.btnClientId = null;
    this.btnLogin = null;
    this.eventType = "onLoginStateChange";
    this.options = jQuery.extend({}, this._defaults, options);
    this.options.defaultWebClientId = (this.options.mode == "Client") ? 'web-client' : 'wan-client';
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._build();
    this._setFields();
    //auto login using previous user credential info.
    if (this.options.autoLogin) {
      if (this.getAuthParams()) {
        this.doLogin(this.options.userAuthParams.user, this.options.userAuthParams.password, this.options.userAuthParams.server, this.options.userAuthParams.protocol, this.options.userAuthParams.clientId, this.options.userAuthParams.serviceVersion);
      }
    }
    this.log(bINFO, "init", "return", "Done");
  },

  /**
  * _build function, to inject HTML 
  */
  _build: function () {
    var clientSettingStr = '';
    if (this.options.mode == "Network") {
      clientSettingStr =
        '      <div id="divNetworkClientSetting" class="form-group segmentBtn ">' +
        '          <input id="segLanClient" type="radio" name="networkCientSetting" value="LAN">' +
        '          <label for="segLanClient">LAN</label> ' +
        '          <input id="segWanClient" type="radio" name="networkCientSetting" value="WAN" checked="checked" >' +
        '          <label for="segWanClient">WAN</label> ' +
        '      </div>';
    } else {
      clientSettingStr =
        '      <div id="divLocalClientSetting" class="form-group clearfix">' +
        '         <input type="text" class="form-control" style="width:140px;float:left;margin:0;display:none" value="web-client" placeholder="" id="client-id" >' +
        '         <a id="btnClientId" href="#" class="client-settings-btn pull-right">' +
        '         <i class="fa fa-cog" style="margin-right:8px"></i>Client Settings</a>' +
        '      </div>';
    }
    var html =
        '<div class="card" style="width:300px;height:280px; margin:70px auto 50px;">' +
        '   <div class="form-stacked">' +
        '      <h4 class="pull-center">Login</h4>' +
        '      <div class="form-group">' +
        '         <label>Username</label>' +
        '         <input type="text" class="form-control" value="" placeholder="" id="login-name" data-validation="length" data-validation-length="min1">' +
        '      </div>' +
        '      <div class="form-group">' +
        '         <label>Password</label>' +
        '         <input type="password" class="form-control" value="" placeholder="" id="login-pass">' +
        '      </div>' + clientSettingStr +
        '      <div class="form-group" style="margin-top:20px">' +
        '         <a id="login-submit" href="#" class="btn btn-primary btn-block text-center">Login</a>' +
        '      </div>' +
        '      <div id="login-error-msg" class="hidden" style="margin:0 auto;" >' +
        '      </div>' +
        '   </div>' +
        '</div>';
    jQuery(html).appendTo(this.$elem);
    this.log(bINFO, "_build", "return", "Done");
  },

  /**
  * conf function 
  */
  conf: function (options) {
    this.options = jQuery.exentd({}, this.options, options);
  },

  /**
  * destroy function 
  */
  destroy: function () {
    this._resetFields();
    this.$elem.removeData('edp_login');
    this.log(bINFO, "destroy", "return", "Done");
  },

  /**
  * _setFields function, get handler of view element fields 
  */
  _setFields: function () {
    this.txtUsername = this.$elem.find('#login-name');
    this.txtPassword = this.$elem.find('#login-pass');
    if (this.options.mode == "Client") {
      this.txtClientId = this.$elem.find('#client-id');
      this.btnClientId = this.$elem.find('#btnClientId');
    } else {
      this.selectionNetworkClient = this.$elem.find('input:radio[name=networkCientSetting]');
    }
    this.btnLogin = this.$elem.find('#login-submit');
    this.divLoginErrMsg = this.$elem.find('#login-error-msg');
    this._attachEvents();
  },

  /**
  * _resetFields function, reset view element fields 
  */
  _resetFields: function () {
    this._detachEvents();
    this.txtUsername = null;
    this.txtPassword = null;
    if (this.options.mode == "Client") {
      this.txtClientId = null;
      this.btnClientId = null;
    } else {
      this.selectionNetworkClient = null;
    }
    this.btnLogin = null;
    this.divLoginErrMsg = null;
  },

  /**
  * _validateTxtFields function 
  * @param inputTxtElement {Object}, 
  */
  _validateTxtFields: function (inputTxtElement) {
    if (inputTxtElement.val() == "") {
      inputTxtElement.fadeIn().fadeOut().fadeIn();
    }
  },

  /**
  * _attachEvents function, attach events 
  */
  _attachEvents: function () {
    this.btnLogin.on('click', jQuery.proxy(this._onClickLogin, this));
    if (this.options.mode == "Client") {
      this.btnClientId.on('click', jQuery.proxy(this._onClickbtnClientId, this));
    } else {
      this.selectionNetworkClient.on('click', jQuery.proxy(this._onNetworkClientSetting, this));
    }
    this.txtUsername.on('blur', jQuery.proxy(this._validateTxtFields, this, this.txtUsername));
    this.txtPassword.on('blur', jQuery.proxy(this._validateTxtFields, this, this.txtPassword));
    this.$elem.on('keypress', jQuery.proxy(this._onEnterLogin, this));
    jQuery.subscribe("KickedOut", jQuery.proxy(this._handlKickedoutNotification, this));
    this.$elem.find('input').on('keypress', jQuery.proxy(this._hideErrMsg, this));
  },

  /**
  * _detachEvents function, detach events
  */
  _detachEvents: function () {
    this.btnLogin.off('click', jQuery.proxy(this._onClickLogin, this));
    if (this.options.mode == "Client") {
      this.btnClientId.off('click', jQuery.proxy(this._onClickbtnClientId, this));
    } else {
      this.selectionNetworkClient.off('click', jQuery.proxy(this._onNetworkClientSetting, this));
    }
    this.txtUsername.off('blur', jQuery.proxy(this._validateTxtFields, this, this.txtUsername));
    this.$elem.off('keypress', jQuery.proxy(this._onEnterLogin, this));
    jQuery.unsubscribe("KickedOut", jQuery.proxy(this._handlKickedoutNotification, this));
    this.$elem.find('input').off('keypress', jQuery.proxy(this._hideErrMsg, this));
  },

  /**
  * _handlKickedoutNotification function 
  * @param event {Object}, 
  * @param data {JSON}
  */
  _handlKickedoutNotification: function (event, data) {
    this.log(bINFO, "_handlKickedoutNotification", "call", "Notification call back, event topic: " + event.type);
    this.options.loginDataInfo = { state: "KICKEDOUT", data: '', errorCode: '', errorDesc: '' };
    if (this.options.callback) this.options.callback.call(this, this.eventType, this.options.loginDataInfo);
    this.options.loginDataInfo = { state: 'NONE', data: '', errorCode: '', errorDesc: '' };
  },


  /**
  * _onEnterLogin function 
  * @param event {Object}, 
  * @param data {JSON}
  */
  _onEnterLogin: function (e, data) {
    if (e.keyCode == 13) {
      this._onClickLogin();
    }
  },

  /**
  * _onClickLogin function 
  */
  _onClickLogin: function () {
    var user = this.txtUsername.val(),
        password = this.txtPassword.val(),
        clientId = '';

    if (this.options.mode == "Client") {
      clientId = this.txtClientId.val();
    } else {
      clientId = this.options.defaultWebClientId;
    }

    var errorMsg = '';
    if (!user) {
      errorMsg = 'Username is required';
      this.txtUsername.focus();
    } else if (!password) {
      errorMsg = 'Password is required';
      this.txtPassword.focus();
    } else if (this.options.mode == "Client" && !this.txtClientId.val().trim()) {
      errorMsg = 'Client is required';
      this._showFocusOnClientId();
    }
    if (errorMsg) {
      this._showErrMsg(errorMsg);
    } else {
      this.doLogin(user, password, this.options.ipAddress, this.options.protocol, clientId, this.options.serviceVersion);
    }

  },

  /**
  * _showFocusOnClientId function 
  */
  _showFocusOnClientId: function () {
    if (!this.txtClientId.is(':visible')) {
      this.txtClientId.show('slide');
    }
    this.txtClientId.val('').focus();
  },

  /**
  * _onClickbtnClientId function 
  */
  _onClickbtnClientId: function () {
    if (this.txtClientId.is(':visible')) {
      this.txtClientId.hide('slide');
    }
    else {
      this.txtClientId.show('slide');
    }
  },

  /**
  * doLogin function 
  * @param user {String}, 
  * @param password {String},
  * @param server_ip {String}, 
  * @param protocol {String},  
  * @param client_id {String},
  * @param service_ver {String} 
  */
  doLogin: function (user, password, server_ip, protocol, client_id, service_ver) {
    this.options.userAuthParams.user = user;
    this.options.userAuthParams.password = password;
    this.options.userAuthParams.clientId = client_id;
    this.options.userAuthParams.server = (location.host == 'localhost') ? server_ip : location.host;
    this.options.userAuthParams.protocol = protocol;
    this.options.userAuthParams.serviceVersion = service_ver;
    this.btnLogin.prop("disabled", true);
    this.options.barcoAPI.doLogin(user, password, protocol, server_ip, client_id, service_ver, jQuery.proxy(function (resp) {
      this.btnLogin.prop("disabled", false);
      if (resp.State === "SUCCESS") {
        this.log(bDEBUG, "doLogin", "success", resp.State);
        //this._loginSuccessCb.call(this, resp);
        this.options.loginDataInfo = { state: "SUCCESS", data: '', errorCode: '', errorDesc: '' };
        this._getServiceClient();
      }
      else {
        this.options.loginDataInfo = { state: "ERROR", data: '', errorCode: resp.Error.Code, errorDesc: resp.Error.Desc };
        this._loginFailCb.call(this, resp);
        if (this.options.callback) this.options.callback(this.eventType, this.options.loginDataInfo);
      }
    }, this));
  },

  /**
  * _loginSuccessCb function 
  */
  _loginSuccessCb: function () {
    this.txtPassword.val('');
    this.hide();
    this.log(bINFO, "_loginSuccessCb", "return", "Done");
    if (this.options.callback) this.options.callback(this.eventType, this.options.loginDataInfo);
  },

  _getServiceClient: function () {
    var url = this.options.protocol + "//" + this.options.ipAddress + "/controller/Service/ServiceClient";
    this.options.barcoAPI.getServiceClient(url, jQuery.proxy(function (status, respData) {
      var userPermissions = {};
      var privateData = "";
      if (status === "success") {
        this.log(bDEBUG, "getServiceClient", "success", "");
        userPermissions = respData.GetServiceClientResponseData.GetServiceClient.Permissions;
        privateData = respData.GetServiceClientResponseData.GetServiceClient.User.UserPrivateData;
      }
      else {
        this.log(bERROR, "getServiceClient", "Failed", respData.State);
      }
      if (this.options.userPermissions) {
        this.options.userPermissions.setPermissions(userPermissions);
        this.options.userPermissions.setPrivateData(privateData);
      }
      this._loginSuccessCb();
    }, this));
  },

  /**
  * _loginFailCb function 
  * @param resp {JSON}, 
  */
  _loginFailCb: function (resp) {
    this.log(bDEBUG, "_loginFailCb", "call", errorMsg);
    var errorMsg = resp.Error.Desc;
    if (resp.Error.Code == '409') {
      //user already logged in
      if (!this.options.enforceKickout) {
        var $dialog = $('<div/>').html("<i>This user is logged in on different device, would you like to end that session and log in here?<i>").dialog({
          height: 200,
          width: 320,
          autoOpen: true,
          title: 'Confirmation',
          show: { effect: "blind", duration: 800 },
          buttons: {
            "Yes": jQuery.proxy(function () {
              this.kickoutAndLogin('Login here even if the user already login on different device.');
              $dialog.dialog("close");
            }, this),
            "No": jQuery.proxy(function () {
              this.txtUsername.val('');
              this.txtPassword.val('');
              $dialog.dialog("close");
            }, this)
          }
        });
      }
      else {
        this.kickoutAndLogin('Enforce to kickout other user and login here.');
      }
    }
    else {
      if (resp.Error.Code == '500' && errorMsg.indexOf('Invalid client name') >= 0 && this.options.mode == "Client") {
        this._showFocusOnClientId();
      }
      this._showErrMsg(errorMsg);
    }
  },

  /**
  * show error message function 
  */
  _showErrMsg: function (errorMsg) {
    this.txtPassword.val('');
    this.divLoginErrMsg.removeClass('hidden');
    this.divLoginErrMsg.text(errorMsg);
  },

  /**
  * hide error message function 
  */
  _hideErrMsg: function () {
    this.divLoginErrMsg.addClass('hidden');
    this.divLoginErrMsg.text('');
  },

  /**
  * show function 
  */
  show: function () {
    this.$elem.removeClass('hidden');
  },

  /**
  * hide function 
  */
  hide: function () {
    this.$elem.addClass('hidden');
  },

  /**
  * kickoutAndLogin function 
  * @param msg {String},  
  **/
  kickoutAndLogin: function (msg) {
    this.options.barcoAPI.kickout(jQuery.proxy(function (resp) {
      if (resp.State == "SUCCESS") {
        this.log(bINFO, "kickoutAndLogin", "call", "Kickout request success (" + msg + ").");
        this.doLogin(this.options.userAuthParams.user, this.options.userAuthParams.password, this.options.userAuthParams.server, this.options.userAuthParams.protocol, this.options.userAuthParams.clientId, this.options.userAuthParams.serviceVersion);
      }
      else {
        this.log(bINFO, "kickoutAndLogin", "call", "Kickout fail");
      }
    }, this));
  },

  /**
  * doLogout function 
  */
  doLogout: function () {
    this.options.barcoAPI.doLogout(jQuery.proxy(function (resp) {
      if (resp.State == "SUCCESS") {
        this.options.loginDataInfo = { state: "LOGGEDOUT", data: '', errorCode: '', errorDesc: '' };
      }
      else {
        this.options.loginDataInfo = { state: "LOGGEDOUT", data: '', errorCode: resp.Error.Code, errorDesc: resp.Error.Desc };
      }
      this.log(bDEBUG, "doLogout", "call", resp.State);
      if (this.options.userPermissions) this.options.userPermissions.setPermissions({});
      if (this.options.callback) this.options.callback.call(this, this.eventType, this.options.loginDataInfo);
      this.options.loginDataInfo = { state: 'NONE', data: '', errorCode: '', errorDesc: '' };
    }, this));
  },

  /**
  * setAuthParams function 
  * @param authParamObj {JSON}, 
  */
  setAuthParams: function (authParamObj) {
    this.options.userAuthParams.user = authParamObj.user;
    this.options.userAuthParams.password = authParamObj.password;
    this.options.userAuthParams.server = authParamObj.server;
    this.options.userAuthParams.clientId = authParamObj.clientId;
    this.options.userAuthParams.serviceVersion = authParamObj.serviceVersion;
    this.options.userAuthParams.callback = authParamObj.callback;
  },

  /**
  * getAuthParams function 
  * return {JSON}
  */
  getAuthParams: function () {
    return this.options.userAuthParams;
  },

  /**
  * _onNetworkClientSetting function 
  */
  _onNetworkClientSetting: function (evt) {
    var clientMode = evt.currentTarget.value;
    this.options.defaultWebClientId = (clientMode == "LAN") ? 'lan-client' : 'wan-client';
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('edp_login', Login);
