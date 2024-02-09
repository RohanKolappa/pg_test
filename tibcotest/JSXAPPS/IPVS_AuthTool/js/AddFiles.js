/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* AddFiles class provides UI to configure the medialist while Create/Edit a project.
* This is second step of NewProject.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.AddFiles", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(AddFiles, AddFiles_prototype) {

  /** @private @jsxobf-clobber */
  AddFiles_prototype._LOG = jsx3.util.Logger.getLogger(AddFiles.jsxclass.getName());

  /**
  * The instance initializer.
  */
  AddFiles_prototype.init = function() {
    //Get handle of child components and subscribe as required
    this._ViewState = false;
    this._exePath = null;
    this._tempPath = null;
    this._clsFSOFileSystem = null;
    this._dateTimeFormatForDestTitle = null;
    this._projectSize = 0;
    this._projectLength = 0;
    this.clsInputDir = this.getDescendantOfName("blkInputDir").getDescendantOfName("blkDirPicker");
    this.mtxMediaList = this.getDescendantOfName("mtxMedialist");
    this.btnAdd = this.getDescendantOfName("btnAdd");
    this.btnAddAll = this.getDescendantOfName("btnAddAll");
    this.mtxProjectMediaList = this.getDescendantOfName("mtxProjectMediaList");
    this.btnMediaDelete = this.mtxProjectMediaList.getDescendantOfName("imgDeleteProjectMedia");
    this.btnMediaEdit = this.mtxProjectMediaList.getDescendantOfName("imgEditProjectMedia");
    this.btnRemoveAll = this.getDescendantOfName("btnRemoveAll");
    this.lblTotalAdded = this.getDescendantOfName("lblTotalAdded");
    this.lblLoadingMsg = this.getDescendantOfName("blkLoadingMsg");
    this.blkEditMediaSegment = this.getDescendantOfName("blkEditMediaSegment");

    this.btnAdd.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Add);
    this.btnAddAll.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_AddAll);
    this.btnMediaDelete.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._deleteProjectMedia);
    this.btnMediaEdit.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._editProjectMedia);
    this.btnRemoveAll.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_RemoveAll);
    this.clsInputDir.subscribe(com.ipvs.util.DirPicker.ON_FOLDER_SELECT, this, this._loadMediaList);

    //get the handle of Ancestor(NewProject) class
    this.blkNewProject = this.getAncestorOfType("com.ipvs.gui.authtool.NewProject");
    this.blkNewProject.subscribe(com.ipvs.gui.authtool.NewProject.MENU_SELECT, this, this._toggle);
    this._setFormatHandlersForList();
    this._resetSubCmp();
    this._LOG.info("ProjectAddFiles: Initiated");
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._toggle = function(objEvent) {
    if(this.blkNewProject.getMenuSelected() == null) {
      this._ViewState = false;
      this._resetSubCmp();
    }
    else if(this.blkNewProject.getMenuSelected() == com.ipvs.gui.authtool.NewProject.MENU_ADDFILES) {
      this.cmpShow();
      jsx3.sleep((function() {
        this._LOG.info("ProjectAddFiles: Show");
        if(!this._ViewState) {
          this._initLoad();
        }
      }), null, this);

    }
    else {
      this.cmpHide();
      this._LOG.info("ProjectAddFiles: Hide");
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._initLoad = function() {
    this._projectSize = 0;
    this._projectLength = 0;
    this._clsFSOFileSystem = new jsx3.io.FSOFileSystem();
    this._dateTimeFormatForDestTitle = new jsx3.util.DateFormat("yyyy-MM-dd-HH-mm-ss");
    this.clsInputDir.setFolderPath(clsDefaultSettings.getInputDir());
    this._exePath = clsDefaultSettings.getGIAppPath().replace(/\//g, "\\") + "\\utils\\mediaconverter.exe";
    this._tempPath = clsDefaultSettings.getProjectsPath() + "\\" + "temp";
    //check for temp dir in projects place holder
    //other wise create it to use in mkd file processing
    var objTempDir = this._clsFSOFileSystem.getFile(this._tempPath);
    if(!objTempDir.exists()) {
      objTempDir.mkdir();
      this._LOG.info("ProjectAddFiles: temp dir added");
    }
    try {
      this._activeXWSH = new ActiveXObject('WScript.Shell');
    }
    catch(e) {
      this._LOG.warn("ProjectAddFiles: " + e.description);
    }

    //Load Medialist from selected Directory
    this._loadMediaList();
    //If it is edit project load media list from project xml
    if(this.blkNewProject.getProjectName() != null) {
      this._loadProjectMediaList();
      this.btnRemoveAll.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this._updateTotalAddedLabel();
    this._ViewState = true;
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._resetSubCmp = function() {
    this.mtxMediaList.clearXmlData();
    this.mtxMediaList.repaintData();
    this.mtxProjectMediaList.clearXmlData();
    this.mtxProjectMediaList.repaintData();
    this.btnAdd.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnAddAll.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnRemoveAll.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.lblLoadingMsg.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN, true);
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._loadMediaList = function() {
    this.btnAdd.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnAddAll.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.mtxMediaList.clearXmlData();
    this.mtxMediaList.repaintData();
    if(jsx3.util.strEmpty(this.clsInputDir.getFolderPath())) {
      this._LOG.info("ProjectAddFiles: Input directory is not selected");
      return;
    }
    this.lblLoadingMsg.setVisibility(jsx3.gui.Block.VISIBILITYVISIBLE, true);
    jsx3.sleep((function() {
      //Get list of files and check for required files and process the same
      var files = this.clsInputDir.getFilesList();
      if(jsx3.util.strEmpty(files)) {
        this._LOG.warn("ProjectAddFiles: Invalid Input directory");
        this.lblLoadingMsg.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN, true);
        return;
      }

      //check for 2.x media
      var dbDir = null;
      var blobDir = null;
      var bFilesAdded = false;
      for(var i = 0;i < files.length;i++) {
        var folderItem = files[i];
        if(!folderItem.isDirectory() || folderItem.isHidden() || !folderItem.exists()) continue;

        if(folderItem.getName() == "db") {
          dbDir = folderItem;
        }
        else if(folderItem.getName() == "blob") {
          blobDir = folderItem;
        }
      }
      if(dbDir != null && blobDir != null) {
        var dbFilesList = dbDir.listFiles();
        for(var j = 0;j < dbFilesList.length;j++) {
          var dbFileItem = dbFilesList[j];
          if(dbFileItem.isHidden() || !dbFileItem.exists()) continue;
          var dbFileContent = dbFileItem.read();
          this._processTwoDotXMedia(dbFileContent, dbFileItem.getName());
          bFilesAdded = true;
        }
      }
      else {
        for(var i = 0;i < files.length;i++) {
          var file = files[i];
          if(file.isHidden() || !file.exists()) continue;

          var bFile = file.isFile();
          if(bFile && (!jsx3.util.strEmpty(file.getExtension()) && file.getExtension().toLowerCase() == "mkd")) {
            var strFileData = this._processMKDFile(file, "metadata");
            if(!jsx3.util.strEmpty(strFileData)) {
              this._insertMKDMediaList(strFileData);
              bFilesAdded = true;
            }
          }
        }
      }
      this.mtxMediaList.repaintData();
      this.lblLoadingMsg.setVisibility(jsx3.gui.Block.VISIBILITYHIDDEN, true);
      if(bFilesAdded) {
        this.btnAdd.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        this.btnAddAll.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
      this._LOG.info("ProjectAddFiles: Loaded media list from library");
    }), null, this);
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._processTwoDotXMedia = function(strFileContent, fileName) {
    //Old 2.x exported media has node like <MEDIA ID=>. So replace ID to handle in xml
    //While exporting 2.x media IP was appended to filenames.Need to handle while adding to list
    var fileNameProfix = fileName.split("_")[0];
    strFileContent = strFileContent.replace(/ID=>/, "ID='0'>");
    //If there is & in string then new jsx3.xml.Document().loadXML is returning error.
    //So replacing with &amp;.
    strFileContent = strFileContent.replace(/&/g, "&amp;");
    var dbDoc = (new jsx3.xml.Document()).loadXML(strFileContent);
    if(!jsx3.util.strEmpty(dbDoc.getError()) && dbDoc.getError().code != "0") {
      this._LOG.warn("ProjectAddFiles: Invalid db file");
      return;
    }
    //There is no media group information in clips
    //look for Clips and if it is not empty then it is a group file
    var clipNode = dbDoc.selectSingleNode("//CLIPS");
    if(!jsx3.util.strEmpty(clipNode) && !jsx3.util.strEmpty(clipNode.getValue())) {
      var grpID = fileName;
      var grpFileMetadataNode = dbDoc.selectSingleNode("//METADATA");
      this._insertTwoDotXMediaList(grpFileMetadataNode, fileName, "");

      var clipNodeIDArr = clipNode.getValue().split(",");
      for(var i = 0;i < clipNodeIDArr.length;i++) {
        var clipFileName = fileNameProfix + "_" + clipNodeIDArr[i];
        var clipFile = this._clsFSOFileSystem.getFile(this.clsInputDir.getFolderPath() + "\\db\\" + clipFileName);
        if(!clipFile.isFile() || clipFile.isHidden() || !clipFile.exists()) continue;
        var clipFileContent = clipFile.read();
        clipFileContent = clipFileContent.replace(/ID=>/, "ID='0'>");
        var clipDoc = (new jsx3.xml.Document()).loadXML(clipFileContent);
        var clipFileMetadataNode = clipDoc.selectSingleNode("//METADATA");
        this._insertTwoDotXMediaList(clipFileMetadataNode, clipFileName, grpID);
      }
      this._LOG.info("ProjectAddFiles: Added 2.x media file");
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._insertTwoDotXMediaList = function(fileMetaDataNode, recJSXID, fileGroupID) {
    var metaDataArr = this.unSerializeStr(fileMetaDataNode.getValue());
    if(!jsx3.util.strEmpty(metaDataArr)) {
      if(metaDataArr[11] == 1) {
        var streamType = "V2D";
      }
      else if(metaDataArr[11] == 2) {
        var streamType = "MPEGTS";
      }
      else if(metaDataArr[11] == 3) {
        var streamType = "RTP";
      }
      else if(metaDataArr[11] == 4) {
        var streamType = "UDP";
      }
      else {
        var streamType = "";
      }

      if(streamType != "UDP") {  //bug#31814 udp type not allowed to author
        var objRec = new Object;
        objRec.jsxid = recJSXID;
        objRec.jsxFileNID = recJSXID;
        objRec.jsxName = metaDataArr[2];
        objRec.jsxCreatedDate = parseInt(metaDataArr[8] ? metaDataArr[8] : "0") * 1000;
        objRec.jsxLength = parseInt(metaDataArr[4] ? metaDataArr[4] : "0") * 1000;
        objRec.jsxPlayLength = parseInt(metaDataArr[4] ? metaDataArr[4] : "0") * 1000;
        objRec.jsxSize = parseInt(metaDataArr[12] ? metaDataArr[12] : "0") * 1000;
        objRec.jsxStreamType = streamType;
        objRec.jsxGrpNID = fileGroupID;
        objRec.jsxBasePath = this.clsInputDir.getFolderPath();
        objRec.jsxSDPInfo = metaDataArr[13];
        objRec.jsxIsTwodotXMedia = true;
        this.mtxMediaList.insertRecord(objRec, fileGroupID, false);
      }
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._processMKDFile = function(objFile, infoArg) {
    //For each MKD file get the info using mediaconverter.exe.
    //This will creates tmp file with info.
    //Read the content and delete before process next file.
    if(jsx3.util.strEmpty(infoArg)) infoArg = "metadata";
    var tempFileContent = null;
    var infoCMD = "cmd /c \"\"" + this._exePath + "\" -i " + infoArg + " -f \"" + objFile.getAbsolutePath() + "\" >> \"" + this._tempPath + "\\tmp.xml\"\"";
    var cmdResult = this._activeXWSH.Run(infoCMD, 0, true);
    if(cmdResult == 0) { //0 is success
      var tempFile = this._clsFSOFileSystem.getFile(this._tempPath + "\\tmp.xml");
      tempFileContent = tempFile.read();
      tempFile.deleteFile();
    }
    else {
      this._LOG.warn("MediaConverter: Error while getting Group/File info");
    }
    this._activeXWSH.Quit;
    return tempFileContent;
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._insertMKDMediaList = function(strRecord) {
    //From converter info get the required information and add records to list
    var mediaDoc = (new jsx3.xml.Document()).loadXML(strRecord);
    var grpMediaRecordNode = mediaDoc.selectSingleNode(".//record[.//Type='MediaGroup']");
    var grpMediaNID = "NONE";
    if(!jsx3.util.strEmpty(grpMediaRecordNode)) {
      grpMediaNID = grpMediaRecordNode.getAttribute("nid");
      var grpMSFResourceNode = mediaDoc.selectSingleNode(".//MediaStreamFileResource[@NID='" + grpMediaNID + "']");

      var objGrpRec = new Object;
      objGrpRec.jsxid = grpMediaNID;
      objGrpRec.jsxFileNID = grpMediaNID;
      objGrpRec.jsxName = grpMSFResourceNode.selectSingleNode(".//Info/Title").getValue();
      objGrpRec.jsxCreatedDate = grpMSFResourceNode.selectSingleNode(".//Properties/DateCreated").getValue();
      objGrpRec.jsxLength = grpMSFResourceNode.selectSingleNode(".//MediaGroup/PlayLength").getValue();
      objGrpRec.jsxPlayLength = grpMSFResourceNode.selectSingleNode(".//MediaGroup/PlayLength").getValue();
      objGrpRec.jsxSize = grpMSFResourceNode.selectSingleNode(".//Properties/Size").getValue();
      objGrpRec.jsxStreamType = grpMSFResourceNode.selectSingleNode(".//StreamTypeList/StreamType").getValue();
      objGrpRec.jsxGrpNID = "";
      objGrpRec.jsxBasePath = this.clsInputDir.getFolderPath();
      objGrpRec.jsxSDPInfo = "";
      objGrpRec.jsxIsTwodotXMedia = false;

      this.mtxMediaList.insertRecord(objGrpRec, null, false);
    }

    var clipMediaRecordNode = mediaDoc.selectSingleNode(".//record[.//Type='MediaClip']");
    if(!jsx3.util.strEmpty(clipMediaRecordNode)) {
      var clipMediaNID = clipMediaRecordNode.getAttribute("nid");
      var clipMSFResourceNode = mediaDoc.selectSingleNode("//MediaStreamFileResource[@NID='" + clipMediaNID + "']");
      var streamType = clipMSFResourceNode.selectSingleNode(".//StreamTypeList/StreamType").getValue();
      if(streamType != "UDP") { //bug#31814 udp type not allowed to author  
        var objClipRec = new Object;
        objClipRec.jsxid = clipMediaNID;
        objClipRec.jsxFileNID = clipMediaNID;
        objClipRec.jsxName = clipMSFResourceNode.selectSingleNode(".//Info/Title").getValue();
        objClipRec.jsxCreatedDate = clipMSFResourceNode.selectSingleNode(".//Properties/DateCreated").getValue();
        objClipRec.jsxLength = clipMSFResourceNode.selectSingleNode(".//MediaClip/PlayLength").getValue();
        objClipRec.jsxPlayLength = clipMSFResourceNode.selectSingleNode(".//MediaClip/PlayLength").getValue();
        objClipRec.jsxSize = clipMSFResourceNode.selectSingleNode(".//Properties/Size").getValue();
        objClipRec.jsxStreamType = streamType;
        objClipRec.jsxSDPInfo = "";
        if(streamType == "RTP") {
          objClipRec.jsxSDPInfo = clipMSFResourceNode.selectSingleNode(".//MediaClip/MediaStreamProfileInfo/RTPMediaStreamProfileInfo/SDP-Template-Src").getValue();
        }
        objClipRec.jsxGrpNID = grpMediaNID;
        objClipRec.jsxBasePath = this.clsInputDir.getFolderPath();
        objClipRec.jsxIsTwodotXMedia = false;

        this.mtxMediaList.insertRecord(objClipRec, grpMediaNID, false);
      }
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._loadProjectMediaList = function() {
    this.mtxProjectMediaList.setXMLTransformers("xsl/ProjectMediaListXMLtoCDF.xsl");
    this.mtxProjectMediaList.setSourceXML(this.blkNewProject.getProjectMediaListNode());
    this.mtxProjectMediaList.repaintData();
    this._projectSize = this.blkNewProject.getProjectSize();
    this._projectLength = this.blkNewProject.getProjectLength();
    this._LOG.info("ProjectAddFiles: Loaded project media list");
  }
  /** @private @jsxobf-clobber */
  AddFiles_prototype._deleteProjectMedia = function(objEvent) {
    var recoedId = this.mtxProjectMediaList.getValue();
    var objRecord = this.mtxProjectMediaList.deleteRecord(recoedId);
    this._processDeletedProjectMedia(objRecord);
    this._updateTotalAddedLabel();
    this.blkNewProject.setProjectModified();
    this._LOG.info("ProjectAddFiles: Delete media clip from project media lists:" + recoedId);
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._processDeletedProjectMedia = function(objRecord) {
    //recalculate project size and length if clip deleted
    var recIter = objRecord.selectNodeIterator(".//record");
    if(!recIter.hasNext()) {
      // if deleted only one Item/clip
      this._updateSizeAndLengthOnDelete(objRecord);
    }
    else {
      //if deleted whole group node
      while(recIter.hasNext()) {
        this._updateSizeAndLengthOnDelete(recIter.next());
      }
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._editProjectMedia = function(objEvent) {
    var recoedId = this.mtxProjectMediaList.getValue();
    var selRecordNode = this.mtxProjectMediaList.getRecordNode(recoedId);
    if(jsx3.util.strEmpty(selRecordNode)) return;
    this.blkEditMediaSegment.open(selRecordNode, "EDIT");
  }

  /** @private @jsxobf-clobber */
  //helper function to recalculate project size and length when delete media from project
  AddFiles_prototype._updateSizeAndLengthOnDelete = function(recNode) {
    var grpNodeID = recNode.getAttribute("jsxGrpNID");
    if(!jsx3.util.strEmpty(grpNodeID)) {
      this._projectSize = this._projectSize - recNode.getAttribute("jsxSize");
      this._projectLength = this._projectLength - recNode.getAttribute("jsxPlayLength");
    }
  }

  /**
  * Destroy all child components 
  */
  AddFiles_prototype.destroy = function() {
    //this.btnMain = null;
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._onClick_Add = function(objEvent) {
    if(this._validateForV2DMedia("SELECTED")) {
      //Add Selected clip(s) to project media list
      var selectedRecIDs = this.mtxMediaList.getSelectedIds();
      this._addSelectedRecords(selectedRecIDs);
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._onClick_AddAll = function(objEvent) {
    if(this._validateForV2DMedia("ALL")) {
      //Get all records from medialist add them to project medialist
      var recIDs = this.mtxMediaList.getRecordIds();
      this._addSelectedRecords(recIDs);
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._addSelectedRecords = function(arrRecordId) {
    var lstRecorId = new jsx3.util.List(arrRecordId);
    for(i = 0;i < lstRecorId.size();i++) {
      var objRecord = this.mtxMediaList.getRecord(lstRecorId.get(i));
      var newNID = getUUID().toLowerCase();
      objRecord.jsxid = newNID;
      objRecord.jsxName = this.getNewName(objRecord.jsxName, objRecord.jsxCreatedDate);
      objRecord.isActive = true;
      objRecord.isRoot = "";
      this._insertProjectMediaFile(objRecord);
      //If selected record is group file then add all clips in that group
      if(objRecord.jsxGrpNID == "") {
        var recordNode = this.mtxMediaList.getRecordNode(lstRecorId.get(i));
        var recIter = recordNode.selectNodeIterator(".//record");
        while(recIter.hasNext()) {
          var clipRecordNode = recIter.next();
          var objRec = {
            jsxid: getUUID().toLowerCase(),
            jsxFileNID: clipRecordNode.getAttribute("jsxFileNID"),
            jsxName: this.getNewName(clipRecordNode.getAttribute("jsxName"), clipRecordNode.getAttribute("jsxCreatedDate")),
            jsxCreatedDate: clipRecordNode.getAttribute("jsxCreatedDate"),
            jsxLength: clipRecordNode.getAttribute("jsxLength"),
            jsxPlayLength: clipRecordNode.getAttribute("jsxPlayLength"),
            jsxSize: clipRecordNode.getAttribute("jsxSize"),
            jsxStreamType: clipRecordNode.getAttribute("jsxStreamType"),
            jsxSDPInfo: clipRecordNode.getAttribute("jsxSDPInfo"),
            jsxGrpNID: newNID,
            jsxBasePath: clipRecordNode.getAttribute("jsxBasePath"),
            jsxIsTwodotXMedia: clipRecordNode.getAttribute("jsxIsTwodotXMedia"),
            jsxActive: true,
            isRoot: false
          };
          this._insertProjectMediaFile(objRec);
          //check and remove if this clip is also in selected list which is already processed with group.
          for(j = 0;j < lstRecorId.size();j++) {
            if(clipRecordNode.getAttribute("jsxid") == lstRecorId.get(j)) lstRecorId.remove(lstRecorId.get(j));
          }
        }
      }
    }

    if(i == 0) {
      alert("Select atleast one media clip");
      return false;
    }
    else {
      this.mtxProjectMediaList.repaintData();
      this._updateTotalAddedLabel();
      this._LOG.info("ProjectAddFiles: Add Selected media clip(s) to project media list");
      //mark the project as modified to handle navigation buttons
      this.blkNewProject.setProjectModified();
      this.btnRemoveAll.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  /**
   * This will updates project media list with edited media group/clip
   * @param mediaNode {Entity} MediaGroup/Clip Node
   * @param mediaLength {Intiger} Play length of edited clip/group
   * @param mediaSize {Intiger} size of edited clip/group
   * @param processMode {String} Add/Edit
   */
  AddFiles_prototype.processEditedFile = function(mediaNode, mediaLength, mediaSize, processMode) {
  if(processMode == "EDIT") {
  //Delete the node from project list to calculate size and length
  this._processDeletedProjectMedia(mediaNode);
  }
  this.mtxProjectMediaList.insertRecordNode(mediaNode, null, false);
  this.mtxProjectMediaList.repaintData();
  this._projectLength = parseInt(this._projectLength) + parseInt(mediaLength);
  this._projectSize = parseInt(this._projectSize) + parseInt(mediaSize);
  this._updateTotalAddedLabel();
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._insertProjectMediaFile = function(recordNode) {
    var strParentRecordID = null;
    if(recordNode.jsxGrpNID != "") {
      strParentRecordID = recordNode.jsxGrpNID;
      //If it is a clip then calculate project size and length
      this._projectSize = parseInt(this._projectSize) + parseInt(recordNode.jsxSize);
      this._projectLength = parseInt(this._projectLength) + parseInt(recordNode.jsxPlayLength);
    }
    recordNode.jsxselected = "0";
    this.mtxProjectMediaList.insertRecord(recordNode, strParentRecordID, false);
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._updateTotalAddedLabel = function() {
    //Convert size to GigaBytes before display
    var gigaBytes = 0;
    var pSize = this._projectSize;
    if(pSize > 0) {
      gigaBytes = Math.round((pSize / 1073741824) * 100) / 100;
    }
    this.lblTotalAdded.setText("<i>" + gigaBytes + " GB</i>", true);
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._onClick_RemoveAll = function(objEvent) {
    this.mtxProjectMediaList.clearXmlData();
    this.mtxProjectMediaList.repaintData();
    this._projectSize = 0;
    this._projectLength = 0;
    this._updateTotalAddedLabel();
    this.btnRemoveAll.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.blkNewProject.setProjectModified();
    this._LOG.info("ProjectAddFiles: Delete all media clips from project media list");
  }

  /**
  * Gets final size of the project
  * @return {String} Size
  */
  AddFiles_prototype.getProjectSize = function() {
    return this._projectSize;
  }

  /**
  * Gets final play length of the project
  * @return {String} Length
  */
  AddFiles_prototype.getProjectLength = function() {
    return this._projectLength;
  }

  //It will check with jsxName = <currName>_<dateCreated> in project list
  //If found it will append _<number> like <currName>_<dateCreated>_1 and returns otherwise it returns <currName>_<dateCreated>
  AddFiles_prototype.getNewName = function(currName, dateCreated, nodeToCheck) {
    var i = null;
    var title = currName + "_" + this._dateTimeFormatForDestTitle.format(dateCreated);
    var newTitle = title;
    var projectMediaListXML = this.mtxProjectMediaList.getXML();
    while(!jsx3.util.strEmpty(projectMediaListXML.selectSingleNode("//record[@jsxName='" + newTitle + "']"))) {
      i = i + 1;
      newTitle = title + "_" + i;
    }
    //check nodeToCheck is not null
    //Then validate name in that node also.
    if(!jsx3.util.strEmpty(nodeToCheck)) {
      while(!jsx3.util.strEmpty(nodeToCheck.selectSingleNode("//record[@jsxName='" + newTitle + "']")) || (nodeToCheck.getAttribute("jsxName") == newTitle)) {
        i = i + 1;
        newTitle = title + "_" + i;
      }
    }
    return newTitle;
  }

  //Checks any media has same title as @mediaName.
  AddFiles_prototype.validateMediaName = function(mediaName, jsxid) {
    var xPath = jsx3.util.strEmpty(jsxid) ? "//record[@jsxName='" + mediaName + "']" : "//record[@jsxName='" + mediaName + "' and @jsxid!='" + jsxid + "']";
    var projectMediaListXML = this.mtxProjectMediaList.getXML();
    if(!jsx3.util.strEmpty(projectMediaListXML.selectSingleNode(xPath))) {
      return false;
    }
    return true;
  }

  /**
  * Get the metadata/bookmarks/size from mkd file using mediaconverter
  * @param filePath {String}
  * @param infoParam {String} metadata/bookmarks/size
  * @param strCutPoints {String}/null. If not null then bookmarks/size will return for trimmed clip
  * @return BookmarksNode/Info{jsx3.xml.Entity}/Size {String)/null
  */
  AddFiles_prototype.getInfoFromMKD = function(filePath, infoParam, strCutPoints, is2dotXMedia) {
    if(!jsx3.util.strEmpty(filePath)) {
      var objFile = this._clsFSOFileSystem.getFile(filePath);
      if((is2dotXMedia == "false" && objFile.isFile()
          && (!jsx3.util.strEmpty(objFile.getExtension())
              && objFile.getExtension().toLowerCase() == "mkd")
         ) || is2dotXMedia == "true" && objFile.exists()) {
        var strArg = infoParam;
        if(!jsx3.util.strEmpty(strCutPoints)) strArg = strArg + " -C " + strCutPoints;
        var mkdFileInfo = this._processMKDFile(objFile, strArg);
        if(!jsx3.util.strEmpty(mkdFileInfo)) {
          var mkdFileInfoDoc = (new jsx3.xml.Document()).loadXML(mkdFileInfo);
          if(infoParam == "bookmarks") {
            return mkdFileInfoDoc.selectSingleNode(".//Bookmarks");
          }
          else if(infoParam == "size") {
            return mkdFileInfoDoc.selectSingleNode(".//MediaSize/Size").getValue();
          }
          else {
            return mkdFileInfoDoc;
          }
        }
        this._LOG.info("getInfoFromMKD::MKD FileInfo is null");
        return null;
      }
      this._LOG.info("getInfoFromMKD::Invalid MKD source to get Info");
      return null;
    }
    this._LOG.info("getInfoFromMKD::MKD FilePath is null");
    return null;
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._handleEditDelColumns = function(element, cdfkey, matrix, column, rownumber, server) {
    var isRoot = element.innerText;
    if(jsx3.util.strTrim(isRoot) == "false") {
      element.innerHTML = "";
    }
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._setFormatHandlersForList = function() {
    this.mtxMediaList.getDescendantOfName("mcCreatedDate", false, false).setFormatHandler("@datetime,MM/dd/yyyy HH:mm");
    this.mtxMediaList.getDescendantOfName("mcRuntime", false, false).setFormatHandler(jsx3.$F(this.formatMediaFileTime).bind(this));
    this.mtxMediaList.getDescendantOfName("mcSize", false, false).setFormatHandler(this.formatMediaFileSize);

    this.mtxProjectMediaList.getDescendantOfName("mcCreatedDate", false, false).setFormatHandler("@datetime,MM/dd/yyyy HH:mm");
    this.mtxProjectMediaList.getDescendantOfName("mcRuntime", false, false).setFormatHandler(jsx3.$F(this.formatMediaFileTime).bind(this));
    this.mtxProjectMediaList.getDescendantOfName("mcSize", false, false).setFormatHandler(this.formatMediaFileSize);
    this.mtxProjectMediaList.getDescendantOfName("mcEdit", false, false).setFormatHandler(this._handleEditDelColumns);
    this.mtxProjectMediaList.getDescendantOfName("mcDelete", false, false).setFormatHandler(this._handleEditDelColumns);
  }

  /** @private @jsxobf-clobber */
  AddFiles_prototype._validateForV2DMedia = function(addType) {
    var retVal = true;
    //In first phase we are supporting only V2D incase of transcoding
    if(this.blkNewProject.blkSetUP.selOutputFormat.getValue() != "IPVSMEDIA") {
      if(addType == "ALL") {
        var mediaListXML = this.mtxMediaList.getXML();
        var recIter = mediaListXML.selectNodeIterator(".//record");
      }
      else {
        var recIter = this.mtxMediaList.getSelectedNodes().iterator();
      }

      while(recIter.hasNext()) {
        var recNode = recIter.next();
        //check if it is a group node
        if(jsx3.util.strEmpty(recNode.getAttribute("jsxGrpNID"))) {
          if(!jsx3.util.strEmpty(recNode.selectSingleNode(".//record[@jsxStreamType != 'V2D']"))) {
            retVal = false;
            break;
          }
        }
        else {
          if(recNode.getAttribute("jsxStreamType") != "" && recNode.getAttribute("jsxStreamType") != "V2D") {
            retVal = false;
            break;
          }
        }
      }
    }
    if(!retVal) alert("Transcoding of non V2D type media is not supported");
    return retVal;
  }

//  validateProjectMediaForNonV2D will check for non V2D type media in project media list
//  In this phase we are supporting transcoding of V2D media only
//  So when user tries to change output format during project setup this will be called to check for non V2D media
  AddFiles_prototype.validateProjectMediaForNonV2D = function() {
    var projectMediaListXML = this.mtxProjectMediaList.getXML();
    if(!jsx3.util.strEmpty(projectMediaListXML.selectSingleNode("//record[@jsxStreamType != '' and @jsxStreamType != 'V2D']"))) {
      alert("This project has non V2D type media. Transcoding of non V2D type media is not supported.\r" +
            "If you want to change output format first delete the non V2D type media from project media list then change output format");
      return false;
    }
    return true;
  }
});