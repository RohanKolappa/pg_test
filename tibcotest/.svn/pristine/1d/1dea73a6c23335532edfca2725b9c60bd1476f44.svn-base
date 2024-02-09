/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Settings class provides UI to manage auth tool default settings.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.Settings", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(Settings, Settings_prototype) {
  
  /** @private @jsxobf-clobber */
  Settings_prototype._LOG = jsx3.util.Logger.getLogger(Settings.jsxclass.getName());

  var MainMenu = com.ipvs.gui.authtool.MainMenu;
   
  /**
   * The instance initializer.
   */
  Settings_prototype.init = function() {
    //Get handle of child components and subscribe as required
    this._ViewState = false;
    this.btnClose = this.getDescendantOfName("btnClose");
    this.btnCancel = this.getDescendantOfName("btnCancelDefaultSettings");
    this.txtTitle = this.getDescendantOfName("txtDefaultMediaGuideTitle");
    this.clsFilePathLogo = this.getDescendantOfName("blkLogo").getDescendantOfName("filePicker");
    this.clsInputDir = this.getDescendantOfName("blkInputDir").getDescendantOfName("blkDirPicker");
    this.clsOutputDir = this.getDescendantOfName("blkOutputDir").getDescendantOfName("blkDirPicker");
    this.btnSave = this.getDescendantOfName("btnSaveDefaultSettings");
    this.blkSaveMsg = this.getDescendantOfName("blkSaveMessage");
    this.btnSave.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._saveSettings);
    this.btnClose.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Main);
    this.btnCancel.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Main);
    this.clsFilePathLogo.subscribe(com.ipvs.gui.FilePicker.ON_FILE_SELECT, this, this._checkFileType);
    //get the handle of MainApp class
    this.blkApp = this.getAncestorOfType("com.ipvs.authtool.App");
    this.blkApp.subscribe(com.ipvs.authtool.App.MENU_SELECT, this, this._toggle);
    this._LOG.info("Settings: Initiated");
  }

  /** @private @jsxobf-clobber */
  Settings_prototype._toggle = function(objEvent) {
    if( this.blkApp.getMenuSelection() == MainMenu.MENU_SETTINGS ) {
      this.cmpShow();
      this._LOG.info("Settings: Show");
      this._initLoad();
    }
    else {
      this.cmpHide();
      this._LOG.info("Settings: Hide");
    }
  }

  /** @private @jsxobf-clobber */
  Settings_prototype._initLoad = function() {
    //Load the values from default settings to display UI
    this.txtTitle.setValue(clsDefaultSettings.getTitle());
    this.clsFilePathLogo.setFilePath(clsDefaultSettings.getLogo());
    this.clsInputDir.setFolderPath(clsDefaultSettings.getInputDir());
    this.clsOutputDir.setFolderPath(clsDefaultSettings.getOutputDir());
    if( !this._ViewState ){
      this._clsFSOFileSystem = new jsx3.io.FSOFileSystem();
    }
    this.blkSaveMsg.setText("", true);
    this.blkSaveMsg.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this._ViewState = true;
    this._LOG.info("Settings: Loaded");
  }
  
  /** @private @jsxobf-clobber */  
  Settings_prototype._saveSettings = function() {
    //Save updated settings.It updates default settings and writes back to disk
    this.blkSaveMsg.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.blkSaveMsg.setText("Saving...", true);
    clsDefaultSettings.setTitle(this.txtTitle.getValue());
    clsDefaultSettings.setLogo(this.clsFilePathLogo.getFilePath());
    clsDefaultSettings.setInputDir(this.clsInputDir.getFolderPath());
    clsDefaultSettings.setOutputDir(this.clsOutputDir.getFolderPath());
    clsDefaultSettings.updateSettings();
    this.blkSaveMsg.setText("Settings Saved", true);
  }
  
  /**
   * Destroy all child components 
   */
  Settings_prototype.destroy = function() {
    this.btnClose = null;
    this.btnCancel = null;
  }
  
  /** @private @jsxobf-clobber */
  Settings_prototype._onClick_Main = function(objEvent) {
    //publish MainApp event to go back to main menu
    this._LOG.info("Settings: Menu selected: " + MainMenu.MENU_MAINMENU);
    this.blkSaveMsg.setText("", true);
    this.blkSaveMsg.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_MAINMENU);
  }

  /** @private @jsxobf-clobber */
  Settings_prototype._checkFileType = function() {
    var filePath = this.clsFilePathLogo.getFilePath();
    var file = this._clsFSOFileSystem.getFile(filePath);
    this._LOG.info("Settings_checkFileType:Extension=" + file.getExtension().toLowerCase());
    if ( file.isFile() && (file.getExtension() != null && file.getExtension().toLowerCase() != "png" && file.getExtension().toLowerCase() != "jpg" && file.getExtension().toLowerCase() != "jpeg" && file.getExtension().toLowerCase() != "gif")) {
      alert("Select valid image type(JPEG/PNG/GIF) for Logo");
      this.clsFilePathLogo.setFilePath("");
      return false;
    }
    return true;
  }
});