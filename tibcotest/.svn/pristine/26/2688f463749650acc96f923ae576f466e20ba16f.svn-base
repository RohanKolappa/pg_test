// Function to Create Report Block .
function cmpBlkReports_Create() { 
  topic_Set(tApp, "ReportDelete", "DISABLE");
  topic_Set(tApp, "ReportGetDataState", "NONE");
  var cmpBtnDeleteReport = new CmpImageButton("btnDeleteReport", null,
                                              {action:"FNCCALL",
                                               context:this,
                                               fnc:function() {
                                                  var selRecordNodes = cmpMtxReportList.BlkJSX.getSelectedNodes();
                                                  var agree = confirm('Are you sure you want to delete selected Reports?');
                                                  cmpMtxReportList.deleteRequestCounter = 0;
                                                  if(agree) {
                                                    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                      ++cmpMtxReportList.deleteRequestCounter;
                                                      var selRecordNode = it.next();
                                                      var SelectedAssetNID = selRecordNode.getAttribute("jsxid");
                                                      cmpBlkReportsDelete(SelectedAssetNID);
                                                    }
                                                  }
                                               }
                                              });
                                              
  var cmpBlkDeleteReport = new CmpContainer("blkDeleteReport",
                                            [{ cmp: cmpBtnDeleteReport,
                                                sel:"topic_Get(tApp, 'ReportDelete') == 'ENABLE'"}],
                                            [{ action: "APPEVENT", eventvar: "ReportDelete", eventval: null }],
                                            null);
                                   
  var cmpBtnAddReport = new CmpButton("btnAddReport",
                                      null, 
                                      {action:"FNCCALL", 
                                       context:this,
                                       fnc:function() {
                                         dlgEditReportInfo_Create();
                                       }
                                      });

  var cmpMtxReportList = new CmpCustomPaginationList( "mtxReportList",
                                                      [{ action: "APPEVENT", eventvar: "ReportGetData", eventval:null}],
                                                      {persist: { col: "jsxid", initval: false },
                                                       action: "APPEVENT",eventvar: "ReportSelected", eventval: "jsxid"},
                                                      null,
                                                      null                                                     
                                                    );
  
  cmpMtxReportList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                      CountToFetch: "ObjectList_CountToFetch",
                                      SortOrder: "ObjectList_SortOrder",
                                      ObjectCount: "ObjectCount",
                                      SortByXpath: "ObjectList_SortBy"
                                    };

  cmpMtxReportList.allowMultiSelection = true;
  cmpMtxReportList.deleteRequestCounter = 0;

  cmpMtxReportList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "ReportDataUpdated", eventval: "LOADED" };
  topic_Set(tApp, "ReportSelectFormat", "EXTENDED");
  
  cmpMtxReportList.cmpReq = new cmpReq_GetSessionAPIPaginationList( tApp, "GetReport",
                                                                    [{VarName: "ObjectList_SelectFormat", VarRef: "ReportSelectFormat" }],
                                                                     {action: "FNCCALL", context: cmpMtxReportList,
                                                                     fnc: function(respCode, respVal) {
                                                                            if( respCode == "SUCCESS" && respVal != null ) {
                                                                                this.onResponse(respCode, respVal);
                                                                            }
                                                                            else {
                                                                              log.info("GetReport Request failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetReport",respCode, respVal);
                                                                            }
                                                                          }
                                                                     }
                                                                  );
  cmpMtxReportList.BlkJSX.setXMLTransformers("xsl/ReportList.xsl");
  cmpMtxReportList.MenuEvent = {action:"APPEVENT", eventvar:"ReportMenuSelected", eventval:"jsxid"};
  cmpMtxReportList.cmpItemByNIDReqFnc = function () {
                                          var strWhereNID = cmpMtxReportList.WhereNIDArr.join();
                                          topic_Set(tApp, "UpdateForReportNID", strWhereNID);
                                          return new cmpReq_GetSessionAPIPaginationList(tApp, "GetReport",
                                                                                        [{ VarName: "ObjectList_WhereNID", VarRef: "UpdateForReportNID" },
                                                                                         { VarName: "ObjectList_SelectFormat", VarRef: "ReportSelectFormat" }
                                                                                        ],
                                                                                        { action: "FNCCALL", context: cmpMtxReportList,
                                                                                          fnc: function (respCode, respVal) {
                                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                              this.onResponseUpdateRecord(respCode, respVal);
                                                                                            }
                                                                                            else {
                                                                                              log.info("GetReport Request failed. Error Code :" + respCode);
                                                                                              this.onResponseErrorHandle("GetReport",respCode, respVal);
                                                                                            }
                                                                                          }
                                                                                        }, "ItemByNID"
                                                                                      );
                                        }

  var cmpReportNavControls = new CmpPageNavButtonsContainer(["blkReports","blkReportFooter"],
                                                            [["blkReports", "btnPageFirst"],
                                                             ["blkReports", "btnPagePrev"],
                                                             ["blkReports", "btnPageNext"],
                                                             ["blkReports", "btnPageLast"],
                                                             ["blkReports", "lblRecordInfo"],
                                                             ["blkReports", "txtCurrentRecordNumber"],
                                                             ["blkReports", "btnPageRefresh"],
                                                             ["blkReports", "btnPageRefreshSpinner"],
                                                             ["blkReports", "btnPageRefreshNotify"]
                                                            ],
                                                            cmpMtxReportList,
                                                            [{action:"TOPICEVENT", topic:tApp, eventvar:"ReportDataUpdated", eventval:null}],
                                                            null
                                                           ); 
                                                           
  //Code to Enable Right Click Options  on the report                                                     
  var cmpMenuReportList = new CmpMenu("menuReportList",
                                      [{ObjectType:"Audit", 
                                        MenuListArr:["Edit...", "Download Report"]},
                                       {ObjectType:"Database Configuration", 
                                        MenuListArr:["Edit...", "Download Report"]}                                       
                                      ],
                                      null, null, null,
                                      [{action:"APPEVENT", eventvar:"ReportMenuTypeSelected", eventval:null}],
                                      {action:"APPEVENT", eventvar:"ReportMenuActionSelected", eventval:"jsxtext"}
                                    );

  eventApp.subscribe("ReportSelected", null, this,
                      function() {
                         //Enable-Disable Delete Button
                        var selRecordIds = cmpMtxReportList.BlkJSX.getSelectedIds();
                        if(selRecordIds.length > 0){
                          topic_Publish(tApp, "ReportDelete", "ENABLE");
                        }
                        else{
                          topic_Publish(tApp, "ReportDelete", "DISABLE");
                        }
                      }
                    );
 
  eventApp.subscribe("ReportMenuSelected", null, this,
                      function() {
                        var ReportMenuSelected = topic_Get(tApp, 'ReportMenuSelected');
                        var cmpMtxReportListRecord = cmpMtxReportList.BlkJSX.getRecordNode(ReportMenuSelected);
                        var ReportType = cmpMtxReportListRecord.getAttribute("jsxtype");
                        topic_Publish(tApp, "ReportMenuTypeSelected", ReportType);
                      }
                    );
                    
  eventApp.subscribe("ReportMenuActionSelected", null, this,
                      function() {
                        var ReportMenuActionSelected = topic_Get(tApp, 'ReportMenuActionSelected');
                        var SelectedReportNID = topic_Get(tApp, "ReportSelected");
                        var cmpMtxReportListRecord = cmpMtxReportList.BlkJSX.getRecordNode(SelectedReportNID);
                        var SelectedReportName = cmpMtxReportListRecord.getAttribute("jsxtext");
                        var SelectedReportFormat = cmpMtxReportListRecord.getAttribute("jsxformat");
                        if( ReportMenuActionSelected == "Edit..." ) {
                          dlgEditReportInfo_Create(SelectedReportNID);
                        }
                        else if( ReportMenuActionSelected == "Download Report" ) {
                          dlgDownloadReport_Create(SelectedReportNID, SelectedReportFormat, SelectedReportName);
                        }
                      }
                    );
  // Right click code Ends
  
  
  eventApp.subscribe("SystemAdminMenuSelection", "REPORTS", this,
                      function() {
                        var getDataState = topic_Get(tApp, "ReportGetDataState");
                        if(getDataState == "NONE") {
                          topic_Set(tApp, "ReportGetDataState", (newServerDate()).getTime() );
                          topic_Publish(tApp, "ReportGetData", (newServerDate()).getTime());
                        }
                      }
                    );
  
  eventApp.subscribe("DBUpdate", null, this,
                       function(EventVar, EventVal) {
                         if( jsx3.util.strEmpty(EventVal) ) return;
                         if( EventVal.objType == "Report" ) {
                           cmpMtxReportList.handleNotification(EventVal);
                         }
                       });
 
  var cmpBlkReports = new CmpContainer( "blkReports", 
                                        [{cmp:cmpBlkDeleteReport},                                         
                                         {cmp:cmpBtnAddReport},
                                         {cmp:cmpMtxReportList},
                                         {cmp:cmpReportNavControls},
                                         {cmp:cmpMenuReportList}
                                        ],
                                        null,
                                        null );
                                        
  return cmpBlkReports;
  
  // fucntion to Delete Selected Report List
  function cmpBlkReportsDelete(ReportNID) {
    var reqDeleteReport = new ReqSessionGetRequest("DeleteReport",
                                                    { action: "FNCCALL", context: null,
                                                      fnc: function(respCode, respVal) {
                                                        if (respCode == "SUCCESS" && respVal != null) { 
                                                          var reportNode = respVal;
                                                          --cmpMtxReportList.deleteRequestCounter < 0 ? 0 : cmpMtxReportList.deleteRequestCounter;
                                                          if (cmpMtxReportList.deleteRequestCounter == 0) {
                                                            topic_Publish(tApp, "ReportGetData", (newServerDate()).getTime());
                                                          }
                                                        }
                                                        else {
                                                          --cmpMtxReportList.deleteRequestCounter < 0 ? 0 : cmpMtxReportList.deleteRequestCounter;
                                                          log.error("Delete Report request failed for Report NID: " + reportNID);
                                                        }
                                                      }
                                                    }
                                                  );
    reqDeleteReport.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = ReportNID;
    reqDeleteReport.reqParams = reqParams;
    // send the request
    reqDeleteReport.sendRequest(this);
  }// end cmpBlkReportsDelete
  
}// end cmpBlkReports_Create