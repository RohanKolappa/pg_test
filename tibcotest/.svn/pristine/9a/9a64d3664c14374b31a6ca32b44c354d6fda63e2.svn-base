// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditMediaSDRStorageConfig_Create(MediaDirNID) {

  //if no nid then return
  if( MediaDirNID == null ) return;

  closeDialogIfExists("DialogMediaDirStorageConfig");
  // load view - dialog component file
  var DlgFile = "components/media/storageManagementDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);

  function dlgEditMediaSDR_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditMediaSDR.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }
  
  BlkJSX.showMask("Loading...");

  topic_Set(tDlg, "StorageConfigType", "NONE");
  topic_Set(tDlg, "ILMType", "NONE");
  topic_Set(tDlg, "ILMThresholdSettings", "NONE");
  topic_Set(tDlg, "ILMRuleSettings", "NONE");
  topic_Set(tDlg, "ShowError", "NONE");
  topic_Set(tDlg, "ILMRuleOlderThan", "M");
  topic_Set(tDlg, "ILMRuleOlderThanVal", "1");
  var errorMsg = "";
  var dirTotalSize = 0;

  var cmpSelTypeValue = new CmpSelect([BlkJSX, "selTypeValue"],
                                      null,
                                      { action: "FNCCALL",
                                        context: null,
                                        fnc: function () {
                                          var typeVal = cmpSelTypeValue.BlkJSX.getValue();
                                          if (jsx3.util.strEmpty(typeVal)) {
                                            typeVal = topic_Get(tDlg, "ILMRuleOlderThanVal");
                                          }
                                          topic_Set(tDlg, "ILMRuleOlderThanVal", typeVal);
                                        }
                                      });

  var cmpSelType = new CmpStaticList([BlkJSX, "selType"],
                                      "xml/selections/ILMRuleOlderThan.xml",
                                      null,
                                      { action: "FNCCALL",
                                        context: null,
                                        fnc: function () {
                                          var type = cmpSelType.BlkJSX.getValue();
                                          if (jsx3.util.strEmpty(type)) {
                                            type = topic_Get(tDlg, "ILMRuleOlderThan");
                                            cmpSelType.BlkJSX.setValue(type);
                                          }
                                          topic_Publish(tDlg, "ILMRuleOlderThan", type);
                                        }
                                      });

   var cmpSelILMMediaExportFormat = new CmpStaticList([BlkJSX,"selILMMediaExportFormat"],
                                                      "xml/selections/MediaExportFormatType.xml",  
                                                      null,
                                                      {action:"FNCCALL", 
                                                       context:null, 
                                                       fnc:function() {
                                                          var selMediaExportFormatVal = cmpSelILMMediaExportFormat.BlkJSX.getValue();
                                                          if (jsx3.util.strEmpty(selMediaExportFormatVal)) return;
                                                       }
                                                      });

  var cmpILMMediaFormatHelp = new CmpBlock([BlkJSX, "blKILMMediaFormatHelp"]);
  cmpILMMediaFormatHelp.BlkJSX.setCanSpy(jsx3.Boolean.TRUE);
  cmpILMMediaFormatHelp.BlkJSX.setEvent("showMediaExportFormatHelp();", jsx3.gui.Interactive.SPYGLASS);

  var cmpTxtILMRuleNASURL = new Component([BlkJSX, "txtILMRuleNASURL"]);

  var cmpBtnRadioILMRuleNAS = new CmpRadioButton([BlkJSX, "btnRadioILMRuleNAS"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   cmpTxtILMRuleNASURL.BlkJSX.setValue("nfs://", true);
                                                   cmpTxtILMRuleNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                   cmpTxtILMRuleNASURL.BlkJSX.setSelection();
                                                   cmpSelILMMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                 }
                                               });

  var cmpBtnRadioILMRuleDelete = new CmpRadioButton([BlkJSX, "btnRadioILMRuleDelete"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   cmpTxtILMRuleNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                   cmpSelILMMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                 }
                                               });

  var cmpBlkILMRuleSettings = new CmpContainer([BlkJSX, "blkILMRuleSettings"],
                                                     [{ cmp: cmpSelTypeValue },
                                                      { cmp: cmpSelType },
                                                      { cmp: cmpSelILMMediaExportFormat},
                                                      { cmp: cmpILMMediaFormatHelp},
                                                      { cmp: cmpTxtILMRuleNASURL },
                                                      { cmp: cmpBtnRadioILMRuleNAS },
                                                      { cmp: cmpBtnRadioILMRuleDelete }
                                                     ],
                                                     null,
                                                     null);


  var cmpChkILMRule = new CmpCheckBox([BlkJSX, "chkILMRule"], null,
                                           { action: "FNCCALL",
                                             context: this,
                                             fnc: function () {
                                               if (cmpChkILMRule.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED) {
                                                 topic_Publish(tDlg, "ILMRuleSettings", "SHOW");
                                               }
                                               else {
                                                 topic_Publish(tDlg, "ILMRuleSettings", "NONE");
                                               }
                                             }
                                           }
                                          );

  var cmpBlkILMRule = new CmpContainer([BlkJSX, "blkILMRule"],
                                             [{ cmp: cmpChkILMRule },
                                              { cmp: cmpBlkILMRuleSettings,
                                                sel: "topic_Get('" + tDlg + "', 'ILMRuleSettings') == 'SHOW'"
                                              }
                                             ],
                                             [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ILMRuleSettings", eventval: null}],
                                             null);

//=======================================================================================================================================================//
  var cmpTxtThresholdDiskCapacity = new Component([BlkJSX,"txtThresholdDiskCapacity"]);
  var cmpTxtThresholdNASURL = new Component([BlkJSX,"txtThresholdNASURL"]);
  var cmpTxtThresholdDiskReduceCapacity = new Component([BlkJSX,"txtThresholdDiskReduceCapacity"]);
  
  var cmpSelThresholdMediaExportFormat = new CmpStaticList([BlkJSX,"selThresholdMediaExportFormat"],
                                                          "xml/selections/MediaExportFormatType.xml",  
                                                          null,
                                                          {action:"FNCCALL", 
                                                           context:null, 
                                                           fnc:function() {
                                                              var selMediaExportFormatVal = cmpSelThresholdMediaExportFormat.BlkJSX.getValue();
                                                              if (jsx3.util.strEmpty(selMediaExportFormatVal)) return;
                                                           }
                                                          });

  var cmpThresholdMediaFormatHelp = new CmpBlock([BlkJSX, "blKThresholdMediaFormatHelp"]);
  cmpThresholdMediaFormatHelp.BlkJSX.setCanSpy(jsx3.Boolean.TRUE);
  cmpThresholdMediaFormatHelp.BlkJSX.setEvent("showMediaExportFormatHelp();", jsx3.gui.Interactive.SPYGLASS);

  var cmpBtnRadioThresholdNAS = new CmpRadioButton([BlkJSX, "btnRadioThresholdNAS"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   cmpTxtThresholdNASURL.BlkJSX.setValue("nfs://", true);
                                                   cmpTxtThresholdNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                   cmpTxtThresholdNASURL.BlkJSX.setSelection();
                                                   cmpSelThresholdMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                 }
                                               });

  var cmpBtnRadioThresholdDelete = new CmpRadioButton([BlkJSX, "btnRadioThresholdDelete"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   cmpTxtThresholdNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                   cmpSelThresholdMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                 }
                                               });

  var cmpBlkThresholdRuleSettings = new CmpContainer([BlkJSX, "blkThresholdRuleSettings"],
                                                     [{ cmp: cmpTxtThresholdDiskCapacity },
                                                      { cmp: cmpTxtThresholdNASURL },
                                                      { cmp: cmpTxtThresholdDiskReduceCapacity},
                                                      { cmp: cmpSelThresholdMediaExportFormat},
                                                      { cmp: cmpThresholdMediaFormatHelp},
                                                      { cmp: cmpBtnRadioThresholdNAS},
                                                      { cmp: cmpBtnRadioThresholdDelete }
                                                     ],
                                                     null,
                                                     null);


  var cmpChkThresholdRule = new CmpCheckBox([BlkJSX, "chkThresholdRule"], null,
                                           { action: "FNCCALL",
                                             context: this,
                                             fnc: function () {
                                               if (cmpChkThresholdRule.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED) {
                                                 topic_Publish(tDlg, "ILMThresholdSettings", "SHOW");
                                               }
                                               else {
                                                 topic_Publish(tDlg, "ILMThresholdSettings", "NONE");
                                               }
                                             }
                                           }
                                          );

  var cmpBlkThresholdRule = new CmpContainer([BlkJSX, "blkThresholdRule"],
                                             [{ cmp: cmpChkThresholdRule },
                                              { cmp: cmpBlkThresholdRuleSettings,
                                                sel: "topic_Get('" + tDlg + "', 'ILMThresholdSettings') == 'SHOW'"}
                                             ],
                                             [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ILMThresholdSettings", eventval: null}],
                                             null);

//**************************************************************************************************************************************************************//
  var cmpBlkILMMenuSettings = new CmpContainer([BlkJSX, "blkILMThresholdRule"],
                                               [{ cmp: cmpBlkThresholdRule,
                                                  sel: "topic_Get('" + tDlg + "', 'ILMType') == 'ThresholdRule'"},
                                                { cmp: cmpBlkILMRule,
                                                  sel: "topic_Get('" + tDlg + "', 'ILMType') == 'ILMRule'"}
                                               ],
                                               null,
                                               null);


  var cmpLblThresholdRule = new CmpLabelHighlight([BlkJSX,"blklblThresholdRule"], null,
                                               [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ILMType", eventval: null}],
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "ILMType", eventval: "ThresholdRule" });

  var cmpLblILMRule = new CmpLabelHighlight([BlkJSX,"blklblILMRule"], null,
                                               [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ILMType", eventval: null}],
                                               { action: "TOPICEVENT", topic: tDlg, eventvar: "ILMType", eventval: "ILMRule" });

  var cmpBlkILMMenu = new CmpContainer([BlkJSX, "blkILMMenu"],
                                    [{ cmp: cmpLblThresholdRule},
                                     { cmp: cmpLblILMRule}
                                    ],
                                    null,
                                    null);

  var cmpBlkIMLRuleCap = new CmpContainer([BlkJSX, "blkILMThresholdRule"],
                                   [{ cmp:cmpBlkILMMenu },
                                    { cmp: cmpBlkILMMenuSettings }
                                   ],
                                   [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ILMType", eventval: null}],
                                   null);

  var cmpTxtSchedule = new Component([BlkJSX,"txtSchedule"]);
  var cmpBlkILMSchedule = new CmpContainer([BlkJSX, "blkScheduler"],
                                             [{ cmp:cmpTxtSchedule }
                                             ],
                                             null,
                                             null); 

  var cmpBlkILM = new CmpContainer([BlkJSX, "blkILM"],
                                    [{ cmp: cmpBlkIMLRuleCap},
                                      { cmp: cmpBlkILMSchedule}
                                    ],
                                    null,
                                    null);

  var cmpTxtDSThreshold = new Component([BlkJSX,"txtDSThreshold"]);
  var cmpMtxDSPriority = new Component([BlkJSX, "mtxMediaDirMediaStore"]);

  var cmpBlkDirectorySpan = new CmpContainer([BlkJSX, "blkDirectorySpan"],
                                             [{ cmp:cmpTxtDSThreshold },
                                              { cmp: cmpMtxDSPriority }
                                             ],
                                             null,
                                             null); 


  var cmpBlkBody = new CmpContainer([BlkJSX, "blkBody"],
                                    [{ cmp: cmpBlkDirectorySpan,
                                      sel: "topic_Get('" + tDlg + "', 'StorageConfigType') == 'DiskSpanning'"},
                                      { cmp: cmpBlkILM,
                                        sel: "topic_Get('" + tDlg + "', 'StorageConfigType') == 'ILM'"}
                                    ],
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "StorageConfigType", eventval: null}],
                                    null);



  var cmpBtnRadioDiskSapnning = new CmpRadioButton([BlkJSX, "btnRadioDiskSpanning"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   topic_Publish(tDlg, "ShowError", "NONE");
                                                   topic_Publish(tDlg, "StorageConfigType", "DiskSpanning");
                                                 }
                                               });

  var cmpBtnRadioIlm = new CmpRadioButton([BlkJSX, "btnRadioIlm"], null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   topic_Publish(tDlg, "ShowError", "NONE");
                                                   topic_Publish(tDlg, "StorageConfigType", "ILM");
                                                   var ilmType = topic_Get(tDlg, "ILMType");
                                                   if (jsx3.util.strEmpty(ilmType) || ilmType == "NONE") {
                                                     ilmType = "ThresholdRule";
                                                   }
                                                   topic_Publish(tDlg, "ILMType", ilmType);
                                                 }
                                               });

  var cmpBtnEditMediaDirOK = new CmpButton([BlkJSX, "btnSave"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              cmpDlgEditMediaSDRSetMediaSDR.call(this, MediaDirNID);
                                            }
                                           });

  var cmpBtnEditMediaDirClose = new CmpButton([BlkJSX, "btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSDR_Destroy();
                                                }
                                               });

  var cmpBtnEditMediaDirCancel = new CmpButton([BlkJSX, "btnCancel"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditMediaSDR_Destroy();
                                                }
                                              });

  var cmpLblErrorMsg = new Component([BlkJSX, "lblMetadataErrorMsg"]);

  var cmpBlkErrorMsg = new Component([BlkJSX, "blkErrorMsg"]);
  var cmpBlkError = new CmpContainer(BlkJSX,
                                    [{ cmp: cmpBlkErrorMsg, sel: "topic_Get('" + tDlg + "', 'ShowError') != 'NONE'"}],
                                    [{ action: "TOPICEVENT", topic: tDlg, eventvar: "ShowError", eventval: null}],
                                    null);

  var cmpDlgEditMediaSDR = new CmpContainer([BlkJSX, "DialogMediaDir"],
                                            [{ cmp: cmpBtnEditMediaDirClose },
                                             { cmp: cmpBtnEditMediaDirCancel },
                                             { cmp: cmpBtnEditMediaDirOK },
                                             { cmp: cmpBtnRadioDiskSapnning },
                                             { cmp: cmpBtnRadioIlm },
                                             { cmp: cmpBlkBody },
                                             { cmp: cmpBlkError }
                                            ],
                                            [{ action: "TOPICEVENT", topic: tDlg, eventvar: "EditMediaDirDataState", eventval: MediaDirNID}]);                            
  
  cmpDlgEditMediaSDR.BlkJSX = BlkJSX;
  cmpDlgEditMediaSDR.ObjectNID = MediaDirNID;
  cmpDlgEditMediaSDR.getBlkData = cmpDlgEditMediaSDRGetMediaSDR;
  cmpDlgEditMediaSDR.tDlg = tDlg;

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.ObjectNID);

                 }
               ),
               null, cmpDlgEditMediaSDR );


  function hideBlkMask() {
    BlkJSX.hideMask("");
  }

  eventDlg.subscribe("ILMRuleOlderThan", null, this,
                      function (eventVar, eventVal) {
                        var minVal = 0;
                        var maxVal = 31
                        if (eventVal == "H") {
                          maxVal = 23;
                        }
                        else if (eventVal == "D") {
                          minVal = 1;
                          maxVal = 31;
                        }
                        else if (eventVal == "M") {
                          minVal = 1;
                          maxVal = 12;
                        }
                        else if (eventVal == "Y") {
                          minVal = 1;
                          maxVal = 15;
                        }
                        cmpSelTypeValue.BlkJSX.resetCacheData();
                        for (var i = minVal; i <= maxVal; i++) {
                          var o = new Object;
                          o.jsxid = i;
                          o.jsxtext = i;
                          o.jsximg = "";
                          cmpSelTypeValue.BlkJSX.insertRecord(o, null, false);
                        }
                        var savVal = topic_Get(tDlg, "ILMRuleOlderThanVal");
                        if (parseInt(savVal, 10) >= minVal && parseInt(savVal, 10) <= maxVal ) {
                          minVal = parseInt(savVal, 10);
                        }

                        cmpSelTypeValue.BlkJSX.setValue(minVal);
                        cmpSelTypeValue.BlkJSX.repaint(); cmpSelTypeValue.BlkJSX.getXML()
                      }
                    )

  function processGetDirResponse(MediaDirNID) {
    var mediaDirNode = topic_GetNode(tDlg, "//*[@NID='" + MediaDirNID + "']");
    var storageConfigNode = mediaDirNode.selectSingleNode(".//StorageConfig");
    dirTotalSize = mediaDirNode.getAttribute("size");
    var diskFullThreshold = storageConfigNode.selectSingleNode("./DirSpanning/@diskFullThreshold").getValue();
    var percentThreshold = ((dirTotalSize - diskFullThreshold) / dirTotalSize) * 100;
    percentThreshold = Math.round(percentThreshold);
    cmpTxtDSThreshold.BlkJSX.setValue(percentThreshold, true);
    //set default value
    cmpSelThresholdMediaExportFormat.BlkJSX.setValue("Proprietary");
    cmpSelILMMediaExportFormat.BlkJSX.setValue("Proprietary");

    var cmpMtxMediaDirMount = cmpMtxDSPriority.BlkJSX;
    cmpMtxMediaDirMount.resetCacheData();

    var mediaStoreIter = storageConfigNode.selectNodeIterator(".//PortInfo/Port");
    while (mediaStoreIter.hasNext()) {
      var mediaStoreNode = mediaStoreIter.next();
      var id = mediaStoreNode.getAttribute("deviceElementId");
      var readOnly = mediaStoreNode.getAttribute("readOnly");
      var priority = mediaStoreNode.getAttribute("priority");
      var presenceMap = mediaStoreNode.getAttribute("presence");
      var storePortNID = mediaStoreNode.getAttribute("NID");
      var storePortName = mediaStoreNode.getAttribute("title");
      var presenceShow = "images/icons/icon_grayBall.png";
      if (presenceMap == "available") {
        presenceShow = "images/icons/icon_greenBall.png";
      }
      var o = new Object;
      o.jsxid = storePortNID;
      o.jsxNID = storePortNID;
      o.jsxPresenceMap = presenceMap;
      o.jsxPresenceShow = presenceShow;
      o.jsxDMS = storePortName;
      o.jsxDeviceElementID = id;
      o.jsxPriority = priority;
      o.jsxReadOnly = readOnly;
      cmpMtxMediaDirMount.insertRecord(o, null, false);
    }
    cmpMtxMediaDirMount.repaintData();

    var whereMode = storageConfigNode.getAttribute("mode");
    //if disk spanning
    if (whereMode == "DirSpanning") {
      cmpBtnRadioDiskSapnning.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);

      topic_Publish(tDlg, "StorageConfigType", "DiskSpanning");

    }
    else if (whereMode == "CapacityManagement") {
      cmpBtnRadioIlm.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
      topic_Set(tDlg, "ILMType", "NONE");

      var thresholdRuleNode = storageConfigNode.selectSingleNode("./CapacityManagement/ThresholdRule");
      var chkThresholdVal = thresholdRuleNode.getAttribute("enable");

      if(chkThresholdVal == "true"){
        cmpChkThresholdRule.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
        var thresholdType = thresholdRuleNode.getAttribute("type");
        var nasURL = thresholdRuleNode.getAttribute("nasDriveURL");
        var diskCapacityReached = thresholdRuleNode.getAttribute("diskCapacityReached");
        var percentDiskCapacity = "";
        if (!jsx3.util.strEmpty(diskCapacityReached)) {
          percentDiskCapacity = ((diskCapacityReached) / dirTotalSize) * 100;
          percentDiskCapacity = Math.round(percentDiskCapacity);
        }

        var reduceDiskCapacity = thresholdRuleNode.getAttribute("reduceDiskCapacity");
        var percentReduceDiskCapacity = "";
        if (!jsx3.util.strEmpty(reduceDiskCapacity)) {
          var percentReduceDiskCapacity = ((reduceDiskCapacity) / dirTotalSize) * 100;
          percentReduceDiskCapacity = Math.round(percentReduceDiskCapacity);
        }
                
        var mediaExportFormat = thresholdRuleNode.getAttribute("format");
        cmpSelThresholdMediaExportFormat.BlkJSX.setValue(mediaExportFormat);
        if (thresholdType == "OFFLOAD") {
          cmpBtnRadioThresholdNAS.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
          cmpTxtThresholdNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
          cmpSelThresholdMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        }
        else if (thresholdType == "DELETE") {
          cmpBtnRadioThresholdDelete.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
          cmpTxtThresholdNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
          cmpSelThresholdMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        }
        cmpTxtThresholdNASURL.BlkJSX.setValue(nasURL, true);
        cmpTxtThresholdDiskCapacity.BlkJSX.setValue(percentDiskCapacity, true);
        cmpTxtThresholdDiskReduceCapacity.BlkJSX.setValue(percentReduceDiskCapacity, true);

        topic_Publish(tDlg, "ILMType", "ThresholdRule");
        topic_Publish(tDlg, "ILMThresholdSettings", "SHOW");
      }
      else {
        cmpChkThresholdRule.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
        topic_Publish(tDlg, "ILMThresholdSettings", "NONE");
      }

      var ilmRuleNode = storageConfigNode.selectSingleNode("./CapacityManagement/ILMRule");
      var chkIlmVal = ilmRuleNode.getAttribute("enable");
     
      if(chkIlmVal == "true"){
        cmpChkILMRule.BlkJSX.setChecked(jsx3.gui.CheckBox.CHECKED);
        var ilmType = ilmRuleNode.getAttribute("type");
        var imlNasURL = ilmRuleNode.getAttribute("nasDriveURL");
        var olderThan = ilmRuleNode.getAttribute("olderThan");
        var selType = "M";
        var selTypeVal = "1";
        if (olderThan.search(/Y/) != -1) {
          selType = "Y";
          selTypeVal = olderThan.replace(/Y/, "");
        }
        else if (olderThan.search(/M/) != -1) {
          selType = "M";
          selTypeVal = olderThan.replace(/M/, "");
        }
        else if (olderThan.search(/D/) != -1) {
          selType = "D";
          selTypeVal = olderThan.replace(/D/, "");
        }
        else if (olderThan.search(/H/) != -1) {
          selType = "H";
          selTypeVal = olderThan.replace(/H/, "");
        }

        topic_Set(tDlg, "ILMRuleOlderThan", selType);
        topic_Set(tDlg, "ILMRuleOlderThanVal", selTypeVal);
        cmpSelType.BlkJSX.setValue(selType);
        cmpSelTypeValue.BlkJSX.setValue(parseInt(selTypeVal, 10));

        if (topic_Get(tDlg, "ILMType") == "NONE") {
          topic_Publish(tDlg, "ILMType", "ILMRule");
        }

        var mediaExportFormat = ilmRuleNode.getAttribute("format");
        cmpSelILMMediaExportFormat.BlkJSX.setValue(mediaExportFormat);
        if (ilmType == "OFFLOAD") {
          cmpBtnRadioILMRuleNAS.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
          cmpTxtILMRuleNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
          cmpSelILMMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        }
        else if (ilmType == "DELETE") {
          cmpBtnRadioILMRuleDelete.BlkJSX.setSelected(jsx3.gui.RadioButton.SELECTED);
          cmpTxtILMRuleNASURL.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
          cmpSelILMMediaExportFormat.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        }
        cmpTxtILMRuleNASURL.BlkJSX.setValue(imlNasURL, true);

        topic_Publish(tDlg, "ILMRuleSettings", "SHOW");
      }
      else {
        cmpChkILMRule.BlkJSX.setChecked(jsx3.gui.CheckBox.UNCHECKED);
        topic_Set(tDlg, "ILMRuleOlderThanVal", "1");
        topic_Set(tDlg, "ILMRuleOlderThan", "Months");
        topic_Publish(tDlg, "ILMRuleSettings", "NONE");
      }

      var schedule = storageConfigNode.selectSingleNode("./CapacityManagement/@schedule").getValue();
      cmpTxtSchedule.BlkJSX.setValue(schedule, true);

      topic_Publish(tDlg, "StorageConfigType", "ILM");
    }

    //publish to repaint and show
    topic_Publish(tDlg, "EditMediaDirDataState", MediaDirNID);
    //hide the mask
    hideBlkMask();
  }

  // fnc to get dlg data
  function cmpDlgEditMediaSDRGetMediaSDR(MediaDirNID) {
    var mediaDirNode = topic_GetNode(tDlg, "//*[@NID='" + MediaDirNID + "']");
    if (mediaDirNode == null) {
      var reqGetMediaDir = new ReqSessionGetRequest("GetDir",
                                                { action: "FNCCALL", context: null,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      var dirNode = respVal.selectSingleNode(".//Dir");
                                                      setDialogDataNode(tDlg, dirNode);
                                                      cmpDlgEditMediaSDRGetMediaSDR(MediaDirNID);
                                                    }
                                                    else {
                                                      hideBlkMask();
                                                      var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                      var ErrorMsg = "Error: MediaStreamDirResource get data request failed \n";
                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                      ErrorMsg += "Error code: " + respCode;
                                                      alert(ErrorMsg);
                                                      return false;
                                                    }
                                                  }
                                                }
                                               );
      reqGetMediaDir.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_WhereNID"] = MediaDirNID;
      reqParams["ObjectList_SelectFormat"] = "EXTENDED";
      reqGetMediaDir.reqParams = reqParams;
      // send the request
      reqGetMediaDir.sendRequest(this);
    }
    else {
      cmpLblErrorMsg.BlkJSX.setText(errorMsg, true);
      processGetDirResponse(MediaDirNID);
     
    }
  } // end cmpDlgEditMediaSDRGetMediaSDR

  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditMediaSDRSetMediaSDR(MediaDirNID) {
    // validate input 
    if (cmpDlgEditMediaSDRValidate() != true) return;

    var diskSpanningVal = cmpBtnRadioDiskSapnning.BlkJSX.getSelected();
    var ilmVal = cmpBtnRadioIlm.BlkJSX.getSelected();
    var dirSpanning = "<DirSpanning diskFullThreshold=''><PortInfo><Port byNID='' priority='' /></PortInfo></DirSpanning>";
    var capacityManagement = "<CapacityManagement schedule=''><ThresholdRule enable='' diskCapacityReached='' reduceDiskCapacity='' type='' nasDriveURL='' format='' /><ILMRule enable='' olderThan='' type='' nasDriveURL='' format='' /></CapacityManagement>";
    var whereMode = "";
    if (diskSpanningVal) {
      dirSpanning = "";
      whereMode = "DirSpanning";
      var diskFullThresholdPercent = cmpTxtDSThreshold.BlkJSX.getValue();
      diskFullThresholdPercent = parseInt(100 - diskFullThresholdPercent, 10);
      var diskFullThreshold = Math.round((dirTotalSize * diskFullThresholdPercent) / 100);
      var port = "";
      var objMatrix = cmpMtxDSPriority.BlkJSX;
      var strXMLCacheId = objMatrix.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      if (objXML) {
        var objRecordNodes = objXML.selectNodes("//record");
        for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
          var objRecordNode = it.next();
          var mediaStorePriority = objRecordNode.getAttribute("jsxPriority");
          var mediaStoreNID = objRecordNode.getAttribute("jsxNID");
          port += "<Port byNID='" + mediaStoreNID + "' priority='" + mediaStorePriority  + "' />";
        }
        port = "<PortInfo>" + port + "</PortInfo>";
      }
      dirSpanning = "<DirSpanning diskFullThreshold='" + diskFullThreshold + "'>" + port + "</DirSpanning>";
    }
    else if (ilmVal) {
      capacityManagement = "";
      whereMode = "CapacityManagement";
      var scheduleVal = cmpTxtSchedule.BlkJSX.getValue();
      var thresholdRule = "";
      var ilmRule = "";
      var chkThresholdVal = cmpChkThresholdRule.BlkJSX.getChecked();
      var chkIlmVal = cmpChkILMRule.BlkJSX.getChecked();
      // check if threshold rule is valid
      if (chkThresholdVal) {
        var thresholdNASVal = cmpBtnRadioThresholdNAS.BlkJSX.getSelected();
        var thresholddeleteVal = cmpBtnRadioThresholdDelete.BlkJSX.getSelected();
        var thresholdDiskCapacityVal = cmpTxtThresholdDiskCapacity.BlkJSX.getValue();
        thresholdDiskCapacityVal = parseInt(thresholdDiskCapacityVal, 10);
        thresholdDiskCapacityVal = Math.round((dirTotalSize * thresholdDiskCapacityVal) / 100);

        var thresholdDiskReduceCapacityVal = cmpTxtThresholdDiskReduceCapacity.BlkJSX.getValue();
        thresholdDiskReduceCapacityVal = parseInt(thresholdDiskReduceCapacityVal, 10);
        thresholdDiskReduceCapacityVal = Math.round((dirTotalSize * thresholdDiskReduceCapacityVal) / 100);

        var thresholdNASURLVal = "";
        var thresholdRuleType = "DELETE";
        if (thresholdNASVal) {
          thresholdRuleType = "OFFLOAD";
          thresholdNASURLVal = cmpTxtThresholdNASURL.BlkJSX.getValue();
        }
        thresholdRule = "<ThresholdRule enable='true' diskCapacityReached='" + thresholdDiskCapacityVal + "' reduceDiskCapacity='" + thresholdDiskReduceCapacityVal + "' type='" + thresholdRuleType + "' nasDriveURL='" + thresholdNASURLVal + "' format='"+cmpSelThresholdMediaExportFormat.BlkJSX.getValue()+"'/>";
      }
      else {
        thresholdRule = "<ThresholdRule enable='false' diskCapacityReached='' reduceDiskCapacity='' type='' nasDriveURL='' format=''/>";
      }

      // check if ILM rule is valid
      if (chkIlmVal) {
        var ilmNASVal = cmpBtnRadioILMRuleNAS.BlkJSX.getSelected();
        var ilmdeleteVal = cmpBtnRadioILMRuleDelete.BlkJSX.getSelected();
        var ilmNASURLVal = "";
        var ilmRuleType = "DELETE";
        if (ilmNASVal) {
          ilmRuleType = "OFFLOAD";
          ilmNASURLVal = cmpTxtILMRuleNASURL.BlkJSX.getValue();
        }
        var timeVal =  cmpSelTypeValue.BlkJSX.getValue();
        var timeTypeVal = cmpSelType.BlkJSX.getValue();


        ilmRule = "<ILMRule enable='true' olderThan='" + timeVal+ timeTypeVal + "' type='" + ilmRuleType + "' nasDriveURL='" + ilmNASURLVal + "' format='"+cmpSelILMMediaExportFormat.BlkJSX.getValue()+"' />";
      }
      else {
        ilmRule = "<ILMRule enable='false' type='' olderThan='' nasDriveURL='' format=''/>";
      }

      capacityManagement = "<CapacityManagement schedule='" + scheduleVal + "'>" + thresholdRule + ilmRule +"</CapacityManagement>";
    }

    var dirSpanningNodeDoc = new jsx3.xml.Document();
    dirSpanningNodeDoc.loadXML(dirSpanning);
    var dirSpanningNode = dirSpanningNodeDoc.selectSingleNode("/*").cloneNode(true);

    var capacityManagementNodeDoc = new jsx3.xml.Document();
    capacityManagementNodeDoc.loadXML(capacityManagement);
    var capacityManagementNode = capacityManagementNodeDoc.selectSingleNode("/*").cloneNode(true);

    var reqSetResourceObject = new ReqSessionGetRequest("SetDirStorageConfig",
                                                        { action: "FNCCALL", context: this,
                                                          fnc: function (respCode, respVal) {
                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                              dlgEditMediaSDR_Destroy()
                                                            }
                                                            else {
                                                              try {
                                                                var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                              } catch (e) {
                                                              }
                                                              var ErrorMsg = "Error: SetDirStorageConfig request failed \n";
                                                              ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                              ErrorMsg += "Error code: " + respCode;
                                                              alert(ErrorMsg);
                                                            }
                                                          }
                                                        });

    reqSetResourceObject.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = MediaDirNID;
    reqParams["ObjectList_WhereType"] = whereMode;

    var requestDataObjectArr = new Object;
    requestDataObjectArr["DirSpanning"] = dirSpanningNode;
    requestDataObjectArr["CapacityManagement"] = capacityManagementNode;
    reqSetResourceObject.requestDataObjectArr = requestDataObjectArr;

    reqSetResourceObject.reqParams = reqParams;
    reqSetResourceObject.sendRequest();

  }  // end cmpDlgEditMediaSDRSetMediaSDR


  function cmpDlgEditMediaSDRValidate() {
    var diskSpanningVal = cmpBtnRadioDiskSapnning.BlkJSX.getSelected();
    var ilmVal = cmpBtnRadioIlm.BlkJSX.getSelected();
    var retVal = true;
    var focusBlkJSX = null;
    errorMsg = "";
    //txtBoxBlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
    if (diskSpanningVal) {
      var diskFullThresholdPercent = cmpTxtDSThreshold.BlkJSX.getValue();
      if (diskFullThresholdPercent.match(/[^0-9]/) || jsx3.util.strEmpty(diskFullThresholdPercent) || parseInt(diskFullThresholdPercent, 10) < 1 || parseInt(diskFullThresholdPercent, 10) > 100) {
        errorMsg += "<br>- Please enter integer value for the Threshold between 1 and 100";
        cmpTxtDSThreshold.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000\n", true);
        cmpTxtDSThreshold.BlkJSX.setSelection();
        retVal = false;
        if (jsx3.util.strEmpty(focusBlkJSX)) {
          focusBlkJSX = cmpTxtDSThreshold.BlkJSX;
        }
      }
      else {
        diskFullThresholdPercent = parseInt(diskFullThresholdPercent, 10);
        cmpTxtDSThreshold.BlkJSX.setValue(diskFullThresholdPercent);
        cmpTxtDSThreshold.BlkJSX.setBorder("", true);
      }

      var objMatrix = cmpMtxDSPriority.BlkJSX;
      var strXMLCacheId = objMatrix.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      if (objXML) {
        var objRecordNodes = objXML.selectNodes("//record");
        var mtxError = false;
        for (var it = objRecordNodes.iterator(); it.hasNext(); ) {
          var objRecordNode = it.next();
          var mediaStorePriority = objRecordNode.getAttribute("jsxPriority");
          var jsxID = objRecordNode.getAttribute("jsxid");
          if (mediaStorePriority.match(/[^0-9]/) || jsx3.util.strEmpty(mediaStorePriority) || parseInt(mediaStorePriority, 10) < 1 || parseInt(mediaStorePriority, 10) > 999) {
            if (!mtxError) {
              errorMsg += "<br>- Please enter integer value for the priority between 1 and 999";
            }
            mtxError = true;
            if (retVal) {
              objMatrix.focusCellById(jsxID, "jsxPriority");
            }
            objRecordNode.removeAttribute("jsxselected");
            objRecordNode.setAttribute("jsxstyle", "background-color:#ff0000;");
            retVal = false; 
          }
          else {
            objRecordNode.removeAttribute("jsxstyle");
            objRecordNode.setAttribute("jsxPriority", parseInt(mediaStorePriority, 10));
          }
        }
        if (mtxError) {
          objMatrix.jsxbgcolor = "";
          objMatrix.jsxxslparams.jsx_rowbg1 = ""; 
          objMatrix.jsxxslparams.jsx_rowbg2 = "";
        }
        else {
          objMatrix.jsxbgcolor = "#efefff";
          objMatrix.jsxxslparams.jsx_rowbg1 = "#ffffff";
          objMatrix.jsxxslparams.jsx_rowbg2 = "#efefff";
        }
        objMatrix.repaintData(); 
      }

    } // end if(diskSpanningVal)

    if (ilmVal) {
      var nfsTestUrlExp = /(nfs:\/\/)./;
      var chkThresholdVal = cmpChkThresholdRule.BlkJSX.getChecked();
      var chkIlmVal = cmpChkILMRule.BlkJSX.getChecked();
      if (!chkThresholdVal && !chkIlmVal) {
        errorMsg += "<br>- Please select either Threshold Rule and/or ILM Rule";
        retVal = false;
      }

      if (chkThresholdVal) {
        var thresholdNASVal = cmpBtnRadioThresholdNAS.BlkJSX.getSelected();
        var thresholddeleteVal = cmpBtnRadioThresholdDelete.BlkJSX.getSelected();
        var thresholdDiskCapacityVal = cmpTxtThresholdDiskCapacity.BlkJSX.getValue();
        var thresholdDiskReduceCapacityVal = cmpTxtThresholdDiskReduceCapacity.BlkJSX.getValue();
        var thresholdNASURLVal = "";

        if (!thresholdNASVal && !thresholddeleteVal) {
          errorMsg += "<br>- Please select either offload or delete media in 'Threshold Rule'";
          cmpBtnRadioThresholdNAS.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          cmpBtnRadioThresholdDelete.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          retVal = false;
        }

        if (thresholdDiskCapacityVal.match(/[^0-9]/) || jsx3.util.strEmpty(thresholdDiskCapacityVal) || parseInt(thresholdDiskCapacityVal, 10) < 1 || parseInt(thresholdDiskCapacityVal, 10) > 100) {
          errorMsg += "<br>- Please enter integer value for the 'disk capacity' between 1 and 100";
          cmpTxtThresholdDiskCapacity.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          cmpTxtThresholdDiskCapacity.BlkJSX.setSelection();
          retVal = false;
          if (jsx3.util.strEmpty(focusBlkJSX)) {
            focusBlkJSX = cmpTxtThresholdDiskCapacity.BlkJSX;
          }
        }
        else {
          thresholdDiskCapacityVal = parseInt(thresholdDiskCapacityVal, 10);
          cmpTxtThresholdDiskCapacity.BlkJSX.setValue(thresholdDiskCapacityVal);
          cmpTxtThresholdDiskCapacity.BlkJSX.setBorder("", true);
        }

        if (thresholdNASVal) {
          thresholdNASURLVal = cmpTxtThresholdNASURL.BlkJSX.getValue();
          if (!nfsTestUrlExp.test(thresholdNASURLVal) || jsx3.util.strEmpty(thresholdNASURLVal)) {
            errorMsg += "<br>- Please enter valid NAS URL";
            cmpTxtThresholdNASURL.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
            cmpTxtThresholdNASURL.BlkJSX.setSelection();
            retVal = false;
            if (jsx3.util.strEmpty(focusBlkJSX)) {
              focusBlkJSX = cmpTxtThresholdNASURL.BlkJSX;
            }
          }
          else {
            cmpTxtThresholdNASURL.BlkJSX.setBorder("", true);
          }
        }

        if (thresholdDiskReduceCapacityVal.match(/[^0-9]/) || jsx3.util.strEmpty(thresholdDiskReduceCapacityVal) || parseInt(thresholdDiskReduceCapacityVal, 10) < 1 || parseInt(thresholdDiskReduceCapacityVal, 10) > 100) {
          errorMsg += "<br>- Please enter integer value for the 'Reduce disk capacity' between 1 and 100";
          cmpTxtThresholdDiskReduceCapacity.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          cmpTxtThresholdDiskReduceCapacity.BlkJSX.setSelection();
          retVal = false;
          if (jsx3.util.strEmpty(focusBlkJSX)) {
            focusBlkJSX = cmpTxtThresholdDiskReduceCapacity.BlkJSX;
          }
        }
        else {
          thresholdDiskReduceCapacityVal = parseInt(thresholdDiskReduceCapacityVal, 10);
          cmpTxtThresholdDiskReduceCapacity.BlkJSX.setValue(thresholdDiskReduceCapacityVal);
          cmpTxtThresholdDiskReduceCapacity.BlkJSX.setBorder("", true);
        }

        if (parseInt(thresholdDiskReduceCapacityVal, 10) > parseInt(thresholdDiskCapacityVal, 10)) {
          errorMsg += "<br>- Please enter 'Reduce disk capacity' value less than the 'disk capacity'";
          cmpTxtThresholdDiskReduceCapacity.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          cmpTxtThresholdDiskReduceCapacity.BlkJSX.setSelection();
          retVal = false;
          if (jsx3.util.strEmpty(focusBlkJSX)) {
            focusBlkJSX = cmpTxtThresholdDiskReduceCapacity.BlkJSX;
          }
        }

        if (errorMsg != "") {
          var ilmType = topic_Get(tDlg, "ILMType");
          if (ilmType != "ThresholdRule") {
            topic_Publish(tDlg, "ILMType", "ThresholdRule");
          }
        }

      } // end if(chkThresholdVal)

      if (chkIlmVal) {
        var ilmType = "";
        if (errorMsg == "") {
          ilmType = "ILMRule";
        }
        var ilmNASURLVal = "";
        var ilmNASVal = cmpBtnRadioILMRuleNAS.BlkJSX.getSelected();
        var ilmdeleteVal = cmpBtnRadioILMRuleDelete.BlkJSX.getSelected();
        if (!ilmNASVal && !ilmdeleteVal) {
          errorMsg = "<br>- Please select either offload or delete media in 'ILM Rule'";
          cmpBtnRadioILMRuleNAS.BlkJSX.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          cmpBtnRadioILMRuleDelete.BlkJSX.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
          retVal = false;
        }

        if (ilmNASVal) {
          ilmNASURLVal = cmpTxtILMRuleNASURL.BlkJSX.getValue();
          if (!nfsTestUrlExp.test(ilmNASURLVal) || jsx3.util.strEmpty(ilmNASURLVal)) {
            errorMsg += "<br>- Please enter valid NAS URL";
            cmpTxtILMRuleNASURL.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
            cmpTxtILMRuleNASURL.BlkJSX.setSelection();
            retVal = false;
            if (jsx3.util.strEmpty(focusBlkJSX)) {
              focusBlkJSX = cmpTxtILMRuleNASURL.BlkJSX;
            }
          }
          else {
            cmpTxtILMRuleNASURL.BlkJSX.setBorder("", true);
          }
        }

        if (errorMsg != "" && ilmType != "") {
          topic_Publish(tDlg, "ILMType", "ILMRule");
        }

      } // end if(chkIlmVal)

      var scheduleValOk = true;
      var scheduleVal = cmpTxtSchedule.BlkJSX.getValue();
      if (jsx3.util.strEmpty(scheduleVal) || scheduleVal.match(/[^0-9,]/)) {
        errorMsg += "<br>- Schedule Timing should be from 0 to 23";
        cmpTxtSchedule.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
        cmpTxtSchedule.BlkJSX.setSelection();
        retVal = false;
        if (jsx3.util.strEmpty(focusBlkJSX)) {
          focusBlkJSX = cmpTxtSchedule.BlkJSX;
        }
      }
      else {
        var scheduleValuesArr = scheduleVal.split(",");
        for (var i = 0; i < scheduleValuesArr.length; i++) {
          var scheduleValue = scheduleValuesArr[i];
          if (isNaN(scheduleValue) || scheduleValue < 0 || scheduleValue >= 24) {
            errorMsg += "<br>- Schedule Timing should be from 0 to 23";
            cmpTxtSchedule.BlkJSX.setBorder("solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000;solid 1px #ff0000", true);
            cmpTxtSchedule.BlkJSX.setSelection();
            retVal = false;
            scheduleValOk = false;
            if (jsx3.util.strEmpty(focusBlkJSX)) {
              focusBlkJSX = cmpTxtSchedule.BlkJSX;
            }
          }
        }
        if (scheduleValOk) {
          cmpTxtSchedule.BlkJSX.setBorder("", true);
        }
      }

    } // end if(ilmVal) 

    //show the error block
    cmpLblErrorMsg.BlkJSX.setText(errorMsg, true);
    if (!retVal) {
      topic_Publish(tDlg, "ShowError", "SHOW");
      if (!jsx3.util.strEmpty(focusBlkJSX)) {
        focusBlkJSX.setSelection();
      }
    }
    else {
      topic_Publish(tDlg, "ShowError", "NONE");
    }

    return retVal;
  } //end cmpDlgEditMediaSDRValidate

} // end dlgEditMediaSDRStorageConfig_Create
