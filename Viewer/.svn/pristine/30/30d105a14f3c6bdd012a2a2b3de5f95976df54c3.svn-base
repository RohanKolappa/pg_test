/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* SrcPanel class.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.SrcPanel", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function(SrcPanel, SrcPanel_prototype) {

  /** @private @jsxobf-clobber */
  SrcPanel_prototype._LOG = jsx3.util.Logger.getLogger(SrcPanel.jsxclass.getName());
  //property for MENU_SELECTION
  SrcPanel_prototype.SHOW_ON_MENU_SELECTION = com.ipvs.app.App.MENU_VIEWER;

  /**
  * The common init code
  */
  SrcPanel_prototype.onAfterAttach = function() {
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

  /**
  * Extended initializer.
  */
  SrcPanel_prototype._onAppInit = function() {
    // subscribe to Menu change
    this.clsApp.subscribe(com.ipvs.app.App.MENU_SELECT, this, this._onMenuSelect);
    // subscribe to Login state
    this.clsApp.clsServiceAgent.subscribe(com.ipvs.agent.ServiceAgent.ON_LOGINSTATE_CHANGE, this, this._onLoginStateChange);

    //init Lab Info
    this._selectedSrcTitle = null;
    this._init_SrcList();
    this._LOG.info("SrcPanel: Initiated");
  }

  //===================== Init methods ======================================

  /** @private @jsxobf-clobber */
  SrcPanel_prototype._init_SrcList = function() {
    // get a handle to the list
    this.mtxMediaList = this.getDescendantOfName("mtxMediaList");
    // set list params
    var sortPathArr = { jsxtext: { SortAttr: "_SORTBY_", SortAttrVal: "title"} };
    this.mtxMediaList.initList("xsl/SrcPortList.xsl", false, sortPathArr); //XSLpath, paginationFlag, objSortPathArr
    this.mtxMediaList.resetToStart(1, 5000, "ascending", "title"); //startFrom, countToFetch, defaultSortOrder, defaultSortBy

    // set list Data URL - template - will change based on hospital selected
    var objParam = new Object();
    objParam["Type"] = "SrcPort";
    objParam["SelectFormat"] = "CONNECTIONINFO";
    objParam["StartFrom"] = "_STARTFROM_";
    objParam["CountToFetch"] = "_COUNTTOFETCH_";
    objParam["SortOrder"] = "_SORTORDER_";
    objParam["SortBy"] = "_SORTBY_";
    objParam["WhereTagExpr"] = "";
    objParam["WherePermissions"] = "VIEWFROM";
    objParam["SubscriptionID"] = "Subscription_" + jsx3.xml.CDF.getKey();
    this.mtxMediaList.setObjURLTemplate(objParam);
    // setup list to hadle notifications
    this.mtxMediaList.initNotificationHandling("mediastreamsrcportresourcelist");
    // clear selected and subscribe to onselect
    this._selectedSrc = "NONE";
    this.mtxMediaList.subscribe(jsx3.gui.Interactive.SELECT, this, this._onSrcSelected);
  }


  //===================== Event Handlers ======================================

  SrcPanel_prototype._onMenuSelect = function(objEvent) {
    if(this.clsApp.getMenuSelection() == this.SHOW_ON_MENU_SELECTION) {
      this._LOG.info("_onMenuSelect: Show: " + this.SHOW_ON_MENU_SELECTION);
      this.cmpShow();
    }
    else {
      this.cmpHide();
      this._LOG.info("_onMenuSelect: Hide:" + this.SHOW_ON_MENU_SELECTION);
    }
  }

  /** @private @jsxobf-clobber */
  /* Login State change
  * - extend common for SrcPanel specific actions
  */
  SrcPanel_prototype._onLoginStateChange = function(objEvent) {
    // do SrcPanel specific processing
    if(objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch(stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        // set Src list Data URL
        var objURLParam = this.mtxMediaList.getObjURLTemplate();
        var urlStr = com.ipvs.api.API.GetPortURL(objURLParam);
        // - url for notification handling
        var objNotificationURLParam = this.mtxMediaList.getObjURLTemplate();
        objNotificationURLParam["WhereNID"] = "_WHERENID_";
        objNotificationURLParam["StartFrom"] = "1";
        objNotificationURLParam["CountToFetch"] = "5000";
        objNotificationURLParam["SubscriptionID"] = "";
        var notificationUrlStr = com.ipvs.api.API.GetPortURL(objNotificationURLParam);

        this.mtxMediaList.resetToStart(1, 5000, "ascending", "title"); //startFrom, countToFetch, defaultSortOrder, defaultSortBy
        this.mtxMediaList.setURL(urlStr);
        this.mtxMediaList.setNotificationURL(notificationUrlStr);
        // - set the data to the Src list
        this.mtxMediaList.setList();
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        // clear lab list
        this.mtxMediaList.clearList();
        this._selectedSrc = "NONE";
        this._selectedSrcTitle = null;
        // fake a lab onSelect event to clear selection processing
        this._onSrcSelected();
        break;
      default:
        return;
    }
  }

  //===================== Src Selection ======================================

  /* Src selected 
  */
  SrcPanel_prototype._onSrcSelected = function() {
    // get the selected lab
    this._selectedSrc = this.mtxMediaList.getValue();
    this._LOG.debug("Src Selected : " + this._selectedSrc);
    return; //TBR
    // if empty selection 
    if(jsx3.util.strEmpty(this._selectedSrc)) {
      this._selectedSrcTitle = null;
      // stop any existing connection
      this.objCollabInfo.isSourceSelected = "NONE";
      this.controller.setCollaborationInfo(this.objCollabInfo);
      this._stopSrcConnection();
    }
    else {
      // get lab name
      var record = this.mtxMediaList.getRecord(this._selectedSrc);
      this._selectedSrcTitle = record.jsxtext;
      if(record.jsxdevstate != "Ready" && record.jsxdevstate != "Unmanaged") {
        this.objCollabInfo.isSourceSelected = "OFFLINE";
        this.controller.setCollaborationInfo(this.objCollabInfo);
        this._stopSrcConnection();
      }
      else {
        // start a connection to it
        this.objCollabInfo.isSourceSelected = "READY";
        this.controller.setCollaborationInfo(this.objCollabInfo);
        this._startSrcConnection();

        //hide "Select a lab to view video" message after clicking Labs List
        this.videoMessage.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      }
    }
  }


  //===================== Connection Start/Stop ======================================

  /* _startSrcConnection: start a connection to lab selected 
  * - send View request
  */
  SrcPanel_prototype._startSrcConnection = function() {
    // setup 'View' request w/ pvrEnabled
    var reqParams = new Object();
    reqParams.ReqType = "View";
    reqParams.pvrEnable = "true";

    // - assign conn src info
    var srcTitle = this._selectedSrcTitle;
    // TBD - need to find out if NID is really used
    reqParams.SrcPortNID = this._selectedLab;
    reqParams.SrcPortTitle = srcTitle;

    // - assign conn dest info
    // *HACK* - get actual srcportresourcetitle from getServiceClient response instead
    reqParams.DstPortTitle = this.clsApp.clsPreferences.getLocalPreferences("AssetResourceTitle");
    reqParams.DstEntityId = "";
    reqParams.DstPortNID = "";

    // assign sessionId
    // restart session with existing session id
    reqParams.SessionId = this.sessionId;
    reqParams.RestartSession = true;

    // assign connId- using the same UUID with the SessionID
    // TBD - need to check if this is right - will end up with same connId all the time
    reqParams.ConnId = reqParams.SessionId.replace(/Session_/, "Conn_");
    reqParams.ConnName = srcTitle;

    // assign canvasId, windowId - reuse existing
    reqParams.CanvasId = this.canvasId;
    reqParams.WindowId = this.windowId;

    // call SessionAgent.View
    this.clsApp.clsSessionAgent.View(reqParams);
  }

  /* _stopSrcConnection: stop any lab connection
  * - fake a controller stop button press
  */
  SrcPanel_prototype._stopSrcConnection = function() {
    // call SessionAgent.StopSession
    //this.controller.doStop();
  }

  /* 
  * - Returns the selected Hospital
  */
  SrcPanel_prototype.getSelectedHospital = function() {
    return this._selectedHospital;
  }

  /* 
  * - Returns the selected Lab
  */
  SrcPanel_prototype.getSelectedLab = function() {
    return this._selectedSrc;
  }

  /* 
  * - Returns the selected Hospital
  */
  SrcPanel_prototype.getSelectedHospitalTitle = function() {
    return this._selectedHospitalTitle;
  }

  /* 
  * - Returns the selected Hospital
  */
  SrcPanel_prototype.getSelectedLabTitle = function() {
    return this._selectedSrcTitle;
  }

});
