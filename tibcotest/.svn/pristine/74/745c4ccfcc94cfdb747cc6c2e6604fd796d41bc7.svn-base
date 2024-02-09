/**
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license. 
 *
 * This  acts as the media stream destination agent
 * UI blocks that will instantiate and manage the decoder port instances (ie Video Player Objects)
 * will register a topic to a mediaroomNID
 * all dest agent events for that mediaroom will be parsed by this agent and sent
 * as simplified messages (ideally, similar to SS API) on the registered topic
 */

jsx3.Class.defineClass('com.ipvs.agent.MediaStreamDstAgent', null, null, function (MediaStreamDstAgent, MediaStreamDstAgent_prototype) {


  // create Logger for this class
  MediaStreamDstAgent_prototype._LOG = jsx3.util.Logger.getLogger(MediaStreamDstAgent.jsxclass.getName());


  /**
  * Instance varibales
  */
  MediaStreamDstAgent_prototype.arrDstObj = null;
  MediaStreamDstAgent_prototype.arrMediaRoom = null;
  MediaStreamDstAgent_prototype.arrResourceInstanceList = null;
  MediaStreamDstAgent_prototype.getMediaRoomMutex = 0;
  MediaStreamDstAgent_prototype.SS = null;

  /**
  * The instance initializer.
  * @param eventApp {Object}
  */
  MediaStreamDstAgent_prototype.init = function (eventApp) {
    this._LOG.debug("MediaStreamDstAgent.init");
    this.arrDstObj = new Object;
    this.arrMediaRoom = new Object;
    this.arrResourceInstanceList = new Object;
    this.SS = { "input": {},
      "output": {}
    };
    this.eventWrap(eventApp);
  };

  /**
  * The destroy method.
  */
  MediaStreamDstAgent_prototype.destroy = function () {
    this.arrDstObj = null;
    this.arrMediaRoom = null;
    this.arrResourceInstanceList = null;
    this.getMediaRoomMutex = 0;
    this.SS = null;
  }


  /**
  * Parse room presence and notify rooms as needed
  * @param presence {JASON Object}
  */
  MediaStreamDstAgent_prototype.roomPresence = function (presence) {
    // we are only interested in rooms going away
    if (this.arrMediaRoom[presence.RoomNID] && presence.state == false) {
      // room has gone away - cleanup array
      delete this.arrMediaRoom[presence.RoomNID];
      // also check if this presence is for a registered room
      for (var tDst in this.arrDstObj) {
        if (this.arrDstObj[tDst] == null) continue;
        var objDst = this.arrDstObj[tDst];
        if (presence.RoomNID == objDst.MediaRoomNID) {
          // room has gone away - delete all stream instances
          for (var streamNID in objDst.objMediaRoom) {
            this._LOG.info(" roomPresence:call notifyStreamDst");
            this.notifyStreamDst(tDst, this.arrDstObj[tDst], streamNID, null, "Delete");
          }
          break;
        }
      }
    }
  }


  /**
  * Parse room events for stream states and publish on assoc tDst
  * @param roomevent {JASON Object}
  */
  MediaStreamDstAgent_prototype.roomEvent = function (roomevent) {
    // check if this roomevent is for us - if not return
    var AgentJID = topic_Get(tApp, "LoginUserJID");
    //if a user start both client and adminUI in one machine,then the user's roomevent.ToJID is always covered by
    //adminUI's 'superuser-default'; i.e. roomevent.ToJID as superuser-default@localhost/sunpc3
    //but the AgentJID as  sunuser3@localhost/sunpc3,this fix is noly proper for one client case.
    var roomeventToJIDResourceName = roomevent.ToJID ? roomevent.ToJID.replace(/.*\//, "") : roomevent.ToJID;
    var agentJIDResourceName = AgentJID.replace(/.*\//, "");
    if (roomeventToJIDResourceName != agentJIDResourceName) {
      return;
    }
    //---[1]roomevent process, to see if it is of interest for dest/player
    if (roomevent.Action != "SourceEvent" &&
        roomevent.Action != "DestEvent" &&
        roomevent.Action != "NotifyResponse" &&
    // stx has some processing related to this - TBD
    //        roomevent.Action != "StreamStatusEvent" && 
        roomevent.StatusCode == 200) {
      // if yes - store roomNID,streamNID and their corresponding StreamURL & Action info.
      if (this.arrMediaRoom[roomevent.RoomNID] == null) {
        this.arrMediaRoom[roomevent.RoomNID] = new Object;
      }
      if (this.arrMediaRoom[roomevent.RoomNID][roomevent.StreamNID] == null) {
        this.arrMediaRoom[roomevent.RoomNID][roomevent.StreamNID] = new Object;
      }
      this.arrMediaRoom[roomevent.RoomNID][roomevent.StreamNID].Action = roomevent.Action;
      this.arrMediaRoom[roomevent.RoomNID][roomevent.StreamNID].StreamURL = roomevent.StreamURL;
      // try to match the room nid of the event with a registered player
      var found = false;
      for (var tDst in this.arrDstObj) {
        if (this.arrDstObj[tDst] == null) continue;
        // if match found
        if (roomevent.RoomNID == this.arrDstObj[tDst].MediaRoomNID || this.arrDstObj[tDst].SessionID == "Session_All") {
          // notify destination
          connInfo = this.getStreamConnInfo(tDst, this.arrDstObj[tDst], roomevent.StreamNID, true);
          if (connInfo) {
            this._LOG.info(" roomEvent:call notifyStreamDst");
            this.notifyStreamDst(tDst, this.arrDstObj[tDst], roomevent.StreamNID, roomevent.StreamURL, roomevent.Action, connInfo);
          }
          found = true;
          break;
        }
      }
      //      //---[2] autojoin process(roomevent.Action="Start"), to see if auto join prefs is true - if not return ;
      //      if (!found) {
      //        if( topic_Get(tApp, "DefaultAutoJoin") == "true") {
      //          this.autoJoinSession(roomevent);
      //        }
      ////        else if (topic_Get(tApp, "ClientLaunchMode") == "UnManned") {
      ////          //'UnManned' mode get windowId by connectionId and start player in the window
      ////          //          this.autoJoinConnection(roomevent);
      ////          var connId = this.getConnIdByRoomNID(roomevent);
      ////          
      ////            
      ////        }
      //      }
    }
  }

  /**
  * SS Event Handler
  * simulate what the Streaming server does and translate to Player commands
  * @param objSSCmd {Object}
  */
  MediaStreamDstAgent_prototype._handleSSEvent = function (objSSCmd) {
    // process cmd based on 'action'
    switch (objSSCmd.action) {
      case "add":
        // just stash the cmd by 'operation.id' in the SS object - will be used later when start comes
        this.SS[objSSCmd.operation][objSSCmd.id] = objSSCmd;
        break;
      case "delete":
        // find matching object from 'add+start' - may not exist - extra/precaution deletes are normal
        var objSS = this.SS[objSSCmd.operation][objSSCmd.id];
        if (objSS) {
          // check if this has already been 'start'ed - ie: has a 'queueid'
          if (objSS.queueid) {
            // if started - check if an object of the opposite 'operation' already exists with matching 'queueid'
            var xobjSS = this._getSSEntity(objSS);
            if (xobjSS) {
              //if found - put the 'Stop' Player cmd together
              var objPlayerCmd = this._getPlayerCmd("Stop", objSS, xobjSS);
              // and send to the proper Display Agent
              this._notifyDisplay(objPlayerCmd);
            }
            // done with check - in either case delete this object
            delete this.SS[objSSCmd.operation][objSSCmd.id];
          }
          else {
            // if not started - just delete the obj
            delete this.SS[objSSCmd.operation][objSSCmd.id];
          }
        }
        break;
      case "start":
        // find matching object from 'add'
        var objSS = this.SS[objSSCmd.operation][objSSCmd.id];
        if (objSS) {
          // assign the 'queueid' and 'action' to the matching object - just copy all props to be sure
          for (var i in objSSCmd) {
            objSS[i] = objSSCmd[i];
          }
          // check if an object of the opposite 'operation' already exists with matching 'queueid'
          var xobjSS = this._getSSEntity(objSS);
          if (xobjSS) {
            //if found - put the "Start" Player cmd together
            var objPlayerCmd = this._getPlayerCmd("Start", objSS, xobjSS);
            this._LOG.info(" _handleSSEvent: start: action: " + objPlayerCmd.action + " winId: " + objPlayerCmd.windowId);
            // and send to the proper Display Agent
            this._notifyDisplay(objPlayerCmd);
          }
        }
        break;
      default:
        break;
    }
  }

  /**
  * check if an object of the opposite 'operation' already exists with matching 'queueid'
  * @param objSS {SSEntity Object}
  * @return xobjSS {SSEntity Object}
  */
  MediaStreamDstAgent_prototype._getSSEntity = function (objSS) {
    // get the opposite 'operation'
    var xop = objSS.operation == 'input' ? 'output' : 'input';
    // look in SSEntity arr
    for (var xobjSSid in this.SS[xop]) {
      var xobjSS = this.SS[xop][xobjSSid];
      // check for matching 'queueid'
      if (xobjSS.queueid == objSS.queueid) {
        // if found - return it
        return xobjSS;
      }
    }
    // not found - return null
    return null;
  }

  /**
  * combine the info from the SS entities and create a player Cmd obj
  * @param action {String}
  * @param objSS {SSEntity Object}
  * @param xobjSS {SSEntity Object}
  * @return objPlayerCmd {PlayerCmd Object}
  */
  MediaStreamDstAgent_prototype._getPlayerCmd = function (action, objSS, xobjSS) {
    // put the SS entity info together
    var objPlayerCmd = new Object;
    for (var i in objSS) {
      objPlayerCmd[i] = objSS[i];
    }
    for (var i in xobjSS) {
      objPlayerCmd[i] = xobjSS[i];
    }
    // set the action
    objPlayerCmd.action = action;

    //get canvasId and winId from 'ioport' - if not already specified
    jsx3.util.strEmpty(objPlayerCmd.canvasId) && (objPlayerCmd.canvasId = objPlayerCmd.ioport.split('.')[0]);
    jsx3.util.strEmpty(objPlayerCmd.windowId) && (objPlayerCmd.windowId = objPlayerCmd.ioport.split('.')[1]);

    return objPlayerCmd;
  }

  /**
  * send PlayerCmd to the right display agent/player
  * @param objPlayerCmd {PlayerCmd Object}
  */
  MediaStreamDstAgent_prototype._notifyDisplay = function (objPlayerCmd) {
    if (topic_Get(tApp, 'ClientLaunchMode') == "MainMenu") {
      // check 'AutoJoin' mode - then launch a player by sessionId, exclude unmanned
      if (topic_Get(tApp, "DefaultAutoJoin") == "true") {
        var sessionId = objPlayerCmd.canvasId;
        this._LOG.info(" _notifyDisplay: AutoJoin: sessionId: " + sessionId);
        if (sessionId) {
          var mediaInfo = new Object();
          mediaInfo.Type = "AutoJoin";
          //set window lable to empty, replace it with a session name while the session is started 
          mediaInfo.Name = "";
          mediaInfo.sessionId = sessionId;
          this._LOG.info("MediaStreamDstAgent._notifyDisplay: agtDisplay.launchPlayerBySessionId: Session Id: " + mediaInfo.sessionId + ":tMediaPlayer:null");
          agtDisplay.launchPlayerBySessionId(mediaInfo, true);
        }
      }
      //Exclude SSCmd process for IPVSClient  
      return;
    }

    //call agtDisplay.doAction only for UnManned client
    this._LOG.info(" _notifyDisplay: action: " + objPlayerCmd.action + " winId: " + objPlayerCmd.windowId);
    agtDisplay.doAction(objPlayerCmd);

  }

  //  /**
  //  * Get connectionId from mediaroom (RoomNID) 
  //  * @param roomNID {String}
  //  * @return {String}
  //  */
  //  MediaStreamDstAgent_prototype.launchPlayerByConnId = function(connId, roomevent) {
  //    var playerInstance = null;
  //    //get media player by winId
  //    playerInstance = agtDisplay.getMediaPlayerByConnId(connId);
  //    // if found, load player and start it
  //    if( playerInstance ) {
  //      playerInstance.loadPlayer().when(function() {
  //                    playerInstance.start(roomevent.StreamURL);
  //                  });
  //    }
  //  }

  //  /**
  //  * Get connectionId from mediaroom (RoomNID) 
  //  * @param roomNID {String}
  //  * @return {String}
  //  */
  //  MediaStreamDstAgent_prototype.getConnIdByRoomNID = function(roomevent) {
  //    var roomNID = roomevent.RoomNID;
  //    var streamNID = roomevent.StreamNID;
  //    var connectionId = this.checkConnIdByNID(roomNID, streamNID);
  //    if (connectionId) return connectionId;
  //    var reqGetMediaRoom = new ReqGetMediaRoom(roomNID,
  //                                    { action: "FNCCALL", context: this,
  //                                      fnc: function(respCode, respVal) {
  //                                        var connid = null;
  //                                        // then check on respCode and proceed
  //                                        if (respCode == "SUCCESS" && respVal != null) {
  //                                          var roomNode = respVal.selectSingleNode("//MediaRoom");
  //                                          if (roomNode != null) {
  //                                            var connidNode = roomNode.selectSingleNode(".//Stream[@NID='" + streamNID + "']/@id");
  //                                            connid = connidNode ? connidNode.getValue() : null;
  //                                            if (connid) {
  //                                              this.launchPlayerByConnId(connid, roomevent);
  //                                            }
  //                                            else {
  //                                              this._LOG.error("MediaStreamDstAgent:getConnIdByRoomNID:No connection mapping streamNID(" + streamNID + ") in room(" + roomNID + ").");
  //                                            }
  //                                          }
  //                                        }
  //                                        else {
  //                                          //Error code to the connection
  //                                          this._LOG.error("MediaStreamDstAgent:getConnIdByRoomNID:Room(" + roomNID + ") not found.");
  //                                        }
  //                                      }
  //                                    });
  //    reqGetMediaRoom.sendRequest(this);
  //  } 

  //  /**
  //  * Get connectionId from mediaroom (RoomNID) 
  //  * @param roomNID {String}
  //  * @return {String}
  //  */
  //  MediaStreamDstAgent_prototype.checkConnIdByNID = function(roomNID, streamNID) {
  //    var nodeMediaRoom = topic_GetNIDNode(tCache, roomNID);
  //    var connectionIdNode = (!nodeMediaRoom) ? null : nodeMediaRoom.selectSingleNode(".//Stream[@NID='" + streamNID + "']/@id");
  //    var connectionId = (!connectionIdNode) ? null : connectionIdNode.getValue();
  //    return connectionId;
  //  }
  //
  //  /**
  //  * Process autoJoinConnection 
  //  * @param roomevent {JASON Object}
  //  */
  //  MediaStreamDstAgent_prototype.autoJoinConnection = function(roomevent) {
  //    //task 25861
  //    if (roomevent) {
  //      mediaInfo = new Object();
  //      mediaInfo.Type = "AutoJoin";
  //      mediaInfo.Name = "Auto Join Connection";
  //      mediaInfo.RoomNID = roomevent.RoomNID;
  //      mediaInfo.StreamNID = roomevent.StreamNID;
  //      this._LOG.debug("MediaStreamDstAgent:autoJoinConnection: streamNID: " + mediaInfo.StreamNID);
  //      agtDisplay.launchPlayerByConnId(mediaInfo);
  //    }
  //    else {
  //      this._LOG.error("MediaStreamDstAgent:autoJoinConnection: error: roomevent is null.");
  //    }
  //  }
  //  /**
  //  * Join connection
  //  * @param tDst {String}
  //  * @param connectionID {String}
  //  */
  //  MediaStreamDstAgent_prototype.joinConnection = function(tDst, mediaInfo) {
  //    var MediaRoomNID = mediaInfo.RoomNID;
  //    var streamNID = mediaInfo.StreamNID;
  //    // success
  //    function joinSuccess(tDst) {
  //      this.arrDstObj[tDst] = new Object;
  //      // consolidate status and config
  //      var objDst = this.arrDstObj[tDst];
  //      // - stash MediaRoomNID 
  //      objDst.objMediaRoom = new Object;
  //      objDst.MediaRoomNID = MediaRoomNID;
  //      // get the mediaroom
  //      var nodeMediaRoom = topic_GetNIDNode(tCache, MediaRoomNID);
  //      objDst.nodeMediaRoom = nodeMediaRoom.cloneNode(true);
  //      // get session node if present
  //      var nodeSession = nodeMediaRoom.selectSingleNode(".//Session");
  //      objDst.nodeSession = nodeSession ? nodeSession.cloneNode(true) : null;
  //      //atash sessionId to ojbDst
  //      objDst.SessionID = nodeSession.getRootNode().getAttribute("id"); ;
  //      //store current session ownerUserJID
  //      var ownerUserJIDNode = null;
  //      if (objDst.nodeSession != null) {
  //        ownerUserJIDNode = nodeSession.selectSingleNode(".//Properties/@ownerUserJID");
  //      }
  //      var ownerUserJID = ownerUserJIDNode == null ? null : ownerUserJIDNode.getValue();
  //      topic_Set(tDst, "SessionOwnerUserJID", ownerUserJID);
  //      // update any preexisting stream info for this mediaroom
  //      if (this.arrMediaRoom[objDst.MediaRoomNID] != null) {
  //        var objMediaRoom = this.arrMediaRoom[objDst.MediaRoomNID];
  //        // for each stream nid
  ////        for (var streamNID in objMediaRoom) {
  //          // update to current state
  //          this.notifyStreamDst(tDst, this.arrDstObj[tDst], streamNID, objMediaRoom[streamNID].StreamURL, objMediaRoom[streamNID].Action);
  ////        }
  //      }
  //      // set the lastAction
  //      objDst.lastAction = "JOIN";
  //    }

  //    log.info("MediaStreamDstAgent: joinConnection: PlayerTopic: " + tDst + ": MediaRoomNID: " + MediaRoomNID + ": streamNID:" + streamNID);
  //    var displayConfigNode = topic_GetNode(tDst, "//DisplayConfig");
  //    topic_Publish(tDst, "DisplaySessionCanvas", displayConfigNode);
  //    // reset the objects and state vars - for good measure
  //    this.resetStreamDst(tDst);
  //    this.getMediaRoomList(tDst, joinSuccess);
  //  }

  // /**
  //  * Get sessionId from mediaroom (RoomNID) 
  //  * @param roomNID {String}
  //  */
  //  MediaStreamDstAgent_prototype.getSessionIdByRoomNID = function(roomNID) {
  //    var nodeMediaRoom = topic_GetNIDNode(tCache, roomNID);
  //    var sessionId = nodeMediaRoom ? nodeMediaRoom.getAttribute("id") : null;
  //    return sessionId;
  //  }
  //  /**
  //  * Process autoJoinSession 
  //  * @param roomevent {JASON Object}
  //  */
  //  MediaStreamDstAgent_prototype.autoJoinSession = function(roomevent) {
  //    // if auto join - try to start player for this room if needed
  //    var RoomNID = roomevent.RoomNID;
  //    // first get session id from mediaroom (in cache)
  //    var sessionId = this.getSessionIdByRoomNID(RoomNID);
  //    // if found - check and launch player if needed
  //    if (sessionId) {
  //      mediaInfo = new Object();
  //      mediaInfo.Type = "AutoJoin";
  //      mediaInfo.Name = "Auto Join";
  //      mediaInfo.RoomNID = RoomNID;
  //      mediaInfo.sessionId = sessionId;
  //      this._LOG.debug("MediaStreamDstAgent:autoJoin: agtDisplay.launchPlayerBySessionId: Session Id: " + mediaInfo.sessionId);
  //      agtDisplay.launchPlayerBySessionId(mediaInfo, true);
  //    }
  //    // else send request to get mediaroom first- then - check and launch player if needed
  //    else {
  //      // first check if request already in progress - if so ignore
  //      if (roomEventArr[RoomNID]) return;
  //      // otherwise make entry in array - to prevent repeat call
  //      roomEventArr[RoomNID] = true;
  //      // send the request
  //      var ThisClass = this;
  //      var reqGetMediaRoomList = new ReqGetObjectList("MediaRoom",
  //                                                    { action: "FNCCALL", context: ThisClass,
  //                                                      fnc: function(respCode, respVal) {
  //                                                        if (respCode == "SUCCESS" && respVal != null) {
  //                                                          var mediaroomNodeIter = respVal.selectNodeIterator(".//ObjectNID");
  //                                                          while (mediaroomNodeIter.hasNext()) {
  //                                                            if (mediaroomNodeIter.next().getValue() == RoomNID) {
  //                                                              var sessionId = this.getSessionIdByRoomNID(RoomNID);
  //                                                              if (sessionId) {
  //                                                                mediaInfo = new Object();
  //                                                                mediaInfo.Type = "AutoJoin";
  //                                                                mediaInfo.Name = "Auto Join";
  //                                                                mediaInfo.RoomNID = RoomNID;
  //                                                                mediaInfo.sessionId = sessionId;
  //                                                                log.info("cmpBlkUI: AutoJoin: agtDisplay.launchPlayerBySessionId: SessionId: " + mediaInfo.sessionId + " :RoomNID:" + RoomNID);
  //                                                                agtDisplay.launchPlayerBySessionId(mediaInfo, true);
  //                                                                // cmpBlkSwitchController_Create(tApp,mediaInfo);
  //                                                              }
  //                                                              else {
  //                                                                log.error("cmpBlkUI: AutoJoin: Session Id not found for Room:" + mediaInfo.RoomNID);
  //                                                              }
  //                                                              //delete the array entry
  //                                                              delete roomEventArr[RoomNID];
  //                                                              break;
  //                                                            }
  //                                                            else {
  //                                                              log.error("cmpBlkUI: AutoJoin: Media room not found for RoomNID:" + RoomNID);
  //                                                            }
  //                                                          }
  //                                                        }
  //                                                        else {
  //                                                          log.error("cmpBlkUI: AutoJoin: Get MediaRoom list request failed");
  //                                                        }
  //                                                      }
  //                                                    }
  //                                                  );
  //      reqGetMediaRoomList.ServiceName = "mediaroomservice";
  //      reqGetMediaRoomList.sendRequest(this);
  //    }
  //  }

  //  /**
  //  * Get windows ID from canvas configuration
  //  * @param tDst {String}
  //  * @param sessionId {String}
  //  * @param connId {String}
  //  */
  //  MediaStreamDstAgent_prototype.getCanvasWinId = function(tDst, sessionId, connId) {
  //    if (sessionId == "Session_All") {
  //      var displayConfigNode = topic_GetNode(tDst, "//DisplayConfig");
  //      var winNode = displayConfigNode.selectSingleNode("//Window/ConnectionContent[@connId='" + connId + "']");
  //      if (winNode == null) {
  //        //if there is no window attached to connId then attach the first available window to connId and update Window.
  //        winNode = displayConfigNode.selectSingleNode("//Window/ConnectionContent[@connId='']");
  //        var connContentNode = winNode.selectSingleNode("//ConnectionContent");
  //        connContentNode.setAttribute("connId", connId);
  //      }
  //      var winId = winNode.getAttribute("id");
  //      return winId;
  //    }
  //    else {
  //      return sessionId;
  //    }
  //  }


  /**
  * Notify meida destination stream
  * @param tDst {String}
  * @param objDst {JASON Object}
  * @param streamNID {String}
  * @param streamURL {String}
  * @param streamAction {String}
  */
  MediaStreamDstAgent_prototype.notifyStreamDst = function (tDst, objDst, streamNID, streamURL, streamAction, connInfo) {
    // make sure we have at least some basic connInfo
    if (!connInfo) {
      connInfo = new Object;
      connInfo.SessionID = objDst.SessionID;
    }
    // find this stream in the mediaroom object
    var objStream = objDst.objMediaRoom[streamNID];
    // if still not found
    if (objStream == null || objStream.instanceId == null) {
      // create a new object
      objStream = new Object;
      // assign the player instance id - by default assume new player instance
      var instanceId = jsx3.xml.CDF.getKey();
      // - then look for an existing instance id by connid
      for (var lcStreamNID in objDst.objMediaRoom) {
        var lcObjStream = objDst.objMediaRoom[lcStreamNID];
        if (lcObjStream.connId == connInfo.connId) {
          // if found get the player instance Id from it
          instanceId = lcObjStream.instanceId;
          // (optional) delete the old stream object - should not be needed anymore
          delete objDst.objMediaRoom[lcStreamNID];
          break;
        }
      }
      objStream.instanceId = instanceId;
      // assign the connId
      objStream.connId = connInfo.connId;
      // attach this stream obj to room obj
      objDst.objMediaRoom[streamNID] = objStream;
      // - send an add command
      var objSSCmd = new Object;
      objSSCmd.instanceId = objStream.instanceId;
      objSSCmd.streamAction = "Add";
      objSSCmd.streamURL = streamURL;
      objSSCmd.connId = connInfo.connId;
      objSSCmd.sessionId = connInfo.SessionID;
      //        objSSCmd.winId = this.getCanvasWinId(tDst, objSSCmd.sessionId, objSSCmd.connId);
      objSSCmd.streamNID = streamNID;
      objSSCmd.srcEntityType = connInfo.connSrcEntityType;
      objSSCmd.connSrcEntityName = connInfo.connSrcEntityName;
      //stash srcEntityID for single player mode
      objSSCmd.connSrcEntityID = connInfo.connSrcEntityID;
      objSSCmd.ffTrackState = connInfo.ffTrackState;
      objSSCmd.sessionTimescale = connInfo.sessionTimescale;
      this._LOG.info(" notifyStreamDst:Add: New Stream: SSCmd: SSAction: " + objSSCmd.streamAction +
                                  ": InstanceId: " + objSSCmd.instanceId + ": connId: " + objSSCmd.connId +
                                  ": StreamURL: " + objSSCmd.streamURL +
                                  ": PlayerTopic: " + tDst + ": RoomNID: " + objDst.MediaRoomNID + ": StreamNID: " + streamNID);
      //if a stream go through 'Add' and 'Start' will not continue do other actions except it is a 'Pause'(Bug 21773)
      objStream.newAddedStream = true;
      topic_Publish(tDst, "MediaStreamDstState", objSSCmd);
    }

    // set the stream URL for it
    if (streamURL != null && streamURL != "") {
      // check if the stream url is changing
      if (objStream != null && streamURL != objStream.streamURL) {
        // if there is an existing url
        if (objStream.streamURL != null && objStream.streamURL != "") {
          // if so - it is changing - send stop first
          var objSSCmd = new Object;
          objSSCmd.instanceId = objStream.instanceId;
          objSSCmd.streamAction = "Stop";
          objSSCmd.streamURL = null;
          objSSCmd.connId = connInfo.connId;
          objSSCmd.sessionId = connInfo.SessionID;
          objSSCmd.streamNID = streamNID;
          objSSCmd.srcEntityType = connInfo.connSrcEntityType;
          objSSCmd.connSrcEntityName = connInfo.connSrcEntityName;
          objSSCmd.connSrcEntityID = connInfo.connSrcEntityID;
          this._LOG.info(" notifyStreamDst:Stop: New Stream: SSCmd: SSAction: " + objSSCmd.streamAction +
                                  ": InstanceId: " + objSSCmd.instanceId + ": connId: " + objSSCmd.connId +
                                  ": StreamURL: " + objSSCmd.streamURL +
                                  ": PlayerTopic: " + tDst + ": RoomNID: " + objDst.MediaRoomNID + ": StreamNID: " + streamNID);

          topic_Publish(tDst, "MediaStreamDstState", objSSCmd);
        }

        // stash new stream URL
        objStream.streamURL = streamURL;
        // if the current action is not a start - send a start with the new URL
        if (streamAction != "Start") {
          var objSSCmd = new Object;
          objSSCmd.instanceId = objStream.instanceId;
          objSSCmd.streamAction = "Start";
          objSSCmd.streamURL = streamURL;
          objSSCmd.connId = connInfo.connId;
          objSSCmd.sessionId = connInfo.SessionID;
          objSSCmd.streamNID = streamNID;
          //using srcEntityType only when "Start"
          objSSCmd.srcEntityType = connInfo.connSrcEntityType;
          objSSCmd.connSrcEntityName = connInfo.connSrcEntityName;
          objSSCmd.connSrcEntityID = connInfo.connSrcEntityID;
          objSSCmd.sessionTimescale = connInfo.sessionTimescale;
          this._LOG.info(" notifyStreamDst:Start: New Stream: SSCmd: SSAction: " + objSSCmd.streamAction +
                                  ": InstanceId: " + objSSCmd.instanceId + ": connId: " + objSSCmd.connId +
                                  ": StreamURL: " + objSSCmd.streamURL +
                                  ": PlayerTopic: " + tDst + ": RoomNID: " + objDst.MediaRoomNID + ": StreamNID: " + streamNID);
          topic_Publish(tDst, "MediaStreamDstState", objSSCmd);
          /**
          * Bug fix 21773: Suppose one session(with 2 media clients) start and click SkipBackward 
          * another mediaclient join the session, streamAction="SkipBackward". When join such a session
          * Workflow should be 'Add', 'Start' this stream. 'SkipBackward' has to be skipped otherwise playerSkipBackward
          * thows exception because of IPVideoPlayer not being loaded at the time being. 
          * Using the following jsx.sleep to solve the problem.    
          **/
          jsx3.sleep((function () {
            objStream.newAddedStream = null;
          }), null, this);
        }
      }
    }
    // stash the new action
    objStream.streamAction = streamAction;
    if (objStream.newAddedStream == null || objStream.streamAction == "Pause" || objStream.streamAction == "Start") {
      // tell the destination about it
      var objSSCmd = new Object;
      objSSCmd.instanceId = objStream.instanceId;
      objSSCmd.streamAction = streamAction;
      objSSCmd.streamURL = streamURL;
      objSSCmd.connId = connInfo.connId;
      objSSCmd.sessionId = connInfo.SessionID;
      objSSCmd.streamNID = streamNID;
      objSSCmd.srcEntityType = connInfo.connSrcEntityType;
      objSSCmd.connSrcEntityName = connInfo.connSrcEntityName;
      objSSCmd.connSrcEntityID = connInfo.connSrcEntityID;
      objSSCmd.sessionTimescale = connInfo.sessionTimescale;
      this._LOG.info(" notifyStreamDst: SSCmd: SSAction: " + objSSCmd.streamAction +
                                    ": InstanceId: " + objSSCmd.instanceId + ": connId: " + objSSCmd.connId +
                                    ": StreamURL: " + objSSCmd.streamURL +
                                    ": PlayerTopic: " + tDst + ": RoomNID: " + objDst.MediaRoomNID + ": StreamNID: " + streamNID);

      //clean up this.arrDstObj[tDst] & this.arrMediaRoom
      if (objStream.streamAction == "Delete") {
        for (lcStreamNID in objDst.objMediaRoom) {
          if (lcStreamNID == objSSCmd.streamNID) {
            delete objDst.objMediaRoom[lcStreamNID];
            break;
          }
        }
      }
      //not a new added stream any more
      if (objStream.newAddedStream != null) objStream.newAddedStream = null;
      topic_Publish(tDst, "MediaStreamDstState", objSSCmd);

    }
  }

  /**
  * Add resource instance
  * @param portInstanceId {String}
  * @param streamType {String}
  */
  MediaStreamDstAgent_prototype.addResourceInstance = function (portInstanceId, streamType) {
    //make sure this port instance id exists
    if (this.arrResourceInstanceList[portInstanceId] == null) {
      this.arrResourceInstanceList[portInstanceId] = new Object;
    }
    // create a resource instance for thsi port id
    var resourceInstance = dsaResourceInstanceAdd(portInstanceId, streamType);
    var IpPort = dsaResourceInstanceGetRxMediaIpPort(resourceInstance);
    // store this instance in the array
    var instanceId = jsx3.xml.CDF.getKey();
    this.arrResourceInstanceList[portInstanceId][instanceId] = new Object;
    this.arrResourceInstanceList[portInstanceId][instanceId].resourceInstance = resourceInstance;
    this.arrResourceInstanceList[portInstanceId][instanceId].IpPort = IpPort;
    return instanceId;
  }


  /**
  * Delete resource instance
  * @param portInstanceId {String}
  * @param instanceId {String}
  */
  MediaStreamDstAgent_prototype.delResourceInstance = function (portInstanceId, instanceId) {
    if (instanceId != null) {
      if (this.arrResourceInstanceList[portInstanceId][instanceId] != null) {
        var resourceInstance = this.arrResourceInstanceList[portInstanceId][instanceId].resourceInstance;
        dsaResourceInstanceDelete(resourceInstance);
        this.arrResourceInstanceList[portInstanceId][instanceId] = null;
        return true;
      }
    }
    else {
      if (this.arrResourceInstanceList[portInstanceId] != null) {
        for (var tInstance in this.arrResourceInstanceList[portInstanceId]) {
          if (this.arrResourceInstanceList[portInstanceId][tInstance] != null) {
            var resourceInstance = this.arrResourceInstanceList[portInstanceId][tInstance].resourceInstance;
            dsaResourceInstanceDelete(resourceInstance);
            this.arrResourceInstanceList[portInstanceId][tInstance] = null;
          }
        }
        this.arrResourceInstanceList[portInstanceId] = null;
        return true;
      }
    }
    return false;
  }


  /**
  * Get resource instance IP port
  * @param portInstanceId {String}
  * @param instanceId {String}
  */
  MediaStreamDstAgent_prototype.getResourceInstanceIpPort = function (portInstanceId, instanceId) {
    var IpPort = this.arrResourceInstanceList[portInstanceId][instanceId].IpPort;
    return IpPort;
  }

  /**
  * Reset destination stream
  * @param tDst {String}
  */
  MediaStreamDstAgent_prototype.resetStreamDst = function (tDst) {
    if (this.arrDstObj[tDst] == null) throw new jsx3.Exception("DstAgent: Reset: PlayerTopic: " + tDst + ": Not found");
    var objDst = this.arrDstObj[tDst];
    // clear objs
    objDst.ResourceNID = null;
    objDst.MediaRoomNID = null;
    objDst.objMediaRoom = null;
    objDst.nodeMediaRoom = null;
    objDst.SessionID = null;
    objDst.nodeSession = null;
    // clear the lastAction
    objDst.lastAction = "NONE";
    // TBD - things like layout profiles etc..
    // that will come in the messages from the system
  }

  /**
  * Unregister destination stream
  * @param tDst {String}
  */
  MediaStreamDstAgent_prototype.unregisterStreamDst = function (tDst) {
    if (this.arrDstObj[tDst] == null) {
      //calling from cmpWinMediaPlayer_Destroy(), this.arrDstObj[tDst] already gone,avoid to throw exception
      log.warn("DstAgent: UnRegister: PlayerTopic: " + tDst + ": Not found");
      return;
    }
    /// initialize the objects and state vars
    this.resetStreamDst(tDst);
    //When closing MediaPlayer window remove current topic from this.arrDstObj
    delete this.arrDstObj[tDst];
  }

  /**
  * Register destination stream
  * @param tDst {String}
  * @param ResourceNID {String}
  */
  MediaStreamDstAgent_prototype.registerStreamDst = function (tDst, ResourceNID) {
    // check / create new entry in arr 
    if (this.arrDstObj[tDst] != null) throw new jsx3.Exception("DstAgent: Register: PlayerTopic: " + tDst + ": Already exists");
    this.arrDstObj[tDst] = new Object;
    // initialize the objects and state vars
    this.resetStreamDst(tDst);
    // - init dst info
    var objDst = this.arrDstObj[tDst];
    objDst.ResourceNID = ResourceNID;
  }



  /**
  * Join session
  * @param tDst {String}
  * @param SessionID {String}
  */
  MediaStreamDstAgent_prototype.joinSession = function (tDst, SessionID) {

    // error
    function joinError(tDst) {
      if (this.arrDstObj[tDst] == null) return;
    }

    // success
    function joinSuccess(tDst, MediaRoomNID) {
      if (this.arrDstObj[tDst] == null) return;
      var objDst = this.arrDstObj[tDst];
      // - stash MediaRoomNID 
      objDst.objMediaRoom = new Object;
      objDst.MediaRoomNID = MediaRoomNID;

      // update mediaroom info
      this.updateMediaRoomInfo(tDst, MediaRoomNID);

      //store current session ownerUserJID
      var ownerUserJIDNode = null;
      if (objDst.nodeSession != null) {
        ownerUserJIDNode = objDst.nodeSession.selectSingleNode(".//Properties/@ownerUserJID");
      }
      var ownerUserJID = ownerUserJIDNode == null ? null : ownerUserJIDNode.getValue();
      topic_Set(tDst, "SessionOwnerUserJID", ownerUserJID);

      // set the lastAction
      objDst.lastAction = "JOIN";
    }

    log.info("DstAgent: Join Session: PlayerTopic: " + tDst + ": Session: " + SessionID);
    var displayConfigNode = topic_GetNode(tDst, "//DisplayConfig");
    topic_Publish(tDst, "DisplaySessionCanvas", displayConfigNode);
    // reset the objects and state vars - for good measure
    this.resetStreamDst(tDst);

    // - stash SessionID
    var objDst = this.arrDstObj[tDst];
    objDst.SessionID = SessionID;
    // get mediaroom for this session
    this.getMediaRoomList(tDst, joinSuccess, SessionID);
  }


  /**
  * Leave session
  * @param tDst {String}
  */
  MediaStreamDstAgent_prototype.leaveSession = function (tDst) {
    if (!this.arrDstObj[tDst]) {
      log.warn("DstAgent: Leave session: PlayerTopic: " + tDst + ": Not found");
    }
    else {
      var objDst = this.arrDstObj[tDst];
      // set the lastAction
      objDst.lastAction = "LEAVE";
      // reset the objects and state vars - for good measure
      this.resetStreamDst(tDst);
      log.info("DstAgent: Leave session: PlayerTopic: " + tDst);
    }
    topic_Set(tDst, "SessionOwnerUserJID", "NONE");
  }

  /*
  * find mapping of streamNID and connectionID from MediaRoom/Stream
  * if bUpdate is true - then send a request to get updated mediaroom info
  * @param tDst {String}
  * @param streamNID {String}
  * @param bUpdate (Boolean)
  * @return connInfo {Object}
  */
  MediaStreamDstAgent_prototype.getStreamConnInfo = function (tDst, objDst, streamNID, bUpdate) {
    var connInfo = null;
    // try to get the session-conn node for this stream
    var mediaRoomNode = objDst.nodeMediaRoom;
    if (mediaRoomNode != null) {
      var sessionConnIDNode = mediaRoomNode.selectSingleNode(".//Stream[@NID='" + streamNID + "']/@id");
      // if not found and 'bUpdate' - send request to update media room info - return
      if (sessionConnIDNode == null) {
        if (bUpdate) {
          this.getMediaRoomList(tDst, this.updateMediaRoomInfo, objDst.SessionID, streamNID);
        }
      }
      else {
        connInfo = new Object;
        connInfo.SessionID = objDst.SessionID;
        connInfo.connId = sessionConnIDNode.getValue();
        var connSrcEntityIdNode = mediaRoomNode.selectSingleNode(".//Connection[@id='" + connInfo.connId + "']/@srcEntityId");
        //srcEntity of a connection shouldn't be empty, defensive check.
        if (!connSrcEntityIdNode) {
          log.error("DstAgent::notifyStreamDst::connection(" + connInfo.connId + ") has no srcEntity.");
          return;
        }
        connInfo.connSrcEntityID = connSrcEntityIdNode.getValue();
        var connSrcEntityNameNode = mediaRoomNode.selectSingleNode(".//Entity[@id='" + connInfo.connSrcEntityID + "']/@name");
        connInfo.connSrcEntityName = connSrcEntityNameNode == null ? "" : connSrcEntityNameNode.getValue();
        var connSrcEntityTypeNode = mediaRoomNode.selectSingleNode(".//Entity[@id='" + connInfo.connSrcEntityID + "']/@type");
        connInfo.connSrcEntityType = connSrcEntityTypeNode == null ? "" : connSrcEntityTypeNode.getValue();
        connInfo.ffTrackState = (!jsx3.util.strEmpty(mediaRoomNode.selectSingleNode(".//Stream[@NID='" + streamNID + "']//data//FFTracks"))) && (!jsx3.util.strEmpty(mediaRoomNode.selectSingleNode(".//Stream[@NID='" + streamNID + "']//data//FFTracks").getValue())) ? true : false;
        var lastNodeTrackMediaTimeline = mediaRoomNode.selectSingleNode(".//TrackMediaTimeline/Last");
        var timeScale = lastNodeTrackMediaTimeline ? lastNodeTrackMediaTimeline.getAttribute("TimeScale") : "1.0";
        connInfo.sessionTimescale = timeScale;
      }
    }
    return connInfo;
  }


  /**
  * Update mediaRoom info.
  * @param tDst {String}
  * @param MediaRoomNID {String}
  * @param streamNID {String}
  */
  MediaStreamDstAgent_prototype.updateMediaRoomInfo = function (tDst, MediaRoomNID, streamNID) {
    if (this.arrDstObj[tDst] == null) return;
    var objDst = this.arrDstObj[tDst];

    // get the mediaroom
    var nodeMediaRoom = topic_GetNIDNode(tCache, MediaRoomNID);
    objDst.nodeMediaRoom = nodeMediaRoom.cloneNode(true);
    // get session node if present
    var nodeSession = nodeMediaRoom.selectSingleNode(".//Session");
    objDst.nodeSession = nodeSession ? nodeSession.cloneNode(true) : null;
    // update any preexisting stream info for this mediaroom
    if (this.arrMediaRoom[objDst.MediaRoomNID] != null) {
      var objMediaRoom = this.arrMediaRoom[objDst.MediaRoomNID];      
      // for each stream nid (or only the one specified as input param)
      for (var lcStreamNID in objMediaRoom) {
        if ((!streamNID) || (lcStreamNID == streamNID)) {
          // get the conn info and send stream update
          connInfo = this.getStreamConnInfo(tDst, objDst, lcStreamNID, false);
          if (connInfo) {
            this._LOG.info(" updateMediaRoomInfo:call notifyStreamDst with timescale " + connInfo.sessionTimescale);
            this.notifyStreamDst(tDst, this.arrDstObj[tDst], lcStreamNID, objMediaRoom[lcStreamNID].StreamURL, objMediaRoom[lcStreamNID].Action, connInfo);
          }
        }
      }
    }
  }


  /**
  * Get mediaRoom list
  * @param tDst {String}
  * @param SessionID {String}
  * @param callBackFnc {String}
  * @param streamNID {String}
  */
  MediaStreamDstAgent_prototype.getMediaRoomList = function (tDst, callBackFnc, SessionID, streamNID) {
    var objDst = this.arrDstObj[tDst];
    if (this.arrDstObj[tDst] == null) throw new jsx3.Exception("MediaStreamDstAgent: updateMediaRoom: PlayerTopic: " + tDst + ": Not found");
    var ThisClass = this;
    var reqGetMediaRoomList = new ReqGetObjectList("MediaRoom",
                                                   { action: "FNCCALL", context: ThisClass,
                                                     fnc: function (respCode, respVal) {
                                                       if (respCode == "SUCCESS" && respVal != null) {
                                                         var mediaroomNodeIter = respVal.selectNodeIterator(".//ObjectNID");
                                                         if (SessionID) {
                                                           //call back for 'SessionID' and 'streamNID'
                                                           objDst.SessionID = SessionID;
                                                           var findRoom = false;
                                                           while (mediaroomNodeIter.hasNext()) {
                                                             var roomNID = mediaroomNodeIter.next().getValue();
                                                             var nodeMediaRoom = topic_GetNIDNode(tCache, roomNID);
                                                             if (nodeMediaRoom) {
                                                               var roomSessionID = nodeMediaRoom.getAttribute("id");
                                                               if (roomSessionID == objDst.SessionID) {
                                                                 callBackFnc.call(ThisClass, tDst, roomNID, streamNID);
                                                                 findRoom = true;
                                                                 break;
                                                               }
                                                             }
                                                           }
                                                           if (!findRoom) {
                                                             log.error("MediaStreamDstAgent: getMediaRoomList: PlayerTopic: " + tDst + ": Get MediaRoom for Session: " + SessionID + "not found");
                                                           }
                                                         }
                                                         else {
                                                           //call back for other getMediaroomList
                                                           callBackFnc.call(ThisClass, tDst, mediaroomNodeIter);
                                                         }
                                                       }
                                                       else {
                                                         log.error("MediaStreamDstAgent: getMediaRoomList: PlayerTopic: " + tDst + ": Get MediaRoom for Session: " + SessionID + "request failed");
                                                       }
                                                     }
                                                   }
                                                  );
    reqGetMediaRoomList.ServiceName = "mediaroomservice";
    reqGetMediaRoomList.sendRequest(this);

  }

  /**
  * delete connection for a window
  * @param winId or ConnId {String}
  */
  MediaStreamDstAgent_prototype.sendSSCmdDeleConn = function (windowId) {
    var sendRequest = false;
    var Type = null;
    var Id = null;
    var xopInput = 'input';
    var xopOutput = 'output';
    for (var objSSid in this.SS[xopOutput]) {
      var xobjSSOutput = this.SS[xopOutput][objSSid];
      var objssOutputQueueid = xobjSSOutput.queueid;
      var ssWindowId = this.SS[xopInput][objssOutputQueueid].windowId;
      if (ssWindowId == windowId) {
        Type = xobjSSOutput.dsttype;
        Id = xobjSSOutput.id;
        sendRequest = true;
        break;
      }
    }
    //send the request/message to delete/stop the connection
    if (sendRequest) {
      this._sendDelConnAgentEventMessage(windowId, Id, Type);
    }
  }

  /**
  * delete connection for a window
  * @param winId or ConnId {String}
  */
  MediaStreamDstAgent_prototype._sendDelConnAgentEventMessage = function (windowId, ssCmdId, ssCmdPortType) {
    var AgentJID = topic_Get(tApp, "LoginUserJID");
    var timeStamp = (newServerDate()).getTime();
    /*
    var agentEventXML = "<Event><header><eventLevel>200</eventLevel><eventAgentJID>" + AgentJID + "</eventAgentJID><eventWallclock>" + timeStamp + "</eventWallclock><eventname>SSEvent</eventname></header><data><SSEventData><action>SSEvent</action><SSEventActionData><SSNotification><Header><EventType>Stopped</EventType><Id>" + ssCmdId + "</Id><Type>" + ssCmdPortType + "</Type><Status>0</Status><WallClock>" + timeStamp + "</WallClock></Header><Data><QueueId>0</QueueId><Status>Stopped</Status><StopTimeStamp>" + timeStamp + "</StopTimeStamp><StopWallClock>" + timeStamp + "</StopWallClock></Data></SSNotification></SSEventActionData></SSEventData></data><log></log></Event>";
    
    var agentEventXMLDoc = new jsx3.xml.Document();
    agentEventXMLDoc.loadXML(agentEventXML);
    var agentEventNode = agentEventXMLDoc.selectSingleNode("/*").cloneNode(true);
    */
    var agentEventStr = '"Event": { "header": { "eventLevel": "200", "eventAgentJID": "' + AgentJID + '", "eventWallclock": "' + timeStamp + '", "eventname": "SSEvent" },' + 
                                    '"data": { "SSEventData": { "action": "SSEvent", "SSEventActionData": { "SSNotification": { ' +
                                               '"Header": { "EventType": "Stopped", "Id": "' + ssCmdId + '", "Type": "' + ssCmdPortType + '", "Status": "0", "WallClock": "' + timeStamp + '" },' +
                                               '"Data": { "QueueId": "0", "Status": "Stopped", "StopTimeStamp": "' + timeStamp + '", "StopWallClock": "' + timeStamp + '" } ' +
                                             '} } } } }';

    var reqSendAgentEventMessage = new ReqSendAgentEventMessageObject(AgentJID, agentEventStr,
                                                                          { action: "FNCCALL", context: this,
                                                                            fnc: function (respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                var resVal = respVal;
                                                                                var resCode = respCode;
                                                                              } else {
                                                                                var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                                var ErrorMsg = "Error: Device->Resource get data request failed \n";
                                                                                ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                                ErrorMsg += "Error code: " + respCode;
                                                                                alert(ErrorMsg);
                                                                              }
                                                                            }
                                                                          }
                                                                        );

    reqSendAgentEventMessage.sendRequest(this);

  }

  /**
  * Envent listener sections
  * @param eventApp {Object}
  */
  MediaStreamDstAgent_prototype.eventWrap = function (eventApp) {
    eventApp.subscribe("PRESENCE", null, this,
                      function (EventVar, EventVal) {
                        var presence = EventVal;
                        // check if it is a entity JID presence
                        if (presence.JID != null && presence.JID != "") {
                          // TBD - potentially do auto shutdown / repair logic if state if false etc..
                          return;
                        }
                        // otherwise this is a Room presence
                        this.roomPresence(presence);
                      });


    eventApp.subscribe("ROOMEVENT", null, this,
                        function (EventVar, EventVal) {
                          var roomevent = EventVal;
                          this.roomEvent(roomevent);
                        });

    eventApp.subscribe("SSCmd", null, this,
                        function (EventVar, EventVal) {
                          var objSSCmd = EventVal;
                          this._handleSSEvent(objSSCmd);
                        });
  }
});

