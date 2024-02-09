function cmpBlkMediaPlayerDestPanel_Create() {

  // get vars from object instance
  var winSession = this;
  var winSessionBlkJSX = this.BlkJSX; 
  var tSession = this.tWin;
  var eventSession = this.eventWin;
  var winSessionWindow = winSessionBlkJSX._jsxwindow;
  //init set up publish value  

  topic_Set(tSession, "MediaPlayerDstCanvasCnt", "NONE");
  
  var delayDstPortDisplayStatusNotification = new Object;
  delayDstPortDisplayStatusNotification.timer = null;
  delayDstPortDisplayStatusNotification.count = 0;  
  
  
  topic_Set(tSession, "MediaPlayerDstPortFilter", "NONE");
  topic_Set(tSession, "MediaPlayerDstTagNameSelected", "NONE");
  topic_Set(tSession, "MediaPlayerDstTagValueSelection", "NONE");
  topic_Set(tSession, "MediaPlayerDstPortsTagExpression", "NONE");
  topic_Set(tSession, "OpenCanvasMask", "NONE");
  topic_Set(tSession, "ShowOfflineText", "NONE");
  topic_Set(tSession, "ShowUpdateText", "NONE");
  topic_Set(tSession, 'PortDisplayStatusSubscriptionID', "PlayerDstPortDisplayStatus_Subscription_" + getUUID());
  topic_Set(tSession, 'PortDisplayConfigSubscriptionID', "PlayerDstPortDisplayConfig_Subscription_" + getUUID());
  
  this.cmpSelDstPortTagName = new CmpSelect([winSessionBlkJSX, "selDstTagName"],
                                            null,
                                            { persist: { col: "jsxid", initval: false },
                                              action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerDstTagNameSelected", eventval: "jsxid"
                                            }
                                           );  
  
  this.cmpBtnDstPortTagValueUnselectAll = new CmpButton([winSessionBlkJSX, "btnUnselectDstTagValue"],
                                                       null,
                                                       { action: "FNCCALL",
                                                           context: this,
                                                           fnc: function() {
                                                                  var strXMLCacheId = this.cmpBlkDstPortTagValues.BlkJSX.getXMLId();
                                                                  var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                                                                  var recordIter = objXML.selectNodeIterator(".//record[@jsxselected='1']");
                                                                  var i=0;
                                                                  while(recordIter.hasNext()){
                                                                    var recordNode = recordIter.next();
                                                                    recordNode.removeAttribute("jsxselected");
                                                                  }
                                                                  this.cmpBlkDstPortTagValues.BlkJSX.repaint();
                                                                  // publish the selected tag name
                                                                  var tagNameSelected = topic_Get(tSession, "MediaPlayerDstTagNameSelected");
                                                                  topic_Publish(tSession, "MediaPlayerDstTagNameSelected", tagNameSelected);
                                                           }
                                                       }
                                                    );  
  
  // Media Player Dst Ports Tag Value - Button View  
  this.cmpBlkDstPortTagValues = new CmpButtonPanel([winSessionBlkJSX, "DstTagValueButtonPanel"],
                                                   null,
                                                   { action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerDstTagValueSelection", eventval: "jsxid" }
                                                 );
  this.cmpBlkDstPortTagValues.allowMultiSelection = true;  
  
  this.cmpBtnCloseDstPortFilter = new CmpButton([winSessionBlkJSX,"btnCloseDstPortFilter"],
                                                null,
                                                { action: "FNCCALL", context: this,
                                                   fnc: function() {
                                                          // set the filter image on or off
                                                          var strImageOn = "images/buttons/btnFilterOn.png";
                                                          var strImageOff = "images/buttons/btnFilterOff.png";
                                                          var tagNameSelected = topic_Get(tSession, "MediaPlayerDstTagNameSelected");
                                                          var strXMLCacheId = this.cmpBlkDstPortTagValues.BlkJSX.getXMLId();
                                                          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);   
                                                          var selrecord = objXML.selectSingleNode(".//record[@jsxselected='1']");
                                                          
                                                          if( (tagNameSelected == "All") && ( jsx3.util.strEmpty(selrecord) ) ){
                                                            this.cmpBtnDstPortFilter.BlkJSX.setImage(strImageOff);
                                                          }
                                                          else {
                                                            this.cmpBtnDstPortFilter.BlkJSX.setImage(strImageOn);
                                                          }        
                                                          this.cmpBtnDstPortFilter.BlkJSX.repaint();
                                                          // close/hide the filter block
                                                          topic_Publish(tSession, "MediaPlayerDstPortFilter","NONE");
                                                   }
                                                }
                                                ); 
                                            
  this.cmpDstPortFilter = new Component([winSessionBlkJSX, "blkDstPortFilterCnt"]);
  
  
  this.cmpBlkMediaPlayerDstPortFilter = new CmpContainer([winSessionBlkJSX, "blkDstPortFilterCnt"], 
                                                        [{cmp:this.cmpDstPortFilter, sel: "topic_Get('"+tSession+"', 'MediaPlayerDstPortFilter') != 'NONE'"}
                                                        ],
                                                        [{action:"TOPICEVENT", topic:tSession, eventvar: "MediaPlayerDstPortFilter", eventval: null }],
                                                        null );  
  
  this.cmpBtnDstPortFilter = new CmpButton([winSessionBlkJSX,"btnDstPortFilter"],
                                          null,
                                          { action: "FNCCALL", context: null,
                                             fnc: function() {
                                                    topic_Publish(tSession, "MediaPlayerDstPortFilter","OPEN");
                                             }
                                          }
                                          );  
  
  // Button View
  this.cmpBlkDstPortPanel = new CmpButtonPanel([winSessionBlkJSX, "BlkMediaPlayerDstPanel"],
                                                null,
                                                { action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerDstPortSelected", eventval: "jsxid" });
  
  this.cmpBlkDstPortCanvasMesg = new Component([winSessionBlkJSX,"blkMediaPlayerDestMesg"]);
  
  this.cmpBtnEditCanvasApply = new CmpButton([winSessionBlkJSX, "btnApplyCanvasEdit"],
                                             null,
                                             { action: "FNCCALL",
                                               context: this,
                                               fnc: function() {
                                                 // close window 
                                                 this.setMediaPlayerDstPortCanvasConfig();
                                               }
                                             });

  this.cmpBlkCanvasMask = new Component([winSessionBlkJSX, "blkPlayerCanvasMask"]);

  this.cmpBlkFooter = new CmpContainer([winSessionBlkJSX, "blkFooter"],
                                            [{ cmp: this.cmpBlkCanvasMask,
                                              sel: "topic_Get('" + tSession + "', 'OpenCanvasMask') != 'NONE'"
                                            }],
                                            [{ action: "TOPICEVENT", topic: tSession, eventvar: "OpenCanvasMask", eventval: null}],
                                            null);

  this.cmptxtCanvasOfflineMask = new Component([winSessionBlkJSX, "txtPlayerCanvasOfflineMask"]);

  this.cmptxtCanvasUpdatingMask = new Component([winSessionBlkJSX, "txtPlayerCanvasUpdatingMask"]);

  this.cmpBlkFooterTxt = new CmpContainer([winSessionBlkJSX, "blkFooter"],
                                            [{ cmp: this.cmptxtCanvasOfflineMask,
                                              sel: "topic_Get('" + tSession + "', 'ShowOfflineText') != 'NONE'"
                                            },
                                            { cmp: this.cmptxtCanvasUpdatingMask,
                                              sel: "topic_Get('" + tSession + "', 'ShowUpdateText') != 'NONE'"
                                            }],
                                            [{ action: "TOPICEVENT", topic: tSession, eventvar: "ShowOfflineText", eventval: null},
                                            { action: "TOPICEVENT", topic: tSession, eventvar: "ShowUpdateText", eventval: null}],
                                            null);                                             
                                                                                                                                    
  this.cmpBlkDstPortCanvasSetting = new CmpCanvasEditorContainer([winSessionBlkJSX,"blkMediaPlayerDestCanvas"],
                                                                  [[winSessionBlkJSX,"BlkDisplayEditCanvas"],
                                                                   [winSessionBlkJSX,"masterWindowCDF"],
                                                                   [winSessionBlkJSX,"WindowList"],
                                                                   [winSessionBlkJSX,"txtWindowId"],
                                                                   [winSessionBlkJSX,"txtWindowLeft"],
                                                                   [winSessionBlkJSX,"txtWindowTop"],
                                                                   [winSessionBlkJSX,"txtWindowWidth"],
                                                                   [winSessionBlkJSX,"txtWindowHeight"],
                                                                   [winSessionBlkJSX,"txtWindowTransparency"],
                                                                   [winSessionBlkJSX,"txtWindowZIndex"],
                                                                   [winSessionBlkJSX,"btnAddCanvasWindow"],
                                                                   [winSessionBlkJSX,"txtDisplayWidth"],
                                                                   [winSessionBlkJSX,"txtDisplayHeight"],
                                                                   [winSessionBlkJSX,"chkWindowBorder"],
                                                                   [winSessionBlkJSX,"chkWindowHeader"],
                                                                   [winSessionBlkJSX,"blkWindowFullScreen"],
                                                                   [winSessionBlkJSX,"blkFullScreenCtr"],
                                                                   [winSessionBlkJSX,"blkCanvasCtr"],
                                                                   [winSessionBlkJSX,"blkCanvasContainer"],
                                                                   [winSessionBlkJSX,"selCanvasLayout"],
                                                                   [winSessionBlkJSX,"chkWindow1:1Aspect"],
                                                                   [winSessionBlkJSX,"btnScrollBar"],
                                                                   [winSessionBlkJSX,"btnBack"],
                                                                   [winSessionBlkJSX,"blkPropertiesCtr"],
                                                                   [winSessionBlkJSX,"blkScrollCtr"],
                                                                   [winSessionBlkJSX,"blkWindow"],
                                                                   [winSessionBlkJSX,"blkVideo"],
                                                                   [winSessionBlkJSX,"blkProperties"],
                                                                   [winSessionBlkJSX,"txtScrollTop"],
                                                                   [winSessionBlkJSX,"txtScrollLeft"]
                                                                  ],
                                                                  null,
                                                                  { action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerDstPortWindowSelected", eventval: "id" },
                                                                  winSessionBlkJSX,
                                                                  "ClientEditCanavsStatus"
                                                                  );

  //get the xsl
  var xslUriRes = null;
  var UriRes = this.cmpBlkDstPortCanvasSetting.BlkJSX.getUriResolver();
  var PCWindowingxslUriRes = UriRes.resolveURI("xsl/PcWindowingCanvas.xsl");
  var PCWindowingXSLFile = PNameSpace.loadInclude(PCWindowingxslUriRes, "_Update", "xsl");  

  this.cmpBlkDstCanvas = new CmpContainer([winSessionBlkJSX, "blkCanvasEditorCnt"],
                                          [{cmp: this.cmpBlkDstPortCanvasSetting,
                                            sel: "topic_Get('" + tSession + "', 'MediaPlayerDstCanvasCnt') == 'CANVAS'" },
                                           {cmp: this.cmpBlkDstPortCanvasMesg,
                                            sel: "topic_Get('" + tSession + "', 'MediaPlayerDstCanvasCnt') == 'MESSAGE'" }
                                          ],
                                          [{ action: "TOPICEVENT", topic: tSession, eventvar: "MediaPlayerDstCanvasCnt", eventval: null }],
                                          null);
  
  // dstand canvas panel
  var cmpBlkDstCanvasEditorCnt = new CmpContainer([winSessionBlkJSX, "lytDestCanvasEditorCnt"],
                                                    [{cmp: this.cmpBlkDstPortPanel},
                                                     {cmp: this.cmpBlkDstCanvas},
                                                     {cmp: this.cmpBtnEditCanvasApply },
                                                     {cmp:this.cmpBlkFooter},
                                                     {cmp:this.cmpBlkFooterTxt},
                                                     {cmp: this.cmpBtnDstPortFilter},
                                                     {cmp: this.cmpBlkMediaPlayerDstPortFilter},
                                                     {cmp: this.cmpBlkDstPortTagValues},
                                                     {cmp: this.cmpBtnDstPortTagValueUnselectAll},
                                                     {cmp: this.cmpSelDstPortTagName},
                                                     {cmp: this.cmpBtnCloseDstPortFilter}
                                                    ],
                                                    null,
                                                    null);

  // subscription to listen to PORTDISPLAYSTATUSUPDATE
  // mainly forUMC which we display the status
  var PORTDISPLAYSTATUSUPDATE_ID = eventApp.subscribe("PORTDISPLAYSTATUSUPDATE", null, this,
                                                        function (EventVar, EventVal) {
                                                          var objEvent = EventVal;
                                                          var notifyPortNID = objEvent.PortNID;
                                                          var portDisplayStatusNode = objEvent.PortDisplayStatusNode;
                                                          var notifySubscriptionID = objEvent.subscriptionID;
                                                          var notifyOperation = objEvent.Operation;

                                                          if (notifyOperation == "Update") {
                                                            var dstPortNID = topic_Get(tSession, "MediaPlayerDstPortSelected");
                                                            if (notifyPortNID == dstPortNID && notifySubscriptionID == topic_Get(tSession, 'PortDisplayStatusSubscriptionID')) {
                                                              var recordNode = this.cmpBlkDstPortPanel.BlkJSX.getRecordNode(dstPortNID);
                                                              if (jsx3.util.strEmpty(recordNode)) return;
                                                              var featurenode = recordNode.selectSingleNode(".//FeatureList/Feature");
                                                              var featureName = null;
                                                              if (!jsx3.util.strEmpty(featurenode)) {
                                                                featureName = featurenode.getAttribute("name");
                                                              }

                                                              if (jsx3.util.strEmpty(featureName)) return;

                                                              if (featureName == 'XP200DisplayConfig') {
                                                                // update display status for xp rx
                                                              }
                                                              else if (featureName == 'IPVSClientDisplayConfig') {
                                                                // update display status for UMC
                                                                this.showUMCWindowingStatus(portDisplayStatusNode);
                                                              }

                                                            }
                                                          }
                                                        });

  topic_Set(tSession, "PORTDISPLAYSTATUSUPDATE_ID", PORTDISPLAYSTATUSUPDATE_ID);

  //getData whenever "CollaborationState" is published from clsCtrlAgent.notifySessionState
  eventSession.subscribe("MediaPlayerDstPortGetData", "READY", this,
                          function(eventVar, eventVal) {
                            //get Data
                            cmpBlkDstCanvasEditorCnt.getData.call(this);
                          }); 
                          
                          
  eventSession.subscribe("UpdateSession", null, this,
                          function(eventVar, eventVal) {
                            var eventObj = eventVal;
                            var sessionId = topic_Get(tSession,"SessionID");
                            if(sessionId == eventObj.sessionId  ){
                              if( topic_Get(tSession, "MediaPlayerDstPortInstanceCount") == "1" ){
                                if( topic_Get(tSession, "MediaPlayerDstPortWindowSelected") != "NONE" ){ return;}
                                
                                var dstEntityId = topic_Get(tSession, "MediaPlayerDstPortEntityId");
                                var nodeSession = agtCollaborationCtrl.getSession(tSession, topic_Get(tSession,"SessionID"));
                                if( jsx3.util.strEmpty(nodeSession) ) {return;}                             
                                var nodeConn  = nodeSession.getConn(null, dstEntityId, null);
                                if( !jsx3.util.strEmpty(nodeConn) ){
                                  var windowId = nodeSession.getConnWindowId(nodeConn);
                                  if( (windowId == "undefined") || jsx3.util.strEmpty(windowId) ){
                                    windowId = "NONE";
                                  }
                                  topic_Set(tSession, "MediaPlayerDstPortWindowSelected", windowId);
                                  var canvasId = nodeSession.getConnCanvasId(nodeConn);
                                  if( (canvasId == "undefined") || jsx3.util.strEmpty(canvasId) ){
                                    canvasId = "NONE";
                                  } 
                                  topic_Set(tSession, "MediaPlayerDstPortCanvasSelected", canvasId);                               
                                }
                              }
                            } 
                          });
                                                
  //for the playermode so that we can set the previously selected dest port
  eventSession.subscribe("MediaPlayerMode", "CONTROLLER", this,
                          function(eventVar, eventVal) {
                            // check if any record is selected if not then select the first record
                            var selRecord = this.cmpBlkDstPortPanel.BlkJSX.getXML().selectSingleNode(".//record[@jsxselected='1']");
                            if( !jsx3.util.strEmpty(selRecord) ){
                              topic_Publish(tSession, "MediaPlayerDstPortSelected", selRecord.getAttribute("jsxid"));
                            }

                            
                          }); 
                                                    
  //listen to the EntityPresence. it is published from  Notificationfnc.js
  var ENTITYPRESENCEEVENT_DST_ID =  eventApp.subscribe("ENTITYPRESENCEEVENT", null, this,
                                                    function (eventVar, eventVal) {
                                                      var eventObj = eventVal;
                                                      var sessionId = topic_Get(tSession, "SessionID");
                                                      if (sessionId == eventObj.sessionId) {
                                                        //get the dst panel cdf doc if the entity is in there chnage the presence image
                                                        var dstPortXML = this.cmpBlkDstPortPanel.BlkJSX.getXML();
                                                        var selRecord = dstPortXML.selectSingleNode("//record[@jsxEntityid='" + eventObj.entityId + "']");
                                                        if (!jsx3.util.strEmpty(selRecord)) {
                                                          if (eventObj.presence == "available") {
                                                            //selRecord.setAttribute("jsxdevpresence", "JSXAPPS/IPVS_ClientUI/images/icons/icon_greenBall.png");
                                                            selRecord.setAttribute("jsxdevpresence", "available");
                                                          }
                                                          else if (eventObj.presence == "unavailable") {
                                                            //selRecord.setAttribute("jsxdevpresence", "JSXAPPS/IPVS_ClientUI/images/icons/icon_grayBall.png");
                                                            selRecord.setAttribute("jsxdevpresence", "unavailable");
                                                          }
                                                          else {
                                                            //selRecord.setAttribute("jsxdevpresence", "JSXAPPS/IPVS_ClientUI/images/icons/icon_lightgrayBall.png");
                                                            selRecord.setAttribute("jsxdevpresence", "");
                                                          }
                                                          this.cmpBlkDstPortPanel.onResponse(dstPortXML);

                                                          var dstPortNID = topic_Get(tSession, "MediaPlayerDstPortSelected");
                                                          var selRecordNID = selRecord.getAttribute("jsxid");
                                                          if (dstPortNID == selRecordNID) {
                                                            this.showMaskStatus();
                                                          }

                                                        }
                                                      }
                                                    });

    topic_Set(tSession, "ENTITYPRESENCEEVENT_DST_ID", ENTITYPRESENCEEVENT_DST_ID);
                                                    
  // -- METHOD to parse collabconfig XML and convert to list objects for GUI components
  cmpBlkDstCanvasEditorCnt.getData = function() {
    var nodeSession = agtCollaborationCtrl.getSession(tSession, topic_Get(tSession,"SessionID"));
    if( jsx3.util.strEmpty(nodeSession) ) {return;}  

    //using dstPortsPanel.xsl to transfer dst Entity in nodeSession to CDF
    var xslUriRes = null;
    if (jsx3.util.strEmpty(UriRes)) return;
    xslUriRes = UriRes.resolveURI("xsl/SessionEntityDstPortNIDList.xsl");
    if (jsx3.util.strEmpty(xslUriRes)) return;

    var XSLFile = PNameSpace.loadInclude(xslUriRes, "SessionEntityDstPortNIDList", "xsl");
    var dstNIDListNode = null;
    if (XSLFile) {
      var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
      dstNIDListNode = XSLDoc.transformToObject(nodeSession);
      var dstNIDList = dstNIDListNode.getValue();
    }

    // send the getPort Request
    var reqGetPort = new ReqSessionGetRequest("GetPort",
                                                { action: "FNCCALL", context: this,
                                                  fnc: function(respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      var xslUriRes1 = null;
                                                      var UriRes1 = this.cmpBlkDstPortPanel.BlkJSX.getUriResolver();
                                                      if (jsx3.util.strEmpty(UriRes1)) return;
                                                      xslUriRes1 = UriRes1.resolveURI("xsl/MediaPlayerDstPanel.xsl");
                                                      if (jsx3.util.strEmpty(xslUriRes1)) return;

                                                      var XSLFile1 = PNameSpace.loadInclude(xslUriRes1, "MediaPlayerDstPanel", "xsl");
                                                      var docXformResp = null;
                                                      if (XSLFile1) {
                                                        var XSLDoc1 = jsx3.xml.XslDocument.wrap(XSLFile1);
                                                        docXformResp = XSLDoc1.transformToObject(respVal);
                                                      } 
                                                      // add the dstentityid from the nodesssion to the record
                                                      var nodeSession = agtCollaborationCtrl.getSession(tSession, topic_Get(tSession,"SessionID"));
                                                      if( jsx3.util.strEmpty(nodeSession) ) {return;}
                                                      
                                                      var dstPortRecordIter = docXformResp.selectNodeIterator(".//record");
                                                      while( dstPortRecordIter.hasNext() ) {
                                                        var selRecord = dstPortRecordIter.next();                                                      
                                                        var dstPortNID = selRecord.getAttribute("jsxid");
                                                        var dstEntityNode = nodeSession.getEntityByNID(dstPortNID);                                                      
                                                        var dstEntityId = nodeSession.getEntityId(dstEntityNode);
                                                        var dstEntityPresence = nodeSession.getEntityPresence(dstEntityNode);
                                                        selRecord.setAttribute("jsxEntityid", dstEntityId);
                                                        selRecord.setAttribute("jsxdevpresence", dstEntityPresence);
                                                      }
                                                        
                                                      // check if any record is selected if not then select the first record
                                                      var selRecord = docXformResp.selectSingleNode(".//record[@jsxselected='1']");
                                                      if( jsx3.util.strEmpty(selRecord) ){
                                                        selRecord = docXformResp.selectSingleNode(".//record");
                                                        selRecord.setAttribute("jsxselected","1");
                                                      }
                                                     
                                                      
                                                      this.cmpBlkDstPortPanel.onResponse(docXformResp);
                                                      var selRecordNID = selRecord.getAttribute("jsxid");
                                                      topic_Publish(tSession, "MediaPlayerDstPortSelected", selRecordNID);
                                                      
                                                      // fill the Tag Name drop down
                                                      var xslExtractTagNameRes = UriRes.resolveURI("xsl/TagsFromGetFileORGetPort.xsl");
                                                      if (jsx3.util.strEmpty(xslExtractTagNameRes)) return;
                                                      var xslTagNameRes = UriRes.resolveURI("xsl/TagNameListFromGetPort.xsl");
                                                      if (jsx3.util.strEmpty(xslTagNameRes)) return;
                                                      
                                                      var TagsXSLFile = PNameSpace.loadInclude(xslExtractTagNameRes, "xslExtractTagNameRes", "xsl");
                                                      var TagsData = null;
                                                      if(TagsXSLFile){
                                                        var XSLDoc = jsx3.xml.XslDocument.wrap(TagsXSLFile);
                                                        TagsData = XSLDoc.transformToObject(respVal);    
                                                      }
                                                      
                                                      var TagNameXSLFile = PNameSpace.loadInclude(xslTagNameRes, "xslTagNameRes", "xsl");
                                                      var TagNameDoc = null;
                                                      if(TagsXSLFile){
                                                        var XSLDoc = jsx3.xml.XslDocument.wrap(TagNameXSLFile);
                                                        TagNameDoc = XSLDoc.transformToObject(TagsData);    
                                                      }
                                                      var strXMLCacheId = this.cmpSelDstPortTagName.BlkJSX.getXMLId(); 
                                                      var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                      objXML.loadXML(TagNameDoc.toString()); 
                                                      this.cmpSelDstPortTagName.BlkJSX.setSourceXML(objXML); 
                                                      var tagNameSelected = topic_Get(tSession, "MediaPlayerDstTagNameSelected");
                                                      if( tagNameSelected == "NONE"){
                                                        tagNameSelected = "All";
                                                      }
                                                      this.cmpSelDstPortTagName.BlkJSX.setValue(tagNameSelected);
                                                      topic_Publish(tSession, "MediaPlayerDstTagNameSelected", tagNameSelected);   
                                                      
                                                      //var dstPortXML = this.cmpBlkDstPortPanel.BlkJSX.getXML();                                                                                                             
                                                      
                                                    }
                                                    else {
                                                      
                                                    }
                                                  }
                                                }
                                               );
    reqGetPort.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = dstNIDList;
    reqParams["ObjectList_WhereTextSubStr"] = "";
    reqParams["ObjectList_SelectFormat"] = "BASICCONNECTIONINFO";
    reqParams["ObjectList_Type"] = "DstPort";
    reqParams["ObjectList_SubscriptionID"] = "";
    reqParams["ObjectList_WherePermissions"] = "STREAMTO";
    reqGetPort.reqParams = reqParams;
    // send the request
    reqGetPort.sendRequest(this);    
    

  }

  // listener for the dst ports window id
  eventSession.subscribe("MediaPlayerDstPortWindowSelected", null, this,
                          function(eventVar, eventVal) {
                            var dstPortWindowId = eventVal;
                            if(dstPortWindowId == "NONE"){
                              return;
                            }
                            var canvasID = this.cmpBlkDstPortCanvasSetting.getCanvasId();                           
                            topic_Set(tSession, "MediaPlayerDstPortCanvasSelected", canvasID);
                            topic_Publish(tSession, "MediaPlayerSrcPortDeSelectAll", (newServerDate()).getTime());
                          });
                          
                          
  // listener for the dst port
  eventSession.subscribe("MediaPlayerDstPortSelected", null, this,
                          function (eventVar, eventVal) {
                            var dstPortNID = eventVal;
                            if (dstPortNID == "NONE") {
                              return;
                            }

                            var selRecord = this.cmpBlkDstPortPanel.BlkJSX.getXML().selectSingleNode("//record[@jsxid='" + dstPortNID + "']");
                            var dstEntityId = selRecord.getAttribute("jsxEntityid");
                            var dstEntityName = selRecord.getAttribute("jsxtext");
                            var dstInstanceCount = selRecord.getAttribute("jsxmaxInstanceCount");
                            topic_Set(tSession, "MediaPlayerDstPortEntityId", dstEntityId);
                            topic_Set(tSession, "MediaPlayerDstPortSelectedName", dstEntityName);
                            topic_Set(tSession, "MediaPlayerDstPortInstanceCount", dstInstanceCount);
                            //set the window to NONE
                            topic_Set(tSession, "MediaPlayerDstPortWindowSelected", "NONE");
                            topic_Set(tSession, "MediaPlayerDstPortCanvasSelected", "NONE");
                            if (dstInstanceCount == "1") {
                              var nodeSession = agtCollaborationCtrl.getSession(tSession, topic_Get(tSession, "SessionID"));
                              if (jsx3.util.strEmpty(nodeSession)) { return; }
                              var nodeConn = nodeSession.getConn(null, dstEntityId, null);
                              if (!jsx3.util.strEmpty(nodeConn)) {
                                var windowId = nodeSession.getConnWindowId(nodeConn);
                                if ((windowId == "undefined") || jsx3.util.strEmpty(windowId)) {
                                  windowId = "NONE";
                                }
                                topic_Set(tSession, "MediaPlayerDstPortWindowSelected", windowId);
                                var canvasId = nodeSession.getConnCanvasId(nodeConn);
                                if ((canvasId == "undefined") || jsx3.util.strEmpty(canvasId)) {
                                  canvasId = "NONE";
                                }
                                topic_Set(tSession, "MediaPlayerDstPortCanvasSelected", canvasId);
                              }
                            }

                            // for umc client get the display status for all other display message
                            var featurenode = selRecord.selectSingleNode(".//FeatureList/Feature");
                            var featureName = null;
                            if (!jsx3.util.strEmpty(featurenode)) {
                              featureName = featurenode.getAttribute("name");
                            }

                            if (featureName == "IPVSClientDisplayConfig") {
                              // publish to get the port display status                            
                              topic_Publish(tSession, "GetPortDisplayStatus", (newServerDate()).getTime());
                            }
                            else {
                              topic_Publish(tSession, "MediaPlayerDstCanvasCnt", "MESSAGE");
                            }

                            topic_Publish(tSession, "MediaPlayerSrcPortDeSelectAll", (newServerDate()).getTime());
                          });

  // listener for GetPortDisplayStatus
 eventSession.subscribe("GetPortDisplayStatus", null, this,
                          function (eventVar, eventVal) {
                            var dstPortNID = topic_Get(tSession, "MediaPlayerDstPortSelected");

                            // get new status for the port
                            var reqGetPortDisplayStatus = new ReqSessionGetRequest("GetPortDisplayStatus",
                                                                                    { action: "FNCCALL", context: this,
                                                                                      fnc: function (respCode, respVal) {
                                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                                          var portDisplayStatusNode = respVal.selectSingleNode(".//PortDisplayStatus");
                                                                                          this.showUMCWindowingStatus(portDisplayStatusNode);
                                                                                        }
                                                                                        else {
                                                                                          if (respVal.match(/ResponseXML/)) {
                                                                                            var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
                                                                                            var responseDoc = new jsx3.xml.Document;
                                                                                            responseDoc.loadXML(ResponseXML);
                                                                                            var ipvsErrorNode = responseDoc.selectSingleNode(".//ipvserror");
                                                                                            if (!jsx3.util.strEmpty(ipvsErrorNode)) {
                                                                                              ErrCode = ipvsErrorNode.getAttribute("code");
                                                                                              ErrDesc = ipvsErrorNode.selectSingleNode(".//description").getValue();
                                                                                            }
                                                                                            //alert the user
                                                                                            var alertMsg = "cmpBlkMediaPlayerDstPanel::setMediaPlayerDstPortCanvasConfig::SetPortDisplayConfig request failed. \n ExceptionCode:" + ErrCode + "\n ExceptionDesc:" + ErrDesc;
                                                                                            windowAlert(winSessionBlkJSX, alertMsg);
                                                                                          }
                                                                                          log.error("cmpBlkMediaPlayerDstPanel::setMediaPlayerDstPortCanvasConfig::SetPortDisplayConfig request failed : " + respVal + "\n");
                                                                                          topic_Publish(tSession, "OpenCanvasMask", "NONE");
                                                                                          topic_Publish(tSession, "ShowUpdateText", "NONE");
                                                                                        }
                                                                                      }
                                                                                    }
                                                                                   );
                            reqGetPortDisplayStatus.ServiceName = "SessionService";
                            var reqParams = new Object;
                            reqParams["ObjectList_WhereNID"] = dstPortNID;
                            reqParams["ObjectList_WhereTextSubStr"] = "";
                            reqParams["ObjectList_SubscriptionID"] = topic_Get(tSession, 'PortDisplayStatusSubscriptionID');
                            reqGetPortDisplayStatus.reqParams = reqParams;
                            // send the request
                            reqGetPortDisplayStatus.sendRequest(this);

                          });


        // function to show the mask on apply button or device offline
        this.showMaskStatus = function () {
          var dstPortNID = topic_Get(tSession, "MediaPlayerDstPortSelected");
          var selRecord = this.cmpBlkDstPortPanel.BlkJSX.getXML().selectSingleNode("//record[@jsxid='" + dstPortNID + "']");
          var featurenode = selRecord.selectSingleNode(".//FeatureList/Feature");
          var devPresence = selRecord.getAttribute("jsxdevpresence");
          var featureName = null;
          if (!jsx3.util.strEmpty(featurenode)) {
            featureName = featurenode.getAttribute("name");
          }

          if (jsx3.util.strEmpty(featureName)) return;

          if (featureName == 'XP200DisplayConfig') {

          }
          else if (featureName == 'IPVSClientDisplayConfig') {
            if (devPresence == "available") {
              topic_Publish(tSession, "OpenCanvasMask", "NONE");
              topic_Publish(tSession, "ShowOfflineText", "NONE");
            }
            else {
              topic_Publish(tSession, "OpenCanvasMask", "OPEN");
              topic_Publish(tSession, "ShowOfflineText", "SHOW");
            }
          }
        } // end this.showMaskStatus

      //function to show the umc display status
        this.showUMCWindowingStatus = function (dataNode) {
          //extract the 3 nodes from the data. chances are one or 2 nodes would be missing based on the notification.
          var displayNode = dataNode.selectSingleNode(".//Display");
          var canvasNode = dataNode.selectSingleNode(".//Canvas");
          dataNode.removeChildren();
          var portDisplayStatusNode = dataNode.cloneNode(true);

          //save or get the portdisplaynode
          if (jsx3.util.strEmpty(portDisplayStatusNode)) {
            portDisplayStatusNode = topic_Get(tSession, ".//GetPortDisplayStatus//PortDisplayStatus");
          }

          //save or get the display node
          if (jsx3.util.strEmpty(displayNode)) {
            displayNode = topic_GetNode(tSession, ".//GetPortDisplayStatus//Display");
          }

          //save or get the canvas node
          if (jsx3.util.strEmpty(canvasNode)) {
            canvasNode = topic_GetNode(tSession, ".//GetPortDisplayStatus//Canvas");
          }

          // save the response in the topic
          var dataNode = topic_Set(tSession, "GetPortDisplayStatus", null);
          dataNode.removeChildren();
          dataNode.appendChild(portDisplayStatusNode.cloneNode(true));
          var portStatusNode = dataNode.selectSingleNode(".//PortDisplayStatus");
          portStatusNode.appendChild(displayNode.cloneNode(true));
          portStatusNode.appendChild(canvasNode.cloneNode(true));

          topic_Set(tSession, "MediaPlayerDstPortWindowSelected", "NONE");
          topic_Set(tSession, "MediaPlayerDstPortCanvasSelected", "NONE");

          //For Canvas Panel
          if (PCWindowingXSLFile.hasError()) {
            UriRes = this.cmpBlkDstPortCanvasSetting.BlkJSX.getUriResolver();
            if (jsx3.util.strEmpty(UriRes)) return;
            xslUriRes = UriRes.resolveURI("xsl/PcWindowingCanvas.xsl");
            PCWindowingXSLFile = PNameSpace.loadInclude(xslUriRes, "_Update", "xsl");
          }

          if (PCWindowingXSLFile.hasError()) {
            log.error("showUMCWindowingStatus::PcWindowingCanvas.xsl:: has error\n");
            return;
          }

          var XSLDoc = jsx3.xml.XslDocument.wrap(PCWindowingXSLFile);
          var docXformResp = XSLDoc.transformToObject(dataNode);
          //update the canvas view
          this.cmpBlkDstPortCanvasSetting.onResponse("SUCCESS", docXformResp.toString());

          // disable the canvas resolution text boxes.
          this.cmpBlkDstPortCanvasSetting.cmpTxtDisplayHeight.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
          this.cmpBlkDstPortCanvasSetting.cmpTxtDisplayWidth.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);

          topic_Publish(tSession, "MediaPlayerDstCanvasCnt", "CANVAS");

          this.showMaskStatus();
        }
                          
    // function check for exception code
    this.checkResponseForExceptionCode = function(requestName, responseVal){
      var execptionCodeNode = responseVal.selectSingleNode(".//Exception/Code");
      if( !jsx3.util.strEmpty(execptionCodeNode) ){ 
        return false;                                                                                                                         
      }
      return true;    
    } // end this.checkResponseForExceptionCode
    
    //function to check the dst port status/presence
    this.checkDstPortStatus = function(dstPortNID) {
      var selRecord = this.cmpBlkDstPortPanel.BlkJSX.getXML().selectSingleNode("//record[@jsxid='"+ dstPortNID +"']");
      var dstPortPresence = selRecord.getAttribute("jsxdevpresence");
      if( dstPortPresence != "available" ){
        windowAlert(winSessionBlkJSX, "Cannot perform operation as Device is not ready.");
        return false;
      }
      else {
        return true;
      }
    }
    
    //function on click for the apply button
    this.setMediaPlayerDstPortCanvasConfig = function () {
      var dstPortNID = topic_Get(tSession, "MediaPlayerDstPortSelected");

      if (!this.checkDstPortStatus(dstPortNID)) return;

      topic_Publish(tSession, "OpenCanvasMask", "OPEN");
      topic_Publish(tSession, "ShowUpdateText", "SHOW");

      var newCanvasNodeXML = this.cmpBlkDstPortCanvasSetting.getCanvasXML();
      var newCanvasNode = newCanvasNodeXML.selectSingleNode(".//Canvas");

      // create the set request data
      var setPortDisplayNode = topic_Set(tSession, "SetPortDisplayConfig", null);
      setPortDisplayNode.removeChildren();
      setPortDisplayNode.setAttribute("byNID", dstPortNID);
      setPortDisplayNode.setAttribute("byTitle", "");
      //setPortDisplayNode.setAttribute("subscriptionID", topic_Get(tSession, 'PortDisplayConfigSubscriptionID'));
      setPortDisplayNode.appendChild(newCanvasNode);

      //send therequest
      var setPortDisplayReq = new ReqSessionHandleWindowing(setPortDisplayNode, "SetPortDisplayConfig",
                                                             { action: "FNCCALL", context: this,
                                                               fnc: function (respCode, respVal) {
                                                                 var errorFlag = false;
                                                                 if (respCode == "SUCCESS" && respVal != null) {

                                                                 }
                                                                 else {
                                                                   errorFlag = true;
                                                                   if (respVal.match(/ResponseXML/)) {
                                                                     var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
                                                                     var responseDoc = new jsx3.xml.Document;
                                                                     responseDoc.loadXML(ResponseXML);
                                                                     var ipvsErrorNode = responseDoc.selectSingleNode(".//ipvserror");
                                                                     if (!jsx3.util.strEmpty(ipvsErrorNode)) {
                                                                       ErrCode = ipvsErrorNode.getAttribute("code");
                                                                       ErrDesc = ipvsErrorNode.selectSingleNode(".//description").getValue();
                                                                     }
                                                                     //alert the user
                                                                     var alertMsg = "cmpBlkMediaPlayerDstPanel::setMediaPlayerDstPortCanvasConfig::SetPortDisplayConfig request failed. \n ExceptionCode:" + ErrCode + "\n ExceptionDesc:" + ErrDesc;
                                                                     log.error("cmpBlkMediaPlayerDstPanel::setMediaPlayerDstPortCanvasConfig::SetPortDisplayConfig request failed : " + respVal + "\n");
                                                                     windowAlert(winSessionBlkJSX, alertMsg);
                                                                   }
                                                                 }

                                                                 // if error get the old config
                                                                 if (errorFlag) {
                                                                   topic_Publish(tSession, "GetPortDisplayStatus", (newServerDate()).getTime());
                                                                 }

                                                               }
                                                            }
                                                         );


      setPortDisplayReq.sendRequest(this);
    }  // end this.setMediaPlayerDstPortCanvasConfig
          
  eventSession.subscribe("MediaPlayerDstTagNameSelected", null, this,
                          function(eventVar, eventVal) {
                            if (eventVal != "NONE") {
                                // get the selected value
                                var nameRecord = this.cmpSelDstPortTagName.BlkJSX.getValue();
                                //get the complete record
                                var selNameRecord = this.cmpSelDstPortTagName.BlkJSX.getRecordNode(nameRecord);
                                // get the Tag node which contains the TagValues
                                var tagNode = selNameRecord.selectSingleNode(".//Tag");
                                //get the xsl
                                var xslUriRes = null;
                                var UriRes = this.cmpSelDstPortTagName.BlkJSX.getUriResolver();
                                if( jsx3.util.strEmpty(UriRes) ) return;
                                xslUriRes = UriRes.resolveURI("xsl/TagValueList.xsl");
                                if( jsx3.util.strEmpty(xslUriRes) ) return;

                                var XSLFile = PNameSpace.loadInclude(xslUriRes, "TagValueList" , "xsl");
                                if( XSLFile ) {
                                  var XSLDoc = jsx3.xml.XslDocument.wrap(XSLFile);
                                  var docXformResp = XSLDoc.transformToObject(tagNode);
                                } 
                            
                                // set/send the cdf doc to the panel
                                this.cmpBlkDstPortTagValues.onResponse(docXformResp);

                                // get the Tag Name and Values for the expression
                                var recordIter = tagNode.selectNodeIterator(".//Value");
                                var i=0;                                
                                while(recordIter.hasNext()){
                                  var recordNode = recordIter.next();
                                  var selectedTagName = recordNode.getAttribute("jsxTagName");
                                  var selectedTagValue = recordNode.getValue();
                                  var newTagExpression = "(@Name='" + selectedTagName + "' and @Value='" + selectedTagValue + "')";
                                  if (i == 0) {
                                      var tagExpression = newTagExpression;
                                      i++;
                                  } else {
                                      tagExpression = tagExpression + " or " + newTagExpression;
                                  }
                                }
                                log.info("cmpBlkSessionInfo:sessionInfo MediaPlayerDstTagNameSelected.MediaPlayerDstPortsTagExpression::" + tagExpression + "\n");
                                topic_Set(tSession, "MediaPlayerDstPortsTagExpression", tagExpression);
                                topic_Publish(tSession, "UpdateMediaPlayerDstPortList", (newServerDate()).getTime());                                                                                                   
                            }
                          }
                        );

  // On selecting Source tag Value
  eventSession.subscribe("MediaPlayerDstTagValueSelection", null, this,
                          function(eventVar, eventVal) {
                            var newTagExpression = "";
                            var strXMLCacheId = this.cmpBlkDstPortTagValues.BlkJSX.getXMLId();
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
                              var tagNameSelected = topic_Get(tSession, "MediaPlayerDstTagNameSelected");
                              topic_Publish(tSession, "MediaPlayerDstTagNameSelected", tagNameSelected);                            
                            }
                            else {
                              log.info("cmpBlkSessionInfo:sessionInfo MediaPlayerDstTagNameSelected.MediaPlayerDstTagValueSelection::" + tagExpression + "\n");
                              topic_Set(tSession, "MediaPlayerDstPortsTagExpression", tagExpression);
                              topic_Publish(tSession, "UpdateMediaPlayerDstPortList", (newServerDate()).getTime());
                            }
                          }
                       );
                           
  // On selecting Source tag Value
  eventSession.subscribe("UpdateMediaPlayerDstPortList", null, this,
                          function(eventVar, eventVal) {
                            var tagExpression = topic_Get(tSession, "MediaPlayerDstPortsTagExpression");
                            var dstPortXML = this.cmpBlkDstPortPanel.BlkJSX.getXML();
                            var recordIter = dstPortXML.selectNodeIterator(".//record");
                            while(recordIter.hasNext()){
                              var recordNode = recordIter.next();
                              recordNode.setAttribute("jsxVisible","0");
                            } 
                            //Handling if No tags are present at all (If No tag then show all Destinations)
                            if(!jsx3.util.strEmpty(tagExpression) && tagExpression != "NONE"){                                                                                                
                              var showRecordIter = dstPortXML.selectNodeIterator(".//record[Groups/Tag["+ tagExpression +"]]");
                            }
                            else{
                               var showRecordIter = dstPortXML.selectNodeIterator(".//record");
                            }
                            while(showRecordIter.hasNext()){
                              var recordNode = showRecordIter.next();
                              recordNode.setAttribute("jsxVisible","1");
                            }
                            log.info("cmpBlkSessionInfo:sessionInfo MediaPlayerDstTagNameSelected.UpdateMediaPlayerDstPortList INSERTING XML::" + dstPortXML + "\n");
                            this.cmpBlkDstPortPanel.onResponse(dstPortXML);
                            var dstPortXML1 = this.cmpBlkDstPortPanel.BlkJSX.getXML();
                            log.info("cmpBlkSessionInfo:sessionInfo MediaPlayerDstTagNameSelected.UpdateMediaPlayerDstPortList CDF::" + dstPortXML1 + "\n");                    
                          }
                       );              
              
  //separate cmpBlkSessionInfo from cmpBlkMediaPlayer and cmpBlkCollaboration
  jsx3.sleep((function() {
                this.init();                
                this.show();
                //push this component to parent window's SubCmpArr
                winSession.SubCmpArr.push({ cmp: cmpBlkDstCanvasEditorCnt });
                log.info("cmpBlkMediaPlayer::MediaPlayerMode::Loaded destination panel");
                topic_Publish(tSession, "MediaPlayerDstLoadingStatus", "LOADED");
                log.info("cmpBlkSessionInfo:destination panel is loaded.");
               }
             ), null, cmpBlkDstCanvasEditorCnt);


} // end cmpBlkMediaPlayerDstPanel_Create


