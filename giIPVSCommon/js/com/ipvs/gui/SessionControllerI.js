/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
 * Interface that provides methods for Session controls management.
 */
jsx3.Class.defineInterface("com.ipvs.gui.SessionControllerI", null, function (SessionControllerI, SessionControllerI_prototype) {

  /** @private @jsxobf-clobber */
  SessionControllerI_prototype._LOG = jsx3.util.Logger.getLogger(SessionControllerI.jsxclass.getName());

  SessionControllerI.CONNSTATE = new Object();
  SessionControllerI.CONNSTATE['NONE'] = -1;
  SessionControllerI.CONNSTATE['CREATED'] = 0;
  SessionControllerI.CONNSTATE['AUTHORIZING'] = 1;
  SessionControllerI.CONNSTATE['CHECKING_AVAILABILITY'] = 2;
  SessionControllerI.CONNSTATE['WAITING_FOR_DEST'] = 3;
  SessionControllerI.CONNSTATE['WAITING_FOR_SOURCE-DEST_CONTACTED'] = 4;
  SessionControllerI.CONNSTATE['WAITING_FOR_SOURCE-DEST_ACKNOWLEDGED'] = 5;
  SessionControllerI.CONNSTATE['WAITING_FOR_STREAMURL'] = 6;
  SessionControllerI.CONNSTATE['WAITING_FOR_STREAMSTART'] = 7;
  SessionControllerI.CONNSTATE['STREAMING'] = 8;
  SessionControllerI.CONNSTATE['STREAM_PAUSED'] = 9;
  SessionControllerI.CONNSTATE['DENIED_XAUTH'] = 10;
  SessionControllerI.CONNSTATE['REVOKED_XAUTH'] = 11;
  SessionControllerI.CONNSTATE['END_OF_MEDIA'] = 12;
  SessionControllerI.CONNSTATE['SETUP_ERROR'] = 201;
  SessionControllerI.CONNSTATE['START_ERROR'] = 202;
  SessionControllerI.CONNSTATE['SOURCE_ERROR'] = 203;
  SessionControllerI.CONNSTATE['MEDIA_ERROR'] = 204;
  SessionControllerI.CONNSTATE['DEST_ERROR'] = 205;
  SessionControllerI.CONNSTATE['DELETED'] = 999;

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~ init session controls ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

  /* initSessionControls
  * - init all session controls
  */
  SessionControllerI_prototype.initSessionControls = function () {
    // init collab state
    this.objCollabInfo = new Object();
    this.objCollabInfo.sessionId = "";
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~ update session controls ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
  /* resetSessionControls
  * - set default state for all session controls
  */
  SessionControllerI_prototype.resetSessionControls = function () {
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ collab info ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  
  /* 
  * setCollaborationInfo - sessionId and other params
  */
  SessionControllerI_prototype.setCollaborationInfo = function (objCollabInfo) {
    // subscribe to controller update events from session agent for this sessionId
    // check for change so that we only do it once in case this gets called again
    if ((!jsx3.util.strEmpty(objCollabInfo.sessionId)) && objCollabInfo.sessionId != this.objCollabInfo.sessionId) {
      // un sub from old session
      if (!jsx3.util.strEmpty(this.objCollabInfo.sessionId)) {
        this.clsApp.clsSessionAgent.unsubscribe(com.ipvs.agent.SessionAgent.MSG_CONTROLLERUPDATE + '_' + this.objCollabInfo.sessionId, this, this._onSessionControllerUpdate);
      }
      // sub to new session events
      this.clsApp.clsSessionAgent.subscribe(com.ipvs.agent.SessionAgent.MSG_CONTROLLERUPDATE + '_' + objCollabInfo.sessionId, this, this._onSessionControllerUpdate);
    }

    this.clsApp.setError(null, null);

    // set the sessionId
    this.objCollabInfo.sessionId = objCollabInfo.sessionId;
    // set the modes for other functions
    this.objCollabInfo.enablePVR = objCollabInfo.enablePVR;
    this.objCollabInfo.enableRecord = objCollabInfo.enableRecord;
    this.objCollabInfo.enableViewBookmarks = objCollabInfo.enableViewBookmarks;
    this.objCollabInfo.enableAddBookmarks = objCollabInfo.enableAddBookmarks;
    this.objCollabInfo.confirmOnStop = objCollabInfo.confirmOnStop;
    this.objCollabInfo.isSourceSelected = objCollabInfo.isSourceSelected;
    this.objCollabInfo.SourceName = jsx3.util.strEmpty(objCollabInfo.SourceName) ? "" : objCollabInfo.SourceName;
    this.objCollabInfo.SourceDetails = jsx3.util.strEmpty(objCollabInfo.SourceDetails) ? "" : objCollabInfo.SourceDetails;
    if (this.objCollabInfo.isSourceSelected == "READY" && this.objCollabInfo.state == 'NONE') {
      this.objCollabInfo.state = 'CREATED';
    }
    //    else {
    //      this.objCollabInfo.state = 'DELETING';
    //    }

    // Clear the tip msg
    this.objCollabInfo.connErrorInfo = null;

    // Update the collab info to the controller
    this.applyCollaborationInfo();
  }

  /* 
  * resetCollaborationInfo - init collaboration state vars
  *   - these are derived based on the session/connection states 
  *   - getCollabInfo - timeMode, playlength, state, connId
  *   - getCollabTimeInfo - action, isTimeshifted, timelineId
  *   - getCollabPVRInfo - canPVR, pvrConnId (optional)
  *   - getCollabRecordInfo - recState, recConnId (optional)
  *   - getCollabRecTimeInfo - recAction, recTimelineId
  *   - getCollabBookmarkInfo - canBookmark, bookmarkFileNID
  */
  SessionControllerI_prototype.resetCollaborationInfo = function () {
    this.objCollabInfo.timeMode = 'NONE';       //TimeMode
    this.objCollabInfo.timeInfo = null;         //TimeInfo
    this.objCollabInfo.pvrInfo = null;        // PvrInfo
    this.objCollabInfo.playlength = com.ipvs.api.Collaboration.DefaultTimeLength;
    this.objCollabInfo.state = 'NONE';       //CollaborationState
    this.objCollabInfo.connId = null;

    this.objCollabInfo.action = 'Stop';         //ControllerState
    this.objCollabInfo.isTimeshifted = false;   //CollaborationTimeState
    this.objCollabInfo.timelineId = null;

    this.objCollabInfo.recState = 'NONE';       //CollaborationRecordState
    this.objCollabInfo.recConnId = null;
    this.objCollabInfo.recAction = 'Stop';      //CollaborationRecordState
    this.objCollabInfo.recTimelineId = null;

    this.objCollabInfo.canBookmark = false;
    this.objCollabInfo.bookmarkFileNID = null;
  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ session, conn, timeline, presence event handlers ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~  

  /**
  * The _onSessionStateEvent method.
  * @param objSessionState {JSON}
  */
  SessionControllerI_prototype._onSessionStateEvent = function (objSessionState) {
    this._LOG.info(":onSessionStateEvent: sessionId:" + objSessionState.sessionId + ": state:" + objSessionState.state);
    // ignore the event if not for our sessionId
    if (objSessionState.sessionId != this.objCollabInfo.sessionId) return;
    // process depending on start/stop session
    switch (objSessionState.state) {
      case 'CREATED':
        // get the session doc 
        var docSession = this.clsApp.clsSessionAgent.getSession(objSessionState.sessionId);
        //   wrap it in collab
        this.docCollab = com.ipvs.api.Collaboration.Document.wrap(docSession);
        //   - getCollabInfo - timeMode, playlength, state, connId
        this.objCollabInfo.timeMode = this.docCollab.getCollabTimeMode();
        this.objCollabInfo.timeInfo = this.docCollab.getCollabTimeInfo();
        //this.objCollabInfo.playlength = this.docCollab.getCollabPlaylength();
        this.objCollabInfo.state = this.docCollab.getCollabState();
        this.objCollabInfo.connId = this.docCollab.getCollabConnId();
        //   - getCollabTimeInfo - action, isTimeshifted, timelineId
        this.objCollabInfo.timelineId = this.docCollab.getTimelineId();
        this.objCollabInfo.isTimeshifted = this.docCollab.isCollabTimeShifted();
        this.objCollabInfo.action = this.docCollab.getCollabControlState();
        //   - getCollabPVRInfo - canPVR, pvrConnId (optional)
        this.objCollabInfo.pvrInfo = this.docCollab.getPvrInfo();

        //   - getCollabRecordInfo - recState, recConnId (optional)
        this.objCollabInfo.recState = this.docCollab.getCollabRecordState();
        this.objCollabInfo.recConnId = this.docCollab.getCollabRecordConnId();
        //   - getCollabRecTimeInfo - recAction, recTimelineId    
        this.objCollabInfo.recAction = this.docCollab.getCollabRecordControlState();
        this.objCollabInfo.recTimelineId = this.docCollab.getRecordTimelineId();

        // TBD- getCollabBookmarkInfo - canBookmark, bookmarkFileNID
        this.objCollabInfo.canBookmark = this.docCollab.isCollabBookmarkAvailable();
        this.objCollabInfo.bookmarkFileNID = this.docCollab.getCollabBookmarkFileNID();

        break;
      case 'DELETED':
        // - no connstate updates on stop - reset the local collab state
        this.resetCollaborationInfo();
        this.docCollab = null;
        break;
      default:
        break;
    }
  }

  /**
  * The _onConnStateEvent method.
  * @param objConnState {JSON}
  // connId
  //  - update state
  // pvrConnId
  //  - update canPVR
  // recConnId
  //  - update recState
  */
  SessionControllerI_prototype._onConnStateEvent = function (objConnState) {
    this._LOG.info(":onConnStateEvent: sessionId:" + objConnState.sessionId + ": connId:" + objConnState.connId);
    // - main collab connection
    if (objConnState.connId == this.objCollabInfo.connId) {
      this.objCollabInfo.state = this.docCollab.getCollabState();
    }
    // - PVR connection
    else if (objConnState.connId == this.objCollabInfo.pvrInfo.connId) {
      this.objCollabInfo.pvrInfo.canPVR = this.docCollab.isCollabPVRAvailable();
      this.objCollabInfo.pvrInfo.pvrState = this.docCollab.getCollabState();
    }
    // - Record connection
    else if (objConnState.connId == this.objCollabInfo.recConnId) {
      this.objCollabInfo.recState = this.docCollab.getCollabRecordState();
      // update can boomark state
      this.objCollabInfo.canBookmark = this.docCollab.isCollabBookmarkAvailable();
    }
  }

  /**
  * The _onTimelineEvent method.
  * @param objTimeline {JSON}
  // timelineId
  //  - update action, isTimeshifted
  // recTimelineId
  //  - update recAction
  */
  SessionControllerI_prototype._onTimelineEvent = function (objTimeline) {
    // - main timeline
    if (objTimeline.timelineId == this.objCollabInfo.timelineId) {
      this.objCollabInfo.action = this.docCollab.getCollabControlState();
      this.objCollabInfo.timeInfo = this.docCollab.getCollabTimeInfo();
      this.objCollabInfo.isTimeshifted = this.docCollab.isCollabTimeShifted();
    }
    // - Record timeline
    else if (objTimeline.timelineId == this.objCollabInfo.recTimelineId) {
      this.objCollabInfo.recAction = this.docCollab.getCollabRecordControlState();
    }
  }

//  /**
//  * The _onPVRActionState method.
//  * @param objPVRAction {JSON}
//  // PVRAction failed
//  //  - update status tooltip
//  */
//  SessionControllerI_prototype._onPVRActionState = function (objPVRActionState) {
//    if (objPVRActionState.state == "Failed" && objPVRActionState.action != "SkipFwd") {
//      //this.objCollabInfo.errorInfo += ("\n" + objPVRActionState.action + " " + objPVRActionState.state);
//    }
//  }

//  /**
//  * The _onRecordStartState method.
//  * @param objRecordStartState {JSON}
//  // StartRecordState failed
//  //  - update status tooltip
//  */
//  SessionControllerI_prototype._onRecordStartState = function (objRecordStartState) {
//    if (objRecordStartState.state == "Failed") {
//      //this.objCollabInfo.errorInfo += ("\nStart Record Failed");
//    }
//  }

  //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ apply/update session controls ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~    

  /**
  * The applyCollaborationInfo method.
  * - repaint GUI based on objCollabInfo
  *
  */
  SessionControllerI_prototype.applyCollaborationInfo = function () {
    // process based on state
    switch (this.objCollabInfo.state) {
      case 'CREATED':
        this.objCollabInfo.msgStatus = 'CONNECTING...';
        this.objCollabInfo.connErrorInfo = null;
        break;
      case 'AUTHORIZING':
        this.objCollabInfo.msgStatus = 'AUTHORIZING...';
        this.objCollabInfo.connErrorInfo = null;
        break;
      case 'CHECKING_AVAILABILITY':
      case 'WAITING_FOR_DEST':
      case 'WAITING_FOR_SOURCE-DEST_CONTACTED':
      case 'WAITING_FOR_SOURCE-DEST_ACKNOWLEDGED':
      case 'WAITING_FOR_STREAMURL':
      case 'WAITING_FOR_STREAMSTART':
        this.objCollabInfo.msgStatus = 'CONNECTING...';
        this.objCollabInfo.connErrorInfo = null;
        break;
      case 'STREAMING':
        this.objCollabInfo.msgStatus = 'CONNECTED';
//        //check for PVR connection details
//        if (SessionControllerI.CONNSTATE[this.objCollabInfo.pvrInfo.pvrState] > SessionControllerI.CONNSTATE['STREAMING'] &&
//           SessionControllerI.CONNSTATE[this.objCollabInfo.pvrInfo.pvrState] < SessionControllerI.CONNSTATE['DELETED']) {
//          this.clsApp.setError(this.docCollab.getCollabPVRError(), "PVR controls not available");
//        }
        break;
      case 'DENIED_XAUTH':
      case 'REVOKED_XAUTH':
        this.objCollabInfo.msgStatus = 'DENIED';
        this.objCollabInfo.connErrorInfo = null;
        // stop session on conn Denied/revoked
        this._doStop();
        break;
      case 'END_OF_MEDIA':
      case 'SETUP_ERROR':
      case 'START_ERROR':
      case 'SOURCE_ERROR':
      case 'MEDIA_ERROR':
      case 'DEST_ERROR':
      case 'DELETED':
        //get the error code/decription from session
        this.objCollabInfo.connErrorInfo = this.docCollab.getCollabError();
        // stop session on error or conn end
        this._doStop();
        this.objCollabInfo.msgStatus = 'DISCONNECTING...';
        this.clsApp.setError(null, null);
        break;
      default:
        if (this.objCollabInfo.isSourceSelected == "OFFLINE") {
          var objError = new Object();
          objError.errorCode = "UNABLE TO CONNECT";
          objError.errorData = "Selected Source is not ready";
          this.objCollabInfo.connErrorInfo = objError;
          this.objCollabInfo.msgStatus = 'OFFLINE';
        }
        else {
          if (this.objCollabInfo.msgStatus != 'DENIED') {
            this.objCollabInfo.msgStatus = 'DISCONNECTED';
          }
        }
        this.clsApp.setError(null, null);
        break;
    }
  }

  /**
  * The _onSessionControllerUpdate method.
  * - process the update from session agent
  * - update the local objCollabInfo
  * - repaint GUI based on that
  *
  * @param objEvent {JSON}
  */
  SessionControllerI_prototype._onSessionControllerUpdate = function (objEvent) {
    // break out into specific update based on event
    if (objEvent && (objEvent.context.objSESSIONSTATE != null)) {
      this._onSessionStateEvent(objEvent.context.objSESSIONSTATE);
    }
    else if (objEvent && (objEvent.context.objCONNSTATE != null)) {
      this._onConnStateEvent(objEvent.context.objCONNSTATE);
      //ignoring the connstate event
      if (objEvent.context.objCONNSTATE.state == "STREAMING") {
        if (objEvent.context.objCONNSTATE.objTimeline.lastAction == "SkipBackward" || objEvent.context.objCONNSTATE.objTimeline.lastAction == "SkipForward") {
          return;
        }
      }
    }
    else if (objEvent && (objEvent.context.objTIMELINE != null)) {
      this._onTimelineEvent(objEvent.context.objTIMELINE);
    }
    else if (objEvent && (objEvent.context.objENTITYPRESENCE != null)) {
      // TBD - do nothing for now
      return;
    }
//    else if (objEvent && (objEvent.context.objPVRACTIONSTATE != null)) {
//      this._onPVRActionState(objEvent.context.objPVRACTIONSTATE);
//    }
//    else if (objEvent && (objEvent.context.objRECORDSTARTSTATE != null)) {
//      this._onRecordStartState(objEvent.context.objRECORDSTARTSTATE);
//    }
    // Update the collab info to the controller
    this.applyCollaborationInfo();
  }
});
