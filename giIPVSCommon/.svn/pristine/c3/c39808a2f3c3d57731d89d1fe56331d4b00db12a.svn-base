/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the display agent
*/
jsx3.require("jsx3.gui.Block", "com.ipvs.gui.MediaPlayerWindow", "com.ipvs.util.Common");
jsx3.Class.defineClass('com.ipvs.gui.DisplayPanel', jsx3.gui.Block, null, function(DisplayPanel, DisplayPanel_prototype) {

  DisplayPanel.MAX_ZINDEX = 9999999;
  // create Logger for this class
  DisplayPanel_prototype._LOG = jsx3.util.Logger.getLogger(DisplayPanel.jsxclass.getName());

  /**
  * The instance initializer.
  * @param eventApp {Object}
  */
  DisplayPanel_prototype.onAfterAttach = function() {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if(!this.clsApp) return;
    if(this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  DisplayPanel_prototype._onAppInit = function() {
    // init array of window by winId
    this.arrWindowsByWinId = new Object;

    //canvas id
    this.DOC_DISPLAYCONFIG = null;
    this.canvasId = null;
    this.canvasMaximizeWinId = null;

    //flag of players' mode, default/'true': window mode, 'false' windowless mode
    this.canvasPlayerModeFlg = false;

    this._LOG.debug("com.ipvs.gui.DisplayPanel: Initialized");
  };

  /**
  * setDisplayPanelId: set display panel Id with a composed displayId+panelId
  * @param displayId {String}
  * @param panelId {String}
  **/
  DisplayPanel_prototype.setDisplayPanelId = function(displayId, panelId) {
    // stash display and panel id  
    this.displayId = displayId;
    this.panelId = panelId;
    // get DisplayConfig node from DisplayAgent
    var nodeDisplayConfig = this.clsApp.clsDisplayAgent.getDisplayConfig(this.displayId);
    // apply this display config
    this._applyDisplayPanel(nodeDisplayConfig);
    // subscribe canvas events for this displayId
    if(this.clsApp.clsDisplayAgent.getSubscriberCount("EVT_CANVAS-" + this.displayId) == 0) {
      this.clsApp.clsDisplayAgent.subscribe("EVT_CANVAS-" + this.displayId, this, this.onCanvasEvent);
    }
  }


  /**
  * removeDisplayPanelId: set deletes display panel Id
  * @param displayId {String}
  * @param panelId {String}
  **/
  DisplayPanel_prototype.removeDisplayPanelId = function(displayId, panelId) {
    var nodeDisplayConfig = this.clsApp.clsDisplayAgent.getDisplayConfig(this.displayId);
    if(!jsx3.util.strEmpty(nodeDisplayConfig)) {
      // remove this display panel
      this._removeDisplayPanel(nodeDisplayConfig);
      // unsubscribe canvas events for this displayId
      this.clsApp.clsDisplayAgent.unsubscribe("EVT_CANVAS-" + this.displayId, this, this.onCanvasEvent);
    }
  }


  //===================== Display Methods ======================================
  /**
  * apply the relevant part of the canvas for the current display panel
  * @param nodeDisplayConfig {Object}
  */
  DisplayPanel_prototype._applyDisplayPanel = function(nodeDisplayConfig) {
    this._LOG.debug(":DisplayPanel._applyDisplayPanel");
    // In our case there is a 1:1 mapping between Display and Canvas - only one Panel - so no math needed here
    // TBD - deal with more complex DisplayConfigs (multi-panel)
    var nodeCanvas = nodeDisplayConfig.selectSingleNode(".//Canvas");
    this._applyCanvas(nodeCanvas);
  }

  /**
  * remove the relevant part of the canvas from the current display panel
  * @param nodeDisplayConfig {Object}
  */
  DisplayPanel_prototype._removeDisplayPanel = function(nodeDisplayConfig) {
    this._LOG.debug(":DisplayPanel._removeDisplayPanel");
    // In our case there is a 1:1 mapping between Display and Canvas - only one Panel - so no math needed here
    // TBD - deal with more complex DisplayConfigs (multi-panel)
    var nodeCanvas = nodeDisplayConfig.selectSingleNode(".//Canvas");
    this._removeCanvas(nodeCanvas);
  }

  //===================== Canvas Methods ======================================
  /**
  * render the window layout based on the current display config
  * @param nodeCanvas {Object}
  */
  DisplayPanel_prototype._applyCanvas = function(nodeCanvas) {
    this._LOG.debug(":DisplayPanel._applyCanvas");
    //stash canvasId (only one canvasId at a run time)
    this.canvasId = nodeCanvas.getAttribute("id");;
    // go thru the current connectionWindow array
    for(var winId in this.arrWindowsByWinId) {
      // for each window check if the winId is still in the current Canvas
      if(!nodeCanvas.selectSingleNode(".//Window[@id='" + winId + "']")) {
        // if not found - delete it
        this.deleteWindow(winId);
        if(this.canvasMaximizeWinId == winId) this.canvasMaximizeWinId = null;
        continue;
      }
    }

    // Old performance optimization code to force windowed mode - based on no z-index in canvas
    //    /**
    //    * If all zIndex=0, set this.canvasPlayerModeFlg to true (window mode)
    //    * this setting take effect only before player loaded, calling from 'open',
    //    * this.arrWindowsByWinId is initialized at _addWindow, so 'zIndex' should be place here
    //    * instead of above 'for' loop; 
    //    **/
    //    var iterWindowsForZIndex = nodeCanvas.selectNodeIterator(".//Window/Properties[@zIndex>0]");
    //    this.canvasPlayerModeFlg = (!iterWindowsForZIndex.hasNext());

    // go thru the Windows in the current Canvas
    var hresCanvas = nodeCanvas.getAttribute("hres");
    var vresCanvas = nodeCanvas.getAttribute("vres");

    var iterWindows = nodeCanvas.selectNodeIterator(".//Window");
    while(iterWindows.hasNext()) {
      // for each window
      var nodeWin = iterWindows.next();
      var winId = nodeWin.getAttribute("id");
      var nodeProps = nodeWin.selectSingleNode(".//Properties");
      //check 'maximize' at canvas level before _applyWindowLayout, keep 'maximize' part in case calling from API
      var bMaximize = nodeProps.getAttribute("maximize");
      //check property 'maximize' for all window
      if(bMaximize == 'true') {
        if(jsx3.util.strEmpty(this.canvasMaximizeWinId)) {
          //if no canvasMaximizedWinId, set it to current one
          this.canvasMaximizeWinId = winId;
        }
        else if(this.canvasMaximizeWinId != winId) {
          //for those winId are not equal canvasMaximizedWinId, set their 'maximize' to 'false'
          nodeProps.setAttribute("maximize", "false");
        }
      }
      // update the layout for this window
      this._applyWindowLayout(winId, nodeWin, hresCanvas, vresCanvas);
    }
  }

  /**
  * remove the window layout from the current display config
  * @param nodeCanvas {Object}
  */
  DisplayPanel_prototype._removeCanvas = function(nodeCanvas) {
    this._LOG.debug(":DisplayPanel._removeCanvas");
    // go thru the current connectionWindow array
    for(var winId in this.arrWindowsByWinId) {
      // for each window check if the winId is still in the current Canvas
      if(nodeCanvas.selectSingleNode(".//Window[@id='" + winId + "']")) {
        // if found - delete it
        this.deleteWindow(winId);
      }
    }
  }
  //===================== Window Methods ======================================
  /*
  * getWindow - looks in array and return blkWin
  */
  DisplayPanel_prototype.getWindow = function(winId) {
    return this.arrWindowsByWinId[winId];
  }

  /**
  * add a new window into the current display
  * TBD: this needs to become aware of the display panel to canvas window mapping
  * @param winId {string}
  * @param hpos (string),window start horizontal position,pixel or percentage
  * @param vpos (string),window start vertical position,pixel or percentage
  * @param hres (string),window width,pixel or percentage
  * @param vres (string),window height,pixel or percentage
  * @return window instance blkJSX
  */
  DisplayPanel_prototype._addWindow = function(winId, hpos, vpos, hres, vres, playerType) {
    this._LOG.info(":DisplayPanel._addWindow: " + winId + ":" + hpos + ":" + vpos + ":" + hres + ":" + vres + ":player type: " + playerType);
    // create a new Window instance
    var blkWin = new com.ipvs.gui.MediaPlayerWindow(winId, hpos, vpos, hres, vres, playerType);
    if(blkWin) {
      // save a reference to it in the list
      this.arrWindowsByWinId[winId] = blkWin;

      // get the current canvas block for this panel
      var blkCanvas = this;

      // attach window blk as a child of the matching canvas window
      // load a player into the window - async fnc - do nothing on return
      com.ipvs.util.Common.asyncSetChild(blkCanvas, blkWin).when(
        jsx3.$F(function() {
          this._LOG.debug(":DisplayPanel:_addWindow:" + blkWin.getName() + "'s block load successful.");
          blkWin.asyncLoadPlayer().when(
                    jsx3.$F(function() {
                      // TBD - this should be done directly by player window
                      //subscribe to get any SSCmd event this for the blkwin; windowId  = blkWin.getName()
                      this.clsApp.clsSSAgent.subscribe("EVT_PLAYERACTION-" + blkWin.getName(), blkWin, blkWin.onPlayerActionEvent);
                      //add blkWin to SSAgent to trigger player START
                      this.clsApp.clsSSAgent.getPlayerAction(blkWin.getName());
                    }).bind(this));
        }).bind(this));
    }
    else {
      this._LOG.error(":DisplayPanel._addWindow: " + winId + ": FAILED");
    }
    return blkWin;
  }

  /**
  * deletes an existing window from the current display
  * @param winId {string}
  * @return window instance blkJSX
  */
  DisplayPanel_prototype.deleteWindow = function(winId) {
    this._LOG.debug(":DisplayPanel.deleteWindow: " + winId);
    // get the current display window 
    var blkWin = this.getWindow(winId);
    if(blkWin) {
      jsx3.sleep(function() {
        //unload player and remove its reference in DOM
        blkWin.unloadPlayer();
        // remove it from the current display
        blkWin.getParent().removeChild(blkWin);
      }, null, this);
      // and deref it from the list
      delete this.arrWindowsByWinId[winId];
    }
    // update window status - TBD
    //    var nodeWinStat = this.DOC_DISPLAYCONFIG.selectSingleNode(".//Window[@id='"+winId+"']");
    //    nodeWinStat.getParent().removeChild(nodeWinStat);
    //    //send delete connection from SSCmd
    //    jsx3.sleep(function() { this.clsApp.clsSSAgent.sendSSCmdDeleConn(winId), null, this });

    return blkWin;
  }

  /**
  * updates the layout of an existing window in the current display
  * @param winId {string}
  * @param objWinLayout {object}
  * @param hresCanvas (string),canvas width,pixel or percentage
  * @param vresCanvas (string),canvas height,pixel or percentage
  * @return bDirty {boolean}
  */
  DisplayPanel_prototype._applyWindowLayout = function(winId, nodeWin, hresCanvas, vresCanvas) {
    this._LOG.debug(":DisplayPanel._applyWindowLayout: " + winId);
    var bDirty = false;

    // get the current display window
    var blkWin = this.getWindow(winId);
    // if it does not exist in the display - add it with some hard coded size
    if(!blkWin) {
      //launched players based on zIndex value: 1 - window mode; 0 - windowless mode;
      var playerType = this.canvasPlayerModeFlg ? com.ipvs.gui.MediaPlayer.WINDOW_MODE_PLAYER : com.ipvs.gui.MediaPlayer.WINDOWLESS_MODE_PLAYER;
      blkWin = this._addWindow(winId, 0, 0, 10, 10, playerType);
      bDirty = true;
    }

    // get the pros node
    var nodeProps = nodeWin.selectSingleNode(".//Properties");

    // set window position, size and zindex
    // get the canvas blk for this window
    var blkCanvas = blkWin.getParent();
    var hresBlkCanvas = blkCanvas.getWidth();
    var vresBlkCanvas = blkCanvas.getHeight();
    //check window 'maxmize'
    var maximize = nodeProps.getAttribute("maximize");
    if(maximize == "true") {
      //'maximize' means this window fill full size of its parent canvas
      if(blkWin.getZIndex() != DisplayPanel.MAX_ZINDEX) {
        blkWin.setZIndex(DisplayPanel.MAX_ZINDEX, true);
        blkWin.setDimensions(0, 0, hresBlkCanvas, vresBlkCanvas, true);
        bDirty = true;
      }
    }
    else {
      // get window position and size
      var hpos = nodeProps.getAttribute("hpos");
      var vpos = nodeProps.getAttribute("vpos");
      var hres = nodeProps.getAttribute("hres");
      var vres = nodeProps.getAttribute("vres");
      // - convert pixel to % if needed
      // if canvas hres is not in %
      if(hresCanvas.indexOf('%') < 0) {
        // and hpos is not in %
        if(hpos.indexOf('%') < 0) {
          hpos = (hpos / hresCanvas) * 100;
          hpos = hpos > 100 ? 100 : hpos;
        }
        // or hres is not in %
        if(hres.indexOf('%') < 0) {
          hres = (hres / hresCanvas) * 100;
          hres = hres > 100 ? 100 : hres;
        }
      }
      // if canvas vres is not in %
      if(vresCanvas.indexOf('%') < 0) {
        // and vpos is not in %
        if(vpos.indexOf('%') < 0) {
          vpos = (vpos / vresCanvas) * 100;
          vpos = vpos > 100 ? 100 : vpos;
        }
        // or vres is not in %
        if(vres.indexOf('%') < 0) {
          vres = (vres / vresCanvas) * 100;
          vres = vres > 100 ? 100 : vres;
        }
      }
      //      // - then convert back to pixel based on canvas blk size
      //      hpos = Math.round(hresBlkCanvas * parseFloat(hpos) / 100);
      //      vpos = Math.round(vresBlkCanvas * parseFloat(vpos) / 100);
      //      hres = Math.round(hresBlkCanvas * parseFloat(hres) / 100);
      //      vres = Math.round(vresBlkCanvas * parseFloat(vres) / 100);
      // set the dimensions
      if((blkWin.getLeft() != hpos) || (blkWin.getTop() != vpos) ||
          (blkWin.getWidth() != hres) || (blkWin.getHeight() != vres)) {
        blkWin.setDimensions(hpos, vpos, hres, vres, true);
        bDirty = true;
      }
      // set zIndex only not being maximized
      var zIndex = nodeProps.getAttribute("zIndex");
      (blkWin.getZIndex() != zIndex) && blkWin.setZIndex(zIndex, true) && (bDirty = true);
    }

    // set border
    var border = nodeProps.getAttribute("border");
    (blkWin.getBorder() != border) && blkWin.setBorder(border, true) && (bDirty = true);

    //TBD: deal with aspect
    //TBD-HACK: stash transparency to current blkWin
    var transparency = nodeProps.getAttribute("transparency");
    (blkWin.getBlkTransparency() != transparency) && blkWin.setBlkTransparency(transparency) && (bDirty = true);

    // update the visibility for this window
    var bVisible = nodeProps.getAttribute("visibility");
    var bVisibility = (bVisible == "false") ? jsx3.gui.Block.VISIBILITYHIDDEN : jsx3.gui.Block.VISIBILITYVISIBLE;
    // set visibility and repaint
    if(blkWin.getVisibility() != bVisible) {
      bDirty |= this._applyWindowVisible(winId, bVisibility);
    }

    //when setting properties:'mute' and 'scale' from window 'Edit',apply changes
    bDirty |= this._applyPlayerProps(winId, nodeWin);

    // update window status - TBD
    if(bDirty) {
      //      var nodeWinStat = this.DOC_DISPLAYCONFIG.selectSingleNode(".//Window[@id='"+winId+"']");
      //      var nodePropsStat = nodeWinStat.selectSingleNode(".//Properties");
      //      nodePropsStat.setAttribute("hpos", nodeProps.getAttribute("hpos"));
      //      nodePropsStat.setAttribute("vpos", nodeProps.getAttribute("vpos"));
      //      nodePropsStat.setAttribute("hres", nodeProps.getAttribute("hres"));
      //      nodePropsStat.setAttribute("vres", nodeProps.getAttribute("vres"));
      //      nodePropsStat.setAttribute("border", nodeProps.getAttribute("border"));
      //      nodePropsStat.setAttribute("zIndex", nodeProps.getAttribute("zIndex"));
      //      nodePropsStat.setAttribute("transparency", blkWin.getBlkTransparency());
      //      nodePropsStat.setAttribute("maximize", maximize);
      //      nodePropsStat.setAttribute("visibility", bVisible);
      //      //set player properties mute,scale when _applyWindowLayout otherwise it won't set
      //      nodePropsStat.setAttribute("mute", nodeProps.getAttribute("mute"));
      //      nodePropsStat.setAttribute("remoteKbmCtrl", nodeProps.getAttribute("remoteKbmCtrl"));
      //      nodePropsStat.setAttribute("scale", nodeProps.getAttribute("scale"));
      //      //set player properties scrollHPos,scrollVPos when _applyWindowLayout otherwise it won't set
      //      nodePropsStat.setAttribute("scrollHPos", nodeProps.getAttribute("scrollHPos"));
      //      nodePropsStat.setAttribute("scrollVPos", nodeProps.getAttribute("scrollVPos"));
    }
  }

  /**
  * updates the visibility of an existing window in the current display
  * @param winId {string}
  * @param bVisible {boolean}
  * @return bDirty {boolean}
  */
  DisplayPanel_prototype._applyWindowVisible = function(winId, bVisible) {
    var bDirty = false;
    // get the current display window
    var blkWin = this.getWindow(winId);
    if(blkWin) {
      this._LOG.info("::_applyWindowVisible:winId: " + winId + " visible: " + bVisible);
      //set its visibility and repaint;
      blkWin.setVisibility(bVisible, true) && (bDirty = true);
    }
    else {
      this._LOG.warn("::_applyWindowVisible:blkWin(" + winId + ") not exist.");
    }
    return bDirty;
  }

  /**
  * apply player related properteis to video player assuming this valid only when player is loaded; 
  *    [1] setWindowContentByKBMCtrlK,_setPortWindowContentByRFBStateChange 
  *           --> setPortWindowContent with a parameter winId 
  *    [2] WindowPropertiesUpdate notification 
  *           -->_applyCanvas --> _applyWindowLayout -->(update window status) setPortWindowProperties with parameters winId and property node  
  * @param winId {string} 
  * @param nodeWin {object}
  */
  DisplayPanel_prototype._applyPlayerProps = function(winId, nodeWin) {

    var bDirty = false;

    // get the current display window
    var blkWin = this.getWindow(winId);

    //if 'blkWin' exist and the player loaded and has Filename, then apply player properties
    if(blkWin && blkWin.IPVideoPlayer && !jsx3.util.strEmpty(blkWin.IPVideoPlayer.Filename)) {

      /**
      * zIndex="100" border="" vpos="0" hpos="0" vres="100%" hres="100%" aspect="" transparency="0" layoutId="" layoutPos="" visibility="true"
      * maximize="false" fps="15" scale="fitWindow" mute="false" volume="50%" remoteKbmCtrl="false" scrollHPos="0" scrollVPos="0"
      * properties coming from [1]WindowPropertiesUpdate notification (need set to this.DOC_DISPLAYCONFIG) or [2]this.DOC_DISPLAYCONFIG 
      **/
      var nodeProps = nodeWin.selectSingleNode(".//Properties");

      //set 'mute' of the window
      var propMute = nodeProps.getAttribute("mute");
      if(!jsx3.util.strEmpty(propMute)) {
        (propMute == "true") ? blkWin.mute(true) : blkWin.mute(false);
      }

      //get (1)'srcKbmCtrl' from property and get user profile from blkWin.streamURL(2)get user button status from 'remoteKbmCtrl' 
      //if all true, set localKBMSState to '1' otherwise set to '0'
      if(!jsx3.util.strEmpty(blkWin.streamURL)) {
        //(0)Stream URL - profile
        var propProfileKBMCtrl = blkWin.streamURL.match(/enablekbm=/) ? blkWin.streamURL.replace(/^.*enablekbm=(\w+).*$/, "$1") : "false";
        //(1)KBM server 
        var nodeConnContent = nodeWin.selectSingleNode(".//ConnectionContent");
        var propSrcKBMCtrl = nodeConnContent.getAttribute("srcKbmCtrl");
        //(2)button click
        var propRemoteKbm = nodeProps.getAttribute("remoteKbmCtrl");
        (propProfileKBMCtrl == "true") && (propSrcKBMCtrl == "true") && (propRemoteKbm == "true") ? blkWin.setLocalKBMSState(1) : blkWin.setLocalKBMSState(0);
      }

      //get 'scrollHPos'
      var propScrollHPos = nodeProps.getAttribute("scrollHPos");
      //get 'scrollVPos'
      var propScrollVPos = nodeProps.getAttribute("scrollVPos");
      if(!isNaN(parseInt(propScrollHPos)) && !isNaN(parseInt(propScrollVPos))) {
        blkWin.pan(propScrollHPos, propScrollVPos);
      }
      //set 'scale' of the window
      var propScale = nodeProps.getAttribute("scale");
      if(!jsx3.util.strEmpty(propScale)) {
        if(propScale == "fitNativeSize") {
          blkWin.setNativeSize();
        }
        else {
          blkWin.setScaleToWindow();
          if(propScale.indexOf('%') > 0) {
            blkWin.zoomInOut(propScale, propScale);
          }
        }
      }

      /**
      * Set fps when:
      * 1. current window is maximized; ORs
      * 2. no any 'maximize' window in current canvas;
      **/
      var propFPS = nodeProps.getAttribute("fps");
      if(!jsx3.util.strEmpty(this.canvasMaximizeWinId) && (winId != this.canvasMaximizeWinId)) {
        //have maximize, but not me,drop my fps to '0'
        blkWin.setFps(0);
      }
      else {
        if(!jsx3.util.strEmpty(propFPS)) {
          blkWin.setFps(parseInt(propFPS));
        }
      }
    }
    return bDirty;
  }

  //===================== Event Listener ======================================
  /**
  * onDisplayEvent: trigger applyCanvas after getting SUB
  */
  DisplayPanel_prototype.onCanvasEvent = function() {
    var nodeCanvas = this.clsApp.clsDisplayAgent.getCanvas(this.displayId);
    this._applyCanvas(nodeCanvas);
  }

});
