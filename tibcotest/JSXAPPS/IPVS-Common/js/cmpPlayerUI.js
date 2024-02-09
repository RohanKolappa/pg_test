function cmpPlayerUI_Create(BlkPlayer, tPly, eventPly, onPlayerLoadEvent)
{
  //var eventPly = eval(eventPly);
  var cmpPlayerMessage = new Component([BlkPlayer,"videoMessage"]);
  var cmpPlayer = new CmpPlayer([BlkPlayer, "videoObjectContainer"], tPly,
                                [{ action: "TOPICEVENT", topic: tPly, eventvar: "PlayerEvent", eventval: null}],
                                onPlayerLoadEvent); 
  
  var cmpHeaderDivider = new Component("lytHeader");
  cmpPlayer.HeaderDividerBlkJSX = cmpHeaderDivider.BlkJSX;
  
  var cmpCenterDivider = new Component([BlkPlayer,"lytCenter"]);
  cmpPlayer.CenterDividerBlkJSX = cmpCenterDivider.BlkJSX;
  
  var cmpTop = new Component(BlkPlayer);
  cmpPlayer.TopBlkJSX = cmpTop.BlkJSX;
  
  //--------------
  var cmpBtn1to1 = new CmpImageButton([BlkPlayer,"btn1to1"], null, 
                                      {action:"FNCCALL", context:this,
                                       fnc:function() {
                                             var NativeSizeMode = topic_Get(tPly, "NativeSizeMode");
                                             if(NativeSizeMode == 0) {
                                               topic_Publish(tPly, "PlayerEvent", "NATIVESIZEON");
                                               topic_Publish(tPly, "NativeSizeMode", 1);
                                             }
                                             else {
                                               topic_Publish(tPly, "PlayerEvent", "NATIVESIZEOFF");
                                               topic_Publish(tPly, "NativeSizeMode", 0);
                                             }
                                           }
                                      });
                                      
  var cmpBtnFullScreen = new CmpImageButton([BlkPlayer,"imgFullScreen"], null, 
                                            {action:"FNCCALL", context:this,
                                             fnc:function() {
                                                   var FullscreenMode = topic_Get(tPly, "FullscreenMode");
                                                   if(FullscreenMode == 0) {
                                                     topic_Publish(tPly, "PlayerEvent", "FULLSCREENON");
                                                     topic_Publish(tPly, "FullscreenMode", 1);
                                                   }
                                                   else {
                                                     topic_Publish(tPly, "PlayerEvent", "FULLSCREENOFF");
                                                     topic_Publish(tPly, "FullscreenMode", 0);
                                                   }
                                                 }
                                            });

  var cmpBtnSnapShot = new CmpImageButton([BlkPlayer,"btnSnapShot"], null,
                                          {action:"FNCCALL", context:this,
                                           fnc:function() {                                                      
                                                 topic_Publish(tPly, "PlayerEvent", "SNAPSHOT");                                                                                                              
                                               }
                                         });
  //--------------
  
  var cmpBtnVolume = new CmpImageButton([BlkPlayer,"imgVolume"], null,    
                                        {action:"FNCCALL", context:this,
                                         fnc:function() {
                                               var MuteState = topic_Get(tPly, "MuteState");
                                               if(MuteState == 0) {
                                                 topic_Publish(tPly, "PlayerEvent", "MUTEON");
                                                 topic_Publish(tPly, "MuteState", 1);
                                               }
                                               else {
                                                 topic_Publish(tPly, "PlayerEvent", "MUTEOFF");
                                                 topic_Publish(tPly, "MuteState", 0);
                                               }
                                             }
                                        });
  //--------------
  var cmpBtnRewind = new CmpImageButton([BlkPlayer,"btnRewind"], null,  
                                        {action:"FNCCALL", context:this,
                                         fnc:function() {
                                               var actionData = topic_Get(tApp, "IPVSPlayerDefaultSkipBack");
                                               //HACK: check if the default "actionData" would put us into negative media time
                                               // - this is only approximate because our idea of current media offset may not be accurate
                                               cmpSlider.updateMediaOffset();
                                               if( cmpSlider.CurrMediaOffset < actionData ) { 
                                                 actionData = cmpSlider.CurrMediaOffset;
                                               }
                                               topic_Set(tPly, "ActionData", actionData);
                                               topic_Publish(tPly, "PlayerEvent", "SKIPBACK");
                                             }
                                        });
  
  var cmpBlkPause = new Component([BlkPlayer,"btnPause"]);
  var cmpBtnPause = new CmpImageButton([BlkPlayer,"btnPause"], null,  
                                        {action:"FNCCALL", context:this,
                                         fnc:function() {
                                               topic_Publish(tPly, "PlayerCtrlMask", "ON");
                                               topic_Publish(tPly, "PlayerEvent", "PAUSE");
                                             }
                                        });
  
  var cmpBlkPlay = new Component([BlkPlayer,"btnPlay"]);
  var cmpBtnPlay = new CmpImageButton([BlkPlayer,"btnPlay"], null,   
                                      {action:"FNCCALL", context:this,
                                       fnc:function() {
                                             var ConnectionState = topic_Get(tPly, "ConnectionState");
                                             if(ConnectionState != "CONNECTED") {
                                               topic_Publish(tPly, "PlayerCtrlMask", "ON");
                                               topic_Publish(tPly, "ConnectionReqState", "PLAYER_ADDCONNECTION");
                                             }
                                             else {
                                               topic_Publish(tPly, "PlayerCtrlMask", "ON");
                                               topic_Publish(tPly, "PlayerEvent", "RESUME");
                                             }
                                           }
                                      });
  
  var cmpBtnStop = new CmpImageButton([BlkPlayer,"btnStop"], null,   
                                      {action:"FNCCALL", context:this,
                                       fnc:function() {
                                             var ConnectionState = topic_Get(tPly, "ConnectionState");
                                             var ConnectionXAuthState = topic_Get(tPly, "ConnectionXAuthState");
                                             if (ConnectionState == "CONNECTING" && ConnectionXAuthState == "WAITING") {
                                              var name=confirm("Authorization will be aborted.")
                                               if(name==true){
                                                  topic_Publish(tPly, "ConnectionReqState", "PLAYER_DELETECONNECTION");
                                                  return true;
                                               }else{
                                                  return false;
                                               }
                                             }
                                             else if (ConnectionState == "CONNECTED") {
                                               var TimeMode = topic_Get(tPly, "TimeMode");
                                               if(TimeMode != "PLAYBACK") {
                                                 var name=confirm("Video will no longer be available.\n             Continue?")
                                                 if(name==true){
                                                    topic_Publish(tPly, "ConnectionReqState", "PLAYER_DELETECONNECTION");
                                                    return true;
                                                 }else{
                                                    return false;
                                                 }
                                               }
                                               else{
                                                 topic_Publish(tPly, "ConnectionReqState", "PLAYER_DELETECONNECTION");
                                               }
                                             }else{
                                                topic_Publish(tPly, "ConnectionReqState", "PLAYER_DELETECONNECTION");
                                             }   
                                           }
                                      });
  
  var cmpBtnFastforward = new CmpImageButton([BlkPlayer,"btnFastforward"], null,   
                                              {action:"FNCCALL", context:this,
                                               fnc:function() {
                                                     var actionData = topic_Get(tApp, "IPVSPlayerDefaultSkipFWD");
                                                     topic_Set(tPly, "ActionData", actionData);
                                                     topic_Publish(tPly, "PlayerEvent", "SKIPFWD");
                                                   }
                                              });
  //--------------
  eventApp.subscribe("PlayerSettings", null, this,
                      function(EventVar, EventVal) {
                        var SkipFwdTip = topic_Get(tApp, "IPVSPlayerDefaultSkipFWD");
                        SkipFwdTip = SkipFwdTip/1000;
                        cmpBtnFastforward.BlkJSX.setTip("Skip Forward - " + SkipFwdTip + " Secs");

                        var SkipBackTip = topic_Get(tApp, "IPVSPlayerDefaultSkipBack");
                        SkipBackTip = SkipBackTip/1000;
                        cmpBtnRewind.BlkJSX.setTip("Skip Back - " + SkipBackTip + " Secs");
                      });
  //--------------
  // Connection Status
  var cmpLblStatusDisplay = new CmpLabelErrorTip([BlkPlayer,"lblStatusDisplay"], null, 
                                         [{action:"TOPICEVENT", topic:tPly, eventvar:"ConnectionStatusMsg", eventval:null},
                                          {action:"TOPICEVENT", topic:tPly, eventvar:"ERRORSTATUS", eventval:null}], 
                                         null, 
                                         "DISCONNECTED");
  //--------------
  // Slider                                       
  var cmpLblTimeCode = new CmpLabel([BlkPlayer,"lblTimeCode"]);
  var cmpLblTimeLength = new CmpLabel([BlkPlayer,"lblTimeLength"]);
  var cmpSliderProgress = new Component([BlkPlayer,"sliderTrackProgress"]);  
  var cmpSlider = new CmpSlider([BlkPlayer,"sliderPointer"], tPly, 
                                [{action:"TOPICEVENT", topic:tPly, eventvar:"PlayerCtrlState", eventval:null},
                                 {action:"TOPICEVENT", topic:tPly, eventvar:"AddTrackActionState", eventval:null}],
                                 {action:"FNCCALL", context:this,
                                  fnc:function(Offset) {
                                        if( Offset > 0 ) {
                                          topic_Set(tPly, "ActionData", Offset);
                                          topic_Publish(tPly, "PlayerEvent", "SKIPFWD");
                                        }
                                        else {
                                          topic_Set(tPly, "ActionData", (Offset * -1));
                                          topic_Publish(tPly, "PlayerEvent", "SKIPBACK");
                                        }
                                      }
                                });
  cmpSlider.blkProgress = cmpSliderProgress;                              
  cmpSlider.lblTimeCode = cmpLblTimeCode;
  cmpSlider.lblTimeLength = cmpLblTimeLength;
  // Bookmarks
  var cmpBlkBookmarks = new Component([BlkPlayer,"blkBookmarks"]);
  var cmpBookmarks = new CmpBookmarks([BlkPlayer,"blkBookmarks"], tPly, "topic_Get('" + tPly + "', 'NIDForBookmark')",
                                      [{action:"APPEVENT", eventvar:"MediaSFRDataState", eventval:null}, 
                                       {action:"TOPICEVENT", topic:tPly, eventvar:"ConnectionState", eventval:null},
                                       {action:"TOPICEVENT", topic:tPly, eventvar:"BookmarkState", eventval:null}],
                                      //{action:"TOPICEVENT", topic:tPly, eventvar:"BookmarkSelected", eventval:null});
                                       {action:"FNCCALL", context:this,
                                        fnc:function(BookmarkNID) {
                                              var bookMarkNode = topic_GetNIDNode(tCache, BookmarkNID);
                                              if( bookMarkNode != null ) {
                                                var bookmarkTC = bookMarkNode.selectSingleNode(".//AtTC").getValue();
                                                topic_Set(tPly, "ActionData", bookmarkTC);
                                                topic_Publish(tPly, "PlayerEvent", "RESUMEFROMMEDIATC");
                                              }
                                              topic_Publish(tPly, "BookmarkSelected", BookmarkNID);
                                            }
                                      });
  cmpBookmarks.blkSlider = cmpSlider;
  //--------------
  var cmpTxtBookmarkComments = new CmpTxtBoxWithEnterKey([BlkPlayer, "txtBookmarkComments"], null, true, null,
                                             { action: "FNCCALL", context: this,
                                               fnc: function() {
                                                     var valBookmarkComments = cmpTxtBookmarkComments.BlkJSX.getValue();
                                                     if (valBookmarkComments != "") {
                                                       doSaveBookmark();
                                                     }
                                               }
                                             });
    
  var cmpBlkBookmark = new Component([BlkPlayer,"btnBookmark"]); 
  var cmpBtnBookmark = new CmpImageButton([BlkPlayer,"btnBookmark"], null, 
                                          {action:"FNCCALL", context:this,
                                           fnc:function() {
                                                 cmpTxtBookmarkComments.BlkJSX.setValue("");
                                                 //get bookmark structure with TC and Offset
                                                 cmpReq_GetNewBookmark(tPly);
                                               }
                                            });

  
  var cmpBlkBookmarkX = new Component([BlkPlayer,"btnBookmarkX"]);                                          
  var cmpBtnBookmarkX = new CmpImageButton([BlkPlayer,"btnBookmarkX"], null, 
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  var name=confirm("Selected bookmark will be deleted.\n              Are you sure?")
                                                  if (name==true){
                                                    var BookmarkNID = topic_Get(tPly, "BookmarkSelected");
                                                    var bookMarkNode = topic_GetNIDNode(tCache, BookmarkNID);
                                                    if( bookMarkNode != null ) {
                                                      // send the request to delete it from the DB
                                                      cmpReq_HandleBookmark(tPly, "DeleteBookmark", BookmarkNID, null);
                                                    }
                                                    cmpTxtBookmarkComments.BlkJSX.setValue("");
                                                    topic_Publish(tPly, "BookmarkSelected", "NONE");
                                                    return true;
                                                  }
                                                  else{
                                                    return false;
                                                  }
                                                }
                                           }); 
  var cmpBlkBookmarkEdit = new Component([BlkPlayer,"btnBookmarkEdit"]);                                          
  var cmpBtnBookmarkEdit = new CmpImageButton([BlkPlayer,"btnBookmarkEdit"], null, 
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  cmpTxtBookmarkComments.BlkJSX.setValue(cmpBookmarks.BookmarkSelectedTitle);
                                                  topic_Publish(tPly, "BookmarkDlgState", "EDIT");
                                                }
                                           }); 
  
  var cmpBtnSaveBookmark = new CmpImageButton([BlkPlayer,"btnSaveBookmark"], null, 
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  doSaveBookmark();
                                               }
                                             });  
                                             
  function doSaveBookmark(){
    var valBookmarkComments = cmpTxtBookmarkComments.BlkJSX.getValue();
    if(valBookmarkComments == ""){
      alert("Please enter a  Bookmark Comment");
      cmpTxtBookmarkComments.BlkJSX.focus();
      return false;
    }
    var Mode = topic_Get(tPly, "BookmarkDlgState");
    var MediaNID = topic_Get(tPly, "NIDForBookmark");
    if( Mode == "ADD" ) {
      var nodeBookmark = topic_GetNode(tPly, "//BookmarkNode/Bookmark");
      if( nodeBookmark == null ) return;
      var BookmarkAuthorUserID = topic_Get(tApp, "LoginUserJID");
      var BookmarkTitle = "Title";
      var BookmarkComment = valBookmarkComments;
      // set the values in the bookmark structure
      nodeBookmark.selectSingleNode(".//MediaStreamFileResourceNID").setValue(MediaNID);
      nodeBookmark.selectSingleNode(".//AuthorUserID").setValue(BookmarkAuthorUserID);
      nodeBookmark.selectSingleNode(".//Title").setValue(BookmarkTitle);
      nodeBookmark.selectSingleNode(".//Comment").setValue(valBookmarkComments);
      // send the request to add it to the DB
      cmpReq_HandleBookmark(tPly, "AddBookmark", null, nodeBookmark);
      topic_Set(tPly, "BookmarkNode", "");
    }
    else if( Mode == "EDIT" ) {
      var BookmarkNID = topic_Get(tPly, "BookmarkSelected");
      var bookmarkNode = topic_GetNIDNode(tCache, BookmarkNID).cloneNode(true);
      if(bookmarkNode != null) {
        var BookmarkDateModified = (newServerDate()).getTime();
        var BookmarkAuthorUserID = topic_Get(tApp, "LoginUserJID");
        bookmarkNode.selectSingleNode(".//DateModified").setValue(BookmarkDateModified);
        bookmarkNode.selectSingleNode(".//AuthorUserID").setValue(BookmarkAuthorUserID);
        bookmarkNode.selectSingleNode(".//Comment").setValue(valBookmarkComments);
        // send the request to update it in the DB
        cmpReq_HandleBookmark(tPly, "UpdateBookmark", BookmarkNID, bookmarkNode);
      }
    }
    topic_Publish(tPly, "BookmarkDlgState", "NONE");
    cmpTxtBookmarkComments.BlkJSX.setValue("");
  }

  var cmpBtnCancelBookmark = new CmpImageButton([BlkPlayer,"btnCancelBookmark"], null, 
                                           {action:"FNCCALL", context:this,
                                            fnc:function() {
                                                  topic_Publish(tPly, "BookmarkDlgState", "NONE");
                                                }
                                           });
  topic_Set(tPly, "BookmarkDlgState", "NONE");                                           
  //--------------
  
  var cmpBlkBookmarkDlg = new CmpContainer( [BlkPlayer,"blkBookmarkContainer"],
                                            [ {cmp:cmpTxtBookmarkComments},
                                              {cmp:cmpBtnSaveBookmark},
                                              {cmp:cmpBtnCancelBookmark}],
                                            null
                                          );
                                                            
  //--------------  
  var cmpBlkMediaInfo = new CmpContainer( [BlkPlayer,"blkMediaInfoContainer"],
                                            [ {cmp:cmpLblTimeCode},
                                              {cmp:cmpLblTimeLength, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED'"},
                                              {cmp:cmpSliderProgress},                            
                                              {cmp:cmpSlider, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED'"},
                                              {cmp:cmpBlkBookmarks, sel:"(topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " + 
                                                                         "topic_Get('" + tPly + "', 'ViewBookmarkEnabled') == 1)"},
                                              {cmp:cmpBookmarks, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " +
                                                                     "topic_Get('" + tPly + "', 'ViewBookmarkEnabled') == 1 && " +
                                                                     "topic_Get('" + tPly + "', 'NIDForBookmark') != ''"}],
                                            [ {action:"TOPICEVENT", topic:tPly, eventvar:"ConnectionState", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"ViewBookmarkEnabled", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"NIDForBookmark", eventval:null}]
                                          );
  //--------------                                                                                  
  var cmpBlkControlButtons = new CmpContainer( [BlkPlayer,"blkButtonContainer"],
                                            [ {cmp:cmpBlkBookmark, sel:"topic_Get('" + tPly + "', 'AddBookmarkEnabled') == 1"},
                                              {cmp:cmpBtnBookmark, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " +
                                                                       "topic_Get('" + tPly + "', 'NIDForBookmark') != '' && " +
                                                                       "topic_Get('" + tPly + "', 'ConnectionRecordState') != 'PAUSED' && " + 
                                                                       "topic_Get('" + tPly + "', 'FileWriteMode') == 1"},
                                              {cmp:cmpBlkBookmarkX, sel:"topic_Get('" + tPly + "', 'EditBookmarkEnabled') == 1"},
                                              {cmp:cmpBtnBookmarkX, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " +
                                                                        "topic_Get('" + tPly + "', 'NIDForBookmark') != '' && " + 
                                                                        "topic_Get('" + tPly + "', 'BookmarkSelected') != 'NONE' && " + 
                                                                        "topic_Get('" + tPly + "', 'FileWriteMode') == 1"},
                                              {cmp:cmpBlkBookmarkEdit, sel:"topic_Get('" + tPly + "', 'EditBookmarkEnabled') == 1"},
                                              {cmp:cmpBtnBookmarkEdit, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " +
                                                                           "topic_Get('" + tPly + "', 'NIDForBookmark') != ''&& " + 
                                                                           "topic_Get('" + tPly + "', 'BookmarkSelected') != 'NONE' && " + 
                                                                           "topic_Get('" + tPly + "', 'FileWriteMode') == 1"},
                                              {cmp:cmpBtnRewind, sel:"topic_Get('" + tPly + "', 'ConnectionPVRState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tPly + "', 'SkipbackDisabled') != 1"},
                                              { cmp: cmpBlkPause, sel: "topic_Get('" + tPly + /*"', 'ConnectionPVRState') == 'CONNECTED' && "*/"', 'ConnectionState') != 'NONE' && " + 
                                                                    "topic_Get('" + tPly + "', 'PlayerCtrlState') != 'Pause'"},
                                              { cmp: cmpBtnPause, sel: "topic_Get('" + tPly + /*"', 'ConnectionPVRState') == 'CONNECTED' && "*/"', 'ConnectionState') != 'NONE' && " + 
                                                                    "topic_Get('" + tPly + "', 'PlayerCtrlState') != 'Stop' && " +  
                                                                    "topic_Get('" + tPly + "', 'PlayerCtrlState') != 'Pause' && " +
                                                                    "topic_Get('" + tPly + "', 'PlayerCtrlMask') != 'ON'"},
                                              {cmp:cmpBlkPlay, sel:"( topic_Get('" + tPly + "', 'ConnectionPVRState') != 'CONNECTED') || " + 
                                                                   "( topic_Get('" + tPly + "', 'ConnectionPVRState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tPly + "', 'PlayerCtrlState') == 'Pause')"},
                                              {cmp:cmpBtnPlay, sel:"( topic_Get('" + tPly + "', 'ConnectionState') == 'NONE' && " + 
                                                                     "topic_Get('" + tPly + "', 'SourceNID') != 'NONE' && " + 
                                                                     "topic_Get('" + tPly + "', 'PlayerCtrlState') != 'Start' && " + 
                                                                     "topic_Get('" + tPly + "', 'PlayerCtrlMask') != 'ON') || " + 
                                                                   "( topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tPly + "', 'PlayerCtrlState') == 'Stop') || " + 
                                                                   "( topic_Get('" + tPly + "', 'ConnectionPVRState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tPly + "', 'PlayerCtrlState') == 'Pause' && " + 
                                                                     "topic_Get('" + tPly + "', 'PlayerCtrlMask') != 'ON')"},
                                              {cmp:cmpBtnStop, sel:"topic_Get('" + tPly + "', 'ConnectionState') != 'NONE' && " + 
                                                                   "topic_Get('" + tPly + "', 'PlayerCtrlState') != 'Stop'"},
                                              {cmp:cmpBtnFastforward, sel:"topic_Get('" + tPly + "', 'ConnectionPVRState') == 'CONNECTED' && " + 
                                                                          "topic_Get('" + tPly + "', 'SkipfwdDisabled') != 1 && " + 
                                                                          "topic_Get('" + tPly + "', 'ConnectionTimeState') != 'LIVE'"},
                                              {cmp:cmpBtnSnapShot, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " +
                                                                       "topic_Get('" + tPly + "', 'SnapShotDisabled') != 1 && " +
                                                                       "topic_Get('" + tPly + "', 'TimeMode') != 'NONE'"},
                                              {cmp:cmpBtnFullScreen, sel:"topic_Get('" + tPly + "', 'SourceNID') != 'NONE'"},
                                              {cmp:cmpBtn1to1, sel:"topic_Get('" + tPly + "', 'ConnectionState') == 'CONNECTED' && " +
                                                                   "topic_Get('" + tPly + "', 'TimeMode') != 'NONE'"}],
                                            [ {action:"TOPICEVENT", topic:tPly, eventvar:"ConnectionState", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"ConnectionPVRState", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"PlayerCtrlState", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"PlayerCtrlMask", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"AddBookmarkEnabled", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"EditBookmarkEnabled", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"BookmarkSelected", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"ConnectionRecordState", eventval:null},
                                              {action:"TOPICEVENT", topic:tPly, eventvar:"NIDForBookmark", eventval:null}]
                                          );
  // intercept the show method and check for bookmark dialog
  // if that is open - just publish a message to close it
  // - that will automatically cascade back
  // if not - pass control to the standard 'show' method
  cmpBlkControlButtons.tPly = tPly;
  cmpBlkControlButtons.showBase = cmpBlkControlButtons.show;
  cmpBlkControlButtons.show = function() {
    var BookmarkDlgState = topic_Get(this.tPly, "BookmarkDlgState");
    if( BookmarkDlgState != "NONE" ) {
      topic_Publish(this.tPly, "BookmarkDlgState", "NONE");
    }
    else {
      this.showBase();
    }
  }
                                         
  //--------------  
  var cmpPlayerControls = new CmpContainer( [BlkPlayer,"blkControls"],
                                            [ {cmp:cmpLblStatusDisplay},
                                              {cmp:cmpBlkMediaInfo},
                                              {cmp:cmpBtnVolume, sel:"false"},
                                              {cmp:cmpBlkControlButtons, sel:"topic_Get('" + tPly + "', 'BookmarkDlgState') == 'NONE'"},
                                              {cmp:cmpBlkBookmarkDlg, sel:"topic_Get('" + tPly + "', 'BookmarkDlgState') != 'NONE'"}],
                                            [ {action:"TOPICEVENT", topic:tPly, eventvar:"BookmarkDlgState", eventval:null}]
                                          );
                                                                                  
  var cmpPlayerUI = new CmpContainer( [BlkPlayer,null],
                                      [ {cmp:cmpPlayerControls},
                                        {cmp:cmpPlayerMessage, sel:"topic_Get('" + tPly + "', 'SourceNID') == 'NONE'"},
                                        {cmp:cmpPlayer, sel:"topic_Get('" + tPly + "', 'SourceNID') != 'NONE'"} ],
                                      [ {action:"TOPICEVENT", topic:tPly, eventvar:"SourceNID", eventval:null} ]
                                     );
  //--------------
  eventPly.subscribe("PlayerEventRequest", null, this,
                      function(EventVar, EventVal) {
                        var PlayerEventRequest = EventVal;
                        switch(PlayerEventRequest) {
                          case "PAUSE":
                            cmpBtnPause.onSelect.call(cmpBtnPause);
                            break;
                          case "NATIVESIZETOGGLE":
                            cmpBtn1to1.toggle.call(cmpBtn1to1);
                            break;
                          default:
                            break;
                        }
                      });

  eventApp.subscribe("PRESENCE", null, this,
                      function(EventVar, EventVal) {
                        var presence = EventVal;
                        // check if this presence is for our room
                        var RoomNID = topic_Get(tPly, "RoomNID");
                        if (presence.RoomNID != RoomNID) {
                          return;
                        }
//When Device relogin, UI will get event (14) which handled in RoomEvent handling code
                        // if this is the presence for the source in the room - start the connection
//                        if (presence.State == true) {
//                          var PresenceJID = presence.JID;
//                          var SourceJID = topic_Get(tPly, "SourceJID");
//                          var PresenceUserName = PresenceJID;
//                          var SourceUserName = SourceJID;
//                          if (PresenceUserName == SourceUserName) {
//                            topic_Publish(tPly, "ConnectionReqState", "STARTCONNECTION");
//                          }
//                        }
                        // if any presence goes unavailable - shut down the room
                        if( presence.State != true ) {
                          //don't shutdown the room if the unavailable is for StreamSource/MediaSource
                          //RoomEvent handling code will handle the events(13) in case of StreamSource/MediaSource unavailable
                          var StreamSourceAgentJID = topic_Get(tPly, "StreamSourceAgentJID");
                          var MediaSourceAgentJID = topic_Get(tPly, "MediaSourceAgentJID");
                          if( presence.JID != StreamSourceAgentJID && presence.JID != MediaSourceAgentJID ) {
                            //TBD - need to do something to notify the user what happened
                            topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                          }
                        }
                      });     
  
  eventApp.subscribe("ROOMEVENT", null, this,
                      function(EventVar, EventVal) {
                        var roomevent = EventVal;
                        // check if this presence is for our room
                        var RoomNID = topic_Get(tPly, "RoomNID");
                        var RecordStreamNID = topic_Get(tPly, "RecordStreamNID");
                        if(roomevent.RoomNID != RoomNID) {
                          return;
                        }
                        if(roomevent.StreamNID == RecordStreamNID){
                          if(roomevent.Action == "DestEvent" && roomevent.StatusCode == 0 && roomevent.ErrCode != null ) {
                            if(roomevent.ErrCode == "104"){
                              var errObj = CommonErrorCodeArr[roomevent.ErrCode];
                              topic_Publish(tPly, "ERRORSTATUS", "RECORDING: " + errObj.errorDesc);
                              var ConnectionRecordState = topic_Get(tPly, "ConnectionRecordState");
                              if(ConnectionRecordState != "NONE") {
                                topic_Publish(tPly, "ConnectionReqState", "PLAYER_STOPRECORDING");
                                return;
                              }
                            }
                            return;
                          }
                          return;
                        }
                        if( roomevent.Action != "SourceEvent" &&
                            roomevent.Action != "DestEvent" &&
                            roomevent.Action != "StreamStatusEvent" && 
                            roomevent.Action != "NotifyResponse" &&
                            roomevent.StatusCode == 200) {
                          topic_Set(tPly, "ActionMediaOffset", roomevent.Offset);
                          topic_Set(tPly, "PlayerCtrlMask", "NONE");
                          switch (roomevent.Action) {
                            case "Start":
                              var ConnectionState = topic_Get(tPly, "ConnectionState");
                              if (ConnectionState == "CONNECTING") {
                                cmpPlayer.doAction(roomevent.Action, roomevent.StreamURL);
                                topic_Publish(tPly, "ConnectionReqState", "PLAYER_SOURCESTARTED");
                              }
                              break;
                            case "Pause":
                            case "SkipBackward":
                            case "SkipForward":
                            case "ResumeFromMediaTC":
                              var ConnectionState = topic_Get(tPly, "ConnectionState");
                              if (ConnectionState == "CONNECTED") {
                                cmpPlayer.doAction(roomevent.Action);
                                topic_Publish(tPly, "ConnectionTimeState", "TIMESHIFTED");
                              }
                              break;
                            case "Resume":
                              var ConnectionState = topic_Get(tPly, "ConnectionState");
                              if (ConnectionState == "CONNECTED") {
                                cmpPlayer.doAction(roomevent.Action);
                              }
                              break;
                            case "Catchup":
                              var ConnectionState = topic_Get(tPly, "ConnectionState");
                              if (ConnectionState == "CONNECTED") {
                                cmpPlayer.doAction(roomevent.Action);
                              }
                              topic_Publish(tPly, "ConnectionTimeState", "LIVE");
                              break;
                            default:
                              break;
                          }
                          topic_Publish(tPly, "PlayerCtrlState", roomevent.Action);
                          
                        }
                        else if(roomevent.Action == "NotifyResponse" &&
                                roomevent.StatusCode == 200){
                          if(roomevent.EventCode != null){
                            if(roomevent.EventCode == "1"){
                              topic_Publish(tPly, "ConnectionStatusMsg", "AUTHORIZING ...");
                              var ConnectionState = topic_Get(tPly, "ConnectionState");
                              if (ConnectionState == "CONNECTING") {
                                topic_Set(tPly, "ConnectionXAuthState", "WAITING");
                              }
                            }else{
                              var errObj = CommonErrorCodeArr[roomevent.EventCode];
                              topic_Publish(tPly, "ERRORSTATUS", errObj.errorDesc);
                            }
                          }
                        }
                        else if(roomevent.Action == "StreamStatusEvent"){
                            if(roomevent.EventCode != null){
                              if(roomevent.EventCode == "11"){
                                var ConnectionState = topic_Get(tPly, "ConnectionState");
                                if (ConnectionState == "CONNECTED") {
                                  topic_Set(tPly, "ConnectionXAuthState", "DENIED");
                                  topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                                }
                              }
                            }
                        }
                        else if( roomevent.StatusCode == 0 ) {
                          // Workaround for middleware bug - Ignore End of Media in Viewer case
                          if(roomevent.ErrCode == "2309"){
                            var TimeMode = topic_Get(tPly, "TimeMode");
                            if(TimeMode != "PLAYBACK") {
                              return;
                            }
                          }

                          // in all other cases - display error msg
                          var errorDesc = null;
                          if( roomevent.ErrCode != null ) {
                            var errObj = CommonErrorCodeArr[roomevent.ErrCode];
                            errorDesc = errObj.errorDesc;
                            var PVRStreamNID = topic_Get(tPly, "PVRStreamNID");
                            if( roomevent.StreamNID == PVRStreamNID ) {
                              errorDesc = "PVR: " + errorDesc;
                            }
                          }

                          //handle error codes 13,14,15
                          if( (roomevent.ErrCode == "113") || (roomevent.ErrCode == "114") || (roomevent.ErrCode == "115") ) {
                            var actionData = roomevent.ActionData.selectSingleNode("//sourceEventActionData").getValue();
                            var eventSourceJID = actionData.substring(0, actionData.indexOf("@"));

                            //if source is unavailable error
                            //then wait for some time before shutdown room
                            if( roomevent.ErrCode == "113" ) {
                              var StreamNID = topic_Get(tPly, "StreamNID");
                              if( roomevent.StreamNID == StreamNID ) {
                                var pendingSourceJID = topic_Get(tPly, "PendingSourceJID");
                                if( pendingSourceJID == "NONE" ) {
                                  topic_Set(tPly, "PendingSourceJID", eventSourceJID);
                                  topic_Publish(tPly, "ERRORSTATUS", errorDesc);
                                  topic_Publish(tPly, "ConnectionReqState", "PLAYER_WAITFORRECONNECT");
                                }
                              }
                            }

                            if( roomevent.ErrCode == "114" ) {
                              var pendingSourceJID = topic_Get(tPly, "PendingSourceJID");
                              if( pendingSourceJID != "NONE" && eventSourceJID == pendingSourceJID ) {
                                topic_Publish(tPly, "ConnectionReqState", "PLAYER_RECONNECTED");
                              }
                            }
                            
                            if( roomevent.ErrCode == "115" ) {
                              var pendingSourceJID = topic_Get(tPly, "PendingSourceJID");
                              if( pendingSourceJID == "NONE" ) {
                                topic_Publish(tPly, "ERRORSTATUS", errorDesc);
                                topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                              }
                            }
                          }
                          else {
                            if( errorDesc != null ) {
                              topic_Publish(tPly, "ERRORSTATUS", errorDesc);
                            }
                            // and shut down room
                            topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                          }
                        }
                      });

  // the cases beginning with PLAYER_ are actual externally published entry point events
  eventPly.subscribe("ConnectionReqState", null, this,
                      function(EventVar, EventVal) {
                        log.info(EventVar + " = " + EventVal + "\n");
                        var ConnectionAction = EventVal;
                        switch (ConnectionAction) {
                          case "PLAYER_ADDCONNECTION":
                            // done with init - get on with it
                            topic_Publish(tPly, "ERRORSTATUS", "NONE");
                            topic_Publish(tPly, "ConnectionStatusMsg", "CONNECTING .");
                            topic_Publish(tPly, "ConnectionState", "CONNECTING");
                            topic_Set(tPly, "ReConnectTimerID", "NONE");
                            topic_Set(tPly, "PendingSourceJID", "NONE");
                            // allocate resource instance based on stream type
                            var SourceNID = topic_Get(tPly, "SourceNID");
                            var SourceNode = topic_GetNIDNode(tCache, SourceNID);
                            var StreamType = SourceNode.selectSingleNode(".//Info//StreamType").getValue();
                            cmpPlayer.addResourceInstance(StreamType);
                            // get allocated IP addr and port and set in topic for create conn to use
                            var objRxMediaIpPort = cmpPlayer.getResourceInstanceIpPort();
                            topic_Set(tPly, "RxMediaIpAddress", objRxMediaIpPort.IpAddress);
                            topic_Set(tPly, "RxMediaPort", objRxMediaIpPort.Port);
                            // get relay and profile nid
                            var ProfileNID = topic_Get(tPly, "ProfileNID");
                            var RelayNID = topic_Get(tPly, "RelayNID");
                            // if either is null - get relay profile
                            if (RelayNID == "" || ProfileNID == "") {
                              topic_Publish(tPly, "ConnectionReqState", "GETRELAYPROFILE");
                            }
                            else {
                              topic_Publish(tPly, "ConnectionReqState", "GETRELAYPROFILE_SUCCESS");
                            }
                            break;
                          case "PLAYER_WAITFORRECONNECT":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            var PendingSourceJID = topic_Get(tPly, "PendingSourceJID");
                            if( ConnectionState == "CONNECTED" && PendingSourceJID != "NONE" ) {
                              var timeOutInterval = topic_Get(tApp, "ReconnectTimeout");
                              var waitTimerID = setTimeout("topic_Publish('" + tPly + "', 'ConnectionReqState', 'PLAYER_ERROR')", timeOutInterval);
                              topic_Set(tPly, "ReConnectTimerID", waitTimerID);
                            }
                            break;
                          case "PLAYER_RECONNECTED":
                             var reConnectTimerID = topic_Get(tPly, "ReConnectTimerID");
                             if( reConnectTimerID != "NONE" ) {
                               clearTimeout(reConnectTimerID);
                             }
                             topic_Set(tPly, "ReConnectTimerID", "NONE");
                             topic_Set(tPly, "PendingSourceJID", "NONE");
                             topic_Publish(tPly, "ConnectionStatusMsg", "CONNECTED");
                            break;
                          case "GETRELAYPROFILE":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              cmpReq_GetRelayProfile(tPly);
                            }
                            break;
                          case "GETRELAYPROFILE_SUCCESS":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              topic_Publish(tPly, "ConnectionReqState", "CREATECONNECTION");
                            }
                            break;
                          case "GETRELAYPROFILE_FAIL":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              topic_Publish(tPly, "ERRORSTATUS", "Failed to Get Profile");
                              topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                            }
                            break;
                          case "CREATECONNECTION":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              topic_Set(tPly, "RoomNID", "PENDING");
                              topic_Publish(tPly, "ConnectionStatusMsg", "CONNECTING ..");
                              cmpReq_CreateConnection(tPly);
                            }
                            break;
                          case "CREATECONNECTION_SUCCESS":
                            var RoomNID = topic_Get(tPly, "RoomNID");
                            var StreamNID = topic_Get(tPly, "StreamNID");
                            cmpPlayer.bindResourceInstance(StreamNID);
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              cmpReq_GetMediaRoom(tPly);
                              // normally source presence will trigger further action
                              // unless it is an unmanaged source 
                              // hack: for now we look to see if it has a streamurl pre-configured in it
                              var SourceNID = topic_Get(tPly, "SourceNID");
                              var SourceNode = topic_GetNIDNode(tCache, SourceNID);
                              var SourceStreamURLNode = SourceNode.selectSingleNode(".//ResourceInstance//MediaStreamURL");
                              // OR unless we dont have sourceJID - in that case go ahead here
                              // hack: this is because we dont have the source jid for files - should be in the room info
                              var SourceJID = topic_Get(tPly, "SourceJID");
                              if( SourceStreamURLNode != null && SourceStreamURLNode.getValue() != "" ) {
                                topic_Publish(tPly, "ConnectionReqState", "STARTCONNECTION");
                              }
                              else if (SourceJID == "NONE") {
                                topic_Publish(tPly, "ConnectionReqState", "STARTCONNECTION");
                              }
                              else {
                                // check if source is already present - in that case continue connection setup
                                var SourceUserName = SourceJID;
                                var nodeRoomPresence = topic_GetNode(tApp, "//RoomPresenceList/RoomPresence[@RoomNID='" + RoomNID + "']");
                                if (nodeRoomPresence != null) {
                                  var nodeRoomSourcePresence = nodeRoomPresence.selectSingleNode("./JID[.='" + SourceUserName + "']");
                                  if (nodeRoomSourcePresence != null) {
                                    topic_Publish(tPly, "ConnectionReqState", "STARTCONNECTION");
                                  }
                                }
                              }
                            }
                            // someone stopped the connection while we were away
                            else {
                              topic_Publish(tPly, "ConnectionReqState", "DELETEMEDIAROOM");
                            }
                            break;
                          case "CREATECONNECTION_FAIL":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              topic_Publish(tPly, "ERRORSTATUS", "Failed to Create Connection");
                              topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                            }
                            break;
                          case "STARTCONNECTION":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              topic_Publish(tPly, "ConnectionStatusMsg", "CONNECTING ...");
                              var StreamNID = topic_Get(tPly, "StreamNID");
                              cmpReq_AddStreamAction(tPly, StreamNID, "Start");
                              if (SimulationMode == true) {
                                topic_Publish(tPly, "ConnectionReqState", "PLAYER_SOURCESTARTED");
                              }
                            }
                            break;
                          case "GETMEDIAROOM_SUCCESS":
                            var RoomXMLNode = topic_GetNode(tPly, "//MediaRoomXML");
                            var StreamNID = topic_Get(tPly, "StreamNID");
                            var StreamNode = RoomXMLNode.selectSingleNode("//Stream[@NID='" + StreamNID + "']");
                            var StreamSourceAgentJID = StreamNode.selectSingleNode("//Stream/data/sourceAgentJID").getValue();
                            var MediaSourceNID = StreamNode.selectSingleNode("//Stream/data/mediaSourceNID").getValue();
                            var MediaSourceNode = RoomXMLNode.selectSingleNode("//MediaSource[@NID='" + MediaSourceNID + "']");
                            var MediaSourceAgentJID = MediaSourceNode.selectSingleNode("//MediaSource/data/sourceAgentJID").getValue();
                            topic_Set(tPly, "StreamSourceAgentJID", StreamSourceAgentJID);
                            topic_Set(tPly, "MediaSourceAgentJID", MediaSourceAgentJID);
                            break;
                          case "GETMEDIAROOM_FAIL":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING" || ConnectionState == "CONNECTED") {
                              topic_Publish(tPly, "ERRORSTATUS", "NONE");
                              topic_Publish(tPly, "ERRORSTATUS", "Failed to Get Media Room Info");
                              topic_Publish(tPly, "ConnectionReqState", "PLAYER_ERROR");
                            }
                            break;
                          case "PLAYER_SOURCESTARTED":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTING") {
                              topic_Publish(tPly, "ERRORSTATUS", "NONE");
                              topic_Publish(tPly, "ConnectionState", "CONNECTED");
                              topic_Publish(tPly, "ConnectionStatusMsg", "CONNECTED");
                            }
                            break;
                          case "PLAYER_PAUSERECORDING":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTED") {
                              topic_Publish(tPly, "ConnectionRecordState", "PAUSED");
                              var RecordStreamNID = topic_Get(tPly, "RecordStreamNID");
                              cmpReq_AddTrackAction(tPly, RecordStreamNID, "Pause");
                            }
                            break;
                          case "PLAYER_RESUMERECORDING":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTED") {
                              topic_Publish(tPly, "ConnectionRecordState", "CONNECTED");
                              var RecordStreamNID = topic_Get(tPly, "RecordStreamNID");
                              cmpReq_AddTrackAction(tPly, RecordStreamNID, "Catchup");
                            }
                            break;
                          case "PLAYER_STOPRECORDING":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "CONNECTED") {
                              var DestFileNID = topic_Get(tPly, "DestFileNID");
                              //decrease the reference count
                              handleRefCountForCacheNID("SrcMediaFile", DestFileNID, "DELETE");
                              //trim the file node
                              trimBookmarkCacheNode(DestFileNID);

                              topic_Publish(tPly, "NIDForBookmark", "");
                              topic_Publish(tPly, "ConnectionRecordState", "NONE");
                              var RecordStreamNID = topic_Get(tPly, "RecordStreamNID");
                              cmpReq_AddStreamAction(tPly, RecordStreamNID, "Delete");
                            }
                            break;
                          case "PLAYER_DELETECONNECTION":
                            topic_Publish(tPly, "ConnectionReqState", "DELETECONNECTION");
                            break;
                          case "PLAYER_ERROR":
                            //clear the timer if exists
                            var reConnectTimerID = topic_Get(tPly, "ReConnectTimerID");
                            if( reConnectTimerID != "NONE" ) {
                              clearTimeout(reConnectTimerID);
                            }
                            topic_Publish(tPly, "ConnectionReqState", "DELETECONNECTION");
                            break;
                          case "DELETECONNECTION":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            //TBD - potential race condition here
                            if (ConnectionState != "NONE" && ConnectionState != "DISCONNECTING") {
                              topic_Publish(tPly, "ConnectionState", "DISCONNECTING");
                              topic_Publish(tPly, "ConnectionPVRState", "DISCONNECTING");

                              var ConnectionRecordState = topic_Get(tPly, "ConnectionRecordState");
                              if( ConnectionRecordState != "NONE" ) {
                                //decrease the reference count for DestFileNID
                                var DestFileNID = topic_Get(tPly, "DestFileNID");
                                handleRefCountForCacheNID("SrcMediaFile", DestFileNID, "DELETE");
                                //trim the file node
                                trimBookmarkCacheNode(DestFileNID);
                              }

                              topic_Publish(tPly, "ConnectionRecordState", "DISCONNECTING");
                              topic_Publish(tPly, "ConnectionStatusMsg", "DISCONNECTING .");
                              // check for room id
                              var RoomNID = topic_Get(tPly, "RoomNID");
                              // if valid - we need to shut down the room
                              if (RoomNID != "" && RoomNID != "PENDING") {
                                topic_Publish(tPly, "ConnectionReqState", "DELETEMEDIAROOM");
                              }
                              // else if not pending then we can just go ahead and claen up
                              else if (RoomNID != "PENDING") {
                                topic_Publish(tPly, "ConnectionReqState", "ENDCONNECTION");
                              }
                              // else if pending - cant do anything yet - just exit here
                              // when it comes back - the state will be disconnecting
                              // that will trigger the shutdown to complete
                            }
                            break;
                          case "DELETEMEDIAROOM":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "DISCONNECTING") {
                              // cleanup DSA stream instance
                              var StreamNID = topic_Get(tPly, "StreamNID");
                              cmpPlayer.doAction("Stop");
                              topic_Publish(tPly, "PlayerCtrlState", "Stop");
                              //cmpPlayer.deleteResourceInstance();
                              topic_Publish(tPly, "ConnectionStatusMsg", "DISCONNECTING ..");
                              cmpReq_DeleteMediaRoom(tPly);
                            }
                            break;
                          case "DELETEMEDIAROOM_SUCCESS":
                          case "DELETEMEDIAROOM_FAIL":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "DISCONNECTING") {
                              var DestFileNID = topic_Get(tPly, "PVRDestFileNID");
                              if (DestFileNID != null && DestFileNID != "") {
                                topic_Publish(tPly, "ConnectionStatusMsg", "DISCONNECTING ...");
                                cmpReq_DeleteDestFile(tPly);
                              }
                              else {
                                topic_Publish(tPly, "ConnectionReqState", "ENDCONNECTION");
                              }
                            }
                            break;
                          case "DELETEDESTFILE_SUCCESS":
                          case "DELETEDESTFILE_FAIL":
                            var ConnectionState = topic_Get(tPly, "ConnectionState");
                            if (ConnectionState == "DISCONNECTING") {
                              var TimeMode = topic_Get(tPly, "TimeMode");
                              if (TimeMode == "PVR") {
                                topic_Publish(tPly, "TimeMode", "NONE");
                              }
                              topic_Publish(tPly, "ConnectionReqState", "ENDCONNECTION");
                            }
                            break;
                          case "ENDCONNECTION":
                            topic_Set(tPly, "ReConnectTimerID", "NONE");
                            topic_Set(tPly, "PendingSourceJID", "NONE");
                            // release resource instance
                            cmpPlayer.deleteResourceInstance();
                            // reset conn vars
                            topic_Set(tPly, "RecordStreamNID", "");
                            topic_Set(tPly, "PVRStreamNID", "");
                            topic_Set(tPly, "StreamNID", "");
                            topic_Set(tPly, "RoomNID", "");
                            topic_Set(tPly, "StreamSourceAgentJID", "");
                            topic_Set(tPly, "MediaSourceAgentJID", "");
                            var roomXMLNode = topic_GetNode(tPly, "//MediaRoomXML");
                            if (roomXMLNode != null) {
                              roomXMLNode.removeChildren();
                            }
                            topic_Set(tPly, "RelayNID", "");
                            topic_Set(tPly, "ProfileNID", "");
                            var profileXMLNode = topic_GetNode(tPly, "//ProfileXML");
                            if (profileXMLNode != null) {
                              profileXMLNode.removeChildren();
                            }
                            var TimeMode = topic_Get(tPly, "TimeMode");
                            if (TimeMode == "PLAYBACK") {
                              var SourceNID = topic_Get(tPly, "SourceNID");
                              if( SourceNID != "NONE" ) {
                                //decrease the reference count for mediaFileNID
                                handleRefCountForCacheNID("MediaStreamFileResource", SourceNID, "DELETE");
                              }
                              topic_Set(tPly, "AddBookmarkEnabled", 1);
                            } else {
                              topic_Set(tPly, "AddBookmarkEnabled", 0);
                              topic_Set(tPly, "TimeMode", "NONE");
                            }
                            topic_Set(tPly, "NIDForBookmark", "");
                            topic_Set(tPly, "NIDForGapInfo", null);
                            topic_Set(tPly, "EnableRemote", 0);
                            topic_Set(tPly, "ConnectionTimeState", "LIVE");

                            topic_Set(tPly, "BookmarkDlgState", "NONE");
                            topic_Set(tPly, "BookmarkState", "NONE");
                            topic_Set(tPly, "BookmarkSelected", "NONE");
                            topic_Set(tPly, "BookmarkTitle", "");
                            topic_Set(tPly, "BookmarkAtTC", "");

                            topic_Set(tPly, "DestDirState", "NONE");
                            topic_Set(tPly, "DestPVRDirState", "NONE");
                            topic_Set(tPly, "ConnectionRecordState", "NONE");
                            topic_Set(tPly, "ConnectionPVRState", "NONE");
                            topic_Set(tPly, "ConnectionRemoteState", "NONE");

                            topic_Set(tPly, "ActionData", 0);
                            topic_Set(tPly, "ActionMediaOffset", 0);
                            topic_Set(tPly, "PlayerCtrlMask", "NONE");
                            topic_Publish(tPly, "PlayerCtrlState", "NONE");

                            // this must be the last event
                            topic_Publish(tPly, "ConnectionState", "NONE");
                            topic_Publish(tPly, "ConnectionStatusMsg", "DISCONNECTED");

                            // hack to show denied at end of connection
                            var ConnectionXAuthState = topic_Get(tPly, "ConnectionXAuthState");
                            if (ConnectionXAuthState != "NONE") {
                              topic_Set(tPly, "ConnectionXAuthState", "NONE");
                              if(ConnectionXAuthState != "WAITING"){
                                topic_Publish(tPly, "ConnectionStatusMsg", "DENIED");
                              }
                            }
                            break;
                          default:
                            break;
                        }
                      }); 
  
  //---------------
  // init App Vars
  topic_Set(tPly, "RecordStreamNID", "");
  topic_Set(tPly, "PVRStreamNID", "");                             
  topic_Set(tPly, "StreamNID", "");
  topic_Set(tPly, "RoomNID", "");
  topic_Set(tPly, "MediaRoomXML", null);
  topic_Set(tPly, "RelayNID", "");
  topic_Set(tPly, "ProfileNID", "");
  topic_Set(tPly, "ProfileXML", null);
  
  topic_Set(tPly, "NIDForBookmark", "");
  topic_Set(tPly, "NIDForGapInfo", null);
  topic_Set(tPly, "ViewBookmarkEnabled", 0);
  topic_Set(tPly, "AddBookmarkEnabled", 0);
  topic_Set(tPly, "EditBookmarkEnabled", 0);
  topic_Set(tPly, "EnableRemote", 0);
  topic_Set(tPly, "RecordingEnabled", 0);
  topic_Set(tPly, "ConnectionTimeState", "LIVE");
  topic_Set(tPly, "FileWriteMode", 1);
  
  topic_Set(tPly, "BookmarkDlgState", "NONE");
  topic_Set(tPly, "BookmarkState", "NONE"); 
  topic_Set(tPly, "BookmarkSelected", "NONE");
  topic_Set(tPly, "BookmarkTitle", "");
  topic_Set(tPly, "BookmarkAtTC", "");
  
  topic_Set(tPly, "DestDirState", "NONE");
  topic_Set(tPly, "DestPVRDirState", "NONE");
  topic_Set(tPly, "ConnectionRecordState", "NONE");
  topic_Set(tPly, "ConnectionPVRState", "NONE");
  topic_Set(tPly, "ConnectionRemoteState", "NONE");
  topic_Set(tPly, "ConnectionXAuthState", "NONE");
  
  topic_Set(tPly, "ActionData", 0);
  topic_Set(tPly, "ActionMediaOffset", 0);
  topic_Set(tPly, "PlayerCtrlMask", "NONE");
  topic_Publish(tPly, "PlayerCtrlState", "NONE");
  
  // this must be the last event
  topic_Publish(tPly, "ConnectionState", "NONE");
  topic_Set(tPly, "ConnectionStatusMsg", "DISCONNECTED");
  topic_Set(tPly, "ERRORSTATUS", "NONE");
  topic_Set(tPly, "ConnectionReqState", "NONE");
  
  topic_Set(tPly, "SourceNID", "NONE");
  topic_Set(tPly, "SourceJID", "NONE");
  
  topic_Set(tPly, "TimeMode", "");
  topic_Set(tPly, "TimeScale", "");
  topic_Set(tPly, "PlayLength", "");
  topic_Set(tPly, "StreamStartOffset", "");                         
  
  topic_Set(tPly, "SkipbackDisabled", 0);
  topic_Set(tPly, "SkipfwdDisabled", 0);
  topic_Set(tPly, "SnapShotDisabled", 0);
  
  topic_Set(tPly, "FullscreenMode", 0);
  topic_Set(tPly, "NativeSizeMode", 0);
  topic_Set(tPly, "MuteState", 0); 
                              
  return cmpPlayerUI;
}
