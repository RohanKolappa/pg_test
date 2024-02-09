/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
 * Interface that provides methods for Session controls management.
 */
jsx3.Class.defineInterface("com.ipvs.gui.WindowControllerI", null, function(WindowControllerI, WindowControllerI_prototype) {

  /** @private @jsxobf-clobber */
  WindowControllerI_prototype._LOG = jsx3.util.Logger.getLogger(WindowControllerI.jsxclass.getName());

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~ init window controls ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /* initWindowControls
  * - init all window controls
  */
  WindowControllerI_prototype.initWindowControls = function() {
    // init player info
    this.objPlayerInfo = new Object();
    this.objPlayerInfo.windowId = null;
  }

  /* resetWindowControls
  * - disable all window controls
  */
  WindowControllerI_prototype.resetWindowControls = function() {
  }
  
  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ player info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  /* 
  * setPlayerInfo - sessionId and other params
  */
  WindowControllerI_prototype.setPlayerInfo = function(objPlayerInfo) {
    // subscribe to player update events from blkWin for this windowId
    // check for change so that we only do it once in case this gets called again
    if((!jsx3.util.strEmpty(objPlayerInfo.windowId)) && (objPlayerInfo.windowId != this.objPlayerInfo.windowId)) {
      // un sub from old window
      if(!jsx3.util.strEmpty(this.objPlayerInfo.windowId)) {
        this.objPlayerInfo.blkWin.unsubscribe(com.ipvs.gui.MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + this.objPlayerInfo.windowId, this, this._onWindowControllerUpdate);
      }
      // stash the info
      this.objPlayerInfo.clsDisplayPanel = objPlayerInfo.clsDisplayPanel;
      this.objPlayerInfo.windowId = objPlayerInfo.windowId;
      // get the window from the display panel
      this.objPlayerInfo.blkWin = objPlayerInfo.clsDisplayPanel.getWindow(objPlayerInfo.windowId);
      // if found subscribe to window events
      if(this.objPlayerInfo.blkWin) {
        this.objPlayerInfo.blkWin.subscribe(com.ipvs.gui.MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + objPlayerInfo.windowId, this, this._onWindowControllerUpdate);
      }
    }
  }

  /* 
  * resetPlayerInfo - init collaboration state vars
  */
  WindowControllerI_prototype.resetPlayerInfo = function() {
    this.objPlayerInfo.action = 'Stop';
    this.objPlayerInfo.streamURL = '';
    this.objPlayerInfo.hres = null;
    this.objPlayerInfo.vres = null;
    this.objPlayerInfo.bFullScreen = false;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ playerwindow event handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ 
  /**
  * The _onWindowControllerUpdate method.
  * - process the update from the window
  * - update the local objPlayerInfo
  * - repaint GUI based on that
  *
  * @param objEvent {JSON}
  */
  WindowControllerI_prototype._onWindowControllerUpdate = function(objEvent) {
    // break out into specific update based on event
    if(objEvent && (objEvent.context.objACTION != null)) {
      this.objPlayerInfo.action = objEvent.context.objACTION.action;
      this.objPlayerInfo.streamURL = objEvent.context.objACTION.streamURL;
    }
    else if(objEvent && (objEvent.context.objNATIVEMODE != null)) {
      this.objPlayerInfo.hres = objEvent.context.objNATIVEMODE.hres;
      this.objPlayerInfo.vres = objEvent.context.objNATIVEMODE.vres;
    }
    else if(objEvent && (objEvent.context.objFULLSCREEN != null)) {
      this.objPlayerInfo.bFullScreen = objEvent.context.objFULLSCREEN.bFullScreen;
    }
  }

  /**
  * _apply_NativeSize -
  * - if the window is present them do the command to make the change on the window
  * - also, update the display config to match
  */
  WindowControllerI_prototype._apply_NativeSize = function(btnState) {
    // by default we fit to window
    var propScale = "fitWindow";
    // if the window has been initialized
    if(this.objPlayerInfo.blkWin) {
      // and the button state is ON
      if(btnState == jsx3.gui.ImageButton.STATE_ON) {
        // set Native size mode
        this.objPlayerInfo.blkWin.setNativeSize();
        propScale = "fitNativeSize";
      }
      else {
        // set scale to window mode
        this.objPlayerInfo.blkWin.setScaleToWindow();
      }
    }
    //    // update the displayconfig to match
    this.clsApp.clsDisplayAgent.setDisplayConfigWinProp(this.objPlayerInfo.windowId, "Properties", "scale", propScale)
  }

  /**
  * _apply_ZoomSize -
  * - if the window is present then do the command to make the change on the window
  * - also, update the display config to match
  */
  WindowControllerI_prototype._apply_ZoomSize = function (zoomSize) {
    // by default we fit to window
    var propScale = "fitWindow";
    // if the window has been initialized
    if (this.objPlayerInfo.blkWin) {
      if(zoomSize == "NATIVE" || zoomSize == "100") {
        this.objPlayerInfo.blkWin.setNativeSize();
        propScale = "fitNativeSize";
      }
      else if (zoomSize == "FITTOWINDOW") {
        // set scale to window mode
        this.objPlayerInfo.blkWin.setScaleToWindow();
      }
      else {
        this.objPlayerInfo.blkWin.setScaleToZoom(zoomSize);
      }
    }
    //update the displayconfig to match
    this.clsApp.clsDisplayAgent.setDisplayConfigWinProp(this.objPlayerInfo.windowId, "Properties", "scale", propScale)
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~ fullscreen control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _apply_FullScreen
  * - apply fullscreen state
  */
  WindowControllerI_prototype._apply_FullScreen = function() {
    // call toggle fullscreen on the window
    if(this.objPlayerInfo.blkWin) {
      var toggleDisplayState = this.objPlayerInfo.blkWin.toggleDisplay();
      //In case of HTA toggleDisplay will fail
      //In this case put hack to proceed to/from fullscreen mode
      if(!toggleDisplayState) {
        var bFullScreen = this.objPlayerInfo.bFullScreen ? false : true;
        this.objPlayerInfo.blkWin.doFullScreen(bFullScreen);
      }
    }

  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~ snapshot control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _onClick_SnapShot process snapshot button click 
  */
  WindowControllerI_prototype._apply_SnapShot = function(srcName) {
    try {
      var WshShell = new ActiveXObject("WScript.Shell");
      var strDesktop = WshShell.SpecialFolders("Desktop");
    }
    catch (e) {
      this._LOG.info("_apply_SnapShot:Desktop path.");
    }
    var dateTimeFormat = new jsx3.util.DateFormat("MM-dd-yyyy-HH-mm-ss");
    var dateTime = dateTimeFormat.format(new Date());
    var path = strDesktop + "/" + srcName + "_" + dateTime + ".bmp";
    this.objPlayerInfo.blkWin && this.objPlayerInfo.blkWin.snapShot(path);
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~ zoom control ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /**
  * _onChange_Zoom:handle user moving the slider position
  * @param fpVALUE {JASON}:fpVALUE.context.fpVALUE;fpVALUE.context.fpPREVIOUS;
  * @param fpPREVIOUS {float}:previous slider position
  * @param thisObj {Object}:slider DOM instance
  * @return {boolean}
  */
  WindowControllerI_prototype._onChange_Zoom = function(fpVALUE) {
    // reset the native mode
    this._reset_NativeSize();
    var fpValue = parseInt(fpVALUE.context.fpVALUE);
    var fpPrevious = parseInt(fpVALUE.context.fpPREVIOUS);
    // apply current value to the window
    this._apply_Zoom(fpValue, fpPrevious);
  }
  /**
  * _onIncrChange_Zoom:set user moving the slider position to tool tip
  * @param fpVALUE {float}:slider position
  * @param thisObj {Object}:slider DOM instance
  * @return {boolean}
  */
  WindowControllerI_prototype._onIncrChange_Zoom = function(fpVALUE) {
    // set tool tip value range of 100%-400%
    var zoomVal = (100 + (3 * parseInt(fpVALUE.context.fpVALUE))) + '%';
    this.sldZoom.setTip(zoomVal);
    return true;
  }

  /**
  * _apply_Zoom: apply the slider position
  * @param val {float}:slider position
  * @param valPrev {float}:previous slider position
  * @return {boolean}
  */
  WindowControllerI_prototype._apply_Zoom = function(val, valPrev) {
    //set tip value convert fpVALUE to range 100%-400% as required
    var zoomVal = (100 + 3 * val) + '%';
    // do zoom if changed
    if(val != valPrev) {
      //do zoom
      this.objPlayerInfo.blkWin && this.objPlayerInfo.blkWin.zoom(zoomVal, zoomVal);
      return true;
    }
    else {
      return false;
    }
  }

});
