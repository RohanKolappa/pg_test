//set up a topic for the browse sessions
var tLiveSessions = "LiveSessions";
var tLiveSessionsDoc = null;
var eventLiveSessions = null;
// the top level block
var winLiveSessions = null;
  
var BrowseSessionsCallerID = null;

function cmpWinLiveSessions_Create(tCaller)
{
  // save the callers topic/name in a gloabl static var
  // this will be used to target events later
  LiveSessionsCallerID = tCaller;

  // create only once
  if( winLiveSessions != null ) {
    winLiveSessions.show();
    return;
  }
  
  // create a topic / listner for messaging in it
  tLiveSessionsDoc = topic_Create(tLiveSessions);
  eventLiveSessions = new EventListner(tLiveSessions);

  function cmpAppLiveSessionsWindow_Destroy()
  {
    // destroy any eventlistners
    eventLiveSessions.destroy();
    eventLiveSessions = null;
    // destroy the local topic
    tLiveSessionsDoc = null;
    topic_Destroy(tLiveSessions);
    // dereference the top block
    winLiveSessions = null;
  }
  // Init/Open a window
  winLiveSessions = new CmpAppWindow(tLiveSessions, eventLiveSessions, "components/liveSessions.xml",
                                   { WinTitle: "Live Sessions",
                                     WinHeight: 600,
                                     WinWidth: 1000,
                                     WinResizable: true,
                                     WinScrollable: true
                                   },
                                    null,
                                    null,
                                    { action: "FNCCALL",
                                      context: null,
                                      fnc: function()
                                      {
                                        cmpAppLiveSessionsWindow_Destroy();
                                      }
                                    }
                                  );


  winLiveSessions.createSubCmp = function()
  {
    // get vars from object instance
    var winLiveSessionsBlkJSX = this.BlkJSX;
    var tLiveSessions = this.tWin;
    var eventLiveSessions = this.eventWin;
    var sessionIDListUpdate = false;
 
    var MediaRoomStatusAgentObj = "DeviceStatus_Full";
    var MediaRoomAgentObjectReqSuccessEventVar = arrAgentObjectInfo[MediaRoomStatusAgentObj].successevent.eventvar;
    var MediaRoomIntervalID = null;
    
    // set the window customize image if any
    var cmpClientWindowHeaderLogo = new Component([winLiveSessionsBlkJSX, "clientWindowHeaderLogo"], null, null, null);
    cmpClientWindowHeaderLogo.customizeSkin = function() {
                                                var clientWindowHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientWindowHeaderLogo");
                                                if (!jsx3.util.strEmpty(clientWindowHeaderLogo)) {
                                                  cmpClientWindowHeaderLogo.BlkJSX.setSrc(clientWindowHeaderLogo);
                                                  cmpClientWindowHeaderLogo.BlkJSX.repaint();
                                                }
                                              }    
                                              
    var cmpMtxMediaList = new CmpCustomPaginationList([winLiveSessionsBlkJSX, "mtxLiveSessionsList"],
                                                      [{ action: "TOPICEVENT", topic: tLiveSessions, eventvar: "UpdateDisplayLiveSessionList", eventval: null}],
                                                       { persist: { col: "jsxid", initval: false },
                                                         action: "TOPICEVENT", topic: tLiveSessions, eventvar: "presentSessionID", eventval: "jsxid"
                                                       },
                                                      null,
                                                      null
                                                     );

    cmpMtxMediaList.SortPathArr = { jsxLiveSessionConnectionID: "Session[@id]",
                                    jsxLiveSessionName: "Session[@name]",
                                    jsxLiveSessionCreated: "Properties[@dateCreated]",
                                    jsxLiveSessionState: "Session[@state]",
                                    jsxLiveSessionOwnerJID: "Properties[@ownerUserJID]"
                                  };

    cmpMtxMediaList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
                                       CountToFetch: "ObjectList_CountToFetch",
                                       SortOrder: "ObjectList_SortOrder",
                                       ObjectCount: "ObjectCount",
                                       SortByXpath: "ObjectList_SortBy"
                                     };

    cmpMtxMediaList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tLiveSessions, eventvar: "LiveSessionsDataState", eventval: "LOADED" };
    cmpMtxMediaList.ResetToStart(1, 25, null);
    cmpMtxMediaList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tLiveSessions, "GetSession", null,
                                                                    { action: "FNCCALL", context: cmpMtxMediaList,
                                                                      fnc: function(respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                          this.onResponse(respCode, respVal);
                                                                        }
                                                                        else {
                                                                          log.info("GetSessionListRequest failed. Error Code :" + respCode);
                                                                          this.onResponseErrorHandle("GetSession",respCode, respVal);
                                                                        }
                                                                      }
                                                                    }
                                                                  );

    cmpMtxMediaList.BlkJSX.setXMLTransformers("xsl/LiveSessionList.xsl");

    var cmpSessionNavControls = new CmpPageNavButtonsContainer([winLiveSessionsBlkJSX, "blkLiveSessionsListPaging"],
                                                               [null,
                                                                null,
                                                                null,
                                                                null,
                                                                null,
                                                                null,
                                                                [winLiveSessionsBlkJSX, "btnPageRefresh"],
                                                                [winLiveSessionsBlkJSX, "btnPageRefreshSpinner"]
                                                               ],
                                                               cmpMtxMediaList,
                                                               [{ action: "TOPICEVENT", topic: tLiveSessions, eventvar: "LiveSessionsDataState", eventval: null}],
                                                               null
                                                              );

    //Live Sessions Panel
    var cmpBlkLiveMediaPanel = new CmpContainer([winLiveSessionsBlkJSX, "blkLiveSessionsList"],
                                                [{ cmp: cmpMtxMediaList },
                                                 { cmp: cmpClientWindowHeaderLogo },
                                                 { cmp: cmpSessionNavControls}],
                                                null);

    // Action function
    function getMediaInfo()
    {
      // get the currently selected media info
      var mediaInfo = new Object;
      mediaInfo.Type = "ActiveSession";
      mediaInfo.RoomNID = null;
      mediaInfo.sessionId = topic_Get(tLiveSessions, "presentSessionID");
      var objSelectedRecord = cmpMtxMediaList.BlkJSX.getRecord(mediaInfo.sessionId);
      mediaInfo.Name = objSelectedRecord.jsxLiveSessionName;
      return mediaInfo;
    }

    function checkNIDSelection(){
      var sessionflag = true;
      var sessionConfigNID = topic_Get(tLiveSessions, "presentSessionID");
      if(jsx3.util.strEmpty(sessionConfigNID) || sessionConfigNID == "NONE" ){
        windowAlert(winLiveSessionsBlkJSX, "Please select a live session to open");
        sessionflag = false;
      }
      return sessionflag;
    }

    // Join button
    var cmpBtnOpen = new CmpBlockButton([winLiveSessionsBlkJSX, "blkBtnJoin"], null,
                                          { action: "FNCCALL",
                                            context: this,
                                            fnc: function() {
                                              // get selected media info
                                              if(checkNIDSelection()){
                                                var mediaInfo = getMediaInfo();
                                                if (!jsx3.util.strEmpty(mediaInfo.sessionId)&& mediaInfo.sessionId!="NONE"){
                                                  topic_Publish(tApp, "LiveSessionSelection", mediaInfo);
                                                  // close the live session window
                                                  winLiveSessions.close();
                                                } 
                                              }
                                            }
                                          }, null);

    var cmpBlkOpen = new CmpContainer([winLiveSessionsBlkJSX, "blkBtnJoin"],
                                      [{ cmp: cmpBtnOpen}],
                                      null,
                                      null);
                                              
  //send sessionJoinRequest
    function requestSessionJoin(mediaInfo) {
      var reqGetSessionList = new ReqSessionJoinRequest(mediaInfo.sessionId,
                                    { action: "FNCCALL", context: this,
                                      fnc: function(respCode, respVal) {
                                        var tt = "ttt";
                                        if (respCode == "SUCCESS" && respVal != null) {
                                          mediaInfo.RoomNID = respVal.getValue();
                                          // publish the media info to tApp
                                          mediaInfo.winLiveSessions = winLiveSessions;
                                          topic_Publish(tApp, "LiveSessionIdSelected", mediaInfo);
                                          log.info("cmpBlkLiveSessions::Join session(" + mediaInfo.sessionId + ") successful, roomNID:" + mediaInfo.RoomNID);
                                        }
                                        else {
                                          log.warn("cmpBlkLiveSessions::Request of join session(" + sessionID + ") fail.");
                                          log.error(respVal);
                                        }
                                      }
                                    });
      reqGetSessionList.reqParams = new Object();
      reqGetSessionList.reqParams["Where_xpath"] = "";
      reqGetSessionList.sendRequest(this);
    }
                                        
    // Action buttons
    var cmpBlkActionButtons = new CmpContainer([winLiveSessionsBlkJSX, "blkLiveSessionSubHeader"],
                                               [{cmp: cmpBlkOpen,
                                                 sel: "clsUserPermission.getPermission('SessionConfig', 'read') == 'true'"}],
                                               null
                                              );

    // Top level sub cmp array
    this.SubCmpArr = [{ cmp: cmpBlkLiveMediaPanel },
                      { cmp: cmpBlkActionButtons}];

    // init selection vars
    topic_Set(tLiveSessions, "SessionCollaborationGroupSelected", "NONE");
    topic_Set(tLiveSessions, "SessionConfigNIDSelected", "NONE");
    topic_Set(tApp, "SessionId", null);

    // init and repaint happens after the window is loaded
    jsx3.sleep((function() {
                  this.init();
                  this.show();
                  topic_Publish(tLiveSessions, "UpdateDisplayLiveSessionList", (newServerDate()).getTime());
                }
               ),null, this);
  }
 
  // open the window - which will call createSub - which will init and show
  winLiveSessions.open();
 
}


