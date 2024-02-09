/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the display agent
*/
jsx3.require("com.ipvs.gui.MediaPlayerWindow", "com.ipvs.util.Common");
jsx3.Class.defineClass('com.ipvs.agent.DisplayAgent', null, null, function (DisplayAgent, DisplayAgent_prototype) {

  DisplayAgent.STR_DEFAULTDISPLAY = '<Display configurable="true"  hres="" vres=""/>';
  DisplayAgent.STR_DEFAULTPANEL = '<Panel id="Panel1" enable="" content="" ><Geometry aspect="" border="" hpos="0" hres="100%" transparency="" vpos="0" vres="100%" zIndex=""/></Panel>';
  DisplayAgent.STR_DEFAULTCANVAS = '<Canvas id="" hres="" vres=""/>';
  DisplayAgent.STR_DEFAULTWINDOW = '<Window id=" " content=""><ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId="" vres="" hres="" srcKbmCtrl=""></ConnectionContent><ThumbnailContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" imageURL="" refreshInterval=""></ThumbnailContent><Properties zIndex="100" border="" vpos="0" hpos="0" vres="100%" hres="100%" aspect="" transparency="0" layoutId="" layoutPos="" visibility="true" maximize="false" fps="" scale="fitWindow" mute="false" volume="50%" remoteKbmCtrl="false" scrollHPos="0" scrollVPos="0"/></Window>';  //<Geometry zIndex="1" border="" vpos="0" hpos="0" vres="100%" hres="100%" aspect="" transparency="0"></Geometry></Window>'
  DisplayAgent.STR_DEFAULTCONNECTIONCONTENT = '<ConnectionContent MediaStreamIOPortTypeTitle="" MediaStreamIOPortElementID="" sessionId="" connId="" vres="" hres="" srcKbmCtrl=""></ConnectionContent>';
  DisplayAgent.STR_PORTDISPLAYCONFIG = '<IPVSClientDisplayConfig></IPVSClientDisplayConfig>';
  DisplayAgent.STR_PORTDISPLAYSTATUS = '<IPVSClientDisplayStatus></IPVSClientDisplayStatus>';
  DisplayAgent.STR_PORTDISPLAYCONFIGREQ = '<SetServiceClientDisplayConfig></SetServiceClientDisplayConfig>';
  DisplayAgent.STR_PORTDISPLAYSTATUSREQ = '<SetServiceClientDisplayStatus></SetServiceClientDisplayStatus>';
  DisplayAgent.MAX_ZINDEX = 9999999;
  // create Logger for this class
  DisplayAgent_prototype._LOG = jsx3.util.Logger.getLogger(DisplayAgent.jsxclass.getName());

  /**
  * The instance initializer.
  * @param eventApp {Object}
  */
  DisplayAgent_prototype.init = function (eventApp) {

    this._LOG.debug("com.ipvs.agent.DisplayAgent.init");

    // Display Config Document
    this.DOC_PORTDISPLAYCONFIG = new jsx3.xml.Document().loadXML(DisplayAgent.STR_PORTDISPLAYCONFIG);
    // Display Status Document
    this.DOC_PORTDISPLAYSTATUS = new jsx3.xml.Document().loadXML(DisplayAgent.STR_PORTDISPLAYSTATUS);
    // DB Request Document to update Display Config 
    this.DOC_PORTDISPLAYCONFIGREQ = new jsx3.xml.Document().loadXML(DisplayAgent.STR_PORTDISPLAYCONFIGREQ);
    // DB Request Document to update Display Status 
    this.DOC_PORTDISPLAYSTATUSREQ = new jsx3.xml.Document().loadXML(DisplayAgent.STR_PORTDISPLAYSTATUSREQ);

    // list (assoc. array) of Display Panels
    this.arrDisplayPanels = {};

    //keep handler of window including sessionId,mediaRoomNID,topic,and window handler
    this.arrSessionWindows = {};

    //keep handler of window by winId
    this.arrWindowsByWinId = {};

    //keep SSCmd handler of player by winId
    this.arrSSCmdByWinId = {};

    //canvas id
    this.canvasId = null;
    this.canvasMaxWinId = null;
    this.canvasMaxWinVisibility = true;
    //flag of players' mode, default/'true': window mode, 'false' windowless mode
    this.canvasPlayerModeFlg = true;

    //a open flag, set it to 'true'  while open a panel, 'false' while close the panel
    this.isOpen = false;
    if (!jsx3.util.strEmpty(eventApp)) {
      // setup event subscriptions
      this.eventWrap(eventApp);
    }
  };

  /**
  * The open method.
  * Execute while open full screen window
  */
  DisplayAgent_prototype.open = function () {
    this._LOG.debug("com.ipvs.agent.DisplayAgent.open");
    //Set all 'bDirty' to 'true' to fix the EPC notification bug 28550
    var bDirtyDisplay = true;
    var bDirtyCanvas = true;
    var nodeDisplay = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Display");
    if (nodeDisplay != null) {
      // render panel windows based on Display info
      bDirtyDisplay |= this._applyDisplay(nodeDisplay);

      var nodeCanvas = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Canvas");
      if (nodeCanvas != null) {
        // render connection windows based on Canvas info
        bDirtyCanvas |= this._applyCanvas(nodeCanvas);
        this.isOpen = true;
        // after open set display status to DB
        this._updateDBPortDisplayStatus(bDirtyDisplay, bDirtyCanvas);
        // go thru the arrSSCmdByWinId if any sscmd was missed
        this._processArrSSCmdByWinId();
      }
      else {
        this._LOG.error("com.ipvs.agent.DisplayAgent.open:nodeCanvas=" + nodeCanvas);
      }
    }
    else {
      this._LOG.error("com.ipvs.agent.DisplayAgent.open:nodeDisplay=" + nodeDisplay);
    }
  };

  /**
  * The close method.
  * Execute while closing full screen window
  */
  DisplayAgent_prototype.close = function () {
    this._LOG.debug("com.ipvs.agent.DisplayAgent.close");
    this.isOpen = false;
    var nodeCanvas = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Canvas");
    //TBD: when there are many panel, need find window & canvas on that panel
    // go thru the current connectionWindow array delete all windows on this panel
    for (var winId in this.arrWindowsByWinId) {
      // for each window check if the winId is still in the current Canvas
      if (nodeCanvas.selectSingleNode(".//Window[@id='" + winId + "']")) {
        // if not found - delete it
        this._deleteWindow(winId);
        bDirty = true;
      }
    }
    for (var winId in this.arrSSCmdByWinId) {
      // for each window check if there are attached SSCmd, if have clean them
      delete this.arrSSCmdByWinId[winId];
    }
    // delete all canvs on this panel
    // delete all panels, only one for now
    for (var panelId in this.arrDisplayPanels) {
      this._deletePanel(panelId);
    }
  };

  /**
  * Sets the DeviceNID for Unmanned Client locally;
  * @param deviceNID {String} 
  */
  DisplayAgent_prototype.setDeviceNID = function (deviceNID) {
    this.deviceNID = deviceNID;
    return this;
  }

  /**
  * A place holder to check evironment setting required for UMC;
  */
  DisplayAgent_prototype.checkSettings = function () {
    var dpiSetting = readDPISetting();
    if (!jsx3.util.strEmpty(dpiSetting) && dpiSetting != "96") {
      alert("Your Display DPI setting is not the recommended value. Please set it to the default Windows setting (96) and retry.");
      return false;
    }
    else {
      this._LOG.info("Your DPI Setting is " + dpiSetting);
      return true;
    }
  }

  /**
  * Sets/Applies Local DisplayConfig for a stand alone client.
  * - Will also update the config in the DB - unless bNoDBConfigUpdate is true
  * - Apply of the DisplayConfig also results in a DisplayStatus update
  * @param nodeDisplayConfig {XML Node}
  * @param bNoDBConfigUpdate {boolean} - if true, no DB Config Update needed
  */
  DisplayAgent_prototype.setPortDisplayConfig = function (nodeDisplayConfigObj, bNoDBConfigUpdate) {
    var nodeDisplayConfig = nodeDisplayConfigObj.nodeDisplayConfig;
    var clientInfoObj = nodeDisplayConfigObj.clientInfoObj;

    this._LOG.debug("com.ipvs.agent.DisplayAgent.setPortDisplayConfig");
    var bDirtyDisplay = false;
    var bDirtyCanvas = false;
    // get Display element from input param and append/replace it in local Doc
    var nodeDisplay = nodeDisplayConfig.selectSingleNode(".//Display");
    if (nodeDisplay != null) {
      var nodePanel = nodeDisplay.selectSingleNode(".//Panel");
      if (!nodePanel) {
        var nodePanel = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTPANEL);
        nodeDisplay.appendChild(nodePanel);
      }
      else {
        //make sure there is 'panelId' in nodePanel
        var panelId = nodePanel.getAttribute('id');
        if (jsx3.util.strEmpty(panelId)) {
          //TBD-HACK: making sure there is a valid 'panelId' Display element
          //assuming there is only one panel for now
          nodePanel.setAttribute("id", "Panel1");
        }
      }
      var nodeDisplayOld = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Display");
      if (nodeDisplayOld == null) {
        this.DOC_PORTDISPLAYCONFIG.appendChild(nodeDisplay.cloneNode(true));
      }
      else {
        this.DOC_PORTDISPLAYCONFIG.replaceNode(nodeDisplay.cloneNode(true), nodeDisplayOld);
      }
      if (this.isOpen) {
        // render panel windows based on Display info
        bDirtyDisplay |= this._applyDisplay(nodeDisplay);
      }
    }

    // get Canvas element from input param and append/replace it in local Doc
    var nodeCanvas = nodeDisplayConfig.selectSingleNode(".//Canvas");
    if (nodeCanvas != null) {
      var nodeCanvasOld = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Canvas");
      if (nodeCanvasOld == null) {
        this.DOC_PORTDISPLAYCONFIG.appendChild(nodeCanvas.cloneNode(true));
      }
      else {
        this.DOC_PORTDISPLAYCONFIG.replaceNode(nodeCanvas.cloneNode(true), nodeCanvasOld);
      }
      // make sure the Canvas status is initialized
      if (!this.DOC_PORTDISPLAYSTATUS.selectSingleNode(".//Canvas")) {
        this.DOC_PORTDISPLAYSTATUS.appendChild(nodeCanvas.cloneNode(true));
      }
      if (this.isOpen) {
        // render connection windows based on Canvas info
        bDirtyCanvas |= this._applyCanvas(nodeCanvas);
      }

    }

    //send the response
    if (!jsx3.util.strEmpty(clientInfoObj)) {
      this._sendSetDisplayConfigResponse(clientInfoObj);
    }

    // updating status first. Client UI is listening to status notification
    this._updateDBPortDisplayStatus(bDirtyDisplay, bDirtyCanvas);

    // update the config in the DB - unless requested not to do so
    if (!bNoDBConfigUpdate) {
      this._updateDBPortDisplayConfig(bDirtyDisplay, bDirtyCanvas);
    }



  };

  DisplayAgent_prototype._sendSetDisplayConfigResponse = function (clientInfoObj) {
    var userJID = clientInfoObj.userJID.getValue();
    var requestNID = clientInfoObj.requestNID.getValue();
    var clientdata = clientInfoObj.clientdata.getValue();

    // error code = 1039
    var errorCode = "";
    //error description = CAN_NOT_APPLY_CONFIG 
    var errorDescription = "";
    var state = "8";



    /*
    var responseStr = "<Response><header><servicename>DeviceAdmin</servicename><requestname>DeviceConfigurationRequest</requestname>" +
    "<userJID>" + userJID + "</userJID>" +
    "<requestNID>" + requestNID + "</requestNID>" +
    "<state>" + state + "</state>" +
    "<clientdata>" + clientdata + "</clientdata>" +
    //ref. story #46274 get from previous <agentmessageheader> <agent> element
    "<clientcallback>com.ipvs.mediaroomservice.impl.AppServerAgent</clientcallback>" +
    "</header><data><DeviceConfigurationResponseData><Status>" +
    "<Code>" + errorCode + "</Code>" +
    "<Description>" + errorDescription + "</Description>" +
    "</Status></DeviceConfigurationResponseData></data><log/></Response>";
    */

    var responseStr = '"Response": {"header": { "servicename": "DeviceAdmin", "requestname": "DeviceConfigurationRequest", "userJID": "' + userJID + '", "requestNID": "' + requestNID + '", ' +
                                               '"state": "' + state + '", "clientdata": "' + clientdata + '", "clientcallback": "com.ipvs.mediaroomservice.impl.AppServerAgent" },' +
                          '"data": {' +
                           ' "DeviceConfigurationResponseData": { "Status": { "Code": "' + errorCode + '", "Description": "' + errorDescription + '"} }' +
                          '},' +
                          '"log": ""' +
                       '}';

    //    var agentReqDataNodeDoc = new jsx3.xml.Document();
    //    agentReqDataNodeDoc.loadXML(responseStr);
    //    var agentReqDataNode = agentReqDataNodeDoc.selectSingleNode("/*").cloneNode(true);

    var AgentMessageHeader = new Object;
    //from owner userJID
    AgentMessageHeader.from = topic_Get(tApp, "LoginUserJID");
    AgentMessageHeader.to = topic_Get(tApp, "AppServerAgentJID"); ;
    AgentMessageHeader.type = "Response";

    var AgentMessageDataObjectArr = new Object;
    AgentMessageDataObjectArr["agentmessagedata"] = responseStr;

    var urlStr = 'ipvs:///AgentMessage?ServiceName=systemservice' +
                 '&OpType=Operation' +
                 '&ObjectType=AgentEvent';

    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(urlStr);
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        var response = req.getResponseXML();

      }
      else {

        log.error("DisplayAgent::_sendSetDisplayConfigResponse:: error. Error code:" + respStatus);
      }
      delete serviceAppletReq;
    }

    req.subscribe("*", this, onRequestEvent);
    log.info("Sending _sendSetDisplayConfigResponse...");
    // attach agentmessageheader to the request
    req.setAgentMessageHeader(AgentMessageHeader);
    // attach the AgentMessageDataObjectArr to the request
    req.setAgentMessageDataObjectArr(AgentMessageDataObjectArr);

    req.send();
  };

  // ---- notification of 'setPortWindowProperties' replace 'setPortWindowContent' & 'setPortWindowVisible' ------- 
  /**
  * Sets/Applies the content for the window referenced by winId in the local DisplayConfig
  * - Will also update the config in the DB
  * - Apply of the DisplayConfig also results in a DisplayStatus update
  * @param objPlayerCmd {Object}
  */
  DisplayAgent_prototype.setPortWindowContent = function (objPlayerCmd) {
    this._LOG.info("::setPortWindowContent:windowId=" + objPlayerCmd.windowId + ":connId=" + objPlayerCmd.connId);
    var bDirtyDisplay = false;
    var bDirtyCanvas = false;
    // get Window from local Config Doc
    var nodeWin = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Window[@id='" + objPlayerCmd.windowId + "']");

    if (nodeWin) {
      //update 'Window'/'ConnectionContent' if possible (after the player get streamURL and start)
      bDirtyCanvas |= this._applyConnectionContent(objPlayerCmd);
      //update player/window 'Properties'
      bDirtyCanvas |= this._applyPlayerProps(objPlayerCmd.windowId);
    }

    // set the Status to match the config change if needed
    this._updateDBPortDisplayStatus(bDirtyDisplay, bDirtyCanvas);
  };


  /**
  * process ssCms for the windows. there is a chance that ssCmd came before loading the window
  * 
  */
  DisplayAgent_prototype._processArrSSCmdByWinId = function () {
    // go thru the saved ssCmd's
    for (var winId in this.arrSSCmdByWinId) {
      var blkWin = this.arrWindowsByWinId[winId];
      if (!blkWin) {
        this._LOG.info(" doAction:store objPlayerCmd at arrSSCmdByWinId[" + objPlayerCmd.windowId + "]");
        //send delete connection from SSCmd
        jsx3.sleep(function () { agtMediaStreamDst.sendSSCmdDeleConn(objPlayerCmd.windowId), null, this });
      }
      else {
        //trigger any miss SSCmd
        var objPlayerCmd = this.arrSSCmdByWinId[winId];
        if (typeof (objPlayerCmd) != 'undefined') {
          this._LOG.info(" _addWindow:start:URL" + objPlayerCmd.streamURL + " :winId: " + objPlayerCmd.windowId + " :connId: " + objPlayerCmd.connId);
          blkWin.objPlayerCmd = objPlayerCmd;
          blkWin.start(objPlayerCmd.streamURL);
          blkWin.streamURL = objPlayerCmd.streamURL;
          delete this.arrSSCmdByWinId[winId];
        }
      }
    }
  }

  /**
  * apply ConnectionContent by properties, these properties only effect after player running
  * @param objPlayerCmd (object)
  */
  DisplayAgent_prototype._applyConnectionContent = function (objPlayerCmd) {
    var bDirtyConnContent = false;
    var blkWin = this.arrWindowsByWinId[objPlayerCmd.windowId];
    // get window node of this.DOC_PORTDISPLAYSTATUS
    var nodeWinStat = this._getPortDisplayStatus_WindowNode(objPlayerCmd.windowId);
    this._LOG.info("::_applyConnectionContent:winId: " + objPlayerCmd.windowId);
    //if 'blkWin' exist and the player loaded, then apply player properties
    if (blkWin && blkWin.IPVideoPlayer) {
      //get player native width and height set to 'ConnectionContent'/'hres' & 'vres'
      var plyNativeHres = blkWin.IPVideoPlayer.GetNativeWidth() > 0 ? blkWin.IPVideoPlayer.GetNativeWidth() : blkWin.getWidth();
      var plyNativeVres = blkWin.IPVideoPlayer.GetNativeHeight() > 0 ? blkWin.IPVideoPlayer.GetNativeHeight() : blkWin.getHeight();

      var sessionId = '';
      if (!jsx3.util.strEmpty(objPlayerCmd.sessionId)) {
        sessionId = objPlayerCmd.sessionId;
      }
      var connId = '';
      if (!jsx3.util.strEmpty(objPlayerCmd.connId)) {
        connId = objPlayerCmd.connId;
      }

      // update the 'Window' 'content' info
      nodeWinStat.setAttribute("content", "CONNECTION");
      // update window status of this.DOC_PORTDISPLAYSTATUS when player loaded successful
      var nodeWinConnContent = nodeWinStat.selectSingleNode(".//ConnectionContent");
      nodeWinConnContent.setAttribute("sessionId", sessionId);
      nodeWinConnContent.setAttribute("connId", connId);
      nodeWinConnContent.setAttribute("vres", plyNativeVres);
      nodeWinConnContent.setAttribute("hres", plyNativeHres);

      //get user profile setting from streamURL
      var enableKbm = objPlayerCmd.streamURL.replace(/^.*enablekbm=(\w+).*$/, "$1");

      //'srcKbmCtrl': set 'true' if IsRFBReady and 'enableKbm' are both 'true'
      srcKbmCtrl = (blkWin.IPVideoPlayer.IsRFBReady) && (enableKbm == "true") ? "true" : "false";
      nodeWinConnContent.setAttribute("srcKbmCtrl", srcKbmCtrl);
      bDirtyConnContent = true;
    }
    else {
      this._LOG.warn("::_applyConnectionContent:winId: " + objPlayerCmd.windowId + ":blkWin:" + blkWin + ": blkWin or IPVideoPlayer is not ready.");
    }
    return bDirtyConnContent;
  }


  /**
  * Reset ConnectionContent to initial values
  * - Will also update the config in the DB
  * - Apply of the DisplayConfig also results in a DisplayStatus update
  * @param winId {string}
  * @param sessionId {string}
  * @param connId {string}
  */
  DisplayAgent_prototype.resetPortWindowContent = function (objPlayerCmd) {
    this._LOG.info("::resetPortWindowContent:windowId=" + objPlayerCmd.windowId + ":connId=" + objPlayerCmd.connId);
    // get window node of this.DOC_PORTDISPLAYSTATUS
    var nodeWinStat = this._getPortDisplayStatus_WindowNode(objPlayerCmd.windowId);
    nodeWinStat.setAttribute("content", "");
    var nodeWinConnContent = nodeWinStat.selectSingleNode(".//ConnectionContent");
    nodeWinConnContent.setAttribute("sessionId", "");
    nodeWinConnContent.setAttribute("connId", "");
    nodeWinConnContent.setAttribute("vres", "");
    nodeWinConnContent.setAttribute("hres", "");
    nodeWinConnContent.setAttribute("srcKbmCtrl", "");
    // reset player properties (refer to #32532)
    this._applyPlayerProps(objPlayerCmd.windowId);
    // set the Status to match the config change if needed
    this._updateDBPortDisplayStatus(true, true);
    // reset the connection content after session stop
    var nodeCfgWindowConn = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Window[@id='" + objPlayerCmd.windowId + "']/ConnectionContent");
    if (!jsx3.util.strEmpty(nodeCfgWindowConn)) {
      nodeCfgWindowConn.setAttribute("sessionId", "");
      nodeCfgWindowConn.setAttribute("connId", "");
      nodeCfgWindowConn.setAttribute("vres", "");
      nodeCfgWindowConn.setAttribute("hres", "");
      nodeCfgWindowConn.setAttribute("srcKbmCtrl", "");
    }
    this._updateDBPortDisplayConfig(true, true);
  };

  /**
  * Sets/Applies the properties for the window referenced by winId in the local DisplayConfig
  * - Will also update the config in the DB
  * - Apply of the DisplayConfig also results in a DisplayStatus update
  * @param nodeIDProps {object}
  * @param nodeWindowsProps {object}
  */
  DisplayAgent_prototype.setPortWindowProperties = function (winId, nodeWinProps, clientInfoObj) {
    this._LOG.info(":setPortWindowProperties:winId:" + winId + " :nodeWinProps:" + nodeWinProps.toString());
    var bDirtyDisplay = false;
    var bDirtyCanvas = false;

    //setPortWindowProperties to display config
    var nodeCanvas = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Canvas");
    var nodeWinPropsConfig = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Window[@id='" + winId + "']/Properties");
    if (!nodeWinPropsConfig) return;

    var attributeNameArr = nodeWinProps.getAttributeNames();
    for (var i = 0; i < attributeNameArr.length; i++) {
      var attrName = attributeNameArr[i];
      var propVal = nodeWinProps.getAttribute(attrName);
      // enforce only one maximized window
      if (attrName == 'maximize' && propVal == 'true') {
        // if this window has maximize attr - clear maximize attr from all windows
        var iterMaxWinProp = nodeCanvas.selectNodeIterator(".//Window/Properties[@maximize='true']");
        while (iterMaxWinProp.hasNext()) {
          var nodeMaxWinProp = iterMaxWinProp.next();
          nodeMaxWinProp.setAttribute('maximize', 'false');
        }
      }
      nodeWinPropsConfig.setAttribute(attrName, propVal);
    }

    bDirtyCanvas |= this._applyCanvas(nodeCanvas);

    //send the response
    this._sendSetDisplayConfigResponse(clientInfoObj);

    if (bDirtyCanvas) {
      // updating status first client ui is listening to status notification
      this._updateDBPortDisplayStatus(bDirtyDisplay, bDirtyCanvas);
      // update the config in the DB - unless requested not to do so
      this._updateDBPortDisplayConfig(bDirtyDisplay, bDirtyCanvas);
    }

  };

  /**
  * Gets Display node from DisplayStatus
  * @return nodeDisplayStat (Entity)
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._getPortDisplayStatus_DisplayNode = function () {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._getPortDisplayStatus_DisplayNode");
    var nodeDisplayStat = this.DOC_PORTDISPLAYSTATUS.selectSingleNode(".//Display");
    if (!nodeDisplayStat) {
      nodeDisplayStat = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTDISPLAY);
      this.DOC_PORTDISPLAYSTATUS.appendChild(nodeDisplayStat);
    }
    return nodeDisplayStat;
  };

  /**
  * Gets Canvas node from DisplayStatus
  * @return nodeCanvasStat (Entity)
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._getPortDisplayStatus_CanvasNode = function () {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._getPortDisplayStatus_CanvasNode");
    var nodeCanvasStat = this.DOC_PORTDISPLAYSTATUS.selectSingleNode(".//Canvas");
    if (!nodeCanvasStat) {
      nodeCanvasStat = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTCANVAS);
      this.DOC_PORTDISPLAYSTATUS.appendChild(nodeCanvasStat);
    }
    return nodeCanvasStat;
  };

  /**
  * Gets Panel node from DisplayStatus
  * @param panelId (String)
  * @return nodePanel (Entity)
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._getPortDisplayStatus_PanelNode = function (panelId) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._getPortDisplayStatus_PanelNode: " + panelId);
    var nodeDisplayStat = this._getPortDisplayStatus_DisplayNode();

    var nodePanelStat = nodeDisplayStat.selectSingleNode(".//Panel[@id='" + panelId + "']");
    if (!nodePanelStat) {
      nodePanelStat = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTPANEL);
      nodePanelStat.setAttribute("id", panelId);
      nodeDisplayStat.appendChild(nodePanelStat);
    }
    return nodePanelStat;
  };

  /**
  * Gets Window node from DisplayStatus
  * @param winId (String)
  * @return nodeWinStat (Entity)
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._getPortDisplayStatus_WindowNode = function (winId) {
    var nodeCanvasStat = this._getPortDisplayStatus_CanvasNode();
    var nodeWinStat = nodeCanvasStat.selectSingleNode(".//Window[@id='" + winId + "']");
    if (!nodeWinStat) {
      nodeWinStat = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTWINDOW);
      nodeWinStat.setAttribute("id", winId);
      nodeCanvasStat.appendChild(nodeWinStat);
    }
    else {
      if (!nodeWinStat.selectSingleNode(".//ConnectionContent")) {
        //add ConnectionContent element as the first child of Windows
        var nodeWinConnContent = new jsx3.xml.Document().loadXML(DisplayAgent.STR_DEFAULTCONNECTIONCONTENT);
        var firstChild = nodeWinStat.getFirstChild();
        nodeWinStat.insertBefore(nodeWinConnContent, firstChild);
        this._LOG.info("_getPortDisplayStatus_WindowNode: add ConnectionContent to Window:" + winId + " :nodeWinStat: " + nodeWinStat.toString());
      }
    }
    this._LOG.info("_getPortDisplayStatus_WindowNode: winId:" + winId + " :nodeWinStat: " + nodeWinStat.toString());
    return nodeWinStat;
  };

  //===================== Display / Panel Methods ======================================
  /**
  * open one browser window for every Panel in the Display
  * @param nodeDisplay {Object}
  */
  DisplayAgent_prototype._applyDisplay = function (nodeDisplay) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._applyDisplay");
    var bDirty = false;
    // go thru the current displayPanel array
    for (var panelId in this.arrDisplayPanels) {
      // for each panel check if the panelId is still in the current Display
      if (!nodeDisplay.selectSingleNode(".//Panel[@id='" + panelId + "']")) {
        // if not found - delete it
        this._deletePanel(panelId);
        bDirty = true;
      }
    }
    // go thru the Panels in the current Display
    var iterPanels = nodeDisplay.selectNodeIterator(".//Panel");
    while (iterPanels.hasNext()) {
      // for each panel
      var nodePanel = iterPanels.next();
      var panelId = nodePanel.getAttribute("id");
      // if it does not exist in the display - add it
      if (!this.arrDisplayPanels[panelId]) {
        //node 'Panel' still using 'Geometry' 
        var nodeGeometry = nodePanel.selectSingleNode(".//Geometry");
        this._addPanel(panelId, nodeGeometry);
        bDirty = true;
      }
    }
    return bDirty;
  }

  /**
  * add a new panel into the current display
  * panels are rendered as browser windows
  * @param panelId {string}
  * @param nodeGeometry (Entity)
  * @return window instance blkJSX
  */
  DisplayAgent_prototype._addPanel = function (panelId, nodeGeometry) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._addPanel: " + panelId);
    var winPanel = PNameSpace.getAppWindow(panelId);
    if (!winPanel) {
      // create a new Panel instance 
      winPanel = PNameSpace.createAppWindow(panelId); //TBD: new com.ipvs.gui.WinDisplayPanel(panelId);
    }
    // save a reference to it in the list
    this.arrDisplayPanels[panelId] = winPanel;
    // open it - hand coded instead of calling the GI window class open to deal with fullscreen open
    // URI for open
    var uri = jsx3.util.strEndsWith(jsx3.app.Browser.getLocation().getPath().toLowerCase(), ".xhtml") ?
        "jsx3.gui.window.xhtml" : "jsx3.gui.window.html";
    // param string for open - defaults no chrome
    var winParamStr = "directories=no,location=no,menubar=no,status=no,personalbar=no,titlebar=no,toolbar=no,resizable=no,scrollbars=no";
    // if no geometry - or it is AUTO / 100% - use fullscreen

    //TBD: HACK - for now ignore incoming Geometry
    nodeGeometry = null;

    if ((!nodeGeometry) ||
        (nodeGeometry.getAttribute("hres") == "100%" && nodeGeometry.getAttribute("vres") == "100%") ||
        (nodeGeometry.getAttribute("hres") == "AUTO" && nodeGeometry.getAttribute("vres") == "AUTO")) {
      winParamStr += ",left=0,top=0,fullscreen=yes";
      // set the width and height to match the screen width and height
      winPanel.setWidth(screen.availWidth);
      winPanel.setHeight(screen.availHeight);
      this._LOG.info("_addPanel: screen.availWidth=" + screen.availWidth);
      this._LOG.info("_addPanel: screen.availHeight=" + screen.availHeight);
    }
    else {
      // get hres
      var hres = nodeGeometry.getAttribute("hres");
      if (hres == "AUTO") {
        hres = screen.availWidth;
      }
      else if (hres.indexOf('%') > 0) {
        var hFactor = ((hFactor = hres.replace(/%$/, "") / 100) > 1) ? 1 : hFactor;
        hres = screen.availWidth * hFactor;
      }
      else if (hres > screen.availWidth) {
        hres = screen.availWidth;
      }
      // get vres
      var vres = nodeGeometry.getAttribute("vres");
      if (vres == "AUTO") {
        vres = screen.availHeight;
      }
      else if (vres.indexOf('%') > 0) {
        var vFactor = ((vFactor = vres.replace(/%$/, "") / 100) > 1) ? 1 : vFactor;
        vres = screen.availHeight * vFactor;
      }
      else if (vres > screen.availWidth) {
        vres = screen.availHeight;
      }
      // get pos
      var hpos = 0; //nodeGeometry.getAttribute("hpos");
      var vpos = 0; //nodeGeometry.getAttribute("vpos");
      winParamStr += ",left=" + hpos + ",top=" + vpos + ",width=" + hres + ",height=" + vres;
      // set the width and height to match geometry width and height
      winPanel.setWidth(hres);
      winPanel.setHeight(vres);
    }
    // open a new window
    var w = window.open(uri, winPanel.getId().replace(/\./g, ""), winParamStr);
    // hook the new window to the GI window instance
    if (w != null) {
      w["_jsxwindow"] = winPanel;
      winPanel._jsxwindow = w;
      winPanel._jsxopener = window;
    }

    // set black background
    winPanel.getRootBlock().setBackgroundColor("#00171F");

    // setup subscription to destroy it before close
    winPanel.subscribe(jsx3.gui.Window.WILL_CLOSE, this, this.close);

    // update panel status
    var nodePanelStat = this._getPortDisplayStatus_PanelNode(panelId);
    var nodeGeoStat = nodePanelStat.selectSingleNode(".//Geometry");
    nodeGeoStat.setAttribute("hpos", winPanel.getWindow()["screenLeft"]);
    nodeGeoStat.setAttribute("vpos", winPanel.getWindow()["screenTop"]);
    nodeGeoStat.setAttribute("hres", winPanel.getWidth());
    nodeGeoStat.setAttribute("vres", winPanel.getHeight());

    // also update overall display dims (addup all hres, keep largest vres)
    var nodeDisplayStat = this._getPortDisplayStatus_DisplayNode();
    //init 'hres' & 'vres' value are ""
    var hres = jsx3.util.strEmpty(nodeDisplayStat.getAttribute("hres")) ? winPanel.getWidth() : nodeDisplayStat.getAttribute("hres");
    nodeDisplayStat.setAttribute("hres", hres);
    var vres = jsx3.util.strEmpty(nodeDisplayStat.getAttribute("vres")) ? winPanel.getHeight() : nodeDisplayStat.getAttribute("vres");
    nodeDisplayStat.setAttribute("vres", vres);
    return winPanel;
  }

  /**
  * deletes an existing panel from the current display
  * @param panelId {string}
  * @return window instance blkJSX
  */
  DisplayAgent_prototype._deletePanel = function (panelId) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._deletePanel: " + panelId);
    // get the current display window
    var winPanel = this.arrDisplayPanels[panelId];
    winPanel.unsubscribe(jsx3.gui.Window.WILL_CLOSE, this.close);
    // and deref it from the list
    delete this.arrDisplayPanels[panelId];
    // update panel status
    var nodePanelStat = this._getPortDisplayStatus_PanelNode(panelId);
    nodePanelStat.getParent().removeChild(nodePanelStat);
    //mapping _addPanel, restore 'hres' & 'vres' to initial value ""
    var nodeDisplayStat = this._getPortDisplayStatus_DisplayNode();
    nodeDisplayStat.setAttribute("hres", "");
    nodeDisplayStat.setAttribute("vres", "");
    return winPanel;
  }

  //===================== Canvas Methods ======================================
  /**
  * render the window layout based on the current display config
  * @param nodeCanvas {Object}
  */
  DisplayAgent_prototype._applyCanvas = function (nodeCanvas) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._applyCanvas");

    // drop all player fps = 0 to allow GUI change to have priority
    this._dropFps();

    /**
    * Performance HACK: If all zIndex=0, set this.canvasPlayerModeFlg to true (window mode)
    * this setting take effect only before player loaded, calling from 'open',
    **/
    var iterWindowsForZIndex = nodeCanvas.selectNodeIterator(".//Window/Properties[@zIndex>0]");
    this.canvasPlayerModeFlg = (!iterWindowsForZIndex.hasNext());

    var bDirty = false;
    // get the Canvas geometry and apply it on our display
    var bDirty = this._applyCanvasLayout(nodeCanvas);

    // go thru the current Window array - remove any unneeded windows
    for (var winId in this.arrWindowsByWinId) {
      // for each window check if the winId is still in the current Canvas
      if (!nodeCanvas.selectSingleNode(".//Window[@id='" + winId + "']")) {
        // if not found - delete it
        this._deleteWindow(winId);

        bDirty = true;
        continue;
      }
    }

    // convert window level maximize attr to canvas level - canvasMaxWinId and canvasMaxWinVisibility
    // further apply will use this - not the individual window level attr
    var nodesMaxWin = nodeCanvas.selectNodes(".//Window[Properties[@maximize='true']]");
    if (nodesMaxWin.size() == 0) {
      this.canvasMaxWinId = null;
      this.canvasMaxWinVisibility = true;
    }
    else if (nodesMaxWin.size() == 1) {
      this.canvasMaxWinId = nodesMaxWin.getItem(0).getAttribute('id');
      this.canvasMaxWinVisibility = (nodesMaxWin.getItem(0).selectSingleNode(".//Properties").getAttribute('visibility') == "false") ? false : true;
    }
    else {
      // no change if more than one window with maximize attr
    }

    //stash canvasId (only one canvasId at a run time)
    var canvasId = nodeCanvas.getAttribute("id");
    this.canvasId = canvasId;
    // get canvas dimensions
    var hresCanvas = nodeCanvas.getAttribute("hres");
    var vresCanvas = nodeCanvas.getAttribute("vres");
    // go thru the Windows in the current Canvas
    var iterWindows = nodeCanvas.selectNodeIterator(".//Window");
    while (iterWindows.hasNext()) {
      // for each window
      var nodeWin = iterWindows.next();
      var winId = nodeWin.getAttribute("id");
      var nodeProps = nodeWin.selectSingleNode(".//Properties");
      // update the layout for this window - this will call apply window/player props
      bDirty |= this._applyWindowLayout(winId, nodeProps, hresCanvas, vresCanvas);
    }

    // set all the players Fps
    this._setFps();

    return bDirty;
  }

  /**
  * Get current canvas block for the specified panel
  * @return canvas block handle or null if not found
  */
  DisplayAgent_prototype._getCanvas = function (panelId) {
    // get the matching panel window
    var winPanel = this.arrDisplayPanels[panelId];
    // get its root block
    var blkPanel = winPanel.getRootBlock();
    // its first child is the canvas block
    var blkCanvas = blkPanel.getFirstChild();

    return blkCanvas;
  }

  /**
  * Add a canvas block inside panel
  * This will hold all window blocks in it 
  * @param hposCanvas (string),canvas start horizontal position,pixel or percentage
  * @param vposCanvas (string),canvas start vertical position,pixel or percentage
  * @param hresCanvas (string),canvas width,pixel or percentage
  * @param vresCanvas (string),canvas height,pixel or percentage
  * @param nodeCanvas {Object}
  */
  DisplayAgent_prototype._addCanvas = function (canvasId, hposCanvas, vposCanvas, hresCanvas, vresCanvas) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._addCanvas:" + canvasId + ":" + hposCanvas + ":" + vposCanvas + ":" + hresCanvas + ":" + vresCanvas);
    //TBD: find the right panel based on display-to-canvas mapping. for now just pick the first one
    for (var panelId in this.arrDisplayPanels) {
      var winPanel = this.arrDisplayPanels[panelId];
      var blkPanel = winPanel.getRootBlock();
      //load a simple blank template block
      var blkCanvas = new jsx3.gui.Block(canvasId, hposCanvas, vposCanvas, hresCanvas, vresCanvas);
      blkCanvas.setOverflow(jsx3.gui.Block.OVERFLOWHIDDEN);
      if (blkCanvas) {
        // set absolute positioning if needed
        (blkCanvas.getRelativePosition() != jsx3.gui.Block.ABSOLUTE) && blkCanvas.setRelativePosition(jsx3.gui.Block.ABSOLUTE);
        blkCanvas.setBackgroundColor("#4D4D4D"); //grey 
        // set it as a child of the panel
        blkPanel.setChild(blkCanvas);
        // paint to force rendering
        blkPanel.paintChild(blkCanvas);
      }
      else {
        this._LOG.error("com.ipvs.agent.DisplayAgent._addCanvas:" + canvasId + ":FAILED");
      }
      break;
    }
  }

  /**
  * Apply canvas geometry to the canvas blk
  * Add the canvas blk if not already present
  * Will try to fit the largest possible block in the current display that matches the aspect ratio of the Canvas size
  * @param nodeCanvas {Object}
  * @retrunl bDirty {boolean}
  */
  DisplayAgent_prototype._applyCanvasLayout = function (nodeCanvas) {
    // figure out the best fit position and size of the canvas block in the current display
    this._LOG.debug("com.ipvs.agent.DisplayAgent._applyCanvasLayout");
    var bDirty = true;
    //get display dimension
    var nodeDisplayStat = this._getPortDisplayStatus_DisplayNode();
    var hresDisplay = parseInt(nodeDisplayStat.getAttribute("hres"));
    var vresDisplay = parseInt(nodeDisplayStat.getAttribute("vres"));
    //get canvas geometry
    var hresCanvas = parseInt(nodeCanvas.getAttribute("hres"));
    var vresCanvas = parseInt(nodeCanvas.getAttribute("vres"));
    // check if canvas is bigger than panel
    if (hresCanvas != hresDisplay || vresCanvas != vresDisplay) {
      // find which dimension will dominate
      var arCanvas = hresCanvas / vresCanvas;
      var arDisplay = hresDisplay / vresDisplay;
      if (arCanvas < arDisplay) {
        // vres of Canvas will dominate
        vresCanvas = vresDisplay;
        hresCanvas = vresCanvas * arCanvas;
      }
      else {
        // hres of Canvas will dominate
        hresCanvas = hresDisplay;
        vresCanvas = hresCanvas / arCanvas;
      }
    }
    // now center the canvas on the display
    var hposCanvas = parseInt((hresDisplay - hresCanvas) / 2);
    var vposCanvas = parseInt((vresDisplay - vresCanvas) / 2);

    // for each panel apply the relevant part of the canvas
    // TBD: for now just deal with the first panel
    for (panelId in this.arrDisplayPanels) {
      // figure out the dimensions of the canvas block within each panel
      // TBD: for now this is equal to full canvas
      var hposBlkCanvas = hposCanvas;
      var vposBlkCanvas = vposCanvas;
      var hresBlkCanvas = hresCanvas;
      var vresBlkCanvas = vresCanvas;
      // get the current canvas block for this panel
      var blkCanvas = this._getCanvas(panelId);
      if (!blkCanvas) {
        // if not found add a new block
        var blkCanvasId = nodeCanvas.getAttribute("id") + "_" + panelId;
        blkCanvas = this._addCanvas(blkCanvasId, hposBlkCanvas, vposBlkCanvas, hresBlkCanvas, vresBlkCanvas);
        bDirty = true;
      }
      else {
        if ((blkCanvas.getLeft() != hposBlkCanvas) || (blkCanvas.getTop() != vposBlkCanvas) ||
          (blkCanvas.getWidth() != hresBlkCanvas) || (blkCanvas.getHeight() != vresBlkCanvas)) {
          blkCanvas.setDimensions(hposBlkCanvas, vposBlkCanvas, hresBlkCanvas, vresBlkCanvas, true);
          bDirty = true;
        }
      }
      break;
    }
    // update canvas status
    if (bDirty) {
      var nodeCanvasStat = this._getPortDisplayStatus_CanvasNode();
      nodeCanvasStat.setAttribute("hres", hresCanvas);
      nodeCanvasStat.setAttribute("vres", vresCanvas);
    }
    return bDirty;
  }

  //===================== Window Methods ======================================

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
  DisplayAgent_prototype._addWindow = function (winId, hpos, vpos, hres, vres, playerType) {
    this._LOG.info("com.ipvs.agent.DisplayAgent._addWindow: " + winId + ":" + hpos + ":" + vpos + ":" + hres + ":" + vres + ":player type: " + playerType);
    // create a new Window instance
    var blkWin = new com.ipvs.gui.MediaPlayerWindow(winId, hpos, vpos, hres, vres, playerType);


    if (blkWin) {
      // set absolute positioning if needed
      (blkWin.getRelativePosition() != jsx3.gui.Block.ABSOLUTE) && blkWin.setRelativePosition(jsx3.gui.Block.ABSOLUTE);
      blkWin.setBackgroundColor("#181818");
      blkWin.setOverflow(jsx3.gui.Block.OVERFLOWSCROLL);
      //keep temporary fps
      blkWin.nextFps = null;
      // save a reference to it in the list
      this.arrWindowsByWinId[winId] = blkWin;
      // go thru the current displayPanel array
      //TBD: find the right panel based on display-to-canvas mapping. for now just pick the first one
      var panelId = null;
      for (panelId in this.arrDisplayPanels) {
        break;
      }
      // get the current canvas block for this panel
      var blkCanvas = this._getCanvas(panelId);

      // attach window blk as a child of the matching canvas window
      // load a player into the window - async fnc - do nothing on return
      var thisObj = this;
      com.ipvs.util.Common.asyncSetChild(blkCanvas, blkWin).when(function () {
        thisObj._LOG.debug("com.ipvs.agent.DisplayAgent:_addWindow:" + blkWin.getName() + "'s block load successful.");
        blkWin.asyncLoadPlayer().when(
                  function () {
                    //subscrib com.ipvs.gui.MediaPlayerWindow.MSG_ONNATIVESIZECHANGE
                    blkWin.subscribe(com.ipvs.gui.MediaPlayerWindow.MSG_ONNATIVESIZECHANGE + blkWin._playerId, thisObj, thisObj._setPortWindowContentBySizeChange);
                    thisObj._LOG.info("com.ipvs.agent.DisplayAgent:_addWindow:" + blkWin.getName() + "'s media player load successful.");
                    //subscrib com.ipvs.gui.MediaPlayerWindow.MSG_ONKBMSENABLED for Ctrl+k/Ctrl-Ctrl-K
                    blkWin.subscribe(com.ipvs.gui.MediaPlayerWindow.MSG_ONKBMSENABLED + blkWin._playerId, thisObj, thisObj._setPortWindowContentByKBMCtrlK);
                    //subscrib com.ipvs.gui.MediaPlayerWindow.MSG_ONRFBENABLED for VNC server
                    blkWin.subscribe(com.ipvs.gui.MediaPlayerWindow.MSG_ONRFBENABLED + blkWin._playerId, thisObj, thisObj._setPortWindowContentByRFBStateChange);
                    //trigger any miss SSCmd
                    var objPlayerCmd = thisObj.arrSSCmdByWinId[winId];
                    if (typeof (objPlayerCmd) != 'undefined') {
                      thisObj._LOG.info(" _addWindow:start:URL" + objPlayerCmd.streamURL + " :winId: " + objPlayerCmd.windowId + " :connId: " + objPlayerCmd.connId);
                      blkWin.objPlayerCmd = objPlayerCmd;
                      blkWin.start(objPlayerCmd.streamURL);
                      blkWin.streamURL = objPlayerCmd.streamURL;
                      delete thisObj.arrSSCmdByWinId[winId];
                    }
                  });
      });
    }
    else {
      this._LOG.error("com.ipvs.agent.DisplayAgent._addWindow: " + winId + ": FAILED");
    }
    return blkWin;
  }

  /**
  * deletes an existing window from the current display
  * @param winId {string}
  * @return window instance blkJSX
  */
  DisplayAgent_prototype._deleteWindow = function (winId) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._deleteWindow: " + winId);
    // get the current display window 
    var blkWin = this.arrWindowsByWinId[winId];
    if (blkWin) {
      //unload player and remove its reference in DOM
      blkWin.unloadPlayer();
      // remove it from the current display
      blkWin.getParent().removeChild(blkWin);
      // and deref it from the list
      delete this.arrWindowsByWinId[winId];
    }

    // check if this was maximized - reset state vars
    if (this.canvasMaxWinId == winId) {
      this.canvasMaxWinId = null;
      this.canvasMaxWinVisibility = true;
    }

    // update window status
    var nodeWinStat = this._getPortDisplayStatus_WindowNode(winId);
    nodeWinStat.getParent().removeChild(nodeWinStat);

    //send delete connection from SSCmd
    jsx3.sleep(function () { agtMediaStreamDst.sendSSCmdDeleConn(winId), null, this });

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
  DisplayAgent_prototype._applyWindowLayout = function (winId, nodeProps, hresCanvas, vresCanvas) {
    this._LOG.debug("com.ipvs.agent.DisplayAgent._applyWindowLayout: " + winId);
    var bDirty = false;

    // get the current display window
    var blkWin = this.arrWindowsByWinId[winId];
    // if it does not exist in the display - add it with some hard coded size
    if (!blkWin) {
      //launched players based on zIndex value: 1 - window mode; 0 - windowless mode;
      var playerType = this.canvasPlayerModeFlg ? com.ipvs.gui.MediaPlayerWindow.WINDOW_MODE_PLAYER : com.ipvs.gui.MediaPlayerWindow.WINDOWLESS_MODE_PLAYER;
      blkWin = this._addWindow(winId, 0, 0, 10, 10, playerType);
      bDirty = true;
    }
    // get the canvas blk for this window
    //TBD: need to check if window needs to move from one panel to another (when more than one panel supported)
    var blkCanvas = blkWin.getParent();
    var hresBlkCanvas = blkCanvas.getWidth(); //blkCanvas.getId()
    var vresBlkCanvas = blkCanvas.getHeight();

    // get window position and size
    var hpos = nodeProps.getAttribute("hpos");
    var vpos = nodeProps.getAttribute("vpos");
    var hres = nodeProps.getAttribute("hres");
    var vres = nodeProps.getAttribute("vres");
    // get zIndex
    var zIndex = nodeProps.getAttribute("zIndex");

    // check window 'maxmize' - if true adjust size and zindex
    // don't use maximize property - use canvas level var instead
    var maximize = (winId == this.canvasMaxWinId) ? "true" : "false";
    if (maximize == "true") {
      hpos = 0;
      vpos = 0;
      hres = hresBlkCanvas;
      vres = vresBlkCanvas;
      zIndex = DisplayAgent.MAX_ZINDEX;
    }
    else {
      // - convert pixel to % if needed - based on canvas cfg 'node' size
      if (hpos.indexOf('%') < 0) {
        hpos = (hpos / hresCanvas) * 100;
        hpos = hpos > 100 ? 100 : hpos;
      }
      if (vpos.indexOf('%') < 0) {
        vpos = (vpos / vresCanvas) * 100;
        vpos = vpos > 100 ? 100 : vpos;
      }
      if (hres.indexOf('%') < 0) {
        hres = (hres / hresCanvas) * 100;
        hres = hres > 100 ? 100 : hres;
      }
      if (vres.indexOf('%') < 0) {
        vres = (vres / vresCanvas) * 100;
        vres = vres > 100 ? 100 : vres;
      }
      // - then convert back to pixel based on canvas 'blk' size
      hpos = Math.round(hresBlkCanvas * parseFloat(hpos) / 100);
      vpos = Math.round(vresBlkCanvas * parseFloat(vpos) / 100);
      hres = Math.round(hresBlkCanvas * parseFloat(hres) / 100);
      vres = Math.round(vresBlkCanvas * parseFloat(vres) / 100);
      // make sure window is bounded within canvas blk
      //hpos = (parseInt(hpos) + hres) > hresBlkCanvas ? (hresBlkCanvas - hres) : hpos;
      //vpos = (parseInt(vpos) + vres) > vresBlkCanvas ? (vresBlkCanvas - vres) : vpos;
    }
    // set the dimensions
    if ((blkWin.getLeft() != hpos) || (blkWin.getTop() != vpos) ||
          (blkWin.getWidth() != hres) || (blkWin.getHeight() != vres)) {
      if (blkWin.innerSpan && blkWin.innerSpan.style) {
        //set inner span dimension first
        blkWin.innerSpan.style.width = hres;
        blkWin.innerSpan.style.height = vres;
      }
      blkWin.setDimensions(hpos, vpos, hres, vres, true);
      //blkWin.IPVideoPlayer.setScaleToWindow();  
      bDirty = true;
    }
    // set zIndex
    (blkWin.getZIndex() != zIndex) && blkWin.setZIndex(parseInt(zIndex), true) && (bDirty = true);

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
    (blkWin.getVisibility() != bVisibility) && blkWin.setVisibility(bVisibility, true) && (bDirty = true);

    // update Player properties
    bDirty |= this._applyPlayerProps(winId, nodeProps);

    // if dirty - repaint window and update window status
    if (bDirty) {
      var nodeWinStat = this._getPortDisplayStatus_WindowNode(winId);
      var nodePropsStat = nodeWinStat.selectSingleNode(".//Properties");
      nodePropsStat.setAttribute("hpos", hpos);
      nodePropsStat.setAttribute("vpos", vpos);
      nodePropsStat.setAttribute("hres", hres);
      nodePropsStat.setAttribute("vres", vres);
      nodePropsStat.setAttribute("border", border);
      nodePropsStat.setAttribute("zIndex", zIndex);
      nodePropsStat.setAttribute("transparency", blkWin.getBlkTransparency());
      nodePropsStat.setAttribute("maximize", maximize);
      nodePropsStat.setAttribute("visibility", bVisible);
      //set player properties mute,scale when _applyWindowLayout otherwise it won't set
      nodePropsStat.setAttribute("mute", nodeProps.getAttribute("mute"));
      nodePropsStat.setAttribute("remoteKbmCtrl", nodeProps.getAttribute("remoteKbmCtrl"));
      nodePropsStat.setAttribute("scale", nodeProps.getAttribute("scale"));
      //set player properties scrollHPos,scrollVPos when _applyWindowLayout otherwise it won't set
      nodePropsStat.setAttribute("scrollHPos", nodeProps.getAttribute("scrollHPos"));
      nodePropsStat.setAttribute("scrollVPos", nodeProps.getAttribute("scrollVPos"));
    }

    return bDirty;
  }

  /**
  * apply player related properteis to video player assuming this valid only when player is loaded; 
  *    two callers: 
  *    [1] _setPortWindowContentByKBMCtrlK,_setPortWindowContentByRFBStateChange 
  *           --> setPortWindowContent with a parameter winId 
  *    [2] WindowPropertiesUpdate notification 
  *           -->_applyCanvas --> _applyWindowLayout -->(update window status) setPortWindowProperties with parameters winId and property node  
  * @param winId {string} 
  * @param nodeWinProps {object} (optional, object from SetPortWindowProperties notification)
  */
  DisplayAgent_prototype._applyPlayerProps = function (winId, nodeWinProps) {

    // case [1] call from player call back function, properties must be changed, bDirty is 'true' ;
    // case [2] call from WindowPropertiesUpdate, notify with only changed properties, bDirty is 'true' ;
    var bDirty = false;

    // get the current display window
    var blkWin = this.arrWindowsByWinId[winId];

    // get window node of this.DOC_PORTDISPLAYSTATUS
    var nodeWinStat = this._getPortDisplayStatus_WindowNode(winId);

    //if 'blkWin' exist and the player loaded and has Filename, then apply player properties
    if (blkWin && blkWin.IPVideoPlayer && !jsx3.util.strEmpty(blkWin.IPVideoPlayer.Filename)) {
      /**
      * zIndex="100" border="" vpos="0" hpos="0" vres="100%" hres="100%" aspect="" transparency="0" layoutId="" layoutPos="" visibility="true"
      * maximize="false" fps="" scale="fitWindow" mute="false" volume="50%" remoteKbmCtrl="false" scrollHPos="0" scrollVPos="0"
      * properties coming from [1]WindowPropertiesUpdate notification (need set to this.DOC_PORTDISPLAYSTATUS) or [2]this.DOC_PORTDISPLAYSTATUS 
      **/

      //call from WindowPropertiesUpdate,nodeWinProps only including changed property
      var nodeProps = (nodeWinProps) ? nodeWinProps : nodeWinStat.selectSingleNode(".//Properties");

      //set 'mute' of the window
      var propMute = nodeProps.getAttribute("mute");
      if (!jsx3.util.strEmpty(propMute)) {
        (propMute == "true") ? blkWin.mute(true) : blkWin.mute(false);
        bDirty = true;
      }

      //get (1)'srcKbmCtrl' from property and get user profile from streamURL(2)get user button status from 'remoteKbmCtrl' 
      //if all true, set localKBMSState to '1' otherwise set to '0'
      var nodeWinConnContent = nodeWinStat.selectSingleNode(".//ConnectionContent");
      //If have a connection in the window
      if (!jsx3.util.strEmpty(nodeWinStat.getAttribute("content"))) {
        //(1)KBM server
        var propertySrcKBMCtrl = nodeWinConnContent.getAttribute("srcKbmCtrl");
        //(2)button click
        var propRemoteKbm = nodeProps.getAttribute("remoteKbmCtrl");
        (propertySrcKBMCtrl == "true") && (propRemoteKbm == "true") ? blkWin.setLocalKBMSState(1) : blkWin.setLocalKBMSState(0);
        bDirty = true;
      }

      //set 'scale' of the window
      var propScale = nodeProps.getAttribute("scale");
      var propVisibility = nodeProps.getAttribute("visibility");
      if (!jsx3.util.strEmpty(propScale)) {
        //bug#50139 Incase of RTP 1:1 in hidden mode is not showing video after show.
        if (propVisibility == "true") {
          (propScale == "fitNativeSize") ? blkWin.setNativeSize() : blkWin.setScaleToWindow();
        }
        bDirty = true;
      }
      /**
      * Set fps when:
      * 1. current window is maximized; ORs
      * 2. no any 'maximize' window in current canvas;
      * 3. there is a 'maximize' window but hidden set fps to other than maximized one ;
      * refer to bug 37024,disable property 'fps' temporary
      **/
      var propFPS = nodeProps.getAttribute("fps");
      propFPS = !jsx3.util.strEmpty(propFPS) ? propFPS : 15;

      if ((!jsx3.util.strEmpty(this.canvasMaxWinId) &&
          this.canvasMaxWinId != winId &&
          this.canvasMaxWinVisibility) ||
          (propVisibility == 'false')) {
        blkWin.nextFps = 0;
      }
      else {
        //blkWin.nextFps = parseInt(propFPS);
        blkWin.nextFps = this.arrWindowsByWinId[winId].readFirstFps();
      }

      //get 'scrollHPos'
      var propScrollHPos = nodeProps.getAttribute("scrollHPos");
      //get 'scrollVPos'
      var propScrollVPos = nodeProps.getAttribute("scrollVPos");
      if (!isNaN(parseInt(propScrollHPos)) && !isNaN(parseInt(propScrollVPos))) {
        //TBD: aplly scrollHPOS and scrollVPOS to video block
      }
    }
    return bDirty;
  }

  /**
  * Go through current canvas to drop all window fps to '0'
  */
  DisplayAgent_prototype._dropFps = function () {
    for (var winId in this.arrWindowsByWinId) {
      if (!this.arrWindowsByWinId[winId].streamURL) continue;
      this.arrWindowsByWinId[winId].setFps(0);
    }
  }

  /**
  * Go through current canvas to set all window fps to required values
  */
  DisplayAgent_prototype._setFps = function () {
    for (var winId in this.arrWindowsByWinId) {
      if (!this.arrWindowsByWinId[winId].streamURL) continue;
      this.arrWindowsByWinId[winId].setFps(this.arrWindowsByWinId[winId].nextFps);
    }
  }

  /**
  * Player action wrapper
  * @param winId {string}
  * @param objPlayerCmd {Object}
  */
  DisplayAgent_prototype.doAction = function (objPlayerCmd) {
    // get the target display window mediaplayer
    var blkWin = this.arrWindowsByWinId[objPlayerCmd.windowId];
    if (getAssociativeArrayLength(this.arrWindowsByWinId) <= 0) {
      this.arrSSCmdByWinId[objPlayerCmd.windowId] = objPlayerCmd;
      return;
    }
    // if not found - return
    if (!blkWin) {
      this._LOG.info(" doAction:store objPlayerCmd at arrSSCmdByWinId[" + objPlayerCmd.windowId + "]");
      //send delete connection from SSCmd
      jsx3.sleep(function () { agtMediaStreamDst.sendSSCmdDeleConn(objPlayerCmd.windowId), null, this });
      return false;
    }

    //stash objPlayerCmd in arrWindowsByWinId for 'bOnNativeSizeChange' listener
    blkWin.objPlayerCmd = objPlayerCmd;
    var thisObj = this;
    // found window - act on the 'action'
    switch (objPlayerCmd.action) {

      case "Start":
        if (blkWin.windowContentUpdateTimer) {
          //clean blkWin.windowContentUpdateTimer if it is exist
          //switchSrc:'start' following 'stop' for the same blkWin,skip 'stop' DB update  
          clearTimeout(blkWin.windowContentUpdateTimer);
        }
        // form URL from cmd obj properties
        // player should be loaded but make sure anyway
        blkWin.asyncLoadPlayer().when(function () {
          thisObj._LOG.info(":doAction:Start:URL" + objPlayerCmd.streamURL + " :winId: " + objPlayerCmd.windowId + " :connId: " + objPlayerCmd.connId);
          blkWin.start(objPlayerCmd.streamURL);
          blkWin.streamURL = objPlayerCmd.streamURL;
        });
        break;
      case "Stop":
        blkWin.stop();
        blkWin.streamURL = null;
        //update DB displayConfig and displayStatus with a delay for 'switchSrc' performance reason 
        blkWin.windowContentUpdateTimer = setTimeout(function () {
          thisObj._LOG.info("::doAction:Stop:winId: " + objPlayerCmd.windowId + " :connId: " + objPlayerCmd.connId);
          //remove this connection window
          //reset 'sessionId' and 'connId' if possible
          thisObj.resetPortWindowContent(objPlayerCmd);
        }, 1000);
        break;
      case "Pause":
        break;
      case "Resume":
      case "ResumeFrom":
      case "ResumeFromMediaTC":
        break;
      case "Catchup":
        break;
      case "SkipBackward":
        break;
      case "SkipForward":
        break;
      case "NativesizeOn":
        break;
      case "NativesizeOff":
        break;
      case "MuteOn":
        break;
      case "MuteOff":
        break;
      case "RemoteOn":
        break;
      case "RemoteOff":
        break;
      case "FullScreenOn":
        break;
      case "FullScreenOff":
        break;
      default:
        break;
    }
    return true;
  }

  /**
  * setPortWindowContent when window native size change
  * @param winId {string}
  * @return boolean
  */
  DisplayAgent_prototype._setPortWindowContentBySizeChange = function (EventVal) {
    var playerInstanceId = EventVal.context.objEVENT;
    for (var winId in this.arrWindowsByWinId) {
      var blkWin = this.arrWindowsByWinId[winId];
      if (blkWin._playerId == playerInstanceId) {
        this.setPortWindowContent(blkWin.objPlayerCmd);
      }
    }
  }

  /**
  * setPortWindowContentByKBMCtrlK when unmanned client hit Ctrl+K/Ctrl-Ctrl-K
  */
  DisplayAgent_prototype._setPortWindowContentByKBMCtrlK = function (EventVal) {
    var pubMsg = EventVal.context.objEVENT;
    var playerInstanceId = pubMsg.split(':')[0];
    var callbackMsg = pubMsg.split(':')[1];
    this._LOG.info("::_setPortWindowContentByKBMCtrlK:'playerInstanceId'=" + playerInstanceId + ":callbackMsg=" + callbackMsg);
    for (var winId in this.arrWindowsByWinId) {
      var blkWin = this.arrWindowsByWinId[winId];
      if (blkWin._playerId == playerInstanceId) {
        blkWin.CtrlKCallback = callbackMsg;
        var nodeWinStat = this._getPortDisplayStatus_WindowNode(winId);
        if (nodeWinStat.getAttribute("content") == "CONNECTION") {
          //CTRL K work only when there is connection
          var nodeProps = nodeWinStat.selectSingleNode(".//Properties");
          //if callbackMsg is 'true'/Ctrl+K, set property 'remoteKbmCtrl' to true;
          //if callbackMsg is 'false'/Ctrl-Ctrl-K, set property 'remoteKbmCtrl' to false;
          nodeProps.setAttribute("remoteKbmCtrl", callbackMsg);

          var nodeWinPropsConfig = this.DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Window[@id='" + winId + "']/Properties");
          if (nodeWinPropsConfig) {
            nodeWinPropsConfig.setAttribute("remoteKbmCtrl", callbackMsg);
          }
          this.setPortWindowContent(blkWin.objPlayerCmd);
          this._LOG.info("::_setPortWindowContentByKBMCtrlK:after call setPortWindowContent:nodeProps=" + nodeProps.toString());
        }
        break;
      }
    }
  }


  /**
  * setPortWindowContentByRFBStateChange when window native size change
  */
  DisplayAgent_prototype._setPortWindowContentByRFBStateChange = function (EventVal) {
    var pubMsg = EventVal.context.objEVENT;
    var playerInstanceId = pubMsg.split(':')[0];
    var callbackMsg = pubMsg.split(':')[1];
    this._LOG.info("::_setPortWindowContentByRFBStateChange:'playerInstanceId'=" + playerInstanceId + ":callbackMsg=" + callbackMsg);
    for (var winId in this.arrWindowsByWinId) {
      var blkWin = this.arrWindowsByWinId[winId];
      if (blkWin._playerId == playerInstanceId) {
        var nodeWinStat = this._getPortDisplayStatus_WindowNode(winId);
        var nodeWinConnContent = nodeWinStat.selectSingleNode(".//ConnectionContent");
        //if callbackMsg is 'true', set property 'srcKbmCtrl' to true;
        //if callbackMsg is 'false', set property 'srcKbmCtrl' to false;
        nodeWinConnContent.setAttribute("srcKbmCtrl", callbackMsg);
        this.setPortWindowContent(blkWin.objPlayerCmd);
      }
    }
  }

  //---------------------------------------------------------------------------------------

  /**
  * Updates DisplayConfig for a stand alone client in the DB.
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._updateDBPortDisplayConfig = function (bDirtyDisplay, bDirtyCanvas) {
    if (!bDirtyDisplay && !bDirtyCanvas) return;
    this.DOC_PORTDISPLAYCONFIGREQ.removeChildren();
    this.DOC_PORTDISPLAYCONFIGREQ.setAttribute("byNID", this.deviceNID);
    var DOC_PORTDISPLAYCONFIG = this.DOC_PORTDISPLAYCONFIG.cloneNode(true);
    if (!bDirtyDisplay && bDirtyCanvas) {
      DOC_PORTDISPLAYCONFIG.removeChild(DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Display"));
    }
    if (bDirtyDisplay && !bDirtyCanvas) {
      DOC_PORTDISPLAYCONFIG.removeChild(DOC_PORTDISPLAYCONFIG.selectSingleNode(".//Canvas"));
    }
    this.DOC_PORTDISPLAYCONFIGREQ.appendChild(DOC_PORTDISPLAYCONFIG);
    var setServiceClientDisplayConfigReq = new ReqSessionHandleWindowing(this.DOC_PORTDISPLAYCONFIGREQ, "SetServiceClientDisplayConfig",
                                                                           { action: "FNCCALL", context: this,
                                                                             fnc: function (respCode, respVal) {
                                                                               if (respCode == "SUCCESS" && respVal != null) {
                                                                                 var resp = respVal;
                                                                                 this._LOG.info("::_updateDBPortDisplayConfig:'SUCCESS':respVal=" + respVal.toString());
                                                                               }
                                                                               else {
                                                                                 log.error("SetServiceClientDisplayConfig request failed for deviceNID: " + deviceNID);
                                                                               }
                                                                             }
                                                                           }
                                                                         );
    setServiceClientDisplayConfigReq.sendRequest(this);
  }

  /**
  * Updates DisplayStatus for a stand alone client in the DB.
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._updateDBPortDisplayStatus = function (bDirtyDisplay, bDirtyCanvas) {
    if (!bDirtyDisplay && !bDirtyCanvas) return;
    this.DOC_PORTDISPLAYSTATUSREQ.removeChildren();
    this.DOC_PORTDISPLAYSTATUSREQ.setAttribute("byNID", this.deviceNID);
    var DOC_PORTDISPLAYSTATUS = this.DOC_PORTDISPLAYSTATUS.cloneNode(true);
    if (!bDirtyDisplay && bDirtyCanvas) {
      DOC_PORTDISPLAYSTATUS.removeChild(DOC_PORTDISPLAYSTATUS.selectSingleNode(".//Display"));
    }
    if (bDirtyDisplay && !bDirtyCanvas) {
      DOC_PORTDISPLAYSTATUS.removeChild(DOC_PORTDISPLAYSTATUS.selectSingleNode(".//Canvas"));
    }
    this.DOC_PORTDISPLAYSTATUSREQ.appendChild(DOC_PORTDISPLAYSTATUS);
    var setServiceClientDisplayStatusReq = new ReqSessionHandleWindowing(this.DOC_PORTDISPLAYSTATUSREQ, "SetServiceClientDisplayStatus",
                                                                           { action: "FNCCALL", context: this,
                                                                             fnc: function (respCode, respVal) {
                                                                               if (respCode == "SUCCESS" && respVal != null) {
                                                                                 var resp = respVal;
                                                                                 this._LOG.info("::_updateDBPortDisplayStatus:'SUCCESS':respVal=" + respVal.toString());
                                                                               }
                                                                               else {
                                                                                 log.error("SetServiceClientDisplayStatus request failed for deviceNID: " + deviceNID);
                                                                               }
                                                                             }
                                                                           }
                                                                         );
    this._logDBPortDisplayStatus();
    setServiceClientDisplayStatusReq.sendRequest(this);
  }

  /**
  * Log DisplayStatus before send request to DB.
  * @private @jsxobf-clobber
  */
  DisplayAgent_prototype._logDBPortDisplayStatus = function () {
    var nodeDisplayStat = this._getPortDisplayStatus_DisplayNode();
    this._LOG.info("_logDBPortDisplayStatus:Display:" + nodeDisplayStat.toString());
    var nodeCanvasStat = this._getPortDisplayStatus_CanvasNode();
    this._LOG.info("_logDBPortDisplayStatus:Canvas:" + nodeCanvasStat.toString());
  }
  //(nodePortWindowProperties.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><")
  //---------------------------------------------------------------------------------------

  /**
  * Envent listener sections
  * @param eventApp {Object}
  */
  DisplayAgent_prototype.eventWrap = function (eventApp) {

    //EventListener for DisplayConfigUpdate; the following 3 listen from handleNotification_DeviceAdminServerAgent
    eventApp.subscribe("DisplayConfigUpdate", null, this,
                      function (EventVar, EventVal) {
                        //                        var clientLaunchMode = topic_Get(tApp, "ClientLaunchMode");
                        //                        if(clientLaunchMode != "Unmanned")return;
                        var nodeDisplayConfigObj = EventVal;

                        this.setPortDisplayConfig(nodeDisplayConfigObj, false);
                      });

    //EventListener for DisplayConfigUpdate:SetPortWindowProperties (WindowContentUpdate & SetPortWindowVisible are all obsolete)               
    eventApp.subscribe("WindowPropertiesUpdate", null, this,
                      function (EventVar, EventVal) {
                        //get window properies and related ID info. from notification
                        var nodeSetPortWindowPropertiesObj = EventVal;
                        var nodeSetPortWindowProperties = nodeSetPortWindowPropertiesObj.nodePortWindowProperties;
                        var clientInfoObj = nodeSetPortWindowPropertiesObj.clientInfoObj;
                        //keep for future use
                        var nodeIDProps = new Object;
                        // layoutId,canvasId, & winId are madatoary parameters
                        nodeIDProps["layoutId"] = nodeSetPortWindowProperties.getAttribute("byLayoutId");
                        nodeIDProps["canvasId"] = nodeSetPortWindowProperties.getAttribute("byCanvasId");
                        nodeIDProps["windowId"] = nodeSetPortWindowProperties.getAttribute("byWindowId");

                        //portNID & sessionId, connId are optional parameters;No portNID, have to give other two, vice visa;
                        nodeIDProps["portNID"] = nodeSetPortWindowProperties.getAttribute("byNID");
                        nodeIDProps["sessionId"] = nodeSetPortWindowProperties.getAttribute("bySessionId");
                        nodeIDProps["connId"] = nodeSetPortWindowProperties.getAttribute("byConnId");

                        var nodeWindowProps = nodeSetPortWindowProperties.selectSingleNode(".//WindowProperties");
                        this.setPortWindowProperties(nodeIDProps.windowId, nodeWindowProps, clientInfoObj);
                      });

  }

  //---------------------------------------------------------------------------------------
  /**
  * Launch media player window by session ID; 
  * 'Browse Session', 'Media List', 'Live Session' & 'Autojoin';
  * @param mediaInfo {JASON Object}
  * @param joinMode {Boolean}
  * @param tMediaPlayer {String}
  * @param eventMediaPlayer {Object}
  * @param nodeSession {XML Node}
  */
  DisplayAgent_prototype.launchPlayerBySessionId = function (mediaInfo, joinMode, tMediaPlayer, eventMediaPlayer) {
    if (!mediaInfo.sessionId) return;
    // if sessionID already exists - return - no need for new player
    if (this.arrSessionWindows[mediaInfo.sessionId]) return;

    //set windowInstanceArr
    this.arrSessionWindows[mediaInfo.sessionId] = { "topic": null,
      "windowHandler": null
    };
    // launch a new player window with joinCollabMode 'TRUE' which is no need to stop collaboration in the end
    cmpWinMediaPlayer_Create(mediaInfo, true, this.arrSessionWindows, tMediaPlayer, eventMediaPlayer);
  }

  //---------------------------------------------------------------------------------------
  /**
  * Launch media player window by FileNID; 
  * IPVSAuthTool 'Media List', 'Live Session' & 'Autojoin';
  * @param mediaInfo {JASON Object}
  */
  DisplayAgent_prototype.launchPlayerByFileNID = function (mediaInfo) {
    if (!mediaInfo.fileNID) return;

    // launch a new player window 
    cmpWinMediaPlayer_Create(mediaInfo, com.ipvs.gui.authmediaclient.MediaList.Server);
  }

});                 //END OF CLASS