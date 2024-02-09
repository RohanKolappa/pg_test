// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditHAConfig_Create(AssetNID, AssetType, selectedAssetName, selectedAssetStatus) {
  if( selectedAssetStatus != "Ready" ) {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
          "Device status is " + selectedAssetStatus);
   return false; 
  }

  var DeviceNID = "";
  
  closeDialogIfExists("dialogHAConfig");
  // load view - dialog component file
  var DlgFile = "components/assets/HAConfigDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().loadAndCache(DlgFile);
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  
  // setup a listner for it    
  var eventDlg = new EventListner(tDlg);
  
  function dlgEditHAConfig_Destroy() {
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgEditHAConfig.destroy();

    // destroy the local topic                  
    topic_Destroy(tDlg);
    
    // close dialog    
    BlkJSX.doClose();
  }
  
  var cmpCDFEditHAConfig = new Component([BlkJSX,"cdfDlgEditHAConfig"]);
  cmpCDFEditHAConfig.BlkJSX.setXMLTransformers("JSXAPPS/IPVS_AdminUI/xsl/EditHAConfig.xsl");

  var cmpChkEnableHA = new Component([BlkJSX, "chkHAServerEnable"]);
  var cmpRadioEnablePrimary = new Component([BlkJSX, "radioEnablePrimary"]);
  var cmpRadioEnableSecondary = new Component([BlkJSX, "radioEnableSecondary"]);
  var cmptxtPeerIPAddress = new CmpTxtBox([BlkJSX,"txtPeerIPAddress"], null, null, null, null);
  var cmptxtPeerName = new CmpTxtBox([BlkJSX,"txtPeerName"], null, null, null, null);
  var cmpChkEnableVirtualIP = new Component([BlkJSX, "chkEnableVirtualIP"]);
  var cmptxtVirtualIPAddress = new CmpTxtBox([BlkJSX, "txtVirtualIPAddress"], null, null, null, null);
  var cmptxtVirtualIPAddress2 = new CmpTxtBox([BlkJSX, "txtVirtualIPAddress2"], null, null, null, null);
  var cmpSelHAInterface = new Component([BlkJSX, "selHAInterface"]); 

  var cmpDlgHAConfig = new CmpContainer([BlkJSX,"blkHAConfig"],
                              [ {cmp:cmpChkEnableHA},
                                {cmp:cmpRadioEnablePrimary},
                                {cmp:cmpRadioEnableSecondary},
                                {cmp:cmptxtPeerIPAddress},
                                {cmp:cmptxtPeerName},
                                {cmp:cmpChkEnableVirtualIP},
                                { cmp: cmptxtVirtualIPAddress },
                                { cmp: cmptxtVirtualIPAddress2 },
                                {cmp:cmpSelHAInterface}
                              ],
                              null);

  // event published from func ( cmpReq_GetCompleteDeviceWithReadQuery ) defined in cmpReq.js                  
  eventDlg.subscribe("GetCompleteDeviceDone", AssetNID, this,
                      function() {
                        cmpDlgEditGetHAConfig(AssetNID);
                      }
                    ); // subscribe end    

  var cmpBtnHAConfigClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  dlgEditHAConfig_Destroy();
                                                }
                                               });

  var cmpBtnHAConfigCancel = new CmpButton([BlkJSX,"btnCustomizeCancel"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgEditHAConfig_Destroy();
                                              }
                                             });

  var cmpBtnHAConfigOK = new CmpButton([BlkJSX,"btnCustomizeApply"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditHAConfigSave", "DONE", this,
                                                                function() {
                                                                  dlgEditHAConfig_Destroy();
                                                                });
                                              cmpDlgEditSetHAConfig.call(this, AssetNID);
                                            }
                                           });

  var cmpDlgEditHAConfig = new CmpContainer([BlkJSX,"dialogHAConfig"],
                                          [ {cmp:cmpBtnHAConfigOK},
                                            {cmp:cmpBtnHAConfigCancel},
                                            {cmp:cmpBtnHAConfigClose},
                                            {cmp:cmpDlgHAConfig},
                                            {cmp:cmpCDFEditHAConfig}
                                          ],
                                          [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditHAConfigDataState", eventval:AssetNID}]);                            

  cmpDlgEditHAConfig.BlkJSX = BlkJSX;
  cmpDlgEditHAConfig.ObjectNID = AssetNID;
  // fnc cmpReq_GetCompleteDeviceWithReadQuery defined in cmpReq.js
  cmpDlgEditHAConfig.getBlkDevice = cmpReq_GetDeviceConfig;
  cmpDlgEditHAConfig.tDlg = tDlg;
  cmpDlgEditHAConfig.setDirtyBitChangeHandler =  cmpDlgEditHAConfigSetDirtyBitChangeHandler;

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.setDirtyBitChangeHandler();
                // this.setHAConfig_SetInit();
                 this.getBlkDevice(this.tDlg, this.ObjectNID);
                 }
               ),
               null, cmpDlgEditHAConfig );

  // function to set dirtybit flag
  function cmpDlgEditHAConfigSetDirtyBitChangeHandler() { 
    var objParent = cmpDlgHAConfig.BlkJSX.getParent();
    
    // find all descendant of type chkbox and set its toggle event
    var ChkBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.CheckBox, false);
    for(var i=0; i < ChkBoxNameArr.length; i++ ) {
      var ChkBlkJSX = ChkBoxNameArr[i];
      ChkBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.TOGGLE);
    }

    // find all descendant of type radio and set its select event
    var RadioNameArr = objParent.getDescendantsOfType(jsx3.gui.RadioButton, false);
    for(var i=0; i < RadioNameArr.length; i++ ) {
      var RadioBlkJSX = RadioNameArr[i];
      RadioBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.SELECT);
    }
      
    // find all descendant of type txtbox and set its change event
    var TxtBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.TextBox, false);
    for(var i=0; i < TxtBoxNameArr.length; i++ ) {
      var TxtBlkJSX = TxtBoxNameArr[i];
      var Name = TxtBoxNameArr[i].getName();
      var txtDirtyBitName = "txtDirtyBit" + TxtBoxNameArr[i].getParent().getName();
      if( Name != txtDirtyBitName) {
        TxtBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.CHANGE);
      }
    }

    // find all descendant of type select and set its select event
    var SelBoxNameArr = objParent.getDescendantsOfType(jsx3.gui.Select, false);
    for(var i=0; i < SelBoxNameArr.length; i++ ) {
      var SelBlkJSX = SelBoxNameArr[i];
      SelBlkJSX.setEvent("cmpDlgEditAssetSetDirtyBitValue(this);", jsx3.gui.Interactive.SELECT);
    }
  }  // end cmpDlgEditHAConfigSetDirtyBitChangeHandler


  // fnc to get dlg data
  function cmpDlgEditGetHAConfig(AssetNID) {
    var reqStatus = topic_Get(tDlg, "GetCompleteDeviceStatus");
    if(reqStatus != "SUCCESS") {
      dlgEditHAConfig_Destroy();  
    }
    else {
      var deviceNode = getDialogDataNode(tDlg, "Device");
      var AssetName = deviceNode.getAttribute("title");
      var AssetType = deviceNode.getAttribute("type"); 
      
      DeviceNID = deviceNode.getAttribute("NID");
      
      var EthernetPortIter = deviceNode.selectNodeIterator(".//EthernetPortTableConfig/EthernetPortConfig");
      while(EthernetPortIter.hasNext()) {
        var EthernetPortNode = EthernetPortIter.next();
        var PortID = EthernetPortNode.selectSingleNode(".//PortID").getValue();
        var o = new Object;
        o.jsxid = PortID;
        o.jsxtext = PortID;
        cmpSelHAInterface.BlkJSX.insertRecord(o, null, false);        
      }

      // set the Device Config data
      var strXMLCacheId = cmpCDFEditHAConfig.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(deviceNode.toString());
      cmpCDFEditHAConfig.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditHAConfig.BlkJSX.setCDFId(DeviceNID);

      // finish get data
      topic_Publish(tDlg, "EditHAConfigDataState", AssetNID);
    }
  }

  //Basic Validation  
  function cmpDlgEditHAConfigValidate() {
    if( cmpChkEnableHA.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ) {
      var peerIP = cmptxtPeerIPAddress.BlkJSX.getValue();
      var peerIPAddressValidation = verifyIPAddress(peerIP);
      if(!peerIPAddressValidation){
        alert("Peer IPAddress: " + peerIP + " is not a valid IP address.");
        return false;
      }
      var peerName = cmptxtPeerName.BlkJSX.getValue();
      if(peerName == ""){
        alert("Enter Peer Name");
        return false;
      }
    }
    if( cmpChkEnableVirtualIP.BlkJSX.getChecked() == jsx3.gui.CheckBox.CHECKED ) {
      var virtualIP = cmptxtVirtualIPAddress.BlkJSX.getValue();
      var virtualIPAddressValidation = verifyIPAddress(virtualIP);
      if(!virtualIPAddressValidation){
        alert("Virtual IP Address 1: " + virtualIP + " is not a valid IP address.");
        return false;
      }
    }
    var cmpSelHAInterfaceVal = cmpSelHAInterface.BlkJSX.getValue();
    if(jsx3.util.strEmpty(cmpSelHAInterfaceVal)){
        alert("Select HA Interface");
        return false;
    }
    return true;
  }

  function cmpDlgEditSetHAConfig(AssetNID) {
    if( cmpDlgEditHAConfigValidate() != true ) return;
    
    var deviceNode = getDialogDataNode(tDlg, "Device");
    var SelectedAssetAgentJID = deviceNode.getAttribute("ownerUserJID");
    
    var HAConfigNode = null;
    // write the data back to the CDF
    cmpCDFEditHAConfig.BlkJSX.write();

    //get the HA config data
    var strXMLCacheId = cmpCDFEditHAConfig.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("JSXAPPS/IPVS_AdminUI/xsl/EditHAConfigRequest.xsl","EditHAConfigRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var HAConfigXML = xslDoc.transform(objXML);
    HAConfigNode = new jsx3.xml.Document();
    HAConfigNode.loadXML(HAConfigXML);

    if( HAConfigNode == null ) return;
    
    // check if anything got chnaged
    var modifyNode = HAConfigNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tDlg, "EditHAConfigSave", "DONE");
      return;    
    }

    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig",
                                                     { action:"FNCCALL", context:this,
                                                       fnc:function(respCode, respVal) {
                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                               topic_Publish(tDlg, "EditHAConfigSave", "DONE");
                                                             } else {
                                                               var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                               var ErrorMsg = "Error: Device->HA Config data request failed \n";
                                                               ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                               ErrorMsg += "Error code: " + respCode;
                                                               alert(ErrorMsg);
                                                             }
                                                           }
                                                     }
                                                   );
  
    reqSetDeviceConfig.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["DeviceConfig"] = HAConfigNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = AssetNID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
  }
} // end create
