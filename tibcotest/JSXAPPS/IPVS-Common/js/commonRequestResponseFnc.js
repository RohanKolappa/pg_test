// obsolete - not used anymore
/*
var tRequest = "Request";

function commonRequestResponse_Init() {
  topic_Create(tRequest);
  topic_Set(tRequest, "RequestID", 1);
}
function commonRequestResponse_Final() {
  topic_Destroy(tRequest);
}
*/
/*
  common response handler -
    - for Response
      - uses clientdata to get origianl request-specific topicname
      - puts response data in the topic
      - also puts response data in the cache as needed
    Inputs:
      - nodeResponse = response node from applet notification
      - TopicName = TopicName to use (input during simulation)
*/
// moved to commonNotificationFnc 
/*
function handleResponse(nodeResponse, TopicName) {

  if (nodeResponse.getNodeName() == "Response" ||
      nodeResponse.getNodeName() == "iq") {
    
    // log response string if needed
    if( AppletResponseAlertMode == true ) {
      log.info(nodeResponse.toString());
    }

    // check service version
    var serviceVersion = topic_Get(tApp, "ServiceVersion");
    var serviceVersionResponse = "";
    if (nodeResponse.getNodeName() == "Response") {
      serviceVersionResponse = nodeResponse.getAttribute("serviceVer");
    }
    else if (nodeResponse.getNodeName() == "iq") {
      serviceVersionResponse = nodeResponse.selectSingleNode("./*").getAttribute("serviceVer");
    }
    if (serviceVersionResponse != serviceVersion) {
      log.warn("Service Version mismatch: Response :" + nodeResponse.toString());
    }

    // get request-specific topicname
    if( SimulationMode != true ) {
      if (nodeResponse.getNodeName() == "Response") {
        if(nodeResponse.selectSingleNode(".//header/clientdata") != null) {
          TopicName = nodeResponse.selectSingleNode(".//header/clientdata").getValue();
        }
      }
      else if (nodeResponse.getNodeName() == "iq") {
        TopicName = nodeResponse.getAttribute("id");
      }
    }
    else if( TopicName == null ) {
      // this assumes one request-response at a time
      TopicName = topic_Get(tRequest, "ClientData");
    }
    if( (TopicName == null) || (! topic_Exists(TopicName)) ) {
      //logging the message instead of alert
      log.warn("Internal Error: Unknown Response : " + nodeResponse.toString());
      return;
    }

    // stash the response in the topic
    var topicResponseNode = topic_Set(TopicName, "ResponseNode", null);
    topicResponseNode.appendChild(nodeResponse.cloneNode(true));

    // and fire off a recieive event 
    topic_Publish(TopicName, "RequestResponseState", "RESPONSERECEIVE");
    return;
  }

  log.warn("Unknown Response : " + nodeResponse.toString());
  return;
}
*/

// If you are simulating any request then PLEASE UNCOMENT THIS FUNCTION
/*
function handleResponseXML(ResponseXML, TopicName) {
  // convert to Doc format
  var responseDoc = new jsx3.xml.Document();
  responseDoc.loadXML(ResponseXML);
  responseDoc.setSelectionNamespaces({ 'http://jabber.org/protocol/muc#user': "jsxnsMucUser",
    'http://jabber.org/protocol/muc#admin': "jsxnsMucAdmin",
    'http://jabber.org/protocol/pubsub#event': "jsxnsPubsubEvent",
    'jabber:client': "jsxnsJabberClient",
    'ipvs:mediaroom:ipvsdbservice': "jsxnsIPVSDBService",
    'com.ipvs.session': "jsxnsIPVSSessionService",
    'jabber:iq:roster': "jsxnsJabberRoster"
  });
    
  // clean our excess chars
  ResponseXML = ResponseXML.replace(/<\?.*\?>/,"").replace(/[\n\r]/g," ").replace(/>\s+</g, "><");
  
  if(responseDoc.selectSingleNode("/Response") ||
     responseDoc.selectSingleNode("/iq")) {
    if( AppletResponseAlertMode == true ) {
      log.info(ResponseXML);
    }
    // get request-specific topicname from clientdata
    if( SimulationMode != true ) {
      if( responseDoc.selectSingleNode("//header/clientdata") != null ) {
        var TopicName = responseDoc.selectSingleNode("//header/clientdata").getValue();
      }
      else if (responseDoc.selectSingleNode("/iq")) {
        var TopicName = responseDoc.selectSingleNode("/iq").getAttribute("id");
      }
    }
    else if( TopicName == null ) {
      // this assumes one request-response at a time
      TopicName = topic_Get(tRequest, "ClientData");
    }
    if( (TopicName == null) || (! topic_Exists(TopicName)) ) {
      //logging the message instead of alert
      log.warn("Internal Error: Unknown Response : " + ResponseXML);
      return false;
    }
    // stash the response in the topic 
    var responseNode = responseDoc.getRootNode().cloneNode(true);
    var serviceVersion =topic_Get(tApp, "ServiceVersion");
    var serviceVerAttribute = responseNode.getAttribute("serviceVer");
    if( serviceVerAttribute != serviceVersion ) {
        log.warn("Service Version mismatch: Response :"+ ResponseXML);
    }
    var topicResponseNode = topic_Set(TopicName, "ResponseNode", null);
    topicResponseNode.appendChild(responseNode);
    // and fire off a recieive event 
    topic_Publish(TopicName, "RequestResponseState", "RESPONSERECEIVE");
    return true;
  }
  else if(responseDoc.selectSingleNode("/Notification")){
    if( AppletNotificationAlertMode == true ) {
      log.info(ResponseXML);
    }

//    // handle notification here
//    responseDoc.setSelectionNamespaces({'http://jabber.org/protocol/muc#user':"jsxnsMucUser",
//                                        'http://jabber.org/protocol/muc#admin':"jsxnsMucAdmin",
//                                        'http://jabber.org/protocol/pubsub#event':"jsxnsPubsubEvent",
//                                        'jabber:client':"jsxnsJabberClient",
//                                        'ipvs:mediaroom:ipvsdbservice':"jsxnsIPVSDBService",
//                                        'jabber:iq:roster':"jsxnsJabberRoster" } );

    var NotificationType = responseDoc.selectSingleNode("/Notification//notificationtype").getValue();
    var notificationData = responseDoc.selectSingleNode("/Notification//notificationdata");
    switch (NotificationType) {
      case "AgentMessage":
        if( handleNotification_AgentMessage(notificationData) != true ) {
          log.warn("Unknown AgentMessage : " + ResponseXML);
        }
        break;
      case "AgentEvent":
        if( handleNotification_AgentEvent(notificationData) != true ) {
          log.warn("Unknown AgentEvent : " + ResponseXML);
        }
        break;
      case "XMPPEvent":
        if( handleNotification_XMPPEvent(notificationData) != true ) {
          log.warn("Unknown XMPPEvent : " + ResponseXML);
        }
        break
      default:
        log.warn("Unknown Notification : " + ResponseXML);
        break;
    }
  }
  else {
    log.warn("Unknown Applet Message : " + ResponseXML);
    return false;
  }
  return true;
}
*/

/*
function handleNotification_XMPPEvent(notificationData) {
  // check if it is a presence message
  var presenceNode = notificationData.selectSingleNode("//presence");
  if( presenceNode != null ) {
    var presence = new Object;
    // looking for attribute "type"  if type==unavailable then 
    // state is false otherwise its true. If false then logout wont be issued
    var presenceType = presenceNode.getAttribute("type");
    if( presenceType == "unavailable") {
      presence.State = false;    
    } else {
    presence.State = true;
    }
    presence.JID = presenceNode.getAttribute("from");
    // NONE is being used in the cmpBlkLogin.js file
    // if NONE then its regular presence notification
    presence.RoomNID = "NONE"; 
    topic_Publish(tApp, "PRESENCE", presence);
    return true;    
  }
  //check if it is a shutdown message
  var shutDownNode = notificationData.selectSingleNode("//shutdown");
  if( shutDownNode != null ) {
    log.info("Notification: Shutdown");
    IpvsLogin.doLogout();
	  jsx3.sleep( (function(){
                   alert("Session Terminated");
	             }), null, this); 
    return true;
  }
  // check if it is an IQ stanza
  var iqNode = notificationData.selectSingleNode(".//iq");
  // check if it is an IQ stanza
  if (iqNode != null) {
    var iqType = iqNode.getAttribute("type");
//    if( iqNode.selectSingleNode(".//jsxnsJabberRoster:query") != null ) {
//      log.info( "Notification: XMPP Event: Roster Query" );
//    }
//    else if( iqNode.selectSingleNode(".//jsxnsMucAdmin:query") != null ) {
//      log.info( "Notification: XMPP Event: MUC Room Participant Query" );
//    }
//    else 
    if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:query") != null) {
      if (iqType == "set" || iqType == "get") {
        //for iq node come from 3rd party with 'SessionStart' to simulate 'BrowseSessions' 'CONNECT'
        if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:SessionStart") != null) {
          log.info("Notification: XMPP Event: Session Service Query: SessionStart");
          //send request to get collaborationConfig
          topic_Publish(tApp, "SessionStartIQNode", iqNode);
          return true;
        }
        //for iq node come from 3rd party with 'SessionStart' to close a player window
        if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:Stop") != null) {
          log.info("Notification: XMPP Event: Session Service Query: Stop");
          //send request to get collaborationConfig
          topic_Publish(tApp, "SessionStopIQNode", iqNode);
          return true;
        }
        //for iq node come from 3rd party with 'Playback'
        if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:Playback") != null) {
          log.info("Notification: XMPP Event: Session Service Query: Playback");
          //send request to get collaborationConfig
          topic_Publish(tApp, "SessionPlaybackIQNode", iqNode);
          return true;
        }
      }
      else {
        //pass session response including error response (type = result / error)
        handleResponse(iqNode);
        return true;
      }
    }
    return false;
//    else {
//      var iqFrom = iqNode.getAttribute("from");
//      //var RoomNID = iqFrom.substring(0, iqFrom.indexOf("@"));
//      var JID = iqNode.getAttribute("to");
//      //log.info( "Notification: XMPP Event: Result: Registration Request: RoomNID: " + RoomNID + ": JID: " + JID);
//      log.info( "Notification: XMPP Event: Result: Registration Request : JID: " + JID);
//    }
//    return true;
  }
  // check if it is a message - 
  var messageNode = notificationData.selectSingleNode("//message");
  if( messageNode == null ) return false;

  //check if it is session related message
  //sample ConnState notification
//    <Notification>
//      <notificationheader>
//        <notificationtype>XMPPEvent</notificationtype>
//      </notificationheader>
//      <notificationdata>
//        <message id="53OXa-3037" to="srcuser2@localhost/srcpc2" from="d9e0f9ae-3522-457a-9f26-bcaec4d6ddc5@conference.localhost/srcrelay1@localhost" type="groupchat">
//          <x xmlns="com.ipvs.session">
//            <ConnState connId="strmc1" sessionId="recordSessionID" streamURL="v2d://10.1.5.169/port=6062?bandwidth=10240000?avoption=Video?ismulticast=false?enablekbm=false" value="WAITING_FOR_STREAMSTART"/>
//          </x>
//        </message>
//      </notificationdata>
//    </Notification>

  //sample Timeline notification

//  <Notification>
//    <notificationheader>
//      <notificationtype>XMPPEvent</notificationtype>
//    </notificationheader>
//    <notificationdata>
//      <message id="MH1H4-3507" to="srcuser1@localhost/srcpc2" from="6e7132c4-3baa-4e81-b58d-55f3acb46d71@conference.localhost/srcrelay1@localhost" type="groupchat">
//        <x xmlns="com.ipvs.session">
//          <Timeline connGroupId="" connId="" id="" sessionId="tx1_jsx_1l">
//	            <Start Offset="0" WC="1262860808040" parentTimelineId=""/>
//	            <Last Offset="29779" TimeScale="0" WC="1262860837819" requestedAction="Pause"/>
//          </Timeline>
//        </x>
//      </message>
//    </notificationdata>
//  </Notification>

  var xNode = messageNode.selectSingleNode("//jsxnsIPVSSessionService:x");
  if( xNode != null ) {
    //check for ConnectionState msg
    var ConnStateNode = xNode.selectSingleNode("//jsxnsIPVSSessionService:ConnState");
    if( ConnStateNode != null ) {
      // get values from it
      var objConnState = new Object;
      objConnState.toJID = messageNode.getAttribute("to");
      objConnState.sessionId = ConnStateNode.getAttribute("sessionId");
      objConnState.connId = ConnStateNode.getAttribute("connId");
      objConnState.streamURL = ConnStateNode.getAttribute("streamURL");
      objConnState.state = ConnStateNode.getAttribute("value");

      log.info("Notification: ConnState Event: " +
                  ": state: " + objConnState.state + 
                  ": sessionId: " + objConnState.sessionId +
                  ": connId: " + objConnState.connId +
                  ": streamURL: " + objConnState.streamURL);
      // and publish as connStateEvent
      topic_Publish(tApp, "CONNSTATEEVENT", objConnState);
      return true;
    }

    //check for Timeline msg
    var TimelineNode = xNode.selectSingleNode("//jsxnsIPVSSessionService:Timeline");
    if( TimelineNode != null ) {
      // get values from it
      var objTimeLine = new Object;
      objTimeLine.toJID = messageNode.getAttribute("to");
      objTimeLine.sessionId = TimelineNode.getAttribute("sessionId");
      objTimeLine.connGroupId = TimelineNode.getAttribute("connGroupId");
      objTimeLine.connId = TimelineNode.getAttribute("connId");
      objTimeLine.Id = TimelineNode.getAttribute("id");

      var startNode = TimelineNode.selectSingleNode("//jsxnsIPVSSessionService:Start");
      objTimeLine.startOffset = startNode.getAttribute("Offset");
      objTimeLine.startWC = startNode.getAttribute("WC");

      var lastNode = TimelineNode.selectSingleNode("//jsxnsIPVSSessionService:Last");
      objTimeLine.lastOffset = lastNode.getAttribute("Offset");
      objTimeLine.lastWC = lastNode.getAttribute("WC");
      objTimeLine.timeScale = lastNode.getAttribute("TimeScale");
      objTimeLine.lastAction = lastNode.getAttribute("requestedAction");
      
      log.info( "Notification: TimeLine Event: " +
                  ": sessionId: " + objTimeLine.sessionId +
                  ": connId: " + objTimeLine.connId +
                  ": Id: " + objTimeLine.Id +
                  ": connGroupId: " + objTimeLine.connGroupId + 
                  ": startOffset: "  + objTimeLine.startOffset +
                  ": startWC: " + objTimeLine.startWC + 
                  ": lastOffset: "  + objTimeLine.lastOffset + 
                  ": lastWC: "  + objTimeLine.lastWC + 
                  ": timeScale: "  + objTimeLine.timeScale + 
                  ": lastAction: "  + objTimeLine.lastAction);
      // and publish as TimeLineEvent
      topic_Publish(tApp, "TIMELINEEVENT", objTimeLine);
      return true;
    }
  }

  // check if it is a DB update event
  var updateEventNode = messageNode.selectSingleNode("//jsxnsIPVSDBService:updateEvent");
  if( updateEventNode != null ) {
    var nodeID = updateEventNode.getAttribute("nodeID");
    if( nodeID == null || nodeID == "" ) return false;
    var eventType = updateEventNode.getAttribute("type");
    var eventChildXPath = updateEventNode.getAttribute("childXPath");
    var objType = updateEventNode.getAttribute("objectType");
    var objectCacheNode = topic_GetNIDNode(tCache, nodeID);
    var objType = updateEventNode.getAttribute("objectType");
    var messageID = messageNode.getAttribute("id");
    var parentNID = messageID.replace(/__.*$/, "");
    if( eventType == "add" ) {
      // NEWDB: this is an add
      var parentCacheNode = topic_GetNIDNode(tCache, parentNID);
      // if parent node is in our cache
      if( parentCacheNode != null ) {
        // just add to the dirty NID list - will be handled by cacheSync
        log.info( "Notification: IPVSDB UpdateEvent: Node Add: Dirty NID Parent: " + parentNID );
        arrDirtyCacheNID[parentNID] = newServerDate();
        // also add the node placeholder into the cache - and mark the child node as dirty
        // this is for cases where there is no list request 
        // and we want to handle updates on a per node basis instead of getting the whole list agian
        objectCacheNode = parentCacheNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Node");
        objectCacheNode.setAttribute("NID", nodeID);
        parentCacheNode.appendChild(objectCacheNode);
        log.info("Notification: IPVSDB UpdateEvent: Node Add: Dirty NID: " + nodeID);
        arrDirtyCacheNID[nodeID] = newServerDate();
        return true;
      }
      else if( (eventType == "add") && (!jsx3.util.strEmpty(eventChildXPath)) ){
        var objEvent = new Object;
        objEvent.objType = objType;
        objEvent.nodeID = nodeID;
        objEvent.type = "Update";
        topic_Publish(tApp,"DBUpdate",objEvent);        
      }
      else {
        //do nothing
        log.warn( "Notification: IPVSDB UpdateEvent: Node Add/Update: Unknown NID: " + nodeID + " ParentNID: " + parentNID );
        return true;
      }
    }
    //check for service resource update 
    if( eventType == "replace"  && parentNID == "default.serviceresourcelist" ) {
      //Logout
      log.info( "Notification: Application Server Reset" );
      IpvsLogin.doLogout();
      jsx3.sleep( (function(){
        alert("Application Server Reset: Please Login again to continue");
      }), null, this); 
      return true;
    }
    else if( (eventType == "replace") && (jsx3.util.strEmpty(eventChildXPath)) ) {
      var objEvent = new Object;
      objEvent.objType = objType;
      objEvent.nodeID = nodeID;
      objEvent.type = "Replace";
      topic_Publish(tApp,"DBUpdate",objEvent);      
    }
    else if( eventType == "replace"  && objectCacheNode != null ) {
      // just add to the dirty NID list - will be handled by cacheSync
      log.info( "Notification: IPVSDB PubSub Event: Node Replace: Dirty NID: " + nodeID );
      arrDirtyCacheNID[nodeID] = newServerDate();
      return true;
    }
    else if ((eventType == "delete") && (!jsx3.util.strEmpty(eventChildXPath))) {
      var objEvent = new Object;
      objEvent.objType = objType;
      objEvent.nodeID = nodeID;
      objEvent.type = "Delete";
      topic_Publish(tApp, "DBUpdate", objEvent);
    }
    else if( eventType == "delete" && objectCacheNode != null ) {
      log.info( "Notification: IPVSDB UpdateEvent: Node Delete: Dirty NID: " + nodeID );
      arrDirtyCacheNID[nodeID] = newServerDate();
      // also set a "DELETED" attribute in the cache node to assist in cachesync processing
      objectCacheNode.setAttribute("DBSTATE", "DELETED");
      return true;
    }
    else {
      log.warn( "Notification: IPVSDB UpdateEvent: EventType: " + eventType +" : Unknown NID: " + nodeID );
      return true;
    }
  }

  // unknown message - if we get here
  return false;
}

function handleNotification_AgentEvent(messageNode) {
  var retval = false;
  // check if it is a presence message
  var presenceNode = messageNode.selectSingleNode("//presence");
  if( presenceNode != null ) {
    var presence = new Object;
    var PresenceFrom = presenceNode.getAttribute("from");
    presence.RoomNID = PresenceFrom.substring(0, PresenceFrom.indexOf("@"));
    presence.JID = presenceNode.selectSingleNode("//jsxnsMucUser:item").getAttribute("jid");
    var PresenceType = presenceNode.getAttribute("type");
    if( PresenceType == "unavailable" ) {
      presence.State = false;
      //TBD - handle other unavailable presences here
      var Destroy = presenceNode.selectSingleNode("//jsxnsMucUser:destroy");
      if (Destroy != null) {
        presence.JID = "";
        log.info("Notification: Agent Event: PRESENCE DESTROY: RoomNID: " + presence.RoomNID);
      }
      else {
        log.info("Notification: Agent Event: PRESENCE OFF: RoomNID: " + presence.RoomNID + ": " + presence.JID);
      }
    }
    else {
      //TBD - handle room start presence here?
      presence.State = true;
      log.info( "Notification: Agent Event: PRESENCE ON: RoomNID: " + presence.RoomNID + ": " + presence.JID);
    }
    // update room presence in tApp
    var nodeRoomPresenceList = topic_GetNode(tApp, "//RoomPresenceList");
    if (nodeRoomPresenceList == null) {
      nodeRoomPresenceList = topic_Set(tApp, "RoomPresenceList", "");
    }
    if( presence.RoomNID != "" ) {
      var nodeRoomPresence = nodeRoomPresenceList.selectSingleNode("./RoomPresence[@RoomNID='" + presence.RoomNID + "']");
      if (presence.JID != "") {
        if (nodeRoomPresence == null) {
          nodeRoomPresence = nodeRoomPresenceList.createNode(jsx3.xml.Entity.TYPEELEMENT, "RoomPresence");
          nodeRoomPresence.setAttribute("RoomNID", presence.RoomNID);
          nodeRoomPresenceList.appendChild(nodeRoomPresence);
        }
        var presenceUserName = presence.JID;
        var nodeRoomJIDPresence = nodeRoomPresence.selectSingleNode("./JID='" + presenceUserName + "'");
        if (presence.State == true) {
          if (nodeRoomJIDPresence == null) {
            nodeRoomJIDPresence = nodeRoomPresence.createNode(jsx3.xml.Entity.TYPEELEMENT, "JID");
            nodeRoomJIDPresence.setValue(presenceUserName);
            nodeRoomPresence.appendChild(nodeRoomJIDPresence);
          }
        }
        else {
          if (nodeRoomJIDPresence != null) {
            nodeRoomPresence.removeChild(nodeRoomJIDPresence);
          }
        }
      }
      else {
        if (nodeRoomPresence != null) {
          nodeRoomPresenceList.removeChild(nodeRoomPresence);
        }
      }
    }
    topic_Publish(tApp, "PRESENCE", presence);
    retval = true;
  } 
  return retval;
}

*/