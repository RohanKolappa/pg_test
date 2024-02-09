/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* LivePanel class.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.LivePanel", com.ipvs.ipv.LiveRecCommon, null, function (LivePanel, LivePanel_prototype) {

  /** @private @jsxobf-clobber */
  LivePanel_prototype._LOG = jsx3.util.Logger.getLogger(LivePanel.jsxclass.getName());
  //property for MENU_SELECTION
  LivePanel_prototype.SHOW_ON_MENU_SELECTION = com.ipvs.ipv.MainPanel.MENU_LIVEPANEL;

  /**
  * Extended initializer.
  */
  LivePanel_prototype._onAppInit = function () {
    //init Lab Info
    this._selectedSrcTitle = null;
    this._sortBy = "state";
    this._startFrom = 1;
    this._countToFetch = 15;
    this._tumbnailRefreshTimerId = null;
    this._init_SrcList();
    this.jsxsuper();
    this._LOG.info("LivePanel: Initiated");
  }

  //===================== Init methods ======================================

  /** @private @jsxobf-clobber */
  LivePanel_prototype._init_SrcList = function () {
    // get a handle to the list
    this.mtxMediaList = this.getDescendantOfName("mtxMediaList");
    // set list params
    var sortPathArr = { jsxtext: { SortAttr: "_SORTBY_", SortAttrVal: this._sortBy} };
    this.mtxMediaList.initList("xsl/SrcPortList.xsl", true, sortPathArr); //XSLpath, paginationFlag, objSortPathArr
    this.mtxMediaList.resetToStart(this._startFrom, this._countToFetch, "descending", this._sortBy); //startFrom, countToFetch, defaultSortOrder, defaultSortBy

    // set list Data URL - template - will change based on hospital selected
    var objParam = new Object();
    objParam["Type"] = "SrcPort";
    objParam["SelectFormat"] = "BASIC";
    objParam["StartFrom"] = "_STARTFROM_";
    objParam["CountToFetch"] = "_COUNTTOFETCH_";
    objParam["SortOrder"] = "_SORTORDER_";
    objParam["SortBy"] = "_SORTBY_";
    objParam["WhereTagExpr"] = "";
    objParam["WherePermissions"] = "VIEWFROM";
    objParam["WhereStreamType"] = "";
    objParam["SubscriptionID"] = "Subscription_" + getUUID();
    this.mtxMediaList.setObjURLTemplate(objParam);
    // setup list to hadle notifications
    this.mtxMediaList.initNotificationHandling("mediastreamsrcportresourcelist,mediastreamioportresourcelist");
    // clear selected and subscribe to onselect
    this._selectedSrc = "NONE";
    this.mtxMediaList.subscribe(jsx3.gui.Interactive.SELECT, this, this._onSrcSelected);
    this.mtxMediaList.getDescendantOfName("thumbnail", false, false).setFormatHandler(jsx3.$F(this._formatThumbnail).bind(this));

    // finally attach reference of mtx to pagination controls
    this.pagingControls = this.getDescendantOfName("blkPagination");
    this.pagingControls.initPagination(this.mtxMediaList);
  }

  /** @private @jsxobf-clobber */
  //to scale image size
  LivePanel_prototype._formatThumbnail = function (element, cdfkey, matrix, column, rownumber, server) {
    var objImg = element.childNodes[0];
    if (!jsx3.util.strEmpty(objImg)) {
      objImg.style.width = "100%";
      objImg.style.height = "100%";
    }
  }

  /** @private @jsxobf-clobber */
  //Start/Stop timer for refresh thumbnails based on Show/Hide live panel
  LivePanel_prototype._onMenuSelect = function (objEvent) {
    this.jsxsuper();
    if (this.clsParent.getMenuSelection() == this.SHOW_ON_MENU_SELECTION) {
      if (jsx3.util.strEmpty(this._tumbnailRefreshTimerId)) {
        this._tumbnailRefreshTimerId = setInterval(jsx3.$F(function () { this._handleThumbnails(); }).bind(this), 4000);
      }
    }
    else {
      if (!jsx3.util.strEmpty(this._tumbnailRefreshTimerId)) {
        clearInterval(this._tumbnailRefreshTimerId);
        this._tumbnailRefreshTimerId = null;
      }
    }
  }

  /** @private @jsxobf-clobber */
  //Iterate all rows and refresh thumbnail image if the device is state is 'Ready"
  LivePanel_prototype._handleThumbnails = function () {
    var strIds = this.mtxMediaList.getSortedIds();
    for (var i = 0; i < strIds.length; i++) {
      var record = this.mtxMediaList.getRecord(strIds[i]);
      if (record.jsxdevstate == "Ready") {
        var thumbnailElement = this.mtxMediaList.getContentElement(strIds[i], "jsxthumbnailURL");
        var objImg = thumbnailElement.childNodes[0];
        if (!jsx3.util.strEmpty(objImg)) {
          objImg.src = record.jsxthumbnailURL + "&newImage=" + new Date().getTime();
        }
      }
    }
  }

  //===================== Event Handlers ======================================
  /** @private @jsxobf-clobber */
  /* Login State change
  * - extend common for LivePanel specific actions
  */
  LivePanel_prototype._onLoginStateChange = function (objEvent) {
    // call super to do common stuff
    // do LivePanel specific processing
    if (objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch (stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        var whereStreamType = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_PORTSTREAMTYPE);
        whereStreamType = whereStreamType.replace(/\s+/g, ",");
        // set Src list Data URL
        var objURLParam = this.mtxMediaList.getObjURLTemplate();
        objURLParam["WhereStreamType"] = whereStreamType
        var urlStr = com.ipvs.api.API.GetPortURL(objURLParam);
        // - url for notification handling
        var objNotificationURLParam = this.mtxMediaList.getObjURLTemplate();
        objNotificationURLParam["WhereNID"] = "_WHERENID_";
        objNotificationURLParam["StartFrom"] = "1";
        objNotificationURLParam["CountToFetch"] = "5000";
        objNotificationURLParam["WhereStreamType"] = whereStreamType;
        objNotificationURLParam["SubscriptionID"] = "";
        var notificationUrlStr = com.ipvs.api.API.GetPortURL(objNotificationURLParam);

        this.mtxMediaList.resetToStart(this._startFrom, this._countToFetch, "descending", this._sortBy); //startFrom, countToFetch, defaultSortOrder, defaultSortBy
        this.mtxMediaList.setURL(urlStr);
        this.mtxMediaList.setNotificationURL(notificationUrlStr);
        // - set the data to the Src list
        this.mtxMediaList.setList();
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        //Clear if timer for thumbnail refresh is active
        if (!jsx3.util.strEmpty(this._tumbnailRefreshTimerId)) {
          clearInterval(this._tumbnailRefreshTimerId);
          this._tumbnailRefreshTimerId = null;
        }
        // clear lab list
        this.mtxMediaList.clearList();
        this._selectedSrc = "NONE";
        this._selectedSrcTitle = null;
        this._tumbnailRefreshTimerId = null;
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
  LivePanel_prototype._onSrcSelected = function () {
    // get the selected lab
    this._selectedSrc = this.mtxMediaList.getValue();
    this.mtxMediaList.setPersistSelection(this._selectedSrc);
    this._LOG.debug("Src Selected : " + this._selectedSrc);
    this.clsParent.onSrcSelected(this.SHOW_ON_MENU_SELECTION);
    // if empty selection 
    if (jsx3.util.strEmpty(this._selectedSrc)) {
      if (this.clsParent.controller.objCollabInfo.action == "Stop") {
        this._selectedSrcTitle = null;
        // stop any existing connection
        this.clsParent.objCollabInfo.isSourceSelected = "NONE";
        this.clsParent.objCollabInfo.SourceName = "";
        this.clsParent.objCollabInfo.SourceDetails = "";
        this.clsParent.controller.setCollaborationInfo(this.clsParent.objCollabInfo);
        this._stopSrcConnection();
      }
    }
    else {
      // get lab name
      var record = this.mtxMediaList.getRecord(this._selectedSrc);
      this.clsParent.switchPanel("VIDEO");
      this._selectedSrcTitle = record.jsxtext;
      this.clsParent.objCollabInfo.SourceName = this._selectedSrcTitle;
      this.clsParent.objCollabInfo.SourceDetails = "";
      this.clsParent.objCollabInfo.enablePVR = true;
      this.clsParent.objCollabInfo.enableRecord = true;
      this.clsParent.objCollabInfo.enableViewBookmarks = false;
      this.clsParent.objCollabInfo.enableAddBookmarks = true;
      this.clsParent.objCollabInfo.confirmOnStop = true;
      if (record.jsxdevstate != "Ready" && record.jsxdevstate != "Unmanaged") {
        this.clsParent.objCollabInfo.isSourceSelected = "OFFLINE";
        this.clsParent.controller.setCollaborationInfo(this.clsParent.objCollabInfo);
        this._stopSrcConnection();
      }
      else {
        // start a connection to it
        this.clsParent.objCollabInfo.isSourceSelected = "READY";
        this.clsParent.controller.setCollaborationInfo(this.clsParent.objCollabInfo);
        this._startSrcConnection();
      }
    }
  }


  //===================== Connection Start/Stop ======================================

  /* _startSrcConnection: start a connection to lab selected 
  * - send View request
  */
  LivePanel_prototype._startSrcConnection = function () {
    // setup 'View' request w/ pvrEnabled
    var reqParams = new Object();
    reqParams.ReqType = "View";
    reqParams.pvrEnable = "true";

    // - assign conn src info
    var srcTitle = this._selectedSrcTitle;
    // TBD - need to find out if NID is really used
    reqParams.SrcPortNID = this._selectedSrc;
    reqParams.SrcPortTitle = srcTitle;

    // - assign conn dest info
    reqParams.DstPortTitle = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_PORTTITLE);
    reqParams.DstEntityId = "";
    reqParams.DstPortNID = "";

    // assign sessionId
    // restart session with existing session id
    reqParams.SessionId = this.clsParent.sessionId;
    reqParams.RestartSession = true;

    // assign connId- using the same UUID with the SessionID
    // TBD - need to check if this is right - will end up with same connId all the time
    reqParams.ConnId = reqParams.SessionId.replace(/Session_/, "Conn_");
    reqParams.ConnName = srcTitle;

    // assign canvasId, windowId - reuse existing
    reqParams.CanvasId = this.clsParent.canvasId;
    reqParams.WindowId = this.clsParent.windowId;

    // call SessionAgent.View
    this.clsApp.clsSessionAgent.View(reqParams);
  }

  /* _stopSrcConnection: stop any lab connection
  * - fake a controller stop button press
  */
  LivePanel_prototype._stopSrcConnection = function () {
    // call SessionAgent.StopSession
    this.clsParent.controller.doStop();
  }

  /* 
  * - removes the selection 
  */
  LivePanel_prototype.deSelectSrc = function () {
    this.mtxMediaList.setValue(null);
    this.mtxMediaList.setPersistSelection(null);
  }

  /* 
  * - Returns the selected Src
  */
  LivePanel_prototype.getSelectedSrc = function () {
    return this._selectedSrc;
  }

  /* 
  * - Returns the selected Src title
  */
  LivePanel_prototype.getSelectedSrcTitle = function () {
    return this._selectedSrcTitle;
  }

});
