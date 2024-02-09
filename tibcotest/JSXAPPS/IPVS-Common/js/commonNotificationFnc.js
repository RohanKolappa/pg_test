// entry point for handling all XML from applet.
// Typically everything should be in a 'Notification' tag
function handleAppletXML(strAppletXML) {
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
  strAppletXML = strAppletXML.replace(/[\n\r]/g," ").replace(/>\s+</g, "><");

  // ==================== check if it is a legacy 'Response' (typically will come inside IQ/Message)
  var responseNode = docResponse.selectSingleNode("/Response");
  if (responseNode != null) {
    handleNotification_Response(responseNode);
    return;
  }
  // ==================== check if it is a StreamingServer 'SS' cmd
  var ssCmdNode = docResponse.selectSingleNode("/SS");
  if (ssCmdNode != null) {
    handleNotification_SSCmd(ssCmdNode);
    return;
  }
  // ==================== check if it is an 'iq' or 'notification/iq'
  var iqNode = docResponse.selectSingleNode("/iq");
  if (iqNode == null) iqNode = docResponse.selectSingleNode(".//notificationdata/iq");
  if (iqNode != null) {
    handleNotification_IQ(iqNode);
    return;
  }
  // ==================== check if it is a 'message' or 'notification/message'
  var messageNode = docResponse.selectSingleNode("/message");
  if (messageNode == null) messageNode = docResponse.selectSingleNode(".//notificationdata/message");
  if (messageNode != null) {
    handleNotification_Message(messageNode);
    return;
  }
  // ================== check if it is a 'presence' or 'notification/presence'
  var presenceNode = docResponse.selectSingleNode("/presence");
  if (presenceNode == null) presenceNode = docResponse.selectSingleNode(".//notificationdata/presence");
  if( presenceNode != null ) {
    handleNotification_Presence(presenceNode);
    return;
  }
  // ================== check if it is a 'xmpperror' or 'notification/shutdown'
  var shutdownNode = docResponse.selectSingleNode("/xmpperror");
  if (shutdownNode == null) shutdownNode = docResponse.selectSingleNode(".//notificationdata/shutdown");
  if (shutdownNode != null) {
    handleNotification_Shutdown(shutdownNode);
    return;
  }
  // ============== log unknown packet
  log.warn("Unknown XMPP Packet : " + strAppletXML);
  return;  
}

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
function handleNotification_Response(nodeResponse, TopicName) {

  if (nodeResponse.getNodeName() == "Response" ||
      nodeResponse.getNodeName() == "iq") {
    
    // log response string if needed
    if( AppletResponseAlertMode == true ) {
      log.info((nodeResponse.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><")); 
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
      log.warn("Service Version mismatch: Response :" + (nodeResponse.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
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
    if( (TopicName == null) || (! topic_Exists(TopicName)) ) {
      //logging the message instead of alert
      log.warn("Internal Error: Unknown Response : " + (nodeResponse.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
      return;
    }

    // stash the response in the topic
    var topicResponseNode = topic_Set(TopicName, "ResponseNode", null);
    topicResponseNode.appendChild(nodeResponse.cloneNode(true));

    // and fire off a recieive event 
    topic_Publish(TopicName, "RequestResponseState", "RESPONSERECEIVE");
    return;
  }

  log.warn("Unknown Response : " + (nodeResponse.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
  return;
}

// Streaming Server 'SS' Cmd handler
// - parse SS Cmds and publish for use by DstAgent/Players
/*
<SS>sjain@localhost/sjainpc:action=add id=2002 operation=output streamtype=V2D dsttype=V2DHardWarePort ioport=Session_C484236D-C8A0-0001-4FB52450446A3AF0.Conn_C484236D-C8A0-0001-3F5E11001F205730 </SS> 
<SS>sjain@localhost/sjainpc:action=start id=2002 queueid=2001 operation=output dsttype=V2DHardWarePort </SS> 
<SS>sjain@localhost/sjainpc:action=delete id=2001 operation=input streamtype=V2D srctype=V2DNetworkPort </SS> 
<SS>sjain@localhost/sjainpc:action=add id=2001 operation=input streamtype=V2D srctype=V2DNetworkPort srcip=10.1.5.167 srcport=6063 srcbitrate=20480000 srcmulticast=0 avoption=1 videoport=6060 audioport=6061  streamURL='v2d://10.1.5.167/port=6063?bandwidth=20480000?avoption=Video?ismulticast=false?enablekbm=false' </SS> 
<SS>sjain@localhost/sjainpc:action=start id=2001 queueid=2001 operation=input srctype=V2DNetworkPort </SS> 
<SS>sjain@localhost/sjainpc:action=delete id=2002 operation=output streamtype=V2D dsttype=V2DHardWarePort </SS> 
<SS>sjain@localhost/sjainpc:action=delete id=2001 operation=input streamtype=V2D srctype=V2DNetworkPort </SS> 
<SS>sunuser3@localhost/sunpc3:action=add id=2012 operation=output streamtype=V2D dsttype=V2DHardWarePort ioport=space problem_C48C55AE-BCD0-0001-38A0124F1CA01287.Conn_C48BD0F1-02C0-0001-57B31E5711C76B20 </SS>
action=add id=2001 operation=input streamtype=V2D srctype=V2DNetworkPort srcip=10.1.5.167 srcport=6064 srcbitrate=20480000 srcmulticast=0 avoption=1 videoport=6060 audioport=6061  streamURL='v2d://10.1.5.167/port=6064?bandwidth=20480000?avoption=Video?ismulticast=false?enablekbm=false' windowId='Conn_C48BD0F1-02C0-0001-57B31E5711C76B20' canvasId='space problem_C48C55AE-BCD0-0001-38A0124F1CA01287'
*/
function handleNotification_SSCmd(nodeSSCmd) {
  // get the cmd string
  var strSSCmd = nodeSSCmd.getValue();
  log.info("SSCmd: " + strSSCmd);
  // parse it into object form
  var objSSCmd = new Object();
  // strip off the JID
  strSSCmd = strSSCmd.substring(strSSCmd.indexOf(':') + 1);

  // get token array with "'" 
  var arrSSCmd1 = strSSCmd.match(/\w+=\'[^\'].\'/g);
  addStrArrToObjSSCmd(arrSSCmd1, objSSCmd);
  // remove them from original str
  var strSSCmd = strSSCmd.replace(/\w+=\'[^\'].\'/g, '');
  // get token array without "'"
  var arrSSCmd2 = strSSCmd.match(/\w+=\S+/g);
  addStrArrToObjSSCmd(arrSSCmd2, objSSCmd);
  // publish the object
  topic_Publish(tApp, "SSCmd", objSSCmd);
  return;
}

//help function for handleNotification_SSCmd
function addStrArrToObjSSCmd(arrSSCmd, objSSCmd) {
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
//shut down function
function handleNotification_Shutdown(shutdownNode) {
  log.info("SHUTDOWN");
  //TBD:Need to write common code for both web and installed case.
  // INSTALLED:
  // <Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader>
  //   <notificationdata>
  //     <shutdown/>
  //   </notificationdata>
  // </Notification>
  // WEB:
  // <xmpperror code= "system-shutdown" or "conflict" or "AppServerNotFound" />
//    if (clientType != "WEB") {
//      topic_Publish(tApp, "LoginState", "LOGOUT");
//    }
//    else{
//      topic_Publish(tApp, "LogoutState", "VALID");
  //    }

 //###########################
 //As MW implementation for reliability is not completed instead of reverting UI changes commenting the related code ###
 //Once MW completes then uncomment the following code
 // topic_Publish(tApp, "SCDown", true);
 // #########################
  IpvsLogin.retryLogin(clientType);
  return;
}
//presence notification
function handleNotification_Presence(presenceNode) {
  //check if it is Muc-presence, if it is not then return
  var presenceMucItemNode = presenceNode.selectSingleNode("//jsxnsMucUser:item");
  //var fromLinkLocal = presenceNode.getAttribute("from").search(/@linklocal\//);
  if (presenceMucItemNode) {
    var presence = new Object;
    presence.JID = presenceMucItemNode.getAttribute("jid");
    var PresenceFrom = presenceNode.getAttribute("from");
    presence.RoomNID = PresenceFrom.substring(0, PresenceFrom.indexOf("@"));
    var PresenceType = presenceNode.getAttribute("type");
    if( PresenceType == "unavailable" ) {
      presence.State = false;
      //TBD - handle other unavailable presences here
      var Destroy = presenceNode.selectSingleNode("//jsxnsMucUser:destroy");
      if (Destroy != null) {
        presence.JID = "";
        log.info("PRESENCE: DESTROY: RoomNID: " + presence.RoomNID);
      }
      else {
        log.info("PRESENCE: OFF: RoomNID: " + presence.RoomNID + ": " + presence.JID);
      }
    }
    else {
      //TBD - handle room start presence here?
      presence.State = true;
      log.info( "PRESENCE: ON: RoomNID: " + presence.RoomNID + ": " + presence.JID);
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
  }

  /*
  // device discovery presence
//    <presence from="Padmaja@linklocal/padmajapc" to="Padmaja@linklocal/padmajapc">
//    <status>avail</status>
//      <msg>Registered</msg>
//    <node>http://www.igniterealtime.org/projects/smack/</node>
//    </presence>  
  else if( fromLinkLocal != -1 ){
    var presence = new Object;
    presence.FromJID = presenceNode.getAttribute("from");
    presence.ToJID = presenceNode.getAttribute("to");
    presence.Status = presenceNode.selectSingleNode("//status").getValue();
    var msgNode = presenceNode.selectSingleNode("//msg");
    if (jsx3.util.strEmpty(msgNode)) {
      return;
    }
    presence.Message = msgNode.getValue();
    log.info( "PROCESSING DEVICE DISCOVERY PRESENCE: " + presence.Status + ": fromJID: " + presence.FromJID);
    topic_Publish(tApp, "LINKLOCALPRESENCE", presence);
    return;    
  }
  */
  // sample notifictaion
  // <Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader>
  //   <notificationdata>
  //     <presence id="8lFGy-3" to="anujpc12@localhost/anujpc12" from="anujpc12@localhost/webclient1_anujpc12"/>
  //   </notificationdata>
  // </Notification>
  // this is not for mucuser. Basically using to check for duplicate user login
  else {   
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
    log.info( "PRESENCE: " + presence.State + ": JID: " + presence.JID);
    // NONE is being used in the cmpBlkLogin.js file
    // if NONE then its regular presence notification
    presence.RoomNID = "NONE"; 
  } 
  topic_Publish(tApp, "PRESENCE", presence);
  return;
}

//iq notification
function handleNotification_IQ(iqNode) {
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
        log.info("IQ: Session Service Query: SessionStart");
        //send request to get collaborationConfig
        topic_Publish(tApp, "SessionStartIQNode", iqNode);
        return;
      }
      //for iq node come from 3rd party with 'SessionStart' to close a player window
      if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:Stop") != null) {
        log.info("IQ: Session Service Query: Stop");
        //send request to get collaborationConfig
        topic_Publish(tApp, "SessionStopIQNode", iqNode);
        return;
      }
      //for iq node come from 3rd party with 'Playback'
      if (iqNode.selectSingleNode(".//jsxnsIPVSSessionService:Playback") != null) {
        log.info("IQ: Session Service Query: Playback");
        //send request to get collaborationConfig
        topic_Publish(tApp, "SessionPlaybackIQNode", iqNode);
        return;
      }
    }
    else {
      //pass session response including error response (type = result / error)
      handleNotification_Response(iqNode);
      return;
    }
  }
  log.warn("Unknown IQ: " + (iqNode.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
  return;
}

//message event notification
function handleNotification_Message(messageNode) {
  //------------------- check if it is session related message
  var xNode = messageNode.selectSingleNode(".//jsxnsIPVSSessionService:x");
  if (xNode != null) {
    //check for ConnectionState msg
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
    var ConnStateNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:ConnState");
    if (ConnStateNode != null) {
      // get values from it
      var objConnState = new Object;
      objConnState.toJID = messageNode.getAttribute("to");
      objConnState.sessionId = ConnStateNode.getAttribute("sessionId");
      objConnState.connId = ConnStateNode.getAttribute("connId");
      objConnState.streamURL = ConnStateNode.getAttribute("streamURL");
      objConnState.state = ConnStateNode.getAttribute("value");

      log.info("Message: SessionService: ConnState Event: " +
                  ": state: " + objConnState.state +
                  ": sessionId: " + objConnState.sessionId +
                  ": connId: " + objConnState.connId +
                  ": streamURL: " + objConnState.streamURL);
      // and publish as connStateEvent
      topic_Publish(tApp, "CONNSTATEEVENT", objConnState);
      return;
    }

    // check for EntityPresence msg
    // Sample Notification
    //<Notification>
    //<notificationheader>
    //<notificationtype>XMPPEvent</notificationtype>
    //</notificationheader>
    //<notificationdata>
    //<message id="IgjuB-6968" to="anujpc12@localhost/anujpc12" from="dff9bcf2-470b-4e8a-971a-661995ea1b6f@conference.localhost/managementserver.localhost" type="groupchat">
    //<x xmlns="com.ipvs.session">
    //<EntityPresence sessionId="" entityId="DstPort_C48831BD-3C90-0001-CBE916347D85D000" jid="anujpc12@localhost/anujpc12" value="available"/>
    //</x></message>
    //</notificationdata>
    //</Notification>    
    var EntityPresenceNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:EntityPresence");
    if( EntityPresenceNode!= null ) {
     // get values from it
      var objEntityPresence = new Object;
      objEntityPresence.toJID = messageNode.getAttribute("to");
      objEntityPresence.sessionId = EntityPresenceNode.getAttribute("sessionId");
      objEntityPresence.entityId = EntityPresenceNode.getAttribute("entityId");
      objEntityPresence.jid = EntityPresenceNode.getAttribute("jid");
      objEntityPresence.presence = EntityPresenceNode.getAttribute("value");

      log.info("Message: SessionService: EntityPresence Event: " +
                  ": sessionId: " + objEntityPresence.sessionId +
                  ": entityId: " + objEntityPresence.entityId +
                  ": jid: " + objEntityPresence.jid +                  
                  ": presence: " + objEntityPresence.presence);    
      // and publish as EntityPresenceEvent
      topic_Publish(tApp, "ENTITYPRESENCEEVENT", objEntityPresence);
      return;    
    }
    
    
    //check for Timeline msg
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
    var TimelineNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:Timeline");
    if (TimelineNode != null) {
      // get values from it
      var objTimeLine = new Object;
      objTimeLine.toJID = messageNode.getAttribute("to");
      objTimeLine.sessionId = TimelineNode.getAttribute("sessionId");
      objTimeLine.connGroupId = TimelineNode.getAttribute("connGroupId");
      objTimeLine.connId = TimelineNode.getAttribute("connId");
      objTimeLine.Id = TimelineNode.getAttribute("id");

      var startNode = TimelineNode.selectSingleNode(".//jsxnsIPVSSessionService:Start");
      objTimeLine.startOffset = startNode.getAttribute("Offset");
      objTimeLine.startWC = startNode.getAttribute("WC");

      var lastNode = TimelineNode.selectSingleNode(".//jsxnsIPVSSessionService:Last");
      objTimeLine.lastOffset = lastNode.getAttribute("Offset");
      objTimeLine.lastWC = lastNode.getAttribute("WC");
      objTimeLine.timeScale = lastNode.getAttribute("TimeScale");
      objTimeLine.lastAction = lastNode.getAttribute("requestedAction");

      log.info("Message: SessionService: TimeLine Event: " +
                  ": sessionId: " + objTimeLine.sessionId +
                  ": connId: " + objTimeLine.connId +
                  ": Id: " + objTimeLine.Id +
                  ": connGroupId: " + objTimeLine.connGroupId +
                  ": startOffset: " + objTimeLine.startOffset +
                  ": startWC: " + objTimeLine.startWC +
                  ": lastOffset: " + objTimeLine.lastOffset +
                  ": lastWC: " + objTimeLine.lastWC +
                  ": timeScale: " + objTimeLine.timeScale +
                  ": lastAction: " + objTimeLine.lastAction);
      // and publish as TimeLineEvent
      topic_Publish(tApp, "TIMELINEEVENT", objTimeLine);
      return;
    }

    //check for PortConnectionUpdate
    //sample PortConnectionUpdate notification
//    <Notification>
//    <notificationheader>
//    <notificationtype>XMPPEvent</notificationtype>
//    </notificationheader>
//    <notificationdata>
//    <message id="f8GAS-1844" to="proxy1@localhost/proxy1" from="managementserver.localhost">
//    <x xmlns="com.ipvs.session">
//    <PortConnectionUpdate>
//    <Port NID="default.mediastreamsrcportresourcelist.mssprl_09bd9416-b552-46f8-a8b8-f338ce6e9129" operation="Add" subscriptionID="12,123">
//    <ConnectionStatus>
//    <PortSessionInfo id="Session_C52EE0EC-ABD0-0001-8BAA1850A611D950" name="anujpc12 Apr 06 10:07">
//    <PortConnectionInfo canvasId="Session_C52EE0EC-ABD0-0001-8BAA1850A611D950" dstNID="default.mediastreamdstportresourcelist.msdprl_e73f8538-5914-4773-87eb-65541c98cf35" dstTitle="rx576dst" errorCode="" errorData="" id="Conn_C52EE0EC-ABD0-0001-473E1B2019001B64" name="tx10311" srcNID="default.mediastreamsrcportresourcelist.mssprl_09bd9416-b552-46f8-a8b8-f338ce6e9129" srcTitle="tx10311" state="" type="VIEW" windowId="Conn_C52EE0EC-ABD0-0001-473E1B2019001B64">
//    <Profile allowKMSwitch="true" avOption="Both" burstDuration="4" burstRatio="1.00" highCompression="6" id="64b17f8b-0d06-4b0f-ae2c-a87dc34549d2" lowCompression="4" maxBandwidth="20000" multicast="false" profileNID="default.mediastreamprofilelist.mspl_525480a2-e033-4595-8420-d15e591f7517" profileName="V2D_LAN" remoteKM="true" unicast="true"/>
//    </PortConnectionInfo>
//    </PortSessionInfo>
//    </ConnectionStatus>
//    </Port>
//    </PortConnectionUpdate>
//    </x>
//    </message>
//    </notificationdata>
//    </Notification>
    var portConnectionUpdateNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:PortConnectionUpdate");
    if (portConnectionUpdateNode != null) {
      var noNameSpaceXMLStr = (portConnectionUpdateNode.toString()).replace(/xmlns=\"com.ipvs.session\"/g, "");
      portConnectionUpdateNode = new jsx3.xml.Document();
      portConnectionUpdateNode.loadXML(noNameSpaceXMLStr);
      log.info("\nMessage: SessionService: PortConnectionUpdate Event: " + portConnectionUpdateNode + "\n");
      var portNode = portConnectionUpdateNode.selectSingleNode(".//Port");
      var portNID = portNode.getAttribute("NID");
      var operaion = portNode.getAttribute("operation");
      var subscriptionIDStr = portNode.getAttribute("subscriptionID");
      var subscriptionIDArr = subscriptionIDStr.split(",");
      var portSessionInfo = portNode.selectSingleNode(".//PortSessionInfo");
      var objPortConnection = new Object;
      objPortConnection.PortNID = portNID;
      objPortConnection.Operation = operaion;
      //publish for each subscription id
      for (var i = 0; i < subscriptionIDArr.length; i++) {
        objPortConnection.subscriptionID = subscriptionIDArr[i];
        objPortConnection.PortSessionInfoNode = portSessionInfo.cloneNode(true);
        // and publish as portConnectionUpdateevent
        topic_Publish(tApp, "PORTCONNECTIONUPDATE", objPortConnection);
      }

      return;
    }


    //check for PortConnectionStateUpdate
    //sample PortConnectionStateUpdate notification
    //<Notification>
    //<notificationheader>
    //<notificationtype>XMPPEvent</notificationtype>
    //</notificationheader>
    //<notificationdata>
    //<message id="fXJ86-354" to="adminuser@localhost/adminpc" from="managementserver.localhost">
    //<x xmlns="com.ipvs.session">
    //<PortConnectionStateUpdate>
    //<Port NID="default.mediastreamsrcportresourcelist.mssprl_45eabe91-af9d-4ce8-a44d-7087fcb2650c" operation="ConnectionStateUpdate" subscriptionID="1234">
    //<ConnectionState connId="3c1aaf93-fb23-4187-9956-03ac7ab62f79" sessionId="ccdc9645-2488-42fd-a334-e9283f8f338f" value="STREAMING">
    //<Timeline id="4203d6a8-fce6-413d-a011-a06d9de75634">
    //   <Start Offset="" TC="1334824907093" WC="1334824907093" parentTimelineId=""/>
    //   <Last Offset="1959" TC="1334824907093" TimeScale="1" WC="1334824907093" requestedAction="Start"/>
    //</Timeline>
    //</ConnectionState>
    //</Port>
    //</PortConnectionStateUpdate>
    //</x>
    //</message>
    //</notificationdata>
    //</Notification>
    var portConnectionStateUpdateNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:PortConnectionStateUpdate");
    if (portConnectionStateUpdateNode != null) {
      var noNameSpaceXMLStr = (portConnectionStateUpdateNode.toString()).replace(/xmlns=\"com.ipvs.session\"/g, "");
      portConnectionStateUpdateNode = new jsx3.xml.Document();
      portConnectionStateUpdateNode.loadXML(noNameSpaceXMLStr);
      log.info("\nMessage: SessionService: PortConnectionStateUpdate Event: " + portConnectionStateUpdateNode + "\n");
      var portNode = portConnectionStateUpdateNode.selectSingleNode(".//Port");
      var portNID = portNode.getAttribute("NID");
      var subscriptionIDStr = portNode.getAttribute("subscriptionID");
      var subscriptionIDArr = subscriptionIDStr.split(",");
      var portConnectionInfo = portNode.selectSingleNode(".//ConnectionState");
      var objPortConnectionState = new Object;
      objPortConnectionState.PortNID = portNID;
      objPortConnectionState.Operation = "ConnectionStateUpdate";
      objPortConnectionState.ConnectionID = portConnectionInfo.getAttribute("connId");
      objPortConnectionState.SessionID = portConnectionInfo.getAttribute("sessionId");
      objPortConnectionState.Value = portConnectionInfo.getAttribute("value");
      objPortConnectionState.ErrorCode = portConnectionInfo.getAttribute("errorCode");
      objPortConnectionState.ErrorData = portConnectionInfo.getAttribute("errorData");
      //publish for each subscription id
      for (var i = 0; i < subscriptionIDArr.length; i++) {
        objPortConnectionState.subscriptionID = subscriptionIDArr[i];
        // and publish as portConnectionUpdateevent
        topic_Publish(tApp, "PORTCONNECTIONSTATEUPDATE", objPortConnectionState);
      }

      return;
    }

    //check for PortDisplayConfigUpdate
    //sample PortDisplayConfigUpdate notification
    //XP Rx Display Config
    //<Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader><notificationdata>
    //<message id="Su0kp-2538" to="srcuser1@localhost/srcpc1" from="managementserver@localhost/managementserver">
    //<x>
    //<PortDisplayConfigUpdate>
    //<PortDisplayConfig NID="default.mediastreamioportresourcelist.msioprl_66c05fcb-6877-4542-803d-2fb05f5c8194" operation="Update" subscriptionID="srcuser1@localhost/srcpc1_display_config">
    //<Canvas>
    //  <Background content="LOCALVIDEO" enable="false" zIndex="0"/>
    //  <Window content="REMOTEVIDEO" enable="true" id="id" zIndex="1">
    //    <Geometry aspect="Auto" hpos="100px" hres="75%" transparency="20%" vpos="50px" vres="75%"/>
    //  </Window>
    //</Canvas>
    //</PortDisplayConfig>
    //</PortDisplayConfigUpdate>
    //</x></message></notificationdata></Notification>

    //UMC Display Config
    //<Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader><notificationdata>
    //<message id="Su0kp-2412" to="srcuser1@localhost/srcpc1" from="managementserver@localhost/managementserver">
    //<x>
    //<PortDisplayConfigUpdate>
    //<PortDisplayConfig NID="default.mediastreamdstportresourcelist.msdprl_4206c10c-c5f6-43fe-8908-f6752408e351" operation="Update" subscriptionID="srcuser1@localhost/srcpc1_display_config">
    //<DisplayStatus>
    //<Display configurable=""/>
    //<Canvas hres="1280" id="" vres="1024">
    //  <Background content="" enable="" zIndex="">
    //      <ImageContent color="" imageURL=""/>
    //      <SessionContent enableBookmarkPanel="" enableConnPanel="" enableCtrlPanel="" enableHeaderPanel="" enablePlayerPanel="" sessionId=""/>
    //  </Background>
    //  <Layout active="true" border="Show" borderStyle="[8px]" colWidth="" header="hide" headerStyle="basic" hpad="" id="LayoutId1" nCol="" nRow="" nStrip="" rowHeight="" stripPos="" type="" vpad=""/>
    //  <Window content="" id="Win1">
    //      <ConnectionContent MediaStreamIOPortElementID="" MediaStreamIOPortTypeTitle="" connId="" hres="" sessionId="" srcKbmCtrl="" vres=""/>
    //      <ThumbnailContent MediaStreamIOPortElementID="" MediaStreamIOPortTypeTitle="" imageURL="" refreshInterval=""/>
    //      <Properties aspect="" border="" fps="15" hpos="0" hres="100%" layoutId="LayoutId1" layoutPos="" maximize="false" mute="false" remoteKbmCtrl="false" scale="fitWindow" scrollHPos="0" scrollVPos="0" transparency="0" visibility="true" volume="50%" vpos="0" vres="100%" zIndex="100"/>
    //  </Window>
    //</Canvas>
    //</DisplayStatus>
    //</PortDisplayConfig>
    //</PortDisplayConfigUpdate>
    //</x></message></notificationdata></Notification>

    var portDisplayConfigUpdateNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:PortDisplayConfigUpdate");
    if (portDisplayConfigUpdateNode != null) {
      var noNameSpaceXMLStr = (portDisplayConfigUpdateNode.toString()).replace(/xmlns=\"com.ipvs.session\"/g, "");
      portDisplayConfigUpdateNode = new jsx3.xml.Document();
      portDisplayConfigUpdateNode.loadXML(noNameSpaceXMLStr);
      log.info("\nMessage: SessionService: PortDisplayConfigUpdate Event: " + portDisplayConfigUpdateNode + "\n");
      var portDisplayConfigNode = portDisplayConfigUpdateNode.selectSingleNode(".//PortDisplayConfig");
      var portNID = portDisplayConfigNode.getAttribute("NID");
      var operaion = portDisplayConfigNode.getAttribute("operation");
      var subscriptionIDStr = portDisplayConfigNode.getAttribute("subscriptionID");
      var subscriptionIDArr = subscriptionIDStr.split(",");

      var objPortDisplayConfigUpdate = new Object;
      objPortDisplayConfigUpdate.PortNID = portNID;
      objPortDisplayConfigUpdate.Operation = operaion;
      //publish for each subscription id
      for (var i = 0; i < subscriptionIDArr.length; i++) {
        objPortDisplayConfigUpdate.subscriptionID = subscriptionIDArr[i];
        objPortDisplayConfigUpdate.PortDisplayConfigNode = portDisplayConfigNode.cloneNode(true);
        // and publish as PORTDISPLAYCONFIGUPDATE  event
        topic_Publish(tApp, "PORTDISPLAYCONFIGUPDATE", objPortDisplayConfigUpdate);
      }

      return;
    }

    //check for PortDisplayStatusUpdate
    //sample PortDisplayStatusUpdate notification
    //XP Rx Display Status
    //<Notification>
    //<notificationheader>
    //<notificationtype>XMPPEvent</notificationtype>
    //</notificationheader>
    //<notificationdata>
    //<message id="Su0kp-2545" to="srcuser1@localhost/srcpc1" from="managementserver@localhost/managementserver">
    //<x>
    //<PortDisplayStatusUpdate>
    //<PortDisplayStatus NID="default.mediastreamioportresourcelist.msioprl_66c05fcb-6877-4542-803d-2fb05f5c8194" operation="Update" subscriptionID="srcuser1@localhost/srcpc1_display_status">
    //<Canvas hres="1280px" vres="1024px">
    //  <Display hres="1280px" id="" lut="Server Splash Screen 1280x1024@60.0" vres="1024px"/>
    //  <Background content="LOCALVIDEO" enable="false" zIndex="0"/>
    //  <Window content="REMOTEVIDEO" enable="true" id="" zIndex="1">
    //    <Geometry hpos="100px" hres="960px" transparency="0%" vpos="50px" vres="768px"/>
    //    <Source hres="960px" hscale="75%" lut="Server Splash Screen 1280x1024@60.0" vres="768px" vscale="75%"/>
    //  </Window>
    //</Canvas>
    //</PortDisplayStatus>
    //</PortDisplayStatusUpdate>
    //</x></message></notificationdata></Notification>

    //UMC Display Status
    //<Notification><notificationheader><notificationtype>XMPPEvent</notificationtype></notificationheader><notificationdata>
    //<message id="Su0kp-2416" to="srcuser1@localhost/srcpc1" from="managementserver@localhost/managementserver">
    //<x>
    //<PortDisplayStatusUpdate><PortDisplayStatus NID="default.mediastreamdstportresourcelist.msdprl_4206c10c-c5f6-43fe-8908-f6752408e351" operation="Update" subscriptionID="srcuser1@localhost/srcpc1_display_status">
    //<DisplayStatus>
    //<Display configurable=""/>
    //<Canvas hres="1280" id="" vres="1024">
    //  <Background content="" enable="" zIndex="">
    //      <ImageContent color="" imageURL=""/>
    //      <SessionContent enableBookmarkPanel="" enableConnPanel="" enableCtrlPanel="" enableHeaderPanel="" enablePlayerPanel="" sessionId=""/>
    //  </Background>
    //  <Layout active="true" border="Show" borderStyle="[8px]" colWidth="" header="hide" headerStyle="basic" hpad="" id="LayoutId1" nCol="" nRow="" nStrip="" rowHeight="" stripPos="" type="" vpad=""/>
    //  <Window content="" id="Win1">
    //      <ConnectionContent MediaStreamIOPortElementID="" MediaStreamIOPortTypeTitle="" connId="" hres="" sessionId="" srcKbmCtrl="" vres=""/>
    //      <ThumbnailContent MediaStreamIOPortElementID="" MediaStreamIOPortTypeTitle="" imageURL="" refreshInterval=""/>
    //      <Properties aspect="" border="" fps="15" hpos="0" hres="100%" layoutId="LayoutId1" layoutPos="" maximize="false" mute="false" remoteKbmCtrl="false" scale="fitWindow" scrollHPos="0" scrollVPos="0" transparency="0" visibility="true" volume="50%" vpos="0" vres="100%" zIndex="100"/>
    //  </Window>
    //</Canvas>
    //</DisplayStatus>
    //</PortDisplayStatus>
    //</PortDisplayStatusUpdate>
    //</x>
    //</message></notificationdata></Notification>

    var portDisplayStatusUpdateNode = xNode.selectSingleNode("./jsxnsIPVSSessionService:PortDisplayStatusUpdate");
    if (portDisplayStatusUpdateNode != null) {
      var noNameSpaceXMLStr = (portDisplayStatusUpdateNode.toString()).replace(/xmlns=\"com.ipvs.session\"/g, "");
      portDisplayStatusUpdateNode = new jsx3.xml.Document();
      portDisplayStatusUpdateNode.loadXML(noNameSpaceXMLStr);
      log.info("\nMessage: SessionService: PortDisplayConfigUpdate Event: " + portDisplayStatusUpdateNode + "\n");
      var portDisplayStatusNode = portDisplayStatusUpdateNode.selectSingleNode(".//PortDisplayStatus");
      var portNID = portDisplayStatusNode.getAttribute("NID");
      var operaion = portDisplayStatusNode.getAttribute("operation");
      var subscriptionIDStr = portDisplayStatusNode.getAttribute("subscriptionID");
      var subscriptionIDArr = subscriptionIDStr.split(",");

      var objPortDisplayStatusUpdate = new Object;
      objPortDisplayStatusUpdate.PortNID = portNID;
      objPortDisplayStatusUpdate.Operation = operaion;
      //publish for each subscription id
      for (var i = 0; i < subscriptionIDArr.length; i++) {
        objPortDisplayStatusUpdate.subscriptionID = subscriptionIDArr[i];
        objPortDisplayStatusUpdate.PortDisplayStatusNode = portDisplayStatusNode.cloneNode(true);
        // and publish as PORTDISPLAYCONFIGUPDATE  event
        topic_Publish(tApp, "PORTDISPLAYSTATUSUPDATE", objPortDisplayStatusUpdate);
      }

      return;
    }


    log.warn("Unknown SessionService Message : " + (messageNode.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
    return;
  }

  // ---------------- check if it is a DB update event
  var updateEventNode = messageNode.selectSingleNode(".//jsxnsIPVSDBService:updateEvent");
  if (updateEventNode != null) {
    var nodeID = updateEventNode.getAttribute("nodeID");
    if (nodeID == null || nodeID == "") {
      log.warn("Message: IPVSDBService UpdateEvent: Empty nodeID: ");
      return;
    };
    
    var eventType = updateEventNode.getAttribute("type");
    if( eventType != "add" && eventType != "replace" && eventType != "delete" ) {
      log.warn("Message: IPVSDBService UpdateEvent: EventType: " + eventType + " : NID: " + nodeID);
      return;
    }
    var eventChildXPath = updateEventNode.getAttribute("childXPath");
    var parentList = nodeID.substring(nodeID.indexOf(".")+1, nodeID.lastIndexOf("."));
    var objType = arrObjListInfo[parentList];
    var objEvent = new Object;
    objEvent.nodeID = nodeID;
    objEvent.childXpath = unescape(eventChildXPath);
    objEvent.objType = objType;
    objEvent.type = eventType;    
    if( ( (eventType == "add") || (eventType == "delete") ) && (!jsx3.util.strEmpty(eventChildXPath)) ){
      objEvent.type = "replace";
    }
    
    log.info("Message: IPVSDBService UpdateEvent: Node " + eventType + ": Dirty NID: " + nodeID);
    topic_Publish(tApp, "DBUpdate", objEvent);
    return true;
    
  }
  // -------------- check if it is MR Service Message 
  // TBD: ignore the nested 'x' tag (without namespace)
  var xNode = messageNode.selectSingleNode(".//x");
  if (xNode != null) {
    if( messageNode.selectSingleNode(".//AgentMessage") != null ) {
      if (handleNotification_AgentMessage(messageNode) != true) {
        log.warn("Unknown MRService AgentMessage : " + (messageNode.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
      }
      return;
    }
//    else if (messageNode.selectSingleNode(".//AgentEvent") != null) {
//      if (handleNotification_AgentEvent(messageNode) != true) {
//        log.warn("Unknown MRService AgentEvent : " + messageNode.toString());
//      }
//      return;
//    }
    log.warn("Unknown MRService Message : " + (messageNode.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
    return;
  }
  log.warn("Unknown Message : " + (messageNode.toString()).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
  return;
}

/**
  Basic there are 3 kind of input:
  1)request from MS (based on service name distribut to corresponding agent);
  2)direct response from MS (mapping previous request, need fill clientcallback with agent);
  3)event notification (like destSteramAgent message);
  This handler will process item 1) 2) and 3)

  open question:
  1. how to get event agent information like ?
  2. roomNID still there or not? startStreamResponse URL, presence destroy
 **/

//mediastoreservice may be obsolete only using MW internal
SERVIC_MAPPING_LIST = {
    "systemservice":  "com.ipvs.mediaroomservice.impl.AppServerAgent",
    "mediastoreservice":"com.ipvs.mediaroomservice.mediastore.MediaStoreAdminClientAgent",
    "mediaroomservice":"com.ipvs.mediaroomservice.impl.AppServerAgent",
    "assetadminservice":"com.ipvs.mediaroomservice.impl.AppServerAgent",
    "policyadminservice":"com.ipvs.mediaroomservice.impl.AppServerAgent",
    "sessionservice":"com.ipvs.mediaroomservice.impl.AppServerAgent",
    "Layout":"com.ipvs.mediaroomservice.impl.DestStreamAgent",
    "Media":"com.ipvs.mediaroomservice.impl.SourceStreamAgent",
    "DeviceAdmin":"com.ipvs.common.service.DeviceAdminServerAgent",
    "Config":"com.ipvs.common.service.DeviceAdminClientAgent",
    "MediaStore":"com.ipvs.mediaroomservice.mediastore.MediaStoreAdminClientAgent",
    "FileTransfer":"com.ipvs.systemservice.impl.FileTransferAgent",
    "Operator": "Operator",
    "Discovery": "Discovery"
};

/* unmapping services:
 "com.ipvs.app.IPVSClientApplet" (obsolete)
 "com.ipvs.common.service.XAuthClientAgent" (obsolete)
 "com.ipvs.common.service.XAuthServerAgent" (obsolete)
 */

function handleNotification_AgentMessage(messageNode) {
  var retval = false;
  var agentMessageNode = messageNode.selectSingleNode("//AgentMessage");
  if (agentMessageNode == null) return retval;
  //ownerJID
  var FromJID = agentMessageNode.getAttribute("from");
  //request/response/event 
  var AgentMessageType = agentMessageNode.getAttribute("type");
  //agentJID
  var MessageJID = agentMessageNode.getAttribute("to");

  //check whether request or not, if it is request then figur out agent from service name
  var serviceName = '', messageAgent = '';
  var AgentMessageDataNode = null;
  //serviceName = agentMessageNode.selectSingleNode("//servicename").getValue();
  if (AgentMessageType.trim() == 'Request') {
    serviceName = agentMessageNode.selectSingleNode("//servicename").getValue();
    messageAgent = SERVIC_MAPPING_LIST[serviceName];
    AgentMessageDataNode = agentMessageNode.selectSingleNode("//Request");
  }
  else if (AgentMessageType.trim() == 'Response') {
    serviceName = agentMessageNode.selectSingleNode("//servicename").getValue();
    messageAgent = agentMessageNode.selectSingleNode("//clientcallback").getValue();
    AgentMessageDataNode = agentMessageNode.selectSingleNode("//Response");
  }
  else if (AgentMessageType.trim() == 'Event') {
    serviceName = agentMessageNode.selectSingleNode("//header").getAttribute("serviceName");
    messageAgent = SERVIC_MAPPING_LIST[serviceName];
    AgentMessageDataNode = agentMessageNode.selectSingleNode("//Event");
  }
  //************
  
  // approx Room NID format // d9e7795f-317e-4243-82cb-3f40279ee973
  var RoomNID = FromJID.substring(0, FromJID.indexOf("@"));
  if( RoomNID.match(/-\w\w\w\w-\w\w\w\w-\w\w\w\w-/) == null ) {
      RoomNID = null;
  }

//  var agentMessageNode = messageNode.selectSingleNode("//AgentMessage");
//  if( agentMessageNode == null ) return retval;
//  // get fromJID
//  var FromJID = messageNode.selectSingleNode("//message").getAttribute("from");
//  // grab message type and data
//  var AgentMessageType = agentMessageNode.selectSingleNode("//agentmessagetype").getValue();
//  var AgentMessageDataNode = agentMessageNode.selectSingleNode("//agentmessagedata");
//  // get message agent and parse it
//  var MessageJID = agentMessageNode.selectSingleNode("//agentJID").getValue();
//  var MessageAgent = agentMessageNode.selectSingleNode("//agent").getValue();
//  // check if message came from appserver or Room
//  // if room then get roomNID and pass to the right handler based on the message agent 
//  // approx Room NID format // d9e7795f-317e-4243-82cb-3f40279ee973
//  // till we get AppserverJID this is the hack 
//  var RoomNID = FromJID.substring(0, FromJID.indexOf("@"));
//  if( RoomNID.match(/-\w\w\w\w-\w\w\w\w-\w\w\w\w-/) == null ) {
//    RoomNID = null;
//  }
//  // ignore who this is for - process all events
//  /*
//  // check if mesage is for us
//  var MessageAgentUserName = MessageJID;
//  var AgentJID = topic_Get(tApp, "LoginUserJID");
//  var AgentUserName = AgentJID;
//  if( MessageJID != "*" && MessageAgentUserName != AgentUserName && MessageAgent != "Operator") {
//    retval = handleNotification_ForeignAgentMessage(FromJID, RoomNID, MessageAgent, AgentMessageType, AgentMessageDataNode);
//    return retval;
//  }
//  */
  // pass message to the right agent
  switch (messageAgent) {
    case "Operator":
      retval = handleNotification_OperatorAgentMessage(MessageJID, FromJID, RoomNID, AgentMessageType, AgentMessageDataNode, agentMessageNode);
      break;
    case "com.ipvs.mediaroomservice.impl.DestStreamAgent":
      retval = handleNotification_DestStreamAgentMessage(MessageJID, FromJID, RoomNID, AgentMessageType, AgentMessageDataNode);
      break;
    case "com.ipvs.mediaroomservice.impl.SourceStreamAgent":
      break;
    case "com.ipvs.mediaroomservice.mediastore.MediaStoreAdminClientAgent":
      retval = handleNotification_MediaStoreAdminClientAgentMessage(MessageJID, FromJID, RoomNID, AgentMessageType, AgentMessageDataNode);
      break;
    case "com.ipvs.common.service.XAuthClientAgent":
      break;
    case "com.ipvs.common.service.XAuthServerAgent":
      break;
    case "com.ipvs.common.service.DeviceAdminClientAgent":
    case "com.ipvs.client.MRClientAgent":
    case "com.ipvs.app.IPVSClientApplet":
      // stripping notification header and call handleresponse with the response node
      AgentMessageDataNode = agentMessageNode.selectSingleNode(".//Response");
      if(AgentMessageDataNode != null) {
        handleNotification_Response(AgentMessageDataNode);
        retval = true;
      }
      break;
    case "com.ipvs.common.service.DeviceAdminServerAgent":
      retval = handleNotification_DeviceAdminServerAgent(MessageJID, FromJID, AgentMessageType, AgentMessageDataNode);
      break;
    case "com.ipvs.mediaroomservice.impl.AppServerAgent":
      retval = handleNotification_AppServerAgentMessage(MessageJID, FromJID, RoomNID, AgentMessageType, AgentMessageDataNode);
      break;
    case "com.ipvs.systemservice.impl.FileTransferAgent":
      retval = handleNotification_FileTransferAgentMessage(MessageJID, FromJID, RoomNID, AgentMessageType, AgentMessageDataNode);
      break;
    case "Discovery":
      retval = handleNotification_DiscoveryAgentMessage(MessageJID, FromJID, RoomNID, AgentMessageType, AgentMessageDataNode, agentMessageNode);
      break;
    default:
      break;
  }
  return retval;
}
//froeign agent message event notification
function handleNotification_ForeignAgentMessage(FromJID, RoomNID, ForeignAgent, MessageType, messageDataNode) {
  var retval = false;
  if( messageDataNode == null ) return retval;
  
  var requestAction = messageDataNode.selectSingleNode("//action").getValue();
  log.info( "Message: MRService: Agent Message: Foreign Agent: " + ForeignAgent + ": " + MessageType + ": " + requestAction );                
  retval = true;    
  
  return retval;
}

//operator agent message event notification
function handleNotification_DiscoveryAgentMessage(toJID, fromJID, roomNID, messageType, messageDataNode, messageNode) {
  var retVal = false;
  var eventNameNode = messageDataNode.selectSingleNode("//header/eventname");
  if (!jsx3.util.strEmpty(eventNameNode) && eventNameNode.getValue() == "DiscoveryEvent") {
    var discoveryEventDataNode = messageDataNode.selectSingleNode("//DiscoveryEventData");
    var discoveryEvent = new Object;
    //discoveryEvent.fromJID = fromJID;
    //discoveryEvent.toJID = toJID;
    discoveryEvent.name = discoveryEventDataNode.getAttribute("name");
    discoveryEvent.ipaddress = discoveryEventDataNode.getAttribute("ipaddress");
    discoveryEvent.serviceType = discoveryEventDataNode.getAttribute("serviceType");
    discoveryEvent.registered = discoveryEventDataNode.getAttribute("registered");
    discoveryEvent.available = discoveryEventDataNode.getAttribute("available");
    discoveryEvent.type = discoveryEventDataNode.getAttribute("type");
    discoveryEvent.macaddress = discoveryEventDataNode.getAttribute("macaddress");
    discoveryEvent.port = discoveryEventDataNode.getAttribute("port");
    log.info("PROCESSING DEVICE DISCOVERY EVENT: " + discoveryEvent.available + ": from: " + discoveryEvent.name);
    topic_Publish(tApp, "DISCOVERYEVENT", discoveryEvent);
    retVal = true;
  }
  return retVal; 
}

//Discovery agent message event notification
function handleNotification_OperatorAgentMessage(ToJID, FromJID, RoomNID, MessageType, messageDataNode, messageNode) {
  var retval = false;
  if( messageDataNode == null ) return retval;
  
  if( MessageType == "Request" ) {
    var reqName = messageDataNode.selectSingleNode("//requestname").getValue();
    if( reqName == "ImportFilePackRequest" || reqName == "ImportCleanupMediaFileRequest" ) {
      var importFileEvent = new Object;
      importFileEvent.reqName = reqName;
      importFileEvent.messageNode = messageNode;
      log.info( "Message: MRService: Agent Message: Operator Agent: " + MessageType + ": " + reqName);
      topic_Publish(tApp, "IMPORTFILEEVENT", importFileEvent);                    
      retval = true;
    }
  }
  else if( MessageType == "Response" ) {
    // room response
    if( RoomNID != null ) {
      var roomevent = new Object;
      roomevent.ToJID = ToJID;
      roomevent.RoomNID = RoomNID;
      roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
      switch(roomevent.Action) {
        case "NotifyResponse":
          roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
          if( roomevent.StreamNID != null ) roomevent.StreamNID = roomevent.StreamNID.getValue();
          roomevent.StatusCode = messageDataNode.selectSingleNode("//status");
          if( roomevent.StatusCode != null ) roomevent.StatusCode = roomevent.StatusCode.getValue();
          if( roomevent.StatusCode < 200 ) {
            roomevent.EventCode = roomevent.StatusCode;
            roomevent.StatusCode = 200;
            roomevent.ErrCode = null;
          }
          else if( roomevent.StatusCode == 200 ) {
            roomevent.EventCode = null;
            roomevent.ErrCode = null;
          }
          else if( roomevent.StatusCode > 200 ) {
            roomevent.ErrCode = roomevent.StatusCode;
            roomevent.StatusCode = 0;
            roomevent.EventCode = null;
          }
          else {
            return retval;
          }
          log.info( "Message: MRService: Agent Message: Operator Agent: " + MessageType + ": " + roomevent.Action +
                    ": ToJID: " + roomevent.ToJID +
                    ": RoomNID: " + roomevent.RoomNID +
                    ": StreamNID: " + roomevent.StreamNID +
                    ": StatusCode: " + roomevent.StatusCode + 
                    ": EventCode: " + roomevent.EventCode + 
                    ": ErrCode: " + roomevent.ErrCode);
          topic_Publish(tApp, "ROOMEVENT", roomevent);
          retval = true;
          break;
        default:
          break;
      }
    }
  } 
  else if( MessageType == "Event") {
    // room agent event
    if( RoomNID != null ) {
      var roomevent = new Object;
      roomevent.ToJID = ToJID;
      roomevent.RoomNID = RoomNID;
      roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
      log.info( "Message: MRService: Agent Message: Operator Agent: " + MessageType + ": " + roomevent.Action +
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
      switch(agentevent.Action) {
        case "RecycleBinProcess":
          agentevent.eventCode = messageDataNode.selectSingleNode("//eventCode");
          if( agentevent.eventCode != null ) agentevent.eventCode = agentevent.eventCode.getValue();
          log.info( "Message: MRService: Agent Message: Operator Agent: " + MessageType + ": " + agentevent.Action +
                    ": EventCode: " + agentevent.eventCode );
          topic_Publish(tApp, "AGENTEVENT", agentevent);                    
          retval = true;
          break;
        case "EmptyDirProcess":
          agentevent.eventCode = messageDataNode.selectSingleNode("//eventCode");
          if( agentevent.eventCode != null ) agentevent.eventCode = agentevent.eventCode.getValue();
          agentevent.mediadirnid = messageDataNode.selectSingleNode("//MediaStreamDirNID");
          if( agentevent.mediadirnid != null ) agentevent.mediadirnid = agentevent.mediadirnid.getValue();
          log.info( "Message: MRService: Agent Message: Operator Agent: " + MessageType + ": " + agentevent.Action +
                    ": EventCode: " + agentevent.eventCode );
          topic_Publish(tApp, "AGENTEVENT", agentevent);                    
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
function handleNotification_AppServerAgentMessage(ToJID, FromJID, RoomNID, MessageType, messageDataNode) {
  var retval = false;
  if( messageDataNode == null ) return retval;
  
  if( MessageType == "Event") {
    if( RoomNID != null ) {
      var StreamStatusEventNode = messageDataNode.selectSingleNode("//AgentEvent//StreamStatusEventData");
      if(StreamStatusEventNode != null) {
        var roomevent = new Object;
        roomevent.RoomNID = StreamStatusEventNode.getAttribute("roomNID");
        roomevent.StreamNID = StreamStatusEventNode.getAttribute("streamNID");
        
        roomevent.Action = messageDataNode.selectSingleNode("//eventname");
        if (roomevent.Action != null) roomevent.Action = roomevent.Action.getValue();

        roomevent.EventCode = StreamStatusEventNode.selectSingleNode("//state");
        if(roomevent.EventCode != null)roomevent.EventCode = roomevent.EventCode.getValue();

        roomevent.StatusCode = messageDataNode.selectSingleNode("//eventLevel");
        if (roomevent.StatusCode != null) roomevent.StatusCode = roomevent.StatusCode.getValue();
        if (roomevent.StatusCode==0){
          roomevent.ErrCode = roomevent.StatusCode;
        }
        else{
          roomevent.ErrCode = null;
        }
        log.info( "Message: MRService: Agent Message: AppServer Agent: Stream Status Event: " + MessageType + ": " + roomevent.Action +
                ": ToJID: " + roomevent.ToJID +
                ": RoomNID: " + roomevent.RoomNID +
                ": StreamNID: " + roomevent.StreamNID +
                ": StatusCode: " + roomevent.StatusCode +
                ": EventCode: " + roomevent.EventCode +
                ": ErrCode: " + roomevent.ErrCode); 
        topic_Publish(tApp, "ROOMEVENT", roomevent);
        retval = true; 
      }
    }
  }
  return retval;
}

//dest stream agent message event notification
function handleNotification_DestStreamAgentMessage(ToJID, FromJID, RoomNID, MessageType, messageDataNode) {
  var retval = false;
  if( messageDataNode == null ) return retval;

  if( MessageType == "Request" ) {
    if( RoomNID != null ) {
      var roomevent = new Object;
      roomevent.ToJID = ToJID;
      roomevent.RoomNID = RoomNID;
          
      roomevent.Action = messageDataNode.selectSingleNode("//action");
      if( roomevent.Action != null ) roomevent.Action = roomevent.Action.getValue();
      roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
      if( roomevent.StreamNID != null ) roomevent.StreamNID = roomevent.StreamNID.getValue();
      log.info( "Message: MRService: Agent Message: Dest Stream Agent: " + MessageType + ": " + roomevent.Action +
                  ": ToJID: " + roomevent.ToJID +
                  ": RoomNID: " + roomevent.RoomNID +
                  ": StreamNID: " + roomevent.StreamNID);
      retval = true;
    } 
  }
  else if (MessageType == "Response") {
    var reqName = messageDataNode.selectSingleNode("//header/requestname").getValue();
    var dataNode = messageDataNode.selectSingleNode("//StreamStatus");
    if (!jsx3.util.strEmpty(dataNode)) {
      var roomevent = new Object;
      roomevent.ToJID = ToJID;
      roomevent.RoomNID = dataNode.getAttribute("roomNID");
      roomevent.StreamNID = dataNode.getAttribute("streamNID");
      roomevent.StreamURL = dataNode.selectSingleNode("//URL");
      if (roomevent.StreamURL != null) roomevent.StreamURL = roomevent.StreamURL.getValue();

      roomevent.Action = null;
      roomevent.Offset = null;
      if (reqName == "StartRequest") {
        roomevent.Action = "Start";
      }
      else if (reqName == "StopRequest") {
        roomevent.Action = "Stop";
      }
      else {
        var mediaTimelineNode = dataNode.selectSingleNode("//MediaTimeline");
        if (!jsx3.util.strEmpty(mediaTimelineNode)) {
          roomevent.Action = mediaTimelineNode.selectSingleNode("//Last").getAttribute("requestedAction");
          roomevent.Offset = mediaTimelineNode.selectSingleNode("//Last").getAttribute("Offset");
        }
      }
      
      roomevent.StatusCode = messageDataNode.selectSingleNode("//state");
      if( roomevent.StatusCode != null ) roomevent.StatusCode = roomevent.StatusCode.getValue();
      roomevent.EventCode = null;
      if( roomevent.StatusCode == "0" ) {
        roomevent.ErrCode = messageDataNode.selectSingleNode("//log/Code");
        if( roomevent.ErrCode != null ) roomevent.ErrCode = roomevent.ErrCode.getValue();
      }
      else {
        roomevent.ErrCode = null;
      }
      log.info( "Message: MRService: Agent Message: Dest Stream Agent: " + MessageType + ": " + roomevent.Action +
                  ": ToJID: " + roomevent.ToJID +
                  ": RoomNID: " + roomevent.RoomNID +
                  ": StreamNID: " + roomevent.StreamNID +
                  ": StatusCode: " + roomevent.StatusCode + 
                  ": EventCode: " + roomevent.EventCode + 
                  ": ErrCode: " + roomevent.ErrCode +
                  ": Offset: "  + roomevent.Offset +
                  ": StreamURL: "  + roomevent.StreamURL);
      switch(roomevent.Action) {
        case "Start":
        case "Pause":
        case "Resume":
        case "ResumeFromMediaTC":
        case "ResumeFrom":
        case "SkipBackward":
        case "SkipForward":
        case "Catchup":
        case "Stop":
          topic_Publish(tApp, "ROOMEVENT", roomevent);
          retval = true;
          break;
        default:
          break;
      }
    } 
  } 
  else if( MessageType == "Event" ) {
    if( RoomNID != null ) {
      var roomevent = new Object;
      roomevent.ToJID = ToJID;
      roomevent.RoomNID = RoomNID;
       //skip new RelayEvent which doesn't have 'action' node
      var actionNode = messageDataNode.selectSingleNode("//action");
      if (actionNode != null) {       
        roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
        switch(roomevent.Action) {
          case "SourceEvent":
          case "DestEvent":
            //TBD - this needs to be more specific - there are many streamNIDs in this message
            roomevent.StreamNID = messageDataNode.selectSingleNode("//streamNID");
            if( roomevent.StreamNID != null ) roomevent.StreamNID = roomevent.StreamNID.getValue();
            roomevent.StatusCode = messageDataNode.selectSingleNode("//status");
            if( roomevent.StatusCode != null ) roomevent.StatusCode = roomevent.StatusCode.getValue();
            roomevent.EventCode = messageDataNode.selectSingleNode("//eventCode");
            if( roomevent.EventCode != null ) roomevent.EventCode = roomevent.EventCode.getValue();
            if( roomevent.Action == "SourceEvent" ) {
		          roomevent.ActionData = messageDataNode.selectSingleNode("//sourceEventActionData");
            }
            else if( roomevent.Action == "DestEvent") {
              roomevent.ActionData = messageDataNode.selectSingleNode("//destinationEventActionData");
            } 
            if( roomevent.StatusCode == "0" ) {
              roomevent.ErrCode = messageDataNode.selectSingleNode("//log/Code");
              if( roomevent.ErrCode != null ) roomevent.ErrCode = roomevent.ErrCode.getValue();
            }
            else {
              roomevent.ErrCode = null;
            }
            if( roomevent.EventCode == "112" ||
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
            log.info( "Message: MRService: Agent Message: Dest Stream Agent: " + MessageType + ": " + roomevent.Action +
                    ": ToJID: " + roomevent.ToJID +
                    ": RoomNID: " + roomevent.RoomNID +
                    ": StreamNID: " + roomevent.StreamNID +
                    ": StatusCode: " + roomevent.StatusCode + 
                    ": EventCode: " + roomevent.EventCode + 
                    ": ErrCode: " + roomevent.ErrCode);
            topic_Publish(tApp, "ROOMEVENT", roomevent);
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
function handleNotification_MediaStoreAdminClientAgentMessage(ToJID, FromJID, RoomNID, MessageType, messageDataNode) {
  var retval = false;
  if( messageDataNode == null ) return retval;

  if(MessageType == "Request" || MessageType == "MediaStoreAdminResponseMessage") {
    if( RoomNID != null ) {
      var roomevent = new Object;
      roomevent.ToJID = ToJID;
      roomevent.RoomNID = RoomNID; 
         
      //MessageType ==MediaStoreAdminResponseMessage
      roomevent.Action = messageDataNode.selectSingleNode("//action").getValue();
      switch(roomevent.Action) {
        case "CreateFile":
        case "DeleteFile":
        case "UpdateFile":
          roomevent.FileNID = messageDataNode.selectSingleNode("//fileNID").getValue();
          log.info( "Message: MRService: Agent Message: MediaStoreAdminClient Agent: " + MessageType + ": " + roomevent.Action +
                    ": ToJID: " + roomevent.ToJID +
                    ": FileNID: "  + roomevent.FileNID);
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
function handleNotification_FileTransferAgentMessage(ToJID, FromJID, RoomNID, MessageType, messageDataNode) {
  var retval = false;
  if( messageDataNode == null ) return retval;

  if( MessageType == "Request" ) {
    //TBD ???
    retval = true;
  }
  else if( MessageType == "Response" ) {
    // response will be in case of setup and stop
    // check if any error else do nithing
    var stateNode = messageDataNode.selectSingleNode(".//state");
    if( !jsx3.util.strEmpty(stateNode) ){
      var stateCode = stateNode.getValue();
      if( stateCode != "8" ){
        var filexfervent = new Object;
        filexfervent.ToJID = ToJID;
        filexfervent.eventCode = "Stopped";
        var requestName = messageDataNode.selectSingleNode(".//requestname").getValue(); 
        if( requestName == "FileXferStopRequest" ){
          filexfervent.event = "File transfer stop request failed:";
          filexfervent.Action = "FileXferStopErrorEvent";
        }
        else if( requestName == "FileXferSetupRequest" ) {
          filexfervent.event = "File transfer setup request failed:";
          filexfervent.Action = "FileXferSetupErrorEvent";
        }
        
        filexfervent.FileXferID = messageDataNode.selectSingleNode("//FileXferID");
        if( filexfervent.FileXferID != null ) filexfervent.FileXferID = filexfervent.FileXferID.getValue();     
        filexfervent.errorCode = messageDataNode.selectSingleNode("//log/Code");
        if( filexfervent.errorCode != null ) filexfervent.errorCode = filexfervent.errorCode.getValue();         
        filexfervent.errorDescription = messageDataNode.selectSingleNode("//log/Description");
        if( filexfervent.errorDescription != null ) filexfervent.errorDescription = filexfervent.errorDescription.getValue();     
        
        log.info( "Message: MRService: Agent Message: FileXfer Agent: " + MessageType + ": FileXferEventData : errorCode: " + filexfervent.errorCode );
        topic_Publish(tApp, "FILEXFEREVENT", filexfervent);                
      }
    }
    retval = true;
  } 
  else if( MessageType == "Event" ) {
    var filexfervent = new Object;
    filexfervent.ToJID = ToJID;
    //filexfervent.RoomNID = RoomNID;
    var FileXferEventDataNode = messageDataNode.selectSingleNode("//FileXferEventData");
    if( FileXferEventDataNode != null ) {
      filexfervent.Action = "FileXferEvent";
      filexfervent.FileXferID = FileXferEventDataNode.selectSingleNode("//FileXferID");
      if( filexfervent.FileXferID != null ) filexfervent.FileXferID = filexfervent.FileXferID.getValue();

      filexfervent.PercentageComplete = FileXferEventDataNode.selectSingleNode("//percentageComplete");
      if( filexfervent.PercentageComplete != null ) filexfervent.PercentageComplete = filexfervent.PercentageComplete.getValue();

      filexfervent.State = FileXferEventDataNode.selectSingleNode("//state");
      if( filexfervent.State != null ) filexfervent.State = filexfervent.State.getValue();

      filexfervent.eventCode = FileXferEventDataNode.selectSingleNode("//eventCode");
      if( filexfervent.eventCode != null ) filexfervent.eventCode = filexfervent.eventCode.getValue();

      log.info( "Message: MRService: Agent Message: FileXfer Agent: " + MessageType + ": FileXferEventData : EventCode: " + filexfervent.eventCode );
      topic_Publish(tApp, "FILEXFEREVENT", filexfervent);                    
      retval = true;
    }
  }  
  return retval;
}


//Windowing Notification forwarded to the UnManned Client
function handleNotification_DeviceAdminServerAgent(MessageJID, FromJID, AgentMessageType, AgentMessageDataNode){
  var retval = false;
  if( AgentMessageDataNode == null ) return retval;
  if( AgentMessageType == "Request" ) {
    var requestName =AgentMessageDataNode.selectSingleNode(".//header/requestname").getValue();
    if(requestName =="UpgradeDeviceRequest"){
      var nodeUpgradeDeviceRequestData = AgentMessageDataNode.selectSingleNode(".//data/UpgradeDeviceRequestData");
      log.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: UpgradeDeviceRequest: " + nodeUpgradeDeviceRequestData);
      var nodeUpgradeDevice = nodeUpgradeDeviceRequestData.selectSingleNode(".//UpgradeDevice");
      topic_Publish(tApp,"UpgradeClientSoftware", nodeUpgradeDevice);
    }
    else{
      var nodeDeviceConfigRequestData = AgentMessageDataNode.selectSingleNode(".//data/DeviceConfigurationRequestData");
      if(nodeDeviceConfigRequestData == null) return retval;
      var nodeDisplayConfig = nodeDeviceConfigRequestData.selectSingleNode(".//DeviceConfig/AVCodecElementsConfig/IPVSClientDisplayConfig");
      if(nodeDisplayConfig!= null){
         // Forward to Display Agent of UnManned client.
        log.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: DisplayConfigUpdate: " + (nodeDisplayConfig.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        var clientdata = AgentMessageDataNode.selectSingleNode(".//header/clientdata");
        var userJID = AgentMessageDataNode.selectSingleNode(".//header/userJID");
        var requestNID = AgentMessageDataNode.selectSingleNode(".//header/requestNID");
        
        var clientInfoObj = new Object;
        clientInfoObj.clientdata = clientdata;
        clientInfoObj.userJID = userJID;
        clientInfoObj.requestNID = requestNID;

        var notifyDisplayConfigObj = new Object;
        notifyDisplayConfigObj.clientInfoObj = clientInfoObj;
        notifyDisplayConfigObj.nodeDisplayConfig = nodeDisplayConfig;
        topic_Publish(tApp, "DisplayConfigUpdate", notifyDisplayConfigObj); 
         retval = true;
       }
      var nodePortWindowContent = nodeDeviceConfigRequestData.selectSingleNode(".//SetPortWindowContent");
      if(nodePortWindowContent !=null){
        // Forward to Display Agent of UnManned client.
        log.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: WindowContentUpdate: " + (nodePortWindowContent.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        topic_Publish(tApp,"WindowContentUpdate", nodePortWindowContent); 
        retval = true;
      }
      var nodePortWindowVisible = nodeDeviceConfigRequestData.selectSingleNode(".//SetPortWindowVisible");
      if(nodePortWindowVisible !=null){
        // Forward to Display Agent of UnManned client.
        log.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message: WindowVisibleUpdate: " + (nodePortWindowVisible.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        topic_Publish(tApp,"WindowVisibleUpdate", nodePortWindowVisible); 
        retval = true;
       }
      //unwrap and notify 'SetPortWindowProperties'
      var nodePortWindowProperties = nodeDeviceConfigRequestData.selectSingleNode(".//SetPortWindowProperties");
      if (nodePortWindowProperties != null) {
        // Forward to Display Agent of UnManned client.
        log.info("handleNotification_DeviceAdminServerAgent: MRService: Agent Message:WindowPropertiesUpdate: " + (nodePortWindowProperties.toString()).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
        var clientdata = AgentMessageDataNode.selectSingleNode(".//header/clientdata");
        var userJID = AgentMessageDataNode.selectSingleNode(".//header/userJID");
        var requestNID = AgentMessageDataNode.selectSingleNode(".//header/requestNID");

        var clientInfoObj = new Object;
        clientInfoObj.clientdata = clientdata;
        clientInfoObj.userJID = userJID;
        clientInfoObj.requestNID = requestNID;

        var notifyWinPropObj = new Object;
        notifyWinPropObj.clientInfoObj = clientInfoObj;
        notifyWinPropObj.nodePortWindowProperties = nodePortWindowProperties;
        topic_Publish(tApp, "WindowPropertiesUpdate", notifyWinPropObj);
        retval = true;
      }
    }
  }
  
  return retval; 
}