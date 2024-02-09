jsx3.require("jsx3.gui.Interactive");
jsx3.Class.defineClass("com.ipvs.api.Notification", null, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function (Notification, Notification_prototype) {

  /* Static constant variables of message types of com.ipvs.api.Notification */
  Notification.MSG_PRESENCE = "PRESENCE";
  Notification.MSG_SSCMD = "SSCmd";
  Notification.MSG_RESPONSERECEIVE = "RESPONSERECEIVE";
  Notification.MSG_SESSIONSTATEEVENT = "SESSIONSTATEEVENT";
  Notification.MSG_CONNSTATEEVENT = "CONNSTATEEVENT";
  Notification.MSG_ENTITYPRESENCEEVENT = "ENTITYPRESENCEEVENT";
  Notification.MSG_TIMELINEEVENT = "TIMELINEEVENT";
  Notification.MSG_DBUPDATE = "DBUpdate";
  Notification.MSG_SESSIONPLAYBACKIQNODE = "SessionPlaybackIQNode";
  Notification.MSG_IMPORTFILEEVENT = "IMPORTFILEEVENT";
  Notification.MSG_ROOMEVENT = "ROOMEVENT";
  Notification.MSG_AGENTEVENT = "AGENTEVENT";
  Notification.MSG_FILEXFEREVENT = "FILEXFEREVENT";

  //mediastoreservice may be obsolete only using MW internal
  Notification.SERVIC_MAPPING_LIST = {
                                      "systemservice": "com.ipvs.mediaroomservice.impl.AppServerAgent",
                                      "mediastoreservice": "com.ipvs.mediaroomservice.mediastore.MediaStoreAdminClientAgent",
                                      "mediaroomservice": "com.ipvs.mediaroomservice.impl.AppServerAgent",
                                      "assetadminservice": "com.ipvs.mediaroomservice.impl.AppServerAgent",
                                      "policyadminservice": "com.ipvs.mediaroomservice.impl.AppServerAgent",
                                      "sessionservice": "com.ipvs.mediaroomservice.impl.AppServerAgent",
                                      "Layout": "com.ipvs.mediaroomservice.impl.DestStreamAgent",
                                      "Media": "com.ipvs.mediaroomservice.impl.SourceStreamAgent",
                                      "DeviceAdmin": "com.ipvs.common.service.DeviceAdminClientAgent",
                                      "Config": "com.ipvs.common.service.DeviceAdminClientAgent",
                                      "MediaStore": "com.ipvs.mediaroomservice.mediastore.MediaStoreAdminClientAgent",
                                      "FileTransfer": "com.ipvs.systemservice.impl.FileTransferAgent",
                                      "Operator": "Operator"
                                    };

  /** @private @jsxobf-clobber */
  Notification_prototype._LOG = jsx3.util.Logger.getLogger(Notification.jsxclass.getName());

  Notification_prototype.init = function () {
    // get a handle to the App
    this.clsApp = com.ipvs.app.App.clsApp;
    if (!this.clsApp) return;
  }

  Notification_prototype.handleAppletXML = function (strAppletXML) {
    // convert to Doc format
    var docResponse = new jsx3.xml.Document();
    docResponse.loadXML(strAppletXML);
    docResponse.setSelectionNamespaces({ 'http://jabber.org/protocol/muc#user': "jsxnsMucUser",
      'http://jabber.org/protocol/muc#admin': "jsxnsMucAdmin",
      'http://jabber.org/protocol/pubsub#event': "jsxnsPubsubEvent",
      'jabber:client': "jsxnsJabberClient",
      'ipvs:mediaroom:ipvsdbservice': "jsxnsIPVSDBService",
      'com.ipvs.session': "jsxnsIPVSSessionService",
      'jabber:iq:roster': "jsxnsJabberRoster"
    });

    // clean our excess chars for log
    strAppletXML = strAppletXML.replace(/[\n\r]/g, " ").replace(/>\s+</g, "><");

    // ==================== check if it is a legacy 'Response' (typically will come inside IQ/Message)
    var responseNode = docResponse.selectSingleNode("/Response");
    if (responseNode != null) {
      this._handleNotification_Response(responseNode);
      return;
    }
    // ==================== check if it is a StreamingServer 'SS' cmd
    var ssCmdNode = docResponse.selectSingleNode("/SS");
    if (ssCmdNode != null) {
      this._handleNotification_SSCmd(ssCmdNode);
      return;
    }
    // ==================== check if it is an 'iq' or 'notification/iq'
    var iqNode = docResponse.selectSingleNode("/iq");
    if (iqNode == null) iqNode = docResponse.selectSingleNode(".//notificationdata/iq");
    if (iqNode != null) {
      this._handleNotification_IQ(iqNode);
      return;
    }
    // ==================== check if it is a 'message' or 'notification/message'
    var messageNode = docResponse.selectSingleNode("/message");
    if (messageNode == null) messageNode = docResponse.selectSingleNode(".//notificationdata/message");
    if (messageNode != null) {
      this._handleNotification_Message(messageNode);
      return;
    }
    // ================== check if it is a 'presence' or 'notification/presence'
    var presenceNode = docResponse.selectSingleNode("/presence");
    if (presenceNode == null) presenceNode = docResponse.selectSingleNode(".//notificationdata/presence");
    if (presenceNode != null) {
      this._handleNotification_Presence(presenceNode);
      return;
    }
    // ================== check if it is a 'xmpperror' or 'notification/shutdown'
    var shutdownNode = docResponse.selectSingleNode("/xmpperror");
    if (shutdownNode == null) shutdownNode = docResponse.selectSingleNode(".//notificationdata/shutdown");
    if (shutdownNode != null) {
      this._handleNotification_Shutdown(shutdownNode);
      return;
    }
    // ============== log unknown packet
    this._LOG.warn("Unknown XMPP Packet : " + strAppletXML);
    return;
  }

  //  common response handler -
  //    - for Response
  //      - uses clientdata to get origianl request-specific topicname
  //      - puts response data in the topic
  //      - also puts response data in the cache as needed
  //    Inputs:
  //      - nodeResponse = response node from applet notification
  Notification_prototype._handleNotification_Response = function (nodeResponse) {
    if (nodeResponse.getNodeName() == "Response" || nodeResponse.getNodeName() == "iq") {

      // log response string
      this._LOG.info("Response: " + (nodeResponse.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));

      // check service version
      //Use singleton accessor method of App class and get service version
      var serviceVersion = this.clsApp.clsApplet.getServiceVersion();
      var serviceVersionResponse = "";
      if (nodeResponse.getNodeName() == "Response") {
        serviceVersionResponse = nodeResponse.getAttribute("serviceVer");
      }
      else if (nodeResponse.getNodeName() == "iq") {
        serviceVersionResponse = nodeResponse.selectSingleNode("./*").getAttribute("serviceVer");
      }
      if (serviceVersionResponse != serviceVersion) {
        this._LOG.warn("Service Version mismatch: Response :" + (nodeResponse.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
      }

      // get request-specific topicname
      var TopicName = null;
      if (nodeResponse.getNodeName() == "Response") {
        if (nodeResponse.selectSingleNode(".//header/clientdata") != null) {
          TopicName = nodeResponse.selectSingleNode(".//header/clientdata").getValue();
        }
      }
      else if (nodeResponse.getNodeName() == "iq") {
        TopicName = nodeResponse.getAttribute("id");
      }

      // and fire off a recieive event 
      var objEvent = new Object;
      objEvent.subject = TopicName;
      objEvent.stateVar = "RequestResponseState";
      objEvent.stateVal = Notification.MSG_RESPONSERECEIVE;
      objEvent.responseNode = nodeResponse.cloneNode(true);
      this.publish(objEvent);
      return;
    }

    this._LOG.warn("Unknown Response : " + (nodeResponse.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
    return;
  }

  Notification_prototype._handleNotification_SSCmd = function (nodeSSCmd) {
    // get the cmd string
    var strSSCmd = nodeSSCmd.getValue();
    this._LOG.info("SSCmd: " + strSSCmd);
    // parse it into object form
    var objSSCmd = new Object();
    // strip off the JID
    strSSCmd = strSSCmd.substring(strSSCmd.indexOf(':') + 1);

    // split the cmd str into cmd tokens  
    // get token array without "'"
    var arrSSCmd1 = strSSCmd.match(/\w+=[^!\'\s]+/g);
    this._addStrArrToObjSSCmd(arrSSCmd1, objSSCmd);
    // get token array with "'" 
    var arrSSCmd2 = strSSCmd.match(/\w+=\'[^!\']+\'/g);
    this._addStrArrToObjSSCmd(arrSSCmd2, objSSCmd);
    // publish the object
    objSSCmd.subject = Notification.MSG_SSCMD;
    this.publish(objSSCmd);
    return;
  }

  //help function for handleNotification_SSCmd
  Notification_prototype._addStrArrToObjSSCmd = function (arrSSCmd, objSSCmd) {
    if (!arrSSCmd) return;
    for (var i = 0; i < arrSSCmd.length; i++) {
      var arrSSCmdTokName = arrSSCmd[i].substring(0, arrSSCmd[i].indexOf('='));
      var arrSSCmdTokValue = arrSSCmd[i].substring(arrSSCmd[i].indexOf('=') + 1);
      // trim the value stri
      arrSSCmdTokValue = arrSSCmdTokValue.replace(/\'/g, '');
      // assign to cmd obj prop=val
      objSSCmd[arrSSCmdTokName] = arrSSCmdTokValue;
    }
  }

  Notification_prototype._handleNotification_Message = function (messageNode) {
    // -------- helper function --------------
    function timelineNodeToObj(nodeTimeline) {
      // get values from it
      var objTimeline = new Object;
      objTimeline.sessionId = nodeTimeline.getAttribute("sessionId");
      objTimeline.connGroupId = nodeTimeline.getAttribute("connGroupId");
      objTimeline.connId = nodeTimeline.getAttribute("connId");
      objTimeline.timelineId = nodeTimeline.getAttribute("id");

      var startNode = nodeTimeline.selectSingleNode(".//jsxnsIPVSSessionService:Start");
      objTimeline.startOffset = startNode.getAttribute("Offset");
      objTimeline.startWC = startNode.getAttribute("WC");

      var lastNode = nodeTimeline.selectSingleNode(".//jsxnsIPVSSessionService:Last");
      objTimeline.lastOffset = lastNode.getAttribute("Offset");
      objTimeline.lastWC = lastNode.getAttribute("WC");
      objTimeline.timeScale = lastNode.getAttribute("TimeScale");
      objTimeline.lastAction = lastNode.getAttribute("requestedAction");

      //TBD: there seem to be other fields as well
      // - need to figure out if we need them
      // - action, sourceAction, ssTC, ssWC, lastState

      return objTimeline;
    }
    //------------------- check if it is session related message
    var xNode = messageNode.selectSingleNode(".//jsxnsIPVSSessionService:x");
    if (xNode != null) {
      //check for SessionState msg
      var SessionStateNode = xNode.selectSingleNode(".//jsxnsIPVSSessionService:SessionState");
      if (SessionStateNode != null) {
        // get values from it
        var objSessionState = new Object;
        objSessionState.toJID = messageNode.getAttribute("to");
        objSessionState.sessionId = SessionStateNode.getAttribute("sessionId");
        objSessionState.state = SessionStateNode.getAttribute("value");

        this._LOG.info("Message: SessionService: SessionState Event: " +
                    ": state: " + objSessionState.state +
                    ": sessionId: " + objSessionState.sessionId);
        //publish as SessionStateEvent
        objSessionState.subject = Notification.MSG_SESSIONSTATEEVENT;
        this.publish(objSessionState);
        return;
      }

      //check for ConnectionState msg
      var ConnStateNode = xNode.selectSingleNode(".//jsxnsIPVSSessionService:ConnState");
      if (ConnStateNode != null) {
        // get values from it
        var objConnState = new Object;
        objConnState.toJID = messageNode.getAttribute("to");
        objConnState.sessionId = ConnStateNode.getAttribute("sessionId");
        objConnState.connId = ConnStateNode.getAttribute("connId");
        objConnState.streamURL = ConnStateNode.getAttribute("streamURL");
        objConnState.state = ConnStateNode.getAttribute("value");
        //extract erroe code and description
        if (!jsx3.util.strEmpty(ConnStateNode.getAttribute("errorCode"))) {
          objConnState.errorCode = ConnStateNode.getAttribute("errorCode");
          objConnState.errorData = ConnStateNode.getAttribute("errorData");
        }
        // also extract the embedded timeline
        var nodeConnStateTimeline = ConnStateNode.selectSingleNode(".//jsxnsIPVSSessionService:Timeline");
        if (nodeConnStateTimeline != null) {
          objConnState.objTimeline = timelineNodeToObj(nodeConnStateTimeline);
          // *HACK* patch in the session Id - not being sent by middleware
          objConnState.objTimeline.sessionId = objConnState.sessionId;
        }
        this._LOG.info("Message: SessionService: ConnState Event: " +
                    ": state: " + objConnState.state +
                    ": sessionId: " + objConnState.sessionId +
                    ": connId: " + objConnState.connId +
                    ": streamURL: " + objConnState.streamURL);
        //publish as connStateEvent
        objConnState.subject = Notification.MSG_CONNSTATEEVENT;
        this.publish(objConnState);
        return;
      }

      // check for EntityPresence msg   
      var EntityPresenceNode = xNode.selectSingleNode(".//jsxnsIPVSSessionService:EntityPresence");
      if (EntityPresenceNode != null) {
        // get values from it
        var objEntityPresence = new Object;
        objEntityPresence.toJID = messageNode.getAttribute("to");
        objEntityPresence.sessionId = EntityPresenceNode.getAttribute("sessionId");
        objEntityPresence.entityId = EntityPresenceNode.getAttribute("entityId");
        objEntityPresence.userJID = EntityPresenceNode.getAttribute("jid");
        objEntityPresence.presence = EntityPresenceNode.getAttribute("value");

        this._LOG.info("Message: SessionService: EntityPresence Event: " +
                    ": sessionId: " + objEntityPresence.sessionId +
                    ": entityId: " + objEntityPresence.entityId +
                    ": jid: " + objEntityPresence.jid +
                    ": presence: " + objEntityPresence.presence);
        // and publish as EntityPresenceEvent
        objEntityPresence.subject = Notification.MSG_ENTITYPRESENCEEVENT;
        this.publish(objEntityPresence);
        return;
      }

      //check for Timeline msg
      var TimelineNode = xNode.selectSingleNode(".//jsxnsIPVSSessionService:Timeline");
      if (TimelineNode != null) {
        // get values from it
        var objTimeline = timelineNodeToObj(TimelineNode);
        objTimeline.toJID = messageNode.getAttribute("to");

        this._LOG.info("Message: SessionService: TimeLine Event: " +
                    ": sessionId: " + objTimeline.sessionId +
                    ": connId: " + objTimeline.connId +
                    ": timelineId: " + objTimeline.timelineId +
                    ": connGroupId: " + objTimeline.connGroupId +
                    ": startOffset: " + objTimeline.startOffset +
                    ": startWC: " + objTimeline.startWC +
                    ": lastOffset: " + objTimeline.lastOffset +
                    ": lastWC: " + objTimeline.lastWC +
                    ": timeScale: " + objTimeline.timeScale +
                    ": lastAction: " + objTimeline.lastAction);
        // and publish as TimeLineEvent
        objTimeline.subject = Notification.MSG_TIMELINEEVENT;
        this.publish(objTimeline);
        return;
      }
      this._LOG.warn("Unknown SessionService Message : " + (messageNode.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
      return;
    }

    // ---------------- check if it is a DB update event
    var updateEventNode = messageNode.selectSingleNode(".//jsxnsIPVSDBService:updateEvent");
    if (updateEventNode != null) {
      var nodeID = updateEventNode.getAttribute("nodeID");
      if (nodeID == null || nodeID == "") {
        this._LOG.warn("Message: IPVSDBService UpdateEvent: Empty nodeID: ");
        return;
      };

      var eventType = updateEventNode.getAttribute("type");
      if (eventType != "add" && eventType != "replace" && eventType != "delete") {
        this._LOG.warn("Message: IPVSDBService UpdateEvent: EventType: " + eventType + " : NID: " + nodeID);
        return;
      }
      //return true; //TBR 
      var eventChildXPath = updateEventNode.getAttribute("childXPath");
      var parentList = nodeID.substring(nodeID.indexOf(".") + 1, nodeID.lastIndexOf("."));
      var objType = parentList;
      var objEvent = new Object;
      objEvent.nodeID = nodeID;
      objEvent.childXpath = unescape(eventChildXPath);
      objEvent.objType = objType;
      objEvent.type = eventType;
      if (((eventType == "add") || (eventType == "delete")) && (!jsx3.util.strEmpty(eventChildXPath))) {
        objEvent.type = "replace";
      }

      this._LOG.info("Message: IPVSDBService UpdateEvent: Node " + eventType + ": Dirty NID: " + nodeID);
      objEvent.subject = Notification.MSG_DBUPDATE;
      this.publish(objEvent);
      return true;

    }
    // -------------- check if it is MR Service Message 
    // TBD: ignore the nested 'x' tag (without namespace)
    var xNode = messageNode.selectSingleNode(".//x");
    if (xNode != null) {
      if (messageNode.selectSingleNode(".//AgentMessage") != null) {
        if (this._handleNotification_AgentMessage(messageNode) != true) {
          this._LOG.warn("Unknown MRService AgentMessage : " + (messageNode.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        }
        return;
      }
      this._LOG.warn("Unknown MRService Message : " + (messageNode.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
      return;
    }
    this._LOG.warn("Unknown Message : " + (messageNode.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
    return;
  }

  Notification_prototype._handleNotification_Presence = function (presenceNode) {
    var presenceMucItemNode = presenceNode.selectSingleNode("//jsxnsMucUser:item");
    if (presenceMucItemNode) return;
    var presence = new Object;
    // looking for attribute "type"  if type==unavailable then 
    // state is false otherwise its true. If false then logout wont be issued
    var presenceType = presenceNode.getAttribute("type");
    if (presenceType == "unavailable") {
      presence.State = false;
    } else {
      presence.State = true;
    }
    presence.JID = presenceNode.getAttribute("from");
    this._LOG.info("PRESENCE: " + presence.State + ": JID: " + presence.JID);
    presence.subject = Notification.MSG_PRESENCE;
    this.publish(presence);
    return;
  }

  Notification_prototype._handleNotification_Shutdown = function (shutdownNode) {
    this._LOG.info("SHUTDOWN");
    //###########################
    //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code
    //Once MW completes then uncomment the following code
    // #########################
    //this.clsApp.getDescendantsOfType("com.ipvs.ipv.MainPanel")[0].showConnectionMask(true, true);
    this.clsApp.clsServiceAgent.retryLogin(this.clsApp.getClientType());
    return;
  }

  Notification_prototype._handleNotification_IQ = function (iqNode) {
    var iqType = iqNode.getAttribute("type");
    if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:query") != null) {
      if (iqType == "set" || iqType == "get") {
        //for iq node come from 3rd party with 'SessionStart' to simulate 'BrowseSessions' 'CONNECT'
        if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:SessionStart") != null) {
          this._LOG.info("IQ: Session Service Query: SessionStart");
          //send request to get collaborationConfig
          iqNode.subject = "SessionStartIQNode";
          this.publish(iqNode);
          return;
        }
        //for iq node come from 3rd party with 'SessionStart' to close a player window
        if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:Stop") != null) {
          this._LOG.info("IQ: Session Service Query: Stop");
          //send request to get collaborationConfig
          iqNode.subject = "SessionStopIQNode";
          this.publish(iqNode);
          return;
        }
        //for iq node come from 3rd party with 'Playback'
        if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:Playback") != null) {
          this._LOG.info("IQ: Session Service Query: Playback");
          //send request to get collaborationConfig
          iqNode.subject = Notification.MSG_SESSIONPLAYBACKIQNODE;
          this.publish(iqNode);
          return;
        }
      }
      else {
        //pass session response including error response (type = result / error)
        this._handleNotification_Response(iqNode);
        return;
      }
    }
    this._LOG.warn("Unknown IQ: " + (iqNode.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
    return;
  }

  //agent message event notification
  Notification_prototype._handleNotification_AgentMessage = function (messageNode) {
    var retval = false;
    var agentMessageNode = messageNode.selectSingleNode("//AgentMessage");
    if (agentMessageNode == null) return retval;
    //ownerJID
    var fromJID = agentMessageNode.getAttribute("from");
    //request/response/event 
    var agentMessageType = agentMessageNode.getAttribute("type");
    //agentJID
    var messageJID = agentMessageNode.getAttribute("to");

    //check whether request or not, if it is request then figur out agent from service name
    var serviceName = '', messageAgent = '';
    var agentMessageDataNode = null;
    //serviceName = agentMessageNode.selectSingleNode("//servicename").getValue();
    if (agentMessageType.trim() == 'Request') {
      serviceName = agentMessageNode.selectSingleNode("//servicename").getValue();
      messageAgent = Notification.SERVIC_MAPPING_LIST[serviceName];
      agentMessageDataNode = agentMessageNode.selectSingleNode("//Request");
    }
    else if (agentMessageType.trim() == 'Response') {
      serviceName = agentMessageNode.selectSingleNode("//servicename").getValue();
      messageAgent = agentMessageNode.selectSingleNode("//clientcallback").getValue();
      agentMessageDataNode = agentMessageNode.selectSingleNode("//Response");
    }
    else if (agentMessageType.trim() == 'Event') {
      serviceName = agentMessageNode.selectSingleNode("//header").getAttribute("serviceName");
      messageAgent = Notification.SERVIC_MAPPING_LIST[serviceName];
      agentMessageDataNode = agentMessageNode.selectSingleNode("//Event");
    }
    
    // check if message came from appserver or Room
    // if room then get roomNID and pass to the right handler based on the message agent 
    // approx Room NID format // d9e7795f-317e-4243-82cb-3f40279ee973
    // till we get AppserverJID this is the hack 
    var roomNID = fromJID.substring(0, fromJID.indexOf("@"));
    if (roomNID.match(/-\w\w\w\w-\w\w\w\w-\w\w\w\w-/) == null) {
      roomNID = null;
    }

    // pass message to the right agent
    switch (messageAgent) {
      case "Operator":  //xxxxxx/Operator
        retval = this._handleNotification_OperatorAgentMessage(messageJID, fromJID, roomNID, agentMessageType, agentMessageDataNode, agentMessageNode);
        break;
      case "com.ipvs.mediaroomservice.impl.DestStreamAgent":
        retval = this._handleNotification_DestStreamAgentMessage(messageJID, fromJID, roomNID, agentMessageType, agentMessageDataNode);
        break;
      case "com.ipvs.mediaroomservice.impl.SourceStreamAgent":
        break;
      case "com.ipvs.mediaroomservice.mediastore.MediaStoreAdminClientAgent":
        retval = this._handleNotification_MediaStoreAdminClientAgentMessage(messageJID, fromJID, roomNID, agentMessageType, agentMessageDataNode);
        break;
      case "com.ipvs.common.service.XAuthClientAgent":
        break;
      case "com.ipvs.common.service.XAuthServerAgent":
        break;
      case "com.ipvs.common.service.DeviceAdminClientAgent":
      case "com.ipvs.client.MRClientAgent":
      case "com.ipvs.app.IPVSClientApplet":
        // stripping notification header and call handleresponse with the response node
        agentMessageDataNode = agentMessageNode.selectSingleNode(".//Response");
        if (agentMessageDataNode != null) {
          this._handleNotification_Response(agentMessageDataNode);
          retval = true;
        }
        break;
      case "com.ipvs.common.service.DeviceAdminServerAgent":
        retval = this._handleNotification_DeviceAdminServerAgent(messageJID, fromJID, agentMessageType, agentMessageDataNode);
        break;
      case "com.ipvs.mediaroomservice.impl.AppServerAgent":
        retval = this._handleNotification_AppServerAgentMessage(messageJID, fromJID, roomNID, agentMessageType, agentMessageDataNode);
        break;
      case "com.ipvs.systemservice.impl.FileTransferAgent":
        retval = this._handleNotification_FileTransferAgentMessage(messageJID, fromJID, roomNID, agentMessageType, agentMessageDataNode);
        break;
      default:
        break;
    }
    return retval;
  }

  //froeign agent message event notification
  Notification_prototype._handleNotification_ForeignAgentMessage = function (fromJID, roomNID, foreignAgent, messageType, messageDataNode) {
    var retval = false;
    if (messageDataNode == null) return retval;

    var requestAction = messageDataNode.selectSingleNode("//action").getValue();
    this._LOG.info("Message: MRService: Agent Message: Foreign Agent: " + foreignAgent + ": " + messageType + ": " + requestAction);
    retval = true;

    return retval;
  }

  //operator agent message event notification
  Notification_prototype._handleNotification_OperatorAgentMessage = function (toJID, fromJID, roomNID, messageType, messageDataNode, messageNode) {
    var retval = false;
    if (messageDataNode == null) return retval;

    if (messageType == "Request") {
      var reqName = messageDataNode.selectSingleNode("//requestname").getValue();
      if (reqName == "ImportFilePackRequest" || reqName == "ImportCleanupMediaFileRequest") {
        var importFileEvent = new Object;
        importFileEvent.reqName = reqName;
        importFileEvent.messageNode = messageNode;
        this._LOG.info("Message: MRService: Agent Message: Operator Agent: " + messageType + ": " + reqName);
        importFileEvent.subject = Notification.MSG_IMPORTFILEEVENT;
        this.publish(importFileEvent);
        retval = true;
      }
    }
    else if (messageType == "Response") {
      // room response
      if (roomNID != null) {
        var roomevent = new Object;
        roomevent.ToJID = toJID;
        roomevent.RoomNID = roomNID;
        roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
        switch (roomevent.Action) {
          case "NotifyResponse":
            roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
            if (roomevent.StreamNID != null) roomevent.StreamNID = roomevent.StreamNID.getValue();
            roomevent.StatusCode = messageDataNode.selectSingleNode("//status");
            if (roomevent.StatusCode != null) roomevent.StatusCode = roomevent.StatusCode.getValue();
            if (roomevent.StatusCode < 200) {
              roomevent.EventCode = roomevent.StatusCode;
              roomevent.StatusCode = 200;
              roomevent.ErrCode = null;
            }
            else if (roomevent.StatusCode == 200) {
              roomevent.EventCode = null;
              roomevent.ErrCode = null;
            }
            else if (roomevent.StatusCode > 200) {
              roomevent.ErrCode = roomevent.StatusCode;
              roomevent.StatusCode = 0;
              roomevent.EventCode = null;
            }
            else {
              return retval;
            }
            this._LOG.info("Message: MRService: Agent Message: Operator Agent: " + messageType + ": " + roomevent.Action +
                            ": ToJID: " + roomevent.ToJID +
                            ": RoomNID: " + roomevent.RoomNID +
                            ": StreamNID: " + roomevent.StreamNID +
                            ": StatusCode: " + roomevent.StatusCode +
                            ": EventCode: " + roomevent.EventCode +
                            ": ErrCode: " + roomevent.ErrCode);
            roomevent.subject = Notification.MSG_ROOMEVENT;
            this.publish(roomevent);
            retval = true;
            break;
          default:
            break;
        }
      }
    }
    else if (messageType == "Event") {
      // room agent event
      if (roomNID != null) {
        var roomevent = new Object;
        roomevent.ToJID = toJID;
        roomevent.RoomNID = roomNID;
        roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
        this._LOG.info("Message: MRService: Agent Message: Operator Agent: " + messageType + ": " + roomevent.Action +
                        ": ToJID: " + roomevent.ToJID +
                        ": RoomNID: " + roomevent.RoomNID +
                        ": StreamNID: " + roomevent.StreamNID +
                        ": StatusCode: " + roomevent.StatusCode +
                        ": EventCode: " + roomevent.EventCode +
                        ": ErrCode: " + roomevent.ErrCode);
        retval = true;
      }
      //non room agent event
      else {
        //put Recycle Bin code 
        var agentevent = new Object;
        agentevent.Action = messageDataNode.selectSingleNode("//action").getValue();
        switch (agentevent.Action) {
          case "RecycleBinProcess":
            agentevent.eventCode = messageDataNode.selectSingleNode("//eventCode");
            if (agentevent.eventCode != null) agentevent.eventCode = agentevent.eventCode.getValue();
            this._LOG.info("Message: MRService: Agent Message: Operator Agent: " + messageType + ": " + agentevent.Action +
                      ": EventCode: " + agentevent.eventCode);
            agentevent.subject = Notification.MSG_AGENTEVENT;
            this.publish(agentevent);
            retval = true;
            break;
          case "EmptyDirProcess":
            agentevent.eventCode = messageDataNode.selectSingleNode("//eventCode");
            if (agentevent.eventCode != null) agentevent.eventCode = agentevent.eventCode.getValue();
            agentevent.mediadirnid = messageDataNode.selectSingleNode("//MediaStreamDirNID");
            if (agentevent.mediadirnid != null) agentevent.mediadirnid = agentevent.mediadirnid.getValue();
            this._LOG.info("Message: MRService: Agent Message: Operator Agent: " + messageType + ": " + agentevent.Action +
                      ": EventCode: " + agentevent.eventCode);
            agentevent.subject = Notification.MSG_AGENTEVENT;
            this.publish(agentevent);
            retval = true;
            break;
          default:
            break;
        }
      }
    }
    return retval;
  }

  //app server agent message event notification
  Notification_prototype._handleNotification_AppServerAgentMessage = function (toJID, fromJID, roomNID, messageType, messageDataNode) {
    var retval = false;
    if (messageDataNode == null) return retval;

    if (messageType == "Event") {
      if (roomNID != null) {
        var StreamStatusEventNode = messageDataNode.selectSingleNode("//AgentEvent//StreamStatusEventData");
        if (StreamStatusEventNode != null) {
          var roomevent = new Object;
          roomevent.RoomNID = StreamStatusEventNode.getAttribute("roomNID");
          roomevent.StreamNID = StreamStatusEventNode.getAttribute("streamNID");

          roomevent.Action = messageDataNode.selectSingleNode("//eventname");
          if (roomevent.Action != null) roomevent.Action = roomevent.Action.getValue();

          roomevent.EventCode = StreamStatusEventNode.selectSingleNode("//state");
          if (roomevent.EventCode != null) roomevent.EventCode = roomevent.EventCode.getValue();

          roomevent.StatusCode = messageDataNode.selectSingleNode("//eventLevel");
          if (roomevent.StatusCode != null) roomevent.StatusCode = roomevent.StatusCode.getValue();
          if (roomevent.StatusCode == 0) {
            roomevent.ErrCode = roomevent.StatusCode;
          }
          else {
            roomevent.ErrCode = null;
          }
          this._LOG.info("Message: MRService: Agent Message: AppServer Agent: Stream Status Event: " + messageType + ": " + roomevent.Action +
                          ": ToJID: " + roomevent.ToJID +
                          ": RoomNID: " + roomevent.RoomNID +
                          ": StreamNID: " + roomevent.StreamNID +
                          ": StatusCode: " + roomevent.StatusCode +
                          ": EventCode: " + roomevent.EventCode +
                          ": ErrCode: " + roomevent.ErrCode);
          roomevent.subject = Notification.MSG_ROOMEVENT;
          this.publish(roomevent);
          retval = true;
        }
      }
    }
    return retval;
  }

  //dest stream agent message event notification
  Notification_prototype._handleNotification_DestStreamAgentMessage = function (toJID, fromJID, roomNID, messageType, messageDataNode) {
    var retval = false;
    if (messageDataNode == null) return retval;

    if (messageType == "Request") {
      if (roomNID != null) {
        var roomevent = new Object;
        roomevent.ToJID = toJID;
        roomevent.RoomNID = roomNID;

        roomevent.Action = messageDataNode.selectSingleNode("//action");
        if (roomevent.Action != null) roomevent.Action = roomevent.Action.getValue();
        roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
        if (roomevent.StreamNID != null) roomevent.StreamNID = roomevent.StreamNID.getValue();
        this._LOG.info("Message: MRService: Agent Message: Dest Stream Agent: " + messageType + ": " + roomevent.Action +
                        ": ToJID: " + roomevent.ToJID +
                        ": RoomNID: " + roomevent.RoomNID +
                        ": StreamNID: " + roomevent.StreamNID);
        retval = true;
      }
    }
    else if (messageType == "Response") {
      if (roomNID != null) {
        var roomevent = new Object;
        roomevent.ToJID = toJID;
        roomevent.RoomNID = roomNID;

        roomevent.Action = messageDataNode.selectSingleNode("//useraction");
        if (roomevent.Action != null) roomevent.Action = roomevent.Action.getValue();
        roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
        if (roomevent.StreamNID != null) roomevent.StreamNID = roomevent.StreamNID.getValue();
        roomevent.StreamURL = messageDataNode.selectSingleNode("//streamURL");
        if (roomevent.StreamURL != null) roomevent.StreamURL = roomevent.StreamURL.getValue();
        roomevent.Offset = messageDataNode.selectSingleNode("//actiondata");
        if (roomevent.Offset != null) roomevent.Offset = roomevent.Offset.getValue();
        roomevent.StatusCode = messageDataNode.selectSingleNode("//status");
        if (roomevent.StatusCode != null) roomevent.StatusCode = roomevent.StatusCode.getValue();
        roomevent.EventCode = null;
        if (roomevent.StatusCode == "0") {
          roomevent.ErrCode = messageDataNode.selectSingleNode("//log/Code");
          if (roomevent.ErrCode != null) roomevent.ErrCode = roomevent.ErrCode.getValue();
        }
        else {
          roomevent.ErrCode = null;
        }
        this._LOG.info("Message: MRService: Agent Message: Dest Stream Agent: " + messageType + ": " + roomevent.Action +
                        ": ToJID: " + roomevent.ToJID +
                        ": RoomNID: " + roomevent.RoomNID +
                        ": StreamNID: " + roomevent.StreamNID +
                        ": StatusCode: " + roomevent.StatusCode +
                        ": EventCode: " + roomevent.EventCode +
                        ": ErrCode: " + roomevent.ErrCode +
                        ": Offset: " + roomevent.Offset +
                        ": StreamURL: " + roomevent.StreamURL);
        switch (roomevent.Action) {
          case "Start":
          case "Pause":
          case "Resume":
          case "ResumeFromMediaTC":
          case "ResumeFrom":
          case "SkipBackward":
          case "SkipForward":
          case "Catchup":
          case "Stop":
            roomevent.subject = Notification.MSG_ROOMEVENT;
            this.publish(roomevent);
            retval = true;
            break;
          default:
            break;
        }
      }
    }
    else if (messageType == "Event") {
      if (roomNID != null) {
        var roomevent = new Object;
        roomevent.ToJID = toJID;
        roomevent.RoomNID = roomNID;
        //skip new RelayEvent which doesn't have 'action' node
        var actionNode = messageDataNode.selectSingleNode("//action");
        if (actionNode != null) {
          roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
          switch (roomevent.Action) {
            case "SourceEvent":
            case "DestEvent":
              //TBD - this needs to be more specific - there are many streamNIDs in this message
              roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
              if (roomevent.StreamNID != null) roomevent.StreamNID = roomevent.StreamNID.getValue();
              roomevent.StatusCode = messageDataNode.selectSingleNode("//status");
              if (roomevent.StatusCode != null) roomevent.StatusCode = roomevent.StatusCode.getValue();
              roomevent.EventCode = messageDataNode.selectSingleNode("//eventCode");
              if (roomevent.EventCode != null) roomevent.EventCode = roomevent.EventCode.getValue();
              if (roomevent.Action == "SourceEvent") {
                roomevent.ActionData = messageDataNode.selectSingleNode("//sourceEventActionData");
              }
              else if (roomevent.Action == "DestEvent") {
                roomevent.ActionData = messageDataNode.selectSingleNode("//destinationEventActionData");
              }
              if (roomevent.StatusCode == "0") {
                roomevent.ErrCode = messageDataNode.selectSingleNode("//log/Code");
                if (roomevent.ErrCode != null) roomevent.ErrCode = roomevent.ErrCode.getValue();
              }
              else {
                roomevent.ErrCode = null;
              }
              if (roomevent.EventCode == "112" ||
                  roomevent.EventCode == "113" ||
                  roomevent.EventCode == "114" ||
                  roomevent.EventCode == "115" ||
                  roomevent.EventCode == "100" ||
                  roomevent.EventCode == "104" ||
                  roomevent.EventCode == "204" ||
                  roomevent.EventCode == "2300") {
                roomevent.StatusCode = 0;
                roomevent.ErrCode = roomevent.EventCode;
              }
              this._LOG.info("Message: MRService: Agent Message: Dest Stream Agent: " + messageType + ": " + roomevent.Action +
                              ": ToJID: " + roomevent.ToJID +
                              ": RoomNID: " + roomevent.RoomNID +
                              ": StreamNID: " + roomevent.StreamNID +
                              ": StatusCode: " + roomevent.StatusCode +
                              ": EventCode: " + roomevent.EventCode +
                              ": ErrCode: " + roomevent.ErrCode);
              roomevent.subject = Notification.MSG_ROOMEVENT;
              this.publish(roomevent);
              retval = true;
              break;
            default:
              break;
          }
        }
      }
    }
    return retval;
  }

  //media stream admin client agent message notification
  Notification_prototype._handleNotification_MediaStoreAdminClientAgentMessage = function (toJID, fromJID, roomNID, messageType, messageDataNode) {
    var retval = false;
    if (messageDataNode == null) return retval;

    if (messageType == "Request" || messageType == "MediaStoreAdminResponseMessage") {
      if (roomNID != null) {
        var roomevent = new Object;
        roomevent.ToJID = toJID;
        roomevent.RoomNID = roomNID;

        //messageType ==MediaStoreAdminResponseMessage
        roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
        switch (roomevent.Action) {
          case "CreateFile":
          case "DeleteFile":
          case "UpdateFile":
            roomevent.FileNID = messageDataNode.selectSingleNode("//fileNID").getValue();
            this._LOG.info("Message: MRService: Agent Message: MediaStoreAdminClient Agent: " + messageType + ": " + roomevent.Action +
                            ": ToJID: " + roomevent.ToJID +
                            ": FileNID: " + roomevent.FileNID);
            retval = true;
            //TBD - need to do something in error case?
            break;
          default:
            break;
        }
      }
    }
    return retval;
  }

  //To handle FileXfer related notifications
  Notification_prototype._handleNotification_FileTransferAgentMessage = function (toJID, fromJID, roomNID, messageType, messageDataNode) {
    var retval = false;
    if (messageDataNode == null) return retval;

    if (messageType == "Request") {
      //TBD ???
      retval = true;
    }
    else if (messageType == "Response") {
      //TBD ???
      retval = true;
    }
    else if (messageType == "Event") {
      var filexfervent = new Object;
      filexfervent.ToJID = toJID;
      //filexfervent.RoomNID = roomNID;
      var FileXferEventDataNode = messageDataNode.selectSingleNode("//FileXferEventData");
      if (FileXferEventDataNode != null) {
        filexfervent.Action = "FileXferEvent";
        filexfervent.FileXferID = FileXferEventDataNode.selectSingleNode("//FileXferID");
        if (filexfervent.FileXferID != null) filexfervent.FileXferID = filexfervent.FileXferID.getValue();

        filexfervent.PercentageComplete = FileXferEventDataNode.selectSingleNode("//percentageComplete");
        if (filexfervent.PercentageComplete != null) filexfervent.PercentageComplete = filexfervent.PercentageComplete.getValue();

        filexfervent.State = FileXferEventDataNode.selectSingleNode("//state");
        if (filexfervent.State != null) filexfervent.State = filexfervent.State.getValue();

        filexfervent.eventCode = FileXferEventDataNode.selectSingleNode("//eventCode");
        if (filexfervent.eventCode != null) filexfervent.eventCode = filexfervent.eventCode.getValue();

        this._LOG.info("Message: MRService: Agent Message: FileXfer Agent: " + messageType + ": FileXferEventData : EventCode: " + filexfervent.eventCode);
        filexfervent.subject = Notification.MSG_FILEXFEREVENT;
        this.publish(filexfervent);
        retval = true;
      }
    }
    return retval;
  }

  //Windowing Notification forwarded to the UnManned Client
  Notification_prototype._handleNotification_DeviceAdminServerAgent = function (messageJID, fromJID, agentMessageType, agentMessageDataNode) {
    var retval = false;
    if (agentMessageDataNode == null) return retval;
    if (agentMessageType == "Request") {
      var requestName = agentMessageDataNode.selectSingleNode(".//Request/header/requestname");
      var nodeDeviceConfigRequestData = agentMessageDataNode.selectSingleNode(".//Request/data/DeviceConfigurationRequestData");
      if (nodeDeviceConfigRequestData == null) return retval;
      var nodeDisplayConfig = nodeDeviceConfigRequestData.selectSingleNode(".//DeviceConfig/AVCodecElementsConfig/IPVSClientDisplayConfig");
      if (nodeDisplayConfig != null) {
        // Forward to Display Agent of UnManned client.
        this._LOG.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: DisplayConfigUpdate: " + (nodeDisplayConfig.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        nodeDisplayConfig.subject = "DisplayConfigUpdate";
        this.publish(nodeDisplayConfig);
        retval = true;
      }
      var nodePortWindowContent = nodeDeviceConfigRequestData.selectSingleNode(".//SetPortWindowContent");
      if (nodePortWindowContent != null) {
        // Forward to Display Agent of UnManned client.
        this._LOG.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: WindowContentUpdate: " + (nodePortWindowContent.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        nodePortWindowContent.subject = "WindowContentUpdate";
        this.publish(nodePortWindowContent);
        retval = true;
      }
      var nodePortWindowVisible = nodeDeviceConfigRequestData.selectSingleNode(".//SetPortWindowVisible");
      if (nodePortWindowVisible != null) {
        // Forward to Display Agent of UnManned client.
        this._LOG.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: WindowVisibleUpdate: " + (nodePortWindowVisible.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        nodePortWindowVisible.subject = "WindowVisibleUpdate";
        this.publish(nodePortWindowVisible);
        retval = true;
      }
      //unwrap and notify 'SetPortWindowProperties'
      var nodePortWindowProperties = nodeDeviceConfigRequestData.selectSingleNode(".//SetPortWindowProperties");
      if (nodePortWindowProperties != null) {
        // Forward to Display Agent of UnManned client.
        this._LOG.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message:WindowPropertiesUpdate: " + (nodePortWindowProperties.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        nodePortWindowProperties.subject = "WindowPropertiesUpdate";
        this.publish(nodePortWindowProperties);
        retval = true;
      }
    }
    return retval;
  }

})