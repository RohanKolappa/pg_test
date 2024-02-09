function cmpBlkMedia_Create() {
  var blkMediaBlkJSX = PNameSpace.getJSXByName("blkMedia");
  blkMediaBlkJSX.load("components/media/media.xml");
  
  topic_Set(tApp, "MediaSelected","YES"); // changes NO to Yes //disabled for the time being
  topic_Publish(tApp,"MediaDirAttachDetach","NONE");
  topic_Set(tApp, "MediaSDRGetDataState", "NONE");

  topic_Set(tApp, "MediaSDRSelected", "NONE");
  topic_Set(tApp, "PvrMediaSDRSelected", "NONE");
  topic_Set(tApp, "MediaSDRSelectedModifyPermission", "false");
  topic_Set(tApp, "MediaSDRSelectedDeletePermission", "false");
  topic_Set(tApp, "MediaSDRSelectedImportPermission", "false");   
  topic_Set(tApp, "MediaSDRSelectedPermissions", (newServerDate()).getTime());    
      
  topic_Set(tApp, "MediaDirWherePermissions", "MODIFY,DELETE,IMPORT");  
  topic_Set(tApp, "MediaDirWhereFilePermissions", "MODIFY,DELETE,EXPORT");
  topic_Set(tApp, "MediaDirWhereParentNID", "NONE");
  topic_Set(tApp, "MediaDirWhereTextSubStr", "");
  topic_Set(tApp, "MediaDirSelectFormat", "EXTENDED");
  //topic_Set(tApp, "MediaDirSelectFormat", "SNAPSHOTURL");
  topic_Set(tApp, "MediaDirSubscriptionID", "SUBSCRIPTION_DIR_" + jsx3.xml.CDF.getKey());
  topic_Set(tApp, "MediaDirSearchTextSubStrIn", "");
  topic_Set(tApp, "MediaDirWhereTagExpr", "");
  topic_Set(tApp, "MediaDirWhereNID", "");
  topic_Set(tApp, "ViewMediaFileType", "DATA");
  


  var cmpMtxmediaFile = null;
    
  var cmpBtnDeleteMediaSDR = new CmpImageButton("btnDeleteMediaSDR", null,
                                                { action:"FNCCALL", context:this,
                                                  fnc:function() {
                                                     var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                                                     var matchResult = selectedNID.search(/jsx/);

                                                     if(matchResult != -1) {
                                                       alert("Cannot delete root directory");
                                                       return false;
                                                     } else {                                                 
                                                       dlgMediaSDR_Clean(selectedNID);
                                                     }
                                                  }
                                                });

  var cmpMtxMediaSDRList = new CmpCustomPaginationList("mtxDirList",
                                                       [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRGetData", eventval:null}],
                                                       {persist:{col:"jsxid",initval:false},
                                                        action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRSelected", eventval:"jsxid"},
                                                       null,
                                                       null
                                                      );
  
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpMtxMediaSDRList.SortPathArr = {jsxDirName:"title",
                                    jsxStatus:"state"
                                   };

  cmpMtxMediaSDRList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                       CountToFetch: "ObjectList_CountToFetch",
                                       SortOrder: "ObjectList_SortOrder",
                                       ObjectCount: "ObjectCount",
                                       SortByXpath: "ObjectList_SortBy"
                                      };
                                                                          
  cmpMtxMediaSDRList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tApp, eventvar:"MediaStreamDirResourceDataUpdated", eventval:"LOADED"};
  cmpMtxMediaSDRList.BlkJSX.setXMLTransformers("xsl/MediaDirList.xsl");

  cmpMtxMediaSDRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetDir",
                                                                    [{ VarName: "ObjectList_WhereNID", VarRef: "MediaDirWhereNID" },
                                                                     { VarName: "ObjectList_WhereTextSubStr", VarRef: "MediaDirWhereTextSubStr" },
                                                                     { VarName: "ObjectList_SelectFormat", VarRef: "MediaDirSelectFormat" },
                                                                     { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "MediaDirSearchTextSubStrIn" },
                                                                     { VarName: "ObjectList_WhereTagExpr", VarRef: "MediaDirWhereTagExpr" },
                                                                     { VarName: "ObjectList_WherePermissions", VarRef: "MediaDirWherePermissions" },
                                                                     { VarName: "ObjectList_WhereFilePermissions", VarRef: "MediaDirWhereFilePermissions" },
                                                                     { VarName: "ObjectList_WhereParentDirNID", VarRef: "MediaDirWhereParentNID" },
                                                                     { VarName: "ObjectList_SubscriptionID", VarRef: "MediaDirSubscriptionID" }
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpMtxMediaSDRList,
                                                                      fnc: function (respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                          var mtxCDFXML = this.onResponse(respCode, respVal);
                                                                          var jsxId = "NONE";
                                                                          var readyRecord = mtxCDFXML.selectSingleNode(".//record[@jsxselected='1']");
                                                                          if (jsx3.util.strEmpty(readyRecord)) {
                                                                            readyRecord = mtxCDFXML.selectSingleNode(".//record[@jsxStateFlag='Ready']");
                                                                          }
                                                                          if (!jsx3.util.strEmpty(readyRecord)) {
                                                                            readyRecord.setAttribute("jsxselected", "1");
                                                                            jsxId = readyRecord.getAttribute("jsxid");
                                                                            var pvrjsxId = readyRecord.getAttribute("jsxPVRDirNID");
                                                                            this.selRecordID = jsxId;
                                                                            topic_Set(tApp, "PvrMediaSDRSelected", "pvrjsxId");
                                                                            this.BlkJSX.repaint();
                                                                          }
                                                                          //handleDirSelected();
                                                                          topic_Publish(tApp, "MediaSDRSelected", jsxId);
                                                                        }
                                                                        else {
                                                                          log.info("GetDirRequest failed. Error Code :" + respCode);
                                                                          try {
                                                                            this.onResponseErrorHandle("GetDir", respCode, respVal);
                                                                          }
                                                                          catch (e) {
                                                                          }

                                                                        }
                                                                      }
                                                                    }
                                                                  );  
  
  cmpMtxMediaSDRList.cmpItemByNIDReqFnc = function () {
                                            topic_Set(tApp, "DirNIDS", cmpMtxMediaSDRList.WhereNIDArr.join(",") );
                                            return new cmpReq_GetSessionAPIPaginationList(tApp, "GetDir",
                                                                    [{ VarName: "ObjectList_WhereNID", VarRef: "DirNIDS"},
                                                                     { VarName: "ObjectList_WhereTextSubStr", VarRef: "MediaDirWhereTextSubStr"},
                                                                     { VarName: "ObjectList_SelectFormat", VarRef: "MediaDirSelectFormat"},
                                                                     { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "MediaDirSearchTextSubStrIn"},
                                                                     { VarName: "ObjectList_WhereTagExpr", VarRef: "MediaDirWhereTagExpr"},
                                                                     { VarName: "ObjectList_WherePermissions", VarRef: "MediaDirWherePermissions"},
                                                                     { VarName: "ObjectList_WhereFilePermissions", VarRef: "MediaDirWhereFilePermissions"},
                                                                     { VarName: "ObjectList_WhereParentDirNID", VarRef: "MediaDirWhereParentNID"}
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpMtxMediaSDRList,
                                                                        fnc: function(respCode, respVal) {
                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.handleResponseForNotificationNIDs(respVal, "MediaDirList", null, null);
                                                                                handleDirSelected();
                                                                            }
                                                                            else {
                                                                               log.info("GetDirRequest failed. Error Code :" + respCode);
                                                                               try {
                                                                                 this.onResponseErrorHandle("GetDir",respCode, respVal);                                                                                  
                                                                               }
                                                                               catch(e){
                                                                               }
                                                                                
                                                                            }
                                                                        }
                                                                    }
                                                                  );                                                                                
                                          }  
                                             
  var cmpMtxMediaDirMount = new CmpBlock("mtxDirMount");
  var cmpMtxMediaDirTags = new CmpBlock("mtxDirTags");


//  //***Task 35701 remove ViewOrphan media ***// 
//  //*** Story 36630 remove ViewPVRFiles Media "--ViewPVRFiles..."  ***//
  var cmpMenuDirList = new CmpMenu("menuDirList",
                                    [/*{ObjectType:"Detached", 
                                      MenuListArr:["Register Media Directory..."] },*/
                                     {ObjectType:"ReadyPVREnabledWriteImportwithEdit",
                                       MenuListArr: ["Edit...", "--View Recycle Bin Files...", "--Import Media", "--Capacity Management..."]
                                     },
                                     {ObjectType:"ReadyPVREnabledWriteImport",
                                       MenuListArr: ["View Recycle Bin Files...", "--Import Media"]},
                                     {ObjectType:"ReadyPVREnabledWriteImportNoModify",
                                       MenuListArr: ["View Recycle Bin Files...", "--Import Media"]},
                                     { ObjectType: "ReadyPVREnabledWritewithEdit",
                                       MenuListArr: ["Edit...", "--View Recycle Bin Files...", "--Capacity Management..."]
                                     },   
                                     { ObjectType: "ReadyPVREnabledWrite",
                                       MenuListArr: ["View Recycle Bin Files..."]},   
                                     { ObjectType: "ReadyPVREnabledWriteNoModify",
                                       MenuListArr: ["View Recycle Bin Files..." ]},                                     
                                     {ObjectType:"ReadyPVREnabledReadwithEdit",
                                       MenuListArr: ["Edit...", "--Upgrade...", "--View Recycle Bin Files...", "--Capacity Management..."]
                                     },
                                     {ObjectType:"ReadyPVREnabledRead", 
                                       MenuListArr:["Upgrade...", "--View Recycle Bin Files..."] },
                                     {ObjectType:"ReadyPVREnabledReadNoModify", 
                                       MenuListArr:["Upgrade...", "--View Recycle Bin Files..." ] },
                                     {ObjectType:"ReadyPVRDisabledWritewithEdit",
                                       MenuListArr: ["Edit...", "--View Recycle Bin Files...", "--Import Media", "--Capacity Management..."]
                                     },
                                     {ObjectType:"ReadyPVRDisabledWrite", 
                                       MenuListArr:["View Recycle Bin Files...", "--Import Media"] },
                                     {ObjectType:"ReadyPVRDisabledWriteNoModify", 
                                       MenuListArr:["View Recycle Bin Files...","--Import Media"] },
                                     {ObjectType:"ReadyPVRDisabledReadwithEdit",
                                       MenuListArr: ["Edit...", "--Upgrade...", "--View Recycle Bin Files...", "--Capacity Management..."]
                                     },
                                     {ObjectType:"ReadyPVRDisabledRead", 
                                       MenuListArr:["Upgrade...", "--View Recycle Bin Files..."] },
                                     {ObjectType:"ReadyPVRDisabledReadNoModify", 
                                       MenuListArr:["Upgrade...", "--View Recycle Bin Files..."] },
                                     {ObjectType:"OfflinePVREnabledWrite", 
                                       MenuListArr:["View Recycle Bin Files..."] },
                                     {ObjectType:"OfflinePVREnabledRead", 
                                       //MenuListArr:["Upgrade...", "--View Recycle Bin Files..."] }, To fix bug#42525
                                       MenuListArr:["View Recycle Bin Files..."] },
                                     {ObjectType:"OfflinePVRDisabledWrite", 
                                       MenuListArr:["View Recycle Bin Files..."] },
                                     {ObjectType:"OfflinePVRDisabledRead", 
                                       MenuListArr:["View Recycle Bin Files..."] }
                                    ],
                                    null, null, null,
                                    [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRMenuSelected", eventval:null}],
                                    {action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRMenuActionSelected", eventval:"jsxtext"}
                                  );

  cmpMtxMediaSDRList.MenuEvent = {action:"FNCCALL", 
                                  context:this,
                                      fnc:function() {
                                        var pvrMode = topic_Get(tApp, "PVR");
                                        var dirWriteMode = topic_Get(tApp, 'DirWriteMode');
                                        var MediaDirStateFlag = topic_Get(tApp,"MediaDirStateFlag");
                                        if( MediaDirStateFlag == "Detached" ) {
                                          topic_Publish(tApp, "MediaSDRMenuSelected", "Detached"); // REGDIR
                                        }
                                        else if( MediaDirStateFlag == "Ready" ) {
                                          var mediaOrphan = topic_Get(tApp, 'MediaSDROrphan');
                                          if (topic_Get(tApp, 'MediaSDRSelectedImportPermission') == "true" && pvrMode == "ENABLED" && dirWriteMode == "ENABLED") {
                                            if(topic_Get(tApp, 'MediaSDRSelectedModifyPermission') == "true"){
                                              if(mediaOrphan == null || mediaOrphan == "NONE"){
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledWriteImportwithEdit"); 
                                              }
                                              else{
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledWriteImport"); // RF, OF, PF, UNREGDIR
                                              }
                                            }
                                            else{
                                              topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledWriteImportNoModify"); // RF, OF, PF
                                            }
                                          }
                                          else if (topic_Get(tApp, 'MediaSDRSelectedImportPermission') == "false" && pvrMode == "ENABLED" && dirWriteMode == "ENABLED") {
                                            if(topic_Get(tApp, 'MediaSDRSelectedModifyPermission') == "true"){
                                              if(mediaOrphan == null || mediaOrphan == "NONE"){
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledWritewithEdit"); 
                                              }
                                              else{
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledWrite"); // RF, OF, PF, UNREGDIR
                                              }
                                            }
                                            else{
                                              topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledWriteNoModify"); // RF, OF, PF
                                            }
                                          }                                          
                                          else if( pvrMode == "ENABLED" && dirWriteMode == "NONE" ) {
                                            if(topic_Get(tApp, 'MediaSDRSelectedModifyPermission') == "true"){
                                              if(mediaOrphan == null || mediaOrphan == "NONE"){
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledReadwithEdit");
                                              }
                                              else{
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledRead"); // RF, OF, PF, UPG, UNREGDIR
                                              }
                                            }
                                            else{
                                              topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVREnabledReadNoModify"); // RF, OF, PF, UPG
                                            }
                                          }
                                          else if( pvrMode == "DISABLED" && dirWriteMode == "ENABLED" ) {
                                            if(topic_Get(tApp, 'MediaSDRSelectedModifyPermission') == "true"){
                                              if(mediaOrphan == null || mediaOrphan == "NONE"){
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVRDisabledWritewithEdit"); 
                                              }
                                              else{
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVRDisabledWrite"); // RF, OF,  UNREGDIR
                                              }
                                            }
                                            else{
                                              topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVRDisabledWriteNoModify"); // RF, OF
                                            }
                                          }
                                          else if( pvrMode == "DISABLED" && dirWriteMode == "NONE" ) {
                                            if(topic_Get(tApp, 'MediaSDRSelectedModifyPermission') == "true"){
                                              if(mediaOrphan == null || mediaOrphan == "NONE"){
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVRDisabledReadwithEdit"); 
                                              }
                                              else{
                                                topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVRDisabledRead"); // RF, OF, UPG, UNREGDIR
                                              }
                                            }
                                            else{
                                               topic_Publish(tApp, "MediaSDRMenuSelected", "ReadyPVRDisabledReadNoModify"); // RF, OF, UPG
                                            }
                                          }
                                        }
                                        else if( MediaDirStateFlag == "Offline" ) {
                                          if( pvrMode == "ENABLED" && dirWriteMode == "ENABLED" ) {
                                            topic_Publish(tApp, "MediaSDRMenuSelected", "OfflinePVREnabledWrite"); // RF, OF, PF,
                                          }
                                          else if( pvrMode == "ENABLED" && dirWriteMode == "NONE" ) {
                                            topic_Publish(tApp, "MediaSDRMenuSelected", "OfflinePVREnabledRead"); // RF, OF, PF, UPG
                                          }
                                          else if( pvrMode == "DISABLED" && dirWriteMode == "ENABLED" ) { 
                                            topic_Publish(tApp, "MediaSDRMenuSelected", "OfflinePVRDisabledWrite"); // RF, OF,
                                          }
                                          else if( pvrMode == "DISABLED" && dirWriteMode == "NONE" ) {
                                            topic_Publish(tApp, "MediaSDRMenuSelected", "OfflinePVRDisabledRead"); // RF, OF, UPG
                                          }
                                        }
                                        else if( MediaDirStateFlag == "Deleting") {
                                          topic_Publish(tApp, "MediaSDRMenuSelected", "NONE");
                                        }
                                      }
                                 }
                                 
  var cmpDirNavControls = new CmpPageNavButtonsContainer("blkDirHeaderMenu",
                                                         [null,
                                                          null,
                                                          null,
                                                          null,
                                                          null,
                                                          null,
                                                          ["blkDirHeaderMenu", "btnPageRefresh"],
                                                          ["blkDirHeaderMenu", "btnPageRefreshSpinner"],
                                                          ["blkDirHeaderMenu", "btnPageRefreshNotify"]
                                                         ],
                                                          cmpMtxMediaSDRList,
                                                          [{ action: "TOPICEVENT", topic: tApp, eventvar: "MediaStreamDirResourceDataUpdated", eventval: null}],
                                                          null
                                                        );
  
  /************************** MEDIA FILES **********************************/

  var cmpBlkMediaList = cmpBlkCommonMedia_Create(tApp, eventApp, "blkMediaFilesCnt", "DataDir");
  var cmpBtnDeleteMediaSFR = new CmpImageButton(["blkMediaFilesCnt", "btnDeleteMediaSFR"], null,
                                                { action: "FNCCALL", context: this,
                                                    fnc: function () {
                                                        var dirNID = topic_Get(tApp, "MediaSDRSelected");
                                                        if (dirNID == "NONE") {
                                                            alert("Operation not allowed");
                                                            return false;
                                                        } else {
                                                            cmpBlkMediaSFRPrepareDelete.call(this);
                                                        }
                                                    }
                                                });

  var cmpBtnRepairMediaSFR = new CmpImageButton(["blkMediaFilesCnt","btnRepair"], null,
                                                { action:"FNCCALL", context:this,
                                                  fnc:function() {
                                                    this.SelectedMediaFileNID = topic_Get(tApp, "MediaSFRSelected");
                                                    cmpBlkMediaSFRRepair.call(this);
                                                  }
                                                });

  var cmpBtnExportMediaSFR = new CmpImageButton(["blkMediaFilesCnt","btnExportMedia"], null,
                                                { action:"FNCCALL", context:this,
                                                  fnc:function() {
                                                    var bExportItemAdded = false;
                                                    var bSkippedNonUpgradedMedia = false;
                                                    var cmpMediaList =  cmpBlkMediaList.BlkJSX.getDescendantOfName("cmpMediamtxMediaValue");
                                                    var selRecordNodes = cmpMediaList.getSelectedNodes();
                                                    var objDoc = jsx3.xml.CDF.Document.newDocument();
                                                    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                      var selRecordNode = it.next();
                                                      var mediaGroupID = selRecordNode.getAttribute("jsxmediaGroupID");
                                                      var fileType = selRecordNode.getAttribute("jsxmediaFileType");
                                                      //If it is group file then don't add that to Export list
                                                      //Add all clips from that group to export list
                                                      if(fileType == "MediaGroup" && !jsx3.util.strEmpty(mediaGroupID) ) {
                                                        var clipsIter = selRecordNode.selectNodeIterator(".//record");
                                                        while( clipsIter.hasNext() ) {
                                                          var clipRecord = clipsIter.next();
                                                          addExportItem(clipRecord);
                                                        }
                                                      }
                                                      else { //if it is a clip then add it to export list
                                                        addExportItem(selRecordNode);
                                                      }
                                                    }

                                                    function addExportItem(recNode) {
                                                      //If the clip is nonupgraded media don't allow to export
                                                      var fileReadOnlyMode = recNode.getAttribute("jsxtextmediaReadOnly");
                                                      if( fileReadOnlyMode == "false" ) {
                                                        var objRec = new Object;
                                                        var jsxID = recNode.getAttribute("jsxid");
                                                        objRec.jsxid = jsxID;
                                                        objRec.jsxtextmediaName = recNode.getAttribute("jsxtextmediaName");
                                                        objRec.jsxStreamType = recNode.getAttribute("jsxStreamType");
                                                        objRec.jsxMediaFileImg = recNode.getAttribute("jsxMediaFileImg");
                                                        objRec.jsxtextmediaSize = bytesToSize(recNode.getAttribute("jsxtextmediaSize"));
                                                        objRec.jsxtextmediaLength = recNode.getAttribute("jsxtextmediaLength");
                                                        objRec.jsxtextmediaCreated = getDateTimeFromTimeCode(recNode.getAttribute("jsxtextmediaCreated"));
                                                        objRec.jsxFileName = jsxID + ".mkd";
                                                        objRec.jsxendOffset = "";
                                                        objRec.jsxstartOffset = "";
                                                        objDoc.insertRecord(objRec);
                                                        bExportItemAdded = true;
                                                      }
                                                      else {
                                                        bSkippedNonUpgradedMedia = true;
                                                      }
                                                    }
                                                    if( bExportItemAdded ) {
                                                      if( bSkippedNonUpgradedMedia ) {
                                                        alert("Skipped NonUpgraded Media from export list");
                                                      }
                                                      cmpBlkExportMedia_Create(objDoc);
                                                    }
                                                    else {
                                                      alert("Cannot export NonUpgraded Media");
                                                    }
                                                  }
                                                });

  var cmpBtnExportImportStatus = new CmpImageButton("btnExportImportStatus", null,
                                                    { action:"FNCCALL", context:this,
                                                      fnc:function() {
                                                        if( !checkForStatusDialog() ) {
                                                          cmpBlkExportImportStatus_Create();
                                                        }
                                                      }
                                                    });

  var cmpBlkDeleteMediaSFR = new CmpContainer(["blkMediaFilesCnt","blkDeleteMediaSFR"],
                                              [{ cmp: cmpBtnDeleteMediaSFR, 
                                                  sel:"(topic_Get(tApp, 'MediaSFRSelectedDeletePermission') == 'true') &&" +
                                                      "(topic_Get(tApp, 'MediaSFRSelected') != null && " +
                                                      "topic_Get(tApp, 'MediaSFRSelected') != 'NONE' && " +
                                                      "(topic_Get(tApp, 'FileWriteMode') == 'ENABLED' || topic_Get(tApp, 'FileWriteMode') == 'ENABLEDMETADATA') && " +
                                                      // task 19441 Allow file to delete if state is error and fix for bug#42447
                                                      "topic_Get(tApp, 'FileStateFlag') != 'Busy' " + 
                                                      "&& (topic_Get(tApp, 'MediaSDROrphan') == null || " +
                                                      "topic_Get(tApp, 'MediaSDROrphan') == 'NONE')" +
                                                      ")"}],
                                             null
                                            );

  var cmpBlkRepair = new CmpContainer(["blkMediaFilesCnt","blkRepair"],
                                      [{ cmp: cmpBtnRepairMediaSFR, 
                                          sel:"(topic_Get(tApp, 'MediaSFRSelectedModifyPermission') == 'true') &&" +
                                              "(topic_Get(tApp, 'MediaSFRSelected') != null && " +
                                              "topic_Get(tApp, 'MediaSFRSelected') != 'NONE' && " +
                                              "topic_Get(tApp, 'MediaSFRSelected') != 'MULTI-ROWS' && " +
                                              "(topic_Get(tApp, 'FileWriteMode') == 'ENABLED' || topic_Get(tApp, 'FileWriteMode') == 'ENABLEDMETADATA') && " +
                                              "topic_Get(tApp, 'FileStateFlag') == 'ERROR' " +
                                              "&& (topic_Get(tApp, 'MediaSDROrphan') == null || " +
                                              "topic_Get(tApp, 'MediaSDROrphan') == 'NONE') "+
                                              ")"}],
                                     null
                                    );

  var cmpBlkExportMedia = new CmpContainer(["blkMediaFilesCnt","blkExportMedia"],
                                            [{ cmp: cmpBtnExportMediaSFR, 
                                                sel:"topic_Get(tApp, 'MediaDirStateFlag') == 'Ready' && " +
                                                    "topic_Get(tApp, 'MediaSFRSelectedExportPermission') == 'true' && " +
                                                    "topic_Get(tApp, 'FileStateFlag') == 'Ready' && " +
                                                    "topic_Get(tApp, 'MediaSFRSelected') != null && " +
                                                    "topic_Get(tApp, 'MediaSFRSelected') != 'NONE'"}],
                                           null
                                          );

  var cmpBtnMediaSFRCustomize = new CmpImageButton(["blkMediaFilesCnt","btnCustomizeMediaSFR"],
                                                   null,
                                                   { action: "FNCCALL", context: null,
                                                     fnc: function() {
                                                         topic_Publish(tApp, "MediaSFRColumnDataUpdated", (newServerDate()).getTime());
                                                         jsx3.sleep(function () {
                                                             topic_Publish(tApp, "MediaSFRCustomize", "OPEN");
                                                         }, null, this); 
                                                     }
                                                   }
                                                 );

  var cmpBlkMediaSFRCustomize = new CmpContainer(["blkMediaFilesCnt", "blkMediaFileResourceMenu"],
                                                 [{ cmp: cmpBtnMediaSFRCustomize, 
                                                    sel:"topic_Get(tApp, 'MediaSFRGetData') != 'NONE'"}],
                                                 null
                                                );
                                          
  var cmpBtnMediaSFRFilter = new CmpImageButton(["blkMediaFilesCnt","btnFilterMediaSFR"],
                                                 null,
                                                 { action: "FNCCALL", context: null,
                                                   fnc: function() {
                                                       jsx3.sleep(function () {
                                                           topic_Publish(tApp, "MediaSFRFilter", "OPEN");
                                                       }, null, this);
                                                   }
                                                 }
                                               );

  var cmpBlkMediaSFRFilter = new CmpContainer(["blkMediaFilesCnt", "blkMediaFileResourceMenu"],
                                             [{ cmp: cmpBtnMediaSFRFilter, 
                                                sel:"topic_Get(tApp, 'MediaSFRGetData') != 'NONE'"}],
                                             null
                                            );
                                                                                           
  var cmpBlkMediaSFRHeaderMenu = new CmpContainer(["blkMediaFilesCnt","blkMediaFileResourceMenu"],
                                                   [{ cmp: cmpBlkDeleteMediaSFR},
                                                    { cmp: cmpBlkRepair},
                                                    { cmp: cmpBlkExportMedia},
                                                    { cmp: cmpBlkMediaSFRCustomize},
                                                    { cmp: cmpBlkMediaSFRFilter}],
                                                   [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaSFRSelectedPermissions", eventval:null},
                                                    {action:"TOPICEVENT", topic:tApp, eventvar:"MediaSFRGetData", eventval:null}]);

  var cmpBlkMediaSFRList = new CmpContainer("blkMediaFilesCnt", 
                                              [ { cmp: cmpBlkMediaList },
                                                {cmp:cmpBlkMediaSFRHeaderMenu}
                                              ],
//                                              [{action:"APPEVENT", eventvar:"MediaSFRSelected", eventval:null},
//                                               {action:"APPEVENT", eventvar:"FileWriteMode", eventval:null},
  //                                               {action:"APPEVENT", eventvar:"FileStateFlag", eventval:null}],
                                              null,
                                              null
                                           );

  /************************** END MEDIA FILES **********************************/

  /************************** START PVR MEDIA FILES *******************************/


  var cmpBlkPvrMediaList = cmpBlkPvrMedia_Create(tApp, eventApp, "blkPVRFilesCnt");
  var cmpBtnExportPvrMediaSFR = new CmpImageButton(["blkPVRFilesCnt", "btnPvrExportMedia"], null,
                                                { action: "FNCCALL", context: this,
                                                  fnc: function () {
                                                    //check the playlength of teh selected file
                                                    var jsxPvrPlayLength = 0;
                                                    var cmpMediaList = cmpBlkPvrMediaList.BlkJSX.getDescendantOfName("cmpMediamtxPVRValue");
                                                    var selRecordNodes = cmpMediaList.getSelectedNodes();
                                                    var objDoc = jsx3.xml.CDF.Document.newDocument();
                                                    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                                                      var selRecordNode = it.next();
                                                      jsxPvrPlayLength = selRecordNode.getAttribute("jsxPvrPlayLength");
                                                    }

                                                    if (parseInt(jsxPvrPlayLength, 10) < 0) {
                                                      var ErrorMsg = "Error: Selected PVR File has negative ";
                                                      ErrorMsg += "playlength: " + jsxPvrPlayLength + ".\n";
                                                      ErrorMsg += "This happens only when your Dir and Management Server are not time synced.";
                                                      alert(ErrorMsg);
                                                      return;
                                                    }

                                                    // check if any export is going on or export status is in suspend state
                                                    getExportStatus();
                                                  }
                                                });

 var cmpBlkExportPvrMedia = new CmpContainer(["blkPVRFilesCnt", "blkPvrExportMedia"],
                                            [{ cmp: cmpBtnExportPvrMediaSFR,
                                              sel: "topic_Get(tApp, 'MediaDirStateFlag') == 'Ready' && " +
                                                    "topic_Get(tApp, 'PvrMediaSFRSelectedExportPermission') == 'true' &&" +
                                                    "topic_Get(tApp, 'PvrMediaSFRSelected') != null && " +
                                                    "topic_Get(tApp, 'PvrMediaSFRSelected') != 'NONE'"
                                            }],
                                           null
                                          );

  var cmpBlkPvrMediaSFRHeaderMenu = new CmpContainer(["blkPVRFilesCnt", "blkPvrFileResourceMenu"],
                                                   [{ cmp: cmpBlkExportPvrMedia }],
                                                   [{ action: "TOPICEVENT", topic: tApp, eventvar: "PvrMediaSFRSelectedPermissions", eventval: null },
                                                    { action: "TOPICEVENT", topic: tApp, eventvar: "PvrMediaSFRGetData", eventval: null}]);

   var cmpBlkPvrMediaSFRList = new CmpContainer("blkPVRFilesCnt",
                                              [{ cmp: cmpBlkPvrMediaList },
                                               { cmp: cmpBlkPvrMediaSFRHeaderMenu }
                                              ],
                                              null,
                                              null
                                           );

  /**************************END PVR MEDIA FILES **********************************/

  /**************************START CACHE MEDIA FILES **********************************/

   var cMediaTopic = tApp + "_Cache";
   topic_Create(cMediaTopic);
  // setup a listner for it
   var eventCache = new EventListner(cMediaTopic);
   var cmpBlkExportedMediaList = cmpBlkCommonMedia_Create(cMediaTopic, eventCache, "blkExportCacheFilesCnt", "Cache");

   var cmpBtnCachedExportedMediaSFR = new CmpImageButton(["blkExportCacheFilesCnt", "btnClearExportCache"], null,
                                                { action: "FNCCALL", context: this,
                                                    fnc: function () {
                                                      cmpBlkMediaPrepareClearCache();
                                                    }
                                                });

   var cmpBlkCachedExportedMediaSFRHeaderMenu = new CmpContainer(["blkExportCacheFilesCnt", "blkExportCacheMenu"],
                                                   [{ cmp: cmpBtnCachedExportedMediaSFR,
                                                   sel: "topic_Get('" + cMediaTopic + "', 'MediaSFRSelected') != null && " +
                                                         "topic_Get('" + cMediaTopic + "', 'MediaSFRSelected') != 'NONE' "
                                                   }],
                                                   [{ action: "TOPICEVENT", topic: cMediaTopic, eventvar: "MediaSFRSelectedPermissions", eventval: null },
                                                    { action: "TOPICEVENT", topic: cMediaTopic, eventvar: "MediaSFRGetData", eventval: null }]);

   var cmpBlkCachedExportedMediaSFRList = new CmpContainer("blkExportCacheFilesCnt",
                                              [{ cmp: cmpBlkExportedMediaList },
                                               { cmp: cmpBlkCachedExportedMediaSFRHeaderMenu }
                                              ],
                                              null,
                                              null
                                           );

   /**************************END CACHE MEDIA FILES **********************************/

  var cmpBlkMediaUI = new CmpContainer("cmpMediablkMediaValue1",
                                          [{ cmp: cmpBlkMediaSFRList,
                                            sel: "topic_Get(tApp, 'ViewMediaFileType') == 'DATA'" },
                                            { cmp: cmpBlkPvrMediaSFRList,
                                                sel: "topic_Get(tApp, 'ViewMediaFileType') == 'PVR'"
                                            },
                                            { cmp: cmpBlkCachedExportedMediaSFRList,
                                                sel: "topic_Get(tApp, 'ViewMediaFileType') == 'EXPORTED'"
                                            }
                                          ],
                                          [{ action: "APPEVENT", eventvar: "ViewMediaFileType", eventval: null}]
                                         );

  var cmpBtnDataMedia = new CmpImageButtonButton(["cmpMediablkMediaValue1", "btnTabMediaFiles"], null, { action: "APPEVENT", eventvar: "ViewMediaFileType", eventval: "DATA" });
  var cmpBtnPvrMedia = new CmpImageButtonButton(["cmpMediablkMediaValue1", "btnTabPvrFiles"], null, { action: "APPEVENT", eventvar: "ViewMediaFileType", eventval: "PVR" });
  var cmpBtnExportedMedia = new CmpImageButtonButton(["cmpMediablkMediaValue1", "btnTabExportedFiles"], null, { action: "APPEVENT", eventvar: "ViewMediaFileType", eventval: "EXPORTED" });

  var cmpBlkMediaHeaderMenu = new CmpContainer("blkMediaFileHeader",
                                         [{ cmp: cmpBtnDataMedia},
                                          { cmp: cmpBtnPvrMedia },
                                          { cmp: cmpBtnExportedMedia }
                                         ],
                                         [{ action: "APPEVENT", eventvar: "SHOWTABDATA", eventval: null}]
                                       );

  var cmpBlkmediaSFR = new CmpContainer("cmpMediablkMediaValue",
                                         [{ cmp: cmpBlkMediaHeaderMenu },
                                          { cmp: cmpBlkMediaUI }
                                         ],
                                         null
                                       );

  var cmpBlkDeleteMediaSDR = new CmpContainer("blkDeleteMediaSDR",
                                              [{ cmp: cmpBtnDeleteMediaSDR, 
                                                  sel: "(topic_Get(tApp, 'MediaDirStateFlag') == 'Offline' || "+ 
                                                       "topic_Get(tApp, 'MediaDirStateFlag') == 'Detached') && " +
                                                       "(topic_Get(tApp, 'MediaSDRSelectedDeletePermission') == 'true') && " +
                                                       "(topic_Get(tApp, 'MediaSDRSelected') != null && " +
                                                       "topic_Get(tApp, 'MediaSDRSelected') != 'NONE' " +                                                       
                                                       " && topic_Get(tApp, 'MediaSDROrphan') != null && " +
                                                       "topic_Get(tApp, 'MediaSDROrphan') != 'NONE'" +
                                                       ")"}],
                                             [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRSelectedPermissions", eventval:null}]
                                              
                                            );

  var cmpBlkExportImportStatus = new CmpContainer("blkExportImportStatus",
                                                  [{ cmp: cmpBtnExportImportStatus, 
                                                      sel: "topic_Get(tApp, 'MediaSDRSelectedImportPermission') == 'true' || " +
                                                           "topic_Get(tApp, 'MediaSFRSelectedExportPermission') == 'true'"}],
                                                 [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRSelectedPermissions", eventval:null},
                                                  {action:"TOPICEVENT", topic:tApp, eventvar:"MediaSFRSelectedPermissions", eventval:null}]

                                                );
                                                                                         
  var cmpBlkHeaderMediaSDR = new CmpContainer("blkDirHeaderMenu",
                                             [{ cmp: cmpBlkDeleteMediaSDR},
                                              { cmp: cmpBlkExportImportStatus}
                                             ],
                                             [{action:"TOPICEVENT", topic:tApp, eventvar:"MediaSDRSelectedPermissions", eventval:null}]
                                           );

  var cmpBlkMediaSDR = new CmpContainer("layoutMediaDir",
                                         [//{cmp:cmpBlkMediaSDRTab},
                                          {cmp:cmpBlkHeaderMediaSDR},
                                          {cmp:cmpMtxMediaSDRList},
                                          {cmp:cmpDirNavControls},
                                          {cmp:cmpMtxMediaDirMount},
                                          {cmp:cmpMtxMediaDirTags},
                                          {cmp:cmpMenuDirList}
                                         ],
                                         null
                                       );
                                                                                                                          
  var cmpBlkMedia = new CmpContainer( "blkMedia",
                                     [{ cmp: cmpBlkMediaSDR },
                                      { cmp: cmpBlkmediaSFR }
                                     ],
                                      null );

  /********************************* EXPORT / IMPORT SUBSCRIPTIONS **************************/

  function getExportStatus() {
    var reqHandleExportImportMedia = new ReqExportImportMedia("QueueStatusExportImportFile", null,
                                                              { action: "FNCCALL", context: this,
                                                                fnc: function (respCode, respVal) {
                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                    var exportStatus = false;
                                                                    var queueState = respVal.selectSingleNode(".//QueueState").getValue();
                                                                    var isSuspended = respVal.selectSingleNode(".//Suspended").getValue();
                                                                    if (isSuspended == "true" || queueState == "suspended" || queueState == "running") {
                                                                      exportStatus = true;
                                                                    }
                                                                    processExportPvrFile(exportStatus);
                                                                  }
                                                                }
                                                              }
                                                             );

    reqHandleExportImportMedia.sendRequest();

  }

  function processExportPvrFile(exportStatus) {
    if (exportStatus) {
      var msg = 'Caution: The Import/Export Queue is in "Supended" state or another file is currently being imported/exported.\nAs a result, you may lose some of your live PVR data.\n\nAre you sure you want to continue?\n';
      var agree = confirm(msg);
      if (!agree) {
        return;
      }
    }
    var bExportItemAdded = false;
    var bSkippedNonUpgradedMedia = false;
    var cmpMediaList = cmpBlkPvrMediaList.BlkJSX.getDescendantOfName("cmpMediamtxPVRValue");
    var selRecordNodes = cmpMediaList.getSelectedNodes();
    var objDoc = jsx3.xml.CDF.Document.newDocument();
    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
      var selRecordNode = it.next();
      var mediaGroupID = selRecordNode.getAttribute("jsxmediaGroupID");
      var fileType = selRecordNode.getAttribute("jsxmediaFileType");
      addExportItem(selRecordNode);
    }

    function addExportItem(recNode) {
      //If the clip is nonupgraded media don't allow to export
      var fileReadOnlyMode = recNode.getAttribute("jsxtextmediaReadOnly");
      if (fileReadOnlyMode == "false") {
        var objRec = new Object;
        var jsxID = recNode.getAttribute("jsxid");
        objRec.jsxid = jsxID;
        objRec.jsxtextmediaName = recNode.getAttribute("jsxtextmediaPVRName");
        objRec.jsxFileName = jsxID + ".mkd";
        objRec.jsxendOffset = "";
        objRec.jsxstartOffset = "";
        objDoc.insertRecord(objRec);
        bExportItemAdded = true;
      }
      else {
        bSkippedNonUpgradedMedia = true;
      }
    }

    if (bExportItemAdded) {
      if (bSkippedNonUpgradedMedia) {
        alert("Skipped NonUpgraded Media from export list");
      }
      cmpBlkExportPvrMedia_Create(objDoc);
    }
    else {
      alert("Cannot export NonUpgraded Media");
    }  
  }

  function checkForStatusDialog() {
    if (jsx3.gui.Dialog){
      try {
        var dialogBoxArr = cmpBlkMedia.BlkJSX.getDescendantsOfType(jsx3.gui.Dialog, false);
        var dialogBoxArr = PNameSpace.getBodyBlock().getDescendantsOfType(jsx3.gui.Dialog, false);
        var dlgNameArr = new Object();
        for (var i = 0; i < dialogBoxArr.length; i++) {
          var lcDlgName = dialogBoxArr[i].getName();
          if( lcDlgName == "dlgExportImportStatus" ) {
            dialogBoxArr[i].focus();
            return true;
          }
        }
        return false;
      } catch (e) {
        log.warn("ExportImportStatus dialog error:" + e.description);
        return false;
      }
    }
  }

  eventApp.subscribe("MenuSelection", "MEDIA", this,
                      function (EventVar, EventVal) {
                        var FileMenuSelection = topic_Get(tApp, 'ViewMediaFileType');
                        if (FileMenuSelection == "DATA") {
                          topic_Publish(tApp, "ViewMediaFileType", "DATA");
                        }
                        else if (FileMenuSelection == "PVR") {
                          topic_Publish(tApp, "ViewMediaFileType", "PVR");
                        }
                        else if (FileMenuSelection == "EXPORTED") {
                           topic_Publish(tApp, "ViewMediaFileType", "EXPORTED");
                        }
                      }
                    );

  eventApp.subscribe("ExportState", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "Started" ) {
                          if( !checkForStatusDialog() ) {
                            cmpBlkExportImportStatus_Create();
                          }
                        }
                      }
                    );

  eventApp.subscribe("ImportState", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "Started" ) {
                          if( !checkForStatusDialog() ) {
                            cmpBlkExportImportStatus_Create();
                          }
                        }
                      }
                    );
   
  /********************************* SUBSCRIPTIONS & CUSTOM FUNCTIONS **************************/
  
  eventApp.subscribe("ViewMediaFileType", null, this,
                      function () {
                        cmpBtnDataMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnPvrMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                        cmpBtnExportedMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);

                        var MediaFileType = topic_Get(tApp, 'ViewMediaFileType');
                        if (MediaFileType == null || MediaFileType == "DATA") {
                          cmpBtnDataMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          topic_Publish(tApp, "GetMediaFiles", (newServerDate()).getTime());
                        }
                        else if (MediaFileType == "PVR") {
                          cmpBtnPvrMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                          topic_Publish(tApp, "GetPvrMediaFiles", (newServerDate()).getTime());
                        }
                       else if (MediaFileType == "EXPORTED") {
                           cmpBtnExportedMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                           topic_Publish(cMediaTopic, "GetExportedMediaFiles", (newServerDate()).getTime());
                       }
                      });

  function resetMediaSDRSelectedPermisions() {
    topic_Set(tApp, "MediaSDRSelectedModifyPermission", "false");
    topic_Set(tApp, "MediaSDRSelectedDeletePermission", "false");
    topic_Set(tApp, "MediaSDRSelectedImportPermission", "false");   
    topic_Publish(tApp, "MediaSDRSelectedPermissions", (newServerDate()).getTime());   
  }
  
  // this subscription fills the dir properties section bottom half                                                                                                         
  eventApp.subscribe("MediaSDRSelected", null, this,
                      function() {
                        resetMediaSDRSelectedPermisions();
                        handleDirSelected();
                      }
                    );
    
  function handleDirSelected() {
    var lblTotal = getJsxByName("lblDiskTotal");
    lblTotal.setText("", true);
    var lblUsed = getJsxByName("lblDiskUsed");
    lblUsed.setText("", true);
    var lblDiskFullThreshold = getJsxByName("lblDiskThreshold");
    lblDiskFullThreshold.setText("", true);
    if(cmpMtxMediaDirMount.BlkJSX.clearXmlData != null){
      cmpMtxMediaDirMount.BlkJSX.clearXmlData();
      cmpMtxMediaDirMount.BlkJSX.repaint();
    }
    if(cmpMtxMediaDirTags.BlkJSX.clearXmlData != null){
      cmpMtxMediaDirTags.BlkJSX.clearXmlData();
      cmpMtxMediaDirTags.BlkJSX.repaint();
    }
    var selectedNID = topic_Get(tApp, "MediaSDRSelected");
    if( selectedNID == null || selectedNID == "NONE" ) return;
    
    var mediaDirNode = cmpMtxMediaSDRList.BlkJSX.getRecordNode(selectedNID);
    if( mediaDirNode == null ) return;
    topic_Set(tApp, "SelectedDirTitle", mediaDirNode.getAttribute("jsxDirName"));
    var dirTotalSize = mediaDirNode.getAttribute("jsxTotalSize");
    var dirSizeUsed = mediaDirNode.getAttribute("jsxSizeUsed");
    var diskFullThreshold = mediaDirNode.getAttribute("jsxFullThreshold");
    var mediaDirStateFlag = mediaDirNode.getAttribute("jsxStateFlag");
    if( jsx3.util.strEmpty(mediaDirStateFlag) ){
      topic_Publish(tApp, "MediaDirStateFlag", "Offline");
    }
    else {
      topic_Publish(tApp, "MediaDirStateFlag", mediaDirStateFlag);
    }
    
    var lblTotalText = "Total:&nbsp;<b>" + convertBytesToGigaBytes(dirTotalSize) + "GB</b>";
    lblTotal.setText(lblTotalText, true);

    var persentUsed = (dirSizeUsed/dirTotalSize) * 100;
    //rounding for 2 digits after decimal
    persentUsed = Math.round(persentUsed *100)/100;
    var lblUsedText = "Used:&nbsp;<b>" + persentUsed + "%</b>";
    lblUsed.setText(lblUsedText, true);

    var percentThreshold = ((dirTotalSize - diskFullThreshold)/dirTotalSize) * 100;
    //rounding for 2 digits after decimal
    //percentThreshold = Math.round(percentThreshold *100)/100;
    percentThreshold = Math.round(percentThreshold);
    var lblDiskFullThresholdText = "<i>Threshold set at:&nbsp;<b>" + percentThreshold + "%</b></i>";
    lblDiskFullThreshold.setText(lblDiskFullThresholdText, true);

   var dirOrphanState = true;
    var mediaStoreIter = mediaDirNode.selectNodeIterator(".//PortInfo/Port");
    cmpMtxMediaDirMount.BlkJSX.resetCacheData();
    while(mediaStoreIter.hasNext()) {
      var mediaStoreNode = mediaStoreIter.next();
      var id = mediaStoreNode.getAttribute("deviceElementId");
      var readOnly = mediaStoreNode.getAttribute("readOnly");
      var priority = mediaStoreNode.getAttribute("priority");
      var presence = mediaStoreNode.getAttribute("presence");
      var storePortNID = mediaStoreNode.getAttribute("NID");
      var storePortName = mediaStoreNode.getAttribute("title"); ;
      if( presence == "available" ) {
        presence ="images/icons/icon_greenBall.png";
        dirOrphanState = false;
      } else {
        presence = "images/icons/icon_grayBall.png";
      }
      var o = new Object;
      o.jsxid = storePortNID;
      o.jsxPresence = presence;
      o.jsxStorePortName = storePortName;
      o.jsxDirID = id;
      o.jsxPriority = priority;
      cmpMtxMediaDirMount.BlkJSX.insertRecord(o, null, false);
    }
    cmpMtxMediaDirMount.BlkJSX.repaintData();

    var MediaSDROrphanNID = "NONE";
    if( dirOrphanState ) {
      MediaSDROrphanNID = selectedNID;
    }
    topic_Set(tApp, "MediaSDROrphan", MediaSDROrphanNID);
        
    // get the groupTags
    var tagsIter = mediaDirNode.selectNodeIterator(".//Groups/Tag");
    cmpMtxMediaDirTags.BlkJSX.resetCacheData();
    while(tagsIter.hasNext()) {
      var tagNode = tagsIter.next();
      var tagName = tagNode.getAttribute("Name");
      var tagValue = tagNode.getAttribute("Value");
      if(tagName != "" && tagValue != "") {
        var o = new Object;
        o.jsxid = jsx3.xml.CDF.getKey();
        o.jsxTagName = tagName;
        o.jsxTagValue = tagValue;
        cmpMtxMediaDirTags.BlkJSX.insertRecord(o, null, false);
      }
    }
    cmpMtxMediaDirTags.BlkJSX.repaintData();
    
    var DirReadOnly = mediaDirNode.getAttribute("jsxReadOnly");
    if( DirReadOnly == "false" ) {
      topic_Set(tApp,"DirWriteMode", "ENABLED");
    }
    else {
      topic_Set(tApp,"DirWriteMode", "NONE");
    }
    
    // check if PVR dir exists
    var PVRMode = mediaDirNode.getAttribute("jsxPVRMode");
    topic_Set(tApp,"PVR", PVRMode);

    // check if PVR dir exists
    var PVRDirNID = mediaDirNode.getAttribute("jsxPVRDirNID");
    topic_Set(tApp, "PvrMediaSDRSelected", PVRDirNID);

    //set the dir permissions
    if( mediaDirStateFlag != 'Deleting') {
      var modifyPer = mediaDirNode.selectSingleNode("./Permissions/@modify").getValue();
      if( jsx3.util.strEmpty(modifyPer) ){
        modifyPer = "false";
      }
      topic_Set(tApp, "MediaSDRSelectedModifyPermission", modifyPer);
      
      var deletePer = mediaDirNode.selectSingleNode("./Permissions/@delete").getValue();
      if( jsx3.util.strEmpty(deletePer) ){
        deletePer = "false";
      }    
      topic_Set(tApp, "MediaSDRSelectedDeletePermission", deletePer);
      
      var importPer = mediaDirNode.selectSingleNode("./Permissions/@import").getValue();
      if( jsx3.util.strEmpty(importPer) ){
        importPer = "false";
      }
      topic_Set(tApp, "MediaSDRSelectedImportPermission", importPer);
    }
    else {
      topic_Set(tApp, "MediaSDRSelectedModifyPermission", "fasle");
      topic_Set(tApp, "MediaSDRSelectedDeletePermission", "fasle");
      topic_Set(tApp, "MediaSDRSelectedImportPermission", "fasle");
    }     
         
    // publish the dir permissions
    topic_Publish(tApp, "MediaSDRSelectedPermissions", (newServerDate()).getTime());
        
    // publish to get the media files
    jsx3.sleep(function () {
      if (topic_Get(tApp, 'ViewMediaFileType') == "DATA") {
        cmpBtnDataMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
        topic_Publish(tApp, "GetMediaFiles", (newServerDate()).getTime());
      }
      else if (topic_Get(tApp, 'ViewMediaFileType') == "PVR") {
        cmpBtnPvrMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
        topic_Publish(tApp, "GetPvrMediaFiles", (newServerDate()).getTime());
      }
     else if (topic_Get(tApp, 'ViewMediaFileType') == "EXPORTED") {
         cmpBtnExportedMedia.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
         topic_Publish(cMediaTopic, "GetExportedMediaFiles", (newServerDate()).getTime());
     }
    }, null, this);                                    

  }

  eventApp.subscribe("MediaSDRMenuActionSelected", null, this,
                      function () {
                        var MediaMenuActionSelected = topic_Get(tApp, 'MediaSDRMenuActionSelected');

                        var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                        if (selectedNID == "NONE") return;

                        if (MediaMenuActionSelected == "Edit...") {
                          var matchResult = selectedNID.search(/jsx/);
                          if (matchResult != -1) {
                            alert("Cannot edit root directory");
                            return false;
                          } else {
                            dlgEditMediaSDR_Create(selectedNID);
                          }
                        }
                        else if (MediaMenuActionSelected == "Capacity Management...") {
                          dlgEditMediaSDRStorageConfig_Create(selectedNID);
                        }
                        else if (MediaMenuActionSelected == "View Recycle Bin Files...") {
                          topic_Publish(tApp, "MediaDirViewSelected", "RECYCLEBIN");
                        }
                        else if (MediaMenuActionSelected == "ViewPVRFiles...") {
                          topic_Publish(tApp, "MediaDirViewSelected", "PVR");
                        }
                        else if (MediaMenuActionSelected == "Upgrade...") {
                          dlgMediaSDR_Upgrade(selectedNID);
                        }
                        //  //***Task 35701 remove ViewOrphan media ***//                         
                        //                        else if( MediaMenuActionSelected == "ViewOrphanFiles..." ) {
                        //                          topic_Publish(tApp, "MediaDirViewSelected", "ORPHAN");
                          //                        }
                          // //****Issue#407
                        //else if (MediaMenuActionSelected == "Unregister Media Directory...") {
                        //  dlgMediaSDR_AttachDetach("DetachDir");
                        //}
                        //else if (MediaMenuActionSelected == "Register Media Directory...") {
                        //  dlgMediaSDR_AttachDetach("AttachDir");
                        //}
                        else if (MediaMenuActionSelected == "Import Media") {
                          cmpBlkImportMedia_Create(selectedNID);
                        }
                      }
                    );
                      
  
  //to view RecycleBin and PVR files
  eventApp.subscribe("MediaDirViewSelected", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == null ) return;
                        var DirTitle = EventVal;
                        var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                        var viewDirNID = null;
//  //***Task 35701 remove ViewOrphan media ***//                         
//                        if(DirTitle == "ORPHAN") {
//                          viewDirNID = selectedNID;
//                        }
//                        else 
                        if(DirTitle == "RECYCLEBIN") {
                          var mediaDirNode = cmpMtxMediaSDRList.BlkJSX.getRecord(selectedNID);
                          viewDirNID = mediaDirNode.jsxRecycleBinDirNID;
                        }
                        else if(DirTitle == "PVR") {
                          var mediaDirNode = cmpMtxMediaSDRList.BlkJSX.getRecord(selectedNID);
                          viewDirNID = mediaDirNode.jsxPVRDirNID;                     
                        }
                        
                        if( jsx3.util.strEmpty(viewDirNID) || viewDirNID == "NONE" ) {
                          alert("Cannot find " + DirTitle + " directory");
                          return false;
                        }
                        else {
                          dlgViewFiles_Create(viewDirNID, DirTitle);

//                          var dirViewState = checkSelectedDirViewState(viewDirNID);
//                          if( !dirViewState ) {
//                            topic_Set(viewDirNID, "SelectedDirTitle", DirTitle);
//                            dlgViewFiles_Create(viewDirNID, DirTitle);
//                          }
                        }
                        
                      }
                   );

  eventApp.subscribe("MENUUIDATA", "MEDIA", this,
                      function(EventVar, EventVal) {
                        if( EventVal == "MEDIA" ) {
                          var getDataState = topic_Get(tApp, "MediaSDRGetDataState");
                          if(getDataState == "NONE") {
                            setObjectCacheDefaultXML("MediaStreamFileResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamFileResource.xml", "DEFAULT_MEDIASTREAMFILE_NID");
                            topic_Set(tApp, "EditMediaSFRDataState", "DEFAULT_MEDIASTREAMFILE_NID");
                          
                            setObjectCacheDefaultXML("MediaStreamDirResource", "JSXAPPS/IPVS-Common/model/objectXml/MediaStreamDirResource.xml", "DEFAULT_MEDIASTREAMDIR_NID");
                            topic_Set(tApp, "EditMediaSDRDataState", "DEFAULT_MEDIASTREAMDIR_NID");  
                                                    
                            topic_Set(tApp, "MediaSDRGetDataState", (newServerDate()).getTime() );
                            cmpMtxMediaSDRList.ResetToStart(1,5000,0);
                            topic_Publish(tApp, "MediaSDRGetData", (newServerDate()).getTime() ); 
                          }
                        }
                      }
                    );

  eventApp.subscribe("DBUpdate", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       if( EventVal.objType == "MediaStreamDirResource" ) {
                         cmpMtxMediaSDRList.handleNotification(EventVal);
                         if( EventVal.type == "delete" ) {
                           var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                           if( EventVal.nodeID == selectedNID ) {
                             //topic_Publish(tApp, "MediaSDRSelected", selectedNID);
                             topic_Publish(tApp, "MediaSDRSelected", "NONE");
                           }
                         }
                         else if(EventVal.type == "add"){
                           topic_Publish(tApp, "MediaSDRGetData", (newServerDate()).getTime() );
                         }
                       }                                   
                     });

  eventApp.subscribe("IMPORTFILEEVENT", null, this,
                     function(EventVar, EventVal) {
                       if( jsx3.util.strEmpty(EventVal) ) return;
                       var ImportFileEvent = EventVal;
                       if( ImportFileEvent.reqName == "ImportFilePackRequest" || ImportFileEvent.reqName == "ImportCleanupMediaFileRequest" ) {
                         var messageNode = ImportFileEvent.messageNode;
                         var isMedia2dot4 = messageNode.selectSingleNode(".//ExportImportFileListItem/Internal/@isMedia2dot4").getValue();
                         var blobFolder = messageNode.selectSingleNode(".//ExportImportFileListItem/External/@blobFolder").getValue();
                         var blobName = messageNode.selectSingleNode(".//ExportImportFileListItem/External/@blobName").getValue();
                         var fileName = messageNode.selectSingleNode(".//ExportImportFileListItem/External/@fileName").getValue();
                         var folderPath = messageNode.selectSingleNode(".//ExportImportFileListItem/External/@folderPath").getValue();
                         var respState = true;
                         if( isMedia2dot4 == "true" ) {
                           if( ImportFileEvent.reqName == "ImportFilePackRequest" ) {
                             var cScriptPath = giStartPath + "\\JSXAPPS\\IPVS-Common\\vb\\Create2dot4MediaZip.vbs";
                             var cmd = "cscript " +  "\"" + cScriptPath + "\" \"" + folderPath + "\" \"" + blobFolder + "\" \"" + blobName + "\"";
                             var procState = cmdRun(cmd, 1, true);
                             if( procState != 0 ) { //0 success
                               respState = false;
                             }
                           }
                           else if( ImportFileEvent.reqName == "ImportCleanupMediaFileRequest" ) {
                             var FSOObject = new ActiveXObject("Scripting.FileSystemObject");
                             if(FSOObject.FileExists(folderPath + "\\" + fileName)){
                               FSOObject.DeleteFile(folderPath + "\\" + fileName)
                             }
                             delete FSOObject;
                           }
                         }
                         //run xsl to send response for agent message
                         if( respState ) {
                           var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/ImportFilePack.xsl", "ImportFilePack", "xsl");
                           var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
                           var userJID = topic_Get(tApp, "LoginUserJID");
                           var MSJID = topic_Get(tApp,"AppServerAgentJID");
                           xslDoc.setParam("MS_JID", MSJID);
                           var respAgentMessage = xslDoc.transform(messageNode);
                           var AgentMessageNode = new jsx3.xml.Document();
                           AgentMessageNode.loadXML(respAgentMessage);
                           clsApplet.applet_sendRequest(userJID, AgentMessageNode);
                         }
                       }                                   
                     });      

  function checkSelectedDirViewState(MediaDirNID) {
    if( !topic_Exists(MediaDirNID) ) {
      var dirTopic = topic_Create(MediaDirNID);
    }
    if( topic_Get(MediaDirNID, "DirViewState") != null ) {
      return true;
    }
    else {
      topic_Set(MediaDirNID, "DirViewState", "TRUE");
      return false;
    }
  }

  return cmpBlkMedia;

  function cmpBlkMediaSFRPrepareDelete() {
    cmpMtxmediaFile = null;
    var mediaFileArr = new Object;

    for (var i = 0; i < cmpBlkMediaList.SubCmpArr.length; i++) {
      if (cmpBlkMediaList.SubCmpArr[i].cmp.BlkJSX.getName() == "cmpMediamtxMediaValue") {
        cmpMtxmediaFile = cmpBlkMediaList.SubCmpArr[i].cmp;
        break;
      }
    }
   
    var cmpMediaList = cmpBlkMediaList.BlkJSX.getDescendantOfName("cmpMediamtxMediaValue");
    var selRecordNodes = cmpMediaList.getSelectedNodes();

    //get all the nids and poush in the arr
    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
      var selRecordNode = it.next();
      var SelectedNID = selRecordNode.getAttribute("jsxid");
      var selectedMetaData = selRecordNode.selectSingleNode("./MetaData");
      var mediaFileType = selRecordNode.getAttribute("jsxmediaFileType");
      var mediagroupId = selRecordNode.getAttribute("jsxmediaGroupID");

      if(mediaFileArr[SelectedNID] == null ){
        mediaFileArr[SelectedNID] = new Object;
      }
      mediaFileArr[SelectedNID] = { "NID": SelectedNID, "METADATA": selectedMetaData, "FILETYPE": mediaFileType, "GROUPID": mediagroupId };
    }

    // go thru the arr and find out the mediagroup file with mediagroupid.
    //look for mediaclip and mediagroupid
    //if found then set to null.
    for (var nid in mediaFileArr) {
      if (mediaFileArr[nid].FILETYPE == "MediaGroup") {
        var mediaGrpID = mediaFileArr[nid].GROUPID;
        for (var nid2 in mediaFileArr) {
          if (mediaFileArr[nid2].FILETYPE == "MediaClip" && mediaFileArr[nid2].GROUPID == mediaGrpID) {
            mediaFileArr[nid2] = null;
            delete mediaFileArr[nid2];
          }
        }
      }
    }

    var agree = confirm('Are you sure you want to delete selected Media Files?');
    cmpMtxmediaFile.deleteRequestCounter = 0;
    if (agree) {
        for (var nid in mediaFileArr) {
            ++cmpMtxmediaFile.deleteRequestCounter;
            this.SelectedNID = mediaFileArr[nid].NID;
            var selectedMetaData = mediaFileArr[nid].METADATA;
            cmpBlkMediaSFRDelete(this.SelectedNID, selectedMetaData);
        }
    }
}

function cmpBlkMediaPrepareClearCache() {
    var mediaFileArr = new Object;
    var cmpMediaList = cmpBlkExportedMediaList.BlkJSX.getDescendantOfName("cmpMediamtxMediaValue");
    var selRecordNodes = cmpMediaList.getSelectedNodes();

    //get all the nids and poush in the arr
    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
        var selRecordNode = it.next();
        var SelectedNID = selRecordNode.getAttribute("jsxid");
        var mediaFileType = selRecordNode.getAttribute("jsxmediaFileType");
        var mediagroupId = selRecordNode.getAttribute("jsxmediaGroupID");

        if (mediaFileArr[SelectedNID] == null) {
            mediaFileArr[SelectedNID] = new Object;
        }
        mediaFileArr[SelectedNID] = { "NID": SelectedNID, "FILETYPE": mediaFileType, "GROUPID": mediagroupId };
    }

    // go thru the arr and find out the mediagroup file with mediagroupid.
    //look for mediaclip and mediagroupid
    //if found then set to null.
    for (var nid in mediaFileArr) {
        if (mediaFileArr[nid].FILETYPE == "MediaGroup") {
            var mediaGrpID = mediaFileArr[nid].GROUPID;
            for (var nid2 in mediaFileArr) {
                if (mediaFileArr[nid2].FILETYPE == "MediaClip" && mediaFileArr[nid2].GROUPID == mediaGrpID) {
                    mediaFileArr[nid2] = null;
                    delete mediaFileArr[nid2];
                }
            }
        }
    }
       for (var nid in mediaFileArr) {
            this.SelectedNID = mediaFileArr[nid].NID;
            cmpBlkMediaClearDownloadCache(this.SelectedNID);
        }
}

 function cmpBlkMediaClearDownloadCache(selectedNID) {

     var reqGetClearDownloadCache = new ReqSessionGetRequest("ClearDownloadCache",
                                                      { action: "FNCCALL", context: this,
                                                          fnc: function (respCode, respVal) {
                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                  cmpBlkExportedMediaList.BlkJSX.getDescendantOfName("cmpMediamtxMediaValue").deleteRecord(selectedNID);
                                                              }
                                                              else {
                                                                  log.error("ClearDownloadCache request failed:");
                                                              }
                                                          }
                                                      });
    reqGetClearDownloadCache.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = selectedNID;
    reqGetClearDownloadCache.reqParams = reqParams;
    // send the request
    reqGetClearDownloadCache.sendRequest(this);

  }
  
  function cmpBlkMediaSFRRepair() {
    var reqSendMediaSFRRepair = new ReqHandleMediaStreamFile(this.SelectedMediaFileNID, "RepairMediaStreamFile", 
                                                             { action:"FNCCALL", context:this,
                                                               fnc:function(respCode, respVal) {
                                                                     if(respCode == "SUCCESS" && respVal != null) {
                                                                     }
                                                                     else {
                                                                       var codeErrorMsg = "";
                                                                       //Using try catch to proceed even the response code is not
                                                                       // available in CommonErrorCodeArr
                                                                       try {
                                                                         codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                       }
                                                                       catch(e) {
                                                                       }
                                                                       var ErrorMsg = "Error: Repair MediaStreamFile request failed \n";
                                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                       ErrorMsg += "Error code: " + respCode;
                                                                       alert(ErrorMsg);
                                                                     }
                                                                   }
                                                             }
                                                           );

    reqSendMediaSFRRepair.sendRequest(this);
  }

  function cmpBlkMediaSFRDelete(selectedNID, selectedMetaData) {
    //Send audit event for delete Media
    IpvsLogin.auditEvent("Deleted", selectedMetaData);
    var reqDelMediaSFR = new ReqSessionGetRequest("DeleteFile",
                                                 { action: "FNCCALL", context: this,
                                                   fnc: function (respCode, respVal) {
                                                     if (respCode == "SUCCESS" && respVal != null) {
                                                       --cmpMtxmediaFile.deleteRequestCounter < 0 ? 0 : cmpMtxmediaFile.deleteRequestCounter;
                                                       topic_Publish(tApp, "MediaSFRSelected", "NONE");
                                                       topic_Publish(tApp, "MediaSFRDataDeleteNode", selectedNID);
                                                       if (cmpMtxmediaFile.deleteRequestCounter == 0) {
                                                         topic_Publish(tApp, "RefreshMediaSFRList", (newServerDate()).getTime());
                                                       }
                                                     }
                                                     else if (respCode == "5028") {
                                                       --cmpMtxmediaFile.deleteRequestCounter < 0 ? 0 : cmpMtxmediaFile.deleteRequestCounter;
                                                       alert("Can not delete Media File. \nMedia File is in Use. ");
                                                       return false;
                                                     }
                                                     else {
                                                       --cmpMtxmediaFile.deleteRequestCounter < 0 ? 0 : cmpMtxmediaFile.deleteRequestCounter;
                                                       var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                       var ErrorMsg = "Error: Media file delete request failed \n";
                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                       ErrorMsg += "Error code: " + respCode;
                                                       alert(ErrorMsg);
                                                     }
                                                   }
                                                 }
                                               );
    reqDelMediaSFR.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = selectedNID;
    reqDelMediaSFR.reqParams = reqParams;
    // send the request
    reqDelMediaSFR.sendRequest();
  }

  function dlgMediaSDR_Clean(mediaSDRNID) {
    var agree = confirm('Are you sure you want to Clean this directory?\nThis action will delete all files and corresponding bookmarks.');
    if(agree) {
      log.warn("Clean Dir NID sending request : " + mediaSDRNID);
      //topic_Publish(tApp, "MediaSDRSelected","NONE");
      var reqCleanMediaSDR = new ReqHandleDir(mediaSDRNID, null, "CleanOrphanDir", null,
                                            {action:"FNCCALL", context:this,
                                              fnc:function(respCode, respVal) {
                                                    if(respCode == "SUCCESS" && respVal != null) {
                                                      var dirResourceNodeList = topic_GetNode(tCache, "//MediaStreamDirResourceList");
                                                      var objDirNameNode = dirResourceNodeList.selectSingleNode(".//MediaStreamDirResource[@NID='"+ mediaSDRNID +"']");
                                                      if( !jsx3.util.strEmpty(objDirNameNode) ) {
                                                        dirResourceNodeList.removeChild(objDirNameNode);
                                                      }
                                                      log.warn("Clean Dir NID Success : " + mediaSDRNID);  
                                                    } else {
                                                       var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                       var ErrorMsg = "Error: Media dir Clean request failed \n";
                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                       ErrorMsg += "Error code: " + respCode;
                                                       alert(ErrorMsg);
                                                    }
                                                  }
                                             }
                                           );
      reqCleanMediaSDR.sendRequest();
    }
  }

  function dlgMediaSDR_Upgrade(mediaSDRNID) {
    //var upgradeMediaStreamFiles = false;
    var agree = confirm('This will upgrade the directory and all the media files in this directory. Do you want to continue?');
    if (!agree) {
      return;
      //upgradeMediaStreamFiles = true;
    }
    //log.info("Dir Upgrade All the files: " + upgradeMediaStreamFiles);   
    var reqUpgradeMediaSDR = new ReqSessionGetRequest("UpgradeDir",
                                                      {action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               log.info("Upgrade Dir NID Success : " + mediaSDRNID);  
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Media dir Upgrade request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                      }
                                                     );
    reqUpgradeMediaSDR.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = mediaSDRNID;
    reqParams["UpgradeFiles"] = true;
    reqUpgradeMediaSDR.reqParams = reqParams;
    // send the request
    reqUpgradeMediaSDR.sendRequest();
  }
  
  //// Issue#407
  ////helper function
  //function dlgMediaSDR_AttachDetach(AttachDetachMode) {
  //  var selectedDirNID = topic_Get(tApp, "MediaSDRSelected");
  //  var matchResult = selectedDirNID.search(/jsx/);

  //  if(matchResult != -1) {
  //   alert("Cannot delete root directory");
  //   return false;
  //  } 
    
  //  var agree = false;
  //  if(AttachDetachMode == "DetachDir" ) {
  //    agree = confirm("Upon unregistering a media directory from a Management Server you will lose access to all media stored in the directory."+
  //                    "\n Are you sure you want to continue?");
  //  }
  //  else{
  //    agree = true;
  //  }
    
  //  //send the request
  //  if(agree){
  //    log.warn("Sending request '"+ AttachDetachMode +"' Dir NID: " + selectedDirNID);
  //    var reqMediaSDR = new ReqHandleDir(selectedDirNID, null, AttachDetachMode, null,
  //                                          {action:"FNCCALL", context:this,
  //                                            fnc:function(respCode, respVal) {
  //                                                  if(respCode == "SUCCESS" && respVal != null) {
  //                                                    log.warn( AttachDetachMode + " Dir NID Success : " + selectedDirNID);
  //                                                    // listening in mediacommonutils.js
  //                                                    topic_Publish(tApp,"MediaDirAttachDetach","COMPLETE");  
  //                                                  } else {
  //                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
  //                                                     var ErrorMsg = "Error: Media dir "+ AttachDetachMode +" request failed \n";
  //                                                     ErrorMsg += "Error description: " + codeErrorMsg + "\n";
  //                                                     ErrorMsg += "Error code: " + respCode;
  //                                                     alert(ErrorMsg);
  //                                                  }
  //                                                }
  //                                           }
  //                                         );
  //    reqMediaSDR.sendRequest();      
  //  }

  //} // end 
  
} // end cerate

function cmpBlkMedia_Destroy() {
  var blkMediaBlkJSX = PNameSpace.getJSXByName("blkMedia");
  blkMediaBlkJSX.removeChildren();
}
