// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceInfo_Create(AssetNID, selectedAssetType, selectedAssetName, selectedAssetStatus, displaySelectedAssetType) {   
  // deal with defaults for add
  if( AssetNID == null ) AssetNID = "DEFAULT_ASSETRESOURCE_NID";

  // load view - dialog component file
  var DlgFile = "components/assets/dlgAddNewDevice.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  //var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);

  function dlgEditAsset_Destroy() {
    discoverDeviceAgent.unsubscribe(com.ipvs.agent.DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE, this);
    discoverDeviceAgent.unsubscribe(com.ipvs.agent.DeviceDiscoveryAgent.ON_DEVICE_GETDATA, this);
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditAsset.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }

  topic_Set(tDlg, "AssetNID", AssetNID);
  
  var dialogTitle = "&nbsp;&nbsp;Add Device";
  if( AssetNID != "DEFAULT_ASSETRESOURCE_NID" ){
    dialogTitle = "&nbsp;&nbsp;Edit Device";
  }
  var cmpBlkDialogWindowBar = new Component([BlkJSX,"windowBar"]);
  cmpBlkDialogWindowBar.BlkJSX.setText(dialogTitle, true);
  closeDialogIfExists(dialogTitle);

  var mtxNewStrXMLCacheId = "mtxDoc" + tDlg;
  getJsxByName([BlkJSX, "mtxDeviceDiscovery"]).setXMLId(mtxNewStrXMLCacheId);
                        
  topic_Set(tDlg, "AddEditDeviceAdvanceSettings", "HIDE");
  topic_Set(tDlg, "DeviceAddType", "NONE");
  topic_Set(tDlg, "AssetTypeSelection", "NONE");
  topic_Set(tDlg, "AddEditDeviceStage", "NONE");
  topic_Set(tDlg, "EditAssetAssetTypeSelection", "NONE");
  topic_Set(tDlg, "DeviceDiscoverySelected", "NONE");
  topic_Set(tDlg, "TagNameState", "NONE");
  topic_Set(tDlg, "DeviceAgentJID", "NONE");
  topic_Set(tDlg, "EditAssetAssetPortID", "1");
  topic_Set(tDlg, "EditAssetAssetListIndex", "1");
  topic_Set(tDlg, "LabelAddDevice", "");
  topic_Set(tDlg, "LabelUpdateConfig", "");
  topic_Set(tDlg, "LabelSuccess", "");
  topic_Set(tDlg, "SavingStatus", "SHOW");
  topic_Set(tDlg, "EditAssetAssetEnable", "true");
  topic_Set(tDlg, "EditAssetAssetPrimary", "true");
  topic_Set(tDlg, "ReRegisterAssetTypeSelection", "NONE");
  topic_Set(tDlg, "AssetUnmanage", "NONE");
  topic_Set(tDlg, "AssetPasswordHelpTextSelection", "NONE");
  topic_Set(tDlg, "EditAssetSelectedAssetStatus", selectedAssetStatus);

  topic_Set(tDlg, "AddDeviceSavingStatus", "NONE");
  topic_Set(tDlg, "UpdateConfigSavingStatus", "NONE");
  topic_Set(tDlg, "SuccessSavingStatus", "NONE");
  topic_Set(tDlg, "LabelNumberofDevices", "0");
  topic_Set(tDlg, "LabelDeviceDiscoveryType", "Device Discovery");
  
  topic_Set(tDlg, "NewAssetNID", "NONE");

  var Str_LabelAddDevice_Inprogress = "Adding device in the database...";
  var Str_LabelAddDevice_Success = "Successfully added device in the database.";
  var Str_LabelAddDevice_Error = "Error adding device in the database.";

  var Str_LabelUpdateConfig_Inprogress = "Sent device configuration request.";
  var Str_LabelUpdateConfig_Success = "Successfully updated device configuration.";
  var Str_LabelUpdateConfig_Error = "Error updating device configuration.";

  var Str_LabelSuccess_Inprogress = "Waiting for the device to register with management server.";
  var Str_LabelSuccess_Success = "Device successfully registered with management server.";
  var Str_LabelSuccess_Error = "Error Registering device with management server."; 
   
  var deviceTypeListNode = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/selections/DeviceType.xml");
  var defaultAssetDeviceNode = loadXMLFile("JSXAPPS/IPVS-Common/model/objectXml/AssetResource.xml");

  var cmpImgDeviceDiscovery = new CmpImageButton([BlkJSX, "imgDeviceDiscovery"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function () {
                                                     SetAddDeviceType("DISCOVERY", "Discovery");
                                                     topic_Set(tDlg, "DiscoveryListViewType", "UnRegistered");
                                                     var selVal = cmpSelDiscoveryFilterBy.BlkJSX.getValue();
                                                     if (!jsx3.util.strEmpty(selVal)) {
                                                       cmpSelDiscoveryFilterBy.BlkJSX.setValue("UnRegistered");
                                                       cmpTxtDiscoveryFilterBy.BlkJSX.setValue("Filter by keyword...");
                                                       topic_Publish(tDlg, "AssetTypeSelection", "NONE");
                                                     }
                                                     topic_Set(tDlg, "DiscoveryListFilterKeyword", "");
                                                     
                                                     jsx3.sleep((function () {
                                                       discoverDeviceAgent.startDiscoveryService();
                                                     }
                                                     ),
                                                     null, this);

                                                   }
                                                 }
                                                );

  var cmpImgMS = new CmpImageButton([BlkJSX, "imgMS"], null,
                                     { action: "FNCCALL",
                                       context: null,
                                       fnc: function () {
                                              SetAddDeviceType("BASIC", "ManagementServer");
                                       }
                                     }
                                    );
                                                  
  var cmpImgDMS = new CmpImageButton([BlkJSX, "imgDMS"], null,
                                     { action: "FNCCALL",
                                       context: null,
                                       fnc: function () {
                                              SetAddDeviceType("BASIC", "DigitalMediaServer");
                                       }
                                     }
                                    );
                                                
  var cmpImgEncDec = new CmpImageButton([BlkJSX, "imgV2D"], null,
                                       { action: "FNCCALL",
                                         context: null,
                                         fnc: function () {
                                                SetAddDeviceType("BASIC", "Encoder/Decoder");
                                         }
                                       }
                                      );
                                                  
  var cmpImgClient = new CmpImageButton([BlkJSX, "imgClient"], null,
                                       { action: "FNCCALL",
                                         context: null,
                                         fnc: function () {
                                                SetAddDeviceType("BASIC", "Client");
                                         }
                                       }
                                      );
                                                
  var cmpImg3rdParty = new CmpImageButton([BlkJSX, "img3rdParty"], null,
                                         { action: "FNCCALL",
                                           context: null,
                                           fnc: function () {
                                                  SetAddDeviceType("BASIC", "UnmanagedEncoder/Decoder");
                                           }
                                         }
                                        );     
                                                                                                                                                                                                                                                                                                                                                
  var cmpBlkAssetAddType = new CmpContainer([BlkJSX,"blkDeviceType"],
                                             [ {cmp:cmpImgDeviceDiscovery},
                                               {cmp:cmpImgMS},
                                               {cmp:cmpImgDMS},
                                               {cmp:cmpImgEncDec},
                                               {cmp:cmpImgClient},
                                               {cmp:cmpImg3rdParty}
                                              ],
                                              null);

  var cmpMtxDeviceDiscovery = new CmpList([BlkJSX, "mtxDeviceDiscovery"], 
                                            null, null, null,
                                            { persist: { col: "jsxid", initval: false },
                                              action: "TOPICEVENT", topic: tDlg, eventvar: "DeviceDiscoverySelected", eventval: "jsxid"},
                                            null, null, null
                                           );

  var cmpImgBtnRefresh = new Component([BlkJSX, "imgBtnRefresh"]);

  var cmpSelDiscoveryFilterBy = new CmpStaticList([BlkJSX, "selDiscoveryFilterBy"], "xml/selections/DiscoveryType.xml", null,
                                                  { action: "FNCCALL", context: null,
                                                    fnc: function () {
                                                      var selVal = cmpSelDiscoveryFilterBy.BlkJSX.getValue();
                                                      if (!jsx3.util.strEmpty(selVal)) {
                                                        topic_Set(tDlg, "DiscoveryListViewType", selVal);
                                                        topic_Publish(tDlg, "AssetTypeSelection", "NONE");
                                                        discoverDeviceAgent.applyFilterForDiscoveryList();
                                                      }
                                                      else {
                                                        //default
                                                        cmpSelDiscoveryFilterBy.BlkJSX.setValue("UnRegistered");
                                                        topic_Set(tDlg, "DiscoveryListViewType", "UnRegistered");
                                                      }
                                                    }
                                                  });

  var cmpTxtDiscoveryFilterBy = new CmpTxtBoxWithEnterKey([BlkJSX, "txtDiscoveryFilterBy"], "Filter by keyword...", null, null,
                                                          { action: "FNCCALL",
                                                             context: this,
                                                             fnc: function () {
                                                               topic_Set(tDlg, "DiscoveryListFilterKeyword", cmpTxtDiscoveryFilterBy.BlkJSX.getValue());
                                                               discoverDeviceAgent.applyFilterForDiscoveryList();
                                                             }
                                                          }, "");

  var cmpBtnClear = new CmpImageButton([BlkJSX, "imgBtnClear"], null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function () {
                                            cmpTxtDiscoveryFilterBy.BlkJSX.setValue("Filter by keyword...");
                                            cmpTxtDiscoveryFilterBy.onFocusVal = "";
                                            topic_Set(tDlg, "DiscoveryListFilterKeyword", "");
                                            discoverDeviceAgent.applyFilterForDiscoveryList();
                                          }
                                        });

  var cmpLblBtnRefreshHelpText = new Component([BlkJSX, "lblBtnRefreshHelpText"]);

  var cmpBtnRefreshDeviceDiscovery = new CmpImageButton([BlkJSX, "btnRefreshDiscovery"], null,
                                                        { action: "FNCCALL",
                                                          context: null,
                                                          fnc: function () {
                                                            if ( topic_Get(tDlg, "DeviceDiscoverySelected")  != "NONE") {
                                                              topic_Set(tDlg, "EditAssetAssetPortID", "1");
                                                              topic_Set(tDlg, "EditAssetAssetListIndex", "1");
                                                              topic_Set(tDlg, "EditAssetAssetTypeSelection", "NONE");
                                                              topic_Set(tDlg, "AddEditDeviceStage", "DEVICEDISCOVERY");
                                                              topic_Publish(tDlg, "AssetTypeSelection", "NONE");
                                                            }
                                                            topic_Set(tDlg, "DiscoveryListFilterKeyword", cmpTxtDiscoveryFilterBy.BlkJSX.getValue());
                                                            topic_Publish(tDlg, "LabelNumberofDevices", "0");
                                                            discoverDeviceAgent.restartDiscoveryService();
                                                          }
                                                        }
                                                      );

  var cmpLblNumberOfDevices = new CmpLabel(([BlkJSX, "lblNumDevices"]), null,
                                            [{ action: "TOPICEVENT", topic: tDlg, eventvar: "LabelNumberofDevices", eventval: null}]);

  var cmpLblDeviceDiscoveryType = new CmpLabel(([BlkJSX, "lblDeviceDiscoveryType"]), null,
                                            [{ action: "TOPICEVENT", topic: tDlg, eventvar: "LabelDeviceDiscoveryType", eventval: null}]);

  var cmpBlkDeviceDiscoveryList = new CmpContainer([BlkJSX, "blkDeviceDiscovery"],
                                                   [{ cmp: cmpMtxDeviceDiscovery },
                                                    { cmp: cmpImgBtnRefresh },
                                                    { cmp: cmpSelDiscoveryFilterBy },
                                                    { cmp: cmpTxtDiscoveryFilterBy },
                                                    { cmp: cmpBtnClear },
                                                    { cmp: cmpLblBtnRefreshHelpText},
                                                    { cmp: cmpBtnRefreshDeviceDiscovery },
                                                    { cmp: cmpLblNumberOfDevices },
                                                    { cmp: cmpLblDeviceDiscoveryType }
                                                   ],
                                               null
                                              );
                                                                                          
  var cmpSelSubDeviceTypeList = new CmpSelect([BlkJSX,"selSubDeviceType"],
                                               null,
                                               {action:"FNCCALL", 
                                               context:null, 
                                               fnc:function() {
                                                  var selVal = cmpSelSubDeviceTypeList.BlkJSX.getValue();
                                                  if (jsx3.util.strEmpty(selVal)) return;
//                                                  var strXMLCacheId = cmpSelSubDeviceTypeList.BlkJSX.getXMLId();
//                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
//                                                  if (jsx3.util.strEmpty(objXML)) return;
//                                                  var record = objXML.selectSingleNode("//record[@jsxid='" + selVal + "']");
//                                                  var SelAssetType = record.getAttribute("jsxtext");                                                  

                                                  topic_Publish(tDlg, "EditAssetAssetTypeSelection", selVal);
                                               }
                                              });
                                              
  var cmpBlkEditAssetName = new Component([BlkJSX,"txtAssetDeviceName"]);
  var cmpBlkEditAssetDescription = new Component([BlkJSX,"txtAssetDeviceDescription"]);
  var cmpBlkPasswordHelpText = new Component([BlkJSX, "blkPasswordHelpText"]);

  var cmpTxtEditAssetPassword = new CmpTxtBoxWithBlur([BlkJSX,"txtAssetDevicePassword"], null, null, 
                                                      {action:"FNCCALL", 
                                                         context:null, 
                                                         fnc:function() {
                                                               var passwordVal = this.BlkJSX.getValue();
                                                               /*if(jsx3.util.strEmpty(passwordVal)) {
                                                                 //var chkDevicePassword = validatePassword(passwordVal, [BlkJSX,"txtAssetDevicePassword"], null);
                                                                 var AssetType = topic_Get(tDlg, "EditAssetAssetTypeSelection");
                                                                 if(AssetType != "IPVSClient-Device") {
                                                                    var chkDevicePassword = checkValue(passwordVal , [BlkJSX,"txtAssetDevicePassword"] , "Please enter password");
                                                                    if(!chkDevicePassword){
                                                                      return false;
                                                                    }
                                                                 }
                                                               }*/
                                                         }
                                                      }                                             
                                                     );

  var cmpTxtEditAssetVerifyPassword = new CmpTxtBoxWithBlur([BlkJSX,"txtAssetDeviceVerifyPassword"], null, null, 
                                                            {action:"FNCCALL", 
                                                               context:null, 
                                                               fnc:function() {
                                                                     var devicePassword = cmpTxtEditAssetPassword.BlkJSX.getValue();
                                                                     if(!jsx3.util.strEmpty(devicePassword) ) {
                                                                       var chkDeviceValidateVerifyPassword = validateVerifyPassword(devicePassword, this.BlkJSX.getValue(), [BlkJSX,"txtAssetDevicePassword"], null);
                                                                     }
                                                               }
                                                            }                                                           
                                                           );

  var cmpChkAssetUnmanage = new CmpCheckBox([BlkJSX,"chkUnmanaged"],
                                                null,
                                                {action:"FNCCALL", 
                                                 context:null, 
                                                 fnc:function() {
                                                   var unmanage = cmpChkAssetUnmanage.BlkJSX.getChecked();
                                                   if( unmanage ) {
                                                     cmpTxtEditAssetPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                     cmpTxtEditAssetVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);               
                                                   }
                                                   else{
                                                     cmpTxtEditAssetPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                     cmpTxtEditAssetVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);               
                                                   }
                                                 }
                                                }
                                               );

  var cmpChkApplyTagsToPorts = new Component([BlkJSX,"chkApplyTagsToPorts"]);
  cmpChkApplyTagsToPorts.BlkJSX.setChecked("1");
                                                             
  var cmpBlkApplyTagsToPorts = new CmpContainer([BlkJSX,"blkApplyTagsToPorts"],
                                               [ {cmp:cmpChkApplyTagsToPorts,
                                                  sel:"topic_Get(\"" + tDlg + "\", 'ApplyTagsToPorts') == 'Show' &&"+
                                                      "(topic_Get(\"" + tDlg + "\", 'AssetTypeSelection') != 'DigitalMediaServer' &&" +
                                                      "topic_Get(\"" + tDlg + "\", 'AssetTypeSelection') != 'ManagementServer' && " +
                                                      "topic_Get(\"" + tDlg + "\", 'AssetTypeSelection') != 'Client')"}
                                               ],
                                               [{action:"TOPICEVENT", topic:tDlg, eventvar:"ApplyTagsToPorts", eventval:null},
                                                {action:"TOPICEVENT", topic:tDlg, eventvar:"AssetTypeSelection", eventval:null}]);                                                         
      
  var cmpBtnAdvanceSettings = new CmpImageButton([BlkJSX, "imgAdvanceSettings"], null,
                                                { action: "FNCCALL", context: this,
                                                  fnc: function() {
                                                    topic_Publish(tDlg, "AddEditDeviceAdvanceSettings", "SHOW");
                                                  }
                                                });

  var cmpCmbSelDeviceTagDefName = new CmpCustomPaginationList([BlkJSX, "cmbDeviceAddTagDefName"],
                                                   [{action:"TOPICEVENT", topic: tDlg, eventvar:"TagNameState", eventval:null}],
                                                     { persist: { col: "jsxtext", initval: false },
                                                         action: "TOPICEVENT", topic: tDlg, eventvar: "TagNameSelected", eventval: "jsxid"
                                                     },
                                                   null,
                                                   null,
                                                   "xsl/TagNameList.xsl"
                                                  );

  cmpCmbSelDeviceTagDefName.cmpReqParamArr = {StartFrom: "ObjectList_StartFrom",
                                   CountToFetch: "ObjectList_CountToFetch",
                                   SortOrder: "ObjectList_SortOrder",
                                   ObjectCount: "ObjectCount",
                                   SortByXpath: "ObjectList_SortBy",
                                   WhereObjectType: "ObjectList_WhereObjectType"
                                  };

  topic_Set(tDlg, "TagNameResponseSelectFormat", "NameValueTree");
  topic_Set(tDlg, "TagNameResponseObjectType", "Device");

  cmpCmbSelDeviceTagDefName.cmpReq = new cmpReq_GetSessionAPIPaginationList( tDlg, "GetTags",
                                                                  [{ VarName: "ObjectList_SelectFormat", VarRef: "TagNameResponseSelectFormat"},
                                                                   { VarName: "ObjectList_WhereObjectType", VarRef: "TagNameResponseObjectType"}],
                                                                  { action: "FNCCALL", context: cmpCmbSelDeviceTagDefName,
                                                                    fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                            this.onResponse(respCode, respVal);
                                                                            //cmpDlgEditReportSetPropertiesPermissions();
                                                                        }
                                                                        else {
                                                                            log.info("GetTagsRequest failed. Error Code :" + respCode);
                                                                            this.onResponseErrorHandle("GetTags",respCode, respVal);
                                                                        }
                                                                      }
                                                                  }
                                                                );

  var cmpCmbSelDeviceTagDefValue = new Component([BlkJSX, "cmbDeviceAddTagDefValue"]);
  
  var cmpMtxTags = new Component([BlkJSX,"mtxTags"]);

  eventDlg.subscribe("TagNameSelected", null, this,
                      function(eventVar, eventVal) {
                        if (eventVal != "NONE") {
                          var nameRecord = cmpCmbSelDeviceTagDefName.BlkJSX.getValue();
                          var selNameRecord = cmpCmbSelDeviceTagDefName.BlkJSX.getRecordNode(nameRecord);
                          var tagNode = selNameRecord.selectSingleNode(".//Tag");
                          cmpCmbSelDeviceTagDefValue.BlkJSX.setXMLTransformers("xsl/TagValueList.xsl");
                          var strXMLValueCacheId = cmpCmbSelDeviceTagDefValue.BlkJSX.getXMLId();
                          var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);
                          objXML.loadXML(tagNode.toString());
                          cmpCmbSelDeviceTagDefValue.BlkJSX.setSourceXML(objXML); 
                          cmpCmbSelDeviceTagDefValue.BlkJSX.repaint();
                        }
                      }
                    );
  
  var cmpBtnAddTags = new CmpButton([BlkJSX,"btnAddTags"],
                                    null, 
                                    {action:"FNCCALL", 
                                     context:null, 
                                     fnc:function() {
                                        var nameRecord = cmpCmbSelDeviceTagDefName.BlkJSX.getValue();
                                        var selNameRecord =  cmpCmbSelDeviceTagDefName.BlkJSX.getRecord(nameRecord);
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
                                        var valueRecord = cmpCmbSelDeviceTagDefValue.BlkJSX.getValue();
                                        var selValueRecord = cmpCmbSelDeviceTagDefValue.BlkJSX.getRecord(valueRecord);
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
                                        //o.jsxtext = groupTagName + " = " + groupTagValue;
                                        cmpMtxTags.BlkJSX.insertRecord(o, null, false);
                                        cmpCmbSelDeviceTagDefName.BlkJSX.setValue(null);
                                        cmpCmbSelDeviceTagDefValue.BlkJSX.setValue(null);
                                        cmpMtxTags.BlkJSX.repaintData();
                                     }
                                    });


    var cmpBtnDeviceRegister = new CmpImageButton([BlkJSX, "btnDeviceReregister"], null,
                                                    { action: "FNCCALL",
                                                      context: null,
                                                      fnc: function () {
                                                        var reregsiterAssetTypeSelection = topic_Get(tDlg, 'EditAssetAssetTypeSelection');
                                                        topic_Publish(tDlg, "ReRegisterAssetTypeSelection", reregsiterAssetTypeSelection);
                                                        topic_Publish(tDlg, "LabelDeviceDiscoveryType", "Re-register Device Type: " + displaySelectedAssetType);
                                                        topic_Set(tDlg, "AssetTypeSelection", "NONE");
                                                        SetAddDeviceType("DISCOVERY", "Discovery");
                                                        topic_Set(tDlg, "DiscoveryListViewType", "UnRegistered");
                                                        cmpSelDiscoveryFilterBy.BlkJSX.setValue("UnRegistered");
                                                        cmpTxtDiscoveryFilterBy.BlkJSX.setValue("Filter by keyword...");
                                                        topic_Set(tDlg, "DiscoveryListFilterKeyword", "");
                                                        discoverDeviceAgent.startDiscoveryService();
                                                      }
                                                    }
                                                  );


  var cmpBlkblkDeviceReregister = new Component([BlkJSX, "blkDeviceReregister"]);

  var cmpBlkEditAssetInfo = new CmpContainer([BlkJSX, "blkDeviceInfo"],
                                             [{ cmp: cmpSelSubDeviceTypeList },
                                               { cmp: cmpCmbSelDeviceTagDefName },
                                               { cmp: cmpCmbSelDeviceTagDefValue },
                                               { cmp: cmpMtxTags },
                                               { cmp: cmpBtnAddTags },
                                               { cmp: cmpBlkApplyTagsToPorts },
                                               { cmp: cmpBlkEditAssetName },
                                               { cmp: cmpBlkEditAssetDescription },
                                               { cmp: cmpTxtEditAssetPassword },
                                               { cmp: cmpTxtEditAssetVerifyPassword },
                                               { cmp: cmpBtnAdvanceSettings },
                                               { cmp: cmpBtnDeviceRegister },
                                               { cmp: cmpBlkblkDeviceReregister,
                                                 sel: "topic_Get(\"" + tDlg + "\", 'DeviceAddType') == 'EDIT' && topic_Get(\"" + tDlg + "\", 'ReRegisterAssetTypeSelection') == 'NONE' && " +
                                                      "(topic_Get(\"" + tDlg + "\", 'AssetTypeSelection') != 'Client' &&" +
                                                      "topic_Get(\"" + tDlg + "\", 'AssetTypeSelection') != 'UnmanagedEncoder/Decoder' ) &&" +
                                                      "topic_Get(\"" + tDlg + "\", 'EditAssetSelectedAssetStatus') != 'Ready' && topic_Get(\"" + tDlg + "\", 'EditAssetSelectedAssetStatus') != 'UPGRADE'"
                                               },
                                               { cmp: cmpChkAssetUnmanage,
                                                 sel: "(topic_Get('" + tDlg + "', 'DeviceAddType') == 'EDIT' || topic_Get('" + tDlg + "', 'DeviceAddType') == 'BASIC') && " +
                                                      "(topic_Get(\"" + tDlg + "\", 'AssetUnmanage') != 'NONE')"
                                               },
                                               { cmp: cmpBlkPasswordHelpText,
                                                 sel: "topic_Get(\"" + tDlg + "\", 'AssetPasswordHelpTextSelection') != 'NONE'"
                                               }
                                              ],
                                              [{ action: "TOPICEVENT", topic: tDlg, eventvar: "DeviceAddType", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "ReRegisterAssetTypeSelection", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "AssetUnmanage", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "AssetPasswordHelpTextSelection", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "AssetTypeSelection", eventval: null}]);

  var cmpTxtMacAddrress = new Component([BlkJSX, "txtMACAddress"]);
  var cmpTxtIpAddress = new Component([BlkJSX, "txtIPAddress"]);
  var cmpTxtNetmask = new Component([BlkJSX, "txtNetmask"]);
  var cmpTxtGateway = new Component([BlkJSX, "txtGateway"]);

  var cmpChkDhcp = new CmpCheckBox([BlkJSX, "chkUseDHCP"],
                                    null,
                                    { action: "FNCCALL",
                                      context: null,
                                      fnc: function () {
                                        if (cmpChkDhcp.BlkJSX.getChecked()) {
                                          cmpTxtIpAddress.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
                                          cmpTxtNetmask.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
                                          cmpTxtGateway.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
                                        }
                                        else {
                                          cmpTxtIpAddress.BlkJSX.setReadonly(jsx3.Boolean.FALSE);
                                          cmpTxtNetmask.BlkJSX.setReadonly(jsx3.Boolean.FALSE);
                                          cmpTxtGateway.BlkJSX.setReadonly(jsx3.Boolean.FALSE);
                                        }
                                        if( !verifyIPAddress(cmpTxtIpAddress.BlkJSX.getValue(), false) ) {
                                          cmpTxtIpAddress.BlkJSX.setValue("0.0.0.0", true);
                                          cmpTxtNetmask.BlkJSX.setValue("0.0.0.0", true);
                                        }
                                        if( !validateNetmask(cmpTxtNetmask.BlkJSX.getValue(), false) ) {
                                          cmpTxtNetmask.BlkJSX.setValue("0.0.0.0", true);
                                        }
                                        if (!verifyIPAddress(cmpTxtGateway.BlkJSX.getValue(), false)) {
                                          cmpTxtGateway.BlkJSX.setValue("0.0.0.0", true);
                                        }
                                        cmpTxtIpAddress.BlkJSX.repaint();
                                        cmpTxtNetmask.BlkJSX.repaint();
                                        cmpTxtGateway.BlkJSX.repaint();
                                      }
                                    });


  var cmpBlkNetworkInfo = new CmpContainer([BlkJSX, "blkDeviceNetwork"],
                                             [{ cmp: cmpTxtMacAddrress },
                                               { cmp: cmpTxtIpAddress },
                                               { cmp: cmpTxtNetmask },
                                               { cmp: cmpTxtGateway },
                                               { cmp: cmpChkDhcp }
                                              ],
                                              null);
       
  var cmpBtnPrevious = new CmpButton([BlkJSX, "btnPrevious"], null,
                                          { action: "FNCCALL", context: null,
                                            fnc: function() {
                                              var setup = topic_Get(tDlg, "AddEditDeviceStage");
                                              var deviceAddType = topic_Get(tDlg, "DeviceAddType");
                                              // in Basic it just goes back to the sleect device type
                                              if( deviceAddType == "BASIC" ){
                                                if( setup == "INFO") {
                                                  topic_Set(tDlg, "DeviceAddType", "NONE");
                                                  topic_Publish(tDlg, "AddEditDeviceStage", "NONE");
                                                }
                                              }
                                              else if( deviceAddType == "DISCOVERY" ){
                                                if( setup == "DEVICEDISCOVERY") {
                                                  topic_Set(tDlg, "DeviceAddType", "NONE");
                                                  topic_Publish(tDlg, "AddEditDeviceStage", "NONE");
                                                  cmpSelSubDeviceTypeList.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                }
                                                else if (setup == "INFO") {
                                                  topic_Publish(tDlg, "AddEditDeviceStage", "DEVICEDISCOVERY");
                                                  cmpImgBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                  cmpLblBtnRefreshHelpText.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);                                                  
                                                }
                                                else if( setup == "NETWORK") {
                                                  topic_Publish(tDlg, "AddEditDeviceStage", "INFO");
                                                }
                                              }                                              
                                            }
                                          });                                            
                                              
  var cmpBtnNext = new CmpButton([BlkJSX, "btnNext"], null,
                                      { action: "FNCCALL", context: null,
                                        fnc: function() {
                                          var AssetSelectedType = topic_Get(tDlg, 'EditAssetAssetTypeSelection');
                                          if(jsx3.util.strEmpty(AssetSelectedType)) {
                                            alert("Select a Device.");
                                            return false;
                                          }                                          
                                          var setup = topic_Get(tDlg, "AddEditDeviceStage");
                                          var deviceAddType = topic_Get(tDlg, "DeviceAddType");
                                          // so far in Basic add there is no Next button
                                          if( deviceAddType == "BASIC" ){
                                          }
                                          else if( deviceAddType == "DISCOVERY" ){
                                            if (setup == "DEVICEDISCOVERY") {
                                              if (topic_Get(tDlg, "ReRegisterAssetTypeSelection") == "NONE") {
                                                cmpBlkEditAssetName.BlkJSX.setValue("");
                                              }
                                              cmpTxtEditAssetPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                              cmpTxtEditAssetVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                              var discoverySelectedId = topic_Get(tDlg, "DeviceDiscoverySelected");
                                              var selRecordNode = cmpMtxDeviceDiscovery.BlkJSX.getRecordNode(discoverySelectedId);
                                              var ipAddress = selRecordNode.getAttribute("jsxipaddress");
                                              var port = selRecordNode.getAttribute("jsxport");
                                              var macAddress = selRecordNode.getAttribute("jsxmacaddress");
                                              getDevice_httpRequest(ipAddress, port, macAddress);
                                              topic_Publish(tDlg, "AddEditDeviceStage", "INFO");
                                            }
                                            else if (setup == "INFO") {
                                              if (cmpDlgEditAssetSetAssetValidate() != true ) return;
                                              topic_Publish(tDlg, "AddEditDeviceStage", "NETWORK");
                                            }
                                            else if( setup == "NETWORK") {
                                              topic_Publish(tDlg, "AddEditDeviceStage", "SAVINGSTATUS");
                                            }
                                          }
                                        }
                                      });

  var cmpBtnSave = new CmpButton([BlkJSX, "btnSaveAssetInfo"],
                                           null,
                                           { action: "FNCCALL",
                                             context: null,
                                             fnc: function () {
                                               eventDlg.subscribe("EditAssetSetAssetState", null, this,
                                                                function (eventVar, eventVal) {
                                                                  if (eventVal == "CLOSE") {
                                                                    dlgEditAsset_Destroy();
                                                                  }
                                                                  else if (eventVal == "DONE") {
                                                                    topic_Publish(tApp, "AssetDataUpdate", "ADD");
                                                                    dlgEditAsset_Destroy();
                                                                  }
                                                                  else if (eventVal == "DISCOVERY_ADD_DONE") {
                                                                    topic_Publish(tApp, "AssetDataUpdate", "ADD");
                                                                  }
                                                                });
                                               var deviceAddType = topic_Get(tDlg, "DeviceAddType");
                                               if (deviceAddType == "DISCOVERY") {
                                                 if (cmpDlgEditAssetSetDeviceNetworkInterfaceValidate() != true) return;
                                                 topic_Publish(tDlg, "AddEditDeviceStage", "SAVINGSTATUS");
                                               }
                                               cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                             }
                                           });
            
  var cmpBtnCancel = new CmpButton([BlkJSX,"btnCancel"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
                                                    
  var cmpBtnDone = new CmpButton([BlkJSX,"btnDone"],
                                 null, 
                                 {action:"FNCCALL", 
                                  context:null, 
                                  fnc:function() {
                                    dlgEditAsset_Destroy();
                                  }
                                 });                                              
                                                 
   var cmpBlkDeviceFooter = new CmpContainer([BlkJSX,"blkDeviceFooter"],
                                               [{cmp:cmpBtnPrevious,
                                                 sel:"(topic_Get('" + tDlg + "', 'DeviceAddType') == 'BASIC' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'INFO' ) || "+
                                                     "(topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && (topic_Get('" + tDlg + "', 'AddEditDeviceStage') != 'SAVINGSTATUS' && topic_Get('" + tDlg + "', 'ReRegisterAssetTypeSelection') == 'NONE' )) || "+
                                                     "(topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && (topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'INFO' && topic_Get('" + tDlg + "', 'ReRegisterAssetTypeSelection') != 'NONE' )) || "+
                                                     "(topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && (topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'NETWORK' && topic_Get('" + tDlg + "', 'ReRegisterAssetTypeSelection') != 'NONE' ))"
                                               },
                                                {cmp:cmpBtnNext,
                                                 sel:"(topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && topic_Get('" + tDlg + "', 'AssetTypeSelection') != 'NONE') && "+
                                                     " (topic_Get('" + tDlg + "', 'AddEditDeviceStage') != 'NETWORK' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') != 'SAVINGSTATUS') "},
                                                {cmp:cmpBtnSave,
                                                  sel: "((topic_Get('" + tDlg + "', 'DeviceAddType') == 'EDIT' || topic_Get('" + tDlg + "', 'DeviceAddType') == 'BASIC') && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'INFO') ||  "+
                                                       "(topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'NETWORK') "},
                                                {cmp:cmpBtnCancel,
                                                  sel:"topic_Get('" + tDlg + "', 'AddEditDeviceStage') != 'SAVINGSTATUS' "},
                                                {cmp:cmpBtnDone,
                                                  sel:"topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'SAVINGSTATUS' "}
                                               ],
                                               [{action:"TOPICEVENT", topic: tDlg, eventvar:"AddEditDeviceStage", eventval:null},
                                                {action:"TOPICEVENT", topic: tDlg, eventvar:"AssetTypeSelection", eventval:null}
                                               ]); 
                                                  
  var cmpBtnEditAssetClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           }); 
                                                       
  var cmpBlkDeviceAdvancedSettings = new Component([BlkJSX,"blkDeviceAdvancedSettings"]);
  var cmpBlkAssetPrivateData = new Component([BlkJSX,"txtAssetDevicePrivateData"]);
  var cmpBtnAdvanceSettingsDone = new CmpImageButton([BlkJSX, "imgAdvanceSettingsDone"], null,
                                                      { action: "FNCCALL", context: this,
                                                        fnc: function() {
                                                          var assetPrivateData = cmpBlkAssetPrivateData.BlkJSX.getValue();
                                                          var assetPrivateDataStr = assetPrivateData.toString();
                                                          assetPrivateDataStr = assetPrivateDataStr.split('/').join('-'); 
                                                          var regex = /^[-:.<>~abcdefghijklmnopqrstuvwxyzABCDEFGHIJKLMNOPQRSTUVWXYZ0123456789_\s]*$/;
                                                          if (!regex.test(assetPrivateDataStr)){
                                                            alert("Special characters are not allowed in the Advanced Settings.");
                                                            cmpBlkAssetPrivateData.BlkJSX.setValue("");
                                                            cmpBlkAssetPrivateData.BlkJSX.focus();
                                                            return;
                                                          }
                                                          topic_Publish(tDlg, "AddEditDeviceAdvanceSettings", "HIDE");
                                                        }
                                                      }); 
                                                  
  var cmpBlkAdvanceSettings = new CmpContainer([BlkJSX,"blkDeviceAdvancedSettings"],
                                               [{cmp:cmpBlkDeviceAdvancedSettings,
                                                 sel:"topic_Get('" + tDlg + "', 'AddEditDeviceAdvanceSettings') == 'SHOW'"},
                                                {cmp:cmpBlkAssetPrivateData},
                                                {cmp:cmpBtnAdvanceSettingsDone}
                                               ],
                                               [{ action: "TOPICEVENT", topic: tDlg, eventvar: "AddEditDeviceAdvanceSettings", eventval: null}]);

  var cmpBlkUMS = new Component([BlkJSX, "blkDeviceSavingStatus", "blkUMS"]);
  
  var cmpLblAddDevice = new CmpLabel(([BlkJSX, "lblAddDevice"]), null,
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "LabelAddDevice", eventval: null}]);

  // imgAddDeviceUpdating  imgAddDeviceError   imgAddDeviceSuccess
  var cmpAddDeviceUpdateingImg = new Component([BlkJSX, "imgAddDeviceUpdating"]);
  var cmpAddDeviceErrorImg = new Component([BlkJSX, "imgAddDeviceError"]);
  var cmpAddDeviceSuccessImg = new Component([BlkJSX, "imgAddDeviceSuccess"]);
  var cmpBlkAddDeviceProgress = new CmpContainer([BlkJSX, "blkDeviceSavingStatus"],
                                                     [{ cmp: cmpAddDeviceUpdateingImg,
                                                       sel: "topic_Get('" + tDlg + "', 'AddDeviceSavingStatus') == 'UPDATE'"
                                                      },
                                                      { cmp: cmpAddDeviceErrorImg,
                                                        sel: "topic_Get('" + tDlg + "', 'AddDeviceSavingStatus') == 'ERROR'"
                                                      },
                                                      { cmp: cmpAddDeviceSuccessImg,
                                                        sel: "topic_Get('" + tDlg + "', 'AddDeviceSavingStatus') == 'SUCCESS'"
                                                      }
                                                      ],
                                                      [{ action: "TOPICEVENT", topic: tDlg, eventvar: "AddDeviceSavingStatus", eventval: null}]);


  var cmpLblUpdateConfig = new CmpLabel(([BlkJSX, "lblUpdateConfig"]), null,
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "LabelUpdateConfig", eventval: null}]);

  // imgUpdateConfigUpdating   imgUpdateConfigError   imgUpdateConfigSuccess
  var cmpUpdateConfigUpdateingImg = new Component([BlkJSX, "imgUpdateConfigUpdating"]);
  var cmpUpdateConfigErrorImg = new Component([BlkJSX, "imgUpdateConfigError"]);
  var cmpUpdateConfigSuccessImg = new Component([BlkJSX, "imgUpdateConfigSuccess"]);
  var cmpBlkUpdateConfigProgress = new CmpContainer([BlkJSX, "blkDeviceSavingStatus"],
                                                     [{ cmp: cmpUpdateConfigUpdateingImg,
                                                       sel: "topic_Get('" + tDlg + "', 'UpdateConfigSavingStatus') == 'UPDATE'"
                                                      },
                                                      { cmp: cmpUpdateConfigErrorImg,
                                                        sel: "topic_Get('" + tDlg + "', 'UpdateConfigSavingStatus') == 'ERROR'"
                                                      },
                                                      { cmp: cmpUpdateConfigSuccessImg,
                                                        sel: "topic_Get('" + tDlg + "', 'UpdateConfigSavingStatus') == 'SUCCESS'"
                                                      }
                                                      ],
                                                      [{ action: "TOPICEVENT", topic: tDlg, eventvar: "UpdateConfigSavingStatus", eventval: null}]);

  var cmpLblSuccess = new CmpLabel(([BlkJSX, "lblSuccess"]), null,
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "LabelSuccess", eventval: null}]);

  // imgSuccessUpdating   imgSuccessError    imgSuccessSuccess
  var cmpSuccessUpdateingImg = new Component([BlkJSX, "imgSuccessUpdating"]);
  var cmpSuccessErrorImg = new Component([BlkJSX, "imgSuccessError"]);
  var cmpSuccessSuccessImg = new Component([BlkJSX, "imgSuccessSuccess"]);
  var cmpBlkSuccessProgress = new CmpContainer([BlkJSX, "blkDeviceSavingStatus"],
                                                     [{ cmp: cmpSuccessUpdateingImg,
                                                       sel: "topic_Get('" + tDlg + "', 'SuccessSavingStatus') == 'UPDATE'"
                                                      },
                                                      { cmp: cmpSuccessErrorImg,
                                                        sel: "topic_Get('" + tDlg + "', 'SuccessSavingStatus') == 'ERROR'"
                                                      },
                                                      { cmp: cmpSuccessSuccessImg,
                                                        sel: "topic_Get('" + tDlg + "', 'SuccessSavingStatus') == 'SUCCESS'"
                                                      }
                                                      ],
                                                      [{ action: "TOPICEVENT", topic: tDlg, eventvar: "SuccessSavingStatus", eventval: null}]);

   var cmpBlkDeviceStatusProgress = new CmpContainer([BlkJSX, "blkDeviceSavingStatus"],
                                                     [{ cmp: cmpLblAddDevice },
                                                      { cmp: cmpLblUpdateConfig },
                                                      { cmp: cmpLblSuccess },
                                                      { cmp: cmpBlkUMS,
                                                        sel: "topic_Get('" + tDlg + "', 'SavingStatus') == 'SHOW'" },
                                                      { cmp: cmpBlkAddDeviceProgress },
                                                      {cmp: cmpBlkUpdateConfigProgress},
                                                      {cmp: cmpBlkSuccessProgress}
                                                      ],
                                                      [{ action: "TOPICEVENT", topic: tDlg, eventvar: "SavingStatus", eventval: null}]);
                                                                                               
  var cmpBlkDeviceType = new Component([BlkJSX,"blkDeviceBody","blkDeviceType"]);
  var cmpBlkDeviceInfo = new Component([BlkJSX,"blkDeviceBody","blkDeviceInfo"]);
  var cmpBlkDeviceDiscovery = new Component([BlkJSX,"blkDeviceBody","blkDeviceDiscovery"]);
  var cmpBlkDeviceNetwork = new Component([BlkJSX,"blkDeviceBody","blkDeviceNetwork"]);
  var cmpBlkDeviceSavingStatus = new Component([BlkJSX,"blkDeviceBody","blkDeviceSavingStatus"]);
                                                           
  var cmpBlkDeviceStage = new CmpContainer([BlkJSX,"blkDeviceBody"],
                                           [{cmp:cmpBlkDeviceType,
                                             sel:"topic_Get('" + tDlg + "', 'DeviceAddType') == 'NONE' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'NONE'"},
                                           {cmp:cmpBlkDeviceInfo,
                                             sel:"(topic_Get('" + tDlg + "', 'DeviceAddType') == 'EDIT' || topic_Get('" + tDlg + "', 'DeviceAddType') == 'BASIC' || topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY') && " +
                                                 " topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'INFO'"},
                                           {cmp:cmpBlkDeviceDiscoveryList,
                                             sel:"topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'DEVICEDISCOVERY'"},
                                           {cmp:cmpBlkDeviceNetwork,
                                             sel:"topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'NETWORK'"},
                                           {cmp:cmpBlkDeviceSavingStatus,
                                             sel:"topic_Get('" + tDlg + "', 'DeviceAddType') == 'DISCOVERY' && topic_Get('" + tDlg + "', 'AddEditDeviceStage') == 'SAVINGSTATUS'"}
                                           ],
                                           [{action:"TOPICEVENT", topic: tDlg, eventvar:"AddEditDeviceStage", eventval:null}]); 
                                                                                                                                                                                                                                  
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"dlgAddEditDevice"],
                                          [ {cmp:cmpBtnEditAssetClose},
                                            {cmp:cmpBlkDeviceFooter},
                                            {cmp:cmpBlkAssetAddType },
                                            {cmp:cmpBlkDeviceDiscoveryList },
                                            {cmp:cmpBlkEditAssetInfo },
                                            {cmp:cmpBlkNetworkInfo },
                                            {cmp:cmpBlkAdvanceSettings },
                                            {cmp:cmpBlkDeviceStatusProgress },
                                            {cmp:cmpBlkDeviceStage },
                                            {cmp:cmpBlkDialogWindowBar}
                                          ],
                                          [{action:"TOPICEVENT", topic: tDlg, eventvar:"EditAssetDataState", eventval:null}]);                            
                                          
  cmpDlgEditAsset.BlkJSX = BlkJSX;
  cmpDlgEditAsset.ObjectNID = AssetNID;
  // fnc cmpReq_GetCompleteDeviceWithReadQuery defined in cmpReq.js
  cmpDlgEditAsset.getBlkDevice = cmpReq_GetDevice;
  cmpDlgEditAsset.tDlg = tDlg;
  cmpDlgEditAsset.setDirtyBitChangeHandler =  cmpDlgEditAssetSetDirtyBitChangeHandler;

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.setDirtyBitChangeHandler();
                 this.getBlkDevice(this.tDlg, this.ObjectNID, "EXTENDED");                 
                 }
               ),
               null, cmpDlgEditAsset );
               
  // event published from func ( cmpReq_GetCompleteDeviceWithReadQuery ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteDeviceDone", AssetNID, this,
                      function() {
                        cmpDlgEditAssetGetAsset(AssetNID);
                      }
                    ); // subscribe end

  function getDevice_httpRequest(ipAddress, port, macAddress) {
    ipAddress = ipAddress + (jsx3.util.strEmpty(port) ? "" : ":" + port);
    var urlStr = "http://" + ipAddress + "/deviceinterface/Device";
    var Request = jsx3.net.Request;
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(urlStr);
    var req = Request.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      //var respStatus = req.getStatus();
      if (req.getStatus() == Request.STATUS_OK) {

        var response = req.getResponseXML().selectSingleNode("//Device");
        var state = response.getAttribute("state");
        var deviceAgentJID = response.getAttribute("ownerUserJID");
        var orgtype = response.getAttribute("type");
        var type = orgtype;
        var title = response.getAttribute("title").toLowerCase();
        var password = response.getAttribute("password");
        var description = response.getAttribute("description");
        var primaryEtherNetNode = response.selectSingleNode("//EthernetPortTableConfig/EthernetPortConfig[IsPrimary='true']");
        if (jsx3.util.strEmpty(primaryEtherNetNode)) {
          primaryEtherNetNode = response.selectSingleNode("//EthernetPortTableConfig/EthernetPortConfig[PortID='1']");
        }
        var ipAddress = primaryEtherNetNode.selectSingleNode("//IPConfig/IPAddress").getValue();
        var useDHCP = primaryEtherNetNode.selectSingleNode("//IPConfig/UseDHCP").getValue();
        var netmask = primaryEtherNetNode.selectSingleNode("//IPConfig/Netmask").getValue();

        var portID = primaryEtherNetNode.selectSingleNode("//PortID").getValue();
        var enable = primaryEtherNetNode.selectSingleNode("//EnableInterface").getValue();  //getAttribute("enable");
        var primary = primaryEtherNetNode.selectSingleNode("//IsPrimary").getValue();
        var listIndex = primaryEtherNetNode.getAttribute("ListIndex");
        if (useDHCP == "true") {
          useDHCP = "Yes";
        }
        else if (useDHCP == "false") {
          useDHCP = "No";
        }

        var gateway = "0.0.0.0";
        var routeTableNode = response.selectSingleNode("//RouteTableConfig/RouteEntry[@ListIndex='1']");
        if (!jsx3.util.strEmpty(routeTableNode)) {
          gateway = routeTableNode.selectSingleNode("//Gateway").getValue();
        }
        topic_Set(tDlg, "DeviceAgentJID", deviceAgentJID);

        if (topic_Get(tDlg, "ReRegisterAssetTypeSelection") == "NONE") {
          cmpBlkEditAssetName.BlkJSX.setValue(title);
          cmpBlkEditAssetDescription.BlkJSX.setValue(description);
          cmpTxtEditAssetPassword.BlkJSX.setValue(password);
          cmpTxtEditAssetVerifyPassword.BlkJSX.setValue(password);
        }

        cmpTxtMacAddrress.BlkJSX.setValue(macAddress);
        cmpTxtIpAddress.BlkJSX.setValue(ipAddress);
        cmpTxtNetmask.BlkJSX.setValue(netmask);
        cmpTxtGateway.BlkJSX.setValue(gateway);
        cmpChkDhcp.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
        if (useDHCP == "Yes") {
          cmpChkDhcp.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
          cmpTxtIpAddress.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
          cmpTxtNetmask.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
          cmpTxtGateway.BlkJSX.setReadonly(jsx3.Boolean.TRUE);
        }
        else {
          cmpTxtIpAddress.BlkJSX.setReadonly(jsx3.Boolean.FALSE);
          cmpTxtNetmask.BlkJSX.setReadonly(jsx3.Boolean.FALSE);
          cmpTxtGateway.BlkJSX.setReadonly(jsx3.Boolean.FALSE);
        }
        cmpTxtIpAddress.BlkJSX.repaint();
        cmpTxtNetmask.BlkJSX.repaint();
        cmpTxtGateway.BlkJSX.repaint();

        topic_Set(tDlg, "EditAssetAssetPortID", portID);
        topic_Set(tDlg, "EditAssetAssetListIndex", listIndex);
        topic_Set(tDlg, "EditAssetAssetEnable", enable);
        topic_Set(tDlg, "EditAssetAssetPrimary", primary);
        discoverDeviceAgent.setDeviceNode(topic_Get(tDlg, "DeviceDiscoverySelected"), response);
      }
    }

    req.subscribe("*", this, onRequestEvent);
    req.send();
  }

  // event published for the selected device discovered
  eventDlg.subscribe("DeviceDiscoverySelected", null, this,
                      function (eventVar, eventVal) {
                        if (!jsx3.util.strEmpty(eventVal)) {
                          if (eventVal == "NONE") return;

                          var selRecordNode = cmpMtxDeviceDiscovery.BlkJSX.getRecordNode(eventVal);
                          var status = selRecordNode.getAttribute("jsxstatus");
                          //if (status == "Registered") return;

                          var orgtype = selRecordNode.getAttribute("jsxorgtype");
                          var assetTypeSelection = "";

                          if (orgtype.search(/V2D-/) != -1) {
                            assetTypeSelection = "Encoder/Decoder";
                          }
                          else if (orgtype.search(/DMS-/) != -1) {
                            assetTypeSelection = "DigitalMediaServer";
                          }
                          else if (orgtype.search(/MS-/) != -1) {
                            assetTypeSelection = "ManagementServer";
                          }
                          topic_Set(tDlg, "EditAssetAssetTypeSelection", orgtype);
                          topic_Publish(tDlg, "AssetTypeSelection", assetTypeSelection);
                        }
                      }
                    );      // subscribe end

  // checking for new device has been added successfully.
  //wont get jmdns notification
  eventApp.subscribe("DBUpdate", null, this,
                     function (EventVar, EventVal) {
                       if (jsx3.util.strEmpty(EventVal)) return;
                       if (EventVal.objType == "AssetResource" &&
                           EventVal.type == "replace" && 
                           EventVal.nodeID == topic_Get(tDlg, "NewAssetNID")) {
                         var reqGetDevice = new ReqSessionGetRequest("GetDevice",
                                                    { action: "FNCCALL", context: null,
                                                      fnc: function (respCode, respVal) {
                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                          var deviceNode = respVal.selectSingleNode(".//Device");
                                                          var presence = deviceNode.getAttribute("presence");
                                                          if(presence =="available"){
                                                            topic_Publish(tDlg, "LabelSuccess", Str_LabelSuccess_Success);
                                                            topic_Publish(tDlg, "SuccessSavingStatus", "SUCCESS");
                                                            topic_Publish(tDlg, "SavingStatus", "HIDE");
                                                          }
                                                        }
                                                        else {
                                                          var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                          var ErrorMsg = "Error: Device->Asset get data request failed \n";
                                                          ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                          ErrorMsg += "Error code: " + respCode;
                                                          alert(ErrorMsg);
                                                          return false;
                                                        }
                                                      }
                                                    }
                                                   );
                         reqGetDevice.ServiceName = "SessionService";
                         var reqParams = new Object;
                         reqParams["ObjectList_WhereNID"] = topic_Get(tDlg, "NewAssetNID");
                         reqParams["ObjectList_SelectFormat"] = "BASIC";
                         reqGetDevice.reqParams = reqParams;
                         // send the request
                         reqGetDevice.sendRequest(this);
                       }

                     });

  discoverDeviceAgent.subscribe(com.ipvs.agent.DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE, this,
                                function (objEvent) {
                                  if (objEvent.stateVar == com.ipvs.agent.DeviceDiscoveryAgent.DISCOVERYADDDEVICE) {
                                    var objRecord = objEvent.stateVal;
                                    //check the device node with filter to add/remove to/from current view
                                    var recID = objRecord.getAttribute("jsxid")
                                    var filterKey = topic_Get(tDlg, "DiscoveryListFilterKeyword");
                                    var listType = topic_Get(tDlg, "DiscoveryListViewType");
                                    if (jsx3.util.strEmpty(listType)) listType = "UnRegistered";
                                    var filterXPath = "//record[@jsxid = \"" + recID + "\" and @jsxstatus = \"" + listType + "\"]";
                                    if (!jsx3.util.strEmpty(filterKey) && filterKey != "Filter by keyword...") {
                                      filterKey = filterKey.toLowerCase();
                                      //ISSUE:: Using translate to avoid case-sensitive while filter as 'contains with lower-case' is not working. Need to check why.
                                      filterXPath = "//record[@jsxid = \"" + recID + "\" and @jsxstatus = \"" + listType + "\" and " +
                                                        "( contains(translate(@jsxtext, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") or " +
                                                        "contains(translate(@jsxtype, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") or " +
                                                        "contains(translate(@jsxmacaddress, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") or " +
                                                        "contains(translate(@jsxipaddress, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") )]";
                                    }
                                    var insertNode = objRecord.getRootNode().selectSingleNode(filterXPath);
                                    //add
                                    if (!jsx3.util.strEmpty(insertNode)) {
                                      var numDevices = "0";
                                      //check if re register or add new device thru discovery
                                      // if re register the filter the xml based on the re register type selected
                                      var reregisterType = topic_Get(tDlg, "ReRegisterAssetTypeSelection");
                                      if (reregisterType != "NONE" && !jsx3.util.strEmpty(reregisterType)) {
                                        var orgType = objRecord.getAttribute("jsxorgtype");
                                        if (orgType == reregisterType) {
                                          cmpMtxDeviceDiscovery.BlkJSX.insertRecordNode(objRecord, null, true);
                                        }
                                      }
                                      else {
                                        cmpMtxDeviceDiscovery.BlkJSX.insertRecordNode(objRecord, null, true);
                                      }
                                    }
                                    //delete
                                    else {
                                      var ItemNode = cmpMtxDeviceDiscovery.BlkJSX.getRecord(recID);
                                      if (!jsx3.util.strEmpty(ItemNode)) {
                                        cmpMtxDeviceDiscovery.BlkJSX.deleteRecord(recID, true);
                                      }
                                    }
                                    numDevices = cmpDlgNumberOfDevicesDiscovered();
                                    // publish the number of devices
                                    topic_Publish(tDlg, "LabelNumberofDevices", numDevices);
                                  }
                                  else if (objEvent.stateVar == com.ipvs.agent.DeviceDiscoveryAgent.DISCOVERYDELETEDEVICE) {
                                    var ItemNode = cmpMtxDeviceDiscovery.BlkJSX.getRecord(objEvent.stateVal);
                                    if (!jsx3.util.strEmpty(ItemNode)) {
                                      cmpMtxDeviceDiscovery.BlkJSX.deleteRecord(objEvent.stateVal, true);
                                      var numDevices = cmpDlgNumberOfDevicesDiscovered();
                                      // publish the number of devices
                                      topic_Publish(tDlg, "LabelNumberofDevices", numDevices);
                                    }
                                  }
                                  else if (objEvent.stateVar == com.ipvs.agent.DeviceDiscoveryAgent.DISCOVERYDEVICEUPDATE) {
                                    //updat the device discovery matrix
                                    var deviceAddType = topic_Get(tDlg, "DeviceAddType");
                                    var deviceAddStage = topic_Get(tDlg, "AddEditDeviceStage");
                                    if (deviceAddType == "DISCOVERY" && deviceAddStage != "SAVINGSTATUS") {
                                      var newXML = discoverDeviceAgent.getCDFDoc();
                                      var numDevices = "0";
                                      //check if re register or add new device thru discovery
                                      // if re register the filter the xml based on the re register type selected
                                      var reregisterType = topic_Get(tDlg, "ReRegisterAssetTypeSelection");
                                      if (reregisterType != "NONE" && !jsx3.util.strEmpty(reregisterType)) {
                                        newXML = discoverDeviceAgent.getSpecificDeviceTypeCDFDoc(reregisterType);
                                      }

                                      if (!jsx3.util.strEmpty(newXML)) {
                                        var strXMLCacheId = cmpMtxDeviceDiscovery.BlkJSX.getXMLId();
                                        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);

                                        //Apply filter
                                        var filterKey = topic_Get(tDlg, "DiscoveryListFilterKeyword");
                                        var listType = topic_Get(tDlg, "DiscoveryListViewType");
                                        if (jsx3.util.strEmpty(listType)) listType = "UnRegistered";
                                        var filterXPath = ".//record[@jsxstatus != \"" + listType + "\"]";
                                        if (!jsx3.util.strEmpty(filterKey) && filterKey != "Filter by keyword...") {
                                          filterKey = filterKey.toLowerCase();
                                          //ISSUE:: Using translate to avoid case-sensitive while filter as 'contains with lower-case' is not working. Need to check why.
                                          filterXPath = ".//record[@jsxstatus != \"" + listType + "\" or not" +
                                                        "( contains(translate(@jsxtext, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") or " +
                                                        "contains(translate(@jsxtype, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") or " +
                                                        "contains(translate(@jsxmacaddress, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") or " +
                                                        "contains(translate(@jsxipaddress, 'ABCDEFGHIJKLMNOPQRSTUVWXYZ', 'abcdefghijklmnopqrstuvwxyz'), \"" + filterKey + "\") )]";

                                        }
                                        var recordsToFilterIter = newXML.selectNodeIterator(filterXPath);
                                        while (recordsToFilterIter.hasNext()) {
                                          var recordNode = recordsToFilterIter.next();
                                          newXML.removeChild(recordNode);
                                        }

                                        var selectedNode = objXML.selectSingleNode(".//record[@jsxselected=1]");
                                        var selectedID = null;
                                        if (!jsx3.util.strEmpty(selectedNode)) {
                                          selectedID = selectedNode.getAttribute("jsxid");
                                        }

                                        objXML.loadXML(newXML.toString());

                                        if (!jsx3.util.strEmpty(selectedID)) {
                                          var selectedNode = objXML.selectSingleNode(".//record[@jsxid='" + selectedID + "']");
                                          if (!jsx3.util.strEmpty(selectedNode)) {
                                            selectedNode.setAttribute("jsxselected", "1");
                                            var deviceAgentJID = selectedNode.getAttribute("jsxid");
                                            topic_Publish(tDlg, "DeviceDiscoverySelected", deviceAgentJID);
                                          }
                                        }

                                        cmpMtxDeviceDiscovery.BlkJSX.setSourceXML(objXML);
                                        cmpMtxDeviceDiscovery.BlkJSX.repaint();
                                        numDevices = cmpDlgNumberOfDevicesDiscovered();
                                        // publish the number of devices
                                        topic_Publish(tDlg, "LabelNumberofDevices", numDevices);
                                      }
                                    }
                                  }
                                  // publish the agentjid and the message if the deviceAddStage is saving
                                  else {
                                    if (objEvent.stateVar == topic_Get(tDlg, "DeviceDiscoverySelected") && topic_Get(tDlg, "AddEditDeviceStage") == "SAVINGSTATUS" && objEvent.stateVal == "Registered") {
                                      //                                      topic_Publish(tDlg, "LabelSuccess", Str_LabelSuccess_Success);
                                      //                                      topic_Publish(tDlg, "SuccessSavingStatus", "SUCCESS");
                                      //                                      topic_Publish(tDlg, "SavingStatus", "HIDE");
                                    }
                                    else if (objEvent.stateVar == topic_Get(tDlg, "DeviceDiscoverySelected") && topic_Get(tDlg, "AddEditDeviceStage") == "SAVINGSTATUS" && objEvent.stateVal == "UnRegistered") {
                                      topic_Publish(tDlg, "LabelSuccess", Str_LabelSuccess_Error);
                                      topic_Publish(tDlg, "SuccessSavingStatus", "ERROR");
                                      topic_Publish(tDlg, "SavingStatus", "HIDE");
                                    }
                                  }
                                }
                               );
                                

  discoverDeviceAgent.subscribe(com.ipvs.agent.DeviceDiscoveryAgent.ON_DEVICE_GETDATA, this,
                                  function (objEvent) {
                                    if (objEvent.stateVar == com.ipvs.agent.DeviceDiscoveryAgent.DEVICEDATAUPDATE && objEvent.stateVal == com.ipvs.agent.DeviceDiscoveryAgent.DEVICEDATAUPDATE_SHOW) {
                                      cmpImgBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                      cmpLblBtnRefreshHelpText.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                    }
                                    else if (objEvent.stateVar == com.ipvs.agent.DeviceDiscoveryAgent.DEVICEDATAUPDATE && objEvent.stateVal == com.ipvs.agent.DeviceDiscoveryAgent.DEVICEDATAUPDATE_HIDE) {
                                      cmpImgBtnRefresh.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                      cmpLblBtnRefreshHelpText.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                    }
                                  }
                                 );

    // to the number of records
    function cmpDlgNumberOfDevicesDiscovered(deviceType) {
      var strXMLCacheId = cmpMtxDeviceDiscovery.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      var recLength = "0";
      if (!jsx3.util.strEmpty(deviceType)) {
        recLength = objXML.selectNodes(".//record[@jsxorgtype ='" + deviceType + "']").getLength();
      }
      else {
        recLength = objXML.selectNodes(".//record").getLength();
      }
      return recLength.toString();
    }

  // function to set dirtybit change function on the textboxes, select boxes
  // check boxes. Any value change witkk set the dirty bit true
  function cmpDlgEditAssetSetDirtyBitChangeHandler() {
 
    var objParent = cmpBlkEditAssetInfo.BlkJSX.getParent();
    
    // find all descendant of type chkbox and set its toggle event
    var ChkBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.CheckBox, false);
    for(var i=0; i < ChkBoxNameArr.length; i++ ) {
      var ChkBlkJSX = ChkBoxNameArr[i];
      ChkBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.TOGGLE);
      //ChkBlkJSX.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
      //var Name = ChkBoxNameArr[i].getName();
      //alert(Name);
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
      //SelBlkJSX.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
    }
  }  // end cmpDlgEditAssetSetDirtyBitChangeHandler


  // helper function to publish what is selected
  function SetAddDeviceType(deviceAddType, assetTypeSelection) {
    topic_Set(tDlg, "DeviceAddType", deviceAddType);
    if( deviceAddType == "BASIC" || deviceAddType == "EDIT"){
      topic_Publish(tDlg, "AddEditDeviceStage", "INFO");    
      topic_Publish(tDlg, "AssetTypeSelection", assetTypeSelection);
      topic_Publish(tDlg, "AssetUnmanage", "NONE");
      if(assetTypeSelection == 'Encoder/Decoder'){
        topic_Publish(tDlg, "AssetUnmanage", "SHOW");
      }
    }
    else if( deviceAddType == "DISCOVERY" ) {
      topic_Publish(tDlg, "AddEditDeviceStage", "DEVICEDISCOVERY");   
    }
  } // end SetAddDeviceType


  eventDlg.subscribe("EditAssetAssetTypeSelection", null, this,
                      function () {
                        // disable password field
                        var AssetSelectedType = topic_Get(tDlg, 'EditAssetAssetTypeSelection');
                        cmpBlkPasswordHelpText.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                        var clientDeviceTypeNode = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ AssetSelectedType +"']");
                        var clientDeviceName = "";
                        if(!jsx3.util.strEmpty(clientDeviceTypeNode)){
                          clientDeviceName = clientDeviceTypeNode.getAttribute("jsxtext");
                        }
                        if(clientDeviceName == "Media Client"){
                          topic_Set(tDlg, "AssetPasswordHelpTextSelection", "SHOW");
                          cmpBlkPasswordHelpText.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                        }
                        
                        if ((clientDeviceName == "Proxy Client") ||
                            (clientDeviceName == "Web Client") ||
                            (AssetSelectedType == "Template")) {
                          cmpTxtEditAssetPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                          cmpTxtEditAssetVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                        } else {
                          var AssetTypeSelected = topic_Get(tDlg, 'AssetTypeSelection');
                          var unmanage = cmpChkAssetUnmanage.BlkJSX.getChecked();
                          if (unmanage == 1 || AssetTypeSelected == "UnmanagedEncoder/Decoder") {
                            cmpTxtEditAssetPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                            cmpTxtEditAssetVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                          }
                          else {
                            cmpTxtEditAssetPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                            cmpTxtEditAssetVerifyPassword.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                          }
                        }
                        if (topic_Get(tDlg, "DeviceAddType") == "DISCOVERY") {
                          cmpSelSubDeviceTypeList.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                        }
                      }
                    );    // subscribe end

 eventDlg.subscribe("AssetTypeSelection", null, this,
                      function () {
                        
                        var assetTypeSelection = topic_Get(tDlg, "AssetTypeSelection");
                        if (assetTypeSelection == "NONE" || jsx3.util.strEmpty(assetTypeSelection)) {
                          return;
                        }
                        if (topic_Get(tDlg, "TagNameState") == "NONE") {
                          topic_Publish(tDlg, "TagNameState", (newServerDate()).getTime());
                        }

                        var strXMLValueCacheId = cmpSelSubDeviceTypeList.BlkJSX.getXMLId();
                        var objXML = PNameSpace.getCache().getDocument(strXMLValueCacheId);

                        if (jsx3.util.strEmpty(objXML)) return;

                        var unmanage = cmpChkAssetUnmanage.BlkJSX.getChecked();
                        if (unmanage == 1 && assetTypeSelection == "UnmanagedEncoder/Decoder") {
                          assetTypeSelection = "Encoder/Decoder";
                        }

                        var deviceNode = deviceTypeListNode.selectSingleNode(".//device[@jsxid='" + assetTypeSelection + "']");

                        cmpSelSubDeviceTypeList.BlkJSX.setXMLTransformers("xsl/DeviceValueList.xsl");

                        objXML.loadXML(deviceNode.toString());
                        cmpSelSubDeviceTypeList.BlkJSX.setSourceXML(objXML);
                        if (AssetNID == "DEFAULT_ASSETRESOURCE_NID") {
                          if (topic_Get(tDlg, "DeviceAddType") == "DISCOVERY") {
                            var AssetSelectedType = topic_Get(tDlg, 'EditAssetAssetTypeSelection');
                            cmpSelSubDeviceTypeList.BlkJSX.setValue(AssetSelectedType);
                          }
                          else {
                            if (assetTypeSelection == "Encoder/Decoder") {
                              cmpSelSubDeviceTypeList.BlkJSX.setValue("V2D-XP100-Device");
                            }
                            else if (assetTypeSelection == "UnmanagedEncoder/Decoder") {
                              cmpSelSubDeviceTypeList.BlkJSX.setValue("RTP-Tx-Device");
                            }
                            else if (assetTypeSelection == "DigitalMediaServer") {
                              cmpSelSubDeviceTypeList.BlkJSX.setValue("DMS-100-Device");
                            }
                            else if (assetTypeSelection == "ManagementServer") {
                              cmpSelSubDeviceTypeList.BlkJSX.setValue("MS-Device");
                            }
                            else if (assetTypeSelection == "Client") {
                              cmpSelSubDeviceTypeList.BlkJSX.setValue("IPVSClient-Device");
                            }
                          }
                          cmpSelSubDeviceTypeList.BlkJSX.repaint();
                          var assetType = cmpSelSubDeviceTypeList.BlkJSX.getValue();
                          topic_Publish(tDlg, "EditAssetAssetTypeSelection", assetType);
                        }
                        else {
                          var assetType = topic_Get(tDlg, "EditAssetAssetTypeSelection");
                          cmpSelSubDeviceTypeList.BlkJSX.setValue(assetType);
                          cmpSelSubDeviceTypeList.BlkJSX.repaint();
                        }
                      }
                    );     // subscribe end 
                      
  
  // fnc to get dlg data
  function cmpDlgEditAssetGetAsset(AssetNID) {
    var defaultAssetNID = "DEFAULT_ASSETRESOURCE_NID";
    
    cmpMtxTags.BlkJSX.setXMLTransformers("xsl/NewTagToCDFTag.xsl");
    cmpMtxTags.BlkJSX.setSelectionModel(jsx3.gui.Matrix.SELECTION_ROW);
    var imgDeleteEntityBlkJSX = cmpMtxTags.BlkJSX.getDescendantOfName("jsxDeleteTag");
    imgDeleteEntityBlkJSX.setEvent('eval("DeleteEntity(this);")', jsx3.gui.Interactive.EXECUTE);
    //On Deleting Tag Entry - Check for Change in Group Tags and Show/Hide checkbox
    DeleteEntity = function(obj) {
      obj.getParent().getParent().deleteRecord(obj.emGetSession().recordId, true);
      var assetNode = null;
      var groupsNode = null;
      if(AssetNID == "DEFAULT_ASSETRESOURCE_NID") {
        assetNode = defaultAssetDeviceNode.cloneNode(true);
        groupsNode = assetNode.selectSingleNode(".//Info/Groups");
      }
      else{
        assetNode = getDialogDataNode(tDlg, "Device").cloneNode(true);
        groupsNode = assetNode.selectSingleNode("./Groups");
      }
      if(!changeInGroupTags(groupsNode,cmpMtxTags.BlkJSX)) {
        topic_Publish(tDlg, "ApplyTagsToPorts", 'Hide');
      } 
      else {
        topic_Publish(tDlg, "ApplyTagsToPorts", 'Show');
      }
    }
    
    var resourceTypeArr = [];
      
    var reqStatus = topic_Get(tDlg, "GetCompleteDeviceStatus");
    if(reqStatus != "SUCCESS") {
      dlgEditAsset_Destroy();  
    }
    else {
      var assetNode = defaultAssetDeviceNode.cloneNode(true);
      if(AssetNID == "DEFAULT_ASSETRESOURCE_NID") {
      }
      else{
        var deviceNode = getDialogDataNode(tDlg, "Device").cloneNode(true);
        
        assetNode.setAttribute("NID", deviceNode.getAttribute("NID") );
        assetNode.selectSingleNode(".//Info/Title").setValue( deviceNode.getAttribute("title") );
        assetNode.selectSingleNode(".//Info/Description").setValue( deviceNode.getAttribute("description") );
        assetNode.selectSingleNode(".//Info/Type").setValue( deviceNode.getAttribute("type"));
        assetNode.selectSingleNode(".//AssetPrivateKey/KeyData").setValue( deviceNode.getAttribute("password") );
        assetNode.selectSingleNode(".//AssetPrivateData").setValue( deviceNode.selectSingleNode(".//DevicePrivateData").getValue() );
        var deviceGroupsNode = deviceNode.selectSingleNode("./Groups");
        var oldGroupsNode = assetNode.selectSingleNode(".//Groups");   
        var groupParentNode = assetNode.selectSingleNode(".//Groups").getParent();
        groupParentNode.replaceNode(deviceGroupsNode,oldGroupsNode);        
        
        assetNode.selectSingleNode(".//Contact/Presence").setValue( deviceNode.getAttribute("presence"));
        assetNode.selectSingleNode(".//State/StateFlag").setValue( deviceNode.getAttribute("state"));
        var unManagedVal = deviceNode.getAttribute("unManaged");
        if( unManagedVal == "true" ){
          assetNode.selectSingleNode(".//State/StateFlag").setValue("Unmanaged");
        }
        var unMannedVal = deviceNode.getAttribute("unManned");
        if( unMannedVal == "true" ){
          assetNode.selectSingleNode(".//FeatureList/Feature").setAttribute("Name", "Mode");
          assetNode.selectSingleNode(".//FeatureList/Feature").setValue( "UnManned");
          assetNode.selectSingleNode(".//Info/Type").setValue("RemoteClient-Device");
        }
      }
    
    }
    deleteEmptyGroupTag(assetNode);
    
    var AssetName = assetNode.selectSingleNode(".//Info/Title").getValue();
    var AssetDescription = assetNode.selectSingleNode(".//Info/Description").getValue();
    var AssetType = assetNode.selectSingleNode(".//Info/Type").getValue();
    var AssetPrivateData = assetNode.selectSingleNode(".//AssetPrivateData").getValue();
    //handle unmanaged device state
    var unManagedState = assetNode.selectSingleNode(".//State/StateFlag").getValue(); 
    var encoderDecoderDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Encoder/Decoder']/record[@jsxid='"+ AssetType +"']");
    var dmsDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='DigitalMediaServer']/record[@jsxid='"+ AssetType +"']");
    var msDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='ManagementServer']/record[@jsxid='"+ AssetType +"']");
    var clientDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ AssetType +"']");
    var unmanagedDeviceType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='UnmanagedEncoder/Decoder']/record[@jsxid='"+ AssetType +"']");
    
    if( unManagedState == "Unmanaged" && !jsx3.util.strEmpty(encoderDecoderDeviceType)) {
      cmpChkAssetUnmanage.BlkJSX.setChecked("1");
    }
    else {
      cmpChkAssetUnmanage.BlkJSX.setChecked("0");
    }
    var groupNode = assetNode.selectSingleNode(".//Groups").cloneNode(true); 
    // set the Tags data in the matrix
    var strXMLCacheId = cmpMtxTags.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(groupNode.toString());
    cmpMtxTags.BlkJSX.setSourceXML(objXML);
    cmpMtxTags.BlkJSX.repaint();   
         
    cmpBlkEditAssetName.BlkJSX.setValue(AssetName);
    cmpBlkEditAssetDescription.BlkJSX.setValue(AssetDescription);
    //shwoing the password from the AssetPrivateKey/KeyData
    var KeyData = assetNode.selectSingleNode(".//AssetPrivateKey/KeyData").getValue();
    cmpTxtEditAssetPassword.BlkJSX.setValue(KeyData);
    cmpTxtEditAssetVerifyPassword.BlkJSX.setValue(KeyData); 
    cmpBlkAssetPrivateData.BlkJSX.setValue(AssetPrivateData);
      
    if(AssetNID != "DEFAULT_ASSETRESOURCE_NID") {
      // disbale for edit case
      var cmpTxtDeviceNameBlkJSX = cmpBlkEditAssetName.BlkJSX;
      cmpTxtDeviceNameBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);

      var assetTypeSelection = "";
      if(!jsx3.util.strEmpty(encoderDecoderDeviceType)){
        assetTypeSelection = "Encoder/Decoder";
        cmpChkAssetUnmanage.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        if (unManagedState == "Unmanaged") {
          assetTypeSelection = "UnmanagedEncoder/Decoder";
        }
      }
      else if(!jsx3.util.strEmpty(dmsDeviceType)){
        assetTypeSelection = "DigitalMediaServer";
      }
      else if(!jsx3.util.strEmpty(msDeviceType)){
        assetTypeSelection = "ManagementServer";
      }
      else if(!jsx3.util.strEmpty(clientDeviceType)){
        assetTypeSelection = "Client";
      }
      else if(!jsx3.util.strEmpty(unmanagedDeviceType)){
        assetTypeSelection = "UnmanagedEncoder/Decoder";
      }
      
      var cmpSelDeviceAssetTypeBlkJSX = cmpSelSubDeviceTypeList.BlkJSX;
      cmpSelDeviceAssetTypeBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      topic_Set(tDlg, "EditAssetAssetTypeSelection", AssetType);

      //get the feature name from the assestnode
      var featureNode = assetNode.selectSingleNode(".//Feature");
      if( !jsx3.util.strEmpty(featureNode) ){
        var featureName = featureNode.getAttribute("Name");
        var featureValue = featureNode.getValue();
        if( (featureName == "Mode") && (featureValue == "UnManned") ){
          cmpSelSubDeviceTypeList.BlkJSX.setValue("RemoteClient-Device");  
          cmpSelSubDeviceTypeList.BlkJSX.repaint();
        }
      }
      
      SetAddDeviceType("EDIT", assetTypeSelection);               
    }
    else {
      topic_Publish(tDlg, "EditAssetDataState", AssetNID);
    }

  }// end get dlg data function
      
  //Basic Validation for name, password and duplicate 
  function cmpDlgEditAssetSetAssetValidate() {
    
    var deviceName = cmpBlkEditAssetName.BlkJSX.getValue();
    var devicePassword = cmpTxtEditAssetPassword.BlkJSX.getValue();
    var deviceVerifyPassword = cmpTxtEditAssetVerifyPassword.BlkJSX.getValue();
   // var chkDeviceName = checkDeviceName(deviceName , [BlkJSX,"txtAssetDeviceName"] , null);
    var chkDeviceName = checkValue(deviceName , [BlkJSX,"txtAssetDeviceName"] , "Please enter device name");
    if(!chkDeviceName) {
      return false;
    }
    var GroupTagValuesChanged = "";
    var chkDevicePassword = false;
    var chkDeviceVerifyPassword = false;
    var chkDeviceValidateVerifyPassword = false;
    // dont check password for these type of assets
    // also if device is unmanaged
    var unmanagedType = topic_Get(tDlg, "AssetTypeSelection");
    var AssetType = topic_Get(tDlg, "EditAssetAssetTypeSelection");
    var clientDeviceTypeNode = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ AssetType +"']");
    var clientDeviceName = "";
    if(!jsx3.util.strEmpty(clientDeviceTypeNode)){
      clientDeviceName = clientDeviceTypeNode.getAttribute("jsxtext");
    }
    var unmanage = cmpChkAssetUnmanage.BlkJSX.getChecked();
    if( (unmanagedType == "UnmanagedEncoder/Decoder") || 
         ((clientDeviceName == "Media Client") && (clientDeviceName != "Remote Operated Client") && (jsx3.util.strEmpty(devicePassword)) ) ||
         (clientDeviceName == "Proxy Client") || 
         (clientDeviceName == "Web Client") || 
         (AssetType == "Template") ||
         (unmanage == 1)) {
      chkDevicePassword = true;
      chkDeviceVerifyPassword = true;
      chkDeviceValidateVerifyPassword = true;
    } else {
      //To fix bug#41607, bug#34444 and bug#42000
      //chkDevicePassword = validatePassword(devicePassword, [BlkJSX,"txtAssetDevicePassword"], null);
      chkDevicePassword = checkValue(devicePassword , [BlkJSX,"txtAssetDevicePassword"] , "Please enter password");
      if(chkDevicePassword) {
        chkDeviceValidateVerifyPassword = validateVerifyPassword(devicePassword, deviceVerifyPassword, [BlkJSX,"txtAssetDevicePassword"], null);
      }
      else{
        return false;
      }
    }
    
    if( AssetNID == "DEFAULT_ASSETRESOURCE_NID" ) {
      if(chkDeviceName) {
        if(chkDeviceValidateVerifyPassword){
          return true;
        }
      }
    } 
    else {
      GroupTagValuesChanged = chkGroupTagValueChanged(AssetNID, [BlkJSX,"mtxTags"]);
      if(chkDeviceValidateVerifyPassword){
        return true;
      }
    }
    return false;
  }

  // function to validate teh network page
  function cmpDlgEditAssetSetDeviceNetworkInterfaceValidate() {
    var ipAddress = cmpTxtIpAddress.BlkJSX.getValue();
    var netmask = cmpTxtNetmask.BlkJSX.getValue();
    var gateway = cmpTxtGateway.BlkJSX.getValue();

    var ipValCheck = "";
    var netmaskValCheck = "";
    var gatewayValCheck = "";
    if (cmpChkDhcp.BlkJSX.getChecked()) {
      ipValCheck = true;
      netmaskValCheck = true;
      gatewayValCheck = true;
    }
    else {
      ipValCheck = verifyIPAddress(ipAddress, false);
      netmaskValCheck = validateNetmask(netmask, false);
      gatewayValCheck = verifyIPAddress(gateway, false);
    }
    

    var ipAddressAlert = "IP Address: " + ipAddress + " is not a valid IP address.";
    var netmaskAlert = "Netmask:" + netmask + "is not a valid NetMask.";
    var gatewayAlert = "Gateway:" + gateway + "is not a valid Gateway address.";

    if (!ipValCheck && !netmaskValCheck && !gatewayValCheck) {
      alert(ipAddressAlert + " \n " + netmaskAlert + " \n " + gatewayAlert);
      return false;
    }
    else if (!ipValCheck && !netmaskValCheck) {
      alert(ipAddressAlert + " \n " + netmaskAlert );
      return false;
    }
    else if (!ipValCheck && !gatewayValCheck) {
      alert(ipAddressAlert + " \n " + gatewayAlert);
      return false;
    }
    else if (!netmaskValCheck && !gatewayValCheck ) {
      alert(netmaskAlert + " \n " + gatewayAlert);
      return false;
    }
    else if (!ipValCheck) {
      alert(ipAddressAlert);
      return false;
    }
    else if (!netmaskValCheck) {
      alert(netmaskAlert);
      return false;
    }
    else if (!gatewayValCheck) {
      alert(gatewayAlert);
      return false;
    }

    if (ipValCheck && netmaskValCheck && gatewayValCheck) {
      return true;
    }
    else {
      return false;
    }

  } // end cmpDlgEditAssetSetDeviceNetworkInterfaceValidate


  // function cmpDlgEditAssetSetDeviceConfig to set the xmpp & network properties of the dveice
  function cmpDlgEditAssetSetDeviceConfig() {
    if (cmpDlgEditAssetSetDeviceNetworkInterfaceValidate() != true) return;
    topic_Publish(tDlg, "LabelUpdateConfig", Str_LabelUpdateConfig_Inprogress);
    topic_Publish(tDlg, "UpdateConfigSavingStatus", "UPDATE");

    var agentReqDataNode = "";
    var portId = topic_Get(tDlg, "EditAssetAssetPortID");
    var listIndex = topic_Get(tDlg, "EditAssetAssetListIndex");
    var enable = topic_Get(tDlg, "EditAssetAssetEnable");
    var primary = topic_Get(tDlg, "EditAssetAssetPrimary");

    var useDHCP = "false"; 
    if( cmpChkDhcp.BlkJSX.getChecked() ){
      useDHCP = "true"; 
    }
    var ipAddress = cmpTxtIpAddress.BlkJSX.getValue();
    var netmask = cmpTxtNetmask.BlkJSX.getValue();

    var networkagentStr = "<EthernetPortTableConfig><EthernetPortConfig Dirty=\"MODIFY\" ListIndex=\"" + listIndex + "\">";
    networkagentStr += "<PortID>" + portId + "</PortID><EnableInterface>" + enable + "</EnableInterface><IsPrimary>" + primary + "</IsPrimary><EnableMulticast>false</EnableMulticast>";
    networkagentStr += "<IPConfig><UseDHCP>" + useDHCP + "</UseDHCP><IPAddress>" + ipAddress + "</IPAddress><Netmask>" + netmask + "</Netmask></IPConfig></EthernetPortConfig></EthernetPortTableConfig>";

    var xmppServerIP = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_SERVER);
    var serviceDomain = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_SERVICEDOMAIN);
    var xmppServerPort = IpvsLogin.getLoginParam(com.ipvs.api.Login.LOGIN_PORT);
    var deviceName = cmpBlkEditAssetName.BlkJSX.getValue();
    var password = cmpTxtEditAssetPassword.BlkJSX.getValue();
    var xmppagentStr = "<XMPPAgentServiceConfig Dirty=\"MODIFY\"><Enable>true</Enable><XMPPServer>" + xmppServerIP + "</XMPPServer><DeviceName>" + deviceName + "</DeviceName><DevicePassword>" + password + "</DevicePassword><Domain>default</Domain><ServiceDomain>" + serviceDomain + "</ServiceDomain></XMPPAgentServiceConfig>";

    var discoverySelectedId = topic_Get(tDlg, "DeviceDiscoverySelected");
    var configagentStr = "";
    if ( cmpChkDhcp.BlkJSX.getChecked() ) {
      configagentStr = "<DeviceConfig Dirty=\"MODIFY\"><ServiceElementsConfig>" + xmppagentStr + "<NetworkElementsConfig>" + networkagentStr + "</NetworkElementsConfig></ServiceElementsConfig></DeviceConfig>";
    }
    // send the route/gateway only for static ip
    else {
      var routeagentStr = "";
      var gateway = cmpTxtGateway.BlkJSX.getValue();
      var deviceAgentJIDRouteTableNode = discoverDeviceAgent.getDeviceAgentJIDRouteTableNode(discoverySelectedId);
      if (!jsx3.util.strEmpty(deviceAgentJIDRouteTableNode)) {
        var deviceAgentJIDRouteTableIter = deviceAgentJIDRouteTableNode.selectNodeIterator("//RouteEntry");
        while (deviceAgentJIDRouteTableIter.hasNext()) {
          var routeNode = deviceAgentJIDRouteTableIter.next().cloneNode(true);
          routeNode.setAttribute("Dirty", "DELETE");
          routeagentStr += routeNode.toString();
        }
      }
      routeagentStr += "<RouteEntry Dirty=\"ADD\" ListIndex=\"1\"><Destination>0.0.0.0</Destination><Netmask>0.0.0.0</Netmask><Gateway>" + gateway + "</Gateway></RouteEntry>";
      configagentStr = "<DeviceConfig Dirty=\"MODIFY\"><ServiceElementsConfig>" + xmppagentStr + "<NetworkElementsConfig><RouteTableConfig Dirty=\"MODIFY\">" + routeagentStr + "</RouteTableConfig>" + networkagentStr + "</NetworkElementsConfig></ServiceElementsConfig></DeviceConfig>";
    }
    var requestStr = "<SetDeviceConfigRequestData doNotSaveToFlash=\"false\">" + configagentStr + "</SetDeviceConfigRequestData>";

    var selRecordNode = cmpMtxDeviceDiscovery.BlkJSX.getRecordNode(discoverySelectedId);
    var urlIPAddress = selRecordNode.getAttribute("jsxipaddress");
    var urlPort = selRecordNode.getAttribute("jsxport");
    urlIPAddress = urlIPAddress + (jsx3.util.strEmpty(urlPort) ? "" : ":" + urlPort);
    var urlStr = "http://" + urlIPAddress + "/deviceinterface/Device/Config";
    var Request = jsx3.net.Request;
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(urlStr);
    var req = Request.open("POST", uri, true);
    var onRequestEvent = function (objEvent) {
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        var response = req.getResponseXML();
        topic_Publish(tDlg, "LabelUpdateConfig", Str_LabelUpdateConfig_Success);
        topic_Publish(tDlg, "UpdateConfigSavingStatus", "SUCCESS");
      }
      else {
        topic_Publish(tDlg, "SavingStatus", "HIDE");
        topic_Publish(tDlg, "LabelUpdateConfig", Str_LabelUpdateConfig_Error);
        topic_Publish(tDlg, "UpdateConfigSavingStatus", "ERROR");
        topic_Publish(tDlg, "LabelSuccess", Str_LabelSuccess_Error);
        topic_Publish(tDlg, "SuccessSavingStatus", "ERROR");
        log.error("SetDeviceConfigRequestData error. Error code:" + respStatus);
      }
    }

    req.subscribe("*", this, onRequestEvent);
    req.send(requestStr);
    //making this success because response can come any time before regestering or after register
    topic_Publish(tDlg, "LabelUpdateConfig", Str_LabelUpdateConfig_Success);
    topic_Publish(tDlg, "UpdateConfigSavingStatus", "SUCCESS");

    topic_Publish(tDlg, "LabelSuccess", Str_LabelSuccess_Inprogress);
    topic_Publish(tDlg, "SuccessSavingStatus", "UPDATE");
  } // end cmpDlgEditAssetSetDeviceConfig


  // fnc to set dlg data - runs in the context of the OK button
  // different fnc will be called to add and edit.
  function cmpDlgEditAssetSetAsset(AssetNID) {
    if (cmpDlgEditAssetSetAssetValidate() != true) return;
    if (topic_Get(tDlg, "DeviceAddType") == "DISCOVERY") {
      if (cmpDlgEditAssetSetDeviceNetworkInterfaceValidate() != true) return;
    }

    var sendRequest = true;
    var requestType = "AddDevice";
    
    var unMannedMode = false;
    var assetType = topic_Get(tDlg, "EditAssetAssetTypeSelection"); 
    var clientDeviceTypeNode = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ assetType +"']");
    var clientDeviceName = "";
    if(!jsx3.util.strEmpty(clientDeviceTypeNode)){
      clientDeviceName = clientDeviceTypeNode.getAttribute("jsxtext");
    }
    if(clientDeviceName == "Remote Operated Client"){
      assetType = "IPVSClient-Device";
      unMannedMode = true;
    }

    var unManagedMode = false;
    var unmanagedType = topic_Get(tDlg, "AssetTypeSelection");
    var unmanage = cmpChkAssetUnmanage.BlkJSX.getChecked();
    if(unmanage == 1 || unmanagedType == "UnmanagedEncoder/Decoder") {
      unManagedMode = true;
    }
    
    var applyToPorts = false;
    if(cmpChkApplyTagsToPorts.BlkJSX.getChecked()) {
      applyToPorts = true;
    }
    
    //get the groups
    var strXMLCacheId = cmpMtxTags.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    //convert the matrix data to Tag nodes
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/NewCDFTagToTag.xsl","TagNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var groupTagsXML = xslDoc.transform(objXML);        
    var groupTagsNode = new jsx3.xml.Document();
    groupTagsNode.loadXML(groupTagsXML);

    var assetDescription = cmpBlkEditAssetDescription.BlkJSX.getValue();
    var assetPassword = cmpTxtEditAssetPassword.BlkJSX.getValue();
    var assetPrivateData = cmpBlkAssetPrivateData.BlkJSX.getValue();
    var assetPrivateDataStr = assetPrivateData.toString();
    // check if edit and anything got changed
    if (AssetNID != "DEFAULT_ASSETRESOURCE_NID") {
      Str_LabelAddDevice_Inprogress = "Updating in the database...";
      Str_LabelAddDevice_Success = "Successfully updated device in the database.";
      Str_LabelAddDevice_Error = "Error updating device in the database.";

      requestType = "SetDeviceInfo";
      var deviceNode = getDialogDataNode(tDlg, "Device").cloneNode(true);
      var orgAssetDescription = deviceNode.getAttribute("description");
      var orgAssetPassword = deviceNode.getAttribute("password");
      var orgAssetPrivateDataStr = deviceNode.selectSingleNode(".//DevicePrivateData").getValue().toString();
      orgAssetPrivateDataStr = orgAssetPrivateDataStr.replace(/<\?.*\?>/, "").replace(/[\n\r]/g, "").replace(/>\s+</g, "><");
      var orgAssetGroups = deviceNode.selectSingleNode("./Groups");

      if (assetDescription == orgAssetDescription &&
          assetPassword == orgAssetPassword &&
          assetPrivateDataStr == orgAssetPrivateDataStr &&
          !changeInGroupTags(orgAssetGroups, cmpMtxTags.BlkJSX) &&
          topic_Get(tDlg, "ReRegisterAssetTypeSelection") == "NONE") {
        sendRequest = false;
      }
    }

    if (sendRequest) {
      topic_Publish(tDlg, "LabelAddDevice", Str_LabelAddDevice_Inprogress);
      topic_Publish(tDlg, "AddDeviceSavingStatus", "UPDATE");

      var reqSetDevice = new ReqSessionGetRequest(requestType,
                                              { action: "FNCCALL", context: null,
                                                fnc: function (respCode, respVal) {
                                                  var errorOccured = false;
                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                    if (topic_Get(tDlg, "DeviceAddType") == "DISCOVERY") {
                                                      topic_Publish(tDlg, "LabelAddDevice", Str_LabelAddDevice_Success);
                                                      topic_Publish(tDlg, "AddDeviceSavingStatus", "SUCCESS");
                                                      topic_Publish(tDlg, "EditAssetSetAssetState", "DISCOVERY_ADD_DONE");
                                                      //get the assest resource NID. Listening to dbupdate notification
                                                      //will save the NID to the notification.
                                                      var newAssestNID = respVal.selectSingleNode(".//Device/@NID").getValue();
                                                      topic_Set(tDlg, "NewAssetNID", newAssestNID);
                                                      cmpDlgEditAssetSetDeviceConfig.call(this);
                                                    }
                                                    else {
                                                      topic_Publish(tDlg, "EditAssetSetAssetState", "DONE");
                                                    }
                                                  }
                                                  else if (respCode == "5206") {
                                                    errorOccured = true;
                                                    topic_Publish(tDlg, "LabelAddDevice", Str_LabelAddDevice_Error);
                                                    topic_Publish(tDlg, "AddDeviceSavingStatus", "ERROR");
                                                    alert("Device name already exists. Please enter some other Device name");
                                                  }
                                                  else {
                                                    errorOccured = true;
                                                    topic_Publish(tDlg, "LabelAddDevice", Str_LabelAddDevice_Error);
                                                    topic_Publish(tDlg, "AddDeviceSavingStatus", "ERROR");
                                                    var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                    var ErrorMsg = "Error: Add Device request failed \n";
                                                    ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                    ErrorMsg += "Error code: " + respCode;
                                                    alert(ErrorMsg);
                                                  }

                                                  if (errorOccured) {
                                                    if (topic_Get(tDlg, "DeviceAddType") == "DISCOVERY") {
                                                      topic_Publish(tDlg, "LabelAddDevice", "");
                                                      topic_Publish(tDlg, "LabelUpdateConfig", "");
                                                      topic_Publish(tDlg, "LabelSuccess", "");

                                                      topic_Publish(tDlg, "AddEditDeviceStage", "INFO");
                                                    }
                                                  }
                                                }
                                              }
                                             );
                                             
      reqSetDevice.ServiceName = "SessionService";
      var requestDataObjectArr = new Object;
      requestDataObjectArr["Groups"] = groupTagsNode;
      reqSetDevice.requestDataObjectArr = requestDataObjectArr;
      
      var reqParams = new Object;
      reqParams["ObjectList_Title"] = cmpBlkEditAssetName.BlkJSX.getValue();
      reqParams["ObjectList_Type"] = assetType;
      reqParams["ObjectList_DevicePrivateData"] = assetPrivateData;
      reqParams["ObjectList_Password"] = assetPassword;
      reqParams["ObjectList_description"] = assetDescription;
      reqParams["ObjectList_UnManaged"] = unManagedMode;
      reqParams["ObjectList_UnManned"] = unMannedMode;
      reqParams["ObjectList_ApplyTagsToPorts"] = applyToPorts;
      if( AssetNID != "DEFAULT_ASSETRESOURCE_NID" ) {
        reqParams["ObjectList_ByNID"] = AssetNID;
      }
      reqSetDevice.reqParams = reqParams;
      
      // send the request
      reqSetDevice.sendRequest(this);    
    }
    else {
      topic_Publish(tDlg, "EditAssetSetAssetState", "CLOSE");
    }
    
  } // end cmpDlgEditAssetSetAsset
           
          
} // end create


