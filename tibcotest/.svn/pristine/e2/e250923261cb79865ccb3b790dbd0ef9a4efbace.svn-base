function cmpMediaList_Create(cmpCaller, tCaller, eventCaller, calledFrom) {
  var buttonSelection = topic_Get(cmpCaller.MediaListCallerID, "ButtonSelection");
  var liveOrRecorded = "";
  // set the tab default based on the permission
  if (clsUserPermission.getPermission('Port', 'read') == 'true') {
    if( jsx3.util.strEmpty(buttonSelection) ){
      buttonSelection = "Live";
      liveOrRecorded = "Live";
    }
    if( buttonSelection == "Live" ) {
      liveOrRecorded = "Live";
    }
    else {
      liveOrRecorded = "Destination";    
    }
  }
  else if(clsUserPermission.getPermission('File', 'read') == 'true' && clsUserPermission.getPermission('Dir', 'read') == 'true') {
    buttonSelection = "Recorded";
    liveOrRecorded = "Recorded";     
  }

  topic_Set(tCaller, "ButtonSelection", buttonSelection);
  topic_Set(tCaller, "LiveOrRecorded", liveOrRecorded);
    
  topic_Set(tCaller, "MediaSDRGetDataState", "NONE");
  topic_Set(tCaller, "UpdateDisplaySrcPortList", "NONE");
  topic_Set(tCaller, "UpdateDisplayDstPortList", "NONE");
  topic_Set(tCaller, "UpdateDisplayUserList", "NONE");


  topic_Set(tCaller, "SrcPortRefresh", "NONE");
  topic_Set(tCaller, "DstPortRefresh", "NONE");  

  topic_Set(tCaller, "SourceTagSelection", "NONE");
  topic_Set(tCaller, "DstTagSelection", "NONE");

  //topic_Set(tCaller, "SwitchMode", "NONE");

  // init selection vars
  topic_Set(tCaller, "MediaStreamSrcPortGroupSelected", "NONE");
  topic_Set(tCaller, "MediaStreamSrcPortSelected", "NONE");
  topic_Set(tCaller, "MediaDirectorySelected", "NONE");
  topic_Set(tCaller, "MediaRecordingSelected", "NONE");
  topic_Set(tCaller, "MediaStreamDstPortGroupSelected", "NONE");
  topic_Set(tCaller, "MediaStreamDstPortSelected", "NONE");
  
  topic_Set(tCaller, "TagResponseSelectFormat", "NameValueTree");
  topic_Set(tCaller, "LiveMediaPortsWherePermissions", "VIEWFROM");
  topic_Set(tCaller, "DstPortsWherePermissions", "STREAMTO");

  if (calledFrom == "CREATESESSION") {
    topic_Set(tCaller, "LiveMediaPortsSubscriptionID", "");
    topic_Set(tCaller, "DstMediaPortsSubscriptionID", "");
  }
  else {
    topic_Set(tCaller, "LiveMediaPortsSubscriptionID", "Subscription_" + getUUID());
    topic_Set(tCaller, "DstMediaPortsSubscriptionID", "Subscription_" + getUUID());
  }

  var cmpCallerBlkJSX = cmpCaller.BlkJSX;
  var cmpCallerWindow = cmpCaller.window;
  //var winMediaListWindow = parentWindow;

  // set the window customize image if any
  var cmpClientWindowHeaderLogo = new Component([cmpCallerBlkJSX, "clientWindowHeaderLogo"], null, null, null);
  cmpClientWindowHeaderLogo.customizeSkin = function() {
                                              var clientWindowHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientWindowHeaderLogo");
                                              if (!jsx3.util.strEmpty(clientWindowHeaderLogo)) {
                                                cmpClientWindowHeaderLogo.BlkJSX.setSrc(clientWindowHeaderLogo);
                                                cmpClientWindowHeaderLogo.BlkJSX.repaint();
                                              }
                                            }  
  
  //init tabs 
  var sourceTab = getJsxByName([cmpCallerBlkJSX, "blkLiveMediaButton"]);
  var recorceTab =getJsxByName([cmpCallerBlkJSX,"blkRecordedMediaButton"]);
  var destinationTab = getJsxByName([cmpCallerBlkJSX,"blkDestinationMediaButton"]);
  var userTab = getJsxByName([cmpCallerBlkJSX,"blkUserMediaButton"]);
  sourceTab.setClassName(CSSRule_tabOff);
  recorceTab.setClassName(CSSRule_tabOff);
  destinationTab.setClassName(CSSRule_tabOff);
  userTab.setClassName(CSSRule_tabOff);
   
  //---------------------  Live Media Tab Panel -----------------------------------
  // Live Media Tab Panel - TagName Selection
  var cmpSelSourceTagName = new CmpCustomPaginationList( [cmpCallerBlkJSX, "selSourceTagName"],
                                                         [{ action: "TOPICEVENT", topic: tCaller, eventvar: "ButtonSelection", eventval: "Live"}],
                                                           { persist: { col: "jsxid", initval: false },
                                                               action: "TOPICEVENT", topic: tCaller, eventvar: "TagNameSelected", eventval: "jsxid"
                                                           },
                                                         null,
                                                         null,
                                                         "xsl/TagNameList.xsl"
                                                       );

  cmpSelSourceTagName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                         CountToFetch: "ObjectList_CountToFetch",
                                         SortOrder: "ObjectList_SortOrder",
                                         ObjectCount: "ObjectCount",
                                         SortByXpath: "ObjectList_SortBy"
                                       };

 
  
  cmpSelSourceTagName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetPortTags",
                                                                      [{ VarName: "ObjectList_SelectFormat", VarRef: "TagResponseSelectFormat"},
                                                                       { VarName: "ObjectList_WherePermissions", VarRef: "LiveMediaPortsWherePermissions" }],
                                                                      { action: "FNCCALL", context: cmpSelSourceTagName,
                                                                          fnc: function(respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.onResponse(respCode, respVal);
                                                                                var defaultTagState = topic_Get(tCaller, "DefaultSourceTagState");
                                                                                if (defaultTagState == "NONE") {
                                                                                  var nodeUserEndPointControllerData = topic_GetNode(tApp, "//ServiceClientData//User//EndPointControllerData");
                                                                                  if (nodeUserEndPointControllerData) {
                                                                                    var defaultTagValue = nodeUserEndPointControllerData.selectSingleNode("//DefaultTag").getValue();
                                                                                  }
                                                                                  else {
                                                                                    var defaultTagValue = "All";
                                                                                  }
                                                                                  //var objMatrix = getJsxByName([cmpCallerBlkJSX, "selSourceTagName"]);
                                                                                  var selectedTag = setSelectTagValue(defaultTagValue, cmpSelSourceTagName.BlkJSX);
                                                                                  this.BlkJSX.setValue(selectedTag);
                                                                                  topic_Set(tCaller, "SrcPortRefresh", (newServerDate()).getTime());
                                                                                  topic_Publish(tCaller, "TagNameSelected", selectedTag);
                                                                                  topic_Set(tCaller, "DefaultSourceTagState", (newServerDate()).getTime());
                                                                                }
                                                                              }
                                                                              else {
                                                                                log.info("GetPortTagsListRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetPortTags",respCode, respVal);
                                                                              }
                                                                          }
                                                                      }
                                                                     );
                                                                      
    var cmpBtnSrcUnselectAll = new CmpButton([cmpCallerBlkJSX, "btnUnselectSrcTagName"],
                                             null,
                                             { action: "FNCCALL",
                                                 context: this,
                                                 fnc: function() {
                                                        var strXMLCacheId = cmpBlkSourceTagValues.BlkJSX.getXMLId();
                                                        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                                                        var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                                                        var i=0;
                                                        while(recordIter.hasNext()){
                                                          var recordNode = recordIter.next();
                                                          recordNode.removeAttribute("jsxselected");
                                                          cmpBlkSourceTagValues.BlkJSX.repaint();
                                                        }
                                                        topic_Publish(tCaller, "RefreshList", "Live");
                                                 }
                                             }
                                          );


  // Live Media Tab Panel - Tag Value Selection
  var cmpBlkSourceTagValues = new CmpButtonPanel([cmpCallerBlkJSX, "SrcTagValueButtonPanel"],
                                                 null,
                                                 { action: "TOPICEVENT", topic: tCaller, eventvar: "SourceTagSelection", eventval: "jsxid" }
                                                );
  cmpBlkSourceTagValues.allowMultiSelection = true;

  // On selecting Source tag filter 
  eventCaller.subscribe("SourceTagSelection", null, this,
                          function(eventVar, eventVal) {
                            var newTagExpression = "";
                            var strXMLCacheId = cmpBlkSourceTagValues.BlkJSX.getXMLId();
                            var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                            var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                            var i=0;
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              var selectedTagName = recordNode.getAttribute("jsxTagName");
                              var selectedTagValue = recordNode.getAttribute("jsxtext");
                              var newTagExpression = "Name='" + selectedTagName + "' and Value='" + selectedTagValue + "'";
                              if (i == 0) {
                                  var tagExpression = newTagExpression;
                                  i++;
                              } else {
                                  tagExpression = tagExpression + "," + newTagExpression;
                              }                                  
                            }
                            if (newTagExpression == "") {
                              // topic_Set(tCaller, "LiveMediaPortsTagExpression", "");
                              var buttonSelectionMode = topic_Get(tCaller, "ButtonSelection");
                              topic_Publish(tCaller, "RefreshList", buttonSelectionMode);
                            }
                            else topic_Set(tCaller, "LiveMediaPortsTagExpression", tagExpression);
                            topic_Publish(tCaller, "UpdateDisplaySrcPortList", (newServerDate()).getTime());
                          }
                       );                                                  

  // Live Media Tab Panel - List/Button View Selection
  var cmpBtnShowLiveList = new CmpImageButton([cmpCallerBlkJSX, "imgShowLiveList"], null,
                                                 { action: "FNCCALL", context: this,
                                                     fnc: function() {
                                                         cmpBtnShowLiveList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                                         cmpBtnShowLiveButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);                                                         
                                                         topic_Publish(tCaller, "ListOrButtonView", "List");
                                                     }
                                                 }
                                               );

  var cmpBtnShowLiveButtons = new CmpImageButton([cmpCallerBlkJSX, "imgShowLiveButtons"], null,
                                                   { action: "FNCCALL", context: this,
                                                       fnc: function() {
                                                           cmpBtnShowLiveButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                                           cmpBtnShowLiveList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);                                                           
                                                           topic_Publish(tCaller, "ListOrButtonView", "Buttons");
                                                       }
                                                     });  
                                                     
  var cmpPVRTrimLengthSetting = new CmpPVRTrimLengthContainer([cmpCallerBlkJSX,"blkPVRSessionSetup"],
                                                              [[cmpCallerBlkJSX,"chkPVREnable"],
                                                               [cmpCallerBlkJSX,"txtHrPVRTrimLength"],
                                                               [cmpCallerBlkJSX,"txtPVRTrimLength"]
                                                              ],
                                                              null,
                                                              null,
                                                              cmpCallerBlkJSX,
                                                              null
                                                              );  

  cmpPVRTrimLengthSetting.DataUpdatedEvent = { action: "TOPICEVENT", topic: tApp, eventvar: "PVRTrimLengthDataUpdated", eventval: "LOADED" };;

  // Live Media Tab Panel - Selections
  var cmpBlkLiveSelections = new CmpContainer([cmpCallerBlkJSX, "blkLiveListTags"],
                                                [{ cmp: cmpSelSourceTagName },
                                                 { cmp: cmpBtnSrcUnselectAll },
                                                 { cmp: cmpBlkSourceTagValues },
                                                 { cmp: cmpBtnShowLiveList },
                                                 { cmp: cmpBtnShowLiveButtons },
                                                 { cmp: cmpPVRTrimLengthSetting}
                                                ],
                                                null
                                                );
  // Live Media Tab Panel - List Data
  var cmpMtxMediaList = new CmpCustomPaginationList([cmpCallerBlkJSX, "mtxMediaList"],
                                                      [{ action: "TOPICEVENT", topic: tCaller, eventvar: "UpdateDisplaySrcPortList", eventval: null}],
                                                       { persist: { col: "jsxid", initval: false, reset: true },
                                                           action: "TOPICEVENT", topic: tCaller, eventvar: "MediaStreamSrcPortSelected", eventval: "jsxid"
                                                       },
                                                      null,
                                                      null                                                     
                                                     );
  cmpMtxMediaList.SortPathArr = { jsxdevpresence: "presence",
                                  jsxdevstatus: "state",
                                  jsxdevtype: "streamType",
                                  jsxtext: "title"
                              };
  
  cmpMtxMediaList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                    CountToFetch: "ObjectList_CountToFetch",
                                    SortOrder: "ObjectList_SortOrder",
                                    ObjectCount: "ObjectCount",
                                    SortByXpath: "ObjectList_SortBy"
                                  };
  cmpMtxMediaList.allowMultiSelection = true;
  cmpMtxMediaList.rowDeselection = true ;

  cmpMtxMediaList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tCaller, eventvar: "SrcPortResourceDataState", eventval: "LOADED" };
  topic_Set(tCaller, "LiveMediaPorts", "SrcPort");
  topic_Set(tCaller, "LiveMediaPortsSelectFormat", "BASICCONNECTIONINFO");
  topic_Set(tCaller, "LiveMediaPortsTagExpression", "");

  cmpMtxMediaList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetPort",
                                                                  [{ VarName: "ObjectList_Type", VarRef: "LiveMediaPorts" },
                                                                   { VarName: "ObjectList_SelectFormat", VarRef: "LiveMediaPortsSelectFormat" },
                                                                   { VarName: "ObjectList_WhereTagExpr", VarRef: "LiveMediaPortsTagExpression" },
                                                                   { VarName: "ObjectList_SubscriptionID", VarRef: "LiveMediaPortsSubscriptionID" },
                                                                   { VarName: "ObjectList_WherePermissions", VarRef: "LiveMediaPortsWherePermissions" }
                                                                   ],
                                                                  { action: "FNCCALL", context: cmpMtxMediaList,
                                                                    fnc: function(respCode, respVal) {
                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                        if (calledFrom == "CREATESESSION") {
                                                                          //set the viewfrom attribute to true
                                                                          var recordIter = respVal.selectNodeIterator(".//Port");
                                                                          while (recordIter.hasNext()) {
                                                                            var recordNode = recordIter.next();
                                                                            var portPermissionNode = recordNode.selectSingleNode(".//Permissions");
                                                                            portPermissionNode.setAttribute("viewfrom", "true");
                                                                          }
                                                                        }
                                                                        // this will return the transformed response in a doc
                                                                        var docXformResp = this.onResponse(respCode, respVal);
                                                                        //var portType = this.cmpReq.req.ObjectList_Type;
                                                                        // onResponseButtonView(respCode, respVal, cmpCaller, tCaller, portType);
                                                                        cmpBlkSources.onResponse(docXformResp);
                                                                        highlightConnPorts();
                                                                      }
                                                                      else {
                                                                        log.info("GetSrcPortListRequest failed. Error Code :" + respCode);
                                                                        this.onResponseErrorHandle("GetPort",respCode, respVal);
                                                                      }
                                                                    }
                                                                  }
                                                                );
  cmpMtxMediaList.BlkJSX.setXMLTransformers("xsl/LivePortsList.xsl");
  cmpMtxMediaList.cmpItemByNIDReqFnc = function () {
                                          var strWhereNID = cmpMtxMediaList.WhereNIDArr.join();
                                          topic_Set(tCaller, "UpdateForLiveNID", strWhereNID);
                                          return new cmpReq_GetSessionAPIPaginationList(tCaller, "GetPort",
                                                                                        [{ VarName: "ObjectList_WhereNID", VarRef: "UpdateForLiveNID" },
                                                                                        { VarName: "ObjectList_Type", VarRef: "LiveMediaPorts" },
                                                                                        { VarName: "ObjectList_SelectFormat", VarRef: "LiveMediaPortsSelectFormat" },
                                                                                        { VarName: "ObjectList_WhereTagExpr", VarRef: "LiveMediaPortsTagExpression" }
                                                                                        ],
                                                                                        { action: "FNCCALL", context: cmpMtxMediaList,
                                                                                          fnc: function (respCode, respVal) {
                                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                                              if (calledFrom == "CREATESESSION") {
                                                                                                //set the viewfrom attribute to true
                                                                                                var recordIter = respVal.selectNodeIterator(".//Port");
                                                                                                while (recordIter.hasNext()) {
                                                                                                  var recordNode = recordIter.next();
                                                                                                  var portPermissionNode = recordNode.selectSingleNode(".//Permissions");
                                                                                                  portPermissionNode.setAttribute("viewfrom", "true");
                                                                                                }
                                                                                              }                                                                                            
                                                                                              var endPointNID = topic_Get(tCaller, "EndPointSelected");
                                                                                              this.handleResponseForNotificationNIDs(respVal, "LivePortsList", "NIDArgument", endPointNID);
                                                                                              highlightConnPorts();
                                                                                            }
                                                                                            else {
                                                                                              log.info("GetSrcPort Request failed. Error Code :" + respCode);
                                                                                              this.onResponseErrorHandle("GetPort",respCode, respVal);
                                                                                            }
                                                                                          }
                                                                                        }, "ItemByNID"
                                                                                      );
                                      }

    //METHOD-- to validae the trim length
    function validatePVRTrimLength() {
      if(cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.getChecked()){
        var trimLength = cmpPVRTrimLengthSetting.validatePVRTrimLength();
        if(trimLength != false){
          setLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/PVRTrimLength",trimLength);
          setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/PVRTrimLength",trimLength);  

          topic_Set(tCaller, 'PVRTrimLength', trimLength);
        }
        else{
          return false; 
        }
      }
      return true; 
    } // end validatePVRTrimLength
    
    var PVRTrimLengthDataUpdated_ID = eventApp.subscribe("PVRTrimLengthDataUpdated", null, this,
                                                          function() {
                                                            var checkPVR = cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.getChecked();
                                                            var trimLength = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/PVRTrimLength", "3600000");
                                                            if(checkPVR){
                                                              trimLength = cmpPVRTrimLengthSetting.getEnablePVRTrimLength();
                                                              setLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/EnablePVR","true");
                                                              setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/EnablePVR","true"); 
                                                              if(trimLength != false){
                                                                setLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/PVRTrimLength",trimLength);
                                                                setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/PVRTrimLength",trimLength);                                                                  
                                                              }
                                                              topic_Set(tCaller, 'PVREnable', "true");      
                                                            }
                                                            else{
                                                              setLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/EnablePVR","false");
                                                              setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/EnablePVR","false"); 
                                                              setLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/PVRTrimLength",trimLength);
                                                              setLocalPreferences("GLOBAL", ".//ClientApp[@name='IPVSClient']/PVRTrimLength",trimLength);       
                                                              topic_Set(tCaller, 'PVREnable', "false");
                                                            }
                                                            if(trimLength != false){
                                                              topic_Set(tCaller, 'PVRTrimLength', trimLength);
                                                            }
                                                            return true; 
                                                          });

   topic_Set(tCaller, "PVRTrimLengthDataUpdated_ID", PVRTrimLengthDataUpdated_ID);

  //Live Media Tab Panel - List Data - select a live media/source
  eventCaller.subscribe("MediaStreamSrcPortSelected", null, this,
                          function(eventVar, eventVal) {
                            if (eventVal != "NONE") {
                              if (checkNIDSelection()) {
                                var validateTrimLength = true;
                                //validate the pvr trim length
                                if (calledFrom != "CREATESESSION") {
                                  validateTrimLength = validatePVRTrimLength();
                                }
                                if(validateTrimLength){
                                  var mediaInfo = getMediaInfo();                                       
                                  if (!jsx3.util.strEmpty(mediaInfo.NID)) {
					                          //cmpMtxMediaList.deSelectAfterSelect();
                                    topic_Publish(tCaller, "MediaListSelectionInfo", mediaInfo);
                                  }
                                }
                                else {
                                  cmpMtxMediaList.deSelectAfterSelect();
                                }
                              }
                            }
                          }
                        );
  
  // Live Media Tab Panel - List View
  var cmpBlkLiveList = new CmpContainer([cmpCallerBlkJSX, "blkLiveListContainer"],
                                        [{ cmp: cmpMtxMediaList}],
                                        null);

  // Live Media Tab Panel - Button View
  var cmpBlkSources = new CmpButtonPanel([cmpCallerBlkJSX, "blkSources"],
                                        null,
                                        { action: "TOPICEVENT", topic: tCaller, eventvar: "MediaStreamSrcPortSelected", eventval: "jsxid" });
  cmpBlkSources.allowMultiSelection = true;    

  var cmpBlkLiveButtons = new CmpContainer([cmpCallerBlkJSX, "blkLiveButtonsContainer"],
                                        [{ cmp: cmpBlkSources}],
                                        null);

  // Live Media Tab Panel - List Nav Controls
  var cmpBlkLiveNavControls = new CmpPageNavButtonsContainer([cmpCallerBlkJSX, "blkLiveListPaging"],
                                                            [[cmpCallerBlkJSX, "blkLive", "btnPageFirst"],
                                                             [cmpCallerBlkJSX, "blkLive", "btnPagePrev"],
                                                             [cmpCallerBlkJSX, "blkLive", "btnPageNext"],
                                                             [cmpCallerBlkJSX, "blkLive", "btnPageLast"],
                                                             [cmpCallerBlkJSX, "blkLive", "lblRecordInfo"],
                                                             [cmpCallerBlkJSX, "blkLive", "txtCurrentRecordNumber"],
                                                             [cmpCallerBlkJSX, "blkLive", "btnPageRefresh"],
                                                             [cmpCallerBlkJSX, "blkLive", "btnPageRefreshSpinner"],
                                                             [cmpCallerBlkJSX, "blkLive", "btnPageRefreshNotify"]
                                                            ],
                                                            cmpMtxMediaList,
                                                            [{ action: "TOPICEVENT", topic: tCaller, eventvar: "SrcPortResourceDataState", eventval: null}],
                                                            null
                                                           );

  //Live Media Tab Panel
  var cmpBlkLiveMediaPanel = new CmpContainer([cmpCallerBlkJSX, "blkLive"],
                                                [{ cmp: cmpBlkLiveSelections },
                                                 { cmp: cmpBlkLiveList, sel: "topic_Get('" + tCaller + "', 'ListOrButtonView') != 'Buttons' " },
                                                 { cmp: cmpBlkLiveButtons, sel: "topic_Get('" + tCaller + "', 'ListOrButtonView') == 'Buttons'" },
                                                 { cmp: cmpBlkLiveNavControls }
                                                 ],
                                                //[{ action: "TOPICEVENT", topic: tCaller, eventvar: "ListOrButtonView", eventval: null}]
                                                null
                                                );

 //---------------------  Recorded Media Tab Panel -----------------------------------
 // Recorded Media Dir Panel 
  var cmpBlkRecordingDirValues =new CmpButtonPanel([cmpCallerBlkJSX, "RecordingDirButtonPanel"],
                                                  null,
                                                  { action: "TOPICEVENT", topic: tCaller, eventvar: "MediaSDRSelected", eventval: "jsxid" });
  cmpBlkRecordingDirValues.allowMultiSelection = true;  
  
  
  var cmpBlkDir = new CmpContainer([cmpCallerBlkJSX, "blkDirs"],
                                              [{ cmp: cmpBlkRecordingDirValues }],
                                              null
                                              );
                                                
  eventCaller.subscribe("MediaSDRGetData", null, this,
                          function(eventVar, eventVal) {
                            if (eventVal != "NONE") {
                              var reqParams = new Object;
                              reqParams["ObjectList_SortOrder"] = "ascending";
                              reqParams["ObjectCount"] = "0";
                              reqParams["ObjectList_StartFrom"] = "1";
                              reqParams["ObjectList_CountToFetch"] = "";
                              reqParams["ObjectList_SortBy"] = "";
                              reqParams["ObjectList_SelectFormat"] = "BASIC";
                              reqParams["ObjectList_WherePermissions"] = "";
                              reqParams["ObjectList_WhereFilePermissions"] = "PLAYBACK";
                              reqParams["ObjectList_WhereParentDirNID"] = "NONE";
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
                                                                                        var xslUriRes = null;
                                                                                        var UriRes = cmpBlkRecordingDirValues.BlkJSX.getUriResolver();
                                                                                        if( jsx3.util.strEmpty(UriRes) ) return;
                                                                                        xslUriRes = UriRes.resolveURI("xsl/MediaDirList.xsl");
                                                                                        if( jsx3.util.strEmpty(xslUriRes) ) return;
                                                                                        var XSLFile = PNameSpace.loadInclude(xslUriRes,"_Update", "xsl");
                                                                                        var xslDoc = jsx3.xml.XslDocument.wrap(XSLFile);
                                                                                        var tagValueXML = xslDoc.transformToObject(respVal);
                                                                                        var recordNode = tagValueXML.selectSingleNode(".//record");
                                                                                        if( !jsx3.util.strEmpty(recordNode) ) {
                                                                                          var selectedID= recordNode.getAttribute("jsxid");
                                                                                          recordNode.setAttribute("jsxselected", "1");  
                                                                                          cmpBlkRecordingDirValues.onResponse(tagValueXML);
                                                                                          topic_Publish(tCaller,"MediaSDRSelected", selectedID);
                                                                                        }
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                );
                              reqRecordingDirData.reqParams = reqParams;
                              reqRecordingDirData.sendRequest();
                            }
                          }
                        );

  //Recorded Media Tab Panel - Media Dir Tree - select a media tree node
  eventCaller.subscribe("MediaSDRSelected", null, this,
                                  function(eventVar, eventVal) {
                                    cmpBlkRecordingDirValues.repaint();  
                                    sendGetFileRequest(eventVal);
                                  }
                               );
                               
  function getSelectedParentNID(){
    var dirExpression = "";
    var strXMLCacheId = cmpBlkRecordingDirValues.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
    var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
    var i=0;
    while(recordIter.hasNext()){
    var recordNode = recordIter.next();
    var selectedDirNID = recordNode.getAttribute("jsxid");
      if (i == 0) {
        dirExpression = selectedDirNID;
        i++;
      } else {
        dirExpression = dirExpression + "," + selectedDirNID;
      }                                  
    }
    if( jsx3.util.strEmpty(dirExpression) ){
      var recordNode = objXML.selectSingleNode(".//record");
      var selectedID= recordNode.getAttribute("jsxid");
      recordNode.setAttribute("jsxselected", "1");  
      cmpBlkRecordingDirValues.repaint();
      dirExpression = selectedID;
    }
    return dirExpression;
  }                             
                               
                                                           
  //Recorded Media Tab Panel - Media File List  cmpMtxRecordingList.deSelectAfterSelect();
  var cmpMtxRecordingList = new CmpCustomPaginationList([cmpCallerBlkJSX, "mtxRecordingList"],
                                                         [{ action: "TOPICEVENT", topic: tCaller, eventvar: "UpdateDisplayMediaSFRNIDList", eventval: null}],
                                                         { persist: { col: "jsxtext", initval: false, reset:true },
                                                             action: "TOPICEVENT", topic: tCaller, eventvar: "MediaSFRNIDListSelected", eventval: "jsxid"
                                                         },
                                                         [{ ColumnName: "jsxtextmediaCreated", FncName: "@datetime,MM/dd/yyyy HH:mm" },
                                                          { ColumnName: "jsxtextmediaSize", FncName: "convertbytesToSize"}],
                                                         null
                                                        );
  cmpMtxRecordingList.SortPathArr = { jsxtextmediaCreated: "dateCreated",
                                      jsxtextmediaName: "title",
                                      jsxtextmediaStatus: "state",
                                      jsxtextmediaSize: "size",
                                      jsxtextmediaLength: "playlength"
                                  };
  
  cmpMtxRecordingList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                        CountToFetch: "ObjectList_CountToFetch",
                                        SortOrder: "ObjectList_SortOrder",
                                        ObjectCount: "ObjectCount",
                                        SortByXpath: "ObjectList_SortBy"
                                    };
  
  cmpMtxRecordingList.rowDeselection = true ;
  topic_Set(tCaller, "WhereParentNID", "");
  topic_Set(tCaller, "WhereDateRange", "");
  topic_Set(tCaller, "WhereTextSubStr", "");
  topic_Set(tCaller, "SearchTextSubStrIn", "");
  topic_Set(tCaller, "WhereFileType", "MediaGroup,MediaClip");
  topic_Set(tCaller, "WherePermissions", "PLAYBACK");
  cmpMtxRecordingList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tCaller, eventvar: "MediaSFRDataState", eventval: "LOADED" };
  cmpMtxRecordingList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetFile",
                                              [{ VarName: "ObjectList_WhereParentNID", VarRef: "WhereParentNID"},
                                               { VarName: "ObjectList_WhereDateRange", VarRef: "WhereDateRange"},
                                               { VarName: "ObjectList_WhereTextSubStr", VarRef: "WhereTextSubStr" },
                                               { VarName: "ObjectList_WhereType", VarRef: "WhereFileType" },
                                               { VarName: "ObjectList_SearchTextSubStrIn", VarRef: "SearchTextSubStrIn"},
                                               { VarName: "ObjectList_WherePermissions", VarRef: "WherePermissions"}],
                                              { action: "FNCCALL", context: cmpMtxRecordingList,
                                                  fnc: function(respCode, respVal) {
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
                                                            if (calledFrom == "CREATESESSION") {
                                                              //set the viewfrom attribute to true
                                                              var recordIter = respVal.selectNodeIterator(".//File");
                                                              while (recordIter.hasNext()) {
                                                                var recordNode = recordIter.next();
                                                                var portPermissionNode = recordNode.selectSingleNode(".//Permissions");
                                                                portPermissionNode.setAttribute("playback", "true");
                                                              }
                                                            }                                                          
                                                          this.onResponse(respCode, respVal);
                                                          highlightConnPorts();
                                                      }
                                                      else {
                                                          log.info("GetSrcMediaFileListRequest failed. Error Code :" + respCode);
                                                          this.onResponseErrorHandle("GetFile",respCode, respVal);
                                                      }
                                                  }
                                              }
                                            );
  cmpMtxRecordingList.BlkJSX.setXMLTransformers("xsl/MediaList.xsl");
  cmpMtxRecordingList.BlkJSX.setEvent("cmpHandleToggle(this, strRECORDID, bOPEN);", jsx3.gui.Interactive.TOGGLE);
  cmpHandleToggle = function(objJSX, strRecordID, bOpen) {
                      if (!bOpen) { // to get back plus icon when tree collapsed
                          var objRecordNode = objJSX.getRecordNode(strRecordID);
                          objRecordNode.setAttribute("jsximg", "jsx:/images/tree/plus.gif");
                          objJSX.redrawRecord(strRecordID, jsx3.xml.CDF.UPDATE);
                      }
    }

    ///Recorded Media Tab Panel - List Data - select a recorded media/source
    eventCaller.subscribe("MediaSFRNIDListSelected", null, this,
                                  function(eventVar, eventVal) {
                                    if (eventVal != "NONE") {
                                      if (checkNIDSelection()) {
                                        var mediaInfo = getMediaInfo();
                                        // publish it to the callers topic
                                        if (!jsx3.util.strEmpty(mediaInfo.NID)) {
                                          //cmpMtxRecordingList.deSelectAfterSelect();
                                          topic_Publish(tCaller, "MediaListSelectionInfo", mediaInfo);                                         
                                        }
                                      }
                                    }
                                  }
                        );  
  topic_Set(tCaller, "SearchBy","Name");
  topic_Set(tCaller, "MediaSearch", "DISABLED");
  var cmpChkSearchName = new CmpCheckBox([cmpCallerBlkJSX, "checkBoxSearchName"], null, 
                                          { action: "FNCCALL",
                                            context: this,
                                            fnc: function() {
                                               mediaSearch();
                                            }
                                          });  
                                          
  var cmpChkSearchBookmark = new CmpCheckBox([cmpCallerBlkJSX, "checkBoxSearchBookmark"], null, 
                                              { action: "FNCCALL",
                                                context: this,
                                                fnc: function() {
                                                   mediaSearch(); 
                                                }
                                              });  
                                          
  var cmpTxtSearchMedia = new CmpTxtBoxWithEnterKey("txtSearchMedia", "Search by Name...", null, null,
                                                   { action: "FNCCALL",
                                                     context: this,
                                                     fnc: function() {
                                                        sendGetFileRequest();
                                                      }
                                                    }, null);
                                          
  var cmpBtnSearch = new CmpImageButton([cmpCallerBlkJSX, "btnSearch"], null,
                                               { action: "FNCCALL", context: this,
                                                   fnc: function() {
                                                     var checkedName = cmpChkSearchName.BlkJSX.getChecked();
                                                     var checkedBookmark = cmpChkSearchBookmark.BlkJSX.getChecked();
                                                     if(checkedName == jsx3.gui.CheckBox.CHECKED || checkedBookmark == jsx3.gui.CheckBox.CHECKED){
                                                       var searchString = cmpTxtSearchMedia.BlkJSX.getValue();
                                                       if(jsx3.util.strEmpty(searchString)){
                                                          cmpCallerWindow.alert("Please Enter a Valid Name for Search");
                                                          return false;
                                                       }
                                                     }
                                                     var checkedDate = cmpChkSearchDate.BlkJSX.getChecked();
                                                     if(checkedDate == jsx3.gui.CheckBox.CHECKED ){
                                                       var startDate =  cmpStartDatePicker.BlkJSX.getValue();
                                                       var endDate = cmpEndDatePicker.BlkJSX.getValue();
                                                       if(!jsx3.util.strEmpty(startDate) && !jsx3.util.strEmpty(endDate)){
                                                         if(startDate == "yyyy-MM-dd" || endDate == "yyyy-MM-dd" ){
                                                           cmpCallerWindow.alert("Please Enter a Valid Date");
                                                           return false;
                                                         }
                                                       }
                                                     }
                                                     sendGetFileRequest();
                                                   }
                                               }
                                             );                                        
                                           
  var cmpChkSearchDate = new CmpCheckBox([cmpCallerBlkJSX, "checkBoxSearchDate"], null, 
                                            { action: "FNCCALL",
                                              context: this,
                                              fnc: function() {
                                                 mediaSearch();
                                              }
                                            });
                                            
  var cmpBlkSearchByDate = new Component([cmpCallerBlkJSX, "blkSearchByDate"],null,null,null);
                                            
  var cmpStartDatePicker = new Component([cmpCallerBlkJSX, "startDatePicker"],null,null,null);
  
  var cmpEndDatePicker = new Component([cmpCallerBlkJSX, "endDatePicker"],null,null,null);
  
 var cmpBlkSearchByDateForm = new CmpContainer([cmpCallerBlkJSX, "blkSearchByDateForm"],
                                              [{ cmp: cmpStartDatePicker },
                                               { cmp: cmpEndDatePicker}
                                               ],
                                              null
                                              ); 
                                              
  var cmpBlkSearchMedia = new CmpContainer([cmpCallerBlkJSX, "blkSearchMedia"],
                                              [{ cmp: cmpChkSearchName },
                                               { cmp: cmpChkSearchBookmark },
                                               { cmp: cmpTxtSearchMedia},
                                               { cmp: cmpBtnSearch, sel: "topic_Get('" + tCaller + "', 'MediaSearch') != 'DISABLED'"},
                                               { cmp: cmpChkSearchDate},
                                               { cmp: cmpBlkSearchByDate, sel: "topic_Get('" + tCaller + "', 'SearchBy') == 'Name' "},
                                               { cmp: cmpBlkSearchByDateForm, sel: "topic_Get('" + tCaller + "', 'SearchBy') == 'Date' "}
                                               ],
                                              [{ action: "TOPICEVENT", topic: tCaller, eventvar: "SearchBy", eventval: null},
                                               { action: "TOPICEVENT", topic: tCaller, eventvar: "MediaSearch", eventval: null}]
                                              );                                            
                                           

  //Recorded Media Tab Panel - Media File List - Nav Controls
  var cmpBlkRecordingNavControls = new CmpPageNavButtonsContainer([cmpCallerBlkJSX, "blkRecordingsListPaging"],
                                                                  [[cmpCallerBlkJSX, "blkRecordings", "btnPageFirst"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "btnPagePrev"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "btnPageNext"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "btnPageLast"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "lblRecordInfo"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "txtCurrentRecordNumber"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "btnPageRefresh"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "btnPageRefreshSpinner"],
                                                                   [cmpCallerBlkJSX, "blkRecordings", "btnPageRefreshNotify"]
                                                                  ],
                                                                  cmpMtxRecordingList,
                                                                  [{ action: "TOPICEVENT", topic: tCaller, eventvar: "MediaSFRDataState", eventval: null}],
                                                                  null
                                                                 );

  //Recorded Media Tab Panel
  var cmpBlkRecordedMediaPanel = new CmpContainer([cmpCallerBlkJSX, "blkRecordings"],
                                                    [/*{ cmp: cmpMtxRecordingDirTree },*/
                                                     { cmp: cmpBlkDir },
                                                     { cmp: cmpMtxRecordingList },
                                                     { cmp: cmpBlkRecordingNavControls},
                                                     { cmp: cmpBlkSearchMedia}],
                                                    null
                                                 );


  //---------------------  Destination Ports Tab Panel -----------------------------------
  //Destination Ports Tab Panel - Tag Name Selection
  var cmpSelDstTagName = new CmpCustomPaginationList([cmpCallerBlkJSX, "selDstTagName"],
                                                     [{ action: "TOPICEVENT", topic: tCaller, eventvar: "ButtonSelection", eventval: "Destination"}],
                                                      { persist: { col: "jsxid", initval: false },
                                                          action: "TOPICEVENT", topic: tCaller, eventvar: "TagNameSelected", eventval: "jsxid"
                                                      },
                                                      null,
                                                      null, "xsl/TagNameList.xsl");
                                                      
  cmpSelDstTagName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                      CountToFetch: "ObjectList_CountToFetch",
                                      SortOrder: "ObjectList_SortOrder",
                                      ObjectCount: "ObjectCount",
                                      SortByXpath: "ObjectList_SortBy"
                                  };
                                  
  cmpSelDstTagName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetPortTags",
                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "TagResponseSelectFormat" },
                                                                     { VarName: "ObjectList_WherePermissions", VarRef: "DstPortsWherePermissions"}
                                                                    ],
                                                                    { action: "FNCCALL", context: cmpSelDstTagName,
                                                                        fnc: function(respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                            this.onResponse(respCode, respVal);
                                                                            var defaultTagState = topic_Get(tCaller, "DefaultDstTagState");
                                                                            if (defaultTagState == "NONE") {
                                                                              var nodeUserEndPointControllerData = topic_GetNode(tApp, "//ServiceClientData//User//EndPointControllerData");
                                                                              if (nodeUserEndPointControllerData) {
                                                                                var defaultTagValue = nodeUserEndPointControllerData.selectSingleNode("//DefaultTag").getValue();
                                                                              }
                                                                              else {
                                                                                var defaultTagValue = "All";
                                                                              }
                                                                              var selectedTag = setSelectTagValue(defaultTagValue, cmpSelDstTagName.BlkJSX);
                                                                              this.BlkJSX.setValue(selectedTag);
                                                                              topic_Set(tCaller, "DstPortRefresh", (newServerDate()).getTime());
                                                                              topic_Publish(tCaller, "TagNameSelected", selectedTag);
                                                                              topic_Set(tCaller, "DefaultDstTagState", (newServerDate()).getTime());
                                                                            }
                                                                          }
                                                                          else {
                                                                            log.info("GetPortTagsListRequest failed. Error Code :" + respCode);
                                                                            this.onResponseErrorHandle("GetPortTags",respCode, respVal);
                                                                          }
                                                                        }
                                                                    }
                                                                  );

  //Destination Ports Tab Panel - Tag Value Selection
  var cmpBtnDstUnselectAll = new CmpButton([cmpCallerBlkJSX, "btnUnselectDstTagName"],
                                           null,
                                           { action: "FNCCALL",
                                               context: this,
                                               fnc: function() {
                                                      var strXMLCacheId = cmpBlkDstTagValues.BlkJSX.getXMLId();
                                                      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                                                      var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                                                      var i=0;
                                                      while(recordIter.hasNext()){
                                                        var recordNode = recordIter.next();
                                                        recordNode.removeAttribute("jsxselected");
                                                        cmpBlkDstTagValues.BlkJSX.repaint();
                                                      }
                                                      topic_Publish(tCaller, "RefreshList", "Destination");
                                               }
                                           }
                                        );

  var cmpBlkDstTagValues = new CmpButtonPanel([cmpCallerBlkJSX, "DstTagValueButtonPanel"],
                                              null,
                                              { action: "TOPICEVENT", topic: tCaller, eventvar: "DstTagSelection", eventval: "jsxid" });
  cmpBlkDstTagValues.allowMultiSelection = true; 

  // On sleecting Destination tag filter                     
  eventCaller.subscribe("DstTagSelection", null, this,
                          function(eventVar, eventVal) {
                            var newTagExpression = "";
                            var strXMLCacheId = cmpBlkDstTagValues.BlkJSX.getXMLId();
                            var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                            var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                            var i=0;
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              var selectedTagName = recordNode.getAttribute("jsxTagName");
                              var selectedTagValue = recordNode.getAttribute("jsxtext");
                              var newTagExpression = "Name='" + selectedTagName + "' and Value='" + selectedTagValue + "'";
                              if (i == 0) {
                                  var tagExpression = newTagExpression;
                                  i++;
                              } else {
                                  tagExpression = tagExpression + "," + newTagExpression;
                              }                                  
                            }
                            if (newTagExpression == "") {
                              // topic_Set(tCaller, "LiveMediaPortsTagExpression", "");
                              var buttonSelectionMode = topic_Get(tCaller, "ButtonSelection");
                              topic_Publish(tCaller, "RefreshList", buttonSelectionMode);
                            }
                            else topic_Set(tCaller, "DstPortsTagExpression", tagExpression);
                            topic_Publish(tCaller, "UpdateDisplayDstPortList", (newServerDate()).getTime());
                          }
                         );
                                           
  //Destination Ports Tab Panel - List/Button View Selection
  var cmpBtnShowDstList = new CmpImageButton([cmpCallerBlkJSX, "imgShowDstList"], null,
                                             { action: "FNCCALL", context: this,
                                                 fnc: function() {
                                                     cmpBtnShowDstList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                                     cmpBtnShowDstButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);                                                    
                                                     topic_Publish(tCaller, "ListOrButtonView", "List"); 
                                                 }
                                             });
                                             
  var cmpBtnShowDstButtons = new CmpImageButton([cmpCallerBlkJSX, "imgShowDstButtons"], null,
                                               { action: "FNCCALL", context: this,
                                                   fnc: function() {
                                                       cmpBtnShowDstButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                                       cmpBtnShowDstList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);                                                   
                                                       topic_Publish(tCaller, "ListOrButtonView", "Buttons");
                                                   }
                                                 });
                                                 
  // Destination Port Tab Panel - All Selections
  var cmpBlkDstSelections = new CmpContainer([cmpCallerBlkJSX, "blkDstListTags"],
                                              [{ cmp: cmpSelDstTagName },
                                               { cmp: cmpBtnDstUnselectAll },
                                               { cmp: cmpBlkDstTagValues },
                                               { cmp: cmpBtnShowDstList },
                                               { cmp: cmpBtnShowDstButtons }
                                              ],
                                              null
                                              );
  
  // Destination Port Tab Panel - List Data  cmpMtxDstList.deSelectAfterSelect();
  var cmpMtxDstList = new CmpCustomPaginationList([cmpCallerBlkJSX, "mtxDstMediaList"],
                                                   [{ action: "TOPICEVENT", topic: tCaller, eventvar: "UpdateDisplayDstPortList", eventval: null}],
                                                    { persist: { col: "jsxid", initval: false, reset: true },
                                                        action: "TOPICEVENT", topic: tCaller, eventvar: "MediaStreamDstPortSelected", eventval: "jsxid"
                                                     },
                                                   null,
                                                   null                                                 
                                                 );
  cmpMtxDstList.SortPathArr = { jsxdevpresence: "presence",
                                jsxdevstatus: "state",
                                //jsxdevtype: "streamType", //Commented this line as per Preeta comments in bug#18060
                                jsxtext: "title"
                             };
                            
  cmpMtxDstList.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                  CountToFetch: "ObjectList_CountToFetch",
                                  SortOrder: "ObjectList_SortOrder",
                                  ObjectCount: "ObjectCount",
                                  SortByXpath: "ObjectList_SortBy"
                                };
    
  cmpMtxDstList.allowMultiSelection = true;
  cmpMtxDstList.rowDeselection = true ;
  cmpMtxDstList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tCaller, eventvar: "DstPortResourceDataState", eventval: "LOADED" };
  topic_Set(tCaller, "DstMediaPorts", "DstPort");
  topic_Set(tCaller, "DstMediaPortsSelectFormat", "BASICCONNECTIONINFO");
  topic_Set(tCaller, "DstPortsTagExpression", "");

  cmpMtxDstList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetPort",
                                                                [{ VarName: "ObjectList_Type", VarRef: "DstMediaPorts" },
                                                                 { VarName: "ObjectList_SelectFormat", VarRef: "DstMediaPortsSelectFormat" },
                                                                 { VarName: "ObjectList_WhereTagExpr", VarRef: "DstPortsTagExpression"},
                                                                 { VarName: "ObjectList_SubscriptionID", VarRef: "DstMediaPortsSubscriptionID"},
                                                                 { VarName: "ObjectList_WherePermissions", VarRef: "DstPortsWherePermissions"}
                                                                ],
                                                                { action: "FNCCALL", context: cmpMtxDstList,
                                                                    fnc: function(respCode, respVal) {
                                                                        if( respCode == "SUCCESS" && respVal != null ) {
                                                                          var docXformResp = this.onResponse(respCode, respVal);
//                                                                        var portType = this.cmpReq.req.ObjectList_Type;
//                                                                        onResponseButtonView(respCode, respVal, cmpCaller, tCaller, portType);
                                                                          cmpBlkDestinations.onResponse(docXformResp);    
                                                                          highlightConnPorts();                                                                        
                                                                        }
                                                                        else {
                                                                            log.info("GetDstPortListRequest failed. Error Code :" + respCode);
                                                                            this.onResponseErrorHandle("GetPort",respCode, respVal);
                                                                        }
                                                                    }
                                                                }
                                                              );
  cmpMtxDstList.BlkJSX.setXMLTransformers("xsl/DstPortsList.xsl");
  cmpMtxDstList.cmpItemByNIDReqFnc = function() {
                                       var strWhereNID = cmpMtxDstList.WhereNIDArr.join();
                                       topic_Set(tCaller, "UpdateForDstNID", strWhereNID);
                                       return new cmpReq_GetSessionAPIPaginationList(tCaller, "GetPort",
                                                                                     [{ VarName: "ObjectList_WhereNID", VarRef: "UpdateForDstNID" },
                                                                                      { VarName: "ObjectList_Type", VarRef: "DstMediaPorts" },
                                                                                      { VarName: "ObjectList_SelectFormat", VarRef: "DstMediaPortsSelectFormat" },
                                                                                      { VarName: "ObjectList_WhereTagExpr", VarRef: "DstPortsTagExpression" }
                                                                                     ],
                                                                                     { action: "FNCCALL", context: cmpMtxDstList,
                                                                                        fnc: function(respCode, respVal) {
                                                                                            if( respCode == "SUCCESS" && respVal != null ) {
                                                                                              var endPointNID = topic_Get(tCaller, "EndPointSelected");
                                                                                              this.handleResponseForNotificationNIDs(respVal, "DstPortsList", "NIDArgument", endPointNID);
                                                                                              highlightConnPorts();
                                                                                            }
                                                                                            else {
                                                                                              log.info("GetDstPort Request failed. Error Code :" + respCode);
                                                                                              this.onResponseErrorHandle("GetPort",respCode, respVal);
                                                                                            }
                                                                                        }
                                                                                     }, "ItemByNID"
                                                                                   );
                                     }

//Destinaion Port Tab Panel - List Data - select a live media/source
  eventCaller.subscribe("MediaStreamDstPortSelected", null, this,
                                  function(eventVar, eventVal) {
                                    if (eventVal != "NONE") {
                                      if (checkNIDSelection()) {
                                        var mediaInfo = getMediaInfo();
                                        if (!jsx3.util.strEmpty(mediaInfo.NID)) {
                                          //cmpMtxDstList.deSelectAfterSelect();												
                                          topic_Publish(tCaller, "MediaListSelectionInfo", mediaInfo);                                                
                                        }
                                      }
                                    }
                                  }
                        );
  
  // Destination Port Tab Panel - List View
  var cmpBlkDstList = new CmpContainer([cmpCallerBlkJSX, "blkDstListContainer"],
                                        [{ cmp: cmpMtxDstList}],
                                        null);

  // Destination Port Tab Panel - Button View
  var cmpBlkDestinations = new CmpButtonPanel([cmpCallerBlkJSX, "blkDestinations"],
                                            null,
                                            { action: "TOPICEVENT", topic: tCaller, eventvar: "MediaStreamDstPortSelected", eventval: "jsxid" });
  cmpBlkDestinations.allowMultiSelection = true;   
                                                                                   
  var cmpBlkDstButtons = new CmpContainer([cmpCallerBlkJSX, "blkDstButtonsContainer"],
                                            [{ cmp: cmpBlkDestinations}],
                                            null);

  // Destination Port Tab Panel - Nav Controls
  var cmpDstPortNavControls = new CmpPageNavButtonsContainer([cmpCallerBlkJSX, "blkDstListPaging"],
                                                              [[cmpCallerBlkJSX, "blkDst", "btnPageFirst"],
                                                               [cmpCallerBlkJSX, "blkDst", "btnPagePrev"],
                                                               [cmpCallerBlkJSX, "blkDst", "btnPageNext"],
                                                               [cmpCallerBlkJSX, "blkDst", "btnPageLast"],
                                                               [cmpCallerBlkJSX, "blkDst", "lblRecordInfo"],
                                                               [cmpCallerBlkJSX, "blkDst", "txtCurrentRecordNumber"],
                                                               [cmpCallerBlkJSX, "blkDst", "btnPageRefresh"],
                                                               [cmpCallerBlkJSX, "blkDst", "btnPageRefreshSpinner"],
                                                               [cmpCallerBlkJSX, "blkDst", "btnPageRefreshNotify"]
                                                              ],
                                                              cmpMtxDstList,
                                                              [{ action: "TOPICEVENT", topic: tCaller, eventvar: "DstPortResourceDataState", eventval: null}],
                                                              null
                                                             );

  // Destination Port Tab Panel
  var cmpBlkDestMediaPanel = new CmpContainer([cmpCallerBlkJSX, "blkDst"],
                                                [{ cmp: cmpBlkDstSelections },
                                                 { cmp: cmpBlkDstList, sel: "topic_Get('" + tCaller + "', 'ListOrButtonView') != 'Buttons' "},
                                                 { cmp: cmpBlkDstButtons, sel: "topic_Get('" + tCaller + "', 'ListOrButtonView') == 'Buttons'  " },
                                                 { cmp: cmpDstPortNavControls }
                                                 ],
                                                //[{ action: "TOPICEVENT", topic: tCaller, eventvar: "ListOrButtonView", eventval: null}]
                                                null
                                                );


 //---------------------  Users Tab Panel -----------------------------------
  // Users
  var cmpMtxUserList = new CmpCustomPaginationList([cmpCallerBlkJSX, "mtxUserMediaList"],
                                                     [{ action: "TOPICEVENT", topic: tCaller, eventvar: "UpdateDisplayUserList", eventval: null}],
                                                     { persist: { col: "jsxid", initval: false },
                                                         action: "TOPICEVENT", topic: tCaller, eventvar: "MediaUserSelected", eventval: "jsxid"
                                                     },
                                                     null,
                                                     null,
                                                     "xsl/UserList.xsl"
                                                   );

  cmpMtxUserList.SortPathArr = {jsxUserPresence: "presence",
                                jsxtext: "title"
                               };

  cmpMtxUserList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
      CountToFetch: "ObjectList_CountToFetch",
      SortOrder: "ObjectList_SortOrder",
      ObjectCount: "ObjectCount",
      SortByXpath: "ObjectList_SortBy"
  };

  cmpMtxUserList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tCaller, eventvar: "UserDataState", eventval: "LOADED" };

  topic_Set(tCaller, "UserSelectFormat", "EXTENDED");
  cmpMtxUserList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tCaller, "GetUser",
                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "UserSelectFormat"}],
                                                                    { action: "FNCCALL", context: cmpMtxUserList,
                                                                        fnc: function(respCode, respVal) {
                                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                                                this.onResponse(respCode, respVal);

                                                                            }
                                                                            else {
                                                                                log.info("GetUserListRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetUser",respCode, respVal);
                                                                            }
                                                                        }
                                                                    }
                                                                  );
  // TBR after MW implements the request
  cmpMtxUserList.cmpReq.SimulateResponse = true;
  cmpMtxUserList.cmpReq.SimulateResponseXML = '<iq id="Request_jsx_1h" to="anujpc12@localhost/anujpc12" from="srcrelay1@localhost/srcrelay1" type="result"><query xmlns="com.ipvs.session" serviceVer="2.9"><GetUser from="0" total="50"><User NID="default.userlist.ul_3d23ccf6-1bfe-4e77-9014-04359e3598b5" title="adminuser" jid="" presence=""><description /><Groups><Tag Name="Users" Value="Admin" /></Groups></User><User NID="default.userlist.ul_1582dd26-82c2-46fd-afe9-83d6b50c448a" title="anujv2dxp200" jid="" presence=""><description /><Groups><Tag Name="_RESERVED_" Value="DEVICE_BOT_USERS" /></Groups></User><User NID="default.userlist.ul_5c01f7a7-1b84-4b46-8c76-a27c7193efda" title="drcuser1" jid="" presence=""><description /><Groups><Tag Name="Location" Value="Hyderabad" /></Groups></User><User NID="default.userlist.ul_796901dc-9dde-48a8-a49c-6fcfa67943b9" title="anujpc13" jid="" presence=""><description /><Groups><Tag Name="Location" Value="Sunnyvale" /></Groups></User><User NID="default.userlist.ul_56aaf2fa-50d3-4b21-a54a-290f6a87a548" title="anujtestv2rx" jid="" presence=""><description /><Groups><Tag Name="_RESERVED_" Value="DEVICE_BOT_USERS" /></Groups></User></GetUser></query></iq>';

  var cmpUserNavControls = new CmpPageNavButtonsContainer([cmpCallerBlkJSX, "blkUserListPaging"],
                                                            [[cmpCallerBlkJSX, "blkUser", "btnPageFirst"],
                                                             [cmpCallerBlkJSX, "blkUser", "btnPagePrev"],
                                                             [cmpCallerBlkJSX, "blkUser", "btnPageNext"],
                                                             [cmpCallerBlkJSX, "blkUser", "btnPageLast"],
                                                             [cmpCallerBlkJSX, "blkUser", "lblRecordInfo"],
                                                             [cmpCallerBlkJSX, "blkUser", "txtCurrentRecordNumber"],
                                                             [cmpCallerBlkJSX, "blkUser", "btnPageRefresh"],
                                                             [cmpCallerBlkJSX, "blkUser", "btnPageRefreshSpinner"]
                                                            ],
                                                            cmpMtxUserList,
                                                            [{ action: "TOPICEVENT", topic: tCaller, eventvar: "UserResourceDataState", eventval: null}],
                                                            null
                                                           );

  // User Tab Panel
  var cmpBlkUserMediaPanel = new CmpContainer([cmpCallerBlkJSX, "blkUser"],
                                                [{ cmp: cmpMtxUserList },
                                                 { cmp: cmpUserNavControls}],
                                                null);

  //---------------------  Tab Panels -----------------------------------                                           
  var cmpBlkTabPanels = new CmpContainer([cmpCallerBlkJSX, "blkTabPanels"],
                                           [{ cmp: cmpBlkLiveMediaPanel,
                                               sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'Live'"},
                                            { cmp: cmpBlkRecordedMediaPanel,
                                              sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'Recorded'"},
                                            { cmp: cmpBlkDestMediaPanel,
                                              sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'Destination'"},
                                            { cmp: cmpBlkUserMediaPanel,
                                                sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'User'"}],
                                           [{ action: "TOPICEVENT", topic: tCaller, eventvar: "ButtonSelection", eventval: null},
                                            { action: "TOPICEVENT", topic: tCaller, eventvar: "ListOrButtonView", eventval: null}]
                                          );


  //---------------------  Tab Buttons -----------------------------------
 
  
  
  var cmpBtnLiveMedia = new CmpBlockButton([cmpCallerBlkJSX, "blkLiveMediaButton"], null,
                                             { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function() { 
                                                     topic_Set(tCaller, "LiveOrRecorded", "Live");                                                  
                                                     topic_Publish(tCaller, "ButtonSelection", "Live");
                                                 }
                                             }, CSSRule_tabOn, CSSRule_tabOff);

  var cmpBlkLiveMedia = new CmpContainer([cmpCallerBlkJSX, "blkLiveMediaButton"],
                                          [{ cmp: cmpBtnLiveMedia,
                                              sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'Live'"}],
                                          null);
                                      
  var cmpBtnRecordedMedia = new CmpBlockButton([cmpCallerBlkJSX, "blkRecordedMediaButton"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() { 
                                                         topic_Set(tCaller, "LiveOrRecorded", "Recorded");                                                        
                                                         topic_Publish(tCaller, "ButtonSelection", "Recorded");
                                                     }
                                                 }, CSSRule_tabOn, CSSRule_tabOff);

  var cmpBlkRecordedMedia = new CmpContainer([cmpCallerBlkJSX, "blkRecordedMediaButton"],
                                            [{ cmp: cmpBtnRecordedMedia,
                                                sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'Recorded'"}],
                                            null);
                                          
  var cmpBtnDestMedia = new CmpBlockButton([cmpCallerBlkJSX, "blkDestinationMediaButton"], null,
                                             { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function() {
                                                     topic_Publish(tCaller, "ButtonSelection", "Destination");
                                                 }
                                             }, CSSRule_tabOn, CSSRule_tabOff);

  var cmpBlkDestMedia = new CmpContainer([cmpCallerBlkJSX, "blkDestinationMediaButton"],
                                            [{ cmp: cmpBtnDestMedia,
                                                sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'Destination'"}],
                                            null);
                                            
  var cmpBtnUserMedia = new CmpBlockButton([cmpCallerBlkJSX, "blkUserMediaButton"], null,
                                             { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function() {
                                                     topic_Publish(tCaller, "ButtonSelection", "User");
                                                 }
                                               }, CSSRule_tabOn, CSSRule_tabOff);
                                               
  var cmpBlkTabButtons = new CmpContainer([cmpCallerBlkJSX, "blkTabButtonContainer"],
                                             [{ cmp: cmpBlkLiveMedia,
                                                sel: "(clsUserPermission.getPermission('Port', 'read') == 'true')"},
                                              { cmp: cmpBlkRecordedMedia,
                                                sel:"(clsUserPermission.getPermission('File', 'read') == 'true' && clsUserPermission.getPermission('Dir', 'read') == 'true')"},
                                              { cmp: cmpBlkDestMedia,
                                                sel: "(clsUserPermission.getPermission('Port', 'read') == 'true')"}//,
                                               // { cmp: cmpBtnUserMedia,
                                               //   sel: "topic_Get('" + tCaller + "', 'ButtonSelection') == 'User'" }
                                             ],
                                             [{ action: "TOPICEVENT", topic: tCaller, eventvar: "ButtonSelection", eventval: null}]
                                           );

                                                 
  // Tab Button Selection event handling
  eventCaller.subscribe("ButtonSelection", null, this,
                        function (eventVar, eventVal) {
                          if (eventVal == 'Recorded') {
                            if (tCaller != "MediaList") {
                              cmpBtnLiveMedia.enable = true
                              cmpBtnDestMedia.enable = false;
                              cmpBtnRecordedMedia.enable = true;
                            }
                            var getDataState = topic_Get(tCaller, "MediaSDRGetDataState");
                            if (getDataState == "NONE") {
                              topic_Set(tCaller, "MediaSDRGetDataState", (newServerDate()).getTime());
                              //cmpMtxRecordingDirTree.ResetToStart(1, "", 0);
                              topic_Publish(tCaller, "MediaSDRGetData", (newServerDate()).getTime());
                            }
                            highlightConnPorts();                            
                          }
                          else if (eventVal == 'Live') {
                            if (tCaller != "MediaList") {
                              cmpBtnLiveMedia.enable = true;
                              cmpBtnDestMedia.enable = false;
                              cmpBtnRecordedMedia.enable = true;
                            }
                            var getDataState = topic_Get(tCaller, "UpdateDisplaySrcPortList");
                            if (getDataState == "NONE") {
                              topic_Set(tCaller, "DefaultSourceTagState", "NONE");
                              cmpMtxMediaList.ResetToStart(1, 25, 0);

                              //topic_Publish(tCaller, "UpdateDisplaySrcPortList", (newServerDate()).getTime());
                              var enablePVR = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/EnablePVR", "false");
                              var trimLength = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/PVRTrimLength", "3600000");
                              if( enablePVR == "true" ){ 
                                topic_Set(tCaller, 'PVREnable', "true");
                              }
                              else {
                                topic_Set(tCaller, 'PVREnable', "false");                           
                              }
                              cmpPVRTrimLengthSetting.setPVREnable(enablePVR);
                              cmpPVRTrimLengthSetting.setPVRTrimLengthValues(trimLength); 
                              
                              topic_Set(tCaller, 'PVRTrimLength', trimLength);   
                              
                              if (calledFrom == "CREATESESSION") {
                                cmpPVRTrimLengthSetting.getDisablePVRTrimLength();
                              }                                                                                      
                            }
                            else {
                              var displayMode = topic_Get(tCaller, "ListOrButtonView");
                              if (displayMode == 'Buttons') {
                                cmpBtnShowLiveButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                cmpBtnShowLiveList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                cmpBlkSources.BlkJSX.repaint(); 
                              }
                              else {
                                cmpBtnShowLiveList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                cmpBtnShowLiveButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                cmpMtxMediaList.BlkJSX.repaint();
                              }
                              topic_Publish(tCaller, "ListOrButtonView", displayMode);
                            }
                            highlightConnPorts();
                          }
                          else if (eventVal == 'Destination') {
                            if (tCaller != "MediaList") {
                              cmpBtnLiveMedia.enable = false;
                              cmpBtnDestMedia.enable = true;
                              cmpBtnRecordedMedia.enable = false;
                            }
                            var getDataState = topic_Get(tCaller, "UpdateDisplayDstPortList");
                            if (getDataState == "NONE") {
                              topic_Set(tCaller, "DefaultDstTagState", "NONE");
                              cmpMtxDstList.ResetToStart(1, 25, 0);
                              //topic_Publish(tCaller, "UpdateDisplayDstPortList", (newServerDate()).getTime());
                            }
                            else {
                              var displayMode = topic_Get(tCaller, "ListOrButtonView");
                              if (displayMode == 'Buttons') {
                                cmpBtnShowDstButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                cmpBtnShowDstList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                cmpBlkDestinations.BlkJSX.repaint();
                              }
                              else {
                                cmpBtnShowDstList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                cmpBtnShowDstButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                cmpMtxDstList.BlkJSX.repaint();
                              }
                              topic_Publish(tCaller, "ListOrButtonView", displayMode);
                            }
                            highlightConnPorts();
                          }
                          else if (eventVal == 'User') {
                            var getDataState = topic_Get(tCaller, "UpdateDisplayUserList");
                            if (getDataState == "NONE") {
                              cmpMtxUserList.ResetToStart(1, 25, 0);
                              topic_Publish(tCaller, "UpdateDisplayUserList", (newServerDate()).getTime());
                            }
                          }
                        }
                     );

    var DBUpdate_2_ID = eventApp.subscribe("DBUpdate", null, this,
                                  function(EventVar, EventVal) {
                                    if( jsx3.util.strEmpty(EventVal) ) return;
                                    if( EventVal.objType == "MediaStreamSrcPortResource" ) {
                                      cmpMtxMediaList.handleNotification(EventVal);
                                    }
                                    else if( EventVal.objType == "MediaStreamDstPortResource") {
                                      cmpMtxDstList.handleNotification(EventVal);
                                    }
                                    // incase for the IO Port we dont know its SRc or Dst so updating the both
                                    // will be fixed in the notification when notication fetaure gets implemented;
                                    else if (EventVal.objType == "MediaStreamIOPortResource") {
                                      cmpMtxDstList.handleNotification(EventVal);
                                      cmpMtxMediaList.handleNotification(EventVal);
                                    }                                                        
                                    else if( EventVal.objType == "MediaStreamFileResource" ) {
                                      //Incase of MediaStream file it is tricky to find that the notification is 
                                      //exactly for the current list or not.
                                      //In case of Delete also UI will get replace notification as it is moved to RECYCLEBIN
                                      //So to let user know that just marking as Dirty.
                                      cmpMtxRecordingList.markDirty();
                                    }
                                  });

    topic_Set(tCaller, "DBUpdate_2_ID", DBUpdate_2_ID);


  // SRC or DSt list does not maintain the connection state. For future
//  eventApp.subscribe("PORTCONNECTIONSTATEUPDATE", null, this,
//                          function (EventVar, EventVal) {
//                            var objEvent = EventVal;

//                            var portNID = objEvent.PortNID;
//                            var notifySubscriptionID = objEvent.subscriptionID;
//                            var notifyOperation = objEvent.Operation;
//                            var notifySessionID = objEvent.SessionID;
//                            var notifyConnectionID = objEvent.ConnectionID;
//                            var notifyConnStatevalue = objEvent.Value;

//                            // process only ConnectionStateUpdate notification
//                            if (notifyOperation != "ConnectionStateUpdate") return;

//                            // process only certain value of the connection state
//                            if (notifyConnStatevalue != "SETUP_ERROR" &&
//                                notifyConnStatevalue != "START_ERROR" &&
//                                notifyConnStatevalue != "SOURCE_ERROR" &&
//                                notifyConnStatevalue != "MEDIA_ERROR" &&
//                                notifyConnStatevalue != "DEST_ERROR" &&
//                                notifyConnStatevalue != "STREAMING") return;

//                            var livePortSubscriptionID = topic_Get(tCaller, "LiveMediaPortsSubscriptionID");
//                            var dstPortSubscriptionID = topic_Get(tCaller, "DstMediaPortsSubscriptionID");

//                            if (jsx3.util.strEmpty(livePortSubscriptionID) && jsx3.util.strEmpty(dstPortSubscriptionID)) {
//                              return;
//                            }

//                            var mediaNode = null;
//                            var selNode = null;
//                            var portType = null;
//                            var cmpMtxBlkJSX = null;
//                            var cmpBtnBlk = null;
//                            if (livePortSubscriptionID == notifySubscriptionID) {
//                              portType = "LIVESRC";
//                              cmpBtnBlk = cmpBlkSources;
//                              cmpMtxBlkJSX = cmpMtxMediaList.BlkJSX;
//                              mediaNode = cmpMtxBlkJSX.getRecord(portNID);
//                              selNode = cmpMtxBlkJSX.getRecordNode(portNID);
//                            }
//                            else if (dstPortSubscriptionID == notifySubscriptionID) {
//                              portType = "DEST";
//                              cmpBtnBlk = cmpBlkDestinations;
//                              cmpMtxBlkJSX = cmpMtxDstList.BlkJSX;
//                              mediaNode = cmpMtxBlkJSX.getRecord(portNID);
//                              selNode = cmpMtxBlkJSX.getRecordNode(portNID);
//                            }

//                            if (jsx3.util.strEmpty(mediaNode) || jsx3.util.strEmpty(selNode)) {
//                              return;
//                            }

//                            var portConnectionInfoNode = selNode.selectSingleNode(".//PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
//                            if (jsx3.util.strEmpty(portConnectionInfoNode)) return;

//                            if (notifyConnStatevalue == "SETUP_ERROR" ||
//                                notifyConnStatevalue == "START_ERROR" ||
//                                notifyConnStatevalue == "SOURCE_ERROR" ||
//                                notifyConnStatevalue == "MEDIA_ERROR" ||
//                                notifyConnStatevalue == "DEST_ERROR") {
//                              selNode.setAttribute("jsxsessionstatus", "JSXAPPS/IPVS_ClientUI/images/icons/icon_error_small.png");
//                            }
//                            else if (notifyConnStatevalue == "STREAMING") {
//                              selNode.setAttribute("jsxsessionstatus", "JSXAPPS/IPVS_ClientUI/images/icons/icon_network_on.png");
//                            }
//                            else {
//                              selNode.setAttribute("jsxsessionstatus", "JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png");
//                            }

//                            //insert the node
//                            cmpMtxBlkJSX.insertRecordNode(selNode);

//                            var strXMLCacheId = cmpMtxBlkJSX.getXMLId();
//                            var updatedObjXML = PNameSpace.getCache().getDocument(strXMLCacheId);
//                            cmpBtnBlk.onResponse(updatedObjXML);

//                          });


    var PORTCONNECTIONUPDATE_2_ID = eventApp.subscribe("PORTCONNECTIONUPDATE", null, this,
                                                        function (EventVar, EventVal) {
                                                          var objEvent = EventVal;

                                                          var portNID = objEvent.PortNID;
                                                          var portSessionInfoNode = objEvent.PortSessionInfoNode;
                                                          var portConnectionInfoNode = portSessionInfoNode.selectSingleNode(".//PortConnectionInfo");
                                                          var notifySubscriptionID = objEvent.subscriptionID;
                                                          var notifyOperation = objEvent.Operation;

                                                          var livePortSubscriptionID = topic_Get(tCaller, "LiveMediaPortsSubscriptionID");
                                                          var dstPortSubscriptionID = topic_Get(tCaller, "DstMediaPortsSubscriptionID");

                                                          if (jsx3.util.strEmpty(livePortSubscriptionID) && jsx3.util.strEmpty(dstPortSubscriptionID)) {
                                                            return;
                                                          }

                                                          var mediaNode = null;
                                                          var selNode = null;
                                                          var portType = null;
                                                          var cmpMtxBlkJSX = null;
                                                          var cmpBtnBlk = null;
                                                          if (livePortSubscriptionID == notifySubscriptionID) {
                                                            portType = "LIVESRC";
                                                            cmpBtnBlk = cmpBlkSources;
                                                            cmpMtxBlkJSX = cmpMtxMediaList.BlkJSX;
                                                            mediaNode = cmpMtxBlkJSX.getRecord(portNID);
                                                            selNode = cmpMtxBlkJSX.getRecordNode(portNID);
                                                          }
                                                          else if (dstPortSubscriptionID == notifySubscriptionID) {
                                                            portType = "DEST";
                                                            cmpBtnBlk = cmpBlkDestinations;
                                                            cmpMtxBlkJSX = cmpMtxDstList.BlkJSX;
                                                            mediaNode = cmpMtxBlkJSX.getRecord(portNID);
                                                            selNode = cmpMtxBlkJSX.getRecordNode(portNID);
                                                          }

                                                          if (jsx3.util.strEmpty(mediaNode) || jsx3.util.strEmpty(selNode)) {
                                                            return;
                                                          }

                                                          var notifyConnectionID = portConnectionInfoNode.getAttribute("id");
                                                          if (notifyOperation != "Delete") {
                                                            var notifyConnName = portConnectionInfoNode.getAttribute("name");
                                                            var notifySrcNID = portConnectionInfoNode.getAttribute("srcNID");
                                                            var notifyDstNID = portConnectionInfoNode.getAttribute("dstNID");
                                                            var portSessionInfoStr = "<PortConnectionInfo jsxconnectionid='" + notifyConnectionID + "' jsxconnectionname='" + notifyConnName + "' jsxsrcnid='" + notifySrcNID + "' jsxdstnid='" + notifyDstNID + "' />";
                                                            var portSessionInfoDoc = new jsx3.xml.Document();
                                                            portSessionInfoDoc.loadXML(portSessionInfoStr);
                                                            selNode.setAttribute("jsxsessionstatus", "JSXAPPS/IPVS_ClientUI/images/icons/icon_network_on.png");
                                                          }


                                                          if (notifyOperation == "Add") {
                                                            selNode.appendChild(portSessionInfoDoc);
                                                          }
                                                          else if (notifyOperation == "Update") {
                                                            var oldPortConnectionInfoNode = selNode.selectSingleNode(".//PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
                                                            if (!jsx3.util.strEmpty(oldPortConnectionInfoNode)) {
                                                              selNode.replaceNode(portSessionInfoDoc, oldPortConnectionInfoNode);
                                                            }
                                                          }
                                                          else if (notifyOperation == "Delete") {
                                                            var oldPortConnectionInfoNode = selNode.selectSingleNode(".//PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
                                                            if (!jsx3.util.strEmpty(oldPortConnectionInfoNode)) {
                                                              selNode.removeChild(oldPortConnectionInfoNode);
                                                            }

                                                            var portConnectionInfoIter = selNode.selectNodeIterator(".//PortConnectionInfo");
                                                            if (portConnectionInfoIter.hasNext()) {
                                                              selNode.setAttribute("jsxsessionstatus", "JSXAPPS/IPVS_ClientUI/images/icons/icon_network_on.png");
                                                            }
                                                            else {
                                                              selNode.setAttribute("jsxsessionstatus", "JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png");
                                                            }
                                                          }
                                                          //insert the node
                                                          cmpMtxBlkJSX.insertRecordNode(selNode);

                                                          var strXMLCacheId = cmpMtxBlkJSX.getXMLId();
                                                          var updatedObjXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                          cmpBtnBlk.onResponse(updatedObjXML);

                                                        });

   topic_Set(tCaller, "PORTCONNECTIONUPDATE_2_ID", PORTCONNECTIONUPDATE_2_ID);
                         
  //---------------------  Top Level Cmp Array -----------------------------------
  var SubCmpArr = [{ cmp: cmpBlkTabPanels },
                   { cmp: cmpBlkTabButtons },
                   { cmp: cmpClientWindowHeaderLogo }
                  ];

  
  //Media search
  function mediaSearch(){
    var checkedName = cmpChkSearchName.BlkJSX.getChecked();
    var checkedBookmark = cmpChkSearchBookmark.BlkJSX.getChecked();
    var checkedDate = cmpChkSearchDate.BlkJSX.getChecked();
    if(checkedName == jsx3.gui.CheckBox.CHECKED || checkedBookmark == jsx3.gui.CheckBox.CHECKED || checkedDate == jsx3.gui.CheckBox.CHECKED){
      topic_Set(tCaller, "SearchTextSubStrIn", getSearchTextSubStrIn());
      if(checkedDate == jsx3.gui.CheckBox.CHECKED ){
        topic_Publish(tCaller, "SearchBy","Date");
        cmpStartDatePicker.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        cmpEndDatePicker.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        if(checkedName != jsx3.gui.CheckBox.CHECKED && checkedBookmark != jsx3.gui.CheckBox.CHECKED ){
          topic_Set(tCaller, "WhereTextSubStr", "");
          cmpTxtSearchMedia.BlkJSX.setValue("Search by Name...");
          cmpTxtSearchMedia.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        }
      }
      else{
        topic_Set(tCaller, "WhereDateRange", "");
        topic_Publish(tCaller, "SearchBy","Name");
        cmpStartDatePicker.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        cmpEndDatePicker.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      }
      if(checkedName == jsx3.gui.CheckBox.CHECKED || checkedBookmark == jsx3.gui.CheckBox.CHECKED ){
        var searchString = cmpTxtSearchMedia.BlkJSX.getValue();
        if(searchString == "Search by Name..."){
          cmpTxtSearchMedia.BlkJSX.setValue("");
        }
        cmpTxtSearchMedia.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
      cmpBtnSearch.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      topic_Publish(tCaller, "MediaSearch","ENABLED");
    }
    else if(checkedName != jsx3.gui.CheckBox.CHECKED && checkedBookmark != jsx3.gui.CheckBox.CHECKED && checkedDate != jsx3.gui.CheckBox.CHECKED){
       topic_Set(tCaller, "SearchTextSubStrIn", getSearchTextSubStrIn());
       topic_Set(tCaller, "WhereTextSubStr", "");
       topic_Set(tCaller, "WhereDateRange", "");
       topic_Publish(tCaller, "SearchBy","Name");
       cmpTxtSearchMedia.BlkJSX.setValue("Search by Name...");
       cmpTxtSearchMedia.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
       cmpStartDatePicker.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
       cmpEndDatePicker.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
       cmpBtnSearch.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
       topic_Publish(tCaller, "MediaSearch","DISABLED");
       sendGetFileRequest();
    }
  }

  // Action function
  function getMediaInfo() {
      // get the currently selected media info
      var mediaInfo = new Object;
      var listType = topic_Get(tCaller, "ButtonSelection");
      if (listType == "Live") {
          //mediaInfo.Group = topic_Get(tCaller, "MediaStreamSrcPortGroupSelected");
          mediaInfo.NID = topic_Get(tCaller, "MediaStreamSrcPortSelected");
          mediaInfo.type = "SrcPort";
      }
      else if (listType == "Recorded") {
          //mediaInfo.Group = topic_Get(tCaller, "MediaSDRSelected");
          mediaInfo.NID = topic_Get(tCaller, "MediaSFRNIDListSelected");
          // get title
          var objSelectedFile = cmpMtxRecordingList.BlkJSX.getRecord(mediaInfo.NID);
          mediaInfo.Name = objSelectedFile.jsxtextmediaName;
          // check if grp file
          var fileType = objSelectedFile.jsxfileType;
          // if single file
          if (fileType == "MediaGroup") {
              mediaInfo.type = "GrpFile";
          }
          else {
              mediaInfo.type = "PlyFile";
          }
          mediaInfo.state = objSelectedFile.jsxmediastate;
          mediaInfo.clipID = objSelectedFile.jsxclipID;
          mediaInfo.sessionConfigNID = objSelectedFile.jsxsessionConfigNID;
      }
      else if (listType == "Destination") {
          //mediaInfo.Group = topic_Get(tCaller, "MediaStreamDstPortGroupSelected");
          mediaInfo.NID = topic_Get(tCaller, "MediaStreamDstPortSelected");
          mediaInfo.type = "DstPort";
      }
      if (mediaInfo.type == "SrcPort" || mediaInfo.type == "DstPort") {
          if (mediaInfo.type == "SrcPort") {
            var mediaNode = cmpMtxMediaList.BlkJSX.getRecord(mediaInfo.NID);            
            var selNode = cmpMtxMediaList.BlkJSX.getRecordNode(mediaInfo.NID);
          }
          else {
            var mediaNode = cmpMtxDstList.BlkJSX.getRecord(mediaInfo.NID);        
            var selNode = cmpMtxDstList.BlkJSX.getRecordNode(mediaInfo.NID);
          }
          if (mediaNode) {
              // get the title
              mediaInfo.Name = mediaNode.jsxtext;
              mediaInfo.streamTypeList = new jsx3.util.List();
              mediaInfo.streamTypeList.add(mediaNode.jsxdevStreamType);
              if (mediaInfo.type == "SrcPort") {
                var dstNIDArr = new Array;
                var i = 0;
                var connIterator = selNode.selectNodes(".//PortConnectionInfo ");
                while (connIterator.hasNext()) {
                  var connRecord = connIterator.next();
                  dstNIDArr[i++] = connRecord.getAttribute("jsxdstnid");
                }
                mediaInfo.dstNIDArr = dstNIDArr;           
              }
              else {
                var srcNIDArr = new Array;
                var i = 0;
                var connIterator = selNode.selectNodes(".//PortConnectionInfo ");
                while (connIterator.hasNext()) {
                  var connRecord = connIterator.next();
                  srcNIDArr[i++] = connRecord.getAttribute("jsxsrcnid");
                }
                mediaInfo.srcNIDArr = srcNIDArr;  
              }
              if (mediaInfo.streamTypeList.size() > 1) {
                  mediaInfo.streamType = "MULTI";
              }
              else {
                  mediaInfo.streamType = mediaInfo.streamTypeList.get(0);
              }
              // get the list of tag name/value pairs
              mediaInfo.tagArr = mediaNode.jsxdevTags.split(",");
              // get the max instance count
              mediaInfo.maxInstanceCount = 1; //???? how to get this value
              mediaInfo.state = mediaNode.jsxdevstate;
              // not showing the presence while creating the session
              //mediaInfo.presence = mediaNode.jsxpresence;
          }
      }
      else if (mediaInfo.type == "PlyFile") {
          // just get the streamtype          
          mediaInfo.streamType = objSelectedFile.jsxtextmediaStreamType;
      }
      else if (mediaInfo.type == "GrpFile") {
          // go thru files and get all the clips
          mediaInfo.arrClips = [];
          var files = cmpMtxRecordingList.BlkJSX.getRecordIds();
          for (var i = 0; i < files.length; i++) {
              var clip = cmpMtxRecordingList.BlkJSX.getRecord(files[i]);
              if (clip.jsxfileType == "MediaClip" && clip.jsxmediagroupID == objSelectedFile.jsxmediagroupID) {
                  var clipInfo = new Object();
                  clipInfo.NID = clip.jsxid;
                  clipInfo.Name = clip.jsxtextmediaName;
                  clipInfo.streamType = clip.jsxtextmediaStreamType;
                  clipInfo.clipID = clip.jsxclipID;
                  clipInfo.sessionConfigNID = clip.jsxsessionConfigNID;
                  mediaInfo.arrClips.push(clipInfo);
              }
          }
      }
      return mediaInfo;
  }

  function checkNIDSelection() {
      var selectedflag = true;
      var listType = topic_Get(tCaller, "ButtonSelection");
      if (listType == "Live") {
          var selectedNID = topic_Get(tCaller, "MediaStreamSrcPortSelected");
          if (jsx3.util.strEmpty(selectedNID) || selectedNID == "NONE") {
              cmpCallerWindow.alert("Please select Source");
              selectedflag = false;
          }
      }
      else if (listType == "Recorded") {
          var selectedNID = topic_Get(tCaller, "MediaSFRNIDListSelected");
          if (jsx3.util.strEmpty(selectedNID) || selectedNID == "NONE") {
              cmpCallerWindow.alert("Please select Recorded Media");
              selectedflag = false;
          }
      }
      else if (listType == "Destination") {
          var selectedNID = topic_Get(tCaller, "MediaStreamDstPortSelected");
          if (jsx3.util.strEmpty(selectedNID) || selectedNID == "NONE") {
              cmpCallerWindow.alert("Please select Destination");
              selectedflag = false;
          }
      }
      return selectedflag;
  }

  // fucntion to get which all fields Search should be Applied to
  function getSearchTextSubStrIn(){
    var searchIn = "";
    if(cmpChkSearchName.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED){
      searchIn = searchIn + "TITLE,DESCRIPTION,METADATA";
    }
    if(cmpChkSearchName.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED && cmpChkSearchBookmark.BlkJSX.getChecked()==jsx3.gui.CheckBox.CHECKED){
      searchIn = searchIn + ",";
    }
    if(cmpChkSearchBookmark.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED){
      searchIn = searchIn + "BOOKMARK";
    }
    return searchIn;
  }//end getSearchTextSubStrIn

  // Function (Single point) to send GetFile Request. It takes into account all the search parameters 
  function sendGetFileRequest(NID){
    var dirExpression = getSelectedParentNID();
    var checkedName = cmpChkSearchName.BlkJSX.getChecked();
    var checkedBookmark = cmpChkSearchBookmark.BlkJSX.getChecked();
    topic_Set(tCaller, "WhereTextSubStr", "");
    if(checkedName == jsx3.gui.CheckBox.CHECKED || checkedBookmark == jsx3.gui.CheckBox.CHECKED){
      var searchString = cmpTxtSearchMedia.BlkJSX.getValue();
      if(!jsx3.util.strEmpty(searchString)&& searchString != "Search by Name..."){
        topic_Set(tCaller, "WhereTextSubStr", searchString);
      }
      else{
        //cmpCallerWindow.alert("Please Enter a Valid Name for Search");
        if(!jsx3.util.strEmpty(NID)){
          var recordNode = cmpBlkRecordingDirValues.BlkJSX.getRecordNode(NID);
          if(dirExpression != NID){
            cmpBlkRecordingDirValues.repaint();
          }
          else{
            return;
          }
        }
      }
    }
    var checkedDate = cmpChkSearchDate.BlkJSX.getChecked();
    topic_Set(tCaller, "WhereDateRange", "");
    if(checkedDate == jsx3.gui.CheckBox.CHECKED ){
      var startDate =  cmpStartDatePicker.BlkJSX.getValue();
      var endDate = cmpEndDatePicker.BlkJSX.getValue();
      if(!jsx3.util.strEmpty(startDate) && !jsx3.util.strEmpty(endDate)){
        if(startDate == "yyyy-MM-dd" || endDate == "yyyy-MM-dd" ){
          //cmpCallerWindow.alert("Please Enter a Valid Date");
          if(!jsx3.util.strEmpty(NID)){
            var recordNode = cmpBlkRecordingDirValues.BlkJSX.getRecordNode(NID);
            if(dirExpression != NID){
              cmpBlkRecordingDirValues.repaint();
            }
            else{
              return;
            }
          }
        }
        else{
          var startTime = Date.parse(parseDate(startDate));
          var endTime =  Date.parse(parseDate(endDate)); 
          if(startTime > endTime){
            cmpCallerWindow.alert("End Date should be more than Start Date");
            return;
          }
          var timeZone = getTimeZone();
          startDate = startDate + " 00:00:00.000 " + timeZone;
          endDate = endDate + " 23:59:59.999 " + timeZone;
          var dateRange ="From:"+ startDate + "," + "To:" + endDate;
          topic_Set(tCaller, "WhereDateRange",dateRange);
        }
      }
    }
    topic_Set(tCaller, "SearchTextSubStrIn", getSearchTextSubStrIn());
    var dirExpression = getSelectedParentNID();
    topic_Set(tCaller, "WhereParentNID", dirExpression);
    cmpMtxRecordingList.ResetToStart(1, 50, 0);
    topic_Publish(tCaller, "UpdateDisplayMediaSFRNIDList", newServerDate().getTime());
  }

  //Chekcing if User Deafult Tag Value exist 
  function setSelectTagValue(defaultTagValue, objMatrix) {
      var selectValue = "All";
      if (objMatrix) {
          var strXMLCacheId = objMatrix.getXMLId();
          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
          if (objXML) {
              var objRecordNodes = objXML.selectNodes("//record");
              for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
                  var objRecordNode = it.next();
                  var strRecordName = objRecordNode.getAttribute("jsxid");
                  if (strRecordName == defaultTagValue) {
                      selectValue = defaultTagValue;
                      break;
                  }
              }
          }
      }
      return selectValue;
  }

  // Paint the Button view for each paginated response
  function onResponseButtonView(respCode, respVal, window, eventListener, portType) {
      var cmpCaller = window;
      var tCaller = eventListener;
      if (respCode == "SUCCESS" && respVal != null) {
          if (portType == "SrcPort") {
              for (var i = cmpBlkSources.SubCmpArr.length - 1; i >= 0; i--) {
                  // destroy the block
                  cmpBlkSources.SubCmpArr[i].cmp.destroy();
                  // remove ref from cmp arr
                  cmpBlkSources.SubCmpArr.splice(i, 1);
                  // clear the array
              }
              var sourceResponse = respVal;
              var portIterartor = sourceResponse.selectNodeIterator(".//Port");
              while (portIterartor.hasNext()) {
                  var nodeInfo = portIterartor.next();
                  var sourceDataInfo = processNodeInfo(nodeInfo);
                  //Send to clone function
                  var cmpConnectionSourceBlk = cmpBlkSource_Create(cmpCaller, sourceDataInfo, tCaller);
                  var cmpConnectionSourceBlkObj = { cmp: cmpConnectionSourceBlk };
                  cmpBlkSources.SubCmpArr.push(cmpConnectionSourceBlkObj);
              }
          }
          else if (portType == "DstPort") {
              for (var i = cmpBlkDestinations.SubCmpArr.length - 1; i >= 0; i--) {
                  // destroy the block
                  cmpBlkDestinations.SubCmpArr[i].cmp.destroy();
                  // remove ref from cmp arr
                  cmpBlkDestinations.SubCmpArr.splice(i, 1);
                  // clear the array
              }
              var dstResponse = respVal;
              var portIterartor = dstResponse.selectNodeIterator(".//Port");
              while (portIterartor.hasNext()) {
                  var nodeInfo = portIterartor.next();
                  var dstDataInfo = processNodeInfo(nodeInfo);
                  //Send to clone function
                  var cmpConnectionDstBlk = cmpBlkDst_Create(cmpCaller, dstDataInfo, tCaller);
                  var cmpConnectionDstBlkObj = { cmp: cmpConnectionDstBlk };
                  cmpBlkDestinations.SubCmpArr.push(cmpConnectionDstBlkObj);
              }
          }
      }
  }

  //processNodeInfo each Port response top extract and save details 
  function processNodeInfo(nodeInfo) {
      var sourceInfo = new Object;
      sourceInfo.NID = nodeInfo.getAttribute("NID");
      sourceInfo.title = nodeInfo.getAttribute("title");
      sourceInfo.presence = nodeInfo.getAttribute("presence");
      sourceInfo.streamType = nodeInfo.getAttribute("streamType");
      sourceInfo.state = nodeInfo.getAttribute("state");
      sourceInfo.portType = nodeInfo.getAttribute("portType");
      var tag = new Array;
      var i = 0;
      var tagIterartor = nodeInfo.selectNodeIterator(".//Tag");
      while (tagIterartor.hasNext()) {
          var tagDataInfo = tagIterartor.next();
          var tagInfo = new Object;
          tagInfo.Name = tagDataInfo.getAttribute("Name");
          tagInfo.valueOf = tagDataInfo.getAttribute("Value");
          tag[i++] = tagInfo;
      }
      sourceInfo.tag = tag;
      var device = new Object;
      var deviceInfo = nodeInfo.selectSingleNode("//Device");
      device.NID = deviceInfo.getAttribute("NID");
      device.deviceType = deviceInfo.getAttribute("deviceType");
      sourceInfo.device = device;
      
      // check if any connection is present if yes then show connected else disconnected
      sourceInfo.connState = "DEFAULT";
      var connectionStateInfo = null;
      connectionStateInfo = nodeInfo.selectSingleNode(".//ConnectionStatus/Session");
      if(!jsx3.util.strEmpty(connectionStateInfo)){
        sourceInfo.connState = "CONNECTED";
      }
      
      return sourceInfo;
  }

  // On selecting a Tag Name from drop down
  eventCaller.subscribe("TagNameSelected", null, this,
                          function(eventVar, eventVal) {
                            if (eventVal != "NONE") {
                              var buttonSelectionMode = topic_Get(tCaller, "ButtonSelection");
                              if (buttonSelectionMode == 'Live') {
                                // get the selected value
                                var nameRecord = cmpSelSourceTagName.BlkJSX.getValue();
                                //get the complete record
                                var selNameRecord = cmpSelSourceTagName.BlkJSX.getRecordNode(nameRecord);
                                // get the Tag node which contains the TagValues
                                var tagNode = selNameRecord.selectSingleNode(".//Tag");
                                //get the xsl
                                var xslUriRes = null;
                                var UriRes = cmpSelSourceTagName.BlkJSX.getUriResolver();
                                if( jsx3.util.strEmpty(UriRes) ) return;
                                xslUriRes = UriRes.resolveURI("xsl/TagValueList.xsl");
                                if( jsx3.util.strEmpty(xslUriRes) ) return;

                                var XSLFile = PNameSpace.loadInclude(xslUriRes, "TagValueList" , "xsl");
                                if( XSLFile ) {
                                  var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
                                  var docXformResp = XSLDoc.transformToObject(tagNode);
                                }                                
                                // set/send the cdf doc to the panel
                                cmpBlkSourceTagValues.onResponse(docXformResp);
                              }
                              else if (buttonSelectionMode == 'Destination') {
                                // get the selected value
                                var nameRecord = cmpSelDstTagName.BlkJSX.getValue();
                                //get the complete record
                                var selNameRecord = cmpSelDstTagName.BlkJSX.getRecordNode(nameRecord);
                                // get the Tag node which contains the TagValues
                                var tagNode = selNameRecord.selectSingleNode(".//Tag");
                                //get the xsl
                                var xslUriRes = null;
                                var UriRes = cmpSelDstTagName.BlkJSX.getUriResolver();
                                if( jsx3.util.strEmpty(UriRes) ) return;
                                xslUriRes = UriRes.resolveURI("xsl/TagValueList.xsl");
                                if( jsx3.util.strEmpty(xslUriRes) ) return;

                                var XSLFile = PNameSpace.loadInclude(xslUriRes, "TagValueList" , "xsl");
                                if( XSLFile ) {
                                  var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
                                  var docXformResp = XSLDoc.transformToObject(tagNode);
                                }                                
                                // set/send the cdf doc to the panel
                                cmpBlkDstTagValues.onResponse(docXformResp);                              
                              }
                              topic_Publish(tCaller, "RefreshList", buttonSelectionMode);

                            }
                          }
                        );

  // On Selecting a Tag from the drop down Refresh the list to all tag vales of selected tag
  eventCaller.subscribe("RefreshList", null, this,
                          function(eventVar, eventVal) {
                            var tagExpression = "";
                            if(eventVal == 'Live') {
                              var getSrcRefresh = topic_Get(tCaller, "SrcPortRefresh");
                              if(getSrcRefresh != "NONE") {
                                //Check if Selected tag name is All
                                //Then tagExpression should be empty("")
                                var tagNameSelected = topic_Get(tCaller, "TagNameSelected");
                                if(tagNameSelected != "All") {
                                  var strXMLCacheId = cmpBlkSourceTagValues.BlkJSX.getXMLId();
                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                  var recordIter = objXML.selectNodeIterator(".//record");
                                  var i = 0;
                                  while(recordIter.hasNext()) {
                                    var recordNode = recordIter.next();
                                    var selectedTagName = recordNode.getAttribute("jsxTagName");
                                    var selectedTagValue = recordNode.getAttribute("jsxtext");
                                    var newTagExpression = "Name='" + selectedTagName + "' and Value='" + selectedTagValue + "'";
                                    if(i == 0) {
                                      tagExpression = newTagExpression;
                                      i++;
                                    } else {
                                      tagExpression = tagExpression + "," + newTagExpression;
                                    }
                                  }
                                }
                                //Set new tag expression
                                topic_Set(tCaller, "LiveMediaPortsTagExpression", tagExpression);
                                //Send update to the source list
                                topic_Publish(tCaller, "UpdateDisplaySrcPortList", (newServerDate()).getTime());
                              }
                              topic_Set(tCaller, "SrcPortRefresh", (newServerDate()).getTime());
                            }
                            else if(eventVal == 'Destination') {
                              var getDstRefresh = topic_Get(tCaller, "DstPortRefresh");
                              if(getDstRefresh != "NONE") {
                                //Check if Selected tag name is All
                                //Then tagExpression should be empty("")
                                var tagNameSelected = topic_Get(tCaller, "TagNameSelected");
                                if(tagNameSelected != "All") {
                                  var strXMLCacheId = cmpBlkDstTagValues.BlkJSX.getXMLId();
                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                  var recordIter = objXML.selectNodeIterator(".//record");
                                  var i = 0;
                                  while(recordIter.hasNext()) {
                                    var recordNode = recordIter.next();
                                    var selectedTagName = recordNode.getAttribute("jsxTagName");
                                    var selectedTagValue = recordNode.getAttribute("jsxtext");
                                    var newTagExpression = "Name='" + selectedTagName + "' and Value='" + selectedTagValue + "'";
                                    if(i == 0) {
                                      var tagExpression = newTagExpression;
                                      i++;
                                    } else {
                                      tagExpression = tagExpression + "," + newTagExpression;
                                    }
                                  }
                                }
                                //Set new tag expression
                                topic_Set(tCaller, "DstPortsTagExpression", tagExpression);
                                //Send update to the source list
                                topic_Publish(tCaller, "UpdateDisplayDstPortList", (newServerDate()).getTime());
                              }
                              topic_Set(tCaller, "DstPortRefresh", (newServerDate()).getTime());
                            }
                          }
                         );
                                  
  // On selection of end point higlight connected ports
  eventCaller.subscribe("ConnectedMediaSelection", null, this,
                            function(eventVar, eventVal) {                                 
                              highlightConnPorts();
                            }
                        );                                   
                                  
   function highlightConnPorts() {  
      var NIDList = topic_GetNode(tCaller, "NIDList"); 
      var endPointType = topic_Get(tCaller, "EndPointSelectedType");
      if (endPointType == "StreamDst" || endPointType == "StreamIO") {
          var liveOrRecorded = topic_Get(tCaller, "ButtonSelection");
          if (liveOrRecorded == "Live" || liveOrRecorded == "Recorded") {
            var mtxBlkJSX = (liveOrRecorded == "Live") ? cmpMtxMediaList.BlkJSX : cmpMtxRecordingList.BlkJSX;

            var strXMLCacheId = mtxBlkJSX.getXMLId(); 
            var NIDIterator = NIDList.selectNodeIterator(".//NID");
            if (!NIDIterator.hasNext()) {
              mtxBlkJSX.deselectAllRecords();
            }
            else {
              //get the selected id and go over the nid list
              // if selected id is not in the nid list deselect it
              var selRecordNodes = mtxBlkJSX.getSelectedNodes();
              for (var it = selRecordNodes.iterator(); it.hasNext(); ) {
                var selRecordNode = it.next();
                var jsxID = selRecordNode.getAttribute("jsxid");
                NIDIterator = NIDList.selectNodeIterator(".//NID");
                while (NIDIterator.hasNext()) {
                  var NID = NIDIterator.next().getValue();
                  if (NID != jsxID) {
                    mtxBlkJSX.deselectRecord(jsxID);
                  }
                }
              }

              NIDIterator = NIDList.selectNodeIterator(".//NID");
              var objXML = PNameSpace.getCache().getDocument(strXMLCacheId); 
              //Now select all the records based on the NID LIST
              while(NIDIterator.hasNext()){
                var NID = NIDIterator.next().getValue();         
                var recordNode = objXML.selectSingleNode(".//record[@jsxid ='"+ NID+ "']");
                if (!jsx3.util.strEmpty(recordNode)) {
                  recordNode.setAttribute("jsxselected", "1");
                  if (liveOrRecorded == "Recorded" && !jsx3.util.strEmpty(recordNode.getAttribute("jsxmediagroupID"))) {
                    var parentNode = recordNode.getParent();
                    parentNode.setAttribute("jsxselected", "1");
                    mtxBlkJSX.insertRecordNode(parentNode);
                  }
                  else {
                    mtxBlkJSX.insertRecordNode(recordNode)
                  }
                }
              }
            }

            if (liveOrRecorded == "Live") {
              var updatedObjXML = PNameSpace.getCache().getDocument(strXMLCacheId);
              cmpBlkSources.onResponse(updatedObjXML);
            }
          }
 
      }
      else if (endPointType == "StreamSrc") {
        cmpMtxMediaList.BlkJSX.deselectAllRecords();
        var strXMLCacheId = cmpMtxDstList.BlkJSX.getXMLId();    
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId); 
        var NIDIterator = NIDList.selectNodeIterator(".//NID");
        while(NIDIterator.hasNext()){
          var NID = NIDIterator.next().getValue();         
          var recordNode = objXML.selectSingleNode(".//record[@jsxid ='"+ NID+ "']"); 
          if(!jsx3.util.strEmpty(recordNode)){
            recordNode.setAttribute("jsxselected", "1"); 
            cmpMtxDstList.BlkJSX.insertRecordNode(recordNode);
          }
        }
        var updatedObjXML= PNameSpace.getCache().getDocument(strXMLCacheId); 
        cmpBlkDestinations.onResponse(updatedObjXML);
      }
      else if (endPointType == "NONE") {
        var liveOrRecorded = topic_Get(tCaller, "ButtonSelection");
        if (liveOrRecorded == "Destination") {
          cmpMtxDstList.BlkJSX.deselectAllRecords();
          var strXMLCacheId = cmpMtxDstList.BlkJSX.getXMLId();
          var updatedObjXML = PNameSpace.getCache().getDocument(strXMLCacheId);
          cmpBlkDestinations.onResponse(updatedObjXML);
        }
        if (liveOrRecorded == "Recorded") {
          cmpMtxRecordingList.BlkJSX.deselectAllRecords();
        }
        if (liveOrRecorded == "Live") {
          cmpMtxMediaList.BlkJSX.deselectAllRecords();
          var strXMLCacheId = cmpMtxMediaList.BlkJSX.getXMLId();
          var updatedObjXML = PNameSpace.getCache().getDocument(strXMLCacheId);
          cmpBlkSources.onResponse(updatedObjXML);
        }
      }
   }                           
                                  
  //------------------Setting the Default modes for Installed and web controller----------
  if (clientType == "INSTALLED") {
    topic_Set(tCaller, "ListOrButtonView", "Buttons");
  }
  else {
    cmpBtnShowLiveButtons.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
    cmpBtnShowLiveList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
    topic_Set(tCaller, "ListOrButtonView", "Buttons");
  }
  return SubCmpArr;
 
}

function convertbytesToSize(element, cdfkey, matrix, column, rownumber, server) {
  if (browserType == "MSIE") {
    element.innerHTML = bytesToSize(element.innerText);
  }
  else if (browserType == "FIREFOX") {
    element.innerHTML = bytesToSize(element.firstChild.nodeValue);
  }
}