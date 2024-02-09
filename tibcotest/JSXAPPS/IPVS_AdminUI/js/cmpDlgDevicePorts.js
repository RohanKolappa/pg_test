// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceEditPort_Create(AssetNID, selectedAssetType, selectedAssetName, 
                                          selectedAssetStatus, PortNID, selectedPort, isMNADevice) {
  
  closeDialogIfExists("dlgAssetPreConfigure");
  // load view - dialog component file
  var DlgFile = "components/assets/assetEditPortDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  var portConnectionStatus = false;
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  // setup a private/dlg specific app event handler
  dlgEditAssetEditPort_eventApp = new EventListner( tApp );  

  function dlgEditAsset_Destroy() {
    // destroy any local eventlistners
    portConnectionStatus = false;
    eventDlg.destroy();
    dlgEditAssetEditPort_eventApp.destroy();
    
    // destroy all local components
    cmpDlgEditAsset.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }
 
  var userNIDValue = "";
  var xAuthResourceNID = "";
  var deviceResourceType = "";

  topic_Set(tDlg, "AssetWhereTextSubStr", ""); 
  topic_Set(tDlg, "AssetWhereTagExpr", ""); 
  topic_Set(tDlg, "AssetSelectFormat", "BASIC");
  topic_Set(tDlg, "EditBlkPVRSettings", "NONE");
  topic_Set(tDlg, "EditBlkAutoStart", "NONE");
  topic_Set(tDlg, "EditBlkPreConfigSrcPort", "NONE");
  topic_Set(tDlg, "EditBlkPreConfigDstPort", "NONE");
  topic_Set(tDlg, "EditBlkPreConfigIOPort0", "NONE");
  topic_Set(tDlg, "EditBlkPreConfigIOPort1", "NONE");

  var cmpCDFEditAssetStartSettings = new Component([BlkJSX,"cdfAutoStartSettings"]);
  cmpCDFEditAssetStartSettings.BlkJSX.setXMLTransformers("xsl/EditAssetPortsAutoStart.xsl");
  
  var cmpBlkLblAssetPortInfo = new Component([BlkJSX,"blkLblAssetEditPortInfo"]);
  var cmpBlkLblAssetPVRSettings = new Component([BlkJSX,"blkLblAssetEditPVRSettings"]);
  var cmpBlkLblAssetAutoStart = new Component([BlkJSX,"blkLblAssetEditAutoStart"]);
  var cmpBlkLblAssetPreConfigSrcPort = new Component([BlkJSX,"blkLblAssetPreConfigSrcPort"]);
  var cmpBlkLblAssetPreConfigDstPort = new Component([BlkJSX,"blkLblAssetPreConfigDstPort"]);
  var cmpBlkLblAssetPreConfigIOPort0 = new Component([BlkJSX,"blkLblAssetPreConfigIOPort0"]);
  var cmpBlkLblAssetPreConfigIOPort1 = new Component([BlkJSX,"blkLblAssetPreConfigIOPort1"]);
  
  var cmpBlkAssetPreConfigureLabelBlk = new CmpContainer( [BlkJSX,"MenuSelection"],
                                                          [{cmp:cmpBlkLblAssetPortInfo},
                                                           {cmp:cmpBlkLblAssetPVRSettings,
                                                            sel:"topic_Get(\"" 
                                                            + tDlg + "\", 'EditBlkPVRSettings') != 'NONE' && topic_Get(\"" 
                                                            + tDlg + "\", 'PersistentPVRSelectionType') != 'StreamDst' "}, 
                                                           {cmp:cmpBlkLblAssetAutoStart,
                                                            sel:"topic_Get(\"" 
                                                            + tDlg + "\", 'EditBlkAutoStart') != 'NONE' && topic_Get(\"" 
                                                            + tDlg + "\", 'PersistentPVRSelectionType') != 'StreamDst' "},  
                                                           {cmp:cmpBlkLblAssetPreConfigSrcPort,
                                                            sel:"topic_Get(\"" 
                                                            + tDlg + "\", 'EditBlkPreConfigSrcPort') != 'NONE'"},
                                                           {cmp:cmpBlkLblAssetPreConfigDstPort,
                                                            sel:"topic_Get(\"" 
                                                            + tDlg + "\", 'EditBlkPreConfigDstPort') != 'NONE'"},
                                                           {cmp:cmpBlkLblAssetPreConfigIOPort0,
                                                            sel:"topic_Get(\"" 
                                                            + tDlg + "\", 'EditBlkPreConfigIOPort0') != 'NONE'"},
                                                           {cmp:cmpBlkLblAssetPreConfigIOPort1,
                                                            sel:"topic_Get(\"" 
                                                            + tDlg + "\", 'EditBlkPreConfigIOPort1') != 'NONE'"}  
                                                          ],
                                                          [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkPVRSettings", eventval:null},
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkAutoStart", eventval:null},
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkPreConfigSrcPort", eventval:null},
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkPreConfigDstPort", eventval:null},
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkPreConfigIOPort0", eventval:null},
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkPreConfigIOPort1", eventval:null},
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"PersistentPVRSelectionType", eventval:null}
                                                          ]
                                                        );

   var cmpLblAssetEditPortInfo = new CmpLabelHighlight([BlkJSX,"lblAssetEditPortInfo"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"PortInfo"} );                                               
  var cmpLblAssetEditPVRSettings = new CmpLabelHighlight([BlkJSX,"lblAssetEditPVRSettings"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"PVRSettings"} );                                               
  var cmpLblAssetEditAutoStart = new CmpLabelHighlight([BlkJSX,"lblAssetEditAutoStart"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"AutoStart"} );                                               
  var cmpLblAssetPreConfigSrcPort = new CmpLabelHighlight([BlkJSX,"lblAssetPreConfigSrcPort"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"SrcPort"} );
  var cmpLblAssetPreConfigDstPort = new CmpLabelHighlight([BlkJSX,"lblAssetPreConfigDstPort"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"DstPort"} );
  var cmpLblAssetPreConfigIOPort0 = new CmpLabelHighlight([BlkJSX,"lblAssetPreConfigIOPort0"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"IOPort0"} );
  var cmpLblAssetPreConfigIOPort1 = new CmpLabelHighlight([BlkJSX,"lblAssetPreConfigIOPort1"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:"IOPort1"} );

  var cmpBlkAssetPortInfo = new Component([BlkJSX,"blkAssetEditPortInfo"]);
  var cmpBlkAssetPVRSettings = new Component([BlkJSX,"blkAssetEditPVRSettings"]);
  var cmpBlkAssetAutoStart = new Component([BlkJSX,"blkAssetEditAutoStart"]);
  var cmpBlkAssetPreConfigPort = new Component([BlkJSX,"blkAssetPreConfigurePortInstance"]);
                                              
  var cmpBlkAssetPreConfigureUI = new CmpContainer( [BlkJSX,"InstanceSettings"],
                                                    [{cmp:cmpBlkAssetPortInfo,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'PortInfo'"},
                                                     {cmp:cmpBlkAssetPVRSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'PVRSettings'"},
                                                     {cmp:cmpBlkAssetAutoStart,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'AutoStart'"},
                                                     {cmp:cmpBlkAssetPreConfigPort,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'SrcPort' || " 
                                                        + "topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'DstPort' || "
                                                        + "topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'IOPort0' || "
                                                        + "topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == 'IOPort1' || "
                                                        + "topic_Get(\"" + tDlg + "\", 'EditAssetPreConfigSelection') == null "}
                                                     ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetPreConfigSelection", eventval:null}
                                                    ]
                                                  );

  var cmpBlkEditAssetEditPortUI = new CmpContainer( [BlkJSX,"blkAssetPreConfigure"],
                                                  [{cmp:cmpBlkAssetPreConfigureLabelBlk},
                                                   {cmp:cmpLblAssetEditPortInfo},
                                                   {cmp:cmpLblAssetEditPVRSettings},
                                                   {cmp:cmpLblAssetEditAutoStart},
                                                   {cmp:cmpLblAssetPreConfigSrcPort},
                                                   {cmp:cmpLblAssetPreConfigDstPort},
                                                   {cmp:cmpLblAssetPreConfigIOPort0},
                                                   {cmp:cmpLblAssetPreConfigIOPort1},  
                                                   {cmp:cmpBlkAssetPreConfigureUI}
                                                  ],
                                                  null
                                                );                                           
  
  var cmptxtPortName = new CmpTxtBox([BlkJSX,"txtPortName"], null, null, null, null);  

  var cmptxtPortDescription = new CmpTxtBox([BlkJSX,"txtPortDescription"], null, null, null, null);  

  var cmpselPortStreamType = new CmpStaticList([BlkJSX,"selPortStreamType"],
                                               "xml/selections/MediaStreamType.xml",
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  cmpselPortStreamType.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
                                               }
                                              });

  var cmpselPortType = new CmpStaticList([BlkJSX,"selPortType"],
                                         "xml/selections/PortType.xml",
                                         null,
                                         {action:"FNCCALL", 
                                         context:null, 
                                         fnc:function() {
                                            cmpselPortType.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
                                         }
                                        });

  var cmpmtxPortTags = new CmpBlock([BlkJSX,"mtxPortTags"]);
  cmpmtxPortTags.BlkJSX.setXMLTransformers("xsl/TagToCDFTag.xsl");
  
  var cmpCmbSelEditPortTagDefName = new CmpCustomPaginationList([BlkJSX, "cmbPortAddTagName"],
                                                                 [{action:"TOPICEVENT", topic: tDlg, eventvar:"PortTagDefNameState", eventval:null}],
                                                                   { persist: { col: "jsxtext", initval: false },
                                                                       action: "TOPICEVENT", topic: tDlg, eventvar: "PortTagDefNameSelected", eventval: "jsxid"
                                                                   },
                                                                null,
                                                                null,
                                                                "xsl/TagNameList.xsl"
                                                                );

  cmpCmbSelEditPortTagDefName.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                                 CountToFetch: "ObjectList_CountToFetch",
                                                 SortOrder: "ObjectList_SortOrder",
                                                 ObjectCount: "ObjectCount",
                                                 SortByXpath: "ObjectList_SortBy",
                                                 WhereObjectType: "ObjectList_WhereObjectType"
                                              };

  topic_Set(tDlg, "PortTagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "PortTagNameResponseObjectType", "Device");
  
  cmpCmbSelEditPortTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetTags",
                                                                              [{ VarName: "ObjectList_SelectFormat", VarRef: "PortTagNameResponseSelectFormat"},
                                                                               { VarName: "ObjectList_WhereObjectType", VarRef: "PortTagNameResponseObjectType"}],
                                                                              { action: "FNCCALL", context: cmpCmbSelEditPortTagDefName,
                                                                                  fnc: function(respCode, respVal) {
                                                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                                                          this.onResponse(respCode, respVal);
                                                                                      }
                                                                                      else {
                                                                                          log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                                          this.onResponseErrorHandle("GetTags",respCode, respVal);
                                                                                      }
                                                                                  }
                                                                              }
                                                                            );

  
  var cmpCmbSelEditPortTagDefValue = new Component([BlkJSX, "cmbPortAddTagValue"]);
  cmpCmbSelEditPortTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");

  eventDlg.subscribe("PortTagDefNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          cmpCmbSelEditPortTagDefValue.BlkJSX.setValue("");
                          var nameRecord = cmpCmbSelEditPortTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelEditPortTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          var strXMLValueCacheId = cmpCmbSelEditPortTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelEditPortTagDefValue.BlkJSX.setSourceXML(objXML); 
                        }
                      }
                    );
  
  var cmpBtnEditPortAddGroup = new CmpButton( [BlkJSX,"btnPortAddTag"],
                                              null,
                                              { action:"FNCCALL", 
                                                context:null,
                                                fnc:function() {
                                                  var nameRecord =  cmpCmbSelEditPortTagDefName.BlkJSX.getValue();
                                                  var selNameRecord =  cmpCmbSelEditPortTagDefName.BlkJSX.getRecord(nameRecord);
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
                                                                                                
                                                  var valueRecord = cmpCmbSelEditPortTagDefValue.BlkJSX.getValue();
                                                  var selValueRecord = cmpCmbSelEditPortTagDefValue.BlkJSX.getRecord(valueRecord);
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
                                                  o.jsxTagName = groupTagName;
                                                  o.jsxTagValue = groupTagValue;
                                                  cmpmtxPortTags.BlkJSX.insertRecord(o, null, false);
                                                  cmpCmbSelEditPortTagDefName.BlkJSX.setValue(null);
                                                  cmpCmbSelEditPortTagDefValue.BlkJSX.setValue(null);
                                                  cmpmtxPortTags.BlkJSX.repaintData();
                                                }
                                              });

  var cmpchkPortXAuth = new CmpCheckBox([BlkJSX, "chkPortXAuth"],
                                        null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function() {
                                                if (this.BlkJSX.getChecked()) {
                                                  cmpchkPortXAuth.BlkJSX.setChecked("1");
                                                }
                                                else {
                                                  cmpchkPortXAuth.BlkJSX.setChecked("0");
                                                }
                                          }
                                        });

  var cmpchkPortMonitor = new CmpCheckBox([BlkJSX, "chkPortMonitor"],
                                          null,
                                          { action: "FNCCALL",
                                            context: null,
                                            fnc: function() {
                                                  if (this.BlkJSX.getChecked()) {
                                                    cmpchkPortMonitor.BlkJSX.setChecked("1");
                                                  }
                                                  else {
                                                    cmpchkPortMonitor.BlkJSX.setChecked("0");
                                                  }
                                            }
                                          });

    var cmpSelPortXAuthUser = new CmpCustomPaginationList([BlkJSX, "selPortXAuthUser"],
                                                        [{action:"TOPICEVENT", topic: tDlg, eventvar:"UserNameState", eventval:null}],
                                                           { persist: { col: "jsxtext", initval: false },
                                                               action: "TOPICEVENT", topic: tDlg, eventvar: "UserNameSelected", eventval: "jsxid"
                                                           },
                                                        null,
                                                        null
                                                        );

  cmpSelPortXAuthUser.SortPathArr = { jsxtext:"username"
                                    };

  cmpSelPortXAuthUser.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                         CountToFetch: "ObjectList_CountToFetch",
                                         SortOrder: "ObjectList_SortOrder",
                                         ObjectCount: "ObjectCount",
                                         SortByXpath: "ObjectList_SortBy"
                                       };

  topic_Set(tDlg, "UserWhereTextSubStr", "");
  topic_Set(tDlg, "UserWhereTagExpr", "");
  topic_Set(tDlg, "UserSelectFormat", "EXTENDED");
  cmpSelPortXAuthUser.BlkJSX.setXMLTransformers("xsl/XAuthUserList.xsl");
  cmpSelPortXAuthUser.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetUser",
                                                                      [{ VarName: "ObjectList_SelectFormat", VarRef: "UserSelectFormat"},
                                                                       { VarName: "ObjectList_WhereTextSubStr", VarRef: "UserWhereTextSubStr"},
                                                                       { VarName: "ObjectList_WhereTagExpr", VarRef: "UserWhereTagExpr"}
                                                                      ],
                                                                      { action: "FNCCALL", context: cmpSelPortXAuthUser,
                                                                        fnc: function(respCode, respVal) {
                                                                              if (respCode == "SUCCESS" && respVal != null) {
                                                                                var userNodes = respVal.selectNodes("//GetUserResponse/User");
                                                                                for (var it = userNodes.iterator();it.hasNext();){
                                                                                  var userNode = it.next();
                                                                                  var userName = userNode.getAttribute("username");
                                                                                  if(userName == "superuser-default"){
                                                                                    userNode.getParent().removeChild(userNode);
                                                                                    break;
                                                                                  }
                                                                                }
                                                                                this.onResponse(respCode, respVal);
                                                                                insertElementInSelectBox("UnSelectUser" , "UnSelect User", "selPortXAuthUser" );
                                                                                if( userNIDValue != "" ){
                                                                                  var objRecordNodes = respVal.selectNodes("//GetUserResponse/User");
                                                                                  for (var it = objRecordNodes.iterator();it.hasNext();){
                                                                                    var objRecordNode = it.next();
                                                                                    var strRecordName = objRecordNode.getAttribute("username");
                                                                                    var strRecordid = objRecordNode.getAttribute("NID");
                                                                                    if(strRecordid == userNIDValue ) {
                                                                                      cmpSelPortXAuthUser.BlkJSX.setValue(strRecordid);
                                                                                      break;
                                                                                    }
                                                                                  }
                                                                                }
                                                                              }
                                                                              else{
                                                                                log.info("GetUserRequest failed. Error Code :" + respCode);
                                                                                this.onResponseErrorHandle("GetUser",respCode, respVal);
                                                                              }
                                                                           }
                                                                        } 
                                                                      );

 var cmpSelPortXAuthResource = new CmpCustomPaginationList([BlkJSX, "selPortXAuthResource"],
                                                           [{action:"TOPICEVENT", topic: tDlg, eventvar:"AssetResourceState", eventval:null}],
                                                             { persist: { col: "jsxtext", initval: false },
                                                                 action: "TOPICEVENT", topic: tDlg, eventvar: "AssetResourceSelected", eventval: "jsxid"
                                                             },
                                                           null,
                                                           null
                                                           );

  cmpSelPortXAuthResource.SortPathArr = { jsxtext:"title"
                                        };

  cmpSelPortXAuthResource.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                            CountToFetch: "ObjectList_CountToFetch",
                                            SortOrder: "ObjectList_SortOrder",
                                            ObjectCount: "ObjectCount",
                                            SortByXpath: "ObjectList_SortBy"
                                           };

  topic_Set(tDlg, "AssetWhereType", "Proxy-Device"); 
  cmpSelPortXAuthResource.BlkJSX.setXMLTransformers("xsl/AssetList.xsl");
  cmpSelPortXAuthResource.cmpReq = new cmpReq_GetSessionAPIPaginationList(tDlg, "GetDevice",
                                                                          [{ VarName: "ObjectList_SelectFormat", VarRef: "AssetSelectFormat"},
                                                                           { VarName: "ObjectList_WhereTextSubStr", VarRef: "AssetWhereTextSubStr"},
                                                                           { VarName: "ObjectList_WhereTagExpr", VarRef: "AssetWhereTagExpr"},
                                                                           { VarName: "ObjectList_WhereType", VarRef: "AssetWhereType"}
                                                                          ],
                                                                          {action: "FNCCALL", context: cmpSelPortXAuthResource,
                                                                            fnc: function(respCode, respVal) {
                                                                                if (respCode == "SUCCESS" && respVal != null) {
                                                                                  this.onResponse(respCode, respVal);
                                                                                  insertElementInSelectBox("UnSelectResource" , "UnSelect Resource", "selPortXAuthResource" );
                                                                                  if( xAuthResourceNID != "" ){
                                                                                    var objRecordNodes = respVal.selectNodes("//GetDeviceResponse/Device");
                                                                                    for (var it = objRecordNodes.iterator();it.hasNext();){
                                                                                      var objRecordNode = it.next();
                                                                                      var strRecordName = objRecordNode.getAttribute("title");
                                                                                      var strRecordid = objRecordNode.getAttribute("NID");
                                                                                      if(strRecordid == xAuthResourceNID ) {
                                                                                        cmpSelPortXAuthResource.BlkJSX.setValue(strRecordid);
                                                                                        break;
                                                                                      }
                                                                                    }
                                                                                  }
                                                                                }
                                                                                else{
                                                                                  log.info("GetDeviceRequest failed. Error Code :" + respCode);
                                                                                  this.onResponseErrorHandle("GetDevice",respCode, respVal);
                                                                                }
                                                                              }
                                                                          } 
                                                                         );      
                                                       
  var cmpBlkEditPortInfo = new CmpContainer([BlkJSX, "blkPortInfo"],
                                           [{cmp:cmptxtPortName},
                                            {cmp:cmptxtPortDescription},
                                            {cmp:cmpselPortStreamType},
                                            {cmp:cmpselPortType},
                                            {cmp:cmpCmbSelEditPortTagDefName},
                                            {cmp:cmpCmbSelEditPortTagDefValue},
                                            {cmp:cmpBtnEditPortAddGroup},
                                            {cmp:cmpmtxPortTags},
                                            {cmp:cmpchkPortXAuth},
                                            {cmp:cmpchkPortMonitor},
                                            {cmp:cmpSelPortXAuthUser},
                                            {cmp:cmpSelPortXAuthResource}
                                           ],
                                           [{action:"TOPICEVENT", topic:tDlg, eventvar:"ForceXAuthDataState", eventval:null}]); 
                                         
  topic_Publish(tDlg, "ForceXAuthDataState", (new Date));
  
  var cmpPVRTrimLengthSetting = new CmpPVRTrimLengthContainer([BlkJSX,"blkBody"],
                                                              [[BlkJSX,"chkEnablePVR"],
                                                               [BlkJSX,"txtHrTrimPVRLength"],
                                                               [BlkJSX,"txtTrimPVRLength"]
                                                              ],
                                                              null,
                                                              null,
                                                              BlkJSX,
                                                              null
                                                              );  

  cmpPVRTrimLengthSetting.DataUpdatedEvent = { action: "TOPICEVENT", topic: tDlg, eventvar: "PortPVRTrimLengthDataUpdated", eventval: "LOADED" };;

  topic_Set(tDlg, "PVRRecordingLocationSelected", "NONE");

  var cmpselPVRRecordingLocation = new CmpSelect([BlkJSX, "selPVRRecordingLocation"],
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "GetPVRRecordingLocationList", eventval: null}],
                                    { persist: { col: "jsxid", initval: false },
                                      action: "TOPICEVENT", topic: tDlg, eventvar: "PVRRecordingLocationSelected", eventval: "jsxid"});

  var cmpBlkEditPVRSettings = new CmpContainer([BlkJSX, "blkPVRSettings"],
                                         [{cmp:cmpPVRTrimLengthSetting},
                                          {cmp:cmpselPVRRecordingLocation}
                                         ],
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"PVRSettingsDataState", eventval:null}]); 
                                         
  topic_Publish(tDlg, "PVRSettingsDataState", (new Date));

  eventDlg.subscribe("PortPVRTrimLengthDataUpdated", null, this,
                      function() {
                        var checkPVR = cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.getChecked();
                        if(checkPVR){
                          var trimLength = cmpPVRTrimLengthSetting.getEnablePVRTrimLength();
                          if(trimLength == false){
                            return false;
                          }
                        }
                        return true;
                      });

  eventDlg.subscribe("GetPVRRecordingLocationList",null,this,
                      function(EventVar, EventVal) {
                        var reqParams = new Object;
                        reqParams["ObjectList_SortOrder"] = "ascending";
                        reqParams["ObjectCount"] = "0";
                        reqParams["ObjectList_StartFrom"] = "1";
                        reqParams["ObjectList_CountToFetch"] = "";
                        reqParams["ObjectList_SortBy"] = "";
                        reqParams["ObjectList_SelectFormat"] = "BASIC";
                        reqParams["ObjectList_WherePermissions"] = "PVR";
                        var reqPVRDirData = new ReqSessionGetRequest("GetDir",
                                                                    { action: "FNCCALL", context: this,
                                                                      fnc: function(respCode, respVal) {
                                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                                            var selMediaDirNID = null;
                                                                            var mediaDirNID = topic_Get(tDlg, "PVRRecordingLocationSelection");
                                                                            // delete the PVR and RECYCLEBIN dir from the response
                                                                            var pvrRBDirNodes = respVal.selectNodes(".//Dir[@title='PVR' or @title='RECYCLEBIN']");
                                                                            for (var it = pvrRBDirNodes.iterator(); it.hasNext(); ) {
                                                                              var dirNode = it.next();
                                                                              dirNode.getParent().removeChild(dirNode);
                                                                            }
                                                                            var dirIter = respVal.selectNodeIterator(".//Dir");
                                                                            while (dirIter.hasNext()) {
                                                                              var dirNode = dirIter.next();
                                                                              var objRec = new Object;
                                                                              var dirNID = dirNode.getAttribute("NID"); 
                                                                              objRec.jsxid = dirNID;
                                                                              objRec.jsxtext = dirNode.getAttribute("title");
                                                                              if((!jsx3.util.strEmpty(mediaDirNID)) && (mediaDirNID == dirNID)){
                                                                                selMediaDirNID = mediaDirNID;
                                                                              }
                                                                              cmpselPVRRecordingLocation.BlkJSX.insertRecord(objRec, null, false);
                                                                            }
                                                                            if(!jsx3.util.strEmpty(selMediaDirNID)){
                                                                              cmpselPVRRecordingLocation.BlkJSX.setValue(selMediaDirNID, true);
                                                                            }
                                                                          }
                                                                      }
                                                                    });
                        reqPVRDirData.reqParams = reqParams;
                        reqPVRDirData.sendRequest();
                        return true;
                      });
                     
  eventDlg.subscribe("PVRRecordingLocationSelected",null,this,
                    function(EventVar, EventVal) {
                      var dirNID = EventVal;
                      if( jsx3.util.strEmpty(dirNID) || dirNID == "NONE" ) return;
                      var mediaDirNID = cmpselPVRRecordingLocation.BlkJSX.getValue();
                      topic_Set(tDlg, "PVRRecordingLocationSelection", mediaDirNID);
                    });  

  topic_Set(tDlg, "AutoProfileSelected", "NONE");

  var cmpchkAutoStart = new CmpCheckBox([BlkJSX, "chkEnableAutoStart"],
                                        null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function() {
                                                if (this.BlkJSX.getChecked()) {
                                                  cmpchkAutoStart.BlkJSX.setChecked("1");
                                                  cmpBlkAutoStartSettingsMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                }
                                                else {
                                                  cmpchkAutoStart.BlkJSX.setChecked("0");
                                                  cmpBlkAutoStartSettingsMask.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                                }
                                          }
                                        });

  /*var cmpchkEnableKBM = new CmpCheckBox([BlkJSX, "chkV2dTxEnableKBM"],
                                        null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function() {
                                                if (this.BlkJSX.getChecked()) {
                                                  cmpchkEnableKBM.BlkJSX.setChecked("1");
                                                }
                                                else {
                                                  cmpchkEnableKBM.BlkJSX.setChecked("0");
                                                }
                                          }
                                        });

  var cmpchkEnableKMSwitch = new CmpCheckBox([BlkJSX, "chkV2dTxAllowKmSwitch"],
                                              null,
                                              { action: "FNCCALL",
                                                context: null,
                                                fnc: function() {
                                                      if (this.BlkJSX.getChecked()) {
                                                        cmpchkEnableKMSwitch.BlkJSX.setChecked("1");
                                                      }
                                                      else {
                                                        cmpchkEnableKMSwitch.BlkJSX.setChecked("0");
                                                      }
                                                }
                                              });

  var cmpchkFFTracks = new CmpCheckBox([BlkJSX, "chkV2dFFTracks"],
                                        null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function() {
                                                if (this.BlkJSX.getChecked()) {
                                                  cmpchkFFTracks.BlkJSX.setChecked("1");
                                                }
                                                else {
                                                  cmpchkFFTracks.BlkJSX.setChecked("0");
                                                }
                                          }
                                        });

  var cmpchkUseMulitcast = new CmpCheckBox([BlkJSX, "chkV2dTx_SI_UseMulitcast"],
                                            null,
                                            { action: "FNCCALL",
                                              context: null,
                                              fnc: function() {
                                                    if (this.BlkJSX.getChecked()) {
                                                      cmpchkUseMulitcast.BlkJSX.setChecked("1");
                                                    }
                                                    else {
                                                      cmpchkUseMulitcast.BlkJSX.setChecked("0");
                                                    }
                                              }
                                            });*/

//  var cmpselProfileList = new CmpSelect([BlkJSX, "selAutoStartProfile"],
//                                        [{ action: "TOPICEVENT", topic: tDlg, eventvar: "GetProfileList", eventval: null}],
//                                        { persist: { col: "jsxid", initval: false },
//                                          action: "TOPICEVENT", topic: tDlg, eventvar: "AutoProfileSelected", eventval: "jsxid"});
  
  var cmptxtMaxBandwidth = new CmpTxtBox([BlkJSX,"txtMaxBandwidth"], null, null, null, null);  

  var cmptxtFrameRateDivider = new CmpTxtBox([BlkJSX,"txtFrameRateDivider"], null, null, null, null);  

  var cmptxtBurstRatio = new CmpTxtBox([BlkJSX,"txtBurstRatio"], null, null, null, null);  

  var cmptxtBurstDuration = new CmpTxtBox([BlkJSX,"txtBurstDuration"], null, null, null, null);  
  
  var cmptxtLowComp = new CmpTxtBox([BlkJSX,"txtLowComp"], null, null, null, null);  

  var cmptxtHighComp = new CmpTxtBox([BlkJSX,"txtHighComp"], null, null, null, null);  

  var cmptxtMinRefresh = new CmpTxtBox([BlkJSX,"txtV2dTxMinRefresh"], null, null, null, null);  

  var cmptxtMaxRefresh = new CmpTxtBox([BlkJSX,"txtV2dTxMaxRefresh"], null, null, null, null);  

  var cmptxtMinSlice = new CmpTxtBox([BlkJSX,"txtV2dTxMinSlice"], null, null, null, null);  

  var cmptxtMaxSlice = new CmpTxtBox([BlkJSX,"txtV2dTxMaxSlice"], null, null, null, null);  

  var cmptxtThreshold = new CmpTxtBox([BlkJSX,"txtV2dTxBlockThreshold"], null, null, null, null);  

  var cmpSelColorSampling = new CmpSelect([BlkJSX,"selColorSampling"],
                                           null,
                                           {action:"FNCCALL", 
                                           context:null, 
                                           fnc:function() {
                                              var selVal = cmpSelColorSampling.BlkJSX.getValue();
                                              if (jsx3.util.strEmpty(selVal)) return;
                                           }
                                          });
  
  var cmpSelAVOption = new CmpSelect([BlkJSX, "selV2dTx_SI_AVOption"],
                                     null,
                                     { action: "FNCCALL",
                                       context: null,
                                       fnc: function () {
                                         var selVal = cmpSelAVOption.BlkJSX.getValue();
                                         if (jsx3.util.strEmpty(selVal)) return;
                                       }
                                     });

  var cmpBlkAutoStartSettingsMask = new Component([BlkJSX, "blkAutoStartSettingsMask"]);

  var cmpBlkEditAutoStart = new CmpContainer([BlkJSX, "blkAutoStartSettings"],
                                         [//{cmp:cmpselProfileList},
                                          {cmp: cmpchkAutoStart },
                                          /*{cmp:cmpchkEnableKBM},
                                          {cmp:cmpchkEnableKMSwitch},
                                          {cmp:cmpchkFFTracks},
                                          {cmp:cmpchkUseMulitcast},*/
                                          {cmp: cmpSelAVOption },
                                          { cmp: cmptxtMaxBandwidth },
                                          { cmp: cmptxtFrameRateDivider },
                                          { cmp: cmptxtBurstRatio },
                                          { cmp: cmptxtBurstDuration },
                                          { cmp: cmptxtLowComp },
                                          { cmp: cmptxtHighComp },
                                          { cmp: cmptxtMinRefresh },
                                          { cmp: cmptxtMaxRefresh },
                                          { cmp: cmptxtMinSlice },
                                          { cmp: cmptxtMaxSlice },
                                          { cmp: cmptxtThreshold },
                                          { cmp: cmpSelColorSampling },
                                          { cmp: cmpBlkAutoStartSettingsMask }
                                         ],
                                         [{ action: "TOPICEVENT", topic: tDlg, eventvar: "AutoStartDataState", eventval: null}]); 
                                         
  topic_Publish(tDlg, "AutoStartDataState", (new Date));

  topic_Set(tDlg, "ProfileType", "V2D");
//  eventDlg.subscribe("GetProfileList",null,this,
//                      function(EventVar, EventVal) {
//                        var reqProfileData = new cmpReq_GetListReadQuery(tDlg, "JSXAPPS/IPVS_AdminUI/query/GetCompleteV2DProfileList.xml",
//                                                      [{VarName:"_PROFILETYPE_", VarRef:"ProfileType"}
//                                                      ],
//                                                      {action: "FNCCALL", context: null,
//                                                        fnc: function(respCode, respVal) {
//                                                            if (respCode == "SUCCESS" && respVal != null) {
//                                                              var selprofileNID = null;
//                                                              var v2dProfileNID = topic_Get(tDlg, "AutoStartSelection");
//                                                              var profileIter = respVal.selectNodeIterator(".//V2DMediaStreamProfile");
//                                                              while (profileIter.hasNext()) {
//                                                                var profileNode = profileIter.next();
//                                                                setDialogDataNode(tDlg, profileNode);
//                                                                var objRec = new Object;
//                                                                var profileNID = profileNode.getAttribute("NID"); 
//                                                                objRec.jsxid = profileNID;
//                                                                objRec.jsxtext = profileNode.selectSingleNode(".//Title").getValue();
//                                                                if((!jsx3.util.strEmpty(v2dProfileNID)) && (v2dProfileNID == profileNID)){
//                                                                  selprofileNID = v2dProfileNID;
//                                                                }
//                                                                cmpselProfileList.BlkJSX.insertRecord(objRec, null, false);
//                                                              }
//                                                              if(!jsx3.util.strEmpty(selprofileNID)){
//                                                                 cmpselProfileList.BlkJSX.setValue(selprofileNID, true);
//                                                              }
//                                                            }
//                                                          }
//                                                      }, "ItemByNID");
//                        var reqParams = new Object;
//                        reqParams["Op_Select_ListSpec_SortOrder"] = "ascending";
//                        reqParams["ObjectCount"] = "";
//                        reqParams["Op_Select_ListSpec_StartFrom"] = "1";
//                        reqParams["Op_Select_ListSpec_CountToFetch"] = "5000";
//                        reqParams["Op_Select_ListSpec_SortByXPathList_SortByXpath"] = "";
//                        reqProfileData.reqParams = reqParams;                              
//                        reqProfileData.sendRequest();
//                        return true;
//                      });
                     
//  eventDlg.subscribe("AutoProfileSelected",null,this,
//                    function(EventVar, EventVal) {
//                      var profileNID = EventVal;
//                      if( jsx3.util.strEmpty(profileNID) || profileNID == "NONE" ) return;
//                      var v2dProfileNID = cmpselProfileList.BlkJSX.getValue();
//                      topic_Set(tDlg, "AutoStartSelection", v2dProfileNID);
//                      var profileXMLNode = getDialogStreamProfileDataNode(tDlg, v2dProfileNID);
//                      topic_Publish(tDlg, "AutoStartProfileSelection", profileXMLNode);
//                    });
//                    
//  eventDlg.subscribe("AutoStartProfileSelection",null,this,
//                    function(EventVar, EventVal) {
//                      var profileXMLNode = EventVal;
//                      if( jsx3.util.strEmpty(profileXMLNode) || profileXMLNode == "NONE" ) return;
//                      setProfileXMLValues(profileXMLNode);
//                    });

//  function setProfileXMLValues(profileXMLNode){
//    /*var enableKBM = profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterKbmConfig/EnableKbm").getValue();
//    cmpchkEnableKBM.BlkJSX.setChecked("0");
//    if(enableKBM == "true") {
//      cmpchkEnableKBM.BlkJSX.setChecked("1");
//    }
//    var allowKMSwitch = profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterKbmConfig/AllowKMSwitch").getValue();
//    cmpchkEnableKMSwitch.BlkJSX.setChecked("0");
//    if(allowKMSwitch == "true") {
//      cmpchkEnableKMSwitch.BlkJSX.setChecked("1");
//    }
//    var ffTracks = profileXMLNode.selectSingleNode(".//V2DRecorderConfig/FFTracks").getValue();
//    cmpchkFFTracks.BlkJSX.setChecked("0");
//    if(ffTracks == "1-256") {
//      cmpchkFFTracks.BlkJSX.setChecked("1");
//    }*/
//    cmpSelAVOption.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DStreamConfig/AVOption").getValue());
//    /*var useMulticast = profileXMLNode.selectSingleNode(".//V2DStreamConfig/IsMulticast").getValue();
//    cmpchkUseMulitcast.BlkJSX.setChecked("0");
//    if(useMulticast == "true") {
//      cmpchkUseMulitcast.BlkJSX.setChecked("1");
//    }*/
//    cmptxtMaxBandwidth.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Bandwidth").getValue());
//    cmptxtFrameRateDivider.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/FrameRateDivider").getValue());
//    cmptxtBurstRatio.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/BurstRatio").getValue());
//    cmptxtBurstDuration.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/BurstDuration").getValue());
//    cmptxtLowComp.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Compression/Low").getValue());
//    cmptxtHighComp.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Compression/High").getValue());
//    cmptxtMinRefresh.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Refresh/Min").getValue());
//    cmptxtMaxRefresh.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Refresh/Max").getValue());
//    cmptxtMinSlice.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Slice/Min").getValue());
//    cmptxtMaxSlice.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/Slice/Max").getValue());
//    cmptxtThreshold.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/BlockThreshold").getValue());
//    cmpSelColorSampling.BlkJSX.setValue(profileXMLNode.selectSingleNode(".//V2DEncoderConnectionParameterConfig/ColorSampling").getValue());
//  }

  var cmptxtaAssetPreConfigureInstance = new CmpTxtBox([BlkJSX,"txtaAssetPreConfigureInstance"], null, null, null, null);  

  var cmptxtAssetPreConfigureResourceNID = new CmpTxtBox([BlkJSX,"txtAssetPreConfigureResourceNID"], null, null, null, null);  

  var cmpSelEditAssetPreConfigurePortStreamType = new CmpStaticList([BlkJSX,"selAssetPreConfigureStreamType"],
                                                                     "xml/selections/MediaStreamType.xml",
                                                                     null,
                                                                     {action:"FNCCALL", 
                                                                     context:null, 
                                                                     fnc:function() {
                                                                        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true); 
                                                                     }
                                                                    });  

  var cmpBlkEditAssetEditPortConfigure = new CmpContainer( [BlkJSX,"blkAssetPreConfigurePortInstance"],
                                                           [{cmp:cmptxtaAssetPreConfigureInstance},
                                                            {cmp:cmptxtAssetPreConfigureResourceNID},
                                                            {cmp:cmpSelEditAssetPreConfigurePortStreamType}
                                                           ],
                                                           null
                                                          );                                           

  var cmpBtnEditAssetClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
        
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAssetPreConfigure"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetPreConfigure"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditAssetSetAssetState", "DONE", this,
                                                                function() {
                                                                  dlgEditAsset_Destroy();
                                                                });
                                              if(portConnectionStatus){
                                                var agree = confirm('\n Editing/Disabling the PVR function may adversely affect media clients currently viewing \n this port. In most cases, users will need to close and restart their media client application.\n \n Are you sure you want to continue?');
                                                if(!agree) return false;
                                              }
                                              cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                            }
                                           });

  var cmpCDFEditAssetEditPorts = new Component([BlkJSX,"cdfDlgAssetEditPorts"]);

  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"dlgAssetPreConfigure"],
                                        [ {cmp:cmpBlkEditAssetEditPortUI},
                                          {cmp:cmpBlkEditPortInfo},
                                          {cmp:cmpBlkEditPVRSettings},
                                          {cmp:cmpBlkEditAssetEditPortConfigure},
                                          {cmp:cmpBtnEditAssetClose},
                                          {cmp:cmpBtnEditAssetCancel},
                                          {cmp:cmpBtnEditAssetOK},
                                          {cmp:cmpBlkEditAutoStart},
                                          {cmp:cmpCDFEditAssetStartSettings},
                                          {cmp:cmpCDFEditAssetEditPorts}
                                        ],
                                        [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetDataState", eventval:AssetNID}]);
                                        
  cmpDlgEditAsset.BlkJSX = BlkJSX;
  cmpDlgEditAsset.ObjectNID = PortNID;
  cmpDlgEditAsset.deviceUserJID = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_USERJID);
  // fnc cmpReq_GetPort defined in cmpReq.js
  cmpDlgEditAsset.getBlkDevice = cmpReq_GetPort;
  cmpDlgEditAsset.tDlg = tDlg;
  cmpDlgEditAsset.setDirtyBitChangeHandler =  cmpDlgEditAssetSetDirtyBitChangeHandler;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.setDirtyBitChangeHandler();
                 this.getBlkDevice(this.tDlg, this.ObjectNID, this.deviceUserJID);
                 }
               ),
               null, cmpDlgEditAsset );

  // event published from func ( cmpReq_GetPort ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompletePortDone", PortNID, this,
                      function() {
                        cmpDlgEditAssetGetAsset(PortNID);
                        //Publishing for get Select box List data afetr getting the device data
                        if (clsUserPermission.getPermission('User', 'read') == "true") {
                          topic_Publish(tDlg, "UserNameState", (newServerDate()).getTime());
                        }
                        else{
                          insertElementInSelectBox("NoReadPermission" , "No Read Permission", "selPortXAuthUser" );
                        }
                        topic_Publish(tDlg, "AssetResourceState", (newServerDate()).getTime());
                        topic_Publish(tDlg, "PortTagDefNameState", (newServerDate()).getTime());
                      }
                    );  

  // function to set dirtybit change function on the textboxes, select boxes
  // check boxes. Any value change witkk set the dirty bit true

  function cmpDlgEditAssetSetDirtyBitChangeHandler() {
 
    var objParent = cmpDlgEditAsset.BlkJSX.getParent();
    
    // find all descendant of type chkbox and set its toggle event
    var ChkBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.CheckBox, false);
    for(var i=0; i < ChkBoxNameArr.length; i++ ) {
      var ChkBlkJSX = ChkBoxNameArr[i];
      ChkBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.TOGGLE);
    }
      
    // find all descendant of type txtbox and set its change event
    var TxtBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.TextBox, false);
    for(var i=0; i < TxtBoxNameArr.length; i++ ) {
      var TxtBlkJSX = TxtBoxNameArr[i];
      var Name = TxtBoxNameArr[i].getName();
      var txtDirtyBitName = "txtDirtyBit" + TxtBoxNameArr[i].getParent().getName();
      if( Name != txtDirtyBitName) {
        TxtBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.CHANGE);
        //TxtBlkJSX.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
      }
    }

    // find all descendant of type select and set its select event
    var SelBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.Select, false);
    for(var i=0; i < SelBoxNameArr.length; i++ ) {
      var SelBlkJSX = SelBoxNameArr[i];
      SelBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.SELECT);
    }
  }  // end cmpDlgEditAssetSetDirtyBitChangeHandler

  // fnc to get dlg data
  function cmpDlgEditAssetGetAsset(PortNID) {
    var resourceTypeArr = [];
    
    //sub fnc to publich state
    function cmpDlgEditAssetFinishGetAsset(PortNID) {
      topic_Publish(tDlg, "EditAssetDataState", PortNID);
    }
    
    //sub function to process ResourceInstance Data
    function cmpDlgEditAssetProcessResourceInstance(profileUpNID, profileDownNID, ResourceInstanceNode, ResourceType) {
      var mediaUpSreamURL = "";
      var resourceType = ResourceType.replace(/_\w*/,"");     

      if( resourceType != "MediaStreamRelayPortResource") {
        mediaUpSreamURL  =  ResourceInstanceNode.getAttribute("streamURL");
        cmptxtaAssetPreConfigureInstance.BlkJSX.setValue(mediaUpSreamURL);
      } 
    }
      
    // sub fnc to process getresource data
    function cmpDlgEditAssetProcessResource(AssetNID, ResourceType, PortID) {
      if(PortID != null) {
        ResourceType = ResourceType + "_" + PortID;
      }
      var srcPortNode = getDialogDataNode(tDlg, "Port");
      //var srcPortID = srcPortNode.selectSingleNode(".//Port/@portId").getValue();
      var srcPortNodeNID = srcPortNode.getAttribute("NID");
      deviceResourceType = ResourceType;
      
      //set the resource nid in the text bos. will be used when we run rule
      // to the instance list
      if( ResourceType.replace(/_\w*/,"") != "MediaStreamRelayPortResource" ) {
        cmptxtAssetPreConfigureResourceNID.BlkJSX.setValue(srcPortNodeNID, true);          
      }
      // get the name
      var portName = srcPortNode.getAttribute("title");
      cmptxtPortName.BlkJSX.setValue(portName,true);
      //StreamType
      var portStreamType = srcPortNode.getAttribute("streamType");
      var deviceType = srcPortNode.selectSingleNode(".//Asset/@deviceType").getValue();
      if(portStreamType == "V2D MPEGTS RTP"){
        portStreamType = "V2D";
      }
      cmpselPortStreamType.BlkJSX.setValue(portStreamType,true);
      //PortType
      var UnmanagedState = srcPortNode.getAttribute("presence");
      var portType = srcPortNode.getAttribute("portType");
      cmpselPortType.BlkJSX.setValue(portType, true);
      if( ResourceType.replace(/_\w*/,"") == "MediaStreamIOPortResource" && UnmanagedState == "unmanaged" ) {
        cmpselPortType.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      }
      topic_Publish(tDlg, "PersistentPVRSelectionType", portType);
      // get the description
      var portDescription = srcPortNode.selectSingleNode("//Description").getValue();
      if(portDescription != null ) {
        cmptxtPortDescription.BlkJSX.setValue(portDescription, true);
      }
      var groupNode = srcPortNode.selectSingleNode(".//Groups").cloneNode(true); 
      // set the Tags data in the matrix
      var strXMLCacheId = cmpmtxPortTags.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(groupNode.toString());
      cmpmtxPortTags.BlkJSX.setSourceXML(objXML);        
      cmpmtxPortTags.BlkJSX.repaintData();

      if( jsx3.util.strEmpty(topic_Get(tDlg, "GetPVRRecordingLocationList"))) {
        topic_Publish(tDlg, "GetPVRRecordingLocationList", (newServerDate()).getTime());
      }
      var pvrInfo = srcPortNode.selectSingleNode(".//PVRInfo");
      if( !jsx3.util.strEmpty(pvrInfo)){
        var enablePVR = srcPortNode.selectSingleNode(".//PVRInfo/@enable").getValue();
        cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.setChecked("0");
        if(enablePVR == "true") {
          portConnectionStatus = true;
          cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.setChecked("1");
        }
        var trimPVRLength = srcPortNode.selectSingleNode(".//PVRInfo/@trimLength").getValue();
        if( !jsx3.util.strEmpty(trimPVRLength)){
          cmpPVRTrimLengthSetting.setPVRTrimLengthValues(trimPVRLength);
        }
        else{
          if(enablePVR == "" || enablePVR == "false") {
            cmpPVRTrimLengthSetting.cmpTxtSessionMinPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
            cmpPVRTrimLengthSetting.cmpTxtSessionHrPVRTrimLength.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
          }
          cmpPVRTrimLengthSetting.cmpTxtSessionMinPVRTrimLength.BlkJSX.setValue("");
          cmpPVRTrimLengthSetting.cmpTxtSessionHrPVRTrimLength.BlkJSX.setValue("");
        }

        var mediaStreamDirNID = srcPortNode.selectSingleNode(".//PVRInfo/@mediaStreamDirResourceNID").getValue();
        topic_Set(tDlg, "PVRRecordingLocationSelection", mediaStreamDirNID);
      }

//      if( jsx3.util.strEmpty(topic_Get(tDlg, "GetProfileList"))) {
//        topic_Publish(tDlg, "GetProfileList", (newServerDate()).getTime());
//      }
      var profileXMLNode = srcPortNode.selectSingleNode(".//InstanceList/Instance/ProfileXML");
      if( !jsx3.util.strEmpty(profileXMLNode)){
        var nodeIter = profileXMLNode.selectNodeIterator("./*");
        while (nodeIter.hasNext()) {
          var nodeNode = nodeIter.next();
          nodeNode.setAttribute("Dirty", "CLEAN");
        }
        var autoStartEnable = srcPortNode.selectSingleNode(".//Instance/@mode").getValue();
        cmpchkAutoStart.BlkJSX.setChecked("0");
        var maskMode = jsx3.gui.Block.DISPLAYBLOCK;
        if(autoStartEnable == "AutoStart") {
          cmpchkAutoStart.BlkJSX.setChecked("1");
          maskMode = jsx3.gui.Block.DISPLAYNONE;
        }
        cmpBlkAutoStartSettingsMask.BlkJSX.setDisplay(maskMode, true);
        var strXMLCacheId = cmpCDFEditAssetStartSettings.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
        objXML.loadXML(srcPortNode.toString());
        cmpCDFEditAssetStartSettings.BlkJSX.setSourceXML(objXML);         
        cmpCDFEditAssetStartSettings.BlkJSX.setCDFId(srcPortNodeNID);
      }
      
      // get the ForceXAuth
      //need to repaint to load select lists with users/Resourcees
      topic_Publish(tDlg, "ForceXAuthDataState", (new Date));

      var forceXAuth = srcPortNode.selectSingleNode(".//ObserverInfo/Observer/@xAuth").getValue();
      cmpchkPortXAuth.BlkJSX.setChecked("0");
      if(forceXAuth == "true") {
        cmpchkPortXAuth.BlkJSX.setChecked("1");
      }
      var observerMonitor = srcPortNode.selectSingleNode(".//ObserverInfo/Observer/@monitor").getValue();
      cmpchkPortMonitor.BlkJSX.setChecked("0");
      if(observerMonitor == "true") {
        cmpchkPortMonitor.BlkJSX.setChecked("1");
      }
      // get the XAuth User and Resource
      userNIDValue = srcPortNode.selectSingleNode(".//ObserverInfo/Observer/@userNID").getValue();          
      xAuthResourceNID = srcPortNode.selectSingleNode(".//ObserverInfo/Observer/@resourceNID").getValue();

      // iter over the InstanceInfo/InstanceList/RespurceInstance
      var resourceInstanceIter = srcPortNode.selectNodeIterator("./InstanceList/Instance");
      if(!resourceInstanceIter.hasNext()) {
        var instanceInfo = srcPortNode.selectSingleNode(".//InstanceList");
        cmpDlgEditAssetProcessResourceInstance(null, null, instanceInfo, ResourceType);
      } else {
        while(resourceInstanceIter.hasNext()) {
          var resourceInstanceNode = resourceInstanceIter.next();
          cmpDlgEditAssetProcessResourceInstance(null, null, resourceInstanceNode, ResourceType);
        }
      }
      var empty = true;
      for(var i=0; i<resourceTypeArr.length; i++) {
        if( resourceTypeArr[i] != null ) {
          if( resourceTypeArr[i].objtype == ResourceType ) {
            resourceTypeArr[i] = null;
          }
          else {
            empty = false;
          }
        }
      }
      if( empty == true ) {
        cmpDlgEditAssetFinishGetAsset(PortNID);
      }
    } //end cmpDlgEditAssetProcessResource()
 
    //sub func to set default for drop down
    function setDeafultsForStreamProfile(streamType) {
      // publish left hand selection 
      topic_Publish(tDlg ,"EditAssetPreConfigSelection","PortInfo");
      // disbale streamType drop down
      cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      //set the stream type for the selAssetPreConfigureStreamType based on the selected asset type
      // selAssetPreConfigureStreamType for other respirce ports
      if( streamType == "V2D" ) {
        topic_Publish(tDlg, "EditBlkAutoStart", streamType);
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("V2D");
      } else if( streamType == "MPEGTS" ) {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("MPEGTS");
      } else if( streamType == "RTP" ) {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("RTP");
      } else if( streamType == "RTP-RAW" ) {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("RTP-RAW");
      } else if( streamType == "RTP-PCM" ) {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("RTP-PCM");
      } else if( streamType == "RFB" ) {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("RFB");
      } else if( streamType == "UDP" ) {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("UDP");
      } else {
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        cmpSelEditAssetPreConfigurePortStreamType.BlkJSX.setValue("V2D");
      }
    }; // end  setDeafultsForStreamProfile         
    
    // main fnc code for asset data        
    
    /********************** !!!!!!!!!!! ***********/  
    var reqStatus = topic_Get(tDlg, "GetCompletePortStatus");
    if(reqStatus != "SUCCESS") {
      dlgEditAsset_Destroy();  
      return;
    }
    
    var portNode = getDialogDataNode(tDlg, "Port");
    var AssetName = portNode.selectSingleNode(".//Asset/@title").getValue();
    var AssetType = portNode.selectSingleNode(".//Asset/@deviceType").getValue();
    var portId = portNode.getAttribute("portId");
    var streamType = portNode.getAttribute("streamType");
    var portType = portNode.getAttribute("portType");
    var state = portNode.getAttribute("state");
    var nid = portNode.getAttribute("NID");
    setDeafultsForStreamProfile(streamType);
    if(!jsx3.util.strEmpty(portNode.selectSingleNode(".//PVRInfo")) && isMNADevice != "yes") {
      topic_Publish(tDlg, "EditBlkPVRSettings", portNode.selectSingleNode(".//PVRInfo"));
    }
    if(portType == "StreamSrc"){
      if(nid.indexOf("mediastreamsrcportresource")!=-1 && isMNADevice != "yes"){
        topic_Publish(tDlg, "EditBlkPreConfigSrcPort", portType);
      }
      resourceTypeArr.push({objtype:"MediaStreamSrcPortResource"});           
    }
    else if(portType == "StreamDst"){
      if(streamType != "V2D MPEGTS RTP" && nid.indexOf("mediastreamdstportresource")!=-1 && isMNADevice != "yes"){
        topic_Publish(tDlg, "EditBlkPreConfigDstPort", portType);
      }
      resourceTypeArr.push({objtype:"MediaStreamDstPortResource"});
    }
    if( portId == "1" && selectedPort == "1" && nid.indexOf("mediastreamioportresource")!=-1 && isMNADevice != "yes"){
      topic_Publish(tDlg, "EditBlkPreConfigIOPort0", selectedPort);
      resourceTypeArr.push({objtype:"MediaStreamIOPortResource_1"});
    } 
    else if(portId == "2" &&  selectedPort == "2" && isMNADevice != "yes") {
      topic_Publish(tDlg, "EditBlkPreConfigIOPort1", selectedPort);
      resourceTypeArr.push({objtype:"MediaStreamIOPortResource_2"});
    }
    else if (portId == "3" &&  selectedPort == "3" && isMNADevice != "yes") {
      resourceTypeArr.push({objtype: "MediaStreamIOPortResource_3" });
    }
    else if (portId == "4" &&  selectedPort == "4" && isMNADevice != "yes") {
      resourceTypeArr.push({objtype: "MediaStreamIOPortResource_4" });
    }
    else if (portId == "5" &&  selectedPort == "5" && isMNADevice != "yes") {
      resourceTypeArr.push({objtype: "MediaStreamIOPortResource_5" });
    }
    for(var i=0;i<resourceTypeArr.length;i++){
      var resourceType = resourceTypeArr[i].objtype.replace(/_\w*/,"");
      var portID = resourceTypeArr[i].objtype.replace(/\w*_/,"");
      if( portID == resourceType) {
        portID = null;
      }      
      cmpDlgEditAssetProcessResource(AssetNID, resourceType, portID);
    }    
        
  }// end get dlg data function


  //Basic Validation  
  function cmpDlgEditAssetPreConfigureValidate() {
    // check for Port Name should not be empty
    var portName = jsx3.util.strTrim(cmptxtPortName.BlkJSX.getValue());
    if(jsx3.util.strEmpty(portName)) {
      alert("Please enter Port Name");
      cmptxtPortName.BlkJSX.focus();
      return false;
    }
    var portNode = getDialogDataNode(tDlg, "Port");
    if(!jsx3.util.strEmpty(portNode.selectSingleNode(".//PVRInfo"))) {
        if(cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.getChecked()){
          var trimLength = cmpPVRTrimLengthSetting.validatePVRTrimLength();
          if(trimLength == false){
            return false;
          }
          var pvrLocationJIDVal = cmpselPVRRecordingLocation.BlkJSX.getValue();
          if(pvrLocationJIDVal == null) {
            alert("Please select PVR Recording Location");
            return false;
          }
        }
    }
    return true;
  }

  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetSetAsset(AssetNID) {
    if( cmpDlgEditAssetPreConfigureValidate() != true ) return;
    var FlagHandlePortInfo = false;
    var FlagHandleObserverInfo = false;
    var FlagHandleStreamURL = false;
    var FlagHandlePortInfoRequest = false;
    var FlagHandleObserverInfoRequest = false;
    var FlagHandleStreamURLRequest = false;
    var FlagHandlePVRInfoRequest = false;
    var FlagHandlePVRInfo = false;
    var FlagHandleAutoStartRequest = false;
    var FlagHandleAutoStart = false;

    // helper function when all the resource pointers are edited then edit AssetResource fnc is called
    function cmpDlgEditAssetEditFinishResourcePointer(objectNID, objectType, portID) {
      if(portID != null) {
        objectType = objectType + "_" + portID;  
      }           
      var empty = true;
      if( (PortNID == objectNID) && ( objectType == deviceResourceType) ) {
        deviceResourceType = "";
      } else {
        empty = false;
      }
      if( empty == true ) {
        topic_Publish(tDlg, "EditAssetSetAssetState", "DONE");
      }         
    } // end cmpDlgEditAssetEditFinishResourcePointer    
    
    function updateMediaPortResource(ObjectNID, ObjectType, ObjectNode, requestName, handleType){
      var reqSetResourceObject = new ReqSessionGetRequest(requestName, 
                                                    {action:"FNCCALL", context:this, 
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                                var resNID = respVal.selectSingleNode(".//Port/@NID").getValue();
                                                                if( resNID!= null) {
                                                                  var portID = null;
                                                                  var resourceNode = getDialogPortResourceDataNode(tDlg, resNID);
                                                                  if(!jsx3.util.strEmpty(resourceNode)){
                                                                    var deviceType = resourceNode.selectSingleNode(".//Asset/@deviceType").getValue();
                                                                    if( resourceNode.getAttribute("NID").indexOf("mediastreamioportresource")!=-1 && isMNADevice != "yes") {
                                                                        portID = resourceNode.getAttribute("portId");
                                                                    }
                                                                    if(handleType == "PortInfo"){
                                                                      FlagHandlePortInfo = true;
                                                                    }
                                                                    if(handleType == "ObserverInfo"){
                                                                      FlagHandleObserverInfo = true;
                                                                    }
                                                                    if(handleType == "StreamURL"){
                                                                      FlagHandleStreamURL = true;
                                                                    }
                                                                    if(handleType == "PVRInfo"){
                                                                      FlagHandlePVRInfo = true;
                                                                    }
                                                                    if(handleType == "AutoStart"){
                                                                      FlagHandleAutoStart = true;
                                                                    }
                                                                    if(FlagHandlePortInfo == true && FlagHandleObserverInfo == true && FlagHandleStreamURL == true && FlagHandlePVRInfo == true && FlagHandleAutoStart == true){
                                                                      cmpDlgEditAssetEditFinishResourcePointer(resNID, ObjectType, portID);
                                                                    }
                                                                  }
                                                                }else {
                                                                  alert("Internal error: Resource added but no NID returned");
                                                                  return false;
                                                                }
                                                             }
                                                             else if(respCode == "5206" ) {
                                                               FlagHandlePortInfo = false;
                                                               FlagHandleObserverInfo = false;
                                                               FlagHandleStreamURL = false;
                                                               FlagHandlePVRInfo = false;
                                                               alert("Duplicate Port name. Please enter some other Port name");
                                                               return false;
                                                             }                                                                
                                                             else {
                                                               try{
                                                                 var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               } catch(e){
                                                               }
                                                               FlagHandlePortInfo = false;
                                                               FlagHandleObserverInfo = false;
                                                               FlagHandleStreamURL = false;
                                                               FlagHandlePVRInfo = false;
                                                               var ErrorMsg = "Error: Device->Resource add request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                         }
                                                    });
      reqSetResourceObject.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_ByNID"] = ObjectNID;
      reqParams["ObjectList_ByTitle"] = ObjectType;
      if(FlagHandlePortInfoRequest){
        FlagHandlePortInfoRequest = false;
        reqParams["ObjectList_Title"] = ObjectNode.getAttribute("title");
        reqParams["ObjectList_description"] = ObjectNode.selectSingleNode("//Description").getValue();
        var requestDataObjectArr = new Object;
        requestDataObjectArr["Groups"] = ObjectNode.selectSingleNode(".//Groups");
        reqSetResourceObject.requestDataObjectArr = requestDataObjectArr;
      }
      if(FlagHandleObserverInfoRequest){
        FlagHandleObserverInfoRequest = false;
        reqParams["ObjectList_XAuth"] = ObjectNode.selectSingleNode(".//ObserverInfo/Observer/@xAuth").getValue();
        reqParams["ObjectList_Monitor"] = ObjectNode.selectSingleNode(".//ObserverInfo/Observer/@monitor").getValue();
        reqParams["ObjectList_UserNID"] = ObjectNode.selectSingleNode(".//ObserverInfo/Observer/@userNID").getValue();
        reqParams["ObjectList_ResourceNID"] = ObjectNode.selectSingleNode(".//ObserverInfo/Observer/@resourceNID").getValue();
      }
      if(FlagHandleStreamURLRequest){
        FlagHandleStreamURLRequest = false;
        reqParams["ObjectList_StreamURL"] = ObjectNode.selectSingleNode(".//InstanceList/Instance/@streamURL").getValue();
      }
      if(FlagHandlePVRInfoRequest){
        FlagHandlePVRInfoRequest = false;
        reqParams["ObjectList_Enable"] = ObjectNode.selectSingleNode(".//PVRInfo/@enable").getValue();
        reqParams["ObjectList_TrimLength"] = ObjectNode.selectSingleNode(".//PVRInfo/@trimLength").getValue();
        reqParams["ObjectList_MediaStreamDirResourceNID"] = ObjectNode.selectSingleNode(".//PVRInfo/@mediaStreamDirResourceNID").getValue();
      }
      if(FlagHandleAutoStartRequest){
        FlagHandleAutoStartRequest = false;
        reqParams["ObjectList_Enable"] = cmpchkAutoStart.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ? true : false;
        var requestDataObjectArr = new Object;
        requestDataObjectArr["ProfileXML"] = ObjectNode.selectSingleNode(".//ProfileXML");
        reqSetResourceObject.requestDataObjectArr = requestDataObjectArr;
      }
      reqSetResourceObject.reqParams = reqParams;
      reqSetResourceObject.sendRequest(); 
    }

    // helper function to send edited SrcPortResource data    
    function cmpDlgEditAssetEditRequestResourcePointer(objectNID, objectType) {
        var orgResourceNode = getDialogPortResourceDataNode(tDlg, objectNID);
        if( orgResourceNode == null ) return;
        
        if ( (objectType == "MediaStreamIOPortResource_1") ||
             (objectType == "MediaStreamIOPortResource_2") ||
             (objectType == "MediaStreamIOPortResource_3") ) {
          objectType = objectType.replace(/_\w*/,"");
        }
        //get the stream URL from the rule
        var streamURL = cmptxtaAssetPreConfigureInstance.BlkJSX.getValue();
        if( !jsx3.util.strEmpty(streamURL) ) {
          //set the value in the original node which will be sent in the request
          orgResourceNode.selectSingleNode(".//InstanceList/Instance/@streamURL").setValue(streamURL);       
        }
        else {
          orgResourceNode.selectSingleNode(".//InstanceList/Instance/@streamURL").setValue("");       
        }
        
        // set the Port Name
        var portName = cmptxtPortName.BlkJSX.getValue();   
        orgResourceNode.setAttribute("title", portName);
        // set the Port Description
        var portDesc = cmptxtPortDescription.BlkJSX.getValue();   
        orgResourceNode.selectSingleNode("//Description").setValue(portDesc);
        //set StreamType
        var portStreamType =cmpselPortStreamType.BlkJSX.getValue();
        orgResourceNode.setAttribute("streamType", portStreamType); 
        //set portType
        var portType = cmpselPortType.BlkJSX.getValue();   
        orgResourceNode.setAttribute("type", portType); 
        //set portGroups by converting the matrix data to Tag nodes
        var strXMLCacheId = cmpmtxPortTags.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
        var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFTagToTag.xsl","TagNode","xsl");
        var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
        var newGroupsXML = xslDoc.transform(objXML);        
        var newGroupsNode = new jsx3.xml.Document().loadXML(newGroupsXML);
        var oldGroupsNode = orgResourceNode.selectSingleNode(".//Groups");
        var groupParentNode = orgResourceNode.selectSingleNode(".//Groups").getParent();
        groupParentNode.replaceNode(newGroupsNode,oldGroupsNode);

        var trimPVRLength = cmpPVRTrimLengthSetting.getEnablePVRTrimLength();  
        orgResourceNode.selectSingleNode(".//PVRInfo").setAttribute("trimLength", trimPVRLength);
        var enablePVRBlkJSX = cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX;
        orgResourceNode.selectSingleNode(".//PVRInfo").setAttribute("enable", "false");
        if(enablePVRBlkJSX.getChecked()) {
         orgResourceNode.selectSingleNode(".//PVRInfo").setAttribute("enable", "true");
        }
        var pvrLocationBlkJSX = cmpselPVRRecordingLocation.BlkJSX;
        var pvrLocationJIDVal = pvrLocationBlkJSX.getValue();
        if(pvrLocationJIDVal != null) {
          orgResourceNode.selectSingleNode(".//PVRInfo").setAttribute("mediaStreamDirResourceNID", pvrLocationJIDVal);
        }

        // set the ForceXAuth
        var forceXAuthBlkJSX = cmpchkPortXAuth.BlkJSX;
        orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("xAuth", "false");
        if(forceXAuthBlkJSX.getChecked()) {
         orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("xAuth", "true");
        }
        var monitorBlkJSX = cmpchkPortMonitor.BlkJSX;
        orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("monitor", "false");
        if(monitorBlkJSX.getChecked()) {
         orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("monitor", "true");
        }
        // get the XAuthOperatorList          
        var userJIDBlkJSX = cmpSelPortXAuthUser.BlkJSX;
        var resourceNIDBlkJSX = cmpSelPortXAuthResource.BlkJSX;
        
        var userJIDVal = userJIDBlkJSX.getValue();
        if(userJIDVal != null) {
          var mtachResult = userJIDVal.search(/Select /);

          if(mtachResult != -1){
            userJIDVal = null;
          }
        }
          
        var resourceVal = resourceNIDBlkJSX.getValue();
        if(resourceVal!=null) {
          var mtachResult = resourceVal.search(/Select /);
          if(mtachResult != -1){
            resourceVal = null;
          }          
        }  
        if(userJIDVal != null) {
          orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("userNID", userJIDVal);
        }
        else{
          userJIDVal = "";
          orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("userNID", "");
        }
        if(resourceVal != null) {
          orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("resourceNID", resourceVal);
        }
        else{
          resourceVal = "";
          orgResourceNode.selectSingleNode(".//ObserverInfo/Observer").setAttribute("resourceNID", "");
        }
        
        var enableProfile = cmpchkAutoStart.BlkJSX;
        if(enableProfile.getChecked()) {
         orgResourceNode.selectSingleNode(".//Instance").setAttribute("mode", "AutoStart");
        }
        
        cmpCDFEditAssetStartSettings.BlkJSX.write();

        //get the device System data
        var strXMLCacheId = cmpCDFEditAssetStartSettings.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
        var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetPortsAutoStartRequest.xsl","EditAssetPortsAutoStartRequest","xsl");
        // genrate the request xml
        var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
        var profileXML = xslDoc.transform(objXML);
        var profileXMLNode = new jsx3.xml.Document();
        profileXMLNode.loadXML(profileXML);

        var orginalNode = getDialogPortResourceDataNode(tDlg, objectNID);
        var newTitleName = cmptxtPortName.BlkJSX.getValue();   
        var oldTitleName = orginalNode.getAttribute("title");
        var newDesc = cmptxtPortDescription.BlkJSX.getValue();
        var oldDesc = orginalNode.selectSingleNode("//Description").getValue();
        var groupsNode = orginalNode.selectSingleNode("./Groups");
        if( newTitleName != oldTitleName || newDesc != oldDesc || changeInGroupTags(groupsNode,cmpmtxPortTags.BlkJSX)){
          FlagHandlePortInfoRequest = true;
          updateMediaPortResource(objectNID, objectType, orgResourceNode, "SetPortInfo", "PortInfo"); 
        }
        else{
          FlagHandlePortInfo = true;
        }

        var selectedPortType = topic_Get(tDlg, "PersistentPVRSelectionType");
        if(selectedPortType != "StreamDst"){
          var newPVREnableBlkJSX = cmpPVRTrimLengthSetting.chkEnablePVR.BlkJSX.getChecked();
          if( newPVREnableBlkJSX == "true" || newPVREnableBlkJSX == 1){
            newPVREnableBlkJSX = 1;
          }
          else{
            newPVREnableBlkJSX = 0;
          }
          var oldPVREnableBlkJSX = orginalNode.selectSingleNode(".//PVRInfo/@enable").getValue();
          if( oldPVREnableBlkJSX == "true"){
            oldPVREnableBlkJSX = 1;
          }
          else{
            oldPVREnableBlkJSX = 0;
          }
          var newTrimLength = cmpPVRTrimLengthSetting.getEnablePVRTrimLength();
          var oldTrimLength = orginalNode.selectSingleNode(".//PVRInfo/@trimLength").getValue();

          var newMediaDirNID = cmpselPVRRecordingLocation.BlkJSX.getValue();
          var oldMediaDirNID = orginalNode.selectSingleNode(".//PVRInfo/@mediaStreamDirResourceNID").getValue();
          if( newPVREnableBlkJSX != oldPVREnableBlkJSX || (!jsx3.util.strEmpty(oldTrimLength) && newTrimLength != oldTrimLength) || (!jsx3.util.strEmpty(newMediaDirNID) && !jsx3.util.strEmpty(oldMediaDirNID) && newMediaDirNID != oldMediaDirNID)){
            FlagHandlePVRInfoRequest = true;
            updateMediaPortResource(objectNID, objectType, orgResourceNode, "SetPortPVRInfo", "PVRInfo"); 
          }
          else{
            FlagHandlePVRInfo = true;
          }
        }
        else{
          FlagHandlePVRInfo = true;
        }

        var newForceXAuthBlkJSX = cmpchkPortXAuth.BlkJSX.getChecked();
        if( newForceXAuthBlkJSX == "true" || newForceXAuthBlkJSX == 1){
          newForceXAuthBlkJSX = 1;
        }
        else{
          newForceXAuthBlkJSX = 0;
        }
        var newMonitorBlkJSX = cmpchkPortMonitor.BlkJSX.getChecked();
        if( newMonitorBlkJSX == "true" || newMonitorBlkJSX == 1){
          newMonitorBlkJSX = 1;
        }
        else{
          newMonitorBlkJSX = 0;
        }
        var oldForceXAuthBlkJSX = orginalNode.selectSingleNode(".//ObserverInfo/Observer/@xAuth").getValue();
        if( oldForceXAuthBlkJSX == "true"){
          oldForceXAuthBlkJSX = 1;
        }
        else{
          oldForceXAuthBlkJSX = 0;
        }
        var oldMonitorBlkJSX = orginalNode.selectSingleNode(".//ObserverInfo/Observer/@monitor").getValue();
        if( oldMonitorBlkJSX == "true"){
          oldMonitorBlkJSX = 1;
        }
        else{
          oldMonitorBlkJSX = 0;
        }
        var oldUserJIDVal = orginalNode.selectSingleNode(".//ObserverInfo/Observer/@userNID").getValue();
        var oldReourceVal = orginalNode.selectSingleNode(".//ObserverInfo/Observer/@resourceNID").getValue();
        if( newMonitorBlkJSX!=oldMonitorBlkJSX || newForceXAuthBlkJSX != oldForceXAuthBlkJSX || userJIDVal != oldUserJIDVal || resourceVal != oldReourceVal){
          FlagHandleObserverInfoRequest = true;
          updateMediaPortResource(objectNID, objectType, orgResourceNode, "SetPortObserverInfo", "ObserverInfo"); 
        }
        else{
          FlagHandleObserverInfo = true;
        }
        var newStreamURL = cmptxtaAssetPreConfigureInstance.BlkJSX.getValue();
        var oldStreamURL = orginalNode.selectSingleNode(".//InstanceList/Instance/@streamURL").getValue();
        if(newStreamURL != oldStreamURL){
          FlagHandleStreamURLRequest = true;
          updateMediaPortResource(objectNID, objectType, orgResourceNode, "SetPortStreamURL", "StreamURL"); 
        }
        else{
          FlagHandleStreamURL = true;
        }
        
        var newAutoStartBlkJSX = cmpchkAutoStart.BlkJSX.getChecked();
        if( newAutoStartBlkJSX == "true" || newAutoStartBlkJSX == 1){
          newAutoStartBlkJSX = 1;
        }
        else{
          newAutoStartBlkJSX = 0;
        }
        var oldAutoStartBlkJSX = orginalNode.selectSingleNode(".//Instance/@mode").getValue();
        if( oldAutoStartBlkJSX == "AutoStart"){
          oldAutoStartBlkJSX = 1;
        }
        else{
          oldAutoStartBlkJSX = 0;
        }
        var profileXMLChanges = false;
        var newProfileXMLNode = profileXMLNode.selectSingleNode("//ProfileXML");
        var modifyNode = newProfileXMLNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
        if(modifyNode != null) {
          profileXMLChanges = true;  
        }
        else{
          var v2dProfileNID = topic_Get(tDlg, "AutoStartSelection");
          if(!jsx3.util.strEmpty(v2dProfileNID)){
            profileXMLChanges = true;
          }
        }
        if(newAutoStartBlkJSX != oldAutoStartBlkJSX || profileXMLChanges == true){
          FlagHandleAutoStartRequest = true;
          var orgProfileXMLNode = orgResourceNode.selectSingleNode("//ProfileXML");
          var parentNode = orgResourceNode.selectSingleNode(".//ProfileXML").getParent();
          parentNode.replaceNode(newProfileXMLNode, orgProfileXMLNode);        
          updateMediaPortResource(objectNID, objectType, orgResourceNode, "SetPortAutoStart", "AutoStart"); 
        }
        else{
          FlagHandleAutoStart = true;
        }
        if(FlagHandlePortInfo == true && FlagHandleObserverInfo == true && FlagHandleStreamURL == true && FlagHandlePVRInfo == true && FlagHandleAutoStart == true){
          topic_Publish(tDlg, "EditAssetSetAssetState", "DONE");
        }
    } // end cmpDlgEditAssetEditRequestResourcePointer    
    
    cmpDlgEditAssetEditRequestResourcePointer(PortNID, deviceResourceType);

  } // end cmpDlgEditAssetSetAsset
} // end create


