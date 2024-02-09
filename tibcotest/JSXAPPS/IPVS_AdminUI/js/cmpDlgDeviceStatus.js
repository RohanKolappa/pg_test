// this code has some potential issues 
// - w.r.t object reference counts - when objects created in here go out of scope etc
// - loading sync issues - dlg may not be loaded before the object init methods are called

function dlgShowDevice_Status(AssetNID, SelectedAssetOwnerJID, selectedAssetType, selectedAssetName, selectedAssetStatus) {
  
  //check status
  if( selectedAssetStatus != "Ready" ) {
    alert("Operation cannot be performed on the selected device: " + selectedAssetName + "\n" +
      "Device status is " + selectedAssetStatus
     );
   return false; 
  }

  // load view - dialog component file
  var DlgFile = "components/assets/streamingMonitor.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);
  var windowTitle = selectedAssetName + " Status";

  var winBlkJsxTitle = getJsxByName([BlkJSX,"windowBar"]);
  winBlkJsxTitle.setText(windowTitle,"true");
  
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey()
  topic_Create(tDlg);
  var eventDlg = new EventListner(tDlg);

  if( jsx3.util.strEmpty(dialogCellRefreshArr)){
    dialogCellRefreshArr = new Object;
  }
  dialogCellRefreshArr[tDlg] = new Object;

  if( jsx3.util.strEmpty(dialogLocalArr)){
    dialogLocalArr = new Object;
  }  
  dialogLocalArr[tDlg] = new Object;
  dialogLocalArr[tDlg] = {"SysmonStatusInterval":null,
                          "SysmonIntervalID":null,
                          "AgentContactJID":SelectedAssetOwnerJID,
                          "BlkJSX":BlkJSX
                         };

  var SysmonStatusAgentObj = "SysMonDeviceStatus";
  var SysmonAgentObjectReqSuccessEventVar = arrAgentObjectInfo[SysmonStatusAgentObj].successevent.eventvar;
  
  var mtxNewStrXMLCacheId = "mtxDoc" + tDlg;
  getJsxByName([BlkJSX,"mtxSysMonOutput"]).setXMLId(mtxNewStrXMLCacheId);
  var blkNewStrXMLCacheId = "SSDoc" + tDlg;
  getJsxByName([BlkJSX,"blkStatsOutputContainer"]).setXMLId(blkNewStrXMLCacheId);  
                          
  var RecordArr = {jsxid:{objtype:"SysMonDeviceStatus",
                           objxpath:"\".//Parameter[@name='Id']\""},
                    sysMonType:{objtype:"SysMonDeviceStatus",
                                objxpath:"\".//Parameter[@name='Type']\""},
                    sysMonQueued:{objtype:"SysMonDeviceStatus",
                                  objxpath:"\".//Parameter[@name='QueueId']\""},
                    sysMonDuration:{objtype:"SysMonDeviceStatus",
                                    objxpath:"\".//Parameter[@name='Duration']\""},
                    sysMonFrames:{objtype:"SysMonDeviceStatus",
                                  objxpath:"\".//Parameter[@name='FramesTransfered']\""},
                    sysMonBitrate:{objtype:"SysMonDeviceStatus",
                                   objxpath:"\".//Parameter[@name='Bitrate']\""},
                    sysMonConnected:{objtype:"SysMonDeviceStatus",
                                     objxpath:"\".//Parameter[@name='Connected']\""},
                    sysMonStarted:{objtype:"SysMonDeviceStatus",
                                     objxpath:"\".//Parameter[@name='Started']\""}
                  };
  
  function dlgShowDevice_Destroy() {
    IpvsLogin.unsubscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, BlkJSX);
    clearSysmonStatusAgentReqInterval();
    // destroy any local eventlistners
    eventDlg.destroy();
    // destroy all local components
    cmpDlgShowDevice.destroy();

    dialogCellRefreshArr[tDlg] = null;
    delete dialogCellRefreshArr[tDlg];
    
    dialogLocalArr[tDlg] = null;
    delete dialogLocalArr[tDlg];
    // destroy the local topic  
    topic_Destroy(tDlg);
    // close dialog
    BlkJSX.doClose();
  }  
  
  function clearSysmonStatusAgentReqInterval() {
    for(var topic in eventHandlerArr) {
      if( !jsx3.util.strEmpty(dialogLocalArr[tDlg]) ){
        if( topic == dialogLocalArr[tDlg].AgentContactJID) {
          eventHandlerArr[topic].destroy();
          delete eventHandlerArr[topic];
        }
      }
    }
    if( !jsx3.util.strEmpty(dialogLocalArr[tDlg]) ){
      clearAgentObjRequest(dialogLocalArr[tDlg].AgentContactJID, SysmonStatusAgentObj);
      clearInterval(dialogLocalArr[tDlg].SysmonIntervalID);
      dialogLocalArr[tDlg].SysmonIntervalID = null; 
    }
  }

  function insertDataInDetailBlk(dataString) {

    var BlkJSXStatsOutput = getJsxByName([BlkJSX,"blkStatsOutputContainer"]);
    var StrXMLCacheId = BlkJSXStatsOutput.getXMLId();
    var newDoc = new jsx3.xml.Document();
    newDoc.loadXML(dataString);    
    PNameSpace.getCache().setDocument(StrXMLCacheId, newDoc);     
    BlkJSXStatsOutput.repaint();
  }
  
  eventDlg.subscribe("DeviceStreamSelected", null, this,
                      function() {
                        var instanceStatusString = null;
                        var rowID = topic_Get(tDlg, "DeviceStreamSelected");
                        var instanceStatusNode = topic_GetNode(dialogLocalArr[tDlg].AgentContactJID, ".//StreamingServiceStatus/InstanceList");
                        if(instanceStatusNode != null) {
                          var instanceListIter  = instanceStatusNode.selectNodeIterator(".//Parameters");
                          while(instanceListIter.hasNext()) {
                            var instanceListIterNode = instanceListIter.next();
                            var Type = instanceListIterNode.selectSingleNode(".//Parameter[@name='Type']").getValue();
                            var RowID = instanceListIterNode.selectSingleNode(".//Parameter[@name='Id']").getValue();
                            if( rowID == RowID ) {
                              instanceStatusString = instanceStatusNode.toString();
                              insertDataInDetailBlk(instanceStatusString);
                            }
                          }
                        }
                      }
                    ); // subscribe end 

  eventDlg.subscribe("SysmonStreamInstanceType", null, this,
                      function() {
                        var SysmonStreamInstanceType = topic_Get(tDlg, "SysmonStreamInstanceType");
                        var stateOff = "";
                        var stateOn = "color:#fff;font-weight:bold;background-color:#aaa;";
                        cmpBtnShowDeviceWriters.BlkJSX.setCSSOverride(stateOff).repaint();
                        cmpBtnShowDeviceReaders.BlkJSX.setCSSOverride(stateOff).repaint();
                        cmpBtnShowDeviceReceivers.BlkJSX.setCSSOverride(stateOff).repaint();
                        cmpBtnShowDeviceSenders.BlkJSX.setCSSOverride(stateOff).repaint();
                        if(SysmonStreamInstanceType == "WRITER") {
                          cmpBtnShowDeviceWriters.BlkJSX.setCSSOverride(stateOn).repaint();
                        } else if(SysmonStreamInstanceType == "READER") {
                          cmpBtnShowDeviceReaders.BlkJSX.setCSSOverride(stateOn).repaint();
                        } else if(SysmonStreamInstanceType == "RECEIVER") {
                          cmpBtnShowDeviceReceivers.BlkJSX.setCSSOverride(stateOn).repaint();
                        } else if(SysmonStreamInstanceType == "SENDER") {
                          cmpBtnShowDeviceSenders.BlkJSX.setCSSOverride(stateOn).repaint();
                        } 
                      }
                    ); // subscribe end                            
  
  var cmpBlkDeviceStreamList = new CmpNIDCustomList([BlkJSX,"mtxSysMonOutput"],cmpBlkDeviceStreamList_Repaint,
                                                    null,
                                                    {persist:{col:"jsxtext",initval:false},
                                                     action:"TOPICEVENT",  topic:tDlg, eventvar:"DeviceStreamSelected", eventval:"jsxid"}
                                                   );
                                            
  var cmpBtnShowDeviceRefreshSet = new CmpButton([BlkJSX,"btnSysMonRefreshSet"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    clearSysmonStatusAgentReqInterval();
                                                    cmpDlgShowDeviceGetDeviceStatus(AssetNID, dialogLocalArr[tDlg].AgentContactJID, selectedAssetStatus);
                                                  }
                                                   });
                                                   
  var cmpBtnShowDeviceRates = new CmpButton([BlkJSX,"btnSysMonRates"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  
                                                }
                                                 });
                                                
  var cmpBtnShowDeviceControl = new CmpButton([BlkJSX,"btnSysMonControl"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  
                                                }
                                                 });
                                                 
  var cmpBtnShowDevicePause = new CmpButton([BlkJSX,"btnSysMonPause"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  clearSysmonStatusAgentReqInterval();
                                                  //cmpBlkDeviceStreamList_Repaint();
                                                }
                                                 });
                                                 
  var cmpBtnShowDeviceGlobal = new CmpButton([BlkJSX,"btnSysMonGlobal"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  
                                                }
                                                 });
                                                 
  var cmpBtnShowDeviceWriters = new CmpButton([BlkJSX,"btnSysMonWriters"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  topic_Publish(tDlg, "SysmonStreamInstanceType", "WRITER");
                                                  cmpBlkDeviceStreamList_Repaint();  
                                                }
                                                 });
                                                 
  var cmpBtnShowDeviceReaders = new CmpButton([BlkJSX,"btnSysMonReaders"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  topic_Publish(tDlg, "SysmonStreamInstanceType", "READER");
                                                  cmpBlkDeviceStreamList_Repaint();
                                                }
                                                 });
                                                 
  var cmpBtnShowDeviceReceivers = new CmpButton([BlkJSX,"btnSysMonReceivers"],
                                                 null, 
                                                 {action:"FNCCALL", 
                                                  context:null, 
                                                  fnc:function() {
                                                    topic_Publish(tDlg, "SysmonStreamInstanceType", "RECEIVER");
                                                    cmpBlkDeviceStreamList_Repaint();
                                                  }
                                                   });
                                                 
  var cmpBtnShowDeviceSenders = new CmpButton([BlkJSX,"btnSysMonSenders"],
                                               null, 
                                               {action:"FNCCALL", 
                                                context:null, 
                                                fnc:function() {
                                                  topic_Publish(tDlg, "SysmonStreamInstanceType", "SENDER");
                                                  cmpBlkDeviceStreamList_Repaint();
                                                }
                                                 });
                                               
  var cmpBtnShowDeviceQuit = new CmpButton([BlkJSX,"btnSysMonQuit"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgShowDevice_Destroy();
                                              }
                                               });
                                               
  var cmpBtnShowDeviceClose = new CmpButton([BlkJSX,"btnDialogCloseX"],
                                             null, 
                                             {action:"FNCCALL", 
                                              context:null, 
                                              fnc:function() {
                                                dlgShowDevice_Destroy();
                                              }
                                               });
                                               
    var cmpDlgShowDevice = new CmpContainer([BlkJSX,"DialogDeviceStatus"],  
                                            [ {cmp:cmpBtnShowDeviceClose},
                                              {cmp:cmpBtnShowDeviceQuit},
                                              {cmp:cmpBtnShowDeviceSenders},
                                              {cmp:cmpBtnShowDeviceReceivers},
                                              {cmp:cmpBtnShowDeviceReaders},
                                              {cmp:cmpBtnShowDeviceWriters},
                                              //{cmp:cmpBtnShowDeviceGlobal},
                                              //{cmp:cmpBtnShowDevicePause},
                                              //{cmp:cmpBtnShowDeviceControl},
                                              //{cmp:cmpBtnShowDeviceRates},
                                              {cmp:cmpBtnShowDeviceRefreshSet},
                                              {cmp:cmpBlkDeviceStreamList}  
                                            ],
                                            null);  
                              
    cmpDlgShowDevice.BlkJSX = BlkJSX;   
    cmpDlgShowDevice.AssetNID = AssetNID;
    cmpDlgShowDevice.AssetOwnerJID = SelectedAssetOwnerJID;
    cmpDlgShowDevice.AssetStatus = selectedAssetStatus;
    cmpDlgShowDevice.clearAnyOldData = clearSysmonStatusAgentReqInterval;
    cmpDlgShowDevice.getBlkData =  cmpDlgShowDeviceGetDeviceStatus;
  
    // init and repaint happens after the dialog is loaded
    jsx3.sleep( (function(){
                   this.init();
                   this.clearAnyOldData();
                   this.getBlkData(this.AssetNID, this.AssetOwnerJID, this.AssetStatus);
                 }
               ),
               null, cmpDlgShowDevice );
 
      
    // fnc to get dlg data
    function cmpDlgShowDeviceGetDeviceStatus(AssetNID, AssetOwnerJID, AssetStatus) {
      var SysmonStreamInstanceType = topic_Get(tDlg, "SysmonStreamInstanceType");
      if( SysmonStreamInstanceType == null) {
        topic_Publish(tDlg, "SysmonStreamInstanceType", "RECEIVER");
      }
      cmpBlkDeviceStreamList_Repaint();
      var AgentJID = AssetOwnerJID;

      SysmonStatusInterval = getJsxByName([BlkJSX,"txtSysMonRefresh"]).getValue();
      if( (SysmonStatusInterval == "") || (SysmonStatusInterval.match(/[^0-9]/)) ) {
        alert("Please enter an integer refresh rate value");
        var intervalTime = dialogLocalArr[tDlg].SysmonStatusInterval;
        getJsxByName([BlkJSX,"txtSysMonRefresh"]).setValue(intervalTime);
        getJsxByName([BlkJSX,"txtSysMonRefresh"]).focus();
        return false;
      }
      SysmonStatusInterval = parseInt(SysmonStatusInterval, 10);
      getJsxByName([BlkJSX, "txtSysMonRefresh"]).setValue(SysmonStatusInterval);

      if(dialogLocalArr[tDlg].SysmonIntervalID == null) {
        dialogLocalArr[tDlg].SysmonStatusInterval = SysmonStatusInterval;
        dialogLocalArr[tDlg].SysmonIntervalID = setAgentReqInterval(SysmonStatusInterval, AgentJID, SysmonStatusAgentObj, AssetStatus );
        
        var AgentObjectReqTopic = AgentJID;
        if( eventHandlerArr[AgentObjectReqTopic] == null ) {
          eventHandlerArr[AgentObjectReqTopic] = new EventListner(AgentObjectReqTopic);
          eventHandlerArr[AgentObjectReqTopic].subscribe(SysmonAgentObjectReqSuccessEventVar, 
                                                       null, this, cmpBlkDeviceStreamList_Refresh);
        }
      }
        

    }// end cmpDlgShowDeviceGetDeviceStatus
    
    function cmpBlkDeviceStreamList_Refresh() {
      var SysmonStreamInstanceType = topic_Get(tDlg, "SysmonStreamInstanceType");
      var instanceStatusNode = topic_GetNode(dialogLocalArr[tDlg].AgentContactJID, ".//StreamingServiceStatus/InstanceList");
      var mtxBlkJSX = getJsxByName([BlkJSX,"mtxSysMonOutput"]);
      var mtxStrXMLCacheId = mtxBlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(mtxStrXMLCacheId);
      if(instanceStatusNode != null) {
        var instanceListIter  = instanceStatusNode.selectNodeIterator(".//Parameters");
        var instanceFlag = false;
        var CacheRecordArr = [];
        while(instanceListIter.hasNext()) {
          var instanceListIterNode = instanceListIter.next();
          var Type = instanceListIterNode.selectSingleNode(".//Parameter[@name='Type']").getValue();
          var RowID = instanceListIterNode.selectSingleNode(".//Parameter[@name='Id']").getValue();
          for( var rowID in dialogCellRefreshArr[tDlg] ) {
            if( rowID == RowID ) {
              instanceFlag = true;
              CacheRecordArr.push({"ID":RowID});
              for( var ColName in dialogCellRefreshArr[tDlg][rowID] ) {
                var ColObj = dialogCellRefreshArr[tDlg][rowID][ColName].ColObj;
                var ColXPath = dialogCellRefreshArr[tDlg][rowID][ColName].ColObjXPath;
                var cellValue = instanceListIterNode.selectSingleNode(ColXPath).getValue();
                if (objXML) {
                  var objRecordNodes = objXML.selectNodes("//record");
                  for (var it = objRecordNodes.iterator();it.hasNext();){
                    var objRecordNode = it.next();
                    var strRecordNID = objRecordNode.getAttribute("jsxid");
                    if(strRecordNID == RowID){
                      objRecordNode.setAttribute(ColName, cellValue);
                    }
                  }
                }
                mtxBlkJSX.redrawCell(rowID,ColObj ,"");
              }
              var DeviceStreamSelected = topic_Get(tDlg, "DeviceStreamSelected");
              if(DeviceStreamSelected == RowID) {
                var instanceListIterString = instanceListIterNode.toString();
                insertDataInDetailBlk(instanceListIterString);
              }
            }
            else{
              //chek if the any new conn status is ther for the selected tab
              var SysmonStreamInstanceType = topic_Get(tDlg, "SysmonStreamInstanceType");
              var Type = instanceListIterNode.selectSingleNode(".//Parameter[@name='Type']").getValue();
              if( SysmonStreamInstanceType == "ALL" ) {
                instanceFlag = false;
              } else if( (SysmonStreamInstanceType == "WRITER") && ( (Type == "DMSRecorderPort") || (Type == "DeleterPort") ) ) {
                instanceFlag = false;
              } else if( (SysmonStreamInstanceType == "READER") && (Type == "DMSMediaPort")  ) {
                instanceFlag = false;
              } else if( (SysmonStreamInstanceType == "RECEIVER") && ((Type == "V2DEncoderPort") || (Type == "V2DNetworkPort") )  ) {
                instanceFlag = false;
              } else if( (SysmonStreamInstanceType == "SENDER") && ( (Type == "V2DDecoderPort") || (Type == "V2DHardWarePort") )  ) {
                instanceFlag = false;
              }              
            }
          }                    
        }
        if(!instanceFlag) {
          cmpBlkDeviceStreamList_Repaint();  
        }
        else {
          // clean up the matrix and dialogCellRefreshArr( delete rows for the deleted conn)
          var objXML1 = PNameSpace.getCache().getDocument(mtxStrXMLCacheId);
          var rowflag = true;
          if(objXML1) {
            var objRecordNodes = objXML1.selectNodes("//record");
            for (var it = objRecordNodes.iterator();it.hasNext();){
              var RecordNode = it.next();
              var strRecordID = RecordNode.getAttribute("jsxid");
              for(var CRAL=0; CRAL < CacheRecordArr.length; CRAL++) {
                var CacheID = CacheRecordArr[CRAL].ID;
                if( strRecordID == CacheID){
                  rowflag = true;
                  break;
                }
                else if( strRecordID != CacheID){
                  rowflag = false;                  
                }
              }
              if(!rowflag) {
                mtxBlkJSX.deleteRecord(strRecordID,true);
                delete dialogCellRefreshArr[tDlg][strRecordID];
                // clear the deatil status section if its the current selected connection
                var DeviceStreamSelected = topic_Get(tDlg, "DeviceStreamSelected");
                if(DeviceStreamSelected == strRecordID) {
                  var dataString = null;
                  insertDataInDetailBlk(dataString);
                }                
              }                
            }                
          }

        }        
      } 
    } // end cmpBlkDeviceStreamList_Refresh
    
    function cmpBlkDeviceStreamList_Repaint() {
      var instanceListIterString = null;
      insertDataInDetailBlk(instanceListIterString);
      var SysmonStreamInstanceType = topic_Get(tDlg, "SysmonStreamInstanceType");
      var instanceStatusNode = topic_GetNode(dialogLocalArr[tDlg].AgentContactJID, ".//StreamingServiceStatus/InstanceList");
 
      var mtxBlkJSX = getJsxByName([BlkJSX,"mtxSysMonOutput"]);
      var mtxStrXMLCacheId = mtxBlkJSX.getXMLId();
      var objXML = PNameSpace.getCache().getDocument(mtxStrXMLCacheId);
      dialogCellRefreshArr[tDlg] = new Object;
      if(instanceStatusNode != null) {
        mtxBlkJSX.clearXmlData();
        var instanceListIter  = instanceStatusNode.selectNodeIterator(".//Parameters");
        while(instanceListIter.hasNext()) {
          var instanceListIterNode = instanceListIter.next();
          var Type = instanceListIterNode.selectSingleNode(".//Parameter[@name='Type']").getValue();
          var RowID = instanceListIterNode.selectSingleNode(".//Parameter[@name='Id']").getValue();
          var flag = false;                                                
          if( SysmonStreamInstanceType == "ALL" ) {
            flag = true;
          } else if( (SysmonStreamInstanceType == "WRITER") && 
                     ( (Type == "DMSRecorderPort") ||
                       (Type == "DeleterPort") ||
                       (Type == "AnyRecorderPort")
                     ) ) {
            flag = true;
          } else if( (SysmonStreamInstanceType == "READER") && 
                     ( (Type == "DMSMediaPort") ||
                       (Type == "V2DMediaPort") ||
                       (Type == "AnyMediaPort")
                     ) ) {
            flag = true;
          } else if( (SysmonStreamInstanceType == "RECEIVER") && 
                     ( (Type == "V2DEncoderPort") || 
                       (Type == "V2DNetworkPort") ||
                       (Type == "WISEncoderPort") ||
                       (Type == "UDPNetworkPort") ||
                       (Type == "WISNetworkPort") ||
                       (Type == "VNCNetworkPort") ||
                       (Type == "RTPNetworkPort") ||
                       (Type == "SchedulerPort") ||
                       (Type == "MPEGTSNetworkPort") ||
                       (Type == "V2DAudioEncoderPort") ||
                       (Type == "AnyNetworkPort")
                     )  ) {
            flag = true;
          } else if( (SysmonStreamInstanceType == "SENDER") && 
                     ( (Type == "V2DDecoderPort") || 
                       (Type == "V2DHardWarePort") ||
                       (Type == "UDPDecoderPort") ||
                       (Type == "MPEGTSDecoderPort") ||
                       (Type == "RTPDecoderPort") ||
                       (Type == "V2DFastFwdPort") ||
                       (Type == "V2DAudioDecoderPort") ||
                       (Type == "AnyDecoderPort")
                     )  ) {
            flag = true;
          }
          if(flag) {
            var o = new Object();
            for( var colName in RecordArr ) {
              var colValObjectType = RecordArr[colName].objtype;
              var colValObjectXPath = eval(RecordArr[colName].objxpath); 
              var cellValue = instanceListIterNode.selectSingleNode(colValObjectXPath).getValue();                       
              if( dialogCellRefreshArr[tDlg][RowID] == null ) dialogCellRefreshArr[tDlg][RowID] = new Object;
              var colObj = mtxBlkJSX.getDescendantOfName(colName, false, false);
              dialogCellRefreshArr[tDlg][RowID][colName] = {"AgentObj":colValObjectType,
                                                            "ColObj":colObj,
                                                            "ColObjXPath":colValObjectXPath};
             o[colName] = cellValue;
            }
            mtxBlkJSX.insertRecord(o, null, false); 
          }
        }
        mtxBlkJSX.repaintData();
      }  
    } // end cmpBlkDeviceStreamList_Repaint
    
  // if login state = LOGOUT - clear the status requests
  IpvsLogin.subscribe(com.ipvs.api.Login.ON_LOGINSTATE_CHANGE, BlkJSX, handleDeviceStatusOnLogout);

  function handleDeviceStatusOnLogout(objEvent) {
    if( objEvent.stateVar == com.ipvs.api.Login.LOGINSTATE  && objEvent.stateVal == com.ipvs.api.Login.LOGIN_LOGOUT_SUCCESS) {
                          clearSysmonStatusAgentReqInterval();
                          dialogCellRefreshArr = null;
                          dialogLocalArr = null;
                        }
  }       
} // end dlg status

