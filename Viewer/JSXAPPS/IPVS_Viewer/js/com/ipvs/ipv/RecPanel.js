/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* RecPanel class.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.RecPanel", com.ipvs.ipv.LiveRecCommon, null, function (RecPanel, RecPanel_prototype) {

  /** @private @jsxobf-clobber */
  RecPanel_prototype._LOG = jsx3.util.Logger.getLogger(RecPanel.jsxclass.getName());
  //property for MENU_SELECTION
  RecPanel_prototype.SHOW_ON_MENU_SELECTION = com.ipvs.ipv.MainPanel.MENU_RECPANEL;

  /**
  * Extended initializer.
  */
  RecPanel_prototype._onAppInit = function () {
    //init Lab Info
    this._selectedRecTitle = null;
    this._startFrom = 1;
    this._countToFetch = 15;
    this._init_RecList();
    this.jsxsuper();
    this._LOG.info("RecPanel: Initiated");
  }

  //===================== Init methods ======================================

  /** @private @jsxobf-clobber */
  RecPanel_prototype._init_RecList = function () {
    // get a handle to the list
    this.mtxRecordingsList = this.getDescendantOfName("mtxRecordingsList");
    // set list params
    var sortPathArr = { jsxtext: { SortAttr: "_SORTBY_", SortAttrVal: "dateCreated"} };
    this.mtxRecordingsList.initList("xsl/RecordingsList.xsl", true, sortPathArr); //XSLpath, paginationFlag, objSortPathArr
    this.mtxRecordingsList.resetToStart(this._startFrom, this._countToFetch, "descending", "dateCreated"); //startFrom, countToFetch, defaultSortOrder, defaultSortBy

    // set list Data URL - template - will change based on archiver selected, daterange and searchstr
    var objParam = new Object;
    objParam["SortOrder"] = "_SORTORDER_";
    objParam["SortBy"] = "_SORTBY_";
    objParam["SortByMetaData"] = "";
    objParam["SelectFormat"] = "EXTENDEDURL";
    objParam["StartFrom"] = "_STARTFROM_";
    objParam["CountToFetch"] = "_COUNTTOFETCH_";
    objParam["WhereTagExpr"] = "";
    objParam["WhereDateRange"] = "";
    objParam["WhereTextSubStr"] = "";
    objParam["SearchTextSubStrIn"] = "";
    objParam["WhereMetaDataExpr"] = "";
    objParam["WhereState"] = "Ready";
    objParam["WhereType"] = "MediaClip,MediaImage";
    objParam["WherePermissions"] = "PLAYBACK";
    objParam["SubscriptionID"] = "Subscription_" + jsx3.xml.CDF.getKey();
    this.mtxRecordingsList.setObjURLTemplate(objParam);
    // setup list to hadle notifications
    this.mtxRecordingsList.initNotificationHandling("mediastreamfileresourcelist");
    // clear selected and subscribe to onselect
    this._selectedRec = "NONE";
    this.mtxRecordingsList.subscribe(jsx3.gui.Interactive.SELECT, this, this._onRecSelected);

    // finally attach reference of mtx to pagination controls
    this.pagingControls = this.getDescendantOfName("blkPagination");
    this.pagingControls.initPagination(this.mtxRecordingsList);
    this.mtxRecordingsList.onAfterRecordUpdate = jsx3.$F(this.onRecListItemUpdate).bind(this);
  }


  /* Rec list Item refresh 
  * -For recording delete, UI will get Update notification instead of delete.
  * -To delete that item from list need to check the state.
  */
  RecPanel_prototype.onRecListItemUpdate = function (recordNode) {
    if (recordNode.getAttribute("jsxmediastate") != "Ready") {
      var recID = recordNode.getAttribute("jsxid");
      this.mtxRecordingsList.deleteRecord(recID);
      if (this._selectedRec == recID) {
        this._onRecSelected();
      }
    }
  }

  //===================== Event Handlers ======================================

  /** @private @jsxobf-clobber */
  /* Login State change
  * - extend common for RecPanel specific actions
  */
  RecPanel_prototype._onLoginStateChange = function (objEvent) {
    // call super to do common stuff
    // do RecPanel specific processing
    if (objEvent.stateVar != com.ipvs.agent.ServiceAgent.LOGINSTATE) return;
    var stateVal = objEvent.stateVal;
    switch (stateVal) {
      case com.ipvs.agent.ServiceAgent.LOGIN_SUCCESS:
        // set Src list Data URL
        var objURLParam = this.mtxRecordingsList.getObjURLTemplate();
        var urlStr = com.ipvs.api.API.GetFileURL(objURLParam);
        // - url for notification handling
        var objNotificationURLParam = this.mtxRecordingsList.getObjURLTemplate();
        objNotificationURLParam["WhereNID"] = "_WHERENID_";
        objNotificationURLParam["StartFrom"] = "1";
        objNotificationURLParam["CountToFetch"] = "5000";
        objNotificationURLParam["SubscriptionID"] = "";
        var notificationUrlStr = com.ipvs.api.API.GetFileURL(objNotificationURLParam);

        this.mtxRecordingsList.resetToStart(this._startFrom, this._countToFetch, "descending", "dateCreated"); //startFrom, countToFetch, defaultSortOrder, defaultSortBy
        this.mtxRecordingsList.setURL(urlStr);
        this.mtxRecordingsList.setNotificationURL(notificationUrlStr);
        // - set the data to the Src list
        this.mtxRecordingsList.setList();
        break;
      case com.ipvs.agent.ServiceAgent.LOGIN_LOGOUT_SUCCESS:
        // clear lab list
        this.mtxRecordingsList.clearList();
        this._selectedRec = "NONE";
        this._selectedRecTitle = null;
        // fake a Rec onSelect event to clear selection processing
        this._onRecSelected();
        break;
      default:
        return;
    }
  }

  //===================== Rec Selection ======================================

  /* Src selected 
  */
  RecPanel_prototype._onRecSelected = function () {
    // get the selected lab
    this._selectedRec = this.mtxRecordingsList.getValue();
    this.mtxRecordingsList.setPersistSelection(this._selectedRec);
    this._LOG.debug("Rec Selected : " + this._selectedRec);
    this.clsParent.onSrcSelected(this.SHOW_ON_MENU_SELECTION);
    // if empty selection 
    if (jsx3.util.strEmpty(this._selectedRec)) {
      if(this.clsParent.controller.objCollabInfo.action == "Stop"){
        this._selectedRecTitle = null;
        // stop any existing connection
        this.clsParent.objCollabInfo.isSourceSelected = "NONE";
        this.clsParent.objCollabInfo.SourceName = "";
        this.clsParent.objCollabInfo.SourceDetails = "";
        this.clsParent.controller.setCollaborationInfo(this.clsParent.objCollabInfo);
        this._stopRecConnection();
      }
    }
    else {
      // start a connection to it
      var record = this.mtxRecordingsList.getRecord(this._selectedRec);
      this.clsParent.switchPanel((record.jsxfileType == "MediaImage") ? "IMAGE" : "VIDEO");
      this.clsParent.objCollabInfo.enablePVR = true;
      this.clsParent.objCollabInfo.enableRecord = false;
      this.clsParent.objCollabInfo.enableViewBookmarks = true;
      this.clsParent.objCollabInfo.enableAddBookmarks = true;
      this.clsParent.objCollabInfo.confirmOnStop = false;
      this.clsParent.objCollabInfo.isSourceSelected = (record.jsxfileType == "MediaImage") ? "IMAGE" : "READY";
      this.clsParent.objCollabInfo.SourceName = record.jsxtextmediaName
      this.clsParent.objCollabInfo.SourceDetails = getDateTimeFromTimeCode(record.jsxtextmediaCreated);
      if (record.jsxfileType == "MediaImage") {
        this._stopRecConnection();
        this.clsParent.controller.setCollaborationInfo(this.clsParent.objCollabInfo);
        this.clsParent.clsImagePanel.showSnapShot(record.jsxmediaURL);
      }
      else {
        this.clsParent.controller.setCollaborationInfo(this.clsParent.objCollabInfo);
        this._startRecConnection();
      }
    }
  }


  //===================== Connection Start/Stop ======================================

  /* _startSrcConnection: start a connection to lab selected 
  * - send View request
  */
  RecPanel_prototype._startRecConnection = function () {
    var reqParams = new Object();
    var record = this.mtxRecordingsList.getRecord(this._selectedRec);

    // req type based on clip/grp selection  
    if (record.jsxfileType == "MediaClip") {
      reqParams.ReqType = "PlayBackSrc";
    }
    else {
      reqParams.ReqType = "PlayBackGrp";
    }

    // TBD - need to find out if NID is really used
    reqParams.FileNID = this._selectedRec;

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
    reqParams.ConnName = record.jsxclipID;

    // assign canvasId, windowId - reuse existing
    reqParams.CanvasId = this.clsParent.canvasId;
    reqParams.WindowId = this.clsParent.windowId;

    // call SessionAgent.Playback
    this.clsApp.clsSessionAgent.Playback(reqParams);
  }

  /* _stopSrcConnection: stop any lab connection
  * - fake a controller stop button press
  */
  RecPanel_prototype._stopRecConnection = function () {
    // call SessionAgent.StopSession
    this.clsParent.controller.doStop();
  }

  /* 
  * - removes the selection 
  */
  RecPanel_prototype.deSelectSrc = function () {
    this.mtxRecordingsList.setValue(null);
    this.mtxRecordingsList.setPersistSelection(null);
  }

  /* 
  * - Returns the selected Src
  */
  RecPanel_prototype.getSelectedRec = function () {
    return this._selectedRec;
  }

  /* 
  * - Returns the selected Src title
  */
  RecPanel_prototype.getSelectedRecTitle = function () {
    return this._selectedRecTitle;
  }

});
