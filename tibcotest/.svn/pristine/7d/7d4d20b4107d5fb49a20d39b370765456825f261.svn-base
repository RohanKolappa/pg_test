// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgHAStatus_Create(AssetNID, SelectedAssetOwnerJID) {
  
  // load view - dialog component file
  var DlgFile = "components/assets/HADiagDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);
  var agentEventDlg = new EventListner(SelectedAssetOwnerJID);
  
  function dlgHAStatus_Destroy() {
    // destroy any local eventlistners
    agentEventDlg.destroy();
    eventDlg.destroy();
    // destroy all local components
    cmpDlgHAServerStatus.destroy();
    topic_Destroy(tDlg);
    // close dialog
    BlkJSX.doClose();
  }                      
  
  var cmpHeartbeatStatus = new CmpLabel(([BlkJSX, "HeartbeatStatus"]), null, null);
  var cmpPeerStatus = new CmpLabel(([BlkJSX, "PeerStatus"]), null, null);
  var cmpResourceStatus = new CmpLabel(([BlkJSX, "ResourceStatus"]), null, null);

  var cmpDlgHAStatus = new CmpContainer([BlkJSX,"blkHAStatus"],
                                        [ {cmp:cmpHeartbeatStatus},
                                          {cmp:cmpPeerStatus},
                                          {cmp:cmpResourceStatus}
                                        ],
                                        null);
                                          
  var cmpBtnHAStatusClose = new CmpButton([BlkJSX,"btnDialogCloseX"], null,  
                                          {action:"FNCCALL", context:null, 
                                            fnc:function() {
                                              dlgHAStatus_Destroy();
                                            }
                                          });

  var cmpBtnHAClose = new CmpButton([BlkJSX,"btnHAStatusDone"], null, 
                                    {action:"FNCCALL", context:null, 
                                      fnc:function() {
                                        dlgHAStatus_Destroy();
                                      }
                                    });
                                               
  var cmpDlgHAServerStatus = new CmpContainer([BlkJSX,"HAStatusDialog"],  
                                               [ {cmp:cmpBtnHAClose},
                                                 {cmp:cmpBtnHAStatusClose},
                                                 {cmp:cmpDlgHAStatus} 
                                               ],
                                               null);  
                              
  cmpDlgHAServerStatus.BlkJSX = BlkJSX;   
  cmpDlgHAServerStatus.AssetNID = AssetNID;
  cmpDlgHAServerStatus.AssetOwnerJID = SelectedAssetOwnerJID;
  cmpDlgHAServerStatus.getBlkData =  cmpDlgGetHAStatus;
  cmpDlgHAServerStatus.tDlg = tDlg;

  // init and repaint happens after the dialog is loaded
  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData(this.AssetNID, this.AssetOwnerJID);
               }
             ),
             null, cmpDlgHAServerStatus );
 
  agentEventDlg.subscribe("DeviceStatus_Full", null, this,
                           function() {
                             cmpDlgGetHAStatus(AssetNID, SelectedAssetOwnerJID);
                           }
                         );

  function cmpDlgGetHAStatus(assetNID, assetOwnerJID) {
    cmpHeartbeatStatus.BlkJSX.setText("", true);
    cmpPeerStatus.BlkJSX.setText("", true);
    cmpResourceStatus.BlkJSX.setText("", true);
    if( assetOwnerJID == null ) return;
    var haStatusNode = topic_GetNode(assetOwnerJID, ".//HAServerStatus");
    if( haStatusNode != null ) {
      var haListIter  = haStatusNode.selectNodeIterator(".//Parameters");
      while( haListIter.hasNext() ) {
        var haListItemNode = haListIter.next();
        if( !jsx3.util.strEmpty(haListItemNode.selectSingleNode("//Parameter[@name='Heartbeat Status']")) ){
          var heartbeatStatus = haListItemNode.selectSingleNode("//Parameter[@name='Heartbeat Status']").getValue();
          if( heartbeatStatus == "True" ){
            cmpHeartbeatStatus.BlkJSX.setText("UP", true);
          }
          else{
            cmpHeartbeatStatus.BlkJSX.setText("Down", true);
          }
        }
        if( !jsx3.util.strEmpty(haListItemNode.selectSingleNode("//Parameter[@name='Peer Status']")) ){
          var peerStatus = haListItemNode.selectSingleNode("//Parameter[@name='Peer Status']").getValue();
          if( peerStatus == "True" ){
            cmpPeerStatus.BlkJSX.setText("UP", true);
          }
          else{
            cmpPeerStatus.BlkJSX.setText("Down", true);
          }
        }
        if( !jsx3.util.strEmpty(haListItemNode.selectSingleNode("//Parameter[@name='Resource Status']")) ){
          var resourceStatus = haListItemNode.selectSingleNode("//Parameter[@name='Resource Status']").getValue();
          cmpResourceStatus.BlkJSX.setText(resourceStatus, true);
        }
      }
    }
  }

} // end dlg status

