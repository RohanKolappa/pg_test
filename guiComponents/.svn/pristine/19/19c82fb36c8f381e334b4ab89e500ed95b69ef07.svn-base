/* ============================================================================
 * AddEditUser jQuery plugin
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */

var AddEditUser = Class.extend({
  /**
   * _defaults options 
   */
  _defaults: {
    userProps: {}
  },
  _resource: null,
  /**
   * init function 
   * @param options {JSON}, 
   * @param elem {Object}
   */
  init: function (options, elem) {
    //initialize instance variables
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._resource = BarcoUtil.setupLocalization(BarcoCommonResource.langs, 'en');
    this._build();
    this._setFields();
    this.log(bINFO, "init", "return", "Done");
  },
  /**
   * _build function, to inject HTML 
   */
  _build: function () {
    var html = '<form style="margin-top: 50px;">';
    if (this._showField('fullName'))
      html += '<div class="form-group">' +
      '  <label>' + this._t('umFullName') + ':</label>' +
      '  <input id="addEditUserName" type="text" class="form-control" value="" placeholder="' +
      this._t('umFullNameP') +'" data-validation-engine="validate[maxSize[32]]">' +
      '</div>';
    if (this._showField('username'))
      html += '<div class="form-group">' +
      '  <label>' + this._t('umUsername') + ':</label>' +
      '  <input id="addEditUserId" type="text" class="form-control" value="" placeholder="' +
      this._t('umUsernameP') + '" data-validation-engine="validate[required,maxSize[14],custom[onlyLetterNumber]]">' +
      '</div>';
    if (this._showField('password'))
      html += '<div class="form-group">' +
      '  <label>' + this._t('umPassword1') + ':</label>' +
      '  <input id="addEditUserPw1" type="password" class="form-control validate[required]" value="" placeholder="' +
      this._t('umPassword1P') + '" data-validation-engine="validate[required,maxsize[14],custom[password]]">' +
      '</div>' +
      '<div class="form-group">' +
      '  <label>' + this._t('umPassword2') + ':</label>' +
      '  <input id="addEditUserPw2" type="password" class="form-control" value="" placeholder="' +
      this._t('umPassword2P') + '" data-validation-engine="validate[required,maxsize[14],custom[password]]">' +
      '</div>';
    if (this._showField('ctrlaccess'))
      html += '<div class="form-group" style="white-space:nowrap;">' +
      '  <input type="radio" name="addEditUserAccess" value="View Only" checked>&nbsp' +
      this._t('umViewOnly') + '&nbsp;&nbsp;' +
      '  <input type="radio" name="addEditUserAccess" value="Controller">&nbsp;' +
      this._t('umController') +
      '</div>';
    html += '</form>'
    jQuery(html).appendTo(this.$elem);
    this.$form = this.$elem.find('form');
    if (this._showField('fullName'))
      this.$txtFullname = this.$elem.find('#addEditUserName');
    if (this._showField('username'))
      this.$txtLoginName = this.$elem.find('#addEditUserId');
    if (this._showField('password')) {
      this.$txtPassword1 = this.$elem.find('#addEditUserPw1');
      this.$txtPassword2 = this.$elem.find('#addEditUserPw2');
    }
    if (this._showField('ctrlaccess'))
      this.$radAccess = this.$elem.find('input[name="addEditUserAccess"]');
    this.log(bINFO, "_build", "return", "Done");
  },
  /**
   * destroy function 
   */
  destroy: function () {
    this._resetFields();
    this.$elem.removeData('edp_addEditUser');
    this.log(bINFO, "destroy", "return", "Done");
  },
  /**
   * set edit mode vs. add new user
   * @param {boolean} editMode
   * @returns {undefined}
   */
  setEditMode: function (editMode) {
    this.editMode = editMode;
    if (this._showField('username')) {
      if (editMode) {
        this.$txtLoginName.prop("readonly", true);
      } else {
        this.$txtLoginName.prop("readonly", false);
      }
    }
  },
  /**
   * doSave function 
   */
  doSave: function () {
    var p = this.options.userProps;
    if (p) {
      var valid = this.$form.validationEngine('validate');
      var pwMatchGood = true;
      if (this._showField('password')) pwMatchGood = (p.password === p.pwCheck);
      if (valid && pwMatchGood) {
        this._saveToServer();
      } else {
        if (this.options.callBack) {
          if (this._showField('password') && !pwMatchGood)
            this.options.callBack('pwMismatch', this._t('umPwMismatch'));
        }
      }
    } else {
        if (this.options.callBack) {
          this.options.callBack('empty', this._t('umEmptyForm'));
        }
    }
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
    this.$form.validationEngine('hide');
    this.$elem.addClass('hidden');
  },
  /**
   * clearUserProps function 
   */
  clearUserProps: function () {
    this.options.userProps = {};
    if (this._showField('fullName')) this.$txtFullname.val("");
    if (this._showField('username')) this.$txtLoginName.val("");
    if (this._showField('password')) {
      this.$txtPassword1.val("");
      this.$txtPassword2.val("");
    }
  },
  /**
   * setUserProps function 
   * @param authParamObj {object}, 
   */
  setUserProps: function (obj) {
    this.options.userProps = obj;
    if (this._showField('fullName')) this.$txtFullname.val(obj.fullName);
    if (this._showField('username')) this.$txtLoginName.val(obj.username);
    if (this._showField('password') && obj.password) {
      this.$txtPassword1.val(obj.password);
      this.options.userProps.pwCheck = obj.password;
      this.options.userProps.oldPw = obj.password;
      this.$txtPassword2.val(obj.password);
    }
    if (this._showField('ctrlaccess') && obj.groups_tag) {
      // TBD: Search for tag with key of 'ctrlaccess'. If true, set selector
      // to Controller radio button, else View Only radio.
    }
  },
  /**
   * getUserProps function 
   * @returns {object}
   */
  getUserProps: function () {
    return this.options.userProps;
  },
  _showField: function(fld) {
    return this.options.showFields.indexOf(fld) >= 0;
  },
  /**
   * _setFields function, get handler of view element fields 
   */
  _setFields: function () {
    this._attachEvents();
  },
  /**
   * _resetFields function, reset view element fields 
   */
  _resetFields: function () {
    this._detachEvents();
  },
  /**
   * _validateTxtFields function 
   * @param inputTxtElement {Object}, 
   */
  _onFieldBlur: function ($inp) {
    switch ($inp.selector) {
      case '#addEditUserName':
        this.options.userProps.fullName = $inp.val(); break;
      case '#addEditUserId':
        this.options.userProps.username = $inp.val(); break;
      case '#addEditUserPw1':
        this.options.userProps.password = $inp.val(); break;
      case '#addEditUserPw2':
        this.options.userProps.pwCheck = $inp.val(); break;
      }
  },
  /**
   * _attachEvents function, attach events 
   */
  _attachEvents: function () {
    if (this._showField('fullName'))
      this.$txtFullname.on('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtFullname));
    if (this._showField('username'))
      this.$txtLoginName.on('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtLoginName));
    if (this._showField('password')) {
      this.$txtPassword1.on('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtPassword1));
      this.$txtPassword2.on('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtPassword2));
    }
    this.$form.validationEngine('attach');
  },
  /**
   * _detachEvents function, detach events
   */
  _detachEvents: function () {
    this.$form.validationEngine('detach');
    if (this._showField('password')) {
      this.$txtPassword1.off('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtPassword1));
      this.$txtPassword2.off('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtPassword2));
    }
    if (this._showField('username'))
      this.$txtLoginName.off('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtLoginName));
    if (this._showField('fullName'))
      this.$txtFullname.off('blur', jQuery.proxy(this._onFieldBlur, this, this.$txtFullname));
  },
  _saveToServer: function() {
    if (!this._showField('fullName')) // internally make fullName == username
      this.options.userProps.fullName = this.options.userProps.username;
    var accessTag = {ctrlaccess:"false"};
    if (this._showField('ctrlaccess')) {
      var accessSelection = this.$elem.find('input[name="addEditUserAccess"]:checked').val();
      if (accessSelection == "View Only") {
        accessTag = {ctrlaccess:"false"}
      } else if (accessSelection == "Controller") {
        accessTag = {ctrlaccess:"true"}
      }
    }
    var urlStr = this.options.protocol + "//" + this.options.ipAddress + "/controller/Resource/User";
    if (this.options.userProps.groups_tag) {
      // TBD: Need to check if tag already exists and modify, rather than add
      this.options.userProps.groups_tag.push(accessTag);
    } else this.options.userProps.groups_tag = [accessTag];
    var groupsTagsAsJSON = this._convertGroupTagsToJsonAPI(this.options.userProps.groups_tag)
    if (this.editMode) {
      urlStr += '?NID=' + this.options.userProps.NID;
      var pwChanged = this.options.userProps.password !== this.options.userProps.oldPw;
      if (this._showField('fullName') || this._showField('ctrlaccess')) {
        this.options.barcoAPI.changeUserInfoRequest(urlStr, this.options.userProps,
            groupsTagsAsJSON,
            jQuery.proxy(function (status, retData) {
              if (status === "success") {
                if (!pwChanged) this._saveToServerSucceeded.call(this);
              }
              else {
                this._saveToServerFailed.call(this, retData);
              }
            }, this)
          );
      }
      if (pwChanged) {
        this.options.barcoAPI.changeUserPasswordRequest(urlStr, this.options.userProps.oldPw,
            this.options.userProps.password,
            jQuery.proxy(function (status, retData) {
              if (status === "success") {
                this._saveToServerSucceeded.call(this);
              }
              else {
                this._saveToServerFailed.call(this, retData);
              }
            }, this)
          );
      }
    } else { // Adding a new user
      this.options.barcoAPI.addUserRequest(urlStr, this.options.userProps,
          groupsTagsAsJSON,
          jQuery.proxy(function (status, retData) {
            if (status === "success") {
              this._saveToServerSucceeded.call(this);
            }
            else {
              this._saveToServerFailed.call(this, retData);
            }
          }, this)
        );
    }
  },
  /**
   * _saveToServerSucceeded function 
   */
  _saveToServerSucceeded: function () {
    if (this.options.callBack)
      this.options.callBack('success', null);
    this.log(bINFO, "_saveToServerSucceeded", "return", "Done");

  },
  /**
   * _saveToServerFailed function 
   * @param resp {JSON}, 
   */
  _saveToServerFailed: function (resp) {
    this.log(bDEBUG, "_saveToServerFailed", "call", "");
    var errorMsg = "?";
    if (resp.indexOf('BarcoError') >= 0) {
      var r = JSON.parse(resp);
      if (r.BarcoError.Description) {
        if (r.BarcoError.Description.content) {
          errorMsg = r.BarcoError.Description.content + ' [code:' +
            r.BarcoError['@code'] + ']';
        } else {
          errorMsg = r.BarcoError.Description + ' [code:' +
            r.BarcoError['@code'] + ']';
        }
      } else {
        errorMsg = 'Unknown server error [code:' + r.BarcoError['@code'] + ']';
      }
    }
    this.log(bERROR, "_saveToServer", "failed", errorMsg);
    if (this.options.callBack)
      this.options.callBack('failure', errorMsg);
  },
  _convertGroupTagsToJsonAPI: function (tags) {
    var jsonStr = '';
    var len = tags.length;
    if (len > 0) {
      jsonStr += '    "Groups": [';
      for (var i = 0; i < len; i++) {
        var tag = tags[i];
        var key = Object.keys(tag)[0];
        var val = tag[key];
        jsonStr += '      {' +
          '        "Tag": {' +
          '          "@Name": "' + key + '",' +
          '          "@Value": "' + val + '"' +
          '        }' +
          '      }';
        if (i < len-1) jsonStr += ',';
      }
      jsonStr += '    ],';
    }
    return jsonStr;
  },
  // Localization functions
  _t: function(id) { return this._resource.ref[id]; }
});

// expose Objects as jQuery plugins
jQuery.plugin('edp_AddEditUser', AddEditUser);
