// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgShowStorage_Status(AssetNID, SelectedAssetOwnerJID, selectedAssetType, selectedAssetName, selectedAssetStatus) {
  
  //check status
  if( selectedAssetStatus != "Ready" ) {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus
     );
   return false; 
  }

  // load view - dialog component file
  var DlgFile = "components/assets/storageStatusDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  var windowTitle = selectedAssetName + " Storage Status";

  var winBlkJsxTitle = getJsxByName([BlkJSX,"windowBar"]);
  winBlkJsxTitle.setText(windowTitle,"true");
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  var agentEventDlg = new EventListner(SelectedAssetOwnerJID);
  
  var blkHardDiskStrXMLCacheId = "HardDiskDoc" + tDlg;
  var blkHardDiskStorageStatus = getJsxByName([BlkJSX,"blkHardDiskStatus"]);  
  blkHardDiskStorageStatus.setXMLId(blkHardDiskStrXMLCacheId);

  var blkNASStrXMLCacheId = "NASDoc" + tDlg;
  var blkNASStatus = getJsxByName([BlkJSX,"blkNASStatus"]);  
  blkNASStatus.setXMLId(blkNASStrXMLCacheId);

  function dlgShowStorage_Destroy() {
    // destroy any local eventlistners
    agentEventDlg.destroy();
    eventDlg.destroy();
    // destroy all local components
    cmpDlgStorageShow.destroy();
    topic_Destroy(tDlg);
    // close dialog
    BlkJSX.doClose();
  }                      
  
                                               
  var cmpBtnShowStorageClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgShowStorage_Destroy();
                                              }
                                               });
                                               
  var cmpDlgStorageShow = new CmpContainer([BlkJSX,"DialogStorageStatus"],  
                                       [ {cmp:cmpBtnShowStorageClose}
                                       ],
                                       null);  
                              
  cmpDlgStorageShow.BlkJSX = BlkJSX;   
  cmpDlgStorageShow.AssetNID = AssetNID;
  cmpDlgStorageShow.AssetOwnerJID = SelectedAssetOwnerJID;
  cmpDlgStorageShow.getBlkData =  cmpDlgGetStorageStatus;

  agentEventDlg.subscribe("DeviceStatus_Full", null, this,
                           function() {
                             cmpDlgGetStorageStatus(AssetNID, SelectedAssetOwnerJID);
                           }
                         );

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.AssetNID, this.AssetOwnerJID);
               }
             ),
             null, cmpDlgStorageShow );
 
  function cmpDlgGetStorageStatus(AssetNID, AssetOwnerJID) {
    if( AssetOwnerJID == null ) return;
    var storageElementStatusNode = topic_GetNode(AssetOwnerJID, ".//StorageElementsStatus");
    var hardDiskStatusNode = storageElementStatusNode.selectSingleNode(".//HardDiskTableStatus/ParametersList");
    if( hardDiskStatusNode != null ) {
       var hardDiskListIter  = hardDiskStatusNode.selectNodeIterator(".//Parameters");
       while(hardDiskListIter.hasNext()) {
         var hardDiskListItemNode = hardDiskListIter.next();
         cmpDeviceStorageStatus(hardDiskListItemNode);
       }
       var strXMLCacheId = blkHardDiskStorageStatus.getXMLId();
       var newDoc = new jsx3.xml.Document();
       newDoc.loadXML(hardDiskStatusNode.toString());    
       PNameSpace.getCache().setDocument(strXMLCacheId, newDoc);     
       blkHardDiskStorageStatus.repaint();
    }
    var nasStatusNode = storageElementStatusNode.selectSingleNode(".//NASStatus/ParametersList");
    if( nasStatusNode != null ) {
       var nasListIter  = nasStatusNode.selectNodeIterator(".//Parameters");
       if(nasListIter.hasNext()) {
         blkNASStatus.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
         while(nasListIter.hasNext()) {
           var nasListItemNode = nasListIter.next();
           cmpDeviceStorageStatus(nasListItemNode);
         }
         var strXMLCacheId = blkNASStatus.getXMLId();
         var newDoc = new jsx3.xml.Document();
         newDoc.loadXML(nasStatusNode.toString());    
         PNameSpace.getCache().setDocument(strXMLCacheId, newDoc);     
         blkNASStatus.repaint();
       }
       else{
          blkNASStatus.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
       }
    }
  }

  function cmpDeviceStorageStatus(deviceStorageNode){
     var deviceTotal = deviceStorageNode.selectSingleNode(".//Parameter[@name='Total']").getValue();
     if(/^\d+$/.test(deviceTotal) != false){
      deviceTotal = convertBytesToGigaBytes(deviceTotal);
     }
     var deviceUsed = deviceStorageNode.selectSingleNode(".//Parameter[@name='Used']").getValue();
     if(/^\d+$/.test(deviceUsed) != false){
      deviceUsed = convertBytesToGigaBytes(deviceUsed);
     }
     var deviceAvailable = deviceStorageNode.selectSingleNode(".//Parameter[@name='Available']").getValue();
     if(/^\d+$/.test(deviceAvailable) != false){
      deviceAvailable = convertBytesToGigaBytes(deviceAvailable);
     }
     deviceStorageNode.selectSingleNode(".//Parameter[@name='Total']").setValue(deviceTotal);
     deviceStorageNode.selectSingleNode(".//Parameter[@name='Used']").setValue(deviceUsed);
     deviceStorageNode.selectSingleNode(".//Parameter[@name='Available']").setValue(deviceAvailable);
  }

} // end dlg status

