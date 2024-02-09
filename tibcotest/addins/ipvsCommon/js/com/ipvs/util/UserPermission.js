/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* An object-oriented class to handle User Permission functions 
*/
jsx3.Class.defineClass('com.ipvs.util.UserPermission', jsx3.lang.Object, null, function(UserPermission, UserPermission_prototype) {

  /** @private @jsxobf-clobber */
  UserPermission_prototype._LOG = jsx3.util.Logger.getLogger(UserPermission.jsxclass.getName());

  /**
  * Instance varibales
  */
  UserPermission_prototype.arrUserPermission = null;

  /**
  * The instance initializer.
  */
  UserPermission_prototype.init = function() {
    this._LOG.info("UserPermission::init");
    this.arrUserPermission = new Object;
  }

  /**
  * The destroy method.
  */
  UserPermission_prototype.destroy = function() {
    this._LOG.debug("UserPermission.destroy");
    this.arrPresence = null;
  };

  /**
  * save the Permissions in the array.
  */
  UserPermission_prototype.setPermission = function(objPermissionNode) {
    var userPermissionNode = objPermissionNode.selectSingleNode(".//User");
    var portPermissionNode = objPermissionNode.selectSingleNode(".//Port");
    var devicePermissioNode = objPermissionNode.selectSingleNode(".//Device");
    var dirPermissioNode = objPermissionNode.selectSingleNode(".//Dir");
    var filePermissioNode = objPermissionNode.selectSingleNode(".//File");
    var profilePermissioNode = objPermissionNode.selectSingleNode(".//Profile");
    var configuresessionPermissioNode = objPermissionNode.selectSingleNode(".//SessionConfig");
    var policyPermissioNode = objPermissionNode.selectSingleNode(".//Policy");
    var systemPermissioNode = objPermissionNode.selectSingleNode(".//System");
    var reportPermissionNode = objPermissionNode.selectSingleNode(".//Report");

    //set the user permission
    this.arrUserPermission["User"] = new Object;
    this.arrUserPermission["User"]["add"] = userPermissionNode.getAttribute("add");
    this.arrUserPermission["User"]["delete"] = userPermissionNode.getAttribute("delete");
    this.arrUserPermission["User"]["modify"] = userPermissionNode.getAttribute("modify");
    this.arrUserPermission["User"]["read"] = userPermissionNode.getAttribute("read");
    //set the port permission
    this.arrUserPermission["Port"] = new Object;
    this.arrUserPermission["Port"]["add"] = portPermissionNode.getAttribute("add");
    this.arrUserPermission["Port"]["delete"] = portPermissionNode.getAttribute("delete");
    this.arrUserPermission["Port"]["modify"] = portPermissionNode.getAttribute("modify");
    this.arrUserPermission["Port"]["read"] = portPermissionNode.getAttribute("read");
    this.arrUserPermission["Port"]["viewfrom"] = portPermissionNode.getAttribute("viewfrom");
    this.arrUserPermission["Port"]["streamto"] = portPermissionNode.getAttribute("streamto");
    //set the Device permission
    this.arrUserPermission["Device"] = new Object;
    this.arrUserPermission["Device"]["add"] = devicePermissioNode.getAttribute("add");
    this.arrUserPermission["Device"]["delete"] = devicePermissioNode.getAttribute("delete");
    this.arrUserPermission["Device"]["modify"] = devicePermissioNode.getAttribute("modify");
    this.arrUserPermission["Device"]["read"] = devicePermissioNode.getAttribute("read");
    //set the Dir permission
    this.arrUserPermission["Dir"] = new Object;
    this.arrUserPermission["Dir"]["delete"] = dirPermissioNode.getAttribute("delete");
    this.arrUserPermission["Dir"]["modify"] = dirPermissioNode.getAttribute("modify");
    this.arrUserPermission["Dir"]["read"] = dirPermissioNode.getAttribute("read");
    this.arrUserPermission["Dir"]["import"] = dirPermissioNode.getAttribute("import");
    this.arrUserPermission["Dir"]["record"] = dirPermissioNode.getAttribute("record");
    //set the File permission
    this.arrUserPermission["File"] = new Object;
    this.arrUserPermission["File"]["delete"] = filePermissioNode.getAttribute("delete");
    this.arrUserPermission["File"]["modify"] = filePermissioNode.getAttribute("modify");
    this.arrUserPermission["File"]["read"] = filePermissioNode.getAttribute("read");
    this.arrUserPermission["File"]["export"] = filePermissioNode.getAttribute("export");
    this.arrUserPermission["File"]["playback"] = filePermissioNode.getAttribute("playback");
    //set the Profile permission
    this.arrUserPermission["Profile"] = new Object;
    this.arrUserPermission["Profile"]["add"] = profilePermissioNode.getAttribute("add");
    this.arrUserPermission["Profile"]["delete"] = profilePermissioNode.getAttribute("delete");
    this.arrUserPermission["Profile"]["modify"] = profilePermissioNode.getAttribute("modify");
    this.arrUserPermission["Profile"]["read"] = profilePermissioNode.getAttribute("read");
    //set the ConfigureSession permission
    this.arrUserPermission["SessionConfig"] = new Object;
    this.arrUserPermission["SessionConfig"]["add"] = configuresessionPermissioNode.getAttribute("add");
    this.arrUserPermission["SessionConfig"]["delete"] = configuresessionPermissioNode.getAttribute("delete");
    this.arrUserPermission["SessionConfig"]["modify"] = configuresessionPermissioNode.getAttribute("modify");
    this.arrUserPermission["SessionConfig"]["read"] = configuresessionPermissioNode.getAttribute("read");
    this.arrUserPermission["SessionConfig"]["start"] = configuresessionPermissioNode.getAttribute("start");
    //set the Policy permission
    this.arrUserPermission["Policy"] = new Object;
    this.arrUserPermission["Policy"]["add"] = policyPermissioNode.getAttribute("add");
    this.arrUserPermission["Policy"]["delete"] = policyPermissioNode.getAttribute("delete");
    this.arrUserPermission["Policy"]["modify"] = policyPermissioNode.getAttribute("modify");
    this.arrUserPermission["Policy"]["read"] = policyPermissioNode.getAttribute("read");
    //set the System permission
    this.arrUserPermission["System"] = new Object;
    this.arrUserPermission["System"]["upgrade"] = systemPermissioNode.getAttribute("upgrade");
    this.arrUserPermission["System"]["restore"] = systemPermissioNode.getAttribute("restore");
    this.arrUserPermission["System"]["backup"] = systemPermissioNode.getAttribute("backup");
    this.arrUserPermission["System"]["sessionmanagement"] = systemPermissioNode.getAttribute("sessionmanagement");
    this.arrUserPermission["System"]["read"] = systemPermissioNode.getAttribute("read");
    this.arrUserPermission["System"]["modify"] = systemPermissioNode.getAttribute("modify");
//    this.arrUserPermission["System"]["report"] = systemPermissioNode.getAttribute("report");
      //set the Report permission
    this.arrUserPermission["Report"] = new Object;
    this.arrUserPermission["Report"]["add"] = reportPermissionNode.getAttribute("add");
    this.arrUserPermission["Report"]["delete"] = reportPermissionNode.getAttribute("delete");
    this.arrUserPermission["Report"]["modify"] = reportPermissionNode.getAttribute("modify");
    this.arrUserPermission["Report"]["read"] = reportPermissionNode.getAttribute("read");
    //device tab in the admin ui is actually a assetresource list
    //to show/hide tab and opreation add/delete/modify depens upon Ports and Device permissions
    // if of them are true then only operatoin can be performed
    this.arrUserPermission["Asset"] = new Object;
    this.arrUserPermission["Asset"]["add"] = "false";
    this.arrUserPermission["Asset"]["delete"] = "false";
    this.arrUserPermission["Asset"]["modify"] = "false";
    this.arrUserPermission["Asset"]["read"] = "false";

    if (this.arrUserPermission["Port"]["add"] == "true" && this.arrUserPermission["Device"]["add"] == "true") {
      this.arrUserPermission["Asset"]["add"] = "true";
    }
    if (this.arrUserPermission["Port"]["delete"] == "true" && this.arrUserPermission["Device"]["delete"] == "true") {
      this.arrUserPermission["Asset"]["delete"] = "true";
    }
    if (this.arrUserPermission["Port"]["modify"] == "true" && this.arrUserPermission["Device"]["modify"] == "true") {
      this.arrUserPermission["Asset"]["modify"] = "true";
    }
    if (this.arrUserPermission["Port"]["read"] == "true" && this.arrUserPermission["Device"]["read"] == "true") {
      this.arrUserPermission["Asset"]["read"] = "true";
    }
  }

  /**
  * get the Permissions from the array.
  */
  UserPermission_prototype.getPermission = function(objName, objRole) {
  var objPermission = true;
    if (jsx3.util.strEmpty(this.arrUserPermission[objName])) {
    }
    else {
      objPermission = this.arrUserPermission[objName][objRole];
      if (jsx3.util.strEmpty(objPermission) || objPermission == "undefined") {
        objPermission = "false";
      }
    }
    return objPermission;
  }
  
   /**
  * Return if the User has Admin permissions to View the Object tab.
  * Admin Mean it has Management Policies on  the system Add/Modify/Delete (similar) permissions
  */
  UserPermission_prototype.hasAdminPermission = function(objName) {
    if(objName == "Asset"){
      if ( this.arrUserPermission["Asset"]["add"] == "true" ||
           this.arrUserPermission["Asset"]["modify"] == "true" ||
           this.arrUserPermission["Asset"]["delete"] == "true"  
          ) {
          return "true";
      }
      else{
          return "false";
      }
    }
    else if(objName == "Profile"){
      if ( this.arrUserPermission["Profile"]["add"] == "true" ||
           this.arrUserPermission["Profile"]["modify"] == "true" ||
           this.arrUserPermission["Profile"]["delete"] == "true"
          ) {
          return "true";
      }
      else{
          return "false";
      }
    }
    else if(objName == "Policy"){
      if ( this.arrUserPermission["Policy"]["add"] == "true" ||
           this.arrUserPermission["Policy"]["modify"] == "true" ||
           this.arrUserPermission["Policy"]["delete"] == "true"
          ) {
          return "true";
      }
      else{
          return "false";
      }    
    }
    else if(objName == "Session"){
      if ( this.arrUserPermission["System"]["sessionmanagement"] == "true"){
        return "true";
      }
      else{
        return "false";
      }
    }   
    else if(objName == "User"){
      if ( this.arrUserPermission["User"]["add"] == "true" ||
           this.arrUserPermission["User"]["modify"] == "true" ||
           this.arrUserPermission["User"]["delete"] == "true" 
          ) {
          return "true";
      }
      else{
          return "false";
      }          
    }
    else if(objName == "Media"){
       if ( (this.arrUserPermission["Dir"]["import"] == "true" ||
             this.arrUserPermission["Dir"]["modify"] == "true" ||
             this.arrUserPermission["Dir"]["delete"] == "true") ||  
            (this.arrUserPermission["File"]["delete"] == "true" ||
             this.arrUserPermission["File"]["modify"] == "true" ||
             this.arrUserPermission["File"]["export"] == "true") 
          ) {
          return "true";
      }
      else{
          return "false";
      }          
    }
    else if(objName == "System"){
       if (this.arrUserPermission["System"]["read"] == "true" ||
           this.arrUserPermission["System"]["restore"] == "true" ||
           this.arrUserPermission["System"]["upgrade"] == "true" ||
           this.arrUserPermission["System"]["backup"] == "true"  ||
           this.arrUserPermission["Report"]["read"] == "true"
          ) {
          return "true";
      }
      else{
          return "false";
      }            
    }   
    return "false";
  }

});