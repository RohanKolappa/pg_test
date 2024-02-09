function cmpBlkPvrMedia_Create(tName, eventName, DlgBlkJsx) {

  var blkJSX = DlgBlkJsx;
  var tMedia = tName;
  var eventMedia = eventName; 
  
  topic_Set(tMedia, "PvrMediaStreamFileResourceDataUpdated","LOADED" );
  topic_Set(tMedia, "PvrMediaStreamFileResourceNavButtonClicked", (newServerDate()).getTime());
  topic_Set(tMedia, "PvrMediaStreamFileResourceObjectType", "MediaStreamFileResource");
  topic_Set(tMedia, "PvrSelectFormat", "EXTENDED");
  topic_Set(tMedia, "PvrWherePermissions", "MODIFY,DELETE,EXPORT");

  topic_Set(tMedia, "PvrMediaSFRSelected", "NONE");
  topic_Set(tMedia, "PvrMediaSFRSelectedExportPermission", "false");
  topic_Set(tMedia, "PvrMediaSFRSelectedPermissions", (newServerDate()).getTime());

  topic_Set(tMedia, "PvrMediaSFRGetData", "NONE");

                                             
  /************************** MEDIA FILES **********************************/

  var cmpMtxPvrMediaSFRList = new CmpCustomPaginationList([blkJSX, "cmpMediamtxPVRValue"],
                                                       [{ action: "TOPICEVENT", topic: tMedia, eventvar: "PvrMediaSFRGetData", eventval: null}],
                                                       {persist:{col:"jsxid",initval:false},
                                                         action: "TOPICEVENT", topic: tMedia, eventvar: "PvrMediaSFRSelected", eventval: "jsxid"},
                                                       [{ ColumnName: "jsxtextmediaPVRCreated", FncName: "@datetime,MM/dd/yyyy HH:mm" }
                                                       ],
                                                       null
                                                     );
  cmpMtxPvrMediaSFRList.allowMultiSelection = false;

  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
   cmpMtxPvrMediaSFRList.SortPathArr = { jsxtextmediaPVRCreated: "dateCreated",
                                      jsxtextmediaPVRName: "title",
                                      jsxtextmediaStatus: "state",
                                      jsxtextmediaPVRSize: "size"
                                  };
  
  cmpMtxPvrMediaSFRList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                        CountToFetch: "ObjectList_CountToFetch",
                                        SortOrder: "ObjectList_SortOrder",
                                        ObjectCount: "ObjectCount",
                                        SortByXpath: "ObjectList_SortBy",
                                        SortByMetaData: "ObjectList_SortByMetaData"
                                    };

  cmpMtxPvrMediaSFRList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tMedia, eventvar: "PvrMediaStreamFileResourceDataUpdated", eventval: "LOADED" };

    cmpMtxPvrMediaSFRList.BlkJSX.setXMLTransformers("xsl/PVRMediaList.xsl");
    cmpMtxPvrMediaSFRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tMedia, "GetPVRFile",
                                                                      [{ VarName: "ObjectList_WhereParentNID", VarRef: "PvrMediaSDRSelected"},
                                                                       { VarName: "ObjectList_SelectFormat", VarRef: "PvrSelectFormat"},
                                                                       { VarName: "ObjectList_WherePermissions", VarRef: "PvrWherePermissions"}
                                                                      ],
                                                                      { action: "FNCCALL", context: cmpMtxPvrMediaSFRList,
                                                                          fnc: function(respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                  //HACK:: strip out duplicates from list
                                                                                  var iterFile = respVal.selectNodeIterator("./File");
                                                                                  while (iterFile.hasNext()) {
                                                                                      var nodeFile = iterFile.next();
                                                                                      var fileNID = nodeFile.getAttribute("NID");
                                                                                      var dateCreated = getDateTimeFromTimeCode(nodeFile.getAttribute("dateCreated"));
                                                                                      nodeFile.setAttribute("dateCreated", dateCreated);
                                                                                      // if there is a clip in a group that has the same NID - remove this from the list
                                                                                      var nodeClip = respVal.selectSingleNode(".//FileList/File[@NID='" + fileNID + "']");
                                                                                      if (nodeClip) {
                                                                                          nodeFile.getParent().removeChild(nodeFile);
                                                                                      }
                                                                                  }
                                                                                  this.onResponse(respCode, respVal);
                                                                                  handlePvrMediaSFRSelected();
                                                                              }
                                                                              else {
                                                                                  log.info("GetFileRequest failed. Error Code :" + respCode);
                                                                                  this.onResponseErrorHandle("GetPVRFile",respCode, respVal);
                                                                              }
                                                                          }
                                                                      }
                                                                    );
  
  cmpMtxPvrMediaSFRList.BlkJSX.setEvent("cmpHandleToggle(this, strRECORDID, bOPEN);", jsx3.gui.Interactive.TOGGLE);

  this.cmpHandleToggle = function(objJSX, strRecordID, bOpen) {
      if( !bOpen ) { // to get back plus icon when tree collapsed
        var objRecordNode = objJSX.getRecordNode(strRecordID);
        objRecordNode.setAttribute("jsximg", "jsx:/images/tree/plus.gif");
        objJSX.redrawRecord(strRecordID, jsx3.xml.CDF.UPDATE);
      }
    }

 var cmpMediaSFRNavControls = new CmpPageNavButtonsContainer([blkJSX, "blkPvrPagination"],
                                                              [[blkJSX, "btnPageFirst"],
                                                               [blkJSX, "btnPagePrev"],
                                                               [blkJSX, "btnPageNext"],
                                                               [blkJSX, "btnPageLast"],
                                                               [blkJSX, "lblRecordInfo"],
                                                               [blkJSX, "txtCurrentRecordNumber"],
                                                               [blkJSX, "btnPageRefresh"],
                                                               [blkJSX, "btnPageRefreshSpinner"],
                                                               [blkJSX, "btnPageRefreshNotify"]
                                                              ],
                                                              cmpMtxPvrMediaSFRList,
                                                              [{action:"TOPICEVENT", topic:tMedia, eventvar:"PvrMediaStreamFileResourceDataUpdated", eventval:null}],
                                                              null
                                                             );

  var cmpPvrMediaSFRList = new CmpContainer(blkJSX,
                                          [{ cmp: cmpMediaSFRNavControls },
                                           { cmp: cmpMtxPvrMediaSFRList }
                                          ],
                                          null);
                                                                       
  /********************************* SUBSCRIPTIONS & CUSTOM FUNCTIONS **************************/                                                  
  
  // subscription to the AGENTEVNT for dir empty process finish etc. defined in cmpNotification.js 
  var PvrAgenteventEventID = eventApp.subscribe("AGENTEVENT", null, this,
                                             function(EventVar, EventVal) {
                                               var agentEvent = EventVal;
                                               if( (agentEvent.Action == "EmptyDirProcess") && 
                                                   (agentEvent.eventCode == "1001") ) {
                                                 var DirNID = agentEvent.mediadirnid;
                                                 topic_Set(DirNID, "EmptyDirState", "ENABLE");
                                                 if( topic_Get(tMedia, "PvrMediaSDRSelected") == DirNID ) {
                                                   topic_Publish(tMedia, "EmptyDirState", "ENABLE");
                                                   // call repaint for the list
                                                   cmpMtxPvrMediaSFRList.doRepaint();                                                   
                                                 }
                                               }
                                             } // function end
                                          );

  function resetPvrMediaFileSelectedPermissions() {
    topic_Set(tMedia, "PvrMediaSFRSelectedExportPermission", "false"); 
  
    topic_Publish(tMedia, "PvrMediaSFRSelectedPermissions", (newServerDate()).getTime());  
  }
    
  //when the file is selected checks whether it has write permission is true or false and stateflag
  eventMedia.subscribe("PvrMediaSFRSelected", null, this,
                      function() {
                        resetPvrMediaFileSelectedPermissions();
                        handlePvrMediaSFRSelected();
                      } // function end
                   );
  function handlePvrMediaSFRSelected() {
    var MediaFileNID = topic_Get(tMedia, "PvrMediaSFRSelected");
    if( (MediaFileNID == "NONE") || (MediaFileNID == null) ) {
      // check if anything selected in the matrix
      var selRecordNodes = cmpMtxPvrMediaSFRList.BlkJSX.getSelectedNodes();
      if(selRecordNodes.size() > 1){
        topic_Set(tMedia, "MediaSFRSelected", "MULTI-ROWS");
        MediaFileNID = "MULTI-ROWS";
      }
      else if(selRecordNodes.size() == 1){
        for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
          var selRecordNode = it.next();
          MediaFileNID = selRecordNode.getAttribute("jsxid");
          topic_Set(tMedia, "PvrMediaSFRSelected", MediaFileNID);
        }        
      }
      else {
        return;
      }
    }
    //For now operations like Edit/Repair etc.. 
    //will be supported only for single file
    topic_Set(tMedia,"PVRFileWriteMode", "NONE");
    topic_Set(tMedia,"PVRFileStateFlag", "NONE");
    if( MediaFileNID != "MULTI-ROWS" ) {
      var selRecord = cmpMtxPvrMediaSFRList.BlkJSX.getRecord(MediaFileNID);
      if( selRecord == null ) return;
      var fileReadOnlyMode = selRecord.jsxtextmediaReadOnly;  
      var fileStateFlag = selRecord.jsxtextmediaStateFlag;
      var fileName = selRecord.jsxtextmediaName;
      if( fileName == "ORPHANED_MEDIA_GROUP_FILE" ){
        topic_Set(tMedia,"FileName", "ORPHANED_MEDIA_GROUP_FILE");
      }
      else {
        topic_Set(tMedia,"FileName", "NONE");
      }
      
      if( fileReadOnlyMode != null && fileReadOnlyMode == "false" ) {
        topic_Set(tMedia,"PVRFileWriteMode", "ENABLED");
      }
      else {
        topic_Set(tMedia,"PVRFileWriteMode", "NONE");
      }
      
      if( fileStateFlag == "Error" ) {
        topic_Set(tMedia,"PVRFileStateFlag", "ERROR");
      }
      else {
        topic_Set(tMedia,"PVRFileStateFlag", "NONE");
      }
            
      //set the file permissions
      var dirstateFlag = topic_Get(tApp, "MediaDirStateFlag");
      if ( jsx3.util.strEmpty(dirstateFlag) || dirstateFlag != 'Deleting') {
        var mediaFileNode = cmpMtxPvrMediaSFRList.BlkJSX.getRecordNode(MediaFileNID);    
        
        var exportPer = mediaFileNode.selectSingleNode("./Permissions/@export").getValue();
        if( jsx3.util.strEmpty(exportPer) ){
          exportPer = "false";
        }     
        topic_Set(tMedia, "PvrMediaSFRSelectedExportPermission", exportPer);
      }
      else {
        topic_Set(tMedia, "PvrMediaSFRSelectedExportPermission", "false"); 
      }     
      
      // publish the file permissions
      topic_Publish(tMedia, "PvrMediaSFRSelectedPermissions", (newServerDate()).getTime());
   
    }
  }

  eventMedia.subscribe("GetPvrMediaFiles", null, this,
                      function () {
                        if (topic_Get(tMedia, "PvrMediaSFRGetData") != "NONE") return;

                        topic_Set(tMedia, "PvrMediaSFRSelected", "NONE");
                        resetPvrMediaFileSelectedPermissions();
                        var selectedNID = topic_Get(tApp, "PvrMediaSDRSelected");
                        if (selectedNID == "NONE") return;
                        cmpMtxPvrMediaSFRList.ResetToStart(1, 50, 0);
                        topic_Publish(tMedia, "PvrMediaSFRGetData", (newServerDate()).getTime());
                      }
                   );                   

  eventMedia.subscribe("PvrRefreshMediaSFRList", null, this,
                      function() {
                        var selectedNID = topic_Get(tApp, "PvrMediaSDRSelected");
                        if( selectedNID == "NONE" ) {
                          cmpMtxPvrMediaSFRList.clearXMLData();
                          return;
                        }
                        cmpMtxPvrMediaSFRList.doRepaint();
                      }
                   );

  eventMedia.subscribe("MediaSDRSelected", null, this,
                      function (EventVar, EventVal) {
                        if (EventVal == "NONE") {
                          cmpMtxPvrMediaSFRList.doRepaint();
                        }
                        topic_Set(tMedia, "PvrMediaSFRGetData", "NONE");
                      }
                   );
  eventMedia.subscribe("PvrMediaSDRSelected", "NONE", this,
                      function() {
                        cmpMtxPvrMediaSFRList.doRepaint();
                      }
                   );


  var PvrMediaSFRUpdateEventID = eventApp.subscribe("MediaSFRDataUpdateNode", null, this,
                                                 function(EventVar, EventVal) {
                                                   var MediaFileNID = EventVal;
                                                   var mediaSFRNode = topic_GetNIDNode(tCache, MediaFileNID);
                                                   if( mediaSFRNode != null && checkDisplayMediaSFRNIDList(mediaSFRNode) == true ) {
                                                     //already checked that this files belongs to the matrix so update it
                                                     var objRecord = getObjectRecord(MediaFileNID, mediaSFRNode);
                                                     cmpMtxPvrMediaSFRList.BlkJSX.insertRecord(objRecord, null, false);
                                                     cmpMtxPvrMediaSFRList.BlkJSX.repaintData();
                                                   }
                                                 });

  var PvrMediaSFRAddEventID = eventApp.subscribe("MediaSFRDataAddNode", null, this,
                                               function(EventVar, EventVal) {
                                                 var MediaFileNID = EventVal;
                                                 var mediaSFRNode = topic_GetNIDNode(tCache, MediaFileNID);
                                                 if( mediaSFRNode!= null && checkDisplayMediaSFRNIDList(mediaSFRNode) == true ) {
                                                   var objRecord = getObjectRecord(MediaFileNID, mediaSFRNode);
                                                   cmpMtxPvrMediaSFRList.BlkJSX.insertRecord(objRecord, null, false);
                                                   cmpMtxPvrMediaSFRList.BlkJSX.repaintData();
                                                 }
                                               });

  var PvrMediaSFRDeleteEventID = eventApp.subscribe("MediaSFRDataDeleteNode", null, this,
                                                 function(EventVar, EventVal) {
                                                   var MediaFileNID = EventVal;
                                                   // no need to check node
                                                   cmpMtxPvrMediaSFRList.BlkJSX.deleteRecord(MediaFileNID);
                                                 });

  var PvrMediaSFRDBUpdateEventID = eventApp.subscribe("DBUpdate", null, this,
                                                     function (EventVar, EventVal) {
                                                       if (jsx3.util.strEmpty(EventVal)) return;
                                                       if (EventVal.objType == "MediaStreamFileResource") {
                                                         //Incase of MediaStream file it is tricky to find that the notification is 
                                                         //exactly for the current list or not.
                                                         //In case of Delete also UI will get replace notification as it is moved to RECYCLEBIN
                                                         //So to let user know that just marking as Dirty.
                                                         var fileNID = EventVal.nodeID;
                                                         var strXMLCacheId = cmpMtxPvrMediaSFRList.BlkJSX.getXMLId();
                                                         var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                         if (objXML) {
                                                           var fileRecord = objXML.selectSingleNode(".//record[@jsxid='" + fileNID + "']");
                                                           if (!jsx3.util.strEmpty(fileRecord)) {
                                                             cmpMtxPvrMediaSFRList.markDirty();
                                                           }
                                                         }

                                                       }
                                                     });

    topic_Set(tMedia, "PvrMediaSFRUpdateEventID", PvrMediaSFRUpdateEventID);
    topic_Set(tMedia, "PvrMediaSFRAddEventID", PvrMediaSFRUpdateEventID);
    topic_Set(tMedia, "PvrMediaSFRDeleteEventID", PvrMediaSFRUpdateEventID);
    topic_Set(tMedia, "PvrAgenteventEventID", PvrAgenteventEventID);
    topic_Set(tMedia, "PvrMediaSFRDBUpdateEventID", PvrMediaSFRDBUpdateEventID);


  // subscribe to the attach detach event and then refresh the files list
  eventMedia.subscribe("MediaDirAttachDetach", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "COMPLETE" ) {
                           // call repaint for the list
                           cmpMtxPvrMediaSFRList.doRepaint();          
                        }
                      }
                 ); 
                 
  return cmpPvrMediaSFRList;
                   
  
  function checkDisplayMediaSFRNIDList(mediaSFRNode, MediaDirNID) {
    // no need to check whether the file belongs the selected dir.
    //pagination gets the # of files, so check whether the updated file belongs to the currently displayed files.
    //check the file nid is in the current matrix cdf if yes then return true else false
    var fileNID = mediaSFRNode.getAttribute("NID");
    var strXMLCacheId = cmpMtxPvrMediaSFRList.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    if (objXML) {
      var fileRecord = objXML.selectSingleNode(".//record[@jsxid='"+ fileNID +"']"); 
      if(!jsx3.util.strEmpty(fileRecord) ) {
        return true;
      }
      else {
        return false;
      }  
    }
    return false; 
  }


  function getObjectRecord(objectNID, objectNode) {
    var mediaNode = objectNode;
    var o = new Object();
    // use objectNID for jsxid
    o.jsxid = objectNID;

    var mediaName = "";
    var mediaNameNode = mediaNode.selectSingleNode(".//Info/Title");
    if( mediaNameNode != null ) {
      mediaName = mediaNameNode.getValue();
    }
    o.jsxtextmediaName = mediaName;

    var mediaStatus = "";
    var mediaStatusNode = mediaNode.selectSingleNode(".//State/StateFlag");
    if( mediaStatusNode != null ) {
      mediaStatus = mediaStatusNode.getValue();
    }
    o.jsxtextmediaStatus = mediaStatus;

    var mediaSize = "";
    var mediaSizeNode = mediaNode.selectSingleNode(".//Size");
    if( mediaSizeNode != null ) {
      mediaSize = mediaSizeNode.getValue();
    }
    o.jsxtextmediaSize = convertBytesToMegaBytes(mediaSize);

    var mediaCreated = "";
    var mediaCreatedNode = mediaNode.selectSingleNode(".//Info/Properties/DateCreated");
    if( mediaCreatedNode != null ) {
      mediaCreated = mediaCreatedNode.getValue();
    }
    o.jsxtextmediaCreated = getDateTimeFromTimeCode(mediaCreated);

    var mediaLength = "";
    var mediaLengthNode = mediaNode.selectSingleNode(".//PlayLength");
    if( mediaLengthNode != null ) {
      mediaLength = mediaLengthNode.getValue();
    }
    o.jsxtextmediaLength = covertMilliSecondToHhMmSs(mediaLength);
    return o;
  }


} // end cerate
