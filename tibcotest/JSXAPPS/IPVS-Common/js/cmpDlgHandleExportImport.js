//Common code to handle Export and Imort
function dlgFileXfers_Create(XferTopicName, XferMode) {
  var FileXferTopicName = XferTopicName;
  var FileXferMode = XferMode;
  var DlgFile = "components/FileXferDialog.xml";
  var BlkJSX = PNameSpace.getBodyBlock().load(DlgFile);

  var windowTitle = topic_Get(FileXferTopicName, "XferWindowTitle");
  var winBlkJsxTitle = getJsxByName([BlkJSX,"windowXfersTitle"]);
  winBlkJsxTitle.setText(windowTitle,"true");
  
  // setup a local topic for dlg specific messaging
  var tDlg = "Dialog_" + jsx3.xml.CDF.getKey();
  topic_Create(tDlg);

  // setup a listner for it
  var eventDlg = new EventListner(tDlg);
  
  function dlgFileXfers_Destroy() {
    eventApp.unsubscribe("FILEXFEREVENT");
    // destroy any local eventlistners
    eventDlg.destroy();
    
    // destroy all local components
    cmpDlgFileXfers.destroy();
    
    // destroy the local topic
    topic_Destroy(tDlg);
    
    // close dialog
    BlkJSX.doClose();
  }

  var cmpBtnXferDlgCancel = new CmpButton(["dlgFileXfers","btnCancel"],
                                            null, 
                                            {action:"FNCCALL", 
                                             context:null, 
                                             fnc:function() {
                                               //dlgFileXfers_Destroy();
                                               cmpDlgFileXfers_Abort.call(this);
                                             }
                                            });

  var cmpBtnXferDlgClose = new CmpButton(["dlgFileXfers","btnClose"],
                                           null, 
                                           {action:"FNCCALL", 
                                            context:null, 
                                            fnc:function() {
                                              //dlgFileXfers_Destroy();
                                              cmpDlgFileXfers_Abort.call(this);
                                            }
                                           });

  var cmpDlgFileXfers = new CmpContainer("dlgFileXfers",
                              [ {cmp:cmpBtnXferDlgCancel},
                                {cmp:cmpBtnXferDlgClose}
                              ],
                              null);
                                                            
  cmpDlgFileXfers.BlkJSX = BlkJSX;
  cmpDlgFileXfers.getBlkData =  cmpDlgFileXfers_Init;

  eventApp.subscribe("FILEXFEREVENT", null, this,
                      function(EventVar, EventVal) {
                        var fileXferEvent = EventVal;
                        if( fileXferEvent.Action == "FileXferEvent" ) {
                          cmpDlgFileXfers_HandleEvent.call(this, fileXferEvent);
                        }
                        else if( fileXferEvent.Action == "FileXferStopErrorEvent" || fileXferEvent.Action == "FileXferSetupErrorEvent" ){
                          alert(fileXferEvent.event +"\n Error Coe:" + fileXferEvent.errorCode + "\n Error Description:" + fileXferEvent.errorDescription);
                          cmpDlgFileXfers_HandleError.call(this, fileXferEvent.FileXferID);
                        }
                      } // function end
                   ); 

  jsx3.sleep( (function(){
                 this.init();
                 this.getBlkData();
                 }
               ),
               null, cmpDlgFileXfers );

  function cmpDlgFileXfers_Init() {
    var FileXferCacheNodeIterator = topic_GetNodeIterator(FileXferTopicName, ".//FileXfer");
    var objMtx = PNameSpace.getJSXByName("mtxFileXferList");
    objMtx.resetCacheData();
    while( FileXferCacheNodeIterator.hasNext() ) {
      var FileXferCacheNode = FileXferCacheNodeIterator.next();
      var FileXferID = "Xfer_" + jsx3.xml.CDF.getKey();
      var o = new Object;
      o.jsxid = FileXferID;
      var xferMode = FileXferMode;
      if( xferMode == "Export" ) {
        o.jsxLocalFileName = FileXferCacheNode.selectSingleNode(".//toFileName").getValue();
      }
      else {
        o.jsxLocalFileName = FileXferCacheNode.selectSingleNode(".//fromFileName").getValue();
      }
      o.jsxXferMode = xferMode;
      o.jsxXferCompleted = "0";
      o.jsxXferStatus = "Started";
      objMtx.insertRecord(o, null, false);
      var toAgentJID = FileXferCacheNode.selectSingleNode(".//toAgentJID").getValue();
      cmpDlgFileXfer_StartStopSetup.call(this, FileXferID, FileXferCacheNode, "FileXferSetup", toAgentJID);
      FileXferCacheNode.setAttribute("ID", FileXferID);
      FileXferCacheNode.setAttribute("XferState", "Started");
    }
    objMtx.repaint();
  }

  function cmpDlgFileXfer_StartStopSetup(XferID, XferNode, ObjectType, AgentJID) {
    var FileXferID = XferID;
    var ReqXferDataNode = XferNode;
    var reqSendFileXferAgentObject = new ReqSendFileXferAgentObject(AgentJID, ObjectType, FileXferID, ReqXferDataNode, 
                                                                    {action:"FNCCALL", context:this, 
                                                                       fnc:function(respCode, respVal) {
                                                                             if(respCode == "SUCCESS" && respVal != null) {
                                                                             }
                                                                             else {
                                                                               alert("Error in " + ObjectType);
                                                                             }
                                                                           }
                                                                      } 
                                                                   );
    reqSendFileXferAgentObject.sendRequest(this);
  }

  function cmpDlgFileXfers_HandleEvent(fileXferEvent) {
    var FileXferID = fileXferEvent.FileXferID;
    if( FileXferID != null ) {
      var objMtx = PNameSpace.getJSXByName("mtxFileXferList");
      var FileXferNode = objMtx.getRecord(FileXferID);
      if( FileXferNode != null ) {
        var FileXferStatus = fileXferEvent.eventCode;
        var PercentageComplete = fileXferEvent.PercentageComplete;
        FileXferNode.jsxXferCompleted = PercentageComplete;
        FileXferNode.jsxXferStatus = FileXferStatus;
        objMtx.insertRecord(FileXferNode, null, true);
        if( (FileXferStatus == "Completed") || (FileXferStatus == "Error") ) {
          var FileXferCacheNode = topic_GetNode(FileXferTopicName, ".//*[@ID='" + FileXferID + "']");
          FileXferCacheNode.setAttribute("XferState", FileXferStatus);
          var toAgentJID = FileXferCacheNode.selectSingleNode(".//toAgentJID").getValue();
          cmpDlgFileXfer_StartStopSetup(FileXferID, null, "FileXferStop", toAgentJID);
        }
        else if( FileXferStatus == "Stopped" ) {
          //check for all xfers done and stopped
          var allXfersDone = true;
          var strXMLCacheId = objMtx.getXMLId();
          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
          if (objXML) {
            var objRecordNodes = objXML.selectNodes("//record");
            for (var it = objRecordNodes.iterator();it.hasNext();){
              var objRecordNode = it.next();
              if( objRecordNode.getAttribute("jsxXferStatus") != "Stopped" ) {
                allXfersDone = false;
              }
            }
          }
          //If all xfers done then publish done status to proceed.
          if( allXfersDone ) {
            topic_Publish(FileXferTopicName, "FileXferDone", FileXferMode);
            dlgFileXfers_Destroy();
          }
        }
      }
    }
  }

  function cmpDlgFileXfers_HandleError(fileXferID) {
    var FileXferCacheNode = null;
    if( jsx3.util.strEmpty(fileXferID) ) {
      FileXferCacheNode = topic_GetNode(tXferDlg, ".//*[@XferState!='Completed']");
    }
    else {
      FileXferCacheNode = topic_GetNode(FileXferTopicName, ".//*[@ID='" + fileXferID + "']");
    }
    
    FileXferCacheNode.setAttribute("XferState", "Completed");    
    topic_Publish(FileXferTopicName, "FileXferDone", "ERROR");
    dlgFileXfers_Destroy();      
  }
  
  function cmpDlgFileXfers_Abort() {
    var objMtx = PNameSpace.getJSXByName("mtxFileXferList");
    var strXMLCacheId = objMtx.getXMLId();
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    var allState = true;
    if (objXML) {
      var objRecordNodes = objXML.selectNodes("//record");
      for (var it = objRecordNodes.iterator();it.hasNext();){
        var objRecordNode = it.next();
        if( objRecordNode.getAttribute("jsxXferStatus") != "Stopped" ) {
          allState = false;
          var fileXferID = objRecordNode.getAttribute("jsxid");
          var FileXferCacheNode = topic_GetNode(FileXferTopicName, ".//*[@ID='" + fileXferID + "']");
          FileXferCacheNode.setAttribute("XferState", "Aborted");
          var toAgentJID = FileXferCacheNode.selectSingleNode(".//toAgentJID").getValue();
          cmpDlgFileXfer_StartStopSetup(fileXferID, null, "FileXferStop", toAgentJID);
        }
      }
    }
    // if for some reason notification is not handled then close the dialog
    if(allState){
      cmpDlgFileXfers_HandleError(fileXferID);
    }
  }
}