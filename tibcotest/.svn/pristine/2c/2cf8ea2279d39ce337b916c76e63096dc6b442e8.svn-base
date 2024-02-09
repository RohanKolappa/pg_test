function cmpBlkCommonMedia_Create(tName, eventName, DlgBlkJsx, ViewFileType) {

  var blkJSX = DlgBlkJsx;
  var tMedia = tName;
  var eventMedia = eventName; 
  var viewFileType = ViewFileType;
  
  topic_Set(tMedia, "MediaStreamFileResourceDataUpdated","LOADED" );
  topic_Set(tMedia, "MediaStreamFileResourceNavButtonClicked", (newServerDate()).getTime());
  topic_Set(tMedia, "MediaSFRCustomize", "NONE");
  topic_Set(tMedia, "MediaSFRFilter", "NONE");
  topic_Set(tMedia, "MediaStreamFileTagNameObjectType", "Media");
  topic_Set(tMedia, "WhereTextSubStr", "");
  topic_Set(tMedia, "SelectFormat", "EXTENDED");
  topic_Set(tMedia, "WherePermissions", "MODIFY,DELETE,EXPORT");
  topic_Set(tMedia, "SearchTextSubStrIn", "");
  topic_Set(tMedia, "WhereTagExpr", "");
  topic_Set(tMedia, "MediaMenuSelected", "NONE");

  topic_Set(tMedia, "MediaSFRSelected", "NONE");
  topic_Set(tMedia, "MediaSFRSelectedDeletePermission", "false");
  topic_Set(tMedia, "MediaSFRSelectedExportPermission", "false"); 
  topic_Set(tMedia, "MediaSFRSelectedModifyPermission", "false");  
  topic_Set(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());

  topic_Set(tMedia, "MediaSFRGetData", "NONE");

  topic_Set(tMedia, "BypassAutoStorageRules", "--Keep");
  topic_Set(tApp, "WhereMediaListNID", "NONE");

  topic_Set(tMedia, "WhereDownloadState", "Ready");
  topic_Set(tApp, "WhereMediaListNID", "NONE");

  var cmpBtnEmptyDir = new Component();
  var cmpBtnStopEmptyDir = new Component();
  if (viewFileType != "DataDir" && viewFileType != "Cache") {     
    cmpBtnEmptyDir = new CmpImageButton([blkJSX, "btnEmptyMediaDir"], null,
                                            { action:"FNCCALL", context:this,
                                              fnc:function() {
                                                 var selectedNID = topic_Get(tMedia, "MediaSDRSelected");
                                                 if( (selectedNID == null) || (selectedNID == "NONE") ) return;
                                                 var matchResult = selectedNID.search(/jsx/);

                                                 if(matchResult != -1) {
                                                   alert("Cannot delete root directory");
                                                   return false;
                                                 } else {
                                                   var SelectedMediaSDRName = topic_Get(tMedia, "SelectedDirTitle");
                                                   if( SelectedMediaSDRName == null || SelectedMediaSDRName == "NONE" ) return;
                                                   log.warn("Empty Dir NID : " + selectedNID);
                                                   var agree = confirm('Are you sure you want to PERMANENTLY DELETE all the files in the Media Directory: "'+ SelectedMediaSDRName +'" ?' );
                                                   if(agree) {
                                                     topic_Set(selectedNID, "EmptyDirState","NONE");
                                                     topic_Publish(tMedia, "EmptyDirState","NONE");
                                                     cmpBlkDirEmpty.call(this, "Start", selectedNID);
                                                   }
                                                 
                                                 }
                                              }
                                            });

    cmpBtnStopEmptyDir = new CmpButton([blkJSX, "btnStopEmptyMediaDir"], 
                                             [{action:"TOPICEVENT", topic:tMedia, eventvar:"MediaSDRSelected", eventval:null}],
                                             {action:"FNCCALL", 
                                              context:this,
                                                  fnc:function() {
                                                     var selectedNID = topic_Get(tMedia, "MediaSDRSelected");
                                                     log.warn("Stop Empty Dir NID : " + selectedNID);
                                                     var agree = confirm('Are you sure you want to STOP empty Media Directory Process? ');
                                                     if(agree) {
                                                       topic_Set(selectedNID, "EmptyDirState","ENABLE");
                                                       cmpBlkDirEmpty.call(this, "Stop", selectedNID);
                                                     }
                                                  }
                                             });
  }

  var cmpBlkDelMsg = new CmpContainer([blkJSX, "blkEmptyingMediaDir"],
                                      [{cmp:cmpBtnStopEmptyDir}],
                                       null );
  var cmpBtnRecycleRestore = new Component();
  if (viewFileType == "RECYCLEBIN") {
    cmpBtnRecycleRestore = new CmpImageButton([blkJSX, "btnRecycleRestore"], null,
                                          { action:"FNCCALL", context:this,
                                            fnc:function() {
                                               cmpBlkRecycleRestoreFiles.call(this);
                                            }
                                          });
  }
  
  var cmpBlkEmptyStartStop = new CmpContainer([blkJSX, "blkMediaFileResourceMenu"],
                                              [{ cmp: cmpBtnEmptyDir, sel: "(clsUserPermission.getPermission('File', 'delete') == 'true' && " +
                                                                          "topic_Get('" + tMedia + "', 'EmptyMediaDirState') != 'NONE' && " +
                                                                          "topic_Get('" + tApp + "', 'MediaDirStateFlag') == 'Ready' &&" +
                                                                          "topic_Get('" + tMedia + "', 'SelectedDirTitle') == 'RECYCLEBIN' ||" +
                                                                          "topic_Get('" + tMedia + "', 'SelectedDirTitle') == 'PVR') ||" +
                                                                          "(topic_Get('" + tMedia + "', 'MediaSFRSelectedDeletePermission') == 'true' && " +
                                                                          "topic_Get('" + tMedia + "', 'EmptyMediaDirState') != 'NONE' && " +
                                                                          "topic_Get('" + tApp + "', 'MediaDirStateFlag') == 'Ready')"
                                              },
                                               { cmp: cmpBtnRecycleRestore, sel: "(topic_Get('" + tMedia + "', 'MediaSFRSelectedDeletePermission') == 'true' && topic_Get('" + tApp + "', 'MediaDirStateFlag') == 'Ready')" },
//            #34857. Delete flag in Manage Media policy does not hide the "Delete All" button                                               
//                                                                           "(clsUserPermission.getPermission('Dir', 'delete') == 'true') && " +
//                                                                           "(topic_Get('" + tMedia + "', 'MediaSDRSelected') != 'NONE' && " + 
//                                                                           "topic_Get('" + tMedia + "', 'MediaSDRSelected') != null && " +
//                                                                           "(topic_Get('" + tMedia + "', 'MediaSDROrphan') == null || topic_Get('" + tMedia + "', 'MediaSDROrphan') == 'NONE'))"},

//            //***Task 35701 remove ViewOrphan media ***// 
//                                               {cmp:cmpBlkDelMsg, sel:"topic_Get('" + tMedia + "', 'EmptyDirState') == 'NONE' && " +
//                                                                      "topic_Get('" + tMedia + "', 'MediaSDRSelected') != 'NONE' && " +
//                                                                      "topic_Get('" + tMedia + "', 'MediaSDRSelected') != null && " +
//                                                                      "(topic_Get('" + tMedia + "', 'MediaSDROrphan') == null || topic_Get('" + tMedia + "', 'MediaSDROrphan') == 'NONE')"}
                                                  {cmp:cmpBlkDelMsg, sel:"topic_Get('" + tMedia + "', 'EmptyDirState') == 'NONE' && " +
                                                                          "topic_Get('" + tMedia + "', 'MediaSDRSelected') != 'NONE' && " +
                                                                          "topic_Get('" + tMedia + "', 'MediaSDRSelected') != null "}
                                              ],
                                              [{action: "TOPICEVENT", topic: tMedia, eventvar: "EmptyDirState", eventval: null },
                                               {action: "TOPICEVENT", topic: tApp, eventvar: "MediaDirStateFlag", eventval: null },
                                               {action:"TOPICEVENT", topic:tMedia, eventvar:"MediaSFRSelectedPermissions", eventval:null}] );

                                             
  /************************** MEDIA FILES **********************************/

   var cmpMtxMediaSFRList = new CmpCustomPaginationList([blkJSX, "cmpMediamtxMediaValue"],
                                                       [{ action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRGetData", eventval: null}],
                                                       { persist: { col: "jsxid", initval: false },
                                                         action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRSelected", eventval: "jsxid"
                                                       },
                                                       [{ ColumnName: "jsxtextmediaCreated", FncName: "@datetime,MM/dd/yyyy HH:mm" },
                                                        { ColumnName: "jsxtextmediaSize", FncName: "convertbytesToSize" }
                                                       ],
                                                       null
                                                     );
  //Set selection model as multi row only for main media list  
 if (viewFileType == "RECYCLEBIN" || viewFileType == "DataDir" || viewFileType == "Cache") {
    cmpMtxMediaSFRList.allowMultiSelection = true;
    cmpMtxMediaSFRList.deleteRequestCounter = 0;
  }
  //translate($b/Info/Title ,'ABCDEFGHIJKLMNOPQRSTUVWXYZ','abcdefghijklmnopqrstuvwxyz')
  cmpMtxMediaSFRList.SortPathArr = { jsxtextmediaCreated: "dateCreated",
                                      jsxtextmediaName: "title",
                                      jsxtextmediaStatus: "state",
                                      jsxtextmediaSize: "size",
                                      jsxtextmediaLength: "playlength"
                                  };
  
  cmpMtxMediaSFRList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                        CountToFetch: "ObjectList_CountToFetch",
                                        SortOrder: "ObjectList_SortOrder",
                                        ObjectCount: "ObjectCount",
                                        SortByXpath: "ObjectList_SortBy",
                                        SortByMetaData: "ObjectList_SortByMetaData"
                                    };
                                                                             
  cmpMtxMediaSFRList.DataUpdatedEvent = {action:"TOPICEVENT", topic:tMedia, eventvar:"MediaStreamFileResourceDataUpdated", eventval:"LOADED"};

  var cmpBlkMediaSFRCustomize = new Component();
  var cmpBlkMediaSFRFilter =  new Component();
  
//  //***Task 35701 remove ViewOrphan media ***//  
//  if(viewFileType == "ORPHAN") {
//    cmpMtxMediaSFRList.BlkJSX.setXMLTransformers("xsl/OrphanMediaList.xsl");
//    cmpMtxMediaSFRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tMedia, "GetOrphanFile",
//                                                                      [{ VarName: "ObjectList_WhereParentNID", VarRef: "MediaSDRSelected"},
//                                                                       { VarName: "ObjectList_SelectFormat", VarRef: "SelectFormat"},
//                                                                       { VarName: "ObjectList_WherePermissions", VarRef: "WherePermissions"}
//                                                                      ],
//                                                                      { action: "FNCCALL", context: cmpMtxMediaSFRList,
//                                                                          fnc: function(respCode, respVal) {
//                                                                              if (respCode == "SUCCESS" && respVal != null) {
//                                                                                  //HACK:: strip out duplicates from list
//                                                                                  var iterFile = respVal.selectNodeIterator("./File");
//                                                                                  while (iterFile.hasNext()) {
//                                                                                      var nodeFile = iterFile.next();
//                                                                                      var fileNID = nodeFile.getAttribute("NID");
//                                                                                      var dateCreated = getDateTimeFromTimeCode(nodeFile.getAttribute("dateCreated"));
//                                                                                      nodeFile.setAttribute("dateCreated", dateCreated);
//                                                                                      // if there is a clip in a group that has the same NID - remove this from the list
//                                                                                      var nodeClip = respVal.selectSingleNode(".//FileList/File[@NID='" + fileNID + "']");
//                                                                                      if (nodeClip) {
//                                                                                          nodeFile.getParent().removeChild(nodeFile);
//                                                                                      }
//                                                                                  }
//                                                                                  this.onResponse(respCode, respVal);
//                                                                                  handleMediaSFRSelected();
//                                                                              }
//                                                                              else {
//                                                                                  log.info("GetFileRequest failed. Error Code :" + respCode);
//                                                                                  
//                                                                              }
//                                                                          }
//                                                                      }
//                                                                    );
//                                                                    
//  }
//  else
  if (viewFileType == "PVR") {
      cmpMtxMediaSFRList.BlkJSX.setXMLTransformers("xsl/PVRMediaList.xsl");
      cmpMtxMediaSFRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tMedia, "GetPVRFile",
                                                                      [{ VarName: "ObjectList_WhereParentNID", VarRef: "MediaSDRSelected" },
                                                                       { VarName: "ObjectList_SelectFormat", VarRef: "SelectFormat" },
                                                                       { VarName: "ObjectList_WherePermissions", VarRef: "WherePermissions" }
                                                                      ],
                                                                      { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                          fnc: function (respCode, respVal) {
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
                                                                                  handleMediaSFRSelected();
                                                                              }
                                                                              else {
                                                                                  log.info("GetFileRequest failed. Error Code :" + respCode);
                                                                                  this.onResponseErrorHandle("GetPVRFile", respCode, respVal);
                                                                              }
                                                                          }
                                                                      }
                                                                    );

 } else if (viewFileType == "RECYCLEBIN") {
      cmpMtxMediaSFRList.BlkJSX.setXMLTransformers("xsl/RecycleMediaList.xsl");
      cmpMtxMediaSFRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tMedia, "GetFile",
                                                                      [{ VarName: "ObjectList_WhereParentNID", VarRef: "MediaSDRSelected" },
                                                                       { VarName: "ObjectList_SelectFormat", VarRef: "SelectFormat" },
                                                                       { VarName: "ObjectList_WherePermissions", VarRef: "WherePermissions" }
                                                                      ],
                                                                      { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                          fnc: function (respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                  //HACK:: strip out duplicates from list
                                                                                  var iterFile = respVal.selectNodeIterator("./File");
                                                                                  while (iterFile.hasNext()) {
                                                                                      var nodeFile = iterFile.next();
                                                                                      var fileNID = nodeFile.getAttribute("NID");
                                                                                      // if there is a clip in a group that has the same NID - remove this from the list
                                                                                      var nodeClip = respVal.selectSingleNode(".//FileList/File[@NID='" + fileNID + "']");
                                                                                      if (nodeClip) {
                                                                                          nodeFile.getParent().removeChild(nodeFile);
                                                                                      }
                                                                                  }


                                                                                  // Hack:: calculate the deleted files size and put it in the group file size
                                                                                  var iterFile = respVal.selectNodeIterator("./File[@fileType='MediaGroup']");
                                                                                  var newSize = 0;
                                                                                  while (iterFile.hasNext()) {
                                                                                      var newSize = 0;
                                                                                      var grpFileNode = iterFile.next();
                                                                                      var deleteFileIter = grpFileNode.selectNodeIterator("./FileList/File[@state='Deleted']");
                                                                                      while (deleteFileIter.hasNext()) {
                                                                                          var fileNode = deleteFileIter.next();
                                                                                          var fileSize = fileNode.getAttribute("size");
                                                                                          newSize += parseInt(fileSize);
                                                                                      }
                                                                                      grpFileNode.setAttribute("size", newSize);
                                                                                  }

                                                                                  this.onResponse(respCode, respVal);
                                                                                  handleMediaSFRSelected();
                                                                                  var deletePer = clsUserPermission.getPermission('File', 'delete');
                                                                                  if (deletePer) {
                                                                                      var noofFiles = respVal.getAttribute("total");
                                                                                      if (!jsx3.util.strEmpty(noofFiles) && parseInt(noofFiles) > 0) {
                                                                                          topic_Set(tMedia, "EmptyMediaDirState", "ENABLE");
                                                                                      }
                                                                                      else {
                                                                                          topic_Set(tMedia, "EmptyMediaDirState", "NONE");
                                                                                      }
                                                                                      topic_Publish(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());
                                                                                  }
                                                                              }
                                                                              else {
                                                                                  log.info("GetFileRequest failed. Error Code :" + respCode);
                                                                                  this.onResponseErrorHandle("GetFile", respCode, respVal);
                                                                              }
                                                                          }
                                                                      }
                                                                    );

} else if (viewFileType == "DataDir") {
      var getMediaListXSLfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/MediaList.xsl", "MediaList", "xsl");
      var getgetMediaListXSLDoc = jsx3.xml.XslDocument.wrap(getMediaListXSLfile);
      cmpMtxMediaSFRList.BlkJSX.setXMLTransformers("xsl/MediaList.xsl");
      cmpMtxMediaSFRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tMedia, "GetFile",
                                                                          [{ VarName: "ObjectList_WhereParentNID", VarRef: "MediaSDRSelected" },
                                                                           { VarName: "ObjectList_WhereTextSubStr", VarRef: "WhereTextSubStr" },
                                                                           { VarName: "ObjectList_SelectFormat", VarRef: "SelectFormat" },
                                                                           { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "SearchTextSubStrIn" },
                                                                           { VarName: "ObjectList_WhereTagExpr", VarRef: "WhereTagExpr" },
                                                                           { VarName: "ObjectList_WherePermissions", VarRef: "WherePermissions" }
                                                                          ],
                                                                          { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                              fnc: function (respCode, respVal) {
                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                      //HACK:: strip out duplicates from list
                                                                                      var iterFile = respVal.selectNodeIterator("./File");
                                                                                      while (iterFile.hasNext()) {
                                                                                          var nodeFile = iterFile.next();
                                                                                          var fileNID = nodeFile.getAttribute("NID");
                                                                                          // if there is a clip in a group that has the same NID - remove this from the list
                                                                                          var nodeClip = respVal.selectSingleNode(".//FileList/File[@NID='" + fileNID + "']");
                                                                                          if (nodeClip) {
                                                                                              nodeFile.getParent().removeChild(nodeFile);
                                                                                          }
                                                                                      }

                                                                                      //var tempStr = '<GetFile from="1" total="5"><File bypassAutoStorageRules ="false" NID="default.mediastreamfileresourcelist.msfrl_0072e03e-ae96-465e-8176-912343ecc4d1" title="testrec" fileType="MediaGroup" streamType="" size="135100316" playlength="210189" dateCreated="1350934858043" dateModified="1350935071266" readOnly="false" mediaGroupID="47b36259-894e-4b86-915a-d3b3247a2d84.default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418.Session_C56EF3CC-40B0-0001-D23F105BADB0161E" clipID="RecFile_C56DB974-0760-0001-F859A6A01A961FD3" sessionConfigNID="default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418" state="Ready" ffTracks="1-256"><description/><Dir NID="NONE" dirTitle=""/><MetaData><Tag Name=""/></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/><Tag Name="Location" Value="RTPSunnyvale"/></Groups><FileList><File bypassAutoStorageRules ="false" NID="default.mediastreamfileresourcelist.msfrl_4257edd7-c292-4bd1-b675-e4eb31c7983b" title="xp220515095_CHANNEL1" fileType="MediaClip" streamType="V2D" size="45180083" playlength="210189" state="Ready" dateCreated="1350934858043" dateModified="1350935070687" readOnly="false" mediaGroupID="47b36259-894e-4b86-915a-d3b3247a2d84.default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418.Session_C56EF3CC-40B0-0001-D23F105BADB0161E" clipID="RecFile_C56DB974-0760-0001-F859A6A01A961FD3" sessionConfigNID="default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418" ffTracks="1-256"><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name=""/></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/><Tag Name="Location" Value="RTPSunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File><File bypassAutoStorageRules ="true" NID="default.mediastreamfileresourcelist.msfrl_871a3816-625f-4b3e-96da-bdab0ead3d71" title="xp220515094_CHANNEL1" fileType="MediaClip" streamType="V2D" size="45039606" playlength="209543" state="Ready" dateCreated="1350934858043" dateModified="1350935070430" readOnly="false" mediaGroupID="47b36259-894e-4b86-915a-d3b3247a2d84.default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418.Session_C56EF3CC-40B0-0001-D23F105BADB0161E" clipID="RecFile_C56EF2CB-2790-0001-5A6B162050B236C0" sessionConfigNID="default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418" ffTracks="1-256"><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name=""/></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/><Tag Name="Location" Value="RTPSunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File><File bypassAutoStorageRules ="true" NID="default.mediastreamfileresourcelist.msfrl_9adb8ec6-f1d3-44d5-9239-2060d25b823b" title="xp200515093_CHANNEL1" fileType="MediaClip" streamType="V2D" size="44880627" playlength="208795" state="Ready" dateCreated="1350934858043" dateModified="1350935070268" readOnly="false" mediaGroupID="47b36259-894e-4b86-915a-d3b3247a2d84.default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418.Session_C56EF3CC-40B0-0001-D23F105BADB0161E" clipID="RecFile_C56EF3A3-0630-0001-537D14FF15501DC7" sessionConfigNID="default.sessionconfiglist.scl_3560e30a-fbc2-4223-87cd-501615b6f418" ffTracks="1-256"><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name=""/></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/><Tag Name="Location" Value="RTPSunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File></FileList><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File><File bypassAutoStorageRules ="false" NID="default.mediastreamfileresourcelist.msfrl_ffa03ec7-4c2c-405f-83f0-fa306c76d9e8" title="xp220515095_CHANNEL1" fileType="MediaClip" streamType="V2D" size="63638248" playlength="435000" state="Ready" dateCreated="1.350603467594e+12" dateModified="1350604448180" readOnly="false" mediaGroupID="" clipID="50f29b66-c40f-4e8e-829d-0b36017ca193PVR" sessionConfigNID="" ffTracks=""><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name="none">none</Tag></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File><File bypassAutoStorageRules ="false" NID="default.mediastreamfileresourcelist.msfrl_bed70ef3-87e9-429f-a972-8c5a8964f5fc" title="tx515011" fileType="MediaClip" streamType="V2D" size="63639300" playlength="429000" state="Ready" dateCreated="1.350603039285e+12" dateModified="1350604454109" readOnly="false" mediaGroupID="" clipID="48a53815-23ce-4b48-bd52-e03d5bf44705PVR" sessionConfigNID="" ffTracks=""><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name="none">none</Tag></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File><File bypassAutoStorageRules ="true" NID="default.mediastreamfileresourcelist.msfrl_9b9505f1-bf38-4ae5-9c30-1df9191125b2" title="tx515012" fileType="MediaClip" streamType="V2D" size="1770697" playlength="8262" state="Ready" dateCreated="1347362629537" dateModified="1350403991069" readOnly="false" mediaGroupID="" clipID="50ee7a88-f208-45fb-a8c7-3a78d67ed6c9" sessionConfigNID="" ffTracks="1-256"><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name="none">none</Tag></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File><File  NID="default.mediastreamfileresourcelist.msfrl_b786a14e-fd25-4a9d-bc9e-48b6dcc6acbf" title="tx515015" fileType="MediaClip" streamType="V2D" size="262356548" playlength="2232000" state="Ready" dateCreated="1345984114531" dateModified="1350403991259" readOnly="false" mediaGroupID="" clipID="ee7c2869-5c37-49e2-aa62-4365d377400bPVR" sessionConfigNID="" ffTracks=""><description/><Dir NID="default.mediastreamdirresourcelist.msdrl_c50d0a2f-734b-4fe5-a2b1-8d8951337996" dirTitle="dataAnuj"/><MetaData><Tag Name="none">none</Tag></MetaData><Groups><Tag Name="Location" Value="Sunnyvale"/></Groups><Permissions delete="true" modify="true" read="true" export="true" playback="true"/></File></GetFile>';
                                                                                      //var newDoc = new jsx3.xml.Document();
                                                                                      //newDoc.loadXML(tempStr);
                                                                                      //respVal = newDoc.selectSingleNode("/*").cloneNode(true);

                                                                                      this.onResponse(respCode, respVal);
                                                                                      handleMediaSFRSelected();
                                                                                      var deletePer = clsUserPermission.getPermission('File', 'delete');
                                                                                      if (deletePer) {
                                                                                          var selRecordNodes = cmpMtxMediaSFRList.BlkJSX.getSelectedNodes();
                                                                                          if (selRecordNodes.size() == 0) {
                                                                                              var noofFiles = respVal.getAttribute("total");
                                                                                              if (!jsx3.util.strEmpty(noofFiles) && parseInt(noofFiles) > 0) {
                                                                                                  topic_Set(tMedia, "MediaSFRSelectedDeletePermission", deletePer);
                                                                                                  topic_Set(tMedia, "MediaSFRSelectedModifyPermission", "false");
                                                                                                  topic_Set(tMedia, "MediaSFRSelectedExportPermission", "false");
                                                                                                  topic_Publish(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());
                                                                                              }
                                                                                          }
                                                                                      }
                                                                                  }
                                                                                  else {
                                                                                      log.info("GetFileRequest failed. Error Code :" + respCode);
                                                                                      this.onResponseErrorHandle("GetFile", respCode, respVal);
                                                                                  }
                                                                              }
                                                                          }
                                                                        );

    cmpMtxMediaSFRList.cmpItemByNIDReqFnc = function () {
          topic_Set(tApp, "WhereMediaListNID", cmpMtxMediaSFRList.WhereNIDArr.join(","));
          return new cmpReq_GetSessionAPIPaginationList(tMedia, "GetFile",
                                                            [{ VarName: "ObjectList_WhereNID", VarRef: "WhereMediaListNID" },
                                                             { VarName: "ObjectList_WhereParentNID", VarRef: "MediaSDRSelected" },
                                                             { VarName: "ObjectList_WhereTextSubStr", VarRef: "WhereTextSubStr" },
                                                             { VarName: "ObjectList_SelectFormat", VarRef: "SelectFormat" },
                                                             { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "SearchTextSubStrIn" },
                                                             { VarName: "ObjectList_WhereTagExpr", VarRef: "WhereTagExpr" },
                                                             { VarName: "ObjectList_WherePermissions", VarRef: "WherePermissions" }
                                                            ],
                                                            { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                fnc: function (respCode, respVal) {
                                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                                        var respValNode = getgetMediaListXSLDoc.transform(respVal);
                                                                        var mediaNode = new jsx3.xml.Document();
                                                                        mediaNode.loadXML(respValNode);
                                                                        //this.onResponseUpdateRecord(respCode, mediaNode);
                                                                        var dataNode = mediaNode.selectSingleNode(".//record");
                                                                        if (!jsx3.util.strEmpty(dataNode)) {
                                                                            var jsxID = dataNode.getAttribute("jsxid");
                                                                            var orgRecordNode = this.BlkJSX.getRecordNode(jsxID);
                                                                            var isRecordSelected = orgRecordNode.getAttribute("jsxselected");
                                                                            var isRecordSelectedOpen = orgRecordNode.getAttribute("jsxopen");
                                                                            if (!jsx3.util.strEmpty(isRecordSelected)) {
                                                                                dataNode.setAttribute("jsxselected", "1");
                                                                            }
                                                                            if (!jsx3.util.strEmpty(isRecordSelectedOpen)) {
                                                                                dataNode.setAttribute("jsxopen", "1");
                                                                            }

                                                                            if (jsx3.util.strEmpty(isRecordSelected) && isRecordSelectedOpen == "1") {
                                                                                var recordNodeIterartor = orgRecordNode.selectNodeIterator(".//record");
                                                                                while (recordNodeIterartor.hasNext()) {
                                                                                    var recordNode = recordNodeIterartor.next();
                                                                                    var nodeId = recordNode.getAttribute("jsxid");
                                                                                    var isRecordSelected = recordNode.getAttribute("jsxselected");
                                                                                    if (!jsx3.util.strEmpty(isRecordSelected)) {
                                                                                        var recNode = dataNode.selectSingleNode(".//record[@jsxid='" + nodeId + "']");
                                                                                        recNode.setAttribute("jsxselected", "1");
                                                                                    }
                                                                                }
                                                                            }

                                                                            this.BlkJSX.insertRecordNode(dataNode);
                                                                        }
                                                                        handleMediaSFRSelected();
                                                                        this.BlkJSX.getParent().hideMask();
                                                                    }
                                                                    else {
                                                                        this.BlkJSX.getParent().hideMask();
                                                                        log.info("GetFileRequest failed. Error Code :" + respCode);
                                                                        this.onResponseErrorHandle("GetFile", respCode, respVal);
                                                                    }
                                                                }
                                                            }, "ItemByNID"
                                                            );
      }
      cmpMtxMediaSFRList.columnDataState = null;
      cmpMtxMediaSFRList.columnMtxDoc = new jsx3.xml.CDF.Document.newDocument();
      cmpMtxMediaSFRList.screenName = "MediaStreamFile";
      cmpMtxMediaSFRList.ColumnDataUpdatedEvent = { action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRColumnDataUpdated", eventval: "DATE" };
      cmpMtxMediaSFRList.InsertBeforeColumnName = "jsxtextmediaCreated";
      cmpMtxMediaSFRList.columnCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "MediaStreamFileTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                      fnc: function (respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                              var filterTagData = respVal.cloneDocument();
                                                                              var respValNode = getTagsXSLDoc.transform(respVal);
                                                                              var tagsNode = new jsx3.xml.Document();
                                                                              tagsNode.loadXML(respValNode);
                                                                              this.setColumnsFromDB(respCode, tagsNode);
                                                                              this.setMtxColumn();

                                                                              // use the same data for the filter block
                                                                              var filterdata = getFilterXSLDoc.transform(filterTagData);
                                                                              var filterTagNode = new jsx3.xml.Document();
                                                                              filterTagNode.loadXML(filterdata);
                                                                              this.setFilterTagDoc(respCode, filterTagNode);

                                                                              this.doRepaint();
                                                                          }
                                                                          else {
                                                                              log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetTags", respCode, respVal);
                                                                          }
                                                                      }
                                                                  }
                                                                );

      cmpMtxMediaSFRList.filterTagDataState = null;
      cmpMtxMediaSFRList.filterTagDoc = new jsx3.xml.CDF.Document.newDocument();
      cmpMtxMediaSFRList.filterTagDataUpdatedEvent = { action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRFilterTagDataUpdated", eventval: "LOADED" };
      cmpMtxMediaSFRList.filterTagCmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat" },
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "MediaStreamFileTagNameObjectType"}],
                                                                  { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                      fnc: function (respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                              var filterTagData = respVal.cloneDocument();
                                                                              var filterdata = getFilterXSLDoc.transform(filterTagData);
                                                                              var filterTagNode = new jsx3.xml.Document();
                                                                              filterTagNode.loadXML(filterdata);
                                                                              this.setFilterTagDoc(respCode, filterTagNode);
                                                                          }
                                                                          else {
                                                                              log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                              this.onResponseErrorHandle("GetTags", respCode, respVal);
                                                                          }
                                                                      }
                                                                  }
                                                                );


      var cmpMediaSFRCustomize = new cmpCustomizeContainer([blkJSX, "blkMediaSFRCustomize"],
                                                          [["blkMediaSFRCustomize", "mtxCustomize"],
                                                           ["blkMediaSFRCustomize", "btnCustomizeApply"],
                                                           ["blkMediaSFRCustomize", "btnCustomizeCancel"],
                                                           ["blkMediaSFRCustomize", "btnRefreshColumnList"],
                                                           ["blkMediaSFRCustomize", "btnCustomizeClose"],
                                                           ["blkMediaSFRCustomize", "mtxMetaDataTable"],
                                                           ["blkMediaSFRCustomize", "imgbtnAddMetaData"],
                                                           ["blkMediaSFRCustomize", "imgbtnDeleteMetaData"],
                                                           ["blkMediaSFRCustomize", "CustomizeLayout"],
                                                           ["blkMediaSFRCustomize", "blkMetaData"],
                                                           ["ShowMetaData"]
                                                          ],
                                                          cmpMtxMediaSFRList,
                                                          tMedia,
                                                          { action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRCustomize", eventval: "NONE" },
                                                          { action: "FNCCALL", context: cmpMediaSFRCustomize,
                                                              fnc: function () {
                                                                  //get the tag values and other mtx columns
                                                                  var strXMLCacheId = cmpMediaSFRCustomize.cmpMtxCustomize.BlkJSX.getXMLId();
                                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                                  var customizeDoc = cmpMediaSFRCustomize.MtxObjectList.columnMtxDoc;
                                                                  //hide any other previous metadata columns
                                                                  var recordIter = customizeDoc.selectNodeIterator(".//record[@jsxColumn='MetaData']");
                                                                  while (recordIter.hasNext()) {
                                                                      var record = recordIter.next();
                                                                      //hide from the display list
                                                                      var colName = record.getAttribute("jsxid");
                                                                      colName = colName.split(' ').join('');
                                                                      var colObj = cmpMediaSFRCustomize.MtxObjectList.BlkJSX.getDescendantOfName(colName, false, false);
                                                                      colObj.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                                      //remove from the doc
                                                                      record.getParent().removeChild(record);
                                                                  }
                                                                  //get the metadata columns name which will be cerated
                                                                  var strXMLCacheId = cmpMediaSFRCustomize.cmpMtxMetaData.BlkJSX.getXMLId();
                                                                  var metaDataObjXML = PNameSpace.getCache().getDocument(strXMLCacheId).cloneNode(true);
                                                                  var recordIter = metaDataObjXML.selectNodeIterator(".//record");
                                                                  while (recordIter.hasNext()) {
                                                                      var record = recordIter.next();
                                                                      var colName = record.getAttribute("jsxColumnName");
                                                                      if (!jsx3.util.strEmpty(colName)) {
                                                                          colName = colName.split(' ').join('');
                                                                          var recJSXId = "jsxMetaData" + colName;
                                                                          var existRec = objXML.selectSingleNode(".//record[@jsxid='" + recJSXId + "']");
                                                                          if (jsx3.util.strEmpty(existRec)) {
                                                                              record.setAttribute("jsxid", recJSXId);
                                                                              record.removeAttribute("jsxselected");
                                                                              objXML.appendChild(record);
                                                                          }
                                                                      }
                                                                  }
                                                                  cmpMediaSFRCustomize.MtxObjectList.columnMtxDoc = objXML;
                                                                  //save the columns doc in the Perferences
                                                                  setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/MediaStreamFile", objXML);
                                                                  //get the noAutoDisplayTags val if not there then add the attribute
                                                                  var noAutoDisplayTags = getLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/MediaStreamFile/@noAutoDisplayTags", null);
                                                                  if (jsx3.util.strEmpty(noAutoDisplayTags)) {
                                                                      var addSuccess = addAttributeLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSAdmin']/CustomizeList/MediaStreamFile", "noAutoDisplayTags", "false");
                                                                      log.info("Added noAutoDisplayTags for MediaStreamFile in Preferences.xml :" + addSuccess);
                                                                  }
                                                                  cmpMediaSFRCustomize.MtxObjectList.setMtxColumn();
                                                                  //cmpMediaSFRCustomize.MtxObjectList.doRepaint();
                                                                  topic_Publish(tMedia, "MediaSFRCustomize", "NONE");
                                                              }
                                                          },
                                                          [{ action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRColumnDataUpdated", eventval: null}],
                                                          null,
                                                          null
                                                         );

      cmpBlkMediaSFRCustomize = new CmpContainer(blkJSX,
                                              [{ cmp: cmpMediaSFRCustomize, sel: "topic_Get('" + tMedia + "', 'MediaSFRCustomize') != 'NONE'"}],
                                              [{ action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRCustomize", eventval: null}],
                                              null);

      var cmpMediaSFRFilter = new cmpFilterContainer([blkJSX, "blkMediaSFRFilter"],
                                                [["blkMediaSFRFilter", "blkTagOrg"],
                                                 ["blkMediaSFRFilter", "blkTagList"],
                                                 ["blkMediaSFRFilter", "txtNameSearch"],
                                                 ["blkMediaSFRFilter", "chkNameSearch"],
                                                 ["blkMediaSFRFilter", "chkDescriptionSearch"],
                                                 ["blkMediaSFRFilter", "btnFilterCancel"],
                                                 ["blkMediaSFRFilter", "btnFilterApply"],
                                                 ["blkMediaSFRFilter", "btnFilterClearAll"],
                                                 ["blkMediaSFRFilter", "btnFilterClose"],
                                                 ["blkMediaSFRFilter", "btnFilterRefresh"],
                                                 ["blkMediaSFRFilter", "btnFilterRefreshSpinner"],
                                                 ["blkMediaSFRFilter", "btnFilterRefreshNotify"],
                                                 ["blkMediaSFRFilter", "chkBookmarksSearch"],
                                                 ["blkMediaSFRFilter", "chkMetaDataSearch"]
                                                ],
                                                cmpMtxMediaSFRList,
                                                tMedia,
                                                { action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRFilter", eventval: "NONE" },
                                                { action: "FNCCALL", context: cmpMediaSFRFilter,
                                                    fnc: function () {
                                                        var SearchTextSubStrIn = "";
                                                        var MediaSFRSearchBookmark = cmpMediaSFRFilter.cmpTagList.getChkBookmark();
                                                        var MediaSFRSearchName = cmpMediaSFRFilter.cmpTagList.getChkName();
                                                        var MediaSFRSearchDescription = cmpMediaSFRFilter.cmpTagList.getChkDescription();
                                                        var MediaSFRSearchMetaData = cmpMediaSFRFilter.cmpTagList.getChkMetaData();

                                                        if (SearchTextSubStrIn == "" && MediaSFRSearchName == "1") {
                                                            SearchTextSubStrIn += "TITLE";
                                                        }
                                                        else if (SearchTextSubStrIn != "" && MediaSFRSearchName == "1") {
                                                            SearchTextSubStrIn += ",TITLE";
                                                        }
                                                        if (SearchTextSubStrIn == "" && MediaSFRSearchDescription == "1") {
                                                            SearchTextSubStrIn += "DESCRIPTION";
                                                        }
                                                        else if (SearchTextSubStrIn != "" && MediaSFRSearchDescription == "1") {
                                                            SearchTextSubStrIn += ",DESCRIPTION";
                                                        }
                                                        if (SearchTextSubStrIn == "" && MediaSFRSearchBookmark == "1") {
                                                            SearchTextSubStrIn += "BOOKMARK";
                                                        }
                                                        else if (SearchTextSubStrIn != "" && MediaSFRSearchBookmark == "1") {
                                                            SearchTextSubStrIn += ",BOOKMARK";
                                                        }
                                                        if (SearchTextSubStrIn == "" && MediaSFRSearchMetaData == "1") {
                                                            SearchTextSubStrIn += "METADATA";
                                                        }
                                                        else if (SearchTextSubStrIn != "" && MediaSFRSearchMetaData == "1") {
                                                            SearchTextSubStrIn += ",METADATA";
                                                        }

                                                        topic_Set(tMedia, "SearchTextSubStrIn", SearchTextSubStrIn);

                                                        var WhereTextSubStr = cmpMediaSFRFilter.cmpTagList.getTextSubStr();
                                                        topic_Set(tMedia, "WhereTextSubStr", WhereTextSubStr);

                                                        var WhereTagExpr = cmpMediaSFRFilter.cmpTagList.getAPITagExpr();
                                                        topic_Set(tMedia, "WhereTagExpr", WhereTagExpr);

                                                        cmpMtxMediaSFRList.ResetToStart(1, 50, 0);
                                                        cmpMediaSFRFilter.MtxObjectList.doRepaint();
                                                        // Send Audit Event for search Media
                                                        IpvsLogin.auditEvent("Searched", WhereTextSubStr);
                                                        topic_Publish(tMedia, "MediaSFRFilter", "NONE");
                                                    }
                                                },
                                                [{ action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRFilterTagDataUpdated", eventval: null}],
                                                null,
                                                null
                                               );

      cmpBlkMediaSFRFilter = new CmpContainer(blkJSX,
                                                [{ cmp: cmpMediaSFRFilter, sel: "topic_Get('" + tMedia + "', 'MediaSFRFilter') != 'NONE'"}],
                                                [{ action: "TOPICEVENT", topic: tMedia, eventvar: "MediaSFRFilter", eventval: null}],
                                                null);

      eventMedia.subscribe("MediaSFRFilter", "OPEN", this,
                      function () {
                          //cmpMediaSFRFilter.cmpBtnClearAll.clearAllSelection();
                      } // function end
                   );

  } else if (viewFileType == "Cache") {
      //Use same Matix as above  
      cmpMtxMediaSFRList.BlkJSX.setXMLTransformers("xsl/MediaList.xsl");
      cmpMtxMediaSFRList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tApp, "GetFile",
                                                                          [{ VarName: "ObjectList_WhereDownloadState", VarRef: "WhereDownloadState" },
                                                                           { VarName: "ObjectList_subscriptionID", VarRef: "subscriptionID" },
                                                                           { VarName: "ObjectList_WhereParentNID", VarRef: "MediaSDRSelected" }
                                                                          ],
                                                                          { action: "FNCCALL", context: cmpMtxMediaSFRList,
                                                                              fnc: function (respCode, respVal) {
                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                      //HACK:: strip out duplicates from list
                                                                                      var iterFile = respVal.selectNodeIterator("./File");
                                                                                      while (iterFile.hasNext()) {
                                                                                          var nodeFile = iterFile.next();
                                                                                          var fileNID = nodeFile.getAttribute("NID");
                                                                                          // if there is a clip in a group that has the same NID - remove this from the list
                                                                                          var nodeClip = respVal.selectSingleNode(".//FileList/File[@NID='" + fileNID + "']");
                                                                                          if (nodeClip) {
                                                                                              nodeFile.getParent().removeChild(nodeFile);
                                                                                          }
                                                                                      }

                                                                                      this.onResponse(respCode, respVal);
                                                                                      handleMediaSFRSelected();
                                                                                      var deletePer = clsUserPermission.getPermission('File', 'delete');
                                                                                      if (deletePer) {
                                                                                          var selRecordNodes = cmpMtxMediaSFRList.BlkJSX.getSelectedNodes();
                                                                                          if (selRecordNodes.size() == 0) {
                                                                                              var noofFiles = respVal.getAttribute("total");
                                                                                              if (!jsx3.util.strEmpty(noofFiles) && parseInt(noofFiles) > 0) {
                                                                                                  topic_Set(tMedia, "MediaSFRSelectedDeletePermission", deletePer);
                                                                                                  topic_Set(tMedia, "MediaSFRSelectedModifyPermission", "false");
                                                                                                  topic_Set(tMedia, "MediaSFRSelectedExportPermission", "false");
                                                                                                  topic_Publish(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());
                                                                                              }
                                                                                          }
                                                                                      }
                                                                                  }
                                                                                  else {
                                                                                      log.info("GetFileRequest failed for Cache files. Error Code :" + respCode);
                                                                                      this.onResponseErrorHandle("GetFile", respCode, respVal);
                                                                                  }
                                                                              }
                                                                          }
                                                                        );

 }
  cmpMtxMediaSFRList.BlkJSX.setEvent("cmpHandleToggle(this, strRECORDID, bOPEN);", jsx3.gui.Interactive.TOGGLE);

  this.cmpHandleToggle = function(objJSX, strRecordID, bOpen) {
      if( !bOpen ) { // to get back plus icon when tree collapsed
        var objRecordNode = objJSX.getRecordNode(strRecordID);
        objRecordNode.setAttribute("jsximg", "jsx:/images/tree/plus.gif");
        objJSX.redrawRecord(strRecordID, jsx3.xml.CDF.UPDATE);
      }
    }

//  //***Task 35701 remove ViewOrphan media ***// 
//  if( blkJSX == "blkMediaFilesCnt" || viewFileType == "ORPHAN" ) {
    if (blkJSX == "blkMediaFilesCnt") {

    cmpMtxMediaSFRList.MenuEvent = {action:"FNCCALL", 
                                    context:this,
                                    fnc: function() {
                                          if (topic_Get(tMedia, 'MediaSFRSelectedModifyPermission') == 'false') {
                                            return;
                                          }
                                          var fileWriteMode = topic_Get(tMedia, "FileWriteMode");
                                          //Bug#47418 disable right click options for a file which has state error 
                                          if ((topic_Get(tMedia, 'FileStateFlag') == 'ERROR' || topic_Get(tMedia, 'FileStateFlag') == 'Error') && fileWriteMode != "DISABLED") {
                                            topic_Publish(tMedia, "MediaMenuSelected", "ERROR");
                                            return;
                                          }                                      
                                          
                                          var fileName = topic_Get(tMedia, "FileName");
//  //***Task 35701 remove ViewOrphan media ***//                                           
//                                          if( fileWriteMode == "ENABLED" && fileName == "ORPHANED_MEDIA_GROUP_FILE" ) {
//                                            topic_Publish(tMedia, "MediaMenuSelected", "MediaSFROrphan");
//                                          }
//                                          else 
                                          if( fileWriteMode == "ENABLED") {
                                            topic_Publish(tMedia, "MediaMenuSelected", "MediaSFRWrite");
                                          }
                                          else if (fileWriteMode == "ENABLEDMETADATA") {
                                            topic_Publish(tMedia, "MediaMenuSelected", "MediaSFRWriteMetadata");
                                          }
                                          else {
                                            topic_Publish(tMedia, "MediaMenuSelected", "MediaSFRRead");
                                          }
                                        }
                                   }

    var cmpMenuMediaList = new CmpMenu("menuMediaList",
                                        [{ObjectType:"MediaSFRWrite",
                                          MenuListArr: ["Edit Title...", "--Edit Tags..."],
                                          DynamicListArr: ["topic_Get('"+tMedia+"','BypassAutoStorageRules')"]
                                        },
                                        { ObjectType: "MediaSFRWriteMetadata",
                                          MenuListArr: ["Edit Title...", "--Edit Tags...", "--Edit Metadata..."],
                                          DynamicListArr: ["topic_Get('" + tMedia + "','BypassAutoStorageRules')"]
                                        },
//  //***Task 35701 remove ViewOrphan media ***//                                           
//                                         {ObjectType:"MediaSFROrphan", 
//                                          MenuListArr:["Repair..."] }, 
                                         {ObjectType:"MediaSFRRead",
                                          MenuListArr:["Upgrade..."]
                                          //To Fix bug#46782
                                          //DynamicListArr: ["topic_Get('" + tMedia + "','BypassAutoStorageRules')"]
                                        }
                                        ],
                                        null, null, null,
                                        [{action:"TOPICEVENT", topic:tMedia, eventvar:"MediaMenuSelected", eventval:null}],
                                        {action:"TOPICEVENT", topic:tMedia, eventvar:"MediaMenuActionSelected", eventval:"jsxtext"}
                                      );
  }
  else {
    var cmpMenuMediaList = new Component();
  }

var cmpMediaSFRNavControls = new CmpPageNavButtonsContainer([blkJSX, "blkMediaPagination"],
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
                                                              cmpMtxMediaSFRList,
                                                              [{action:"TOPICEVENT", topic:tMedia, eventvar:"MediaStreamFileResourceDataUpdated", eventval:null}],
                                                              null
                                                             );

var cmpMediaSFRList = new CmpContainer(blkJSX,
                                      [{ cmp: cmpMediaSFRNavControls },
                                        { cmp: cmpMtxMediaSFRList },
                                        { cmp: cmpBlkEmptyStartStop },
                                        { cmp: cmpMenuMediaList },
                                        { cmp: cmpBlkMediaSFRCustomize },
                                        { cmp: cmpBlkMediaSFRFilter}
                                      ],
                                      null);
                                                                       
  /********************************* SUBSCRIPTIONS & CUSTOM FUNCTIONS **************************/                                                  
  
  // subscription to the AGENTEVNT for dir empty process finish etc. defined in cmpNotification.js 
  var AgenteventEventID = eventApp.subscribe("AGENTEVENT", null, this,
                                             function(EventVar, EventVal) {
                                               var agentEvent = EventVal;
                                               if( (agentEvent.Action == "EmptyDirProcess") && 
                                                   (agentEvent.eventCode == "1001") ) {
                                                 var DirNID = agentEvent.mediadirnid;
                                                 topic_Publish(DirNID, "EmptyDirState", "ENABLE");
                                                 if( topic_Get(tMedia, "MediaSDRSelected") == DirNID ) {
                                                   topic_Publish(tMedia, "EmptyDirState", "ENABLE");
                                                   // call repaint for the list
                                                   cmpMtxMediaSFRList.doRepaint();                                                   
                                                 }
                                               }
                                             } // function end
                                          );

  function resetMediaFileSelectedPermissions() {
    topic_Set(tMedia, "MediaSFRSelectedDeletePermission", "false");
    topic_Set(tMedia, "MediaSFRSelectedExportPermission", "false");
    topic_Set(tMedia, "MediaSFRSelectedModifyPermission", "false");
    topic_Publish(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());  
  }
    
  //when the file is selected checks whether it has write permission is true or false and stateflag
  eventMedia.subscribe("MediaSFRSelected", null, this,
                      function() {
                        resetMediaFileSelectedPermissions();
                        handleMediaSFRSelected();
                      } // function end
                   );
  function handleMediaSFRSelected() {
    var MediaFileNID = topic_Get(tMedia, "MediaSFRSelected");
    if( (MediaFileNID == "NONE") || (MediaFileNID == null) ) {
      // check if anything selected in the matrix
      var selRecordNodes = cmpMtxMediaSFRList.BlkJSX.getSelectedNodes();
      if(selRecordNodes.size() > 1){
        topic_Set(tMedia, "MediaSFRSelected", "MULTI-ROWS");
        MediaFileNID = "MULTI-ROWS";
      }
      else if(selRecordNodes.size() == 1){
        for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
          var selRecordNode = it.next();
          MediaFileNID = selRecordNode.getAttribute("jsxid");
          topic_Set(tMedia, "MediaSFRSelected", MediaFileNID);
        }        
      }
      else {
        return;
      }
    }
    //For now operations like Edit/Repair etc.. 
    //will be supported only for single file
    topic_Set(tMedia,"FileWriteMode", "NONE");
    topic_Set(tMedia,"FileStateFlag", "NONE");
    if( MediaFileNID != "MULTI-ROWS" ) {
      var selRecord = cmpMtxMediaSFRList.BlkJSX.getRecord(MediaFileNID);
      if( selRecord == null ) return;
      var fileReadOnlyMode = selRecord.jsxtextmediaReadOnly;  
      var fileStateFlag = selRecord.jsxtextmediaStateFlag;
      var fileName = selRecord.jsxtextmediaName;
      var bypass = selRecord.jsxbypassAutoStorageRules;
      var metadata = cmpMtxMediaSFRList.BlkJSX.getRecordNode(MediaFileNID).selectNodeIterator("./MetaData/Tag[@Name!='']").hasNext();
      if( fileName == "ORPHANED_MEDIA_GROUP_FILE" ){
        topic_Set(tMedia,"FileName", "ORPHANED_MEDIA_GROUP_FILE");
      }
      else {
        topic_Set(tMedia,"FileName", "NONE");
      }

      if (fileReadOnlyMode == "true") {
        topic_Set(tMedia, "FileWriteMode", "DISABLED");
      }      
      else if( fileReadOnlyMode != null && fileReadOnlyMode == "false" ) {
        topic_Set(tMedia, "FileWriteMode", "ENABLED");
        if(metadata){
          topic_Set(tMedia, "FileWriteMode", "ENABLEDMETADATA");
        }
      }
      else {
        topic_Set(tMedia,"FileWriteMode", "NONE");
      }
      
      //set the keep value
      if (bypass == "true") {
        topic_Set(tMedia, "BypassAutoStorageRules", "--Keep:../../addins/ipvsCommon/images/icons/icon_keep.png");
      }
      else {
        topic_Set(tMedia, "BypassAutoStorageRules", "--Keep");
      }

      if( fileStateFlag == "Error" ) {
        topic_Set(tMedia,"FileStateFlag", "ERROR");
      }
      else {
        topic_Set(tMedia,"FileStateFlag", fileStateFlag);
      }
      
      //set the file permissions
      var dirstateFlag = topic_Get(tApp, "MediaDirStateFlag");
      if ( jsx3.util.strEmpty(dirstateFlag) || dirstateFlag != 'Deleting') {
        var mediaFileNode = cmpMtxMediaSFRList.BlkJSX.getRecordNode(MediaFileNID);      
        var modifyPer = mediaFileNode.selectSingleNode("./Permissions/@modify").getValue();
        if( jsx3.util.strEmpty(modifyPer) ){
          modifyPer = "false";
        }
        topic_Set(tMedia, "MediaSFRSelectedModifyPermission", modifyPer);
        
        var deletePer = mediaFileNode.selectSingleNode("./Permissions/@delete").getValue();
        if( jsx3.util.strEmpty(deletePer) ){
          deletePer = "false";
        }    
        topic_Set(tMedia, "MediaSFRSelectedDeletePermission", deletePer);
        
        var exportPer = mediaFileNode.selectSingleNode("./Permissions/@export").getValue();
        if( jsx3.util.strEmpty(exportPer) ){
          exportPer = "false";
        }     
        topic_Set(tMedia, "MediaSFRSelectedExportPermission", exportPer);
      }
      else {
        topic_Set(tMedia, "MediaSFRSelectedDeletePermission", "false");
        topic_Set(tMedia, "MediaSFRSelectedExportPermission", "false"); 
        topic_Set(tMedia, "MediaSFRSelectedModifyPermission", "false");       
      }     
      
      // publish the file permissions
      topic_Publish(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());
   
    }
    // Multi-select is supported only for the export/Delete. get the export permission from clsUserPermission
    else {
      var selRecordNodes = cmpMtxMediaSFRList.BlkJSX.getSelectedNodes();
      for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
        var selRecordNode = it.next();
        var fileStatus = selRecordNode.getAttribute("jsxtextmediaStateFlag");
        if(fileStatus == "Busy"){
          //Export and Delete buttons are disabled
          topic_Set(tMedia,"FileStateFlag", fileStatus);
          break;
        }
        else if(fileStatus == "Error"){
          //Export button should be disabled and Delete button should be enabled
          topic_Set(tMedia,"FileStateFlag", fileStatus);
          break;
        }
        else{
          topic_Set(tMedia,"FileStateFlag", fileStatus);
        }
      }
      var exportPer = clsUserPermission.getPermission('File', 'export');
      topic_Set(tMedia, "MediaSFRSelectedExportPermission", exportPer);
      var deletePer = clsUserPermission.getPermission('File', 'delete');
      topic_Set(tMedia, "MediaSFRSelectedDeletePermission", deletePer);
      topic_Set(tMedia,"FileWriteMode", "ENABLED");
      
      // publish the file permissions
      topic_Publish(tMedia, "MediaSFRSelectedPermissions", (newServerDate()).getTime());      
    }   
  }
  
  eventMedia.subscribe("GetMediaFiles", null, this,
                      function () {
                        if (topic_Get(tMedia, "MediaSFRGetData") != "NONE") return;

                        topic_Set(tMedia, "MediaSFRSelected", "NONE");
                        resetMediaFileSelectedPermissions();
                        var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                        if( selectedNID == "NONE" ) return;
                        cmpMtxMediaSFRList.ResetToStart(1,50,0);                        
                        topic_Publish(tMedia, "MediaSFRGetData", (newServerDate()).getTime());
                      }
                   );

 eventMedia.subscribe("GetExportedMediaFiles", null, this,
                      function () {
                          if (topic_Get(tMedia, "MediaSFRGetData") != "NONE") return;
                          topic_Set(tMedia, "MediaSFRSelected", "NONE");
                          resetMediaFileSelectedPermissions();
                          var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                          if (selectedNID == "NONE") return;
                          cmpMtxMediaSFRList.ResetToStart(1, 50, 0);
                          topic_Publish(tMedia, "MediaSFRGetData", (newServerDate()).getTime());
                      }
                   ); 

  eventMedia.subscribe("RefreshMediaSFRList", null, this,
                      function() {
                        var selectedNID = topic_Get(tApp, "MediaSDRSelected");
                        if( selectedNID == "NONE" ) {
                          cmpMtxMediaSFRList.clearXMLData();
                          return;
                        }
                        cmpMtxMediaSFRList.doRepaint();
                      }
                   );

  eventMedia.subscribe("MediaSDRSelected", null, this,
                      function (EventVar, EventVal) {
                        if (EventVal== "NONE") {
                          cmpMtxMediaSFRList.doRepaint();
                        }
                        topic_Set(tMedia, "MediaSFRGetData", "NONE");
                      }
                   );
                                                            
  eventMedia.subscribe("EmptyDir", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == null ) return;
                        cmpBlkDirEmpty.call(this, "Stop", EventVal);          
                      }
                   ); 

  var MediaSFRUpdateEventID = eventApp.subscribe("MediaSFRDataUpdateNode", null, this,
                                                 function(EventVar, EventVal) {
                                                   var MediaFileNID = EventVal;
                                                   var mediaSFRNode = topic_GetNIDNode(tCache, MediaFileNID);
                                                   if( mediaSFRNode != null && checkDisplayMediaSFRNIDList(mediaSFRNode) == true ) {
                                                     //already checked that this files belongs to the matrix so update it
                                                     var objRecord = getObjectRecord(MediaFileNID, mediaSFRNode);
                                                     cmpMtxMediaSFRList.BlkJSX.insertRecord(objRecord, null, false);
                                                     cmpMtxMediaSFRList.BlkJSX.repaintData();
                                                   }
                                                 });
                     
  var MediaSFRAddEventID = eventApp.subscribe("MediaSFRDataAddNode", null, this,
                                               function(EventVar, EventVal) {
                                                 var MediaFileNID = EventVal;
                                                 var mediaSFRNode = topic_GetNIDNode(tCache, MediaFileNID);
                                                 if( mediaSFRNode!= null && checkDisplayMediaSFRNIDList(mediaSFRNode) == true ) {
                                                   var objRecord = getObjectRecord(MediaFileNID, mediaSFRNode);
                                                   cmpMtxMediaSFRList.BlkJSX.insertRecord(objRecord, null, false);
                                                   cmpMtxMediaSFRList.BlkJSX.repaintData();
                                                 }
                                               });
                     
  var MediaSFRDeleteEventID = eventApp.subscribe("MediaSFRDataDeleteNode", null, this,
                                                 function(EventVar, EventVal) {
                                                   var MediaFileNID = EventVal;
                                                   // no need to check node
                                                   cmpMtxMediaSFRList.BlkJSX.deleteRecord(MediaFileNID);
                                               });
// var CacheMediaSFRDeleteEventID = eventApp.subscribe("CacheMediaDataDeleteNode", null, this,
//                                                 function (EventVar, EventVal) {
//                                                     var MediaFileNID = EventVal;
//                                                     cmpMtxMediaSFRList.BlkJSX.deleteRecord(MediaFileNID);
//                                                 });

  var MediaSFRDBUpdateEventID = eventApp.subscribe("DBUpdate", null, this,
                                                     function (EventVar, EventVal) {
                                                       if (jsx3.util.strEmpty(EventVal)) return;
                                                       if (EventVal.objType == "MediaStreamFileResource") {
                                                         //Incase of MediaStream file it is tricky to find that the notification is 
                                                         //exactly for the current list or not.
                                                         //In case of Delete also UI will get replace notification as it is moved to RECYCLEBIN
                                                         //So to let user know that just marking as Dirty.
                                                         var fileNID = EventVal.nodeID;
                                                         var strXMLCacheId = cmpMtxMediaSFRList.BlkJSX.getXMLId();
                                                         var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                         if (objXML) {
                                                           var fileRecord = objXML.selectSingleNode(".//record[@jsxid='" + fileNID + "']");
                                                           if (!jsx3.util.strEmpty(fileRecord)) {
                                                             if (fileNID == topic_Get(tMedia, "MediaSFRSelected")) {
                                                               cmpMtxMediaSFRList.NotificationNIDArr.push(fileNID);
                                                               cmpMtxMediaSFRList.sendReqForNotificationNIDs();
                                                             }
                                                             else {
                                                               cmpMtxMediaSFRList.markDirty();
                                                             }
                                                           }
                                                         }
                                                       }
                                                     });

  topic_Set(tMedia, "MediaSFRUpdateEventID", MediaSFRUpdateEventID);
  topic_Set(tMedia, "MediaSFRAddEventID", MediaSFRUpdateEventID);
  topic_Set(tMedia, "MediaSFRDeleteEventID", MediaSFRUpdateEventID);
  topic_Set(tMedia, "AgenteventEventID", AgenteventEventID);
  topic_Set(tMedia, "MediaSFRDBUpdateEventID", MediaSFRDBUpdateEventID);
  

//  //***Task 35701 remove ViewOrphan media ***// 
//  if( blkJSX == "blkMediaFilesCnt" || viewFileType == "ORPHAN" ) {
  if (blkJSX == "blkMediaFilesCnt") {
    eventMedia.subscribe("MediaMenuActionSelected", null, this,
                        function () {
                          //check parent dir write permissions to allow edit/upgrade for file
                          var DirWriteMode = topic_Get(tApp, "DirWriteMode");
                          if (DirWriteMode == "NONE") {
                            alert("Operation cannot be performed on the selected File as the parent Directory is Readonly");
                            return false;
                          }
                          var MediaMenuActionSelected = topic_Get(tMedia, 'MediaMenuActionSelected');
                          this.SelectedMediaFileNID = topic_Get(tMedia, "MediaSFRSelected");
                          /* each of the functions will check if the file is in the tCache, if not then get the file                    
                          var mediaNode1 = topic_GetNode(tCache, "//*[@NID='" + this.SelectedMediaFileNID + "']");
                          if(mediaNode1 == null) {
                          alert("Please select Media");
                          return false;                        
                          }
                          */
                          if ((this.SelectedMediaFileNID == "NONE") || (this.SelectedMediaFileNID == null)) {
                            alert("Please select Media");
                            return false;
                          }

                          if (MediaMenuActionSelected == "Edit Tags...") {
                            this.SelectedMenuEdit = "Edit Tags";
                            dlgEditMediaSFRGroup_Create(this.SelectedMediaFileNID);
                          }
                          else if (MediaMenuActionSelected == "Edit Metadata...") {
                            this.SelectedMenuEdit = "Edit Metadata";
                            dlgEditMediaSFRMetaData_Create(this.SelectedMediaFileNID);
                          }
                          else if (MediaMenuActionSelected == "Edit Title...") {
                            this.SelectedMenuEdit = "Edit Title";
                            dlgEditMediaSFRTitle_Create(this.SelectedMediaFileNID);
                          }
                          else if (MediaMenuActionSelected == "Edit Bookmarks...") {
                            this.SelectedMenuEdit = "Edit Bookmarks";
                            dlgEditMediaSFRBookmarks_Create(this.SelectedMediaFileNID);
                          }
                          else if (MediaMenuActionSelected == "Upgrade...") {
                            this.SelectedMenuEdit = "Upgrade";
                            cmpBlkMediaSFRUpgrade(this.SelectedMediaFileNID);
                          }
                          else if (MediaMenuActionSelected == "Keep") {
                            this.SelectedMenuEdit = "Keep";
                            cmpBlkMediaSFRKeep(this.SelectedMediaFileNID);
                          }
                          //  //***Task 35701 remove ViewOrphan media ***//                           
                          //                          else if (MediaMenuActionSelected == "Repair...") {
                          //                            this.SelectedMenuEdit = "RepairOrphan";
                          //                            cmpBlkOrphanMediaGroupFile(this.SelectedMediaFileNID);
                          //                          }
                        }
                      );
  }


  // subscribe to the attach detach event and then refresh the files list
  eventMedia.subscribe("MediaDirAttachDetach", null, this,
                      function(EventVar, EventVal) {
                        if( EventVal == "COMPLETE" ) {
                           // call repaint for the list
                           cmpMtxMediaSFRList.doRepaint();          
                        }
                      }
                 ); 
                 
  return cmpMediaSFRList;
                   
  
  function checkDisplayMediaSFRNIDList(mediaSFRNode, MediaDirNID) {
    // no need to check whether the file belongs the selected dir.
    //pagination gets the # of files, so check whether the updated file belongs to the currently displayed files.
    //check the file nid is in the current matrix cdf if yes then return true else false
    var fileNID = mediaSFRNode.getAttribute("NID");
    var strXMLCacheId = cmpMtxMediaSFRList.BlkJSX.getXMLId();
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

  function cmpBlkDirEmpty(ReqAction, MediaSDRNID) {
    if(viewFileType == "ORPHAN") {
    }
    else {
      var reqEmptyDir = new ReqHandleDir(MediaSDRNID, null, "EmptyDir", ReqAction, 
                                         {action:"FNCCALL", context:this,
                                            fnc:function(respCode, respVal) {
                                                  if(respCode == "SUCCESS" && respVal != null) {
                                                     //var ActionResponse = respVal.selectSingleNode("//ActionResponse").getValue();
                                                     //topic_Publish(tApp, EmptyDirStateVar, ActionResponse);
                                                     // repaint of the matrix files gets called from the
                                                     // var AgenteventEventID = eventApp.subscribe("AGENTEVENT", null, this,) defined above
                                                   } else {
                                                       var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                       var ErrorMsg = "Error: Empty Dirctory request failed \n";
                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                       ErrorMsg += "Error code: " + respCode;
                                                       alert(ErrorMsg);
                                                   }
                                                }
                                         }
                                       );
      reqEmptyDir.sendRequest();
    }
  }
  
  function cmpBlkRecycleRestoreFiles(){
    //helper function to send the restore request
    function cmpBlkRestoreFile(selectedNID, selectedMetaData){
      //Send audit event for delete Media
      IpvsLogin.auditEvent("Restored", selectedMetaData);
      var reqRestoreMediaSFR = new ReqHandleMediaStreamFile(selectedNID, "RestoreMediaStreamFile",
                                                           {action:"FNCCALL", context:this,
                                                              fnc:function(respCode, respVal) {
                                                                    if(respCode == "SUCCESS" && respVal != null) {
                                                                       cmpMtxMediaSFRList.doRepaint();
                                                                     }                                                             
                                                                     else {
                                                                         var codeErrorMsg = "";
                                                                         //Using try catch to proceed even the response code is not
                                                                         //available in CommonErrorCodeArr
                                                                         try {
                                                                           codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                         }
                                                                         catch(e) {
                                                                         }                                                                     
                                                                         var ErrorMsg = "Error: Media file restore request failed \n";
                                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                         ErrorMsg += "Error code: " + respCode;
                                                                         alert(ErrorMsg);
                                                                     }
                                                                  }
                                                           }
                                                         );
      reqRestoreMediaSFR.sendRequest();    
    } // end cmpBlkRestoreFile
  
    // main code
    var selRecordNodes = cmpMtxMediaSFRList.BlkJSX.getSelectedNodes();
    if( jsx3.util.strEmpty(selRecordNodes) ){
      return;
    }
     
    for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
     var selRecordNode = it.next();
     var selectedNID = selRecordNode.getAttribute("jsxid");
     var selectedMetaData = selRecordNode.selectSingleNode("./MetaData");
     cmpBlkRestoreFile(selectedNID, selectedMetaData);
    }      
  } // end cmpBlkRecycleRestoreFiles

  function cmpBlkMediaSFRUpgrade() {    
    // check if the MediaFile is in the tCache, if not then get the file 
    
    var reqSendMediaSFRUpgrade = new ReqSessionGetRequest("UpgradeFile",
                                                             { action:"FNCCALL", context:this,
                                                               fnc:function(respCode, respVal) {
                                                                     if(respCode == "SUCCESS" && respVal != null) {
                                                                       log.info("Upgrade File NID Success : " + this.SelectedMediaFileNID); 
                                                                       //Refresh the list on upgrade 
                                                                       topic_Publish(tApp, "RefreshMediaSFRList",(newServerDate()).getTime());                                                                     
                                                                     }
                                                                     else {
                                                                       var codeErrorMsg = "";
                                                                       //Using try catch to proceed even the response code is not
                                                                       //available in CommonErrorCodeArr
                                                                       try {
                                                                         codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                       }
                                                                       catch(e) {
                                                                       }
                                                                       var ErrorMsg = "Error: Upgrade MediaStreamFile request failed \n";
                                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                       ErrorMsg += "Error code: " + respCode;
                                                                       alert(ErrorMsg);
                                                                     }
                                                                   }
                                                             }
                                                           );
    reqSendMediaSFRUpgrade.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = this.SelectedMediaFileNID;
    reqSendMediaSFRUpgrade.reqParams = reqParams;
    reqSendMediaSFRUpgrade.sendRequest(this);
  }

  function cmpBlkMediaSFRKeep() {
    var MediaFileNID = this.SelectedMediaFileNID;
    if (jsx3.util.strEmpty(MediaFileNID)) return;
    var selRecord = cmpMtxMediaSFRList.BlkJSX.getRecord(MediaFileNID);
    if (selRecord == null) return;
    var bypass = selRecord.jsxbypassAutoStorageRules;
    var setBypassValue = "false";
    if (bypass == "false" || bypass == "") {
      setBypassValue = "true";
    }
    //show the mask
    cmpMtxMediaSFRList.BlkJSX.getParent().showMask("Processing...");

    //send the request
    var reqSetResourceObject = new ReqSessionGetRequest("SetFileBypassAutoStorageRules",
                                                    { action: "FNCCALL", context: this,
                                                      fnc: function (respCode, respVal) {
                                                        if (respCode == "SUCCESS" && respVal != null) {
//                                                          var o = new Object;
//                                                          o.type = "replace";
//                                                          o.nodeID = MediaFileNID;
//                                                          cmpMtxMediaSFRList.handleNotification(o);
//                                                          cmpMtxMediaSFRList.BlkJSX.getParent().hideMask();
                                                        }
                                                        else {
                                                          cmpMtxMediaSFRList.BlkJSX.getParent().hideMask();
                                                          try {
                                                            var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                          } catch (e) {
                                                          }
                                                          var ErrorMsg = "Error: SetFileBypassAutoStorageRules request failed \n";
                                                          ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                          ErrorMsg += "Error code: " + respCode;
                                                          alert(ErrorMsg);
                                                        }
                                                      }
                                                    });

    reqSetResourceObject.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = MediaFileNID;
    reqParams["ObjectList_BypassAutoStorageRules"] = setBypassValue;

    reqSetResourceObject.reqParams = reqParams;
    reqSetResourceObject.sendRequest();

  } // end cmpBlkMediaSFRKeep
  
//  //***Task 35701 remove ViewOrphan media ***//   
//  function cmpBlkOrphanMediaGroupFile(MediaGroupID){
//    var mediaGroupFIleClipID = null;
//    var mediaFileArr = [];
//    var mediaClipFileNIDs = "";
//    
//    // helper function to update the bookmark files MediaStreamFileResourceNID = MediaGroupFile NID
//    function updateBookmarkFiles(mediaGroupFileNID){
//      var replaceMediaStreamFileResourceNode = new jsx3.xml.Document().createDocumentElement("MediaStreamFileResourceNID");
//      replaceMediaStreamFileResourceNode.setValue(mediaGroupFileNID);
//      var reqSetBookmark = new ReqSetObject( "Replace", "MediaStreamFileResource", replaceMediaStreamFileResourceNode, 
//                                             {action:"FNCCALL", context:this, 
//                                               fnc:function(respCode, respVal) {
//                                                     if(respCode == "SUCCESS" && respVal != null) {
//                                                       // refresh the list
//                                                       cmpMtxMediaSFRList.doRepaint();
//                                                     } else {
//                                                       var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
//                                                       var ErrorMsg = "Error: Bookmark Media Files update MediaStreamFileResourceNID request failed \n";
//                                                       ErrorMsg += "Error description: " + codeErrorMsg + "\n";
//                                                       ErrorMsg += "Error code: " + respCode;
//                                                       alert(ErrorMsg);
//                                                     }
//                                                   }
//                                             }                                                 
//                                           );
//      var reqParams = new Object;
//      reqParams["Op_Where_WhereXPath"] = "MediaStreamFileResourceNID=(" + mediaClipFileNIDs + ")";
//      reqParams["Op_In_ChildXPath"] = "MediaStreamFileResourceNID";
//      
//      reqSetBookmark.reqParams = reqParams;
//      reqSetBookmark.sendRequest();    
//    } // end updateBookmarkFiles
//    
//        
//    // helper functions
//    function addOrphanMediaGroupFile(mediaGroupID) {
//      var mediaFilesListNode = getObjectCacheListNode("MediaStreamFileResource");
//      var mediaFileNodes = mediaFilesListNode.selectNodes("./MediaStreamFileResource[Info/MediaStoreFileInfo/MediaGroupID='"+ mediaGroupID +"']");
//      var flag = true;
//      var mediaFileSize = 0;
//      var playLength = 0;
//      var mediaGroupFileWrite = true;
//      for (var it = mediaFileNodes.iterator();it.hasNext();){
//        var mediaFileNode = it.next();  
//        if(flag){
//          var grpTagNode = mediaFileNode.selectSingleNode(".//Info/Groups");
//          var grpFileName = mediaFileNode.selectSingleNode(".//Info/Title").getValue();
//          var owner = mediaFileNode.selectSingleNode(".//Info/Properties/Owner").getValue();
//          var dateCreated = mediaFileNode.selectSingleNode(".//Info/Properties/DateCreated").getValue();
//          var recInfnoNode = mediaFileNode.selectSingleNode(".//Info/RecInfo");
//          var mediagrpidNode = mediaFileNode.selectSingleNode(".//Info/MediaStoreFileInfo/MediaGroupID");
//         // add values to the mediafile node
//          var newGrpTagNode = newGroupMediafileNode.selectSingleNode(".//Info/Groups");
//          newGroupMediafileNode.selectSingleNode(".//Info/Groups").getParent().replaceNode(grpTagNode, newGrpTagNode);
//          newGroupMediafileNode.selectSingleNode(".//Info/Title").setValue(grpFileName);
//          newGroupMediafileNode.selectSingleNode(".//Info/Description").setValue(grpFileName);          
//          newGroupMediafileNode.selectSingleNode(".//Info/Properties/Owner").setValue(owner);
//          newGroupMediafileNode.selectSingleNode(".//Info/Properties/DateCreated").setValue(dateCreated);
//          newGroupMediafileNode.selectSingleNode(".//Info/Properties/DateModified").setValue(dateCreated); 
//          newGroupMediafileNode.selectSingleNode(".//Info/Properties/ParentMediaDirNID").setValue("");
//          var newRecInfoNode = newGroupMediafileNode.selectSingleNode(".//Info/RecInfo");
//          newGroupMediafileNode.selectSingleNode(".//Info/RecInfo").getParent().replaceNode(recInfnoNode, newRecInfoNode);
//          var newmediagroupidNode = newGroupMediafileNode.selectSingleNode(".//Info/MediaStoreFileInfo/MediaGroupID");
//          newGroupMediafileNode.selectSingleNode(".//Info/MediaStoreFileInfo/MediaGroupID").getParent().replaceNode(mediagrpidNode, newmediagroupidNode);
//          
//          flag = false;
//        }
//        mediaFileSize += parseInt( mediaFileNode.selectSingleNode(".//Info/Properties/Size").getValue() );
//        var filePlayLength = parseInt( mediaFileNode.selectSingleNode(".//MediaClip/PlayLength").getValue() );
//        if( playLength < filePlayLength ){
//          playLength = filePlayLength;
//        }
//        var fileWrite = mediaFileNode.selectSingleNode(".//Info/Properties/Permissions/Write").getValue();
//        if( (!jsx3.util.strEmpty(fileWrite)) && (fileWrite=="false") ) {
//          mediaGroupFileWrite = fileWrite;
//        }
//      }
//      newGroupMediafileNode.selectSingleNode(".//Info/Properties/Size").setValue(mediaFileSize);
//      newGroupMediafileNode.selectSingleNode(".//Info/MediaGroup/PlayLength").setValue(playLength);
//      //add the rest of the static values
//      newGroupMediafileNode.selectSingleNode(".//State/StateFlag").setValue("Ready");
//      newGroupMediafileNode.selectSingleNode(".//Info/Properties/Permissions/Read").setValue("true");
//      newGroupMediafileNode.selectSingleNode(".//Info/Properties/Permissions/Write").setValue(mediaGroupFileWrite);
//      newGroupMediafileNode.selectSingleNode(".//Info/Properties/Permissions/Xfer").setValue("true");
//      newGroupMediafileNode.selectSingleNode(".//Info/Properties/Permissions/Stream").setValue("true");      
//      newGroupMediafileNode.selectSingleNode(".//Info/MediaStoreFileInfo/Type").setValue("MediaGroup");      
//      
//      // delete MediaclipNOde
//      var mediaClipNode = newGroupMediafileNode.selectSingleNode(".//MediaClip");
//      newGroupMediafileNode.selectSingleNode(".//MediaClip").getParent().removeChild(mediaClipNode);
//      // delete PVRMediaClipNode
//      var pvrMediaNode = newGroupMediafileNode.selectSingleNode(".//PVRMediaClip");
//      newGroupMediafileNode.selectSingleNode(".//PVRMediaClip").getParent().removeChild(pvrMediaNode);
//      // delete PlaylistNode
//      var playListNode = newGroupMediafileNode.selectSingleNode(".//Playlist");
//      newGroupMediafileNode.selectSingleNode(".//Playlist").getParent().removeChild(playListNode);
//      
//      // remove the NID=''
//      newGroupMediafileNode.setAttribute("NID","");
//      // set the clipID
//      var mediaGroupIDNode = newGroupMediafileNode.selectSingleNode(".//Info/MediaStoreFileInfo/MediaGroupID");
//      mediaGroupIDNode.setAttribute("ClipID",mediaGroupFIleClipID);
//      // send the add request
//      var reqAddMediaStreamFileObject = new ReqSetObject("Add", "MediaStreamFileResource", newGroupMediafileNode, 
//                                                         {action:"FNCCALL", context:this, 
//                                                           fnc:function(respCode, respVal) {
//                                                                 if(respCode == "SUCCESS" && respVal != null) {
//                                                                   // refresh the list
//                                                                   cmpMtxMediaSFRList.doRepaint();
//                                                                   // mediaGroupFileNID to update the bookmark files
//                                                                   //var mediaGroupFileNID = respVal.selectSingleNode(".//ObjectNID").getValue();
//                                                                   //if( !jsx3.util.strEmpty(mediaGroupFileNID) ){
//                                                                   //  updateBookmarkFiles(mediaGroupFileNID);  
//                                                                   //}
//                                                                   //else{
//                                                                   //}                                                                   
//                                                                 } else {
//                                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
//                                                                   var ErrorMsg = "Error: MediaStreamFile add request failed \n";
//                                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
//                                                                   ErrorMsg += "Error code: " + respCode;
//                                                                   alert(ErrorMsg);
//                                                                 }                                                             
//                                                               }
//                                                         } 
//                                                       );
//      reqAddMediaStreamFileObject.sendRequest();      
//      
//    } // end helper addOrphanMediaGroupFile    
//    
//    // helper function check the clipID's of the media files.
//    // if not there then create the clipID and send MODIFY/REPLACE request
//    function processMediaFiles(mediaGroupID) {
//      var mediaFilesListNode = getObjectCacheListNode("MediaStreamFileResource");
//      var mediaFileNodes = mediaFilesListNode.selectNodes("./MediaStreamFileResource[Info/MediaStoreFileInfo/MediaGroupID='"+ mediaGroupID +"']");
//      //get all the file NID's and check the clipID. If not then add to array
//      for (var it = mediaFileNodes.iterator();it.hasNext();){
//        var mediaFileNode = it.next();
//        var mediaGroupIDNode = mediaFileNode.selectSingleNode(".//Info/MediaStoreFileInfo/MediaGroupID");
//        var mediaClipID = mediaGroupIDNode.getAttribute("ClipID");
//        var fileNID = mediaFileNode.getAttribute("NID");        
//        if( mediaClipFileNIDs == "") {
//          mediaClipFileNIDs = "'" + fileNID + "'";
//        }
//        else{
//          mediaClipFileNIDs += ", '" + fileNID + "'";
//        }         
//        if( !jsx3.util.strEmpty(mediaClipID) ) {
//           mediaGroupFIleClipID = mediaClipID;
//          continue;
//        }        

//        mediaFileArr.push({NID:fileNID});       
//      }
//      // update the clipID's for the mediaclips
//      for (var i=0; i<mediaFileArr.length; i++){
//        var mediaFileNode = topic_GetNIDNode(tCache, mediaFileArr[i].NID );
//        var mediaGroupIDNode = mediaFileNode.selectSingleNode(".//Info/MediaStoreFileInfo/MediaGroupID");
//        // create the new clipid
//        var newClipID = "RecFile_" + getUUID();
//        //set the value for the mediagroupfile clipID
//        if( jsx3.util.strEmpty(mediaGroupFIleClipID) ) {
//          mediaGroupFIleClipID = newClipID;
//        }
//        mediaGroupIDNode.setAttribute("ClipID", newClipID);
//        // send the add request
//        var reqAddMediaStreamFileObject = new ReqSetObject("Replace", "MediaStreamFileResource", mediaFileNode, 
//                                                           {action:"FNCCALL", context:this, 
//                                                             fnc:function(respCode, respVal) {
//                                                                   if(respCode == "SUCCESS" && respVal != null) {
//                                                                     var mediaFileNID = respVal.selectSingleNode(".//ObjectNID").getValue();
//                                                                     if( !jsx3.util.strEmpty(mediaFileNID) ) {
//                                                                       cmpMediaFileResourceHandler(mediaGroupID, mediaFileNID);
//                                                                     }
//                                                                   } else {
//                                                                     var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
//                                                                     var ErrorMsg = "Error: MediaStreamFile add request failed \n";
//                                                                     ErrorMsg += "Error description: " + codeErrorMsg + "\n";
//                                                                     ErrorMsg += "Error code: " + respCode;
//                                                                     alert(ErrorMsg);
//                                                                   }                                                             
//                                                                 }
//                                                           } 
//                                                         );
//        reqAddMediaStreamFileObject.sendRequest();      
//      }
//      // if mediaclips have the clipid in it
//      if( mediaFileArr.length == 0 ) {
//        cmpMediaFileResourceHandler(mediaGroupID, null);
//      }
//    } // end helper processMediaFiles
//    
//    function cmpMediaFileResourceHandler(mediaGroupID, mediaFileNID) {
//      for (var i=0; i<mediaFileArr.length; i++){
//        if( mediaFileArr[i].NID == mediaFileNID) {
//          mediaFileArr[i].NID = null;
//          mediaFileArr.splice(i, 1);
//          break;
//        }
//      }
//      //all the mediaclips have been updated with the clipID so now add the group file
//      if( mediaFileArr.length == 0 ) {
//        addOrphanMediaGroupFile(MediaGroupID);
//      }
//    } // end helper cmpMediaFileResourceHandler
//    
//    
//    // main code
//    var strXMLCacheId = cmpMtxMediaSFRList.BlkJSX.getXMLId();
//    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
//    var fileRecord = null;
//    if (objXML) {
//      fileRecord = objXML.selectSingleNode(".//record[@jsxmediaGroupID='"+ MediaGroupID +"']");
//    }
//    if( jsx3.util.strEmpty(fileRecord) ) {
//      alert('Cannot repair the selected MediaGroupFile');
//      return false;
//    }

//    // create new media groupfile node
//    var newGroupMediafileNode = topic_GetNIDNode(tCache, "DEFAULT_MEDIASTREAMFILE_NID").cloneNode(true);
//    if( jsx3.util.strEmpty(newGroupMediafileNode)){
//      alert("Cannot add new media group file");
//      return false;
//    }
//    // get the all the files with the selected mediagroupid
//    var mediaStreamFileWhereXPath = "[//MediaGroupID='" + MediaGroupID + "']";
//    var reqGetMediaFiles = new ReqGetObjectWhere("MediaStreamFileResource", mediaStreamFileWhereXPath,
//                                                {action:"FNCCALL", context:this, 
//                                                   fnc:function(respCode, respVal) {
//                                                         if( respCode == "SUCCESS" && respVal != null ) {
//                                                           processMediaFiles(MediaGroupID);
//                                                         } else {
//                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
//                                                           var ErrorMsg = "Error: Get Orphan MediaStreamFiles get data request failed \n";
//                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
//                                                           ErrorMsg += "Error code: " + respCode;
//                                                           alert(ErrorMsg);                                                           
//                                                         }
//                                                       }
//                                                } 
//                                             );
//    reqGetMediaFiles.sendRequest();    
//    
//  } // end cmpBlkOrphanMediaGroupFile

} // end cerate

function convertbytesToSize(element, cdfkey, matrix, column, rownumber, server) {
  element.innerHTML = bytesToSize(element.innerText);
}

function showMediaExportFormatHelp(){
  var strSpyHtmlArr = ["<div style='font-size:12px'><b>Barco MKD:</b><br />MKD is an archival format that maintains context and grouping of media.  Use this format:<br />",
  "&#8212;To package media in a format which can be reimported later or moved to another system<br />",
  "&#8212;To package all content (bookmarks, metadata, video & audio) into single files for subsequent use<br />",
  "&#8212;<i>Playback of the media and access to additional content typically requires a Barco Media Player</i><br />",
  "<br /><b>Same as Original:</b><br />Offloaded media will be kept in its original file format as much as possible (see chart below)<br />",
  "Output will be a zip file containing:<br />",
  "&#8212;The media file in the standard container format (e.g. MP4) which can be played in standard players<br />",
  "&#8212;XML files containing metadata (if applicable)<br />",
  "&#8212;<i>You will not be able to import these files back into the system</i><br />",
  "<br /><div style='margin-left:20px;'><table width='300' border='1' cellspacing='0' cellpadding='2' style='font-family:Arial, Helvetica, sans-serif;font-size:10px'>",
  "<tr><td width='143' bgcolor='#CCCCCC'>Original Format</td><td width='143' bgcolor='#CCCCCC'>Export as:</td></tr>",
  "<tr><td>V2D, UDP</td><td>Barco MKD</td></tr><tr><td>MPEG2 TS</td><td>MPEG2 TS</td></tr><tr><td>RTP (MPEG4 &amp; H264)</td>",
  "<td>MP4</td></tr><tr><td>RTP (other)</td><td>Barco MKD</td></tr><tr><td>BMP</td><td>BMP</td></tr><tr><td>GIF</td><td>GIF</td></tr>",
  "<tr><td>JPG</td><td>JPG</td></tr><tr><td>PNG</td><td>PNG</td></tr><tr><td>TIFF</td><td>TIFF</td></tr></table></div><br />",
  "<b>Export/Offload in both formats:</b><br />&#8212;Get a copy of the media in both formats<br />",
  "&#8212;Offload requires additional storage space and time to complete the process<br />",
  "&#8212;For V2D/UDP media, only a single copy will be made<br /></div>"];
  var strSpyHtml = strSpyHtmlArr.join('');
  return strSpyHtml;
}
