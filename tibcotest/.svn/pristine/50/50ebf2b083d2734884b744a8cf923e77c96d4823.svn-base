/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* DefaultSettings class which handles with Settigs for Authtool;
* This class reads settings file from disk and provides methods to access those settings;
*/
jsx3.Class.defineClass("com.ipvs.gui.authtool.DefaultSettings", null, null, function(DefaultSettings, DefaultSettings_prototype) {

  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._LOG = jsx3.util.Logger.getLogger(DefaultSettings.jsxclass.getName());
  
  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._DefaultSettings = null;
  
  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._DefSettingsFilePath = null;

  /**
  * The instance initializer.
  */
  DefaultSettings_prototype.init = function() {
    this._giStartPath = this._getGIStartPath();
    this._appPath = PNameSpace.getEnv("APPPATH");
    if (this._appPath.indexOf('..') > -1) {
      // remove '\..\..\..\' from '\..\..\..\JSXAPPS\IPVS_AuthTool'
      this._appPath = this._appPath.substring(this._appPath.lastIndexOf('..') + 3);
    }
    this._DefSettingsFilePath = this._appPath + "/xml/default/settings.xml";
    var defSettingsDoc = PNameSpace.loadInclude(this._DefSettingsFilePath, "DefSettings_xml", "xml", true);
    this._DefaultSettings = defSettingsDoc?defSettingsDoc.selectSingleNode("//SETTINGS"):null;
  }
  
  /**
   * Gets the Settings
   * @return {jsx3.xml.Entity} Settings
   */
  DefaultSettings_prototype.getSettings = function(){
    return this._DefaultSettings;
  }
  
  /**
   * Sets the Settings
   * @param settingsXMLNode {jsx3.xml.Entity}
   */
  DefaultSettings_prototype.setSettings = function(settingsXMLNode) {
    this._DefaultSettings = settingsXMLNode;
  }
  
  /**
   * Gets the Title (Default Media Guide Title)
   * @return {String}
   */
  DefaultSettings_prototype.getTitle = function() {
    return this._getValue("TITLE");
  }
  
  /**
   * Set the Title (Default Media Guide Title)
   * @param title {String}
   */
  DefaultSettings_prototype.setTitle = function(title) {
    this._setValue("TITLE", title);
  }
  
  /**
   * Gets the Logo path (Default Media Guide Logo)
   * @return {String}
   */
  DefaultSettings_prototype.getLogo = function() {
    return this._getValue("LOGO");
  }

  /**
   * Set the Logo path (Default Media Guide Logo)
   * @param logoPath {String}
   */
  DefaultSettings_prototype.setLogo = function(logoPath) {
    this._setValue("LOGO", logoPath);
  }
  
  /**
   * Gets the Input Directory path (Default Media Input Dir)
   * @return {String}
   */
  DefaultSettings_prototype.getInputDir = function() {
    return this._getValue("INPUTDIR");
  }
  
  /**
   * Set the Input Directory path (Default Media Input Dir)
   * @param inputDirPath {String}
   */
  DefaultSettings_prototype.setInputDir = function(inputDirPath) {
    this._setValue("INPUTDIR", inputDirPath);
  }

  /**
   * Gets the Output Directory path (Default Media Output Dir)
   * @return {String}
   */
  DefaultSettings_prototype.getOutputDir = function() {
    return this._getValue("OUTPUTDIR");
  } 
  
  /**
   * Set the Outout Directory path (Default Media Input Dir)
   * @param outputDirPath {String}
   */
  DefaultSettings_prototype.setOutputDir = function(outputDirPath) {
    this._setValue("OUTPUTDIR", outputDirPath);
  }

  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._getValue = function(xPath) {
    var objNode = this._DefaultSettings.selectSingleNode(".//" + xPath);
    return objNode? objNode.getValue():"";
  }
  
  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._setValue = function(xPath, nVal) {
    this._DefaultSettings.selectSingleNode(".//" + xPath).setValue(nVal ? nVal : "");
  }

  /**
   * Updates settings file on disk 
   */
  DefaultSettings_prototype.updateSettings = function() {
    this._writeSettingsToFile();
  }
  
  /**
   * Returns App path
   * @return {String} 
   */
  DefaultSettings_prototype.getGIAppPath = function() {
    return this._giStartPath + "/" + this._appPath;
  }
  
  /**
   * Returns Projects path
   * @return {String} 
   */
  DefaultSettings_prototype.getProjectsPath = function() {
    var giStartPath = this._getGIStartPath();
    giStartPath = giStartPath.substring(0, giStartPath.lastIndexOf("/"));
    giStartPath = giStartPath.replace(/\//g, "\\");
    return giStartPath + "\\Projects";
  }

  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._writeSettingsToFile = function(){
    var theFile = null;
    try {
      var FSO = new ActiveXObject("Scripting.FileSystemObject");
      theFile = FSO.OpenTextFile(this._giStartPath + "//" + this._DefSettingsFilePath, 2, true);
      theFile.write(this._DefaultSettings.toString());
      theFile.close();
      this._LOG.info("Settings: Saved successfully");
    } catch (e) {
      this._LOG.warn("Settings: Failed to write to" + this._giStartPath + ": " + e.description);
    }
  }
  
  /** @private @jsxobf-clobber */
  DefaultSettings_prototype._getGIStartPath = function() {
    var clientLocation = window.location.href
    clientLocation = clientLocation.substring(0, clientLocation.lastIndexOf("/"));
    var giStartPath = clientLocation.substring(0);
    if( clientLocation.indexOf("file:")!=-1) {
      giStartPath = clientLocation.substring(8);
      if (giStartPath.match("/shell.html")) {
        //tibco gitak using path like:igStartPath=C:/gitakRecorder/authtool/GI//shell.html?jsxapppath=file:///
        //C:/svn-work/gitakRecorderTest/authtool/GI//shell.html?jsxapppath=file:///C:/svn-work/gitakRecorderTest/authtool/GI
        giStartPath = giStartPath.substring(0, giStartPath.indexOf("//shell.html"));
      }
      else if (giStartPath.match("/GI_Builder")) {
        //launch in recorder using path like:file:///C:/C:/gitakRecorder/authtool/GI_Builder/pugins/jsx3.ide.recorder/recorder.html
        giStartPath = giStartPath.substring(0, giStartPath.indexOf("/GI_Builder")); ;
      }
    }
    giStartPath = giStartPath.replace(/%20/g, " ");
    //giStartPath = "E://work//Feature_24691"; //TBR
    return giStartPath;
  }

});