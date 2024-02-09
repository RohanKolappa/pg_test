/* ============================================================================
 * User List Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var UserList = Class.extend({
  _defaults: {
  },

  viewState: false,
  listOfUsers: null,
  _resource: null,

  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._resource = BarcoUtil.setupLocalization(BarcoCommonResource.langs, 'en');
    this._build();
    this.log(bDEBUG, "init", "success", "");
  },

  /**
  * _build function 
  */
  _build: function () {
    var html =
      '<table>' +
      '  <thead>' +
      '    <tr class="toolbar-panel">';
    if (this._showColumn('fullName'))
      html += '      <td class="name-cell">' + this._t('umFullName') + '</td>';
    if (this._showColumn('username'))
      html += '      <td class="login-cell">' + this._t('umUsername') + '</td>';
    if (this._showColumn('state'))
      html += '      <td class="status-cell">' + this._t('umState') + '</td>';
    if (this._showColumn('creationDate'))
      html += '      <td class="date-cell">' + this._t('umCreated') + '</td>';
    if (this._showColumn('actions'))
      html += '      <td class="actions-cell">' + this._t('umActions') + '</td>';
    html +=
      '    </tr>' +
      '  </thead>' +
      '  <tbody>' +
      '  </tbody>' +
      '</table>';
    jQuery(html).appendTo(this.$elem);
    this.$tbody = this.$elem.find('tbody');
  },
/**
 * destructor
 * @returns {undefined}
 */
  destroy: function () {
    this.resetList();
    this.$elem.removeData('edp_userList');
  },
/**
 * populates user list
 * @returns {undefined}
 */
  setList: function () {
    if (!this.viewState) {
      this.viewState = true;
      this.getListOfUsers();
      this.show();
    }
  },
/**
 * unpopulates user list
 * @returns {undefined}
 */
  resetList: function () {
    this.hide();
    this.clearList();
    this.listOfUsers = null;
    this.viewState = false;
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
 * sends request to server to get user list
 * @returns {undefined}
 */
  getListOfUsers: function () {
    if (!this.viewState) return;
    this.log(bDEBUG, "getListOfUsers", "call", "");
    var urlStr = this.options.protocol + "//" + this.options.ipAddress + "/controller/Resource/User?sortBy=username&sortOrder=ascending&selectFormat=BASIC&startFrom=0";
    this.options.barcoAPI.getUserList(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, "getListOfUsers", "success", "getListOfUsers success");
        this._onGetListOfUsersSuccess(data);
      } else {
        this.log(bERROR, "getSourceList", "fail", "getListOfUsers failed");
      }
    }, this));
  },
/**
 * Renders an HTML table of users and inserts into the DOM at tbody
 * @returns {undefined}
 */
  displayList: function () {
    jQuery.each(this.listOfUsers, jQuery.proxy(function (key, obj) {
      var del_style = '';
      if (obj.username === 'admin') del_style = ' style="visibility:hidden;"';
      var html = '<tr id="usr_' + obj.NID + '">';
      if (this._showColumn('fullName'))
        html += '  <td class="name-cell">' + obj.fullName + '</td>';
      if (this._showColumn('username'))
        html += '  <td class="login-cell">' + obj.username + '</td>';
      if (this._showColumn('state'))
        html += '  <td class="status-cell">' +
        (obj.state === 'Ready' ? '<span class="user-online">' + this._t('umReady') + '</span>' :
                                 '<span class="user-offline">' + this._t('umOffline') + '</span>') +
        '</td>';
      if (this._showColumn('creationDate'))
        html += '  <td class="date-cell">' + this._date(obj.creationDate) + '  </td>';
      if (this._showColumn('actions')) {
        html += '  <td class="actions-cell">' +
             '    <a class="usr-lst-edit" href="#"><i class="fa fa-pencil" title="' +
              this._t('umListEdit') + '" style="margin-right:20px;"></i></a>' +
             '    <a class="usr-lst-del" href="#" title="' +
             this._t('umDeleteUser') + '"' + del_style + '><i class="fa fa-times"></i></a>' +
             '  </td>';
      }
      html += '</tr>';
       jQuery(html).appendTo(this.$tbody);
    }, this));
    this._attachEvents();
    this.log(bINFO, "displayList", "success", "UserList is ready");
  },
/**
 * Removes the HTML table from the DOM at tbody
 * @returns {undefined}
 */
  clearList: function () {
    this._detachEvents();
    this.$tbody.empty();
  },
/**
 * Performs a user delete request
 * @param {type} usr
 * @returns {undefined}
 */
  doDelete: function (usr) {
    if (!this.viewState) return;
    this.log(bDEBUG, "doDelete", "call", "");
    var urlStr = this.options.protocol + "//" + this.options.ipAddress + "/controller/Resource/User?NID=" + usr.NID;
    this.options.barcoAPI.deleteUserRequest(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, "deleteUser", "success", "deleteUser success");
        if (this.options.callBack)
          this.options.callBack('deleteSucceed', null);
      } else {
        this._deleteFailed.call(this, data)
      }
    }, this));
  },

  _showColumn: function(col) {
    return this.options.showColumns.indexOf(col) >= 0;
  },
  /**
  * _attachEvents function
  */
  _attachEvents: function () {
    if (this._showColumn('actions')) {
      this.$elem.find('a.usr-lst-edit').on('click', jQuery.proxy(this._editUserClicked, this));
      this.$elem.find('a.usr-lst-del').on('click', jQuery.proxy(this._deleteUserClicked, this));
    }
  },

  /**
  * _detachEvents function
  */
  _detachEvents: function () {
    if (this._showColumn('actions')) {
      this.$elem.find('a.usr-lst-edit').off('click', jQuery.proxy(this._editUserClicked, this));
      this.$elem.find('a.usr-lst-del').off('click', jQuery.proxy(this._deleteUserClicked, this));
    }
  },

  _onGetListOfUsersSuccess: function (responseObj) {
    this.listOfUsers = [];
    var userNode = responseObj.Resource;
    if (userNode.User instanceof Array) {
      var userCnt = userNode.User.length;
      for (var i = 0; i < userCnt; i++) {
        var j = this.listOfUsers.push({});
        var listOfUsersProp;
        for (var userNodeProp in userNode.User[i]) {
          var nvp2 = this._translateProperty(userNodeProp, userNode.User[i]);
          if (userNodeProp === "Groups") {
            this._addGroupProperties(j - 1, nvp2.val);
          } else {
            this.listOfUsers[j - 1][nvp2.name] = nvp2.val;
          }
        }
      }
    } else {
      var j = this.listOfUsers.push({});
      var listOfUsersProp;
      for (var userNodeProp in userNode.User) {
        var nvp2 = this._translateProperty(userNodeProp, userNode.User);
        if (userNodeProp === "Groups") {
          this._addGroupProperties(j - 1, nvp2.val);
        } else {
          this.listOfUsers[j - 1][nvp2.name] = nvp2.val;
        }
      }
    }
    this.displayList(this.listOfUsers);
  },

  _translateProperty: function(key, parent) {
    var name = key;
    var val = parent[key];
    if (key.substr(0, 1) === '@') {
      name = key.substr(1);
    }
    if (name === "modifiedDate" || name === "creationDate") {
      val = this._convertDate(val);
    }
    return {name:name, val:val};
  },
  _convertDate: function(dateString) {
    var d;
    if (dateString.length === 0) {
      d = new Date(); // Just make it now given there is no info
    } else {
      d = new Date(dateString);
      if (d.toString() === dateString) {
        // IE is not as flexible at parsing date strings. Need to do extra work
        var dateInParts = dateString.split(' ');
        var simplerDate = dateInParts[1] + ' ' + dateInParts[2] + ', ' + dateInParts[5];
        d = new Date(simplerDate);
      }
    }
    return d;
  },
  _addGroupProperties: function (userListIdx, groupNode) {
    this.log(bDEBUG, "_addGroupProperties", "call", userListIdx);
    if (typeof groupNode !== 'object') return; // not an obect
    for (var groupNodeProp in groupNode) {
      if (groupNodeProp === "Tag") {
        this.listOfUsers[userListIdx].groups_tag = [];
        var tagNode = groupNode.Tag;
        if (tagNode instanceof Array) {
          var len = tagNode.length;
          for (var i = 0; i < len; i++) {
            this._addTagProperties(userListIdx, tagNode[i]);
          }
        } else {
          this._addTagProperties(userListIdx, tagNode);
        }
      }
    }
  },
  _addTagProperties: function (userListIdx, tagNode) {
    var saveName = "default";
    for (var prop in tagNode) {
      var nvp = this._translateProperty(prop, tagNode);
      if (nvp.name === "Name") {
         saveName = nvp.val;
      } else if (nvp.name === "Value") {
        var o = new Object;
        o[saveName] = nvp.val;
        if (saveName && saveName.length > 0)
          this.listOfUsers[userListIdx].groups_tag.push(o);
      } else {
        this.listOfUsers[userListIdx].groups_tag.push(nvp);
      }
    }
    return saveName;
  },
  _editUserClicked: function(evt) {
    if (this.options.callBack) {
      var trElt = evt.currentTarget.parentNode.parentNode;
      var usr = this._lookupUserFromDOMRef(trElt);
      this.options.callBack('edit', usr);
    }
  },
  _deleteUserClicked: function(evt) {
    if (this.options.callBack) {
      var trElt = evt.currentTarget.parentNode.parentNode;
      var usr = this._lookupUserFromDOMRef(trElt);
      this.options.callBack('delete', usr);
    }
  },
  _lookupUserFromDOMRef: function(domRef) {
    var nid = domRef.getAttribute('id').substr(4);
    var len = this.listOfUsers.length;
    for (var i = 0; i < len; i++) {
      var usr = this.listOfUsers[i];
      if (nid === usr.NID) return usr;
    }
    return null;
  },
  _deleteFailed: function (resp) {
    this.log(bDEBUG, "_deleteFailed", "call", "");
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
    this.log(bERROR, "deleteUser", "fail", errorMsg);
    if (this.options.callBack)
      this.options.callBack('deleteFailed', errorMsg);
  },

    /**
  * _closePanel function
  */
  _closePanel: function () {
    if (this.options.callBack)
      this.options.callBack('closeUsers', null);
  },
  // Localization functions
  _t: function(id) { return this._resource.ref[id]; },
  // _s: function(num) { return this._resource.plural(num); },
  // _is: function(num) { return this._resource.plural_is(num); },
  _date: function(num) { return this._resource.shortDate(date); }

});

// expose Objects as jQuery plugins
jQuery.plugin('edp_userList', UserList);
