// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceAV_Create(AssetNID, selectedAssetType, selectedAssetName, selectedAssetStatus, selectedPort) {
  
  if( selectedAssetStatus != "Ready") {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus);
   return false; 
  }

  var DeviceNID = "";
  var DeviceResourceNIDArr = [];
  
  closeDialogIfExists("DialogAVSettings");
  // load view - dialog component file
  var DlgFile = "components/assets/assetAVSettingsDialog.xml";  
  var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditAsset_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditAsset.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }

  topic_Set(tDlg, "EditBlkTxVideoParam", "NONE");
  topic_Set(tDlg, "EditBlkXPPortType", "NONE");
  topic_Set(tDlg, "EditBlkTxPortKeyboardMouse", "NONE");
  topic_Set(tDlg, "EditBlkTxSerialParam", "NONE");
  topic_Set(tDlg, "EditBlkTxSplashScreen", "NONE");
  topic_Set(tDlg, "EditBlkTxVideoLookup", "NONE");
  topic_Set(tDlg, "EditBlkRxVideoParam", "NONE");
  topic_Set(tDlg, "EditBlkRxSerialParam", "NONE");
  topic_Set(tDlg, "EditBlkRxSplashScreen", "NONE");
  topic_Set(tDlg, "EditBlkRxVideoLookup", "NONE");
  var cmpCDFEditAssetAVSettings = new Component([BlkJSX,"cdfDlgAssetAVSettings"]);
  var cmpBlkLblAssetXPPortType = new Component([BlkJSX,"blkLblAssetEditXPPortType"]);
  var cmpBlkLblV2dTxVideoParam = new Component([BlkJSX,"blkLblV2dTxVideoParam"]);
  var cmpBlkLblV2dTxPortKeyboardMouse = new Component([BlkJSX,"blkLblV2dTxPortKeyboardMouse"]);
  var cmpBlkLblV2dTxSerialParam = new Component([BlkJSX,"blkLblV2dTxSerialParam"]);
  var cmpBlkLblV2dTxSplashScreen = new Component([BlkJSX,"blkLblV2dTxSplashScreen"]);
  var cmpBlkLblV2dTxVideoLookup = new Component([BlkJSX,"blkLblV2dTxVideoLookup"]);
  var cmpBlkLblV2dRxVideoParam = new Component([BlkJSX,"blkLblV2dRxVideoParam"]);
  var cmpBlkLblV2dRxSerialParam = new Component([BlkJSX,"blkLblV2dRxSerialParam"]);
  var cmpBlkLblV2dRxSplashScreen = new Component([BlkJSX,"blkLblV2dRxSplashScreen"]);
  var cmpBlkLblV2dRxVideoLookup = new Component([BlkJSX,"blkLblV2dRxVideoLookup"]);

  var cmpBlkEditAssetAVNav = new CmpContainer([BlkJSX,"addV2DNav"],
                                              [ {cmp:cmpBlkLblV2dTxVideoParam,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkTxVideoParam') != 'NONE'"},
                                                {cmp:cmpBlkLblAssetXPPortType,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkXPPortType') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dTxPortKeyboardMouse,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkTxPortKeyboardMouse') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dTxSerialParam,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkTxSerialParam') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dTxSplashScreen,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkTxSplashScreen') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dTxVideoLookup,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkTxVideoLookup') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dRxVideoParam,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkRxVideoParam') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dRxSerialParam,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkRxSerialParam') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dRxSplashScreen,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkRxSplashScreen') != 'NONE'"},
                                                {cmp:cmpBlkLblV2dRxVideoLookup,
                                                 sel:"topic_Get(\"" + tDlg + "\", 'EditBlkRxVideoLookup') != 'NONE'"}
                                              ],
                                              [{ action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkTxVideoParam", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkXPPortType", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkTxPortKeyboardMouse", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkTxSerialParam", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkTxSplashScreen", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkTxVideoLookup", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkRxVideoParam", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkRxSerialParam", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkRxSplashScreen", eventval: null },
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "EditBlkRxVideoLookup", eventval: null }
                                              ]
                                             );
  
  var cmpLblAssetEditXPPortType = new CmpLabelHighlight([BlkJSX,"lblAssetEditXPPortType"], null, 
                                                        [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                        {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"XPPortType"} );
  var cmpLblV2dTxVideoParam = new CmpLabelHighlight([BlkJSX,"lblV2dTxVideoParam"], null, 
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                    {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dTxVideoParam"} );
  var cmpLblV2dTxPortKeyboardMouse = new CmpLabelHighlight([BlkJSX,"lblV2dTxPortKeyboardMouse"], null, 
                                                           [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                           {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dTxPortKeyboardMouse"} );
  var cmpLblV2dTxSerialParam = new CmpLabelHighlight([BlkJSX,"lblV2dTxSerialParam"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dTxSerialParam"} );
  var cmpLblV2dTxSplashScreen = new CmpLabelHighlight([BlkJSX,"lblV2dTxSplashScreen"], null, 
                                                      [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                      {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dTxSplashScreen"} );
  var cmpLblV2dTxVideoLookup = new CmpLabelHighlight([BlkJSX,"lblV2dTxVideoLookup"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dTxVideoLookup"} );
  var cmpLblV2dRxVideoParam = new CmpLabelHighlight([BlkJSX,"lblV2dRxVideoParam"], null, 
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                    {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dRxVideoParam"} );
  var cmpLblV2dRxSerialParam = new CmpLabelHighlight([BlkJSX,"lblV2dRxSerialParam"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dRxSerialParam"} );
  var cmpLblV2dRxSplashScreen = new CmpLabelHighlight([BlkJSX,"lblV2dRxSplashScreen"], null, 
                                                      [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                      {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dRxSplashScreen"} );
  var cmpLblV2dRxVideoLookup = new CmpLabelHighlight([BlkJSX,"lblV2dRxVideoLookup"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:"V2dRxVideoLookup"} );

  var cmpBlkAssetXPPortType = new Component([BlkJSX,"blkAssetEditXPPortType"]);
  var cmpBlkV2dTxVideoParam = new Component([BlkJSX,"blkAssetV2DTxVideoParam"]);
  var cmpBlkV2dTxPortKeyboardMouse = new Component([BlkJSX,"blkAssetV2DTxPortKeyboardMouse"]);
  var cmpBlkV2dTxSerialParam = new Component([BlkJSX,"blkAssetV2DTxSerialParam"]);
  var cmpBlkV2dTxSplashScreen = new Component([BlkJSX,"blkAssetV2DTxSplashScreen"]);
  var cmpBlkEditLUTMatrix = new CmpList([BlkJSX,"mtxLookupTable"],
                                        null, null, null,
                                        {action:"TOPICEVENT", topic:tDlg, eventvar:"LUTSelected", eventval:"jsxid"});
  cmpBlkEditLUTMatrix.BlkJSX.setXMLTransformers("xsl/TxV2DLookupTableToCDF.xsl");

  var cmpBlkV2dTxVideoLookup = new CmpContainer([BlkJSX,"blkAssetV2DTxVideoLookup"],
                                                [{cmp:cmpBlkEditLUTMatrix}],
                                                null);

  var cmpSelEditAssetGenlockModeList = new CmpStaticList([BlkJSX,"selV2dRxGenlockMode"],
                                                         "xml/selections/V2DGenlockMode.xml",
                                                         null,
                                                         {action:"FNCCALL", 
                                                         context:null, 
                                                         fnc:function() {
                                                            if(!jsx3.util.strEmpty(this.selRecord)){
                                                              var SelAssetType = this.selRecord.jsxid;
                                                              topic_Publish(tDlg, "EditAssetGenlockModeSelection", SelAssetType);
                                                            }
                                                         }
                                                        });
  
  var cmpBlkEditMasterDelay = new CmpTxtBox([BlkJSX,"txtMasterDelay"], null, null, null, null);
  cmpBlkEditMasterDelay.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);

  var cmpBlkV2dRxVideoParam = new CmpContainer([BlkJSX,"blkAssetV2DRxVideoParam"],
                                               [ {cmp:cmpSelEditAssetGenlockModeList},
                                                 {cmp:cmpBlkEditMasterDelay}
                                                ],
                                                null);

  var cmpBlkV2dRxSerialParam = new Component([BlkJSX,"blkAssetV2DRxSerialParam"]);
  var cmpBlkV2dRxSplashScreen = new Component([BlkJSX,"blkAssetV2DRxSplashScreen"]);
  var cmpBlkEditRxLUTMatrix = new CmpList([BlkJSX,"mtxRxLookupTable"],
                                       null, null, null,
                                       {action:"TOPICEVENT", topic:tDlg, eventvar:"RxLUTSelected", eventval:"jsxid"});
  cmpBlkEditRxLUTMatrix.BlkJSX.setXMLTransformers("xsl/RxV2DLookupTableToCDF.xsl");

  var cmpBlkV2dRxVideoLookup = new CmpContainer([BlkJSX,"blkAssetV2DRxVideoLookup"],
                                                [{cmp:cmpBlkEditRxLUTMatrix}],
                                                null);
  
  var cmpBlkAssetV2DEncoderAVUI = new CmpContainer( [BlkJSX,"addV2DDetail"],
                                                    [{cmp:cmpBlkAssetXPPortType,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'XPPortType'"},
                                                     {cmp:cmpBlkV2dTxVideoParam,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dTxVideoParam'"},
                                                     {cmp:cmpBlkV2dTxPortKeyboardMouse,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dTxPortKeyboardMouse'"},
                                                     {cmp:cmpBlkV2dTxSerialParam,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dTxSerialParam'"},
                                                     {cmp:cmpBlkV2dTxSplashScreen,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dTxSplashScreen'"},
                                                     {cmp:cmpBlkV2dTxVideoLookup,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dTxVideoLookup'"},
                                                     {cmp:cmpBlkV2dRxVideoParam,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dRxVideoParam'"}, 
                                                     {cmp:cmpBlkV2dRxSerialParam,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dRxSerialParam'"},
                                                     {cmp:cmpBlkV2dRxSplashScreen,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dRxSplashScreen'"},
                                                     {cmp:cmpBlkV2dRxVideoLookup,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditV2DEncoderPortConfigSelection') == 'V2dRxVideoLookup'"}
                                                    ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditV2DEncoderPortConfigSelection", eventval:null}] 
                                                   );
  
  var cmpBtnEditAssetClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAddAssetPorts"],
                                            null, 
                                            {action:"FNCCALL", 
                                             context:null, 
                                             fnc:function() {
                                               dlgEditAsset_Destroy();
                                             }
                                            });
            
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetPorts"],
                                        null, 
                                        {action:"FNCCALL", 
                                         context:null, 
                                         fnc:function() {
                                           eventDlg.subscribe("EditAssetSetAssetStatePorts", "DONE", this,
                                                             function() {
                                                               dlgEditAsset_Destroy();
                                                             });
                                           cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                         }
                                        });

  var cmpBtnV2dVideoLookupDelete = new CmpButton([BlkJSX,"btnDeleteLUT"], 
                                                 [{action:"TOPICEVENT", topic:tDlg, eventvar:"LUTSelected", eventval:null}],
                                                 {action:"FNCCALL", 
                                                  context:this,
                                                      fnc:function() {
                                                          var selRecordID = cmpBlkEditLUTMatrix.BlkJSX.getValue();
                                                          var selRecordNode = cmpBlkEditLUTMatrix.BlkJSX.getRecordNode(selRecordID);
                                                          if( (selRecordID != null) && (selRecordNode != null) ) {
                                                            var stateVal = selRecordNode.getAttribute("jsxLookupEntryDirtyBit");
                                                            if( stateVal == "ADD" || stateVal == "UPDATELIST") {
                                                              alert("Cannot delete LUT entry until saved");
                                                              return false;
                                                            }
                                                            else{
                                                              selRecordNode.setAttribute("jsxLookupEntryDirtyBit","DELETE");
                                                              cmpBlkEditLUTMatrix.BlkJSX.redrawRecord(selRecordID,jsx3.xml.CDF.UPDATE);
                                                            
                                                              cmpBlkEditLUTMatrix.BlkJSX.redrawRecord(cmpBlkEditLUTMatrix.selRecordID, "UPDATE");
                                                              
                                                              cmpDlgEditAssetSetDirtyBitValue(PNameSpace.getJSXByName("btnDeleteLUT"));  
                                                            }
                                                          }
                                                      }
                                                 });

  var cmpBtnV2dVideoLookupEdit = new CmpButton([BlkJSX,"btnEditLUT"], 
                                               [{action:"TOPICEVENT", topic:tDlg, eventvar:"LUTSelected", eventval:null}],
                                               {action:"FNCCALL", 
                                                context:this,
                                                    fnc:function() {
                                                      var selRecordID = cmpBlkEditLUTMatrix.BlkJSX.getValue();
                                                      var nodeLUT = cmpBlkEditLUTMatrix.BlkJSX.getRecordNode(selRecordID);
                                                      var stateVal = nodeLUT.getAttribute("jsxLookupEntryDirtyBit");
                                                      if( stateVal == "DELETE" || stateVal == "ADD") {
                                                        alert("Cannot edit LUT entry until saved");
                                                        return false;
                                                      } else {
                                                        dlgEditLUT_Create(tDlg, BlkJSX, cmpBlkEditLUTMatrix.BlkJSX, nodeLUT, cmpBlkEditLUTMatrix.selRecordID);
                                                      }
                                                    }
                                               });
  
  var cmpBtnV2dVideoLookupAdd = new CmpButton([BlkJSX,"btnAddLUT"],
                                              null, 
                                              {action:"FNCCALL", 
                                               context:this,
                                                   fnc:function() {
                                                        dlgEditLUT_Create(tDlg, BlkJSX, cmpBlkEditLUTMatrix.BlkJSX, null, null);
                                                   }
                                              });

  var cmpBtnV2dRxVideoLookupDelete = new CmpButton([BlkJSX,"btnRxDeleteLUT"], 
                                                   [{action:"TOPICEVENT", topic:tDlg, eventvar:"RxLUTSelected", eventval:null}],
                                                    {action:"FNCCALL", 
                                                      context:this,
                                                      fnc:function() {
                                                            var selRecordID = cmpBlkEditRxLUTMatrix.BlkJSX.getValue();
                                                            var selRecordNode = cmpBlkEditRxLUTMatrix.BlkJSX.getRecordNode(selRecordID);
                                                            if( (selRecordID != null) && (selRecordNode != null) ) {
                                                              var stateVal = selRecordNode.getAttribute("jsxLookupEntryDirtyBit");
                                                              if( stateVal == "ADD" || stateVal == "UPDATELIST") {
                                                                alert("Cannot delete LUT entry until saved");
                                                                return false;
                                                              }
                                                              else{
                                                                selRecordNode.setAttribute("jsxLookupEntryDirtyBit","DELETE");
                                                                cmpBlkEditRxLUTMatrix.BlkJSX.redrawRecord(selRecordID,jsx3.xml.CDF.UPDATE);
                                                              
                                                                cmpBlkEditRxLUTMatrix.BlkJSX.redrawRecord(cmpBlkEditRxLUTMatrix.selRecordID, "UPDATE");  
                                                                
                                                                cmpDlgEditAssetSetDirtyBitValue(PNameSpace.getJSXByName("btnRxDeleteLUT"));
                                                              }
                                                            }
                                                        }
                                                      });

  var cmpBtnV2dRxVideoLookupEdit = new CmpButton([BlkJSX,"btnRxEditLUT"], 
                                                 [{action:"TOPICEVENT", topic:tDlg, eventvar:"RxLUTSelected", eventval:null}],
                                                 {action:"FNCCALL", 
                                                  context:this,
                                                      fnc:function() {
                                                        var selRecordID = cmpBlkEditRxLUTMatrix.BlkJSX.getValue();
                                                        var nodeLUT = cmpBlkEditRxLUTMatrix.BlkJSX.getRecordNode(selRecordID);
                                                        var stateVal = nodeLUT.getAttribute("jsxLookupEntryDirtyBit");
                                                        if( stateVal == "DELETE" || stateVal == "ADD") {
                                                          alert("Cannot edit LUT entry until saved");
                                                          return false;
                                                        } else {
                                                           dlgEditLUT_Create(tDlg, BlkJSX, cmpBlkEditRxLUTMatrix.BlkJSX, nodeLUT, cmpBlkEditRxLUTMatrix.selRecordID);
                                                        }
                                                      }
                                                 });

  var cmpBtnV2dRxVideoLookupAdd = new CmpButton([BlkJSX,"btnRxAddLUT"],
                                                null, 
                                                {action:"FNCCALL", 
                                                 context:this,
                                                     fnc:function() {
                                                        dlgEditLUT_Create(tDlg, BlkJSX, cmpBlkEditRxLUTMatrix.BlkJSX, null);
                                                     }
                                                });

  var cmpTxtXP200PortIDShow = new Component([BlkJSX,"txtXP200PortIDShow"]);
  var cmpRadioXP200PortTypeSource = new Component([BlkJSX,"radioXP200PortTypeSource"]);
  var cmpRadioXP200PortTypeDestination = new Component([BlkJSX,"radioXP200PortTypeDestination"]);
  
  var cmpBlkAssetAVSettings = new CmpContainer( [BlkJSX,"blkAssetAVSettings"],
                                                [{cmp:cmpBlkEditAssetAVNav},
                                                 {cmp:cmpLblAssetEditXPPortType},
                                                 {cmp:cmpLblV2dTxVideoParam},
                                                 {cmp:cmpLblV2dTxPortKeyboardMouse},
                                                 {cmp:cmpLblV2dTxSerialParam},
                                                 {cmp:cmpLblV2dTxSplashScreen},
                                                 {cmp:cmpLblV2dTxVideoLookup},
                                                 {cmp:cmpLblV2dRxVideoParam},  
                                                 {cmp:cmpLblV2dRxSerialParam},
                                                 {cmp:cmpLblV2dRxSplashScreen},
                                                 {cmp:cmpLblV2dRxVideoLookup},  
                                                 {cmp:cmpBlkAssetV2DEncoderAVUI}
                                                ],
                                                null
                                              ); 
                                                
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"DialogAVSettings"],
                                         [ {cmp:cmpBtnEditAssetOK},
                                           {cmp:cmpBtnEditAssetCancel},
                                           {cmp:cmpBtnEditAssetClose},
                                           {cmp:cmpBtnV2dVideoLookupAdd},
                                           {cmp:cmpBtnV2dVideoLookupEdit},
                                           {cmp:cmpBtnV2dVideoLookupDelete},
                                           {cmp:cmpBtnV2dRxVideoLookupAdd},
                                           {cmp:cmpBtnV2dRxVideoLookupEdit},
                                           {cmp:cmpBtnV2dRxVideoLookupDelete},
                                           {cmp:cmpTxtXP200PortIDShow},
                                           {cmp:cmpRadioXP200PortTypeSource},
                                           {cmp:cmpRadioXP200PortTypeDestination}, 
                                           {cmp:cmpBlkAssetAVSettings},
                                           {cmp:cmpCDFEditAssetAVSettings}
                                         ],
                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetDataState", eventval:AssetNID}]);                            
                            
  cmpDlgEditAsset.BlkJSX = BlkJSX;
  cmpDlgEditAsset.ObjectNID = AssetNID;
  // fnc cmpReq_GetCompleteDeviceWithReadQuery defined in cmpReq.js
  cmpDlgEditAsset.getBlkDevice = cmpReq_GetDeviceConfig;
  cmpDlgEditAsset.tDlg = tDlg;
  cmpDlgEditAsset.setDirtyBitChangeHandler =  cmpDlgEditAssetSetDirtyBitChangeHandler;
  
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.setDirtyBitChangeHandler();
                 this.getBlkDevice(this.tDlg, this.ObjectNID);
                 }
               ),
               null, cmpDlgEditAsset );

  // event published from func ( cmpReq_GetCompleteDeviceWithReadQuery ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteDeviceDone", AssetNID, this,
                      function() {
                        cmpDlgEditAssetGetAsset(AssetNID);
                      }
                    ); // subscribe end    

  // function to set dirtybit change function on the textboxes, select boxes
  // check boxes. Any value change witkk set the dirty bit true

  function cmpDlgEditAssetSetDirtyBitChangeHandler() {
    var objParent = cmpBlkAssetAVSettings.BlkJSX.getParent();
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
    
    // find all descendant of type RadioButton and set its select event
    var RadioButtonNameArr = objParent.getDescendantsOfType(jsx3.gui.RadioButton, false);
    for(var i=0; i < RadioButtonNameArr.length; i++ ) {
      var RadioBlkJSX = RadioButtonNameArr[i];
      RadioBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.SELECT);
    }    
  }  // end cmpDlgEditAssetSetDirtyBitChangeHandler

  // fnc to get dlg data
  function cmpDlgEditAssetGetAsset(AssetNID) {

    var reqStatus = topic_Get(tDlg, "GetCompleteDeviceStatus");
    if(reqStatus != "SUCCESS") {
      dlgEditAsset_Destroy();  
    }
    else {
      var deviceNode = getDialogDataNode(tDlg, "Device");
      var AssetName = deviceNode.getAttribute("title");
      var AssetType = deviceNode.getAttribute("type"); 
      topic_Publish(tDlg, "tabPortSelected","DATE");      
      
      DeviceNID = deviceNode.getAttribute("NID");
      var txVideoConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig");
      var xpVideoConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/XP200PortConfig/V2DEncoderVideoConfig");
      if( !jsx3.util.strEmpty(txVideoConfig) || !jsx3.util.strEmpty(xpVideoConfig)){
        if(!jsx3.util.strEmpty(txVideoConfig)){
          cmpCDFEditAssetAVSettings.BlkJSX.setXMLTransformers("xsl/EditAssetTxAVSettings.xsl");
        }
        else if(!jsx3.util.strEmpty(xpVideoConfig)){
          cmpCDFEditAssetAVSettings.BlkJSX.setXMLTransformers("xsl/EditAssetXPAVSettings.xsl");
        }
        topic_Publish(tDlg, "EditBlkTxVideoParam", "Show");
      }
      var txKBMConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderPortKbmConfig");
      var xpKBMConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/XP200PortConfig/XP200KBMSConfig");
      if( !jsx3.util.strEmpty(txKBMConfig) || !jsx3.util.strEmpty(xpKBMConfig)) {
        topic_Publish(tDlg, "EditBlkTxPortKeyboardMouse", "Show");
      }
      var txSerialConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSerialConfig");
      var xpSerialConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/XP200PortConfig/V2DXP200SerialConfig");
      if( !jsx3.util.strEmpty(txSerialConfig) || !jsx3.util.strEmpty(xpSerialConfig)) {
        topic_Publish(tDlg, "EditBlkTxSerialParam", "Show");
      }
      var txSplashParameters = deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderSplashParametersConfig");
      var xpSplashParameters = deviceNode.selectSingleNode(".//AVCodecElementsConfig/XP200PortConfig/V2DEncoderSplashParametersConfig");
      if( !jsx3.util.strEmpty(txSplashParameters) || !jsx3.util.strEmpty(xpSplashParameters)) {
        topic_Publish(tDlg, "EditBlkTxSplashScreen", "Show");
      }
      var VideoLookupTableNode = "";
      if (!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//V2DCodecVideoLookupTable"))){
        topic_Publish(tDlg, "EditBlkTxVideoLookup", deviceNode.selectSingleNode(".//V2DCodecVideoLookupTable"));
        VideoLookupTableNode = deviceNode.selectSingleNode(".//V2DCodecVideoLookupTable").cloneNode(true); 
        var strXMLCacheId = cmpBlkEditLUTMatrix.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
        objXML.loadXML(VideoLookupTableNode.toString());
        cmpBlkEditLUTMatrix.BlkJSX.setSourceXML(objXML);        
        cmpBlkEditLUTMatrix.BlkJSX.repaint();
      }
      else{
        topic_Publish(tDlg, "EditBlkRxVideoLookup", deviceNode.selectSingleNode(".//V2DDecoderVideoLookupTable"));
        VideoLookupTableNode = deviceNode.selectSingleNode(".//V2DDecoderVideoLookupTable").cloneNode(true); 
        var strXMLCacheId = cmpBlkEditRxLUTMatrix.BlkJSX.getXMLId();
        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
        objXML.loadXML(VideoLookupTableNode.toString());
        cmpBlkEditRxLUTMatrix.BlkJSX.setSourceXML(objXML);        
        cmpBlkEditRxLUTMatrix.BlkJSX.repaint();
      }
      var rxVideoConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderVideoConfig");
      var xpRxVideoConfig = deviceNode.selectSingleNode(".//AVCodecElementsConfig/XP200PortConfig/V2DDecoderVideoConfig");
      if( !jsx3.util.strEmpty(rxVideoConfig) || !jsx3.util.strEmpty(xpRxVideoConfig)){
        if(!jsx3.util.strEmpty(rxVideoConfig)){
          cmpCDFEditAssetAVSettings.BlkJSX.setXMLTransformers("xsl/EditAssetRxAVSettings.xsl");
          cmpLblV2dRxVideoParam.BlkJSX.setText("Video Parameters", true);
          cmpLblV2dRxSplashScreen.BlkJSX.setText("Splash Screen", true);
        }
        topic_Publish(tDlg, "EditBlkRxVideoParam", "Show");
      }
      if( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig"))) {
        topic_Publish(tDlg, "EditBlkRxSerialParam", deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSerialConfig"));
      }
      var rxSplashParameters = deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DDecoderConfig/V2DDecoderSplashParametersConfig");
      var xpRxSplashParameters = deviceNode.selectSingleNode(".//AVCodecElementsConfig/XP200PortConfig/V2DDecoderSplashParametersConfig");
      if( !jsx3.util.strEmpty(rxSplashParameters) || !jsx3.util.strEmpty(xpRxSplashParameters)) {      
        topic_Publish(tDlg, "EditBlkRxSplashScreen", "Show");
      }
      if ( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//DeviceConfig/AVCodecElementsConfig/XP200PortConfig"))) {
        var DeviceConfigIter = deviceNode.selectNodeIterator(".//DeviceConfig/AVCodecElementsConfig/XP200PortConfig");
        while( DeviceConfigIter.hasNext() ) {
          var DeviceConfigNode = DeviceConfigIter.next();
          var PortID = DeviceConfigNode.selectSingleNode(".//PortID").getValue();
          if( PortID != selectedPort ) {
            deviceNode.selectSingleNode(".//XP200PortConfig").getParent().removeChild(DeviceConfigNode);
          }
        }
      }
      if ( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//Info/PortIOType"))){
        topic_Publish(tDlg, "EditBlkXPPortType", deviceNode.selectSingleNode(".//Info/PortIOType"));
      }
      if( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig"))) {
        topic_Publish(tDlg ,"EditV2DEncoderPortConfigSelection", "V2dTxVideoParam");
        PNameSpace.getJSXByName("mtxLookupTable").repaintData();
      } 
      if ( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//V2DDecoderConfig/V2DDecoderVideoConfig"))) {
        topic_Publish(tDlg ,"EditV2DEncoderPortConfigSelection", "V2dRxVideoParam");
        PNameSpace.getJSXByName("mtxRxLookupTable").repaintData();
      } 

      // set the Device Config data
      var strXMLCacheId = cmpCDFEditAssetAVSettings.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(deviceNode.toString());
      cmpCDFEditAssetAVSettings.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditAssetAVSettings.BlkJSX.setCDFId(DeviceNID);

      // Finish and repaint rule data
      topic_Publish(tDlg, "EditAssetDataState", AssetNID);    
      
      if ( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//DeviceConfig/AVCodecElementsConfig/XP200PortConfig"))) {
        topic_Publish(tDlg ,"EditV2DEncoderPortConfigSelection", "XPPortType");
        PNameSpace.getJSXByName("mtxLookupTable").repaintData();
        // get the port id
        var portID = deviceNode.selectSingleNode(".//XP200PortConfig/PortID").getValue();
        var portIDShowBlkJSX = cmpTxtXP200PortIDShow.BlkJSX;
        var portIDShow = parseInt(portID);
        portIDShowBlkJSX.setValue(portIDShow, true); 

        var radioXP200PortTypeSourceBlkJSX = cmpRadioXP200PortTypeSource.BlkJSX;
        var radioXP200PortTypeDestinationBlkJSX = cmpRadioXP200PortTypeDestination.BlkJSX;              
        // get the port type
        var type = deviceNode.selectSingleNode(".//Info/PortIOType").getValue();
        if(type == "StreamSrc") {
          topic_Publish(tDlg, "EditBlkRxSplashScreen", "NONE");
          radioXP200PortTypeSourceBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
          radioXP200PortTypeDestinationBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);        
        } else if(type == "StreamDst") {
          topic_Publish(tDlg, "EditBlkTxSplashScreen", "NONE");
          radioXP200PortTypeSourceBlkJSX.setSelected(jsx3.gui.RadioButton.UNSELECTED);
          radioXP200PortTypeDestinationBlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
        }  
      }      
    }    

  }// end get dlg data function

  function validateVideoandIOParameters(devicePortsNode){
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/SamplingPhase").getValue(), "txtV2dTxSampling", "Sampling Phase", -16, 15, "") != true) return;
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/NoiseElimination/Coarse").getValue(), "txtV2dTxCoarseNoise", "Coarse Noise Elimination", 1, 256, "") != true) return;
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/NoiseElimination/Fine").getValue(), "txtV2dTxFineNoise", "Fine Noise Elimination", 1, 2048, "") != true) return;
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/Position/Horizontal").getValue(), "txtV2dTxHorz", "Horizontal Position", -135, 375, "") != true) return;
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/Position/Vertical").getValue(), "txtV2dTxVert", "Vertical Position", -25, 50, "") != true) return;
    if (devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "Auto" || devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "50" || devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "100" || 
        devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "150" || devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "250" || devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "350" ||
        devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "500" || devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "750" || devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue() == "1500") {
    }
    else {
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderVideoConfig/LUTPumpCurr").getValue(), "txtV2dTxLUTChargePump", "LUT Charge Pump Current", "Auto or 50 or 100 or 150 or 250 or 350 or 500 or 750 or 1500", "", "LUT") != true) return;
    }
    return true;
  }

  function validateKbmConfigParameters(devicePortsNode){
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderPortRFBConfig/Port").getValue(), "txtV2dTxRFBPort", "Port", 0, 65535, "")!= true) return;
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderPortRFBConfig/XOffset").getValue(), "txtV2dTxXOffset", "X Offset", -65535, 65535, "") != true) return;
    if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderPortRFBConfig/YOffset").getValue(), "txtV2dTxYOffset", "Y Offset", -65535, 65535, "") != true) return;
    return true;
  }

  function validateVideoandIOSerialConfig(devicePortsNode){
    if(!jsx3.util.strEmpty(devicePortsNode.selectSingleNode(".//V2DEncoderSerialConfig"))){
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSerialConfig/DataBits").getValue(), "txtDataBits", "Data Bits", 5, 8, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSerialConfig/StopBits").getValue(), "txtStopBits", "Stop Bits", 1, 2, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSerialConfig/MaximumWaitTime").getValue(), "txtMaxWait", "Maximum Wait Time", 0, 200, "")!= true) return;
    }
    else{
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSerialConfig/DataBits").getValue(), "txtV2dRxSerialDatabits", "Data Bits", 5, 8, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSerialConfig/StopBits").getValue(), "txtV2dRxSerialStopBits", "Stop Bits", 1, 2, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSerialConfig/MaximumWaitTime").getValue(), "txtV2dRxSerialMaxWait", "Maximum Wait Time", 0, 200, "")!= true) return;
    }
    return true;
  }

  function validateVideoSplashParameters(devicePortsNode, deviceNode){
    if( !jsx3.util.strEmpty(devicePortsNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig")) || (!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//Info/PortIOType")) && deviceNode.selectSingleNode(".//Info/PortIOType").getValue() == "StreamSrc")) {
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/Resolution/Horizontal").getValue(), "txtV2dTxSplashHres", "Horizontal Resolution", 512, 3000, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/Resolution/Vertical").getValue(), "txtV2dTxSplashVres", "Vertical Resolution", 480, 2048, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/RefreshRate").getValue(), "txtV2dTxSplashRefreshRate", "Refresh Rate", 30, 250, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/PixelClockRate").getValue(), "txtV2dTxSplashPixelClock", "Pixel Clock Rate", 30, 200, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/FrontPorch/Horizontal").getValue(), "txtV2dTxSplashHfrontPorch", "Horizontal Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/FrontPorch/Vertical").getValue(), "txtV2dTxSplashVfrontPorch", "Vertical Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/BackPorch/Horizontal").getValue(), "txtV2dTxSplashHbackPorch", "Horizontal Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/BackPorch/Vertical").getValue(), "txtV2dTxSplashVbackPorch", "Vertical Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/Sync/Horizontal").getValue(), "txtV2dTxSplashHsync", "Horizontal Sync", 1, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DEncoderSplashParametersConfig/Sync/Vertical").getValue(), "txtV2dTxSplashVsync", "Vertical Sync", 1, 1000, "")!= true) return;
    }
    else{
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/Resolution/Horizontal").getValue(), "txtV2dRxSplashHres", "Horizontal Resolution", 512, 3000, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/Resolution/Vertical").getValue(), "txtV2dRxSplashVres", "Vertical Resolution", 480, 2048, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/RefreshRate").getValue(), "txtV2dRxSplashRefreshRate", "Refresh Rate", 30, 250, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/PixelClockRate").getValue(), "txtV2dRxSplashPixelClock", "Pixel Clock Rate", 30, 200, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/FrontPorch/Horizontal").getValue(), "txtV2dRxSplashHfrontPorch", "Horizontal Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/FrontPorch/Vertical").getValue(), "txtV2dRxSplashVfrontPorch", "Vertical Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/BackPorch/Horizontal").getValue(), "txtV2dRxSplashHbackPorch", "Horizontal Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/BackPorch/Vertical").getValue(), "txtV2dRxSplashVbackPorch", "Vertical Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/Sync/Horizontal").getValue(), "txtV2dRxSplashHsync", "Horizontal Sync", 1, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderSplashParametersConfig/Sync/Vertical").getValue(), "txtV2dRxSplashVsync", "Vertical Sync", 1, 1000, "")!= true) return;
    }
    return true;
  }

  function validateDecoderVideoandIOParameters(devicePortsNode){
    if(!jsx3.util.strEmpty(devicePortsNode.selectSingleNode(".//V2DDecoderVideoConfig"))){
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderVideoConfig/PixelAdjust").getValue(), "txtV2dRxPixelAdjust", "Pixel Adjust", -25, 25, "") != true) return;
      if (chkForValueExistenceandValidation(devicePortsNode.selectSingleNode(".//V2DDecoderVideoConfig/MasterDelay").getValue(), "txtMasterDelay", "Master Delay", 0, 9, "") != true) return;
    }
    return true;
  }

  //Basic Validation 
  function cmpDlgEditAssetSetAssetValidate() {
    return true;
  }
      
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetSetAsset(AssetNID) {
  
    if( cmpDlgEditAssetSetAssetValidate() != true ) return;
    
    var deviceNode = getDialogDataNode(tDlg, "Device");
    var SelectedAssetAgentJID = deviceNode.getAttribute("ownerUserJID");

    var AssetType = selectedAssetType;
    var devicePortsNode = null;
    // write the data back to the CDF
    cmpCDFEditAssetAVSettings.BlkJSX.write();

    //get the Device Config data
    var strXMLCacheId = cmpCDFEditAssetAVSettings.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = "";
    if ( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//DeviceConfig/AVCodecElementsConfig/XP200PortConfig"))) {
      xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetXPAVSettingsRequest.xsl","EditAssetXPAVSettingsRequest","xsl");
    }
    else if( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//AVCodecElementsConfig/V2DEncoderConfig/V2DEncoderVideoConfig"))){
      xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetTxAVSettingsRequest.xsl","EditAssetTxAVSettingsRequest","xsl");
    }
    else{
      xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetRxAVSettingsRequest.xsl","EditAssetRxAVSettingsRequest","xsl");
    }
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var assetXML = xslDoc.transform(objXML);
    devicePortsNode = new jsx3.xml.Document();
    devicePortsNode.loadXML(assetXML);
    if( devicePortsNode == null ) return;

    //get the LookupTable
    var xslLUTfile = "";
    var objXML = "";
    if ( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//V2DCodecVideoLookupTable"))) {
      var strXMLCacheId = cmpBlkEditLUTMatrix.BlkJSX.getXMLId();
      objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      xslLUTfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToTxV2DLookupTable.xsl","V2DLookupTableNode","xsl");
    }
    else{
      var strXMLCacheId = cmpBlkEditRxLUTMatrix.BlkJSX.getXMLId();
      objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      xslLUTfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToRxV2DLookupTable.xsl","V2DLookupTableNode","xsl");
    }
    var xslDoc = jsx3.xml.XslDocument.wrap(xslLUTfile);
    var newV2DLookupTableXML = xslDoc.transform(objXML);        
    var newV2DLookupTableNode = new jsx3.xml.Document();
    newV2DLookupTableNode.loadXML(newV2DLookupTableXML);
    
    // replace the V2DLookupTableNode in the devicePortsNode
    var oldV2DLookupTableNode = "";
    var groupV2DLookupTableNode = "";
    var decoderLookupTable = deviceNode.selectSingleNode(".//V2DDecoderVideoLookupTable");
    if( jsx3.util.strEmpty(decoderLookupTable)) {
      oldV2DLookupTableNode = devicePortsNode.selectSingleNode(".//V2DCodecVideoLookupTable");              
      groupV2DLookupTableNode = devicePortsNode.selectSingleNode(".//V2DCodecVideoLookupTable").getParent();
    }
    else{
      oldV2DLookupTableNode = devicePortsNode.selectSingleNode(".//V2DDecoderVideoLookupTable");              
      groupV2DLookupTableNode = devicePortsNode.selectSingleNode(".//V2DDecoderVideoLookupTable").getParent();
    }
    var V2DCodecVideoLookupTableNodeVal = oldV2DLookupTableNode.getAttribute("Dirty");
    newV2DLookupTableNode.setAttribute("Dirty", V2DCodecVideoLookupTableNodeVal);
    groupV2DLookupTableNode.replaceNode(newV2DLookupTableNode,oldV2DLookupTableNode);
 
    // find out if port type got changed
    if( !jsx3.util.strEmpty(deviceNode.selectSingleNode(".//Info/PortIOType"))) {
      var radioXP200PortTypeSourceBlkJSX = cmpRadioXP200PortTypeSource.BlkJSX;
      var radioXP200PortTypeDestinationBlkJSX = cmpRadioXP200PortTypeDestination.BlkJSX;              
      var XP200Type = "";
      if(radioXP200PortTypeSourceBlkJSX.getSelected()) {
        XP200Type = "StreamSrc";
      }
      if(radioXP200PortTypeDestinationBlkJSX.getSelected()) {
        XP200Type = "StreamDst";
      }
      devicePortsNode.selectSingleNode(".//Info/PortIOType").setValue(XP200Type);        
    }
    
    // check if anything got chnaged
    var modifyNode = devicePortsNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tDlg, "EditAssetSetAssetStatePorts", "DONE");
      return;   
    }
    else{
      var decoderVideoConfig = devicePortsNode.selectSingleNode(".//AVCodecElementsConfig/V2DDecoderConfig");
      if( jsx3.util.strEmpty(decoderVideoConfig)){
        if(validateVideoandIOParameters(devicePortsNode) != true) return;
        if(validateKbmConfigParameters(devicePortsNode) != true) return;
        if(validateVideoandIOSerialConfig(devicePortsNode) != true) return;
        if(validateVideoSplashParameters(devicePortsNode, deviceNode) != true) return;
        if(validateDecoderVideoandIOParameters(devicePortsNode) != true) return;
      }
      else{
        //For Rx Devices
        if(validateVideoandIOSerialConfig(devicePortsNode) != true) return;
        if(validateVideoSplashParameters(devicePortsNode, deviceNode) != true) return;
        if(validateDecoderVideoandIOParameters(devicePortsNode) != true) return;
      }
    }
    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig", 
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               var resVal = respVal;
                                                               var resCode = respCode;
                                                               topic_Publish(tDlg, "EditAssetSetAssetStatePorts", "DONE");
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->Resource get data request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                     }
                                                   );
                                                                   

    reqSetDeviceConfig.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["DeviceConfig"] = devicePortsNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = AssetNID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
    
  } // end cmpDlgEditAssetSetAsset
       
} // end create
   
function chkForValueExistenceandValidation(a_Val, a_FieldName, a_Msg, a_ValMin, a_ValMax, a_Values) {
  var a_Message = "Please enter " + a_Msg + "";
  var a_Message_Range = "" + a_Msg + " value should be any number in between " + a_ValMin + "-" + a_ValMax + "";
  var a_Message_Values = "" + a_Msg + " value should be " + a_ValMin;
  if (jsx3.util.strEmpty(a_Val)) {
    alert(a_Message);
    PNameSpace.getJSXByName(a_FieldName).focus();
    return false;
  }
  else {
    if(!jsx3.util.strEmpty(a_Values)){
      alert(a_Message_Values);
      PNameSpace.getJSXByName(a_FieldName).focus();
      return false;
    }
    else{
      if(isNaN(a_Val) || (a_Val < a_ValMin || a_Val > a_ValMax)) {
        alert(a_Message_Range);
        PNameSpace.getJSXByName(a_FieldName).focus();
        return false;
      }
    }
  }
  return true;
}
  
function dlgEditLUT_Create(tDlg, parentDialogBlkJSX, cmpMtxBlkJSX, recLUT, recID) {
  var DlgFile = "";
  var AddRule = "";
  var AddDefaultFile = "";
  var dlgEditLUTBlkJSX = "";
  var cmpCDFVideoLookupTable = "";
  // load view - dialog component file
  var deviceNode = getDialogDataNode(tDlg, "Device");
  var decoderLookupTable = deviceNode.selectSingleNode(".//V2DDecoderVideoLookupTable");
  if( jsx3.util.strEmpty(decoderLookupTable)) {
    DlgFile = "components/assets/v2dTxAddVideoLookup.xml";
    AddRule = "V2DAddVideoLookup";
    AddDefaultFile = "JSXAPPS/IPVS_AdminUI/xml/defaults/V2DLookupTableRecord.xml";
    dlgEditLUTBlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
    cmpCDFVideoLookupTable = new Component([dlgEditLUTBlkJSX,"cdfTxVideoLookupTable"]);
    cmpCDFVideoLookupTable.BlkJSX.setXMLTransformers("xsl/DefaultTxVideoLookupTable.xsl");
  } else {
    DlgFile = "components/assets/v2dRxAddVideoLookup.xml";
    AddRule = "V2DRxAddVideoLookup";
    AddDefaultFile = "JSXAPPS/IPVS_AdminUI/xml/defaults/V2DRxLookupTableRecord.xml";    
    dlgEditLUTBlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
    cmpCDFVideoLookupTable = new Component([dlgEditLUTBlkJSX,"cdfRxVideoLookupTable"]);
    cmpCDFVideoLookupTable.BlkJSX.setXMLTransformers("xsl/DefaultRxVideoLookupTable.xsl");
  }
  if( recID == null ) recID = "DEFAULT_LUT_NID";
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);  

  function dlgLUT_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditLUT.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    dlgEditLUTBlkJSX.doClose();    
  }

  var cmpBtnEditLUTClose = new CmpButton([dlgEditLUTBlkJSX, "btnAddVideoLookupClose"],
                                         null, 
                                         {action:"FNCCALL", 
                                          context:null, 
                                          fnc:function() {
                                            dlgLUT_Destroy();
                                          }
                                         });
  var cmpBtnEditLUTCancel = new CmpButton([dlgEditLUTBlkJSX, "btnAddVideoLookupCancel"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgLUT_Destroy();
                                            }
                                          });
  
  function validateVideoLookUPTable(nodeLUT) {
    if (jsx3.util.strEmpty(decoderLookupTable)) {
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHres"), "txtV2dTxVideoLookupHres", "Horizontal Resolution", 512, 3000, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVres"), "txtV2dTxVideoLookupVres", "Vertical Resolution", 480, 2048, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableRefreshRate"), "txtV2dTxVideoLookupRefreshRate", "Refresh Rate", 30, 250, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTablePixelClockRate"), "txtV2dTxVideoLookupPixelClockRate", "Pixel Clock Rate", 30, 200, "")!= true) return;
      if (nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "TRUE" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "FALSE" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "YES" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "NO" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "BOTH") {
      }
      else {
        if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableStereo"), "txtV2dTxVideoLookupStereo", "Stereo", "true or false or Both", "", "Stereo")!= true) return;
      }
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHFrontPorch"), "txtV2dTxVideoLookupHfrontPorch", "Horizontal Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVFrontPorch"), "txtV2dTxVideoLookupVfrontPorch", "Vertical Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHBackPorch"), "txtV2dTxVideoLookupHbackPorch", "Horizontal Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVBackPorch"), "txtV2dTxVideoLookupVbackPorch", "Vertical Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHSync"), "txtV2dTxVideoLookupHsync", "Horizontal Sync", 1, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVSync"), "txtV2dTxVideoLookupVsync", "Vertical Sync", 1, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVerticalMargin"), "txtV2dTxVideoLookupVmargin", "Vertical Margin", 0, 3, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTablePhase"), "txtV2dTxVideoLookupPhase", "Phase", -16, 15, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHPos"), "txtV2dTxVideoLookupHpos", "Horizontal Position", -135, 375, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVPos"), "txtV2dTxVideoLookupVpos", "Vertical Position", -25, 50, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableCoarseNoise"), "txtV2dTxVideoLookupCoarseNoise", "Coarse Noise", 1, 256, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableFineNoise"), "txtV2dTxVideoLookupFineNoise", "Fine Noise", 1, 2048, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableGainRed"), "txtV2dTxVideoLookupRedGain", "Red Gain", 0, 255, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableGainGreen"), "txtV2dTxVideoLookupGreenGain", "Green Gain", 0, 255, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableGainBlue"), "txtV2dTxVideoLookupBlueGain", "Blue Gain", 0, 255, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableOffsetRed"), "txtV2dTxVideoLookupRedOffset", "Red Offset", 0, 127, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableOffsetGreen"), "txtV2dTxVideoLookupGreenOffset", "Green Offset", 0, 127, "") != true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableOffsetBlue"), "txtV2dTxVideoLookupBlueOffset", "Blue Offset", 0, 127, "") != true) return; 
    }
    else {
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHres"), "txtV2dRxVideoLookupHres", "Horizontal Resolution", 512, 3000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVres"), "txtV2dRxVideoLookupVres", "Vertical Resolution", 480, 2048, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableRefreshRate"), "txtV2dRxVideoLookupRefreshRate", "Refresh Rate", 30, 250, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTablePixelClockRate"), "txtV2dRxVideoLookupPixelClockRate", "Pixel Clock Rate", 30, 200, "")!= true) return;
      if (nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "TRUE" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "FALSE" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "YES" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "NO" || nodeLUT.getAttribute("jsxLookupTableStereo").toUpperCase() == "BOTH") {
      }
      else {
        if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableStereo"), "txtV2dRxVideoLookupStereo", "Stereo", "true or false or Both", "", "Stereo")!= true) return;
      }
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHFrontPorch"), "txtV2dRxVideoLookupHfrontPorch", "Horizontal Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVFrontPorch"), "txtV2dRxVideoLookupVfrontPorch", "Vertical Front Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHBackPorch"), "txtV2dRxVideoLookupHbackPorch", "Horizontal Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVBackPorch"), "txtV2dRxVideoLookupVbackPorch", "Vertical Back Porch", 0, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableHSync"), "txtV2dRxVideoLookupHsync", "Horizontal Sync", 1, 1000, "")!= true) return;
      if (chkForValueExistenceandValidation(nodeLUT.getAttribute("jsxLookupTableVSync"), "txtV2dRxVideoLookupVsync", "Vertical Sync", 1, 1000, "") != true) return;
    }
    return true;
  }

  var cmpBtnEditLUTOK = new CmpButton([dlgEditLUTBlkJSX, "btnAddVideoLookupAdd"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              // genratethe request xml
                                              cmpCDFVideoLookupTable.BlkJSX.write();
                                              var strXMLId = cmpCDFVideoLookupTable.BlkJSX.getXMLId();
                                              var objectXML = PNameSpace.getCache().getDocument(strXMLId);      
                                              var xslfile = "";
                                              if( jsx3.util.strEmpty(decoderLookupTable)) {
                                                xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/TxVideoLookTableRequest.xsl","TxVideoLookTableRequest","xsl");
                                              }
                                              else {
                                                xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/RxVideoLookTableRequest.xsl","RxVideoLookTableRequest","xsl");
                                              }
                                              var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
                                              var LUTXML = xslDoc.transform(objectXML);
                                              var docLUT = new jsx3.xml.Document();
                                              docLUT.loadXML(LUTXML);
                                              //var docLUT = utilRuleGetOutboundDoc(AddRule);
                                              var nodeLUT = docLUT.selectSingleNode("//record");
                                              if( nodeLUT != null ) {
                                                // check for duplicate video lookup id
                                                //var objMatrix = PNameSpace.getJSXByName("mtxLookupTable");
                                                var objMatrix = cmpMtxBlkJSX;
                                                var lutID = "";
                                                var lutIDBlkJSX = "";
                                                var newDirtyBitState = "";
                                                var newDirtyBitStateBlkJSX = "";
                                                var val = true;
                                                var lookupId = nodeLUT.getAttribute("jsxid");
                                                var lookupName = nodeLUT.getAttribute("jsxLookupTableName");
                                                if( jsx3.util.strEmpty(decoderLookupTable)) {
                                                  if(jsx3.util.strEmpty(lookupId) || lookupId <= "0" || isNaN(lookupId)){
                                                    alert("Please enter positive integer other than zero for ID");
                                                    PNameSpace.getJSXByName("txtV2dTxVideoLookupID").focus();
                                                    return false;
                                                  }
                                                  if(jsx3.util.strEmpty(lookupName)){
                                                    alert("Please enter Video Lookup Name");
                                                    PNameSpace.getJSXByName("txtV2dTxVideoLookupName").focus();
                                                    return false;
                                                  }
                                                  if (validateVideoLookUPTable(nodeLUT) != true) return;
                                                  lutIDBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dTxVideoLookupID"]);
                                                  newDirtyBitStateBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dTxVideoLookupDirtyBit"]);
                                                } else {
                                                  if(jsx3.util.strEmpty(lookupId) || lookupId <= "0" || isNaN(lookupId)){
                                                    alert("Please enter positive integer other than zero for ID");
                                                    PNameSpace.getJSXByName("txtV2dRxVideoLookupID").focus();
                                                    return false;
                                                  }
                                                  if(jsx3.util.strEmpty(lookupName)){
                                                    alert("Please enter Video Lookup Name");
                                                    PNameSpace.getJSXByName("txtV2dRxVideoLookupName").focus();
                                                    return false;
                                                  }
                                                  if (validateVideoLookUPTable(nodeLUT) != true) return;
                                                  lutIDBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dRxVideoLookupID"]);
                                                  newDirtyBitStateBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dRxVideoLookupDirtyBit"]);                                                
                                                }
                                                lutID = lutIDBlkJSX.getValue(); 
                                                newDirtyBitState = newDirtyBitStateBlkJSX.getValue();                                               
                                                 // disabling to check the duplicate Lut ID 
                                                if (objMatrix) {
                                                  var strXMLCacheId = objMatrix.getXMLId();
                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                  if (objXML) {
                                                    var objRecordNodes = objXML.selectNodes("//record");
                                                    for (var it = objRecordNodes.iterator();it.hasNext();){
                                                      var objRecordNode = it.next();
                                                      var strRecordIndex = objRecordNode.getAttribute("jsxLookupTableID");
                                                      var strRecordDirty = objRecordNode.getAttribute("jsxLookupEntryDirtyBit");
                                                      if(newDirtyBitState != "UPDATELIST") {
                                                        if( (lutID == strRecordIndex) && (strRecordDirty!= "DELETE")  ){
                                                          alert("Video Lookup ID already exists");
                                                          PNameSpace.getJSXByName("txtV2dTxVideoLookupID").focus();
                                                          return false;
                                                        }
                                                      }
                                                    }
                                                  }
                                                }                                                
                                                if( recID != null && recID != "DEFAULT_LUT_NID") {
                                                  cmpMtxBlkJSX.deleteRecord(recID, true);  
                                                }
                                                //nodeLUT.setAttribute("jsxLookupEntryDirtyBit","UPDATELIST");
                                                cmpMtxBlkJSX.insertRecordNode(nodeLUT);
                                                var btnAddLutBlkJSX = getJsxByName([parentDialogBlkJSX,"btnAddLUT"]);
                                                cmpDlgEditAssetSetDirtyBitValue(btnAddLutBlkJSX);
                                              }
                                              
                                              dlgLUT_Destroy();
                                            }
                                           });                                      
  var cmpDlgEditLUT = new CmpContainer("DialogLUT",
                            [ {cmp:cmpBtnEditLUTClose},
                              {cmp:cmpBtnEditLUTCancel},
                              {cmp:cmpBtnEditLUTOK},
                              {cmp:cmpCDFVideoLookupTable}
                            ],
                            [{ action: "TOPICEVENT", topic: tDlg, eventvar: "LUTDataState", eventval: recID}]);                            
  cmpDlgEditLUT.BlkJSX = dlgEditLUTBlkJSX;  
  
  cmpDlgEditLUT.recLUT = recLUT;
  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                  this.init();
                  var recLUTDoc = "";
                  if( this.recLUT != null ) {
                    recLUTDoc = new jsx3.xml.Document().loadXML(this.recLUT.toString());
                    //this.setMapRuleData(AddRule, "IN", "DOC", recLUTDoc);
                  } else {
                    var defaultLUTXML = loadXMLFile(AddDefaultFile);
                    recLUTDoc = new jsx3.xml.Document().loadXML(defaultLUTXML.toString());
                    //this.setMapRuleData(AddRule, "IN", "FILE", AddDefaultFile);
                  } 
                  var strXMLCacheId = cmpCDFVideoLookupTable.BlkJSX.getXMLId();
                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                  var nodeLUT = recLUTDoc.selectSingleNode("//record");
                  objXML.loadXML("<V2DCodecVideoLookupTable>"+nodeLUT.toString()+"</V2DCodecVideoLookupTable>");
                  cmpCDFVideoLookupTable.BlkJSX.setSourceXML(objXML);         
                  cmpCDFVideoLookupTable.BlkJSX.setCDFId(recID);
                  cmpCDFVideoLookupTable.BlkJSX.repaint();
                  topic_Publish(tDlg, "LUTDataState", recID);

                  this.show();
                  
                  if( this.recLUT != null ) {
                    var dirtyBitStateBlkJSX = "";
                    var videoLookUpIDBlkJSX = "";
                    if( jsx3.util.strEmpty(decoderLookupTable)) {
                      videoLookUpIDBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dTxVideoLookupID"]);
                      dirtyBitStateBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dTxVideoLookupDirtyBit"]);
                    } else {
                      videoLookUpIDBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dRxVideoLookupID"]);
                      dirtyBitStateBlkJSX = getJsxByName([dlgEditLUTBlkJSX, "txtV2dRxVideoLookupDirtyBit"]);                                                
                    }                    
                    dirtyBitStateBlkJSX.setValue("UPDATELIST");
                    videoLookUpIDBlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                  }
                 }
               ),
               null, cmpDlgEditLUT ); 
}

function cmpBlkAssetDMSPort_Create(EncoderPortBlkName, tDlg) {
  //var cmpBlkAssetDMSPort = new Component(EncoderPortBlkName,null);

  var cmpBtnMediaStreamRelayAddGroup = new CmpButton("btnMediaStreamRelayPortAddTag",
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var o = new Object;
                                          o.jsxid = jsx3.xml.CDF.getKey();
                                          o.jsxPortTagName = this.GroupTagNameBlkJSX.getValue();
                                          o.jsxPortTagValue = this.GroupTagValueBlkJSX.getValue();
                                          this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                          this.GroupTagNameBlkJSX.setValue(null);
                                          this.GroupTagValueBlkJSX.setValue(null);
                                          this.GroupTagListBlkJSX.repaintData();
                                          this.cmpDlgEditAssetSetDirtyBitValue(this.btnPortAddTagobjJSX);
                                        }
                                      });
  cmpBtnMediaStreamRelayAddGroup.GroupTagNameBlkJSX = PNameSpace.getJSXByName("btnMediaStreamRelayPortAddTag");
  cmpBtnMediaStreamRelayAddGroup.GroupTagValueBlkJSX = PNameSpace.getJSXByName("btnMediaStreamRelayPortAddTag");
  cmpBtnMediaStreamRelayAddGroup.GroupTagListBlkJSX = PNameSpace.getJSXByName("mtxMediaStreamRelayPortTags");
  cmpBtnMediaStreamRelayAddGroup.btnPortAddTagobjJSX = PNameSpace.getJSXByName("btnMediaStreamRelayPortAddTag");
  cmpBtnMediaStreamRelayAddGroup.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
     
  var cmpBlkEditAssetDMS_MediaStreamRelay = new CmpContainer( "blkMediaStreamRelayPort",
                                                            [ {cmp:cmpBtnMediaStreamRelayAddGroup}
                                                            ],
                                                            null
                                                          );

  var cmpBtnMediaStorePortAddGroup = new CmpButton("btnMediaStorePortAddTag",
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var o = new Object;
                                          o.jsxid = jsx3.xml.CDF.getKey();
                                          o.jsxPortTagName = this.GroupTagNameBlkJSX.getValue();
                                          o.jsxPortTagValue = this.GroupTagValueBlkJSX.getValue();
                                          this.GroupTagListBlkJSX.insertRecord(o, null, false);
                                          this.GroupTagNameBlkJSX.setValue(null);
                                          this.GroupTagValueBlkJSX.setValue(null);
                                          this.GroupTagListBlkJSX.repaintData();
                                          this.cmpDlgEditAssetSetDirtyBitValue(this.btnPortAddTagobjJSX);
                                        }
                                      });
  cmpBtnMediaStorePortAddGroup.GroupTagNameBlkJSX = PNameSpace.getJSXByName("btnMediaStorePortAddTag");
  cmpBtnMediaStorePortAddGroup.GroupTagValueBlkJSX = PNameSpace.getJSXByName("btnMediaStorePortAddTag");
  cmpBtnMediaStorePortAddGroup.GroupTagListBlkJSX = PNameSpace.getJSXByName("mtxMediaStorePortTags");
  cmpBtnMediaStorePortAddGroup.btnPortAddTagobjJSX = PNameSpace.getJSXByName("btnMediaStorePortAddTag");
  cmpBtnMediaStorePortAddGroup.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;                                                          
  
  var cmpBlkEditAssetDMS_MediaStorePort = new CmpContainer( "blkMediaStorePort",
                                                            [ {cmp:cmpBtnMediaStorePortAddGroup}
                                                            ],
                                                            null
                                                          );
  
  var cmpBlkAssetDMSPortTabs = new CmpContainer("blkDMSDevicePortsTabPane",
                                            [{cmp:cmpBlkEditAssetDMS_MediaStorePort},
                                             {cmp:cmpBlkEditAssetDMS_MediaStreamRelay}
                                            ],
                                            null
                                           );
   
  var cmpBlkAssetDMSPort = new CmpContainer(EncoderPortBlkName,
                                            [{cmp:cmpBlkAssetDMSPortTabs}
                                            ],
                                            null
                                           );                                        
  return cmpBlkAssetDMSPort;
}  

