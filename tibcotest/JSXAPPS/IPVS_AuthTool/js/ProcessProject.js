/* place JavaScript code here */
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.ProcessProject", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(ProcessProject, ProcessProject_prototype) {
  ProcessProject_prototype._LOG = jsx3.util.Logger.getLogger(ProcessProject.jsxclass.getName());

  var MainMenu = com.ipvs.gui.authtool.MainMenu;
  ProcessProject_prototype.init = function() {
    this._ViewState = false;
    this._wsh = null;
    this._fso = null;
    this._inProcess = false;
    this._projectSize = 0;
    this._IPVSAuthToolPath = null;
    this._projectName = null;
    this._autorunfile = null;
    this._mediaConverterFilePath = null;
    this._dvdFolderPath = null;
    this._logo = null;
    this._mediaFolder = null;
    this._blobFolder = null;
    this._imagesFolder = null;
    this._processedFilesSize = 0;
    this._strTranscodeSettings = null;
    this.mtxProjectFilesList = this.getDescendantOfName("mtxProjectFilesList");
    this.btnCreateProjectOutput = this.getDescendantOfName("btnCreateProjectOutput");
    this.blkProcessStatus = this.getDescendantOfName("blkProcessStatus");
    this.btnCancel = this.getDescendantOfName("btnCancel");
    this.progressBar = this.getDescendantOfName("blkProgressBar");
    this.lblPrgPercent = this.getDescendantOfName("lblPrgPercent");
    this.blkProgressBar = this._initProgressBar();
    this.blkProcessComplete = this.getDescendantOfName("blkProcessComplete");
    this.lblTotalProcessed = this.getDescendantOfName("lblTotalProcessed");
    this.btnOutputdir = this.getDescendantOfName("btnOutputdir");
    this.btnCreateProjectOutput.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._createProjectOutput);
    this.btnCancel.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._stopProjectProcessing);
    this.btnOutputdir.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._viewOutputDir);
    this.blkNewProject = this.getAncestorOfType("com.ipvs.gui.authtool.NewProject");
    this.blkNewProject.subscribe(com.ipvs.gui.authtool.NewProject.MENU_SELECT, this, this._toggle);
    this.blkApp = this.getAncestorOfType("com.ipvs.authtool.App");
    this.blkApp.subscribe(com.ipvs.authtool.App.PROJECT_PROCESSCANCEL, this, this._cancleProjectProcessing);
    this.blkApp.subscribe(com.ipvs.authtool.App.WINDOW_DESTROY, this, this._destroy);
  }

  ProcessProject_prototype._toggle = function(objEvent) {
    if(this.blkNewProject.getMenuSelected() == null) {
      this._ViewState = false;
    }
    else if(this.blkNewProject.getMenuSelected() == com.ipvs.gui.authtool.NewProject.MENU_PROCESSPROJECT) {
      this.cmpShow();
      if(!this._ViewState) {
        this._initFS();
      }
      this._loadMediaList();
    }
    else {
      this.cmpHide();
    }
  }

  ProcessProject_prototype._loadMediaList = function() {
    this._inProcess = false;
    this._windowClose = true;
    // Project Details
    //this._authFolderPath =clsDefaultSettings.getGIAppPath().replace(/\//g, "\\");
    this._IPVSAuthToolPath = clsDefaultSettings._getGIStartPath().substring(0, clsDefaultSettings._getGIStartPath().length - 3);
    //this._projectsFolderPath = this._authFolderPath + "\\Projects";
    this._projectName = this.blkNewProject.projectName;
    //this._projectXMLFilePath = this._projectsFolderPath + "\\" + this._projectName +".xml";
    this._autorunfile = clsDefaultSettings.getGIAppPath().replace(/\//g, "\\") + "\\utils\\";
    this._mediaConverterFilePath = clsDefaultSettings.getGIAppPath().replace(/\//g, "\\") + "\\utils\\mediaconverter.exe";

    //Load Matrix Data for selcted Project
    this.mtxProjectFilesList.clearXmlData();
    this.mtxProjectFilesList.setXMLTransformers("xsl/MediaList.xsl");
    this.mtxProjectFilesList.setSourceXML(this.blkNewProject.getProjectMediaListNode());
    this.mtxProjectFilesList.repaintData();
    this._ViewState = true;

    //Set the Output Folder Path
    this._projectSize = this.blkNewProject.getProjectSize();
    if(this.blkNewProject.getOutputType() == "IPVSMEDIA") {
      this._logo = this.blkNewProject.getMediaGuideLogo();
    }
    this._dvdFolderPath = this.blkNewProject.getOutpurDir() + "\\" + this._projectName;
    this.btnCreateProjectOutput.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.blkProcessStatus.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkProcessComplete.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this._loadProgressBar();
  }

  ProcessProject_prototype._initFS = function() {
    this._fso = new ActiveXObject('Scripting.FileSystemObject');
  }

  ProcessProject_prototype._createProjectOutput = function(objEvent) {
    if(!this._checkSpaceonDisk()) return;
    this._processedFilesSize = 0;
    this._loadProgressBar();
    this._wsh = new ActiveXObject('WScript.Shell');
    var bProjectSaved = this.blkNewProject.saveProject();
    this.blkApp.setAddEditProject(objEvent, this._projectName);
    //Project Path string cleaned up
    var slashExists = this._dvdFolderPath.charAt(this._dvdFolderPath.length - 1);
    if((slashExists == "/") || (slashExists == "\\"))
      this._dvdFolderPath = this._dvdFolderPath.substring(0, this._dvdFolderPath.length - 1);
    var folderBool = this._fso.FolderExists(this._dvdFolderPath);
    if(!folderBool) {
      // If Folder doesnot exsist create a new folder
      try {
        this._fso.CreateFolder(this._dvdFolderPath);
      }
      catch(er) {
        alert("Output directory path is not valid");
        return;
      }
    }
    else {
      // Overwriting if Folder Aready Exsist.
      try {
        this._fso.DeleteFolder(this._dvdFolderPath, true);
      }
      catch(er) {
        alert("Check Permissions to overwrite existing project");
        return;
      }
      this._fso.CreateFolder(this._dvdFolderPath);
    }
    this._processProject();
  }

  ProcessProject_prototype._checkSpaceonDisk = function() {
    var driveName = this._fso.GetDrive(this._fso.GetDriveName(this._fso.GetAbsolutePathName(this._dvdFolderPath)));
    //Space required for IPVSAuthMediaClient folder, index.html, index.hta, IPVSAutoRun.bat and other files
    var spaceForMediaClient = "15000000";
    var totalMediaSize = parseInt(spaceForMediaClient) + parseInt(this._projectSize);
    if(driveName.FreeSpace < totalMediaSize) {
      alert("Disk space is not sufficient to create project:" + "Space required for project:" + totalMediaSize + " bytes Available free space:" + driveName.FreeSpace + " bytes");
      return false;
    }
    return true;
  }

  ProcessProject_prototype._processProject = function() {
    this.btnCreateProjectOutput.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkProcessComplete.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkProcessStatus.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this._inProcess = true;
    this.blkNewProject.setProjectInProcess(this._inProcess);
    //IF output type is 'IPVSMEDIA' then create Media folder and Blob Folder in target Directory i.e DVD Folder
    var outputType = this.blkNewProject.getOutputType();
    if(outputType == "IPVSMEDIA") {
      this._mediaFolder = this._fso.CreateFolder(this._dvdFolderPath + "\\Media");
      this._blobFolder = this._fso.CreateFolder(this._dvdFolderPath + "\\Media" + "\\blob");
      this._imagesFolder = this._fso.CreateFolder(this._dvdFolderPath + "\\Media" + "\\images");
      this._triggerProgressBar(this._blobFolder);
    }
    else {
      this._strTranscodeSettings = this._getTranscodeCmd(outputType);
      this._triggerProgressBar(this._dvdFolderPath);
    }

    // Parse the xml file in projects to Process MKD Files 
    var projectMediaListNode = this.blkNewProject.getProjectMediaListNode();
    var mediaList = projectMediaListNode.selectNodeIterator("//MEDIALIST/MEDIA");
    while(mediaList.hasNext()) {
      if(this._inProcess == false) return;
      var mediaNode = mediaList.next();
      var mediaID = mediaNode.getAttribute("ID");
      this.mtxProjectFilesList.insertRecordProperty(mediaID, "jsxtextprocessingstatus", "In Progress", true);
      var clipList = mediaNode.selectNodeIterator(".//CLIP");
      while(clipList.hasNext()) {
        var clipNode = clipList.next();
        if(clipNode.getAttribute("isActive") != "false") {
          this._processClip(clipNode, outputType);
        }
      }
      if(this._inProcess == false)
        this.mtxProjectFilesList.insertRecordProperty(mediaID, "jsxtextprocessingstatus", "Queued", true);
      else
        this.mtxProjectFilesList.insertRecordProperty(mediaID, "jsxtextprocessingstatus", "Completed", true);
    }
    var clipListNode = projectMediaListNode.selectNodeIterator("//MEDIALIST/CLIP");
    while(clipListNode.hasNext()) {
      if(this._inProcess == false) return;
      var clipNode = clipListNode.next();
      if(!jsx3.util.strEmpty(clipNode)) {
        if(clipNode.getAttribute("isActive") != "false") {
          this._processClip(clipNode, outputType);
        }
      }
    }
    clearInterval(this.interval);
    if(outputType == "IPVSMEDIA") {
      this._copyFilesToProject();
    }
    if(this._inProcess == false) return;
    this._inProcess = "Processed";
    this.blkNewProject.setProjectInProcess(this._inProcess);
    this._inProcess = false;
    this._updateTotalProcessedLabel();
    this._windowClose = true;
    this.btnCreateProjectOutput.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkProcessStatus.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkProcessComplete.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
  }

  ProcessProject_prototype._getTranscodeCmd = function(transcodeType) {
    //  [ -i Input_File<absolute path>  -o Output_Dir<absolute path>  -m Media_Name 
    //    -f Format(MP4, AVI or TS) -fd Ffmpeg_Dir<absolute path> -td Temp_Dir<absolute path> 
    //    -vc Video_Codec_Name<mpeg2, mp4, or h264>  -vb Video_Bitrate<in bps>  -vq Video_Quality<2-31> 
    //    -s Video_Resolution<WidthxHeight> -fps Video_Framerate<Fps> -ac Audio_Codec_Name<mp2 etc> -ab Audio_Bitrate<Inbps> ]

    var objSettingsNode = this.blkNewProject.getProjectTranscodingMediaSettings();
    var utilsDirPath = clsDefaultSettings.getGIAppPath().replace(/\//g, "\\") + "\\utils";
    var tempDirPath = clsDefaultSettings.getProjectsPath() + "\\temp";
    var cmd = "cscript \"" + utilsDirPath + "\\Transcode.vbs\" -fd \"" + utilsDirPath + "\" -td \"" + tempDirPath + "\" -o \"" + this._dvdFolderPath + "\"";
    if(transcodeType == "TRANSCODEDMEDIA") {
      cmd = cmd + " -f " + objSettingsNode.FORMATTYPE;
      cmd = cmd + " -vc " + objSettingsNode.VIDEOTYPE;
      if(objSettingsNode.VIDEOMODE == "CQ") {
        cmd = cmd + " -vq " + objSettingsNode.VIDEOQUANT;
      }
      else {
        cmd = cmd + " -vb " + objSettingsNode.VIDEOBITRATE;
      }
      cmd = cmd + " -s " + objSettingsNode.VIDEORESOLUTION;
      cmd = cmd + " -fps " + objSettingsNode.VIDEOFRAMERATE;
      cmd = cmd + " -ac " + objSettingsNode.AUDIOTYPE;
      cmd = cmd + " -ab " + objSettingsNode.AUDIOBITRATE;
      cmd = cmd + " -tr true";
    }
    else {
      cmd = cmd + " -tr false";
    }
    return cmd;
  }

  ProcessProject_prototype._processClip = function(clipNode, outPutType) {
    if(this._inProcess == false) return;
    var basepath = clipNode.selectSingleNode(".//BASEPATH").getValue();
    var srcFName = clipNode.selectSingleNode(".//FILENAME").getValue();
    var dstFName = clipNode.getAttribute("ID");

    //set processing status in matrix
    var clipRecord = this.mtxProjectFilesList.getRecordNode(dstFName);
    if(this._inProcess == false)
      clipRecord.setAttribute("jsxtextprocessingstatus", "Queued");
    else
      clipRecord.setAttribute("jsxtextprocessingstatus", "In Progress");
    this.mtxProjectFilesList.redrawRecord(dstFName, jsx3.xml.CDF.UPDATE);
    var isTwoDotX = clipNode.selectSingleNode(".//ISTWODOTXMEDIA").getValue();
    if((isTwoDotX == "false" && !this._fso.FileExists(basepath + "\\" + srcFName + ".mkd")) || (isTwoDotX == "true" && !this._fso.FolderExists(basepath + "\\blob\\" + srcFName))) {
      this._LOG.info("_processClip:Source is not available at " + basepath);
      return false;
    }

    var strCutPoints = "";
    var segmentIter = clipNode.cloneNode(true).selectNodeIterator("//SEGMENTLIST/SEGMENT");
    while(segmentIter.hasNext()) {
      segment = segmentIter.next();
      strCutPoints = strCutPoints + "," + segment.getAttribute("inPoint") + ":" + segment.getAttribute("outPoint");
    }
    if(!jsx3.util.strEmpty(strCutPoints)) {
      strCutPoints = strCutPoints.replace(/^,*(.*)/, '$1');
    }

    if(isTwoDotX == "false") {
      var srcPath = basepath + "\\" + srcFName + ".mkd";
    }
    else {
      var srcPath = basepath + "\\blob\\" + srcFName;
    }

    if(outPutType == "IPVSMEDIA") {
      //create destination dir in dest blob folder for this clip
      var dstBlobDirForClip = this._fso.CreateFolder(this._blobFolder + "\\" + dstFName);
      var processMediaCMD = "\"" + this._mediaConverterFilePath + "\"" + " -x" + " -f " + "\"" + srcPath + "\" -b " + "\"" + dstBlobDirForClip + "\"";
    }
    else {
      var mediaName = clipNode.selectSingleNode(".//TITLE").getValue();
      var processMediaCMD = this._strTranscodeSettings + " -i \"" + srcPath + "\" -m " + mediaName;
    }

    if(!jsx3.util.strEmpty(strCutPoints)) {
      processMediaCMD = processMediaCMD + " -C " + strCutPoints;
    }
    if(this._wsh) {
      this._LOG.info("_processClip:CMD::" + processMediaCMD);
      var proc = this._wsh.Run(processMediaCMD, 0, true);
      if(proc == 0) {
        //0 for success
        this._LOG.info(" [1] _processClip >>>>> status:" + proc);
        if(this._inProcess == false)
          clipRecord.setAttribute("jsxtextprocessingstatus", "Queued");
        else {
          clipRecord.setAttribute("jsxtextprocessingstatus", "Completed");
        }
        this.mtxProjectFilesList.redrawRecord(dstFName, jsx3.xml.CDF.UPDATE);
      }
      else if(proc == 1) {
        this._LOG.info(" [2] _processClip >>>>> status:" + proc);
        if(this._windowClose == true) {
          var killCmd = "taskkill /PID " + HTAPid + " /T /F";
          //from HTA closing window
          this._LOG.info("_processClip:Closing HTA window, kill task:" + killCmd);
          var proc = this._wsh.Run(killCmd, 0, false);
        }
        else {
          this._LOG.info("_processClip:Stopped the processing using Stop or Cancel:");
          this._windowClose = true;
        }
      }
      else {
        this._LOG.info(" [3] _processClip >>>> Error while extracting MKD file proc:" + proc);
        return false;
      }
    }
  }

  ProcessProject_prototype._copyFilesToProject = function() {
    if(this._inProcess == false) return;
    this._createAutorunInfoFile();
    this._createMediaListFile();

    if(this._inProcess == false) return;
    this._fso.CopyFolder(this._IPVSAuthToolPath + "\\IPVSAuthMediaClient", this._dvdFolderPath + "\\IPVSAuthMediaClient", true);
    this._fso.CopyFile(this._autorunfile + "\\IPVSAutoRun.bat", this._dvdFolderPath + "\\IPVSAutoRun.bat", true);
    this._fso.CopyFile(this._IPVSAuthToolPath + "\\IPVSAuthMediaClient\\GI\\JSXAPPS\\IPVS_AuthMediaClient\\*.lnk", this._dvdFolderPath + "\\", true);
  }

  ProcessProject_prototype._stopProjectProcessing = function(objEvent) {
    if(this._inProcess == true) {
      var confirmation = confirm("Are you sure you want to cancel processing?");
      if(!confirmation) {
        this._inProcess == true;
        return;
      }
      else {
        this._cancleProjectProcessing(objEvent);
      }
    }
  }

  ProcessProject_prototype._cancleProjectProcessing = function(objEvent) {
    if(this._inProcess == true) {
      this._inProcess = false;
      this._windowClose = false;
      var folderBool = this._fso.FolderExists(this._dvdFolderPath);;
      if(folderBool) {
        if(this._wsh) {
          var killCmd = "taskkill /f /im mediaconverter.exe";
          var proc = this._wsh.Run(killCmd, 0, false);
          if(proc != 0 && proc != 128) {
            //Not sucessful or process going on
            this._LOG.info("ProcessProject: CancelProject cmd Failed");
            this._inProcess = true;
            this.blkNewProject.setProjectInProcess(this._inProcess);
            return;
          }
          else {
            //Kill is sucessful or no process existed to kill
            alert("Processing of project " + this._projectName + " has been cancelled");
          }
        }
      }
      var recordList = this.mtxProjectFilesList.getRecordIds();
      for(var i = 0;i < recordList.length;i++) {
        var record = this.mtxProjectFilesList.getRecordNode(recordList[i]);
        record.setAttribute("jsxtextprocessingstatus", "Queued");
      }
      this.mtxProjectFilesList.repaint();
    }
    this.blkNewProject.setProjectInProcess(this._inProcess);
    this.btnCreateProjectOutput.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    this.blkProcessStatus.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this._loadProgressBar();
    this.blkProcessComplete.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  ProcessProject_prototype._viewOutputDir = function(objEvent) {
    if(this._wsh) {
      var explorerCmd = "explorer " + this._dvdFolderPath;
      var proc = this._wsh.Run(explorerCmd, 1, false);
    }
  }

  /** @private @jsxobf-clobber */
  ProcessProject_prototype._updateTotalProcessedLabel = function() {
    //Convert size to GigaBytes before display
    var gigaBytes = 0;
    var pSize = this._projectSize;
    if(pSize > 0) {
      gigaBytes = Math.round((pSize / 1073741824) * 100) / 100;
    }
    this.lblTotalProcessed.setText("<i>Output Completed.  &nbsp;&nbsp; Total Files processed: &nbsp;" + gigaBytes + "GB</i>", true);
  }

  ProcessProject_prototype._createMediaListFile = function() {
    if(this._inProcess == false) return;
    var xmlDoc = new jsx3.xml.Document().loadXML(this.blkNewProject.getProjectXML().toString());
    var projectNode = xmlDoc.selectSingleNode("//PROJECT");
    var logoPath = projectNode.selectSingleNode(".//LOGOPATH").getValue();
    if(!jsx3.util.strEmpty(logoPath) && this._fso.FileExists(logoPath)) {
      var logoImage = logoPath.substring(logoPath.lastIndexOf("\\") + 1, logoPath.length);
      var dvdLogoPath = "Media\\images\\" + logoImage;
      projectNode.selectSingleNode(".//LOGOPATH").setValue(dvdLogoPath);
      this._fso.CopyFile(logoPath, this._dvdFolderPath + "\\Media" + "\\images\\" + logoImage, true);
    }
    else {
      projectNode.selectSingleNode(".//LOGOPATH").setValue("");
    }
    try {
      var file = this._fso.CreateTextFile(this._dvdFolderPath + "\\Media" + "\\Medialist.xml", true);
    }
    catch(er) {
      alert("Set permissions to create a file");
      return;
    }
    file.WriteLine(xmlDoc.toString());
    file.close();
  }

  ProcessProject_prototype._createAutorunInfoFile = function() {
    if(this._inProcess == false) return;
    var autorunStr = "[Autorun]\nicon=ipvslogo.bmp\nlabel=" + this._logo + "\nopen=IPVSAutoRun.bat\n";
    try {
      var file = this._fso.CreateTextFile(this._dvdFolderPath + "\\autorun.inf", true);
    }
    catch(er) {
      alert("Set permissions to create a file");
      return;
    }
    file.WriteLine(autorunStr);
    file.close();
  }

  ProcessProject_prototype._destroy = function() {
    this._windowClose = true;
    if(this._wsh) {
      var killCmd = "taskkill /f /im mediaconverter.exe";
      var proc = this._wsh.Run(killCmd, 0, false);
      if(proc != 0 && proc != 128) {
        this._LOG.info("Problems destroying Process Project Window proc:" + proc);
      }
      else {
        this._LOG.info("Process Project Window Destroyed sucessfully proc:" + proc);
      }
    }
    this.mtxProjectFilesList = null;
    this.btnCreateProjectOutput = null;
    this.btnCancel = null;
    this.blkProcessStatus = null;
    this.lblTotalProcessed = null;
    this.btnOutputdir = null;
    this.blkProcessComplete = null;
  }

  ProcessProject_prototype._initProgressBar = function() {
    var progressBar = new Array();
    for(i = 0;i < 10;i++) {
      var blkName = "blkPrg" + i;
      progressBar[i] = this.getDescendantOfName(blkName);
    }
    this.lblPrgPercent.setText("<i>1%</i>", true);
    return progressBar;
  }

  ProcessProject_prototype._loadProgressBar = function() {
    for(i = 0;i < this.blkProgressBar.length;i++) {
      var blk = this.blkProgressBar[i];
      blk.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }
    this.lblPrgPercent.setText("<i>1%</i>", true);
    this.progressBar.repaint();
  }

  ProcessProject_prototype._updateProgressBar = function(folderSize) {
    this._processedFilesSize = folderSize;
    var percentageProcessed = parseInt((this._processedFilesSize / this._projectSize) * 100);
    var updateBlk = parseInt(percentageProcessed / 10);
    if(updateBlk > 0 && percentageProcessed <= 100) {
      for(i = 0;i < updateBlk;i++) {
        var blk = this.blkProgressBar[i];
        if(blk)
          blk.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }
      this.lblPrgPercent.setText("<i>" + percentageProcessed + "% </i>", true);
    }
  }

  ProcessProject_prototype._triggerProgressBar = function(fldPath) {
    var ThisClass = this;
    this.interval = setInterval(function() {
      var folder = ThisClass._fso.getFolder(fldPath);
      var folderSize = folder.size;
      ThisClass._LOG.warn(folderSize);
      ThisClass._updateProgressBar(folderSize);
    }, 1000);
  }

});