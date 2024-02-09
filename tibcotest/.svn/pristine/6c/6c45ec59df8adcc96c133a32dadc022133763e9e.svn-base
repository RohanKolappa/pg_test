function cmpBlkExportImportStatus_Create() {
  var DlgFile = "components/media/mediaExportStatusDialog.xml";
  
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function cmpBlkExportImportStatus_Destroy() {
    eventApp.unsubscribe("DBUpdate", MediaExportImportNotificationEventID);
    
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgExportImportStatus.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
  topic_Set(tDlg, "ExportImportItemSelected", "NONE");
  topic_Set(tDlg, "ExportImportQueueStatus", "NONE");
  topic_Set(tDlg, "QueueListXpath", "[//@queue_state!='-1000']");
  topic_Set(tDlg, "HistoryListXpath", "");

  var cmpBtnClose = new CmpButton(["dlgExportImportStatus","btnDialogCloseX"],
                                   null, 
                                   {action:"FNCCALL", context:null, 
                                    fnc:function() {
                                      cmpBlkExportImportStatus_Destroy();
                                    }
                                   });

  var cmpBtnMinimize = new CmpButton(["dlgExportImportStatus","btnMinimize"],
                                     null, 
                                     {action:"FNCCALL", context:null, 
                                      fnc:function() {
                                        if(topic_Get(tDlg, "QueueStatusTabSelected") == null || topic_Get(tDlg, "QueueStatusTabSelected") == "QUEUE") {
                                          cmpQueueList.BlkJSX.repaint();
                                        }
                                        BlkJSX.doToggleState();
                                      }
                                     });

  var cmpTabQueue = new CmpTab(["dlgExportImportStatus","tabQueue"],
                               null,
                               {action:"TOPICEVENT", topic:tDlg, eventvar:"QueueStatusTabSelected", eventval:"QUEUE"} 
                              );

  var cmpTabHistory = new CmpTab(["dlgExportImportStatus","tabHistory"],
                                 null,
                                 {action:"TOPICEVENT", topic:tDlg, eventvar:"QueueStatusTabSelected", eventval:"HISTORY"} 
                                );

  eventDlg.subscribe("QueueStatusTabSelected", null, this,
                     function(EventVar, EventVal) {
                       if( EventVal == "QUEUE" ) {
                         topic_Publish(tApp, "ExportImportQueueGetData", (newServerDate()).getTime() );
                       }
                       else if( EventVal == "HISTORY" ) {
                         topic_Publish(tApp, "ExportImportHistoryGetData", (newServerDate()).getTime() );
                       }                                                    
                     }); 

  var cmpQueueList = new CmpCustomPaginationList(["blkQueueList","mtxExportImportlist"],
                                                 [{action:"APPEVENT", eventvar:"ExportImportQueueGetData", eventval:null}],
                                                 {persist:{col:"jsxid",initval:false},
                                                  action:"TOPICEVENT", topic:tDlg, eventvar:"ExportImportItemSelected", eventval:"jsxid"},
                                                 [{ColumnName:"mediaExportFileDate", FncName:"@datetime,MM/dd/yyyy HH:mm"}],
                                                 "showErrorDetails",
                                                 null
                                               );
  
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpQueueList.SortPathArr =  {mediaExportFunction:"upper-case($b/@type)",
                               mediaExportFilename:"upper-case($b//Internal/@title)",
                               mediaExportClientPath:"upper-case($b//External/@folderPath)",
                               mediaExportFileStatus:"upper-case($b/@state_description)",
                               mediaExportMediaDir:"upper-case($b//Internal/@dirTitle)",
                               mediaExportRequestor:"upper-case($b//External/@JID)",
                               mediaExportFileDate:"$b/@dateCreated/number(.)",
                               mediaExportQueuePriority:"$b/@priority/number(.)"
                              };
                               
  cmpQueueList.cmpReqParamArr = { StartFrom: "Op_Select_ListSpec_StartFrom",
                                  CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                  SortOrder: "Op_Select_ListSpec_SortOrder",
                                  ObjectCount: "ObjectCount",
                                  SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                };                                
                                                        
  cmpQueueList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tDlg, eventvar:"QueueDataUpdated", eventval:"LOADED"};                       
  
  cmpQueueList.cmpReq = new cmpReq_GetListReadQuery(tDlg, "JSXAPPS/IPVS_AdminUI/query/MediaExportImportStatus.xml",
                                                    [{VarName:"_WHEREXPATH_", VarRef:"QueueListXpath"}
                                                    ],
                                                    {action: "FNCCALL", context: cmpQueueList,
                                                      fnc: function(respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                            this.onResponse(respCode, respVal);
                                                          }
                                                          else{
                                                            log.info("GetMediaExportImportStatusRequest failed. Error Code :" + respCode);
                                                            this.onResponseErrorHandle("GetMediaExportImportStatus",respCode, respVal);
                                                          }
                                                        }
                                                    }
                                                   );
  cmpQueueList.cmpItemByNIDReqFnc = function () {
                                      var strWhereNID = cmpQueueList.WhereNIDArr.join("\",\"");
                                      topic_Set(tDlg, "WhereNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                      return new cmpReq_GetListReadQuery(tDlg, "JSXAPPS/IPVS_AdminUI/query/MediaExportImportStatus.xml",
                                                                         [{VarName:"_WHEREXPATH_", VarRef:"WhereNID"}
                                                                         ],
                                                                         {action: "FNCCALL", context: cmpQueueList,
                                                                           fnc: function(respCode, respVal) {
                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                    this.onResponseUpdateRecord(respCode, respVal);
                                                                                  }
                                                                                  else{
                                                                                    log.info("GetMediaExportImportStatusRequest failed. Error Code :" + respCode);
                                                                                    this.onResponseErrorHandle("GetMediaExportImportStatus",respCode, respVal);
                                                                                  }
                                                                           }
                                                                         }, "ItemByNID" 
                                                                        );
                                    }

  var cmpQueueListNavControls = new CmpPageNavButtonsContainer(["blkQueueControls","blkPaginationControls"],
                                                                [["blkQueueControls", "btnPageFirst"],
                                                                 ["blkQueueControls", "btnPagePrev"],
                                                                 ["blkQueueControls", "btnPageNext"],
                                                                 ["blkQueueControls", "btnPageLast"],
                                                                 ["blkQueueControls", "lblRecordInfo"],
                                                                 ["blkQueueControls", "txtCurrentRecordNumber"],
                                                                 ["blkQueueControls", "btnPageRefresh"],
                                                                 ["blkQueueControls", "btnPageRefreshSpinner"],
                                                                 ["blkQueueControls", "btnPageRefreshNotify"]
                                                                ],
                                                                cmpQueueList,
                                                                [{action:"TOPICEVENT", topic:tDlg, eventvar:"QueueDataUpdated", eventval:null}],
                                                                null
                                                               ); 

  var cmpBtnClearQueue = new CmpButton(["blkQueueControls","btnExportMediaClearQueue"],
                                       null, 
                                       {action:"FNCCALL", context:null, 
                                        fnc:function() {
                                          cmpDlgHandleExportImportMedia("ClearQueue");
                                        }
                                       });

  var cmpBtnSuspend = new CmpButton(["blkQueueControls","btnExportImportSuspend"],
                                    null, 
                                    {action:"FNCCALL", context:null, 
                                     fnc:function() {
                                      cmpDlgHandleExportImportMedia("Suspend");
                                     }
                                    });

  var cmpBtnResume = new CmpButton(["blkQueueControls","btnExportImportResume"],
                                   null, 
                                   {action:"FNCCALL", context:null, 
                                    fnc:function() {
                                      cmpDlgHandleExportImportMedia("Resume");
                                    }
                                   });

  var cmpBlkQueueControls = new CmpContainer(["dlgExportImportStatus", "blkQueueControls"],
                                             [{cmp:cmpBtnClearQueue},
                                              {cmp:cmpBtnSuspend, sel:"topic_Get(\"" + tDlg + "\", 'ExportImportQueueStatus') == 'InProgress' && " +
                                                                      "topic_Get(\"" + tDlg + "\", 'ExportImportQueueStatus') != 'NONE'"},
                                              {cmp:cmpBtnResume, sel:"topic_Get(\"" + tDlg + "\", 'ExportImportQueueStatus') == 'Suspended' && " +
                                                                      "topic_Get(\"" + tDlg + "\", 'ExportImportQueueStatus') != 'NONE'"}
                                             ],
                                             [{action:"TOPICEVENT", topic:tDlg, eventvar:"ExportImportQueueStatus", eventval:null}] );

  var cmpBlkQueue = new CmpContainer(["dlgExportImportStatus","blkQueue"],
                                     [{cmp:cmpQueueList},
                                      {cmp:cmpBlkQueueControls},
                                      {cmp:cmpQueueListNavControls}
                                     ],
                                     null);

  var cmpHistoryList = new CmpCustomPaginationList(["blkHistoryList","mtxMediaQueueHistory"],
                                                 [{action:"APPEVENT", eventvar:"ExportImportHistoryGetData", eventval:null}],
                                                 {persist:{col:"jsxid",initval:false},
                                                  action:"TOPICEVENT", topic:tDlg, eventvar:"ExportImportItemSelected", eventval:"jsxid"},
                                                 [{ColumnName:"mediaExportFileDateCompleted", FncName:"@datetime,MM/dd/yyyy HH:mm"}],
                                                 "showErrorDetails",
                                                 null
                                               );
  
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpHistoryList.SortPathArr ={mediaExportFunction:"upper-case($b/@type)",
                               mediaExportFilename:"upper-case($b//Internal/@title)",
                               mediaExportClientPath:"upper-case($b//External/@folderPath)",
                               mediaExportFileStatus:"upper-case($b/@state_description)",
                               mediaExportMediaDir:"upper-case($b//Internal/@dirTitle)",
                               mediaExportRequestor:"upper-case($b//External/@JID)",
                               mediaExportFileDateCompleted:"$b/@dateCompleted/number(.)"
                              };
                               
  cmpHistoryList.cmpReqParamArr = { StartFrom: "Op_Select_ListSpec_StartFrom",
                                  CountToFetch: "Op_Select_ListSpec_CountToFetch",
                                  SortOrder: "Op_Select_ListSpec_SortOrder",
                                  ObjectCount: "ObjectCount",
                                  SortByXpath: "Op_Select_ListSpec_SortByXPathList_SortByXpath"
                                };                                
                                                        
  cmpHistoryList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tDlg, eventvar:"HistoryDataUpdated", eventval:"LOADED"};                       
  
  cmpHistoryList.cmpReq = new cmpReq_GetListReadQuery(tDlg, "JSXAPPS/IPVS_AdminUI/query/MediaExportImportStatus.xml",
                                                    [{VarName:"_WHEREXPATH_", VarRef:"HistoryListXpath"}
                                                    ],
                                                    {action: "FNCCALL", context: cmpHistoryList,
                                                      fnc: function(respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                            this.onResponse(respCode, respVal);
                                                          }
                                                          else{
                                                            log.info("GetMediaExportImportStatusRequest failed. Error Code :" + respCode);
                                                            this.onResponseErrorHandle("GetMediaExportImportStatus",respCode, respVal);
                                                          }
                                                        }
                                                    } 
                                                   );
  cmpHistoryList.cmpItemByNIDReqFnc = function () {
                                        var strWhereNID = cmpHistoryList.WhereNIDArr.join("\",\"");
                                        topic_Set(tDlg, "WhereNID", "[//@NID=(\"" + strWhereNID + "\")]");
                                        return new cmpReq_GetListReadQuery(tDlg, "JSXAPPS/IPVS_AdminUI/query/MediaExportImportStatus.xml",
                                                                           [{VarName:"_WHEREXPATH_", VarRef:"WhereNID"}
                                                                           ],
                                                                           {action: "FNCCALL", context: cmpHistoryList,
                                                                             fnc: function(respCode, respVal) {
                                                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                                                      this.onResponseUpdateRecord(respCode, respVal);
                                                                                    }
                                                                                    else{
                                                                                      log.info("GetMediaExportImportStatusRequest failed. Error Code :" + respCode);
                                                                                      this.onResponseErrorHandle("GetMediaExportImportStatus",respCode, respVal);
                                                                                    }
                                                                             }
                                                                           }, "ItemByNID" 
                                                                          );
                                    }

  var cmpBtnClearHistory = new CmpButton(["blkHistoryControls","btnExportMediaClearHistory"],
                                          null, 
                                          {action:"FNCCALL", context:null, 
                                           fnc:function() {
                                             cmpDlgHandleExportImportMedia("ClearHistory");
                                           }
                                          });

  var cmpBlkHistoryControls = new CmpContainer(["dlgExportImportStatus", "blkHistoryControls"],
                                               [{cmp:cmpBtnClearHistory}],
                                               null );

  var cmpHistoryListNavControls = new CmpPageNavButtonsContainer(["blkHistoryControls","blkPaginationControls"],
                                                                [["blkHistoryControls", "btnPageFirst"],
                                                                 ["blkHistoryControls", "btnPagePrev"],
                                                                 ["blkHistoryControls", "btnPageNext"],
                                                                 ["blkHistoryControls", "btnPageLast"],
                                                                 ["blkHistoryControls", "lblRecordInfo"],
                                                                 ["blkHistoryControls", "txtCurrentRecordNumber"],
                                                                 ["blkHistoryControls", "btnPageRefresh"],
                                                                 ["blkHistoryControls", "btnPageRefreshSpinner"],
                                                                 ["blkHistoryControls", "btnPageRefreshNotify"]
                                                                ],
                                                                cmpHistoryList,
                                                                [{action:"TOPICEVENT", topic:tDlg, eventvar:"HistoryDataUpdated", eventval:null}],
                                                                null
                                                               );
                                             
  var cmpBlkHistory = new CmpContainer(["dlgExportImportStatus","blkHistory"],
                                     [{cmp:cmpHistoryList},
                                      {cmp:cmpBlkHistoryControls},
                                      {cmp:cmpHistoryListNavControls}
                                     ],
                                     null);

  var cmpBtnCancel = new CmpButton(["blkFooter","btnExportStatusClose"],
                                   null, 
                                   {action:"FNCCALL", context:null, 
                                    fnc:function() {
                                      cmpBlkExportImportStatus_Destroy();
                                    }
                                   });
                                                                                       
  var cmpDlgExportImportStatus = new CmpContainer("dlgExportImportStatus",
                                                  [ {cmp:cmpTabQueue},
                                                    {cmp:cmpTabHistory},
                                                    {cmp:cmpBtnClose},
                                                    {cmp:cmpBtnMinimize},
                                                    {cmp:cmpBlkQueue, sel:"topic_Get(\"" + tDlg + "\", 'QueueStatusTabSelected') == 'QUEUE' " +
                                                                          "|| topic_Get(\"" + tDlg + "\", 'QueueStatusTabSelected') == null "},
                                                    {cmp:cmpBlkHistory, sel:"topic_Get(\"" + tDlg + "\", 'QueueStatusTabSelected') == 'HISTORY'"},
                                                    {cmp:cmpBtnCancel}
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"QueueStatusTabSelected", eventval:null}]);
                                                            
  cmpDlgExportImportStatus.BlkJSX = BlkJSX;
  cmpDlgExportImportStatus.getBlkData =  cmpDlgExportImportStatus_Repaint;
      
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData();
               }
              ),
              null, cmpDlgExportImportStatus );

  var MediaExportImportNotificationEventID = eventApp.subscribe("DBUpdate", null, this,
                                                                 function(EventVar, EventVal) {
                                                                   if( jsx3.util.strEmpty(EventVal) ) return;
                                                                   if( EventVal.objType != "ExportImportFileListItem" ) return;
                                                                   if( topic_Get(tDlg, "QueueStatusTabSelected") == "HISTORY" ) {
                                                                     cmpHistoryList.handleNotification(EventVal);
                                                                   }
                                                                   else {
                                                                     cmpQueueList.handleNotification(EventVal);
                                                                   }                                                    
                                                                 });
               
  function cmpDlgExportImportStatus_Repaint() {
    topic_Publish(tApp, "ExportImportQueueGetData", (newServerDate()).getTime() );
    cmpDlgHandleExportImportMedia("QueueStatus");
    //Attach the function to list to delete record
    var imgDeleteQueueBlkJSX = cmpQueueList.BlkJSX.getDescendantOfName("imgQueueDelete");
    imgDeleteQueueBlkJSX.setEvent('eval("DeleteQueueItem(this);")', jsx3.gui.Interactive.EXECUTE);
                
    DeleteQueueItem = function(obj) {
      var objRecordId = obj.emGetSession().recordId;
      var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
      //if offload then return no delete
      if (ObjRecord.mediaExportFunction == "Offload" && ObjRecord.mediaExportFileQueueState != "-10") return;
      
      if( ObjRecord.mediaExportFileQueueState == "1") { //currently running
        var objAlert = PNameSpace.confirm("Warning", 
                                          "Deleting this item from the queue will abort the Import/Export function and the file will be unusable", 
                                          abortQueueItem, null, "Continue", "Cancel");
        objAlert.selectedNID = objRecordId;
      }
      else {
        cmpDlgHandleExportImportMedia("Delete", objRecordId);
      }
    }

    cmpQueueList.BlkJSX.removeEvent(jsx3.gui.Interactive.BEFORE_EDIT);
    cmpQueueList.BlkJSX.setEvent('eval("beforeEdit(this,strRECORDID);")', jsx3.gui.Interactive.BEFORE_EDIT);
    beforeEdit = function (obj, strRecordID) {
      var objRecord = obj.getRecord(strRecordID);
      if ( (objRecord.mediaExportQueuePriority == "Prioritized") || (objRecord.mediaExportFunction == "Offload") ) return false;
    }

    var objPriorityBlkJSX = cmpQueueList.BlkJSX.getDescendantOfName("selMediaQueuePriority");
    objPriorityBlkJSX.setEvent('eval("OnPriorityChange(this);")', jsx3.gui.Interactive.SELECT);
    
    OnPriorityChange = function(obj) {
      var objRecordId = obj.emGetSession().recordId;
      var ObjRecord = obj.emGetSession().matrix.getRecord(objRecordId);
      if( ObjRecord.mediaExportFileQueueState == "1") { //currently running
        alert("Cannot change the priority for this item as current status is 'Running'");
        //reset original priority for mask
        if (ObjRecord.selMediaQueuePriority == "0") {
          var OrgPriority = "Prioritized";
        }
        else if( ObjRecord.selMediaQueuePriority == "1" ) {
          var OrgPriority = "High";
        }
        else if( ObjRecord.selMediaQueuePriority == "2" ) {
          var OrgPriority = "Medium";
        }
        else {
          var OrgPriority = "Low";
        }
        obj.setValue(OrgPriority);
        cmpQueueList.BlkJSX.resetMask();
      }
      else {
        var newPriority = obj.getValue();
        if(newPriority == "High" ) {
          newPriority = 1;
        }
        else if(newPriority == "Medium" ) {
          newPriority = 2;
        }
        else {
          newPriority = 3;
        }

        var objExportImportFile = new Object;
        objExportImportFile.Priority = newPriority;
        objExportImportFile.ExternalFileName = "";
        objExportImportFile.ExternalJID = "";
        objExportImportFile.ExternalFolderPath = "";

        cmpDlgUpdateExportImportMedia(objRecordId, objExportImportFile);
        cmpQueueList.BlkJSX.resetMask();
      }
    }
  }

  function abortQueueItem(ObjConfirm) {
    if( ObjConfirm != null ) {
      var QueueNID = ObjConfirm.selectedNID;
      cmpDlgHandleExportImportMedia("Delete", QueueNID);
      ObjConfirm.doClose();
    }
  }

  //To update ExportImportListItem attribute/element or node
  function cmpDlgUpdateExportImportMedia(whereNID, objEIFLI) {    
    var reqUpdateExportImportMedia = new ReqUpdateExportImportMedia(objEIFLI, 
                                                                    {action:"FNCCALL", context:this, 
                                                                       fnc:function(respCode, respVal) {
                                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                                               log.info("Update ExportImportListItem Success.");
                                                                             }
                                                                             else {
                                                                              log.info("Update ExportImportListItem Failed.");
                                                                             }
                                                                           }
                                                                      } 
                                                                   );
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = whereNID;
    reqUpdateExportImportMedia.reqParams = reqParams;  
    reqUpdateExportImportMedia.sendRequest();
  }

  //This function is for all Actions regarding Export/Import
  //Get,Suspend,Resume,QueueStatus,Delete,ClearQueue.
  function cmpDlgHandleExportImportMedia(OpType, QueueNID, ObjNode) {
    if( OpType == "ClearHistory" ) {
      var ObjType = "DeleteExportImportFile";
    }
    else {
      var ObjType = OpType + "ExportImportFile";
    }
    var reqHandleExportImportMedia = new ReqExportImportMedia(ObjType, ObjNode, 
                                                              {action:"FNCCALL", context:this, 
                                                                 fnc:function(respCode, respVal) {
                                                                       if(respCode == "SUCCESS" && respVal != null) {
                                                                         if( OpType == "QueueStatus" ) {
                                                                           var queueState = respVal.selectSingleNode(".//QueueState").getValue();
                                                                           var isSuspended = respVal.selectSingleNode(".//Suspended").getValue();
                                                                           if(isSuspended == "true" || queueState == "suspended") {
                                                                             topic_Publish(tDlg, "ExportImportQueueStatus", "Suspended");
                                                                           }
                                                                           else if(isSuspended == "false" || queueState == "running" ) {
                                                                             topic_Publish(tDlg, "ExportImportQueueStatus", "InProgress");
                                                                           }
                                                                         }
                                                                         else if( OpType == "Suspend" ) {
                                                                           var isSuspended = respVal.selectSingleNode(".//Suspended").getValue();
                                                                           if( isSuspended == "true" ) {
                                                                             topic_Publish(tDlg, "ExportImportQueueStatus", "Suspended");
                                                                           }
                                                                         }
                                                                         else if( OpType == "Resume" ) {
                                                                           topic_Publish(tDlg, "ExportImportQueueStatus", "InProgress");
                                                                         }
                                                                         log.info(ObjType + " Success.");
                                                                       }
                                                                       else {
                                                                        log.info(ObjType + " Failed.");
                                                                       }
                                                                     }
                                                                } 
                                                             );
    if( OpType == "Delete" && QueueNID != null) {
      var reqParams = new Object;
      reqParams["ObjectList_WhereNID"] = QueueNID;
      reqParams["ObjectList_WhereStateDescription"] = "";
      reqHandleExportImportMedia.reqParams = reqParams;
    }
    else if( OpType == "ClearHistory" ) {
      var reqParams = new Object;
      reqParams["ObjectList_WhereStateDescription"] = "Completed";
      reqHandleExportImportMedia.reqParams = reqParams;
    }  
    reqHandleExportImportMedia.sendRequest();
  }
}

function showErrorDetails(objJSX, strRecordId) {
  var ObjRecord = objJSX.getRecord(strRecordId);
  if( jsx3.util.strEmpty(ObjRecord) ) return;

  if( ObjRecord.mediaExportFileQueueState != "-10" ) return;//-10 is an error
  var strSpyHTML = '<div id="outerDiv" style="margin: -4px; background-color: #ffffcc;">';
  strSpyHTML = strSpyHTML + '<table width="200" hight="200" border="0">';
  strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">' + ObjRecord.mediaExportFileErrorCode + '</td></tr>';
  strSpyHTML = strSpyHTML + '<tr><td style="font-family:Arial;font-size:10px;">' + ObjRecord.mediaExportFileErrorDescription + '</td></tr></table></div>';
  return strSpyHTML;
}