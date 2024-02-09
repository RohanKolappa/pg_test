function cmpBlkConnectionDestination_Create(winCollaboration, connectionBlkJSX, dstDataInfo, tConn) {


  function cmpBlkConnectionDst_Destroy(cmpBlkConnectionDst) {
    // get a handle to the current BlkJSX
    var ConnectionDstBlkJSX = cmpBlkConnectionDst.BlkJSX;
    var ConnectionDstListBlkJSX = ConnectionDstBlkJSX.getParent();

   // finally destroy the DOM element
    ConnectionDstListBlkJSX.removeChild(ConnectionDstBlkJSX);
    ConnectionDstListBlkJSX.repaint();  
  } // end cmpBlkConnectionDst_Destroy
  
  
  // clone a destination block
  var orgDstBlkJSX = getJsxByName([connectionBlkJSX, "blkConnectionDstNew"]);
  var newDstBlkJSX = orgDstBlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
  // attach it to the connection Destination list
  var DstListBlkJSX = getJsxByName([connectionBlkJSX, "blkConnectionDstList"]);
  DstListBlkJSX.adoptChild(newDstBlkJSX, true);
  
  var tConnDst = tConn;
  var gId = dstDataInfo.ConnectionGrpID;
  var destEntityId = dstDataInfo.dstEntityID;
  var ConnectionID = dstDataInfo.ConnectionID;
  var eventConCollaboration = winCollaboration.eventWin;
  // event listener for ConnectionState
  eventConCollaboration.subscribe("ConnectionState", null, this,
                                  function(EventVar, EventVal) {
                                    var connectionInfo = EventVal;
                                    var connectionId = connectionInfo.connId;
                                    var connState = connectionInfo.state;
                                    // update destination connection image
                                    cmpImgConnState.updateConnImage(connectionId, connState);
                                  
                                  }
                               )
  // event listener for CollaborationState
  eventConCollaboration.subscribe("CollaborationState", "NONE", this,
                                  function(EventVar, EventVal) {
                                    var connectionId = topic_Get(tConnDst, "ConnectionID");
                                    cmpImgConnState.updateConnImage(connectionId, "");
                                                                                                          
                                  }
                                )                                                                
  
  var cmpBtnDstRemove = new CmpImageButton([newDstBlkJSX,"btnRemoveDst"],
                                                null,
                                                {action: "FNCCALL",
                                                 context: this,
                                                 fnc: function() {                                                        
                                                        // delete connection from the xml - if last conn - only clear dest entiyt - no delete
                                                        winCollaboration.sessionConfig.updateConnGroupDelDst(gId, destEntityId)                                                        
                                                        
                                                        // notify UI
                                                        topic_Publish(tConnDst, "ConnectionDeleteDst", destEntityId);
                                                      }
                                                }
                                              );

  var cmpLblDstName = new CmpLabel(([newDstBlkJSX, "lblDstMedia"]),
                                   null,
                                   [{action:"TOPICEVENT", topic:tConnDst, eventvar:"dstName", eventval:null}]);
                                                                                      
  var cmpImgDstType = new Component([newDstBlkJSX, "imgDstEntityType"], null); 
 
  var cmpImgConnState = new Component([newDstBlkJSX, "imgConnState"], null);
  cmpImgConnState.updateConnImageInfo = {setVal:{CONNECTED:"JSXAPPS/IPVS_ClientUI/images/icons/icon_network_on.png",
                                                 CONNECTING:"JSXAPPS/IPVS_ClientUI/images/icons/icon_network_waiting.png", 
                                                 ERROR:"JSXAPPS/IPVS_ClientUI/images/icons/icon_network_error.png", 
                                                 DISCONNECTED:"JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png",
                                                 DEFAULT:"JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png"}};
                                                
  cmpImgConnState.updateConnImage = function(id, val) {
                                      // now set the connection state image
                                      var connectionID = topic_Get(tConnDst, "ConnectionID");
                                      if( (connectionID == id) ) {
                                        if( ! this.updateConnImageInfo.setVal[val] ) val = "DEFAULT";
                                        this.BlkJSX.setSrc(this.updateConnImageInfo.setVal[val]);
                                        this.BlkJSX.repaint();                                           
                                      }
                                     }                                                            
                                                            
                                                               
  var cmpBlkConnectionDst = new CmpContainer([connectionBlkJSX, newDstBlkJSX],
                                             [{cmp:cmpImgConnState},
                                              {cmp:cmpImgDstType},
                                              {cmp:cmpLblDstName},
                                              {cmp:cmpBtnDstRemove}
                                             ],
                                             [{action:"TOPICEVENT", topic:tConnDst, eventvar:"ConnectionDstDataUpdate", eventval:null}],
                                              {action: "FNCCALL",
                                               context: null,
                                               fnc: function(){
                                                        cmpBlkConnectionDst_Destroy(cmpBlkConnectionDst);
                                                      }                                         
                                               }
                                            );
  
  cmpBlkConnectionDst.destEntityId = destEntityId
  
  cmpBlkConnectionDst.getData = function() {
    // publish the dstName in the conn topic
    cmpLblDstName.BlkJSX.setText(dstDataInfo.dstName, true);
    cmpImgDstType.BlkJSX.setSrc(dstDataInfo.typeImage);
    cmpImgDstType.BlkJSX.repaint();
    cmpBlkConnectionDst.repaint();  
    
    // show the block
    newDstBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);    
  } // end cmpBlkConnectionDst.getData
  
  // init and show it - in its own context
  jsx3.sleep((function() {
                 this.init();
                 this.show();
                 this.getData();
               }
              ), null, cmpBlkConnectionDst);

  // return a handle to it
  return cmpBlkConnectionDst;  
                                             
} // end cmpBlkConnectionDestination_Create