function cmpWinMediaPlayer_Create(mediaInfo, joinCollabMode, windowInstanceArr, tMediaPlayer, eventMediaPlayer) {

  tMediaPlayer = jsx3.util.strEmpty(tMediaPlayer) ? mediaInfo.sessionId : tMediaPlayer;
  if (!topic_Exists(tMediaPlayer)) {
    topic_Create(tMediaPlayer);
  }
  if (!eventMediaPlayer){
    eventMediaPlayer = new EventListner(tMediaPlayer);
  }
  this.timeoutID = null;
  this.WINDOWCLOSINGDELAY = 1000;

  //keep topic in the array
  var windowIndex = mediaInfo.StreamNID ? mediaInfo.StreamNID : mediaInfo.sessionId;
  windowInstanceArr[windowIndex].topic = tMediaPlayer;
  
  //init "SaveAndClose" and "MediaSrcGroupNumber"
  topic_Set(tMediaPlayer, "SaveAndClose", "NONE"); 
  topic_Set(tMediaPlayer, "MediaPlayerMode", "NONE");
  topic_Set(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
  topic_Set(tMediaPlayer, "ShowViewOptions", "NONE");
  topic_Set(tMediaPlayer, "MediaPlayerDstLoadingStatus", "NONE");
  //initial player load
  topic_Set(tApp, "PlayerLoaded", "NONE");

  function cmpWinMediaPlayer_Destroy() {
    // unsubscribe click from the player
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, winMediaPlayer, "processClick");

    var winMediaListHandler = PNameSpace.getAppWindow(tMediaList);
    if ((topic_Get(tMediaPlayer, "SaveAndClose") != "NONE") && 
        (winMediaListHandler != null)&&(winMediaListHandler.isOpen())){
      winMediaListHandler.close();
    }
    // set to initial value
    topic_Set(tApp, "PlayerLoaded", "NONE");
    //closing the status window
    var statusWinName = topic_Get(tMediaPlayer, "SessionStatusWindow");
    var winSessionStatusHandler = PNameSpace.getAppWindow(statusWinName);
    if ( (winSessionStatusHandler != null)&&(winSessionStatusHandler.isOpen())){
      winSessionStatusHandler.close();
    }

    var PlayerLoaded_ID = topic_Get(tMediaPlayer, "PlayerLoaded_ID");
    if (!jsx3.util.strEmpty(PlayerLoaded_ID)) {
      eventApp.unsubscribe("PlayerLoaded", PlayerLoaded_ID);
    }
    var ENTITYPRESENCEEVENT_DST_ID = topic_Get(tMediaPlayer, "ENTITYPRESENCEEVENT_DST_ID");
    if (!jsx3.util.strEmpty(ENTITYPRESENCEEVENT_DST_ID)) {
      eventApp.unsubscribe("ENTITYPRESENCEEVENT", ENTITYPRESENCEEVENT_DST_ID);
    }
    var ENTITYPRESENCEEVENT_SRC_ID = topic_Get(tMediaPlayer, "ENTITYPRESENCEEVENT_SRC_ID");
    if (!jsx3.util.strEmpty(ENTITYPRESENCEEVENT_SRC_ID)) {
      eventApp.unsubscribe("ENTITYPRESENCEEVENT", ENTITYPRESENCEEVENT_SRC_ID);
    }

    // wipe out any pending resource instances
    agtMediaStreamDst.delResourceInstance(tMediaPlayer);
    // clear registration with dst agent
    agtMediaStreamDst.unregisterStreamDst(tMediaPlayer);
    // destroy any eventlistners
    eventMediaPlayer.destroy();
    eventMediaPlayer = null;
    topic_Destroy(tMediaPlayer);
    winMediaPlayer = null;
    this.timeoutID = null;
    //remove it from the associative array
    delete windowInstanceArr[windowIndex];
  }

  var winMediaPlayer = new CmpAppWindow(tMediaPlayer, eventMediaPlayer, "components/mediaPlayer.xml",
                                         { WinTitle: "Media Player",
                                           WinHeight: 900,
                                           WinWidth: 1300,
                                           WinResizable: true,
                                           WinScrollable: true
                                         },
                                          null,
                                          null,
                                          { action: "FNCCALL",
                                            context: null,
                                            fnc: function () {
                                              log.debug("Closing MediaPlayer window...");
                                              if (topic_Get(tApp, "LoginUserJID") == mediaInfo.SessionOwnerUserJID || topic_Get(tApp, "LoginUserJID") == topic_Get(tMediaPlayer, "SessionOwnerUserJID")) {
                                                //Closing a window Launched from MediaList,tear down connections #18515
                                                //                                                if (topic_Get(tMediaPlayer, "CollaborationState") != "DISCONNECTING") {
                                                //                                                  topic_Publish(tMediaPlayer, "ControllerEventRequest", "STOP");
                                                //                                                }
                                                //bug fix 20295, using easy macro recorder auto test,left some connections after window closed 
                                                //                                                topic_Publish(tMediaPlayer, "ControllerEvent", "STOP");

                                                //                                                eventMediaPlayer.subscribe("StopSession", "SUCCESS", this,
                                                //                                                                             function() {
                                                //                                                                               log.debug("Stop session success...");
                                                //                                                                               cmpWinMediaPlayer_Destroy();
                                                //                                                                             }
                                                //                                                                          );
                                                // to avoid 2 stopsession reqquest. this is when stop button is clicked on the controller.
                                                if (topic_Get(tMediaPlayer, "SessionID") != "NONE") {
                                                  var reqStopSession = new ReqSessionStartStopRequest("Stop", null,
                                                                                                    { action: "FNCCALL", context: this,
                                                                                                      fnc: function (respCode, respVal) {
                                                                                                        if (respCode == "SUCCESS" && respVal != null) {
                                                                                                          log.debug("Stop session success...");
                                                                                                          cmpWinMediaPlayer_Destroy();
                                                                                                        }
                                                                                                        else {

                                                                                                        }
                                                                                                      }
                                                                                                    }
                                                                                                    );
                                                  reqStopSession.ServiceName = "SessionService";
                                                  var reqParams = new Object;
                                                  reqParams["SessionID"] = mediaInfo.sessionId;
                                                  reqStopSession.reqParams = reqParams;
                                                  reqStopSession.sendRequest(this);
                                                }
                                                else {
                                                  cmpWinMediaPlayer_Destroy();
                                                }
                                              }
                                              else {
                                                //when closing MediaPlayer window, destroy and remove the topic from the collaboration
                                                if (joinCollabMode) topic_Publish(tMediaPlayer, "SessionID", "NONE");
                                                cmpWinMediaPlayer_Destroy();
                                              }
                                            }
                                          }
                                        );

  //keep window handler in the array
  windowInstanceArr[windowIndex].windowHandler = winMediaPlayer;

  //set pixel for lytSrcDstBkmk
  var lytSrcDstBkmk = getJsxByName([this.BlkJSX, "lytSrcDstBkmk"]);
  
  //help function to set lytSrcDstBkmk
  winMediaPlayer.setLytSrcDstBkmk = function(panelType, showType) {
    var currentSettingsArr = lytSrcDstBkmk.getCols().split(',');
    if (panelType == "BKMKPANEL") {
      currentSettingsArr[2] = (showType == "SHOW") ? "300" : "0";
    }
    if (panelType == "SRCPANEL") {
      currentSettingsArr[0] = (showType == "SHOW") ? "300" : "0";
    }
    lytSrcDstBkmk.setCols(currentSettingsArr.join(), true);
  }

  winMediaPlayer.createSubCmp = function () {
    // get vars from object instance
    var winMediaPlayer = this;
    var winMediaPlayerBlkJSX = this.BlkJSX;
    var tMediaPlayer = this.tWin;
    var eventMediaPlayer = this.eventWin;
    var winMediaPlayerWindow = winMediaPlayerBlkJSX._jsxwindow;
    this.sessionId = mediaInfo.sessionId;

    // ------ session media list and connection lists + biz logic
    this.cmpCtrSessionInfo = null;
    //media player controller hide by setting lytHeader
    this.mainBlkJSX = getJsxByName([winMediaPlayerBlkJSX, "lytHeader"]);

    var ThisClass = this;

    var cdfMenuOptions = getJsxByName([this.BlkJSX, "cdfMenuOptions"]);
    this.ViewCheckboxNodes = cdfMenuOptions.getDescendantsOfType(jsx3.gui.CheckBox, true);

    //join a session (for FFTrack case)
    this.joinSession = (mediaInfo.Type && mediaInfo.Type == "ActiveSession");

    // ------ create a multi-player UI
    this.cmpCtrMultiPlayerUI = cmpBlkMultiPlayerUI_Create.call(this, this.mainBlkJSX, eventMediaPlayer);

    // bookmark lazy loading flag
    winMediaPlayer.BookmarkDataLazyloading = false;
    winMediaPlayer.SessionInfoMissConnected = false;

    //keep initial value of 'viewCheckBox'
    winMediaPlayer.initViewCheckboxNodes = new Array;

    // ------ create a player status UI
    this.cmpPlayerStatus = new CmpLabelTxtStream([winMediaPlayerBlkJSX, "lblPlayerStatus"], null,
                                               [{ action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "ERRORSTATUS", eventval: null },
                                                { action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "CollaborationEventMsg", eventval: null },
                                                { action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "MultiPlayerUIEventMsg", eventval: null },
                                                { action: "APPEVENT", eventvar: "IPVideoPlayerStatus", eventval: null}],
                                               null,
                                               "\nMedia Player Event Messages ...",
                                               "JSXAPPS/IPVS_ClientUI/components/dialogEventLog.xml",
                                               "blkEventLog");

    this.cmpLblWindowName = new CmpLabel(([winMediaPlayerBlkJSX, "lblWindowName"]),
                                           null,
                                           [{ action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "CollaborationName", eventval: null}]);

    this.cmpBtnPlayerModeShow = new CmpButton([winMediaPlayerBlkJSX, "btnPlayerMode"],
                                               null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   var playerMode = topic_Get(tMediaPlayer, "ShowMediaPlayerMode");
                                                   if (playerMode == "OPEN") {
                                                     topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                                                   }
                                                   else {
                                                     topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "OPEN");
                                                   }
                                                 }
                                               }
                                             );

    this.cmpBtnComtrollerModeShow = new CmpButton([winMediaPlayerBlkJSX, "btnControllerMode"],
                                                   null,
                                                   { action: "FNCCALL",
                                                     context: null,
                                                     fnc: function () {
                                                       var playerMode = topic_Get(tMediaPlayer, "ShowMediaPlayerMode");
                                                       if (playerMode == "OPEN") {
                                                         topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                                                       }
                                                       else {
                                                         topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "OPEN");
                                                       }
                                                     }
                                                   }
                                                 );



    this.cmpBtnPlayerModeSelect = new CmpButton([winMediaPlayerBlkJSX, "btnPlayerModeSelect"],
                                           null,
                                           { action: "FNCCALL",
                                             context: null,
                                             fnc: function () {
                                               topic_Publish(tMediaPlayer, "MediaPlayerMode", "PLAYER");
                                               topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                                             }
                                           }
                                         );

    this.cmpBtnControllerModeSelect = new CmpButton([winMediaPlayerBlkJSX, "btnControllerModeSelect"],
                                               null,
                                               { action: "FNCCALL",
                                                 context: null,
                                                 fnc: function () {
                                                   topic_Publish(tMediaPlayer, "MediaPlayerMode", "CONTROLLER");
                                                   topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                                                 }
                                               }
                                             );


    this.cmpBlkModeCnt = new Component([winMediaPlayerBlkJSX, "blkPlayerControllerModeCnt"]);
    this.cmpBlkPlayerModeCnt = new CmpContainer([winMediaPlayerBlkJSX, "blkPlayerControllerModeCnt"],
                                                [{ cmp: this.cmpBlkModeCnt,
                                                  sel: "topic_Get('" + tMediaPlayer + "', 'ShowMediaPlayerMode') != 'NONE'"
                                                },
                                                 { cmp: this.cmpBtnPlayerModeSelect },
                                                 { cmp: this.cmpBtnControllerModeSelect}],
                                                [{ action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "ShowMediaPlayerMode", eventval: null}],
                                                null
                                              );

    this.cmpBlkPlayerViewModeDropList = new Component([winMediaPlayerBlkJSX, "blkPlayerViewModeDropList"]);

    this.cmpBtnViewMode = new CmpImageButton([winMediaPlayerBlkJSX, "btnPlayerViewMode"],
                                             null,
                                             { action: "FNCCALL",
                                               context: null,
                                               fnc: function () {
                                                 topic_Set(tMediaPlayer, "ShowViewOptions", "NONE");
                                                 //hide/clsoe the player mode block
                                                 var playerMode = topic_Get(tMediaPlayer, "ShowMediaPlayerMode");
                                                 if (playerMode == "OPEN") {
                                                   topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                                                 }
                                                 //open the view mode
                                                 if (winMediaPlayer.cmpViewMenu.BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK) {
                                                   jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, winMediaPlayer, "processClick");
                                                   winMediaPlayer.cmpViewMenu.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                                                   winMediaPlayer.cancelViewCheckBox();
                                                   return;
                                                 }
                                                 else {
                                                   var ThisCalss = winMediaPlayer;
                                                   jsx3.gui.Event.subscribe(jsx3.gui.Event.CLICK, winMediaPlayer, "processClick");
                                                   winMediaPlayer.cmpViewMenu.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
                                                   jsx3.sleep((function () {
                                                     topic_Set(tMediaPlayer, "ShowViewOptions", "OPEN");
                                                   }), null, winMediaPlayer);

                                                 }
                                                 for (var i = 0; i < winMediaPlayer.ViewCheckboxNodes.length; i++) {
                                                   winMediaPlayer.initViewCheckboxNodes[i] = winMediaPlayer.ViewCheckboxNodes[i].getChecked();
                                                 }
                                               }
                                             });
    this.cancelViewCheckBox = function () {
      for (var i = 0; i < winMediaPlayer.ViewCheckboxNodes.length; i++) {
        var recordNode = winMediaPlayer.ViewCheckboxNodes[i];
        recordNode.setChecked(winMediaPlayer.initViewCheckboxNodes[i]);
      }
    }

    this.processClick = function (objEvent) {
      var xx = "";
      if (objEvent.event.getType() == "click") {
        if (jsx3.util.strEmpty(winMediaPlayer.BlkJSX)) return;
        if (winMediaPlayer.BlkJSX.getWindow().document.hasFocus() && topic_Get(tMediaPlayer, "ShowViewOptions") == "OPEN") {
          var xPos = objEvent.event.getTrueX();
          var yPos = objEvent.event.getTrueY();
          var viewBlkTop = winMediaPlayer.cmpBlkPlayerViewModeDropList.BlkJSX.getTop();
          var viewBlkLeft = winMediaPlayer.cmpBlkPlayerViewModeDropList.BlkJSX.getLeft();
          var viewBlkWidth = winMediaPlayer.cmpBlkPlayerViewModeDropList.BlkJSX.getWidth();
          var viewBlkHeight = winMediaPlayer.cmpBlkPlayerViewModeDropList.BlkJSX.getHeight();

          if ((xPos >= parseInt(viewBlkLeft, 10) && xPos <= parseInt(viewBlkLeft + viewBlkWidth, 10)) && 
              (yPos >= parseInt(viewBlkTop,10) && yPos <= parseInt(viewBlkTop + viewBlkHeight, 10) ) ) {
            //log.error("cmpBlkMediaPlayer::processClick :: WITHIN VIEW OPTIONS AREA X:" + xPos + " Y:" + yPos + "\n");
          }
          else if (winMediaPlayer.cmpViewMenu.BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK) {
            //hide view block
            topic_Set(tMediaPlayer, "ShowViewOptions", "NONE");
            jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, winMediaPlayer, "processClick");
            winMediaPlayer.cmpViewMenu.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
            winMediaPlayer.cancelViewCheckBox();
            //log.error("cmpBlkMediaPlayer::processClick :: OUTSIDE VIEW OPTIONS AREA X:" + xPos + " Y:" + yPos + "\n");
          }
        }
      }
    }


    this.cmpViewMenu = new CmpMenuBlock([winMediaPlayerBlkJSX, "blkPlayerViewModeCnt"],
                                        [winMediaPlayerBlkJSX, "btnPlayerViewMode"],
                                        "btnOK", "btnCancel",
                                        null,
                                        this.cancelViewCheckBox,
                                        { action: "FNCCALL",
                                          context: null,
                                          fnc: function () {
                                            handlePlayerSettings(winMediaPlayer.ViewCheckboxNodes);
                                          }
                                        });

    this.cmpBlkPlayerModeShow = new CmpContainer([winMediaPlayerBlkJSX, "blkIPVSBranding"],
                                         [{ cmp: this.cmpBtnPlayerModeShow,
                                           sel: "topic_Get('" + tMediaPlayer + "', 'MediaPlayerMode') == 'PLAYER'"
                                         },
                                          { cmp: this.cmpBtnComtrollerModeShow,
                                            sel: "topic_Get('" + tMediaPlayer + "', 'MediaPlayerMode') == 'CONTROLLER'"
                                          },
                                          { cmp: this.cmpBtnViewMode,
                                            sel: "topic_Get('" + tMediaPlayer + "', 'MediaPlayerMode') == 'PLAYER'"
                                          },
                                          { cmp: this.cmpBlkPlayerViewModeDropList }
                                         ],
                                         [{ action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "MediaPlayerMode", eventval: null}],
                                         null
                                        );
    this.cmpBlkVideoPanelCnt = new Component([winMediaPlayerBlkJSX, "blkVideoPanelCnt"]);
    this.cmpBlkMediaPlayerCanvasEditorCnt = new Component([winMediaPlayerBlkJSX, "blkMediaPlayerCanvasEditorCnt"]);
    this.cmpBlkCenterPanel = new CmpContainer([winMediaPlayerBlkJSX, "blkCenterPanelCnt"],
                                                 [{ cmp: this.cmpBlkVideoPanelCnt },
                                                  { cmp: this.cmpBlkMediaPlayerCanvasEditorCnt}],
                                                 null,
                                                 null
                                                );

    //blkMediaPlayerRoot
    this.cmpController = new CmpController([winMediaPlayerBlkJSX, "blkMediaPlayerRoot"], tMediaPlayer,
                                        [{ action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "ControllerEvent", eventval: null },
                                         { action: "TOPICEVENT", topic: tMediaPlayer, eventvar: "SessionID", eventval: null}]);

    // new added for lyt control
    //this.cmpBlkSrcPortButtonCtn = cmpBlkSourcePanel_Create.call(this);

    //-------- wrapper all components to a container
    this.SubCmpArr = [{ cmp: this.cmpCtrMultiPlayerUI },
                      { cmp: this.cmpPlayerStatus },
                      { cmp: this.cmpLblWindowName },
                      { cmp: this.cmpController },
                      { cmp: this.cmpBlkPlayerModeShow },
                      { cmp: this.cmpViewMenu, sel: "topic_Get('" + tMediaPlayer + "', 'ViewModeStatus') != 'NONE'" },
                      { cmp: this.cmpBlkCenterPanel },
                      { cmp: this.cmpBlkPlayerModeCnt }
                     ];
    cmpControllerUI_Create.call(this);


    //read default player settings from preferences and set in view menu
    function setViewModeByPreference(checkboxNodes) {
      jsx3.$A(checkboxNodes).each(function (recordNode) {
        var recordName = recordNode.getName();
        //checkbox "Show Bookmarks"
        if (recordName == "chkBookmarks") {
          var setChecked = jsx3.gui.CheckBox.CHECKED;
          var showBookmarks = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowBookmarks", "false");
          if (showBookmarks == "false") {
            //checking the View drop down value
            if (recordNode.getChecked() != 1) {
              setChecked = jsx3.gui.CheckBox.UNCHECKED;
            }
          }
          else {
            //only meet one of two load Bookmark panel 2 conditions(recordChecked )
            log.info("cmpBlkMediaPlayer::set BookmarkDataLazyloading to true for loading bookmark panel by Presences settings.");
            winMediaPlayer.BookmarkDataLazyloading = true;
          }
          recordNode.setChecked(setChecked);
        }
        //If set preference 'single player mode', check "Show Source" 
        if (recordName == "chkSources") {
          var setChecked = jsx3.gui.CheckBox.CHECKED;
          //checkbox "Show Source"
          var showSourcePrf = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowConnections", "false");
          if (showSourcePrf == "false") {
            //checking the View drop down value
            if (recordNode.getChecked() != 1) {
              setChecked = jsx3.gui.CheckBox.UNCHECKED;
            }
          }
          recordNode.setChecked(setChecked);
        }
        //checkbox "Show Controller"
        if (recordName == "chkController") {
          var setChecked = jsx3.gui.CheckBox.CHECKED;
          var showController = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowController", "true");
          if (showController == "false") {
            //checking the View drop down value
            if (recordNode.getChecked() != 1) {
              setChecked = jsx3.gui.CheckBox.UNCHECKED;
            }
          }
          recordNode.setChecked(setChecked);
        }
        //checkbox "Show Fullscreen"
        if (recordName == "chkFullscreen") {
          var setChecked = jsx3.gui.CheckBox.CHECKED;
          var showFullscreen = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/ShowFullscreen", "false");
          if (showFullscreen == "false") setChecked = jsx3.gui.CheckBox.UNCHECKED;
          recordNode.setChecked(setChecked);
        }
      });
    }

    //read from mtx/cdf cmp and change view as needed
    function handlePlayerSettings(checkboxNodes, status) {
      // get each attribute value and change view as needed
      for (var i = 0; i < checkboxNodes.length; i++) {
        var recordNode = checkboxNodes[i];
        var recordName = recordNode.getName();
        var recordChecked = recordNode.getChecked();
        //checkbox "Show Bookmarks"
        if (recordName == "chkBookmarks") {
          //video player & bookmark dimension
          if (recordChecked == 1) {
            //load Bookmark panel 2 conditions: recordChecked 
            if (topic_Get(tMediaPlayer, "BookmarkLoadingStatus") == "NONE" &&
                      topic_Get(tMediaPlayer, "LoadSessionInfoOrBookmark") != "BookmarkPanel") {
              //avoid duplicate loading bookmark which call from "ViewBookmarkEnabled" listener
              log.info("cmpBlkMediaPlayer::handlePlayerSettings:: load bookmark panel by View check list settings. LoadSessionInfoOrBookmark:" + topic_Get(tMediaPlayer, "LoadSessionInfoOrBookmark"));
              topic_Publish(tMediaPlayer, "LoadSessionInfoOrBookmark", "BookmarkPanel");
            }
            ThisClass.setLytSrcDstBkmk("BKMKPANEL", "SHOW");
          }
          else {
            var playerMode = topic_Get(tMediaPlayer, "MediaPlayerMode");
            if (playerMode == "CONTROLLER") {
              ThisClass.setLytSrcDstBkmk("BKMKPANEL", "SHOW");
            }
            else {
              ThisClass.setLytSrcDstBkmk("BKMKPANEL", "HIDE");
            }

            if (recordChecked == 1) {
              /*one user case fall in here:check bookmark from 'View' list and 
              then add in one recording connection to current session*/
              winMediaPlayer.BookmarkDataLazyloading = true;
            }
          }
        }
        //checkbox "Show Connection Info"
        if (recordName == "chkSources") {
          if (recordChecked == 1) {
            ThisClass.setLytSrcDstBkmk("SRCPANEL", "SHOW");
            if (topic_Get(tMediaPlayer, "SessionInfoLoadingStatus") == "NONE" && topic_Get(tMediaPlayer, "LoadSessionInfoOrBookmark") != "SessionInfoPanel") {
              log.info("cmpBlkMediaPlayer::handlePlayerSettings::load sessionInfo panel by View check list settings. LoadSessionInfoOrBookmark:" + topic_Get(tMediaPlayer, "LoadSessionInfoOrBookmark"));
              topic_Publish(tMediaPlayer, "LoadSessionInfoOrBookmark", "SessionInfoPanel");
              /*scenario here is:if check SessionInfo from 'View', need re-publish "CollaborationState"
              for cmpBlkSessionInfo, because clsCtrlAgent join success occurs before SessionInfo loading  */
              if (topic_Get(tMediaPlayer, "CollaborationState") == "CONNECTED") {
                winMediaPlayer.SessionInfoMissConnected = true;
              }

            }
          }
          else {
            var playerMode = topic_Get(tMediaPlayer, "MediaPlayerMode");
            if (playerMode == "CONTROLLER") {
              ThisClass.setLytSrcDstBkmk("SRCPANEL", "SHOW");
            }
            else {
              ThisClass.setLytSrcDstBkmk("SRCPANEL", "HIDE");
            }
          }
        }
        //checkbox "Show Controller"
        if (recordName == "chkController") {
          var controllerRowStr = "26,*,76";
          if (recordChecked == 0) {
            var playerMode = topic_Get(tMediaPlayer, "MediaPlayerMode");
            if (playerMode != "CONTROLLER") {
              //i.e. setting lytHeader to  "26,*,0"
              controllerRowStr = controllerRowStr.replace(/,\d\d/, ",0");
            }
          }
          winMediaPlayer.mainBlkJSX.setRows(controllerRowStr, true);
        }
        //checkbox simulation "fullScreen"
        if (recordName == "chkFullscreen") {
          var defaultChecked = recordNode.getDefaultChecked();
          var aHeight = screen.availHeight;
          var aWidth = screen.availWidth;
          var bHeight = winMediaPlayer.BlkJSX.getWidth();
          var bWidth = winMediaPlayer.BlkJSX.getHeight();
          var fullFlag = (aHeight <= bHeight) && (aWidth <= bWidth);
          //assum 'fullscreen' all the time unchecked by default
          if (recordChecked == jsx3.gui.CheckBox.CHECKED) {
            if (!fullFlag) {
              //check 'FullScreen'
              fullScreen();
              recordNode.setDefaultChecked(recordChecked);
            }
          }
          else {
            if (fullFlag) {
              //check 'FullScreen'
              fullScreen();
              recordNode.setDefaultChecked(recordChecked);
            }
          }
        }
      }
      topic_Set(tMediaPlayer, "ViewModeStatus", "START");
    }

    //A session with view connections,launch media player window and change layout after player loaded
    var PlayerLoaded_ID = eventApp.subscribe("PlayerLoaded", "TRUE", this,
                                              function (eventVar, eventVal) {
                                                //setViewModeByPreference(winMediaPlayer.ViewCheckboxNodes);
                                                handlePlayerSettings(winMediaPlayer.ViewCheckboxNodes);
                                              }
                                            );

    topic_Set(tMediaPlayer, "PlayerLoaded_ID", PlayerLoaded_ID);

    // eventlistener to show the src panel based on the playermode
    eventMediaPlayer.subscribe("MediaPlayerMode", null, this,
                                function (eventVar, eventVal) {
                                  var playerMode = eventVal;
                                  var srcPanelVisible = null;
                                  var bookmarkPanelVisible = null;
                                  var controllerVisible = null;
                                  var controllerRowStr = "26,*,76";
                                  var viewCheckBoxes = winMediaPlayer.ViewCheckboxNodes;
                                  for (var i = 0; i < viewCheckBoxes.length; i++) {
                                    var recordNode = viewCheckBoxes[i];
                                    var recordName = recordNode.getName();
                                    srcPanelVisible = recordNode.getChecked();
                                    if (recordName == "chkSources") {
                                      break;
                                    }
                                  }

                                  for (var i = 0; i < viewCheckBoxes.length; i++) {
                                    var recordNode = viewCheckBoxes[i];
                                    var recordName = recordNode.getName();
                                    bookmarkPanelVisible = recordNode.getChecked();
                                    if (recordName == "chkBookmarks") {
                                      break;
                                    }
                                  }

                                  for (var i = 0; i < viewCheckBoxes.length; i++) {
                                    var recordNode = viewCheckBoxes[i];
                                    var recordName = recordNode.getName();
                                    controllerVisible = recordNode.getChecked();
                                    if (recordName == "chkController") {
                                      break;
                                    }
                                  }

                                  if (playerMode == "CONTROLLER") {
                                    if (this.cmpBlkCenterPanel.BlkJSX.getRows() != "*,0") {
                                      //bug 27143: player container not update with controller settings
                                      this.cmpBlkCenterPanel.BlkJSX.setRows("*,0", true);
                                    }
                                    //show the src panel if already visible then leave not then show
                                    //if (srcPanelVisible == "0") {
                                    winMediaPlayer.setLytSrcDstBkmk("SRCPANEL", "SHOW");
                                    if (topic_Get(tMediaPlayer, "SessionInfoLoadingStatus") == "NONE") {
                                      log.info("cmpBlkMediaPlayer::MediaPlayerMode::load sessionInfo panel by View check list settings. SessionInfoLoadingStatus::" + topic_Get(tMediaPlayer, "SessionInfoLoadingStatus"));
                                      topic_Publish(tMediaPlayer, "LoadSessionInfoOrBookmark", "SessionInfoPanel");
                                      /*scenario here is:if check SessionInfo from 'View', need re-publish "CollaborationState"
                                      for cmpBlkSessionInfo, because clsCtrlAgent join success occurs before SessionInfo loading  */
                                      if (topic_Get(tMediaPlayer, "CollaborationState") == "CONNECTED") {
                                        winMediaPlayer.SessionInfoMissConnected = true;
                                      }
                                    }
                                    //}

                                    //show the bookmark panel
                                    //if (bookmarkPanelVisible == "0") {
                                    winMediaPlayer.setLytSrcDstBkmk("BKMKPANEL", "SHOW");
                                    //load Bookmark panel 2 conditions: recordChecked 
                                    if (topic_Get(tMediaPlayer, "BookmarkLoadingStatus") == "NONE" &&
                                                topic_Get(tMediaPlayer, "LoadSessionInfoOrBookmark") != "BookmarkPanel") {
                                      //avoid duplicate loading bookmark which call from "ViewBookmarkEnabled" listener
                                      log.info("cmpBlkMediaPlayer::MediaPlayerMode::load bookmark panel by View check list settings. SessionInfoLoadingStatus::" + topic_Get(tMediaPlayer, "SessionInfoLoadingStatus"));
                                      topic_Publish(tMediaPlayer, "LoadSessionInfoOrBookmark", "BookmarkPanel");
                                    }
                                    //}

                                    //if (controllerVisible == "0") {
                                    winMediaPlayer.mainBlkJSX.setRows(controllerRowStr, true);
                                    //}

                                    if (topic_Get(tMediaPlayer, "MediaPlayerDstLoadingStatus") == "NONE") {
                                      topic_Set(tMediaPlayer, "MediaPlayerDstLoadingStatus", "LOADING");
                                      log.info("cmpBlkMediaPlayer::MediaPlayerMode::Loading destination panel");
                                      cmpBlkMediaPlayerDestPanel_Create.call(this);
                                    }
                                  }
                                  else {
                                    if (this.cmpBlkCenterPanel.BlkJSX.getRows() != "0,*") {
                                      //bug 27143: player container not update with controller settings
                                      this.cmpBlkCenterPanel.BlkJSX.setRows("0,*", true);
                                    }
                                    if (srcPanelVisible == "0") {
                                      winMediaPlayer.setLytSrcDstBkmk("SRCPANEL", "HIDE");
                                    }
                                    if (bookmarkPanelVisible == "0") {
                                      winMediaPlayer.setLytSrcDstBkmk("BKMKPANEL", "HIDE");
                                    }
                                    if (controllerVisible == "0") {
                                      //i.e. setting lytHeader to  "26,*,0"
                                      controllerRowStr = controllerRowStr.replace(/,\d\d/, ",0");
                                      winMediaPlayer.mainBlkJSX.setRows(controllerRowStr, true);
                                    }

                                    //'true':single player mode;'false':multiplayer mode
                                    var singlePlayer = topic_Get(tMediaPlayer, "SinglePlayerMode");
                                    var nodeSession = agtCollaborationCtrl.getSession(tMediaPlayer, topic_Get(tMediaPlayer, "SessionID"));
                                    if (!jsx3.util.strEmpty(nodeSession)) {
                                      var dstPortName = topic_Get(tApp, "DefaultPortTitle");
                                      //get the destentity id from the destportnid
                                      var destEntityNode = nodeSession.getEntityByName(dstPortName);
                                      if (!jsx3.util.strEmpty(destEntityNode)) {
                                        var dstEntityId = destEntityNode.getAttribute("id");
                                        var dstPortNID = destEntityNode.selectSingleNode(".//Port").getAttribute("resourceNID");
                                        topic_Set(tMediaPlayer, "MediaPlayerDstPortSelected", dstPortNID);
                                        topic_Set(tMediaPlayer, "MediaPlayerDstPortEntityId", dstEntityId);
                                        topic_Set(tMediaPlayer, "MediaPlayerDstPortSelectedName", dstPortName);
                                        topic_Set(tMediaPlayer, "MediaPlayerDstPortInstanceCount", "16");
                                        topic_Set(tMediaPlayer, "MediaPlayerDstPortWindowSelected", "NONE");
                                        topic_Set(tMediaPlayer, "MediaPlayerDstPortCanvasSelected", "NONE");

                                        if (singlePlayer == "true") {
                                          var connId = topic_Get(tMediaPlayer, "SingleMediaPlayerConnId");
                                          if (topic_Get(tMediaPlayer, "SingleMediaPlayerWinId") == "NONE") {
                                            var connNode = nodeSession.getConnById(connId);
                                            if (!jsx3.util.strEmpty(connNode)) {
                                              var windowId = nodeSession.getConnWindowId(connNode);
                                              if ((windowId == "undefined") || jsx3.util.strEmpty(windowId)) {
                                                windowId = "NONE";
                                              }
                                              //this topic preserves the windowid
                                              topic_Set(tMediaPlayer, "SingleMediaPlayerWinId", windowId);
                                              topic_Set(tMediaPlayer, "MediaPlayerDstPortWindowSelected", windowId);
                                              var canvasId = nodeSession.getConnCanvasId(connNode);
                                              if ((canvasId == "undefined") || jsx3.util.strEmpty(canvasId)) {
                                                canvasId = "NONE";
                                              }
                                              //this topic preserves the canvasid
                                              topic_Set(tMediaPlayer, "SingleMediaPlayerCanvasId", canvasId);
                                              topic_Set(tMediaPlayer, "MediaPlayerDstPortCanvasSelected", canvasId);
                                            }
                                          }
                                          else {
                                            topic_Set(tMediaPlayer, "MediaPlayerDstPortWindowSelected", topic_Get(tMediaPlayer, "SingleMediaPlayerWinId"));
                                            topic_Set(tMediaPlayer, "MediaPlayerDstPortCanvasSelected", topic_Get(tMediaPlayer, "SingleMediaPlayerCanvasId"));
                                          }
                                        }
                                      }
                                    }

                                    //update grid view
                                    jsx3.sleep(function () { topic_Publish(tMediaPlayer, "MultiPlayerViewUpdate", (new Date)); }, null, this);
                                  }

                                });


    // close window when get SessionStop
    eventMediaPlayer.subscribe("SessionStop", null, this,
                      function (eventVar, eventVal) {
                        if (winMediaPlayer != null && winMediaPlayer.BlkJSX != null) {
                          winMediaPlayer.close();
                        }
                      });

    // load panel based on Preferences.xml setting and get bookmark data after ViewBookmarkEnabled
    eventMediaPlayer.subscribe("ViewBookmarkEnabled", null, this,
                                function (eventVar, eventVal) {
                                  if (winMediaPlayer.BookmarkDataLazyloading) {
                                    log.info("cmpBlkMediaPlayer::load bookmark panel by 'Preferences' when ViewBookmarkEnable='1' and winMediaPlayer.BookmarkDataLazyloading=" + winMediaPlayer.BookmarkDataLazyloading);
                                    winMediaPlayer.BookmarkDataLazyloading = false;
                                    topic_Publish(tMediaPlayer, "LoadSessionInfoOrBookmark", "BookmarkPanel");
                                    handlePlayerSettings(winMediaPlayer.ViewCheckboxNodes);
                                    //lazy reading bookmark data when neccessary
                                    topic_Publish(tMediaPlayer, "ShowBookmarkPanel", (newServerDate()).getTime());
                                  }
                                });

    // when multiplayer FullScreenOff re-set sessionInfo & bookmark panel 
    eventMediaPlayer.subscribe("FullScreenOff", null, this,
                      function (eventVar, eventVal) {
                        if (eventVal) {
                          //hide/close the view mode
                          if (winMediaPlayer.cmpViewMenu.BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK) {
                            topic_Set(tMediaPlayer, "ShowViewOptions", "NONE");
                            jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, winMediaPlayer, "processClick");
                            winMediaPlayer.cmpViewMenu.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                            winMediaPlayer.cancelViewCheckBox();
                          }
                          //hide the player mode
                          if (topic_Get(tMediaPlayer, "ShowMediaPlayerMode") == "OPEN") {
                            topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                          }
                          handlePlayerSettings(winMediaPlayer.ViewCheckboxNodes);
                        }
                        else {
                          //hide/close the view mode
                          if (winMediaPlayer.cmpViewMenu.BlkJSX.getDisplay() == jsx3.gui.Block.DISPLAYBLOCK) {
                            topic_Set(tMediaPlayer, "ShowViewOptions", "NONE");
                            jsx3.gui.Event.unsubscribe(jsx3.gui.Event.CLICK, winMediaPlayer, "processClick");
                            winMediaPlayer.cmpViewMenu.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
                            winMediaPlayer.cancelViewCheckBox();
                          }
                          //hide the player mode
                          if (topic_Get(tMediaPlayer, "ShowMediaPlayerMode") == "OPEN") {
                            topic_Publish(tMediaPlayer, "ShowMediaPlayerMode", "NONE");
                          }
                        }
                      });

    //event for closing auto join window & #18715 If 'Stop' then close window
    eventMediaPlayer.subscribe("SessionID", null, this,
                      function (eventVar, eventVal) {
                        var sessionId = eventVal;
                        if (sessionId == "NONE") {//&& mediaInfo.Type == "AutoJoin") 
                          log.info("MediaPlayer: " + tMediaPlayer + " SessionID: " + mediaInfo.sessionId + " mediaInfo type: " + mediaInfo.Type);
                          // HACK: without timeout was causing stack overflow 
                          if (winMediaPlayer != null && winMediaPlayer.BlkJSX != null) {
                            ThisClass.timeoutID = window.setTimeout(function () {
                              winMediaPlayer.close();
                            }, ThisClass.WINDOWCLOSINGDELAY);
                          }
                        }
                      });

    //lazy loading SessionInfo and Bookmark by 'View' check list
    eventMediaPlayer.subscribe("LoadSessionInfoOrBookmark", null, this,
                      function (eventVar, eventVal) {
                        if (eventVal == "SessionInfoPanel" && topic_Get(tMediaPlayer, "SessionInfoLoadingStatus") == "NONE") {
                          topic_Set(tMediaPlayer, "SessionInfoLoadingStatus", "LOADING");
                          cmpBlkSourcePanel_Create.call(this);
                        }
                        if (eventVal == "BookmarkPanel" && topic_Get(tMediaPlayer, "BookmarkLoadingStatus") == "NONE") {
                          topic_Set(tMediaPlayer, "BookmarkLoadingStatus", "LOADING");
                          cmpBlkSessionBookmark_Create.call(this);
                        }
                      });
    //SessionInfo loading successful
    eventMediaPlayer.subscribe("SessionInfoLoadingStatus", "LOADED", this,
                      function (eventVar, eventVal) {
                        /*user scenario:setting sessionInfo from 'View' but miss clsCtrlAgent publish
                        "CollaborationState" then publish here again to get sessionInfo data  */
                        if (topic_Get(tMediaPlayer, "CollaborationState") == "CONNECTED") {
                          topic_Publish(tMediaPlayer, "MediaPlayerSrcPortGetData", (newServerDate()).getTime());
                        }
                      });

    //Session Dest port loading successful
    eventMediaPlayer.subscribe("CollaborationState", "CONNECTED", this,
                                function (eventVar, eventVal) {
                                  if ((topic_Get(tMediaPlayer, "MediaPlayerDstPortGetData") != "READY") &&
                                      (topic_Get(tMediaPlayer, "MediaPlayerDstLoadingStatus") == "LOADED")) {
                                    topic_Publish(tMediaPlayer, "MediaPlayerDstPortGetData", "READY");
                                  }
                                  //publish the player mode for the 1st time so that dest port values can be set for the multi/single player
                                  topic_Publish(tMediaPlayer, "MediaPlayerMode", topic_Get(tMediaPlayer, "MediaPlayerMode"));

                                  if (topic_Get(tMediaPlayer, "BookmarkLoadingStatus") == "LOADED") {
                                    topic_Publish(tMediaPlayer, "ShowBookmarkPanel", (newServerDate()).getTime());
                                  }
                                });

    //Session Dest port loading successful
    eventMediaPlayer.subscribe("MediaPlayerDstLoadingStatus", "LOADED", this,
                                function (eventVar, eventVal) {
                                  if ((topic_Get(tMediaPlayer, "MediaPlayerDstPortGetData") != "READY") &&
                                      (topic_Get(tMediaPlayer, "CollaborationState") == "CONNECTED")) {
                                    topic_Publish(tMediaPlayer, "MediaPlayerDstPortGetData", "READY");
                                  }
                                });

    //load Bookmark by View check list, if loaded 
    eventMediaPlayer.subscribe("BookmarkLoadingStatus", "LOADED", this,
                                function (eventVar, eventVal) {
                                  // fire event to get bookmarks data
                                  if (topic_Get(tMediaPlayer, "CollaborationState") == "CONNECTED") {
                                    topic_Publish(tMediaPlayer, "ShowBookmarkPanel", (newServerDate()).getTime());
                                  }
                                });

    //bug fix 19331 start and stop session from API cause player window left in half way   
    if (mediaInfo.Type == "AutoJoin") {
      // when auto join start and stop the media player window open with no sessionID
      eventMediaPlayer.subscribe("ERRORSTATUS", null, this,
                        function (eventVar, eventVal) {
                          var errorResponseStr = eventVal;
                          if (errorResponseStr.match(/ErrorResponse/) && errorResponseStr.match(/ObjectCount="0"/)) {
                            winMediaPlayer.close();
                          }
                        });
    }

    // init and repaint happens after the window is loaded
    var ThisClass = this;
    jsx3.sleep((function () {
      if (ThisClass.BlkJSX._jsxopener) {
        if (winBrowseSessionsBlkJSX != null) {
          winBrowseSessionsBlkJSX._jsxwindow.blur();
          winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
        }
        if (winMediaController != null) {
          winMediaController.BlkJSX._jsxwindow.blur();
          winMediaController.BlkJSX._jsxwindow.hideFocus = false;
        }
        if (winLiveSessions != null) {
          winLiveSessions.BlkJSX._jsxwindow.blur();
          winLiveSessions.BlkJSX._jsxwindow.hideFocus = false;
        }

      } else if (ThisClass.BlkJSX._jsxpB) {
        if (winBrowseSessionsBlkJSX != null) {
          winBrowseSessionsBlkJSX._jsxwindow.blur();
          winBrowseSessionsBlkJSX._jsxwindow.hideFocus = false;
        }
        if (winMediaController != null) {
          winMediaController.BlkJSX._jsxwindow.blur();
          winMediaController.BlkJSX._jsxwindow.hideFocus = false;
        }
        if (winLiveSessions != null) {
          winLiveSessions.BlkJSX._jsxwindow.blur();
          winLiveSessions.BlkJSX._jsxwindow.hideFocus = false;
        }
      }

      ThisClass.topicSetInit();
      ThisClass.init();
      ThisClass.show();
      topic_Publish(tMediaPlayer, "MediaPlayerInit", "DONE");
      if (joinCollabMode) {
        topic_Publish(tMediaPlayer, "SessionID", mediaInfo.sessionId);
      }
      topic_Publish(tMediaPlayer, "CollaborationName", topic_Get(tMediaPlayer, "CollaborationName"));
      setViewModeByPreference(ThisClass.ViewCheckboxNodes);
      handlePlayerSettings(ThisClass.ViewCheckboxNodes);
    }
    ), null, this);

  }


  //topic set initializations
  winMediaPlayer.topicSetInit = function() {
    // setup state vars
    topic_Set(tMediaPlayer, "CollaborationName", mediaInfo.Name);
    topic_Set(tMediaPlayer, "ViewModeStatus", "NONE");
    topic_Set(tMediaPlayer, "MuteStateAll", "NONE");
    topic_Set(tMediaPlayer, "UpdateMute", "NONE");
    topic_Set(tMediaPlayer, "SessionInfoLoadingStatus", "NONE");
    topic_Set(tMediaPlayer, "BookmarkLoadingStatus", "NONE");
    topic_Set(tMediaPlayer, "LoadSessionInfoOrBookmark", "NONE");
    topic_Set(tMediaPlayer, "SingleMediaPlayerConnId", "NONE");
    topic_Set(tMediaPlayer, "MediaPlayerDstPortSelected", "NONE");
    topic_Set(tMediaPlayer, "MediaPlayerDstPortEntityId", "NONE");
    topic_Set(tMediaPlayer, "MediaPlayerDstPortSelectedName", "NONE");
    topic_Set(tMediaPlayer, "MediaPlayerDstPortInstanceCount", "NONE");
    topic_Set(tMediaPlayer, "MediaPlayerDstPortWindowSelected", "NONE");
    topic_Set(tMediaPlayer, "MediaPlayerDstPortCanvasSelected", "NONE");
    topic_Set(tMediaPlayer, "SingleMediaPlayerWinId", "NONE");
    topic_Set(tMediaPlayer, "SingleMediaPlayerCanvasId", "NONE");
    topic_Set(tMediaPlayer, "SessionOwnerUser", 'TRUE');
    topic_Set(tMediaPlayer, "CollaborationFFTrackState", 'NONE');
    topic_Set(tMediaPlayer, "SpeedSettingBtnState", 'NONE');
    topic_Set(tMediaPlayer, "PlayBySpeed", '1.0x');
    
    // setup default player preferences
    var selViewerAspectRatio = getLocalPreferences(PNameSpace, ".//AspectRatio", "4:3");
    var selViewerFps = getLocalPreferences(PNameSpace, ".//Fps", 25);
    var selViewerSkipFWD = getLocalPreferences(PNameSpace, ".//SkipFWD", 30000);
    var selViewerSkipBack = getLocalPreferences(PNameSpace, ".//SkipBack", 10000);
    var selViewerBandwidth = getLocalPreferences(PNameSpace, ".//BandwidthKbps", 10240);
    var selViewerChkUseHTTP = getLocalPreferences(PNameSpace, ".//UseHTTP", 1);
    var selViewerChkUseMMS = getLocalPreferences(PNameSpace, ".//UseMMS", 0);

    var IPVSPlayerDefaultServerPort = getLocalPreferences(PNameSpace, ".//ServerPort", "6060");
    var IPVSPlayerDefaultAVOption = getLocalPreferences(PNameSpace, ".//AVOption", 3);
    var IPVSPlayerDefaultMulticast = getLocalPreferences(PNameSpace, ".//Multicast", 0);
    var IPVSPlayerDefaultSliceReduction = getLocalPreferences(PNameSpace, ".//SliceReduction", 1);
    var IPVSPlayerDefaultEnableKbm = getLocalPreferences(PNameSpace, ".//EnableKbm", 1);
    var IPVSDefaultLocalKBMSState = getLocalPreferences(PNameSpace, ".//LocalKBMSState", 0);
    var IPVSPlayerDefaultBorder = getLocalPreferences(PNameSpace, ".//Border", 8);
    var IPVSPlayerDefaultPriority = getLocalPreferences(PNameSpace, ".//Priority", "false");


    topic_Set(tApp, "IPVSPlayerDefaultAspectRatio", selViewerAspectRatio);
    topic_Set(tApp, "IPVSPlayerDefaultFps", selViewerFps);
    topic_Set(tApp, "IPVSPlayerDefaultSkipFWD", selViewerSkipFWD);
    topic_Set(tApp, "IPVSPlayerDefaultSkipBack", selViewerSkipBack);
    topic_Set(tApp, "IPVSPlayerDefaultBandwidthKbps", selViewerBandwidth);
    topic_Set(tApp, "IPVSPlayerDefaultUseHTTP", selViewerChkUseHTTP);
    topic_Set(tApp, "IPVSPlayerDefaultUseMMS", selViewerChkUseMMS);

    topic_Set(tApp, "IPVSPlayerDefaultServerPort", IPVSPlayerDefaultServerPort);
    topic_Set(tApp, "IPVSPlayerDefaultAVOption", IPVSPlayerDefaultAVOption);
    topic_Set(tApp, "IPVSPlayerDefaultMulticast", IPVSPlayerDefaultMulticast);
    topic_Set(tApp, "IPVSPlayerDefaultSliceReduction", IPVSPlayerDefaultSliceReduction);
    topic_Set(tApp, "IPVSPlayerDefaultEnableKbm", IPVSPlayerDefaultEnableKbm);
    topic_Set(tApp, "IPVSDefaultLocalKBMSState", IPVSDefaultLocalKBMSState);
    //set player Border to tApp
    topic_Set(tApp, "IPVSPlayerDefaultBorder", IPVSPlayerDefaultBorder);
    topic_Set(tApp, "IPVSPlayerDefaultPriority", IPVSPlayerDefaultPriority);
  }
  
  //get default single player mode preference setting
  var showSinglePlayer = getLocalPreferences(PNameSpace, ".//ClientApp[@name='IPVSClient']/SinglePlayerMode", "false");
  topic_Set(tMediaPlayer, "SinglePlayerMode", showSinglePlayer);
  // get out dest port nid
  var ResourceNID = topic_Get(tApp, "DestNID");
  topic_Set(tMediaPlayer, "ResourceNID", ResourceNID);
  
  // register with the dst agent
  agtMediaStreamDst.registerStreamDst(tMediaPlayer, ResourceNID);
    
  // open the window - which will call createSub - which will init and show
  winMediaPlayer.open();

}

