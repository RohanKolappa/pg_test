//set up a topic for the browse sessions
var tBrowseSessions = "BrowseSessions";
var tBrowseSessionsDoc = null;
var eventBrowseSessions = null;
var winBrowseSessionsBlkJSX = null;
// the top level block
var winBrowseSessions = null;

var BrowseSessionsCallerID = null;

function cmpWinBrowseSessions_Create(tCaller)
{
  // save the callers topic/name in a gloabl static var
  // this will be used to target events later
  BrowseSessionsCallerID = tCaller;

  // create only once
  if( winBrowseSessions != null ) {
    winBrowseSessions.show();
    return;
  }
  
  // create a topic / listner for messaging in it
  tBrowseSessionsDoc = topic_Create(tBrowseSessions);
  eventBrowseSessions = new EventListner(tBrowseSessions);
 
  function cmpAppWindow_Destroy()
  {
    //destroy any DBUpdate listners
    var DBUpdateEventID = topic_Get(tBrowseSessions, "ConfigListDBUpdateEventID");
    if( ! jsx3.util.strEmpty(DBUpdateEventID) ) {
      eventApp.unsubscribe("DBUpdate", DBUpdateEventID);
    }
    // destroy any eventlistners
    eventBrowseSessions.destroy();
    eventBrowseSessions = null;
    // destroy the local topic
    tBrowseSessionsDoc = null;
    topic_Destroy(tBrowseSessions);
    // dereference the top block
    winBrowseSessionsBlkJSX = null;
    winBrowseSessions = null;
  }
  // Init/Open a window
  winBrowseSessions = new CmpAppWindow(tBrowseSessions, eventBrowseSessions, "components/sessions.xml",
                                       { WinTitle: "Browse Sessions",
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
                                            cmpAppWindow_Destroy();
                                          }
                                        }
                                      );

  //init tabs
  PNameSpace.getJSXByName("blkTabActiveSessions").setClassName(CSSRule_tabOff);
  PNameSpace.getJSXByName("blkTabCfgSessions").setClassName(CSSRule_tabOn);

  winBrowseSessions.createSubCmp = function () {
    // get vars from object instance
    winBrowseSessionsBlkJSX = this.BlkJSX;

    // set the window customize image if any
    var cmpClientWindowHeaderLogo = new Component([winBrowseSessionsBlkJSX, "clientWindowHeaderLogo"], null, null, null);
    cmpClientWindowHeaderLogo.customizeSkin = function () {
                                                  var clientWindowHeaderLogo = clsCustomizeSkin.getAttrValue("IPVSClient", "clientWindowHeaderLogo");
                                                  if (!jsx3.util.strEmpty(clientWindowHeaderLogo)) {
                                                    cmpClientWindowHeaderLogo.BlkJSX.setSrc(clientWindowHeaderLogo);
                                                    cmpClientWindowHeaderLogo.BlkJSX.repaint();
                                                  }
                                                }
    //Configured session tree data
    var cmpMtxMediaList = new CmpCustomPaginationList([winBrowseSessionsBlkJSX, "mtxCfgSessions"],
                                                      [{ action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "UpdateDisplayBrowserSessionList", eventval: null}],
                                                       { persist: { col: "jsxid", initval: false },
                                                         action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "SessionConfigNIDSelected", eventval: "jsxid"
                                                       },
                                                      null,
                                                      null
                                                     );

    cmpMtxMediaList.SortPathArr = { jsxCfgSessionName: "title",
      jsxCfgSessionCreated: "dateCreated"
    };

    cmpMtxMediaList.cmpReqParamArr = { StartFrom: "ObjectList_StartFrom",
      CountToFetch: "ObjectList_CountToFetch",
      SortOrder: "ObjectList_SortOrder",
      ObjectCount: "ObjectCount",
      SortByXpath: "ObjectList_SortBy"
    };
    cmpMtxMediaList.DataUpdatedEvent = { action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "BrowserSessionsDataState", eventval: "LOADED" };
    topic_Set(tBrowseSessions, "BrowserSessionSelectFormat", "EXTENDED");
    cmpMtxMediaList.cmpReq = new cmpReq_GetSessionAPIPaginationList(tBrowseSessions, "GetSessionConfig",
                                                                    [{ VarName: "ObjectList_SelectFormat", VarRef: "BrowserSessionSelectFormat"}],
                                                                    { action: "FNCCALL", context: cmpMtxMediaList,
                                                                      fnc: function (respCode, respVal) {
                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                          this.onResponse(respCode, respVal);
                                                                        }
                                                                        else {
                                                                          log.info("GetRequest failed. Error Code :" + respCode);
                                                                          this.onResponseErrorHandle("GetSessionConfig", respCode, respVal);
                                                                        }
                                                                      }
                                                                    }
                                                                  );

    cmpMtxMediaList.BlkJSX.setXMLTransformers("xsl/BrowserSessionList.xsl");

    cmpMtxMediaList.cmpItemByNIDReqFnc = function () {
      topic_Set(tBrowseSessions, "UpdateForSessionConfigNID", cmpMtxMediaList.WhereNIDArr.join());
      return new cmpReq_GetSessionAPIPaginationList(tBrowseSessions, "GetSessionConfig",
                                                                                         [{ VarName: "ObjectList_WhereNID", VarRef: "UpdateForSessionConfigNID" },
                                                                                          { VarName: "ObjectList_SelectFormat", VarRef: "BrowserSessionSelectFormat" }
                                                                                         ],
                                                                                         { action: "FNCCALL", context: cmpMtxMediaList,
                                                                                           fnc: function (respCode, respVal) {
                                                                                             if (respCode == "SUCCESS" && respVal != null) {
                                                                                               this.handleResponseForNotificationNIDs(respVal, "BrowserSessionList");
                                                                                             }
                                                                                             else {
                                                                                               log.info("GetSessionConfig Request failed. Error Code :" + respCode);
                                                                                               this.onResponseErrorHandle("GetSessionConfig", respCode, respVal);
                                                                                             }
                                                                                           }
                                                                                         }, "ItemByNID"
                                                                                       );
    }

    var cmpSessionNavControls = new CmpPageNavButtonsContainer([winBrowseSessionsBlkJSX, "blkCfgListPaging"],
                                                               [[winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPageFirst"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPagePrev"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPageNext"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPageLast"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "lblRecordInfo"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "txtCurrentRecordNumber"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPageRefresh"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPageRefreshSpinner"],
                                                                [winBrowseSessionsBlkJSX, "blkCfgSessions", "btnPageRefreshNotify"]
                                                               ],
                                                               cmpMtxMediaList,
                                                               [{ action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "BrowserSessionsDataState", eventval: null}],
                                                               null
                                                              );
    //jsxCfgSessionCreated Sessions Tab Panel
    var cmpBlkLiveMediaPanel = new CmpContainer([winBrowseSessionsBlkJSX, "blkCfgSessions"],
                                                [{ cmp: cmpMtxMediaList },
                                                 { cmp: cmpClientWindowHeaderLogo },
                                                 { cmp: cmpSessionNavControls}],
                                                null);


    // Tab Panels                                            
    var cmpBlkTabPanels = new CmpContainer([winBrowseSessionsBlkJSX, "blkTabPanels"],
                                           [{ cmp: cmpBlkLiveMediaPanel}],
                                           [{ action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "ButtonSelection", eventval: null}]
                                          );


    // Action function
    function getMediaInfo() {
      // get the currently selected media info
      var mediaInfo = new Object;
      mediaInfo.Type = "BrowseSession";
      mediaInfo.SessionConfigNID = topic_Get(tBrowseSessions, "SessionConfigNIDSelected");
      var selRecord = cmpMtxMediaList.BlkJSX.getRecord(mediaInfo.SessionConfigNID);
      mediaInfo.Name = selRecord.jsxCfgSessionName;
      //      mediaInfo.Name = topic_GetNIDNode(tCache, mediaInfo.SessionConfigNID).selectSingleNode("//SessionConfig/Title").getValue();
      return mediaInfo;
    }
    function checkNIDSelection() {
      var sessionflag = true;
      var sessionConfigNID = topic_Get(tBrowseSessions, "SessionConfigNIDSelected");
      if (jsx3.util.strEmpty(sessionConfigNID) || sessionConfigNID == "NONE") {
        windowAlert(winBrowseSessionsBlkJSX, "Please select Session");
        sessionflag = false;
      }
      return sessionflag;
    }
    // fnc to get dlg data
    function cmpGet_SessionConfig(SessionConfigNID, Mode) {
      var reqSessionConfig = new ReqSessionGetRequest("GetSessionConfig",
                                                { action: "FNCCALL", context: this,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      var nodeSessionConfig = respVal.selectSingleNode("//SessionConfig");
                                                      if (!jsx3.util.strEmpty(nodeSessionConfig)) {
                                                        var SessionConfigNode = topic_GetNode(tCache, "//SessionConfigList");
                                                        if (SessionConfigNode != null) {
                                                          var orgSessionConfigNode = topic_GetNIDNode(tCache, SessionConfigNID);
                                                          if (orgSessionConfigNode != null) {
                                                            SessionConfigNode.replaceNode(nodeSessionConfig, orgSessionConfigNode);
                                                          }
                                                          else {
                                                            SessionConfigNode.appendChild(nodeSessionConfig);
                                                          }
                                                        }
                                                        if (Mode == "Start") {
                                                          StartSessionConfig();
                                                        }
                                                        else if (Mode == "Edit") {
                                                          EditSessionConfig();
                                                        }
                                                      }
                                                      else {
                                                        log.info("GetSessionConfigRequest failed." + respVal);
                                                      }
                                                    }
                                                    else {
                                                      log.error("Get SessionConfig List: " + respVal);
                                                    }
                                                  }
                                                }
                                               );
      reqSessionConfig.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_WhereNID"] = SessionConfigNID;
      reqParams["ObjectList_SelectFormat"] = "EXTENDED";
      reqSessionConfig.reqParams = reqParams;
      // send the request
      reqSessionConfig.sendRequest(this);
    }
    function StartSessionConfig() {
      var mediaInfo = getMediaInfo();
      // check the sessionCOnfig
      var sessionConfig = topic_GetNIDNode(tCache, mediaInfo.SessionConfigNID);
      // check if any plyFile is present
      var plyFileIer = sessionConfig.selectNodeIterator(".//Entity[@type='PlyFile']");
      //if no then send the start request
      if (!plyFileIer.hasNext()) {
        preSetSessionXML(mediaInfo, sessionConfig);
      }
      // if yes then check the recVersion
      else if (plyFileIer.hasNext()) {
        var recVersion = sessionConfig.selectSingleNode(".//Settings/Playback").getAttribute("recVersion");
        // send the start request
        if (recVersion == "DEFAULT") {
          preSetSessionXML(mediaInfo, sessionConfig);
        }
        // else get the versions
        else {
          getPlaybackOption(mediaInfo);
        }
      }
    }
    function EditSessionConfig() {
      var mediaInfo = getMediaInfo();
      // publish the media info to the callers topic
      if (mediaInfo.SessionConfigNID != null) {
        topic_Publish(BrowseSessionsCallerID, "CollabSessionEdit", mediaInfo);
      }
    }

    var cmpBtnSpinner = new CmpImageButton([winBrowseSessionsBlkJSX, "imgSpinner"], null,
                                           { action: "FNCCALL",
                                             context: null,
                                             fnc: function () {
                                               this.cmpBtnSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                             }
                                           }, null);

    var cmpBlkSpinner = new CmpContainer([winBrowseSessionsBlkJSX, "blkSpinner"],
                                         [{ cmp: cmpBtnSpinner}],
                                         null,
                                         null);

    // Connect/Select/Done button
    var cmpBtnConnect = new CmpBlockButton([winBrowseSessionsBlkJSX, "blkOpenButton"], null,
                                          { action: "FNCCALL",
                                            context: null,
                                            fnc: function () {
                                              if (checkNIDSelection()) {
                                                cmpBtnSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                                var sessionConfigNID = topic_Get(tBrowseSessions, "SessionConfigNIDSelected");
                                                var ThisClass = this;
                                                jsx3.sleep((function () {
                                                  winBrowseSessionsBlkJSX._jsxwindow.blur();
                                                  winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
//                                                  if (winBrowseSessionsBlkJSX._jsxopener) {
//                                                    winBrowseSessionsBlkJSX._jsxopener.blur();
//                                                    winBrowseSessionsBlkJSX._jsxopener.hideFocus = false;
//                                                  } else if (winBrowseSessionsBlkJSX._jsxpB) {
//                                                    winBrowseSessionsBlkJSX._jsxpB.blur();
//                                                    winBrowseSessionsBlkJSX._jsxpB.hideFocus = false;
//                                                  }
                                                  cmpGet_SessionConfig(sessionConfigNID, "Start");
                                                }), null, ThisClass);


                                              }
                                            }
                                          }, null);

    var cmpBlkConnect = new CmpContainer([winBrowseSessionsBlkJSX, "blkOpenButton"],
                                            [{ cmp: cmpBtnConnect}],
                                            null,
                                            null);
    // Edit button
    var cmpBtnEdit = new CmpBlockButton([winBrowseSessionsBlkJSX, "blkEditButton"], null,
                                          { action: "FNCCALL",
                                            context: null,
                                            fnc: function () {
                                              // get selected media info
                                              if (checkNIDSelection()) {
                                                var sessionConfigNID = topic_Get(tBrowseSessions, "SessionConfigNIDSelected");
                                                var ThisClass = this;
                                                jsx3.sleep((function () {
                                                  winBrowseSessionsBlkJSX._jsxwindow.blur();
                                                  winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
//                                                  if (winBrowseSessionsBlkJSX._jsxopener) {
//                                                    winBrowseSessionsBlkJSX._jsxopener.blur();
//                                                    winBrowseSessionsBlkJSX._jsxopener.hideFocus = false;
//                                                  } else if (winBrowseSessionsBlkJSX._jsxpB) {
//                                                    winBrowseSessionsBlkJSX._jsxpB.blur();
//                                                    winBrowseSessionsBlkJSX._jsxpB.hideFocus = false;
//                                                  }
                                                  cmpGet_SessionConfig(sessionConfigNID, "Edit");
                                                }), null, ThisClass);


                                              }
                                            }
                                          }, null);

    var cmpBlkEdit = new CmpContainer([winBrowseSessionsBlkJSX, "blkEditButton"],
                                        [{ cmp: cmpBtnEdit}],
                                        null,
                                        null);

    // Delete button
    var cmpBtnDelete = new CmpBlockButton([winBrowseSessionsBlkJSX, "blkDeleteButton"],
                                          [{ action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "SessionConfigNIDSelected", eventval: null}],
                                          { action: "FNCCALL",
                                            context: null,
                                            fnc: function () {
                                              // get selected media info
                                              if (checkNIDSelection()) {
                                                var mediaInfo = getMediaInfo();
                                                // publish the media info to the callers topic
                                                if (mediaInfo.SessionConfigNID != null &&
                                                     mediaInfo.SessionConfigNID != "NONE") {
                                                  var selRecord = cmpMtxMediaList.BlkJSX.getRecord(mediaInfo.SessionConfigNID);
                                                  var SelectedCollabName = selRecord.jsxCfgSessionName;

                                                  var agree = windowConfirm(winBrowseSessionsBlkJSX, 'Delete Session Config: "' + SelectedCollabName + '" ? ');
                                                  if (agree) {
                                                    cmpBlkCollaborationDelete(mediaInfo.SessionConfigNID);
                                                  }
                                                }
                                                else {
                                                  windowAlert(winBrowseSessionsBlkJSX, "Please select a collaboration configure to delete");
                                                  return false;
                                                }
                                              }
                                            }
                                          }, null);

    var cmpBlkDelete = new CmpContainer([winBrowseSessionsBlkJSX, "blkDeleteButton"],
                                          [{ cmp: cmpBtnDelete}],
                                          null,
                                          null);

    // Action buttons
    var cmpBlkActionButtons = new CmpContainer([winBrowseSessionsBlkJSX, "blkMediaListSubHeader"],
                                               [{ cmp: cmpBlkConnect,
                                                 sel: "clsUserPermission.getPermission('SessionConfig', 'start') == 'true'"
                                               },
                                                { cmp: cmpBlkSpinner,
                                                  sel: "clsUserPermission.getPermission('SessionConfig', 'start') == 'true'"
                                                },
                                                { cmp: cmpBlkDelete,
                                                  sel: "clsUserPermission.getPermission('SessionConfig', 'delete') == 'true'"
                                                },
                                                { cmp: cmpBlkEdit,
                                                  sel: "clsUserPermission.getPermission('SessionConfig', 'modify') == 'true'"
                                                }],
                                               null
                                              );

    var cmpBtnPlaybackOptionCancel = new CmpButton([winBrowseSessionsBlkJSX, "blkMediaPlaybackVersion", "btnMediaPlaybackVersionCancel"],
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function () {
                                                       cmpBtnSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                       topic_Publish(tBrowseSessions, "PlaybackOptionSelected", "NONE");
                                                     }
                                                   }
                                                );

    var cmpBtnPlaybackOptionOK = new CmpButton([winBrowseSessionsBlkJSX, "blkMediaPlaybackVersion", "btnMediaPlaybackVersionOk"],
                                               null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   //topic_Publish(tBrowseSessions, "PlaybackOptionSelected", "NONE");
                                                   var mediaInfo = getMediaInfo();
                                                   setPlaybackOption(mediaInfo);
                                                 }
                                               }
                                            );

    var cmpBlkMediaPlaybackVersionList = new Component([winBrowseSessionsBlkJSX, "blkMediaPlaybackVersion", "blkMediaPlaybackVersionList"]);

    var cmpDlgPlaybackOption = new CmpContainer([winBrowseSessionsBlkJSX, "dlgMediaPlaybackVersion"],
                                                [{ cmp: cmpBlkMediaPlaybackVersionList },
                                                 { cmp: cmpBtnPlaybackOptionCancel },
                                                 { cmp: cmpBtnPlaybackOptionOK }

                                                 ],
                                                null
                                               );
    var cmpPlaybackOption = new CmpContainer(winBrowseSessionsBlkJSX,
                                            [{ cmp: cmpDlgPlaybackOption,
                                              sel: "topic_Get('" + tBrowseSessions + "', 'PlaybackOptionSelected') != 'NONE'"
                                            }],
                                            [{ action: "TOPICEVENT", topic: tBrowseSessions, eventvar: "PlaybackOptionSelected", eventval: null}],
                                            null
                                           );
    // Top level sub cmp array
    this.SubCmpArr = [{ cmp: cmpPlaybackOption },
                      { cmp: cmpBlkTabPanels },
                      { cmp: cmpBlkActionButtons }
                     ];

    // init selection vars
    topic_Set(tBrowseSessions, "ButtonSelection", "Active");
    topic_Set(tBrowseSessions, "SessionCollaborationGroupSelected", "NONE");
    topic_Set(tBrowseSessions, "SessionConfigNIDSelected", "NONE");
    topic_Set(tBrowseSessions, "PlaybackOptionSelected", "NONE");

    var ConfigListDBUpdateEventID = eventApp.subscribe("DBUpdate", null, this,
                                                     function (EventVar, EventVal) {
                                                       if (jsx3.util.strEmpty(EventVal)) return;
                                                       if (EventVal.objType == "SessionConfig") {
                                                         cmpMtxMediaList.handleNotification(EventVal);
                                                       }
                                                     });
    topic_Set(tBrowseSessions, "ConfigListDBUpdateEventID", ConfigListDBUpdateEventID);
    // init and repaint happens after the window is loaded
    jsx3.sleep((function () {
      this.init();
      this.show();
      cmpMtxMediaList.ResetToStart(1, 25, 0);
      topic_Publish(tBrowseSessions, "UpdateDisplayBrowserSessionList", (newServerDate()).getTime());
    }
               ),
               null, this);

    function sendSetSessionXML(mediaInfo, sessionConfig, tMediaPlayer, eventMediaPlayer) {
      // defined in the cmpBlkUI.js
      // setup a Session node using the sessioninfo from the sessionconfig
      //send StartSession request and Join it in MediaPlayer to accelarate speed
      var nodeSession = setSessionXML(sessionConfig, mediaInfo.sessionId, mediaInfo.Name, mediaInfo.SessionConfigNID);
      var ThisClass = this;
      agtCollaborationCtrl.startSession(tMediaPlayer, nodeSession.cloneNode(true));
      eventMediaPlayer.subscribe("STARTSESSIONSUCCESS", null, this,
                                  function (eventVar, eventVal) {
                                    log.info("cmpBlkBrowseSessions.startSessionSuccess: agtDisplay.launchPlayerBySessionId: Session Id: " + eventVal + " :tMediaPlayer: " + tMediaPlayer);
                                    cmpBtnSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                    //start a session from BrowseSession the value of sencode parameter of launchPlayerBySessionId joinmode='false'
                                    mediaInfo.SessionOwnerUserJID = topic_Get(tApp, "LoginUserJID");
                                    agtDisplay.launchPlayerBySessionId(mediaInfo, true, tMediaPlayer, eventMediaPlayer);
                                  }
                                );

      eventMediaPlayer.subscribe("ERRORSTATUS", null, this,
                                  function (eventVar, eventVal) {
                                    log.error("CtrlAgent: StartSession: CtrlTopic: " + tBrowseSessions + ": Start Session for SessionID: " + eventVal + "request failed");
                                    cmpBtnSpinner.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                  }
                                );
    } // end prepareSetSessionXML


    function preSetSessionXML(mediaInfo, sessionConfig) {

      if (jsx3.util.strEmpty(mediaInfo.Name)) {
        mediaInfo.Name = "BrowseSession";
      }
      var sessionName = mediaInfo.Name;
      var sessionId = "Session_" + getUUID();
      mediaInfo.sessionId = sessionId;
      var sessionConfigNID = mediaInfo.SessionConfigNID;

      log.info("cmpBlkBrowseSession: BrowseSession: Session Id: " + sessionId);
      // create a dynamic topic for each media player window
      var tMediaPlayer = sessionId;
      // create a topic / listner for messaging in it
      if (!topic_Exists(tMediaPlayer)) {
        topic_Create(tMediaPlayer);
      }
      var eventMediaPlayer = new EventListner(tMediaPlayer);

      // add SessionConfigNID to all the recSessionConfigNID attribute(GrpFile, Entity)
      var recSessionConfigIter = sessionConfig.selectNodeIterator(".//*[@recSessionConfigNID]");
      while (recSessionConfigIter.hasNext()) {
        var recSessionConfigNode = recSessionConfigIter.next();
        var recSessionConfigNID = recSessionConfigNode.getAttribute("recSessionConfigNID");
        if (jsx3.util.strEmpty(recSessionConfigNID)) {
          recSessionConfigNode.setAttribute("recSessionConfigNID", mediaInfo.SessionConfigNID);
        }
      }

      sendSetSessionXML(mediaInfo, sessionConfig, tMediaPlayer, eventMediaPlayer);
    }



    function getPlaybackOption(mediaInfo) {
      // get the recSessionCOnfigNID from the sessionconfig
      var sessionConfig = topic_GetNIDNode(tCache, mediaInfo.SessionConfigNID);
      var plyFileEntityIter = sessionConfig.selectNodeIterator(".//Entity[@type='PlyFile']");
      var whereRecSessionConfigNID = "";
      var ObjectRecSessionConfigNIDArr = new Object;
      while (plyFileEntityIter.hasNext()) {
        var plyFileEntityNode = plyFileEntityIter.next();
        var recSessionConfigNID = plyFileEntityNode.selectSingleNode(".//File").getAttribute("recSessionConfigNID");
        var clipId = plyFileEntityNode.selectSingleNode(".//File").getAttribute("clipId");
        var grpFileId = plyFileEntityNode.selectSingleNode(".//File").getAttribute("grpFileId");
        var resourceNID = plyFileEntityNode.selectSingleNode(".//File").getAttribute("resourceNID");
        if (ObjectRecSessionConfigNIDArr[recSessionConfigNID] == null) {
          ObjectRecSessionConfigNIDArr[recSessionConfigNID] = new Object;
        }
        ObjectRecSessionConfigNIDArr[recSessionConfigNID][clipId] = { "GroupFileID": grpFileId,
          "FileNID": resourceNID
        };

      }
      for (var recSessionConfigNID in ObjectRecSessionConfigNIDArr) {
        if (whereRecSessionConfigNID == "") {
          whereRecSessionConfigNID += recSessionConfigNID;
        }
        else {
          whereRecSessionConfigNID = whereRecSessionConfigNID + "," + recSessionConfigNID;
        }
      }
      // prepare get file request
      var reqGetFile = new ReqSessionGetRequest("GetFile",
                                                { action: "FNCCALL", context: this,
                                                  fnc: function (respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) {
                                                      log.warn("Get Files PlayBack Options: " + respVal);
                                                      savePlaybackDataInCache(respVal.cloneNode(true));
                                                      showPlaybackOption(mediaInfo, ObjectRecSessionConfigNIDArr, respVal.cloneNode(true));
                                                    }
                                                    else {
                                                      log.error("Get Files PlayBack Options: " + respVal);

                                                    }
                                                  }
                                                }
                                               );
      reqGetFile.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_WhereSessionConfigNID"] = whereRecSessionConfigNID;
      reqParams["ObjectList_WherePermissions"] = "PLAYBACK";
      reqGetFile.reqParams = reqParams;
      // send the request
      reqGetFile.sendRequest(this);


    } // end getPlaybackOption

    function savePlaybackDataInCache(respVal) {
      // set the playbackdata in the topic
      // init the PlaybackData node
      var PlaybackData = topic_Set(tBrowseSessions, "PlaybackData");
      if (PlaybackData) {
        PlaybackData.removeChildren();
      }
      else {
        PlaybackData = topic_Set(tBrowseSessions, "PlaybackData", null);
      }
      var iterFile = respVal.selectNodeIterator("./File");
      while (iterFile.hasNext()) {
        var fileNode = iterFile.next();
        PlaybackData.appendChild(fileNode);
      }
    } // end savePlaybackDataInCache

    function showPlaybackOption(mediaInfo, ObjectRecSessionConfigNIDArr, playbackDataXML) {
      var sessionConfig = topic_GetNIDNode(tCache, mediaInfo.SessionConfigNID);
      var recVersion = sessionConfig.selectSingleNode(".//Settings/Playback").getAttribute("recVersion");
      // clear up the list
      var playbackOptionBlkJSXList = cmpBlkMediaPlaybackVersionList.BlkJSX.getChildren();
      var playbackBlkJSXArr = [];
      for (var i = 0; i < playbackOptionBlkJSXList.length; i++) {
        var playbackOptionBlkJSX = playbackOptionBlkJSXList[i];
        // if block is visible then get the index
        if (jsx3.util.strEmpty(playbackOptionBlkJSX.getDisplay())) {
          playbackBlkJSXArr.push(playbackOptionBlkJSX);
        }
      }
      if (playbackBlkJSXArr.length > 0) {
        cmpBlkMediaPlaybackVersionList.BlkJSX.removeChildren(playbackBlkJSXArr);
      }

      // check if grpfileID is present
      for (var recSessionConfigNID in ObjectRecSessionConfigNIDArr) {
        var fileType = "MediaGroup";
        for (var clipID in ObjectRecSessionConfigNIDArr[recSessionConfigNID]) {
          var fileIter = null;
          // MediaClip check
          if (jsx3.util.strEmpty(ObjectRecSessionConfigNIDArr[recSessionConfigNID][clipID].GroupFileID)) {
            fileType = "MediaClip";
          }
          if (fileType == "MediaGroup") {
            fileIter = playbackDataXML.selectNodeIterator(".//*[@fileType='" + fileType + "' and @sessionConfigNID='" + recSessionConfigNID + "' and not( FileList//File/@state ='NotReady' or FileList//File/@state ='Busy' or FileList//File/@state ='Error')]");
          }
          else if (fileType == "MediaClip") {
            fileIter = playbackDataXML.selectNodeIterator(".//*[@fileType='" + fileType + "' and @sessionConfigNID='" + recSessionConfigNID + "' and @clipID='" + clipID + "' and @state='Ready']");
          }

          var ObjectList = [];
          while (fileIter.hasNext()) {
            var fileNode = fileIter.next();
            var o = new Object;
            o.NID = fileNode.getAttribute("NID");
            o.clipID = fileNode.getAttribute("clipID");
            o.dateCreated = fileNode.getAttribute("dateCreated");
            o.fileType = fileNode.getAttribute("fileType");
            o.mediaGroupID = fileNode.getAttribute("mediaGroupID");
            o.playlength = fileNode.getAttribute("playlength");
            o.size = fileNode.getAttribute("size");
            o.title = fileNode.getAttribute("title");
            ObjectList.push(o);
          }
          ObjectList.sort(function (a, b) { return parseInt(b.dateCreated) - parseInt(a.dateCreated) });
          createPlaybackOption(ObjectList);
          // if its mediagroup then break else it will display the clips with versions
          if (fileType == "MediaGroup") {
            break;
          }
        }
      }
      //show the dialog
      if (recVersion == "ADHOC") {
        topic_Publish(tBrowseSessions, "PlaybackOptionSelected", "OPEN");
      }
      else if (recVersion == "LATEST") {
        // if LATEST then simulate the OK
        setPlaybackOption(mediaInfo);
      }
    } // end showPlaybackOption

    function createPlaybackOption(ObjectList) {
      var cmpBlkMediaplaybackOption = new Component([winBrowseSessionsBlkJSX, "dlgMediaPlaybackVersion", "blkMediaPlaybackVersionNew"]);

      var newPlaybackOptionBlkJSX = cmpBlkMediaplaybackOption.BlkJSX.doClone(jsx3.app.Model.PERSISTEMBED);
      cmpBlkMediaPlaybackVersionList.BlkJSX.adoptChild(newPlaybackOptionBlkJSX, true);
      // show the block
      newPlaybackOptionBlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);

      var imgFileTyeBlkJSX = getJsxByName([newPlaybackOptionBlkJSX, "imgFileTye"]);
      var lblFileNameBlkJSX = getJsxByName([newPlaybackOptionBlkJSX, "lblFileName"]);
      var selFileVersionBlkJSX = getJsxByName([newPlaybackOptionBlkJSX, "selMediaPlaybackVersion"]);

      var imgFileType = "images/icons/folderMedia.png";
      for (var i = 0; i < ObjectList.length; i++) {
        var o = ObjectList[i];
        if (i == 0) {
          if (o.fileType == "MediaClip") {
            imgFileType = "images/icons/film.png";
          }
          imgFileTyeBlkJSX.setSrc(imgFileType);
          imgFileTyeBlkJSX.repaint();
          lblFileNameBlkJSX.setText(o.title, true);
        }
        // set the versions
        var objVer = new Object;
        objVer.jsxid = o.NID;
        objVer.jsxMediaGrpID = o.mediaGroupID;
        objVer.clipID = o.clipID;
        objVer.fileType = o.fileType;
        objVer.dateCreated = o.dateCreated;
        objVer.jsxtext = o.title + " (" + getDateTimeFromTimeCode(o.dateCreated) + ")";
        selFileVersionBlkJSX.insertRecord(objVer, null, false);
        var selectedval = selFileVersionBlkJSX.getValue();
        if (jsx3.util.strEmpty(selectedval)) {
          selFileVersionBlkJSX.setValue(objVer.jsxid);
        }
      }
      newPlaybackOptionBlkJSX.repaint();
    } // end createPlaybackOption

    function setPlaybackOption(mediaInfo) {
      var PlaybackDataNode = topic_GetNode(tBrowseSessions, "PlaybackData");
      var sessionConfig = topic_GetNIDNode(tCache, mediaInfo.SessionConfigNID);
      var recVersion = sessionConfig.selectSingleNode(".//Settings/Playback").getAttribute("recVersion");
      var objParent = cmpBlkMediaPlaybackVersionList.BlkJSX.getParent();
      // find all descendant of type select
      var selArr = objParent.getDescendantsOfType(jsx3.gui.Select, false);
      for (var i = 0; i < selArr.length; i++) {
        var selBlkJSX = selArr[i];
        var selName = selBlkJSX.getName();
        if (selName != "selMediaPlaybackVersion") continue;

        var selectedNID = selBlkJSX.getValue();
        if (jsx3.util.strEmpty(selectedNID)) continue;
        // find the latest file NID from the select XML

        if (recVersion == "LATEST") {
          var recordIDs = selBlkJSX.getRecordIds();
          var datetimeCreated = 0;
          for (var j = 0; j < recordIDs.length; j++) {
            var recordID = recordIDs[j];
            var recordNode = selBlkJSX.getRecordNode(recordID);
            var recordDateCreated = recordNode.getAttribute("dateCreated");
            if (datetimeCreated < recordDateCreated) {
              datetimeCreated = recordDateCreated;
              selectedNID = recordID;
            }
          }
        }

        var SelectedNode = selBlkJSX.getRecordNode(selectedNID);
        var fileType = SelectedNode.getAttribute("fileType");

        var playbackNode = PlaybackDataNode.selectSingleNode(".//File[@NID='" + selectedNID + "']");
        //if mediagroup the update the mediagroupfile NID also
        if (fileType == "MediaGroup") {
          var playbackNodeFileIter = playbackNode.selectNodeIterator(".//FileList/File");
          var playbackNodeFileGrpFileID = null;
          var sessionConfigEntityFileGrpFileID = null;
          while (playbackNodeFileIter.hasNext()) {
            var playbackNodeFile = playbackNodeFileIter.next();
            var playbackNodeFileClipID = playbackNodeFile.getAttribute("clipID");
            var playbackNodeFileNID = playbackNodeFile.getAttribute("NID");
            //get the respective entity file node from the sessionconfig and replace the NID
            var sessionConfigEntityFileNode = sessionConfig.selectSingleNode(".//Entity/File[@clipId='" + playbackNodeFileClipID + "']");
            if (jsx3.util.strEmpty(sessionConfigEntityFileNode)) { continue; }
            sessionConfigEntityFileNode.setAttribute("resourceNID", playbackNodeFileNID);
            sessionConfigEntityFileGrpFileID = sessionConfigEntityFileNode.getAttribute("grpFileId");
          }
          if (!jsx3.util.strEmpty(sessionConfigEntityFileGrpFileID)) {
            // get the groupfileNID
            var playbackGrpFileNID = playbackNode.getAttribute("NID");
            //set this NID in the sessionConfig
            var sessionConfigGrpFileNode = sessionConfig.selectSingleNode(".//GrpFileList/GrpFile[@id='" + sessionConfigEntityFileGrpFileID + "']");
            sessionConfigGrpFileNode.setAttribute("resourceNID", playbackGrpFileNID);
          }
        }
        else {
          var playbackNodeFileClipID = playbackNode.getAttribute("clipID");
          var playbackNodeFileNID = playbackNode.getAttribute("NID");
          //get the respective entity file node from the sessionconfig and replace the NID
          var sessionConfigEntityFileNode = sessionConfig.selectSingleNode(".//Entity/File[@clipId='" + playbackNodeFileClipID + "']");
          sessionConfigEntityFileNode.setAttribute("resourceNID", playbackNodeFileNID);
        }

      }

      topic_Publish(tBrowseSessions, "PlaybackOptionSelected", "NONE");
      // 
      preSetSessionXML(mediaInfo, sessionConfig);
    } // end setPlaybackOption         


  }   // end winBrowseSessions.createSubCmp
 
  // open the window - which will call createSub - which will init and show
  winBrowseSessions.open();
 
  function cmpBlkCollaborationDelete(collabNID) {
    // delete asset
    var reqDelAsset = new ReqDeleteObjectNID(collabNID, "SessionConfig", 
                                             {action:"FNCCALL", context:this,
                                                fnc:function(respCode, respVal) {
                                                      if(respCode == "SUCCESS" && respVal != null) {
                                                         topic_Publish(tBrowseSessions, "UpdateDisplayBrowserSessionList", (newServerDate()).getTime());
                                                       } else {
                                                         var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                         var ErrorMsg = "Error: Collaboartion delete request failed \n";
                                                         ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                         ErrorMsg += "Error code: " + respCode;
                                                         windowAlert(winBrowseSessionsBlkJSX, ErrorMsg);
                                                      }
                                                    }
                                             }
                                            );
    reqDelAsset.sendRequest();        
  } // end cmpBlkCollaborationDelete
  

  
}


