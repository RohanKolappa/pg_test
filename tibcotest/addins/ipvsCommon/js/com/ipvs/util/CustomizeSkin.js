/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* An object-oriented class to handle skin customization functions 
*/
jsx3.Class.defineClass('com.ipvs.util.CustomizeSkin', jsx3.lang.Object, null, function(CustomizeSkin, CustomizeSkin_prototype) {

  /** @private @jsxobf-clobber */
  CustomizeSkin_prototype._LOG = jsx3.util.Logger.getLogger(CustomizeSkin.jsxclass.getName());

  /**
  * Instance varibales
  */
  CustomizeSkin_prototype._customizeSkinSettings = null;
  CustomizeSkin_prototype._customizeSkinBaseDirPath = null;

  /**
  * The instance initializer.
  */
  CustomizeSkin_prototype.init = function() {
    this._LOG.info("CustomizeSkin::init");
    this._appPath = PNameSpace.getEnv("APPPATH");
    this._customizeSkinFilePath = this._appPath + "/../../../skin/skin.xml";
    this._customizeSkinBaseDirPath = this._appPath + "/../../../";
    // For Debugging. make sure skin dir is under the IPVS_Admin/IPVS_ClientUI projects
    //this._customizeSkinBaseDirPath = this._appPath + "/"; //TBC
    //this._customizeSkinFilePath = this._customizeSkinBaseDirPath + "skin/skin.xml"; //TBC
    var skinDoc = PNameSpace.loadInclude(this._customizeSkinFilePath, "skin_xml", "xml", true);
    this._customizeSkinSettings = skinDoc ? skinDoc.selectSingleNode("//skinItemList") : null;
    this._LOG.info("CustomizeSkin::init::_customizeSkinSettings:" + this._customizeSkinSettings);
  }

  /**
  * The destroy method.
  */
  CustomizeSkin_prototype.destroy = function() {
    this._LOG.debug("CustomizeSkin.destroy");
    this._customizeSkinSettings = null;

  };

  /**
  * The destroy method.
  */
  CustomizeSkin_prototype.getAttrValue = function(attrApp, attrName) {
    var attrValue = null;
    if (jsx3.util.strEmpty(this._customizeSkinSettings)) { return attrValue; }
    var objNode = this._customizeSkinSettings.selectSingleNode(".//skinItem[@app='" + attrApp + "' and @name='" + attrName + "']");
    if (!jsx3.util.strEmpty(objNode)) {
      var attrType = objNode.getAttribute("type");
      attrValue = objNode.getAttribute("value");
      if (attrType == "image") {
        attrValue = this._customizeSkinBaseDirPath + objNode.getAttribute("value");
      }
    }
    return attrValue;
  };


});