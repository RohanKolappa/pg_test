/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* ViewProject class provides UI to view/edit/delete projects
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.ViewProject", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(ViewProject, ViewProject_prototype) {

  /** @private @jsxobf-clobber */
  ViewProject_prototype._LOG = jsx3.util.Logger.getLogger(ViewProject.jsxclass.getName());

  var MainMenu = com.ipvs.gui.authtool.MainMenu;

  /**
   * The instance initializer.
   */
  ViewProject_prototype.init = function() {
    //Get handle of child components and subscribe as required
    this._ViewState = false;
    this.mtxProjectsList = this.getDescendantOfName("mtxProjectslist");    
    this.btnProjectDelete = this.mtxProjectsList.getDescendantOfName("imgProjectDelete");
    this.btnProjectEdit = this.mtxProjectsList.getDescendantOfName("imgProjectEdit");
    this.btnProjectProcess = this.mtxProjectsList.getDescendantOfName("imgProcessProject");
    this.btnViewProjectsClose = this.getDescendantOfName("btnViewProjectsClose");
    this.btnNewProject = this.getDescendantOfName("btnNewProject");
    this.btnCloseViewProjects = this.getDescendantOfName("btnCloseViewProjects");

    //this.mtxProjectsList.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._editProject);
    this.btnProjectEdit.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._editProject);
    this.btnProjectProcess.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._processProject);
    this.btnProjectDelete.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._deleteProject);
    this.btnNewProject.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_NewProject);    
    this.btnViewProjectsClose.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Main);
    this.btnCloseViewProjects.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Main);

    //get the handle of MainApp class
    this.blkApp = this.getAncestorOfType("com.ipvs.authtool.App");
    this.blkApp.subscribe(com.ipvs.authtool.App.MENU_SELECT, this, this._toggle);
    this.blkApp.subscribe(com.ipvs.authtool.App.PROJECT_ADDEDIT, this, this._handleAddEditProject);
    this._setFormatHandlersForList();
    this._LOG.info("ViewProjects: Initiated");
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._toggle = function(objEvent) {
    if( this.blkApp.getMenuSelection() == MainMenu.MENU_VIEWPROJECTS ) {
      this.cmpShow();
      this._LOG.info("ViewProjects: Show");
      //Only first time read the list from projects area and display
      if( !this._ViewState ) {
        this._initFS();
        this._initLoad();
      }
    }
    else {
      this.cmpHide();
      this._LOG.info("ViewProjects: Hide");
    }
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._initLoad = function() {
    this.mtxProjectsList.clearXmlData();   
    //Read all projects and display the list
    var listFiles = this._clsFSOProjects.listFiles();
    this._LOG.info("ViewProjects: Access to project files");
    for( var i = 0; i < listFiles.length; i++ ) {
      var listFile = listFiles[i];
      this._processProjectFile(listFile);
    }
    this.mtxProjectsList.repaintData();
    this._ViewState = true;
    this._LOG.info("ViewProjects: List loaded");
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._processProjectFile = function(projectFile) {
     this._LOG.info("ViewProjects_processProjectFile:Hidden =" + projectFile.isHidden());
     this._LOG.info("ViewProjects_processProjectFile:isFile =" + projectFile.isFile());
     this._LOG.info("ViewProjects_processProjectFile:Extension=" + projectFile.getExtension());
    if( !projectFile.isHidden() && projectFile.isFile() && (projectFile.getExtension()== "xml")) {
      var projectDoc = (new jsx3.xml.Document()).loadXML(projectFile.read());
      var error = projectDoc.getError();
      if(!jsx3.util.strEmpty(error) && error.code =="0"){
        var projectNode =projectDoc.selectSingleNode("//PROJECT");
        if(jsx3.util.strEmpty(projectNode))return;
        var objRec = new Object;
        objRec.jsxid = projectDoc.selectSingleNode("//PROJECT/NAME").getValue();
        objRec.jsxProjectName = projectDoc.selectSingleNode("//PROJECT/NAME").getValue();
        objRec.jsxProjectCreateddate = projectDoc.selectSingleNode("//PROJECT/CREATED").getValue();
        objRec.jsxProjectLength = projectDoc.selectSingleNode("//PROJECT/LENGTH").getValue();
        objRec.jsxProjectSize = projectDoc.selectSingleNode("//PROJECT/SIZE").getValue();
        this.mtxProjectsList.insertRecord(objRec, null, false);
      }
    }
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._initFS = function() {
    //Get the handle of Projects place holder using file system
    this._clsFSOFileSystem = new jsx3.io.FSOFileSystem();
    this._clsFSOProjects = this._clsFSOFileSystem.getFile(clsDefaultSettings.getProjectsPath());
    if( !this._clsFSOProjects.exists() || !this._clsFSOProjects.isDirectory() ) {
      this._clsFSOProjects.mkdir();
    }
    this._LOG.info("ViewProjects: Access to Projects");
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._onClick_NewProject = function(objEvent) {
    //publish MainApp event to go back to main menu
    this._LOG.info("ViewProjects: Menu selected: " + MainMenu.MENU_NEWPROJECT);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_NEWPROJECT);
  }
  
  /** @private @jsxobf-clobber */
  ViewProject_prototype._editProject = function(objEvent) {
    //Set the global var ProjectSelected and publish EDITPROJECT event
    var objRecordId = this.mtxProjectsList.getValue();
    this.blkApp.setProjectSelected(objEvent, objRecordId, false);
    this._LOG.info("ViewProjects: Edit project :" + objRecordId);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_EDITPROJECT);
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._processProject = function(objEvent) {
    //Set the global var ProjectSelected and publish PROCESSPROJECT event
    var objRecordId = this.mtxProjectsList.getValue();
    this.blkApp.setProjectSelected(objEvent, objRecordId, false);
    this._LOG.info("ViewProjects: Process project :" + objRecordId);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_PROCESSPROJECT);
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._deleteProject = function(objEvent) {
    //Delete selected project from disk and from list
    var objRecordId = this.mtxProjectsList.getValue();
    var ObjRecord = this.mtxProjectsList.deleteRecord(objRecordId);
    this._LOG.info("ViewProjects: Delete project :" + objRecordId);
    try {   
      this._clsFSOFileSystem.getFile(clsDefaultSettings.getProjectsPath() + "\\" + objRecordId + ".xml").deleteFile();
    }
    catch(e) {
      this._LOG.warn("ViewProjects: Error while deleting project :" + objRecordId + " ERROR:" + e.description);      
    }
  }

  /** @private @jsxobf-clobber */
  ViewProject_prototype._handleAddEditProject = function() {
    var pName = this.blkApp.getProjectSelected();
    if(!this._clsFSOFileSystem)return;
    var projectFile = this._clsFSOFileSystem.getFile(clsDefaultSettings.getProjectsPath() + "\\" + pName + ".xml");
    this._processProjectFile(projectFile);
    this.mtxProjectsList.repaintData();
    this._LOG.info("ViewProjects: Add/Update project :" + pName);
  }
  
  /**
   * Destroy all child components 
   */
  ViewProject_prototype.destroy = function() {
    this.btnViewProjectsClose = null;
    this.btnCloseViewProjects = null;
  }
  
  /** @private @jsxobf-clobber */
  ViewProject_prototype._onClick_Main = function(objEvent) {
    //publish MainApp event to go back to main menu
    this._LOG.info("ViewProjects: Menu selected: " + MainMenu.MENU_MAINMENU);
    this.blkApp.setMenuSelection(objEvent, MainMenu.MENU_MAINMENU);
  }
  
  /** @private @jsxobf-clobber */
  ViewProject_prototype._setFormatHandlersForList = function() {
    this.mtxProjectsList.getDescendantOfName("mcProjectCreatedDate", false, false).setFormatHandler("@datetime,MM/dd/yyyy HH:mm");
    this.mtxProjectsList.getDescendantOfName("mcProjectLength", false, false).setFormatHandler(jsx3.$F(this.formatMediaFileTime).bind(this));
    this.mtxProjectsList.getDescendantOfName("mcProjectSize", false, false).setFormatHandler(this.formatMediaFileSize);
  }

});