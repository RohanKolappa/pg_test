/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the display agent
*/
jsx3.Class.defineClass('com.ipvs.agent.DisplayAgent', null, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(DisplayAgent, DisplayAgent_prototype) {
  
  DisplayAgent.STR_DISPLAYCONFIG = '<DisplayConfig></DisplayConfig>';
  DisplayAgent.STR_DEFAULTDISPLAY = '<Display configurable="true"  hres="" vres=""/>';
  DisplayAgent.STR_DEFAULTPANEL = '<Panel id="Panel1" enable="" content="" ><Geometry aspect="" border="" hpos="0" hres="100%" transparency="" vpos="0" vres="100%" zIndex=""/></Panel>';
  DisplayAgent.STR_DEFAULTCANVAS = '<Canvas id="" hres="" vres=""/>';
  DisplayAgent.STR_DEFAULTWINDOW = '<Window id=" " content=""><ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId="" vres="" hres="" srcKbmCtrl=""></ConnectionContent><ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent><Properties zIndex="100" border="" vpos="0" hpos="0" vres="100%" hres="100%" aspect="" transparency="0" layoutId="" layoutPos="" visibility="true" maximize="false" fps="15" scale="fitWindow" mute="false" volume="50%" remoteKbmCtrl="false" scrollHPos="0" scrollVPos="0"/></Window>';  //<Geometry zIndex="1" border="" vpos="0" hpos="0" vres="100%" hres="100%" aspect="" transparency="0"></Geometry></Window>'


  // create Logger for this class
  DisplayAgent_prototype._LOG = jsx3.util.Logger.getLogger(DisplayAgent.jsxclass.getName());
  
  /**
  * The instance initializer.
  */
  DisplayAgent_prototype.init = function() {
    this._LOG.info("com.ipvs.agent.DisplayAgent.init");
    // init array of displayConfigs by displayId
    this.arrDisplayConfigByDisplayId = new Object;
    
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    } 
  }
  DisplayAgent_prototype._onAppInit = function() {
  };


  /**
  * getDefaultDisplayConfig: make a pre-configed display config for Odyssey client
  * @param respParams {JSON Object}:paramters including session information;
  * @return display config node {XMl Document}
  * @private @jsxobf-clobber 
  **/
  DisplayAgent_prototype.getDefaultDisplayConfig = function(displayId, panelId, canvasId, windowId) {
    this._LOG.info(":getDefaultDisplayConfig: windowId=" + windowId + ":canvasId=" + canvasId + "hres=100%; vres=100%");
    // Start with a displayConfig node
    var nodeDefaultDisplayConfig = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DISPLAYCONFIG);
    nodeDefaultDisplayConfig.setAttribute("hres", '100%');
    nodeDefaultDisplayConfig.setAttribute("vres", '100%');

    // Create Display node
    var nodeDisplay = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTDISPLAY);
    nodeDisplay.setAttribute("hres", '100%');
    nodeDisplay.setAttribute("vres", '100%');
    nodeDisplay.setAttribute("id", displayId);
    // Create Panel node
    var nodePanel = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTPANEL);
    nodePanel.setAttribute("id", panelId);
    var nodePanelGeometry = nodePanel.selectSingleNode(".//Geometry ");
    nodePanelGeometry.setAttribute("hpos", '0');
    nodePanelGeometry.setAttribute("vpos", '0');
    nodePanelGeometry.setAttribute("hres", '100%');
    nodePanelGeometry.setAttribute("vres", '100%');

    // Add Panel in Display
    nodeDisplay.appendChild(nodePanel);
    // Add Display in DisplayConfig
    nodeDefaultDisplayConfig.appendChild(nodeDisplay);

    // Create a Canvas node
    var nodeCanvas = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTCANVAS);
    nodeCanvas.setAttribute("id", canvasId);
    nodeCanvas.setAttribute("hres", '100%');
    nodeCanvas.setAttribute("vres", '100%');
    // Create a Window node
    var nodeWin = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTWINDOW);
    nodeWin.setAttribute("id", windowId);
    var nodeWindowProperties = nodeWin.selectSingleNode(".//Properties");
    nodeWindowProperties.setAttribute("hpos", '0');
    nodeWindowProperties.setAttribute("vpos", '0');
    nodeWindowProperties.setAttribute("hres", '100%');
    nodeWindowProperties.setAttribute("vres", '100%');

    // Add window in Canvas
    nodeCanvas.appendChild(nodeWin);
    // Add Canvas in DisplayConfig
    nodeDefaultDisplayConfig.appendChild(nodeCanvas);

    return nodeDefaultDisplayConfig;
  };

  /**
  * getDisplayConfig: get a Display by display id.
  * @param displayId {string}
  * @return Display {XML Node}
  */
  DisplayAgent_prototype.getDisplayConfig = function(displayId) {
    this._LOG.debug(":getDisplayConfig: displayId:" + displayId);
    return this.arrDisplayConfigByDisplayId[displayId];
  };

  /**
  * getDisplayConfigByWinId: get a Display by win id.
  * @param winId {string}
  * @return Display {XML Node}
  */
  DisplayAgent_prototype.getDisplayConfigByWinId = function(winId) {
    this._LOG.debug(":getDisplayConfigByWinId: winId:" + winId);
    for( var displayId in this.arrDisplayConfigByDisplayId ) {
      var nodeDisplayConfig = this.arrDisplayConfigByDisplayId[displayId];
      if( nodeDisplayConfig.selectSingleNode(".//Window[@id='"+winId+"']")) {
        return nodeDisplayConfig;
      }
    }
    return null;
  };

  /**
  * Set displayConfig for different clients;
  * @param displayId {String}:type of display, i.e. "VIEW" or "ARCHIVE"
  * @param nodeDisplayConfig {XML Node}
  */
  DisplayAgent_prototype.setDisplayConfig = function(displayId, nodeDisplayConfig) {
    this._LOG.debug(":setDisplayConfig: displayId:" + displayId);
    this.arrDisplayConfigByDisplayId[displayId] = nodeDisplayConfig;
  };

  /**
  * deleteDisplayConfig: get a Display by display id.
  * @param displayId {string}
  */
  DisplayAgent_prototype.deleteDisplayConfig = function(displayId) {
    this._LOG.debug(":deleteDisplayConfig: displayId:" + displayId);
    if (this.arrDisplayConfigByDisplayId[displayId]) {
      delete this.arrDisplayConfigByDisplayId[displayId];
    }
  };

  //  /**
  //  * Apply DisplayConfig for different clients;
  //  * @param displayId {String}:type of display, i.e. "VIEW" or "ARCHIVE"
  //  */
  //  DisplayAgent_prototype.applyDisplayConfig = function(displayId) {
  //    this._LOG.debug(":applyDisplayConfig: displayId:" + displayId);
  //    //publish event EVT_DISPLAY_displayId or EVT_CANVAS_displayId to trigger applyDisplay, applyCanvas
  //    this.doEvent("EVT_CANVAS-" + displayId, this);
  //  };


  /**
  * getCanvas: get a Canvas which mapping given a display id
  * @param displayId {string}
  * @return Canvas {XML Node}
  */
  DisplayAgent_prototype.getCanvas = function(displayId) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent.getCanvas with displayId:" + displayId);
    var nodeDisplayConfig = this.getDisplayConfig(displayId);
    var nodeCanvas = nodeDisplayConfig.selectSingleNode(".//Canvas");
    return nodeCanvas;
  };

  /**
  * setDisplayConfigWinProp
  * @param propElement{String}: "null/ConnectionContent"/ "Properties"
  * @param propName{String}: "maximize"/ "scale"
  * @param propValue{String}: "true"/ "100%"
  * @return current canvasNode
  */
  DisplayAgent_prototype.setDisplayConfigWinProp = function(winId, propElement, propName, propValue) {
    var nodeDisplayConfig = this.getDisplayConfigByWinId(winId);
    var nodeProps = nodeDisplayConfig.selectSingleNode(".//Window[@id='" + winId + "']");
    if( propElement ) {
      var nodeProps = nodeProps.selectSingleNode(".//"+propElement);
    }
    nodeProps.setAttribute(propName, propValue);
  }

});