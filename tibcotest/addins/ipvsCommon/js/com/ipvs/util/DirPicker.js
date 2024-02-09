/* place JavaScript code here */
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.util.DirPicker", jsx3.gui.Block, null, function(DirPicker, DirPicker_prototype) {
  DirPicker_prototype._LOG = jsx3.util.Logger.getLogger(DirPicker.jsxclass.getName());

  DirPicker_prototype.PlugInHandle = null;
  DirPicker_prototype.FsSelect = null;
  DirPicker_prototype.FncCallback = null;
  DirPicker_prototype.FolderPath = null;
  DirPicker.ON_FOLDER_SELECT = "ONFOLDERSELECTED";
     
  DirPicker_prototype.init = function() {
    this.txtFolderPath = this.getDescendantOfName("txtDirPath");
    this.btnBrowse = this.getDescendantOfName("btnBrowseForDir");
    this.btnBrowse.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._doBrowse);
    this.PlugInHandle = jsx3.amp.Engine.getEngine(this.getServer()).getPlugIn("jsx3.io");
    var Fs = this.PlugInHandle.getAvailableFileSystems();
    var FsWrite = Fs.filter(function(e) { return e.hasWrite(); });
    this.FsSelect = FsWrite[0].getId();
    this._onFSSelect(this.FsSelect);
  }

  DirPicker_prototype._doBrowse = function() {
    var fs = this._getFS(this.FsSelect);
    if (!fs) {
      log.warn("No file system " + this.FsSelect);
      return;
    }

    if(jsx3.util.strEmpty(this.FolderPath)) {
      var objFolder = fs.getInstance().getRoots()[0];
    }
    else{
      var objFolder = fs.getInstance().getFile(this.FolderPath);
    }
    while (objFolder && ! objFolder.isDirectory()) {
      objFolder = objFolder.getParentFile();
    }

    if (!objFolder || !objFolder.isDirectory())
      objFolder = fs.getInstance().getFile("/");

    jsx3.amp.Engine.getEngine(this.getServer()).getPlugIn("jsx3.io.browser").chooseFolder(this.getServer().getRootBlock(), {
        name:"newhomedialog", modal:true, title:"Choose Folder", okLabel:"Choose",
        folder: objFolder, onChoose: jsx3.$F(function(objFile) { this._onBrowseChoose(objFile); }).bind(this)
    });
  }

  DirPicker_prototype.getFolderPath = function() {
    return this.FolderPath;
  }
  
  DirPicker_prototype.setFolderPath = function(fPath) {
    this.txtFolderPath.setValue(fPath);
    this.FolderPath = fPath;
  }

  DirPicker_prototype._getFS = function(id) {
    return this.PlugInHandle.getFileSystemById(id);
  }

  DirPicker_prototype._onFSSelect = function(id) {
    var fileSystem = this._getFS(id);

    if( fileSystem ) {
      var ThisClass = this;
      fileSystem.load().when(function() {
        var fs = fileSystem.getInstance();
        var userDocs = fs.getUserDocuments();
        if (userDocs)
          ThisClass.txtFolderPath.setValue(userDocs.resolve(jsx3.ide.DEFAULT_USER_HOME).getAbsolutePath(), true);
          ThisClass.FolderPath = userDocs.resolve(jsx3.ide.DEFAULT_USER_HOME).getAbsolutePath();
      });
    }
  };

  DirPicker_prototype._onBrowseChoose = function(objFile) {
    this.txtFolderPath.setValue(objFile.getAbsolutePath());
    this.FolderPath = objFile.getAbsolutePath();
    this.doEvent(DirPicker.ON_FOLDER_SELECT, {objEVENT:this});
  };

  DirPicker_prototype.getFilesList = function() {
    var fs = this._getFS(this.FsSelect);
    var objFSList = fs.getInstance().getFile(this.FolderPath);
    if( objFSList.exists() && objFSList.isDirectory() ) {
      return objFSList.listFiles();
    }
    else {
      return null;
    }
  }
});