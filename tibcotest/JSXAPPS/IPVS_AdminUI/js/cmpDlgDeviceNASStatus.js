// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgShowNAS_Status(AssetNID, SelectedAssetOwnerJID, selectedAssetType, selectedAssetName, selectedAssetStatus) {
  
  //check status
  if( selectedAssetStatus != "Ready" ) {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus
     );
   return false; 
  }  

  // load view - dialog component file
  var DlgFile = "components/assets/nasStatusDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  var windowTitle = selectedAssetName + " NAS Status";

  var winBlkJsxTitle = getJsxByName([BlkJSX,"windowBar"]);
  winBlkJsxTitle.setText(windowTitle,"true");
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  var agentEventDlg = new EventListner(SelectedAssetOwnerJID);
  
  function dlgShowNAS_Destroy() {
    // destroy any local eventlistners
    agentEventDlg.destroy();
    eventDlg.destroy();
    // destroy all local components
    cmpDlgShowNAS.destroy();
    topic_Destroy(tDlg);
    // close dialog
    BlkJSX.doClose();
  }                      
  
  var cmpMtxNASList = new CmpNIDCustomList([BlkJSX,"mtxNasStatus"],cmpBlkNASList_Repaint,
                                            null,
                                            {persist:{col:"jsxtext",initval:false}}
                                           );
                                               
  var cmpBtnShowNASClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgShowNAS_Destroy();
                                              }
                                               });
                                               
  var cmpDlgShowNAS = new CmpContainer([BlkJSX,"DialogNASStatus"],  
                                       [ {cmp:cmpBtnShowNASClose},
                                         {cmp:cmpMtxNASList} 
                                       ],
                                       null);  
                              
  cmpDlgShowNAS.BlkJSX = BlkJSX;   
  cmpDlgShowNAS.AssetNID = AssetNID;
  cmpDlgShowNAS.AssetOwnerJID = SelectedAssetOwnerJID;
  cmpDlgShowNAS.getBlkData =  cmpBlkNASList_Repaint;

  agentEventDlg.subscribe("DeviceStatus_Full", null, this,
                           function() {
                             cmpBlkNASList_Repaint(AssetNID, SelectedAssetOwnerJID);
                           }
                         );

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.AssetNID, this.AssetOwnerJID);
               }
             ),
             null, cmpDlgShowNAS );
 
  function cmpBlkNASList_Repaint(AssetNID, AssetOwnerJID) {
    if( AssetOwnerJID == null ) return;
    var NASStatusNode = topic_GetNode(AssetOwnerJID, ".//NASStatus/ParametersList");
    var mtxBlkJSX = getJsxByName([BlkJSX,"mtxNasStatus"]);
    mtxBlkJSX.clearXmlData();
    mtxBlkJSX.setValue(null);
    if( NASStatusNode != null ) {
      var NASListIter  = NASStatusNode.selectNodeIterator(".//Parameters");
      while(NASListIter.hasNext()) {
        var NASListItemNode = NASListIter.next();
        var o = new Object();
        o.jsxid = NASListItemNode.selectSingleNode(".//Parameter[@name='Index']").getValue();
        o.jsxname = NASListItemNode.selectSingleNode(".//Parameter[@name='Name']").getValue();
        o.jsxserver = NASListItemNode.selectSingleNode(".//Parameter[@name='Server']").getValue();
        o.jsxstatus = NASListItemNode.selectSingleNode(".//Parameter[@name='Status']").getValue();
        o.jsxerror = NASListItemNode.selectSingleNode(".//Parameter[@name='ErrorMessage']").getValue();
        
        var NASTotal = NASListItemNode.selectSingleNode(".//Parameter[@name='Total']").getValue();
        NASTotal = convertBytesToGigaBytes(NASTotal);
        var NASUsed = NASListItemNode.selectSingleNode(".//Parameter[@name='Used']").getValue();
        NASUsed = convertBytesToGigaBytes(NASUsed);
        var NASAvailable = NASListItemNode.selectSingleNode(".//Parameter[@name='Available']").getValue();
        NASAvailable = convertBytesToGigaBytes(NASAvailable);
        var NASPercentUsed = NASListItemNode.selectSingleNode(".//Parameter[@name='PercentUsed']").getValue();

        var strUsage = "Total : " + NASTotal + " GB<br>";
        strUsage += "Used : " + NASUsed + " GB<br>";
        strUsage += "Available : " + NASAvailable + " GB<br>";
        strUsage += "PercentUsed : " + NASPercentUsed + "%";

        o.jsxusage = strUsage;

        mtxBlkJSX.insertRecord(o, null, false);        
      }
    }
    mtxBlkJSX.repaintData();
  }

} // end dlg status

