/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* NewProject class provides UI to Create/Edit a project.
* This will have 3 steps Setup,AddFiles and Process
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.NewProject", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(NewProject, NewProject_prototype) {

  /** @private @jsxobf-clobber */
  NewProject_prototype._LOG = jsx3.util.Logger.getLogger(NewProject.jsxclass.getName());

  var MainMenu = com.ipvs.gui.authtool.MainMenu;

  /**
  * {String} Step published during project creation.
  * @final @jsxobf-final
  */
  NewProject.MENU_SELECT = "PROJECTMENUSELECTION";
  NewProject.MENU_SETUP = "SETUP";
  NewProject.MENU_ADDFILES = "ADDFILES";
  NewProject.MENU_PROCESSPROJECT = "PROCESSPROJECT";

  /**
  * The instance initializer.
  */
  NewProject_prototype.init = function() {
    //Get handle of child components and subscribe as required
    this.projectName = null;
    this.MenuSelected = null;
    this._projectSaved = false;
    this._projectInProcess = false;
    this.blkSetUP = null;
    this.blkAddFiles = null;
    this.btnClose = this.getDescendantOfName("btnClose");
    this.lblStep1 = this.getDescendantOfName("lblStep1");
    this.lblStep2 = this.getDescendantOfName("lblStep2");
    this.lblStep3 = this.getDescendantOfName("lblStep3");
    this.btnCancel = this.getDescendantOfName("btnCancel");
    this.btnSave = this.getDescendantOfName("btnSaveExit");
    this.btnPrevious = this.getDescendantOfName("btnPrevious");
    this.btnNext = this.getDescendantOfName("btnNext");
    this.btnClose.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Main);
    this.btnCancel.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Cancel);
    this.btnPrevious.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Previous);
    this.btnNext.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Next);
    this.btnSave.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Save);
    //get the handle of MainApp class
    this.blkApp = this.getAncestorOfType("com.ipvs.authtool.App");
    this.blkApp.subscribe(com.ipvs.authtool.App.MENU_SELECT, this, this._toggle);
    this.subscribe(NewProject.MENU_SELECT, this, this._onClick_PrevNext)
    this._LOG.info("Project: Initiated");
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._toggle = function(objEvent) {
    var menuSelected = this.blkApp.getMenuSelection();
    if(menuSelected == MainMenu.MENU_NEWPROJECT || menuSelected == MainMenu.MENU_EDITPROJECT || menuSelected == MainMenu.MENU_PROCESSPROJECT) {
      if(menuSelected == MainMenu.MENU_NEWPROJECT) {
        this.projectName = null;
        this._projectSaved = false;
      }
      else {
        this.projectName = this.blkApp.getProjectSelected();
        this._projectSaved = true;
      }

      var stepToGo = NewProject.MENU_SETUP;
      if(menuSelected == MainMenu.MENU_PROCESSPROJECT) {
        stepToGo = NewProject.MENU_PROCESSPROJECT;
      }
      this.cmpShow();
      this._LOG.info("Project: Show");
      this._initLoad();
      this._setMenuSelection(objEvent, stepToGo);
    }
    else {
      this._setMenuSelection(objEvent, null);
      this.cmpHide();
      this._LOG.info("Project: Hide");
    }
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._initLoad = function() {
    //Check if it is EditPRoject then get selected project file
    //Or get default Project.xml
    this._clsFSOFileSystem = new jsx3.io.FSOFileSystem();
    this._projectFolderPath = clsDefaultSettings.getProjectsPath();
    var objProjectFld = this._clsFSOFileSystem.getFile(this._projectFolderPath); 
    if(!objProjectFld.exists() || !objProjectFld.isDirectory()) {
      objProjectFld.mkdir();
    }
    if(this.projectName == null) {
      var strProjectNode = this._clsFSOFileSystem.getFile(clsDefaultSettings.getGIAppPath().replace(/\//g, "\\") + "\\xml\\default\\Project.xml").read();
      this._LOG.info("Project: New Project");
    }
    else {
      var strProjectNode = this._clsFSOFileSystem.getFile(this._projectFolderPath + "\\" + this.projectName + ".xml").read();
      this._LOG.info("Project: Edit Project");
    }
    this._projectXML = (new jsx3.xml.Document()).loadXML(strProjectNode);
    if(!jsx3.util.strEmpty(this.projectName)) {
      this._checkAndUpgrade();
    }
    //Get handle of child classes(SetUp/AddFiles)
    this.blkSetUP = this.getDescendantsOfType("com.ipvs.gui.authtool.ProjectSetup")[0];
    this.blkAddFiles = this.getDescendantsOfType("com.ipvs.gui.authtool.AddFiles")[0];
  }

  /** @private @jsxobf-clobber */
  //Checks if project is created before transcoding changes
  //If it is old project then upgrades project xml.
  NewProject_prototype._checkAndUpgrade = function() {
    if(jsx3.util.strEmpty(this._projectXML.selectSingleNode(".//IPVSMEDIA"))) {
      var xslfile = this.getServer().loadInclude("jsxuser:///JSXAPPS/IPVS_AuthTool/xsl/UpgradeProject.xsl", "UpgradeProject", "xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var strProjectXML = xslDoc.transform(this._projectXML);
      this._projectXML = new jsx3.xml.Document().loadXML(strProjectXML);
      this.saveProject();
    }
  }

  /**
  * Gets the Project xml
  * @return {jsx3.xml.Entity} Settings
  */
  NewProject_prototype.getProjectXML = function() {
    return this._projectXML;
  }

  /**
  * Gets the Project Name
  * @return {String} ProjectName
  */
  NewProject_prototype.getProjectName = function() {
    return this.projectName;
  }

  /**
  * Gets OutputType of the project.
  * If CreatePRoject then return default OutputType(IPVSMEDIA)
  * If EditPRoject then return project OutputType.
  * @return {String} OutputType
  */
  NewProject_prototype.getOutputType = function() {
    if(this.projectName == null) {
      return "IPVSMEDIA";
    }
    else {
      return this._projectXML.selectSingleNode(".//OUTPUTTYPE").getValue();
    }
  }

  /**
  * Gets OutputDir(OUTPUTPATH) of the project.
  * If CreatePRoject then return default OutputDir path
  * If EditPRoject then return project OutputDir path.
  * @return {String} OutputDir path
  */
  NewProject_prototype.getOutpurDir = function() {
    if(this.projectName == null) {
      return clsDefaultSettings.getOutputDir();
    }
    else {
      return this._projectXML.selectSingleNode(".//OUTPUTPATH").getValue();
    }
  }

  /**
  * Gets MediaGuideTitle of the project.
  * If CreatePRoject then return default MediaGuideTitle
  * If EditPRoject then return project MediaGuideTitle.
  * @return {String} MediaGuideTitle
  */
  NewProject_prototype.getMediaGuideTitle = function() {
    if(this.projectName == null) {
      return clsDefaultSettings.getTitle();
    }
    else {
      return this._projectXML.selectSingleNode("//LOGOTITLE").getValue();
    }
  }

  /**
  * Gets MediaGuideLogPath of the project.
  * If CreatePRoject then return default MediaGuideLogPath
  * If EditPRoject then return project MediaGuideLogPath.
  * @return {String} MediaGuideLogPath
  */
  NewProject_prototype.getMediaGuideLogo = function() {
    if(this.projectName == null) {
      return clsDefaultSettings.getLogo();
    }
    else {
      return this._projectXML.selectSingleNode("//LOGOPATH").getValue();
    }
  }

  /**
  * Gets TranscodingMediaSettings of the project.
  * If CreatePRoject then return null
  * If EditPRoject then return project TranscodingMediaSettings.
  * @return null/{Object} TranscodingMediaSettings
  */
  NewProject_prototype.getProjectTranscodingMediaSettings = function() {
    var retVal = null;
    if(this.projectName != null) {
      var nodeTranscodeMediaSettings = this._projectXML.selectSingleNode("//TRANSCODEDMEDIA");
      if(!jsx3.util.strEmpty(nodeTranscodeMediaSettings)) {
        var objTranscodeMediaSettings = new Object;
        objTranscodeMediaSettings.OUTPUTPRESET = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION").getAttribute("name");
        objTranscodeMediaSettings.VIDEOTYPE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/VIDEO").getAttribute("type");
        objTranscodeMediaSettings.FORMATTYPE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/FORMAT").getAttribute("type");
        objTranscodeMediaSettings.VIDEOMODE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/VIDEO").getAttribute("mode");
        objTranscodeMediaSettings.VIDEOBITRATE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/VIDEO").getAttribute("bitrate");
        objTranscodeMediaSettings.VIDEOQUANT = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/VIDEO").getAttribute("quant");
        objTranscodeMediaSettings.VIDEOFRAMERATE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/VIDEO").getAttribute("frameRate");
        objTranscodeMediaSettings.VIDEORESOLUTION = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/VIDEO").getAttribute("resolution");
        objTranscodeMediaSettings.AUDIOTYPE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/AUDIO").getAttribute("type");
        objTranscodeMediaSettings.AUDIOBITRATE = nodeTranscodeMediaSettings.selectSingleNode(".//CONFIGURATION/AUDIO").getAttribute("bitrate");
        retVal = objTranscodeMediaSettings;
      }
    }
    return retVal;
  }

  /**
  * Gets MediaList of the project.
  * @return {jsx3.xml.Entity} MEDIALIST
  */
  NewProject_prototype.getProjectMediaListNode = function() {
    return this._projectXML.selectSingleNode(".//MEDIALIST");
  }

  /**
  * Gets total size of the project
  * @return {String} Size
  */
  NewProject_prototype.getProjectSize = function() {
    return this._projectXML.selectSingleNode(".//SIZE").getValue();
  }

  /**
  * Gets total play length of the project
  * @return {String} Length
  */
  NewProject_prototype.getProjectLength = function() {
    return this._projectXML.selectSingleNode(".//LENGTH").getValue();
  }

  /**
  * Gets MenuSelected(Step) in Create/Edit PRoject process
  * @return {String} MenuSelected
  */
  NewProject_prototype.getMenuSelected = function() {
    return this.MenuSelected;
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._setMenuSelection = function(objEvent, menuSelected, bPublish) {
    this.MenuSelected = menuSelected;
    if (bPublish != false) this.publish({ subject: NewProject.MENU_SELECT, context: { objEVENT: objEvent} });
  }

  /**
  * Destroy all child components 
  */
  NewProject_prototype.destroy = function() {
    this.btnMain = null;
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._onClick_Save = function(objEvent) {
    //Save project xml to disk
    var bProjectSaved = this.saveProject();
    this.blkApp.setAddEditProject(objEvent, this.projectName);
    var menuOnSave = MainMenu.MENU_MAINMENU;
    if(this.blkApp.getMenuSelection() == MainMenu.MENU_EDITPROJECT || this.blkApp.getMenuSelection() == MainMenu.MENU_PROCESSPROJECT) {
      menuOnSave = MainMenu.MENU_VIEWPROJECTS;
    }
    this.blkApp.setMenuSelection(objEvent, menuOnSave);
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype.saveProject = function() {
    var projectFile = this._clsFSOFileSystem.getFile(this._projectFolderPath + "\\" + this.projectName + ".xml");
    //Save file to disk
    try {
      projectFile.write(this._projectXML.toString());
      this._LOG.info("Project: Save project success. " + this.projectName);
    }
    catch(e) {
      this._LOG.warn("Project: Error while saving project. ERROR: " + e.description);
      this._projectSaved = false;
      return false;
    }
    this._projectSaved = true;
    return true;
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._createProject = function() {
    this.projectName = this.blkSetUP.txtProjectName.getValue();
    var projectFile = this._clsFSOFileSystem.getFile(this._projectFolderPath + "\\" + this.projectName + ".xml");
    //Check if new project and other project exists with same name
    if(this.blkApp.getMenuSelection() != MainMenu.MENU_EDITPROJECT && this.blkApp.getMenuSelection() != MainMenu.MENU_PROCESSPROJECT) {
      if(projectFile.isFile() && projectFile.exists()) {
        this._LOG.info("Project already exists with name " + this.projectName);
        alert("Project already exists with name " + this.projectName);
        this._setMenuSelection(this, NewProject.MENU_SETUP);
        return false;
      }
    }
    //Update project xml with gui values      
    this._projectXML.selectSingleNode(".//NAME").setValue(this.projectName);
    this._projectXML.selectSingleNode(".//CREATED").setValue(new Date().getTime());
    this._projectXML.selectSingleNode(".//LENGTH").setValue(this.blkAddFiles.getProjectLength());
    this._projectXML.selectSingleNode(".//SIZE").setValue(this.blkAddFiles.getProjectSize());
    this._projectXML.selectSingleNode(".//OUTPUTPATH").setValue(this.blkSetUP.clsOutputDir.getFolderPath());

    var projectOutputType = this.blkSetUP.selOutputFormat.getValue();
    this._projectXML.selectSingleNode(".//OUTPUTTYPE").setValue(projectOutputType);
    if(projectOutputType == "IPVSMEDIA") {
      var objIPVSMediaSettings = this.blkSetUP.getIPVSMediaSettings();
      this._projectXML.selectSingleNode(".//IPVSMEDIA/LOGOPATH").setValue(objIPVSMediaSettings.LOGOPATH);
      this._projectXML.selectSingleNode(".//IPVSMEDIA/LOGOTITLE").setValue(objIPVSMediaSettings.LOGOTITLE);
    }
    else if(projectOutputType == "TRANSCODEDMEDIA") {
      var objTranscodeMediaSettings = this.blkSetUP.getTranscodingMediaSettings();
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION").setAttribute("name", objTranscodeMediaSettings.OUTPUTPRESET);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/VIDEO").setAttribute("type", objTranscodeMediaSettings.VIDEOTYPE);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/VIDEO").setAttribute("mode", objTranscodeMediaSettings.VIDEOMODE);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/VIDEO").setAttribute("bitrate", objTranscodeMediaSettings.VIDEOBITRATE);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/VIDEO").setAttribute("quant", objTranscodeMediaSettings.VIDEOQUANT);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/VIDEO").setAttribute("frameRate", objTranscodeMediaSettings.VIDEOFRAMERATE);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/VIDEO").setAttribute("resolution", objTranscodeMediaSettings.VIDEORESOLUTION);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/AUDIO").setAttribute("type", objTranscodeMediaSettings.AUDIOTYPE);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/AUDIO").setAttribute("bitrate", objTranscodeMediaSettings.AUDIOBITRATE);
      this._projectXML.selectSingleNode(".//TRANSCODEDMEDIA/CONFIGURATION/FORMAT").setAttribute("type", objTranscodeMediaSettings.FORMATTYPE);
    }
    else {
      //use AVS interface
    }

    var objXML = this.blkAddFiles.mtxProjectMediaList.getXML();
    var xslfile = this.getServer().loadInclude("jsxuser:///JSXAPPS/IPVS_AuthTool/xsl/ProjectMediaListCDFtoXML.xsl", "ProjectMediaList", "xsl");
    // genrate the medialist xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var mediaListXML = xslDoc.transform(objXML);
    var mediaListDoc = new jsx3.xml.Document();
    mediaListDoc.loadXML(mediaListXML);
    var mediaListNode = mediaListDoc.selectSingleNode("//MEDIALIST");
    //check atleast one media clip added to project
    if(jsx3.util.strEmpty(mediaListNode.selectSingleNode("//CLIP"))) {
      this._LOG.info("No Media added to project");
      alert("Add at least one media clip to save project");
      this._setMenuSelection(this, NewProject.MENU_ADDFILES);
      return false;
    }
    //Replace Medialist in Project xml with new medialist generated from gui
    var projectMediaListNode = this._projectXML.selectSingleNode(".//MEDIALIST");
    projectMediaListNode.getParent().replaceNode(mediaListNode, projectMediaListNode);
    return true;
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._onClick_Previous = function(objEvent) {
    this._LOG.info("Project: Click Previous");
    if(this.getMenuSelected() == NewProject.MENU_PROCESSPROJECT) {
      this._setMenuSelection(objEvent, NewProject.MENU_ADDFILES);
    }
    else if(this.getMenuSelected() == NewProject.MENU_ADDFILES) {
      this._setMenuSelection(objEvent, NewProject.MENU_SETUP);
    }
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._onClick_Next = function(objEvent) {
    this._LOG.info("Project: Click Next");
    if(this.getMenuSelected() == NewProject.MENU_SETUP) {
      if(jsx3.util.strEmpty(jsx3.util.strTrim(this.blkSetUP.txtProjectName.getValue()))) {
        alert("Enter valid Project Name");
        this.blkSetUP.txtProjectName.focus();
        return;
      }
      if(jsx3.util.strEmpty(jsx3.util.strTrim(this.blkSetUP.clsOutputDir.getFolderPath()))) {
        alert("Select Output Directory");
        return;
      }
      var regex = /^[-abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_\s]*$/;
      if(!regex.test(this.blkSetUP.txtProjectName.getValue())) {
        alert("Special characters are not allowed in the Project Name.");
        this.blkSetUP.txtProjectName.setValue("");
        this.blkSetUP.txtProjectName.focus();
        return;
      }

      if(!this.blkSetUP.validateTranscodingMediaSettings()) {
        return;
      }
      this._setMenuSelection(objEvent, NewProject.MENU_ADDFILES);
    }
    else if(this.getMenuSelected() == NewProject.MENU_ADDFILES) {
      //---bug#30855---
      if(this._createProject()) {
        this._setMenuSelection(objEvent, NewProject.MENU_PROCESSPROJECT);
      }
    }
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._onClick_PrevNext = function(objEvent) {
    if(this.getMenuSelected() == NewProject.MENU_SETUP) {
      this._highLightStepLabel(true, false, false);
      this.btnPrevious.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.btnNext.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);;
      this.btnSave.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    else if(this.getMenuSelected() == NewProject.MENU_ADDFILES) {
      this._highLightStepLabel(false, true, false);
      this.btnPrevious.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.btnNext.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      this.btnSave.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    else if(this.getMenuSelected() == NewProject.MENU_PROCESSPROJECT) {
      this._highLightStepLabel(false, false, true);
      this.btnNext.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.btnSave.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      if(this._projectInProcess == false) {
        this.btnPrevious.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        this.btnSave.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
      else if(this._projectInProcess == "Processed") {
        if(this.blkApp.getMenuSelection() == MainMenu.MENU_NEWPROJECT) {
          this.btnPrevious.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
          this.btnSave.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        }
        else {
          this.btnPrevious.setEnabled(jsx3.gui.Form.STATEENABLED, true);
          this.btnSave.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        }
        this._projectInProcess = false;
      }
      else {
        this.btnPrevious.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        this.btnSave.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
    }
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._highLightStepLabel = function(step1, step2, step3) {
    if(step1 == true) {
      this.lblStep1.setFontWeight(jsx3.gui.Block.FONTBOLD);
      this.lblStep1.setColor("#ffffff", true);
    }
    else {
      this.lblStep1.setFontWeight(jsx3.gui.Block.FONTNORMAL);
      this.lblStep1.setColor("#aaaaaa", true);
    }
    if(step2 == true) {
      this.lblStep2.setFontWeight(jsx3.gui.Block.FONTBOLD);
      this.lblStep2.setColor("#ffffff", true);
    }
    else {
      this.lblStep2.setFontWeight(jsx3.gui.Block.FONTNORMAL);
      this.lblStep2.setColor("#aaaaaa", true);
    }
    if(step3 == true) {
      this.lblStep3.setFontWeight(jsx3.gui.Block.FONTBOLD);
      this.lblStep3.setColor("#ffffff", true);
    }
    else {
      this.lblStep3.setFontWeight(jsx3.gui.Block.FONTNORMAL);
      this.lblStep3.setColor("#aaaaaa", true);
    }
    this.lblStep1.repaint();
    this.lblStep2.repaint();
    this.lblStep3.repaint();
  }

  /** 
  * Set the projectSaved variable to false
  * This var is used to handle Prev/Next/Save buttons in Create/Edit project
  */
  NewProject_prototype.setProjectModified = function() {
    this._projectSaved = false;
    this._onClick_PrevNext();
  }

  /** 
  * Set the projectSaved variable to false
  * This var is used to handle Prev/Next/Save buttons in Create/Edit project
  */
  NewProject_prototype.setProjectInProcess = function(inProcess) {
    this._projectInProcess = inProcess;
    this._onClick_PrevNext();
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._onClick_Main = function(objEvent) {
    if(this.getMenuSelected() == NewProject.MENU_PROCESSPROJECT) {
      if(this._projectInProcess == true) {
        var confirmation = confirm("Are you sure you want to cancel processing?");
        if(!confirmation) {
          return false;
        }
        else {
          this.blkApp.setCancelProcessProject(objEvent, this.projectName);
        }
      }
    }
    //publish MainApp event to go back to main menu
    this._LOG.info("Project: Menu selected: " + MainMenu.MENU_MAINMENU);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_MAINMENU);
  }

  /** @private @jsxobf-clobber */
  NewProject_prototype._onClick_Cancel = function(objEvent) {
    if(this.getMenuSelected() == NewProject.MENU_PROCESSPROJECT) {
      if(this._projectInProcess == true) {
        var confirmation = confirm("Are you sure you want to cancel processing?");
        if(!confirmation) {
          return false;
        }
        else {
          this.blkApp.setCancelProcessProject(objEvent, this.projectName);
        }
      }
    }
    var menuOnCancel = MainMenu.MENU_MAINMENU;
    if(this.blkApp.getMenuSelection() == MainMenu.MENU_EDITPROJECT || this.blkApp.getMenuSelection() == MainMenu.MENU_PROCESSPROJECT) {
      menuOnCancel = MainMenu.MENU_VIEWPROJECTS;
    }
    //publish MainApp event to go back to main menu
    this._LOG.info("Project: Menu selected: " + menuOnCancel);
    this.blkApp.setMenuSelection(objEvent, menuOnCancel);
  }
});