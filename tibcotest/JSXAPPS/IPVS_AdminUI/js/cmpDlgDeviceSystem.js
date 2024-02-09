// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceSystem_Create(AssetNID, selectedAssetType, selectedAssetName, selectedAssetStatus) {

  var ManagementMode = "Local";
  if( selectedAssetStatus == "Unmanaged" ) {
    ManagementMode = "Central";
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
  
  closeDialogIfExists("DialogSystemConfig");
  // load view - dialog component file
  var DlgFile = "components/assets/assetSystemConfigDialog.xml";
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
  
  var cmpCDFEditAssetSystemSettings = new Component([BlkJSX,"cdfDlgAssetSystemConfig"]);
  cmpCDFEditAssetSystemSettings.BlkJSX.setXMLTransformers("xsl/EditAssetSystemConfig.xsl");

  var cmpBlkAssetSystemBanner = new Component([BlkJSX, "blkAssetSystemBanner"]);

  var cmpBlkAssetSystemUI = new CmpContainer( [BlkJSX,"SYSTEMSETTINGS"],
                                                  [{cmp:cmpBlkAssetSystemBanner,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetSystemSystemConfigSelection') == 'AssetSystemBanner'"}  
                                                  ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}
                                                  ]
                                                );
                                                 
  var cmpLblAssetBanner = new CmpLabelHighlight([BlkJSX,"lblAssetSystemVersionInfo"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSystemConfigSelection", eventval:"AssetSystemBanner"} );
                                                       
  var cmpBlkAssetSystemLabel = new CmpContainer( [BlkJSX,"MENU"],
                                                  [ {cmp:cmpLblAssetBanner}
                                                  ],
                                                  null
                                                );

  var cmpBlkLblAssetSystemBanner = new Component([BlkJSX,"blkLblAssetSystemLoginBanner"]);

  
  var cmpBlkAssetSystemLabelBlk = new CmpContainer( [BlkJSX,"MENU"],
                                                    [{cmp:cmpBlkLblAssetSystemBanner}
                                                    ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetSystemSelection", eventval:null}
                                                    ]
                                                  ); 

  var cmptxtAssetBanner = new CmpTxtBox([BlkJSX,"txtBannerText"], null, null, null, null);  


  var cmpBlkEditAssetSystem = new CmpContainer( [BlkJSX,"blkAssetSystemConfig"],
                                            [{cmp:cmpBlkAssetSystemLabelBlk},
                                             {cmp:cmptxtAssetBanner}
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
            
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAddAssetSystemConfig"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
        
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetSystemConfig"],
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
                                                                                         
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"DialogSystemConfig"],
                                          [ {cmp:cmpBtnEditAssetOK},
                                            {cmp:cmpBtnEditAssetCancel},
                                            {cmp:cmpBtnEditAssetClose},
                                            {cmp:cmpBlkEditAssetSystem},
                                            {cmp:cmpCDFEditAssetSystemSettings}
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
 
    var objParent = cmpBlkEditAssetSystem.BlkJSX.getParent();
    
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
    
      var SystemBannerConfigNode = deviceNode.selectSingleNode(".//SystemElementsConfig/LoginBannerConfig/Banner");
      if( SystemBannerConfigNode != null ) {
        var bannerVal = SystemBannerConfigNode.getValue();
        cmptxtAssetBanner.BlkJSX.setValue(bannerVal);
      }

      // set the Device Config data
      var strXMLCacheId = cmpCDFEditAssetSystemSettings.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(deviceNode.toString());
      cmpCDFEditAssetSystemSettings.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditAssetSystemSettings.BlkJSX.setCDFId(DeviceNID);

      // finish get data
      topic_Publish(tDlg, "EditAssetDataState", AssetNID);
                     
    }
    topic_Publish(tDlg, "EditAssetSystemSelection", AssetType);
    //to hightlight 1st label in the system
    topic_Publish(tDlg , "EditAssetSystemSystemConfigSelection", "AssetSystemBanner");
    
  }// end get dlg data function
      


  //Basic Validation  
  function cmpDlgEditAssetSetAssetValidate() {
    var bannelVal = cmptxtAssetBanner.BlkJSX.getValue();
    if(bannelVal.length >2500 ){
      alert("Login Banner string length cannot be more than 2500 characters.");
      return false;
    }              

    return true;
  }
      
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetSetAsset(AssetNID) {
  
    if( cmpDlgEditAssetSetAssetValidate() != true ) return;
    
    var deviceNode = getDialogDataNode(tDlg, "Device");
    var SelectedAssetAgentJID = deviceNode.getAttribute("ownerUserJID");
    
    var AssetType = topic_Get(tDlg, "EditAssetSystemSelection");
    var deviceSystemNode = null;
    // write the data back to the CDF
    cmpCDFEditAssetSystemSettings.BlkJSX.write();

    //get the device System data
    var strXMLCacheId = cmpCDFEditAssetSystemSettings.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetSystemConfigRequest.xsl","EditAssetSystemConfigRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var deviceSystemXML = xslDoc.transform(objXML);
    deviceSystemNode = new jsx3.xml.Document();
    deviceSystemNode.loadXML(deviceSystemXML);


    if( deviceSystemNode == null ) return;
    // check if anything got chnaged
    var modifyNode = deviceSystemNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
      return;    
    }

    if( ManagementMode != "Local" ) {
      var loginBannerConfigNode = deviceSystemNode.selectSingleNode(".//LoginBannerConfig").cloneNode(true);    
      
      var reqSetDeviceObject = new ReqSetObject( "Replace", AssetType, loginBannerConfigNode, 
                                                 {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
                                                         } else {
                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                           var ErrorMsg = "Error: Device->System set data request failed \n";
                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                           ErrorMsg += "Error code: " + respCode;
                                                           alert(ErrorMsg);
                                                         }
                                                       }
                                                 }                                                 
                                               );
      var reqParams = new Object;
      reqParams["Op_Where_WhereXPath"] = "//"+AssetType+"[AssetResourceNID='" + AssetNID + "']";
      reqParams["Op_In_ChildXPath"] = "DeviceConfig/SystemElementsConfig/LoginBannerConfig";
      reqSetDeviceObject.reqParams = reqParams;
      reqSetDeviceObject.sendRequest();
    }
    else {
    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig",
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                              if(respCode == "SUCCESS" && respVal != null) {
                                                               var resVal = respVal;
                                                               var resCode = respCode;
                                                               topic_Publish(tDlg, "EditAssetSetAssetStateSystem", "DONE");
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->System set data request failed \n";
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
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
    }    
  } // end done func
       
} // end create

// global functions
