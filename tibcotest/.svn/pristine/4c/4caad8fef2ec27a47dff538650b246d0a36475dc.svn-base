function cmpBlkProgram_Create() {    
  topic_Set(tApp, "MediaRoomListRefreshInterval",30);  
  var MediaRoomStatusInterval = 30000; // millisecond
  var MediaRoomStatusAgentObj = "DeviceStatus_Full";
  var MediaRoomAgentObjectReqSuccessEventVar = arrAgentObjectInfo[MediaRoomStatusAgentObj].successevent.eventvar;
  var MediaRoomIntervalID = null;
                      
  function MediaRoomStatusRepaint() {
   cmpStreamStatisticsInput_Repaint();
   cmpStreamStatisticsOutput_Repaint();
  }
  
  function setMediaRoomStatusAgentReqInterval() {
    var AgentJID = topic_Get(tApp, "RoomContactAgentJID");
    var AgentObjectReqTopic = AgentJID;
    if(MediaRoomIntervalID == null) {
      MediaRoomIntervalID = setAgentReqInterval(MediaRoomStatusInterval, AgentJID, MediaRoomStatusAgentObj, "Ready" );
      if( eventHandlerArr[AgentObjectReqTopic] == null ) {
        eventHandlerArr[AgentObjectReqTopic] = new EventListner(AgentObjectReqTopic);
        eventHandlerArr[AgentObjectReqTopic].subscribe(MediaRoomAgentObjectReqSuccessEventVar, 
                                                     null, this, MediaRoomStatusRepaint);
      }
      
    }  
  }
  
  function clearMediaRoomStatusAgentReqInterval() {
    var AgentJID = topic_Get(tApp, "RoomContactAgentJID");
    clearAgentObjRequest(AgentJID, MediaRoomStatusAgentObj);
    clearInterval(MediaRoomIntervalID);
    MediaRoomIntervalID = null;
    MediaRoomStatusRepaint();  
  } 
                                   
  eventApp.subscribe("StreamSelected", null, this,
                      function() {
                        clearMediaRoomStatusAgentReqInterval();
                        setMediaRoomStatusAgentReqInterval(); 
                      } 
                    );
                    
  eventApp.subscribe("ProgramSelected", null, this,
                      function() {
                        clearMediaRoomStatusAgentReqInterval();
                        topic_Set(tApp, "StreamSelected", "");
                        topic_Set(tApp, "RoomContactAgentJID", "");
                        topic_Set(tApp, "RoomNID", "");                      
                                        
                      } 
                    );
                    
  eventApp.subscribe("MenuSelection", null, this,
                      function() {
                        var MenuSelection = topic_Get(tApp, 'MenuSelection');
                        if( (MenuSelection != null) && (MenuSelection != "PROGRAM") ) {
                          if(MediaRoomIntervalID != null) {
                            clearMediaRoomStatusAgentReqInterval();
                          }
                        } else {
                          cmpMtxActiveProgramList.repaint();
                          var StreamSelected = topic_Get(tApp, "StreamSelected");
                          if( StreamSelected != null ) {
                            setMediaRoomStatusAgentReqInterval();   
                          }
                        }
                      } 
                    );                                        
                 
  var cmpMtxStreamStatisticsList = new CmpNIDElementRefreshList("mtxProgramStreamStat",
                                                          "ProgramSelected",
                                                          ".//systemtracklist/StreamTrack/streamlist/Stream",
                                                          {jsxid:{objxpath:"\"./@NID\""},
                                                           //jsxtext:{objxpath:"\".//StreamTrack/data/description\""},
                                                           jsxtext:{objxpath:"\"./@NID\""},
                                                           jsxSrcAgentJID:{objxpath:"\".//data/sourceAgentJID\""},
                                                           jsxRoomNID:{objxpath:"\".//data/roomNID\""}
                                                           //jsxSourceDeviceInputInstanceNID:{objxpath:"\".//Instance/sourceDeviceInputInstanceNID\""},
                                                           //jsxSourceDeviceOutputInstanceNID:{objxpath:"\".//StreamTrack//streamlist/Stream/Instance/sourceDeviceOutputInstanceNID\""}
                                                          },
                                                          [/*{action:"REQEVENT", eventvar:"ProgramDataState", eventval:null},*/
                                                           {action:"APPEVENT", eventvar:"ProgramSelected", eventval:null}],
                                                          {action:"FNCCALL", 
                                                            context:this,
                                                                fnc:function() {
                                                                  if(MediaRoomIntervalID != null) {
                                                                    var AgentJID = topic_Get(tApp, "RoomContactAgentJID");
                                                                    clearAgentObjRequest(AgentJID, MediaRoomStatusAgentObj)
                                                                  }
                                                                  //topic_Set(tApp, "StreamSelected", "");
                                                                  //topic_Set(tApp, "RoomContactAgentJID", "");
                                                                  //topic_Set(tApp, "RoomNID", "");
                                                                  var selRecordID = cmpMtxStreamStatisticsList.BlkJSX.getValue();
                                                                  if(selRecordID != null) {                                                                                                                                      
                                                                    var srcAgentJID = cmpMtxStreamStatisticsList.BlkJSX.getRecord(selRecordID).jsxSrcAgentJID;
                                                                    var roomNID = cmpMtxStreamStatisticsList.BlkJSX.getRecord(selRecordID).jsxRoomNID;
                                                                    var StreamSelected = cmpMtxStreamStatisticsList.BlkJSX.getRecord(selRecordID).jsxtext;
                                                                    topic_Set(tApp, "RoomContactAgentJID", srcAgentJID);
                                                                    topic_Set(tApp, "RoomNID", roomNID);
                                                                    topic_Publish(tApp, "StreamSelected", StreamSelected);
                                                                  }
                                                                }
                                                          }
                                                        );

  var cmpMtxStreamList = new CmpNIDElementRefreshList("mtxProgramStreamDetail",
                                                "ProgramSelected",
                                                ".//systemtracklist/StreamTrack/streamlist/Stream",
                                                {jsxid:{objxpath:"\"./@NID\""},
                                                 jsximg:{objxpath:"\".//*/profileXML//StreamType\"",
                                                         lookuparr:{V2D:"images/icons/icon_v2d.gif",
                                                                    MPEG2:"images/icons/icon_mpeg2.gif",
                                                                    MPEG4:"images/icons/icon_mpeg4.gif"
                                                                   }
                                                 },
                                                 //jsxtext:{objxpath:"\".//StreamTrack//streamlist/Stream//data/description\""},
                                                 jsxtext:{objxpath:"\"./@NID\""},
                                                 jsxStatus:{objxpath:"\".//Status/state\"",
                                                            lookuparr:{0:"Stop",
                                                                       1:"Waiting for XAUTH",
                                                                       2:"Waiting for CONTACTJID",
                                                                       3:"Waiting for DEST",
                                                                       4:"Watch sent to Dest",
                                                                       5:"Waiting for Src",
                                                                       6:"Waiting for StreamURL",
                                                                       7:"Waiting for StreamStarted",
                                                                       8:"Start",
                                                                       9:"Pause",
                                                                       10:"Denied XAUTH",
                                                                       202:"Stream Error",
                                                                       203:"Src Error",
                                                                       204:"Dest Error"
                                                                     }
                                                  },
                                                 jsxAction:{objxpath:"\".//data/trackStateAtStart\""},
                                                 jsxPropertyPVR:{objxpath:"\".//StreamTrack/data/pvrEnabled\"",
                                                                  lookuparr:{"false":"Disabled",
                                                                             "true":"Enabled"
                                                                            }
                                                 },
                                                 jsxPropertyRelay:{objxpath:"\".//*//relayNID\""/*,
                                                                   formathandler:{"Type":"Static",
                                                                                   "FncName":"checkRelayNID",
                                                                                   "Interval":null
                                                                                  } SJ-was not being used*/
                                                 },
                                                 jsxDestAgentJID:{objxpath:"\".//*//destAgentJID\""},
                                                 jsxSrcAgentJID:{objxpath:"\".//*//sourceAgentJID\""},                                                      
                                                 jsxStreamProfileName:{objxpath:"\".//*/profileXML//Title\""},
                                                 jsxStreamIsMulticast:{objxpath:"\".//*/profileXML//V2DStreamConfig/IsMulticast\"",
                                                                   lookuparr:{"false":"No",
                                                                              "true":"Yes"
                                                                            }
                                                 }, 
                                                 jsxStreamAVOption:{objxpath:"\".//*/profileXML//V2DStreamConfig/AVOption\""},
                                                 jsxStreamKbm:{objxpath:"\".//*/profileXML//V2DEncoderConnectionParameterKbmConfig/EnableKbm\"",
                                                                   lookuparr:{"false":"No",
                                                                              "true":"Yes"
                                                                            }
                                                  },                                         
                                                 jsxStreamBandwidth:{objxpath:"\".//*/profileXML//V2DEncoderConnectionParameterConfig/Bandwidth\""}
                                                },
                                                [/*{action:"REQEVENT", eventvar:"ProgramDataState", eventval:null},*/
                                                 {action:"APPEVENT", eventvar:"ProgramSelected", eventval:null}],
                                                {persist:{col:"jsxtext",initval:false}
                                                }
                                               );

  var cmpBlkProgramActiveDetailList = new CmpContainer( "blkMonitor_ProgramDetails", 
                              [ {cmp:cmpMtxStreamList},    
                                //{cmp:cmpBlkStreamRecording},
                               // {cmp:cmpBlkStreamHistory},   
                                {cmp:cmpMtxStreamStatisticsList}
                              ],
                              null );


  var cmpChkProgramActive = "";
  var cmpChkProgramCollab = "";
  var cmpChkProgramChannel = "";
  
  var cmpBtnProgramRefreshSet = new CmpButton("btnProgramRefreshSet",
                                               null, 
                                               {action:"FNCCALL", 
                                                context:this,
                                                    fnc:function() {
                                                      var newMediaRoomListRefreshInterval = PNameSpace.getJSXByName("txtProgramRefreshTime").getValue();
                                                      var chkValue = checkIntergerValue(newMediaRoomListRefreshInterval, "txtProgramRefreshTime", "Please enter an integer refresh time value");
                                                      if(chkValue) {
                                                        if(newMediaRoomListRefreshInterval == 0 ) {
                                                          var agree = confirm("Are you sure you want to disable mediaroom refresh?");
                                                          if(agree) {
                                                            topic_Publish(tApp, "MediaRoomListRefreshInterval",newMediaRoomListRefreshInterval);
                                                          }
                                                        } else if(newMediaRoomListRefreshInterval < 5) {
                                                          alert("Please enter an integer refresh time value greater than 5");
                                                          PNameSpace.getJSXByName("txtProgramRefreshTime").focus();
                                                          return false;
                                                        } else {
                                                          topic_Publish(tApp, "MediaRoomListRefreshInterval",newMediaRoomListRefreshInterval);
                                                        }
                                                      }

                                                    }
                                               }
                                             );

  //cmpBtnProgramRefreshSet.ProgramRefreshTime = PNameSpace.getJSXByName("txtProgramRefreshTime");                                            
  
  var cmptxtProgramRefreshSet = new CmpTxtBox("txtProgramRefreshTime",
                                               topic_Get(tApp, "MediaRoomListRefreshInterval"),
                                               null,
                                               null,
                                               null
                                              );
                                                                            
  var cmpMtxActiveProgramList = new CmpNIDRefreshList("mtxProgramList",
                                                      [{type:"appobjtype",objtype:"MediaRoomList",
                                                        xpath:".//MediaRoom[.//data/roomType='Session' or .//data/roomType='Connection']", refreshInterval:"MediaRoomListRefreshInterval"}
                                                      ],
                                                      {jsxid:{objtype:"MediaRoomList",
                                                              objxpath:"\"./@NID\""},
                                                       jsximg:{objtype:"MediaRoomList",
                                                               objxpath:"\".//data/roomType\"",
                                                               lookuparr:{"Connection":"images/icons/chan20x30.gif",
                                                                          "Session":"images/icons/collab20x30.gif"}},
                                                       jsxtext:{objtype:"MediaRoomList",
                                                                //objxpath:"\".//data/Info/Title\""},
                                                                objxpath:"\"./@NID\""},
                                                       jsxDuration:{objtype:"MediaRoomList",
                                                                    objxpath:"\".//data/creationWallClock\"",
                                                                    formathandler:{"Type":"Static",
                                                                                   "FncName":"showDuration",
                                                                                   "Interval":null
                                                                                  }
                                                                   },
                                                       jsxCreationTime:{objtype:"MediaRoomList",
                                                                        objxpath:"\".//data/creationWallClock\""},
                                                       jsxStartTime:{objtype:"MediaRoomList",
                                                                     objxpath:"\".//data/creationWallClock\"",
                                                                     formathandler:{"Type":"Static",
                                                                                    "FncName":"@datetime",
                                                                                    "Interval":null
                                                                                    }
                                                                     }
                                                      },
                                                      [//{action:"REQEVENT", eventvar:"ProgramDataState", eventval:null},
                                                       {action:"APPEVENT", eventvar:"MediaRoomListRefreshInterval", eventval:null}],
                                                      {persist:{col:"jsxtext",initval:false},
                                                       action:"APPEVENT", eventvar:"ProgramSelected", eventval:"jsxid"}
                                                     );
                                              

  var cmpBlkProgramActiveList = new CmpContainer( "blkMonitor_ActivePrograms", 
                              [ {cmp:cmpMtxActiveProgramList},
                                {cmp:cmptxtProgramRefreshSet},
                                {cmp:cmpBtnProgramRefreshSet} /*,
                                {cmp:cmpChkProgramActive},
                                {cmp:cmpChkProgramCollab},
                                {cmp:cmpChkProgramChannel},
                                {cmp:cmpBtnProgramSearch}*/
                              ],
                              null );

  var cmpBlkProgram = new CmpContainer( "blkPrograms",  
                        [ {cmp:cmpBlkProgramActiveList},
                          {cmp:cmpBlkProgramActiveDetailList}   
                        ],
                        null );


  
  return cmpBlkProgram;
  
  function checkMonitorTab() {
    var activeIndex = StreamTabPane.getSelectedIndex();
    if( activeIndex == 3) {
      return true;
    } else {
      return false;
    }
  }  
    
  
  function cmpStreamStatisticsInput_Repaint() {
    var inputInstanceNode = null;
    var inputInstanceNID = null;
    var instanceStatusNode = null;
    var instanceStatusString = null;
    var BlkJSX = PNameSpace.getJSXByName("blkStreamDetailInput"); 
    
    var strXMLCacheId = BlkJSX.getXMLId();      
    var roomNID = topic_Get(tApp, "RoomNID");
    var agentJID = topic_Get(tApp, "RoomContactAgentJID");
    if(MediaRoomIntervalID != null) {
      var roomResourceInstanceNode = topic_GetNode(agentJID, ".//ResourceInstance[RoomNID='"+roomNID+"']");    
      if(roomResourceInstanceNode != null) {
        inputInstanceNode = topic_GetNode(agentJID, ".//ResourceInstance[RoomNID='"+roomNID+"']/DeviceStreamingServiceInputInstanceNID");
        inputInstanceNID = inputInstanceNode.getValue();
      } else {
        clearMediaRoomStatusAgentReqInterval();
      }
      if(inputInstanceNID != null) {
        instanceStatusNode = topic_GetNode(agentJID,".//StreamingServiceStatus/InstanceList/*[@NID='"+inputInstanceNID+"']/Parameters");
        instanceStatusString = instanceStatusNode.toString();
      }
    } 
    var newDoc = new jsx3.xml.Document();
    newDoc.loadXML(instanceStatusString);    
    PNameSpace.getCache().setDocument(strXMLCacheId, newDoc);     
    BlkJSX.repaint();
  }  // end cmpStreamStatisticsInput_Repaint
  
  function cmpStreamStatisticsOutput_Repaint() {
    var outputInstanceNode = null;
    var outputInstanceNID = null;
    var instanceStatusNode = null;
    var instanceStatusString = null;
    var BlkJSX = PNameSpace.getJSXByName("blkStreamDetailOutput"); 
     
    var strXMLCacheId = BlkJSX.getXMLId();
    var roomNID = topic_Get(tApp, "RoomNID");
    var agentJID = topic_Get(tApp, "RoomContactAgentJID");
    if( MediaRoomIntervalID != null) {
      var roomResourceInstanceNode = topic_GetNode(agentJID, ".//ResourceInstance[RoomNID='"+roomNID+"']");    
      if(roomResourceInstanceNode != null) {
        outputInstanceNode = topic_GetNode(agentJID, ".//ResourceInstance[RoomNID='"+roomNID+"']/DeviceStreamingServiceOutputInstanceNID");
        outputInstanceNID = outputInstanceNode.getValue();
      } else {
        clearMediaRoomStatusAgentReqInterval();
      }    
      if(outputInstanceNID != null) {
        instanceStatusNode = topic_GetNode(agentJID,".//StreamingServiceStatus/InstanceList/*[@NID='"+outputInstanceNID+"']/Parameters");
        instanceStatusString = instanceStatusNode.toString();
      }
    } 
    var newDoc = new jsx3.xml.Document();
    newDoc.loadXML(instanceStatusString);    
    PNameSpace.getCache().setDocument(strXMLCacheId, newDoc);     
    BlkJSX.repaint();
  } //end cmpStreamStatisticsOutput_Repaint
  

  
} // end create

function cmpBlkProgram_Destroy() {
  
}

//format handler global function
function checkRelayNID(element, cdfkey, matrix, column, rownumber, server) {
  if( element.innerText != "") {
    element.innerHTML = "Yes";
  } else {
    element.innerHTML = "No";
  }
}
