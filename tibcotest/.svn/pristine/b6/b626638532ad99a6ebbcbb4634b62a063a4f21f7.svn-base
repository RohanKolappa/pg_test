jsx3.Class.defineClass("DirSelection", null, null, function(DirSelection, DirSelection_prototype) {

    //DirSelection.CmpFolderPath = null;
    DirSelection.PlugInHandle = null;
    DirSelection.FsSelect = null;
    DirSelection.FncCallback = null;
    DirSelection.FolderPath = null;
    
    DirSelection_prototype.init = function(fncCallback) {
      //DirSelection.CmpFolderPath = getJsxByName(objName);
      DirSelection.FncCallback = fncCallback;
      DirSelection.PlugInHandle = jsx3.amp.Engine.getEngine(PNameSpace).getPlugIn("jsx3.io");
      var Fs = DirSelection.PlugInHandle.getAvailableFileSystems();
      var FsWrite = Fs.filter(function(e) { return e.hasWrite(); });
      DirSelection.FsSelect = FsWrite[0].getId();
      DirSelection._onFSSelect(DirSelection.FsSelect);
    }

    DirSelection_prototype.doBrowse = function() {
      var fs = DirSelection._getFS(DirSelection.FsSelect);
      if (!fs) {
        log.warn("No file system " + DirSelection.FsSelect);
        return;
      }

      //var strValue = DirSelection.CmpFolderPath.getValue();
      var objFolder = fs.getInstance().getFile(DirSelection.FolderPath);
      while (objFolder && ! objFolder.isDirectory()) {
        objFolder = objFolder.getParentFile();
      }

      if (!objFolder || !objFolder.isDirectory())
        objFolder = fs.getInstance().getFile("/");

      jsx3.amp.Engine.getEngine(PNameSpace).getPlugIn("jsx3.io.browser").chooseFolder(PNameSpace.getRootBlock(), {
          name:"newhomedialog", modal:true, title:"Choose Folder", okLabel:"Choose",
          folder: objFolder, onChoose: jsx3.$F(function(objFile) { DirSelection._onBrowseChoose(objFile); }).bind(this)
      });
    }
  
    DirSelection_prototype.getFolderPath = function() {
      return DirSelection.FolderPath;
    }

    DirSelection._getFS = function(id) {
      return DirSelection.PlugInHandle.getFileSystemById(id);
    }

    DirSelection._onFSSelect = function(id) {
      var fileSystem = DirSelection._getFS(id);

     // DirSelection.CmpFolderPath.setValue("", true);

      if (fileSystem) {
        fileSystem.load().when(function() {
          var fs = fileSystem.getInstance();
          var userDocs = fs.getUserDocuments();
          if (userDocs)
            //DirSelection.CmpFolderPath.setValue(userDocs.resolve(jsx3.ide.DEFAULT_USER_HOME).getAbsolutePath(), true);
            DirSelection.FolderPath = userDocs.resolve(jsx3.ide.DEFAULT_USER_HOME).getAbsolutePath();
        });
      }
    };

    DirSelection._onBrowseChoose = function(objFile) {
      //DirSelection.CmpFolderPath.setValue(objFile.getAbsolutePath());
      DirSelection.FolderPath = objFile.getAbsolutePath();
      DirSelection.FncCallback();
    };

    DirSelection_prototype.getFilesList = function() {
      var fs = DirSelection._getFS(DirSelection.FsSelect);
      var objFSList = fs.getInstance().getFile(DirSelection.FolderPath);
      return objFSList.listFiles();
    }

});
