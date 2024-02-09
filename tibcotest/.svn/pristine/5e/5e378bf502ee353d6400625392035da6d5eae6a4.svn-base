// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgEditDeviceEditStorage_Create(AssetNID, AssetType, selectedAssetName, selectedAssetStatus) {
  if( selectedAssetStatus != "Ready" ) {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus
     );
   return false; 
  }

  var DeviceNID = "";
  var DeviceResourceNIDArr = [];
  var CompleteAddDeviceArr = [];
  
  closeDialogIfExists("dialogStorage");
  // load view - dialog component file
  var DlgFile = "components/assets/assetStorageSettingsDialog.xml";
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
  
  var cmpCDFEditAssetStorage = new Component([BlkJSX,"cdfDlgEditAssetStorage"]);
  cmpCDFEditAssetStorage.BlkJSX.setXMLTransformers("xsl/EditAssetStorage.xsl");

  var cmpMtxNAS = new CmpBlock([BlkJSX,"mtxNASTable"]);
  cmpMtxNAS.BlkJSX.setXMLTransformers("xsl/NASTableToCDF.xsl");

  var cmpBlkAssetBtnDeleteNASEntry = new CmpButton([BlkJSX,"imgbtnDeleteNASEntry"],
                                                    null,
                                                    { action:"FNCCALL", 
                                                      context:null,
                                                      fnc:function() {
                                                        var objMatrix = this.NASTableBlkJSX;
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
                                                          this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkAssetBtnDeleteNASEntry.BlkJSX);
                                                        } else if (objMatrix.getRecordIds().length > 0 && jsx3.util.strEmpty(selRecordNode)) {
                                                          alert("Please select NAS entry to delete");
                                                          return false;
                                                        }                                                  
                                                      }
                                                    }
                                                  );
  cmpBlkAssetBtnDeleteNASEntry.NASTableBlkJSX = cmpMtxNAS.BlkJSX;
  cmpBlkAssetBtnDeleteNASEntry.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;
    
  var cmpBlkAssetBtnAddNASEntry = new CmpButton([BlkJSX,"imgbtnAddNASEntry"],
                                                null,
                                                { action:"FNCCALL", 
                                                  context:null,
                                                  fnc:function() {
                                                    var o = new Object;
                                                    o.jsxid = jsx3.xml.CDF.getKey();
                                                    o.jsxDirtyBit = "ADD";
                                                    o.jsxMediaStoreRootDirPath = "/data/DSS/data/portal/site/NAS";
                                                    var objMatrix = this.NASTableBlkJSX;
                                                    var objMatrixSortedIds = objMatrix.getSortedIds();
                                                    if( objMatrixSortedIds.length == 0 ) {
                                                      objMatrix.insertRecord(o,true);
                                                    }
                                                    else {
                                                      objMatrix.insertRecordBefore(o, objMatrixSortedIds[0], true);
                                                    }
                                                    this.cmpDlgEditAssetSetDirtyBitValue(cmpBlkAssetBtnAddNASEntry.BlkJSX);
                                                  }
                                                }
                                               );
  cmpBlkAssetBtnAddNASEntry.NASTableBlkJSX =  cmpMtxNAS.BlkJSX;
  cmpBlkAssetBtnAddNASEntry.cmpDlgEditAssetSetDirtyBitValue = cmpDlgEditAssetSetDirtyBitValue;                                               
   
  var cmpBlkAssetStorageNAS = new Component([BlkJSX,"blkAssetStorageNAS"]);
                                              
  var cmpBlkAssetStorageUI = new CmpContainer([BlkJSX,"StorageSettingsUI"],
                                              [{cmp:cmpBlkAssetStorageNAS,
                                                sel:"topic_Get(\"" + tDlg + "\", 'EditAssetStorageConfigSelection') == 'NAS' || " +
                                                    "topic_Get(\"" + tDlg + "\", 'EditAssetStorageConfigSelection') == null "}
                                              ],
                                              [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStorageConfigSelection", eventval:null}
                                              ]
                                             );
                                                 
  var cmpLblAssetNASConfig = new CmpLabelHighlight([BlkJSX,"lblAssetStorageNAS"], null, 
                                                   [{action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStorageConfigSelection", eventval:null}],
                                                   {action:"TOPICEVENT", topic:tDlg, eventvar:"EditAssetStorageConfigSelection", eventval:"NAS"} );

  var cmpBlkAssetStorageLabel = new CmpContainer( [BlkJSX,"StorageSettingsMenu"],
                                                  [ {cmp:cmpLblAssetNASConfig}
                                                  ],
                                                  null
                                                );
                                                
  var cmpBlkLblAssetNASConfig = new Component([BlkJSX,"blkLblAssetStorageNAS"]);
  
  var cmpBlkAssetStorageLabelBlk = new CmpContainer([BlkJSX,"StorageSettingsMenu"],
                                                    [{cmp:cmpBlkLblAssetNASConfig}
                                                    ],
                                                    null
                                                   );                                                 

  var cmpBlkEditAssetStorage = new CmpContainer([BlkJSX,"blkAssetStorageSettings"],
                                            [{cmp:cmpBlkAssetStorageLabelBlk},
                                             {cmp:cmpBlkAssetStorageLabel},
                                             {cmp:cmpBlkAssetStorageUI},
                                             {cmp:cmpMtxNAS},
                                             {cmp:cmpBlkAssetBtnAddNASEntry},
                                             {cmp:cmpBlkAssetBtnDeleteNASEntry}
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
        
  var cmpBtnEditAssetCancel = new CmpButton([BlkJSX,"btnCancelAddAssetStorage"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              dlgEditAsset_Destroy();
                                            }
                                           });
            
  var cmpBtnEditAssetOK = new CmpButton([BlkJSX,"btnAddAssetStorage"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              eventDlg.subscribe("EditAssetSetAssetStateStorage", "DONE", this,
                                                                function() {
                                                                  dlgEditAsset_Destroy();
                                                                });
                                              cmpDlgEditAssetSetAsset.call(this, AssetNID);
                                            }
                                           });
                                                                                         
      
  var cmpDlgEditAsset = new CmpContainer([BlkJSX,"dialogStorage"],
                                          [ {cmp:cmpBtnEditAssetOK},
                                            {cmp:cmpBtnEditAssetCancel},
                                            {cmp:cmpBtnEditAssetClose},
                                            {cmp:cmpBlkEditAssetStorage},
                                            {cmp:cmpCDFEditAssetStorage}
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
 
    var objParent = cmpBlkEditAssetStorage.BlkJSX.getParent();
    
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

      var NASNode = deviceNode.selectSingleNode(".//NASConfig").cloneNode(true); 
      if(!jsx3.util.strEmpty(NASNode.selectSingleNode(".//NASEntry"))){
        var strXMLCacheId = cmpMtxNAS.BlkJSX.getXMLId();
        var objXMLNAS = PNameSpace.getCache().getDocument(strXMLCacheId);
        objXMLNAS.loadXML(NASNode.toString());
        cmpMtxNAS.BlkJSX.setSourceXML(objXMLNAS);        
        cmpMtxNAS.BlkJSX.repaint();
      }

      // set the Device Config data
      var strXMLCacheId = cmpCDFEditAssetStorage.BlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      objXML.loadXML(deviceNode.toString());
      cmpCDFEditAssetStorage.BlkJSX.setSourceXML(objXML);         
      cmpCDFEditAssetStorage.BlkJSX.setCDFId(DeviceNID);

      topic_Publish(tDlg, "EditAssetDataState", AssetNID);      
      //      
      topic_Publish(tDlg ,"EditAssetStorageConfigSelection","NAS");      
    }

  }// end get dlg data function
      

  //Basic Validation  
  function cmpDlgEditAssetSetAssetValidate() {
    // check for duplicate mediastorename in the matrix
    var objMatrix = cmpMtxNAS.BlkJSX;
    if (objMatrix) {
      var strXMLCacheId = objMatrix.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
      if (objXML) {
        var objRecordNodes = objXML.selectNodes("//record");
        for (var it = objRecordNodes.iterator();it.hasNext();){
          var objRecordNode = it.next();
          var mediaStoreNameID = objRecordNode.getAttribute("jsxMediaStoreName");
          var duplicateCount = objXML.selectNodes(".//*[@jsxMediaStoreName='"+ mediaStoreNameID +"']");
          if((duplicateCount.size() > 1 ) ){
            alert("'"+ mediaStoreNameID + "' ID already exists. Please enter different ID");
            return false;
          }
          var jsxDirtyBit = objRecordNode.getAttribute("jsxDirtyBit");
          if (jsxDirtyBit == "DELETE") continue;
          var jsxID = objRecordNode.getAttribute("jsxid");
          // check for store anme, ipaddress and path
          var jsxMediaStoreName = objRecordNode.getAttribute("jsxMediaStoreName");
          var jsxServer = objRecordNode.getAttribute("jsxServer");
          var jsxRemoteDir = objRecordNode.getAttribute("jsxRemoteDir");
          var msg = "";
          if (jsx3.util.strEmpty(jsxMediaStoreName)) {
            msg += "Please enter ID.\n";
          }
          if (jsx3.util.strEmpty(jsxServer)) {
            msg += "Please enter IP address.\n";
          }
          else {
            var verifyIP = verifyIPAddress(jsxServer);
            if (!verifyIP) {
              msg += "Please enter valid IP address.\n";
            }
          }
          if (jsx3.util.strEmpty(jsxRemoteDir)) {
            msg += "Please enter mount path.\n";
          }
          else{
            if (jsxRemoteDir.indexOf('\\') != -1){
              msg += "Please enter correct NFS mount path(For example,  /home/videorec1).\n";
              objMatrix.focusCellById(jsxID, "jsxRemoteDir");
            }
          }
          if (!jsx3.util.strEmpty(msg)) {
            alert(msg);
            if(jsx3.util.strEmpty(jsxMediaStoreName)){
              objMatrix.focusCellById(jsxID, "jsxMediaStoreName");
            }
            else if (jsx3.util.strEmpty(jsxServer) || !verifyIP) {
              objMatrix.focusCellById(jsxID, "jsxServer");
            }
            else if(jsx3.util.strEmpty(jsxRemoteDir)){
              objMatrix.focusCellById(jsxID, "jsxRemoteDir");
            }

            return false;
          }

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
    
    var deviceStorageNode = null;
    // write the data back to the CDF
    cmpCDFEditAssetStorage.BlkJSX.write();

    //get the device System data
    var strXMLCacheId = cmpCDFEditAssetStorage.BlkJSX.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);      
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/EditAssetStorageRequest.xsl","EditAssetStorageRequest","xsl");
    // genrate the request xml
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var deviceSystemXML = xslDoc.transform(objXML);
    deviceStorageNode = new jsx3.xml.Document();
    deviceStorageNode.loadXML(deviceSystemXML);
    
    //get the NAS Config data
    var strXMLCacheId = cmpMtxNAS.BlkJSX.getXMLId(); cmpMtxNAS.BlkJSX.getValue()
    var objXMLEthernetPort = PNameSpace.getCache().getDocument(strXMLCacheId);
    var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS_AdminUI/xsl/CDFToNASTable.xsl","NASConfigNode","xsl");
    var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
    var newNASXML = xslDoc.transform(objXMLEthernetPort);        
    var newNASNode = new jsx3.xml.Document();
    newNASNode.loadXML(newNASXML);
    
    var oldNASNode = deviceStorageNode.selectSingleNode(".//NASConfig");              
    var NASParentNode = deviceStorageNode.selectSingleNode(".//NASConfig").getParent();
    var NASTableNodeVal = oldNASNode.getAttribute("Dirty");
    newNASNode.setAttribute("Dirty", NASTableNodeVal);
    NASParentNode.replaceNode(newNASNode,oldNASNode);

    if( deviceStorageNode == null ) return;

    // check if anything got chnaged
    var modifyNode = deviceStorageNode.selectSingleNode('.//*[@Dirty="MODIFY"]');
    if(modifyNode == null) {
      topic_Publish(tDlg, "EditAssetSetAssetStateStorage", "DONE");
      return;    
    }

    var reqSetDeviceConfig = new ReqSessionGetRequest("SetDeviceConfig",
                                                     { action: "FNCCALL", context: this,
                                                         fnc: function (respCode, respVal) {
                                                             if (respCode == "SUCCESS" && respVal != null) {
                                                                 var resVal = respVal;
                                                                 var resCode = respCode;
                                                                 topic_Publish(tDlg, "EditAssetSetAssetStateStorage", "DONE");
                                                             } else {
                                                                 var strXMLCacheId = cmpMtxNAS.BlkJSX.getXMLId(); 
                                                                 var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                                 if (objXML) {
                                                                    var selRecord = objXML.selectSingleNode(".//record[@jsxselected='1' or @jsxDirtyBit='ADD']");
                                                                    if (selRecord) {
                                                                        var jsxId = selRecord.getAttribute("jsxid");
                                                                        cmpMtxNAS.BlkJSX.focusCellById(jsxId, "jsxMediaStoreName");
                                                                    }
                                                                 }

                                                                 var errorDesc = jsx3.util.strEmpty(CommonErrorCodeArr[respCode]) ? "Unknown" : CommonErrorCodeArr[respCode].errorDesc;
                                                                 var ErrorMsg = "Error: Device->Storage request failed \n";
                                                                 ErrorMsg += "Error description: " + errorDesc + "\n";
                                                                 ErrorMsg += "Error code: " + respCode;
                                                                 alert(ErrorMsg);
                                                             }
                                                         }
                                                     }
                                                   );
  
    reqSetDeviceConfig.ServiceName = "SessionService";
    var requestDataObjectArr = new Object;
    requestDataObjectArr["DeviceConfig"] = deviceStorageNode;
    reqSetDeviceConfig.requestDataObjectArr = requestDataObjectArr;
    var reqParams = new Object;
    reqParams["ObjectList_ByNID"] = AssetNID;
    reqParams["ObjectList_ByTitle"] = "";
    reqParams["ObjectList_Path"] = "";
    reqSetDeviceConfig.reqParams = reqParams;
    reqSetDeviceConfig.sendRequest();
 
  } // end cmpDlgEditAssetSetAsset
       
} // end create
