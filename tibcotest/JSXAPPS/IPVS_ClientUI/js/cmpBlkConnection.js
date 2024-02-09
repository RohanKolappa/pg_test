function cmpBlkConnection_Create(winCollaboration, connId, repaintFlag) {

  // create a dynamic topic based on connId
  var tConn = "Connection_";
  if( connId == null ) {
    tConn += jsx3.xml.CDF.getKey();
  }
  else {
    tConn += connId;
  }
  
  var tConnDoc = topic_Create(tConn);
  var eventConn = new EventListner(tConn);
  
  function cmpBlkConnection_Destroy(cmpBlkConnection) {
    // get a handle to the current BlkJSX
    var ConnectionBlkJSX = cmpBlkConnection.BlkJSX;
    var ConnectionListBlkJSX = ConnectionBlkJSX.getParent();
    // destroy any eventlistners
    eventConn.destroy();
    eventConn = null;
    // destroy the local topic
    tConnDoc = null;                                            
    topic_Destroy(tConn);
   // finally destroy the DOM element
    ConnectionListBlkJSX.removeChild(ConnectionBlkJSX);
    ConnectionListBlkJSX.repaint();
  }
  
  
  // get vars from object instance
  var winCollaborationBlkJSX = winCollaboration.BlkJSX;
  var tCollaboration = winCollaboration.tWin;
  var eventConCollaboration = winCollaboration.eventWin;
                    
  // clone a connection block
  var orgConnectionBlkJSX = getJsxByName([winCollaborationBlkJSX, "blkConnectionNew"]);
  var newConnectionBlkJSX = orgConnectionBlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
  // attach it to the connection list
  var ConnectionListBlkJSX = getJsxByName([winCollaborationBlkJSX, "blkConnectionsList"]);
  if(repaintFlag){ 
    var blkConnListchildren = ConnectionListBlkJSX.getChildren();
    ConnectionListBlkJSX.insertBefore(newConnectionBlkJSX,blkConnListchildren[5],true);
    //ConnectionListBlkJSX.repaint();
  }
  else {
    ConnectionListBlkJSX.adoptChild(newConnectionBlkJSX, false);
  }

  topic_Set(tConn, "ShowRecordingEnabled", "ENABLED"); 
   
  // create instances for all the components inside this conn block
                                                                                        
  // delete conn button                                      
  var cmpRemoveConnection = new CmpImageButton([newConnectionBlkJSX,"blkConnectionHeader","btnRemoveConnection"],
                                                null,
                                                {action: "FNCCALL",
                                                 context: this,
                                                 fnc: function() {
                                                        // remove data from collab config xml
                                                        winCollaboration.sessionConfig.delConnGroup(cmpBlkConnection.ConnectionID);
                                                        // notify UI
                                                        topic_Publish(tCollaboration, "CollaborationDataDelConnectionGroup", cmpBlkConnection.ConnectionID);
                                                        // destroy all local components
                                                       // cmpBlkConnection.destroy();
                                                      }
                                                }
                                              );
  
  var cmpBlkConnectionHeader = new CmpContainer([newConnectionBlkJSX,"blkConnectionHeader"],
                                                [{cmp:cmpRemoveConnection}],
                                                null
                                               );

  // Rec  toggles
  var cmpConnectionRec = new CmpImageButton([newConnectionBlkJSX, "blkSourceMediaName", "btnEnableStreamRec"],
                                            null,
                                            {action: "FNCCALL",
                                             context: this,
                                             fnc: function() {
                                                    var objBlkState = cmpConnectionRec.BlkJSX.getState();
                                                                                                        
                                                    if( objBlkState == "1" ) {
                                                      var srcName = topic_Get(tConn, "srcName");
                                                      
                                                      //var recordingName = "RECORD_"+cmpBlkConnection.ConnectionID;
                                                      var recordingName = srcName;
                                                      var objFile = new Object();
                                                      objFile.fileTitle = recordingName;
                                                      var dirNID = cmpSelDirList.BlkJSX.getValue();
                                                      if( !jsx3.util.strEmpty(dirNID) && dirNID != "NONE" ) {
                                                        objFile.dirNID = dirNID;
                                                      }
                                                      // check if any Record connection is present for this cmpBlkConnection.ConnectionID
                                                      var recordConnNode = winCollaboration.sessionConfig.getRecordConnByGrpId(cmpBlkConnection.ConnectionID);
                                                      //if not found connNode then add otherwise update
                                                      if( jsx3.util.strEmpty(recordConnNode) ) {
                                                        if(!winCollaboration.sessionConfig.updateConnGroupAddRec(cmpBlkConnection.ConnectionID, objFile)){
                                                          windowAlert(winCollaborationBlkJSX, "Cannot enable recording for the selected source.");
                                                          return false;
                                                        }
                                                      }
                                                      else {
                                                        var connId = winCollaboration.sessionConfig.getConnId(recordConnNode);
                                                        winCollaboration.sessionConfig.updateConnGroupRecInfo(cmpBlkConnection.ConnectionID, connId, objFile);
                                                      }
                                                    }
                                                    else if( objBlkState == "0" ) {
                                                      winCollaboration.sessionConfig.updateConnGroupDelRec(cmpBlkConnection.ConnectionID, null);
                                                    }
                                                  } // end fnc
                                            }
                                          );
  var cmpSelDirList = new CmpSelect([newConnectionBlkJSX, "selRecDstDirectory"],
                                    null,
                                    { persist: { col: "jsxid", initval: false },
                                      action: "TOPICEVENT", topic: tConn, eventvar: "DstRecDirSelected", eventval: "jsxid"
                                    }
                                   );
                                          
  var cmpImgSrcType = new Component([newConnectionBlkJSX, "blkSourceMediaName", "imgSrcEntityType"], null); 

  var cmpLblSrcName = new CmpLabel(([newConnectionBlkJSX, "blkSourceMediaName", "lblSourceMedia"]),
                                       null,
                                       [{action:"TOPICEVENT", topic:tConn, eventvar:"srcName", eventval:null}]);

  // src drop block cmp
  var cmpBlkSrc = new CmpBlkDropTarget([newConnectionBlkJSX, "blkConnectionSrc"],
                                         null,
                                         { action: "FNCCALL",
                                           context: this,
                                           fnc: function (EventVal) {
                                             var objSOURCE = EventVal.objSOURCE;
                                             var strDRAGID = EventVal.strDRAGID;
                                             var strDRAGTYPE = EventVal.strDRAGTYPE;
                                             // check source of dropped item
                                             var dropFrom = objSOURCE.jsxname;
                                             if (dropFrom != "mtxSessionSources") {
                                               windowAlert(winCollaborationBlkJSX, "Cannot drop the selected record");
                                               return false;
                                             }
                                             // get entity ID of dropped item
                                             var entityId = strDRAGID;
                                             // check if source is available
                                             if (!winCollaboration.sessionConfig.isSrcEntityAvailable(entityId)) {
                                               return false;
                                             }
                                             var connNode = winCollaboration.sessionConfig.getConnByGrpId(cmpBlkConnection.ConnectionID);
                                             if (!jsx3.util.strEmpty(connNode)) {
                                               var dstEntityID = winCollaboration.sessionConfig.getConnDstEntityId(connNode);
                                               var dstEntityObj = winCollaboration.sessionConfig.getEntityObj(dstEntityID);
                                               var srcEntityObj = winCollaboration.sessionConfig.getEntityObj(entityId);
                                               // for IO prevent port connecting to itself
                                               if (!jsx3.util.strEmpty(srcEntityObj) && !jsx3.util.strEmpty(dstEntityObj) && (srcEntityObj.portNID == dstEntityObj.portNID)) {
                                                 windowAlert(winCollaborationBlkJSX, "Please select a different Source or Destination.Cannot connect the Media Source Destination to itself");
                                                 return false;
                                               }
                                             }
                                             //update the record entity if any
                                             // check if any Record connection is present for this cmpBlkConnection.ConnectionID
                                             var recordConnNode = winCollaboration.sessionConfig.getRecordConnByGrpId(cmpBlkConnection.ConnectionID);
                                             if (!jsx3.util.strEmpty(recordConnNode)) {
                                               var objFile = new Object();
                                               objFile.fileTitle = winCollaboration.sessionConfig.getEntityObj(entityId).name;
                                               var dirNID = cmpSelDirList.BlkJSX.getValue();
                                               if (!jsx3.util.strEmpty(dirNID) && dirNID != "NONE") {
                                                 objFile.dirNID = dirNID;
                                               }
                                               var connId = winCollaboration.sessionConfig.getConnId(recordConnNode);
                                               winCollaboration.sessionConfig.updateConnGroupRecInfo(cmpBlkConnection.ConnectionID, connId, objFile);
                                             }

                                             // update connection with entity
                                             if (winCollaboration.sessionConfig.updateConnGroupSrc(cmpBlkConnection.ConnectionID, entityId)) {
                                               cmpBlkConnection.getData();
                                             }
                                           }
                                         }
                                       );
  //cmpBlkSrc.ConnectionID = connId;
  
  var cmpBlkConnectionSrc = new CmpContainer([newConnectionBlkJSX,"blkConnectionSrc"],
                                              [{cmp:cmpBlkSrc},
                                               { cmp: cmpConnectionRec},
                                               { cmp: cmpSelDirList},
                                               {cmp:cmpImgSrcType},
                                               {cmp:cmpLblSrcName}],
                                              [{ action: "TOPICEVENT", topic: tConn, eventvar: "ShowRecordingEnabled", eventval: null}]
                                              );

                                              
  // new dst drop block 
  var cmpBlkConnectionDstList = new CmpBlkDropTarget([newConnectionBlkJSX, "blkConnectionDstList"],
                                                     null,
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function(EventVal) {
                                                         var objSOURCE = EventVal.objSOURCE;
                                                         var strDRAGID = EventVal.strDRAGID;
                                                         var strDRAGTYPE = EventVal.strDRAGTYPE;
                                                         // check source of dropped item
                                                         var dropFrom = objSOURCE.jsxname;
                                                         if (dropFrom != "mtxSessionDestinations") {
                                                           windowAlert(winCollaborationBlkJSX, "Cannot drop the selected record");
                                                           return false;
                                                         }
                                                         // simpler way to do get source record
                                                         var dropDestRecordNode = objSOURCE.getRecord(strDRAGID);
                                                         var dropDestRole = dropDestRecordNode.jsxRole;
                                                         // if its of type record then dont drop on the destination area
                                                         if (dropDestRole == "RecFile") {
                                                           windowAlert(winCollaborationBlkJSX, "Cannot drop the selected record");
                                                           return false;
                                                         }
                                                         
                                                         // get entity ID of dropped item
                                                         var entityId = strDRAGID;;
                                                         
                                                         // check if destination is available
                                                         if(!winCollaboration.sessionConfig.isDestEntityAvailable(entityId)){
                                                           return false;
                                                         }                                                         
                                                         var connNode = winCollaboration.sessionConfig.getConnByGrpId(cmpBlkConnection.ConnectionID);
                                                         if(!jsx3.util.strEmpty(connNode) ) {
                                                           var srcEntityID = winCollaboration.sessionConfig.getConnSrcEntityId(connNode);
                                                           var srcEntityObj = winCollaboration.sessionConfig.getEntityObj(srcEntityID);
                                                           var dstEntityObj = winCollaboration.sessionConfig.getEntityObj(entityId);
                                                            // for IO prevent port connecting to itself
                                                           if(!jsx3.util.strEmpty(srcEntityObj)&&!jsx3.util.strEmpty(dstEntityObj)&&(srcEntityObj.portNID == dstEntityObj.portNID) ){
                                                             windowAlert(winCollaborationBlkJSX, "Please select a different Source or Destination.Cannot connect the Media Source Destination to itself");
                                                             return false;
                                                           }         
                                                         }
                                                         // add connection with entity
                                                         if(winCollaboration.sessionConfig.updateConnGroupAddDst(cmpBlkConnection.ConnectionID, entityId)){
                                                           cmpBlkConnection.getData();
                                                         }
                                                       }
                                                     }
                                                    );
                                                                         
  // array of destination blocks - the subcmparr of this container is filled dynamically by the getData function
  var arrDstList = new Object();
  var cmpCtrDstList = new CmpContainer([newConnectionBlkJSX, "blkConnectionDstList"],
                                        [],
                                        null,
                                        null
                                       );                                          

  var cmpImgConnDstDefault = new Component([newConnectionBlkJSX, "blkConnectionDstList", "imgDragDstHere"], null);                                              
  var cmpLblConnDstDefaultMsg = new Component([newConnectionBlkJSX, "blkConnectionDstList", "lblDragDstHere"], null);                                              
  
  var cmpBlkConnection = new CmpContainer([newConnectionBlkJSX],
                                          [ {cmp:cmpBlkConnectionHeader},
                                            {cmp:cmpBlkConnectionSrc},
                                            {cmp:cmpBlkConnectionDstList},
                                            {cmp:cmpCtrDstList},
                                            {cmp:cmpImgConnDstDefault},
                                            {cmp:cmpLblConnDstDefaultMsg}
                                          ],
                                          [{action:"TOPICEVENT", topic:tConn, eventvar:"ConnectionDataUpdate", eventval:null}],
                                          {action: "FNCCALL",
                                           context: null,
                                           fnc: function(){
                                                    cmpBlkConnection_Destroy(cmpBlkConnection);
                                                  }                                         
                                           }
                                         );                                        
  cmpBlkConnection.ConnectionID = connId;
                     
  //-- METHOD - to parse conn xml info into cmp info
  cmpBlkConnection.getData = function() {
    
    // get one connection for the cmpBlkConnection.ConnectionID
    // since src will be the same for all the connections for this cmpBlkConnection.ConnectionID
    var connNode = winCollaboration.sessionConfig.getConnByGrpId(cmpBlkConnection.ConnectionID);
    if( jsx3.util.strEmpty(connNode) ) return;

    //check dir recording permissions
    var recordPermission = clsUserPermission.getPermission("Dir", "record");
    if( recordPermission == "false" ){
      topic_Set(tConn, "ShowRecordingEnabled", "DISABLED");
    }      
    
    var ConnectionID = winCollaboration.sessionConfig.getConnId(connNode);
    var ConnectionGrpID = winCollaboration.sessionConfig.getConnGrpId(connNode);
    var ConnectionType = winCollaboration.sessionConfig.getConnType(connNode);
    var srcEntityID = winCollaboration.sessionConfig.getConnSrcEntityId(connNode);
    var dstEntityID = winCollaboration.sessionConfig.getConnDstEntityId(connNode);
    var srcEntityNode = null;
    var recDirNID = null;
    // check if any Record connection is present for this cmpBlkConnection.ConnectionID
    var recordConnNode = winCollaboration.sessionConfig.getRecordConnByGrpId(cmpBlkConnection.ConnectionID);
    if( !jsx3.util.strEmpty(recordConnNode) ) {
      cmpConnectionRec.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      cmpConnectionRec.BlkJSX.repaint();
      var recDstEntityID = winCollaboration.sessionConfig.getConnDstEntityId(recordConnNode);
      var recDstEntityObj = winCollaboration.sessionConfig.getEntityObj(recDstEntityID);
      recDirNID = recDstEntityObj.dirNID;
    }
        
    // publish src name
    if( srcEntityID != "" ) {
      var srcName = winCollaboration.sessionConfig.getEntityName(srcEntityID);
      var srcNID = winCollaboration.sessionConfig.getEntityNID(srcEntityID);
      var srcStreamType = winCollaboration.sessionConfig.getEntityStreamType(srcEntityID);
      var srcType = winCollaboration.sessionConfig.getEntityType(srcEntityID);
      // if not ply file then only populate the dropdown
      if( srcType != "PlyFile" ) {
        var userRecordingDir = topic_GetNode(tCollaboration, "UserRecordingDir");
        if( jsx3.util.strEmpty(userRecordingDir) ){
          topic_Set(tConn, "ShowRecordingEnabled", "DISABLED");
        }
        else {
          var selRecID = null;
          var dirIter = userRecordingDir.selectNodeIterator(".//Dir");
          while (dirIter.hasNext()) {
            var dirNode = dirIter.next();
            var objRec = new Object;
            objRec.jsxid = dirNode.getAttribute("NID");
            objRec.jsxtext = dirNode.getAttribute("title");
            var DestDirNID = objRec.jsxid;
            if ( (!jsx3.util.strEmpty(recDirNID)) && (recDirNID == DestDirNID) ) {
              selRecID = DestDirNID;
            }
            cmpSelDirList.BlkJSX.insertRecord(objRec, null, false);
          }
          if( !jsx3.util.strEmpty(selRecID) ) {
            cmpSelDirList.BlkJSX.setValue(selRecID);
          }
        }
        cmpSelDirList.BlkJSX.repaint();
      }
      
      if(srcStreamType == "V2D") {
        cmpImgSrcType.BlkJSX.setSrc("images/icons/thumb_v2d.png");
      } else if(srcStreamType == "RTP") {
        cmpImgSrcType.BlkJSX.setSrc("images/icons/thumb_rtp.png");
      } else if(srcStreamType == "UDP") {
        cmpImgSrcType.BlkJSX.setSrc("images/icons/thumb_udp.png");
      } else if(srcStreamType == "MPEGTS") {
        cmpImgSrcType.BlkJSX.setSrc("images/icons/thumb_mpgts.png");
      } else if(srcStreamType == "VNC") {
        cmpImgSrcType.BlkJSX.setSrc("images/icons/thumb_vnc.png");
      }
      cmpImgSrcType.BlkJSX.repaint();   
      // publish the srcName in the conn topic
      topic_Publish(tConn, "srcName", srcName);
      topic_Set(tConn, "SrcEntityID", srcEntityID);
      if(ConnectionType == "RECORD") {
         topic_Set(tConn, "RecEntityID", srcEntityID); 
         topic_Set(tConn, "RecConnectionID", ConnectionID);
      }
      
      // if src is of type PlyFile and then disable the Rec button
      if( srcType == "PlyFile" || (topic_Get(tConn, "ShowRecordingEnabled") != "ENABLED") ){
        cmpConnectionRec.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        cmpSelDirList.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      }
      else {
        cmpConnectionRec.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
        cmpSelDirList.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
      }      
    }
    
    // cerate the destionation block
    var connNodeIter = winCollaboration.sessionConfig.getConnIterByGrpId(cmpBlkConnection.ConnectionID);;
    while( connNodeIter.hasNext() ) {
      var connNode = connNodeIter.next();
      var dstEntityID = winCollaboration.sessionConfig.getConnDstEntityId(connNode);
      if( jsx3.util.strEmpty(dstEntityID) ) continue;
      var connType = winCollaboration.sessionConfig.getConnType(connNode);
      // if connection type is of type=RECORD then skip
      if(connType == "RECORD") continue;
                                
      // insert the new destEnityid in the array
      if(arrDstList[dstEntityID] == null ) {
        arrDstList[dstEntityID] = false;     
      }
      var paintNewDst = true;
      if( arrDstList[dstEntityID] == true ){
        paintNewDst = false;
      }
     
      if(!paintNewDst) continue;
      // destination not painted yet, create a new destination block
      var dstData = new Object;
      dstData.connID = ConnectionGrpID;       
      dstData.ConnectionID = winCollaboration.sessionConfig.getConnId(connNode);
      dstData.ConnectionGrpID = winCollaboration.sessionConfig.getConnGrpId(connNode);
      dstData.ConnectionType = winCollaboration.sessionConfig.getConnType(connNode);
      dstData.dstEntityID = dstEntityID;
      
      dstData.dstName = winCollaboration.sessionConfig.getEntityName(dstEntityID);
      dstData.dstNID = winCollaboration.sessionConfig.getEntityNID(dstEntityID);
      var dstType = winCollaboration.sessionConfig.getEntityStreamType(dstEntityID);
      var imagePath = "images/icons/icon_v2d.png";
      if(dstType == "V2D") {
        imagePath = "images/icons/icon_v2d.png";
      } else if(dstType == "RTP") {
        imagePath = "images/icons/icon_rtp.png";
      } else if(dstType == "RTP-RAW") {
        imagePath = "images/icons/icon_rtpraw.png";
      } else if(dstType == "RTP-PCM") {
        imagePath = "images/icons/icon_rtppcm.png";
      } else if(dstType == "RFB") {
        imagePath = "images/icons/icon_rfb.png";
      } else if(dstType == "UDP") {
        imagePath = "images/icons/icon_udp.png";
      } else if(dstType == "MPEGTS") {
        imagePath = "images/icons/icon_mpgts.png";
      } else if(dstType == "VNC") {
        imagePath = "images/icons/icon_vnc.png";
      } else if(dstType == "MULTI") {
        imagePath = "images/icons/icon_multi.png";
      }     
      dstData.typeImage = imagePath;
      topic_Set(tConn, "DstEntityID", dstData.dstEntityID);
      topic_Set(tConn, "ConnectionID", dstData.ConnectionID); 
      topic_Publish(tConn, "CreateConnectionDst", dstData);   
    }    
    //winCollaboration.cmpImgConnDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);     
  }

// event listener for the rec dst dir selection
  eventConn.subscribe("DstRecDirSelected",null,this,
                      function(EventVar, EventVal) {
                        var dirNID = EventVal;
                        if( jsx3.util.strEmpty(dirNID) || dirNID == "NONE" ) return;
                        cmpConnectionRec.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                        cmpConnectionRec.BlkJSX.repaint();
                        cmpConnectionRec.onSelect.call(cmpConnectionRec);
                      }
                     );  

  // event listener for the create destination
  eventConn.subscribe("CreateConnectionDst",null,this,
                      function(EventVar, EventVal) {
                        var dstDataInfo = EventVal;
                        // create a conn block for it
                        var cmpBlkConnectionDst = cmpBlkConnectionDestination_Create(winCollaboration, newConnectionBlkJSX, dstDataInfo, tConn);
                        // add it to the sub cmp arr of the connectionDst list container
                        var cmpBlkConnectionDstObj = { cmp: cmpBlkConnectionDst };
                        cmpCtrDstList.SubCmpArr.push(cmpBlkConnectionDstObj);
                        cmpImgConnDstDefault.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                        cmpLblConnDstDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);                       
                        // mark it in the conn array
                        if(arrDstList[dstDataInfo.dstEntityID] != null ) {
                          arrDstList[dstDataInfo.dstEntityID] = true;     
                        }                        
                        return true;
                      }
                     );  
                     
  // event listener for the delete destination
  eventConn.subscribe("ConnectionDeleteDst",null,this,
                      function(EventVar, EventVal) {
                        var destEntityId = EventVal;
                        for (var i=0; i< cmpCtrDstList.SubCmpArr.length; i++) {
                          var cmp = cmpCtrDstList.SubCmpArr[i].cmp;
                          if( cmp.destEntityId == destEntityId ) {
                            // destroy the block
                            cmpCtrDstList.SubCmpArr[i].cmp.destroy();
                            // remove ref from cmp arr
                            cmpCtrDstList.SubCmpArr.splice(i, 1);                        
                            // clear the array
                            arrDstList[destEntityId] = null;
                            delete arrDstList[destEntityId];
                          }
                        }
                        // if any destination is there, if None then show the msg
                        if(cmpCtrDstList.SubCmpArr.length < 1){
                          cmpImgConnDstDefault.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                          cmpLblConnDstDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);                        
                        }
                      }
                     ); 
                                          
  // init and show it - in its own context
  jsx3.sleep((function() {
                 this.init();
                 this.show();
                 this.getData();
               }
              ), null, cmpBlkConnection);

  // return a handle to it
  return cmpBlkConnection;
}

