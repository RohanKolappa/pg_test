// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceNetwork_Create(AssetNID, selectedAssetType, selectedAssetName, selectedAssetStatus) {

  var ManagementMode = "Local";
  var deviceTypeListNode = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/selections/DeviceType.xml");
  var ipvsClientType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ selectedAssetType +"']");
  if( !jsx3.util.strEmpty(ipvsClientType) || selectedAssetStatus == "Unmanaged") {
    var ManagementMode = "Central";
  }

  if( ManagementMode == "Local" && selectedAssetStatus != "Ready" ) {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus);
    return false; 
  }

  // deal with defaults for add
  if( AssetNID == null ) AssetNID = "DEFAULT_ASSETRESOURCE_NID";

  var DeviceNID = "";
  var DeviceResourceNIDArr = [];
  var CompleteAddDeviceArr = [];
  var deviceConfigPath = "";
  
  closeDialogIfExists("DialogSystemSettings");
  // load view - dialog component file
  var DlgFile = "components/assets/assetNetworkConfigDialog.xml";
  //var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
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
  
  topic_Set(tDlg, "EditBlkNetworkSettings", "NONE");
  topic_Set(tDlg, "EditBlkDNSSettings", "NONE");
  topic_Set(tDlg, "EditBlkNetworkRoutes", "NONE");
  topic_Set(tDlg, "EditBlkHostTable", "NONE");
  topic_Set(tDlg, "EditBlkSystemClock", "NONE");
  topic_Set(tDlg, "EditBlkNATTable", "NONE");
  topic_Set(tDlg, "EditBlkNTPServer", "NONE");
  var cmpCDFEditAssetNetworkSettings = new Component([BlkJSX,"cdfDlgAssetNetworkSettings"]);
  cmpCDFEditAssetNetworkSettings.BlkJSX.setXMLTransformers("xsl/EditAssetNetworkSettings.xsl");

  var cmpMtxInterface = new CmpBlock([BlkJSX,"mtxLanTable"]);
  cmpMtxInterface.BlkJSX.setXMLTransformers("xsl/EthernetPortTableToCDF.xsl");
  var cmpMtxRoute = new CmpBlock([BlkJSX,"mtxAssetRouteTable"]);    
  cmpMtxRoute.BlkJSX.setXMLTransformers("xsl/RouteTableToCDF.xsl");
  var cmpMtxHost = new CmpBlock([BlkJSX,"mtxHostTable"]);    
  cmpMtxHost.BlkJSX.setXMLTransformers("xsl/HostTableToCDF.xsl");
  var cmpMtxNat = new CmpBlock([BlkJSX,"mtxNatTable"]);    
  cmpMtxNat.BlkJSX.setXMLTransformers("xsl/NATZoneTableToCDF.xsl");

  var cmpBlkAssetSystemClock = new Component([BlkJSX, "blkAssetSystemSystemClock"]);
  var cmpBlkAssetSystemNTPServer = new Component([BlkJSX,"blkAssetSystemSystemNTPServer"]);
  var cmpBlkAssetNetworkSettings = new Component([BlkJSX,"blkAssetSystemNetworkSettings"]);
  var cmpBlkAssetDNSSettings = new Component([BlkJSX,"blkAssetSystemDNSSettings"]);
  
  var cmpBlkDeleteNetworkRoute = new CmpButton( [BlkJSX,"imgbtnDeleteRouteEntry"],
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var objMatrix = this.RouteTableBlkJSX;
                                          var selRecordID = objMatrix.getValue();
                                          var selRecordNode = objMatrix.getRecordNode(selRecordID);
                                          if( (selRecordID != null) && (selRecordNode != null) ) {
                                            var dirtyBitVal = selRecordNode.getAttribute("jsxDirtyBit");
                                            if( dirtyBitVal == "ADD" ) {
                                              objMatrix.deleteRecord(selRecordID, true);
                                            }
                                            else {
                                              selRecordNode.setAttribute("jsxDirtyBit","DELETE");
                                              objMatrix.redrawRecord(selRecordID,jsx3.xml.CDF.UPDATE);
                                            }
                                            this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkDeleteNetworkRoute.BlkJSX);
                                          } else if (objMatrix.getRecordIds().length > 0 && jsx3.util.strEmpty(selRecordNode)) {
                                            alert("Please select Route entry to delete");
                                            return false;
                                          }                                          
                                          
                                        }
                                      });
  cmpBlkDeleteNetworkRoute.RouteTableBlkJSX = cmpMtxRoute.BlkJSX;
  cmpBlkDeleteNetworkRoute.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
                                      
  var cmpBlkAddNetworkRoute = new CmpButton([BlkJSX,"imgbtnAddRouteEntry"],
                                            null,
                                            { action:"FNCCALL", 
                                              context:null,
                                              fnc:function() {                                          
                                                var o = new Object;
                                                o.jsxid = jsx3.xml.CDF.getKey();
                                                o.jsxDirtyBit = "ADD";
                                                var objMatrix = this.RouteTableBlkJSX;
                                                var objMatrixSortedIds = objMatrix.getSortedIds();
                                                if( objMatrixSortedIds.length == 0 ) {
                                                  objMatrix.insertRecord(o,true);
                                                }
                                                else {
                                                  objMatrix.insertRecordBefore(o, objMatrixSortedIds[0], true);
                                                }
                                                this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkAddNetworkRoute.BlkJSX);                                          
                                              }
                                            });


  cmpBlkAddNetworkRoute.RouteTableBlkJSX = cmpMtxRoute.BlkJSX;
  cmpBlkAddNetworkRoute.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
  
  var cmpBtnAssetNetworkRoutesAddRoute = new CmpContainer([BlkJSX,"blkNetworkRouteTable"],
                                                    [{cmp:cmpBlkAddNetworkRoute},
                                                     {cmp:cmpBlkDeleteNetworkRoute}
                                                    ],null
                                                    );

  var cmpBlkAssetNetworkRoutes = new CmpContainer([BlkJSX,"blkAssetSystemNetworkRoutes"],
                                                  [ {cmp:cmpBtnAssetNetworkRoutesAddRoute}
                                                  ],
                                                  null);

  var cmpBlkDeleteHost = new CmpButton( [BlkJSX,"imgbtnDeleteHostEntry"],
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var objMatrix = this.HostTableBlkJSX;
                                          var selRecordID = objMatrix.getValue();
                                          var selRecordNode = objMatrix.getRecordNode(selRecordID);
                                          if( (selRecordID != null) && (selRecordNode != null) ) {
                                            var dirtyBitVal = selRecordNode.getAttribute("jsxDirtyBit");
                                            if( dirtyBitVal == "ADD" ) {
                                              objMatrix.deleteRecord(selRecordID, true);
                                            }
                                            else {
                                              selRecordNode.setAttribute("jsxDirtyBit","DELETE");
                                              objMatrix.redrawRecord(selRecordID,jsx3.xml.CDF.UPDATE);
                                            }
                                            this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkDeleteHost.BlkJSX);
                                          } else if (objMatrix.getRecordIds().length > 0 && jsx3.util.strEmpty(selRecordNode)) {
                                            alert("Please select Host entry to delete");
                                            return false;
                                          }                                           
                                        }
                                      });
  cmpBlkDeleteHost.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
  cmpBlkDeleteHost.HostTableBlkJSX = cmpMtxHost.BlkJSX;
                                      
  var cmpBlkAddHost = new CmpButton( [BlkJSX,"imgbtnAddHostEntry"],
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {                                          
                                          var o = new Object;
                                          o.jsxid = jsx3.xml.CDF.getKey();
                                          o.jsxDirtyBit = "ADD";
                                          var objMatrix = this.HostTableBlkJSX;
                                          var objMatrixSortedIds = objMatrix.getSortedIds();
                                          if( objMatrixSortedIds.length == 0 ) {
                                            objMatrix.insertRecord(o,true);
                                          }
                                          else {
                                            objMatrix.insertRecordBefore(o, objMatrixSortedIds[0], true);
                                          }
                                          this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkAddHost.BlkJSX);
                                        }
                                      });

  cmpBlkAddHost.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
  cmpBlkAddHost.HostTableBlkJSX = cmpMtxHost.BlkJSX;

  var cmpBtnAssetHostTableAddHost = new CmpContainer([BlkJSX,"blkHostTable"],
                                            [{cmp:cmpBlkAddHost},
                                             {cmp:cmpBlkDeleteHost}
                                            ],null
                                            );
  var cmpBlkAssetHostTable = new CmpContainer([BlkJSX,"blkAssetSystemNetworkHost"],
                                                  [ {cmp:cmpBtnAssetHostTableAddHost}
                                                  ],
                                                  null);

  var cmpBlkDeleteNat = new CmpButton( [BlkJSX,"imgbtnDeleteNATEntry"],
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var objMatrix = this.NatTableBlkJSX;
                                          var selRecordID = objMatrix.getValue();
                                          var selRecordNode = objMatrix.getRecordNode(selRecordID);
                                          if( (selRecordID != null) && (selRecordNode != null) ) {
                                            var dirtyBitVal = selRecordNode.getAttribute("jsxDirtyBit");
                                            if( dirtyBitVal == "ADD" ) {
                                              objMatrix.deleteRecord(selRecordID, true);
                                            }
                                            else {
                                              selRecordNode.setAttribute("jsxDirtyBit","DELETE");
                                              objMatrix.redrawRecord(selRecordID,jsx3.xml.CDF.UPDATE);
                                            }
                                            this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkDeleteNat.BlkJSX);
                                          } else if (objMatrix.getRecordIds().length > 0 && jsx3.util.strEmpty(selRecordNode)) {
                                            alert("Please select NAT entry to delete");
                                            return false;
                                          }
                                        }
                                      });
                                      
  cmpBlkDeleteNat.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
  cmpBlkDeleteNat.NatTableBlkJSX = cmpMtxNat.BlkJSX;

  var cmpBlkAddNat = new CmpButton( [BlkJSX,"imgbtnAddNATEntry"],
                                      null,
                                      { action:"FNCCALL", 
                                        context:null,
                                        fnc:function() {
                                          var o = new Object;
                                          o.jsxid = jsx3.xml.CDF.getKey();
                                          o.jsxDirtyBit = "ADD";
                                          var objMatrix = this.NatTableBlkJSX;
                                          var objMatrixSortedIds = objMatrix.getSortedIds();
                                          if( objMatrixSortedIds.length == 0 ) {
                                            objMatrix.insertRecord(o,true);
                                          }
                                          else {
                                            objMatrix.insertRecordBefore(o, objMatrixSortedIds[0], true);
                                          }
                                          this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkAddNat.BlkJSX);
                                        }
                                      });

  cmpBlkAddNat.NatTableBlkJSX = cmpMtxNat.BlkJSX;
  cmpBlkAddNat.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
  
  
  var cmpBtnNatTableAssetAddNat = new CmpContainer([BlkJSX,"blkNatTable"],
                                            [{cmp:cmpBlkAddNat},
                                             {cmp:cmpBlkDeleteNat}
                                            ],null
                                            );
  var cmpBlkAssetNatTable = new CmpContainer([BlkJSX,"blkAssetSystemNetworkNAT"],
                                                  [ {cmp:cmpBtnNatTableAssetAddNat}
                                                  ],
                                                  null);                                                                                                    

  var cmpBlkAssetNetworkUI = new CmpContainer( [BlkJSX,"SYSTEMSETTINGS"],
                                                  [{cmp:cmpBlkAssetSystemClock,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetSystemClock'"},
                                                   {cmp:cmpBlkAssetSystemNTPServer,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetSystemNTPServer'"}, 
                                                   {cmp:cmpBlkAssetNetworkSettings,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetNetworkSettings'"},
                                                   {cmp:cmpBlkAssetDNSSettings,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetDNSSettings'"},
                                                   {cmp:cmpBlkAssetNetworkRoutes,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetNetworkRoutes'"},
                                                   {cmp:cmpBlkAssetHostTable,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetHostTable'"},
                                                   {cmp:cmpBlkAssetNatTable,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetNatTable'"}  
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}
                                                  ]
                                                );
                                                 
  var cmpLblAssetVersionInfo = new CmpLabelHighlight([BlkJSX,"lblAssetSystemVersionInfo"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetVersionInfo"} );
  var cmpLblAssetSystemClock = new CmpLabelHighlight([BlkJSX,"lblAssetSystemSystemClock"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetSystemClock"} );
  var cmpLblAssetSystemNTPServer = new CmpLabelHighlight([BlkJSX,"lblAssetSystemNTPServer"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetSystemNTPServer"} );                                                     
  var cmpLblAssetNetworkSettings = new CmpLabelHighlight([BlkJSX,"lblAssetSystemNetworkSettings"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetNetworkSettings"} );
  var cmpLblAssetDNSSettings = new CmpLabelHighlight([BlkJSX,"lblAssetSystemDNSSettings"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetDNSSettings"} ); 
  var cmpLblAssetNetworkRoutes = new CmpLabelHighlight([BlkJSX,"lblAssetSystemNetworkRoutes"], null, 
                                                       [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                       {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetNetworkRoutes"} );
  var cmpLblAssetHostTable = new CmpLabelHighlight([BlkJSX,"lblAssetSystemHostTable"], null, 
                                                       [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                       {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetHostTable"} );
  var cmpLblAssetNatTable = new CmpLabelHighlight([BlkJSX,"lblAssetSystemNatTable"], null, 
                                                       [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                       {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetNatTable"} );
                                                                                                                
  var cmpBlkAssetSystemLabel = new CmpContainer( [BlkJSX,"MENU"],
                                                  [ {cmp:cmpLblAssetSystemClock},
                                                    {cmp:cmpLblAssetSystemNTPServer},
                                                    {cmp:cmpLblAssetNetworkSettings},
                                                    {cmp:cmpLblAssetDNSSettings},
                                                    {cmp:cmpLblAssetNetworkRoutes},
                                                    {cmp:cmpLblAssetHostTable},
                                                    {cmp:cmpLblAssetNatTable}
                                                  ],
                                                  null
                                                );

  var cmpBlkLblAssetSystemDNSSettings = new Component([BlkJSX,"blkLblAssetSystemDNSSettings"]);
  var cmpBlkLblAssetSystemNetworkSettings = new Component([BlkJSX,"blkLblAssetSystemNetworkSettings"]);
  var cmpBlkLblAssetSystemNetworkRoutes = new Component([BlkJSX,"blkLblAssetSystemNetworkRoutes"]);
  var cmpBlkLblAssetSystemHostTable = new Component([BlkJSX,"blkLblAssetSystemHostTable"]);
  var cmpBlkLblAssetSystemNatTable = new Component([BlkJSX,"blkLblAssetSystemNatTable"]);
  var cmpBlkLblAssetSystemSystemClock = new Component([BlkJSX,"blkLblAssetSystemSystemClock"]);
  var cmpBlkLblAssetNTPServer = new Component([BlkJSX,"blkLblAssetSystemNTPServer"]);
  
  var cmpBlkAssetSystemLabelBlk = new CmpContainer( [BlkJSX,"MENU"],
                                                    [{cmp:cmpBlkLblAssetNTPServer,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkNTPServer') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetSystemDNSSettings,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditBlkDNSSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetSystemNetworkSettings,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditBlkNetworkSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetSystemNetworkRoutes,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditBlkNetworkRoutes') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetSystemHostTable,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditBlkHostTable') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetSystemSystemClock,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditBlkSystemClock') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetSystemNatTable,
                                                       sel:"topic_Get(\"" + tDlg + "\", 'EditBlkNATTable') != 'NONE'"}
                                                    ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkNTPServer", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkDNSSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkNetworkSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkNetworkRoutes", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkHostTable", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkSystemClock", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkNATTable", eventval:null}
                                                    ]
                                                  ); 
 var cmpChkEnableNTPServer = new CmpCheckBox([BlkJSX, "chkEnableNTPServer"],
                                                    null,
                                                    { action: "FNCCALL",
                                                      context: null,
                                                      fnc: function() {
                                                            if (this.BlkJSX.getChecked()) {
                                                              cmpChkEnableNTPServer.BlkJSX.setChecked("1");
                                                            }
                                                            else {
                                                              cmpChkEnableNTPServer.BlkJSX.setChecked("0");
                                                            }
                                                      }
                                                    });

var cmpChkEnableNTPClient = new CmpCheckBox([BlkJSX, "chkEnableNTPClient"],
                                                    null,
                                                    { action: "FNCCALL",
                                                      context: null,
                                                      fnc: function() {
                                                            if (this.BlkJSX.getChecked()) {
                                                              cmpChkEnableNTPClient.BlkJSX.setChecked("1");
                                                              cmptxtNTPServerIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                            }
                                                            else {
                                                              cmpChkEnableNTPClient.BlkJSX.setChecked("0");
                                                              cmptxtNTPServerIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                            }
                                                      }
                                                    });

  var cmptxtNTPServerServerIP = new CmpTxtBox([BlkJSX,"txtNTPServerServerIP"], null, null, null, null);  

  var cmptxtNTPServerIP = new CmpTxtBox([BlkJSX,"txtNTPServerIP"], null, null, null, null);  

  var cmpBlkEditAssetNetwork = new CmpContainer( [BlkJSX,"blkAssetSystemSettings"],
                                            [{cmp:cmpBlkAssetSystemLabelBlk},
                                             {cmp:cmpBlkAssetSystemLabel},
                                             {cmp:cmpBlkAssetNetworkUI},
                                             {cmp:cmpMtxRoute},
                                             {cmp:cmpMtxHost},
                                             {cmp:cmpChkEnableNTPServer},
                                             {cmp:cmpChkEnableNTPClient},
                                             {cmp:cmptxtNTPServerServerIP},
                                             {cmp:cmptxtNTPServerIP},
                                             {cmp:cmpMtxNat},
                                             {cmp:cmpMtxInterface}
                                            ],
                                            null
                                          );                                           

  // event published from func ( cmpReq_GetCompleteDeviceWithReadQuery ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteDeviceDone", AssetNID, this,
                      function() {
                        cmpDlgEditAssetGetAsset(AssetNID);
                      }
                    ); // subscribe end    
      
  var cmpBtnEditAssetClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAddAssetSystem"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
        
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetSystem"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditAssetSetAssetStateSystem", "DONE", this,
                                                                function() {
                                                                  dlgEditAsset_Destroy();
                                                                });
                                              cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                            }
                                           });
                                                                                         
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"DialogSystemSettings"],
                                          [ {cmp:cmpBtnEditAssetOK},
                                            {cmp:cmpBtnEditAssetCancel},
                                            {cmp:cmpBtnEditAssetClose},
                                            {cmp:cmpBlkEditAssetNetwork},
                                            {cmp:cmpCDFEditAssetNetworkSettings}
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

  // function to set dirtybit change function on the textboxes, select boxes
  // check boxes. Any value change witkk set the dirty bit true

  function cmpDlgEditAssetSetDirtyBitChangeHandler() {
 
    var objParent = cmpBlkEditAssetNetwork.BlkJSX.getParent();
    
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
      
      DeviceNID = deviceNode.getAttribute("NID");

      var EthernetPortTableNode = deviceNode.selectSingleNode(".//EthernetPortTableConfig").cloneNode(true); 
      var strXMLCacheId = cmpMtxInterface.BlkJSX.getXMLId();
      var objXMLEthernet = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXMLEthernet.loadXML(EthernetPortTableNode.toString());
      cmpMtxInterface.BlkJSX.setSourceXML(objXMLEthernet);        
      cmpMtxInterface.BlkJSX.repaint();

      var NATZoneTableNode = deviceNode.selectSingleNode(".//NATZoneTableConfig").cloneNode(true); 
      var strXMLCacheId = cmpMtxNat.BlkJSX.getXMLId();
      var objXMLNAT = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXMLNAT.loadXML(NATZoneTableNode.toString());
      cmpMtxNat.BlkJSX.setSourceXML(objXMLNAT);        
      cmpMtxNat.BlkJSX.repaint();

      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//DNSClientConfig"))){
        topic_Publish(tDlg, "EditBlkDNSSettings", deviceNode.selectSingleNode(".//DNSClientConfig"));
      }

      if(!jsx3.util.strEmpty(EthernetPortTableNode)) {
        topic_Publish(tDlg, "EditBlkNetworkSettings", EthernetPortTableNode);
      }

      if (!jsx3.util.strEmpty(NATZoneTableNode)) {
          topic_Publish(tDlg, "EditBlkNATTable", NATZoneTableNode);
      }      
      
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//RouteTableConfig"))){
        var RouteTableNode = deviceNode.selectSingleNode(".//RouteTableConfig").cloneNode(true); 
        topic_Publish(tDlg, "EditBlkNetworkRoutes", RouteTableNode);
        var strXMLCacheId = cmpMtxRoute.BlkJSX.getXMLId();
        var objXMLRoute = PNameSpace.getCache().getDocument(strXMLCacheId);
        objXMLRoute.loadXML(RouteTableNode.toString());
        cmpMtxRoute.BlkJSX.setSourceXML(objXMLRoute);        
        cmpMtxRoute.BlkJSX.repaint();

        var HostTableNode = deviceNode.selectSingleNode(".//HostTableConfig").cloneNode(true); 
        topic_Publish(tDlg, "EditBlkHostTable", HostTableNode);
        var strXMLCacheId = cmpMtxHost.BlkJSX.getXMLId();
        var objXMLHost = PNameSpace.getCache().getDocument(strXMLCacheId);
        objXMLHost.loadXML(HostTableNode.toString());
        cmpMtxHost.BlkJSX.setSourceXML(objXMLHost);        
        cmpMtxHost.BlkJSX.repaint();

        var SystemClockConfigNode = deviceNode.selectSingleNode(".//SystemElementsConfig/SystemClockConfig");
        if(!jsx3.util.strEmpty(SystemClockConfigNode)) {
          topic_Publish(tDlg, "EditBlkSystemClock", SystemClockConfigNode);
          var EnableNTPClient = SystemClockConfigNode.selectSingleNode(".//EnableNTPClient").getValue();
          var NTPServerIP = SystemClockConfigNode.selectSingleNode(".//NTPServerIP").getValue();
          cmptxtNTPServerIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
          if( EnableNTPClient == "true" ) {
            cmptxtNTPServerIP.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
            cmpChkEnableNTPClient.BlkJSX.setChecked("1");
          }
          else {
            cmpChkEnableNTPClient.BlkJSX.setChecked("0");
          }
          cmptxtNTPServerIP.BlkJSX.setValue(NTPServerIP);
        }

        var NTPServiceConfigNode = deviceNode.selectSingleNode(".//ServiceElementsConfig/NTPServiceConfig");
        if(!jsx3.util.strEmpty(NTPServiceConfigNode)) {
          topic_Publish(tDlg, "EditBlkNTPServer", NTPServiceConfigNode);
          var EnableNTPServer = NTPServiceConfigNode.selectSingleNode(".//EnableNTPServer").getValue();
          var NTPServiceServerIP = NTPServiceConfigNode.selectSingleNode(".//NTPServerIP").getValue();
          if( EnableNTPServer == "true" ) {
            cmpChkEnableNTPServer.BlkJSX.setChecked("1");
          }
          else {
            cmpChkEnableNTPServer.BlkJSX.setChecked("0");
          }
          cmptxtNTPServerServerIP.BlkJSX.setValue(NTPServiceServerIP);
        }
      }

      // set the Device Config data
      var strXMLCacheId = cmpCDFEditAssetNetworkSettings.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(deviceNode.toString());
      cmpCDFEditAssetNetworkSettings.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditAssetNetworkSettings.BlkJSX.setCDFId(DeviceNID);

      // finish get data
      topic_Publish(tDlg, "EditAssetDataState", AssetNID);
      
      // attach the function to the toggle event, this will set the dirtybit 
      var chkInterfaceEnableBlkJSX = cmpMtxInterface.BlkJSX.getDescendantOfName("chkjsxLanIsEnable");
      chkInterfaceEnableBlkJSX.setEvent('eval("UpdateDirtyBit(this);")', jsx3.gui.Interactive.TOGGLE);
      // attach the function to the toggle event, this will set the dirtybit 
      var chkDhcpBlkJSX = cmpMtxInterface.BlkJSX.getDescendantOfName("chkjsxLanDhcp");
      chkDhcpBlkJSX.setEvent('eval("UpdateDirtyBit(this);")', jsx3.gui.Interactive.TOGGLE);

      var chkMulticastBlkJSX = cmpMtxInterface.BlkJSX.getDescendantOfName("chkjsxLanMulticast");
      chkMulticastBlkJSX.setEvent('eval("UpdateDirtyBit(this);")', jsx3.gui.Interactive.TOGGLE);

      // attach the function to the toggle event and check if any other interface is set to primary, 
      // this will set the dirtybit       
      var chkIsPrimaryBlkJSX = cmpMtxInterface.BlkJSX.getDescendantOfName("chkjsxLanIsPrimary");
      chkIsPrimaryBlkJSX.setEvent('eval("CheckIsPrimary(this);")', jsx3.gui.Interactive.TOGGLE);
      
      var txtJsxAssetRouteIPAddress = cmpMtxRoute.BlkJSX.getDescendantOfName("txtjsxAssetRouteIPAddress");
      txtJsxAssetRouteIPAddress.setEvent('eval("RouteTableUpdate(this);")', jsx3.gui.Interactive.JSXFOCUS);
      var txtjsxAssetRouteNetmask = cmpMtxRoute.BlkJSX.getDescendantOfName("txtjsxAssetRouteNetmask");
      txtjsxAssetRouteNetmask.setEvent('eval("RouteTableUpdate(this);")', jsx3.gui.Interactive.JSXFOCUS);
      var txtjsxAssetRouteGateway = cmpMtxRoute.BlkJSX.getDescendantOfName("txtjsxAssetRouteGateway");
      txtjsxAssetRouteGateway.setEvent('eval("RouteTableUpdate(this);")', jsx3.gui.Interactive.JSXFOCUS);

      RouteTableUpdate = function (obj) {
        var objRecordId = obj.emGetSession().recordId;
        var ObjRecord = obj.emGetSession().matrix.getRecordNode(objRecordId);
        var jsxDirtyValue = ObjRecord.getAttribute("jsxDirtyBit");
        var jsxIPAddressValue = ObjRecord.getAttribute("jsxAssetRouteIPAddress");
        var jsxNetmask = ObjRecord.getAttribute("jsxAssetRouteNetmask");
        var jsxGateway = ObjRecord.getAttribute("jsxAssetRouteGateway");
        if (jsxDirtyValue == "CLEAN") {
          alert("Route modify not supported");
          obj.emGetSession().matrix.repaint();
          return false;
        }
      }

      UpdateDirtyBit = function(obj) {
        var objRecordId = obj.emGetSession().recordId;
        var ObjRecord = obj.emGetSession().matrix.getRecordNode(objRecordId);
        ObjRecord.setAttribute("jsxDirtyBit", "UPDATELIST");
        obj.emGetSession().matrix.redrawRecord(objRecordId, jsx3.xml.CDF.UPDATE);
        PNameSpace.getJSXByName("txtDirtyBitblkLanTable").setValue("MODIFY");
      }
      
      CheckIsPrimary = function(obj) {
        var objCheckedState = obj.jsxchecked;
        var objRecordId = obj.emGetSession().recordId;
        var objMatrix  = obj.emGetSession().matrix;
        var objRecordIds = objMatrix.getRecordIds();
        var ObjRecord = objMatrix.getRecordNode(objRecordId);
        if(objRecordIds.length > 1) {
          var objXML = PNameSpace.getCache().getDocument(objMatrix.getXMLId());
          var primaryNodes = objXML.selectNodes(".//record[@jsxLanIsPrimary='1']");
          if(primaryNodes.size() == 2){
            alert("Only one(1) interface can be set as Primary");
            ObjRecord.setAttribute("jsxLanIsPrimary", "0");
            objMatrix.redrawRecord(objRecordId, jsx3.xml.CDF.UPDATE);
            return false;
          }
        }
        ObjRecord.setAttribute("jsxDirtyBit", "UPDATELIST");
        objMatrix.redrawRecord(objRecordId, jsx3.xml.CDF.UPDATE);
        PNameSpace.getJSXByName("txtDirtyBitblkLanTable").setValue("MODIFY");
      }      
      
                            
    }
    //to hightlight 1st label in the system
    if(jsx3.util.strEmpty(deviceNode.selectSingleNode(".//DNSClientConfig"))){
      topic_Publish(tDlg , "EditAssetSystemSystemConfigSelection", "AssetNetworkSettings");
    }
    else {
      topic_Publish(tDlg , "EditAssetSystemSystemConfigSelection", "AssetDNSSettings");
    }
  }// end get dlg data function
      


  //Basic Validation  
  function cmpDlgEditAssetSetAssetValidate() {
    var interfaceXML = PNameSpace.getCache().getDocument(cmpMtxInterface.BlkJSX.getXMLId());
    var primaryNodes = interfaceXML.selectNodes(".//record[@jsxLanIsPrimary='1']");  
    if(primaryNodes.size() > 1) {
      alert("Only one(1) interface can be set as Primary");
      return false;
    }
    if( cmpChkEnableNTPClient.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ) {
      var ntpClientValue = cmptxtNTPServerIP.BlkJSX.getValue();
      if(jsx3.util.strEmpty(ntpClientValue)){
        alert("Please enter NTP Server IP Address");
        cmptxtNTPServerIP.BlkJSX.focus();
        return false;
      }
      var ntpServerIPAddressValidation = verifyIPAddress(ntpClientValue);
      if(!ntpServerIPAddressValidation){
        alert("IP Address: " + ntpClientValue + " is not a valid IP address.");
        cmptxtNTPServerIP.BlkJSX.focus();
        return false;
      }
    }
    // do the MTU validation min/max 100/9000 as defined in the feature # 34801
    var recordIter = interfaceXML.selectNodeIterator(".//record");
    var mtuAlertMsg = "MTU value can be any number in between 100-9000";
    while( recordIter.hasNext() ) {
      var recordNode = recordIter.next();
      var mtuVal = recordNode.getAttribute("jsxLanMtu");
      //check for number
      if(mtuVal.match(/[^0-9]/)){
        alert(mtuAlertMsg);
        return false;
      }
      else if(mtuVal < 100 || mtuVal > 9000){
        alert(mtuAlertMsg);
        return false;      
      }
    }                
    
    
    return true;
  }
      
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetSetAsset(AssetNID) {
  
    if( cmpDlgEditAssetSetAssetValidate() != true ) return;
    
    var deviceNode = getDialogDataNode(tDlg, "Device");
    var SelectedAssetAgentJID = deviceNode.getAttribute("ownerUserJID");
    
    var AssetType = selectedAssetType;
    var deviceSystemNode = null;
    // write the data back to the CDF
    cmpCDFEditAssetNetworkSettings.BlkJSX.write();

    //get the device System data
    var strXMLCacheId = cmpCDFEditAssetNetworkSettings.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetNetworkSettingsRequest.xsl","EditAssetNetworkSettingsRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var deviceSystemXML = xslDoc.transform(objXML);
    deviceSystemNode = new jsx3.xml.Document();
    deviceSystemNode.loadXML(deviceSystemXML);
    
    //get the ethernet data
    var strXMLCacheId = cmpMtxInterface.BlkJSX.getXMLId();
    var objXMLEthernetPort = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToEthernetPortTable.xsl","EthernetPortNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newEthernetPortXML = xslDoc.transform(objXMLEthernetPort);        
    var newEthernetPortNode = new jsx3.xml.Document();
    newEthernetPortNode.loadXML(newEthernetPortXML);
    
    var oldEthernetPortNode = deviceSystemNode.selectSingleNode(".//EthernetPortTableConfig");              
    var EthernetPortParentNode = deviceSystemNode.selectSingleNode(".//EthernetPortTableConfig").getParent();
    var EthernetPortTableNodeVal = oldEthernetPortNode.getAttribute("Dirty");
    newEthernetPortNode.setAttribute("Dirty", EthernetPortTableNodeVal);
    EthernetPortParentNode.replaceNode(newEthernetPortNode,oldEthernetPortNode);

    //get the route data
    var strXMLCacheId = cmpMtxRoute.BlkJSX.getXMLId();
    var objXMLRoute = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToRouteTable.xsl","RouteNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newRouteXML = xslDoc.transform(objXMLRoute);        
    var newRouteNode = new jsx3.xml.Document();
    newRouteNode.loadXML(newRouteXML);
    
    var oldRouteNode = deviceSystemNode.selectSingleNode(".//RouteTableConfig");              
    var RouteParentNode = deviceSystemNode.selectSingleNode(".//RouteTableConfig").getParent();
    var RouteTableNodeVal = oldRouteNode.getAttribute("Dirty");
    newRouteNode.setAttribute("Dirty", RouteTableNodeVal);
    RouteParentNode.replaceNode(newRouteNode,oldRouteNode);

    //get the host data
    var strXMLCacheId = cmpMtxHost.BlkJSX.getXMLId();
    var objXMLHost = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToHostTable.xsl","HostNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newHostXML = xslDoc.transform(objXMLHost);        
    var newHostNode = new jsx3.xml.Document();
    newHostNode.loadXML(newHostXML);
    
    var oldHostNode = deviceSystemNode.selectSingleNode(".//HostTableConfig");              
    var HostParentNode = deviceSystemNode.selectSingleNode(".//HostTableConfig").getParent();
    var HostTableNodeVal = oldHostNode.getAttribute("Dirty");
    newHostNode.setAttribute("Dirty", HostTableNodeVal);
    HostParentNode.replaceNode(newHostNode,oldHostNode);

    //get the NATZone data
    var strXMLCacheId = cmpMtxNat.BlkJSX.getXMLId();
    var objXMLNATZone = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToNATZoneTable.xsl","NATZoneNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newNATZoneXML = xslDoc.transform(objXMLNATZone);        
    var newNATZoneNode = new jsx3.xml.Document();
    newNATZoneNode.loadXML(newNATZoneXML);
    
    var oldNATZoneNode = deviceSystemNode.selectSingleNode(".//NATZoneTableConfig");              
    var NATZoneParentNode = deviceSystemNode.selectSingleNode(".//NATZoneTableConfig").getParent();
    var NATZoneTableNodeVal = oldNATZoneNode.getAttribute("Dirty");
    newNATZoneNode.setAttribute("Dirty", NATZoneTableNodeVal);
    NATZoneParentNode.replaceNode(newNATZoneNode,oldNATZoneNode);

    if( deviceSystemNode == null ) return;
    
    // check if anything got chnaged
    var modifyNode = deviceSystemNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
      return;    
    }

    deleteEmptyTableConfig(deviceSystemNode);

    //Check whether the settings panel is displayed
    //if not remove that node from req string for Device Command/ Device Edit
    var networkElementsConfigNode = deviceNode.selectSingleNode(".//NetworkElementsConfig").cloneNode(true);
    networkElementsConfigNode.setAttribute("Dirty", "MODIFY");
    var NetworkElementsConfigMode = "CLEAN";

    var objBlkAssetDNSSettings = cmpBlkLblAssetSystemDNSSettings.BlkJSX;
    var DNSClientConfigNode = deviceSystemNode.selectSingleNode("//DNSClientConfig");
    if( objBlkAssetDNSSettings.getDisplay() == null || objBlkAssetDNSSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(DNSClientConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = DNSClientConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgDNSClientConfigNode = networkElementsConfigNode.selectSingleNode("//DNSClientConfig");
          DNSClientConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = networkElementsConfigNode.selectSingleNode(".//DNSClientConfig").getParent();
          parentNode.replaceNode(DNSClientConfigNode, orgDNSClientConfigNode);        
          NetworkElementsConfigMode = "MODIFY";
        }
      }
    }

    var objBlkAssetEthernetSettings = cmpBlkLblAssetSystemNetworkSettings.BlkJSX;
    var EthernetPortTableConfigNode = deviceSystemNode.selectSingleNode("//EthernetPortTableConfig");
    if( objBlkAssetEthernetSettings.getDisplay() == null || objBlkAssetEthernetSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(EthernetPortTableConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = EthernetPortTableConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          EthernetPortTableConfigNode.setAttribute("Dirty", "CLEAN");
          var EthernetPortConfigNodeIter = EthernetPortTableConfigNode.selectNodeIterator(".//EthernetPortConfig");
          while( EthernetPortConfigNodeIter.hasNext() ) {
            var EthernetPortConfigNode = EthernetPortConfigNodeIter.next();
            var EthernetPortConfigDirtyNodeVal = EthernetPortConfigNode.getAttribute("Dirty");
            if( EthernetPortConfigDirtyNodeVal == "DELETE" ) {
              EthernetPortTableConfigNode.removeChild(EthernetPortConfigNode);
            }
            else {
              EthernetPortConfigNode.setAttribute("Dirty", "CLEAN");
            }
          }
          var orgEthernetPortTableConfigNode = networkElementsConfigNode.selectSingleNode("//EthernetPortTableConfig");
          var parentNode = networkElementsConfigNode.selectSingleNode(".//EthernetPortTableConfig").getParent();
          parentNode.replaceNode(EthernetPortTableConfigNode, orgEthernetPortTableConfigNode);        
          NetworkElementsConfigMode = "MODIFY";
        }
      }
    }

    var objBlkAssetNATZoneSettings = cmpBlkLblAssetSystemNatTable.BlkJSX;
    var NATZoneTableConfigNode = deviceSystemNode.selectSingleNode("//NATZoneTableConfig");
    if( objBlkAssetNATZoneSettings.getDisplay() == null || objBlkAssetNATZoneSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(NATZoneTableConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = NATZoneTableConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          NATZoneTableConfigNode.setAttribute("Dirty", "CLEAN");
          var NATEntryNodeIter = NATZoneTableConfigNode.selectNodeIterator(".//NATZoneEntry");
          while( NATEntryNodeIter.hasNext() ) {
            var NATEntryNode = NATEntryNodeIter.next();
            var NATEntryDirtyNodeVal = NATEntryNode.getAttribute("Dirty");
            if( NATEntryDirtyNodeVal == "DELETE" ) {
              NATZoneTableConfigNode.removeChild(NATEntryNode);
            }
            else {
              NATEntryNode.setAttribute("Dirty", "CLEAN");
            }
          }
          var orgNATZoneTableConfigNode = networkElementsConfigNode.selectSingleNode("//NATZoneTableConfig");
          var parentNode = networkElementsConfigNode.selectSingleNode(".//NATZoneTableConfig").getParent();
          parentNode.replaceNode(NATZoneTableConfigNode, orgNATZoneTableConfigNode);        
          NetworkElementsConfigMode = "MODIFY";
        }
      }
    }

    var objBlkAssetRouteSettings = cmpBlkLblAssetSystemNetworkRoutes.BlkJSX;
    var RouteTableConfigNode = deviceSystemNode.selectSingleNode("//RouteTableConfig");
    if( objBlkAssetRouteSettings.getDisplay() == null || objBlkAssetRouteSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(RouteTableConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = RouteTableConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          RouteTableConfigNode.setAttribute("Dirty", "CLEAN");
          var RouteEntryNodeIter = RouteTableConfigNode.selectNodeIterator(".//RouteEntry");
          while( RouteEntryNodeIter.hasNext() ) {
            var RouteEntryNode = RouteEntryNodeIter.next();
            var RouteEntryDirtyNodeVal = RouteEntryNode.getAttribute("Dirty");
            if( RouteEntryDirtyNodeVal == "DELETE" ) {
              RouteTableConfigNode.removeChild(RouteEntryNode);
            }
            else {
              RouteEntryNode.setAttribute("Dirty", "CLEAN");
            }
          }
          var orgRouteTableConfigNode = networkElementsConfigNode.selectSingleNode("//RouteTableConfig");
          var parentNode = networkElementsConfigNode.selectSingleNode(".//RouteTableConfig").getParent();
          parentNode.replaceNode(RouteTableConfigNode, orgRouteTableConfigNode);        
          NetworkElementsConfigMode = "MODIFY";
        }
      }
    }

    var objBlkAssetHostSettings = cmpBlkLblAssetSystemHostTable.BlkJSX;
    var HostTableConfigNode = deviceSystemNode.selectSingleNode("//HostTableConfig");
    if( objBlkAssetHostSettings.getDisplay() == null || objBlkAssetHostSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(HostTableConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = HostTableConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          HostTableConfigNode.setAttribute("Dirty", "CLEAN");
          var HostEntryNodeIter = HostTableConfigNode.selectNodeIterator(".//HostEntry");
          while( HostEntryNodeIter.hasNext() ) {
            var HostEntryNode = HostEntryNodeIter.next();
            var HostEntryDirtyNodeVal = HostEntryNode.getAttribute("Dirty");
            if( HostEntryDirtyNodeVal == "DELETE" ) {
              HostTableConfigNode.removeChild(HostEntryNode);
            }
            else {
              HostEntryNode.setAttribute("Dirty", "CLEAN");
            }
          }
          var orgHostTableConfigNode = networkElementsConfigNode.selectSingleNode("//HostTableConfig");
          var parentNode = networkElementsConfigNode.selectSingleNode(".//HostTableConfig").getParent();
          parentNode.replaceNode(HostTableConfigNode, orgHostTableConfigNode);        
          NetworkElementsConfigMode = "MODIFY";
        }
      }
    }

    var objBlkAssetNTPClientSettings = cmpBlkLblAssetSystemSystemClock.BlkJSX;
    var NTPClientConfigNode = deviceSystemNode.selectSingleNode("//SystemClockConfig");
    if( objBlkAssetNTPClientSettings.getDisplay() == null || objBlkAssetNTPClientSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(NTPClientConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        deviceConfigPath = "//SystemClockConfig";
        /*var DirtyVal = NTPClientConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          NTPClientConfigNode.setAttribute("Dirty", "CLEAN");
          if(AssetType == "SessionClient-Device"){
            AssetType = "IPVSClient-Device";
          }          
          var reqSetDeviceSysElementsObject = new ReqSetObject( "Replace", AssetType, NTPClientConfigNode, 
                                                                {action:"FNCCALL", context:this, 
                                                                   fnc:function(respCode, respVal) {
                                                                         if(respCode == "SUCCESS" && respVal != null) {
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
          var reqParams = new Object;
          reqParams["Op_Where_WhereXPath"] = "//"+AssetType+"[AssetResourceNID='" + AssetNID + "']";
          reqParams["Op_In_ChildXPath"] = "DeviceConfig/SystemElementsConfig/SystemClockConfig";
          reqSetDeviceSysElementsObject.reqParams = reqParams;
          reqSetDeviceSysElementsObject.sendRequest();          
        }*/
      }
    }

    var objBlkAssetNTPServerSettings = cmpBlkLblAssetNTPServer.BlkJSX;
    var NTPServerConfigNode = deviceSystemNode.selectSingleNode("//NTPServiceConfig");
    if( objBlkAssetNTPServerSettings.getDisplay() == null || objBlkAssetNTPServerSettings.getDisplay() == "none" ) {
      deviceSystemNode.removeChild(NTPServerConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        deviceConfigPath = "//NTPServiceConfig";
        /*var DirtyVal = NTPServerConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          NTPServerConfigNode.setAttribute("Dirty", "CLEAN");
          if(AssetType == "SessionClient-Device"){
            AssetType = "IPVSClient-Device";
          }          
          var reqSetDeviceServiceElementsObject = new ReqSetObject( "Replace", AssetType, NTPServerConfigNode, 
                                                                {action:"FNCCALL", context:this, 
                                                                   fnc:function(respCode, respVal) {
                                                                         if(respCode == "SUCCESS" && respVal != null) {
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
          var reqParams = new Object;
          reqParams["Op_Where_WhereXPath"] = "//"+AssetType+"[AssetResourceNID='" + AssetNID + "']";
          reqParams["Op_In_ChildXPath"] = "DeviceConfig/ServiceElementsConfig/NTPServiceConfig";
          reqSetDeviceServiceElementsObject.reqParams = reqParams;
          reqSetDeviceServiceElementsObject.sendRequest();          
        }*/
      }
    }

    if( ManagementMode != "Local" ) {
      if( NetworkElementsConfigMode == "CLEAN" ) {
        topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
      }
      deviceConfigPath = "//NetworkElementsConfig";
      var configNode = deviceSystemNode.selectSingleNode(".//NetworkElementsConfig");   
      var parentNode = deviceSystemNode.selectSingleNode(".//NetworkElementsConfig").getParent();
      parentNode.replaceNode(networkElementsConfigNode, configNode);        
      /*if(AssetType == "SessionClient-Device"){
        AssetType = "IPVSClient-Device";
      }
      var reqSetDeviceObject = new ReqSetObject( "Replace", AssetType, networkElementsConfigNode, 
                                                 {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
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
      var reqParams = new Object;
      reqParams["Op_Where_WhereXPath"] = "//"+AssetType+"[AssetResourceNID='" + AssetNID + "']";
      reqParams["Op_In_ChildXPath"] = "DeviceConfig/NetworkElementsConfig";
      reqSetDeviceObject.reqParams = reqParams;
      reqSetDeviceObject.sendRequest();*/
    }
    //else {
    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig",
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               var resVal = respVal;
                                                               var resCode = respCode;
                                                               topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
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
    requestDataObjectArr["DeviceConfig"] = deviceSystemNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = AssetNID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = deviceConfigPath;
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
    //}    
  } // end done func
       
} // end create

// global functions
