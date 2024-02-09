/* ============================================================================
 * UserPermissions Data Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var UserPermissions = Class.extend({
  _defaults: {
  },

  _defaultPermissions: {
    "User": { "@add": true, "@modify": true, "@delete": true, "@read": true },
    "Dir": { "@record": true, "@import": true, "@pvr": true, "@modify": true, "@delete": true, "@read": true },
    "Port": { "@add": true, "@modify": true, "@delete": true, "@streamto": true, "@viewfrom": true, "@read": true },
    "Device": { "@add": true, "@modify": true, "@delete": true, "@read": true },
    "Policy": { "@add": true, "@modify": true, "@delete": true, "@read": true },
    "File": { "@playback": true, "@export": true, "@modify": true, "@delete": true, "@read": true },
    "Report": { "@add": true, "@modify": true, "@delete": true, "@read": true },
    "Profile": { "@add": true, "@modify": true, "@delete": true, "@read": true },
    "SessionConfig": { "@add": true, "@modify": true, "@delete": true, "@start": true, "@read": true },
    "System": { "@backup": true, "@upgrade": true, "@sessionmanagement": true, "@modify": true, "@restore": true, "@read": true }
  },

  _defaultUserPrivateData: "",

  /**
  * init function 
  */
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.resetPermissions();
    this.resetPrivateData();
    this.log(bDEBUG, "init", "success", "");
  },

  /**
  * destroy function 
  */
  destroy: function () {
    this.resetPermissions();
    this.resetPrivateData();
    this.$elem.removeData('edp_userPermissions');
  },

  /**
  * setPermissions function 
  */
  setPermissions: function (objPermissions) {
    if (jQuery.isPlainObject(objPermissions) && !jQuery.isEmptyObject(objPermissions)) {
      this._userPermissions = objPermissions;
    }
    else {
      this._userPermissions = this._defaultPermissions;
    }
  },

  /**
  * setPrivateData function 
  */
  setPrivateData: function (privateData) {
    this._userPrivateData = privateData;
  },

  /**
  * resetPermissions function 
  */
  resetPermissions: function () {
    this._userPermissions = this._defaultPermissions;
  },

  /**
  * resetPrivateData function 
  */
  resetPrivateData: function () {
    this._userPrivateData = this._defaultUserPrivateData;
  },

  getPermission: function (objName, objRole) {
    var retPermission = JSON.search(this._userPermissions, "//" + objName + "/@" + objRole)[0];
    if (retPermission == true || retPermission == "true") {
      return true;
    }
    else {
      return false;
    }
  },

  hasAdminPermission: function (objName) {
    var hasPermission = false;
    if (objName == "Device") {
      if (this._userPermissions.Device["@add"] == true ||
           this._userPermissions.Device["@modify"] == true ||
           this._userPermissions.Device["@delete"] == true
          ) {
        hasPermission = true;
      }
    }
    else if (objName == "Profile") {
      if (this._userPermissions.Profile["@add"] == true ||
           this._userPermissions.Profile["@modify"] == true ||
           this._userPermissions.Profile["@delete"] == true
          ) {
        hasPermission = true;
      }
    }
    else if (objName == "Policy") {
      if (this._userPermissions.Policy["@add"] == true ||
           this._userPermissions.Policy["@modify"] == true ||
           this._userPermissions.Policy["@delete"] == true
          ) {
        hasPermission = true;
      }
    }
    else if (objName == "Session") {
      if (this._userPermissions.System["@sessionmanagement"] == true) {
        hasPermission = true;
      }
    }
    else if (objName == "User") {
      if (this._userPermissions.User["@add"] == true ||
           this._userPermissions.User["@modify"] == true ||
           this._userPermissions.User["@delete"] == true
          ) {
        hasPermission = true;
      }
    }
    else if (objName == "Media") {
      if ((this._userPermissions.Dir["@import"] == true ||
            this._userPermissions.Dir["@modify"] == true ||
            this._userPermissions.Dir["@delete"] == true) ||
           (this._userPermissions.File["@delete"] == true ||
            this._userPermissions.File["@modify"] == true ||
            this._userPermissions.File["@export"] == true)
         ) {
        hasPermission = true;
      }
    }
    else if (objName == "System") {
      if (this._userPermissions.System["@read"] == true ||
          this._userPermissions.System["@restore"] == true ||
          this._userPermissions.System["@upgrade"] == true ||
          this._userPermissions.System["@backup"] == true ||
          this._userPermissions.System["@read"] == true
         ) {
        hasPermission = true;
      }
    }
    return hasPermission;
  },

  getPrivateData: function () {
    return this._userPrivateData;
  }

});

// expose Objects as jQuery plugins
jQuery.plugin('edp_userPermissions', UserPermissions);


