// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceStreams_Create(AssetNID, AssetType, selectedAssetName, selectedAssetStatus) {
  
  var ManagementMode = "Local";
  var deviceTypeListNode = loadXMLFile("JSXAPPS/IPVS_AdminUI/xml/selections/DeviceType.xml");
  var ipvsClientType = deviceTypeListNode.selectSingleNode(".//device[@jsxid='Client']/record[@jsxid='"+ AssetType +"']");
  if( !jsx3.util.strEmpty(ipvsClientType) || selectedAssetStatus == "Unmanaged") {
     ManagementMode = "Central";
  }

  if( ManagementMode == "Local" && selectedAssetStatus != "Ready") {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus);
    return false; 
  }

  var DeviceNID = "";
  var DeviceResourceNIDArr = [];
  var CompleteAddDeviceArr = [];
  var deviceConfigPath = "";
  
  closeDialogIfExists("DialogStreamSettings");
  // load view - dialog component file
  var DlgFile = "components/assets/assetStreamSettingsDialog.xml";
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

  topic_Set(tDlg, "EditBlkAllTxSettings", "NONE");
  topic_Set(tDlg, "EditBlkTxSettings", "NONE");
  topic_Set(tDlg, "EditBlkRxSettings", "NONE");
  topic_Set(tDlg, "EditBlkMPEGTSRxSettings", "NONE");
  topic_Set(tDlg, "EditBlkRTPRxSettings", "NONE");
  topic_Set(tDlg, "EditBlkUDPRxSettings", "NONE");

  var cdfDlgEditStreaming = new Component([BlkJSX,"cdfDlgEditStreaming"]);
  cdfDlgEditStreaming.BlkJSX.setXMLTransformers("xsl/EditAssetStreaming.xsl");

  var cmpBlkAssetStreamAll = new Component([BlkJSX,"blkAssetStreamAllTxInfo"]);

  var cmpBlkAssetStreamTx = new Component([BlkJSX,"blkAssetStreamV2DTxInfo"]);

  var cmpBlkAssetStreamRx = new Component([BlkJSX,"blkAssetStreamV2DRxInfo"]);

  var cmpBlkAssetStreamMPEGTSRx = new Component([BlkJSX,"blkAssetStreamMPEGTSRxInfo"]);

  var cmpBlkAssetStreamRTPRx = new Component([BlkJSX,"blkAssetStreamRTPRxInfo"]);

  var cmpBlkAssetStreamUDPRx = new Component([BlkJSX,"blkAssetStreamUDPRxInfo"]);

  var cmpMulticastAddrRangeFrom = new CmpTxtBox([BlkJSX,"txtMulticastAddrRangeFrom"], null, null, null, null);
  
  var cmpMulticastAddrRangeTo = new CmpTxtBox([BlkJSX,"txtMulticastAddrRangeTo"], null, null, null, null);   
  
  var cmpMulticastTTL = new CmpTxtBox([BlkJSX,"txtMulticastTTL"], null, null, null, null);        
  
  var cmpCtrlPortRangeFrom = new CmpTxtBox([BlkJSX,"txtCtrlPortRangeFrom"], null, null, null, null);   
  
  var cmpCtrlPortRangeTo = new CmpTxtBox([BlkJSX,"txtCtrlPortRangeTo"], null, null, null, null);
  
  var cmpAVPortRangeFrom = new CmpTxtBox([BlkJSX,"txtAVPortRangeFrom"], null, null, null, null); 
  
  var cmpAVPortRangeTo = new CmpTxtBox([BlkJSX,"txtAVPortRangeTo"], null, null, null, null);   
  
  var cmpMPEGTSAVPortRangeFrom = new CmpTxtBox([BlkJSX,"txtMPEGTSAVPortRangeFrom"], null, null, null, null);  
  
  var cmpMPEGTSAVPortRangeTo = new CmpTxtBox([BlkJSX,"txtMPEGTSAVPortRangeTo"], null, null, null, null);  
  
  var cmpRTPAVPortRangeFrom = new CmpTxtBox([BlkJSX,"txtRTPAVPortRangeFrom"], null, null, null, null); 
  
  var cmpRTPAVPortRangeTo = new CmpTxtBox([BlkJSX,"txtRTPAVPortRangeTo"], null, null, null, null);     
  
  var cmpUDPAVPortRangeFrom = new CmpTxtBox([BlkJSX,"txtUDPAVPortRangeFrom"], null, null, null, null);   
  
  var cmpUDPAVPortRangeTo = new CmpTxtBox([BlkJSX,"txtUDPAVPortRangeTo"], null, null, null, null);     
  
  var cmpBlkAssetStreamUI = new CmpContainer( [BlkJSX,"STREAMSETTINGS"],
                                                  [{cmp:cmpBlkAssetStreamAll,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == 'AssetStreamAll' || topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == null "},
                                                   {cmp:cmpBlkAssetStreamTx,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == 'AssetStreamTx'"},
                                                   {cmp:cmpBlkAssetStreamRx,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == 'AssetStreamRx'"},
                                                   {cmp:cmpBlkAssetStreamMPEGTSRx,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == 'AssetStreamMPEGTSRx'"},
                                                   {cmp:cmpBlkAssetStreamRTPRx,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == 'AssetStreamRTPRx'"},
                                                   {cmp:cmpBlkAssetStreamUDPRx,
                                                    sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStreamConfigSelection') == 'AssetStreamUDPRx'"},
                                                   {cmp:cmpMulticastAddrRangeFrom},
                                                   {cmp:cmpMulticastAddrRangeTo},
                                                   {cmp:cmpMulticastTTL},
                                                   {cmp:cmpCtrlPortRangeFrom},
                                                   {cmp:cmpCtrlPortRangeTo},
                                                   {cmp:cmpAVPortRangeFrom},
                                                   {cmp:cmpAVPortRangeTo},
                                                   {cmp:cmpMPEGTSAVPortRangeFrom},
                                                   {cmp:cmpMPEGTSAVPortRangeTo},
                                                   {cmp:cmpRTPAVPortRangeFrom},
                                                   {cmp:cmpRTPAVPortRangeTo},
                                                   {cmp:cmpUDPAVPortRangeFrom},
                                                   {cmp:cmpUDPAVPortRangeTo}
                                                   ],
                                                  [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}
                                                  ]
                                                );
                                                 
  var cmpLblAssetStreamAllTx = new CmpLabelHighlight([BlkJSX,"lblAssetStreamAllTx"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:"AssetStreamAll"} );
  var cmpLblAssetStreamV2DTx = new CmpLabelHighlight([BlkJSX,"lblAssetStreamV2DTx"], null, 
                                                     [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}],
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:"AssetStreamTx"} );
  var cmpLblAssetStreamV2DRx = new CmpLabelHighlight([BlkJSX,"lblAssetStreamV2DRx"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:"AssetStreamRx"} );
  var cmpLblAssetStreamMPEGTSRx = new CmpLabelHighlight([BlkJSX,"lblAssetStreamMPEGTSRx"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:"AssetStreamMPEGTSRx"} );
  var cmpLblAssetStreamRTPRx = new CmpLabelHighlight([BlkJSX,"lblAssetStreamRTPRx"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:"AssetStreamRTPRx"} );
  var cmpLblAssetStreamUDPRx = new CmpLabelHighlight([BlkJSX,"lblAssetStreamUDPRx"], null, 
                                                         [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:null}],
                                                         {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStreamConfigSelection", eventval:"AssetStreamUDPRx"} );
  
  var cmpBlkAssetStreamLabel = new CmpContainer( [BlkJSX,"AssetStreamingMENU"],
                                                  [ {cmp:cmpLblAssetStreamAllTx},
                                                    {cmp:cmpLblAssetStreamV2DTx},
                                                    {cmp:cmpLblAssetStreamV2DRx},
                                                    {cmp:cmpLblAssetStreamMPEGTSRx},
                                                    {cmp:cmpLblAssetStreamRTPRx},
                                                    {cmp:cmpLblAssetStreamUDPRx}
                                                  ],
                                                  null
                                                );

  var cmpBlkLblAssetAllTxSettings = new Component([BlkJSX,"blkLblAssetStreamAllTx"]);

  var cmpBlkLblAssetTxSettings = new Component([BlkJSX,"blkLblAssetStreamV2DTx"]);

  var cmpBlkLblAssetRxSettings = new Component([BlkJSX,"blkLblAssetStreamV2DRx"]);

  var cmpBlkLblAssetMPEGTSRxSettings = new Component([BlkJSX,"blkLblAssetStreamMPEGTSRx"]);

  var cmpBlkLblAssetRTPRxSettings = new Component([BlkJSX,"blkLblAssetStreamRTPRx"]);

  var cmpBlkLblAssetUDPRxSettings = new Component([BlkJSX,"blkLblAssetStreamUDPRx"]);
  
  var cmpBlkAssetStreamLabelBlk = new CmpContainer( [BlkJSX,"AssetStreamingMENU"],
                                                    [{cmp:cmpBlkLblAssetAllTxSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkAllTxSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetTxSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkTxSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetRxSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkRxSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetMPEGTSRxSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkMPEGTSRxSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetRTPRxSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkRTPRxSettings') != 'NONE'"},
                                                     {cmp:cmpBlkLblAssetUDPRxSettings,
                                                      sel:"topic_Get(\"" + tDlg + "\", 'EditBlkUDPRxSettings') != 'NONE'"}
                                                     ],
                                                    [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkAllTxSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkTxSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkRxSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkMPEGTSRxSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkRTPRxSettings", eventval:null},
                                                     {action:"TOPICEVENT", topic:tDlg, eventvar:"EditBlkUDPRxSettings", eventval:null}
                                                    ]
                                                  );

  var cmpBlkEditAssetStream = new CmpContainer( [BlkJSX,"blkAssetStreamSettings"],
                                            [{cmp:cmpBlkAssetStreamLabelBlk},
                                             {cmp:cmpBlkAssetStreamLabel},
                                             {cmp:cmpBlkAssetStreamUI}
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
        
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAddAssetStream"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetStream"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditAssetSetAssetStateStream", "DONE", this,
                                                                function() {
                                                                  dlgEditAsset_Destroy();
                                                                });
                                              cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                            }
                                           });
                                                                                         
      
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"DialogStreamSettings"],
                            [ {cmp:cmpBtnEditAssetOK},
                              {cmp:cmpBtnEditAssetCancel},
                              {cmp:cmpBtnEditAssetClose},
                              {cmp:cmpBlkEditAssetStream},
                              {cmp:cdfDlgEditStreaming}
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
 
    var objParent = cmpBlkEditAssetStream.BlkJSX.getParent();
    
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
      
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//StreamsTxConfig"))) {
        topic_Publish(tDlg, "EditBlkAllTxSettings", deviceNode.selectSingleNode(".//StreamsTxConfig"));
      }
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//V2DStreamsTxConfig"))) {
        topic_Publish(tDlg, "EditBlkTxSettings", deviceNode.selectSingleNode(".//V2DStreamsTxConfig"));
      }
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//V2DStreamsRxConfig"))) {
        topic_Publish(tDlg, "EditBlkRxSettings", deviceNode.selectSingleNode(".//V2DStreamsRxConfig"));
      }
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//MPEGTSStreamsRxConfig"))) {
        topic_Publish(tDlg, "EditBlkMPEGTSRxSettings", deviceNode.selectSingleNode(".//MPEGTSStreamsRxConfig"));
      }
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//RTPStreamsRxConfig"))) {
        topic_Publish(tDlg, "EditBlkRTPRxSettings", deviceNode.selectSingleNode(".//RTPStreamsRxConfig"));
      }
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//UDPStreamsRxConfig"))) {
        topic_Publish(tDlg, "EditBlkUDPRxSettings", deviceNode.selectSingleNode(".//UDPStreamsRxConfig"));
      }
      // set the Device Config data
      var strXMLCacheId = cdfDlgEditStreaming.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(deviceNode.toString());
      cdfDlgEditStreaming.BlkJSX.setSourceXML(objXML);         
      cdfDlgEditStreaming.BlkJSX.setCDFId(DeviceNID);
      
      topic_Publish(tDlg, "EditAssetDataState", AssetNID);      
            
      if(jsx3.util.strEmpty(deviceNode.selectSingleNode(".//StreamsTxConfig"))) {
        topic_Publish(tDlg ,"EditAssetStreamConfigSelection","AssetStreamRx");
      } else {
        topic_Publish(tDlg ,"EditAssetStreamConfigSelection","AssetStreamAll");
      }       
    }

  }// end get dlg data function


  //Basic Validation  
  function cmpDlgEditAssetSetAssetValidate() {
    var deviceNode = getDialogDataNode(tDlg, "Device");
    if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//StreamsTxConfig"))){
      var MulticastFromIP = cmpMulticastAddrRangeFrom.BlkJSX.getValue();
      var MulticastToIP = cmpMulticastAddrRangeTo.BlkJSX.getValue();
      var FromIPAddressValidation = verifyMulticastIPAddress(MulticastFromIP);
      if(!FromIPAddressValidation){
        cmpMulticastAddrRangeFrom.BlkJSX.focus();
        return false;
      }
      var ToIPAddressValidation = verifyMulticastIPAddress(MulticastToIP);
      if(!ToIPAddressValidation){
        cmpMulticastAddrRangeTo.BlkJSX.focus();
        return false;
      }
      var startIPValue = ipAddress2num(MulticastFromIP);
      var endIPValue = ipAddress2num(MulticastToIP);
      if(endIPValue <= startIPValue){
        alert("Multicast End Address should be greater than Multicast Start Address");
        return false;
      }
      var multicastTTL = cmpMulticastTTL.BlkJSX.getValue();
      if(multicastTTL == "") {
        alert("Enter Multicast TTL Value");
        cmpMulticastTTL.BlkJSX.focus();
        return false;
      }
      else{
        if( isNaN(multicastTTL) || multicastTTL < 1 || multicastTTL > 255 || multicastTTL.search(/\./i )!=-1){
          alert("Multicast TTL value should be an integer value from 1 to 255");
          cmpMulticastTTL.BlkJSX.focus();
          return false;
        }
      }
      var controlPortRangeFrom = cmpCtrlPortRangeFrom.BlkJSX.getValue();
      var controlPortRangeTo = cmpCtrlPortRangeTo.BlkJSX.getValue();
      if(jsx3.util.strEmpty(controlPortRangeFrom)){
        alert("Enter Control Port Start value");
        cmpCtrlPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if(jsx3.util.strEmpty(controlPortRangeTo)){
        alert("Enter Control Port End value");
        cmpCtrlPortRangeTo.BlkJSX.focus();
        return false;
      }
      else if( isNaN(controlPortRangeFrom) || controlPortRangeFrom < 1 || controlPortRangeFrom.search(/\./i )!=-1){
        alert("Control Port Start value should be an integer value and greater than 1");
        cmpCtrlPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if( isNaN(controlPortRangeTo) || controlPortRangeTo < 1 || controlPortRangeTo < controlPortRangeFrom || controlPortRangeTo.search(/\./i )!=-1){
        alert("Control Port End value should be an integer value and greater than or equal to Start Port value");
        cmpCtrlPortRangeTo.BlkJSX.focus();
        return false;
      }
    }
    if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//V2DStreamsRxConfig"))){
      var avPortRangeFrom = cmpAVPortRangeFrom.BlkJSX.getValue();
      var avPortRangeTo = cmpAVPortRangeTo.BlkJSX.getValue();
      if(jsx3.util.strEmpty(avPortRangeFrom)){
        alert("Enter Audio/Video Port Start value");
        cmpAVPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if(jsx3.util.strEmpty(avPortRangeTo)){
        alert("Enter Audio/Video Port End value");
        cmpAVPortRangeTo.BlkJSX.focus();
        return false;
      }
      else if( isNaN(avPortRangeFrom) || avPortRangeFrom < 1 || avPortRangeFrom.search(/\./i )!=-1){
        alert("Audio/Video Port Start value should be an integer value and greater than 1");
        cmpAVPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if( isNaN(avPortRangeTo) || avPortRangeTo < 1 || avPortRangeTo < avPortRangeFrom || avPortRangeTo.search(/\./i )!=-1){
        alert("Audio/Video Port End value should be an integer value and greater than or equal to Start Port value");
        cmpAVPortRangeTo.BlkJSX.focus();
        return false;
      }
    }
    if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//RTPStreamsRxConfig"))){
      var mpegAVPortRangeFrom = cmpMPEGTSAVPortRangeFrom.BlkJSX.getValue();
      var mpegAVPortRangeTo = cmpMPEGTSAVPortRangeTo.BlkJSX.getValue();
      if(jsx3.util.strEmpty(mpegAVPortRangeFrom)){
        alert("Enter MPEG Audio/Video Port From value");
        cmpMPEGTSAVPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if(jsx3.util.strEmpty(mpegAVPortRangeTo)){
        alert("Enter MPEG Audio/Video Port To value");
        cmpMPEGTSAVPortRangeTo.BlkJSX.focus();
        return false;
      }
      else if( isNaN(mpegAVPortRangeFrom) || mpegAVPortRangeFrom < 1 || mpegAVPortRangeFrom.search(/\./i )!=-1){
        alert("MPEG Audio/Video Port From value should be an integer value and greater than 1");
        cmpMPEGTSAVPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if( isNaN(mpegAVPortRangeTo) || mpegAVPortRangeTo < 1 || mpegAVPortRangeTo < mpegAVPortRangeFrom || mpegAVPortRangeTo.search(/\./i )!=-1){
        alert("MPEG Audio/Video Port To value should be an integer value and greater than or equal to From value");
        cmpMPEGTSAVPortRangeTo.BlkJSX.focus();
        return false;
      }
      var rtpAVPortRangeFrom = cmpRTPAVPortRangeFrom.BlkJSX.getValue();
      var rtpAVPortRangeTo = cmpRTPAVPortRangeTo.BlkJSX.getValue();
      if(jsx3.util.strEmpty(rtpAVPortRangeFrom)){
        alert("Enter RTP Audio/Video Port From value");
        cmpRTPAVPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if(jsx3.util.strEmpty(rtpAVPortRangeTo)){
        alert("Enter RTP Audio/Video Port To value");
        cmpRTPAVPortRangeTo.BlkJSX.focus();
        return false;
      }
      else if( isNaN(rtpAVPortRangeFrom) || rtpAVPortRangeFrom < 1 || rtpAVPortRangeFrom.search(/\./i )!=-1){
        alert("RTP Audio/Video Port From value should be an integer value and greater than 1");
        cmpRTPAVPortRangeFrom.BlkJSX.focus();
        return false;
      }
      else if( isNaN(rtpAVPortRangeTo) || rtpAVPortRangeTo < 1 || rtpAVPortRangeTo < rtpAVPortRangeFrom || rtpAVPortRangeTo.search(/\./i )!=-1){
        alert("RTP Audio/Video Port To value should be an integer value and greater than or equal to From value");
        cmpRTPAVPortRangeTo.BlkJSX.focus();
        return false;
      }
      if(!jsx3.util.strEmpty(deviceNode.selectSingleNode(".//UDPStreamsRxConfig"))){
        var udpAVPortRangeFrom = cmpUDPAVPortRangeFrom.BlkJSX.getValue();
        var udpAVPortRangeTo = cmpUDPAVPortRangeTo.BlkJSX.getValue();
        if(jsx3.util.strEmpty(udpAVPortRangeFrom)){
          alert("Enter UDP Audio/Video Port From value");
          cmpUDPAVPortRangeFrom.BlkJSX.focus();
          return false;
        }
        else if(jsx3.util.strEmpty(udpAVPortRangeTo)){
          alert("Enter UDP Audio/Video Port To value");
          cmpUDPAVPortRangeTo.BlkJSX.focus();
          return false;
        }
        else if( isNaN(udpAVPortRangeFrom) || udpAVPortRangeFrom < 1 || udpAVPortRangeFrom.search(/\./i )!=-1){
          alert("UDP Audio/Video Port From value should be an integer value and greater than 1");
          cmpUDPAVPortRangeFrom.BlkJSX.focus();
          return false;
        }
        else if( isNaN(udpAVPortRangeTo) || udpAVPortRangeTo < 1 || udpAVPortRangeTo < udpAVPortRangeFrom || udpAVPortRangeTo.search(/\./i )!=-1){
          alert("UDP Audio/Video Port To value should be an integer value and greater than or equal to From value");
          cmpUDPAVPortRangeTo.BlkJSX.focus();
          return false;
        }
      }
    }
    return true;
  }
      
  // fnc to set dlg data - runs in the context of the OK button
  function cmpDlgEditAssetSetAsset(AssetNID) {
  
    if( cmpDlgEditAssetSetAssetValidate() != true ) return;
     
    var deviceNode = getDialogDataNode(tDlg, "Device");
    var SelectedAssetAgentJID = deviceNode.getAttribute("ownerUserJID");
    
    var deviceStreamNode = null;
    // write the data back to the CDF
    cdfDlgEditStreaming.BlkJSX.write();

    //get the Device Config data
    var strXMLCacheId = cdfDlgEditStreaming.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetStreamingRequest.xsl","EditAssetStreamingRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var assetXML = xslDoc.transform(objXML);
    deviceStreamNode = new jsx3.xml.Document();
    deviceStreamNode.loadXML(assetXML);
    if( deviceStreamNode == null ) return;
    
    // check if anything got chnaged
    var modifyNode = deviceStreamNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tDlg, "EditAssetSetAssetStateStream", "DONE");
      return;    
    }

    var streamingServiceConfigNode = deviceNode.selectSingleNode(".//DeviceConfig/ServiceElementsConfig/StreamingServiceConfig").cloneNode(true);
    streamingServiceConfigNode.setAttribute("Dirty", "MODIFY");
    var objBlkAssetAllTxSettings = cmpBlkLblAssetAllTxSettings.BlkJSX;
    var streamsAllTXConfigNode = deviceStreamNode.selectSingleNode("//StreamsTxConfig");
    if( objBlkAssetAllTxSettings.getDisplay() == null || objBlkAssetAllTxSettings.getDisplay() == "none" ) {
      streamsAllTXConfigNode.getParent().removeChild(streamsAllTXConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = streamsAllTXConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgStreamsAllTXConfigNode = streamingServiceConfigNode.selectSingleNode("//StreamsTxConfig");
          streamsAllTXConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = streamingServiceConfigNode.selectSingleNode(".//StreamsTxConfig").getParent();
          parentNode.replaceNode(streamsAllTXConfigNode, orgStreamsAllTXConfigNode);        
        }
      }
    }

    var objBlkV2DTxSettings = cmpBlkLblAssetTxSettings.BlkJSX;
    var streamsV2DTXConfigNode = deviceStreamNode.selectSingleNode("//V2DStreamsTxConfig");
    if( objBlkV2DTxSettings.getDisplay() == null || objBlkV2DTxSettings.getDisplay() == "none" ) {
      streamsV2DTXConfigNode.getParent().removeChild(streamsV2DTXConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = streamsV2DTXConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgStreamsV2DTXConfigNode = streamingServiceConfigNode.selectSingleNode("//V2DStreamsTxConfig");
          streamsV2DTXConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = streamingServiceConfigNode.selectSingleNode(".//V2DStreamsTxConfig").getParent();
          parentNode.replaceNode(streamsV2DTXConfigNode, orgStreamsV2DTXConfigNode);        
        }
      }
    }

    var objBlkV2DRxSettings = cmpBlkLblAssetRxSettings.BlkJSX;
    var streamsV2DRxConfigNode = deviceStreamNode.selectSingleNode("//V2DStreamsRxConfig");
    if( objBlkV2DRxSettings.getDisplay() == null || objBlkV2DRxSettings.getDisplay() == "none" ) {
      streamsV2DRxConfigNode.getParent().removeChild(streamsV2DRxConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = streamsV2DRxConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgStreamsV2DRxConfigNode = streamingServiceConfigNode.selectSingleNode("//V2DStreamsRxConfig");
          streamsV2DRxConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = streamingServiceConfigNode.selectSingleNode(".//V2DStreamsRxConfig").getParent();
          parentNode.replaceNode(streamsV2DRxConfigNode, orgStreamsV2DRxConfigNode);        
        }
      }
    }

    var objBlkMPEGTSRxSettings = cmpBlkLblAssetMPEGTSRxSettings.BlkJSX;
    var streamsMPEGTSRxConfigNode = deviceStreamNode.selectSingleNode("//MPEGTSStreamsRxConfig");
    if( objBlkMPEGTSRxSettings.getDisplay() == null || objBlkMPEGTSRxSettings.getDisplay() == "none" ) {
      streamsMPEGTSRxConfigNode.getParent().removeChild(streamsMPEGTSRxConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = streamsMPEGTSRxConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgStreamsMPEGTSRxConfigNode = streamingServiceConfigNode.selectSingleNode("//MPEGTSStreamsRxConfig");
          streamsMPEGTSRxConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = streamingServiceConfigNode.selectSingleNode(".//MPEGTSStreamsRxConfig").getParent();
          parentNode.replaceNode(streamsMPEGTSRxConfigNode, orgStreamsMPEGTSRxConfigNode);        
        }
      }
    }

    var objBlkRTPRxSettings = cmpBlkLblAssetRTPRxSettings.BlkJSX;
    var streamsRTPRxConfigNode = deviceStreamNode.selectSingleNode("//RTPStreamsRxConfig");
    if( objBlkRTPRxSettings.getDisplay() == null || objBlkRTPRxSettings.getDisplay() == "none" ) {
      streamsRTPRxConfigNode.getParent().removeChild(streamsRTPRxConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = streamsRTPRxConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgStreamsRTPRxConfigNode = streamingServiceConfigNode.selectSingleNode("//RTPStreamsRxConfig");
          streamsRTPRxConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = streamingServiceConfigNode.selectSingleNode(".//RTPStreamsRxConfig").getParent();
          parentNode.replaceNode(streamsRTPRxConfigNode, orgStreamsRTPRxConfigNode);        
        }
      }
    }

    var objBlkUDPRxSettings = cmpBlkLblAssetUDPRxSettings.BlkJSX;
    var streamsUDPRxConfigNode = deviceStreamNode.selectSingleNode("//UDPStreamsRxConfig");
    if( objBlkUDPRxSettings.getDisplay() == null || objBlkUDPRxSettings.getDisplay() == "none" ) {
      streamsUDPRxConfigNode.getParent().removeChild(streamsUDPRxConfigNode);
    }
    else {
      if( ManagementMode != "Local" ) {
        var DirtyVal = streamsUDPRxConfigNode.getAttribute("Dirty");
        if( DirtyVal == "MODIFY" ) {
          var orgStreamsUDPRxConfigNode = streamingServiceConfigNode.selectSingleNode("//UDPStreamsRxConfig");
          streamsUDPRxConfigNode.setAttribute("Dirty", "CLEAN");
          var parentNode = streamingServiceConfigNode.selectSingleNode(".//UDPStreamsRxConfig").getParent();
          parentNode.replaceNode(streamsUDPRxConfigNode, orgStreamsUDPRxConfigNode);        
        }
      }
    }

    if( ManagementMode != "Local"  ) {
      deviceConfigPath = "//StreamingServiceConfig";
      var streamingConfigNode = deviceStreamNode.selectSingleNode(".//StreamingServiceConfig");   
      var streamsConfigParentNode = deviceStreamNode.selectSingleNode(".//StreamingServiceConfig").getParent();
      streamsConfigParentNode.replaceNode(streamingServiceConfigNode, streamingConfigNode);        
      /*if(AssetType == "SessionClient-Device"){
        AssetType = "IPVSClient-Device";
      }
      var reqSetDeviceObject = new ReqSetObject( "Replace", AssetType, streamingServiceConfigNode, 
                                                 {action:"FNCCALL", context:this, 
                                                   fnc:function(respCode, respVal) {
                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                           topic_Publish(tDlg, "EditAssetSetAssetStateStream", "DONE");
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
      reqParams["Op_Where_WhereXPath"] = "//IPVSClient-Device[AssetResourceNID='" + AssetNID + "']";
      reqParams["Op_In_ChildXPath"] = "DeviceConfig/ServiceElementsConfig/StreamingServiceConfig";
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
                                                               topic_Publish(tDlg, "EditAssetSetAssetStateStream", "DONE");
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
    requestDataObjectArr["DeviceConfig"] = deviceStreamNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = AssetNID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = deviceConfigPath;
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
    //} 
  } // end cmpDlgEditAssetSetAsset
       
} // end create
