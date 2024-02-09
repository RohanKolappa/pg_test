/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the controller agent
* it presents a simplified method API to the GUI components on the one side
* and talks to the app server / other agents as needed on the other side
* actions and state update events are aggregated in terms of "connections" 
* one connection = one source + all the dests + assoc rec / pvr / relay
* methods should be invoked within try/catch blocks for exception handling 
*/

jsx3.Class.defineClass('com.ipvs.agent.SessionCtrlAgent', null, null, function (SessionCtrlAgent, SessionCtrlAgent_prototype) {

  // create Logger for this class
  SessionCtrlAgent_prototype._LOG = jsx3.util.Logger.getLogger(SessionCtrlAgent.jsxclass.getName());

  /**
  * Instance varibales
  */
  SessionCtrlAgent_prototype.arrCtrlObj = null;
  SessionCtrlAgent_prototype.arrPresence = null;

  /**
  * The instance initializer.
  * @param eventApp {Object}
  */
  SessionCtrlAgent_prototype.init = function (eventApp) {
    this._LOG.debug("SessionCtrlAgent.init");
    this.arrCtrlObj = new Object;
    this.arrPresence = new Object;
    this.eventWrap(eventApp);
  };

  /**
  * The destroy method.
  */
  SessionCtrlAgent_prototype.destroy = function () {
    this._LOG.debug("SessionCtrlAgent.destroy");
    this.arrCtrlObj = null;
    this.arrPresence = null;
  };

  /**
  * The upon recieving the connStateEvent update the sessioninfo in the cache.
  * @param objConnState {String}
  */
  SessionCtrlAgent_prototype.sessionStateUpdate = function (objConnState) {
    var SessionID = null;
    for (var tCtrl in this.arrCtrlObj) {
      if (this.arrCtrlObj[tCtrl] == null) continue;
      SessionID = this.arrCtrlObj[tCtrl].SessionID;
      if (objConnState.sessionId != SessionID || jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].nodeSession)) {
        SessionID = null;
        continue;
      }

      //check if any request is in queue                                  
      if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].setGetSessionRequestTimer)) {
        var ThisClass = this;
        ThisClass.sessionTopic = tCtrl;
        this.arrCtrlObj[tCtrl].setGetSessionRequestTimer = setTimeout(function getSession() {
          topic_Set(ThisClass.sessionTopic, "SessionConnSelectFormat", "EXTENDED");
          topic_Set(ThisClass.sessionTopic, "SessionID", SessionID);
          //send 'GetSession' request and update nodesession here
          var getSessionReq = new cmpReq_GetSessionAPIPaginationList(ThisClass.sessionTopic, "GetSession",
                                                                                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "SessionConnSelectFormat" },
                                                                                                                                     { VarName: "ObjectList_WhereID", VarRef: "SessionID" }
                                                                                                                                    ],
                                                                                                                                    { action: "FNCCALL", context: ThisClass,
                                                                                                                                      fnc: function (respCode, respVal) {
                                                                                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                                                                                          if (ThisClass.arrCtrlObj[ThisClass.sessionTopic] != null) {
                                                                                                                                            var nodeSession = respVal.selectSingleNode(".//Session");
                                                                                                                                            var nodeConnIter = nodeSession.selectNodeIterator(".//Connection");
                                                                                                                                            while (nodeConnIter.hasNext()) {
                                                                                                                                              var nodeRecConn = nodeConnIter.next();
                                                                                                                                              var connType = nodeRecConn.getAttribute("type");
                                                                                                                                              if (nodeRecConn != null && connType == "RECORD") {
                                                                                                                                                var errorCode = nodeRecConn.getAttribute("errorCode");
                                                                                                                                                if (!jsx3.util.strEmpty(errorCode) && errorCode == "9021") {
                                                                                                                                                  topic_Set(tCtrl, "RecordingEnabled", 0);
                                                                                                                                                  topic_Publish(tCtrl, "CollaborationRecordState", "NONE");
                                                                                                                                                }
                                                                                                                                              }
                                                                                                                                            }
                                                                                                                                            var sessionId = nodeSession.getAttribute("id");
                                                                                                                                            ThisClass.arrCtrlObj[ThisClass.sessionTopic].nodeSession = nodeSession;
                                                                                                                                            var sessionObj = new Object();
                                                                                                                                            sessionObj.sessionId = sessionId;
                                                                                                                                            topic_Publish(ThisClass.sessionTopic, "UpdateSession", sessionObj);
                                                                                                                                            ThisClass.notifySessionState(ThisClass.sessionTopic);
                                                                                                                                          }
                                                                                                                                        }
                                                                                                                                        else {
                                                                                                                                          this._LOG.info("IPVSClientUI:GetSessionRequest failed. Error Code :" + respCode);
                                                                                                                                        }
                                                                                                                                        //clear out the timer
                                                                                                                                        if (ThisClass.arrCtrlObj[ThisClass.sessionTopic] && ThisClass.arrCtrlObj[ThisClass.sessionTopic].setGetSessionRequestTimer) {
                                                                                                                                          clearTimeout(ThisClass.arrCtrlObj[ThisClass.sessionTopic].setGetSessionRequestTimer);
                                                                                                                                          ThisClass.arrCtrlObj[ThisClass.sessionTopic].setGetSessionRequestTimer = null;
                                                                                                                                        }
                                                                                                                                      }
                                                                                                                                    }
                                                                                                                                  );

          var reqParams = new Object;
          reqParams["SortOrder"] = "ascending";
          reqParams["ObjectCount"] = "";
          reqParams["StartFrom"] = "";
          reqParams["CountToFetch"] = "";
          getSessionReq.reqParams = reqParams;
          getSessionReq.sendRequest(ThisClass);
        }, 1000);

      }
    }
  }

  /**
  * The timeLineEvent method.
  * @param objConnState {String}
  */
  SessionCtrlAgent_prototype.timeLineEvent = function (objTimeLine) {
    var SessionID = null;
    for (var tCtrl in this.arrCtrlObj) {
      if (this.arrCtrlObj[tCtrl] == null) continue;
      SessionID = this.arrCtrlObj[tCtrl].SessionID;
      if (objTimeLine.sessionId != SessionID || jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].nodeSession)) {
        continue;
      }
      var nodeSession = this.arrCtrlObj[tCtrl].nodeSession;
      var timelineId = nodeSession.getAttribute("timelineId");
      var recTimelineId = nodeSession.getAttribute("recordTimelineId");
      var objTimeLineId = objTimeLine.Id;
      // check if the ID is for the STREAM 
      if (objTimeLineId == timelineId) {
        topic_Set(tCtrl, "ActionMediaOffset", objTimeLine.lastOffset);
        topic_Set(tCtrl, "lastActionWallClock", objTimeLine.lastWC);
        topic_Set(tCtrl, "TimeScale", objTimeLine.timeScale);
        // clear controller mask
        topic_Set(tCtrl, "ControllerMask", "NONE");
        switch (objTimeLine.lastAction) {
          case "Pause":
          case "SkipBackward":
          case "SkipForward":
          case "ResumeFrom":
            topic_Publish(tCtrl, "CollaborationTimeState", "TIMESHIFTED");
            break;
          case "Resume":
            break;
          case "Catchup":
            topic_Publish(tCtrl, "CollaborationTimeState", "LIVE");
            break;
          default:
            break;
        }
        //bug fix 20369 remove unnecessary check under before session-mediaroom-stream structure(session-connection now)
        topic_Publish(tCtrl, "ControllerState", objTimeLine.lastAction);
        if (objTimeLine.lastAction != "Pause" && topic_Get(tCtrl, "CollaborationFFTrackState") != "NONE") {
          topic_Publish(tCtrl, "PlayBySpeed", objTimeLine.timeScale + "x");
        }
      }
      // check if the ID is for the RECORD
      else if (objTimeLineId == recTimelineId) {
        if (objTimeLine.lastAction == "Pause") {
          topic_Publish(tCtrl, "CollaborationRecordState", "PauseRecord");
        }
        else if (objTimeLine.lastAction == "Resume" || objTimeLine.lastAction == "Catchup") {
          topic_Publish(tCtrl, "CollaborationRecordState", "CONNECTED");
        }
      }

    }
  }

  /**
  * The notifyEntityPresence method.
  * @param tCtrl {String}
  * @param nodeEntity {XML Node}
  * @param presence {String}
  */
  SessionCtrlAgent_prototype.notifyEntityPresence = function (tCtrl, nodeEntity, presence) {
    // mark entity presence
    nodeEntity.setAttribute("presence", presence.State);
    // put presence info in entity presence update event
    var objPresence = new Object;
    objPresence.entityId = nodeEntity.getAttribute("id");
    objPresence.JID = presence.JID;

    objPresence.State = (presence.State == true ? "READY" : "OFFLINE");
    topic_Publish(tCtrl, "EntityPresence", objPresence);
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nEntity Presence: JID: " + objPresence.JID + " : State: " + objPresence.State);
  }

  /**
  * The notifySessionState method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.notifySessionState = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("CtrlAgent: notifySessionState: CtrlTopic: " + tCtrl + ": Not found");
    var objCtrl = this.arrCtrlObj[tCtrl];
    var previousLastAction = objCtrl.lastAction;
    var nodeSession = new com.ipvs.api.Session.Document(objCtrl.nodeSession);
    // setup default time vars
    var TimeMode = "NONE";
    var Playlength = DefaultTimeLength; //change 3600000 to 4*3600000;
    var trimLength = DefaultTimeLength;
    // if collab info is available
    if (nodeSession) {
      //add for task 19599 check "sessionType"
      var sessionType = nodeSession.getSessionTypeAttribute();
      //se the nodeSession in the cache
      topic_Set(tCtrl, "NodeSession", nodeSession);
      //set session name for 'autojoin' client
      if (topic_Get(tApp, "DefaultAutoJoin") == "true") {
        topic_Publish(tCtrl, "CollaborationName", nodeSession.getName());
      }
      var publishViewBookmarkFlag = false;
      var nodeConnIter = nodeSession.getConnIterByType("RECORD");
      while (nodeConnIter.hasNext()) {
        var nodeRecConn = nodeConnIter.next();
        // if found then set RECORDINGENABLE =1 for the controller
        if (nodeRecConn != null) {
          this._LOG.info("clsCtrlAgent::notifySessionState:RECORD CONNECTION:nodeSession.getConnState(nodeRecConn)=" + nodeSession.getConnState(nodeRecConn));
          // remove the check of nodeSession.getConnState(nodeRecConn) == "STREAMING" as the stat may be different in dynamic
          if (sessionType != "MIXED") {
            topic_Set(tCtrl, "AddBookmarkEnabled", 1);
            topic_Set(tCtrl, "RecordingEnabled", 1);
            if (topic_Get(tCtrl, "CollaborationRecordState") != "PauseRecord") {
              topic_Publish(tCtrl, "CollaborationRecordState", "CONNECTED");
            }
            publishViewBookmarkFlag = true;
          }
        }
      }
      //using entity iterator in case of a session without connection
      var nodeEntityIter = nodeSession.getEntityIter();
      var maxPlayLength = 0;
      while (nodeEntityIter.hasNext()) {
        var entityId = nodeEntityIter.next().getAttribute("id");
        if (!entityId) {
          var errStr = "clsCtrlAgent::notifySessionState:: srcEntityId = null";
          this._LOG.error(errStr);
          topic_Publish(tCtrl, "ERRORSTATUS", "\n " + errStr + ": SessionID: " + objCtrl.SessionID + " ...ERROR");
          return;
        }
        var nodeEntity = nodeSession.getEntityObj(entityId);
        if (nodeEntity.type == "SrcPort") {
          if (nodeEntity.PVREnable == "true" && sessionType != "MIXED") {
            //CASE_1 :PVR -- check PVR enabled and publish if true
            TimeMode = "PVR";
            maxPlayLength = DefaultTimeLength;
            var srcEntityIter = nodeSession.getEntityIterByType(com.ipvs.api.SessionInfo.ENTITYTYPE_SRCPORT);
            //take the lowest value of the trimlength across all the SrcPort elements (bug#38463)
            while (srcEntityIter.hasNext()) {
              var srcEntityId = srcEntityIter.next().getAttribute("id");
              var srcNodeEntity = nodeSession.getEntityObj(srcEntityId);
              if (!jsx3.util.strEmpty(srcNodeEntity.trimLength) && parseInt(trimLength) > parseInt(srcNodeEntity.trimLength)) {
                trimLength = srcNodeEntity.trimLength;
              }
            }
            topic_Set(tCtrl, "CollaborationPVRState", "CONNECTED");
            break;
          }
          else {
            //CASE_2 :LIVE -- check LIVE enabled and publish
            TimeMode = "LIVE";
            maxPlayLength = DefaultTimeLength;
          }
        }
        else if (nodeEntity.type == "PlyFile" && sessionType != "MIXED") {
          //CASE_3 :PLAYBACK -- check srcEntity type="File"
          TimeMode = "PLAYBACK";
          //update Playlength
          Playlength = nodeEntity.playLength;
          if (jsx3.util.strEmpty(Playlength)) {
            Playlength = DefaultTimeLength;
          }
          if (nodeEntity.grpFileType == "SyncPly") {
            if (parseInt(Playlength) > parseInt(maxPlayLength)) {
              maxPlayLength = Playlength;
            }
          }
          else {
            if (parseInt(maxPlayLength) > parseInt(Playlength)) {
              maxPlayLength = Playlength;
            }
          }
          publishViewBookmarkFlag = true;
          //publishing CollaborationPVRState to enable PVR buttons 
          topic_Set(tCtrl, "CollaborationPVRState", "CONNECTED");
        }
      }

      //for bug fix 27157:played from bookmark in muliple streams
      var numberOfGrpFile = nodeSession.getGrpFiles();

      //avoid repeatly publish "ViewBookmarkEnabled"
      if (publishViewBookmarkFlag && numberOfGrpFile < 2) {
        // this publish is being listened in the cmpSessionBookmakr.js
        topic_Publish(tCtrl, "ViewBookmarkEnabled", 1);
      }
    }

    //check session for timeline info otherwise use timeline info from mediaroom.
    var timelineId = nodeSession.getTimelineId();
    if (!jsx3.util.strEmpty(timelineId) && !jsx3.util.strEmpty(nodeSession.getTimeline(timelineId))) {
      var startWallClock = nodeSession.getTimelineStartWC(timelineId);
      var lastActionWallClock = nodeSession.getTimelineLastWC(timelineId);
      var lastTrackOffset = nodeSession.getTimelineLastOffset(timelineId);
      var lastAction = nodeSession.getTimelineRequestedAction(timelineId);
      var timeScale = nodeSession.getTimelineTimeScale(timelineId);
    }
    else {
      // notify action / action data vars based on info in mediaroom
      var nodeMediaRoom = objCtrl.nodeMediaRoom;
      var startWallClock = nodeMediaRoom.selectSingleNode(".//StreamTrack/data/TrackMediaTimeline/Start/@WC").getValue();
      var lastActionWallClock = nodeMediaRoom.selectSingleNode(".//StreamTrack/data/TrackMediaTimeline/Last/@WC").getValue();
      var lastTrackOffset = nodeMediaRoom.selectSingleNode(".//StreamTrack/data/TrackMediaTimeline/Last/@Offset").getValue();
      var lastAction = nodeMediaRoom.selectSingleNode(".//StreamTrack/data/TrackMediaTimeline/Last/@action").getValue();
      var timeScale = nodeMediaRoom.selectSingleNode(".//StreamTrack/data/TrackMediaTimeline/Last/@TimeScale").getValue();
    }

    //When we start normal session the lastAction is 'Start'
    //but when we start Mediagroup session then the lastAction is 'ResumeFrom' instead of 'Start'
    //bug#24074
    if (previousLastAction == "NONE" && lastAction == "ResumeFrom") {
      lastAction = "Start";
    }

    if (lastAction == "Pause" || lastAction == "Resume" || lastAction == "ResumeFrom" ||
         lastAction == "SkipBackward" || lastAction == "SkipForward" ||
         TimeMode == "PLAYBACK") {
      TimeState = "TIMESHIFTED";
    }
    else {
      TimeState = "LIVE";
    }

    //FFTrack
    //Phase-I supporting FF only for playback session
    //check atleast one destination has trickPlay='true'
    var FFTrackState = 'NONE';
    if (sessionType == com.ipvs.api.SessionInfo.SESSIONTYPE_PLAYBACK && nodeSession.isDestTrickPlayAvailable()) {
      //FFTrackState = 'SLOWONLY'; //In phase-I not supporting slow motion
      if (nodeSession.isSrcFFTrackAvailable()) {
        FFTrackState = 'ALL';
        if (!jsx3.util.strEmpty(timeScale)) {
          if (timeScale != "0.0") {
            topic_Publish(tCtrl, "PlayBySpeed", timeScale + "x");
          }
          topic_Set(tCtrl, "TimeScale", parseInt(timeScale, 10));
        }
      }
    }

    topic_Set(tCtrl, "PlayLength", maxPlayLength);
    topic_Set(tCtrl, "TrimLength", trimLength);
    topic_Publish(tCtrl, "TimeMode", TimeMode);
    topic_Publish(tCtrl, "CollaborationTimeState", TimeState);
    topic_Set(tCtrl, "startWallClock", startWallClock);
    topic_Set(tCtrl, "lastActionWallClock", lastActionWallClock);
    topic_Set(tCtrl, "ActionMediaOffset", lastTrackOffset);
    topic_Set(tCtrl, "CollaborationFFTrackState", FFTrackState);
    //repaint controller UI
    topic_Publish(tCtrl, "ControllerState", lastAction);

    //repaint controller UI, call it connected
    topic_Publish(tCtrl, "CollaborationState", "CONNECTED");
    // TBD - need to account for Authenticating state
    topic_Publish(tCtrl, "CollaborationStatusMsg", "CONNECTED");
  }

  /**
  * The resetController method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.resetController = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) return;
    var objCtrl = this.arrCtrlObj[tCtrl];
    // clear any pending presences
    if (objCtrl.MediaRoomNID != null) {
      this.arrPresence[objCtrl.MediaRoomNID] = null;
      objCtrl.nodeMediaRoom = null;
      objCtrl.MediaRoomNID = null;
    }
    // - clear session info
    objCtrl.nodeSession = null;
    objCtrl.SessionID = null;

    // set the lastAction
    objCtrl.lastAction = "NONE";

    // init state vars 
    topic_Set(tCtrl, "TimeScale", 1);
    topic_Set(tCtrl, "PlayLength", "");
    //topic_Set(tCtrl, "StreamStartOffset", 0);
    topic_Publish(tCtrl, "TimeMode", "NONE");

    topic_Publish(tCtrl, "CollaborationTimeState", "LIVE");

    topic_Set(tCtrl, "ActionData", 0);
    topic_Set(tCtrl, "ActionMediaOffset", 0);
    topic_Publish(tCtrl, "AddTrackActionState", "NONE");

    topic_Set(tCtrl, "ControllerMask", "NONE");
    topic_Set(tCtrl, "ControllerEvent", "NONE");
    topic_Publish(tCtrl, "ControllerState", "NONE");

    topic_Publish(tCtrl, "CollaborationPVRState", "NONE");

    topic_Set(tCtrl, "NIDForBookmark", "");
    topic_Set(tCtrl, "NIDForGapInfo", null);
    topic_Set(tCtrl, "ViewBookmarkEnabled", null);
    topic_Set(tCtrl, "AddBookmarkEnabled", 0);
    topic_Set(tCtrl, "EditBookmarkEnabled", 0);
    topic_Publish(tCtrl, "BookmarkState", "NONE");
    topic_Set(tCtrl, "RecordingEnabled", 0);
    topic_Publish(tCtrl, "CollaborationRecordState", "NONE");

    topic_Publish(tCtrl, "ERRORSTATUS", "\nNONE");
    topic_Publish(tCtrl, "CollaborationStatusMsg", "DISCONNECTED");
    topic_Set(tCtrl, "SessionID", "NONE");
    topic_Publish(tCtrl, "CollaborationState", "NONE");

  }


  /**
  * The unregisterController method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.unregisterController = function (tCtrl) {
    // clear out state vars
    //    this.resetController(tCtrl);
    if (this.arrCtrlObj[tCtrl] == null) return;
    var objCtrl = this.arrCtrlObj[tCtrl];
    // clear any pending presences
    if (objCtrl.MediaRoomNID != null) {
      this.arrPresence[objCtrl.MediaRoomNID] = null;
      objCtrl.nodeMediaRoom = null;
      objCtrl.MediaRoomNID = null;
    }
    // - clear session info
    objCtrl.nodeSession = null;
    objCtrl.SessionID = null;

    // set the lastAction
    objCtrl.lastAction = "NONE";
    // wipe out arr entry
    this.arrCtrlObj[tCtrl].arrReqSeq = null;
    this.arrCtrlObj[tCtrl] = null;
  }

  /**
  * The registerController method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.registerController = function (tCtrl) {
    // check / create new entry in arr 
    if (this.arrCtrlObj[tCtrl] != null) throw new jsx3.Exception("CtrlAgent: Register: CtrlTopic: " + tCtrl + ": Already exists");
    this.arrCtrlObj[tCtrl] = new Object;
    // - create request list object
    var objCtrl = this.arrCtrlObj[tCtrl];
    objCtrl.arrReqSeq = new Object;
    // initialize the objects and state vars
    this.resetController(tCtrl);
  }

  /**
  * The joinSession method.
  * @param tCtrl {String}
  * @param SessionID {String}
  */
  SessionCtrlAgent_prototype.joinSession = function (tCtrl, SessionID) {

    // check entity 
    function joinCheckEntity(tCtrl, MediaRoomNID) {
      if (this.arrCtrlObj[tCtrl] == null) return;
      var objCtrl = this.arrCtrlObj[tCtrl];
      // get the mediaroom
      objCtrl.MediaRoomNID = MediaRoomNID;
      var nodeMediaRoom = topic_GetNIDNode(tCache, MediaRoomNID);
      topic_Publish(tCtrl, "CollaborationEventMsg", "\nJoin Session: CtrlTopic: " + tCtrl + ": MediaRoomNID: " + MediaRoomNID);
      var nodeMediaRoomString = nodeMediaRoom.toString().replace(/</g, "&lt;").replace(/>/g, "&gt;");
      objCtrl.nodeMediaRoom = nodeMediaRoom.cloneNode(true);
      // &#042;=*
      topic_Publish(tCtrl, "CollaborationEventMsg", "\n&#042;&#042;&#042;Join Session: CtrlTopic: " + tCtrl + ": MediaRoomNode:\n " + nodeMediaRoomString + "\n\n");

      // get session node if present
      var nodeSession = nodeMediaRoom.selectSingleNode(".//Session");
      objCtrl.nodeSession = nodeSession ? nodeSession.cloneNode(true) : null;
      var nodeSessionString = nodeSession.toString().replace(/</g, "&lt;").replace(/>/g, "&gt;");
      topic_Publish(tCtrl, "CollaborationEventMsg", "\n&#042;&#042;&#042;Join Session: CtrlTopic: " + tCtrl + ": SessionNode:\n " + nodeSessionString + "\n\n");

      //if current resource is not in the session's Entity/Port/resourceTitle,
      //join the session so as to control it (sessions like:Tx/Rx or UDP/UDP);
      if (objCtrl.nodeSession) {
        var ThisClass = this;

        //var loginResource = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_RESOURCE);
        // port title/name can be changed from admin ui. so using the actual port title/name not the resource name
        var loginResourcePortTitle = topic_Get(tApp, "DefaultPortTitle");
        var entityPortResource = objCtrl.nodeSession.selectSingleNode(".//EntityList/Entity/Port[@resourceTitle='" + loginResourcePortTitle + "']");
        if (!entityPortResource) {
          topic_Publish(tCtrl, "MediaPlayerMode", "CONTROLLER");
          //don't find loginResource in the session's Entity list,send a request to join the session
          var reqGetSessionList = new ReqSessionJoinRequest(objCtrl.SessionID,
                                      { action: "FNCCALL", context: this,
                                        fnc: function (respCode, respVal) {
                                          var tt = "ttt";
                                          if (respCode == "SUCCESS" && respVal != null) {
                                            var errorNode = objCtrl.nodeSession.selectSingleNode("");
                                            if (objCtrl.MediaRoomNID == respVal.getValue()) {
                                              entityPortResource = objCtrl.nodeSession.selectSingleNode(".//EntityList/Entity/Port[@resourceTitle='" + loginResourcePortTitle + "']");
                                              //below 'IF' comments will be removed after middleware add 'loginResource' to EntityList 
                                              //if (entityPortResource) {
                                              // if join successful new entity of 'loginResource' will be added to the session node
                                              this._LOG.info("clsCtrlAgent::joinCheckEntity:request join session(" + objCtrl.SessionID + ") successful, roomNID:" + objCtrl.MediaRoomNID);
                                              joinSuccess.call(ThisClass, tCtrl);
                                              //}
                                            }
                                            else {
                                              //defensive log: return a different room to be joined
                                              this._LOG.error("clsCtrlAgent::joinCheckEntity:request join room(" + objCtrl.MediaRoomNID + "), but return room:" + respVal.getValue());
                                            }
                                          }
                                          else {
                                            this._LOG.warn("clsCtrlAgent::joinCheckEntity:Response fail for join session(" + objCtrl.SessionID + ").");
                                            this._LOG.error(respVal);
                                          }
                                        }
                                      });
          reqGetSessionList.reqParams = new Object();
          reqGetSessionList.reqParams["Where_xpath"] = "";
          reqGetSessionList.sendRequest(this);
        }
        else {
          topic_Publish(tCtrl, "MediaPlayerMode", "PLAYER");
          //if 'loginResource' is already in the session's Entity list,just join it
          joinSuccess.call(ThisClass, tCtrl);
        }
      }
      else {
        //defensive log:nodeSession is null
        this._LOG.error("There isn't any session node in room (" + MediaRoomNID + ").");
      }
    }

    function joinError(tCtrl, ErrorResponse) {
      if (this.arrCtrlObj[tCtrl] == null) return;
      var objCtrl = this.arrCtrlObj[tCtrl];
      topic_Publish(tCtrl, "ERRORSTATUS", "\nJoin Session: CtrlTopic: " + tCtrl + ": SessionID: " + objCtrl.SessionID + " ...ERROR");
      var ErrorResponseString = ErrorResponse.toString().replace(/</g, "&lt;").replace(/>/g, "&gt;");
      // publish error to topic
      topic_Publish(tCtrl, "ERRORSTATUS", "\nJoin Session: CtrlTopic: " + tCtrl + ": ErrorResponse: " + ErrorResponseString);
    }

    function joinSuccess(tCtrl) {
      topic_Publish(tCtrl, "CollaborationEventMsg", "\nJoin Session: CtrlTopic: " + tCtrl + ": SessionID: " + objCtrl.SessionID + " ...Success");
      //check and publish sessionOwnerUser, disable 'STOP' button if current client is not the session owner
      var sessionOwnerUserJID = topic_Get(tCtrl, "SessionOwnerUserJID");
      if (!jsx3.util.strEmpty(sessionOwnerUserJID) && topic_Get(tApp, "LoginUserJID") != sessionOwnerUserJID) {
        topic_Publish(tCtrl, "SessionOwnerUser", 'FALSE');
      }
      this.notifySessionState(tCtrl);
    }

    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("CtrlAgent: JoinCollaboration: CtrlTopic: " + tCtrl + ": Not found");
    var objCtrl = this.arrCtrlObj[tCtrl];
    // associate this session with this controller  
    objCtrl.SessionID = SessionID;

    topic_Publish(tCtrl, "CollaborationEventMsg", "\nJoin Session: CtrlTopic: " + tCtrl + ": SessionID: " + objCtrl.SessionID + " ...Begin");

    // get mediaroom node
    var ThisClass = this;
    var reqGetMediaRoomList = new ReqGetObjectList("MediaRoom",
                                                      { action: "FNCCALL", context: ThisClass,
                                                        fnc: function (respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                            var mediaroomNodeIter = respVal.selectNodeIterator(".//ObjectNID");
                                                            var findRoom = false;
                                                            while (mediaroomNodeIter.hasNext()) {
                                                              var roomNID = mediaroomNodeIter.next().getValue();
                                                              var nodeMediaRoom = topic_GetNIDNode(tCache, roomNID);
                                                              if (nodeMediaRoom) {
                                                                var roomSessionID = nodeMediaRoom.getAttribute("id");
                                                                if (roomSessionID == objCtrl.SessionID) {
                                                                  joinCheckEntity.call(ThisClass, tCtrl, roomNID);
                                                                  findRoom = true;
                                                                  break;
                                                                }
                                                              }
                                                            }
                                                            if (!findRoom) {
                                                              this._LOG.error("CtrlAgent: JoinSession: CtrlTopic: " + tCtrl + ": Get MediaRoom for Session: " + objCtrl.SessionID + "not found");
                                                              joinError.call(ThisClass, tCtrl, respVal);
                                                            }
                                                          }
                                                          else {
                                                            this._LOG.error("CtrlAgent: JoinSession: CtrlTopic: " + tCtrl + ": Get MediaRoom for Session: " + objCtrl.SessionID + "request failed");
                                                            joinError.call(ThisClass, tCtrl, respVal);
                                                          }
                                                        }
                                                      }
                                                    );
    reqGetMediaRoomList.ServiceName = "mediaroomservice";
    reqGetMediaRoomList.sendRequest(this);
  }

  /**
  * The leaveSession method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.leaveSession = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) return;
    // initialize the objects and state vars
    this.resetController(tCtrl);
  }

  /**
  * The startSession method.
  * @param tCtrl {String}
  * @param nodeSession {XML Node}
  */
  SessionCtrlAgent_prototype.startSession = function (tCtrl, nodeSession) {
    var ThisClass = this;
    var reqStartSession = new ReqSessionStartStopRequest("Start", nodeSession,
                                                            { action: "FNCCALL", context: ThisClass,
                                                              fnc: function (respCode, respVal) {
                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                  topic_Publish(tCtrl, "STARTSESSIONSUCCESS", nodeSession.getAttribute("id"));
                                                                }
                                                                else {
                                                                  this._LOG.error("CtrlAgent: StartSession: CtrlTopic: " + tCtrl + ": Start Session for SessionID: " + nodeSession.getAttribute("id") + " request failed");
                                                                  topic_Publish(tCtrl, "ERRORSTATUS", nodeSession.getAttribute("id"));
                                                                }
                                                              }
                                                            }
                                                          );
    reqStartSession.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["SessionID"] = nodeSession.getAttribute("id");
    reqParams["SessionConfigID"] = "";
    reqStartSession.reqParams = reqParams;
    reqStartSession.sendRequest(this);
  }

  /**
  * The stopSession method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.stopSession = function (tCtrl) {

    function stopSuccess(tCtrl, sessionID) {
      if (this.arrCtrlObj[tCtrl] == null) {
        //#18515:StopSession during closing window,this.arrCtrlObj[tCtrl] is null
        topic_Publish(tCtrl, "StopSession", "SUCCESS");
        return;
      }
      var objCtrl = this.arrCtrlObj[tCtrl];
      topic_Publish(tCtrl, "CollaborationEventMsg", "\nStop Session: CtrlTopic: " + tCtrl + ": SessionID: " + objCtrl.SessionID + " Success");
      this.endSession(tCtrl);
    }

    function stopError(tCtrl, errMsg) {
      if (this.arrCtrlObj[tCtrl] == null) return;
      var objCtrl = this.arrCtrlObj[tCtrl];
      topic_Publish(tCtrl, "ERRORSTATUS", "\nStop Session: CtrlTopic: " + tCtrl + ": ERROR: " + errMsg);
    }

    if (this.arrCtrlObj[tCtrl] == null) {
      this._LOG.error("CtrlAgent: StopSession: CtrlTopic: " + tCtrl + ": Not found");
      return;
    }
    var objCtrl = this.arrCtrlObj[tCtrl];
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nStop Session: CtrlTopic: " + tCtrl + ": SessionID: " + objCtrl.SessionID + " Begin");

    topic_Set(tCtrl, "CollaborationRecordState", "DISCONNECTING");
    topic_Set(tCtrl, "CollaborationPVRState", "DISCONNECTING");
    topic_Publish(tCtrl, "CollaborationState", "DISCONNECTING");
    topic_Publish(tCtrl, "CollaborationStatusMsg", "DISCONNECTING .");

    var ThisClass = this;
    var reqStopSession = new ReqSessionStartStopRequest("Stop", null,
                                                            { action: "FNCCALL", context: ThisClass,
                                                              fnc: function (respCode, respVal) {
                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                  stopSuccess.call(ThisClass, tCtrl, respVal.getValue());
                                                                }
                                                                else {
                                                                  this._LOG.error("CtrlAgent: StopSession: CtrlTopic: " + tCtrl + ": Stop Session for SessionID: " + objCtrl.SessionID + "request failed");
                                                                  stopError.call(ThisClass, tCtrl, respVal);
                                                                }
                                                              }
                                                            }
                                                          );
    reqStopSession.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["SessionID"] = objCtrl.SessionID;
    reqStopSession.reqParams = reqParams;
    reqStopSession.sendRequest(this);
  }
  /**
  * The switchSrc method.
  * @param tCtrl {String}
  * @param objReq {JASON Object}
  */
  SessionCtrlAgent_prototype.switchSrc = function (tCtrl, objReq) {
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nswitch source: CtrlTopic: " + tCtrl + ": SessionID: " + objReq.SessionID + " Begin");

    var ThisClass = this;
    //Set Connection Contents to the selected Window
    var reqSwitchSrc = new ReqSessionGetRequest("SwitchSrc",
                                                { action: "FNCCALL", context: this,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      var Response = respVal;
                                                      objReq.callBackSuccess.call(ThisClass, Response, objReq.mediaInfo, objReq.reqType);
                                                    }
                                                    else {
                                                      objReq.callBackErr.call(ThisClass, respVal, objReq.mediaInfo, objReq.reqType);
                                                    }
                                                  }
                                                }
                                                );
    reqSwitchSrc.ServiceName = "SessionService";
    var reqParamsSwitchSrc = new Object;
    reqParamsSwitchSrc["ObjectList_sessionId"] = objReq.SessionID;
    reqParamsSwitchSrc["ObjectList_connId"] = objReq.ConnId;
    reqParamsSwitchSrc["ObjectList_newSrcEntityId"] = objReq.EntityId;
    reqSwitchSrc.reqParams = reqParamsSwitchSrc;
    // send the request to set window contents
    reqSwitchSrc.sendRequest(this);
  }

  /**
  * The setWindowContent method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.setWindowContent = function (tCtrl, objReq) {
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nSet Window Content for Dest Port: CtrlTopic: " + tCtrl + ": SessionID: " + objReq.SessionID + " Begin");

    var ThisClass = this;
    //Set Connection Contents to the selected Window
    var reqSetPortWindowContent = new ReqSessionGetRequest("SetPortWindowContent",
                                                            { action: "FNCCALL", context: this,
                                                              fnc: function (respCode, respVal) {
                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                  var Response = respVal;
                                                                  //<SetPortWindowContent><PortWindowContent NID="" id="win2"><Exception><Code>4904</Code><Description>Device Not Ready</Description></Exception></PortWindowContent></SetPortWindowContent>                                                                    
                                                                }
                                                                else {
                                                                  this._LOG.error("SetPortWindowContent request failed for PortNID: " + objReq.DstportNID + "\n")
                                                                }
                                                              }
                                                            }
                                                           );
    reqSetPortWindowContent.ServiceName = "SessionService";
    var reqParamsWindowContent = new Object;
    reqParamsWindowContent["ObjectList_whereNID"] = objReq.DstPortNID;
    reqParamsWindowContent["ObjectList_windowId"] = objReq.WindowId;
    reqParamsWindowContent["ObjectList_sessionId"] = objReq.SessionId
    reqParamsWindowContent["ObjectList_connId"] = objReq.ConnId;
    reqSetPortWindowContent.reqParams = reqParamsWindowContent;
    // send the request to set window contents
    reqSetPortWindowContent.sendRequest(this);
  }

  /**
  * The view(session)/playBack(file,group files) method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.viewOrPlayBack = function (tCtrl, objReq) {
    topic_Publish(tCtrl, "CollaborationEventMsg", "\n" + objReq.reqType + " Session: CtrlTopic: " + tCtrl + ": SessionID: " + objReq.SessionId + " Begin");
    var ThisClass = this;
    var reqView = new ReqSessionGetRequest(objReq.reqType,
                                    { action: "FNCCALL", context: this,
                                      fnc: function (respCode, respVal) {
                                        if (respCode == "SUCCESS" && respVal != null) {
                                          objReq.callBackSuccess.call(ThisClass, respVal, objReq.mediaInfo, objReq.reqType);
                                        }
                                        else {
                                          objReq.callBackErr.call(ThisClass, respVal, objReq.mediaInfo, objReq.reqType);
                                        }
                                      }
                                    }
                                   );
    reqView.ServiceName = "SessionService";
    var reqParams = new Object;


    if (!jsx3.util.strEmpty(objReq.DstEntityId)) {
      reqParams["ObjectList_DstEntityId"] = objReq.DstEntityId;
    }
    if (!jsx3.util.strEmpty(objReq.DstPortNID)) {
      reqParams["ObjectList_dstEntityId"] = objReq.DstEntityId;
      reqParams["ObjectList_dstPortNID"] = objReq.DstPortNID;
      reqParams["ObjectList_dstPortTitle"] = objReq.DstportName;
    }

    reqParams["ObjectList_connName"] = objReq.ConnName;
    reqParams["ObjectList_sessionId"] = objReq.SessionId;
    reqParams["ObjectList_connId"] = objReq.ConnId;
    reqParams["ObjectList_windowId"] = objReq.WindowId;
    reqParams["ObjectList_canvasId"] = objReq.CanvasId;
    reqParams["ObjectList_restartSession"] = objReq.restartSession;
    //different for 'View' and 'PlayBack'
    if (objReq.reqType == "View") {
      if (!jsx3.util.strEmpty(objReq.SrcEntityId)) {
        reqParams["ObjectList_SrcEntityId"] = objReq.SrcEntityId;
      }
      if (!jsx3.util.strEmpty(objReq.SrcPortNID)) {
        reqParams["ObjectList_srcEntityId"] = objReq.SrcEntityId;
        reqParams["ObjectList_srcPortNID"] = objReq.SrcPortNID;
        reqParams["ObjectList_srcPortTitle"] = objReq.SrcPortName;
        reqParams["ObjectList_srcPVREnable"] = objReq.PVREnable;
        reqParams["ObjectList_srcPVRTrimLength"] = objReq.PVRTrimLength;
      }
    }
    else if (objReq.reqType == "PlayBackGrp" || objReq.reqType == "PlayBackSrc") {
      if (!jsx3.util.strEmpty(objReq.FileNID)) {
        reqParams["ObjectList_fileEntityId"] = objReq.SrcEntityId;
        reqParams["ObjectList_fileNID"] = objReq.FileNID;
      }
      if (!jsx3.util.strEmpty(objReq.SrcEntityId)) {
        reqParams["ObjectList_SrcEntityId"] = objReq.SrcEntityId;
      }
    }

    reqView.reqParams = reqParams;
    // send the request
    reqView.sendRequest(this);
  }
  /**
  * The endSession method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.endSession = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) return;
    if (topic_Get(tCtrl, "CollaborationState") == "NONE") return;
    var objCtrl = this.arrCtrlObj[tCtrl];
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nEnd Session: CtrlTopic: " + tCtrl + ": SessionID: " + objCtrl.SessionID);
    // publishing this state for slider to stop
    topic_Publish(tCtrl, "ControllerState", "Stop");
    // reset the objects and state vars - for good measure
    this.resetController(tCtrl);
    // reset SessionID
    topic_Publish(tCtrl, "SessionID", "NONE");
  }


  /**
  * -- PVR commands - pause / resume / catchup / skipback / skipfwd
  * sessionId already stash in arrCtrlObj[tCtrl] when createCollaboration success OR joinSuccess
  * it is used implicitly in clsRequest.ReqSessionPvrRequest
  */

  /**
  * The pause method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.pause = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    topic_Publish(tCtrl, "AddTrackActionState", "PAUSED");
    this.trackAction(tCtrl, "STREAM", "Pause");
  }

  /**
  * The resume method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.resume = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    topic_Publish(tCtrl, "AddTrackActionState", "Resume");
    //get FF track speed
    var ffSpeed = topic_Get(tCtrl, "PlayBySpeed");
    //convert '0.5x' to '0.5'
    ffSpeed = parseFloat(ffSpeed.replace(/x$/, ''));
    this.trackAction(tCtrl, "STREAM", "Play", ffSpeed);
  }

  /**
  * The resumeFromBookmark method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.resumeFromBookmark = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    topic_Publish(tCtrl, "AddTrackActionState", "Resume");

    //call ReqSessionResumeFromBookmarkRequest directly
    topic_Publish(tCtrl, "ERRORSTATUS", "NONE");
    // get ActionBookmarkNID which set at cmpBlkSessionBookmark.PlayFromBookmark(obj)
    var ActionBookmarkNID = topic_Get(tCtrl, "ActionBookmarkNID");
    var trackAction = "ResumeFromBookmark";
    this.arrCtrlObj[tCtrl].lastAction = trackAction;
    var ffSpeed = topic_Get(tCtrl, "PlayBySpeed");
    //convert '0.5x' to '0.5'
    ffSpeed = parseFloat(ffSpeed.replace('/x$/', ''));
    this._LOG.warn("\n\nSessionCtrlAgent_prototype:: CollaborationEventMsg:: ResumeFromBookmark :BookmarkNID: " + ActionBookmarkNID + "::SESSIONID:" + this.arrCtrlObj[tCtrl].SessionID + ":: tCtrl:" + tCtrl);
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nCollaboration : STREAM: ResumeFromBookmark :BookmarkNID: " + ActionBookmarkNID);
    var reqSessionResumeFromBookmark = new ReqSessionResumeFromBookmarkRequest(this.arrCtrlObj[tCtrl].SessionID, trackAction, ActionBookmarkNID,
                                                                  { action: "FNCCALL", context: this,
                                                                    fnc: function (respCode, respVal) {
                                                                      // decide success or failure based on respCode
                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                        this._LOG.info(trackAction + " successful.");
                                                                      }
                                                                      else {
                                                                        this._LOG.error(trackAction + " fail.");
                                                                      }
                                                                      return true;
                                                                    }
                                                                  });
    if (!jsx3.util.strEmpty(ffSpeed)) {
      reqSessionResumeFromBookmark.reqParams = new Object();
      reqSessionResumeFromBookmark.reqParams["timeScale"] = ffSpeed;
    }
    reqSessionResumeFromBookmark.sendRequest(this);
  }

  /**
  * The catchup method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.catchup = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    topic_Publish(tCtrl, "AddTrackActionState", "Catchup");
    this.trackAction(tCtrl, "STREAM", "Catchup");
  }

  /**
  * The skipback method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.skipback = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    topic_Publish(tCtrl, "AddTrackActionState", "SkipBack");
    //get FF track speed
    var ffSpeed = topic_Get(tCtrl, "PlayBySpeed");
    //convert '0.5x' to '0.5'
    ffSpeed = parseFloat(ffSpeed.replace('/x$/', ''));
    this.trackAction(tCtrl, "STREAM", "SkipBack", ffSpeed);
  }

  /**
  * The skipfwd method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.skipfwd = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    topic_Publish(tCtrl, "AddTrackActionState", "SkipFwd");
    //get FF track speed
    var ffSpeed = topic_Get(tCtrl, "PlayBySpeed");
    //convert '0.5x' to '0.5'
    ffSpeed = parseFloat(ffSpeed.replace('/x$/', ''));
    this.trackAction(tCtrl, "STREAM", "SkipFwd", ffSpeed);
  }

  /**
  * The pauseRecording method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.pauseRecording = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    //topic_Publish(tCtrl, "CollaborationRecordState", "PauseRecord");
    this.trackAction(tCtrl, "RECORD", "PauseRecord");
  }

  /**
  * The resumeRecording method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.resumeRecording = function (tCtrl) {
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("Controller ID: " + tCtrl + ": Not found");
    if (jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].SessionID)) throw new jsx3.Exception("Session ID not found");
    //topic_Publish(tCtrl, "CollaborationRecordState", "CONNECTED");
    this.trackAction(tCtrl, "RECORD", "ResumeRecord");
  }

  /**
  * The trackAction method,internal method called by all pvr actions for main stream and recordings.
  * @param tCtrl {String}
  * @param track {String}
  * @param trackAction {String}
  */
  SessionCtrlAgent_prototype.trackAction = function (tCtrl, track, trackAction, timeScale) {
    // callback functions
    // error
    function trackActionError(tCtrl) {
      // clear reqseq
      if (this.arrCtrlObj[tCtrl] == null) return;
      this.arrCtrlObj[tCtrl].arrReqSeq.reqTrackActionCollab.endRequestSeq();
      this.arrCtrlObj[tCtrl].arrReqSeq.reqTrackActionCollab = null;
      if (track == "STREAM") {
        topic_Publish(tCtrl, "AddTrackActionState", trackAction + "_FAIL");
      }
    }
    // success
    function trackActionSuccess(tCtrl) {
      // clear reqseq
      if (this.arrCtrlObj[tCtrl] == null) return;
      this.arrCtrlObj[tCtrl].arrReqSeq.reqTrackActionCollab.endRequestSeq();
      this.arrCtrlObj[tCtrl].arrReqSeq.reqTrackActionCollab = null;
      // fire event for controller
      if (track == "STREAM") {
        topic_Publish(tCtrl, "AddTrackActionState", trackAction + "_SUCCESS");
        // hack - fire controller state event from here - should come from roomevent listner
        // once we get action data back from middleware - we will fire it from here 
        // and turn off the room event msg - (which is one per connection)
        //topic_Publish(tCtrl, "ControllerState", trackAction);
      }
    }
    // set the lastAction
    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("CtrlAgent: TrackAction: CtrlTopic: " + tCtrl + ": Not found");
    var objCtrl = this.arrCtrlObj[tCtrl];
    objCtrl.lastAction = trackAction;
    // send track action request, sessionId already embedded in objCtrl when createCollaboration/join success
    topic_Publish(tCtrl, "ERRORSTATUS", "NONE");
    // get action data
    var ActionData = topic_Get(tCtrl, "ActionData");
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nCollaboration " + track + " Track: " + trackAction + " " + ActionData + " ...");
    ActionData = ActionData == "NONE" ? null : ActionData;
    var reqSessionPVRAction = new ReqSessionPvrRequest(objCtrl.SessionID, objCtrl.lastAction, ActionData,
                                    { action: "FNCCALL", context: this,
                                      fnc: function (respCode, respVal) {
                                        // decide success or failure based on respCode
                                        if (respCode == "SUCCESS" && respVal != null) {
                                          this._LOG.info(objCtrl.lastAction + " successful.");
                                        }
                                        else {
                                          this._LOG.info(objCtrl.lastAction + " fail.");
                                          topic_Publish(tCtrl, "ControllerMask", "NONE");
                                        }
                                        return true;
                                      }
                                    });
    if (!jsx3.util.strEmpty(timeScale)) {
      reqSessionPVRAction.reqParams = new Object();
      reqSessionPVRAction.reqParams["timeScale"] = timeScale;
    }
    reqSessionPVRAction.sendRequest(this);
  }

  /**
  * The DeleteConn method,delete connection.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.DeleteConn = function (tCtrl, sessionID, connectionID) {
    function deleteSuccess(tCtrl, sessionID, connctionID) {
      if (this.arrCtrlObj[tCtrl] == null) return;
      var objCtrl = this.arrCtrlObj[tCtrl];
      topic_Publish(tCtrl, "CollaborationEventMsg", "\nDelete a Connection: CtrlTopic: " + tCtrl + ": SessionID: " + sessionID + ": ConnectionID: " + connectionID + " Success");
    }

    function deleteError(tCtrl, sessionID) {
      if (this.arrCtrlObj[tCtrl] == null) return;
      var objCtrl = this.arrCtrlObj[tCtrl];
      topic_Publish(tCtrl, "ERRORSTATUS", "\nDelete a Connection: CtrlTopic: " + tCtrl + ": SessionID: " + sessionID + ": ConnectionID: " + connectionID + " ERROR");
    }

    if (this.arrCtrlObj[tCtrl] == null) throw new jsx3.Exception("CtrlAgent: DeleteConnection: CtrlTopic: " + tCtrl + ": Not found");
    var objCtrl = this.arrCtrlObj[tCtrl];
    topic_Publish(tCtrl, "CollaborationEventMsg", "\nDelete a Connection: CtrlTopic: " + tCtrl + ": SessionID: " + sessionID + ": ConnectionID: " + connectionID + " Begin");

    var ThisClass = this;
    var reqSessionDeleteConn = new ReqSessionDeleteConnRequest(sessionID, connectionID,
                                      { action: "FNCCALL", context: this,
                                        fnc: function (respCode, respVal) {
                                          var tt = "ttt";
                                          if (respCode == "SUCCESS" && respVal != null) {
                                            this._LOG.info("clsCtrlAgent::DeleteConn:request(" + sessionID + "," + connectionID + ") success");
                                          }
                                          else {
                                            this._LOG.warn("clsCtrlAgent::DeleteConn:fail(" + sessionID + "," + connectionID + ").");
                                            this._LOG.error(respVal);
                                          }
                                        }
                                      });
    reqSessionDeleteConn.reqParams = new Object();
    reqSessionDeleteConn.reqParams["Where_xpath"] = "";
    reqSessionDeleteConn.sendRequest(this);
  }


  /**
  * The getSession method.
  * @param tCtrl {String}
  */
  SessionCtrlAgent_prototype.getSession = function (tCtrl, sessionId) {
    var docSession = null;
    if (sessionId == this.arrCtrlObj[tCtrl].SessionID) {
      if (!jsx3.util.strEmpty(this.arrCtrlObj[tCtrl].nodeSession)) {
        docSession = new com.ipvs.api.Session.Document(this.arrCtrlObj[tCtrl].nodeSession);
      }
    }
    return docSession;
  }


  /**
  * Envent listener sections
  */
  SessionCtrlAgent_prototype.eventWrap = function (eventApp) {
    eventApp.subscribe("PRESENCE", null, this,
                      function (EventVar, EventVal) {
                        var presence = EventVal;
                        // check if this presence is for an existing room
                        var presenceMediaRoomNID = null;
                        for (var tCtrl in this.arrCtrlObj) {
                          if (this.arrCtrlObj[tCtrl] == null) continue;
                          var objCtrl = this.arrCtrlObj[tCtrl];
                          var MediaRoomNID = objCtrl.MediaRoomNID;
                          if (MediaRoomNID != null) {
                            // if found 
                            if (presence.RoomNID == MediaRoomNID) {
                              // mark room found
                              presenceMediaRoomNID = MediaRoomNID;
                              // make sure it is a entity JID presence
                              if (presence.JID != null && presence.JID != "") {
                                // look for a matching entity - skip if no collab node
                                var nodeCollab = objCtrl.nodeCollab;
                                if (nodeCollab) {
                                  var nodeEntity = nodeCollab.selectSingleNode(".//Entity[agentJID='" + presence.JID + "']");
                                  // if not a known entity - create a participant entity node for it
                                  if (nodeEntity == null) {
                                    nodeEntity = getNewEntityNode("Participant", presence.JID);
                                    var nodeEntityList = nodeCollab.selectSingleNode(".//EntityList");
                                    nodeEntityList.appendChild(nodeEntity);
                                  }
                                  // tell the controller of this event
                                  this.notifyEntityPresence(tCtrl, nodeEntity, presence);
                                  // TBD - poke START if needed
                                  // TBD - potentially do auto shutdown / repair logic if state if false etc..
                                }
                              }
                              // otherwise this is a Room presence
                              else {
                                topic_Publish(tCtrl, "CollaborationEventMsg", "\nRoom Presence: RoomNID: " + presence.RoomNID + " : State: " + presence.State);
                                if (presence.State == false) {
                                  // room has gone away - end the collab
                                  this.endSession(tCtrl);
                                }
                              }
                            }
                          }
                        }
                        // presence for room not in our list (yet)
                        // if this is an entity presence
                        if (presence.JID != null && presence.JID != "") {
                          // just stash it in an arr for use later
                          if (presenceMediaRoomNID == null) {
                            if (!this.arrPresence[presence.RoomNID]) {
                              this.arrPresence[presence.RoomNID] = new Object;
                            }
                            var presenceUserName = presence.JID;
                            this.arrPresence[presence.RoomNID][presenceUserName] = presence;
                          }
                        }
                        // otherwise this is a Room presence
                        else {
                          if (presence.State == false) {
                            // room has gone away - cleanup
                            delete this.arrPresence[presence.RoomNID];
                          }
                        }
                      });

    eventApp.subscribe("CONNSTATEEVENT", null, this,
                        function (EventVar, EventVal) {
                          var connstateevent = EventVal;
                          this.sessionStateUpdate(connstateevent);
                        });

    eventApp.subscribe("TIMELINEEVENT", null, this,
                        function (EventVar, EventVal) {
                          var timelineevent = EventVal;
                          this.timeLineEvent(timelineevent);
                        });
  }

});

