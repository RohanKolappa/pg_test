/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Controller class.
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.ipv.Controller", com.ipvs.gui.Controller, null, function (Controller, Controller_prototype) {

  var SessionControllerI = com.ipvs.gui.SessionControllerI;

  /** @private @jsxobf-clobber */
  Controller_prototype._LOG = jsx3.util.Logger.getLogger(Controller.jsxclass.getName());

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ extend initailization  ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /* initSessionControls
  * - init all session controls
  */
  Controller_prototype.initSessionControls = function () {
    this.jsxsupermix();
    // init play controls

    this.btnPause = this.getDescendantOfName("btnPause");
    this.btnPause.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Pause);

    this.btnPlay = this.getDescendantOfName("btnPlay");
    this.btnPlay.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Play);

    //button RecordStart
    this.btnRecordStart = this.getDescendantOfName("btnRecordStart");
    this.btnRecordStart.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_RecordStart);

    //button RecordStop
    this.btnRecordStop = this.getDescendantOfName("btnRecordStop");
    this.btnRecordStop.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_RecordStop);

    // init timeline slider
    this.clsSlider = this.getDescendantsOfType("com.ipvs.gui.SessionSlider")[0];
    this.clsSlider.subscribe("ONSLIDERSELECT", this, this._onSliderSelect);
    this._recordPermission = "NONE";

    //init bookmarks
    this._selectedBookmark = "NONE";
    this.clsBookmarksList = this.getDescendantsOfType("com.ipvs.gui.BookmarksList")[0];
    this.clsBookmarksList.subscribe("BOOKMARKSELECTED", this, this._onBookmarkSelect);

    // init bookmark controls
    this.btnBookmark = this.getDescendantOfName("btnBookmark");
    this.btnBookmark.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_BookmarkAdd);

    this.btnBookmarkEdit = this.getDescendantOfName("btnBookmarkEdit");
    this.btnBookmarkEdit.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_BookmarkEdit);

    this.btnBookmarkX = this.getDescendantOfName("btnBookmarkX");
    this.btnBookmarkX.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_BookmarkX);

    this.clsBookmark = this.getDescendantsOfType("com.ipvs.gui.Bookmark")[0];

    this.lblSrcName = this.getDescendantOfName("lblSourceName");
    this.lblSrcDetails = this.getDescendantOfName("lblSourceDetails");
    this._objAlert = null;
    this._zoomSize = "FITTOWINDOW";
  }

  /* initWindowControls
  * - init all window controls
  */
  Controller_prototype.initWindowControls = function () {
    this.jsxsupermix();

    this.btnZoom = this.getDescendantOfName("window_zoom");
    this.btnZoom.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Zoom);

    this.imgFullScreen = this.getDescendantOfName("window_fullscreen");
    this.imgFullScreen.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_FullScreen);

    this.btnMute = this.getDescendantOfName("window_mute");
    this.btnMute.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_WinMute);

    this.btnKBM = this.getDescendantOfName("window_kbm");
    this.btnKBM.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_WinKBM);

    this.btnSnapShot = this.getDescendantOfName("window_snapshot");
    this.btnSnapShot.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_SnapShot);

    this.avOption = 'Both';
  }

  //~~~~~~~~~~~~~~~~~~~~~~~ Session controls set/get enable/reset ~~~~~~~~~~~~~~~~~~~~~~

  /* resetSessionControls
  * - init all session controls
  */
  Controller_prototype.resetSessionControls = function () {
    this.jsxsupermix();
    //disable all PVR/boookmark btns
    this.btnPause.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnPlay.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.btnRecordStart.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnRecordStart.repaint();
    this.btnRecordStop.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnRecordStop.repaint();
    this.btnBookmark.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnBookmark.setEnabled(jsx3.gui.Form.STATEDISABLED);
    this.btnBookmarkEdit.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnBookmarkX.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnBookmark.repaint();
    this.btnBookmarkEdit.repaint();
    this.btnBookmarkX.repaint();

    this.lblSrcName.setText("", true);
    this.lblSrcDetails.setText("", true);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~ Window controls set/get enable/reset ~~~~~~~~~~~~~~~~~~~~~~

  /* getWindowControls
  * - returns an object with references to all the window related controls
  * - extensions should add their own extra to this
  */
  Controller_prototype.getWindowControls = function () {
    var objWindowControls = new Object;
    objWindowControls['ZOOM'] = this.btnZoom;
    objWindowControls['FULLSCREEN'] = this.imgFullScreen;
    objWindowControls['SNAPSHOT'] = this.btnSnapShot;
    objWindowControls['MUTE'] = this.btnMute;
    objWindowControls['KBMCONTROL'] = this.btnKBM;
    objWindowControls.enable = jsx3.$F(this.enableWindowControls).bind(this);
    objWindowControls.reset = jsx3.$F(this.resetWindowControls).bind(this);
    return objWindowControls;
  }

  /* setWindowControls
  * - accepts an object with references to all the window related controls
  * - allows this class to manage controls that are not direct descendants
  */
  Controller_prototype.setWindowControls = function (objWindowControls) {
    objWindowControls['ZOOM'] && (this.btnZoom = objWindowControls['ZOOM']);
    objWindowControls['FULLSCREEN'] && (this.imgFullScreen = objWindowControls['FULLSCREEN']);
    objWindowControls['SNAPSHOT'] && (this.btnSnapShot = objWindowControls['SNAPSHOT']);
    objWindowControls['MUTE'] && (this.btnMute = objWindowControls['MUTE']);
    objWindowControls['KBMCONTROL'] && (this.btnKBM = objWindowControls['KBMCONTROL']);
  }

  /* resetWindowControls
  * - extend controller add CONTROLLON, CONTROLOFF 
  * - disable all window controls
  */
  Controller_prototype.resetWindowControls = function () {
    this.jsxsupermix();
    //disable btnZoom - reset zoom size if needed
    this._reset_ZoomSize();
    //disable imgFullScreen - reset fullscreen state if needed
    this._reset_FullScreen();
    //incase of switched from video to snapshot enable 1:1 fullscreen btn
    if (this.objCollabInfo.isSourceSelected != "IMAGE") {
      this.btnZoom.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.imgFullScreen.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    //disable btnSnapShot
    this.btnSnapShot.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    //disable btnMute
    this._reset_WinMute();
    this.btnMute.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    //disable btnKBM
    this._reset_WinKBM();
    this.btnKBM.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ player info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  /* 
  * setPlayerInfo - sessionId and other params
  */
  Controller_prototype.setPlayerInfo = function (objPlayerInfo) {
    this.jsxsupermix(objPlayerInfo);
    // Update the player info to the controller
    this.applyPlayerInfo();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  extend controller state update with collab info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /**
  * The applyCollaborationInfo method.
  * - repaint GUI based on objCollabInfo
  *
  * @param objEvent {JSON}
  */
  Controller_prototype.applyCollaborationInfo = function () {
    // do common update
    this.jsxsupermix();
    this.lblSrcName.setText(this.objCollabInfo.SourceName, true);
    this.lblSrcDetails.setText(this.objCollabInfo.SourceDetails, true);

    if (!jsx3.util.strEmpty(this.clsConnError)) this.clsConnError.applyCollaborationInfo(this.objCollabInfo);

    //Pause / Play btn
    // if pvr enabled and conn is streaming and we did not just hit the pause  btn - show pause btn
    if (this.objCollabInfo.isSourceSelected == "IMAGE") {
      this.btnPlay.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnPlay.repaint();
      this.btnPause.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnPause.repaint();
      this.clsApp.setError(null, null);
    }
    else if (this.objCollabInfo.enablePVR && (this.objCollabInfo.state == 'STREAMING') && (this.objCollabInfo.action != 'Pause')) {
      // while streaming - hide play btn and show pause btn
      this.btnPlay.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnPlay.repaint();
      this.btnPause.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      if (this.objCollabInfo.pvrInfo.canPVR) {
        // further - if can pvr and not already hit pause - enable the pause btn
        this.btnPause.setEnabled(jsx3.gui.Form.STATEENABLED);
      }
      else {
        // otherwise just show it in disabled state
        this.btnPause.setEnabled(jsx3.gui.Form.STATEDISABLED);
      }
      this.btnPause.repaint();
    }
    else {
      // if not streaming or we just hit pause - hide the pause btn and show the play btn
      this.btnPause.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnPause.repaint();
      if (this.objCollabInfo.isSourceSelected != "IMAGE") {
        this.btnPlay.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      }
      if (((this.objCollabInfo.state == 'STREAMING') && (this.objCollabInfo.action != 'Resume')) ||
          (this.objCollabInfo.isSourceSelected == "READY" && (SessionControllerI.CONNSTATE[this.objCollabInfo.state] < SessionControllerI.CONNSTATE['CREATED'] && (this.objCollabInfo.action != 'Start')))) {
        // if stream is paused and we have not already hit resume OR
        // if the stream is not started and we have not already hit start
        // - enable the play btn
        this.btnPlay.setEnabled(jsx3.gui.Form.STATEENABLED);
      }
      else {
        // otherwise - just show it in disabled state
        this.btnPlay.setEnabled(jsx3.gui.Form.STATEDISABLED);
      }
      this.btnPlay.repaint();
    }

    //Timeline slider
    this.clsSlider.applyCollaborationInfo(this.objCollabInfo);

    //btnKBM
    // if streaming and not time shifted enable remote control
    // update srcKBMPaused in player info to match
    if (this.objCollabInfo.state == "STREAMING" && !this.objCollabInfo.isTimeshifted) {
      this.objPlayerInfo.srcKBMPaused = false;
    }
    else {
      //On PVR actions, disable the remote KBM so that when Catchup happens the KBM should be available only on clicking btnKBM button.
      //This is because during PVR the tx might have lost the KBM setting and hence on Catchup Player has no way to findout.
      this.objPlayerInfo.remoteKBM = false;
      this.objPlayerInfo.srcKBMPaused = true;
      var btnState = this.btnKBM.getState();
      if (btnState != jsx3.gui.ImageButton.STATE_OFF) {
        this.btnKBM.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
    }

    //Rec btns
    if (this.objCollabInfo.timeMode != 'PLAYBACK' && this.objCollabInfo.state == "STREAMING" && this._recordPermission == "true" && this.objCollabInfo.isSourceSelected != "IMAGE") {
      //this._LOG.info(":Controller:: applyCollaborationInfo: RecState" + this.objCollabInfo.recState);
      if (this.objCollabInfo.recState == 'STREAMING') {
        this.btnRecordStart.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.btnRecordStop.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
        this.btnRecordStop.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
      else if (this.objCollabInfo.recState == 'WAITING_FOR_SOURCE-DEST_CONTACTED' || this.objCollabInfo.recState == 'WAITING_FOR_STREAMSTART') {
      }
      else if (this.objCollabInfo.recState == "SETUP_ERROR" || this.objCollabInfo.recState == "START_ERROR" ||
               this.objCollabInfo.recState == "SOURCE_ERROR" || this.objCollabInfo.recState == "MEDIA_ERROR" || this.objCollabInfo.recState == "DEST_ERROR") {
        this.clsApp.setError(this.docCollab.getCollabRecordError(), "Recording error");
        // stop recording on error
        this.clsApp.clsSessionAgent.stopRecording(this.objCollabInfo.sessionId);
      }
      else {
        this.btnRecordStop.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        this.btnRecordStart.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
        this.btnRecordStart.setEnabled(jsx3.gui.Block.STATEENABLED, true);
      }
    }
    else {
      this.btnRecordStart.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      this.btnRecordStop.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    }

    //Bookmarks list
    this.clsBookmarksList.applyCollaborationInfo(this.objCollabInfo);

    //Bookmark Add btn
    if ((this.objCollabInfo.enableViewBookmarks || (this.objCollabInfo.enableAddBookmarks && this._recordPermission == "true")) &&
        (this.objCollabInfo.state == 'STREAMING') && this.objCollabInfo.isSourceSelected != "IMAGE") {

      if (this.objCollabInfo.canBookmark && this.objCollabInfo.recAction != 'Pause') {
        this.btnBookmark.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
        this.btnBookmark.setEnabled(jsx3.gui.Form.STATEENABLED);
      }
      else {
        this.clsBookmark.destroy();
        this.btnBookmark.setDisplay(jsx3.gui.Block.DISPLAYNONE);
        this.btnBookmark.setEnabled(jsx3.gui.Form.STATEDISABLED);
      }
    }
    else {
      this.btnBookmark.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    }
    this.btnBookmark.repaint();

    //Bookmark Edit / Delete btn
    if (this.objCollabInfo.enableViewBookmarks && (this.objCollabInfo.state == 'STREAMING') && this.objCollabInfo.isSourceSelected != "IMAGE") {
      this.btnBookmarkEdit.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnBookmarkX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      if (this._selectedBookmark != "NONE") {
        this.btnBookmarkEdit.setEnabled(jsx3.gui.Form.STATEENABLED);
        this.btnBookmarkX.setEnabled(jsx3.gui.Form.STATEENABLED);
      }
      else {
        this.btnBookmarkEdit.setEnabled(jsx3.gui.Form.STATEDISABLED);
        this.btnBookmarkX.setEnabled(jsx3.gui.Form.STATEDISABLED);
      }
    }
    else {
      this.btnBookmarkEdit.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnBookmarkX.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    }
    this.btnBookmarkEdit.repaint();
    this.btnBookmarkX.repaint();
    //Bookmarks
    this.clsBookmark.applyCollaborationInfo(this.objCollabInfo);

    this.applyPlayerKBMInfo();
    if (this.objCollabInfo.isSourceSelected == "IMAGE") {
      this.updateWindowControlsForSnapShot();
    }
    else {
      this.applyAVOption();
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ extend playerwindow event handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /**
  * Extend the _onWindowControllerUpdate method.
  * - process the update from the window
  * - update the local objPlayerInfo
  * - repaint GUI based on that
  *
  * @param objEvent {JSON}
  */
  Controller_prototype._onWindowControllerUpdate = function (objEvent) {
    this.jsxsupermix(objEvent);

    // break out into specific update based on event
    if (objEvent && (objEvent.context.objACTION != null)) {
      // Update the player info to the controller
      this.applyPlayerActionInfo();
      //get V2D AVOption setting
      this.avOption = this.objPlayerInfo.streamURL.replace(/^.*avoption=(\w+).*$/, "$1");
      //hide 1:1,snapshot,fullscreen if audio only;hide mute if video only;
      this.applyAVOption();
      // if enableKBM change
      this.objPlayerInfo.enableKBM = (objEvent.context.objACTION.enablekbm == "1" ? true : false);
      this.applyPlayerKBMInfo();
    }
    else if (objEvent && (objEvent.context.objFULLSCREEN != null)) {
      // Update the player info to the controller
      this.applyPlayerFullScreenInfo();
    }
    else if (objEvent && (objEvent.context.objSRCKBM != null)) {
      // if srcKBM change
      this.objPlayerInfo.srcKBM = objEvent.context.objSRCKBM.srcKBM;
      this.applyPlayerKBMInfo();
    }
  }

  Controller_prototype.updateWindowControlsForSnapShot = function () {
    this.btnMute.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnZoom.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.imgFullScreen.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnSnapShot.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.btnZoom.setEnabled(jsx3.gui.Form.STATEENABLED);
    this.btnMute.repaint();
    this.btnZoom.repaint();
    this.imgFullScreen.repaint();
    this.btnSnapShot.repaint();
    this._reset_ZoomSize();
  }

  //~~~~~~~~~~~~~~~~~~~~ extend controller state update with player info ~~~~~~~~~~~~~~~~~~~

  /**
  * Extend the applyPlayerInfo method.
  * - repaint GUI based on objPlayerInfo
  *
  */
  Controller_prototype.applyPlayerInfo = function () {
    this.applyPlayerActionInfo();
  }

  /**
  * The applyPlayerActionInfo method.
  * - repaint GUI based on objPlayerInfo
  *
  */
  Controller_prototype.applyPlayerActionInfo = function () {
    if (this.objPlayerInfo.action == 'Stop') {
      this.resetWindowControls();
    }
  }

  /**
  * applyPlayerFullScreenInfo process fullscreen callback
  * - reset zoom - publish event for rest of UI to adjust - set button state 
  */
  Controller_prototype.applyPlayerFullScreenInfo = function () {
    // set button state + enabled
    var btnState = this.objPlayerInfo.bFullScreen ? jsx3.gui.ImageButton.STATE_ON : jsx3.gui.ImageButton.STATE_OFF;
    this.imgFullScreen.setState(btnState, true);
    // publish event for rest of GUI
    var objFullScreen = { windowId: this.objPlayerInfo.windowId, isFullScreen: this.objPlayerInfo.bFullScreen };
    this.publish({ subject: com.ipvs.gui.Controller.MSG_FULLSCREEN, context: { objFULLSCREEN: objFullScreen} });
  };

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Apply AVOption to player buttons ~~~~~~~~~~~~~~~~~~~~~~~~~
  /**
  * The applyAVOption method.
  * - based on V2D profile setting hide/show player video buttons,1:1,snapshot,fullscreen,mute; 
  * - KBM button process separately
  */
  Controller_prototype.applyAVOption = function () {
    if (this.objPlayerInfo.action == 'Stop' || this.objCollabInfo.state != 'STREAMING') return;
    if (this.avOption == "Video") {//'Video' only
      this.btnMute.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnZoom.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.imgFullScreen.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnSnapShot.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnZoom.setEnabled(jsx3.gui.Form.STATEENABLED);
      this.imgFullScreen.setEnabled(jsx3.gui.Form.STATEENABLED);
      this.btnSnapShot.setEnabled(jsx3.gui.Form.STATEENABLED);
    }
    else if (this.avOption == "Audio") {//'Audio' only
      this.btnMute.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnMute.setEnabled(jsx3.gui.Form.STATEENABLED);
      this.btnZoom.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.imgFullScreen.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.btnSnapShot.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    }
    else {//'Both',MPGTS,RTP;
      this.btnMute.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnZoom.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.imgFullScreen.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnSnapShot.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.btnMute.setEnabled(jsx3.gui.Form.STATEENABLED);
      this.btnZoom.setEnabled(jsx3.gui.Form.STATEENABLED);
      this.imgFullScreen.setEnabled(jsx3.gui.Form.STATEENABLED);
      this.btnSnapShot.setEnabled(jsx3.gui.Form.STATEENABLED);
    }
    this.btnMute.repaint();
    this.btnZoom.repaint();
    this.imgFullScreen.repaint();
    this.btnSnapShot.repaint();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Zoom control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Controller_prototype.setZoomEvents = function () {
    if (!jsx3.util.strEmpty(this.clsZoomPanel)) {
      this.clsZoomPanel.subscribe("ONZOOMSELECT", this, this._apply_Zoom);
    }
  }
  /**
  * _onClick_Zoom process to show panel to select zoom size 
  */
  Controller_prototype._onClick_Zoom = function (objEvent) {
    objEvent.context.objEVENT.cancelBubble();
    if (jsx3.util.strEmpty(this.clsZoomPanel)) return;
    if (this.btnZoom.getState() == jsx3.gui.ImageButton.STATE_ON) {
      this.clsZoomPanel.showPanel();
    }
    else {
      this.clsZoomPanel.hidePanel();
    }
  }

  /**
  * _onClick_Zoom process to show panel to select zoom size 
  */
  Controller_prototype._apply_Zoom = function (objEvent) {
    this.btnZoom.setState(jsx3.gui.ImageButton.STATE_OFF);
    if (objEvent.context.ZoomSize == "CANCEL") return;
    if (this.objCollabInfo.isSourceSelected == "IMAGE" && this.objCollabInfo.state == "NONE") {
      this.publish({ subject: com.ipvs.gui.Controller.MSG_ZOOM, context: objEvent.context });
    }
    else {
      this._apply_ZoomSize(objEvent.context.ZoomSize);
    }
    this._zoomSize = objEvent.context.ZoomSize;
  }

  /**
  * _reset_ZoomSize 
  * - reset native size state
  */
  Controller_prototype._reset_ZoomSize = function () {
    if (this._zoomSize != "FITTOWINDOW") this.clsZoomPanel.resetZoom();
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Native size control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _onClick_NativeSize process 1:1 button click 
  */
  Controller_prototype._onClick_NativeSize = function () {
    // apply current value to the window
    var btnState = this.btn1to1.getState()
    this._apply_NativeSize(btnState);
  }

  /**
  * Extend the _apply_NativeSize method.
  * If selected source is type of IMAGE then publish event otherwise call super method
  */
  Controller_prototype._apply_NativeSize = function (btnState) {
    if (this.objCollabInfo.isSourceSelected == "IMAGE") {
      this.publish({ subject: com.ipvs.gui.Controller.MSG_ZOOM, context: { ZoomSize: btnState ? "NATIVE" : "FITTOWINDOW"} });
    }
    else {
      this.jsxsupermix(btnState);
    }
  }


  /**
  * _reset_NativeSize 
  * - reset native size state
  */
  Controller_prototype._reset_NativeSize = function () {
    var btnState = this.btn1to1.getState();
    if (btnState != jsx3.gui.ImageButton.STATE_OFF) {
      this.btn1to1.setState(jsx3.gui.ImageButton.STATE_OFF);
      this._apply_NativeSize(jsx3.gui.ImageButton.STATE_OFF);
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Fullscreen control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _onClick_FullScreen process fullscreen button click 
  * - if the window is present them do the command to make the change on the window
  * - rest of processing will happen in callback
  */
  Controller_prototype._onClick_FullScreen = function () {
    //disable the button to prohibit multi-click; call back will enable
    //    this.imgFullScreen.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this._apply_FullScreen();
  }

  /**
  * Extend the _apply_FullScreen method.
  * If selected source is type of IMAGE then publish event otherwise call super method
  */
  Controller_prototype._apply_FullScreen = function () {
    if (this.objCollabInfo.isSourceSelected == "IMAGE") {
      this.objPlayerInfo.bFullScreen = this.objPlayerInfo.bFullScreen ? false : true;
      this.applyPlayerFullScreenInfo();
    }
    else {
      this.jsxsupermix();
    }
  }

  /**
  * _reset_FullScreen
  * - reset fullscreen state
  */
  Controller_prototype._reset_FullScreen = function () {
    var btnState = this.imgFullScreen.getState();
    if (btnState != jsx3.gui.ImageButton.STATE_OFF) {
      this.imgFullScreen.setState(jsx3.gui.ImageButton.STATE_OFF);
      this._apply_FullScreen();
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Mute/Unmute control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _onClick_WinMute process mute button click 
  */
  Controller_prototype._apply_Mute = function (muteType) {
    this.objPlayerInfo.blkWin && this.objPlayerInfo.blkWin.mute(muteType);
  }

  /**
  * _onClick_WinMute process mute button click 
  */
  Controller_prototype._onClick_WinMute = function () {
    var btnState = this.btnMute.getState();
    if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
      this._apply_Mute(jsx3.gui.ImageButton.STATE_ON);
    }
    else {
      this._apply_Mute(jsx3.gui.ImageButton.STATE_OFF);
    }
  }

  /**
  * _reset_WinMute 
  * - reset mute state
  */
  Controller_prototype._reset_WinMute = function () {
    var btnState = this.btnMute.getState();
    if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
      this.btnMute.setState(jsx3.gui.ImageButton.STATE_ON);
      this._apply_Mute(jsx3.gui.ImageButton.STATE_OFF);
    }
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ KBM control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /**
  * _onClick_WinKBM process kbm button click 
  */
  Controller_prototype._apply_KBMOnOff = function (newKBMState) {
    // check if blk win is valid and request is changing the state
    if (this.objPlayerInfo.blkWin && (newKBMState != this.objPlayerInfo.currentKBM)) {
      // set the player state to match
      this.objPlayerInfo.currentKBM = newKBMState;
      this.objPlayerInfo.blkWin.setLocalKBMSState(newKBMState);
    }
  }

  /**
  * The applyPlayerKBMInfo method.
  * - repaint GUI based on objPlayerInfo
  * - update the player state to match as well
  */
  Controller_prototype.applyPlayerKBMInfo = function () {
    var value = false;
    if ((this.objPlayerInfo.srcKBM && !this.objPlayerInfo.srcKBMPaused) && this.objPlayerInfo.enableKBM) {
      var streamURL = this.objPlayerInfo.streamURL;
      if (streamURL.match(/avoption=/)) {
        var avOption = streamURL.replace(/^.*avoption=(\w+).*$/, "$1");
        if (avOption != "Audio") {
          this.btnKBM.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
          this.btnKBM.setEnabled(jsx3.gui.Form.STATEENABLED);
          if (this.objPlayerInfo.remoteKBM) {
            value = true;
          }
          else {
            value = false;
          }
        }
      }
    }
    else {
      this.btnKBM.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      value = false;
    }
    this.btnKBM.repaint();
    this._apply_KBMOnOff(value);
  }

  /**
  * _onClick_WinKBM process KBM button click 
  */
  Controller_prototype._onClick_WinKBM = function () {
    var btnState = this.btnKBM.getState();
    if (btnState != jsx3.gui.ImageButton.STATE_OFF) {
      this.objPlayerInfo.remoteKBM = true;
    }
    else {
      this.objPlayerInfo.remoteKBM = false;
    }
    this.applyPlayerKBMInfo();
  }

  /**
  * _reset_WinKBM 
  * - reset KBM state
  */
  Controller_prototype._reset_WinKBM = function () {
    var btnState = this.btnKBM.getState();
    if (btnState != jsx3.gui.ImageButton.STATE_OFF) {
      this.btnKBM.setState(jsx3.gui.ImageButton.STATE_OFF);
      this.applyPlayerKBMInfo();
    }
    this.objPlayerInfo.srcKBM = false;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Snapshot control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _onClick_SnapShot process Snapshot button click 
  */
  Controller_prototype._onClick_SnapShot = function () {
    var srcName = this.objCollabInfo.SourceName;
    srcName = srcName.replace(/[&\/\\#,+()$~%.:*?<>{}|@`!^'"]/g,'_');
    this._apply_SnapShot(srcName);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ Control buttons click ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  Controller_prototype._onSliderSelect = function (objSliderEvent) {
    var offSet = objSliderEvent.context;
    if (offSet == "CATCHUP") {
      this._onClick_CatchUp();
    }
    else if (offSet > 0) {
      this.clsApp.clsSessionAgent.skipfwd(this.objCollabInfo.sessionId, offSet);
    }
    else {
      this.clsApp.clsSessionAgent.skipback(this.objCollabInfo.sessionId, (offSet * -1));
    }
  }

  Controller_prototype._onBookmarkSelect = function (objBookmarkEvent) {
    this._selectedBookmark = objBookmarkEvent.context;
    if (this._selectedBookmark == "NONE") return;
    this.clsBookmark.destroy();
    this.btnBookmarkEdit.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.btnBookmarkEdit.setEnabled(jsx3.gui.Form.STATEENABLED);
    this.btnBookmarkX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.btnBookmarkX.setEnabled(jsx3.gui.Form.STATEENABLED);
    this.btnBookmarkEdit.repaint();
    this.btnBookmarkX.repaint();
    this.clsApp.clsSessionAgent.resumeFromBookmark(this.objCollabInfo.sessionId, this._selectedBookmark);
  }

  Controller_prototype._onClick_BookmarkAdd = function () {
    this.clsBookmark.newBookmark(this.objCollabInfo.sessionId);
  }

  Controller_prototype._onClick_BookmarkEdit = function () {
    this.clsBookmark.editBookmark(this._selectedBookmark);
  }

  Controller_prototype._onClick_BookmarkX = function () {
    var ThisClass = this;
    ThisClass._objAlert = PNameSpace.confirm("Warning",
                                             "Selected bookmark will be deleted.\n              Are you sure?",
                                             function () { Controller_prototype._deleteBookmark(ThisClass); }, null, "OK", "Cancel");
  }

  // OK will this function and Cancel will automatically close the dialog
  Controller_prototype._deleteBookmark = function (ThisClass) {
    if (ThisClass._objAlert != null) {
      ThisClass._objAlert.doClose();
      ThisClass._objAlert = null;
      ThisClass.clsBookmark.deleteBookmark(ThisClass._selectedBookmark);
      ThisClass._selectedBookmark = "NONE";
      ThisClass.btnBookmarkEdit.setEnabled(jsx3.gui.Form.STATEDISABLED);
      ThisClass.btnBookmarkX.setEnabled(jsx3.gui.Form.STATEDISABLED);
      ThisClass.btnBookmarkEdit.repaint();
      ThisClass.btnBookmarkX.repaint();
    }
  }

  Controller_prototype._onClick_Rewind = function () {
    // disable the button till action succeeds
    //    this.btnRewind.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    // send skipback request
    var actionData = this.clsApp.clsPreferences.getLocalPreferences("SkipBack", 10000);
    this.clsApp.clsSessionAgent.skipback(this.objCollabInfo.sessionId, actionData);
  }

  Controller_prototype._onClick_Pause = function () {
    // disable the button till action succeeds
    this.btnPause.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    // send pause request
    this.clsApp.clsSessionAgent.pause(this.objCollabInfo.sessionId);
  }

  Controller_prototype._onClick_Play = function () {
    // disable the button till action succeeds
    this.btnPlay.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    //check state - If it is Paused then Resume otherwise Start
    if (this.objCollabInfo.action == 'Pause') {
      this.clsApp.clsSessionAgent.resume(this.objCollabInfo.sessionId);
    }
    else {
      this.clsApp.clsSessionAgent.rePlay(this.objCollabInfo.sessionId);
    }
  }

  Controller_prototype._onClick_Fastforward = function () {
    // disable the button till action succeeds
    //    this.btnFastforward.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    // send skipfwd request
    var actionData = this.clsApp.clsPreferences.getLocalPreferences("SkipFWD", 30000);
    this.clsApp.clsSessionAgent.skipfwd(this.objCollabInfo.sessionId, actionData);
  }

  /**
  * _onClick_CatchUp: click Live Off button
  * TBD - should not need any btn state update here
  */
  Controller_prototype._onClick_CatchUp = function () {
    var sessionId = this.objCollabInfo.sessionId;
    if (sessionId != "NONE") {
      this.clsApp.clsSessionAgent.catchup(sessionId);
    }
  }

  Controller_prototype._onClick_Stop = function () {
    this._doStop();
  }

  Controller_prototype.doStop = function () {
    this._doStop();
  }

  Controller_prototype._doStop = function (needConfirmation) {
    this._LOG.info(":stopSession:: _objControllerState:" + this.objCollabInfo.state);
    if (this.objCollabInfo.state == "NONE") return;
    // if need stop - disable the button till action succeeds
    //    this.btnStop.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    // send stop request
    this.clsApp.clsSessionAgent.StopSession(this.objCollabInfo.sessionId);
  }

  /**
  * _onClick_RecordStart: click Record Start button
  */
  Controller_prototype._onClick_RecordStart = function () {
    this.btnRecordStart.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.clsApp.clsSessionAgent.startRecording(this.objCollabInfo.sessionId, this.clsApp.getDescendantsOfType("com.ipvs.ipv.MainPanel")[0].getSelectedSrc(), null);
  }

  /**
  * _onClick_RecordStop: click Record Stop button
  */
  Controller_prototype._onClick_RecordStop = function () {
    this.btnRecordStop.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    this.clsApp.clsSessionAgent.stopRecording(this.objCollabInfo.sessionId);
  }

});