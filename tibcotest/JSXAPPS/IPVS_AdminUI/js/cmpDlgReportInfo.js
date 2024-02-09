function dlgEditReportInfo_Create(ReportNID) {   
  // deal with defaults for add
  if( ReportNID == null ) ReportNID = "DEFAULT_REPORT_NID";

  // load view - dialog component file
  var DlgFile = "components/admin/dlgCreateNewReport.xml";
  //var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  
  var dlgText = "Create New Report";
  if(ReportNID != "DEFAULT_REPORT_NID"){
     dlgText = "Edit Report";
  }

  var windowBarBlkJSX = getJsxByName([BlkJSX, "windowBar"]);
  windowBarBlkJSX.setText(dlgText, true);
  closeDialogIfExists(dlgText);

  function dlgEditReport_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditReport.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }
  
  topic_Set(tDlg, "ReportSetupStage", "BASIC");
  topic_Set(tDlg, "RowSelectionMode", "BASIC");
  topic_Set(tDlg, "ColumnSelectionMode", "BASIC");
  topic_Set(tDlg, "PropertiesBlock", "HIDE");
  
  var cmpTxtReportName = new Component([BlkJSX,"txtReportName"]);
  var cmpTxtReportDesc = new Component([BlkJSX,"txtReportDesc"]);
  var cmpSelReportType = new CmpStaticList([BlkJSX,"selReportType"],
                                           "xml/selections/ReportType.xml",
                                           null,
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              var type = cmpSelReportType.BlkJSX.getValue();
                                              topic_Publish(tDlg, "ReportType",type);
                                              cmpDlgEditReportSetPropertiesPermissions();
                                            }
                                           });
  var cmpChkExportReport = new CmpCheckBox([BlkJSX,"ckhExportReport"],null,
                                           {action:"FNCCALL", 
                                            context:this, 
                                            fnc:function() {
                                              topic_Publish(tDlg, "ExportReport",(newServerDate()).getTime());
                                            }
                                           }    
                                          );
                                          
  eventDlg.subscribe("ExportReport", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          if(cmpChkExportReport.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED){
                            cmpTxtReportExportURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED,true);
                            cmpTxtReportExportSchedule.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED,true);
                          }
                          else{
                            cmpTxtReportExportURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED,true);
                            cmpTxtReportExportSchedule.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED,true);
                          }
                        }
                      }
                    );
                    
  var cmpTxtReportExportURL = new Component([BlkJSX,"txtReportExportURL"]);
  var cmpTxtReportExportSchedule = new Component([BlkJSX,"txtReportExportSchedule"]);
  
  var cmpBlkReportSetup = new CmpContainer("blkReportSetup", 
                                            [{cmp:cmpTxtReportName},
                                             {cmp:cmpTxtReportDesc},
                                             {cmp:cmpSelReportType},
                                             {cmp:cmpChkExportReport},
                                             {cmp:cmpTxtReportExportURL},
                                             {cmp:cmpTxtReportExportSchedule}
                                            ],
                                            null,
                                            null );
                                            
  var cmpBtnRadioUserList = new CmpRadioButton([BlkJSX, "radioUserList"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function(){
                                                    clearSelections();
                                                    topic_Set(tDlg, "TagNameResponseObjectType", "Users");
                                                    topic_Publish(tDlg, "TagNameState", (newServerDate()).getTime());
                                                 }
                                               });
  
  var cmpBtnRadioClientList = new CmpRadioButton([BlkJSX, "radioClientList"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function(){
                                                     clearSelections();
                                                     topic_Set(tDlg, "TagNameResponseObjectType", "Device");
                                                     topic_Publish(tDlg, "TagNameState", (newServerDate()).getTime());
                                                   }
                                                 });
                                            
  var cmpTxtFilterByKeyword = new Component([BlkJSX,"txtFilterByKeyword"]); 
  
  
  var cmpMtxTagNames = new CmpCustomPaginationList([BlkJSX, "mtxTagNames"],
                                                   [{action:"TOPICEVENT", topic: tDlg, eventvar:"TagNameState", eventval:null}],
                                                     { persist: { col: "jsxtext", initval: false },
                                                         action: "TOPICEVENT", topic: tDlg, eventvar: "TagNameSelected", eventval: "jsxid"
                                                     },
                                                   null,
                                                   null,
                                                   "xsl/TagNameList.xsl"
                                                  );

  cmpMtxTagNames.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                   CountToFetch: "ObjectList_CountToFetch",
                                   SortOrder: "ObjectList_SortOrder",
                                   ObjectCount: "ObjectCount",
                                   SortByXpath: "ObjectList_SortBy",
                                   WhereObjectType: "ObjectList_WhereObjectType"
                                  };

  topic_Set(tDlg, "TagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "TagNameResponseObjectType", "Device");

  cmpMtxTagNames.cmpReq = new cmpReq_GetSessionAPIPaginationList( tDlg, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat"},
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "TagNameResponseObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxTagNames,
                                                                    fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                            this.onResponse(respCode, respVal);
                                                                            cmpDlgEditReportSetPropertiesPermissions();
                                                                        }
                                                                        else {
                                                                            log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                            this.onResponseErrorHandle("GetTags",respCode, respVal);
                                                                        }
                                                                      }
                                                                  }
                                                                );
                                                                
  var cmpMtxTagValues = new Component([BlkJSX,"mtxTagValues"]);
  
  var cmpMtxFilterTags = new Component([BlkJSX,"mtxFilterTags"]);

  eventDlg.subscribe("TagNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          var nameRecord = cmpMtxTagNames.BlkJSX.getValue();
                          var selNameRecord = cmpMtxTagNames.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          cmpMtxTagValues.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");
                          var strXMLValueCacheId = cmpMtxTagValues.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpMtxTagValues.BlkJSX.setSourceXML(objXML); 
                          cmpMtxTagValues.BlkJSX.repaint();
                        }
                      }
                    );
  
  var cmpBtnAddTags = new CmpButton([BlkJSX,"btnAddTags"],
                                    null, 
                                    {action:"FNCCALL", 
                                     context:null, 
                                     fnc:function() {
                                        var nameRecord = cmpMtxTagNames.BlkJSX.getValue();
                                        var selNameRecord =  cmpMtxTagNames.BlkJSX.getRecord(nameRecord);
                                        var groupTagName = "";
                                        var groupTagValue = "";
                                        if( selNameRecord == null) {
                                          groupTagName = nameRecord
                                        } else {
                                          groupTagName = selNameRecord.jsxtext;
                                        }
                                        if( groupTagName == "" || groupTagName == null ) {
                                          alert("Please enter/select tag name");
                                          return false;                                          
                                        }
                                        var valueRecord = cmpMtxTagValues.BlkJSX.getValue();
                                        var selValueRecord = cmpMtxTagValues.BlkJSX.getRecord(valueRecord);
                                        if( selValueRecord == null) {
                                          groupTagValue = valueRecord;
                                        } else {
                                          groupTagValue = selValueRecord.jsxtext;
                                        } 
                                        if( groupTagValue == "" || groupTagValue == null) {
                                          alert("Please enter/select tag value");
                                          return false;
                                        }
                                        var o = new Object;
                                        o.jsxid = groupTagName + groupTagValue;
                                        o.jsxName = groupTagName;
                                        o.jsxValue = groupTagValue;
                                        o.jsxtext = groupTagName + " = " + groupTagValue;
                                        cmpMtxFilterTags.BlkJSX.insertRecord(o, null, false);
                                        cmpMtxFilterTags.BlkJSX.repaintData();
                                     }
                                    });
  
  var cmpBtnAdvancedFilter = new CmpButton([BlkJSX,"btnAdvancedFilter"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              var advancedExpr = cmpDlgEditReportGetAdvancedSelectedTags();
                                              cmpTxtAdvancedFilter.BlkJSX.setValue(advancedExpr);
                                              topic_Publish(tDlg, "RowSelectionMode","ADVANCED"); 
                                            }
                                           });      
                                              
  var cmpBlkReportFilterTags = new CmpContainer([BlkJSX,"blkReportFilterTags"], 
                                                [{cmp:cmpMtxTagNames},
                                                 {cmp:cmpMtxTagValues},
                                                 {cmp:cmpBtnAddTags},
                                                 {cmp:cmpMtxFilterTags},
                                                 {cmp:cmpBtnAdvancedFilter}
                                                ],
                                                null,
                                                null );      
                                            
  var cmpTxtAdvancedFilter = new Component([BlkJSX,"txtAdvancedFilter"]);
  
     
  var cmpBtnBasicFilter = new CmpButton([BlkJSX,"btnBasicFilter"],
                                        null, 
                                        {action:"FNCCALL", 
                                         context:null, 
                                         fnc:function() {
                                            cmpTxtAdvancedFilter.BlkJSX.setValue("");
                                            topic_Publish(tDlg, "RowSelectionMode","BASIC");       
                                         }
                                        });  
                                                                                  
                                              
  var cmpBlkReportFilterAdvanced = new CmpContainer([BlkJSX,"blkReportFilterAdvanced"], 
                                                    [{cmp:cmpTxtAdvancedFilter},
                                                     {cmp:cmpBtnBasicFilter}
                                                    ],
                                                    null,
                                                    null );                                            
                                              
  
  var cmpBlkReportRows = new CmpContainer("blkReportRows", 
                                          [{cmp:cmpBtnRadioUserList},
                                           {cmp:cmpBtnRadioClientList},
                                           {cmp:cmpTxtFilterByKeyword},
                                           {cmp:cmpBlkReportFilterTags,
                                            sel:"topic_Get('" + tDlg + "', 'RowSelectionMode') == 'BASIC'"},
                                           {cmp:cmpBlkReportFilterAdvanced,
                                            sel:"topic_Get('" + tDlg + "', 'RowSelectionMode') == 'ADVANCED'"}
                                          ],
                                          [{action:"TOPICEVENT", topic: tDlg, eventvar:"RowSelectionMode", eventval:null}],
                                          null );
                                            


  var cmpBtnRadioXMLFortmat = new CmpRadioButton([BlkJSX, "radioXMLFortmat"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function()
                                                   {
                                                   }
                                                 });
                                                 
  var cmpBtnRadioCSVFortmat = new CmpRadioButton([BlkJSX, "radioCSVFortmat"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function()
                                                   {
                                                   }
                                                 });
                                                 
  var cmpBlkReportFormat = new CmpContainer([BlkJSX,"blkReportFormat"], 
                                            [{cmp:cmpBtnRadioXMLFortmat},
                                             {cmp:cmpBtnRadioCSVFortmat}
                                            ],
                                            null,
                                            null );                                            
                                                                                            
                                                 
  var cmpMtxProperties = new CmpStaticList([BlkJSX,"mtxProperties"],
                                           "xml/selections/AuditReportProperties.xml",
                                           null,
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              
                                            }
                                           });                                               
                                                 
  var cmpBtnAddProperties = new CmpButton([BlkJSX,"btnAddProperties"],
                                          null, 
                                          {action:"FNCCALL", 
                                           context:null, 
                                           fnc:function() {
                                              var propertyRecord = cmpMtxProperties.BlkJSX.getValue();
                                              var selPropertyRecord = cmpMtxProperties.BlkJSX.getRecord(propertyRecord);
                                              if( selPropertyRecord == "" || selPropertyRecord == null) {
                                                alert("Please enter/select Property value");
                                                return false;
                                              }
                                              var o = new Object;
                                              o.jsxid = jsx3.xml.CDF.getKey();
                                              o.jsxName = selPropertyRecord.jsxName;
                                              o.jsxtext = selPropertyRecord.jsxtext;
                                              cmpMtxSelectedProperties.BlkJSX.insertRecord(o, null, false);
                                              cmpMtxSelectedProperties.BlkJSX.repaintData();    
                                           }
                                          });         
                                                                                      
  var cmpMtxSelectedProperties = new CmpStaticList([BlkJSX,"mtxSelectedProperties"],
                                           "xml/selections/AuditReportProperties.xml",
                                           null,
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              
                                            }
                                           });     
  
  var cmpBtnAdvancedColumn = new CmpButton([BlkJSX,"btnAdvancedColumn"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              var XSLString = cmpDlgEditReportGetXSLString();
                                              cmpTxtAdvancedColumn.BlkJSX.setValue(XSLString);
                                              topic_Publish(tDlg, "ColumnSelectionMode","ADVANCED"); 
                                            }
                                           });                   
  
  var cmpBlkReportProperties = new CmpContainer([BlkJSX,"blkReportProperties"], 
                                                [{cmp:cmpMtxProperties},
                                                 {cmp:cmpBtnAddProperties},
                                                 {cmp:cmpMtxSelectedProperties}
                                                ],
                                                null,
                                                null );
                                                 
  var cmpMtxPermissionName = new CmpStaticList([BlkJSX,"mtxPermissionName"],
                                               "xml/selections/Permissions.xml",
                                               null,
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  var nameRecord = cmpMtxPermissionName.BlkJSX.getValue();
                                                  if(jsx3.util.strEmpty(nameRecord))return;
                                                  var selNameRecord = cmpMtxPermissionName.BlkJSX.getRecordNode(nameRecord);
                                                  cmpMtxPermissionValues.BlkJSX.setXMLTransformers("xsl/PermissionValueList.xsl");
                                                  var strXMLValueCacheId = cmpMtxPermissionValues.BlkJSX.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                                                  objXML.loadXML(selNameRecord.toString());
                                                  cmpMtxPermissionValues.BlkJSX.setSourceXML(objXML); 
                                                  cmpMtxPermissionValues.BlkJSX.repaint();
                                                }
                                               }); 
  
  
  var cmpMtxPermissionValues = new Component([BlkJSX,"mtxPermissionValues"]);
                                          
  var cmpBtnAddPermissions = new CmpButton([BlkJSX,"btnAddPermissions"],
                                            null, 
                                            {action:"FNCCALL", 
                                             context:null, 
                                             fnc:function() {
                                                var nameRecord = cmpMtxPermissionName.BlkJSX.getValue();
                                                var selNameRecord =  cmpMtxPermissionName.BlkJSX.getRecord(nameRecord);
                                                var permissionName = "";
                                                var permissionValue = "";
                                                if( selNameRecord == null) {
                                                  permissionName = nameRecord
                                                } else {
                                                  permissionName = selNameRecord.jsxtext;
                                                }
                                                if( permissionName == "" || permissionName == null ) {
                                                  alert("Please enter/select permission name");
                                                  return false;                                          
                                                }
                                                var valueRecord = cmpMtxPermissionValues.BlkJSX.getValue();
                                                var selValueRecord = cmpMtxPermissionValues.BlkJSX.getRecord(valueRecord);
                                                if( selValueRecord == null) {
                                                  permissionValue = valueRecord;
                                                } else {
                                                  permissionValue = selValueRecord.jsxtext;
                                                } 
                                                if( permissionValue == "" || permissionValue == null) {
                                                  alert("Please enter/select tag value");
                                                  return false;
                                                }
                                                var o = new Object;
                                                o.jsxid = jsx3.xml.CDF.getKey();
                                                o.jsxName = permissionName;
                                                o.jsxValue = valueRecord;
                                                o.jsxtext = permissionName + " = " + permissionValue;
                                                cmpMtxSelectedPermissions.BlkJSX.insertRecord(o, null, false);
                                                cmpMtxSelectedPermissions.BlkJSX.repaintData();
                                             }
                                            });
  
  var cmpMtxSelectedPermissions = new Component([BlkJSX,"mtxSelectedPermissions"]);   
  
  var cmpBlkReportPermissions = new CmpContainer( [BlkJSX,"blkReportPermissions"], 
                                                  [{cmp:cmpMtxPermissionName},
                                                   {cmp:cmpMtxPermissionValues},
                                                   {cmp:cmpBtnAddPermissions},
                                                   {cmp:cmpMtxSelectedPermissions}
                                                  ],
                                                  null,
                                                  null );
  
  var cmpTxtAdvancedColumn = new Component([BlkJSX,"txtAdvancedColumn"]);
  
     
  var cmpBtnBasicColumn = new CmpButton([BlkJSX,"btnBasicColumn"],
                                        null, 
                                        {action:"FNCCALL", 
                                         context:null, 
                                         fnc:function() {
                                            cmpTxtAdvancedColumn.BlkJSX.setValue("");
                                            topic_Publish(tDlg, "ColumnSelectionMode","BASIC");     
                                         }
                                        });  
                                                                                  
                                              
  var cmpBlkReportColumnAdvanced = new CmpContainer([BlkJSX,"blkReportColumnAdvanced"], 
                                                    [{cmp:cmpTxtAdvancedColumn},
                                                     {cmp:cmpBtnBasicColumn}
                                                    ],
                                                    null,
                                                    null ); 
                                                    
                  
  var cmpBlkReportColumns = new CmpContainer( [BlkJSX,"blkReportColumns"], 
                                              [{cmp:cmpBlkReportFormat,
                                                sel:"topic_Get('" + tDlg + "', 'ColumnSelectionMode') == 'BASIC'"},
                                               {cmp:cmpBlkReportProperties,
                                                sel:"topic_Get('" + tDlg + "', 'PropertiesBlock') == 'SHOW'"},
                                               {cmp:cmpBtnAdvancedColumn,
                                                sel:"topic_Get('" + tDlg + "', 'ColumnSelectionMode') == 'BASIC'"},                                                
                                               {cmp:cmpBlkReportPermissions,
                                                sel:"topic_Get('" + tDlg + "', 'ColumnSelectionMode') == 'BASIC'&& " +
                                                    "topic_Get('" + tDlg + "', 'ReportType') == 'Policy'"},
                                               {cmp:cmpBlkReportColumnAdvanced,
                                                sel:"topic_Get('" + tDlg + "', 'ColumnSelectionMode') == 'ADVANCED'"}
                                              ],
                                              [{action:"TOPICEVENT", topic: tDlg, eventvar:"ColumnSelectionMode", eventval:null}
//                                               {action:"TOPICEVENT", topic: tDlg, eventvar:"ReportType", eventval:null}
                                               ],
                                              null );
                                              
  var cmpBtnPrevious = new CmpImageButton([BlkJSX, "btnPrevious"], null,
                                          { action: "FNCCALL", context: this,
                                            fnc: function() {
                                              var setup = topic_Get(tDlg, "ReportSetupStage");
                                              if(setup == "ROWS"){
                                                topic_Publish(tDlg, "ReportSetupStage","BASIC");
                                              }
                                              else if(setup == "COLUMNS"){
                                                var reportType = topic_Get(tDlg, "ReportType");
                                                if(reportType == "Audit"){
                                                  topic_Publish(tDlg, "ReportSetupStage","BASIC");
                                                }
                                                else{
                                                  topic_Publish(tDlg, "ReportSetupStage","ROWS");
                                                }
                                              }
                                            }
                                          });                                            
                                              
  var cmpBtnNext = new CmpImageButton([BlkJSX, "btnNext"], null,
                                      { action: "FNCCALL", context: this,
                                        fnc: function() {
                                          var reportName = cmpTxtReportName.BlkJSX.getValue();
                                          if(jsx3.util.strEmpty(reportName)) {
                                            alert("Enter Valid Report Name");
                                            cmpTxtReportName.BlkJSX.focus();
                                            return false;
                                          }
                                          var reportType = topic_Get(tDlg, "ReportType");
                                          if(jsx3.util.strEmpty(reportType)) {
                                            alert("Select Report Type");
                                            return false;
                                          }
                                          if( cmpDlgEditReportExportValidate() != true ) return;
                                          var setup = topic_Get(tDlg, "ReportSetupStage");
                                          if(setup == "BASIC"){
                                            if(reportType == "Audit"){
                                              topic_Publish(tDlg, "ReportSetupStage","COLUMNS")
                                            }
                                            else{
                                              topic_Publish(tDlg, "ReportSetupStage","ROWS");
                                            }
                                          }
                                          else if(setup == "ROWS"){
                                            if(reportType == "Policy"){
                                              if(cmpBtnRadioUserList.BlkJSX.getSelected() != jsx3.gui.RadioButton.SELECTED && cmpBtnRadioClientList.BlkJSX.getSelected() != jsx3.gui.RadioButton.SELECTED){
                                                alert("Select a Report Target");
                                                return;
                                              }
                                            }
                                            topic_Publish(tDlg, "ReportSetupStage","COLUMNS");
                                          }
                                        }
                                      });                                            
                                              
  var cmpBtnSave = new CmpImageButton([BlkJSX, "btnSave"], null,
                                      { action: "FNCCALL", context: this,
                                        fnc: function() {
                                          cmpDlgEditReportSaveReport(ReportNID);
                                        }
                                      });   
                                                     
  var cmpBtnCancel = new CmpImageButton([BlkJSX, "btnCancel"], null,
                                        { action: "FNCCALL", context: this,
                                          fnc: function() {
                                            dlgEditReport_Destroy();
                                          }
                                        });        
  
   var cmpBtnClose = new CmpImageButton([BlkJSX, "btnDialogCloseX"], null,
                                        { action: "FNCCALL", context: this,
                                          fnc: function() {
                                            dlgEditReport_Destroy();
                                          }
                                        });

   var cmpBlkReportFooter = new CmpContainer([BlkJSX,"blkReportFooter"],
                                             [{cmp:cmpBtnPrevious,
                                               sel:"topic_Get('" + tDlg + "', 'ReportSetupStage') != 'BASIC'"},
                                              {cmp:cmpBtnNext,
                                               sel:"topic_Get('" + tDlg + "', 'ReportSetupStage') != 'COLUMNS'"},
                                              {cmp:cmpBtnSave,
                                               sel:"topic_Get('" + tDlg + "', 'ReportSetupStage') == 'COLUMNS'"},
                                              {cmp:cmpBtnCancel},
                                              {cmp:cmpBtnClose}
                                             ],
                                             [{action:"TOPICEVENT", topic: tDlg, eventvar:"ReportSetupStage", eventval:null}]);                                                                                                                          
                                              
  var cmpDlgEditReport = new CmpContainer([BlkJSX,"dlgCreateNewReport"],
                                          [ {cmp:cmpBlkReportSetup,
                                             sel:"topic_Get('" + tDlg + "', 'ReportSetupStage') == 'BASIC'"},
                                            {cmp:cmpBlkReportRows,
                                             sel:"topic_Get('" + tDlg + "', 'ReportSetupStage') == 'ROWS'"},
                                            {cmp:cmpBlkReportColumns,
                                             sel:"topic_Get('" + tDlg + "', 'ReportSetupStage') == 'COLUMNS'"},
                                            {cmp:cmpBlkReportFooter}
                                          ],
                                          [{action:"TOPICEVENT", topic: tDlg, eventvar:"ReportSetupStage", eventval:null}]); 
  cmpDlgEditReport.BlkJSX = BlkJSX;
  cmpDlgEditReport.ObjectNID = ReportNID;
  cmpDlgEditReport.getReportInfo = cmpDlgEditReportGetReport;
    // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getReportInfo(this.ObjectNID);
                 }
               ),
               null, cmpDlgEditReport );
               
               
  //To Get the Report details if the Dlg context is Edit            
  function cmpDlgEditReportGetReport(reportNID) { 
    if(reportNID == "DEFAULT_REPORT_NID")return;
    // If Edit Report then Get the report details and populate the Dialog 
    var reqGetReport = new ReqSessionGetRequest("GetReport",
                                                { action: "FNCCALL", context: null,
                                                  fnc: function(respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) { 
                                                      var reportNode = respVal.selectSingleNode(".//Report");
                                                      setDialogDataNode(tDlg, reportNode)
                                                      cmpDlgEditReportSetReport();
                                                    }
                                                    else {
                                                      log.error("Get Report request failed for Report NID: " + reportNID);
                                                    }
                                                  }
                                                }
                                               );
    reqGetReport.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = reportNID;
    reqParams["ObjectList_SelectFormat"] = "EXTENDED";
    reqGetReport.reqParams = reqParams;
    // send the request
    reqGetReport.sendRequest(this);
  }            

  //To Set the Report Contents to the dialog if the Dlg context is Edit
  function cmpDlgEditReportSetReport(){
    var currentReportNode = getDialogDataNode(tDlg, "Report");
    
    var reportName = currentReportNode.getAttribute("name");
    cmpTxtReportName.BlkJSX.setValue(reportName);
    cmpTxtReportName.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED,true);
        
    var reportDesc = currentReportNode.getAttribute("description");
    cmpTxtReportDesc.BlkJSX.setValue(reportDesc);
    
    var reportType = currentReportNode.getAttribute("type");
    cmpSelReportType.BlkJSX.setValue(reportType);
    cmpSelReportType.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED,true);
    topic_Publish(tDlg, "ReportType",reportType); 
    
    var reportEnableExport = currentReportNode.getAttribute("enableExport");
    if(reportEnableExport == "true"){
      cmpChkExportReport.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);         
    }
    else{
      cmpChkExportReport.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);      
    }
    topic_Publish(tDlg, "ExportReport",(newServerDate()).getTime());
    var reportExportSchedule = currentReportNode.getAttribute("exportSchedule");
    cmpTxtReportExportSchedule.BlkJSX.setValue(reportExportSchedule);     
    var reportExportURL = currentReportNode.getAttribute("exportURL");
    cmpTxtReportExportURL.BlkJSX.setValue(reportExportURL);
    
    if(reportType =="Policy"){
      var reportTarget = currentReportNode.selectSingleNode(".//Policy").getAttribute("whereTarget");
      if(reportTarget == "User"){
        cmpBtnRadioUserList.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
        topic_Set(tDlg, "TagNameResponseObjectType", "Users");                                                    
      }
      else{
        cmpBtnRadioClientList.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
        topic_Set(tDlg, "TagNameResponseObjectType", "Device");
      }
      topic_Publish(tDlg, "TagNameState", (newServerDate()).getTime());
      cmpDlgEditReportSetPropertiesPermissions();
      var reportKeywordFilter = currentReportNode.selectSingleNode(".//Policy").getAttribute("whereTextSubStr");
      cmpTxtFilterByKeyword.BlkJSX.setValue(reportKeywordFilter);
      var reportTagExpr = currentReportNode.selectSingleNode(".//Policy").getAttribute("whereTagExpr");
      var reportTagExprMode = currentReportNode.selectSingleNode(".//Policy").getAttribute("whereTagExprMode");
      cmpDlgEditReportSetSelectedTags(reportTagExpr, reportTagExprMode);
      var properties = currentReportNode.selectSingleNode(".//Policy").getAttribute("properties");
      var permissions = currentReportNode.selectSingleNode(".//Policy").getAttribute("permissions");
      var nodeXSL = currentReportNode.selectSingleNode(".//Policy/FormatXSL");
      var formatXSLMode = nodeXSL.getAttribute("mode");
      var XSLString = nodeXSL.getValue();
      if(formatXSLMode == "ADVANCED") {
        // XSL loaded into Advanced text box Code goes here
        cmpTxtAdvancedColumn.BlkJSX.setValue(XSLString);
        topic_Set(tDlg, "ColumnSelectionMode","ADVANCED");
      }
      else{
        cmpTxtAdvancedColumn.BlkJSX.setValue("");
        // no need to fill up the selected propeties it will be same as propeties
        //cmpDlgEditReportSetSelectedProperties(properties);
        cmpDlgEditReportSetSelectedPermissions(permissions);
        topic_Set(tDlg, "ColumnSelectionMode", "BASIC");
      }
    }
    else {
      cmpDlgEditReportSetPropertiesPermissions();
      var properties = currentReportNode.selectSingleNode(".//Audit").getAttribute("properties");
      var nodeXSL = currentReportNode.selectSingleNode(".//Audit/FormatXSL");
      var formatXSLMode = nodeXSL.getAttribute("mode");
      var XSLString = nodeXSL.getValue();
      if(formatXSLMode == "ADVANCED") {
        cmpTxtAdvancedColumn.BlkJSX.setValue(XSLString);
        topic_Set(tDlg, "ColumnSelectionMode","ADVANCED");
      }
      else{
        cmpTxtAdvancedColumn.BlkJSX.setValue("");
        // no need to fill up the selected propeties it will be same as propeties        
        //cmpDlgEditReportSetSelectedProperties(properties);
        topic_Set(tDlg, "ColumnSelectionMode", "BASIC");
      }
    }
    var reportFormat = currentReportNode.getAttribute("format");
    if(reportFormat == "XML"){
      cmpBtnRadioXMLFortmat.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    }
    else{
      cmpBtnRadioCSVFortmat.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
    }
  }// end cmpDlgEditReportSetReport


  //To Save the Report (Either AddReport or SetReport request is sent depending upon the Dlg context Add/Edit)
  function cmpDlgEditReportSaveReport(reportNID) { 
    var requestType = "";
    var reportName = "";
    var reportDesc = "";
    var reportType = "";
    var enableExport = false ;
    var exportURL  = "";
    var exportSchedule = "";
    var listOf = "";
    var keywordFilter = "";
    var tagExpr = "";
    var tagExprMode = "";
    var reportFormat = "";
    var policyProperties = "";
    var policyPermissions = "";
    var policyFormatXSL = "";
    var auditProperties ="";
    var auditFormatXSL = "";
    var formatXSLMode = "";
    
    if(reportNID == "DEFAULT_REPORT_NID"){
      requestType = "AddReport";
    }
    else{
      requestType = "SetReport";
    }
    reportName = cmpTxtReportName.BlkJSX.getValue();
    if(jsx3.util.strEmpty(reportName)) {
      alert("Enter Valid Report Name");
      return;
    }
    reportDesc = cmpTxtReportDesc.BlkJSX.getValue();
    
    reportType = cmpSelReportType.BlkJSX.getValue();
    if(jsx3.util.strEmpty(reportType)) {
      alert("Select Report Type");
      return;
    }

    if(cmpDlgEditReportExportValidate()){
      if(cmpChkExportReport.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED){
        enableExport = true;        
      }
      exportURL = cmpTxtReportExportURL.BlkJSX.getValue();
      exportSchedule = cmpTxtReportExportSchedule.BlkJSX.getValue();
    }
    else{
      return;
    }
    if(reportType == "Policy"){
      if(cmpBtnRadioUserList.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
        listOf = "User";
      }
      else if(cmpBtnRadioClientList.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
        listOf = "Resource";
      }
      else{
        alert("Select a Report Target");
        return;
      }
      keywordFilter = cmpTxtFilterByKeyword.BlkJSX.getValue();
      tagExprMode = topic_Get(tDlg, "RowSelectionMode");
      if(tagExprMode == "BASIC") {
        tagExpr = cmpDlgEditReportGetSelectedTags();
      }
      else{
        tagExpr = cmpTxtAdvancedFilter.BlkJSX.getValue();
      }
    }
    if(cmpBtnRadioXMLFortmat.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      reportFormat = "XML";
    }
    else if(cmpBtnRadioCSVFortmat.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      reportFormat = "CSV";
    }
    else{
      alert("Select a Report Format");
      return;
    }
    formatXSLMode = topic_Get(tDlg, "ColumnSelectionMode");
    if(reportType == "Policy"){
      //var mode = topic_Get(tDlg, "ColumnSelectionMode");
      if(formatXSLMode == "BASIC") {
         policyProperties = cmpDlgEditReportGetSelectedProperties();
         policyPermissions = cmpDlgEditReportGetSelectedPermissions();
      }
      policyFormatXSL = cmpTxtAdvancedColumn.BlkJSX.getValue();
      if(jsx3.util.strEmpty(policyFormatXSL)){
        policyFormatXSL = cmpDlgEditReportGetXSLString();
      }
    }
    else{
      //var mode = topic_Get(tDlg, "ColumnSelectionMode");
      if(formatXSLMode == "BASIC") {
        auditProperties = cmpDlgEditReportGetSelectedProperties();
      }
      auditFormatXSL = cmpTxtAdvancedColumn.BlkJSX.getValue();
      if(jsx3.util.strEmpty(auditFormatXSL)){
        auditFormatXSL = cmpDlgEditReportGetXSLString();
      }
    }
    
     var reqSaveReport = new ReqSessionGetRequest(requestType,
                                              { action: "FNCCALL", context: null,
                                                fnc: function(respCode, respVal) {
                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                    topic_Publish(tApp, "ReportGetData", (newServerDate()).getTime());
                                                    dlgEditReport_Destroy();
                                                  }
                                                  else {
                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                     if(respCode == "5210"){
                                                       codeErrorMsg = "Validation error- special characters and spaces are not allowed in the report name."
                                                     }
                                                     var ErrorMsg = "Error: "+ requestType + " request failed \n";
                                                     ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                     ErrorMsg += "Error code: " + respCode;
                                                     alert(ErrorMsg);
                                                  }                                                    
                                                }
                                              }
                                             );
      reqSaveReport.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_ByNID"] = reportNID;
      reqParams["ObjectList_ByName"] = reportName;
      reqParams["ObjectList_name"] = reportName;
      reqParams["ObjectList_description"] = reportDesc;
      reqParams["ObjectList_Type"] = reportType;
      reqParams["ObjectList_format"] = reportFormat;
      reqParams["ObjectList_enableExport"] = enableExport;
      reqParams["ObjectList_exportURL"] = exportURL;
      reqParams["ObjectList_exportSchedule"] = exportSchedule;
      reqParams["ObjectList_WhereTarget"] = listOf;
      reqParams["ObjectList_WhereTextSubStr"] = keywordFilter;
      reqParams["ObjectList_WhereTagExpr"] = tagExpr;
      reqParams["ObjectList_WhereTagExprMode"] = tagExprMode;
      reqParams["ObjectList_policyProperties"] = policyProperties;
      reqParams["ObjectList_auditProperties"] = auditProperties;
      reqParams["ObjectList_policyPermissions"] = policyPermissions;
      reqParams["ObjectList_policyFormatXSL"] = policyFormatXSL;
      reqParams["ObjectList_auditFormatXSL"] = auditFormatXSL;
      reqParams["ObjectList_formatXSLMode"] = formatXSLMode;
      reqSaveReport.reqParams = reqParams;
      // send the request
      reqSaveReport.sendRequest(this);
  }// end cmpDlgEditReportSaveReport

  //To get the tag Expression if the user selects the Tags in Basic Mode(for filtering policy report Rows)
  function cmpDlgEditReportGetSelectedTags() { 
    var newTagExpression = cmpDlgEditReportGetMatrixSelections(cmpMtxFilterTags);;
    return newTagExpression;
  }// end cmpDlgEditReportGetSelectedTags
  
  //To set the  selected tags if the Dlg context is Edit   
  function cmpDlgEditReportSetSelectedTags(tagExpr, tagExprMode){
    var tagList = tagExpr.split(",");
    if(tagExprMode == "ADVANCED") {
      cmpTxtAdvancedFilter.BlkJSX.setValue(tagExpr);
      topic_Set(tDlg, "RowSelectionMode","ADVANCED");
      return;
    }
    topic_Set(tDlg, "RowSelectionMode","BASIC");
    cmpDlgEditReportSetMatrixSelections(cmpMtxFilterTags,tagList);
  }//end cmpDlgEditReportSetSelectedTags
  
  //To get the Advanced tag Expression if the user selects the Tags in Advanced Mode(for filtering policy report Rows)
  function cmpDlgEditReportGetAdvancedSelectedTags() { 
    var recordIds = cmpMtxFilterTags.BlkJSX.getRecordIds();
    var newExpression = "";
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxFilterTags.BlkJSX.getRecord(id);
      var Name = record.jsxName;
      var Value = record.jsxValue;
      if (i == 0) {
        newExpression = "(Name='" + Name + "' and Value='" + Value + "')";
      } else {
        newExpression = newExpression + " or " + "(Name='" + Name + "' and Value='" + Value + "')";
      }                             
    }
    return newExpression;
  }// end cmpDlgEditReportGetAdvancedSelectedTags
  
  //To get the Permissions if the user selects the permissions in Basic Mode(for filtering policy report Columns)
  function cmpDlgEditReportGetSelectedPermissions(){
    var newPermisions = cmpDlgEditReportGetMatrixSelections(cmpMtxSelectedPermissions);
    return newPermisions;
  }// end cmpDlgEditReportGetSelectedPermissions
  
  //To set the  selected permissions if the Dlg context is Edit   
  function cmpDlgEditReportSetSelectedPermissions(permissionExpr){
    var permissionList = permissionExpr.split(",");
    cmpDlgEditReportSetMatrixSelections(cmpMtxSelectedPermissions,permissionList);
  }//end cmpDlgEditReportSetSelectedPermissions
  
  //To get the Properties if the user selects the properties in Basic Mode(for filtering report Columns for both audit and policy reports)
  function cmpDlgEditReportGetSelectedProperties(){
    var recordIds = cmpMtxSelectedProperties.BlkJSX.getRecordIds();
    var newProperties = "";
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxSelectedProperties.BlkJSX.getRecord(id);
      var property = record.jsxtext;
      if (i == 0) {
        newProperties = property;
      } else {
        newProperties = newProperties + "," + property;
      }                             
    }
    return newProperties;
  }// end cmpDlgEditReportGetSelectedProperties
  
  //To set the  selected properties if the Dlg context is Edit   
  function cmpDlgEditReportSetSelectedProperties(propertiesExpr){
    
    var propertyList = propertiesExpr.split(",");
    for( var i=0 ; i < propertyList.length ; i++){
      var selectionName = propertyList[i];
      var selPropertyRecord = cmpMtxProperties.BlkJSX.getRecord(selectionName);
      var o = new Object;
      o.jsxid = jsx3.xml.CDF.getKey();
      if(!jsx3.util.strEmpty(selPropertyRecord)){
        o.jsxName = selPropertyRecord.jsxName;
        o.jsxtext = selPropertyRecord.jsxtext;
      }
      else{
        o.jsxName = selectionName;
        o.jsxtext = selectionName;
      }
      cmpMtxSelectedProperties.BlkJSX.insertRecord(o, null, false);
    }
    cmpMtxSelectedProperties.BlkJSX.repaint();
  }// cmpDlgEditReportSetSelectedProperties
  
  //To form the Name-Value Expression from Tags and Permissions matrix selections
  function cmpDlgEditReportGetMatrixSelections(mtx){
    var recordIds = mtx.BlkJSX.getRecordIds();
    var newExpression = "";
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = mtx.BlkJSX.getRecord(id);
      var Name = record.jsxName;
      var Value = record.jsxValue;
      if (i == 0) {
        newExpression = "Name='" + Name + "' and Value='" + Value + "'";
      } else {
        newExpression = newExpression + "," + "Name='" + Name + "' and Value='" + Value + "'";
      }                             
    }
    return newExpression;
  }//cmpDlgEditReportGetMatrixSelections
  
  // To parse the Name- Value Expression to set Tags and Permissions matrix selections
  function cmpDlgEditReportSetMatrixSelections(mtx,selectionList){
    for( var i=0 ; i < selectionList.length ; i++){
      var selection = selectionList[i];
      if( !jsx3.util.strEmpty(selection) ){
        var selectionName = selection.split(" and ")[0].split("='")[1].split("'")[0];
        var selectionValue = selection.split(" and ")[1].split("='")[1].split("'")[0];
        var o = new Object;
        o.jsxid = selectionName + selectionValue;
        o.jsxName = selectionName;
        o.jsxValue = selectionValue;
        o.jsxtext = selectionName + " = " + selectionValue;
        mtx.BlkJSX.insertRecord(o, null, false);
      }
    }  
    mtx.BlkJSX.repaintData();
  }//cmpDlgEditReportSetMatrixSelections
  
  //Set the Properties and Permission-Name Matrices for both Policy and Audit Report 
  function cmpDlgEditReportSetPropertiesPermissions(){
    var reportType = topic_Get(tDlg, "ReportType");
    if(reportType == "Policy"){
      //Set Properties Matrix for Policy report for either Users/Clients
      if(cmpBtnRadioUserList.BlkJSX.getSelected() || cmpBtnRadioClientList.BlkJSX.getSelected()){
        var tagNamesXML = cmpMtxTagNames.BlkJSX.getXML();
        var propertiesUriRes = cmpMtxProperties.BlkJSX.getUriResolver();
        if (jsx3.util.strEmpty(propertiesUriRes)) return;
        if(cmpBtnRadioUserList.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
          var propertiesXslUriRes = propertiesUriRes.resolveURI("xsl/UserProperties.xsl");
        }
        else if(cmpBtnRadioClientList.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
          var propertiesXslUriRes = propertiesUriRes.resolveURI("xsl/ClientProperties.xsl");
        }
        if (jsx3.util.strEmpty(propertiesXslUriRes)) return;
        var propertiesXSLFile = PNameSpace.loadInclude(propertiesXslUriRes, "_Update", "xsl");
        var propertiesXslDoc = jsx3.xml.XslDocument.wrap(propertiesXSLFile);
        var docSrcXML = propertiesXslDoc.transformToObject(tagNamesXML);
        cmpMtxProperties.BlkJSX.setSourceXML(docSrcXML);
        cmpMtxProperties.BlkJSX.repaint();
        
        cmpMtxSelectedProperties.BlkJSX.setSourceXML(docSrcXML);
        cmpMtxSelectedProperties.BlkJSX.repaint();
                
        //Set Permissions Matrix for Policy Report
        var xmlURL = "xml/selections/Permissions.xml";
        cmpMtxPermissionName.BlkJSX.setXMLURL(xmlURL);
        cmpMtxPermissionName.BlkJSX.repaint();       
      }
    }
    else{
      //Set Permissions Matrix for Audit Report
      var xmlURL = "xml/selections/AuditReportProperties.xml";
      cmpMtxProperties.BlkJSX.setXMLURL(xmlURL);
      cmpMtxProperties.BlkJSX.repaint();

      cmpMtxSelectedProperties.BlkJSX.setXMLURL(xmlURL);
      cmpMtxSelectedProperties.BlkJSX.repaint();     
            
    }
  }// end cmpDlgEditReportSetPropertiesPermissions
  
  //To Validate the Export Report fields
  function cmpDlgEditReportExportValidate() {
    if(cmpChkExportReport.BlkJSX.getChecked()== jsx3.gui.CheckBox.CHECKED) {
      var exportAt = cmpTxtReportExportSchedule.BlkJSX.getValue();
      if(jsx3.util.strEmpty(exportAt)) {
        alert("Enter Export Schedule");
        cmpTxtReportExportSchedule.BlkJSX.focus();
        return false;
      }
      else{
        var exportValues = exportAt.split(","); 
        for(var i = 0;i < exportValues.length;i++){
          var exportValue = exportValues[i];
          if( isNaN(exportValue) || exportValue < 0 || exportValue >= 24){
            alert("Export Schedule Timing should be from 0 to 23");
            cmpTxtReportExportSchedule.BlkJSX.focus();
            return false;
          }
        }
      }
      var ftpUrl = cmpTxtReportExportURL.BlkJSX.getValue();
      if(jsx3.util.strEmpty(ftpUrl)) {
        alert("Enter Valid FTP URL");
        cmpTxtReportExportURL.BlkJSX.focus();
        return false;
      }
    }
    return true;
  }// end cmpDlgEditReportExportValidate
  
  // function to XSL string for column selections
  function cmpDlgEditReportGetXSLString() {
    var reportType = cmpSelReportType.BlkJSX.getValue();
    var reportFormat = "";
    if(cmpBtnRadioXMLFortmat.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      reportFormat = "XML";
    }
    else if(cmpBtnRadioCSVFortmat.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
      reportFormat = "CSV";
    }
    var XSLString ="";
    if(reportType == "Audit"){
      XSLString = cmpDlgEditReportGetAuditXSLString(reportFormat);
    }
    else{
      if(cmpBtnRadioUserList.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
        var reportTarget ="User";
      }
      else if(cmpBtnRadioClientList.BlkJSX.getSelected() == jsx3.gui.RadioButton.SELECTED){
        var reportTarget ="Resource";
      }
      XSLString = cmpDlgEditReportGetPolicyXSLString(reportFormat,reportTarget);
    }  
    return XSLString;
  }// end cmpDlgEditReportGetXSLString
  
  //function to get XSL String for Audit Reports
  function cmpDlgEditReportGetAuditXSLString(format){
    var XSLString =""
    if(format == "CSV"){
      XSLString = cmpDlgEditReportGetAuditXSLStringCSV();
    }
    else{
      XSLString = cmpDlgEditReportGetAuditXSLStringXML();
    } 
    return  XSLString;
  }// end cmpDlgEditReportGetAuditXSLString
  
  // funtion to get XSL string for audit properties for XML to selected CSV conversion (running report in middleware)
  function cmpDlgEditReportGetAuditXSLStringCSV() {
    var XSLString = "<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>"+"<xsl:output method='text' indent='no'/>" + "<xsl:template match='/'>";
    var columnHeaders = cmpDlgEditReportGetSelectedProperties();
    XSLString = XSLString + columnHeaders + '<xsl:text>&#xa;</xsl:text>';
    XSLString = XSLString +"<xsl:for-each select='//Log'>";
    var recordIds = cmpMtxSelectedProperties.BlkJSX.getRecordIds();
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxSelectedProperties.BlkJSX.getRecord(id);
      var Name = record.jsxName;
      if(Name == "Description" ){
        var firstString = "<xsl:choose><xsl:when test='contains(./Description/@text,\\\"ERROR\\\")'><xsl:value-of select='substring-before(./Description/@text, \\\"ERROR:\\\")' /><xsl:text>, </xsl:text><xsl:value-of select='substring-after(./Description/@text, \\\"ERROR:\\\")' /></xsl:when><xsl:otherwise> ";
        XSLString = XSLString + firstString + "<xsl:value-of select='./" +Name + "/@text'/></xsl:otherwise></xsl:choose>";    		
      }
      else {
        XSLString = XSLString + "<xsl:value-of select='@" +Name.toLowerCase() + "' />";        
      } 
      if(i < recordIds.length - 1){     
        XSLString = XSLString + '<xsl:text>, </xsl:text>'; 
      }
    }
    XSLString = XSLString + '<xsl:text>&#xa;</xsl:text>' + '</xsl:for-each><xsl:text> </xsl:text></xsl:template>' + '</xsl:stylesheet>';
    return XSLString;
  }// end cmpDlgEditReportGetAuditXSLStringCSV
  
  // funtion to get XSL string for audit properties for XML to selected XML conversion (running report in middleware)
  function cmpDlgEditReportGetAuditXSLStringXML() {
    var XSLString = "<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'><xsl:output method='xml' indent='yes' /><xsl:template match='/'><xsl:apply-templates/></xsl:template>";
    XSLString = XSLString +"<xsl:template match='node()|@*'><xsl:copy><xsl:apply-templates select='node()|@*'/></xsl:copy></xsl:template>";
    var objXML = cmpMtxSelectedProperties.BlkJSX.getXML();
    var recordIds = cmpMtxProperties.BlkJSX.getRecordIds();
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxProperties.BlkJSX.getRecord(id);
      var Name = record.jsxtext;
      var presentRecord = objXML.selectSingleNode(".//record[@jsxtext='"+Name+"']");
      if(jsx3.util.strEmpty(presentRecord)){
        if(Name == "Description" ){
          XSLString = XSLString + "<xsl:template match ='Log/" +Name + "' />";        
        }
        else {
          XSLString = XSLString + "<xsl:template match ='@" + Name.toLowerCase()+ "' />";
        }    
      }  
    }
    XSLString = XSLString +'</xsl:stylesheet>';
    return XSLString;
  }// end cmpDlgEditReportGetAuditXSLStringXML
  
  // function to Get XSL String for Policy Reports
  function cmpDlgEditReportGetPolicyXSLString(format,target) {
    var XSLString ="";
    if(format == "CSV"){
      XSLString = cmpDlgEditReportGetPolicyXSLStringCSV(target);
    }
    else{
      XSLString = cmpDlgEditReportGetPolicyXSLStringXML(target);
    } 
    return XSLString;
  }//cmpDlgEditReportGetPolicyXSLString
  
  // funtion to get XSL string for policy properties for XML to selected CSV conversion (running report in middleware)
  function cmpDlgEditReportGetPolicyXSLStringCSV(target){
    var XSLString = "<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'>"+"<xsl:output method='text' indent='no'/>" ;
    XSLString = XSLString + "<xsl:template match='" +target+ "PolicyList" + "'>" + "<xsl:for-each select='./" +target+ "'>";
    var recordIds = cmpMtxSelectedProperties.BlkJSX.getRecordIds();
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxSelectedProperties.BlkJSX.getRecord(id);
      var Text = record.jsxtext;
      var Name= record.jsxName;
      if(target == "User"){
        if( Text == 'Username' || Text == 'Email'|| Text == 'FullName' || Text == 'CreationDate' ||  Text == 'ModifiedDate' || Text =='Presence' || Text =='State'){
          XSLString = XSLString + "<xsl:value-of select='@"+Name+"' />";
        }
        else{
          XSLString = XSLString +"<xsl:for-each select='./Groups/Tag[@Name=\\\"" +Name + "\\\"]'>" + "<xsl:value-of select='@Value' />" + "<xsl:text> </xsl:text>" + "</xsl:for-each>";
        }
      }
      else{
        if( Text == 'OwnerUserJID' || Text == 'Presence'|| Text == 'State' || Text == 'Title' ||  Text == 'Description' || Text =='Type'){
          XSLString = XSLString + "<xsl:value-of select='@" +Name+ "' />";
        }
        else{
          XSLString = XSLString + "<xsl:for-each select='./Groups/Tag[@Name=\\\"" +Name + "\\\"]'>" + "<xsl:value-of select='@Value' />" + "<xsl:text> </xsl:text>" + "</xsl:for-each>";
        }
      }
  	  XSLString = XSLString + '<xsl:text>, </xsl:text>'; 
  	}  
  	XSLString = XSLString +  cmpDlgEditReportGetPermissionsXSLStringCSV();		
  	XSLString = XSLString + '<xsl:text>&#xa;</xsl:text>'+'</xsl:for-each></xsl:template></xsl:stylesheet>';
    return XSLString; 
  }// end cmpDlgEditReportGetPolicyXSLStringCSV
  
  // funtion to get XSL string for policy properties for XML to selected XML conversion (running report in middleware)
  // For XML generation using empty Template to Revome unselected fields
  function cmpDlgEditReportGetPolicyXSLStringXML(target){
    var XSLString = "<xsl:stylesheet version='1.0' xmlns:xsl='http://www.w3.org/1999/XSL/Transform'><xsl:output method='xml' indent='yes' /><xsl:template match='/'><xsl:apply-templates/></xsl:template>";
    XSLString = XSLString +"<xsl:template match='node()|@*'><xsl:copy><xsl:apply-templates select='node()|@*'/></xsl:copy></xsl:template>";
    var objXML = cmpMtxSelectedProperties.BlkJSX.getXML();
    var recordIds = cmpMtxProperties.BlkJSX.getRecordIds();
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxProperties.BlkJSX.getRecord(id);
      var Text = record.jsxtext;
      var Name= record.jsxName;
      var presentRecord = objXML.selectSingleNode(".//record[@jsxtext='"+Text+"']");
      if(jsx3.util.strEmpty(presentRecord)){
        if(target == "User"){
          if( Text == 'Username' || Text == 'Email'|| Text == 'FullName' || Text == 'CreationDate' ||  Text == 'ModifiedDate' || Text =='Presence' || Text =='State'){
            XSLString = XSLString + "<xsl:template match ='@" +Name+ "'/>";
          }
          else{
            XSLString = XSLString + "<xsl:template match ='" +target+ "/Groups/Tag[@Name=\\\"" +Name + "\\\"]'/>";
          }
        }
        else{
          if( Text == 'OwnerUserJID' || Text == 'Presence'|| Text == 'State' || Text == 'Title' ||  Text == 'Description' || Text =='Type'){
            XSLString = XSLString + "<xsl:template match ='@" +Name+ "'/>";
          }
          else{
            XSLString = XSLString + "<xsl:template match ='" +target+ "/Groups/Tag[@Name=\\\"" +Name + "\\\"]'/>";
          }
        }
      }
    }
    XSLString = XSLString + cmpDlgEditReportGePermissionsXSLStringXML(target);
    XSLString = XSLString + '</xsl:stylesheet>';   
    return XSLString;
  }// end cmpDlgEditReportGetPolicyXSLStringXML
  
  // funtion to get XSL string for policy permissions for XML to selected CSV conversion (running report in middleware)
  function cmpDlgEditReportGetPermissionsXSLStringCSV(){
    var XSLString ="";
    var recordIds = cmpMtxSelectedPermissions.BlkJSX.getRecordIds();
    for( var i=0 ; i < recordIds.length ; i++){
      var id = recordIds[i];
      var record = cmpMtxSelectedPermissions.BlkJSX.getRecord(id);
      var Name = record.jsxName;
      var Value = record.jsxValue;
      XSLString = XSLString +"<xsl:for-each select='./Permissions/" +Name+ "/Action[@name=\\\"" +Value + "\\\"]/Groups/Tag'>" + "<xsl:value-of select='@Name' /><xsl:text>=</xsl:text>" + "<xsl:value-of select='@Value' /><xsl:text> </xsl:text>"	+	"</xsl:for-each>";
      if(i < recordIds.length -1){
  	    XSLString = XSLString + '<xsl:text>, </xsl:text>'; 
  	  }
    }
    return XSLString ;
  }// end cmpDlgEditReportGetPermissionsXSLStringCSV
  
  // funtion to get XSL string for policy permissions for XML to selected XML conversion (running report in middleware)
  // For XML generation using empty Template to Revome unselected fields
  function cmpDlgEditReportGePermissionsXSLStringXML(target){
    var XSLString ="";
    var objXML = cmpMtxPermissionName.BlkJSX.getXML();
    var selectedXML = cmpMtxSelectedPermissions.BlkJSX.getXML();
    var recordIterator = objXML.selectNodeIterator(".//record");
    while(recordIterator.hasNext()){
      var record = recordIterator.next();
      var permissionName = record.getAttribute("jsxid");
      var innerRecordIterator = record.selectNodeIterator(".//record");
      while(innerRecordIterator.hasNext()){
        var innerRecord = innerRecordIterator.next();
        var permisionValue = innerRecord.getAttribute("jsxid");
        var presentRecord = selectedXML.selectSingleNode(".//record[@jsxName='"+permissionName+"'and @jsxValue='"+permisionValue+"']");
        if(jsx3.util.strEmpty(presentRecord)){
          XSLString = XSLString + "<xsl:template match ='" +target+ "/Permissions/" +permissionName+ "/Action[@name=\\\"" +permisionValue + "\\\"]'/>";
        }
      }
    }
    return XSLString ;
  }// end cmpDlgEditReportGePermissionsXSLStringXML 
   
  // On Selecting Report target clear off previous selections. 
  function clearSelections(){
    //cmpMtxTagNames.BlkJSX.deselectAllRecords();
    cmpMtxTagValues.BlkJSX.clearXmlData();
    cmpMtxTagValues.BlkJSX.repaintData();
    var objXML = cmpMtxTagNames.BlkJSX.getXML();
    var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
    while(recordIter.hasNext()){
      var recordNode = recordIter.next();
      var jsxid = recordNode.getAttribute("jsxid");
      topic_Publish(tDlg, "TagNameSelected",jsxid);
    }
    cmpMtxFilterTags.BlkJSX.clearXmlData();
    cmpMtxFilterTags.BlkJSX.repaintData();
    cmpTxtAdvancedFilter.BlkJSX.setValue("");
    cmpMtxProperties.BlkJSX.deselectAllRecords();
    cmpMtxSelectedProperties.BlkJSX.clearXmlData();
    cmpMtxSelectedProperties.BlkJSX.repaintData();
    cmpMtxPermissionName.BlkJSX.deselectAllRecords();
    cmpMtxPermissionValues.BlkJSX.clearXmlData();
    cmpMtxPermissionValues.BlkJSX.repaintData();
    cmpMtxSelectedPermissions.BlkJSX.clearXmlData();
    cmpMtxSelectedPermissions.BlkJSX.repaintData();
    cmpTxtAdvancedColumn.BlkJSX.setValue("");
  }
            
}// end dlgEditReportInfo_Create