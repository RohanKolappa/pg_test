function cmpBlkSessionMonitor_Create() {
  var blkSessionBlkJSX = PNameSpace.getJSXByName("blkSessionMonitor");
  blkSessionBlkJSX.load("components/programs/sessionMonitor.xml");
  
  topic_Set(tApp, "SessionListRefreshInterval",5); 
  topic_Set(tApp, "SessionGetDataState", "NONE");
  var SessionCountToFetch = 50;

  var cmpMtxSessionList = new CmpCustomPaginationList(["blkSessionList", "mtxSessionsList"],
                                                      [{ action: "APPEVENT", eventvar: "SessionGetData", eventval: null },
                                                       { action: "APPEVENT", eventvar: "SessionListRefreshInterval", eventval: null}],
                                                      { persist: { col: "jsxid", initval: false },
                                                        action: "APPEVENT", eventvar: "SessionSelected", eventval: "jsxid"
                                                      },
                                                      [{ ColumnName: "jsxSessionDuration", FncName: "showDuration" },
                                                       { ColumnName: "jsxSessionStartTime", FncName: "@datetime,MM/dd/yyyy HH:mm" },
                                                       { ColumnName: "jsxPersistentPVR", FncName: "showPersistentPVR"},
                                                       { ColumnName: "imgDeleteButtonColumn", FncName: "showDeleteButtonColumn"}],
                                                      null
                                                     );                                                    
                                                 
  cmpMtxSessionList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                       CountToFetch: "ObjectList_CountToFetch",
                                       SortOrder: "ObjectList_SortOrder",
                                       ObjectCount: "ObjectCount",
                                       SortByXpath: "ObjectList_SortBy"
                                     };
                                     
  cmpMtxSessionList.RecordArr = [{objColumnName:"jsxid", objXPath:"\"./@id\""},
                                 {objColumnName:"jsxSessionName", objXPath:"\"./@name\""},
                                 {objColumnName:"jsxSessionOwner", objXPath:"\"./Properties/@ownerUserJID\""},
                                 {objColumnName:"jsxSessionDuration", objXPath:"\"./Properties/@dateCreated\""},
                                 {objColumnName:"jsxSessionStartTime", objXPath:"\"./Properties/@dateCreated\""},
                                 {objColumnName:"jsxCreationTime", objXPath:"\"./Properties/@dateCreated\""},
                                 {objColumnName:"jsxPersistentPVR", objXPath:"\"./@type\""}
                                ];                                     
  cmpMtxSessionList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "SessionDataUpadted", eventval: "LOADED" };

  topic_Set(tApp, "SessionSelectFormat", "BASIC");
  cmpMtxSessionList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetSession",
                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "SessionSelectFormat"}],
                                                                    { action: "FNCCALL", context: cmpMtxSessionList,
                                                                      fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                          this.onRefreshResponse(respCode, respVal);
                                                                        }
                                                                        else {
                                                                          log.info("GetSessionListRequest failed. Error Code :" + respCode);
                                                                          this.onResponseErrorHandle("GetSession",respCode, respVal);
                                                                        }
                                                                      }
                                                                    }
                                                                  ); 
                                                                    
  cmpMtxSessionList.RefreshInterval = {topic:tApp, topicVar:"SessionListRefreshInterval", DataNodeIteratorXPath:".//Session"};
  cmpMtxSessionList.IntervalID = null;

  if (clsUserPermission.getPermission("Session", "sessionmanagement") == "false") {
    var colObj = cmpMtxSessionList.BlkJSX.getDescendantOfName("imgDeleteButtonColumn", false, false);
    colObj.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  //Attach the function to cmpMtxSessionList delete button to delete/stop session
  var imgDeleteSessionBlkJSX = cmpMtxSessionList.BlkJSX.getDescendantOfName("imgSessionDelete");
  imgDeleteSessionBlkJSX.setEvent('eval("DeleteSession(this);")', jsx3.gui.Interactive.EXECUTE);
   
  // function associated with the delete session
  DeleteSession = function(obj) {
    if (clsUserPermission.getPermission("Session", "delete") == "false") {
      alert("You do not have permissions to delete/stop the session.");
      return;
    }
    var objRecordId = obj.emGetSession().recordId;
    var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
    var persistentPVR = ObjRecord.jsxPersistentPVR;
    if(!jsx3.util.strEmpty(persistentPVR) && persistentPVR == "SYSTEM"){
      var objAlert = PNameSpace.confirm("Warning",
                                  "Deleting a persistent PVR session may adversely affect media clients currently active in this session. In most cases, users will need to close and restart their media client application. Are you sure you want to continue?",
                                  DeleteSessionItem, null, "Yes", "No");
    }
    else if(!jsx3.util.strEmpty(persistentPVR) && persistentPVR == "SYSTEM.READONLY"){
      alert("Auto start session can not be deleted. It will be deleted, when device is deleted.");
      return;
    }
    else{
      var objAlert = PNameSpace.confirm("Warning",
                                      "Are you sure you want delete/stop this session?",
                                      DeleteSessionItem, null, "Yes", "No");
    }
    objAlert.selectedID = objRecordId;
  }   // end DeleteSession
  
  // Yes will this function and No will automatically close the dialog 
  function DeleteSessionItem(ObjConfirm) {
    if( ObjConfirm == null ) { return;}
    var sessionID = ObjConfirm.selectedID;
    ObjConfirm.doClose();
    
    var reqStopSession = new ReqSessionStartStopRequest("Stop", null,
                                                          { action: "FNCCALL", context: null,
                                                            fnc: function(respCode, respVal) {
                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                topic_Publish(tApp,"SessionSelected", null);
                                                              }
                                                              else {
                                                                log.error("Stop Session for SessionID: " + sessionID + "request failed");
                                                              }
                                                            }
                                                          }
                                                        );
    reqStopSession.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["SessionID"] = sessionID;
    reqStopSession.reqParams = reqParams;
    reqStopSession.sendRequest(this);       
  }// end DeleteSessionItem
        
  var cmpSessionNavControls = new CmpPageNavButtonsContainer(["blkSessionList", "blkSessionListPaging"],
                                                            [["blkSessionListPaging", "btnPageFirst"],
                                                             ["blkSessionListPaging", "btnPagePrev"],
                                                             ["blkSessionListPaging", "btnPageNext"],
                                                             ["blkSessionListPaging", "btnPageLast"],
                                                             ["blkSessionListPaging", "lblRecordInfo"],
                                                             ["blkSessionListPaging", "txtCurrentRecordNumber"],
                                                             ["blkSessionListPaging", "btnPageRefresh"],
                                                             ["blkSessionListPaging", "btnPageRefreshSpinner"]
                                                            ],
                                                            cmpMtxSessionList,
                                                            [{ action: "TOPICEVENT", topic: tApp, eventvar: "SessionDataUpadted", eventval: null}],
                                                            null
                                                           );

                                                                                                        
  var cmptxtSessionRefreshSet = new CmpTxtBox(["blkSessionList","txtSessionRefreshTime"],
                                               topic_Get(tApp, "SessionListRefreshInterval"),
                                               null,
                                               null,
                                               null
                                              );
                                                                                                         
  var cmpBtnSessionRefreshSet = new CmpButton(["blkSessionList","btnSessionRefreshSet"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:this,
                                                    fnc:function() {
                                                      var newMediaRoomListRefreshInterval = cmptxtSessionRefreshSet.BlkJSX.getValue();
                                                      var chkValue = checkIntergerValue(newMediaRoomListRefreshInterval, "txtSessionRefreshTime", "Please enter an integer refresh time value");
                                                      if(chkValue) {
                                                        if(newMediaRoomListRefreshInterval == 0 ) {
                                                          var agree = confirm("Are you sure you want to disable mediaroom refresh?");
                                                          if(agree) {
                                                            topic_Publish(tApp, "SessionListRefreshInterval",newMediaRoomListRefreshInterval);
                                                          }
                                                        } else if(newMediaRoomListRefreshInterval < 5) {
                                                          alert("Please enter an integer refresh time value greater than or equal to 5");
                                                          var intervalTime = topic_Get(tApp, "SessionListRefreshInterval");
                                                          cmptxtSessionRefreshSet.BlkJSX.setValue(intervalTime);
                                                          cmptxtSessionRefreshSet.BlkJSX.focus();
                                                          return false;
                                                        } else {
                                                          topic_Publish(tApp, "SessionListRefreshInterval", newMediaRoomListRefreshInterval);
                                                        }
                                                      }
                                                      else{
                                                        var intervalTime = topic_Get(tApp, "SessionListRefreshInterval");
                                                        cmptxtSessionRefreshSet.BlkJSX.setValue(intervalTime);
                                                        cmptxtSessionRefreshSet.BlkJSX.focus();
                                                      }

                                                    }
                                               }
                                             );

                                                                                                                                                                                                                                                   
  var cmpBlkSessionList = new CmpContainer("blkSessionList",
                                           [{cmp:cmpMtxSessionList},
                                            {cmp:cmpSessionNavControls},
                                            {cmp:cmptxtSessionRefreshSet},
                                            {cmp:cmpBtnSessionRefreshSet}
                                           ],
                                           null
                                           );
  var cmpBtnSessionRefresh = new CmpButton(["blkSessionDetails", "btnSessionRefresh"], null,
                                            { action: "FNCCALL", context: null,
                                              fnc: function() {
                                                var sessionID = topic_Get(tApp,"SessionSelected");
                                                if( (!jsx3.util.strEmpty(sessionID)) && 
                                                    (sessionID != "NONE") ){
                                                  cmpMtxSessionConnSeg.doRepaint();
                                                }
                                              }
                                            });

  var cmpMtxSessionConnSrc = new Component(["blkSessionDetails", "mtxSessionSrcDetail"]
                                          );
  cmpMtxSessionConnSrc.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConnSrc.xsl"); 
                                           
  var cmpMtxSessionConnDest = new Component(["blkSessionDetails", "mtxSessionDestDetail"]
                                           );
  cmpMtxSessionConnDest.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConnDest.xsl");                                          
  
  var cmpMtxSessionConnPvrRec = new Component(["blkSessionDetails", "mtxSessionPVRDetail"]
                                             );                                                                                    
  cmpMtxSessionConnPvrRec.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConnPvrRec.xsl");
       
  
  var cmpMtxSessionConnSeg = new CmpCustomPaginationList(["blkSessionDetails","mtxSessionConnections"],
                                                      [{action:"APPEVENT", eventvar:"SessionConnData", eventval:null}],
                                                      {persist:{col:"jsxid",initval:false},
                                                       action:"APPEVENT", eventvar:"SessionConnSelected", eventval:"jsxid"},
                                                      null,
                                                      "showConnErrorDetails"
                                                     ); 
  cmpMtxSessionConnSeg.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                         CountToFetch: "ObjectList_CountToFetch",
                                         SortOrder: "ObjectList_SortOrder",
                                         ObjectCount: "ObjectCount",
                                         SortByXpath: "ObjectList_SortBy"
                                        };
                                                                                           
  cmpMtxSessionConnSeg.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "SessionConnDataUpadted", eventval: "DATE" };
  cmpMtxSessionConnSeg.BlkJSX.setXMLTransformers("../IPVS-Common/xsl/SessionConn.xsl");
  
  topic_Set(tApp, "SessionConnSelectFormat", "EXTENDED");
  cmpMtxSessionConnSeg.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetSession",
                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "SessionConnSelectFormat"},
                                                                     { VarName: "ObjectList_WhereID", VarRef: "SessionSelected"}
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpMtxSessionConnSeg,
                                                                      fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                          // clone response for the src entities
                                                                          var DataNodeForSrc = respVal.cloneNode(true);
                                                                          setMatrixSourceXML(DataNodeForSrc, cmpMtxSessionConnSrc);
                                                                          // clone response for the dest entities
                                                                           var DataNodeForDest = respVal.cloneNode(true);
                                                                           setMatrixSourceXML(DataNodeForDest, cmpMtxSessionConnDest);
                                                                          // clone response for the pvr/rec entities
                                                                           var DataNodeForPvrRec = respVal.cloneNode(true);
                                                                           setMatrixSourceXML(DataNodeForPvrRec, cmpMtxSessionConnPvrRec);
                                                                          // data for the connections
                                                                          this.onResponse(respCode, respVal);
                                                                        }
                                                                        else {
                                                                          log.info("GetSessionRequest failed. Error Code :" + respCode);
                                                                          this.onResponseErrorHandle("GetSession",respCode, respVal);
                                                                        }
                                                                      }
                                                                    }
                                                                  );   
  
                                     
  var cmpBlkSessionDetails = new CmpContainer("blkSessionDetails",
                                              [{cmp:cmpMtxSessionConnSeg},
                                               {cmp:cmpMtxSessionConnSrc},
                                               {cmp:cmpMtxSessionConnDest},
                                               {cmp:cmpMtxSessionConnPvrRec},
                                               {cmp:cmpBtnSessionRefresh}
                                              ],
                                              null
                                              );
                                                
  var cmpBlkSessionMonitor = new CmpContainer("blkSessionMonitor",  
                                              [{cmp:cmpBlkSessionList},
                                               {cmp:cmpBlkSessionDetails}
                                              ],
                                              null );


  eventApp.subscribe("MENUUIDATA", "SESSIONMONITOR", this,
                      function(EventVar, EventVal) {
                        if( EventVal == "SESSIONMONITOR" ) {
                          var getDataState = topic_Get(tApp, "SessionGetDataState");
                          if(getDataState == "NONE") {
                            topic_Set(tApp, "SessionGetDataState", (newServerDate()).getTime() );
                            cmpMtxSessionList.ResetToStart(1,SessionCountToFetch,0);
                            topic_Publish(tApp, "SessionGetData", (newServerDate()).getTime() ); 
                          }
                        }
                        else {
                          // clear the refreshInterval
                          if( cmpMtxSessionList.IntervalID != null) {
                            //clearInterval(cmpMtxSessionList.IntervalID);
                            //cmpMtxSessionList.IntervalID = null;
                          }
                        }
                      }
                    );
                      
  eventApp.subscribe("SessionSelected", null, this,
                      function(EventVar, EventVal) {
                        var sessionID = EventVal;
                        if( (!jsx3.util.strEmpty(sessionID)) && 
                            (sessionID != "NONE") ){
                          topic_Publish(tApp, "SessionConnData", (newServerDate()).getTime() );
                        }
                        else{
                          // clear the connection detail, entities block
                          var EmptyDataNode = topic_Set(tApp, "GetSession", null);
                          setMatrixSourceXML(EmptyDataNode, cmpMtxSessionConnSrc);
                          setMatrixSourceXML(EmptyDataNode, cmpMtxSessionConnDest);
                          setMatrixSourceXML(EmptyDataNode, cmpMtxSessionConnPvrRec);                          
                          setMatrixSourceXML(EmptyDataNode, cmpMtxSessionConnSeg);                          
                        }
                        
                      }
                    );   
                    
  function setMatrixSourceXML(dataNode, mtxObj) {
    var strXMLCacheId = mtxObj.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(dataNode.toString());

    mtxObj.BlkJSX.setSourceXML(objXML);
    mtxObj.BlkJSX.repaint();  
    var objXML2 = PNameSpace.getCache().getDocument(strXMLCacheId);
  } // end   setMatrixSourceXML
                              
                                       
  return cmpBlkSessionMonitor;
} // end create

function showConnErrorDetails(objJSX, strRecordId){
  var ObjRecord = objJSX.getRecord(strRecordId);
  var conStatus = ObjRecord.jsxSessionConnectionStatus;
  if( (conStatus=="SETUP_ERROR") || (conStatus=="START_ERROR") || (conStatus=="SOURCE_ERROR") || (conStatus == "MEDIA_ERROR") || (conStatus == "DEST_ERROR") || (conStatus=="error") ) {
    var ErrCode =  ObjRecord.jsxSessionConnectionErrorCode;
    var ErrDesc = ObjRecord.jsxSessionConnectionErrorDesc;
    var strSpyHTML = '<div id="outerDiv" style="margin: -4px; background-color: #ffffcc;">';
    strSpyHTML = strSpyHTML + '<table width="200" hight="200" border="0">';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">Error Code: </td><td style="font-family:Arial;font-size:10px;">' + ErrCode + '</td></tr>';
    strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">Error Desc: </td><td style="font-family:Arial;font-size:10px;">' + ErrDesc + '</td></tr></table></div>';
    return strSpyHTML;
  }
  return;
} // end showConnErrorDetails

function showPersistentPVR(element, cdfkey, matrix, column, rownumber, server){
  var selRecord = matrix.getRecord(cdfkey);
  var persistentPVR = selRecord.jsxPersistentPVR;
  var innerText = element.innerHTML;
  var sessionPersistentPVRButton = matrix.getDescendantOfName("jsxPersistentPVR");
  if(persistentPVR == "SYSTEM"){
    element.innerHTML = innerText;
    sessionPersistentPVRButton.setTip("Uncheck Enable PVR in PVR Settings page to stop the session");
  }
  else{
    element.innerHTML = "<SPAN onmouseup='jsx3.dl(event,this,'yo',0);' class=jsx30imagebutton onmousedown='jsx3.dl(event,this,'Fo',0);' id=_jsx_0_7n onkeydown='jsx3.dl(event,this,'_ebKeyDown',0);' onmouseover='jsx3.dl(event,this,'Ri',0);' + "
    "+style='WIDTH: 16px; CURSOR: pointer; POSITION: relative; HEIGHT: 16px' onclick='jsx3.dl(event,this,'nj',0);' tabIndex=0 onmouseout='jsx3.dl(event,this,'Uc',0);' label='imgSessionPersistentPVR' jsxtabindex='0'> + "
    "+<DIV style='POSITION: relative; HEIGHT: 16px'><IMG height=16 alt='' src='' width=16></DIV></SPAN>";
  }
}

function showDeleteButtonColumn(element, cdfkey, matrix, column, rownumber, server){
  var selRecord = matrix.getRecord(cdfkey);
  var persistentPVR = selRecord.jsxPersistentPVR;
  var innerText = element.innerHTML;
  if(persistentPVR != "SYSTEM.READONLY"){
    element.innerHTML = innerText;
  }
  else{
    element.innerHTML = "<SPAN onmouseup='jsx3.dl(event,this,'yo',0);' class=jsx30imagebutton onmousedown='jsx3.dl(event,this,'Fo',0);' id=_jsx_0_7n onkeydown='jsx3.dl(event,this,'_ebKeyDown',0);' onmouseover='jsx3.dl(event,this,'Ri',0);' + "
    "+style='WIDTH: 16px; CURSOR: pointer; POSITION: relative; HEIGHT: 16px' onclick='jsx3.dl(event,this,'nj',0);' tabIndex=0 onmouseout='jsx3.dl(event,this,'Uc',0);' label='imgSessionDelete' jsxtabindex='0'> + "
    "+<DIV style='POSITION: relative; HEIGHT: 16px'><IMG height=16 alt='' src='' width=16></DIV></SPAN>";
  }
}

function cmpBlkSessionMonitor_Destroy() {
  var blkSessionBlkJSX = PNameSpace.getJSXByName("blkSessionMonitor");
  blkSessionBlkJSX.removeChildren();
}

