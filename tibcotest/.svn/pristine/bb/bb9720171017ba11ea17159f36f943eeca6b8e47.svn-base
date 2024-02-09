/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* ProjectSetup class provides UI to setup  a project while Create/Edit.
* This is first step of NewProject
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.authtool.ProjectSetup", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(ProjectSetup, ProjectSetup_prototype) {

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._LOG = jsx3.util.Logger.getLogger(ProjectSetup.jsxclass.getName());

  /**
  * The instance initializer.
  */
  ProjectSetup_prototype.init = function() {
    //Get handle of child components and subscribe as required
    this._ViewState = false;
    this.txtProjectName = this.getDescendantOfName("txtProjectName");
    this.clsOutputDir = this.getDescendantOfName("blkOutputDir").getDescendantOfName("blkDirPicker");
    this.selOutputFormat = this.getDescendantOfName("selOutputFormat");
    this.selOutputFormat.resetCacheData();
    this.selOutputFormat.setXmlAsync(false);
    this.selOutputFormat.setXMLURL("xml/default/selection/outputFormat.xml");
    this.selOutputFormat.subscribe(jsx3.gui.Interactive.SELECT, this, this._onSetOutputFormat);

    this.selOutputFormat.removeEvent(jsx3.gui.Interactive.BEFORE_SELECT);
    this.selOutputFormat.setEvent("this.onBeforeSelect(strRECORDID);", jsx3.gui.Interactive.BEFORE_SELECT);
    // the above event will fire in the context of the blk - so we setup its onBeforeSelect to point to ours
    this.selOutputFormat.onBeforeSelect = jsx3.$F(this._onBeforeSetOutputFormat).bind(this);
    //this.selOutputFormat.subscribe(jsx3.gui.Interactive.BEFORE_SELECT, this, this._onBeforeSetOutputFormat);

    //Setup for IPVSMedia DVD
    this.blkIpvsOutput = this.getDescendantOfName("blkIpvsOutput");
    this.txtTitle = this.getDescendantOfName("txtMediaGuideTitle");
    this.clsFilePathLogo = this.getDescendantOfName("blkFilePathLogo").getDescendantOfName("filePicker");
    this.clsFilePathLogo.subscribe(com.ipvs.gui.FilePicker.ON_FILE_SELECT, this, this._checkFileType);

    //Setup for TranscodedMedia
    this.blkTranscodedOutput = this.getDescendantOfName("blkTranscodedOutput");
    this.selOutputPreset = this.getDescendantOfName("selOutputPreset");
    this.selOutputPreset.resetCacheData();
    this.selOutputPreset.setXmlAsync(false);
    this.selOutputPreset.setXMLURL("xml/default/selection/outputPreset.xml");
    this.selOutputPreset.subscribe(jsx3.gui.Interactive.SELECT, this, this._onSetOutputPreset);
    //Video settings
    this.selVideo = this.getDescendantOfName("selVideo");
    this.selVideo.setXmlAsync(false);
    this.selVideo.setXMLURL("xml/default/selection/video.xml");
    this.selFileFormat = this.getDescendantOfName("selFileFormat");
    this.selFileFormat.setXmlAsync(false);
    this.selFileFormat.setXMLURL("xml/default/selection/fileFormat.xml");
    this.selFrameRate = this.getDescendantOfName("selFrameRate");
    this.selFrameRate.setXmlAsync(false);
    this.selFrameRate.setXMLURL("xml/default/selection/frameRate.xml");
    this.radioVideoQuality = this.getDescendantOfName("radioVideoQuality");
    this.sliderVideoQuality = this.getDescendantOfName("sliderVideoQuality");
    this.radioVideoBitrate = this.getDescendantOfName("radioVideoBitrate");
    this.selVideoBitrate = this.getDescendantOfName("selVideoBitrate");
    this.selVideoBitrate.setXmlAsync(false);
    this.selVideoBitrate.setXMLURL("xml/default/selection/videoBitrate.xml");

    //Audio settings
    this.selAudio = this.getDescendantOfName("selAudio");
    this.selAudio.setXmlAsync(false);
    this.selAudio.setXMLURL("xml/default/selection/audio.xml");
    this.selAudioBitrate = this.getDescendantOfName("selAudioBitrate");
    this.selAudioBitrate.setXmlAsync(false);
    this.selAudioBitrate.setXMLURL("xml/default/selection/audioBitrate.xml");
    this.selResolution = this.getDescendantOfName("selResolution");
    this.selResolution.setXmlAsync(false);
    this.selResolution.setXMLURL("xml/default/selection/resolution.xml");

    //Setup for TranscodedMedia
    this.blkAVSOutput = this.getDescendantOfName("blkAVSOutput");

    //get the handle of Ancestor(NewProject) class
    this.blkNewProject = this.getAncestorOfType("com.ipvs.gui.authtool.NewProject");
    this.blkNewProject.subscribe(com.ipvs.gui.authtool.NewProject.MENU_SELECT, this, this._toggle);
    this._LOG.info("ProjectSetup: Initiated");
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._toggle = function(objEvent) {
    if(this.blkNewProject.getMenuSelected() == null) {
      this._ViewState = false;
    }
    else if(this.blkNewProject.getMenuSelected() == com.ipvs.gui.authtool.NewProject.MENU_SETUP) {
      this.cmpShow();
      this._LOG.info("ProjectSetup: Show");
      if(!this._ViewState) {
        this._initLoad();
        this._clsFSOFileSystem = new jsx3.io.FSOFileSystem();
      }
    }
    else {
      this.cmpHide();
      this._LOG.info("ProjectSetup: Hide");
    }
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._initLoad = function() {
    var pName = this.blkNewProject.getProjectName();
    this.txtProjectName.setValue(pName);
    if(pName != null) {
      this.txtProjectName.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    else {
      this.txtProjectName.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this.clsOutputDir.setFolderPath(this.blkNewProject.getOutpurDir());
    this._setProjectOutputFormat(this.blkNewProject.getOutputType());
    this._setIPVSMediaSettings();
    this._setTranscodingMediaSettings();
    this._ViewState = true;
    this._LOG.info("ProjectSetup: Loaded");
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._setProjectOutputFormat = function(outputFormat) {
    this.selOutputFormat.setValue(outputFormat);
    this._onSetOutputFormat();
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._setIPVSMediaSettings = function() {
    this.txtTitle.setValue(this.blkNewProject.getMediaGuideTitle());
    this.clsFilePathLogo.setFilePath(this.blkNewProject.getMediaGuideLogo());
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._setTranscodingMediaSettings = function() {
    var objTranscodeMediaSettings = this.blkNewProject.getProjectTranscodingMediaSettings();
    if(!jsx3.util.strEmpty(objTranscodeMediaSettings)) {
      this.selOutputPreset.setValue(objTranscodeMediaSettings.OUTPUTPRESET);
      this.selVideo.setValue(objTranscodeMediaSettings.VIDEOTYPE);
      this.selFileFormat.setValue(objTranscodeMediaSettings.FORMATTYPE);
      if(objTranscodeMediaSettings.VIDEOMODE == "CQ") {
        this.radioVideoQuality.setSelected(jsx3.gui.RadioButton.SELECTED);
        this._setVideoQuality(objTranscodeMediaSettings.VIDEOQUANT);
      }
      else {
        this.radioVideoBitrate.setSelected(jsx3.gui.RadioButton.SELECTED);
        this.selVideoBitrate.setValue(objTranscodeMediaSettings.VIDEOBITRATE);
        this._setVideoQuality(0);
      }
      this.selFrameRate.setValue(objTranscodeMediaSettings.VIDEOFRAMERATE);
      this.selResolution.setValue(objTranscodeMediaSettings.VIDEORESOLUTION);
      this.selAudio.setValue(objTranscodeMediaSettings.AUDIOTYPE);
      this.selAudioBitrate.setValue(objTranscodeMediaSettings.AUDIOBITRATE);
    }
    else {
      //set default preset as DVD Quality
      this.selOutputPreset.setValue("PRESET1");
    }
    this._onSetOutputPreset();
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._onBeforeSetOutputFormat = function(stRecId) {
    if(stRecId != "IPVSMEDIA") {
      if(!this.blkNewProject.blkAddFiles.validateProjectMediaForNonV2D()) {
        return false;
      }
    }
    return true;
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._onSetOutputFormat = function(objEvent) {
    var outputFormat = this.selOutputFormat.getValue();
    this.blkIpvsOutput.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkTranscodedOutput.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    this.blkAVSOutput.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    if(outputFormat == "TRANSCODEDMEDIA") {
      this.blkTranscodedOutput.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
    else if(outputFormat == "AVSMEDIA") {
      this.blkAVSOutput.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
    else {
      this.blkIpvsOutput.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._onSetOutputPreset = function(objEvent) {
    var ourputPreset = this.selOutputPreset.getValue();
    var bSettingsEnable = jsx3.gui.Form.STATEENABLED;
    if(ourputPreset != "CUSTOM") {
      bSettingsEnable = jsx3.gui.Form.STATEDISABLED;
      var presetRecord = this.selOutputPreset.getRecordNode(ourputPreset);
      if(!jsx3.util.strEmpty(presetRecord)) {
        var presetNode = presetRecord.selectSingleNode(".//" + ourputPreset);
        //set preset values
        this.selVideo.setValue(presetNode.getAttribute("video"));
        this.selFileFormat.setValue(presetNode.getAttribute("format"));
        this.selFrameRate.setValue(presetNode.getAttribute("framerate"));
        var isCQ = presetNode.getAttribute("cq") ? 1 : 0
        var isCBR = presetNode.getAttribute("cbr") ? 1 : 0
        if(isCQ) {
          this.radioVideoQuality.setSelected(jsx3.gui.RadioButton.SELECTED);
          this._setVideoQuality(presetNode.getAttribute("cq"));
        }
        else if(isCBR) {
          this.radioVideoBitrate.setSelected(jsx3.gui.RadioButton.SELECTED);
          this.selVideoBitrate.setValue(presetNode.getAttribute("cbr"));
          this._setVideoQuality(0);
        }
        this.selAudio.setValue(presetNode.getAttribute("audio"));
        this.selAudioBitrate.setValue(presetNode.getAttribute("audiobitrate"));
        this.selResolution.setValue("native");
      }
    }

    this.selVideo.setEnabled(bSettingsEnable, true);
    this.selFileFormat.setEnabled(bSettingsEnable, true);
    this.selFrameRate.setEnabled(bSettingsEnable, true);
    this.radioVideoQuality.setEnabled(bSettingsEnable, true);
    this.sliderVideoQuality.setEnabled(bSettingsEnable, true);
    this.radioVideoBitrate.setEnabled(bSettingsEnable, true);
    this.selVideoBitrate.setEnabled(bSettingsEnable, true);
    this.selAudio.setEnabled(bSettingsEnable, true);
    this.selAudioBitrate.setEnabled(bSettingsEnable, true);
    this.selResolution.setEnabled(bSettingsEnable, true);
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._setVideoQuality = function(videoQuality) {
    var sliderPosition = ((31 - videoQuality) * 100) / 30
    this.sliderVideoQuality.setValue(sliderPosition);
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._getVideoQuality = function() {
    var sliderPostion = parseInt(this.sliderVideoQuality.getValue());
    var valQuality = parseInt(31 - ((30 * sliderPostion) / 100));
    this._LOG.info("valQuality : " + valQuality);
    return valQuality;
  }

  /**
  * Validates TranscodedMediaSettings.
  * @return Boolean
  */
  ProjectSetup_prototype.validateTranscodingMediaSettings = function() {
    if(this.selOutputFormat.getValue() == "TRANSCODEDMEDIA" && this.selOutputPreset.getValue() == "CUSTOM") {
      if(jsx3.util.strEmpty(this.selVideo.getValue())) {
        alert("Select Video");
        return false;
      }

      if(jsx3.util.strEmpty(this.selFileFormat.getValue())) {
        alert("Select file format");
        return false;
      }

      if(jsx3.util.strEmpty(this.selFrameRate.getValue())) {
        alert("Select frame rate");
        return false;
      }

      if(this.radioVideoQuality.getSelected() == jsx3.gui.RadioButton.UNSELECTED && this.radioVideoBitrate.getSelected() == jsx3.gui.RadioButton.UNSELECTED) {
        alert("Select video quality or video bitrate");
        return false;
      }

      if(jsx3.util.strEmpty(this.selAudio.getValue())) {
        alert("Select audio");
        return false;
      }

      if(jsx3.util.strEmpty(this.selAudioBitrate.getValue())) {
        alert("Select audio bitrate");
        return false;
      }
    }
    return true
  }

  /**
  * Reads values from gui and return IPVSMediaSettings.
  * @return object  {IPVSMediaSettings}
  */
  ProjectSetup_prototype.getIPVSMediaSettings = function() {
    var objIPVSMediaSettings = new Object;
    objIPVSMediaSettings.LOGOPATH = this.clsFilePathLogo.getFilePath();
    objIPVSMediaSettings.LOGOTITLE = this.txtTitle.getValue();
    return objIPVSMediaSettings;
  }

  /**
  * Reads values from gui and return TranscodingMediaSettings.
  * @return object {TranscodingMediaSettings}
  */
  ProjectSetup_prototype.getTranscodingMediaSettings = function() {
    var objTranscodeMediaSettings = new Object;
    objTranscodeMediaSettings.OUTPUTPRESET = this.selOutputPreset.getValue();
    objTranscodeMediaSettings.VIDEOTYPE = this.selVideo.getValue();
    objTranscodeMediaSettings.FORMATTYPE = this.selFileFormat.getValue();
    if(this.radioVideoQuality.getSelected() == jsx3.gui.RadioButton.SELECTED) {
      objTranscodeMediaSettings.VIDEOMODE = "CQ";
      objTranscodeMediaSettings.VIDEOQUANT = this._getVideoQuality();
      objTranscodeMediaSettings.VIDEOBITRATE = "";
    }
    else {
      objTranscodeMediaSettings.VIDEOMODE = "CBR";
      objTranscodeMediaSettings.VIDEOQUANT = "";
      objTranscodeMediaSettings.VIDEOBITRATE = this.selVideoBitrate.getValue();
    }
    objTranscodeMediaSettings.VIDEOFRAMERATE = this.selFrameRate.getValue();
    objTranscodeMediaSettings.VIDEORESOLUTION = this.selResolution.getValue();
    objTranscodeMediaSettings.AUDIOTYPE = this.selAudio.getValue();
    objTranscodeMediaSettings.AUDIOBITRATE = this.selAudioBitrate.getValue();
    return objTranscodeMediaSettings;
  }

  ProjectSetup_prototype.destroy = function() {
    //this.btnMain = null;
  }

  /** @private @jsxobf-clobber */
  ProjectSetup_prototype._checkFileType = function() {
    var filePath = this.clsFilePathLogo.getFilePath();
    var file = this._clsFSOFileSystem.getFile(filePath);
    this._LOG.info("ProjectSetup_checkFileType:Extension=" + file.getExtension().toLowerCase());
    if(file.isFile() && (file.getExtension() != null && file.getExtension().toLowerCase() != "png" && file.getExtension().toLowerCase() != "jpg" && file.getExtension().toLowerCase() != "jpeg" && file.getExtension().toLowerCase() != "gif")) {
      alert("Select valid image type(JPEG/PNG/GIF) for Logo");
      this.clsFilePathLogo.setFilePath("");
      return false;
    }
    return true;
  }
});