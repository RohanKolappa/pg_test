/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the session agent
* it presents a simplified method API to the GUI components on the one side
* and talks to the app server / other agents as needed on the other side
* actions and state update events are aggregated in terms of "connections" 
* one connection = one source + all the dests + assoc rec / pvr / relay
* methods should be invoked within try/catch blocks for exception handling 
*/
jsx3.require("com.ipvs.api.Session","com.ipvs.api.SessionInfo");
jsx3.Class.defineClass('com.ipvs.agent.SessionAgent', null, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function (SessionAgent, SessionAgent_prototype) {

  // create Logger for this class
  SessionAgent_prototype._LOG = jsx3.util.Logger.getLogger(SessionAgent.jsxclass.getName());


  SessionAgent.MSG_CONTROLLERUPDATE = "SessionControllerUpdate";
  SessionAgent.SHOW_CONTROLLER = "ShowController";

  /**
  * Instance variables
  */
  SessionAgent_prototype.arrSessions = null;

  /**
  * The instance initializer.
  */
  SessionAgent_prototype.init = function () {
    this._LOG.info("SessionAgent:init");
    this.arrSessions = new Object;

    this.clsApp = com.ipvs.app.App.clsApp;
    if (!this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  SessionAgent_prototype._onAppInit = function () {

    this.clsApp.clsNotification.subscribe(com.ipvs.api.Notification.MSG_TIMELINEEVENT, this, this.onTimelineEvent);
    this.clsApp.clsNotification.subscribe(com.ipvs.api.Notification.MSG_CONNSTATEEVENT, this, this.onConnStateEvent);
    this.clsApp.clsNotification.subscribe(com.ipvs.api.Notification.MSG_ENTITYPRESENCEEVENT, this, this.onEntityPresenceEvent);
    this.clsApp.clsNotification.subscribe(com.ipvs.api.Notification.MSG_SESSIONSTATEEVENT, this, this.onSessionStateEvent);
  };

  /**
  * The destroy method.
  */
  SessionAgent_prototype.destroy = function () {
    this._LOG.debug("SessionAgent:destroy");
    this.arrSessions = null;
  };

  /**
  * The getSession method. - add session to list
  * - fake a session state event
  * @param sessionId {String}
  * @return docSession {Session.Document}
  */
  SessionAgent_prototype.getSession = function (sessionId) {
    return (jsx3.util.strEmpty(this.arrSessions[sessionId]) ? null : this.arrSessions[sessionId].docSession);
  }

  /**
  * The _addSession method. - add session to list
  * - fake a session state event
  * @param nodeSession {String}
  */
  SessionAgent_prototype._addSession = function (nodeSession) {
    // wrap session node in a session document
    var docSession = new com.ipvs.api.Session.Document(nodeSession);

    // save it in the array by sessionId
    var sessionId = docSession.getID();
    // init the arr entry if null
    if (this.arrSessions[sessionId] == null) {
      this.arrSessions[sessionId] = new Object;
    }
    //check if already StopSession sent before vieworplayback response.
    if (this.arrSessions[sessionId].state == "DELETING") return;
    // TBD - what if it already exisits? just override it and log for now
    if (this.arrSessions[sessionId].docSession != null) {
      this._LOG.warn(":_addSession: sessionId:" + sessionId + ": Already exists");
    }
    this.arrSessions[sessionId].docSession = docSession;

    // *HACK * - mimic a fake notification object
    var objSessionState = new Object;
    objSessionState.subject = com.ipvs.api.Notification.MSG_SESSIONSTATEEVENT;
    objSessionState.sessionId = sessionId;
    // *HACK* - fake 'CREATED' state - should be docSession.getState();
    objSessionState.state = "CREATED";
    // notify controller of event
    this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + objSessionState.sessionId, context: { objSESSIONSTATE: objSessionState} });
  }

  /**
  * The onSessionStateEvent method.
  * @param objSessionState {String}
  */
  SessionAgent_prototype.onSessionStateEvent = function (objSessionState) {
    this._LOG.info(":onSessionStateEvent: sessionId:" + objSessionState.sessionId + ": state:" + objSessionState.state);
    // ignore the event if we dont have this session in our list
    var docSession = this.getSession(objSessionState.sessionId);
    if (docSession == null) return;
    // process depending on start/stop session
    switch (objSessionState.state) {
      case "CREATED":
        // if this was real - we would do get Session here - also deal with session restarts
        // the controller update event would happen in the 'when' success response
        return;
        break;
      case "DELETED":
        // dont delete yet - subscribed controllers might need session info to do cleanup
        // TBD - schedule session for GC from list
        break;
      default:
        break;
    }
    // notify controller of event
    this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + objSessionState.sessionId, context: { objSESSIONSTATE: objSessionState} });
    //    this.doEvent(SessionAgent.MSG_CONTROLLERUPDATE + '_' + objSessionState.sessionId, { objSESSIONSTATE: objSessionState });
  }

  /**
  * The onConnStateEvent method.
  * @param objConnState {JSON}
  */
  SessionAgent_prototype.onConnStateEvent = function (objConnState) {
    this._LOG.info(":onConnStateEvent: sessionId:" + objConnState.sessionId + ": connId:" + objConnState.connId);
    // ignore the event if we dont have this session in our list
    var docSession = this.getSession(objConnState.sessionId);
    if (docSession == null) return;
    // update the conn state to the session doc
    if (!docSession.updateConnState(objConnState.connId, objConnState)) {
      // if not exist - add - not really an option
      // TBD - should just getSession - and do update event in 'when'
    }
    // notify controller of event
    this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + objConnState.sessionId, context: { objCONNSTATE: objConnState} });
    //    this.doEvent(SessionAgent.MSG_CONTROLLERUPDATE + '_' + objConnState.sessionId, { objCONNSTATE: objConnState });
  }

  /**
  * The onTimelineEvent method.
  * @param objConnState {String}
  */
  SessionAgent_prototype.onTimelineEvent = function (objTimeline) {
    this._LOG.info(":onTimelineEvent: sessionId:" + objTimeline.sessionId + ": lastAction:" + objTimeline.lastAction);
    // ignore the event if we dont have this session in our list
    var docSession = this.getSession(objTimeline.sessionId);
    if (docSession == null) return;
    // update the timeline to the session doc
    if (!docSession.updateTimeline(objTimeline.timelineId, objTimeline)) {
      // if not exist - add
      docSession.addTimeline(objTimeline);
    }
    // notify controller of event
    this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + objTimeline.sessionId, context: { objTIMELINE: objTimeline} });
    //    this.doEvent(SessionAgent.MSG_CONTROLLERUPDATE + '_' + objTimeline.sessionId, { objTIMELINE: objTimeline });
  }

  /**
  * The onEntityPresenceEvent method.
  * @param objEntityPresence {JSON}
  */
  SessionAgent_prototype.onEntityPresenceEvent = function (objEntityPresence) {
    this._LOG.info(":onEntityPresenceEvent: sessionId:" + objEntityPresence.sessionId + ":objEntityPresence.entityId:" + objEntityPresence.entityId);
    // ignore the event if we dont have this session in our list
    var docSession = this.getSession(objEntityPresence.sessionId);
    if (docSession == null) return;
    // update the conn state to the session doc
    if (!docSession.updateEntity(objEntityPresence.entityId, objEntityPresence)) {
      // if not exist - add - not really an option
      // TBD - should just getSession - and do update event in 'when'
    }
    // notify controller of event
    this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + objEntityPresence.sessionId, context: { objENTITYPRESENCE: objEntityPresence} });
    //    this.doEvent(SessionAgent.MSG_CONTROLLERUPDATE + '_' + objEntityPresence.sessionId, { objENTITYPRESENCE: objEntityPresence });
  }

  /**
  * The View method.
  * @param reqParams {JASON}
  */
  SessionAgent_prototype.View = function (reqParams) {
    this._LOG.info("View: sessionId:" + reqParams.SessionId);
    this._ViewOrPlayback(reqParams);
  }
  /**
  * The Playback method.
  * @param reqParams {JASON}
  */
  SessionAgent_prototype.Playback = function (reqParams) {
    this._LOG.info("Playback: sessionId:" + reqParams.SessionId);
    this._ViewOrPlayback(reqParams);
  }

  /**
  * The ViewOrPlayback method.
  * @param reqParams {JASON}
  */
  SessionAgent_prototype._ViewOrPlayback = function (reqParams) {
    // init the sessionId arr entry if null
    if (this.arrSessions[reqParams.SessionId] == null) {
      this.arrSessions[reqParams.SessionId] = new Object;
    }

    this.arrSessions[reqParams.SessionId].state = "CREATING";
    // save the req params for replay later
    this.arrSessions[reqParams.SessionId].reqParams = reqParams;
    //send 'View' OR 'Playback' request
    com.ipvs.api.API.ViewOrPlayback(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      // if successful
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.debug(":ViewOrPlayback successful: response status:" + respStatus);
        // get the session node from the response
        var nodeSession = respVal.selectSingleNode("//Session");
        // add/update it in the session list
        this._addSession(nodeSession);
      }
      else {
        this._LOG.error(":ViewOrPlayback fail: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "View/Playback Failed");
        }
      }
    }).bind(this));
  }

  /**
  * The StopSession method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.StopSession = function (sessionId) {
    if (jsx3.util.strEmpty(this.arrSessions[sessionId])) {
      this._LOG.error("StopSession: There is no session with sessionId:" + sessionId);
      return;
    }
    this._LOG.info("StopSession: sessionId:" + sessionId);
    this.arrSessions[sessionId].state = "DELETING";
    var reqParams = new Object();
    reqParams.sessionId = sessionId;
    //send 'StopSession' request
    com.ipvs.api.API.StopSession(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.debug(":StopSession successful: response status:" + respStatus);
      }
      else {
        this._LOG.error(":StopSession fail: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "Stop session failed");
        }
      }
    }).bind(this));
  }

  /**
  * The RePlay method.
  * @param sessionId {String}
  * start -- stop -- start case
  * Controller class has to call resetControll while clicking differen Selection 
  */
  SessionAgent_prototype.rePlay = function (sessionId) {
    this._LOG.info("Play: sessionId:" + sessionId);
    //send 'VIEW' request again with previous params
    if ((this.arrSessions[sessionId] == null) && (this.arrSessions[sessionId].reqParams == null)) {
      this._LOG.error(":RePlay fail: no previous session params available.");
      return;
    }
    var reqParams = this.arrSessions[sessionId].reqParams;
    this._ViewOrPlayback(reqParams);
  }

  /**
  * The pvrAction method - internal method called by all pvr actions for main stream and recordings.
  * @param sessionId {String}
  * @param pvrAction {String}
  * -- PVR commands - pause / resume / catchup / skipback / skipfwd
  * @param actionData {Offset}
  */
  SessionAgent_prototype._pvrAction = function (sessionId, pvrAction, actionData) {
    if (this.arrSessions[sessionId] == null) {
      this._LOG.error(":pvrAction: " + pvrAction + " :Session Id: " + sessionId + ": Not found");
      return;
    }
    //Make request paramter object  
    var reqParams = new Object;
    reqParams.action = pvrAction;
    reqParams.sessionId = sessionId;
    reqParams.skipOffset = jsx3.util.strEmpty(actionData) ? "" : actionData;
    //Send out request with param object
    com.ipvs.api.API.SessionPvr(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.debug(":pvrAction successful: response status:" + respStatus);
      }
      else {
        this._LOG.error(":pvrAction fail: response status:" + respStatus);
        //get PVRActionNode from query in response
        var nodeQuery = response.selectSingleNode("//query");
        var nodeAction = nodeQuery.getFirstChild();
        if (!jsx3.util.strEmpty(nodeAction)) {
          //get PVRAction name and sessionId
          var pvrActionName = nodeAction.getNodeName();
          if (!jsx3.util.strEmpty(objResponse.ObjError)) {
            this.clsApp.setError(objResponse.ObjError, pvrActionName + " failed");
          }
          var sessionId = nodeAction.getAttribute("sessionId");
          //publish objPVRActionState to update controller
          var objPVRActionState = new Object();
          objPVRActionState.action = pvrActionName;
          objPVRActionState.state = "Failed";
          this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + sessionId, context: { objPVRACTIONSTATE: objPVRActionState} });
        }
      }
    }).bind(this));
  }

  /**
  * The pause method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.pause = function (sessionId) {
    this._LOG.info("Pause: sessionId:" + sessionId);
    this._pvrAction(sessionId, "Pause");
  }

  /**
  * The resume method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.resume = function (sessionId) {
    this._LOG.info("Resume: sessionId:" + sessionId);
    this._pvrAction(sessionId, "Resume");
  }

  /**
  * The resumeFromBookmark method.
  * @param sessionId {String}
  * @param bookmarkId {String}
  */
  SessionAgent_prototype.resumeFromBookmark = function (sessionId, bookmarkId) {
    this._LOG.info("ResumeFromBookmark: sessionId:" + sessionId + " bookmarkId: " + bookmarkId);
    if (this.arrSessions[sessionId] == null) {
      this._LOG.error("resumeFromBookmark: Session Id: " + sessionId + ": Not found");
      return;
    }
    var reqParams = new Object();
    reqParams.sessionId = sessionId;
    reqParams.bookmarkId = bookmarkId;
    //send 'ResumeFromBookmark' request
    com.ipvs.api.API.ResumeFromBookmark(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.debug(":resumeFromBookmark successful: response status:" + respStatus);
      }
      else {
        this._LOG.error(":resumeFromBookmark fail: response status:" + respStatus);
      }
    }).bind(this));
  }

  /**
  * The catchup method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.catchup = function (sessionId) {
    this._LOG.info("Catchup: sessionId:" + sessionId);
    this._pvrAction(sessionId, "Catchup");
  }

  /**
  * The skipback method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.skipback = function (sessionId, actionData) {
    this._LOG.info("Skipback: sessionId:" + sessionId);
    this._pvrAction(sessionId, "SkipBack", actionData);
  }

  /**
  * The skipfwd method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.skipfwd = function (sessionId, actionData) {
    this._LOG.info("Skipfwd: sessionId:" + sessionId);
    this._pvrAction(sessionId, "SkipFwd", actionData);
  }

  /**
  * The StartRecording method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.startRecording = function (sessionId, srcNID, metaData) {
    this._LOG.info("StartRecording: sessionId:" + sessionId);
    var docSession = this.getSession(sessionId);
    if (docSession == null) {
      this._LOG.error("startRecording: Session Id: " + sessionId + ": Not found");
      return;
    }
    // *HACK* - find the src entity from the existing session
    var nodeSrcEntity = docSession.getEntity("SrcPort", srcNID);
    // use it to set request params
    var reqParams = new Object();
    reqParams.SessionId = sessionId;
    reqParams.ConnId = "Conn_" + getUUID();
    reqParams.SrcEntityId = docSession.getEntityId(nodeSrcEntity);
    reqParams.DstFileTitle = docSession.getEntityName(reqParams.SrcEntityId);
    reqParams.resetTimeline = "true";
    //send 'Record' request
    com.ipvs.api.API.Record(reqParams, metaData).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.debug(":startRecording successful: response status:" + respStatus);
        var nodeSession = respVal.selectSingleNode("//Session");
        // add/update it in the session list
        this._addSession(nodeSession);
      }
      else {
        this._LOG.error(":startRecording fail: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "Start recording failed");
          this.publish({ subject: SessionAgent.MSG_CONTROLLERUPDATE + '_' + sessionId, context: { objRECORDSTARTSTATE: objResponse.ObjError} });
        }
      }
    }).bind(this));
  }

  /**
  * The StopRecording method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.stopRecording = function (sessionId) {
    this._LOG.info("StopRecording: sessionId:" + sessionId);
    var docSession = this.getSession(sessionId);
    if (docSession == null) {
      this._LOG.error("stopRecording: Session Id: " + sessionId + ": Not found");
      return;
    }
    // *HACK* - find the first record connection in the session
    var nodeConnIter = docSession.getConnIterByType("RECORD");
    var connId = "";
    var nodeRecConn = nodeConnIter.next();
    if (nodeRecConn != null) {
      connId = docSession.getConnId(nodeRecConn);
    }
    if (jsx3.util.strEmpty(connId)) return;
    // use that conn id to make request params
    var reqParams = new Object();
    reqParams.SessionId = sessionId;
    reqParams.ConnId = connId;
    //send 'DeleteConn' request to stop recording
    com.ipvs.api.API.SessionDeleteConn(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var respVal = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.debug(":stopRecording successful: response status:" + respStatus);
      }
      else {
        this._LOG.error(":stopRecording fail: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "Stop recording failed");
        }
      }
    }).bind(this));
  }

  /**
  * The pauseRecording method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.pauseRecording = function (sessionId) {
    this._LOG.info("PauseRecording: sessionId:" + sessionId);
    this._pvrAction(sessionId, "PauseRecord");
  }

  /**
  * The resumeRecording method.
  * @param sessionId {String}
  */
  SessionAgent_prototype.resumeRecording = function (sessionId) {
    this._LOG.info("ResumeRecording: sessionId:" + sessionId);
    this._pvrAction(sessionId, "ResumeRecord");
  }

});

