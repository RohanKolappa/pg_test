
function cmpMediaControllerSubArr_Create() {
    var cmpMediaController = this;
    // get vars from object instance
    var cmpMediaControllerBlkJSX = cmpMediaController.BlkJSX;
    var tMediaController = cmpMediaController.tMediaController;
    var eventMediaController = cmpMediaController.eventMediaController;
    var winMediaControllerWindow = cmpMediaController.window;
    
    var cmpClientWindowHeaderLogo = new Component([cmpMediaControllerBlkJSX, "clientWindowHeaderLogo"], null, null, null);
    // Header Block - visible by default - To be hidden when Media Controller is used in Web mode.
    if (clientType == "WEB") {
      var cmpBlkMediaControllerHeader = getJsxByName([cmpMediaControllerBlkJSX, "blkHeader"]);
      cmpBlkMediaControllerHeader.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
      var lytCtrlMain = getJsxByName([cmpMediaControllerBlkJSX, "MediaControllerGrid"]);
      lytCtrlMain.setRows("0,*", true);
      log.info("cmpBlkMediaController::cmpMediaControllerSubArr_Create");
    }
    else {
      // set the window customize image if any
      cmpClientWindowHeaderLogo.customizeSkin = function() {
                                                  var clientWindowHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientWindowHeaderLogo");
                                                  if (!jsx3.util.strEmpty(clientWindowHeaderLogo)) {
                                                    cmpClientWindowHeaderLogo.BlkJSX.setSrc(clientWindowHeaderLogo);
                                                    cmpClientWindowHeaderLogo.BlkJSX.repaint();
                                                  }
                                                }      
    }

    //----------------------------- End PointController Panel ---------------------------
    //Initialize Controller to be set into connect mode
    topic_Set(tMediaController, 'ConnectOrAdd', 'CONNECT');
    //Initialize Controller end Point to set for default  
    topic_Set(tMediaController, 'EndPointSelected', "");  
    topic_Set(tMediaController, "AddEnabledDisabled", 'ENABLED');
    
    topic_Set(tMediaController, "OpenWindowingPanel", "OPEN");
    topic_Set(tMediaController, "WindowingSize", "NONE");
    topic_Set(tMediaController, "WindowingLocation", "NONE");
    topic_Set(tMediaController, "WindowingMode", "NONE"); 
    topic_Set(tMediaController, "OpenWindowingMask", "NONE");
    topic_Set(tMediaController, "SelectCanvasWindow","NONE");
    topic_Set(tMediaController, 'NIDList',null);
    topic_Set(tMediaController, 'WindowMode', "CANVAS");
    topic_Set(tMediaController, 'WindowKbm', "DISABLE");
    
    topic_Set(tMediaController, 'PVREnable', "false");
    topic_Set(tMediaController, 'PVRTrimLength', "3600000");

    topic_Set(tMediaController, 'PortDisplayConfigSubscriptionID', "PortDisplayConfig_Subscription_" + getUUID());
    topic_Set(tMediaController, 'PortDisplayStatusSubscriptionID', "PortDisplayStatus_Subscription_" + getUUID());
    
    //End PointController Panel- End Point Add button
    var cmpBtnConnAddMode = new CmpImageButton([cmpMediaControllerBlkJSX, "btnAdd"],
                                                  null,
                                                   { action: "FNCCALL", context: this,
                                                     fnc: function() {
                                                       var addMode = topic_Get(tMediaController, 'ConnectOrAdd');
                                                       if (addMode == 'CONNECT') {
                                                         var configurationMode = topic_Get(tApp, "EndPointConfigurable");
                                                         if (configurationMode == "true") {
                                                           cmpBtnConnAddMode.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                                           topic_Publish(tMediaController, 'ConnectOrAdd', 'ADD');
                                                           topic_Publish(tMediaController, 'ClearEndPoints', 'NONE');
                                                           topic_Publish(tMediaController, "ButtonSelection", "Destination");
                                                         }
                                                         //close any windowing dialog if any
                                                         closeWindowingDialog();

                                                       }
                                                       else {
                                                         cmpBtnConnAddMode.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                                         topic_Publish(tMediaController, 'ConnectOrAdd', 'CONNECT');
                                                         var tabButton = topic_Get(tMediaController, "LiveOrRecorded");
                                                         topic_Publish(tMediaController, "ButtonSelection", tabButton);
                                                       }
                                                     }
                                                   }
                                                 );

    var cmpBlkConnAddMode = new CmpContainer([cmpMediaControllerBlkJSX, "blkAdd"],
                                             [{ cmp: cmpBtnConnAddMode,
                                               sel: "topic_Get('" + tMediaController + "', 'AddEnabledDisabled') != 'DISABLED'"}],
                                             null
                                             );
                                                                                    
    //End PointController Panel- End Point Refresh button
    var cmpBtnEPRefresh = new CmpImageButton([cmpMediaControllerBlkJSX, "btnEPRefresh"],
                                             null,
                                               { action: "FNCCALL", context: this,
                                                 fnc: function() {
                                                   topic_Publish(tMediaController, "RefreshEndPoints", newServerDate());
                                                 }
                                               }
                                             );

    var cmpBlkEPRefresh = new CmpContainer([cmpMediaControllerBlkJSX, "blkEPRefresh"],
                                          [{ cmp: cmpBtnEPRefresh}],
                                          null,
                                          null); 

    var cmpBlkEndPointControllerHeader = new CmpContainer([cmpMediaControllerBlkJSX, "blkEndPointControllerHeader"],
                                                          [{ cmp: cmpBlkConnAddMode,
                                                              sel: "clsUserPermission.getPermission('Port', 'read') == 'true'"},
                                                           { cmp: cmpBlkEPRefresh,
                                                             sel: "clsUserPermission.getPermission('Port', 'read') == 'true'"}
                                                          ],
                                                          [{ action: "TOPICEVENT", topic: tMediaController, eventvar: "AddEnabledDisabled", eventval: null}]);


    var cmpBlkEndPointContainer = new  CmpEndPointPanel([cmpMediaControllerBlkJSX, "BlockEndPointPanel"],
                                                     null,
                                                     { action: "TOPICEVENT", topic: tMediaController, eventvar: "EndPointSelected", eventval: "jsxid" });

    var UriRes = cmpBlkEndPointContainer.BlkJSX.getUriResolver();
    var EndPointxslUriRes = UriRes.resolveURI("xsl/EndPoint.xsl");
    var EndPointXSLFile = PNameSpace.loadInclude(EndPointxslUriRes, "_Update", "xsl");

    var PCWindowingxslUriRes = UriRes.resolveURI("xsl/PcWindowingCanvas.xsl");
    var PCWindowingXSLFile = PNameSpace.loadInclude(PCWindowingxslUriRes, "_Update", "xsl");
                                                     
    // subscription to listen to DBUpdate
    var DBUpdate_ID = eventApp.subscribe("DBUpdate", null, this,
                                          function(EventVar, EventVal) {
                                            var objEvent = EventVal;
                                            if ((objEvent.type == "replace") ||
                                                (objEvent.type == "delete")) {
                                              var updatePortNID = objEvent.nodeID;
                                              var updatechildXpath = objEvent.childXpath;
                                              var endPointIterator = cmpBlkEndPointContainer.BlkJSX.getXML().selectNodeIterator("//record");
                                              while(endPointIterator.hasNext()){
                                                var portNode = endPointIterator.next();
                                                var portNID = portNode.getAttribute("jsxid");
                                                var portName = portNode.getAttribute("jsxtext");
                                                var portType = portNode.getAttribute("jsxportType");
                                                if (portType == "StreamSrc") {
                                                  portType = "SrcPort";
                                                }
                                                else if (portType == "StreamDst") {
                                                  portType = "DstPort";
                                                }
                                                else if(portType == "StreamIO"){
                                                  portType = "IOPort";
                                                }
                                                //if yes then issue refresh  childXpath == //MediaStreamInfo/MediaStreamList
                                                var childXpath = "//InstanceInfo/MediaStreamList";
                                                if ( updatePortNID == portNID ) {
                                                  updateEndPointSession(cmpMediaController, portNID);
                                                }                        
                                              }
                                            }
                                          });
    
      topic_Set(tMediaController, "DBUpdate_ID", DBUpdate_ID);

      // subscription to listen to PORTCONNECTIONUPDATE
      var PORTCONNECTIONUPDATE_ID = eventApp.subscribe("PORTCONNECTIONUPDATE", null, this,
                                                        function (EventVar, EventVal) {
                                                          var objEvent = EventVal;

                                                          var portNID = objEvent.PortNID;
                                                          var portSessionInfoNode = objEvent.PortSessionInfoNode;
                                                          var portConnectionInfoNode = portSessionInfoNode.selectSingleNode(".//PortConnectionInfo");
                                                          var notifySubscriptionID = objEvent.subscriptionID;
                                                          var notifyOperation = objEvent.Operation;
                                                          var notifyType = portConnectionInfoNode.getAttribute("type");
                                                          //return if type equal to PVR or RECORD
                                                          if (notifyType != "VIEW" && notifyType != "PLAYBACK") return;

                                                          // look for the nid and subscription id
                                                          var endPointNode = cmpBlkEndPointContainer.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='" + portNID + "' and @jsxsubscriptionid='" + notifySubscriptionID + "']");
                                                          if (jsx3.util.strEmpty(endPointNode)) return;

                                                          var endPointType = endPointNode.getAttribute("jsxportType");

                                                          var notifySessionID = portSessionInfoNode.getAttribute("id");
                                                          var notifyConnectionID = portConnectionInfoNode.getAttribute("id");

                                                          if (notifyOperation != "Delete") {
                                                            var notifySessionName = portSessionInfoNode.getAttribute("name");

                                                            var notifyConnName = portConnectionInfoNode.getAttribute("name");
                                                            var notifySrcNID = portConnectionInfoNode.getAttribute("srcNID");
                                                            var notifyDstNID = portConnectionInfoNode.getAttribute("dstNID");

                                                            var notifyState = portConnectionInfoNode.getAttribute("state");
                                                            var notifyWindowID = portConnectionInfoNode.getAttribute("windowId");
                                                            var notifyCanvasID = portConnectionInfoNode.getAttribute("canvasId");
                                                            var notifyErrorCode = portConnectionInfoNode.getAttribute("errorCode");
                                                            var notifyErrorData = portConnectionInfoNode.getAttribute("errorData");

                                                            var portSessionInfoStr = "<PortSessionInfo jsxsessionid ='" + notifySessionID + "' jsxsessionname ='" + notifySessionName + "'></PortSessionInfo>";
                                                            var portConnectionInfoStr = "<PortConnectionInfo jsxconnectionid ='" + notifyConnectionID + "'" +
                                                                                        " jsxconnectionname ='" + notifyConnName + "'" +
                                                                                        " jsxsrcnid ='" + notifySrcNID + "'" +
                                                                                        " jsxdstnid ='" + notifyDstNID + "'" +
                                                                                        " jsxconnectionstate ='" + notifyState + "'" +
                                                                                        " jsxwindowid ='" + notifyWindowID + "'" +
                                                                                        " jsxcanvasid ='" + notifyCanvasID + "'" +
                                                                                        " jsxerrorcode ='" + notifyErrorCode + "'" +
                                                                                        " jsxerrordata ='" + notifyErrorData + "'" + " />";

                                                            var portSessionInfoDoc = new jsx3.xml.Document();
                                                            portSessionInfoDoc.loadXML(portSessionInfoStr);

                                                            var portConnectionInfoDoc = new jsx3.xml.Document();
                                                            portConnectionInfoDoc.loadXML(portConnectionInfoStr);

                                                            var featureName = endPointNode.getAttribute("jsxfeature");
                                                            if (featureName != "IPVSClientDisplayConfig") {
                                                              portConnectionInfoDoc.setAttribute("jsxunselectable", "1");
                                                            }

                                                            if (notifyState == "SETUP_ERROR" || notifyState == "START_ERROR" || notifyState == "SOURCE_ERROR" || notifyState == "MEDIA_ERROR" || notifyState == "DEST_ERROR") {
                                                              portConnectionInfoDoc.setAttribute("jsxerrorimg", "images/icons/icon_error_small.png");
                                                            }

                                                          }

                                                          if (objEvent.Operation == "Add") {
                                                            var oldPortConnectionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']/PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
                                                            if (!jsx3.util.strEmpty(oldPortConnectionInfoNode)) {
                                                              var PortSessionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']");
                                                              PortSessionInfoNode.replaceNode(portConnectionInfoDoc, oldPortConnectionInfoNode);
                                                            }
                                                            else {
                                                              var portSessionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']");
                                                              if (jsx3.util.strEmpty(portSessionInfoNode)) {
                                                                endPointNode.appendChild(portSessionInfoDoc);
                                                                portSessionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']");
                                                              }
                                                              portSessionInfoNode.appendChild(portConnectionInfoDoc);
                                                            }
                                                          }
                                                          else if (objEvent.Operation == "Update") {
                                                            var oldPortConnectionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']/PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
                                                            if (!jsx3.util.strEmpty(oldPortConnectionInfoNode)) {
                                                              var PortSessionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']");
                                                              PortSessionInfoNode.replaceNode(portConnectionInfoDoc, oldPortConnectionInfoNode);
                                                            }
                                                          }
                                                          else if (objEvent.Operation == "Delete") {
                                                            var oldPortConnectionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']/PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
                                                            if (!jsx3.util.strEmpty(oldPortConnectionInfoNode)) {
                                                              oldPortConnectionInfoNode.getParent().removeChild(oldPortConnectionInfoNode);
                                                            }
                                                            var portSessionInfoConnectionIter = endPointNode.selectNodeIterator(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']/PortConnectionInfo");
                                                            if (!portSessionInfoConnectionIter.hasNext()) {
                                                              var PortSessionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']");
                                                              endPointNode.removeChild(PortSessionInfoNode);
                                                            }
                                                          }

                                                          cmpBlkEndPointContainer.BlkJSX.insertRecordNode(endPointNode, null, false);

                                                          if (endPointType == "StreamDst" || endPointType == "StreamIO") {
                                                            //Connection Highlight for window
                                                            var record = cmpBlkEndPointContainer.BlkJSX.getRecordNode(portNID);
                                                            var windowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                            if (!jsx3.util.strEmpty(windowId) && windowId != "NONE") {
                                                              cmpBlkEndPointContainer.BlkJSX.deselectAllConnections(record);
                                                              var connectionNode = record.selectSingleNode(".//PortSessionInfo/PortConnectionInfo[@jsxwindowid ='" + windowId + "']");
                                                              if (!jsx3.util.strEmpty(connectionNode)) {
                                                                connectionNode.setAttribute("jsxselected", "1");
                                                              }
                                                            }
                                                          }

                                                          var strXMLCacheId = cmpBlkEndPointContainer.BlkJSX.getXMLId();
                                                          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                          cmpBlkEndPointContainer.onResponse(objXML);

                                                          var selectedEndPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                          if (selectedEndPointNID == portNID) {
                                                            var selectedWindowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                            //generate the new nidlist
                                                            var NIDList = topic_GetNode(tMediaController, 'NIDList');
                                                            NIDList.removeChildren();
                                                            var connectionIterator = endPointNode.selectNodeIterator(".//PortSessionInfo/PortConnectionInfo");
                                                            while (connectionIterator.hasNext()) {
                                                              var connectionNode = connectionIterator.next();
                                                              var srcNID = connectionNode.getAttribute("jsxsrcnid");
                                                              var connWinId = connectionNode.getAttribute("jsxwindowid");

                                                              if (!jsx3.util.strEmpty(srcNID)) {
                                                                if (featureName == "IPVSClientDisplayConfig" && connWinId != selectedWindowId) {
                                                                }
                                                                else {
                                                                  var stringNID = "<NID>" + srcNID + "</NID>";
                                                                  var nodeNID = new jsx3.xml.Document().loadXML(stringNID);
                                                                  NIDList.appendChild(nodeNID);
                                                                }
                                                              }
                                                            }
                                                            jsx3.sleep((function () {
                                                              topic_Publish(tMediaController, 'ConnectedMediaSelection', NIDList);
                                                            }
                                                                        ),
                                                            null, this);

                                                          }


                                                        });

     topic_Set(tMediaController, "PORTCONNECTIONUPDATE_ID", PORTCONNECTIONUPDATE_ID);

     // subscription to listen to PORTCONNECTIONSTATEUPDATE
     var PORTCONNECTIONSTATEUPDATE_ID =  eventApp.subscribe("PORTCONNECTIONSTATEUPDATE", null, this,
                                                            function (EventVar, EventVal) {
                                                              var objEvent = EventVal;

                                                              var portNID = objEvent.PortNID;
                                                              var notifySubscriptionID = objEvent.subscriptionID;
                                                              var notifyOperation = objEvent.Operation;
                                                              var notifySessionID = objEvent.SessionID;
                                                              var notifyConnectionID = objEvent.ConnectionID;
                                                              var notifyConnStatevalue = objEvent.Value;

                                                              // process only ConnectionStateUpdate notification
                                                              if (notifyOperation != "ConnectionStateUpdate") return;

                                                              // process only certain value of the connection state
                                                              if (notifyConnStatevalue != "SETUP_ERROR" &&
                                                                  notifyConnStatevalue != "START_ERROR" &&
                                                                  notifyConnStatevalue != "SOURCE_ERROR" &&
                                                                  notifyConnStatevalue != "MEDIA_ERROR" &&
                                                                  notifyConnStatevalue != "DEST_ERROR" &&
                                                                  notifyConnStatevalue != "STREAMING") return;


                                                              // look for the nid and subscription id
                                                              var endPointNode = cmpBlkEndPointContainer.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='" + portNID + "' and @jsxsubscriptionid='" + notifySubscriptionID + "']");
                                                              if (jsx3.util.strEmpty(endPointNode)) return;

                                                              // findright PortConnectionInfo Node
                                                              var portConnectionInfoNode = endPointNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid='" + notifySessionID + "']/PortConnectionInfo [@jsxconnectionid='" + notifyConnectionID + "']");
                                                              if (!jsx3.util.strEmpty(portConnectionInfoNode)) {
                                                                if (notifyConnStatevalue == "SETUP_ERROR" || notifyConnStatevalue == "START_ERROR" || notifyConnStatevalue == "SOURCE_ERROR" || notifyConnStatevalue == "MEDIA_ERROR" || notifyConnStatevalue == "DEST_ERROR") {
                                                                  portConnectionInfoNode.setAttribute("jsxerrorimg", "images/icons/icon_error_small.png");
                                                                  portConnectionInfoNode.setAttribute("jsxerrorcode", objEvent.ErrorCode);
                                                                  portConnectionInfoNode.setAttribute("jsxerrordata", objEvent.ErrorData);
                                                                }
                                                                else if (notifyConnStatevalue == "STREAMING") {
                                                                  portConnectionInfoNode.removeAttribute("jsxerrorimg");
                                                                  portConnectionInfoNode.setAttribute("jsxerrorcode","");
                                                                  portConnectionInfoNode.setAttribute("jsxerrordata","");
                                                                }
                                                                portConnectionInfoNode.setAttribute("jsxconnectionstate", notifyConnStatevalue);
                                                              }

                                                              cmpBlkEndPointContainer.BlkJSX.insertRecordNode(endPointNode, null, false);


                                                              var strXMLCacheId = cmpBlkEndPointContainer.BlkJSX.getXMLId();
                                                              var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                              cmpBlkEndPointContainer.onResponse(objXML);

                                                            });

     topic_Set(tMediaController, "PORTCONNECTIONSTATEUPDATE_ID", PORTCONNECTIONSTATEUPDATE_ID);

    // On Refresh endpoints
    eventMediaController.subscribe("RefreshEndPoints", null, this,
                                    function(eventVar, eventVal) {
                                      var endPointIterator = cmpBlkEndPointContainer.BlkJSX.getXML().selectNodeIterator("//record");
                                      while(endPointIterator.hasNext()){
                                        var portNode = endPointIterator.next();
                                        var portNID = portNode.getAttribute("jsxid");
                                        updateEndPointSession(cmpMediaController, portNID);
                                      }                                     
                                    }
                                  );

    // On selection of end point move to CONNECT mode and set the type of selected endpoint
    eventMediaController.subscribe("EndPointSelected", null, this,
                                    function (eventVar, eventVal) {
                                      if (eventVal != "NONE") {
                                        topic_Publish(tMediaController, 'ConnectOrAdd', 'CONNECT');
                                        cmpBlkEndPointContainer.BlkJSX.deselectAllConnections();
                                        var endPointNode = cmpBlkEndPointContainer.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='" + eventVal + "']");
                                        cmpBlkEndPointContainer.BlkJSX.deselectAllConnections();
                                        var feature = endPointNode.getAttribute("jsxfeature");
                                        var featurename = endPointNode.getAttribute("jsxfeaturename");
                                        if (jsx3.util.strEmpty(featurename)) {
                                          // reset the variables
                                          resetWindowingVars();
                                        }
                                        var endPointType = endPointNode.getAttribute("jsxportType");
                                        var streamto = endPointNode.getAttribute("jsxstreamto");
                                        //close the windowing dialog if any
                                        closeWindowingDialog();

                                        topic_Set(tMediaController, "EndPointSelectedType", endPointType);
                                        topic_Set(tMediaController, "EndPointSelectedStreamTo", streamto);
                                        var selected = endPointNode.getAttribute("jsxselected");
                                        if (jsx3.util.strEmpty(selected)) {
                                          topic_Set(tMediaController, "EndPointSelected", "NONE");
                                          topic_Set(tMediaController, "EndPointSelectedType", "NONE");
                                          topic_Set(tMediaController, "EndPointSelectedStreamTo", "NONE");
                                          return;
                                        }
                                        else {
                                          var NIDList = topic_GetNode(tMediaController, 'NIDList');
                                          NIDList.removeChildren();
                                          var connectionIterator = endPointNode.selectNodeIterator(".//PortSessionInfo/PortConnectionInfo");
                                          while (connectionIterator.hasNext()) {
                                            var connectionNode = connectionIterator.next();
                                            var srcNID = connectionNode.getAttribute("jsxsrcnid");
                                            if (!jsx3.util.strEmpty(srcNID)) {
                                              var stringNID = "<NID>" + srcNID + "</NID>";
                                              var nodeNID = new jsx3.xml.Document().loadXML(stringNID);
                                              NIDList.appendChild(nodeNID);
                                            }
                                          }
                                          topic_Publish(tMediaController, 'ConnectedMediaSelection', NIDList);
                                        }

                                        if (endPointType == "StreamDst" ||endPointType == "StreamIO") {
                                          var buttonSelectionMode = topic_Get(tMediaController, "ButtonSelection");
                                          if ((buttonSelectionMode == "Destination") &&
                                               ((buttonSelectionMode != "Live") || (buttonSelectionMode != "Recorded"))) {
                                            var tabButton = topic_Get(tMediaController, "LiveOrRecorded");
                                            topic_Publish(tMediaController, "ButtonSelection", tabButton);
                                          }
                                          //topic_Set(tMediaController, "SelectCanvasWindow", "NONE");
                                          topic_Publish(tMediaController, "Windowing", eventVal);
                                        }
                                        else {
                                          topic_Publish(tMediaController, "ButtonSelection", 'Destination');
                                        }
                                      }
                                    }
                                );

    //Delete the endpoint from Left Panel
    eventMediaController.subscribe("DeleteTarget", null, this,
                                      function(eventVar, eventVal) {                                  
                                        var endPointPanelXML= cmpBlkEndPointContainer.BlkJSX.getXML();
                                        var endpointNode = endPointPanelXML.selectSingleNode(".//record[@jsxid ='" +eventVal+ "']");
                                        endPointPanelXML.removeChild(endpointNode); 
                                        var strXMLCacheId = cmpBlkEndPointContainer.BlkJSX.getXMLId();
                                        var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                        cmpBlkEndPointContainer.onResponse(objXML);
                                        topic_Set(tMediaController, "EndPointSelectedType", "NONE");
                                        topic_Set(tMediaController, "EndPointSelectedStreamTo", "NONE");
                                        topic_Set(tMediaController, "EndPointSelected", "NONE");
                                        resetWindowingVars();                                                         
                                      }
                                  );
                                  
    eventMediaController.subscribe("SessionInfo", null, this,
                                    function(eventVar, eventVal) {                            
                                        var sessionID = eventVal;
                                        var endPointPanelXML = cmpBlkEndPointContainer.BlkJSX.getXML();
                                        var recordNode = endPointPanelXML.selectSingleNode(".//record[.//PortSessionInfo/@jsxsessionid ='" + eventVal + "']"); 
                                        var endPointNID = recordNode.getAttribute("jsxid");
                                        var sessionNode = recordNode.selectSingleNode(".//PortSessionInfo[@jsxsessionid ='" + eventVal + "']"); 
                                        var sessionName = sessionNode.getAttribute("jsxsessionname");
                                        // create topic and event listener for each enpoint block
                                        var tEndPointSession = endPointNID + "__"+ sessionID;
                                        var tEndPointSessionExists = topic_Exists(tEndPointSession);
                                        if(!tEndPointSessionExists){
                                          var tEndPointSessionDoc = topic_Create(tEndPointSession);
                                          var eventEndPointSession = new EventListner(tEndPointSession);
                                        }  
                                        topic_Publish(tEndPointSession, "SessionID", sessionID);                                     
                                        dlgSessionMonitor_Create(cmpMediaController,tEndPointSession, eventEndPointSession,sessionID, sessionName);
                                    }
                                  );
                   

     eventMediaController.subscribe("ConnectOrAdd", "ADD", this,
                                    function(eventVar, eventVal) {
                                      // reset the variables
                                      resetWindowingVars();
                                      return;                                    
                                    }
                                   );
                                                                   
     eventMediaController.subscribe("Windowing", null, this,
                                      function(eventVar, eventVal) {                            
                                        var endPointNID = eventVal;
                                        var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                        if(jsx3.util.strEmpty(recordNode))return;
                                        if(jsx3.util.strEmpty( cmpBlkEndPointContainer.allowMultiSelection))
                                        cmpBlkEndPointContainer.BlkJSX.deselectAllRecords();
                                        recordNode.setAttribute("jsxselected","1"); 
                                        cmpBlkEndPointContainer.BlkJSX.repaint();
                                        //topic_Publish(tMediaController,"EndPointSelected", endPointNID);  
                                        var featurename =recordNode.getAttribute("jsxfeaturename");
                                        if(jsx3.util.strEmpty( featurename)){
                                          // reset the variables
                                          resetWindowingVars();
                                          topic_Publish(tMediaController, "OpenWindowingPcOrRx", "No");
                                          return;
                                        }                                        
                                        var featureValue = recordNode.getAttribute('jsxfeature');
                                        if (jsx3.util.strEmpty(featureValue)) return;

                                        if (featureValue == 'XP200DisplayConfig') {
                                          // publish to tMediaController to get the display config for xp rx
                                          topic_Publish(tMediaController, "GetDisplayPortConfig", newServerDate());
                                        }
                                        else if (featureValue == 'IPVSClientDisplayConfig') {
                                          // publish to tMediaController to get the display status for UMC
                                          topic_Publish(tMediaController, "GetDisplayPortStatus", (new Date()).getTime());
                                        }

                                      }
                                    );                             
                                  
                 
     eventMediaController.subscribe("DeleteConnection", null, this,
                                    function(eventVar, eventVal) { 
                                       var connectionId =  eventVal;
                                       var endPointPanelXML = cmpBlkEndPointContainer.BlkJSX.getXML();
                                       var sessionNode = endPointPanelXML.selectSingleNode(".//record/PortSessionInfo[.//PortConnectionInfo/@jsxconnectionid ='" + eventVal + "']");
                                       var sessionId = sessionNode.getAttribute("jsxsessionid");
                                       var sessionName = sessionNode.getAttribute("jsxsessionname"); 
                                       var reqSessionDeleteConn = new ReqSessionDeleteConnRequest(sessionId ,connectionId ,
                                                                                                    { action: "FNCCALL", context: this,
                                                                                                      fnc: function(respCode, respVal) {
                                                                                                        var tt = "ttt";
                                                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                                                        var Response = respVal;
//                                                                                                          topic_Publish(tEndPointSessionConn, "DeleteEndPointSessionConn", sessionConnID);  
                                                                                                        }
                                                                                                        else {
                                                                                                          log.error("MediaList Ctrl: StopSession Connection: EndPoint Topic: " + tMediaController + ": Stop for SessionID: " + sessionId + ", Conenction ID: " + connectionId + ", Session Name:" + sessionName + " request failed.\n");
                                                                                                        }
                                                                                                      }
                                                                                                    }
                                                                                                  );
                                        reqSessionDeleteConn.reqParams = new Object();
                                        reqSessionDeleteConn.reqParams["Where_xpath"] = "";
                                        reqSessionDeleteConn.sendRequest(this);                                                                                   
                                      }
                                    );

    // function to update the endpoint session
    function updateEndPointSession(cmpMediaController, updatePortNID) {
      //get the port details
      var endPointTitle = "";
      var endPointType = "";
      var tEndPoint = "";
      var endPointBlkJSX = "";
      var cmpEndPoint = "";  
      var wherePermissions = "";
      var endPointNode = cmpBlkEndPointContainer.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='" +updatePortNID+ "']");
      var endPointTitle = endPointNode.getAttribute("jsxtext");
      var endPointType = endPointNode.getAttribute("jsxportType");
      if(endPointType == "StreamDst"){
        endPointType = "DstPort";
        wherePermissions = "STREAMTO";
      }
      else{
        endPointType = "SrcPort";
        wherePermissions = "VIEWFROM";
      }
      var SubscriptionID = endPointNode.getAttribute("jsxsubscriptionid");
      var selected = endPointNode.getAttribute("jsxselected");
      
      // get new status for the port
      var reqGetPort = new ReqSessionGetRequest("GetPort",
                                                  { action: "FNCCALL", context: null,
                                                    fnc: function (respCode, respVal) {
                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                        // making sure to get the specified title port only
                                                        var endPointNode = respVal.selectSingleNode(".//Port[@title ='" + endPointTitle + "']");
                                                        if (!jsx3.util.strEmpty(endPointNode)) {
                                                          var endPointType = endPointNode.getAttribute("portType");
                                                          var endPointNID = endPointNode.getAttribute("NID");

                                                          if (EndPointXSLFile.hasError()) {
                                                            var UriRes = cmpBlkEndPointContainer.BlkJSX.getUriResolver();
                                                            if (jsx3.util.strEmpty(UriRes)) return;
                                                            xslUriRes = UriRes.resolveURI("xsl/EndPoint.xsl");
                                                            if (jsx3.util.strEmpty(xslUriRes)) return;
                                                            EndPointXSLFile = PNameSpace.loadInclude(xslUriRes, "_Update", "xsl");
                                                          }

                                                          if (EndPointXSLFile.hasError()) {
                                                            log.error("Get Port request::EndPoint.xsl:: has error\n");
                                                            return;
                                                          }

                                                          var xslDoc = jsx3.xml.XslDocument.wrap(EndPointXSLFile);
                                                          var recordNode = xslDoc.transformToObject(respVal);
                                                          recordNode.setAttribute("jsxsubscriptionid", SubscriptionID);
                                                          var selectedEndPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                          if (selectedEndPointNID == endPointNID) {
                                                            recordNode.setAttribute("jsxselected", "1");
                                                            var NIDList = topic_GetNode(tMediaController, 'NIDList');
                                                            NIDList.removeChildren();
                                                            var connectionIterator = recordNode.selectNodeIterator(".//PortSessionInfo/PortConnectionInfo");
                                                            while (connectionIterator.hasNext()) {
                                                              var connectionNode = connectionIterator.next();
                                                              var srcNID = connectionNode.getAttribute("jsxsrcnid");
                                                              if (!jsx3.util.strEmpty(srcNID)) {
                                                                var stringNID = "<NID>" + srcNID + "</NID>";
                                                                var nodeNID = new jsx3.xml.Document().loadXML(stringNID);
                                                                NIDList.appendChild(nodeNID);
                                                              }
                                                            }
                                                            topic_Publish(tMediaController, 'ConnectedMediaSelection', NIDList);
                                                          }
                                                          cmpBlkEndPointContainer.BlkJSX.insertRecordNode(recordNode, null, false);
                                                          if (endPointType == "StreamDst" || endPointType == "StreamIO") {
                                                            // For Tx to Rx conversion unfreez the endpoint.
                                                            var record = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                                            if (!jsx3.util.strEmpty(record)) {
                                                              var unselectable = record.getAttribute("jsxunselectable");
                                                              if (!jsx3.util.strEmpty(unselectable)) {
                                                                record.removeAttribute("jsxunselectable");
                                                              }
                                                            }
                                                            //Connection Highlight for window
                                                            var windowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                            if (!jsx3.util.strEmpty(windowId) && windowId != "NONE") {
                                                              cmpBlkEndPointContainer.BlkJSX.deselectAllConnections(record);
                                                              var connectionNode = record.selectSingleNode(".//PortSessionInfo/PortConnectionInfo[@jsxwindowid ='" + windowId + "']");
                                                              if (!jsx3.util.strEmpty(connectionNode)) {
                                                                connectionNode.setAttribute("jsxselected", "1");
                                                              }
                                                            }
                                                          }
                                                          else if (endPointType == "StreamSrc") {
                                                            var recordId = recordNode.getAttribute("jsxid");
                                                            if (recordId == topic_Get(tMediaController, "EndPointSelected")) {
                                                              topic_Publish(tMediaController, 'ClearEndPoints', 'NONE');
                                                            }
                                                          }
                                                          var strXMLCacheId = cmpBlkEndPointContainer.BlkJSX.getXMLId();
                                                          var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                          cmpBlkEndPointContainer.onResponse(objXML);
                                                        }
                                                        else {
                                                          var objXML = cmpBlkEndPointContainer.BlkJSX.getXML();
                                                          var recordNode = objXML.selectSingleNode(".//record[@jsxtext='" + endPointTitle + "']");
                                                          if (!jsx3.util.strEmpty(recordNode)) {
                                                            var recordId = recordNode.getAttribute("jsxid");
                                                            if (recordId == topic_Get(tMediaController, "EndPointSelected")) {
                                                              topic_Publish(tMediaController, 'ClearEndPoints', 'NONE');
                                                            }
                                                            cmpBlkEndPointContainer.BlkJSX.deleteRecord(recordId, false);
                                                            cmpBlkEndPointContainer.repaint();
                                                          }
                                                          log.error("GetEndpoint: No Port found with Port title/name: " + endPointTitle);
                                                        }
                                                      }
                                                      else {
                                                        log.error("Get Port request failed for Port: " + endPointTitle);
                                                      }
                                                    }
                                                  }
                                                 );
      reqGetPort.ServiceName = "SessionService";
      var reqParams = new Object;
      // added regular expression ^ $ to getthe exact string
      reqParams["ObjectList_WhereTextSubStr"] = "^" + endPointTitle + "$";
      reqParams["ObjectList_SelectFormat"] = "BASICCONNECTIONINFO";
      reqParams["ObjectList_Type"] = endPointType;
      reqParams["ObjectList_SubscriptionID"] = SubscriptionID;
      reqParams["ObjectList_WherePermissions"] = wherePermissions;
      reqGetPort.reqParams = reqParams;
      // send the request
      reqGetPort.sendRequest(this);

    } // end updateEndPointSession


    //Get Port status for endpoint and add it to the left panel                             
    function addEndPoint(cmpMediaController, endPointTitle, endPointType) {
      // check if the end point already exist
      var addFlag = true;
      var endPointNode = cmpBlkEndPointContainer.BlkJSX.getXML().selectSingleNode(".//record[@jsxtext ='" +endPointTitle+ "']");
      if (!jsx3.util.strEmpty(endPointNode)) {
      // end port already exists
        addFlag = false;       
      }
      // add the endpoint
      if (addFlag) {
        var wherePermissions = "";
        if(endPointType == "DstPort"){
          wherePermissions = "STREAMTO";
        }
        else if (endPointType == "SrcPort") {
          wherePermissions = "VIEWFROM";
        }
        var SubscriptionID = "EP_" + endPointType + "_Subscription_" + getUUID();
        // get the port details send request
        var reqGetPort = new ReqSessionGetRequest("GetPort",
                                                    { action: "FNCCALL", context: null,
                                                      fnc: function (respCode, respVal) {
                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                          var endPointNode = respVal.selectSingleNode(".//Port[@title='" + endPointTitle + "']");
                                                          if (!jsx3.util.strEmpty(endPointNode)) {
                                                            var endPointType = endPointNode.getAttribute("portType");
                                                            var endPointNID = endPointNode.getAttribute("NID");

                                                            if (EndPointXSLFile.hasError()) {
                                                              var UriRes = cmpBlkEndPointContainer.BlkJSX.getUriResolver();
                                                              if (jsx3.util.strEmpty(UriRes)) return;
                                                              xslUriRes = UriRes.resolveURI("xsl/EndPoint.xsl");
                                                              if (jsx3.util.strEmpty(xslUriRes)) return;
                                                              EndPointXSLFile = PNameSpace.loadInclude(xslUriRes, "_Update", "xsl");
                                                            }
                                                            if (EndPointXSLFile.hasError()) {
                                                              log.error("addEndPoint::Get Port request::EndPoint.xsl:: has error\n");
                                                              return;
                                                            }  
                                                            var xslDoc = jsx3.xml.XslDocument.wrap(EndPointXSLFile);
                                                            var recordNode = xslDoc.transformToObject(respVal);
                                                            recordNode.setAttribute("jsxsubscriptionid", SubscriptionID);
                                                            cmpBlkEndPointContainer.BlkJSX.insertRecordNode(recordNode, null, false);
                                                            var strXMLCacheId = cmpBlkEndPointContainer.BlkJSX.getXMLId();
                                                            var objXML = PNameSpace.getCache().getDocument(strXMLCacheId);
                                                            cmpBlkEndPointContainer.onResponse(objXML);
                                                            //For Default end point- Mediaclient PC
                                                            var checkEndPointSelection = topic_Get(tMediaController, "EndPointSelected");
                                                            if (checkEndPointSelection == "") {
                                                              var userEndPointTitle = topic_Get(tApp, "DefaultPortTitle");
                                                              var connectMode = topic_Get(tMediaController, 'ConnectOrAdd');
                                                              if (((clientType == "INSTALLED") && (userEndPointTitle == endPointTitle)) || ((clientType == "WEB") && (connectMode == "CONNECT"))) {
                                                                recordNode.setAttribute("jsxselected", "1");
                                                                var streamto = recordNode.getAttribute("jsxstreamto");
                                                                topic_Set(tMediaController, "EndPointSelectedType", endPointType);
                                                                topic_Set(tMediaController, "EndPointSelectedStreamTo", streamto);
                                                                topic_Publish(tMediaController, "EndPointSelected", endPointNID);
                                                              }
                                                              if ((clientType == "WEB") && (connectMode == "ADD")) {
                                                                topic_Set(tMediaController, "EndPointSelectedType", "NONE");
                                                                topic_Set(tMediaController, "EndPointSelectedStreamTo", "NONE");
                                                                topic_Publish(tMediaController, "EndPointSelected", "NONE");
                                                              }
                                                            }
                                                          }
                                                          else {
                                                            log.error("GetEndpoint: No Port found with Port title/name: " + endPointTitle);
                                                          }
                                                        }
                                                        else {
                                                          log.error("Get Port request failed for Port: " + endPointTitle);
                                                        }
                                                      }
                                                    }
                                                   );
        reqGetPort.ServiceName = "SessionService";
        var reqParams = new Object;
        // added regular expression ^ $ to getthe exact string
        reqParams["ObjectList_WhereTextSubStr"] = "^" + endPointTitle + "$";
        reqParams["ObjectList_SelectFormat"] = "BASICCONNECTIONINFO";
        reqParams["ObjectList_Type"] = endPointType;
        reqParams["ObjectList_SubscriptionID"] = SubscriptionID;
        reqParams["ObjectList_WherePermissions"] = wherePermissions;
        reqGetPort.reqParams = reqParams;
        // send the request
        reqGetPort.sendRequest(this);
      }
    }

    //Populate EndPoint Controller panel with default end points
    eventMediaController.subscribe("UpdateControlTarget", null, this,
                                    function(eventVar, eventVal) {
                                      var defaultEndPointTitle = "";
                                      if (clientType == "INSTALLED") {
                                        var defaultEndPointTitle = topic_Get(tApp, "DefaultPortTitle");
                                        addEndPoint(cmpMediaController, defaultEndPointTitle, "DstPort");
                                      }
                                      // Create End points set by the user via Admin UI , For Each end point send GetPortStatus Request, Send Response to create and paint end Point
                                      var nodeUserEndPointControllerData = topic_GetNode(tApp, "//ServiceClientData//User//EndPointControllerData");
                                      if (nodeUserEndPointControllerData) {
                                        var configurationMode = topic_Get(tApp, "EndPointConfigurable");
                                        if (configurationMode == "false") {
                                          //Disable the Add button                                              
                                          topic_Publish(tMediaController, "AddEnabledDisabled", 'DISABLED');
                                        }
                                        else {
                                          topic_Publish(tMediaController, "AddEnabledDisabled", 'ENABLED');
                                        }
                                        var endPointIterator = nodeUserEndPointControllerData.selectNodeIterator(".//EndPointEntry");
                                        if (endPointIterator != null) {
                                          while (endPointIterator.hasNext()) {
                                            var endPoint = endPointIterator.next();
                                            var endPointTitle = endPoint.getAttribute("Title");
                                            var endPointType = endPoint.getAttribute("Type");
                                            // if endpoint type is null of empty forcing it to be DstPort
                                            // right now endpoint can have only dst port.
                                            if( jsx3.util.strEmpty(endPointType) ){
                                              endPointType = "DstPort";
                                            }
                                            if (!jsx3.util.strEmpty(endPointTitle)) {
                                               if (endPointTitle != defaultEndPointTitle) {
                                                addEndPoint(cmpMediaController, endPointTitle ,endPointType);
                                              }
                                            }
                                          }

                                        }
                                      }
                                    }
                              );

    // To unselect all end points when any one is selected
    eventMediaController.subscribe("ClearEndPoints", null, this,
                                    function(eventVar, eventVal) {
                                      topic_Set(tMediaController, "EndPointSelectedType", "NONE");
                                      topic_Set(tMediaController, "EndPointSelectedStreamTo", "NONE");
                                      topic_Publish(tMediaController, "EndPointSelected", "NONE");
                                      cmpBlkEndPointContainer.BlkJSX.deselectAllRecords(); 
                                      cmpBlkEndPointContainer.BlkJSX.deselectAllConnections();
                                      cmpBlkEndPointContainer.BlkJSX.repaint();                               
                                    }
                                );

    //Create MediaList as a subcomponent of Media Controller
    var cmpBlkMediaList = new CmpContainer([cmpMediaControllerBlkJSX, "blkMediaList"],
                                           [],
                                           null);
    cmpBlkMediaList.SubCmpArr = cmpMediaList_Create(cmpMediaController, tMediaController, eventMediaController);

    // Header Block - visible by default - To be hidden when Media list is a subcomponent of media controller.
    var cmpBlkMediaListHeader = getJsxByName([cmpMediaControllerBlkJSX, "blkMediaListHeader"]);
    cmpBlkMediaListHeader.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
    var lytMain = getJsxByName([cmpMediaControllerBlkJSX, "lytMain"]);
    lytMain.setRows("0,34,*", true);

     //event Handler for media selected from Media List
    eventMediaController.subscribe("MediaListSelectionInfo", null, this,
                                    function(eventVar, eventVal) {
                                      var mediaInfo = eventVal;
                                      var modeConnectAdd = topic_Get(tMediaController, "ConnectOrAdd");
                                      if (modeConnectAdd == 'CONNECT') {
                                        //In Connect Mode
                                        var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                        if (!jsx3.util.strEmpty(endPointNID) && endPointNID != "NONE") {
                                          //EndPoint selected
                                          var endPointObj = getEndPointInfo(endPointNID);
                                          var userEndPointTitle = topic_Get(tApp, "DefaultPortTitle");
                                          if (!jsx3.util.strEmpty(mediaInfo.NID)) {
                                            if (endPointObj.streamto == "false") {
                                              log.info("End point does not have Streamto permissions. EndPoint NID:"+endPointObj.NID + " EndPoint Name:" +endPointObj.Name);
                                              return false;
                                            }
                                            //Media Selected
                                            if (endPointObj.type == "DstPort" || endPointObj.type == "StreamDst" || endPointObj.type == "StreamIO" || endPointObj.type == "IOPort") {
                                              //End Point is destination                                              
                                              if (mediaInfo.type == "SrcPort" || mediaInfo.type == "StreamSrc" || mediaInfo.type == "GrpFile" || mediaInfo.type == "PlyFile") {
                                                // Media type is compatible to End Point

                                                //Check if State ready and Type compatible
                                                if ((endPointObj.state != "Ready") && (endPointObj.state != "Unmanaged")) {
                                                  winMediaControllerWindow.alert("Connection failed.Destination not Ready");
                                                  return false;
                                                }
                                                if ((mediaInfo.state != "Ready") && (mediaInfo.state != "Unmanaged")) {
                                                  winMediaControllerWindow.alert("Connection failed.Source not Ready");
                                                  return false;
                                                }
                                                if(endPointObj.NID == mediaInfo.NID){
                                                  winMediaControllerWindow.alert("Please select a different Source or Destination.Cannot connect the Media Source Destination to itself");
                                                  return false;
                                                }
                                                //Window content Params
                                                var windowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                // check if its a UnManned MediaClient detination and windowid is selected
                                                if ((endPointObj.feature == "IPVSClientDisplayConfig") && (jsx3.util.strEmpty(windowId) || windowId == "NONE")) {
                                                  winMediaControllerWindow.alert("Cannot connect: Please select a window from the canvas. ");
                                                  var NIDList = topic_GetNode(tMediaController, 'NIDList');
                                                  topic_Publish(tMediaController, 'ConnectedMediaSelection', NIDList);
                                                  return false;
                                                }

                                                if (endPointObj.maxInstanceCount > "1") {
                                                  if (mediaInfo.type != "GrpFile") {
                                                    //Selected End Point is a Pc
                                                    if (mediaInfo.streamType == "UDP") {
                                                      winMediaControllerWindow.alert("Cannot connect PC client to UDP Stream type");
                                                      return false;
                                                    }
                                                  }

                                                  //if UnManned client window is selected and MediaGroupFile is selected throw an alert
                                                  if (mediaInfo.type == "GrpFile" && windowId != "NONE") {
                                                    winMediaControllerWindow.alert("Cannot connect UnManned client window to Group file. Please select a single file for connection");
                                                    return false;
                                                  }
                                                }
                                                else if (endPointObj.maxInstanceCount == "1") {
                                                  //Selected End Point is a Rx
                                                  if (mediaInfo.type == "GrpFile") {
                                                    winMediaControllerWindow.alert("Cannot connect a Rx to Group file. Please select a single file for connection");
                                                    return false;
                                                  }
                                                  else if (endPointObj.streamType != mediaInfo.streamType) {
                                                    winMediaControllerWindow.alert("Cannot connect a " + endPointObj.streamType + " Rx to " + mediaInfo.streamType + " Tx");
                                                    return false;
                                                  }
                                                }
                                                
                                                // Create View/Playback request Object                                            
                                                var objView = new Object;
                                                if(mediaInfo.type == "SrcPort" || mediaInfo.type == "StreamSrc" || mediaInfo.type == "StreamIO" ||  mediaInfo.type == "IOPort"){
                                                  objView.reqType ="View";
                                                  objView.SrcPortNID = mediaInfo.NID;
                                                  objView.SrcPortName = mediaInfo.Name;
                                                }
                                                else if (mediaInfo.type == "GrpFile") {
                                                  objView.reqType = "PlayBackGrp";
                                                  objView.FileNID = mediaInfo.NID;
                                                }
                                                else if (mediaInfo.type == "PlyFile") {
                                                  objView.reqType = "PlayBackSrc";
                                                  objView.FileNID = mediaInfo.NID;
                                                }

                                                objView.DstPortNID = endPointObj.NID;
                                                objView.DstportName = endPointObj.Name;
                                                objView.DstEntityId = "";
                                                objView.SrcEntityId = "";
                                                objView.ConnName = mediaInfo.Name;
                                                objView.callBackSuccess = viewOrPlayBackSuccessFnc;
                                                objView.callBackErr = viewOrPlayBackFailFnc;
                                                objView.mediaInfo = mediaInfo;
                                                objView.restartSession = "";
                                                objView.WindowId = "";
                                                objView.CanvasId = "";
                                                objView.PVREnable = topic_Get(tMediaController, 'PVREnable');
                                                objView.PVRTrimLength = topic_Get(tMediaController, 'PVRTrimLength');
                                                
                                                var ThisClass = this;
                                                if (!jsx3.util.strEmpty(windowId) && windowId != "NONE") {
                                                  //Set the WindowId of View Request Obj
                                                  objView.WindowId = windowId;
                                                  var nodeCanvas = cmpBlkPcCanvasDisplay.BlkJSX.getXML().selectSingleNode(".//record/Canvas[.//Window/@id ='" + windowId + "']");
                                                  var canvasId = nodeCanvas.getAttribute("id");
                                                  if (!jsx3.util.strEmpty(canvasId)) {
                                                    objView.CanvasId = canvasId;
                                                  }
                                                  else {
                                                    objView.CanvasId = "DstPort";
                                                  }
                                                }

                                                if (endPointObj.maxInstanceCount == "1") {
                                                  // Selected End Point is Rx
                                                  //For Rx always use the same session and connection id                                                    
                                                  objView.SessionId = jsx3.util.strEmpty(endPointObj.sessionId) ? "Session_" + endPointObj.Name : endPointObj.sessionId;
                                                  objView.ConnId = jsx3.util.strEmpty(endPointObj.connId) ? "Conn_" + endPointObj.Name : endPointObj.connId;
                                                  objView.restartSession = true;
                                                }
                                                else {
                                                  if (!jsx3.util.strEmpty(windowId) && windowId != "NONE") {
                                                    //Pc with windowing
                                                    objView.SessionId = jsx3.util.strEmpty(endPointObj.sessionId) ?  "Session_" + endPointObj.Name + "_" + windowId : endPointObj.sessionId;
                                                    objView.ConnId = jsx3.util.strEmpty(endPointObj.connId) ?  "Conn_" + endPointObj.Name + "_" + windowId : endPointObj.connId;
                                                    objView.restartSession = true;
                                                  }
                                                  else {
                                                    //Pc without windowing
                                                    var selectedSessionId = topic_Get(tMediaController, "EndPointSessionSelected");
                                                    if (!jsx3.util.strEmpty(selectedSessionId) && (selectedSessionId != "NONE")) {
                                                      objView.SessionId = selectedSessionId;
                                                    }
                                                    else {
                                                      objView.SessionId = "Session_" + getUUID();
                                                    }
                                                    var selectedSessionConnId = topic_Get(tMediaController, "EndPointSessionConnSelected");
                                                    if (!jsx3.util.strEmpty(selectedSessionConnId) && (selectedSessionConnId != "NONE")) {
                                                      objView.ConnId = selectedSessionConnId;
                                                    }
                                                    else {
                                                      objView.ConnId = "Conn_" + getUUID();
                                                    }
                                                  }
                                                }
                                                agtCollaborationCtrl.viewOrPlayBack.call(this, tMediaController, objView);
                                              }
                                              else {
                                                //  Dst-Dst type of selection
                                                winMediaControllerWindow.alert("Please select a Source - Destination pair to start connection");
                                              }
                                            } //End Dst EndPoint Selection
                                          }
                                          else {
                                            //Media not selected
                                            winMediaControllerWindow.alert("Please select a Media");
                                          }

                                        }
                                        else {
                                          //End Point not selected
                                          winMediaControllerWindow.alert("Please select an End Point");
                                        }

                                      }
                                      else if ((modeConnectAdd == 'ADD') && (mediaInfo.type == "SrcPort" || mediaInfo.type == "DstPort")) {
                                        //In Add mode
                                        addEndPoint(cmpMediaController, mediaInfo.Name, mediaInfo.type);
                                      }
                                    }
                                );
                                
    viewOrPlayBackSuccessFnc = function(Response,mediaInfo){
                                                    var dstPort = Response.selectSingleNode(".//EntityList/Entity[@type='DstPort']/Port");
                                                    var endPointNID = dstPort.getAttribute("resourceNID");
                                                    if (isResponseErrored(Response,endPointNID)) {
                                                      return;
                                                    } 
                                                    var userEndPointTitle = topic_Get(tApp, "DefaultPortTitle");
                                                    var endPointObj = getEndPointInfo(endPointNID);
                                                    if ((clientType == "INSTALLED") && (userEndPointTitle == endPointObj.Name)) {
                                                      // create a dynamic topic for each media player window
                                                      var sessionNode = Response.selectSingleNode("//Session");
                                                      if (!sessionNode) {
                                                        log.error(":cmpBlkMediaController:viewOrPlayBackSuccessFnc:Response sessionNode is null. Response: " + Response.toString());
                                                        return;
                                                      }
                                                      var sessionId = sessionNode.getAttribute("id");
                                                      mediaInfo.sessionId = sessionId;
                                                      mediaInfo.SessionOwnerUserJID = sessionNode.selectSingleNode(".//Properties/@ownerUserJID").getValue();
                                                      //send StartSession by call ctrlAgent 'START'
                                                      log.info("cmpBlkMediaController.viewOrPlayBackSuccessFnc: agtDisplay.launchPlayerBySessionId:Session Id: " + mediaInfo.sessionId+":tMediaPlayer:null");
                                                      agtDisplay.launchPlayerBySessionId(mediaInfo, true);
                                                    }
                                                };
    viewOrPlayBackFailFnc = function(Response){  
                                                var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                log.error("View Command request failed as : " + Response);
                                              };
    function isResponseErrored(respVal,endPointNID){
      //var connectionNode = respVal.selectSingleNode(".//PortConnectionInfo");
      var connectionNode = respVal.selectSingleNode(".//Connection");
      var errorCode = connectionNode.getAttribute("errorCode");
      var errorData = connectionNode.getAttribute("errorData");
      var state = connectionNode.getAttribute("state");   
      if(!jsx3.util.strEmpty(errorCode)){ 
        winMediaControllerWindow.alert("Session cannot be established \n Error Code :" + errorCode + " \n Description :" + errorData + "\n State :" + state);
        return true;
      }
      else{
       return false;
      }         
    }

    eventMediaController.subscribe("SelectCanvasWindow", null, this,
                                    function (EventVar, EventVal) {
                                      var windowNode = cmpBlkPcCanvasDisplay.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window[@id='" + EventVal + "']");
                                      if (jsx3.util.strEmpty(windowNode)) return;
                                      var selected = windowNode.getAttribute("jsxselected");
                                      var recordList = cmpBlkWindowListPanel.BlkJSX.getXML();
                                      var selectedListWindow = recordList.selectSingleNode(".//record[@jsxid='" + EventVal + "']");
                                      cmpBlkWindowListPanel.BlkJSX.deselectAllRecords();
                                      if (selected == "1") {
                                        selectedListWindow.setAttribute("jsxselected", "1");
                                        cmpBlkWindowListPanel.repaint();
                                        var windowNode = cmpBlkPcCanvasDisplay.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window[@id='" + EventVal + "']");
                                        if (!jsx3.util.strEmpty(windowNode)) {
                                          SetWindowPropertiesBtnState(windowNode);
                                        }
                                        //Highlight the connection in end point when window is selected
                                        var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                        if (!jsx3.util.strEmpty(endPointNID) && endPointNID != "NONE") {
                                          var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                          if (!jsx3.util.strEmpty(recordNode)) {
                                            cmpBlkEndPointContainer.BlkJSX.deselectAllConnections(recordNode);
                                            var NIDList = topic_GetNode(tMediaController, 'NIDList');
                                            NIDList.removeChildren();
                                            var connectionNode = recordNode.selectSingleNode(".//PortSessionInfo/PortConnectionInfo[@jsxwindowid ='" + EventVal + "']");
                                            if (!jsx3.util.strEmpty(connectionNode)) {
                                              connectionNode.setAttribute("jsxselected", "1");
                                              var srcNID = connectionNode.getAttribute("jsxsrcnid");
                                              if (!jsx3.util.strEmpty(srcNID)) {
                                                var stringNID = "<NID>" + srcNID + "</NID>";
                                                var nodeNID = new jsx3.xml.Document().loadXML(stringNID);
                                                NIDList.appendChild(nodeNID);
                                              }
                                            }
                                            topic_Publish(tMediaController, 'ConnectedMediaSelection', NIDList);
                                            cmpBlkEndPointContainer.repaint();
                                          }
                                        }
                                      }
                                      else {
                                        cmpBlkWindowListPanel.repaint();
                                        cmpBtnWinFullScreen.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                        cmpBtnWin1to1.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                        cmpBtnWinMute.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                        cmpBtnWinVisible.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                        cmpBtnWinKbm.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);

                                        topic_Set(tMediaController, "SelectCanvasWindow", "NONE");
                                        var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                        if (!jsx3.util.strEmpty(endPointNID) && endPointNID != "NONE") {
                                          var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                          if (!jsx3.util.strEmpty(recordNode)) {
                                            cmpBlkEndPointContainer.BlkJSX.deselectAllConnections(recordNode);
                                            var NIDList = topic_GetNode(tMediaController, 'NIDList');
                                            NIDList.removeChildren();
                                            var connectionIterator = recordNode.selectNodeIterator(".//PortSessionInfo/PortConnectionInfo");
                                            while (connectionIterator.hasNext()) {
                                              var connectionNode = connectionIterator.next();
                                              var srcNID = connectionNode.getAttribute("jsxsrcnid");
                                              if (!jsx3.util.strEmpty(srcNID)) {
                                                var stringNID = "<NID>" + srcNID + "</NID>";
                                                var nodeNID = new jsx3.xml.Document().loadXML(stringNID);
                                                NIDList.appendChild(nodeNID);
                                              }
                                            }
                                            cmpBlkEndPointContainer.repaint();
                                            topic_Publish(tMediaController, 'ConnectedMediaSelection', NIDList);
                                          }
                                        }
                                      }
                                    });
                                     
    eventMediaController.subscribe("SelectListWindow", null, this,
                                    function(EventVar, EventVal) { 
                                     var selectedListWindow = cmpBlkWindowListPanel.BlkJSX.getXML().selectSingleNode(".//record[@jsxid='"+EventVal+"']");  
                                     var selected = selectedListWindow.getAttribute("jsxselected");
                                     var windowNode = cmpBlkPcCanvasDisplay.BlkJSX.getXML().selectSingleNode(".//record/Canvas/Window[@id='"+EventVal+"']");
                                     cmpBlkPcCanvasDisplay.BlkJSX.deselectAllWindows();
                                     if(selected == "1"){
                                       if(!jsx3.util.strEmpty(windowNode)){
                                        windowNode.setAttribute("jsxselected","1");
                                       }
                                     }
                                     cmpBlkPcCanvasDisplay.repaint();
                                     topic_Publish(tMediaController, "SelectCanvasWindow",EventVal);
                                   });      
                                                            
    eventMediaController.subscribe("SelectConnection", null, this,
                                    function(EventVar, EventVal) {
                                    //Highlight the window in the Canvas when connection is selected.
                                      var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                      var windowNode = objXML.selectSingleNode(".//record/Canvas/Window[ConnectionContent/@connId ='" + EventVal +"']");
                                      cmpBlkPcCanvasDisplay.BlkJSX.deselectAllWindows();
                                      if(!jsx3.util.strEmpty(windowNode)){                                   
                                        windowNode.setAttribute("jsxselected","1");
                                        topic_Publish(tMediaController, "SelectCanvasWindow",windowNode.getAttribute("id"));
                                      }
                                      var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                      if(endPointNID == objXML.selectSingleNode(".//record").getAttribute("jsxid")){
                                        cmpBlkEndPointContainer.repaint();
                                      }
                                      cmpBlkPcCanvasDisplay.repaint();
                                    }
                                  );
    
   
    /**************** WINDOWING *****************/

     // subscription to listen to PORTDISPLAYCONFIGUPDATE
     // mainly for XP Rx for which we display the config
     var PORTDISPLAYCONFIGUPDATE_ID = eventApp.subscribe("PORTDISPLAYCONFIGUPDATE", null, this,
                                                          function (EventVar, EventVal) {
                                                            var objEvent = EventVal;
                                                            var notifyPortNID = objEvent.PortNID;
                                                            var portDisplayConfigNode = objEvent.PortDisplayConfigNode;
                                                            var notifySubscriptionID = objEvent.subscriptionID;
                                                            var notifyOperation = objEvent.Operation;

                                                            if (notifyOperation == "Update") {
                                                              var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                              if (notifyPortNID == endPointNID && notifySubscriptionID == topic_Get(tMediaController, 'PortDisplayConfigSubscriptionID')) {
                                                                var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                                                if (jsx3.util.strEmpty(recordNode)) return;

                                                                var featureValue = recordNode.getAttribute('jsxfeature');
                                                                if (jsx3.util.strEmpty(featureValue)) return;

                                                                if (featureValue == 'XP200DisplayConfig') {
                                                                  // update display config for xp rx
                                                                  //jsx3.sleep((function () {
                                                                    getWindowingData(portDisplayConfigNode);
                                                                  //}), null, this);
                                                                }
                                                                else if (featureValue == 'IPVSClientDisplayConfig') {
                                                                  // update display config for UMC
                                                                }

                                                              }
                                                            }
                                                          });

      topic_Set(tMediaController, "PORTDISPLAYCONFIGUPDATE_ID", PORTDISPLAYCONFIGUPDATE_ID);

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
                                                              var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                              if (notifyPortNID == endPointNID && notifySubscriptionID == topic_Get(tMediaController, 'PortDisplayStatusSubscriptionID')) {
                                                                var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                                                if (jsx3.util.strEmpty(recordNode)) return;

                                                                var featureValue = recordNode.getAttribute('jsxfeature');
                                                                if (jsx3.util.strEmpty(featureValue)) return;

                                                                if (featureValue == 'XP200DisplayConfig') {
                                                                  // update display status for xp rx
                                                                }
                                                                else if (featureValue == 'IPVSClientDisplayConfig') {
                                                                  // update display status for UMC
                                                                  showUMCWindowingStatus(portDisplayStatusNode);
                                                                }

                                                              }
                                                            }
                                                          });

    topic_Set(tMediaController, "PORTDISPLAYSTATUSUPDATE_ID", PORTDISPLAYSTATUSUPDATE_ID);

    var lytEndPointController = getJsxByName([cmpMediaControllerBlkJSX, "layoutEndPointController"]);
                                
    var cmpEndPointWindowing = new Component([cmpMediaControllerBlkJSX, "blkWindowingContainer"]);

    var cmpBtnRefreshWindowing = new CmpImageButton([cmpMediaControllerBlkJSX, "btnRefreshWindowing"], null,
                                                   { action: "FNCCALL", context: null,
                                                     fnc: function () {
                                                       var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                       if (!jsx3.util.strEmpty(endPointNID) && endPointNID != "NONE") {
                                                         var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                                         var feature = recordNode.getAttribute("jsxfeature");
                                                         if (!jsx3.util.strEmpty(feature)) {
                                                           var featureValue = recordNode.getAttribute('jsxfeature');
                                                           if (featureValue == 'XP200DisplayConfig') {
                                                             // publish to tMediaController to get the display config for xp rx
                                                             topic_Publish(tMediaController, "GetDisplayPortConfig", newServerDate());
                                                           }
                                                           else if (featureValue == 'IPVSClientDisplayConfig') {
                                                             // publish to tMediaController to get the display status for UMC
                                                             topic_Publish(tMediaController, "GetDisplayPortStatus", (new Date()).getTime());
                                                           }
                                                         }
                                                         else {
                                                           topic_Publish(tMediaController, "OpenWindowingPcOrRx", "No");
                                                         }
                                                       }
                                                     }
                                                   }
                                                  );

    var cmpBtnCloseWindowing = new CmpImageButton([cmpMediaControllerBlkJSX, "btnCloseWindowing"], null,
                                                   { action: "FNCCALL", context: null,
                                                     fnc: function () {
                                                       var btnState = cmpBtnCloseWindowing.BlkJSX.getState();
                                                       if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                         lytEndPointController.setRows("26,*,250", true);
                                                         topic_Publish(tMediaController, "OpenWindowingPanel", "OPEN");
                                                         cmpBlkEndPointContainer.BlkJSX.repaint();
                                                         var endPointNID = topic_Get(tMediaController, "EndPointSelected");
                                                         if (!jsx3.util.strEmpty(endPointNID) && endPointNID != "NONE") {
                                                           var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                                           if (jsx3.util.strEmpty(recordNode)) return;
                                                           var featurename = recordNode.getAttribute("jsxfeaturename");
                                                           if (jsx3.util.strEmpty(featurename)) {
                                                             // reset the variables
                                                             resetWindowingVars();
                                                             topic_Publish(tMediaController, "OpenWindowingPcOrRx", "No");
                                                             return;
                                                           }
                                                           else {
                                                             var featureValue = recordNode.getAttribute('jsxfeature');
                                                             if (featureValue == 'XP200DisplayConfig') {
                                                               // publish to tMediaController to get the display config for xp rx
                                                               topic_Publish(tMediaController, "GetDisplayPortConfig", newServerDate());
                                                             }
                                                             else if (featureValue == 'IPVSClientDisplayConfig') {
                                                               // publish to tMediaController to get the display status for UMC
                                                               topic_Publish(tMediaController, "GetDisplayPortStatus", (new Date()).getTime());
                                                             }
                                                           }
                                                         }
                                                         else {
                                                           // reset the variables
                                                           resetWindowingVars();
                                                           return;
                                                         }
                                                       }
                                                       else if (btnState == jsx3.gui.ImageButton.STATE_ON) {
                                                         lytEndPointController.setRows("26,*,25", true);
                                                         topic_Publish(tMediaController, "OpenWindowingPanel", "NONE");
                                                         cmpBlkEndPointContainer.BlkJSX.repaint();
                                                         // reset the variables
                                                         resetWindowingVars();
                                                       }
                                                     }
                                                   }
                                                 );

    var cmpWindowingModeBtnRemote = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnRemoteMode"], null,
                                                       { action: "FNCCALL",
                                                           context: null,
                                                           fnc: function() {
                                                                 if(checkEndPointState("WindowCommand")){
                                                                   log.info("Set Windowing Mode: Remote\n");
                                                                   topic_Publish(tMediaController, "WindowingMode", "REMOTE");
                                                                   setGetDisplayStatus("SetWindowingMode");
                                                                 }
                                                           }
                                                       }, CSSbtnRemoteModeDown, CSSbtnRemoteModeUp );

                                                                                                   
    
    var cmpWindowingModeBtnLocal = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnLocalMode"], null,
                                                       { action: "FNCCALL",
                                                           context: null,
                                                           fnc: function() {
                                                                 if(checkEndPointState("WindowCommand")){
                                                                   log.info("Set Windowing Mode: Local\n");
                                                                   topic_Publish(tMediaController, "WindowingMode", "LOCAL");
                                                                   setGetDisplayStatus("SetWindowingMode");
                                                                 }
                                                           }
                                                       }, CSSbtnLocalModeDown, CSSbtnLocalModeUp );
                                                                                                             
                                                          
    var cmpWindowingModeBtnPip = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPMode"], null,
                                                     { action: "FNCCALL",
                                                         context: null,
                                                         fnc: function() {
                                                               if(checkEndPointState("WindowCommand")){
                                                                 log.info("Set Windowing Mode: PIP\n");
                                                                 topic_Publish(tMediaController, "WindowingMode", "PIP");
                                                                 setGetDisplayStatus("SetWindowingMode");
                                                               }
                                                         }
                                                     }, CSSbtnPIPModeDown, CSSbtnPIPModeUp ); 
    
    var cmpBlkWindowingModeBtn = new CmpContainer([cmpMediaControllerBlkJSX, "blkWindowingModeButtons"], 
                                                  [{cmp:cmpWindowingModeBtnRemote,
                                                     sel: "topic_Get('" + tMediaController + "', 'WindowingMode') == 'REMOTE' " },
                                                   {cmp:cmpWindowingModeBtnLocal,
                                                     sel: "topic_Get('" + tMediaController + "', 'WindowingMode') == 'LOCAL' " },
                                                   {cmp:cmpWindowingModeBtnPip,
                                                     sel: "topic_Get('" + tMediaController + "', 'WindowingMode') == 'PIP' " }
                                                  ],
                                                  [{ action: "TOPICEVENT", topic: tMediaController, eventvar: "WindowingMode", eventval: null }],
                                                  null );
                                                  
    var cmpBlkPIPLocation1 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation1"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){
                                                             log.info("Set Windowing Mode: PIP: LOCATION1\n");
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION1");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
    
    var cmpBlkPIPLocation2 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation2"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){     
                                                             log.info("Set Windowing Mode: PIP: LOCATION2\n");                                                
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION2");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation3 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation3"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){     
                                                             log.info("Set Windowing Mode: PIP: LOCATION3\n");                                                
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION3");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation4 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation4"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){            
                                                             log.info("Set Windowing Mode: PIP: LOCATION4\n");                                         
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION4");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation5 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation5"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){       
                                                             log.info("Set Windowing Mode: PIP: LOCATION5\n");                                              
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION5");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation6 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation6"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){      
                                                             log.info("Set Windowing Mode: PIP: LOCATION6\n");                                               
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION6");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation7 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation7"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){    
                                                             log.info("Set Windowing Mode: PIP: LOCATION7\n");                                                 
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION7");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation8 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation8"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){    
                                                             log.info("Set Windowing Mode: PIP: LOCATION8\n");                                                 
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION8");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    
    var cmpBlkPIPLocation9 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPLocation9"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){  
                                                             log.info("Set Windowing Mode: PIP: LOCATION9\n");                                                     
                                                             topic_Publish(tMediaController, "WindowingLocation", "LOCATION9");
                                                             setGetDisplayStatus("SetWindowingLocation");
                                                           }
                                                     }
                                                 }, CSSbtnPIPLocationUp, CSSbtnPIPLocationDown); 
                                                         
    var cmpBlkPIPParentWin = new CmpBlock([cmpMediaControllerBlkJSX, "blkPipPopsitionNav"]);
                                                         
    var cmpBlkPIPLocation = new CmpContainer([cmpMediaControllerBlkJSX, "blkPIPPosition"], 
                                              [{cmp:cmpBlkPIPLocation1,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION1'  "},
                                               {cmp:cmpBlkPIPLocation2,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION2' "},
                                               {cmp:cmpBlkPIPLocation3,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION3' "},
                                               {cmp:cmpBlkPIPLocation4,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION4' "},
                                               {cmp:cmpBlkPIPLocation5,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION5' "},
                                               {cmp:cmpBlkPIPLocation6,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION6' "},
                                               {cmp:cmpBlkPIPLocation7,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION7'"},
                                               {cmp:cmpBlkPIPLocation8,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION8' "},
                                               {cmp:cmpBlkPIPLocation9,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingLocation') == 'LOCATION9' "}
                                              ],
                                              [{ action: "TOPICEVENT", topic: tMediaController, eventvar: "WindowingLocation", eventval: null }],
                                              null );

    var cmpBlkPIPSize2x8 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPSize2x8"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){    
                                                             log.info("Set Windowing Mode: PIP: SIZE2X8\n");                                                   
                                                             topic_Publish(tMediaController, "WindowingSize", "SIZE2X8");
                                                             setGetDisplayStatus("SetWindowingSize");
                                                           }
                                                     }
                                                 }, CSSbtnPIPSize2x8Up, CSSbtnPIPSize2x8Down);

    var cmpBlkPIPSize4x8 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPSize4x8"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){  
                                                             log.info("Set Windowing Mode: PIP: SIZE4X8\n");                                                     
                                                             topic_Publish(tMediaController, "WindowingSize", "SIZE4X8");
                                                             setGetDisplayStatus("SetWindowingSize");
                                                           }
                                                     }
                                                 }, CSSbtnPIPSize4x8Up, CSSbtnPIPSize4x8Down);
                                                 
    
    var cmpBlkPIPSize6x8 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPSize6x8"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){  
                                                             log.info("Set Windowing Mode: PIP: SIZE6X8\n");                                                     
                                                             topic_Publish(tMediaController, "WindowingSize", "SIZE6X8");
                                                             setGetDisplayStatus("SetWindowingSize");
                                                           }
                                                     }
                                                 }, CSSbtnPIPSize6x8Up, CSSbtnPIPSize6x8Down);
                                                 
    
    var cmpBlkPIPSize8x8 = new CmpBlockButton([cmpMediaControllerBlkJSX, "btnPIPSize8x8"], null,
                                                 { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function() {
                                                           if(checkEndPointState("WindowCommand")){  
                                                             log.info("Set Windowing Mode: PIP: SIZE8X8\n");                                                   
                                                             topic_Publish(tMediaController, "WindowingSize", "SIZE8X8");
                                                             setGetDisplayStatus("SetWindowingSize");
                                                           }
                                                     }
                                                 }, CSSbtnPIPSize8x8Up, CSSbtnPIPSize8x8Down);
                                                 
                                                     
    var cmpBlkPIPSize = new CmpContainer([cmpMediaControllerBlkJSX, "blkPIPSize"], 
                                              [{cmp:cmpBlkPIPSize2x8,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingSize') == 'SIZE2X8' "},
                                               {cmp:cmpBlkPIPSize4x8,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingSize') == 'SIZE4X8'  "},
                                               {cmp:cmpBlkPIPSize6x8,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingSize') == 'SIZE6X8' "},
                                               {cmp:cmpBlkPIPSize8x8,
                                                 sel: "topic_Get('" + tMediaController + "', 'WindowingSize') == 'SIZE8X8'  "}
                                              ],
                                              [{ action: "TOPICEVENT", topic: tMediaController, eventvar: "WindowingSize", eventval: null }],
                                              null );
    
    var cmpTxtOpacity = new CmpLabel([cmpMediaControllerBlkJSX, "lblTxtOpacity"], true);
    var sliderMaxValue = 100;
    var cmpOpacitySlider = new CmpSimpleSlider([cmpMediaControllerBlkJSX, "sldOpacity"], sliderMaxValue, cmpTxtOpacity.BlkJSX, null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function(eventVal) {
                                                        if(checkEndPointState("WindowCommand")){    
                                                          log.info("Set Windowing Mode: PIP: Opacity:"+ eventVal +"\n");                                              
                                                          topic_Set(tMediaController, "WindowingOpacity", eventVal);
                                                          setGetDisplayStatus("SetWindowingOpacity");
                                                        }
                                                 }
                                               }
                                              );
    var cmpBlkPIPOpacity = new CmpContainer([cmpMediaControllerBlkJSX, "blkOpacity"],
                                            [{ cmp: cmpTxtOpacity },
                                             { cmp: cmpOpacitySlider}],
                                             null, null);

    var cmpEndPointWindowingPIP = new CmpContainer([cmpMediaControllerBlkJSX, "blkWindowingPIP"],
                                                   [{cmp:cmpBlkPIPLocation},
                                                    {cmp:cmpBlkPIPSize},
                                                    {cmp:cmpBlkPIPOpacity},
                                                    {cmp:cmpBlkPIPParentWin}
                                                   ],
                                                   null,
                                                   null
                                                  );
                                                                                                    
    var cmpBlkEndPointWindowingPIP = new CmpContainer([cmpMediaControllerBlkJSX, "blkPIPContainer"], 
                                                      [{cmp:cmpEndPointWindowingPIP, 
                                                        sel: "topic_Get('"+tMediaController+"', 'WindowingMode') == 'PIP'" }],
                                                      [{action: "TOPICEVENT", topic: tMediaController, eventvar:"WindowingMode", eventval: null }],
                                                      null );
                                                      
    // topic_Publish(tEndPoint, "OpenWindowingPanel", "OPEN");
    var cmpBlkWindowingMask = new Component([cmpMediaControllerBlkJSX, "blkWindowingMask"]);
    
    var cmpBlkWindowing = new CmpContainer([cmpMediaControllerBlkJSX, "blkWindowing"], 
                                            [{cmp:cmpBlkWindowingMask,
                                              sel: "topic_Get('"+tMediaController+"', 'OpenWindowingMask') != 'NONE'"}],
                                            [{action: "TOPICEVENT", topic: tMediaController, eventvar:"OpenWindowingMask", eventval: null }],
                                            null );
                                                                                                                                                         
    var cmpBlkRxWindowing = new CmpContainer([cmpMediaControllerBlkJSX, "blkRxWindowing"], 
                                              [{cmp:cmpBlkWindowingModeBtn},
                                               {cmp:cmpBlkEndPointWindowingPIP}
                                              ],
                                              null,
                                              null );
                                              
    var cmpTxtCanvasName = new CmpLabel([cmpMediaControllerBlkJSX, "lblCanvasName"], true);
                                              
    var cmpBtnEditCanvas = new CmpImageButton([cmpMediaControllerBlkJSX, "btnEditCanvas"], null,
                                             { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function() {
                                                    if( checkEndPointState("WindowEdit") ) {
                                                      var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                                      dlgEditCanvas_Create(objXML); 
                                                    }
                                                 }
                                              }
                                             );
                                             
//    var cmpSelCanvasLayout = new CmpSelect([cmpMediaControllerBlkJSX, "selCanvasLayout"],
//                                           null,
//                                           { persist: null,
//                                             action: "TOPICEVENT", topic: tMediaController, eventvar: "CanvasLayoutSelected", eventval: "jsxid"
//                                           }
//                                          );    
                                             
    var cmpBtnList = new CmpImageButton([cmpMediaControllerBlkJSX, "btnList"], null,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function() {
                                            cmpBtnList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                            cmpBtnCanvas.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                            topic_Publish(tMediaController, "WindowMode","LIST");
                                          }
                                        }
                                       );
                                
    var cmpBtnCanvas = new CmpImageButton([cmpMediaControllerBlkJSX, "btnCanvas"], null,
                                          { action: "FNCCALL",
                                            context: null,
                                            fnc: function() {
                                              cmpBtnCanvas.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                              cmpBtnList.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                              topic_Publish(tMediaController, "WindowMode","CANVAS");
                                            }
                                          }
                                         );                                          
    
    
    var cmpBlkWindowListPanel = new CmpButtonPanel([cmpMediaControllerBlkJSX, "BlockWindowListPanel"],
                                                   null,
                                                   { action: "TOPICEVENT", topic: tMediaController, eventvar: "SelectListWindow", eventval: "jsxid" }
                                                   );
    
    var cmpBlkListView = new CmpContainer([cmpMediaControllerBlkJSX, "blkListView"], 
                                          [{cmp:cmpBlkWindowListPanel}
                                          ],
                                          null,
                                          null );
    
    var cmpBlkFullScreen = new Component([cmpMediaControllerBlkJSX, "blkFullScreen"],
                                         null,
                                         null,
                                         null
                                         ); 
                                                    
                                     
    var cmpBlkFullScreenView = new CmpContainer([cmpMediaControllerBlkJSX, "blkFullScreenView"], 
                                                [{cmp:cmpBlkFullScreen}
                                                ],
                                                null,
                                                null );

    var cmpBtnWinFullScreen = new CmpImageButton([cmpMediaControllerBlkJSX, "btnWinFullScreen"], null,
                                                 { action: "FNCCALL",
                                                   context: null,
                                                   fnc: function () {
                                                         var btnState = cmpBtnWinFullScreen.BlkJSX.getState();
                                                         if (!checkEndPointState("WindowCommand")) {
                                                           if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                             cmpBtnWinFullScreen.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                           }
                                                           else {
                                                             cmpBtnWinFullScreen.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                           }
                                                           return;
                                                         }
                                                         var windowPropertiesString = '<WindowProperties/>';
                                                         var windowPropertiesDoc = new jsx3.xml.Document();
                                                         var windowPropertiesNode = windowPropertiesDoc.loadXML(windowPropertiesString.toString());

                                                         var selectedWindowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                         var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                                         var windowNode = objXML.selectSingleNode(".//record/Canvas/Window[@id ='" + selectedWindowId + "']");
                                                         var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                     
                                                         if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                           propertiesNode.setAttribute("maximize", "false");
                                                           windowPropertiesNode.setAttribute("maximize", "false");
                                                           //                                                       topic_Publish(tMediaController, "WindowMode","CANVAS");
                                                         }
                                                         else {
                                                           propertiesNode.setAttribute("maximize", "true");
                                                           windowPropertiesNode.setAttribute("maximize", "true");
                                                           //                                                       topic_Publish(tMediaController, "WindowMode","FULLSCREEN");
                                                         }
                                                         SetWindowProperties(windowPropertiesNode);
                                                   }
                                                 }
                                                );
                                                                      
    var cmpBtnWin1to1 = new CmpImageButton([cmpMediaControllerBlkJSX, "btnWin1to1"], null,
                                           { action: "FNCCALL",
                                             context: null,
                                             fnc: function () {
                                                    var btnState = cmpBtnWin1to1.BlkJSX.getState();
                                                    if (!checkEndPointState("WindowCommand")) {
                                                      if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                        cmpBtnWin1to1.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                      }
                                                      else {
                                                        cmpBtnWin1to1.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                      }
                                                      return;
                                                    }
                                                    var windowPropertiesString = '<WindowProperties/>';
                                                    var windowPropertiesDoc = new jsx3.xml.Document();
                                                    var windowPropertiesNode = windowPropertiesDoc.loadXML(windowPropertiesString.toString());
                                                
                                                    var selectedWindowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                    var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                                    var windowNode =objXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedWindowId+"']");
                                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                    
                                                    if(btnState == jsx3.gui.ImageButton.STATE_OFF){
                                                      propertiesNode.setAttribute("scale","fitWindow");
                                                      windowPropertiesNode.setAttribute("scale","fitWindow");
                                                    }
                                                    else {
                                                      propertiesNode.setAttribute("scale","fitNativeSize");
                                                      windowPropertiesNode.setAttribute("scale","fitNativeSize");
                                                    }
                                                    SetWindowProperties(windowPropertiesNode);
                                             }
                                           }
                                          );                                         
                                           
    var cmpBtnWinMute = new CmpImageButton([cmpMediaControllerBlkJSX, "btnWinMute"], null,
                                           { action: "FNCCALL",
                                             context: null,
                                             fnc: function () {
                                                   var btnState = cmpBtnWinMute.BlkJSX.getState();
                                                   if (!checkEndPointState("WindowCommand")) {
                                                     if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                       cmpBtnWinMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                     }
                                                     else {
                                                       cmpBtnWinMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                     }
                                                      return;
                                                    }
                                                    var windowPropertiesString = '<WindowProperties/>';
                                                    var windowPropertiesDoc = new jsx3.xml.Document();
                                                    var windowPropertiesNode = windowPropertiesDoc.loadXML(windowPropertiesString.toString());
                                                
                                                    var selectedWindowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                    var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                                    var windowNode =objXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedWindowId+"']");
                                                    var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                    
                                                    if(btnState == jsx3.gui.ImageButton.STATE_OFF){
                                                      propertiesNode.setAttribute("mute","false");
                                                      windowPropertiesNode.setAttribute("mute","false");
                                                    }
                                                    else {
                                                      propertiesNode.setAttribute("mute","true");
                                                      windowPropertiesNode.setAttribute("mute","true");
                                                    }
                                                    cmpBlkPcCanvasDisplay.repaint();
                                                    SetWindowProperties(windowPropertiesNode);
                                             }
                                           }
                                          );  
    function SetWindowProperties(windowPropertiesNode){
      var endPointNID = topic_Get(tMediaController, "EndPointSelected");   
      var nodeCanvas = cmpBlkPcCanvasDisplay.BlkJSX.getXML().selectSingleNode(".//record/Canvas[.//Window/@jsxselected='1']");
      var canvasId = nodeCanvas.getAttribute("id");  
      var nodeLayout = nodeCanvas.selectSingleNode(".//Layout");
      var layoutId = nodeLayout.getAttribute("id");  
      var nodeWindow = nodeCanvas.selectSingleNode(".//Window[@jsxselected='1']");
      var windowId = nodeWindow.getAttribute("id");
      // create the set request data
      var setPortWindowPropertiesNode = topic_Set(tMediaController,"SetPortWindowProperties",null);
      setPortWindowPropertiesNode.removeChildren();
      setPortWindowPropertiesNode.setAttribute("bySessionId","");
      setPortWindowPropertiesNode.setAttribute("byConnId","");
      setPortWindowPropertiesNode.setAttribute("byNID",endPointNID);
      setPortWindowPropertiesNode.setAttribute("byCanvasId",canvasId);
      setPortWindowPropertiesNode.setAttribute("byLayoutId",layoutId);
      setPortWindowPropertiesNode.setAttribute("byWindowId",windowId);
      setPortWindowPropertiesNode.appendChild(windowPropertiesNode);

      // show the mask
      topic_Publish(tMediaController, "OpenWindowingMask", "OPEN");
            
      var setPortWindowPropertiesReq = new ReqSessionHandleWindowing(setPortWindowPropertiesNode,"SetPortWindowProperties",
                                                                     {action:"FNCCALL", context:this,
                                                                       fnc:function(respCode, respVal){
                                                                            var errorFlag = false;
                                                                            if (respCode == "SUCCESS" && respVal != null) {

                                                                            }
                                                                            else {
                                                                              errorFlag = true;
                                                                              processResponseForIpvsError("SetPortWindowProperties", respVal);
                                                                            }

                                                                            // if error get the old config
                                                                            if (errorFlag) {
                                                                              topic_Publish(tMediaController, "GetDisplayPortStatus", (new Date()).getTime());
                                                                            }

                                                                          }
                                                                      }      
                                                                   );
      setPortWindowPropertiesReq.sendRequest(this);                                                             
    }
    
    function SetWindowPropertiesBtnState(windowNode){
      cmpBtnWinFullScreen.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpBtnWin1to1.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpBtnWinMute.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpBtnWinVisible.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      cmpBtnWinKbm.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);

      var connContentNode = windowNode.selectSingleNode(".//ConnectionContent");
      var srcKbmCtrl = connContentNode.getAttribute("srcKbmCtrl");

      var propertiesNode = windowNode.selectSingleNode(".//Properties");
      var fullScreen = propertiesNode.getAttribute("maximize");
      var scale = propertiesNode.getAttribute("scale");
      var mute = propertiesNode.getAttribute("mute");
      var visibility = propertiesNode.getAttribute("visibility");
      var remoteKbmCtrl = propertiesNode.getAttribute("remoteKbmCtrl");
      
      if(!jsx3.util.strEmpty(fullScreen) && fullScreen == "true"){
        cmpBtnWinFullScreen.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }
      else{
        cmpBtnWinFullScreen.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      if(!jsx3.util.strEmpty(scale) && scale == "fitWindow"){
        cmpBtnWin1to1.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      else{
        cmpBtnWin1to1.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }
      if(!jsx3.util.strEmpty(mute) && mute == "true"){
        cmpBtnWinMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }
      else{
        cmpBtnWinMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      if(!jsx3.util.strEmpty(visibility) && visibility == "true"){
        cmpBtnWinVisible.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      else{
        cmpBtnWinVisible.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
      }

      var strImg = jsx3.resolveURI('images/buttons/btnEpcKbmOffX.png');
      //set the kbm button state
      if (!jsx3.util.strEmpty(remoteKbmCtrl) && remoteKbmCtrl == "true") {
        cmpBtnWinKbm.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
        strImg = jsx3.resolveURI('images/buttons/btnEpcKbmOnX.png');
      }
      else {
        cmpBtnWinKbm.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
      }
      cmpBtnWinKbm.BlkJSX.setDisabledImage(strImg);
      
      // condition for the kbm button to be enabled.
      if(!jsx3.util.strEmpty(srcKbmCtrl) && srcKbmCtrl == 'true') {
        cmpBtnWinKbm.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
        topic_Publish(tMediaController, 'WindowKbm', "ENABLE");
      }
      else {
        cmpBtnWinKbm.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
        topic_Publish(tMediaController, 'WindowKbm', "DISABLE");
      }

    }
    
    function SetSelCanvasLayout(objXML){
      var newObjXML = objXML.cloneDocument();
      var xslfile = PNameSpace.loadInclude("jsxuser:///JSXAPPS/IPVS-Common/xsl/CanvasLayout.xsl","CanvasLayout","xsl");
      var xslDoc = jsx3.xml.XslDocument.wrap(xslfile);
      var layoutXML = xslDoc.transform(newObjXML);
      var objLayoutXML = new jsx3.xml.Document().loadXML(layoutXML); 
      cmpSelCanvasLayout.BlkJSX.setSourceXML(objLayoutXML);
      var id = cmpSelCanvasLayout.BlkJSX.getXML().selectSingleNode(".//record").getAttribute("jsxid");
      if(!jsx3.util.strEmpty(id)){
        cmpSelCanvasLayout.BlkJSX.setValue(id);
      }
    }  
                                                                                   
    var cmpBtnWinVisible = new CmpImageButton([cmpMediaControllerBlkJSX, "btnWinVisible"], null,
                                              { action: "FNCCALL",
                                                context: null,
                                                fnc: function () {
                                                      var btnState = cmpBtnWinVisible.BlkJSX.getState();
                                                      if (!checkEndPointState("WindowCommand")) {
                                                        if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                          cmpBtnWinVisible.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                        }
                                                        else {
                                                          cmpBtnWinVisible.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                        }
                                                        return;
                                                      }
                                                      var windowPropertiesString = '<WindowProperties/>';
                                                      var windowPropertiesDoc = new jsx3.xml.Document();
                                                      var windowPropertiesNode = windowPropertiesDoc.loadXML(windowPropertiesString.toString());
                                                      var selectedWindowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                      var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                                      var windowNode =objXML.selectSingleNode(".//record/Canvas/Window[@id ='"+selectedWindowId+"']");
                                                      var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                      
                                                      if(btnState == jsx3.gui.ImageButton.STATE_OFF){
                                                        propertiesNode.setAttribute("visibility","true");
                                                        windowPropertiesNode.setAttribute("visibility","true");
                                                      }
                                                      else {
                                                        propertiesNode.setAttribute("visibility","false");
                                                        windowPropertiesNode.setAttribute("visibility","false");
                                                      }
                                                      cmpBlkPcCanvasDisplay.repaint(); 
                                                      SetWindowProperties(windowPropertiesNode);   
                                                }
                                               }
                                              );

    var cmpBtnWinKbm = new CmpImageButton([cmpMediaControllerBlkJSX, "btnWinKbm"], null,
                                              { action: "FNCCALL",
                                                context: null,
                                                fnc: function () {
                                                      var btnState = cmpBtnWinKbm.BlkJSX.getState();
                                                      if (!checkEndPointState("WindowCommand")) {
                                                        if (btnState == jsx3.gui.ImageButton.STATE_OFF) {
                                                          cmpBtnWinKbm.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                        }
                                                        else {
                                                          cmpBtnWinKbm.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                        }
                                                        return;
                                                      }
                                                      var windowPropertiesString = '<WindowProperties/>';
                                                      var windowPropertiesDoc = new jsx3.xml.Document();
                                                      var windowPropertiesNode = windowPropertiesDoc.loadXML(windowPropertiesString.toString());
                                                      var selectedWindowId = topic_Get(tMediaController, "SelectCanvasWindow");
                                                      var objXML = cmpBlkPcCanvasDisplay.BlkJSX.getXML();
                                                      var windowNode = objXML.selectSingleNode(".//record/Canvas/Window[@id ='" + selectedWindowId + "']");
                                                      var propertiesNode = windowNode.selectSingleNode(".//Properties");
                                                      
                                                      if (btnState == jsx3.gui.ImageButton.STATE_ON) {
                                                        propertiesNode.setAttribute("remoteKbmCtrl", "true");
                                                        windowPropertiesNode.setAttribute("remoteKbmCtrl", "true");
                                                      }
                                                      else {
                                                        propertiesNode.setAttribute("remoteKbmCtrl", "false");
                                                        windowPropertiesNode.setAttribute("remoteKbmCtrl", "false");
                                                      }
                                                      cmpBlkPcCanvasDisplay.repaint();
                                                      SetWindowProperties(windowPropertiesNode);
                                                }
                                              }
                                              ); 
                                                 
    var cmpBlkPcCanvasDisplay = new CmpCanvas([cmpMediaControllerBlkJSX, "BlockPcCanvasDisplay"],
                                              null,
                                              {action: "TOPICEVENT", topic: tMediaController, eventvar: "SelectCanvasWindow", eventval: "id" }
                                             );
    cmpBlkPcCanvasDisplay.allowToggle = true;

    var cmpBlkCanvasView = new CmpContainer([cmpMediaControllerBlkJSX, "blkCanvasView"], 
                                              [{cmp:cmpBlkPcCanvasDisplay}
                                              ],
                                              null,
                                              null );

    var cmpBlkPcWindowing = new CmpContainer([cmpMediaControllerBlkJSX, "blkPcWindowing"],
                                             [{ cmp: cmpBtnEditCanvas },
                                              { cmp: cmpTxtCanvasName },
//                                              { cmp: cmpSelCanvasLayout },
                                              { cmp: cmpBtnList },
                                              { cmp: cmpBtnCanvas },
                                              { cmp: cmpBlkListView,
                                                sel: "topic_Get('" + tMediaController + "', 'WindowMode') == 'LIST'"
                                              },
                                              { cmp: cmpBlkFullScreenView,
                                                sel: "topic_Get('" + tMediaController + "', 'WindowMode') == 'FULLSCREEN'"
                                              },
                                              { cmp: cmpBlkCanvasView,
                                                sel: "topic_Get('" + tMediaController + "', 'WindowMode') == 'CANVAS'"
                                              },
                                              { cmp: cmpBtnWinFullScreen },
                                              { cmp: cmpBtnWin1to1 },
                                              { cmp: cmpBtnWinMute },
                                              { cmp: cmpBtnWinVisible },
                                              { cmp: cmpBtnWinKbm,
                                                sel: "topic_Get('" + tMediaController + "', 'WindowKbm') == 'ENABLE'"
                                              }
                                             ],
                                             [{ action: "TOPICEVENT", topic: tMediaController, eventvar: "WindowMode", eventval: null },
                                              { action: "TOPICEVENT", topic: tMediaController, eventvar: "WindowKbm", eventval: null}],
                                             null);
                                             
    var cmpBlkNoWindowing = new Component([cmpMediaControllerBlkJSX, "blkNoWindowingMsg"], 
                                          null,
                                          null,
                                          null);                                         
                                             
    var cmpBlkEndPointWindowing = new CmpContainer([cmpMediaControllerBlkJSX, "blkWindowingContainer"], 
                                                    [{cmp:cmpBlkWindowing},
                                                     {cmp:cmpBlkRxWindowing,
                                                      sel: "topic_Get('"+tMediaController+"', 'OpenWindowingPcOrRx') == 'Rx' &&" + "topic_Get('"+tMediaController+"', 'OpenWindowingPanel') == 'OPEN'"},
                                                     {cmp:cmpBlkPcWindowing,
                                                      sel: "topic_Get('"+tMediaController+"', 'OpenWindowingPcOrRx') == 'Pc' &&"+"topic_Get('"+tMediaController+"', 'OpenWindowingPanel') == 'OPEN'"},
                                                     {cmp:cmpBlkNoWindowing,
                                                      sel: "topic_Get('"+tMediaController+"', 'OpenWindowingPcOrRx') == 'No' &&"+"topic_Get('"+tMediaController+"', 'OpenWindowingPanel') == 'OPEN'"}
                                                    ],
                                                    [{action: "TOPICEVENT", topic: tMediaController, eventvar:"OpenWindowingPanel", eventval: null },
                                                     {action: "TOPICEVENT", topic: tMediaController, eventvar:"OpenWindowingPcOrRx", eventval: null }],
                                                    null );
                                                    
   
    // Top level sub cmp array
    cmpMediaController.SubCmpArr = [{ cmp: cmpBlkEndPointContainer },
                                    { cmp: cmpBlkEndPointControllerHeader },
                                    { cmp: cmpBlkMediaList },
                                    { cmp: cmpBlkEndPointWindowing },
                                    { cmp: cmpBtnRefreshWindowing },
                                    { cmp: cmpBtnCloseWindowing },
                                    { cmp: cmpClientWindowHeaderLogo }
                                   ];
  
    // To GetPortDisplayConfig
    eventMediaController.subscribe("GetDisplayPortConfig", null, this,
                                    function (eventVar, eventVal) {

                                      var endPointNID = topic_Get(tMediaController, "EndPointSelected");

                                      if (endPointNID == "NONE" || jsx3.util.strEmpty(endPointNID)) {
                                        winMediaControllerWindow.alert("Please Select the EndPoint");
                                        return false;
                                      }

                                      // get new status for the port
                                      var reqGetPortDisplayConfig = new ReqSessionGetRequest("GetPortDisplayConfig",
                                                                                              { action: "FNCCALL", context: this,
                                                                                                fnc: function (respCode, respVal) {
                                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                                    topic_Publish(tMediaController, 'OpenWindowingPcOrRx', 'Rx');
                                                                                                    // Update the windowing data
                                                                                                    getWindowingData(respVal);
                                                                                                  }
                                                                                                  else {
                                                                                                    processResponseForIpvsError("GetPortDisplayConfig", respVal);
                                                                                                  }
                                                                                                }
                                                                                              }
                                                                                             );
                                      reqGetPortDisplayConfig.ServiceName = "SessionService";
                                      var reqParams = new Object;
                                      reqParams["ObjectList_WhereNID"] = endPointNID;
                                      reqParams["ObjectList_WhereTextSubStr"] = "";
                                      reqParams["ObjectList_SubscriptionID"] = topic_Get(tMediaController, 'PortDisplayConfigSubscriptionID');
                                      reqGetPortDisplayConfig.reqParams = reqParams;
                                      // send the request
                                      reqGetPortDisplayConfig.sendRequest(this);

                                    }
                                );    
    
    // To GetPortDisplayStatus
    eventMediaController.subscribe("GetDisplayPortStatus", null, this,
                                    function (eventVar, eventVal) {
                                      var endPointNID = topic_Get(tMediaController, "EndPointSelected");

                                      if (endPointNID == "NONE" || jsx3.util.strEmpty(endPointNID)) {
                                        winMediaControllerWindow.alert("Please Select the EndPoint");
                                        return false;
                                      }
                                      
                                      // get new status for the port
                                      var reqGetPortDisplayStatus = new ReqSessionGetRequest("GetPortDisplayStatus",
                                                                                              { action: "FNCCALL", context: this,
                                                                                                fnc: function (respCode, respVal) {
                                                                                                  if (respCode == "SUCCESS" && respVal != null) {
                                                                                                    // save the response in the topic
                                                                                                    var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
                                                                                                    var featureValue = recordNode.getAttribute('jsxfeature');
                                                                                                    if (featureValue == 'XP200DisplayConfig') {
                                                                                                    }
                                                                                                    else if (featureValue == 'IPVSClientDisplayConfig') {
                                                                                                      topic_Publish(tMediaController, 'OpenWindowingPcOrRx', 'Pc');
                                                                                                      var portDisplayStatusNode = respVal.selectSingleNode(".//PortDisplayStatus");
                                                                                                      showUMCWindowingStatus(portDisplayStatusNode);
                                                                                                    }
                                                                                                  }
                                                                                                  else {
                                                                                                    processResponseForIpvsError("GetPortDisplayStatus", respVal);
                                                                                                  }
                                                                                                }
                                                                                              }
                                                                                             );
                                      reqGetPortDisplayStatus.ServiceName = "SessionService";
                                      var reqParams = new Object;
                                      reqParams["ObjectList_WhereNID"] = endPointNID;
                                      reqParams["ObjectList_WhereTextSubStr"] = "";
                                      reqParams["ObjectList_SubscriptionID"] = topic_Get(tMediaController, 'PortDisplayStatusSubscriptionID');
                                      reqGetPortDisplayStatus.reqParams = reqParams;
                                      // send the request
                                      reqGetPortDisplayStatus.sendRequest(this);

                                    }
                                );    

    //WindowingOpacity eventlistener
    eventMediaController.subscribe("WindowingOpacity", null, this,
                                    function(eventVar, eventVal) {
                                      var windowOpacity = eventVal;
                                      cmpOpacitySlider.setTxtOffset(windowOpacity)
                                      cmpOpacitySlider.setOffset(windowOpacity);
                                      cmpOpacitySlider.BlkJSX.repaint();
                                    }
                                ); 

    // helper function to check the presence
    function checkEndPointState(calledFrom){ 
      var alertMessage = "Cannot send windowing command: EndPoint not Ready";
      if ( calledFrom == "WindowEdit"){
        alertMessage = "Cannot edit display profile: EndPoint not Ready";
      }
      var endPointNID = topic_Get(tMediaController, "EndPointSelected");
      var endPointPanelXML= cmpBlkEndPointContainer.BlkJSX.getXML();
      var endpointNode = endPointPanelXML.selectSingleNode(".//record[@jsxid ='" + endPointNID+ "']");
      var endPointState = endpointNode.getAttribute("jsxdevstate");
      if((endPointState != "Ready") && (endPointState != "Unmanaged")) {
        winMediaControllerWindow.alert(alertMessage);
        return false;
      }
      return true;
    } // end checkEndPointState
    
    // event Listner when user modifies the canwas windows in dialog and clicks done                             
    eventMediaController.subscribe("SetDisplayPortStatus", null, this,
                                    function(eventVar, eventVal) {
                                      setGetDisplayStatus();
                                    }
                                  );      
                                                               
    // helper function to send the request
    function setGetDisplayStatus(whoClicked) {
      var endPointNID = topic_Get(tMediaController, "EndPointSelected");
      if( jsx3.util.strEmpty(endPointNID) || endPointNID == "NONE") return;
      
      var recordNode = cmpBlkEndPointContainer.BlkJSX.getRecordNode(endPointNID);
      var featureValue = recordNode.getAttribute('jsxfeature');
      // for Pc Canvas
      if(featureValue == 'IPVSClientDisplayConfig'){
       var recordNode = topic_GetNode(tMediaController, "IPVSDisplayConfig").cloneNode(true);
       var displayNode = recordNode.selectSingleNode(".//Display");
       var canvasNode = recordNode.selectSingleNode(".//Canvas");
      }
      else{      
        var windowConfigNode = topic_GetNode(tMediaController, "PortDisplayConfig").cloneNode(true);
        var canvasNode = windowConfigNode.selectSingleNode(".//Canvas");
        if( jsx3.util.strEmpty(canvasNode) ) return;
        
        var bgEnable = "false";
        var winEnable = "false";
        
        var mode = topic_Get(tMediaController, "WindowingMode");
        if( mode == "LOCAL" ){
          bgEnable = "true";
          winEnable = "false";
        }
        else if( mode == "REMOTE" ){
          bgEnable = "false";
          winEnable = "true";
          canvasNode.selectSingleNode(".//Geometry").setAttribute("transparency", "0%");  
          topic_Publish(tMediaController, "WindowingOpacity","0");      
          canvasNode.selectSingleNode(".//Geometry").setAttribute("hres", "100%");
          canvasNode.selectSingleNode(".//Geometry").setAttribute("vres", "100%");  
          topic_Publish(tMediaController, "WindowingSize", "SIZE8X8");      
          canvasNode.selectSingleNode(".//Geometry").setAttribute("hpos", "ALIGNLEFT");
          canvasNode.selectSingleNode(".//Geometry").setAttribute("vpos", "ALIGNTOP");  
          topic_Publish(tMediaController, "WindowingLocation","LOCATION1");                      
        }
        else if( mode == "PIP" ){
          bgEnable = "true";
          winEnable = "true";   
          if( whoClicked == "SetWindowingMode" ){
            topic_Publish(tMediaController, "WindowingSize", "SIZE2X8");
            topic_Publish(tMediaController, "WindowingLocation","LOCATION9");
            topic_Publish(tMediaController, "WindowingOpacity","0");
          }          
        }
        
        canvasNode.selectSingleNode(".//Background").setAttribute("enable", bgEnable);
        canvasNode.selectSingleNode(".//Window").setAttribute("enable", winEnable); 
        
        //get the other pip values
        if( mode == "PIP" ){
          // get the window size
          var windowSize = topic_Get(tMediaController, "WindowingSize");
          
          if( windowSize != "NONE") {
            var hres = "";
            var vres = ""; 
                        
            switch(windowSize){
              case "SIZE2X8" :
                hres = "25%";
                vres = "25%";            
                break;
              case "SIZE4X8" :
                hres = "50%";
                vres = "50%";             
                break;
              case "SIZE6X8" :
                hres = "75%";
                vres = "75%";               
                break;
              case "SIZE8X8" :
                 hres = "100%";
                 vres = "100%"; 
                break;
              default:
                break;                                          
            }
            
            canvasNode.selectSingleNode(".//Geometry").setAttribute("hres", hres);
            canvasNode.selectSingleNode(".//Geometry").setAttribute("vres", vres);           
          } // end if windowsize != NONE
          
          var windowLocation = topic_Get(tMediaController, "WindowingLocation");
          if( windowLocation != "NONE"){
            var vpos = "";
            var hpos = "";
            
            switch(windowLocation){
              case "LOCATION1":
                vpos = "ALIGNTOP";
                hpos = "ALIGNLEFT";    
                break; 
              case "LOCATION2":
                vpos = "ALIGNTOP";
                hpos = "ALIGNMIDDLE";    
                break; 
              case "LOCATION3":
                vpos = "ALIGNTOP";
                hpos = "ALIGNRIGHT";    
                break; 
              case "LOCATION4":
                vpos = "ALIGNMIDDLE";
                hpos = "ALIGNLEFT";    
                break; 
              case "LOCATION5":
                vpos = "ALIGNMIDDLE";
                hpos = "ALIGNMIDDLE";    
                break; 
              case "LOCATION6":
                vpos = "ALIGNMIDDLE";
                hpos = "ALIGNRIGHT";    
                break; 
              case "LOCATION7":
                vpos = "ALIGNBOTTOM";
                hpos = "ALIGNLEFT";    
                break; 
              case "LOCATION8":
                vpos = "ALIGNBOTTOM";
                hpos = "ALIGNMIDDLE";    
                break; 
              case "LOCATION9":
                vpos = "ALIGNBOTTOM";
                hpos = "ALIGNRIGHT";    
                break; 
              default:
                break;                                                                                                                                  
            }
            canvasNode.selectSingleNode(".//Geometry").setAttribute("hpos", hpos);
            canvasNode.selectSingleNode(".//Geometry").setAttribute("vpos", vpos); 
          } // end if windowLocation!= NONE
          
          var windowOpacity = topic_Get(tMediaController, "WindowingOpacity");
          if(windowOpacity != "NONE"){
            canvasNode.selectSingleNode(".//Geometry").setAttribute("transparency", windowOpacity+"%");
          } // end if windowOpacity != NONE
          
        } // end if mode == pip         
      } //end for Rx Canvas

      // create the set request data
      var setPortDisplayNode = topic_Set(tMediaController,"SetPortDisplayConfig",null);
      setPortDisplayNode.removeChildren();
      setPortDisplayNode.setAttribute("byNID",endPointNID);
      setPortDisplayNode.setAttribute("byTitle","");
      if(displayNode){
        setPortDisplayNode.appendChild(displayNode);
      }
      setPortDisplayNode.appendChild(canvasNode);

      // show the mask
      topic_Publish(tMediaController, "OpenWindowingMask", "OPEN");
           
      //send therequest
      var setPortDisplayReq = new ReqSessionHandleWindowing(setPortDisplayNode, "SetPortDisplayConfig",
                                                             { action: "FNCCALL", context: this,
                                                               fnc: function (respCode, respVal) {
                                                                 var errorFlag = false;
                                                                 if (respCode == "SUCCESS" && respVal != null) {

                                                                 }
                                                                 else {
                                                                   errorFlag = true;
                                                                   processResponseForIpvsError("SetPortDisplayConfig", respVal);
                                                                 }

                                                                 if (errorFlag) {
                                                                   if (featureValue == 'IPVSClientDisplayConfig') {
                                                                     topic_Publish(tMediaController, "GetDisplayPortStatus", (new Date()).getTime());
                                                                   }
                                                                   else if (featureValue == 'XP200DisplayConfig') {
                                                                     topic_Publish(tMediaController, "GetDisplayPortConfig", newServerDate());
                                                                   }
                                                                 }

                                                               }
                                                             }
                                                           );
   
//      setPortDisplayReq.SimulateResponse = true;
//      setPortDisplayReq.SimulateResponseXML = '<iq id="Request_jsx_b5" to="anujpc12@localhost/anujpc12" from="srcrelay1@localhost/srcrelay1" type="result"><query xmlns="com.ipvs.session" serviceVer="2.16"><SetPortDisplayConfig><PortDisplayConfig NID="default.mediastreamioportresourcelist.msioprl_c49a8e84-3c41-420c-b859-bbc4cf813b6a" title="xp43_CHANNEL2"><Exception><Code>1300</Code><Descritpion>Can not send Message, toJID is empty</Descritpion></Exception></PortDisplayConfig></SetPortDisplayConfig></query></iq>';                                                                                         
      
      setPortDisplayReq.sendRequest(this);
      
    } // end setGetDisplayStatus

    //helper function to close the windowing dialog
    function closeWindowingDialog() {
      var objParent = PNameSpace.getServer().getJSXByName("blkMediaControllerBody");

      if (jsx3.gui.Dialog) {
        try {
          var dialogBoxArr = objParent.getDescendantsOfType(jsx3.gui.Dialog, false);
          for (var i = 0; i < dialogBoxArr.length; i++) {
            dialogBoxArr[i].doClose();
          }
        } catch (e) {
        }
      }
    } // end closeWindowingDialog
            
    // helper function to reset the windowing topic vars
    function resetWindowingVars() {      
      //reset the vars/topics
      topic_Set(tMediaController, "WindowingSize", "NONE");
      topic_Set(tMediaController, "WindowingLocation", "NONE");
      topic_Publish(tMediaController, "WindowingMode", "NONE");   
      topic_Set(tMediaController, "PortDisplayStatus", "");
      topic_Set(tMediaController, "PortDisplayConfig", "");  
      topic_Set(tMediaController, "WindowingOpacity", "NONE"); 
      topic_Set(tMediaController, "OpenWindowingMask", "NONE");
      //topic_Publish(tMediaController, "OpenWindowingPcOrRx", "No"); 
      topic_Publish(tMediaController, "OpenWindowingPcOrRx", "NONE");
      topic_Set(tMediaController, "SelectCanvasWindow","NONE");
      
    } // end resetWindowingVars
    
    // this shows the umv windowing data.
    function showUMCWindowingStatus(dataNode) {
      //extract the 3 nodes from the data. chances are one or 2 nodes would be missing based on the notification.
      var displayNode = dataNode.selectSingleNode(".//Display");
      var canvasNode = dataNode.selectSingleNode(".//Canvas");
      dataNode.removeChildren();
      var portDisplayStatusNode = dataNode.cloneNode(true);

      //save or get the portdisplaynode
      if (jsx3.util.strEmpty(portDisplayStatusNode)) {
        portDisplayStatusNode = topic_Get(tMediaController, ".//GetPortDisplayStatus//PortDisplayStatus");
      }

      //save or get the display node
      if (jsx3.util.strEmpty(displayNode)) {
        displayNode = topic_GetNode(tMediaController, ".//GetPortDisplayStatus//Display");
      }

      //save or get the canvas node
      if (jsx3.util.strEmpty(canvasNode)) {
        canvasNode = topic_GetNode(tMediaController, ".//GetPortDisplayStatus//Canvas");
      }

      // save the response in the topic
      var dataNode = topic_Set(tMediaController, "GetPortDisplayStatus", null);
      dataNode.removeChildren();
      dataNode.appendChild(portDisplayStatusNode.cloneNode(true));
      var portStatusNode = dataNode.selectSingleNode(".//PortDisplayStatus");
      portStatusNode.appendChild(displayNode.cloneNode(true));
      portStatusNode.appendChild(canvasNode.cloneNode(true));

      //For Canvas Panel
      if (PCWindowingXSLFile.hasError()) {
        UriRes = cmpBlkPcCanvasDisplay.BlkJSX.getUriResolver();
        if (jsx3.util.strEmpty(UriRes)) return;
        xslUriRes = UriRes.resolveURI("xsl/PcWindowingCanvas.xsl");
        PCWindowingXSLFile = PNameSpace.loadInclude(xslUriRes, "_Update", "xsl");
      }

      if (PCWindowingXSLFile.hasError()) {
        log.error("showUMCWindowingStatus::PcWindowingCanvas.xsl:: has error\n");
        return;
      } 

      var xslDoc = jsx3.xml.XslDocument.wrap(PCWindowingXSLFile);
      var recordNode = xslDoc.transformToObject(dataNode);
      var canvasId = recordNode.selectSingleNode(".//record/Canvas").getAttribute("id");
      //For Window List Panel
      var listUriRes = cmpBlkWindowListPanel.BlkJSX.getUriResolver();
      if (jsx3.util.strEmpty(listUriRes)) return;
      var listXslUriRes = listUriRes.resolveURI("xsl/WindowList.xsl");
      if (jsx3.util.strEmpty(listXslUriRes)) return;
      var listXSLFile = PNameSpace.loadInclude(listXslUriRes, "_Update", "xsl");
      var listXslDoc = jsx3.xml.XslDocument.wrap(listXSLFile);
      var recordList = listXslDoc.transformToObject(dataNode);


      //Mark the last selected window, if nothing is selcted select the first window.
      var currentWindow = topic_Get(tMediaController, "SelectCanvasWindow");
      var windowId = currentWindow;
      var selectedWindowNode = null;
      if (!jsx3.util.strEmpty(currentWindow) && currentWindow != "NONE") {
        selectedWindowNode = recordNode.selectSingleNode(".//record/Canvas/Window[@id='" + currentWindow + "']");
      }

      if (jsx3.util.strEmpty(selectedWindowNode)) {
        selectedWindowNode = recordNode.selectSingleNode(".//record/Canvas/Window");
      }

      if (!jsx3.util.strEmpty(selectedWindowNode)) {
        windowId = selectedWindowNode.getAttribute("id");
        selectedWindowNode.setAttribute("jsxselected", "1");
        //topic_Publish(tMediaController, "SelectCanvasWindow",windowId);
      }
      cmpTxtCanvasName.BlkJSX.setText(canvasId);
      cmpBlkPcCanvasDisplay.onResponse(recordNode);
      if (currentWindow == "NONE") {
        cmpBlkWindowListPanel.onResponse(recordList);
      }
      else {
        //select the windid for the list
        var record = recordList.selectSingleNode(".//record[@jsxid='" + windowId + "']");
        record.setAttribute("jsxselected", "1");
        cmpBlkWindowListPanel.onResponse(recordList);
      }
      // this will set the window properties button control for the UMC
      topic_Publish(tMediaController, "SelectCanvasWindow", windowId);
      // hide the mask
      topic_Publish(tMediaController, "OpenWindowingMask", "NONE");
    } // end showUMCWindowingStatus
    
    function getWindowingData(dataNode) {
      // save the response in the topic
      var windowingConfig = topic_Set(tMediaController, "PortDisplayConfig", null);
      windowingConfig.removeChildren();
      windowingConfig.appendChild(dataNode);

      var windowConfigNode = dataNode.cloneNode(true);
      if( jsx3.util.strEmpty(windowConfigNode) ) {return;}

      var bgEnable = windowConfigNode.selectSingleNode(".//Background").getAttribute("enable");
      var winEnable = windowConfigNode.selectSingleNode(".//Window").getAttribute("enable");
      if( bgEnable == "true" && winEnable == "true"){
        // mode pip / overlay
        topic_Publish(tMediaController, "WindowingMode", "PIP");
      }
      else if( bgEnable == "false" && winEnable == "true"){
        // mode remote
        topic_Publish(tMediaController, "WindowingMode", "REMOTE");
      }
      else if( bgEnable == "true" && winEnable == "false"){
        // mode local
        topic_Publish(tMediaController, "WindowingMode", "LOCAL");
      }
      else {
        // error
      }
      
      var hpos = windowConfigNode.selectSingleNode(".//Geometry").getAttribute("hpos");
      var vpos = windowConfigNode.selectSingleNode(".//Geometry").getAttribute("vpos"); 
      if( hpos == "ALIGNLEFT" && vpos == "ALIGNTOP" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION1");
      }
      else if( hpos == "ALIGNMIDDLE" && vpos == "ALIGNTOP" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION2");      
      }
      else if( hpos == "ALIGNRIGHT" && vpos == "ALIGNTOP" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION3");      
      }
      else if( hpos == "ALIGNLEFT" && vpos == "ALIGNMIDDLE" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION4");      
      }
      else if( hpos == "ALIGNMIDDLE" && vpos == "ALIGNMIDDLE" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION5");      
      }
      else if( hpos == "ALIGNRIGHT" && vpos == "ALIGNMIDDLE" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION6");      
      }
      else if( hpos == "ALIGNLEFT" && vpos == "ALIGNBOTTOM" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION7");      
      }
      else if( hpos == "ALIGNMIDDLE" && vpos == "ALIGNBOTTOM" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION8");      
      }
      else if( hpos == "ALIGNRIGHT" && vpos == "ALIGNBOTTOM" ) {
        topic_Publish(tMediaController, "WindowingLocation", "LOCATION9");      
      }
      else {
        topic_Publish(tMediaController, "WindowingLocation", "NONE");
      }
      
      var hres = windowConfigNode.selectSingleNode(".//Geometry").getAttribute("hres");
      var vres = windowConfigNode.selectSingleNode(".//Geometry").getAttribute("vres");       
      if( hres == "25%" && vres == "25%"){
        topic_Publish(tMediaController, "WindowingSize", "SIZE2X8");
      }
      else if( hres == "50%" && vres == "50%"){
        topic_Publish(tMediaController, "WindowingSize", "SIZE4X8");      
      }
      else if( hres == "75%" && vres == "75%"){
        topic_Publish(tMediaController, "WindowingSize", "SIZE6X8");      
      }
      else if( hres == "100%" && vres == "100%"){
        topic_Publish(tMediaController, "WindowingSize", "SIZE8X8");      
      }
      else{
        topic_Publish(tMediaController, "WindowingSize", "NONE");
      }

      //get the Opacity
      var windowOpacity = windowConfigNode.selectSingleNode(".//Geometry").getAttribute("transparency");
      var windowOpacity = windowOpacity.replace("%","");
      cmpOpacitySlider.setTxtOffset(windowOpacity)
      cmpOpacitySlider.setOffset(windowOpacity);
      cmpOpacitySlider.BlkJSX.repaint();
      
      topic_Publish(tMediaController, "WindowingOpacity",windowOpacity);
      
      // publish to show the windowing panel
      topic_Publish(tMediaController, "OpenWindowingPanel", "OPEN");
      // hide the mask
      topic_Publish(tMediaController, "OpenWindowingMask", "NONE");      
      
    }// end getWindowingData()    
    
    //
    function getEndPointInfo(selectedEndPointNID) {
      var endPoint = cmpBlkEndPointContainer.BlkJSX.getXML().selectSingleNode(".//record[@jsxid ='" +selectedEndPointNID+ "']");
      if(!jsx3.util.strEmpty(endPoint)){
        var endPointObj = new Object;
        endPointObj.NID = endPoint.getAttribute("jsxid");
        endPointObj.type = endPoint.getAttribute("jsxportType");
        endPointObj.streamType = endPoint.getAttribute("jsxdevStreamType");
        endPointObj.Name = endPoint.getAttribute("jsxtext");
        endPointObj.state = endPoint.getAttribute("jsxdevstate");
        endPointObj.presence = endPoint.getAttribute("jsxpresence");
        endPointObj.maxInstanceCount = endPoint.getAttribute("jsxmaxInstanceCount");
        endPointObj.feature = endPoint.getAttribute("jsxfeature");
        endPointObj.streamto = endPoint.getAttribute("jsxstreamto");
        //Use existing sessionId and ConnectionID if available for selected window
        var winID = topic_Get(tMediaController, "SelectCanvasWindow");
        if (jsx3.util.strEmpty(winID) || winID == "NONE") {
          var sessionNode = endPoint.selectSingleNode(".//PortSessionInfo");
        }
        else {
          var sessionNode = endPoint.selectSingleNode(".//PortSessionInfo[PortConnectionInfo[@jsxwindowid='" + winID + "']]");
        }
        if (sessionNode) {
          endPointObj.sessionId = sessionNode.getAttribute("jsxsessionid");
          var connectionNode = sessionNode.selectSingleNode(".//PortConnectionInfo");
          if (connectionNode)
            endPointObj.connId = connectionNode.getAttribute("jsxconnectionid");
        }
      }
      return endPointObj;
    }

    // function check for exception code
    function processResponseForIpvsError(requestName, responseVal) {
      var ErrCode = "ERROR";
      var ErrDesc = "ERROR_DESCRIPTION";
      if (responseVal.match(/ResponseXML/)) {
        var ResponseXML = (responseVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
        var responseDoc = new jsx3.xml.Document;
        responseDoc.loadXML(ResponseXML);
        var ipvsErrorNode = responseDoc.selectSingleNode(".//ipvserror");
        if (!jsx3.util.strEmpty(ipvsErrorNode)) {
          ErrCode = ipvsErrorNode.getAttribute("code");
          ErrDesc = ipvsErrorNode.selectSingleNode(".//description").getValue();
        }

        //alert the user
        winMediaControllerWindow.alert(requestName + " request failed. \n" +
                                       " ExceptionCode:" + ErrCode + "\n ExceptionDesc:" + ErrDesc); 
      }

      log.error(requestName + " request failed REQUEST_RESPONSE::"+ responseVal + "\n");
          
    } // end processResponseForIpvsError
 
    // init and repaint happens after the window is loaded
    jsx3.sleep((function() {
      this.init();
      this.show();
      //Default is Live
      var tabButton = topic_Get(tMediaController, "ButtonSelection");
      topic_Publish(tMediaController, "ButtonSelection", tabButton);
      topic_Publish(tMediaController, "UpdateControlTarget", "");
    }
  ),
  null, cmpMediaController);

}








