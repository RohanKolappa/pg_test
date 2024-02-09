//-----------------------
// base class to send a sequence of requests
function RequestSeq () {
  
  // initialize
  this.initRequestSeq = function() {
    // create a topic
    this.tReqSeq = "RequestSeq_" + jsx3.xml.CDF.getKey();
    topic_Create(this.tReqSeq);
    // create an event listner
    this.eventReqSeq = new EventListner(this.tReqSeq);
    
    // set initial state
    this.State = "NONE";
  }

  // cleanup
  this.endRequestSeq = function() {
    // destroy any eventlistners
    this.eventReqSeq.destroy();
    this.eventReqSeq = null;
    // destroy the local topic
    topic_Destroy(this.tReqestSeq);
  }
  
  // extend sendrequest
  this.startRequestSeq = function(callbackContext, successFnc, errorFnc) {
    // setup call back functions
    this.callbackContext = callbackContext;
    this.successFnc = successFnc;
    this.errorFnc = errorFnc;
    
    this.stateMachine("START");
  }

  // minimal state machine that must be implenented
  // should be redefined by every class
  this.stateMachine = function(nextState) {
    this.State = nextState;
    switch(nextState) {
      case "START":
        break;
      case "SUCCESS":
        // do success callback
        this.successFnc.call(this.callbackContext, this.tCollab);
        break;
      case "ERROR":
        // do error callback
        this.errorFnc.call(this.callbackContext, this.tCollab);
        break;
      default:
        break;
    }
  }
  
}
//-----------------------

//---------------------------------------
// fill any missing entries - relay / profiles
// create any missing files - pvr / record
// send CreateCollaborationRequest
ReqSeqCreateCollaboration.prototype = new RequestSeq();
function ReqSeqCreateCollaboration(tCollab, objCollab) {
// setup inheritence and call base constructor
  ReqSeqCreateCollaboration.BaseClass = ReqSeqCreateCollaboration.prototype;
  ReqSeqCreateCollaboration.prototype.constructor = ReqSeqCreateCollaboration;
  // pass input args to callback args arr as needed
  RequestSeq.prototype.constructor.call(this);

  // initialize the reqseq
  this.initRequestSeq();

  // stash caller event
  this.tCollab = tCollab;
  // and collab config node 
  this.nodeCollab = objCollab.nodeCollab;

  //  event listner for error msg collection
  this.msgRequestSeq = null;
  this.eventReqSeq.subscribe("ERRORSTATUS", null, this,
      function(EventVar, EventVal) {
        topic_Publish(tCollab, "CollaborationEventMsg", "..." +EventVal+ "...");
      });
      
  // setup state machine behind event listner
  this.eventReqSeq.subscribe("ConnectionReqState", null, this,
                              function(EventVar, EventVal) {
                                this.stateMachine(EventVal);
                              });
  
  // redefine state machine    
  this.stateMachine = function(nextState, arguments) {
    this.State = nextState;
    log.info("CtrlAgent: CreateCollaboration: " + this.tCollab + ": State: " + this.State + "\n");
    switch (nextState) {
      case "START":
        topic_Publish(tCollab, "CollaborationEventMsg", "\nCreateCollaboration ...START");
        // add conn entries for PVR - if needed
        var enablePVR = this.nodeCollab.selectSingleNode(".//PVRInfo/Enable").getValue();
        if( enablePVR == "true" ) {
          // go thru all stream connections
          var iterConn = this.nodeCollab.selectNodeIterator(".//Connection[@type='STREAM']");
          while( iterConn.hasNext() ) {
            var nodeConn = iterConn.next();
            var srcEntityId = nodeConn.selectSingleNode(".//sourceEntityID").getValue();
            // check if this is a file source
            var srcEntityNID = this.nodeCollab.selectSingleNode(".//Entity[@id='"+srcEntityId+"']//resourceNID").getValue();
            // get entitynode 
            var nodeEntityResource = this.nodeCollab.selectSingleNode(".//Entity[@id='"+ srcEntityId +"']");
            var entityObjectType = nodeEntityResource.getAttribute("objectType");
            // if file - no PVR conn needed
            if( entityObjectType == "MediaStreamFileResource" ) {
              continue;
            }
            // regular port src - look for a PVR conn that has this as a source entity
            var nodePVRConn = this.nodeCollab.selectSingleNode(".//Connection[(@type='PVR')" +
                                                               " and .//sourceEntityID='"+srcEntityId+"']");
            // if found - move on
            if( nodePVRConn != null ) {
              continue;
            }
            // add PVR MediaDestination  enity
            var entityNode = getNewEntityNode("MediaDestination", "PVR");
            // get the streamtype for the srcEntityId
            var objEntityNode = this.nodeCollab.selectSingleNode(".//Entity[@id='"+ srcEntityId +"']");
            var objstreamType = objEntityNode.getAttribute("streamType");
            // update the streamType
            entityNode.setAttribute("streamType", objstreamType);
            
            // get the connection type of record with the same srcentity id. From that get the destination entity id
            // with the destination entity id get the entity node and from that get the mediastoreport and recording name
            var nodeRECORDConn = this.nodeCollab.selectSingleNode(".//Connection[(@type='RECORD')" +
                                                                   " and .//sourceEntityID='"+srcEntityId+"']");            
            // check if recording is enabled if yes then get the values else go ahead
            if(!jsx3.util.strEmpty(nodeRECORDConn)) {
              var destinationEntityID = nodeRECORDConn.selectSingleNode(".//destinationEntityID").getValue();
              var destEntityNode = this.nodeCollab.selectSingleNode(".//Entity[@id='"+ destinationEntityID +"']");
              var resNode = destEntityNode.selectSingleNode(".//resourceNID");
              var mediaStorePortResourceNID = resNode.getAttribute("MediaStorePortResourceNID");
              var recodingName = resNode.getAttribute("RecordingName");
             
              // get the resourceNID node of the entityNode
              var entityResourceNode = entityNode.selectSingleNode(".//resourceNID");
              // update the MediaStorePortResourceNID
              entityResourceNode.setAttribute("MediaStorePortResourceNID", mediaStorePortResourceNID);
              // update the RecordingName
              entityResourceNode.setAttribute("RecordingName", recodingName);                        
            }
            
            // append entity to list
            var nodeEntityList = this.nodeCollab.selectSingleNode(".//EntityList");
            nodeEntityList.appendChild(entityNode);
            
            // get dest entity id for putting in new pvr conn 
            var destEntityId = entityNode.getAttribute("id");
            // get the grp id and name from the stream conn node
            var connGrpId = nodeConn.getAttribute("gid");
            var connName = nodeConn.selectSingleNode(".//name").getValue();
            // get a new conn node for pvr
            nodePVRConn = getNewConnectionNode(connGrpId, "PVR", srcEntityId, destEntityId, connName);
            // append it to the list
            var nodeConnList = this.nodeCollab.selectSingleNode(".//ConnectionList");
            nodeConnList.appendChild(nodePVRConn);
          }
        }
        this.stateMachine("CREATEFILE");
        break;
      case "GETRELAYPROFILE":
        topic_Publish(tCollab, "CollaborationEventMsg", "\nChecking Relay / Profile Info ...");
        // check collab node for relay/ profile info
        //var connNode = this.nodeCollab.selectSingleNode(".//Connection[.//profileNID='']");
        var connNode = this.nodeCollab.selectSingleNode(".//Connection[.//profileXML='']");
        var connLen = 1;
        if(connNode != null) {
          var profileXMLNode = connNode.selectSingleNode(".//profileXML");
          connLen = profileXMLNode.getChildNodes().getLength();
        }
        if( connLen == 0 ) {
          // setup and send request to get relay / profile
          var connId = connNode.getAttribute("id");
          topic_Set(this.tReqSeq, "ConnID", connId);
          // get source nid
          var sourceEntityId = connNode.selectSingleNode(".//sourceEntityID").getValue();
          var resourceNID = this.nodeCollab.selectSingleNode(".//Entity[@id='"+sourceEntityId+"']//resourceNID").getValue();
          topic_Set(this.tReqSeq, "SourceNID", resourceNID);
          var destEntityId = connNode.selectSingleNode(".//destinationEntityID").getValue();
          var destNID = "";
          if( (destEntityId != "") && (destEntityId != null) ) {
            destNID = this.nodeCollab.selectSingleNode(".//Entity[@id='"+destEntityId+"']//resourceNID").getValue();
          }
          // dont put the destNID for Record and PVR connections
          var connType = connNode.getAttribute("type");
          if( (connType == "RECORD") || (connType == "PVR") ) {
            topic_Set(this.tReqSeq, "DestNID", "");
          } else {
            var destEntityId = connNode.selectSingleNode(".//destinationEntityID").getValue();
            var destNID = "";
            if( (destEntityId != "") && (destEntityId != null) ) {
              var nodeDstEntity = this.nodeCollab.selectSingleNode(".//Entity[@id='"+destEntityId+"']");
              destNID = ( nodeDstEntity.getAttribute("role") == "MediaSource" ? 
                              nodeDstEntity.selectSingleNode(".//resourceNID").getValue() :
                              nodeDstEntity.selectSingleNode(".//resourceNID").getValue() );
            }
            topic_Set(this.tReqSeq, "DestNID", destNID);
          }
         // topic_Set(this.tReqSeq, "DestNID", destNID);
          topic_Publish(tCollab, "CollaborationEventMsg", "ConnID: " +connId+ " ");
          cmpReq_GetRelayProfile(this.tReqSeq);
        }
        else {
          // done with all relay profile - go ahead and create collab
          topic_Publish(tCollab, "CollaborationEventMsg", " ...Done");
          this.stateMachine("CREATECOLLABORATION");
        }
        break;
      case "GETRELAYPROFILE_SUCCESS": 
        // for the current connid
        var connId = topic_Get(this.tReqSeq, "ConnID");
        var connNode = this.nodeCollab.selectSingleNode(".//Connection[@id='"+connId+"']");
        // get / set the relaynid
        var relayNID = topic_Get(this.tReqSeq, "RelayNID");
        connNode.selectSingleNode(".//relayNID").setValue(relayNID);
        // get / set the profileNID
        var profileNID = topic_Get(this.tReqSeq, "ProfileNID");
        connNode.selectSingleNode(".//profileNID").setValue(profileNID);
        // get / set the profileXML
        var profileXMLNode = topic_GetNode(this.tReqSeq, "ProfileXML");
        connNode.selectSingleNode(".//profileXML").appendChild(profileXMLNode.clone(true));
        // clear out current connId
        topic_Set(this.tReqSeq, "ConnID", "NONE");
        // fire start collab again
        topic_Publish(tCollab, "CollaborationEventMsg", " ...Done");
        this.stateMachine("GETRELAYPROFILE");
        break;
      case "GETRELAYPROFILE_FAIL":
        topic_Publish(tCollab, "CollaborationEventMsg", " ...Error");
        this.stateMachine("ERROR");
        break;
      case "GETDESTDIR":
        var entityId = topic_Get(this.tReqSeq, "EntityID");
        var nodeEntity = this.nodeCollab.selectSingleNode(".//Entity[@id='"+entityId+"']");
        var streamMode = ( nodeEntity.selectSingleNode(".//resourceNID").getValue() );
        var destDirXMLNodeName = (streamMode == "PVR" ? "PVRDestDirXML" : "DestDirXML");
        // check if there is a dest dir nid in the collab
        var destDirNID = this.nodeCollab.selectSingleNode(".//MediaStreamDirResourceNID").getValue();
        // if so - check if the node is in the cache
        if( destDirNID != "" ) {
          var destDirNode = topic_GetNIDNode(tCache, destDirNID);
          // if so, get it and use it for add file
          if( destDirNode != null ) {
            var destDirXMLNode = topic_Set(this.tReqSeq, destDirXMLNodeName, null);
            destDirXMLNode.appendChild(destDirNode.cloneNode(true));
            this.stateMachine("CREATEFILE");
          }
        }
        // otherwise send a get dir request
        else {
          // this needs source NID - get the conn where this is the dest
          var connNode = this.nodeCollab.selectSingleNode(".//Connection[.//destinationEntityID='"+entityId+"']");
          // get the source for that conn
          var sourceEntityId = connNode.selectSingleNode(".//sourceEntityID").getValue();
          // get the NID for that source and put it in the topic
          var nodeSrcEntity = this.nodeCollab.selectSingleNode(".//Entity[@id='"+sourceEntityId+"']");
          var resourceNID = ( nodeSrcEntity.selectSingleNode(".//resourceNID").getValue() );
          topic_Set(this.tReqSeq, "SourceNID", resourceNID);
          // fire get dir
          topic_Publish(tCollab, "CollaborationEventMsg", "...Getting Dest Dir...");
          cmpReq_GetDestDir(this.tReqSeq, streamMode)
        }
        break;
      case "GETDESTDIR_SUCCESS":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
        this.stateMachine("CREATEFILE");
        break;
      case "GETDESTDIR_FAIL":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Error");
        this.stateMachine("ERROR");
        break;
      case "CREATEFILE":
        topic_Publish(tCollab, "CollaborationEventMsg", "\nChecking Record / PVR Info ...");
        // check collab node for pvr / record file info
        // only pick those entities that are being used as a dest in a record / pvr conn
        var nodeEntity = null;
        var mediaStorePortResourceNID = null;
        var iterConn = this.nodeCollab.selectNodeIterator(".//Connection");
        while( iterConn.hasNext() ) {
          var nodeConn = iterConn.next();
          var dstEntityId = nodeConn.selectSingleNode(".//destinationEntityID").getValue();
          nodeEntity = this.nodeCollab.selectSingleNode(".//Entity[@id='"+dstEntityId+"']");
          var entityNID = ( nodeEntity.selectSingleNode(".//resourceNID").getValue() );
          if( entityNID == 'PVR' || entityNID == 'RECORD' ) {
            break;
          }
          else {
            nodeConn = null;
            nodeEntity = null;
          }
        }
        // if there is any RECORD / PVR entity
        if( nodeEntity != null ) {
          var entityId = nodeEntity.getAttribute("id");
          topic_Set(this.tReqSeq, "EntityID", entityId);
          var streamMode = ( nodeEntity.selectSingleNode(".//resourceNID").getValue() );
          var mediaStorePortResourceNID = nodeEntity.selectSingleNode(".//resourceNID").getAttribute("MediaStorePortResourceNID");
          var recordingName = nodeEntity.selectSingleNode(".//resourceNID").getAttribute("RecordingName");
          
          var destDirXMLNodeName = (streamMode == "PVR" ? "PVRDestDirXML" : "DestDirXML");
          // check if dest dir xml is present 
          var destDirXMLNode = topic_GetNode(this.tReqSeq, "//" + destDirXMLNodeName);
          // if yes - then check if media group file nid is set
          if( destDirXMLNode != null && destDirXMLNode.selectSingleNode("./*") != null ) {
            var MediaGroupFileResourceNID = topic_Get(this.tReqSeq, "MediaGroupFileResourceNID");
            // if exists - then create file clips under it
            if( streamMode == "PVR" || MediaGroupFileResourceNID != null ) {
              topic_Publish(tCollab, "CollaborationEventMsg", "...Creating "+streamMode+" File...");
              // create some metatdata
              var destFileMetadataXMLNodeName = (streamMode == "PVR" ? "PVRDestFileMetaDataXML" : "DestFileMetaDataXML");
              var nodeMetadataXML = topic_Set(this.tReqSeq, destFileMetadataXMLNodeName, null);
              nodeMetadataXML.removeChildren();
              var nodeMetaData = loadXMLFile("JSXAPPS/IPVS_ClientUI/xml/defaults/MetaData.xml");
              // put collab nid tag
              var CollaborationNID = this.nodeCollab.getAttribute("NID");
              nodeMetaData.selectSingleNode("//Tag[@Name='CollaborationNID']").setValue(CollaborationNID);
              // put conn id tag
              var nodeConn = this.nodeCollab.selectSingleNode(".//Connection[destinationEntityID='"+entityId+"']");
              if( nodeConn != null ) {
                var connGrpId = nodeConn.getAttribute("gid");
                nodeMetaData.selectSingleNode("//Tag[@Name='ConnectionID']").setValue(connGrpId);
              }
              // finally append all the metadata to the parent
              nodeMetadataXML.appendChild(nodeMetaData);
              // use collab/Recfile name + connname as file title RecordFileName
              var FileTitle = this.nodeCollab.selectSingleNode(".//MediaStreamFileResourceTitle").getValue();
              if( FileTitle == "" ) {
                FileTitle = this.nodeCollab.selectSingleNode(".//collaborationName").getValue();
              }
              FileTitle += "_"+nodeConn.selectSingleNode(".//name").getValue();
              if( recordingName != "" ) {
                FileTitle = recordingName ;
              }
              
              topic_Set(this.tReqSeq, "FileTitle", FileTitle);
              // send add file
              topic_Publish(tCollab, "CollaborationEventMsg", "...Title: "+FileTitle+" ");
              cmpReq_AddDestFile(this.tReqSeq, streamMode, mediaStorePortResourceNID);
            }
            // else create clip group file
            else {
              var streamMode = "RECORDGROUP";
              topic_Publish(tCollab, "CollaborationEventMsg", "...Creating "+streamMode+" File...");
              // create some metatdata
              var destFileMetadataXMLNodeName = "DestGroupFileMetaDataXML";
              var nodeMetadataXML = topic_Set(this.tReqSeq, destFileMetadataXMLNodeName, null);
              nodeMetadataXML.removeChildren();
              var nodeMetaData = loadXMLFile("JSXAPPS/IPVS_ClientUI/xml/defaults/MetaData.xml");
              // put collab nid tag
              var CollaborationNID = this.nodeCollab.getAttribute("NID");
              nodeMetaData.selectSingleNode("//Tag[@Name='CollaborationNID']").setValue(CollaborationNID);
              // put conn id tag
              var syncSourceEntityId = this.nodeCollab.selectSingleNode(".//syncSourceEntityID").getValue();
              if( syncSourceEntityId != "" ) {
                var nodeConn = this.nodeCollab.selectSingleNode(".//Connection[sourceEntityID='"+syncSourceEntityId+"']");
                if( nodeConn != null ) {
                  var connGrpId = nodeConn.getAttribute("gid");
                  nodeMetaData.selectSingleNode("//Tag[@Name='ConnectionID']").setValue(connGrpId);
                }
              }
              // finally append all the metadata to the parent
              nodeMetadataXML.appendChild(nodeMetaData);
              // use collab/rec file name as file title
              var FileTitle = this.nodeCollab.selectSingleNode(".//MediaStreamFileResourceTitle").getValue();
              if( FileTitle == "" ) {
                FileTitle = this.nodeCollab.selectSingleNode(".//collaborationName").getValue();
              }              
              topic_Set(this.tReqSeq, "FileTitle", FileTitle);
              // send add file
              topic_Publish(tCollab, "CollaborationEventMsg", "...Title: "+FileTitle+" ");
              cmpReq_AddDestFile(this.tReqSeq, streamMode, mediaStorePortResourceNID);
            }
          }
          // else get dest dir xml
          else {
            this.stateMachine("GETDESTDIR");
          }
        }
        // no more record / pvr entites - do create collab
        else {
          topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
          this.stateMachine("GETRELAYPROFILE");
        }
        break;
      case "CREATEGROUPFILE_SUCCESS":
        // get file nid from args
        var FileNID = arguments[0];
        // set it as group file nid in topic for other file create to use
        topic_Set(this.tReqSeq, "MediaGroupFileResourceNID", FileNID);
        // stash it in collab node as well
        this.nodeCollab.selectSingleNode(".//RecordingInfo//MediaGroupFileResourceNID").setValue(FileNID);
        // continue to next file create
        topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
        this.stateMachine("CREATEFILE");
        break;    
      case "CREATEFILE_SUCCESS":
        // get file nid from args
        var FileNID = arguments[0];
        // set this as the entity nid
        var entityId = topic_Get(this.tReqSeq, "EntityID");
        var nodeEntity = this.nodeCollab.selectSingleNode(".//Entity[@id='"+entityId+"']");
        nodeEntity.selectSingleNode(".//resourceNID").setValue(FileNID);
        // continue to next file create
        topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
        this.stateMachine("CREATEFILE");
        break;
      case "CREATEFILE_FAIL":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Error");
        this.stateMachine("ERROR");
        break;
      case "CREATECOLLABORATION":
        var ccReqDataNode = topic_Set(this.tReqSeq, "CreateCollaborationRequestData", null);
        var ccNode = this.nodeCollab.cloneNode(true);
        var dataNodes = ccNode.selectNodes("/*");
        for (var it = dataNodes.iterator();it.hasNext();){
          var objRecordNode = it.next();
          ccReqDataNode.appendChild(objRecordNode);                                                    
        }        
        // send the create collaboration
        topic_Publish(tCollab, "CollaborationEventMsg", "\nCreating Media Room ...");
        // strip all the entities with entityType=Resource
        var resourceEntityNodes = ccReqDataNode.selectNodes(".//Entity[@entityType='Resource']");
        for (var it = resourceEntityNodes.iterator();it.hasNext();){
          var entityNode = it.next();
          entityNode.getParent().removeChild(entityNode);
        }         
        cmpReq_CreateCollaboration(this.tReqSeq, ccReqDataNode);
        break;
      case "CREATECOLLABORATION_SUCCESS":
        // call it done
        topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
        this.stateMachine("SUCCESS");
        break;
      case "CREATECOLLABORATION_FAIL":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Error");
        this.stateMachine("ERROR");
        break;
      case "ERROR":
        // do error callback
        topic_Publish(tCollab, "CollaborationEventMsg", "\nCreateCollaboration ...ERROR");
        this.errorFnc.call(this.callbackContext, this.tCollab, this.msgRequestSeq);
        break;
      case "SUCCESS":
        // do success callback
        var roomXMLNode = topic_GetNode(this.tReqSeq, ".//MediaRoomXML");
        topic_Publish(tCollab, "CollaborationEventMsg", "\nCreateCollaboration ...SUCCESS");
        this.successFnc.call(this.callbackContext, this.tCollab, roomXMLNode.cloneNode(true));
        break;
      default:
        break;
    }
  }
  
  this.eventReqSeq.subscribe("DestPVRDirState", null, this,
      function(EventVar, EventVal) {
        log.debug(EventVar + " = " + EventVal + "\n");
        var ConnectionAction = EventVal;
        switch (ConnectionAction) {
          case "SUCCESS":
            this.stateMachine("GETDESTDIR_SUCCESS");
            break;
          case "FAIL":
            topic_Publish(tCollab, "CollaborationEventMsg", "...Failed to Get PVR Dir...");
            this.stateMachine("GETDESTDIR_FAIL");
            break;
          default:
            break;
        }
      });
  
  this.eventReqSeq.subscribe("DestDirState", null, this,
      function(EventVar, EventVal) {
        log.debug(EventVar + " = " + EventVal + "\n");
        var ConnectionAction = EventVal;
        switch (ConnectionAction) {
          case "SUCCESS":
            this.stateMachine("GETDESTDIR_SUCCESS");
            break;
          case "FAIL":
            topic_Publish(tCollab, "CollaborationEventMsg", "\n...Failed to Get Recording Dir...");
            this.stateMachine("GETDESTDIR_FAIL");
            break;
          default:
            break;
        }
      });
  
  this.eventReqSeq.subscribe("DestPVRFileState", null, this,
      function(EventVar, EventVal) {
        log.debug(EventVar + " = " + EventVal + "\n");
        var ConnectionAction = EventVal;
        var arguments = [];
        switch (ConnectionAction) {
          case "SUCCESS":
            var FileNID = topic_Get(this.tReqSeq, "PVRDestFileNID");
            this.stateMachine("CREATEFILE_SUCCESS", [FileNID]);
            break;
          case "FAIL":
            topic_Publish(tCollab, "CollaborationEventMsg", "\n...Failed to Create PVR File...");
            this.stateMachine("CREATEFILE_FAIL");
            break;
          default:
            break;
        }
      });
  this.eventReqSeq.subscribe("DestGroupFileState", null, this,
      function(EventVar, EventVal) {
        log.debug(EventVar + " = " + EventVal + "\n");
        var ConnectionAction = EventVal;
        switch (ConnectionAction) {
          case "SUCCESS":
            var FileNID = topic_Get(this.tReqSeq, "DestFileNID");
            this.stateMachine("CREATEGROUPFILE_SUCCESS", [FileNID]);
            break;
          case "FAIL":
            topic_Publish(tCollab, "CollaborationEventMsg", "\n...Failed to Create Recording Group File...");
            this.stateMachine("CREATEFILE_FAIL");
            break;
          default:
            break;
        }
      });
  this.eventReqSeq.subscribe("DestFileState", null, this,
      function(EventVar, EventVal) {
        log.debug(EventVar + " = " + EventVal + "\n");
        var ConnectionAction = EventVal;
        switch (ConnectionAction) {
          case "SUCCESS":
            var FileNID = topic_Get(this.tReqSeq, "DestFileNID");
            this.stateMachine("CREATEFILE_SUCCESS", [FileNID]);
            break;
          case "FAIL":
            topic_Publish(tCollab, "CollaborationEventMsg", "\n...Failed to Create Recording File...");
            this.stateMachine("CREATEFILE_FAIL");
            break;
          default:
            break;
        }
      });
      
}

//---------------------------------------
ReqSeqConnectionStreamAction.prototype = new RequestSeq();
function ReqSeqConnectionStreamAction(tCollab, objCollab, streamType, streamAction) {
// setup inheritence and call base constructor
  ReqSeqConnectionStreamAction.BaseClass = ReqSeqConnectionStreamAction.prototype;
  ReqSeqConnectionStreamAction.prototype.constructor = ReqSeqConnectionStreamAction;
  // pass input args to callback args arr as needed
  RequestSeq.prototype.constructor.call(this);
  
  // initialize the reqseq
  this.initRequestSeq();

  // stash caller event
  this.tCollab = tCollab;
  // and collab config node
  this.nodeCollab = objCollab.nodeCollab;
  this.nodeMediaRoom = objCollab.nodeMediaRoom;
  
  // Stream Type ALL=STREAM+RECORD, null, RECORD
  this.streamType = streamType;
  // stream Actions Start, Stop
  this.streamAction = streamAction;
  // flg to track all done
  this.flgDone = null;
  
  //  event listner for error msg collection
  this.msgRequestSeq = null;
  this.eventReqSeq.subscribe("ERRORSTATUS", null, this,
      function(EventVar, EventVal) {
        topic_Publish(tCollab, "CollaborationEventMsg", "\n..." +EventVal+ "...");
      });
      
  // setup state machine behind event listner
  this.eventReqSeq.subscribe("ConnectionReqState", null, this,
                              function(EventVar, EventVal) {
                                this.stateMachine(EventVal);
                              });
  
  // redefine state machine    
  this.stateMachine = function(nextState, arguments) {
    this.State = nextState;
    log.info("CtrlAgent: StartCollaboration: " + tCollab + ": State: " + this.State + "\n");
    switch (nextState) {
      case "START":
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStart Connection Stream Action..." + this.streamAction);
        this.stateMachine("ADDSTREAMACTIONSTART");
        break;
      case "ADDSTREAMACTIONSTART":
        // go thru the current collab and start all the streams
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStarting Connection Stream Action...");
        // clear profile info - used by this request
        topic_Set(this.tReqSeq, "ProfileNID", "");
        var profileXMLNode = topic_GetNode(this.tReqSeq, "//ProfileXML");
        if( profileXMLNode != null ) {
          profileXMLNode.removeChildren();
        }
        this.flgDone = false;
        var iterConn = null;

        // connections which are not of RECORD type
        if( this.streamType == null ) {
          iterConn = this.nodeCollab.selectNodeIterator(".//Connection[@type!='RECORD']");
        }
        //  RECORD connections
        else if( this.streamType == "RECORD" ) {
          iterConn = this.nodeCollab.selectNodeIterator(".//Connection[@type='RECORD']");
        }
        // All connections 
        else if( this.streamType == "ALL" ) {
          //iterConn = this.nodeCollab.selectNodeIterator(".//Connection[.//state='8' or .//state='DISCONNECTED']");
          iterConn = this.nodeCollab.selectNodeIterator(".//Connection");
        }
                        
        while( iterConn.hasNext() ) {
          var nodeConn = iterConn.next();
          var connId = nodeConn.getAttribute("id");
          var nodeStream = this.nodeMediaRoom.selectSingleNode(".//Stream[@id='"+connId+"']");
          var StreamNID = nodeStream.getAttribute("NID");
          topic_Set(this.tReqSeq, "StreamNID", StreamNID);
          topic_Publish(tCollab, "CollaborationEventMsg", "\n " +connId+ " ");
          nodeStream.setAttribute("streamState", "START_REQUEST");
          cmpReq_AddStreamAction(this.tReqSeq, StreamNID, this.streamAction);
        }
        this.flgDone = true;
        topic_Publish(tCollab, "CollaborationEventMsg", "\n ...Done");
        this.stateMachine("ADDSTREAMACTIONSTART_DONE");
        break;
      case "ADDSTREAMACTIONSTART_SUCCESS":
        var StreamNID = arguments[0];
        var nodeStream = this.nodeMediaRoom.selectSingleNode(".//Stream[@NID='"+StreamNID+"']");
        if( nodeStream != null ) {
          // update state - but dont clobber if already updated by room event
          if( nodeStream.getAttribute("streamState") == "START_REQUEST" ) {
            nodeStream.setAttribute("streamState", "START_SUCCESS");
          }
          var connId = nodeStream.getAttribute("id");
          topic_Publish(tCollab, "CollaborationEventMsg", "\nConnection: " +connId+ " ..."+ this.streamAction +" Success");
        }
        else {
          topic_Publish(tCollab, "CollaborationEventMsg", "\nUnknown Connection Start Success: " +StreamNID);
        }
        this.stateMachine("ADDSTREAMACTIONSTART_DONE", arguments);
        break;
      case "ADDSTREAMACTIONSTART_FAIL":
        var StreamNID = arguments[0];
        var nodeStream = this.nodeMediaRoom.selectSingleNode(".//Stream[@NID='"+StreamNID+"']");
        if( nodeStream != null ) {
          nodeStream.setAttribute("streamState", "START_ERROR");
          var connId = nodeStream.getAttribute("id");
          topic_Publish(tCollab, "CollaborationEventMsg", "\nConnection: " +connId+ " ...Start Error");
        }
        else {
          topic_Publish(tCollab, "CollaborationEventMsg", "\nUnknown Connection Start Error: " +StreamNID);
        }
        this.stateMachine("ADDSTREAMACTIONSTART_DONE", arguments);
        break;
      case "ADDSTREAMACTIONSTART_DONE":
        // check if all requests are complete
        var flgDone = "SUCCESS";
        var iterConn = this.nodeCollab.selectNodeIterator(".//Connection");
        while( iterConn.hasNext() ) {
          var nodeConn = iterConn.next();
          var connId = nodeConn.getAttribute("id");
          var nodeStream = this.nodeMediaRoom.selectSingleNode(".//Stream[@id='"+connId+"']");
          var connState = nodeStream.getAttribute("streamState");
          if( connState == "START_REQUEST" ) {
            flgDone = null;
          }
          if( connState == "START_ERROR" ) {
            flgDone = "ERROR";
          }
        }
        // check if everything has ben sent and recieved
        if( this.flgDone == true && flgDone != null ) {
          this.stateMachine(flgDone);
        }
        break;
      case "ERROR":
        // do error callback
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStart Connection Stream Action ...ERROR");
        this.errorFnc.call(this.callbackContext, this.tCollab, this.msgRequestSeq);
        break;
      case "SUCCESS":
        // do success callback
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStart Connection Stream Action ...SUCCESS");
        this.successFnc.call(this.callbackContext, this.tCollab);
        break;
      default:
        break;
    }
  }
  
  this.eventReqSeq.subscribe("AddStreamActionState", null, this,
    function(EventVar, EventVal) {
      log.debug(EventVar + " = " + EventVal + "\n");
      var StreamNID = EventVal.replace(/:.*/, "");
      var ConnectionAction = EventVal.replace(/^.*:/, "");
      switch (ConnectionAction) {
        case "Start_SUCCESS":
        case "Stop_SUCCESS":
          this.stateMachine("ADDSTREAMACTIONSTART_SUCCESS", [StreamNID]);
          break;
        case "Start_FAIL":
        case "Stop_FAIL":
          this.stateMachine("ADDSTREAMACTIONSTART_FAIL", [StreamNID]);
          break;
        default:
          break;
      }
    });
}

//---------------------------------------
ReqSeqStopCollaboration.prototype = new RequestSeq();
function ReqSeqStopCollaboration(tCollab, objCollab) {
// setup inheritence and call base constructor
  ReqSeqStopCollaboration.BaseClass = ReqSeqStopCollaboration.prototype;
  ReqSeqStopCollaboration.prototype.constructor = ReqSeqStopCollaboration;
  // pass input args to callback args arr as needed
  RequestSeq.prototype.constructor.call(this);

  // initialize the reqseq
  this.initRequestSeq();

  // stash caller event
  this.tCollab = tCollab;
  // and collab config / mediaroom info
  this.nodeCollab = objCollab.nodeCollab;
  this.nodeMediaRoom = objCollab.nodeMediaRoom;
  this.MediaRoomNID = objCollab.MediaRoomNID;
  
  //  event listner for error msg collection
  this.msgRequestSeq = null;
  this.eventReqSeq.subscribe("ERRORSTATUS", null, this,
      function(EventVar, EventVal) {
        topic_Publish(tCollab, "CollaborationEventMsg", "..." +EventVal+ "...");
      });
      
  // setup state machine behind event listner
  this.eventReqSeq.subscribe("ConnectionReqState", null, this,
                              function(EventVar, EventVal) {
                                this.stateMachine(EventVal);
                              });
  
  // redefine state machine    
  this.stateMachine = function(nextState, arguments) {
    this.State = nextState;
    log.info("CtrlAgent: StopCollaboration: " + tCollab + ": State: " + this.State + "\n");
    switch (nextState) {
      case "START":
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStopCollaboration ...START");
        this.stateMachine("DELETEMEDIAROOM");
        break;
      case "DELETEMEDIAROOM":
        topic_Publish(tCollab, "CollaborationEventMsg", "\nChecking Media Room ...");
        // check if mediaroom exists
        if( this.nodeMediaRoom != null ) {
          // go thru the current media room and start all the streams
          // set mediaroom nid - used by this request
          topic_Set(this.tReqSeq, "RoomNID", this.MediaRoomNID);
          topic_Publish(tCollab, "CollaborationEventMsg", "Deleting ... ");
          cmpReq_DeleteMediaRoom(this.tReqSeq);
        }
        else {
          this.stateMachine("DELETEMEDIAROOM_SUCCESS");
        }
        break;
      case "DELETEMEDIAROOM_FAIL":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Error");
        this.stateMachine("DELETEDESTFILE");
        break;
      case "DELETEMEDIAROOM_SUCCESS":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
        this.stateMachine("DELETEDESTFILE");
        break;
      case "DELETEDESTFILE":
        // check if there are any PVR files to be deleted
        var iterPVRConnection = this.nodeCollab.selectNodeIterator(".//Connection[@type='PVR']");
        var deletedPVRFiles = true;
        while( iterPVRConnection.hasNext() ) {
          var nodePVRConnection = iterPVRConnection.next();
          var destinationEntityID = nodePVRConnection.selectSingleNode(".//destinationEntityID").getValue();
          var nodePVREntity = this.nodeCollab.selectSingleNode(".//Entity[@id='"+ destinationEntityID +"']");
          if(jsx3.util.strEmpty(nodePVREntity)) continue;
          var PVRFileNID = nodePVREntity.selectSingleNode(".//resourceNID").getValue();
          if( PVRFileNID != "PVR" ) {
            deletedPVRFiles = false;
            // reset the pvr file NID in the entity
            nodePVREntity.selectSingleNode(".//resourceNID").setValue("PVR");
            // set it in local topic for delete dest file req
            topic_Set(this.tReqSeq, "PVRDestFileNID", PVRFileNID);
            topic_Publish(tCollab, "CollaborationEventMsg", "\nDeleting PVR File ...");
            cmpReq_DeleteDestFile(this.tReqSeq);
          }
          
        }
        if( deletedPVRFiles == true ) {
          this.stateMachine("SUCCESS");
        }
        break;
      case "DELETEDESTFILE_FAIL":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Error");
        this.stateMachine("DELETEDESTFILE");
        break;
      case "DELETEDESTFILE_SUCCESS":
        topic_Publish(tCollab, "CollaborationEventMsg", "...Done");
        this.stateMachine("DELETEDESTFILE");
        break;
      case "ERROR":
        // do error callback
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStopCollaboration ...ERROR");
        this.errorFnc.call(this.callbackContext, this.tCollab, this.msgRequestSeq);
        break;
      case "SUCCESS":
        // do success callback
        topic_Publish(tCollab, "CollaborationEventMsg", "\nStopCollaboration ...SUCCESS");
        this.successFnc.call(this.callbackContext, this.tCollab);
        break;
      default:
        break;
    }
  }
}

