function cmpBlkSourcePanel_Create() {

  // get vars from object instance
  var winSession = this;
  var winSessionBlkJSX = this.BlkJSX; 
  var tSession = this.tWin;
  var eventSession = this.eventWin;
  var UriRes = null;
  this.GetPortResponse = null;
  this.GetFileResponse = null;
  this.SessionType = null;
  
  //init set up publish value
  topic_Set(tSession, "MediaPlayerSrcPortSelected", "NONE");
  topic_Set(tSession, "MediaPlayerSrcPortGetData", "NONE");
  topic_Set(tSession, "SessionConnSelectFormat", "EXTENDED");
  topic_Set(tSession, "MediaPlayerSrcPortFilter", "NONE");
  topic_Set(tSession, "MediaPlayerSrcTagNameSelected", "NONE");
  topic_Set(tSession, "MediaPlayerSrcTagValueSelection", "NONE");
  topic_Set(tSession, "MediaPlayerSrcPortsTagExpression", "NONE");
                       
  topic_Set(tSession, "MediaPlayerSrcPortEntityId", "NONE");
  topic_Set(tSession, "MediaPlayerSrcPortSelectedName", "NONE");
  topic_Set(tSession, "MediaPlayerSrcPortPresence", "NONE");
  topic_Set(tSession, "MediaPlayerSrcPorttype", "NONE");  
  
  this.cmpSelSrcPortTagName = new CmpSelect([winSessionBlkJSX, "selSourceTagName"],
                                            null,
                                            { persist: { col: "jsxid", initval: false },
                                              action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerSrcTagNameSelected", eventval: "jsxid"
                                            }
                                           );  
  
  this.cmpBtnSrcPortTagValueUnselectAll = new CmpButton([winSessionBlkJSX, "btnUnselectSrcTagValue"],
                                                       null,
                                                       { action: "FNCCALL",
                                                           context: this,
                                                           fnc: function() {
                                                                  var strXMLCacheId = this.cmpBlkSrcPortTagValues.BlkJSX.getXMLId();
                                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                                                                  var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                                                                  var i=0;
                                                                  while(recordIter.hasNext()){
                                                                    var recordNode = recordIter.next();
                                                                    recordNode.removeAttribute("jsxselected");
                                                                  }
                                                                  this.cmpBlkSrcPortTagValues.BlkJSX.repaint();
                                                                  // publish the selected tag name
                                                                  var tagNameSelected = topic_Get(tSession, "MediaPlayerSrcTagNameSelected");
                                                                  topic_Publish(tSession, "MediaPlayerSrcTagNameSelected", tagNameSelected);
                                                           }
                                                       }
                                                    );  
  
  // Media Player Src Ports Tag Value - Button View  
  this.cmpBlkSrcPortTagValues = new CmpButtonPanel([winSessionBlkJSX, "SrcTagValueButtonPanel"],
                                                   null,
                                                   { action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerSrcTagValueSelection", eventval: "jsxid" }
                                                 );
  this.cmpBlkSrcPortTagValues.allowMultiSelection = true;  
  
  this.cmpBtnCloseSrcPortFilter = new CmpButton([winSessionBlkJSX,"btnCloseSrcPortFilter"],
                                                null,
                                                { action: "FNCCALL", context: this,
                                                   fnc: function() {
                                                          // set the filter image on or off
                                                          var strImageOn = "images/buttons/btnFilterOn.png";
                                                          var strImageOff = "images/buttons/btnFilterOff.png";
                                                          var tagNameSelected = topic_Get(tSession, "MediaPlayerSrcTagNameSelected");
                                                          var strXMLCacheId = this.cmpBlkSrcPortTagValues.BlkJSX.getXMLId();
                                                          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                                                          var selrecord = objXML.selectSingleNode(".//record[@jsxselected='1']");
                                                          
                                                          if( (tagNameSelected == "All") && ( jsx3.util.strEmpty(selrecord) ) ){
                                                            this.cmpBtnSrcPortFilter.BlkJSX.setImage(strImageOff);
                                                          }
                                                          else {
                                                            this.cmpBtnSrcPortFilter.BlkJSX.setImage(strImageOn);
                                                          }        
                                                          this.cmpBtnSrcPortFilter.BlkJSX.repaint();
                                                          // close/hide the filter block
                                                          topic_Publish(tSession, "MediaPlayerSrcPortFilter","NONE");
                                                   }
                                                }
                                                ); 
                                            
  this.cmpSrcPortFilter = new Component([winSessionBlkJSX, "blkSrcPortFilterCnt"]);
  
  
  this.cmpBlkMediaPlayerSrcPortFilter = new CmpContainer([winSessionBlkJSX, "blkSrcPortFilterCnt"], 
                                                        [{cmp:this.cmpSrcPortFilter, sel: "topic_Get('"+tSession+"', 'MediaPlayerSrcPortFilter') != 'NONE'"}
                                                        ],
                                                        [{action:"TOPICEVENT", topic:tSession, eventvar: "MediaPlayerSrcPortFilter", eventval: null }],
                                                        null );  
  
  this.cmpBtnSrcPortFilter = new CmpButton([winSessionBlkJSX,"btnSrcPortFilter"],
                                          null,
                                          { action: "FNCCALL", context: null,
                                             fnc: function() {
                                                    topic_Publish(tSession, "MediaPlayerSrcPortFilter","OPEN");
                                             }
                                          }
                                          );  
  
  this.cmpBtnSrcPortExpand = new CmpButton([winSessionBlkJSX,"btnSrcPortExpand"],
                                          null,
                                          { action: "FNCCALL", context: this,
                                             fnc: function() {
                                                    var objBlkState = this.cmpBtnSrcPortExpand.BlkJSX.getState();
                                                    
                                                    var srcPortXML = this.cmpBlkSourcePort.BlkJSX.getXML();
                                                    var recordIter = srcPortXML.selectNodeIterator(".//record/destination");
                                                    while(recordIter.hasNext()){
                                                      var recordNode = recordIter.next();
                                                      recordNode.setAttribute("jsxVisible",objBlkState);
                                                    }
                                                    this.cmpBlkSourcePort.onResponse(srcPortXML);                                                    
                                             }
                                          }
                                          );
                                            
  // Button View
  this.cmpBlkSourcePort = new CmpSessionSrcPanel([winSessionBlkJSX, "BlkMediaPlayerSrcPanel"],
                                                        null,
                                                        { action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerSrcPortSelected", eventval: "jsxid" });
  // source port button panel
  var cmpBlkSrcPortButtonCtn = new CmpContainer([winSessionBlkJSX, "lytSrcPortButtonCtn"],
                                                [{cmp: this.cmpBlkSourcePort},
                                                 {cmp: this.cmpBtnSrcPortFilter},
                                                 {cmp: this.cmpBlkMediaPlayerSrcPortFilter},
                                                 {cmp: this.cmpBlkSrcPortTagValues},
                                                 {cmp: this.cmpBtnSrcPortTagValueUnselectAll},
                                                 {cmp: this.cmpSelSrcPortTagName},
                                                 {cmp: this.cmpBtnCloseSrcPortFilter},
                                                 {cmp: this.cmpBtnSrcPortExpand}
                                                ],
                                                null);
                                        
  //getData whenever "CollaborationState" is published from clsCtrlAgent.notifySessionState
  eventSession.subscribe("CollaborationState", "CONNECTED", this,
                          function(eventVar, eventVal) {
                            //publish to get the session 
                            topic_Publish(tSession, "MediaPlayerSrcPortGetData", (newServerDate()).getTime());
                          });  
  
  eventSession.subscribe("UpdateSession", null, this,
                          function(eventVar, eventVal) {
                            var eventObj = eventVal;
                            var sessionId = topic_Get(tSession,"SessionID");
                            if(sessionId == eventObj.sessionId  ){                          
                              //publish to get the session 
                              topic_Publish(tSession, "MediaPlayerSrcPortGetData", (newServerDate()).getTime());                        
                            } 
                          });    
  
  
  //listen to the EntityPresence. it is published from  Notificationfnc.js                  
  var ENTITYPRESENCEEVENT_SRC_ID = eventApp.subscribe("ENTITYPRESENCEEVENT", null, this,
                                                      function(eventVar, eventVal) {
                                                        var eventObj = eventVal;
                                                        var sessionId = topic_Get(tSession,"SessionID");
                                                        if(sessionId == eventObj.sessionId  ){                          
                                                          //get the src panel cdf doc if the entity is in there chnage the presence image
                                                          var srcPortXML = this.cmpBlkSourcePort.BlkJSX.getXML();
                                                          var selRecord = srcPortXML.selectSingleNode("//record[@jsxEntityid='"+ eventObj.entityId +"']"); 
                                                          if( !jsx3.util.strEmpty(selRecord) ){
                                                            if( eventObj.presence == "available" ){
                                                              selRecord.setAttribute("jsxdevpresence", "JSXAPPS/IPVS_ClientUI/images/icons/icon_greenBall.png");
                                                            }
                                                            else if( eventObj.presence == "unavailable" ){
                                                              selRecord.setAttribute("jsxdevpresence", "JSXAPPS/IPVS_ClientUI/images/icons/icon_grayBall.png");
                                                            }
                                                            else {
                                                              selRecord.setAttribute("jsxdevpresence", "JSXAPPS/IPVS_ClientUI/images/icons/icon_lightgrayBall.png");
                                                            }
                                                            this.cmpBlkSourcePort.onResponse(srcPortXML);
                                                          }                               
                                                        }
                                                      });

  topic_Set(tSession, "ENTITYPRESENCEEVENT_SRC_ID", ENTITYPRESENCEEVENT_SRC_ID);

  eventSession.subscribe("MediaPlayerSrcPortSelected", null, this,
                          function(eventVar, eventVal) {
                            if(eventVal == "NONE"  ){
                              return;
                            }
                            var srcPortNID = eventVal;
                            var selRecord = this.cmpBlkSourcePort.BlkJSX.getXML().selectSingleNode("//record[@jsxid='"+ srcPortNID +"']");
                            var srcPortName = selRecord.getAttribute("jsxtext");
                            var srcType = selRecord.getAttribute("jsxEntityType");
                            var srcEntityId = selRecord.getAttribute("jsxEntityid");
                            var srcEntityPresence = selRecord.getAttribute("jsxpresence");
                            topic_Set(tSession, "MediaPlayerSrcPortSelectedName", srcPortName);
                            topic_Set(tSession, "MediaPlayerSrcPorttype", srcType);
                            topic_Set(tSession, "MediaPlayerSrcPortEntityId", srcEntityId);
                            topic_Set(tSession, "MediaPlayerSrcPortPresence", srcEntityPresence);
                             
                            //publish to send the view or switch cmd
                            topic_Publish(tSession, "MediaPlayerViewSwitchCmd", (newServerDate()).getTime());                            
                          });
                                                                  
  eventSession.subscribe("MediaPlayerViewSwitchCmd", null, this,
                          function(eventVar, eventVal) {                                       
                            if (eventVal != "NONE") {   
                              var connId = null;
                              var actionflag = true;
                              var reqType = null;                              
                                                           
                              // create the mediaInfo Object
                              var mediaInfo = new Object;
                              mediaInfo.srcPortNID = topic_Get(tSession,"MediaPlayerSrcPortSelected");
                              mediaInfo.srcPortName = topic_Get(tSession,"MediaPlayerSrcPortSelectedName");
                              mediaInfo.srcEntityId = topic_Get(tSession,"MediaPlayerSrcPortEntityId");
                              mediaInfo.destPortNID = topic_Get(tSession, "MediaPlayerDstPortSelected");
                              mediaInfo.destPortName = topic_Get(tSession, "MediaPlayerDstPortSelectedName"); 
                              mediaInfo.dstEntityId = topic_Get(tSession, "MediaPlayerDstPortEntityId");
                              mediaInfo.type = topic_Get(tSession,"MediaPlayerSrcPorttype");
                              mediaInfo.connId = "";
                              mediaInfo.sessionId = topic_Get(tSession,"SessionID");
                                                              
                              var nodeSession = agtCollaborationCtrl.getSession(tSession, topic_Get(tSession,"SessionID"));
                              if( jsx3.util.strEmpty(nodeSession) ) {return;}

                              if( mediaInfo.destPortNID == "NONE"){
                                windowAlert(winSessionBlkJSX, "Please select a Source - Destination pair to start connection");
                                return false;
                              }

                              //check if window id is available
                              var windowId = topic_Get(tSession, "MediaPlayerDstPortWindowSelected");
                              //check if canvas id is available
                              var canvasId = topic_Get(tSession, "MediaPlayerDstPortCanvasSelected");
                              // get the maxinstance count
                              var instanceCount = topic_Get(tSession, "MediaPlayerDstPortInstanceCount");
                                                                        
                                                              
                              // pick the request to send - View/SwitchSrc
                              var connNode = null;
                              // for UMC and MC single player
                              if( windowId != "NONE" ){
                                connNode = nodeSession.getConn(mediaInfo.srcEntityId, mediaInfo.dstEntityId, windowId);
                                if( !jsx3.util.strEmpty(connNode) ){
                                }
                                else {
                                  connNode = nodeSession.getConn(null, mediaInfo.dstEntityId, windowId);
                                  if( !jsx3.util.strEmpty(connNode) ){
                                    connId = nodeSession.getConnId(connNode);
                                    // using View for switch also Bug 37836
                                    //reqType = "Switch";
                                    reqType = "View";
                                  }
                                  else  {
                                    reqType = "View";
                                  }                                  
                                }
                              }
                              else {
                                connNode = nodeSession.getConn(mediaInfo.srcEntityId, mediaInfo.dstEntityId, null);
                                if( !jsx3.util.strEmpty(connNode) ){
                                }
                                else {
                                  reqType = "View";
                                }                             
                              }
                              
                              
//                              if( (!jsx3.util.strEmpty(connNode)) && (actionflag) ){
//                                connId = nodeSession.getConnId(connNode);
//                                reqType = "Switch";
//                              }
//                              else if(actionflag) {
//                                reqType = "View";
//                              } 
                              
                                                           
                              //s et the connection id
                              if( connId == null ){
                                connId = "Conn_" + getUUID();
                              }
                              
                              // update the mediaInfo Object                               
                              mediaInfo.connId = connId;

                              // create the view object
                              var objView = new Object;
                              objView.reqType = reqType;    
                              objView.callBackSuccess = this.viewOrPlayBackSuccessFnc1;
                              objView.callBackErr = this.viewOrPlayBackFailFnc1;                                  
                              if( (reqType == "View") ) {
                                if( mediaInfo.type == "PlyFile" ) {
                                  objView.reqType = "PlayBackSrc";
                                  objView.FileNID = "";
                                  objView.SrcPortName = "";
                                  objView.SrcEntityId = mediaInfo.srcEntityId;                                  
                                }
                                else {
                                  objView.SrcPortNID = "";
                                  objView.SrcPortName = mediaInfo.srcPortName;
                                  objView.SrcEntityId = mediaInfo.srcEntityId;
                                }
                                objView.WindowId = "";
                                if( windowId != "NONE"){
                                  objView.WindowId = windowId;
                                }
                                objView.CanvasId = "";
                                if( !jsx3.util.strEmpty(canvasId) && canvasId != "NONE"){
                                  objView.CanvasId = canvasId;
                                }  
                                else{
                                  //Used to indicate that a connection exist for a specific window(Added as connId by the unmanned client)Later to be fixed when SScommand send sessionId,connId.
                                  objView.CanvasId = "DstPort";
                                }                               
                                objView.DstPortNID = "";
                                objView.DstportName = mediaInfo.destPortName;
                                objView.DstEntityId = mediaInfo.dstEntityId;
                                objView.ConnName = mediaInfo.srcPortName;
                                objView.ConnId = mediaInfo.connId;
                                objView.mediaInfo = mediaInfo;
                                objView.SessionId = mediaInfo.sessionId;
                                objView.restartSession = "";     
                                agtCollaborationCtrl.viewOrPlayBack.call(this,tSession,objView);                           
                              }
                              else if( reqType == "Switch" ){
                                objView.SessionID = mediaInfo.sessionId;
                                objView.ConnId = mediaInfo.connId;
                                objView.EntityId = mediaInfo.srcEntityId;

                                agtCollaborationCtrl.switchSrc.call(this,tSession,objView);
                              }
                              else if (jsx3.util.strEmpty(reqType)) {
                                //connection exist for the src deselect the src
                                topic_Publish(tSession, "MediaPlayerSrcPortDeSelectAll", (newServerDate()).getTime());
                              }
                            } // if eventvale != NONE
                          }
                      );                      

  this.viewOrPlayBackSuccessFnc1 = function(Response,mediaInfo,reqType){
                                      log.info(" cmpBlkSourcePanel:: viewOrPlayBackSuccessFnc1 :: " + reqType + " Command request succeeded as : " + Response);
                                      var objCtrl = agtCollaborationCtrl.arrCtrlObj[tSession];
                                      objCtrl.nodeSession = Response ? Response.cloneNode(true) : null;
                                      //in sesseion cntrl agent we are already processing conn state changes
                                      //To fix bug#44240, #44241 and #44383
//                                      var nodeSession = new com.ipvs.api.Session.Document(objCtrl.nodeSession);
//                                      var sessionType = nodeSession.getSessionTypeAttribute();
//                                      if(sessionType == com.ipvs.api.SessionInfo.SESSIONTYPE_PLAYBACK && nodeSession.isDestTrickPlayAvailable()){
//                                        if(nodeSession.isSrcFFTrackAvailable()){
//                                          if(objCtrl.lastAction != "Pause"){
//                                           agtCollaborationCtrl.notifySessionState(tSession);
//                                          }
//                                          else{
//                                            topic_Set(tSession, "CollaborationFFTrackState", "ALL");
//                                            topic_Publish(tSession, "ControllerState", objCtrl.lastAction);
//                                          }
//                                        }
//                                      }
                                   };
                             
  this.viewOrPlayBackFailFnc1 = function(Response,mediaInfo,reqType){  
                                  log.error(" cmpBlkSourcePanel:: viewOrPlayBackFailFnc1 :: "+ reqType +" Command request failed as : " + Response);
                                };
                                              
  eventSession.subscribe("MediaPlayerSrcPortGetData", null, this,
                          function(eventVar, eventVal) {                                       
                            if (eventVal != "NONE") { 
                              var nodeSession = agtCollaborationCtrl.getSession(tSession, topic_Get(tSession,"SessionID"));
                              if( jsx3.util.strEmpty(nodeSession) ) {return;}
                              
                              //fill data to sessionInfo panel
                              this.getData(nodeSession);
                              // get port so that we can fill the filter section
                              var tagNameSelected = topic_Get(tSession, "MediaPlayerSrcTagNameSelected");
                              if( tagNameSelected == "NONE" ) {
                                var portSession = nodeSession.getEntityByType("SrcPort");
                                var playbackSession = nodeSession.getEntityByType("PlyFile");
                                if( !jsx3.util.strEmpty(portSession) ){
                                  this.getSrcPort(nodeSession);
                                }
                                if( !jsx3.util.strEmpty(playbackSession) ){
                                  this.getSrcFile(nodeSession);
                                }
                              }                                                                                        
                            }
                          }
                      ); 

  eventSession.subscribe("DeleteConnection", null, this,
                          function(eventVar, eventVal) { 
                            var connectionId =  eventVal;
                            var sessionId = topic_Get(tSession, "SessionID");
                            agtCollaborationCtrl.DeleteConn(tSession, sessionId, connectionId);
                          }
                       );

  eventSession.subscribe("MediaPlayerSrcTagNameSelected", null, this,
                          function (eventVar, eventVal) {
                            if (eventVal != "NONE") {
                              // get the selected value
                              var nameRecord = this.cmpSelSrcPortTagName.BlkJSX.getValue();
                              //check if SrcTagNameSelected is "All" and SrcTagValueSelection is empty then show all Src list
                              var strXMLCacheId = this.cmpBlkSrcPortTagValues.BlkJSX.getXMLId();
                              var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                              var recSelSrcPortValue = objXML.selectSingleNode(".//record[@jsxselected='1']");

                              //get the complete record
                              var selNameRecord = this.cmpSelSrcPortTagName.BlkJSX.getRecordNode(nameRecord);
                              // get the Tag node which contains the TagValues
                              var tagNode = selNameRecord.selectSingleNode(".//Tag");
                              //get the xsl
                              if (jsx3.util.strEmpty(UriRes)) {
                                UriRes = this.cmpSelSrcPortTagName.BlkJSX.getUriResolver();
                                if (jsx3.util.strEmpty(UriRes)) return;
                              }
                              xslUriRes = UriRes.resolveURI("xsl/TagValueList.xsl");
                              if (jsx3.util.strEmpty(xslUriRes)) return;

                              var XSLFile = PNameSpace.loadInclude(xslUriRes, "TagValueList", "xsl");
                              if (XSLFile) {
                                var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
                                var docXformResp = XSLDoc.transformToObject(tagNode);
                              }

                              // set/send the cdf doc to the panel
                              this.cmpBlkSrcPortTagValues.onResponse(docXformResp);
                              var tagExpression = "";
                              if (nameRecord != "All" || !jsx3.util.strEmpty(recSelSrcPortValue)) {
                                // get the Tag Name and Values for the expression
                                var recordIter = tagNode.selectNodeIterator(".//Value");
                                var i = 0;
                                while (recordIter.hasNext()) {
                                  var recordNode = recordIter.next();
                                  var selectedTagName = recordNode.getAttribute("jsxTagName");
                                  var selectedTagValue = recordNode.getValue();
                                  var newTagExpression = "(@Name='" + selectedTagName + "' and @Value='" + selectedTagValue + "')";
                                  if (i == 0) {
                                    tagExpression = newTagExpression;
                                    i++;
                                  } else {
                                    tagExpression = tagExpression + " or " + newTagExpression;
                                  }
                                }
                              }
                              log.info("cmpBlkSessionInfo:sessionInfo MediaPlayerSrcTagNameSelected.MediaPlayerSrcPortsTagExpression::" + tagExpression + "\n");
                              topic_Set(tSession, "MediaPlayerSrcPortsTagExpression", tagExpression);
                              topic_Publish(tSession, "UpdateMediaPlayerSrcPortList", (newServerDate()).getTime());
                            }
                          }
                        );

  // On selecting Source tag Value
  eventSession.subscribe("MediaPlayerSrcTagValueSelection", null, this,
                          function(eventVar, eventVal) {
                            var newTagExpression = "";
                            var strXMLCacheId = this.cmpBlkSrcPortTagValues.BlkJSX.getXMLId();
                            var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                            var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                            var i=0;
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              var selectedTagName = recordNode.getAttribute("jsxTagName");
                              var selectedTagValue = recordNode.getAttribute("jsxtext");
                              var newTagExpression = "(@Name='" + selectedTagName + "' and @Value='" + selectedTagValue + "')";
                              if (i == 0) {
                                  var tagExpression = newTagExpression;
                                  i++;
                              } else {
                                  tagExpression = tagExpression + " or " + newTagExpression;
                              }                                  
                            }
                            if( jsx3.util.strEmpty(tagExpression) ){
                              var tagNameSelected = topic_Get(tSession, "MediaPlayerSrcTagNameSelected");
                              topic_Publish(tSession, "MediaPlayerSrcTagNameSelected", tagNameSelected);                           
                            }
                            else {
                              log.info("cmpBlkSessionInfo:sessionInfo MediaPlayerSrcTagNameSelected.MediaPlayerSrcTagValueSelection::" + tagExpression + "\n");         
                              topic_Set(tSession, "MediaPlayerSrcPortsTagExpression", tagExpression);
                              topic_Publish(tSession, "UpdateMediaPlayerSrcPortList", (newServerDate()).getTime());
                            }
                          }
                       );
                           
  // On selecting Source tag Value
  eventSession.subscribe("UpdateMediaPlayerSrcPortList", null, this,
                          function(eventVar, eventVal) {
                            var tagExpression = topic_Get(tSession, "MediaPlayerSrcPortsTagExpression");
                            var srcPortXML = this.cmpBlkSourcePort.BlkJSX.getXML();
                            var recordIter = srcPortXML.selectNodeIterator(".//record");
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              recordNode.setAttribute("jsxVisible","0");
                            }
                            //Handling if No tags are present at all (If No tag then show all Sources)
                            if(!jsx3.util.strEmpty(tagExpression) && tagExpression != "NONE"){                                                                                              
                              var showRecordIter = srcPortXML.selectNodeIterator(".//record[Groups/Tag["+ tagExpression +"]]");
                            }
                            else{
                              var showRecordIter = srcPortXML.selectNodeIterator(".//record");
                            }
                            while(showRecordIter.hasNext()){
                              var recordNode = showRecordIter.next();
                              recordNode.setAttribute("jsxVisible","1");
                            }
                            
                            // check the expand/collapse button state, update the destinations
                            var objBlkState = this.cmpBtnSrcPortExpand.BlkJSX.getState();
                            var srcPortXML = this.cmpBlkSourcePort.BlkJSX.getXML();
                            var recordIter = srcPortXML.selectNodeIterator(".//record/destination");
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              recordNode.setAttribute("jsxVisible",objBlkState);
                            }
                            log.info("cmpBlkSessionInfo:sessionInfo UpdateMediaPlayerSrcPortList.INSERTING XML::" + srcPortXML + "\n");                                                    
                            this.cmpBlkSourcePort.onResponse(srcPortXML);
                            var srcPortXML1 = this.cmpBlkSourcePort.BlkJSX.getXML();
                            log.info("cmpBlkSessionInfo:sessionInfo UpdateMediaPlayerSrcPortList.CDF::" + srcPortXML1 + "\n");                     
                          }
                       );
  // On selecting MediaPlayerDestPort or window deselect srcPort
  eventSession.subscribe("MediaPlayerSrcPortDeSelectAll", null, this,
                          function(eventVar, eventVal) {
                            var updateData = false;
                            var srcPortXML = this.cmpBlkSourcePort.BlkJSX.getXML();
                            var recordIter = srcPortXML.selectNodeIterator(".//record[@jsxselected='1']");
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              recordNode.setAttribute("jsxselected","0");
                              updateData = true;
                            }
                            if(updateData){
                              this.cmpBlkSourcePort.onResponse(srcPortXML);                            
                            }
                          }
                       );
                                              
                                                                                               
  // -- METHOD to parse collabconfig XML and convert to list objects for GUI components
  this.getData = function(nodeSession) {
    //using SrcPortsPanel.xsl to transfer source Entity in nodeSession.xml to CDF
    var xslUriRes = null;
    if( jsx3.util.strEmpty(UriRes)) {
      UriRes = this.cmpSelSrcPortTagName.BlkJSX.getUriResolver();
      if( jsx3.util.strEmpty(UriRes) ) return;                                
    }
    var sessionType = nodeSession.getAttribute("type");
    if( jsx3.util.strEmpty(this.SessionType) ){
      this.SessionType = sessionType;
    }
    
    xslUriRes = UriRes.resolveURI("xsl/MediaPlayerSrcPanel.xsl");
    if (jsx3.util.strEmpty(xslUriRes)) return;

    var XSLFile = PNameSpace.loadInclude(xslUriRes, "MediaPlayerSrcPanel", "xsl");
    var docXformResp = null;
    if (XSLFile) {
      var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
      docXformResp = XSLDoc.transformToObject(nodeSession);
    }
//    // find if any record is selected
//    var selRecord = this.cmpBlkSourcePort.BlkJSX.getXML().selectSingleNode("//record[@jsxselected='1']");
//    if(!jsx3.util.strEmpty(selRecord) ){
//      var selRecordId = selRecord.getAttribute("jsxid");
//      //get the record with id and set the record selected
//      var newselRecord = docXformResp.selectSingleNode(".//record[@jsxid='"+ selRecordId +"']");
//      newselRecord.setAttribute("jsxselected","1");
//    }
    
    var recordIter = this.cmpBlkSourcePort.BlkJSX.getXML().selectNodeIterator(".//record[@jsxselected='1']");
    while (recordIter.hasNext()) {
      var recordNode = recordIter.next();
      recordNode.setAttribute("jsxselected", "0");
    }

    var tagNameSelected = topic_Get(tSession, "MediaPlayerSrcTagNameSelected");
    if( tagNameSelected != "NONE") {
      // update the src port data because of notification
      // get the tags from the old data and put it in the new data
      var oldsrcPortRecordIter = this.cmpBlkSourcePort.BlkJSX.getXML().selectNodeIterator(".//record");
      while( oldsrcPortRecordIter.hasNext() ) {
        var oldselRecord = oldsrcPortRecordIter.next();                                                      
        var oldrecordID = oldselRecord.getAttribute("jsxid");
        var datadRecordGroupNode = oldselRecord.selectSingleNode(".//Groups");
        //For multiple playback files having same source but belonging to different GrpFiles iteration required.
        var newrecordGroupNodeIter = docXformResp.selectNodeIterator(".//record[@jsxid='"+ oldrecordID +"']/Groups");     
        while( newrecordGroupNodeIter.hasNext() ){
          var newrecordGroupParentNode = newrecordGroupNodeIter.next().getParent();
          var emptyGroupNode = newrecordGroupParentNode.selectSingleNode(".//Groups");
          var newdatadRecordGroupNode = datadRecordGroupNode.cloneNode(true);
          newrecordGroupParentNode.replaceNode(newdatadRecordGroupNode, emptyGroupNode );
        }
      }    
      // put the new data
      this.cmpBlkSourcePort.setBlkJSXSourceXML(docXformResp);      
      topic_Publish(tSession, "UpdateMediaPlayerSrcPortList", (newServerDate()).getTime());    
    }
    else {
      this.cmpBlkSourcePort.onResponse(docXformResp);
    } 
        
  } // end this.getData
  
  // fnc to get the src port data
  this.getSrcPort = function(nodeSession) {
    //using SrcPortsPanel.xsl to transfer source Entity in nodeSession.xml to CDF
    var xslUriRes = null;
    if( jsx3.util.strEmpty(UriRes)) {
      UriRes = this.cmpSelSrcPortTagName.BlkJSX.getUriResolver();
      if( jsx3.util.strEmpty(UriRes) ) return;                                
    }
    xslUriRes = UriRes.resolveURI("xsl/SessionEntitySrcPortNIDList.xsl");
    if (jsx3.util.strEmpty(xslUriRes)) return;

    var XSLFile = PNameSpace.loadInclude(xslUriRes, "SessionEntitySrcPortNIDList", "xsl");
    var srcNIDListNode = null;
    if (XSLFile) {
      var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
      srcNIDListNode = XSLDoc.transformToObject(nodeSession);
      var srcNIDList = srcNIDListNode.getValue();
    }
    
    // send the getPort Request
    var reqGetPort = new ReqSessionGetRequest("GetPort",
                                                { action: "FNCCALL", context: this,
                                                  fnc: function(respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      this.GetPortResponse = respVal;
                                                      this.appendGroups();                                                   
                                                    }
                                                    else {
                                                      
                                                    }
                                                  }
                                                }
                                               );
                                               
    reqGetPort.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = srcNIDList;
    reqParams["ObjectList_WhereTextSubStr"] = "";
    reqParams["ObjectList_SelectFormat"] = "BASICCONNECTIONINFO";
    reqParams["ObjectList_Type"] = "SrcPort";
    reqParams["ObjectList_SubscriptionID"] = "";
    reqParams["ObjectList_WherePermissions"] = "VIEWFROM";
    reqGetPort.reqParams = reqParams;
    // send the request
    reqGetPort.sendRequest(this);   
  
  } // end this.getSrcPort


  // fnc to get the src File data
  this.getSrcFile = function(nodeSession) {
    //using SrcPortsPanel.xsl to transfer source Entity in nodeSession.xml to CDF
    var xslUriRes = null;
    if( jsx3.util.strEmpty(UriRes)) {
      UriRes = this.cmpSelSrcPortTagName.BlkJSX.getUriResolver();
      if( jsx3.util.strEmpty(UriRes) ) return;                                
    }
    xslUriRes = UriRes.resolveURI("xsl/SessionEntitySrcFileNIDList.xsl");
    if (jsx3.util.strEmpty(xslUriRes)) return;

    var XSLFile = PNameSpace.loadInclude(xslUriRes, "SessionEntitySrcFileNIDList", "xsl");
    var srcNIDListNode = null;
    if (XSLFile) {
      var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
      srcNIDListNode = XSLDoc.transformToObject(nodeSession);
      var srcNIDList = srcNIDListNode.getValue();
    }
    
    // send the getFile Request
    var reqGetFile = new ReqSessionGetRequest("GetFile",
                                                { action: "FNCCALL", context: this,
                                                  fnc: function(respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      this.GetFileResponse = respVal;
                                                      this.appendGroups();
                                                    }
                                                    else {
                                                      
                                                    }
                                                  }
                                                }
                                               );
                                               
    reqGetFile.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = srcNIDList;
    reqParams["ObjectList_WhereTextSubStr"] = "";
    reqParams["ObjectList_SelectFormat"] = "EXTENDED";
    reqParams["ObjectList_Type"] = "";
    reqParams["ObjectList_SubscriptionID"] = "";
    reqParams["ObjectList_WherePermissions"] = "PLAYBACK";
    reqGetFile.reqParams = reqParams;
    // send the request
    reqGetFile.sendRequest(this);   
  
  } // end this.getSrcFile
  
  //function to append the Groups to the src cdf
  this.appendGroups = function () {
    log.info("cmpBlkSessionInfo:sessionInfo appendGroups.SessionType::" + this.SessionType + "\n");
    var response = null;
    if(this.SessionType == "MIXED"){
      if( jsx3.util.strEmpty(this.GetPortResponse) || jsx3.util.strEmpty(this.GetFileResponse) ){
        return;
      }
      var newDoc = new jsx3.xml.Document();
      newDoc.loadXML('<data></data>');
      newDoc.appendChild(this.GetFileResponse.getRootNode().cloneNode(true));
      newDoc.appendChild(this.GetPortResponse.getRootNode().cloneNode(true));
      response = newDoc;
    }
    else if(this.SessionType == "PLAYBACK") {
      response = this.GetFileResponse;
      
    }
    else {
      response = this.GetPortResponse;
    }
    
    if( jsx3.util.strEmpty(UriRes)) {
      UriRes = this.cmpSelSrcPortTagName.BlkJSX.getUriResolver();
      if( jsx3.util.strEmpty(UriRes) ) return;                                
    }
    
    var nodeName = response.getNodeName();  
    //add the Groups node from the response to the src panel cdf
    var srcPortRecordIter = this.cmpBlkSourcePort.BlkJSX.getXML().selectNodeIterator(".//record");
    while( srcPortRecordIter.hasNext() ) {
      var selRecord = srcPortRecordIter.next();                                                      
      var recordID = selRecord.getAttribute("jsxid");
      var recordGroupParentNode = selRecord.selectSingleNode(".//Groups").getParent();
      var RecordGroupNode = selRecord.selectSingleNode(".//Groups");
      var portNode = response.selectSingleNode(".//*[@NID='"+ recordID +"']");
      if( !jsx3.util.strEmpty(portNode) ){
        var portGroupNode = response.selectSingleNode(".//*[@NID='"+ recordID +"']/Groups").cloneNode(true);
        recordGroupParentNode.replaceNode(portGroupNode, RecordGroupNode);
      }
    }


    // fill the Tag Name drop down
    var xslExtractTagNameRes = null;
    xslExtractTagNameRes = UriRes.resolveURI("xsl/TagsFromGetFileORGetPort.xsl");
    
    if (jsx3.util.strEmpty(xslExtractTagNameRes)) return;
    var xslTagNameRes = UriRes.resolveURI("xsl/TagNameListFromGetPort.xsl");
    if (jsx3.util.strEmpty(xslTagNameRes)) return;

    var TagsXSLFile = PNameSpace.loadInclude(xslExtractTagNameRes, "xslExtractTagNameRes", "xsl");
    var TagsData = null;
    if(TagsXSLFile){
      var XSLDoc = jsx3.xml.XslDocument.wrap(TagsXSLFile);
      TagsData = XSLDoc.transformToObject(response);    
    }

    var TagNameXSLFile = PNameSpace.loadInclude(xslTagNameRes, "xslTagNameRes", "xsl");
    var TagNameDoc = null;
    if(TagsXSLFile){
      var XSLDoc = jsx3.xml.XslDocument.wrap(TagNameXSLFile);
      TagNameDoc = XSLDoc.transformToObject(TagsData);    
    }
    var strXMLCacheId = this.cmpSelSrcPortTagName.BlkJSX.getXMLId(); 
    var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
    objXML.loadXML(TagNameDoc.toString()); 
    this.cmpSelSrcPortTagName.BlkJSX.setSourceXML(objXML); 
    var tagNameSelected = topic_Get(tSession, "MediaPlayerSrcTagNameSelected");
    if( tagNameSelected == "NONE"){
      tagNameSelected = "All";
    }
    log.info("cmpBlkSessionInfo:sessionInfo appendGroups.cmpSelSrcPortTagName::" + TagNameDoc + "\n");
    this.cmpSelSrcPortTagName.BlkJSX.setValue(tagNameSelected);
    topic_Publish(tSession, "MediaPlayerSrcTagNameSelected", tagNameSelected);   

    //var srcPortXML = this.cmpBlkSourcePort.BlkJSX.getXML();    
  } // end this.appendGroups
  
  //separate cmpBlkSessionInfo from cmpBlkMediaPlayer and cmpBlkCollaboration
  jsx3.sleep((function() {
                          this.init();
                          this.show();
                          //push this component to parent window's SubCmpArr
                          winSession.SubCmpArr.push({ cmp: cmpBlkSrcPortButtonCtn });
                          topic_Publish(tSession, "SessionInfoLoadingStatus", "LOADED");
                          log.info("cmpBlkSessionInfo:sessionInfo panel is loaded.");
                         }
               ), null, cmpBlkSrcPortButtonCtn);
}


