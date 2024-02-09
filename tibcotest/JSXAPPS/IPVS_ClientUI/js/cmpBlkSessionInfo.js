function cmpBlkSessionInfo_Create(sessionConfigwindow) {

  // get vars from object instance
  var winSession = this;
  var winSessionBlkJSX = this.BlkJSX; 
  var tSession = this.tWin;
  var eventSession = this.eventWin;
  topic_Set(tApp,"EntityResourceDataReady", "TRUE");

  var blkConnections = getJsxByName([winSessionBlkJSX, "blkConnections"]);
  blkConnections.load("components/blkConnections.xml");
  var blkSessionMedia = getJsxByName([winSessionBlkJSX, "blkSessionMedia"]);
  blkSessionMedia.load("components/blkSessionMedia.xml");
  var cmpLblLoadingStreamMsg = getJsxByName([winSessionBlkJSX, "lblLoadingStreams"]); 
                                    
    // add source button
  this.cmpBtnSessionAddSource = new CmpButton([winSessionBlkJSX, "btnSessionAddMediaSource"],
                                               null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   topic_Set(tSession, "ButtonSelection", "Live");
                                                   var ThisClass = this;
                                                   jsx3.sleep((function () {
                                                     topic_Publish(tSession, "OpenMediaList", "OPEN");
                                                   }), null, ThisClass);
                                                 }
                                               });

  this.cmpBlkSessionAddSource = new CmpContainer([winSessionBlkJSX, "blkSessionAddMediaSource"],
                                                [{ cmp: this.cmpBtnSessionAddSource}],
                                                null,
                                                null
                                              );  
                                                                                      
  // add dest button
  this.cmpBtnSessionAddDestination = new CmpButton([winSessionBlkJSX, "btnSessionAddDestination"],
                                                   null,
                                                   { action: "FNCCALL",
                                                       context: null,
                                                       fnc: function() {
                                                           topic_Set(tSession, "ButtonSelection", "Destination");
                                                           var ThisClass = this;
                                                           jsx3.sleep((function () {
                                                             topic_Publish(tSession, "OpenMediaList", "OPEN");
                                                           }), null, ThisClass);                                                           
                                                       }
                                                   });

  this.cmpBlkSessionAddDestination = new CmpContainer([winSessionBlkJSX, "blkSessionAddDestination"],
                                                [{ cmp: this.cmpBtnSessionAddDestination}],
                                                null,
                                                null
                                              ); 
                                              
  // add user button
  this.cmpBtnSessionAddUser = new CmpButton([winSessionBlkJSX, "btnSessionAddUsers"],
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function()
                                                     {
                                                       //topic_Set(tSession, "ButtonSelection", "User");
                                                       cmpWinUserList_Create(tSession, winSessionBlkJSX);
                                                     }
                                                   });                                                   
  // source list
  this.cmpMtxSessionSrcList = new CmpList([winSessionBlkJSX, "mtxSessionSources"],
                                          [],
                                          { "jsxid": "id",
                                            "jsxtext": "name",
                                            "jsxtype": "streamType",
                                            "jsxtypeImage": "streamTypeImage",
                                            "jsxRole": "type",
                                            "jsximgEntityPresence":"presenceImage",
                                            "jsxNID": "id"
                                          },
                                          null, 
                                          { persist: { col: "jsxid", initval: false },
                                            action: "TOPICEVENT", topic: tSession, eventvar: "MediaSrcSelected", eventval: "jsxid"
                                          },
                                          null,
                                          null,
                                          true
                                        );
  // dest list
  this.cmpMtxSessionDstList = new CmpList([winSessionBlkJSX, "mtxSessionDestinations"],
                                          [],
                                          { "jsxid": "id",
                                            "jsxtext": "name",
                                            "jsxtype": "streamType",
                                            "jsxtypeImage": "streamTypeImage",
                                            "jsxRole": "type",
                                            "jsximgEntityPresence":"presenceImage",
                                            "jsxclipId": "clipId",
                                            "jsxrecSessionConfigNID": "recSessionConfigNID",
                                            "jsxNID": "id"
                                          },
                                          null, 
                                          { persist: { col: "jsxid", initval: false },
                                            action: "TOPICEVENT", topic: tSession, eventvar: "MediaDstSelected", eventval: "jsxid"
                                          },
                                          null,
                                          null,
                                          true
                                        );
  // user list
  this.cmpMtxSessionUserList = new CmpList([winSessionBlkJSX, "mtxSessionUsers"],
                                          [],
                                          { "jsxid": "id",
                                            "jsxtext": "name",
                                            "jsxRole": "type",
                                            "jsxNID": "id"
                                          },
                                          null, 
                                          { persist: { col: "jsxid", initval: false },
                                            action: "TOPICEVENT", topic: tSession, eventvar: "MediaUserSelected", eventval: "jsxid"
                                          },
                                          null,
                                          null,
                                          true
                                        );

  eventSession.subscribe("CollaborationDataAddEntity", null, this,
                          function(EventVar, EventVal) {
                            var entityObj = EventVal;
                            var entityId = entityObj.entityId;
                            var matrixRepaint = entityObj.repaint;
                                                       
                            // get the info for this entity
                            var objEntity = this.sessionConfig.getEntityObj(entityId);
                            // if any blanks - skip
                            if( (! objEntity) || jsx3.util.strEmpty(objEntity.id) || jsx3.util.strEmpty(objEntity.type) ) {
                              return false;
                            }
                            // different image fro the recorded files
                            if(objEntity.type == "PlyFile") {
                              // set streamtype image
                              switch(objEntity.streamType){
                                case "V2D":
                                  objEntity.streamTypeImage = "images/icons/icon_rec_v2d.png";
                                  break;
                                case "MPEGTS":
                                  objEntity.streamTypeImage = "images/icons/icon_rec_mpgts.png";
                                  break;
                                case "UDP":
                                  objEntity.streamTypeImage = "images/icons/icon_rec_udp.png";
                                  break;
                                case "RTP":
                                  objEntity.streamTypeImage = "images/icons/icon_rec_rtp.png";
                                  break;  
                                case "RTP-RAW":
                                  objEntity.streamTypeImage = "images/icons/icon_rtpraw.png";
                                  break;  
                                case "RTP-PCM":
                                  objEntity.streamTypeImage = "images/icons/icon_rtppcm.png";
                                  break;  
                                case "RFB":
                                  objEntity.streamTypeImage = "images/icons/icon_rfb.png";
                                  break;  
                                case "VNC":
                                  objEntity.streamTypeImage = "images/icons/icon_rec_vnc.png";
                                  break;
                                case "MULTI":
                                  objEntity.streamTypeImage = "images/icons/icon_rec_multi.png";
                                  break;                     
                                default:
                                  objEntity.streamTypeImage = objEntity.streamType;
                                  break;                                                                      
                              }                            
                            } else {
                              // set streamtype image
                              switch(objEntity.streamType){
                                case "V2D":
                                  objEntity.streamTypeImage = "images/icons/icon_v2d.png";
                                  break;
                                case "MPEGTS":
                                  objEntity.streamTypeImage = "images/icons/icon_mpgts.png";
                                  break;
                                case "UDP":
                                  objEntity.streamTypeImage = "images/icons/icon_udp.png";
                                  break;
                                case "RTP":
                                  objEntity.streamTypeImage = "images/icons/icon_rtp.png";
                                  break;  
                                case "RTP-RAW":
                                  objEntity.streamTypeImage = "images/icons/icon_rtpraw.png";
                                  break;  
                                case "RTP-PCM":
                                  objEntity.streamTypeImage = "images/icons/icon_rtppcm.png";
                                  break;  
                                case "RFB":
                                  objEntity.streamTypeImage = "images/icons/icon_rfb.png";
                                  break;  
                                case "VNC":
                                  objEntity.streamTypeImage = "images/icons/icon_vnc.png";
                                  break;
                                case "MULTI":
                                  objEntity.streamTypeImage = "images/icons/icon_multi.png";
                                  break;                     
                                default:
                                  objEntity.streamTypeImage = objEntity.streamType;
                                  break;                                                                      
                              }
                            }

                            switch(objEntity.presence){
                              case "":
                                objEntity.presenceImage = "images/icons/icon_lightgrayBall.png";
                                break;
                              case "READY":
                                objEntity.presenceImage = "images/icons/icon_greenBall.png";
                                break;
                              case "OFFLINE":
                                objEntity.presenceImage = "images/icons/icon_grayBall.png";
                                break;
                              case "available":
                                objEntity.presenceImage = "images/icons/icon_greenBall.png";
                                break;
                              case "unavailable":
                                objEntity.presenceImage = "images/icons/icon_grayBall.png";
                                break;
                              default:
                                objEntity.presenceImage  = "images/icons/icon_grayBall.png";
                                break;                                  
                            }                                          
                            // pick the right mtx based on type
                            var objCmpMatrixList = null;
                            if( objEntity.type == "User" ) {
                              objCmpMatrixList = this.cmpMtxSessionUserList;
                            }
                            else if( objEntity.type == "SrcPort" || objEntity.type == "PlyFile") {
                              objCmpMatrixList = this.cmpMtxSessionSrcList;
                            }
                            //else if( objEntity.type == "DstPort" || objEntity.type == "RecFile" || objEntity.type == "StorePort") {
                            // displaying on ports in the destination no Recfile
                            else if( objEntity.type == "DstPort" ) {
                              objCmpMatrixList = this.cmpMtxSessionDstList;
                            }
                            if( ! objCmpMatrixList ) return false;
                              
                            // insert obj into mtx 
                            objCmpMatrixList.ObjectList.push(objEntity);
                            // add the function to the delete 
                            var imgDeleteEntityBlkJSX = objCmpMatrixList.BlkJSX.getDescendantOfName("jsximgEntityDelete");
                            var ThisClass = this;
                            imgDeleteEntityBlkJSX.setEvent('eval("DeleteEntity(this);")', jsx3.gui.Interactive.EXECUTE);
                            
                            
                            DeleteEntity = function(obj) {
                              var objRecordId = obj.emGetSession().recordId;        
                              var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
                              var entityName = ObjRecord.jsxtext;
                              var entityType = ObjRecord.jsxtype;
                              var type = ObjRecord.jsxRole;
                              
                              // pick the right mtx based on type
                              var objCmpMatrixList = null;
                              if( type == "User" ) {
                                objCmpMatrixList = ThisClass.cmpMtxSessionUserList;
                              }
                              else if( type == "SrcPort" || type == "PlyFile") {
                                objCmpMatrixList = ThisClass.cmpMtxSessionSrcList;
                              }
                              //else if( type == "DstPort" || type == "RecFile" || type == "StorePort") {
                              else if( type == "DstPort" ) {
                                objCmpMatrixList = ThisClass.cmpMtxSessionDstList;
                              }
                              if( ! objCmpMatrixList ) return false;                              
                              
                              
                              // check if entity is being used in any connection
                              var checkEntity = ThisClass.sessionConfig.checkEntityInConnectionList(objRecordId);
                              if(checkEntity){
                                windowAlert(winSessionBlkJSX, "'"+ entityName +"' cannot be deleted.");
                                return false;
                              }
                              else {
                                // delete record from the matrix and update view
                                var deleteCheck = obj.getParent().getParent().deleteRecord(objRecordId, true);        
                                // remove it from the objectlist
                                for(var i=0; i<objCmpMatrixList.ObjectList.length;i++){
                                  var objEntity = objCmpMatrixList.ObjectList[i];
                                  if(objRecordId == objEntity.id){
                                    objCmpMatrixList.ObjectList.splice(i, 1);
                                    break;
                                  }
                                }
                                
                                // delete entity from the xml 
                                ThisClass.sessionConfig.delEntity(objRecordId);
                                                                
                                //To fix bug#43249
                                var sessionNode = ThisClass.sessionConfig.getRootNode().cloneNode(true);
                                var nodeEntityPlyFile = sessionNode.selectSingleNode(".//SessionInfo/EntityList/Entity[@type='PlyFile']");
                                var recVersion = nodeEntityPlyFile==null? false:true;
                                if(!recVersion){
                                  ThisClass.sessionConfig.setRecVersion("");
                                  topic_Publish(tSession, "NoPlyFileSelected", (new Date));
                                }
                                
                                // repaint  to handlescrollbars
                                objCmpMatrixList.repaint();                                
                              }
                            }                             
                            
                            // repaint 
                            if(matrixRepaint){
                              objCmpMatrixList.repaint();
                            }
                            
                            if(objEntity.type == "PlyFile"){
                              topic_Publish(tSession, "PlyFileSelected", (new Date));
                            }

                            return true;
                          });
                          
  // new connection drop area                                          
  this.cmpBlkConnectionList = new CmpBlkDropTarget([winSessionBlkJSX, "blkConnectionsList"],
                                                     null,
                                                     { action: "FNCCALL",
                                                       context: this,
                                                       fnc: function(EventVal) {
                                                         // get dropped item vars from event
                                                         var objSOURCE = EventVal.objSOURCE;
                                                         var strDRAGID = EventVal.strDRAGID;
                                                         var strDRAGTYPE = EventVal.strDRAGTYPE;
                                                         
                                                         //check if collaboration name is present
                                                         var collaborationName = this.sessionConfig.getName();
                                                         if (jsx3.util.strEmpty(collaborationName.trim())) {
                                                           windowAlert(winSessionBlkJSX, "Please enter Collaboration name before adding any Media Source/Destinations");
                                                           return false;
                                                         }
                                                         // get entity ID of dropped item
                                                         //var entityId = objSOURCE.getValue();
                                                         var entityId = strDRAGID;
                                                         // based on source of dropped item - use it a src or dst
                                                         var srcEntityId = null;
                                                         var dstEntityId = null;
                                                         var dropFrom = objSOURCE.jsxname;
                                                         if (dropFrom == "mtxSessionSources") {
                                                           srcEntityId = entityId;
                                                         }
                                                         else if(dropFrom == "mtxSessionDestinations") {
                                                           dstEntityId = entityId;
                                                         }
                                                         else {
                                                           windowAlert(winSessionBlkJSX, "Please drop a Media Source or Destination here to create Connections");
                                                           return false;
                                                         }
                                                         // check if source is available
                                                         if(!this.sessionConfig.isSrcEntityAvailable(srcEntityId)){
                                                           return false;
                                                         }
                                                         // check if destination is available
                                                         if(!this.sessionConfig.isDestEntityAvailable(dstEntityId)){
                                                           return false;
                                                         }                                                                             
                                                         
                                                         var mixedSessions = false;
                                                         var mixedMediaGroups = false;
                                                         var sessionNode = this.sessionConfig.getRootNode().cloneNode(true);
                                                         var recordNode = objSOURCE.getRecord(strDRAGID);
                                                         var type = recordNode.jsxRole;
                                                         if(type == "SrcPort"){
                                                           var nodeEntityPlyFile = sessionNode.selectSingleNode(".//SessionInfo/EntityList/Entity[@type='PlyFile']");
                                                           mixedSessions = nodeEntityPlyFile==null? false:true;
                                                         }
                                                         else if(type == "PlyFile"){
                                                           var nodeEntitySrcPort = sessionNode.selectSingleNode(".//SessionInfo/EntityList/Entity[@type='SrcPort']");
                                                           mixedSessions = nodeEntitySrcPort==null? false:true;
                                                           var nodeGroupFile = sessionNode.selectSingleNode(".//GrpFileList/GrpFile");
                                                           var groupFileId = nodeGroupFile==null?"":nodeGroupFile.getAttribute("id");
                                                           var nodeDifferentGroupFile = sessionNode.selectSingleNode(".//SessionInfo/EntityList/Entity/File[@grpFileId !='"+groupFileId+"']");
                                                           mixedMediaGroups = nodeDifferentGroupFile==null? false:true;
                                                         }
                                                         if(mixedSessions || mixedMediaGroups){
                                                           var message = "You cannot drag and drop, if session contains a combination of live sources and playback files,\n";
                                                           message += "or playback files created from different recording sessions.\n";
                                                           message += "Please remove one of the entity types from the 'My Media Sources' panel.";
                                                           windowAlert(winSessionBlkJSX, message);
                                                           return false;
                                                         }
                                                         
                                                         // add a new connection group with this entity/instance
                                                         var connGrpId = this.sessionConfig.addConnGroup(srcEntityId, dstEntityId);
                                                         // update the UI
                                                          var connObj = new Object;
                                                          connObj.connGrpId = connGrpId;
                                                          connObj.repaintFlag = true;                                                         
                                                         topic_Publish(tSession, "CollaborationDataAddConnectionGroup", connObj);
                                                       }
                                                     }
                                                   );

  // array of connection blocks - the subcmparr of this container is filled dynamically by the getData function
  this.arrConnectionList = new Object();
  this.cmpCtrConnectionList = new CmpContainer([winSessionBlkJSX, "blkConnectionsList"],
                                                [],
                                                null, 
                                                null
                                              );

  this.cmpImgConnDefaultMsg = new Component([winSessionBlkJSX, "blkConnectionsList", "imgDefaultConnMsg"], null);
  
  eventSession.subscribe("CollaborationDataAddConnectionGroup", null, this,
                          function(EventVar, EventVal) {
                            var connObj = EventVal;
                            var connGrpId = connObj.connGrpId;
                            var repaintFlag = connObj.repaintFlag;
                            // create a conn block for it
                            var cmpBlkConnection = cmpBlkConnection_Create(this, connGrpId, repaintFlag);
                            // add it to the sub cmp arr of the connection list container
                            var cmpBlkConnectionObj = { cmp: cmpBlkConnection };
                            this.cmpCtrConnectionList.SubCmpArr.push(cmpBlkConnectionObj);
                            // mark it in the conn array
                            this.arrConnectionList[connGrpId] = true;
                            
                            //  hide the default message image
                            this.cmpImgConnDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                            return true;
                          });

  eventSession.subscribe("CollaborationDataDelConnectionGroup", null, this,
                          function(EventVar, EventVal) {
                            var connGrpId = EventVal;
                            for (var i=0; i< this.cmpCtrConnectionList.SubCmpArr.length; i++) {
                              var cmp = this.cmpCtrConnectionList.SubCmpArr[i].cmp;
                              if( cmp.ConnectionID == connGrpId ) {
                                // destroy the block
                                this.cmpCtrConnectionList.SubCmpArr[i].cmp.destroy();
                                // remove ref from cmp arr
                                this.cmpCtrConnectionList.SubCmpArr.splice(i, 1);
                                // clear arr
                                this.arrConnectionList[connGrpId] = null;
                                delete this.arrConnectionList[connGrpId];
                                if(this.cmpCtrConnectionList.SubCmpArr.length < 1) {
                                  this.cmpImgConnDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                }                                
                                return true;
                              }
                            }
                            // check if any connection is there, if none then show the degfauly connection message
                            if(this.cmpCtrConnectionList.SubCmpArr.length < 1) {
                              this.cmpImgConnDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                            }
                            return false;
                          });

  // overall session info container                              
  var cmpCtrSessionInfo = new CmpContainer([winSessionBlkJSX, "blkSessionInfo"],
                                                 [{ cmp: this.cmpBlkSessionAddSource,
                                                   sel: "(clsUserPermission.getPermission('Port', 'read') == 'true') || (clsUserPermission.getPermission('Dir', 'read') == 'true' && clsUserPermission.getPermission('File', 'read') == 'true')"},
                                                  { cmp: this.cmpBlkSessionAddDestination,
                                                    sel: "(clsUserPermission.getPermission('Port', 'read') == 'true')"},
                                                  { cmp: this.cmpBtnSessionAddUser },
                                                  { cmp: this.cmpMtxSessionSrcList },
                                                  { cmp: this.cmpMtxSessionDstList },
                                                  { cmp: this.cmpMtxSessionUserList },
                                                  { cmp: this.cmpBlkConnectionList },
                                                  { cmp: this.cmpImgConnDefaultMsg},
                                                  { cmp: this.cmpCtrConnectionList }],
                                                 null,
                                                 null
                                              );


  eventSession.subscribe("MediaListCollabSelection", null, this,
                          function(EventVar, EventVal) {
                            var mediaInfo = EventVal;
                            // check if an entity already exists - avoid duplicates
                            if( ! this.sessionConfig.getEntity(mediaInfo.type, mediaInfo.NID) ) {
                              // get entity info from mediaInfo
                              if( mediaInfo.type == "User" ) {
                                var objUserEntity = new Object();
                                objUserEntity.name = mediaInfo.Name;
                                objUserEntity.type = mediaInfo.type;
                                objUserEntity.userNID = mediaInfo.NID;
                                objUserEntity.userTitle = mediaInfo.Name;
                                // add to entity list
                                var userEntityId = this.sessionConfig.addEntity(objUserEntity);
                                // notify UI to update
                                var entityObj = new Object;
                                entityObj.entityId = userEntityId;
                                entityObj.repaint = true;                                
                                topic_Publish(tSession, "CollaborationDataAddEntity", entityObj);
                              }
                              else if( mediaInfo.type == "SrcPort" || mediaInfo.type == "DstPort") {
                                var objPortEntity = new Object();
                                objPortEntity.name = mediaInfo.Name;
                                objPortEntity.type = mediaInfo.type;
                                objPortEntity.portNID = mediaInfo.NID;
                                objPortEntity.portTitle = mediaInfo.Name;
                                objPortEntity.maxInstanceCount = mediaInfo.maxInstanceCount;
                                objPortEntity.streamType = mediaInfo.streamType;
                                objPortEntity.streamTypeList = mediaInfo.streamTypeList;
                                objPortEntity.presence = mediaInfo.presence;
                                // add to entity list
                                var portEntityId = this.sessionConfig.addEntity(objPortEntity);
                                // notify UI to update
                                var entityObj = new Object;
                                entityObj.entityId = portEntityId;
                                entityObj.repaint = true;                                 
                                topic_Publish(tSession, "CollaborationDataAddEntity", entityObj);                                    
                              }
                              else if( mediaInfo.type == "PlyFile" ) {
                                var objFileEntity = new Object();
                                objFileEntity.name = mediaInfo.Name;
                                objFileEntity.type = mediaInfo.type;
                                objFileEntity.fileNID = mediaInfo.NID;
                                objFileEntity.fileTitle = mediaInfo.Name;
                                objFileEntity.streamType = mediaInfo.streamType;
                                objFileEntity.clipId = mediaInfo.clipID;
                                objFileEntity.recSessionConfigNID = mediaInfo.sessionConfigNID;
                                // add to entity list
                                var fileEntityId = this.sessionConfig.addEntity(objFileEntity);
                                // notify UI to update
                                var entityObj = new Object;
                                entityObj.entityId = fileEntityId;
                                entityObj.repaint = true;                                 
                                topic_Publish(tSession, "CollaborationDataAddEntity", entityObj);
                              }
                              else if( mediaInfo.type == "GrpFile" ) {
                                var objGrpFile = new Object();
                                objGrpFile.grpFileType = "SyncPly";
                                objGrpFile.grpFileNID = mediaInfo.NID;
                                objGrpFile.grpFileTitle = mediaInfo.Name;
                                // add the grpFile to session
                                if(!this.sessionConfig.getGrpFile(objGrpFile.grpFileTitle, objGrpFile.grpFileNID, objGrpFile.grpFileId)){
                                  var grpFileId = this.sessionConfig.addGrpFile(objGrpFile);
                                  for(var i=0; i< mediaInfo.arrClips.length; i++ ) {
                                    var clipInfo = mediaInfo.arrClips[i];
                                    var objClipEntity = new Object();
                                    objClipEntity.name = clipInfo.Name;    
                                    objClipEntity.type = "PlyFile";
                                    objClipEntity.fileNID = clipInfo.NID;
                                    objClipEntity.fileTitle = clipInfo.Name;
                                    objClipEntity.streamType = clipInfo.streamType;
                                    objClipEntity.grpFileId = grpFileId;
                                    objClipEntity.clipId = clipInfo.clipID;
                                    objClipEntity.recSessionConfigNID = clipInfo.sessionConfigNID;
                                    if(! this.sessionConfig.getEntity(objClipEntity.type, objClipEntity.fileNID) ){
                                      // add entity to session
                                      var clipEntityId = this.sessionConfig.addEntity(objClipEntity);
                                      // notify UI to update
                                      var entityObj = new Object;
                                      entityObj.entityId = clipEntityId;
                                      entityObj.repaint = true;                                       
                                      topic_Publish(tSession, "CollaborationDataAddEntity", entityObj);
                                    }
                                  }
                                }
                              }
                            }
                          });

  //getData whenever "CollaborationState" is published from clsCtrlAgent.notifySessionState
  eventSession.subscribe("CollaborationState", "CONNECTED", this,
                      function(eventVar, eventVal) {
                        var nodeSession = topic_Get(tSession, "NodeSession");
                        var nodeSession1 = new jsx3.xml.Document();
                        nodeSession1.loadXML(nodeSession);
                        this.sessionConfig = new com.ipvs.api.Session.Document(nodeSession1);
                        //fill data to sessionInfo panel
                        this.getData();
                      });  

  //getData whenever "CollaborationState" is published from clsCtrlAgent.notifySessionState
  eventSession.subscribe("GetDirListDone", null, this,
                      function(eventVar, eventVal) {
                        //fill data to sessionInfo panel
                        this.getData();
                      }); 
                      
  // -- METHOD to get the recording dir
  this.getDirList = function() {
    var recordPermission = clsUserPermission.getPermission("Dir", "record");
    var userRecordingDir = topic_Set(tSession, "UserRecordingDir", null);
    
    if( recordPermission == "false" ) {
      topic_Publish(tSession, "GetDirListDone", (new Date()).getTime()); 
      return;
    }
    
    //get the recording dir for the user
    var reqParams = new Object;
    reqParams["ObjectList_SortOrder"] = "ascending";
    reqParams["ObjectCount"] = "0";
    reqParams["ObjectList_StartFrom"] = "1";
    reqParams["ObjectList_CountToFetch"] = "";
    reqParams["ObjectList_SortBy"] = "";
    reqParams["ObjectList_SelectFormat"] = "BASIC";
    reqParams["ObjectList_WherePermissions"] = "RECORD";
    var reqRecordingDirData = new ReqSessionGetRequest("GetDir",
                                                      { action: "FNCCALL", context: this,
                                                        fnc: function(respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                            var Response = respVal;
                                                            // delete the PVR and RECYCLEBIN dir from the response
                                                            var pvrRBDirNodes = respVal.selectNodes(".//Dir[@title='PVR' or @title='RECYCLEBIN']");
                                                            for (var it = pvrRBDirNodes.iterator(); it.hasNext(); ) {
                                                              var dirNode = it.next();
                                                              dirNode.getParent().removeChild(dirNode);
                                                            }
                                                            //store dir data to tApp
                                                            var userRecordingDir = topic_Set(tSession, "UserRecordingDir", null);
                                                            userRecordingDir.appendChild(respVal);                                                                                      
                                                          }
                                                          else {                                                                              
                                                          }
                                                          topic_Publish(tSession, "GetDirListDone", (new Date()).getTime()); 
                                                        }
                                                      }
                                                    );
    reqRecordingDirData.reqParams = reqParams;
    reqRecordingDirData.sendRequest();      
  }
  
  // -- METHOD to parse collabconfig XML and convert to list objects for GUI components
  this.getData = function() {
    // get vars from object instance
    var winSession = this;
    var winSessionBlkJSX = this.BlkJSX;
    var tSession = this.tWin;
    var eventSession = this.eventWin;
        
    // if null - get / init it from cache
    if(! this.sessionConfig ) {
      // get the input collab config NID
      var ccNID = topic_Get(tSession, "SessionConfigNID");
      // if NONE - new CC - init with blank
      if( ccNID == null || ccNID == "NONE" ) {
        // get a new sessionconfig document
        this.sessionConfig = com.ipvs.api.SessionConfig.Document.newDocument();
        this.sessionConfig.setDateCreated();
      }
      else {
        // get CC node from cache
        var ccNode = topic_GetNIDNode(tCache, ccNID);
        if( ccNode == null ) {
          log.error("SessionConfig: NID: " + ccNID + ": Missing Node Data in Cache");
          return;
        }
        var nodeSessionConfig = ccNode.cloneNode(true);
        if( ! nodeSessionConfig ) {
          log.error("SessionConfig: Missing SessionConfig Element");
          return;
        }
        this.sessionConfig = new com.ipvs.api.SessionConfig.Document(nodeSessionConfig);
      }
    }

    // get the collaboration name
    var collabName = this.sessionConfig.getName();
     if ( jsx3.util.strEmpty(collabName)) {
       collabName = "New Session";
     }    
    topic_Set(tSession, "CollaborationName", collabName); 
    
    // get entity lists
    this.cmpMtxSessionSrcList.ObjectList = [];
    this.cmpMtxSessionDstList.ObjectList = []; 
    this.cmpMtxSessionUserList.ObjectList = [];
    var iterEntity = this.sessionConfig.getEntityIter();
    while( iterEntity.hasNext() ) {
      var entityNode = iterEntity.next();  
      var entityId = this.sessionConfig.getEntityId(entityNode);
      var entityObj = new Object;
      entityObj.entityId = entityId;
      entityObj.repaint = false;
      topic_Publish(tSession, "CollaborationDataAddEntity", entityObj);
    }
        
    // get connection list
    for (var connGrpId in this.arrConnectionList) {
      this.arrConnectionList[connGrpId] = false;
    }
    // get only the connections of type=STREAM
    var iterConn = this.sessionConfig.getConnViewIter();
    while( iterConn.hasNext() ) {
      var connNode = iterConn.next();  
      // get the connGrpId
      var connGrpId = this.sessionConfig.getConnGrpId(connNode);
      // check if this is a new connGrpId
      if( this.arrConnectionList[connGrpId] == null ) {
        // notify UI to create a conn block for it
        var connObj = new Object;
        connObj.connGrpId = connGrpId;
        connObj.repaintFlag = false;
        // hide the default create connection image
        this.cmpImgConnDefaultMsg.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
        // display loading connection label
        cmpLblLoadingStreamMsg.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
        topic_Publish(tSession, "CollaborationDataAddConnectionGroup", connObj);
      }
      else {
        // just mark it in the conn array
        this.arrConnectionList[connGrpId] = true;
      }
    }
    // anything still marked false in the list needs to be dereferenced
    for (var connGrpId in this.arrConnectionList) {
      for (var i=0; i< this.cmpCtrConnectionList.SubCmpArr.length; i++) {
        var cmp = this.cmpCtrConnectionList.SubCmpArr[i].cmp;
        if( cmp.ConnectionID == connGrpId ) {
          if( this.arrConnectionList[connGrpId] == false ) {
            // notify UI to remove the block
            topic_Publish(tSession, "CollaborationDataDelConnectionGroup", connGrpId);
          }
          else {
            // refresh data view
            // commenting it out because getdata() gets the data and repaint. This was causiing to repaint twice
            //this.cmpCtrConnectionList.SubCmpArr[i].cmp.getData();
          }
        }
      }
    }
  
    this.cmpMtxSessionSrcList.repaint();
    this.cmpMtxSessionDstList.repaint();  
    // repaint the connection block    
    this.cmpBlkConnectionList.repaint();    
    // hide loading connection label
    cmpLblLoadingStreamMsg.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);           
    // publish data update event
    topic_Publish(tSession, "CollaborationDataUpdate", (new Date));
  }
  /*
  // ccxml update methods
  // TBD: last remaining stub - used by mediaplayer - clean when porting that to sessionconfig
  this.addDestEntityIpPort = function(dstEntityId, IpPort) {
    var ccXMLNode = topic_GetNode(tSession, "CollaborationConfigXML");
    var objEntityNode = ccXMLNode.selectSingleNode(".//Entity[@id='"+ dstEntityId +"']");
    if(objEntityNode != null ){
      objEntityNode.selectSingleNode(".//destIP").setValue(IpPort.IpAddress);
      objEntityNode.selectSingleNode(".//destUDPStartPort").setValue(IpPort.Port);
    }
    var objstreamType = objEntityNode.getAttribute("streamType");
    return objstreamType;      
  }
  */


  //separate cmpBlkSessionInfo from cmpBlkMediaPlayer and cmpBlkCollaboration
  jsx3.sleep((function() {
                          this.init();
                          this.show();
                          //push this component to parent window's SubCmpArr
                          winSession.SubCmpArr.push({ cmp: cmpCtrSessionInfo });
                          winSession.getDirList();
                          //winSession.getData();
                          topic_Publish(tSession, "SessionInfoLoadingStatus", "LOADED");
                          log.info("cmpBlkSessionInfo:sessionInfo panel is loaded.");
                         }
               ), null, cmpCtrSessionInfo);
}


