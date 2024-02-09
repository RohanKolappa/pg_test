function cmpControllerUI_Create() {
  
  var winController = this;
  var BlkController = this.BlkJSX;
  var tCtrl = this.tWin;
  var eventCtrl = this.eventWin;


  var blkControllerRoot = getJsxByName([BlkController, "blkControllerRoot"]);
  blkControllerRoot.load("components/controller.xml");
  
  //--------------
  var cmpBtnRewind = new CmpImageButton([BlkController,"btnRewind"], null,  
                                        {action:"FNCCALL", context:this,
                                         fnc:function() {
                                         	   var actionData = topic_Get(tApp, "IPVSPlayerDefaultSkipBack");
                                               //HACK: check if the default "actionData" would put us into negative media time
                                               // - this is only approximate because our idea of current media offset may not be accurate
                                               cmpSlider.updateMediaOffset();
                                               if( cmpSlider.CurrMediaOffset < actionData ) { 
                                                 actionData = cmpSlider.CurrMediaOffset;
                                               }
                                               topic_Set(tCtrl, "ActionData", actionData);
                                               topic_Publish(tCtrl, "ControllerEvent", "SKIPBACK");
                                             }
                                        });
  
  var cmpBlkPause = new Component([BlkController,"btnPause"]);
  var cmpBtnPause = new CmpImageButton([BlkController,"btnPause"], null,  
                                        {action:"FNCCALL", context:this,
                                          fnc: function() {
                                               topic_Publish(tCtrl, "ControllerEvent", "PAUSE");
                                               topic_Publish(tCtrl, "ControllerMask", "ON");
                                             }
                                        });
  
  var cmpBlkPlay = new Component([BlkController,"btnPlay"]);
  var cmpBtnPlay = new CmpImageButton([BlkController,"btnPlay"], null,   
                                      {action:"FNCCALL", context:this,
                                       fnc:function() {
                                             var CollaborationState = topic_Get(tCtrl, "CollaborationState");
                                             if(CollaborationState != "CONNECTED") {
                                               topic_Publish(tCtrl, "ControllerEvent", "START");
                                             }
                                             else {
                                               topic_Publish(tCtrl, "ControllerMask", "ON");
                                               topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                             }
                                           }
                                      });
  
  var cmpBtnStop = new CmpImageButton([BlkController,"btnStop"], null,   
                                      {action:"FNCCALL", context:this,
                                       fnc:function() {
                                             var CollaborationState = topic_Get(tCtrl, "CollaborationState");
                                             if (CollaborationState != "NONE") {
                                               topic_Publish(tCtrl, "ControllerEvent", "STOP");
                                             }   
                                           }
                                      });
  
  var cmpBtnFastforward = new CmpImageButton([BlkController,"btnFastforward"], null,   
                                              {action:"FNCCALL", context:this,
                                               fnc:function() {
                                                     var actionData = topic_Get(tApp, "IPVSPlayerDefaultSkipFWD");
                                                     topic_Set(tCtrl, "ActionData", actionData);
                                                     topic_Publish(tCtrl, "ControllerEvent", "SKIPFWD");
                                                   }
                                              });
  
  var cmpBtnCatchup = new CmpImageButton([BlkController,"btnCatchup"], null,   
                                              {action:"FNCCALL", context:this,
                                               fnc:function() {
                                                     topic_Publish(tCtrl, "ControllerEvent", "CATCHUP");
                                                   }
                                              });
                                                                                          
                                              
  var cmpBlkRecord = new Component([BlkController,"btnRecord"]); 
  var cmpBtnRecord = new CmpImageButton([BlkController,"btnRecord"], null, 
                                          {action:"FNCCALL", context:this,
                                           fnc:function() {
                                                 var CollaborationRecordState = topic_Get(tCtrl, "CollaborationRecordState");
                                                 if(CollaborationRecordState == "NONE") {
                                                   // start the recording
                                                   //var ConnectionState = topic_Get(tCtrl, "ConnectionState");
                                                   var CollaborationState = topic_Get(tCtrl, "CollaborationState");
                                                   if( CollaborationState == "CONNECTED" ) {
                                                     topic_Publish(tCtrl, "ControllerEvent", "STARTRECORDING");
                                                   }
                                                 }
                                                 else if(CollaborationRecordState == "PauseRecord") {
                                                   topic_Publish(tCtrl, "ControllerEvent", "RESUMERECORDING");
                                                 }
                                               }
                                           });
                                          
  var cmpBlkRecordPause = new Component([BlkController,"btnRecordPause"]);
  var cmpBtnRecordPause = new CmpImageButton([BlkController,"btnRecordPause"], null, 
                                          {action:"FNCCALL", context:this,
                                           fnc:function() {
                                                 var CollaborationRecordState = topic_Get(tCtrl, "CollaborationRecordState");
                                                 if(CollaborationRecordState == "CONNECTED") {
                                                   topic_Publish(tCtrl, "ControllerEvent", "PAUSERECORDING");
                                                 }
                                               }
                                          }); 
                                                                                                                           
  var cmpBlkRecordStop = new Component([BlkController,"btnRecordStop"]);
  var cmpBtnRecordStop = new CmpImageButton([BlkController,"btnRecordStop"], null, 
                                          {action:"FNCCALL", context:this,
                                           fnc:function() {
                                                 var CollaborationRecordState = topic_Get(tCtrl, "CollaborationRecordState");
                                                 if(CollaborationRecordState == "NONE") {
                                                   // not recording - do nothing
                                                 }
                                                 else {
                                                   // stop the recording
                                                   topic_Publish(tCtrl, "ControllerEvent", "STOPRECORDING");
                                                 }
                                               }
                                             });
  var cmpBtnMute = new CmpImageButton([BlkController, "btnMute"], null,
                                          { action: "FNCCALL", context: this,
                                            fnc: function() {
                                              var MuteStateAll = topic_Get(tCtrl, "MuteStateAll");
                                              if (MuteStateAll == 1 || MuteStateAll == "NONE") {
                                                topic_Set(tCtrl, "MuteStateAll", 0); //muted
                                                cmpBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF);
                                              }
                                              else {
                                                topic_Set(tCtrl, "MuteStateAll", 1); //unmuted
                                                cmpBtnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON);
                                              }
                                              topic_Publish(tCtrl, "UpdateMute", (new Date));
                                            }
                                          });
  
  //---------------------- story 41748 slow motion and fast speed-----------------
  var blkVarSpeedButtons = new Component([BlkController, "blkVarSpeedButtons"]);
  var lblVarSpeed = new CmpLabel([BlkController, "lblVarSpeed"]);

  //show/hide btnVarSpeed controlled by topic_Publish(tCtrl, "CollaborationFFTrackState", "ENABLE"/"NONE");
  var cmpBtnVarSpeed = new CmpButton([BlkController, "btnVarSpeed"], null,
                                          { action: "FNCCALL", context: this,
                                            fnc: function () {
                                              if (topic_Get(tCtrl, "SpeedSettingBtnState") == "NONE") {
                                                topic_Publish(tCtrl, "SpeedSettingBtnState", "VIEW");
                                                jsx3.gui.Event.subscribe(jsx3.gui.Event.MOUSEMOVE, this, this.cancelFFMenu);
                                              }
                                              else {
                                                jsx3.gui.Event.unsubscribe(jsx3.gui.Event.MOUSEMOVE, this);
                                                topic_Publish(tCtrl, "SpeedSettingBtnState", "NONE");
                                              }
                                            }
                                          });

  // slow motion/fast forward buttons: .1x,.5x,1x,2x,5x,10x
//  //.1x
//  var cmpBtnSpeed1 = new CmpButton([BlkController, "btnVarSpeed1"],null,
//                                  { action: "FNCCALL",
//                                    context: this,
//                                    fnc: function () {
//                                      topic_Publish(tCtrl, "PlayBySpeed", "0.1x");
//                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
//                                    }
//                                  });
//  //.5x
//  var cmpBtnSpeed2 = new CmpButton([BlkController, "btnVarSpeed2"], null,
//                                  { action: "FNCCALL",
//                                    context: this,
//                                    fnc: function () {
//                                      topic_Publish(tCtrl, "PlayBySpeed", "0.5x");
//                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
//                                    }
//                                  });  
  //1x
  var cmpBtnSpeed1 = new CmpButton([BlkController, "btnVarSpeed1"], null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function () {
                                      topic_Publish(tCtrl, "PlayBySpeed", "1.0x");
                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                    }
                                  });  
  //2x
  var cmpBtnSpeed2 = new CmpButton([BlkController, "btnVarSpeed2"], null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function () {
                                      topic_Publish(tCtrl, "PlayBySpeed", "2.0x");
                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                    }
                                  });  
  //5x
  var cmpBtnSpeed3 = new CmpButton([BlkController, "btnVarSpeed3"], null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function () {
                                      topic_Publish(tCtrl, "PlayBySpeed", "5.0x");
                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                    }
                                  });  
  //10x
  var cmpBtnSpeed4 = new CmpButton([BlkController, "btnVarSpeed4"], null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function () {
                                      topic_Publish(tCtrl, "PlayBySpeed", "10.0x");
                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                    }
                                  });

  //60x
  var cmpBtnSpeed5 = new CmpButton([BlkController, "btnVarSpeed5"], null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function () {
                                      topic_Publish(tCtrl, "PlayBySpeed", "60.0x");
                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                    }
                                  });

  //250x
  var cmpBtnSpeed6 = new CmpButton([BlkController, "btnVarSpeed6"], null,
                                  { action: "FNCCALL",
                                    context: this,
                                    fnc: function () {
                                      topic_Publish(tCtrl, "PlayBySpeed", "250.0x");
                                      topic_Publish(tCtrl, "ControllerEvent", "RESUME");
                                    }
                                  });

  eventCtrl.subscribe("PlayBySpeed", null, this,
                      function (EventVar, EventVal) {
                        var ffSpeed = topic_Get(tCtrl, "PlayBySpeed");
                        ffSpeed = ffSpeed.replace(/.0x$/, 'x');
                        lblVarSpeed.BlkJSX.setText(ffSpeed, true);
                        topic_Publish(tCtrl, "SpeedSettingBtnState", "NONE");
                      });

  //--------------
  eventApp.subscribe("PlayerSettings", null, this,
                      function(EventVar, EventVal) {
                        var SkipFwdTip = topic_Get(tApp, "IPVSPlayerDefaultSkipFWD");
                        SkipFwdTip = SkipFwdTip/1000;
                        cmpBtnFastforward.BlkJSX.setTip("Skipforward-" + SkipFwdTip + " Secs");

                        var SkipBackTip = topic_Get(tApp, "IPVSPlayerDefaultSkipBack");
                        SkipBackTip = SkipBackTip/1000;
                        cmpBtnRewind.BlkJSX.setTip("SkipBack-" + SkipBackTip + " Secs");
                      });
  //--------------
  // Connection Status
  var cmpLblStatusDisplay = new CmpLabelErrorTip([BlkController,"lblStatusDisplay"], null, 
                                         [{action:"TOPICEVENT", topic:tCtrl, eventvar:"CollaborationStatusMsg", eventval:null},
                                          {action:"TOPICEVENT", topic:tCtrl, eventvar:"ERRORSTATUS", eventval:null}], 
                                         null, 
                                         "DISCONNECTED");
                                         
  var cmpBtnSessionStatus = new CmpImageButton([BlkController, "btnSessionStatus"], null,
                                              { action: "FNCCALL", context: this,
                                                fnc: function() {
                                                  var sessionID = topic_Get(tCtrl, "SessionID");
                                                  if( (!jsx3.util.strEmpty(sessionID)) && (sessionID != "NONE") ) {
                                                    cmpWinSessionMonitor_Create(tCtrl, eventCtrl,BlkController);
                                                  }
                                                }
                                              });                                          
  //--------------
  // Slider                                       
  var cmpLblTimeCode = new CmpLabel([BlkController,"lblTimeCode"]);
  var cmpLblTimeLength = new CmpLabel([BlkController,"lblTimeLength"]);
  var cmpSliderProgress = new Component([BlkController,"sliderTrackProgress"]);  
  var cmpSlider = new CmpSlider([BlkController,"sliderPointer"], tCtrl, 
                                [{action:"TOPICEVENT", topic:tCtrl, eventvar:"ControllerState", eventval:null}],
                                 {action:"FNCCALL", context:this,
                                  fnc:function(Offset) {
                                        if( Offset > 0 ) {
                                          topic_Set(tCtrl, "ActionData", Offset);
                                          topic_Publish(tCtrl, "ControllerEvent", "SKIPFWD");
                                        }
                                        else {
                                          topic_Set(tCtrl, "ActionData", (Offset * -1));
                                          topic_Publish(tCtrl, "ControllerEvent", "SKIPBACK");
                                        }
                                      }
                                });
  cmpSlider.blkProgress = cmpSliderProgress;                              
  cmpSlider.lblTimeCode = cmpLblTimeCode;
  cmpSlider.lblTimeLength = cmpLblTimeLength;
                                                            
  //--------------  
  var cmpBlkMediaInfo = new CmpContainer( [BlkController,"blkMediaInfoContainer"],
                                            [ {cmp:cmpLblTimeCode},
                                              {cmp:cmpLblTimeLength, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                         "topic_Get('" + tCtrl + "', 'TimeMode') == 'PLAYBACK'"},
                                              {cmp:cmpSliderProgress},                            
                                              {cmp:cmpSlider, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED'"}
                                            ],
                                            [ {action:"TOPICEVENT", topic:tCtrl, eventvar:"CollaborationState", eventval:null},
                                              {action:"TOPICEVENT", topic:tCtrl, eventvar:"CollaborationPVRState", eventval:null}
                                            ]
                                          );
  //--------------                                                                                  
  var cmpBlkControlButtons = new CmpContainer( [BlkController,"blkButtonContainer"],
                                            [ 
                                              {cmp:cmpBtnRewind, sel:"topic_Get('" + tCtrl + "', 'CollaborationPVRState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'SkipbackDisabled') != 1"},
                                              { cmp: cmpBlkPause, sel: "topic_Get('" + tCtrl + "', 'CollaborationPVRState') == 'CONNECTED' && " +
                                                                       "topic_Get('" + tCtrl + "', 'CollaborationState') != 'NONE' && " + 
                                                                       "topic_Get('" + tCtrl + "', 'ControllerState') != 'Pause'"},
                                              { cmp: cmpBtnPause, sel: "topic_Get('" + tCtrl + "', 'CollaborationPVRState') == 'CONNECTED' && " +
                                                                    "topic_Get('" + tCtrl + "', 'CollaborationState') != 'NONE' && " + 
                                                                    "topic_Get('" + tCtrl + "', 'ControllerState') != 'Stop' && " +  
                                                                    "topic_Get('" + tCtrl + "', 'ControllerState') != 'Pause' && " +
                                                                    "topic_Get('" + tCtrl + "', 'ControllerMask') != 'ON'"},
                                              {cmp:cmpBlkPlay, sel:"( topic_Get('" + tCtrl + "', 'CollaborationPVRState') != 'CONNECTED') || " + 
                                                                   "( topic_Get('" + tCtrl + "', 'CollaborationPVRState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerState') == 'Pause')"},
                                              {cmp:cmpBtnPlay, sel:"( topic_Get('" + tCtrl + "', 'CollaborationState') == 'NONE' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerState') != 'NONE' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerState') != 'Start' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerMask') != 'ON') || " + 
                                                                   "( topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerState') == 'Stop') || " + 
                                                                   "( topic_Get('" + tCtrl + "', 'CollaborationPVRState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerState') == 'Pause' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'ControllerMask') != 'ON')"},
                                              {cmp:cmpBtnStop, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') != 'NONE' && " +
                                                                   "topic_Get('" + tCtrl + "', 'CollaborationState') != null && " +
                                                                   "topic_Get('" + tCtrl + "', 'ControllerState') != 'Stop' && " +
                                                                   "topic_Get('" + tCtrl + "', 'SessionOwnerUser') == 'TRUE'"},
                                              {cmp:cmpBtnFastforward, sel:"topic_Get('" + tCtrl + "', 'CollaborationPVRState') == 'CONNECTED' && " + 
                                                                          "topic_Get('" + tCtrl + "', 'SkipfwdDisabled') != 1 && " + 
                                                                          "topic_Get('" + tCtrl + "', 'CollaborationTimeState') != 'LIVE'"},
                                              {cmp:cmpBtnCatchup, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                      "topic_Get('" + tCtrl + "', 'CollaborationTimeState') != 'LIVE' && " +
                                                                      "topic_Get('" + tCtrl + "', 'TimeMode') != 'PLAYBACK'"},
                                              {cmp:cmpBlkRecord, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'RecordingEnabled') == 1"},
                                              {cmp:cmpBtnRecord, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                     "topic_Get('" + tCtrl + "', 'CollaborationRecordState') != 'CONNECTED'"},
                                              {cmp:cmpBlkRecordPause, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                          "topic_Get('" + tCtrl + "', 'RecordingEnabled') == 1 && " + 
                                                                          "topic_Get('" + tCtrl + "', 'CollaborationRecordState') == 'CONNECTED'"},
                                              {cmp:cmpBtnRecordPause},
                                              /* //task 18897 taking away recordStop not implemented in Middleware */
                                              // thats y setting RecordingEnabled') == 9999 otherwise it should be 1
                                              {cmp:cmpBlkRecordStop, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                         "topic_Get('" + tCtrl + "', 'RecordingEnabled') == 9999"},                                                               
                                              {cmp:cmpBtnRecordStop, sel:"topic_Get('" + tCtrl + "', 'CollaborationState') == 'CONNECTED' && " + 
                                                                         "topic_Get('" + tCtrl + "', 'CollaborationRecordState') != 'NONE'&& " +
                                                                         "topic_Get('" + tCtrl + "', 'RecordingEnabled') == 9999"
                                              },
                                              { cmp: cmpBtnMute },
                                              //add Fastforward tracks action (fastforward/slowmotion)
                                              {cmp: blkVarSpeedButtons, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') != 'NONE' && " +
                                                                             "topic_Get('" + tCtrl + "', 'SpeedSettingBtnState') != 'NONE'" },
                                              { cmp: lblVarSpeed, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') != 'NONE' " },
                                              { cmp: cmpBtnVarSpeed, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') != 'NONE' " },
                                              { cmp: cmpBtnSpeed1, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') == 'ALL' " },
                                              { cmp: cmpBtnSpeed2, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') == 'ALL' " },
                                              { cmp: cmpBtnSpeed3, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') == 'ALL' " },
                                              { cmp: cmpBtnSpeed4, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') == 'ALL' " },
                                              { cmp: cmpBtnSpeed5, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') == 'ALL' " },
                                              { cmp: cmpBtnSpeed6, sel: "topic_Get('" + tCtrl + "', 'CollaborationFFTrackState') == 'ALL' " }
                                              ],
                                            [ { action: "TOPICEVENT", topic:tCtrl, eventvar:"ControllerState", eventval:null},
                                              { action: "TOPICEVENT", topic:tCtrl, eventvar:"ControllerMask", eventval:null},
                                              { action: "TOPICEVENT", topic:tCtrl, eventvar:"CollaborationState", eventval:null},
                                              { action: "TOPICEVENT", topic:tCtrl, eventvar:"CollaborationTimeState", eventval:null},
                                              { action: "TOPICEVENT", topic:tCtrl, eventvar:"CollaborationPVRState", eventval:null},
                                              { action: "TOPICEVENT", topic:tCtrl, eventvar:"CollaborationRecordState", eventval:null},
                                              { action: "TOPICEVENT", topic: tCtrl, eventvar: "RecordingEnabled", eventval: null },
                                              { action: "TOPICEVENT", topic: tCtrl, eventvar: "SessionOwnerUser", eventval: false },
                                              { action: "TOPICEVENT", topic: tCtrl, eventvar: "SpeedSettingBtnState", eventval: null }
                                            ]
                                          );
  cmpBlkControlButtons.tCtrl = tCtrl;
                                         
  //--------------  
  var cmpControls = new CmpContainer( [BlkController,"blkControllerControls"],
                                            [ { cmp:cmpLblStatusDisplay },
                                              { cmp:cmpBtnSessionStatus },
                                              { cmp: cmpBlkMediaInfo },
                                              { cmp:cmpBlkControlButtons }
                                            ],
                                            null
                                          );

  // --- add fps slider ----
  var cmpTxtFPS = new CmpLabel([BlkController, "lblTxtFPS"], true);
  var sliderMaxValue = 60;
  var cmpFPSSlider = new CmpSimpleSlider([BlkController, "FPSSlider"], sliderMaxValue, cmpTxtFPS.BlkJSX, null,
                                         { action: "FNCCALL",
                                             context: null,
                                             fnc: function(eventVal) {
                                                 //#43176 minimum fps is '1'
                                                 eventVal = eventVal == 0 ? 1 : eventVal;
                                                 topic_Publish(tCtrl, "UpdateFps", eventVal);
                                             }
                                         }
                                        );
  var cmpBlkPlayerFPS = new CmpContainer([BlkController, "blkPlayerFPS"],
                                        [{ cmp: cmpTxtFPS },
                                         { cmp: cmpFPSSlider}],
                                         null, null);

  var cmpControllerUI = new CmpContainer([BlkController, "blkControllerRoot"],
                                      [{ cmp: cmpControls },
                                       { cmp: cmpBlkPlayerFPS, sel: "topic_Get('" + tCtrl + "', 'MediaPlayerMode') == 'PLAYER'" }
                                      ],
                                      [{ action: "TOPICEVENT", topic: tCtrl, eventvar: "CollaborationNID", eventval: null },
                                       { action: "TOPICEVENT", topic: tCtrl, eventvar: "MediaPlayerMode", eventval: null }]
                                     );
  //--------------

  eventApp.subscribe("MediaSFRDataNodeState", null, this,
                      function(EventVar, EventVal) {
                        // check if this event is for the current source
                        var NIDForBookmark = topic_Get(tCtrl, "NIDForBookmark");
                        if (NIDForBookmark == EventVal) {
                          var CollaborationState = topic_Get(tCtrl, "CollaborationState");
                          // check for connection state
                          // if connected publish NID for bookmark
                          if( CollaborationState == "CONNECTED" ) {
                            topic_Publish( tCtrl, "BookmarkState", (newServerDate()) );
                          }
                        }
                        return;
                      });
  //--------------

  eventCtrl.subscribe("ControllerEventRequest", null, this,
                      function(EventVar, EventVal) {
                        var ControllerEventRequest = EventVal;
                        switch(ControllerEventRequest) {
                          case "PLAY":
                            // simulate click on play
                            cmpBtnPlay.onSelect.call(cmpBtnPlay);
                            break;
                          case "PAUSE":
                            cmpBtnPause.onSelect.call(cmpBtnPause);
                            break;
                          case "STOP":
                            cmpBtnStop.onSelect.call(cmpBtnStop);
                            break;
                          case "MUTE":
                            cmpBtnMute.onSelect.call(cmpBtnMute);
                            break;
                          default:
                            break;
                        }
                      });
  
  eventCtrl.subscribe("CollaborationState", "NONE", this,
                      function(EventVar, EventVal) {
                        topic_Set(tCtrl, "SkipbackDisabled", 0);
                        topic_Set(tCtrl, "SkipfwdDisabled", 0);
                        
                        topic_Set(tCtrl, "FileWriteMode", 1);
                        
                        topic_Set(tCtrl, "ViewBookmarkEnabled", 0);
                        topic_Set(tCtrl, "AddBookmarkEnabled", 0);
                        topic_Set(tCtrl, "EditBookmarkEnabled", 0);
                        
                        topic_Set(tCtrl, "BookmarkDlgState", "NONE");
                        topic_Set(tCtrl, "BookmarkSelected", "NONE");
                        topic_Set(tCtrl, "BookmarkTitle", "");
                        topic_Set(tCtrl, "BookmarkAtTC", "");
                        
                        //topic_Set(tCtrl, "ConnectionXAuthState", "NONE");
                      });

  //push this component to parent window's SubCmpArr
//  winController.SubCmpArr.push({ cmp: cmpControllerUI });
  eventCtrl.subscribe("MediaPlayerInit", "DONE", this,
                      function(EventVar, EventVal) {
                          winController.SubCmpArr.push({ cmp: cmpControllerUI });
                        });

 //conditional set 'STOP' button tool tips #41018
 eventCtrl.subscribe("SessionOwnerUser", "FALSE", this,
                      function(EventVar, EventVal) {
                          cmpBtnStop.BlkJSX.setTip('Participants do not have permission to stop sessions');
                        });

  this.cancelFFMenu = function(objEvent) {
    var menuRenderedPos = blkVarSpeedButtons.BlkJSX.getRendered().getBoundingClientRect();
    var btnRenderedPos = cmpBtnVarSpeed.BlkJSX.getRendered().getBoundingClientRect();
    var mouseClientX = objEvent.event.getTrueX();
    var mouseClientY = objEvent.event.getTrueY();
    if (mouseClientX < menuRenderedPos.left || mouseClientX > menuRenderedPos.right || mouseClientY < menuRenderedPos.top || mouseClientY > menuRenderedPos.bottom) {
      if (mouseClientY < menuRenderedPos.top || mouseClientY > btnRenderedPos.bottom || mouseClientX < btnRenderedPos.left || mouseClientX > btnRenderedPos.right) {
        cmpBtnVarSpeed.onSelect();
      }
    }
  }

  winController.cmpFPSSlider = cmpFPSSlider; 
  //separate cmpControllerUI from cmpBlkMediaPlayer 
  jsx3.sleep((function() {
                            this.init();
                            this.show();
                            jsx3.require("com.ipvs.gui.WindowingPlayerHeader");
                            var instance = com.ipvs.gui.WindowingPlayerHeader.loadView(BlkController, eventCtrl, tCtrl, eventApp);
                            com.ipvs.gui.WindowingPlayerHeader.setInstanceById(tCtrl, instance);
                            instance.initView(instance);
                            log.info("cmpControllerUI:controll panel is loaded.");
                          }
               ), null, cmpControllerUI);
}
