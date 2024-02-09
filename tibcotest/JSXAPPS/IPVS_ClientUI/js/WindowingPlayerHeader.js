/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Class provide windowing player header control;
 * Separate this from cmpControllerUI so as to easily drop off 
 * this logic when moving to UMC Canvas base structure
 */
(!jsx3.gui.ImageButton) && jsx3.require("jsx3.gui.ImageButton");

jsx3.Class.defineClass("com.ipvs.gui.WindowingPlayerHeader", null, null, function(WindowingPlayerHeader, WindowingPlayerHeader_prototype) {

  /** -------------------------------- STATIC property/function ------------------------------ */

  //store windowPlayerHeader instances from different player windows
  var WindowingPlayerHeaderInstanceArr = new Object();

  //set windowPlayerHeader to the WindowingPlayerHeaderInstanceArr
  WindowingPlayerHeader.setInstanceById = function(instanceId, WPHInstance) {
    WindowingPlayerHeaderInstanceArr[instanceId] = WPHInstance;
  }

  //set windowPlayerHeader from the WindowingPlayerHeaderInstanceArr
  WindowingPlayerHeader.getInstanceById = function(instanceId) {
    return WindowingPlayerHeaderInstanceArr[instanceId];
  }

  /**
  * loadView: load and initial blkWindowControls  
  * @param winBlkJSX {String}
  * @param eventWinHeader {Object}
  * @param topicWinHeader {String}
  * @param eventApp {Object}
  */
  WindowingPlayerHeader.loadView = function(winBlkJSX, eventWinHeader, topicWinHeader, eventApp) {
    /** ensure controller.xml is loaded while creating controllerUI **/
      var instanceId = topicWinHeader;
      // ------------------------------------ player windowing buttons ------------------------------------
      var btnFullscreenVar = new CmpImageButton([winBlkJSX, "window_fullscreen"],
                                                  null,
                                                 { action: "FNCCALL",
                                                   context: this,
                                                   fnc: function(EventVar, EventVal) {
                                                    //check player availability for SinglePlayerMode
                                                   checkPlyAvailableForSingleMode(WindowingPlayerHeaderInstanceArr[instanceId]);

                                                    //1. Dropping fps
                                                    topic_Publish(topicWinHeader, "DropFPS", "DROP");
                                                     
                                                    //2.call player toogleDisplay
                                                    WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.IPVideoPlayer.toggleDisplay();
                                                   }
                                                 });
      var btn1To1Var = new CmpImageButton([winBlkJSX, "window_1_to_1"],
                                            null,
                                           { action: "FNCCALL",
                                             context: this,
                                             fnc: function() {
                                               //check player availability for SinglePlayerMode
                                               checkPlyAvailableForSingleMode(WindowingPlayerHeaderInstanceArr[instanceId]);
                                               if (WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.NativeMode) {
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.doAction("NativesizeOff");
                                                 //preserver current button stat in an instance varible
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtn1To1 = jsx3.gui.ImageButton.STATE_OFF;
                                               }
                                               else {
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.doAction("NativesizeOn");
                                                 //preserver current button stat in an instance varible
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtn1To1 = jsx3.gui.ImageButton.STATE_ON;
                                               }
                                               
                                             }
                                           });
        var btnExpandVar = new CmpImageButton([winBlkJSX, "window_expand"],
                                              null,
                                             { action: "FNCCALL",
                                               context: this,
                                               fnc: function() {
                                                 //1. Dropping fps
                                                 topic_Publish(topicWinHeader, "DropFPS", "DROP");

                                                 //2. Set expand status
                                                 if (topic_Get(topicWinHeader, "ExpandStatus") != "ON") {
                                                   WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.blkWindow.BlkJSX.setZIndex(WindowingPlayerHeaderInstanceArr[instanceId].highZIndex, true);
                                                   //In fullscreen status,disabled 'Delete' buttons
                                                   btnDeleteVar.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                   //preserver current button stat in an instance varible
                                                   WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtnExpand = jsx3.gui.ImageButton.STATE_ON;
                                                   topic_Set(topicWinHeader, "ExpandStatus", "ON");
                                                 }
                                                 else {
                                                   WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.blkWindow.BlkJSX.setZIndex(WindowingPlayerHeaderInstanceArr[instanceId].normalZIndex, true);
                                                   //In fullscreen status,disabled 'Delete' buttons
                                                   btnDeleteVar.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED,true);
                                                   //preserver current button stat in an instance varible
                                                   WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtnExpand = jsx3.gui.ImageButton.STATE_OFF;
                                                   topic_Set(topicWinHeader, "ExpandStatus", "OFF");
                                                 }
                                                 //3. Go to full window
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj._toggleViewMode(WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer);

                                                 //4. Repaint cmpLytVideoBlocksGrid
                                                 topic_Publish(topicWinHeader, "MultiPlayerViewUpdate", (new Date));

                                               }
                                             });
     var btnDeleteVar = new CmpImageButton([winBlkJSX, "window_delete"],
                                              null,
                                             { action: "FNCCALL",
                                               context: this,
                                               fnc: function() {
                                                  //1. Delete a connection
                                                  topic_Publish(topicWinHeader, "DeleteSessionConnection", WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.playerInstanceId);
                                                  WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.updateBlkWindowSelectorAfterDelete(WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.playerInstanceId);
                                               }
                                             });
    var btnMuteVar = new CmpImageButton([winBlkJSX, "window_mute"],
                                            null,
                                           { action: "FNCCALL",
                                             context: this,
                                             fnc: function() {
                                              //check player availability for SinglePlayerMode
                                              checkPlyAvailableForSingleMode(WindowingPlayerHeaderInstanceArr[instanceId]);
                                              if (WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.muteState == 0) {
                                                 //mute
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.doAction("MuteOn");
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.muteState = 1;
                                                 //preserver current button stat in an instance varible
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtnMute = jsx3.gui.ImageButton.STATE_ON;
                                               }
                                               else {
                                                 //unmute
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.doAction("MuteOff");
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.muteState = 0;
                                                 //preserver current button stat in an instance varible
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtnMute = jsx3.gui.ImageButton.STATE_OFF;
                                               }
                                               topic_Publish(topicWinHeader, "OneMuteEvent", "Update");

                                             }
                                           });
      var btnKBMVar = new CmpImageButton([winBlkJSX, "window_kbm"],
                                            null,
                                           { action: "FNCCALL",
                                             context: this,
                                             fnc: function() {
                                              /**
                                               * btnKBM 'State' can be changed both by mouse click the button and hot key hit (Ctrl+K/Ctrl-Ctrl-K)
                                               * This requires each button click ensure its 'State' unify with the view
                                               * i.e.:assuming the button itial 'Off', click the button(view 'On') then change its 'State' 'Off'; 
                                               **/ 
                                              //check player availability for SinglePlayerMode
                                              checkPlyAvailableForSingleMode(WindowingPlayerHeaderInstanceArr[instanceId]);
                                              if (WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.remoteEnable == 0) {
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.remoteEnable = 1;
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.doAction("RemoteOn");
                                                 //preserver current button stat in an instance varible
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtnKBM = jsx3.gui.ImageButton.STATE_ON;
                                                 btnKBMVar.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true); 
                                               }
                                               else {
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.remoteEnable = 0;
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.cmpPlayer.doAction("RemoteOff");
                                                 //preserver current button stat in an instance varible
                                                 WindowingPlayerHeaderInstanceArr[instanceId].ThisObj.stateBtnKBM = jsx3.gui.ImageButton.STATE_OFF;
                                                 btnKBMVar.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF,true); 
                                               }
                                             }
                                           });
      // ------------------------------------ player window name ------------------------------------
      var lblSelectedWindowVar = new CmpLabel(([winBlkJSX, "lblSelectedWindow"]),
                                                null,
                                                [{ action: "TOPICEVENT", topic: topicWinHeader, eventvar: "LabelSelectedWindow", eventval: null}]
                                                );

      // ------------------------------------ block window selector panel ---------------------------
      var cntWinSelectorPanelVar = new CmpContainer([winBlkJSX, "lytWinSelector"],
                                                 [],
                                                 [{ action: "TOPICEVENT", topic: topicWinHeader, eventvar: "WindowingHeaderLytUpdate", eventval: null}]
                                                );

      // ------------------------------------ total windowing header container ---------------------------
      var btnRemoteKBMVar = new CmpContainer([winBlkJSX, "blkHeaderButtons"],
                                                         [{ cmp: btnKBMVar}],
                                                         null
                                                         );

      var cntWindowingHeaderContolsVar = new CmpContainer([winBlkJSX, "blkWindowControls"],
                                                         [{ cmp: btnFullscreenVar },
                                                          { cmp: btn1To1Var },
                                                          { cmp: btnExpandVar },
                                                          { cmp: btnDeleteVar },
                                                          { cmp: btnMuteVar },
                                                          { cmp: btnRemoteKBMVar},
                                                          { cmp: lblSelectedWindowVar },
                                                          { cmp: cntWinSelectorPanelVar }
                                                          ],
                                                          null
                                                         );

      //get lytCols for ctrl mode to hide the blkSelectedWindow
      var lytSelectedWindowVar = getJsxByName([winBlkJSX, "layoutCols"]);

      /**
      * checkPlyAvailableForSingleMode: check player available for single player mode 
      * 
      * User case: when start a multi-connections session in SinglePlayerMode, player need start and stop many times,
      * this cause timing issue which player not being updated in time sometimes;
      * since SinglePlayerMode has only one player at a time, from arrVideoBlockList can always get the valid one;
      */
      checkPlyAvailableForSingleMode = function(thisInstance) {
        if (topic_Get(topicWinHeader, "SinglePlayerMode") != "true") return;
        thisInstance.ThisObj._LOG.info("::checkPlyAvailableForSingleMode:thisObj.playerInstanceId= " + thisInstance.ThisObj.playerInstanceId);
        if (thisInstance.ThisObj.cmpPlayer.IPVideoPlayer && thisInstance.ThisObj.cmpPlayer.IPVideoPlayer.toggleDisplay) return;
        //In extreme 'SinglePlayerMode' case, arrVideoBlockList & arrBlkWindow coexist many elements,but only one valid IPVideoPlayer
        for (var instanceId in thisInstance.arrVideoBlockList) {
          var currentPlayer = thisInstance.arrVideoBlockList[instanceId].cmpPlayer;
          if (currentPlayer.IPVideoPlayer && currentPlayer.IPVideoPlayer.toggleDisplay) {
            thisInstance.ThisObj.cmpPlayer = currentPlayer;
            thisInstance.ThisObj._LOG.info("::checkPlyAvailableForSingleMode:EXCHANGE player:thisInstance.ThisObj.cmpPlayer.InstanceID= " + thisInstance.ThisObj.cmpPlayer.InstanceID);
            break;
          }
        }
      }

      return {
            // return public singleton properties and function which can be access outside of instance
            ThisObj: null,
            highZIndex: 10,
            normalZIndex: 0,
            strLytClosShow: "*,240",
            strLytClosHide: "*,0",
            //bind blkWindow to intanceId in an Array
            arrBlkWindow: new Object,
            //pass in from player grid
            arrVideoBlockList: null,
            //by default start and select the first windowing header block
            selectDefaultBlkWindow: true,
            //buttons            
            btnFullscreen:btnFullscreenVar,
            btn1To1: btn1To1Var,
            btnExpand: btnExpandVar,
            btnDelete: btnDeleteVar,
            btnMute: btnMuteVar,
            /**
             * remoteKBM button
             * [1]Enabled: only when IPVPlayer.IsKBMSSuccess '1'("LocalKMBSState" & RFB status enabled);
             * [2]Disabled:IPVPlayer.IsKBMSSuccess '0';
             * [3]State:on;  (1)click from 'off'(2)Ctrl-k 
             * [4]State:off; (1)by default(2)click from 'on'(3)Ctrl-Ctrl-K
             **/
            btnKBM: btnKBMVar,
            // player window name 
            lblSelectedWindow: lblSelectedWindowVar,
            // block window selector panel
            cntWinSelectorPanel: cntWinSelectorPanelVar,
            // windowing header container
            cntWindowingHeaderContols: cntWindowingHeaderContolsVar,
            //get lytCols for ctrl mode to hide the blkSelectedWindow
            lytSelectedWindow: lytSelectedWindowVar,

            initView: (function(instance) {
                                jsx3.sleep(function() {
                                instance.cntWindowingHeaderContols.init();
                                instance.cntWindowingHeaderContols.show();
                                  //subscrib event listener
                                instance.eventWrap(eventApp, instance);
                                  //set initial status for all buttons,especially for a session without any connections
                                instance.btnFullscreen.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                instance.btn1To1.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                instance.btnExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                instance.btnMute.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                instance.btnDelete.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                instance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                }, null, null);
                      }),

            /** -------------------------------------  event wrapper -------------------------------------- */
            /**
            * Envent listener sections
            * @param eventApp {Object}
            */
            eventWrap: function(eventApp,singletonInstance) {
              //a potential bug: cmpPlayer.IsKBMSSuccess always being '0'
              //when Ctrl+K/bKBMSEnabled call back return 'true', if IsRFBReady=1, set 'LocalKBMSState' to '1', goes to KBM
              //when Ctrl-Ctrl-K/bKBMSEnabled call back return 'false', if IsRFBReady=1, set 'LocalKBMSState' to '0', back from KBM
              eventWinHeader.subscribe("bKBMSEnabled", null, this,
                                  function(EventVar, EventVal) { 
                                    var instanceId = EventVal.substring(0,EventVal.indexOf(':'));
                                    var callBackMsg = EventVal.substring(EventVal.indexOf(':') + 1);
                                    singletonInstance.ThisObj._LOG.info("::eventWinHeader:Ctrl+K:(bKBMSEnabled):" + callBackMsg + ": player instanceId= " + instanceId);
                                    //in case of 'SinglePlayerMode'
                                    if (!singletonInstance.arrBlkWindow[instanceId] && topic_Get(topicWinHeader, "SinglePlayerMode") == "true") {
                                      singletonInstance.ThisObj._LOG.info("::eventWinHeader:Ctrl+K:(bKBMSEnabled):SINGLEPLAYERMODE:: singletonInstance.arrBlkWindow[" + instanceId + "] = " + singletonInstance.arrBlkWindow[instanceId]);
                                      instanceId = getAssociativeArrayFirstElement(singletonInstance.arrBlkWindow);
                                      singletonInstance.ThisObj._LOG.info("::eventWinHeader:Ctrl+K:(bKBMSEnabled):SINGLEPLAYERMODE:: instanceId change to first element of arrBlkWindow= " + instanceId);
                                    }
                                    var blkWindowObj = singletonInstance.arrBlkWindow[instanceId];
                                    //If ServerRFB disconnect (VNC server not ready)just return
                                    if (blkWindowObj.cmpPlayer && blkWindowObj.cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady == 0) {
                                      singletonInstance.ThisObj._LOG.info("::eventWinHeader:Ctrl+K:(bKBMSEnabled):IPVideoPlayer is not loaded:" + (blkWindowObj.cmpPlayer.IPVideoPlayer == null) + ":IPVideoPlayer.IsRFBReady: " + (blkWindowObj.cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady != 0));
                                      return;
                                    }
                                    //If Ctrl+K/Ctrl-Ctrl-K not happen in current selected player, just return
                                    //If Ctrl+K/Ctrl-Ctrl-K only take effect for current selected player
                                    if (instanceId != singletonInstance.ThisObj.playerInstanceId) {
                                      singletonInstance.ThisObj._LOG.info("::eventWinHeader:Ctrl+K:(bKBMSEnabled):IPVideoPlayer is not current selected one.");
                                      return;
                                    }
                                    //Condition:
                                    //If IsRFBReady==1, set 'LocalKBMSState' to proper value 0/1 based on profile settings(cmpBtnRemoteKBMFlag) 
                                    //Actions: 
                                    // [1] call back 'true'(Ctrl+K) & STATE_OFF, simulate to click btnKBM, set LocalKBMSState to '1';
                                    // [2] call back 'false' (Ctrl-Ctrl-K) & STATE_ON, simulate to click btnKBM, set LocalKBMSState to '0';
                                    // [3] call back 'true' (Ctrl+K) & STATE_ON, do nothing
                                    // [4] call back 'false' (Ctrl-Ctrl-K) & STATE_OFF, do nothing

                                    if (callBackMsg=='true' && blkWindowObj.cmpPlayer.cmpBtnRemoteKBMFlag && blkWindowObj.stateBtnKBM == jsx3.gui.ImageButton.STATE_OFF) {
                                      //Action [1], Ctrl+K
                                      blkWindowObj.stateBtnKBM = jsx3.gui.ImageButton.STATE_ON;
                                      blkWindowObj.cmpPlayer.remoteEnable = 1;
                                      blkWindowObj.cmpPlayer.doAction("RemoteOn");
                                    }
                                    else if (callBackMsg == 'false' && blkWindowObj.cmpPlayer.cmpBtnRemoteKBMFlag && blkWindowObj.stateBtnKBM == jsx3.gui.ImageButton.STATE_ON) {
                                      //Action [2], Ctrl-Ctrl-K 
                                      blkWindowObj.stateBtnKBM = jsx3.gui.ImageButton.STATE_OFF;
                                      blkWindowObj.cmpPlayer.remoteEnable = 0;
                                      blkWindowObj.cmpPlayer.doAction("RemoteOff");
                                    }
                                    //Repaint:
                                    //if Ctrl+k/Ctrl-Ctrl-K happen in current selected instance,repaint the KBM button immediately
                                    if (instanceId == singletonInstance.ThisObj.playerInstanceId) {
                                      singletonInstance.ThisObj.setBlkHeaderBtnStates();
                                    }
                                  });

              //If disconnect VNC server/bRFBEnabled return 'false',set all remoteKBM buttons to 'Disabled' ('false' happen when ServerRFB disconnects;'true' happens when it connects)
              //If connect VNC server/bRFBEnabled return 'true',set all remoteKBM buttons to 'Enabled' & set all buttons which have 'State' 'On' to 'LocalKBMSState' to '1' 
              eventWinHeader.subscribe("bRFBEnabled", null, this,
                                  function(EventVar, EventVal) { 
                                    var instanceId = EventVal.substring(0,EventVal.indexOf(':'));
                                    var callBackMsg = EventVal.substring(EventVal.indexOf(':') + 1);
                                    singletonInstance.ThisObj._LOG.info("::eventWinHeader:(bRFBEnabled):" + callBackMsg + ":player instanceId= " + instanceId);
                                    if (!singletonInstance.arrBlkWindow[instanceId] && topic_Get(topicWinHeader, "SinglePlayerMode") == "true") {
                                      instanceId = getAssociativeArrayFirstElement(singletonInstance.arrBlkWindow);
                                    }
                                    var blkWindowObj = singletonInstance.arrBlkWindow[instanceId];
                                    //VNC server call back(IsRFBReady)
                                    if (callBackMsg == 'false') {
                                      //Disable remoteKBM button
                                      singletonInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                      if(singletonInstance.ThisObj.playerInstanceId != instanceId){
                                        instanceId = singletonInstance.ThisObj.playerInstanceId;
                                        if (singletonInstance.ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer && singletonInstance.ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.cmpBtnRemoteKBMFlag
                                        && (singletonInstance.ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer && singletonInstance.ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady)
                                        && topic_Get(topicWinHeader, 'CollaborationTimeState') == 'LIVE') {
                                            if (singletonInstance.ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer){ 
                                              singletonInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                            }
                                        }
                                      }
                                    }
                                    else if (blkWindowObj.cmpPlayer && blkWindowObj.cmpPlayer.cmpBtnRemoteKBMFlag) {
                                      //Enable remoteKBM button
                                      singletonInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                      //if the previous is remoteKBM STATE_ON
                                      if (blkWindowObj.stateBtnKBM == jsx3.gui.ImageButton.STATE_ON) {
                                        //restore its LocalKBMSState to '1'
                                        blkWindowObj.cmpPlayer.doAction("RemoteOn");
                                      }
                                    }
                                    else {
                                      singletonInstance.ThisObj._LOG.info("::eventWinHeader:(bRFBEnabled):" + callBackMsg + ":blkWindowObj.cmpPlayer(" + instanceId + ") loaded:" + (blkWindowObj.cmpPlayer != null));
                                      singletonInstance.ThisObj._LOG.info("::eventWinHeader:(bRFBEnabled):clicking current highlighted window.");
                                      singletonInstance.ThisObj.focusDefaultBlkWindow();
                                    }


                                  });

              //Using 'bNativeSizeChange' to indicate player load
              eventWinHeader.subscribe("bNativeSizeChange", null, this,
                                      function(EventVar, EventVal) {
                                        var instanceId = EventVal.substring(0, EventVal.indexOf(':'));
                                        var callBackMsg = EventVal.substring(EventVal.indexOf(':') + 1);
                                        singletonInstance.ThisObj._LOG.info("::eventWinHeader:(bNativeSizeChange):" + callBackMsg + ":player instanceId= " + instanceId);
                                        //current selected instance,repaint the KBM button immediately
                                        if (instanceId == singletonInstance.ThisObj.playerInstanceId) {
                                          //Only when IPVideoPlayer ready, IPVideoPlayer.IsRFBReady is valid, then to set btnKMB STATEENABLED at player first launch time;
                                          if (singletonInstance.arrBlkWindow[instanceId].cmpPlayer && singletonInstance.arrBlkWindow[instanceId].cmpPlayer.cmpBtnRemoteKBMFlag) {
                                            singletonInstance.ThisObj._LOG.info("::eventWinHeader:(bNativeSizeChange):loaded player instanceId= " + instanceId + ":IsRFBReady=" + singletonInstance.arrBlkWindow[instanceId].cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady);
                                            if (singletonInstance.arrBlkWindow[instanceId].cmpPlayer.IPVideoPlayer && singletonInstance.arrBlkWindow[instanceId].cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady
                                              && topic_Get(topicWinHeader, 'CollaborationTimeState') == 'LIVE') {
                                              singletonInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                            }
                                          }
                                          //update the video block. After video starts playing this is published and based on the nativemode do the
                                          // necessary action. If we do before or after the start(url) this does not take effect.
                                          if (singletonInstance.arrBlkWindow[instanceId].cmpPlayer.NativeMode) {
                                            singletonInstance.arrBlkWindow[instanceId].cmpPlayer.doAction("NativesizeOn"); 
                                          }
                                          else {
                                            singletonInstance.arrBlkWindow[instanceId].cmpPlayer.doAction("NativesizeOff");
                                          }

                                        }

                                      });

              eventWinHeader.subscribe("bFullScreen", null, this,
                                  function(EventVar, EventVal) {
                                    var arrEventVal = EventVal.split(':');
                                    var instanceId = arrEventVal[0];
                                    var callBackMsg = arrEventVal[1];
                                    /** 
                                    * SinglePlayerMode reuse a IPVPlayer; Since fullscreen using inject HTML,it always use first initial instanceId,
                                    * get it from arrBlkWindow for multi-connections/single player mode case;
                                    **/
                                    if (jsx3.util.strEmpty(singletonInstance.arrBlkWindow[instanceId]) && topic_Get(topicWinHeader, "SinglePlayerMode") == "true") {
                                      instanceId = getAssociativeArrayFirstElement(singletonInstance.arrVideoBlockList);
                                    }
                                    singletonInstance.ThisObj._LOG.info("::eventWrap:(bFullScreen): doFullScreen listener 'bFullScreen': fullscreenMsg= " + EventVal);
                                    topic_Publish(topicWinHeader, "DropFPS", "DROP");

                                    var expandStatus = topic_Get(topicWinHeader, "ExpandStatus");
                                    if (expandStatus != "ON" && topic_Get(topicWinHeader, "SinglePlayerMode") != "true") {
                                      //become a full window if not yet
                                      if (singletonInstance.arrBlkWindow[instanceId] && !singletonInstance.arrBlkWindow[instanceId].cmpPlayer) {
                                        singletonInstance.arrBlkWindow[instanceId].blkWindow.onSelect.call(singletonInstance.arrBlkWindow[instanceId].blkWindow);
                                      }
                                      else {
                                        singletonInstance.ThisObj._LOG.info("::eventApp:(bFullScreen):expandStatus=" + expandStatus + " :callBackMsg=" + callBackMsg + ": arrBlkWindow[" + instanceId + "].cmpPlayer=" + singletonInstance.arrBlkWindow[instanceId].cmpPlayer);
                                      }
                                      singletonInstance.arrBlkWindow[instanceId]._toggleViewMode(singletonInstance.arrBlkWindow[instanceId].cmpPlayer);
                                    }

                                    var LabelSelectedWindow = singletonInstance.ThisObj.EllipsisLabelSelectedWindow(singletonInstance.arrVideoBlockList[instanceId].connSrcEntityName, 20);
                                    topic_Publish(topicWinHeader, "LabelSelectedWindow", LabelSelectedWindow);
                                    //3. manage src panel & bookmark panel if necessary
                                    topic_Publish(topicWinHeader, "ToggleDisplay", instanceId + ":" + callBackMsg);

                                    //4. repaint cmpLytVideoBlocksGrid
                                    topic_Publish(topicWinHeader, "MultiPlayerViewUpdate", new Date);
                                    singletonInstance.ThisObj._LOG.info("::eventApp:(bFullScreen):PUB 'MultiPlayerViewUpdate'");
                                     
                                  }
                                );


              eventWinHeader.subscribe("MediaPlayerMode", null, this,
                                      function(EventVar, EventVal) {
                                        var playerMode = EventVal;
                                        if (playerMode == "PLAYER") {
                                          if (singletonInstance.lytSelectedWindow.getCols() != singletonInstance.strLytClosShow) {
                                            singletonInstance.lytSelectedWindow.setCols(singletonInstance.strLytClosShow, true);
                                          }
                                        }
                                        else {
                                          if (singletonInstance.lytSelectedWindow.getCols() != singletonInstance.strLytClosHide) {
                                            singletonInstance.lytSelectedWindow.setCols(singletonInstance.strLytClosHide, true);
                                          }
                                        }
                                      });

            // topic_Publish(tCtrl, "UpdateMute", (new Date)); at controllerUI
            eventWinHeader.subscribe("UpdateMute", null, this,
                                      function(EventVar, EventVal) {
                                        //dynamicaly set each player mute state synchronize with mute all in controller UI
                                        if (!singletonInstance.ThisObj) return;
                                        // controllerUI set a reverse state like topic_Set(tCtrl, "MuteStateAll", 0); //muted
                                        singletonInstance.stateBtnMute = topic_Get(topicWinHeader, "MuteStateAll") == 1 ? 0 : 1;
                                        singletonInstance.btnMute.BlkJSX.setState(this.stateBtnMute, true);
                                      });

            // delete connection from srcCtrlPanel 'delete' icon
            eventWinHeader.subscribe("DeleteConnection", null, this,
                                      function(EventVar, EventVal) {
                                        var sessionConnId = EventVal;
                                        if (!sessionConnId) return;
                                        for (var instanceId in singletonInstance.arrVideoBlockList) {
                                          if (topic_Get(topicWinHeader, "SinglePlayerMode") != "true") {
                                            if(sessionConnId == singletonInstance.arrVideoBlockList[instanceId].connId) {
                                               singletonInstance.arrVideoBlockList[instanceId].isProcessed = true;
                                               //from delete connectionId find mapping instanceId
                                               singletonInstance.ThisObj.updateBlkWindowSelectorAfterDelete(instanceId);
                                               break;
                                            }
                                          }
                                          else {
                                            //deleteConn of SinglePlayerMode disable buttons,unselect header block,reset name
                                            singletonInstance.ThisObj.setBlkHeaderBtnEnabled("DISABLED");
                                            singletonInstance.ThisObj.resetCSSpvrWindowForOtherWindows(instanceId);
                                            topic_Publish(topicWinHeader, "LabelSelectedWindow", "");
                                          }
                                        }
                                    });
            // topic_Publish(tCtrl, "CollaborationTimeState", "TIMESHIFTED"); from SessionCtrlAgent.timeLineEvent
            eventWinHeader.subscribe("CollaborationTimeState", null, this,
                                      function(EventVar, EventVal) {
                                          //dynamicaly set each player kbm state synchronize with 'TIMESHIFTED' status in controller UI
                                          if (!singletonInstance.ThisObj) return;
                                          if (EventVal == 'TIMESHIFTED') {
                                            if(singletonInstance.ThisObj.stateBtnKBM == jsx3.gui.ImageButton.STATE_ON) {
                                              // disable kbm if it is enabled
                                              singletonInstance.ThisObj.cmpPlayer.doAction("RemoteOff");
                                            }
                                            singletonInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                          }
                                          else if (EventVal == 'LIVE' && (singletonInstance.ThisObj.cmpPlayer.cmpBtnRemoteKBMFlag == false || (!jsx3.util.strEmpty(singletonInstance.ThisObj.cmpPlayer.IPVideoPlayer.IPVideoPlayer) && !singletonInstance.ThisObj.cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady))) {
                                            singletonInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                          }
                                      });                                      
      }//event wrapper
  
  }//return

 }//loadView
      
  /** ------------------------------------ INSTANCE properties/methods ------------------------------------ */

  /** @private @jsxobf-clobber */
  WindowingPlayerHeader_prototype._LOG = jsx3.util.Logger.getLogger(WindowingPlayerHeader.jsxclass.getName());
  //cmpplayer of current WindowingPlayerHeader instance  
  WindowingPlayerHeader_prototype.cmpPlayer  = null;
  //instanceId of current WindowingPlayerHeader instance
  WindowingPlayerHeader_prototype.playerInstanceId = null;
  //keep block index for Delete operation
  WindowingPlayerHeader_prototype.blockIndex = null;


  //header buttons state of current WindowingPlayerHeader instance
  WindowingPlayerHeader_prototype.stateBtnFullscreen = jsx3.gui.ImageButton.STATE_OFF;
  WindowingPlayerHeader_prototype.stateBtn1To1 = jsx3.gui.ImageButton.STATE_OFF;
  WindowingPlayerHeader_prototype.stateBtnExpand = jsx3.gui.ImageButton.STATE_OFF; 
  WindowingPlayerHeader_prototype.stateBtnMute = jsx3.gui.ImageButton.STATE_OFF; 
  WindowingPlayerHeader_prototype.stateBtnDelete = jsx3.gui.ImageButton.STATE_OFF;
  WindowingPlayerHeader_prototype.stateBtnKBM = jsx3.gui.ImageButton.STATE_OFF;

  WindowingPlayerHeader_prototype.tWinHeader = null;
  WindowingPlayerHeader_prototype.currentInstance = null;
  /**
  * init method need player instanceId and enventTopic instance.
  * @param playerInstanceId {String}
  * @param eventTopic {Object}
  */
  WindowingPlayerHeader_prototype.init = function(playerInstanceId,tHeader) {
    this.currentInstance = WindowingPlayerHeaderInstanceArr[tHeader];
    this.tWinHeader = tHeader;
    this.playerInstanceId = playerInstanceId;
    topic_Set(this.tWinHeader, "UpdateBlockIndex", "NONE");
    // setup event subscriptions

  };

  /**
  * Set 'lytWinSeletor' rows 
  * @param rowsStr {String}
  */
  WindowingPlayerHeader_prototype.setRows = function(rowsStr) {
    this.currentInstance.cntWinSelectorPanel.BlkJSX.setRows(rowsStr, true);
  };

  /**
  * Set 'lytWinSeletor' clos 
  * @param rowsStr {String}
  */
  WindowingPlayerHeader_prototype.setCols = function(closStr) {
    this.currentInstance.cntWinSelectorPanel.BlkJSX.setCols(closStr, true);
  };

  /**
  * Add a player windowing header block in 'lytWinSelector' while multiPlayerUI adding a video in the Grid
  * 1.get & clone a paneGrid template; 2. wrapper it as a CmpBlockButton ;
  * 3.adopt in cntWinSelectorPanel DOM; 4. push it in cntWinSelectorPanel container 
  * @param blkIndex {int}
  * @param instanceId {String}
  * @param arrVideoBlockList {associative array} 
  */
  WindowingPlayerHeader_prototype.addBlkWinToLytWinSeletor = function(blkIndex, instanceId, arrVideoBlockList) {
    this.currentInstance.arrVideoBlockList = arrVideoBlockList;
    var ThisObj = this;
    var cmpBlkWindowTemplate = getJsxByName([this.currentInstance.winBlkJSX, "paneGrid"]);
    var cmpBlkWindow = cmpBlkWindowTemplate.doClone(jsx3.app.Model.PERSISTEMBED, 0);
    //set default CSS rule to solve the display problem of lytGrid
    //cmpBlkWindow.setClassName(CSSpvrWindow); set in GI
//    cmpBlkWindow.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true); 
    var blkWindow = new CmpBlockButton([cmpBlkWindow], null,
                                             { action: "FNCCALL",
                                               context: this,
                                               fnc: function() {
                                                  ThisObj._LOG.info("::addBlkWinToLytWinSeletor::Click blkWindow:blkIndex:" + blkIndex + ":instanceId:" + instanceId);
                                                  if (topic_Get(this.tWinHeader, "SinglePlayerMode") != "true" || !this.currentInstance.ThisObj) {
                                                    //set selected CSS rule to solve the display problem of lytGrid
                                                    if (topic_Get(this.tWinHeader, "UpdateBlockIndex") == instanceId) {
                                                      //update blkIndex when 'Delete' happens in a non-last element
                                                      var indexInDOM = blkWindow.BlkJSX.getChildIndex();
                                                      blkIndex = (indexInDOM + 1 == blkIndex) ? blkIndex : indexInDOM + 1;
                                                      //instanceId should be change to topic_Get value
                                                      instanceId = topic_Get(this.tWinHeader, "UpdateBlockIndex");
                                                      ThisObj._LOG.info("::addBlkWinToLytWinSeletor::Update_BlockIndex:indexInDOM: " + indexInDOM + " :blkIndex: " + blkIndex + " :update to deleted instanceId: " + instanceId);
                                                    }

                                                    //fix obsolete instanceId problem (may cause by DOM not updating)
                                                    if (!ThisObj.currentInstance.arrBlkWindow[instanceId]) {
                                                      var indexInDOM = blkWindow.BlkJSX.getChildIndex();
                                                      instanceId = getAssociativeArrayByIndex(ThisObj.currentInstance.arrBlkWindow, indexInDOM);
                                                      ThisObj._LOG.info("::addBlkWinToLytWinSeletor::Obsolete instanceId mapping:indexInDOM: " + indexInDOM + " :blkIndex: " + blkIndex + " :mapping indexInDOM to instanceId: " + instanceId);
                                                    }

                                                    (!ThisObj.currentInstance.arrVideoBlockList) && (ThisObj.currentInstance.arrVideoBlockList = arrVideoBlockList);
                                                    //update thisObj(blkWindow) properties during clicking
                                                    ThisObj.cmpPlayer = ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer;
                                                    ThisObj.playerInstanceId = instanceId;
                                                    //preserve windowing block index
                                                    ThisObj.blockIndex = blkIndex;
                                                    //stash thisObj to a class variable
                                                    ThisObj.currentInstance.ThisObj = ThisObj;
                                                    //stash 'blkIndex' and 'cmpPlayer' to arrBlkWindow so as to be indexed by 'instanceId'
                                                    ThisObj.currentInstance.arrBlkWindow[instanceId].blockIndex = blkIndex;
                                                    ThisObj.currentInstance.arrBlkWindow[instanceId].cmpPlayer = ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer;
                                                    //update arrBlkWindow[instanceId] dynamicly during clicking
                                                    ThisObj.currentInstance.arrBlkWindow[instanceId] = ThisObj;


                                                    //if delete the first block, swap last to first and focus on first,but the first is on processing 
                                                    //it can't be updated on time then using last in this case
                                                    if (instanceId == getAssociativeArrayFirstElement(ThisObj.currentInstance.arrBlkWindow) && 
                                                        topic_Get(this.tWinHeader, "UpdateBlockIndex")!="NONE") {
                                                      var lastElement = getAssociativeArrLastElement(ThisObj.currentInstance.arrVideoBlockList);
                                                      instanceId = lastElement;
                                                    }
                                                    topic_Set(this.tWinHeader, "UpdateBlockIndex", "NONE");
                                                    //unselect others not focus blocks for non-SinglePlayerMode
                                                    ThisObj.resetCSSpvrWindowForOtherWindows(instanceId);
                                                  }
                                                  else {
                                                    instanceId = !ThisObj.currentInstance.arrVideoBlockList[instanceId] ? getAssociativeArrayFirstElement(ThisObj.currentInstance.arrVideoBlockList) : instanceId;
                                                  }
                                                  //----------------- update windowing header block buttons,block name, and highlight selection ----------------

                                                  ThisObj.blkWindow.BlkJSX.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true); 
                                                  //set selected CSS rule for current window
                                                  ThisObj.blkWindow.BlkJSX.setClassName(CSSpvrWindowSelected).repaint();
                                                  ThisObj._LOG.info("::addBlkWinToLytWinSeletor::After setClassName :blkIndex: " + blkIndex + " :instanceId: " + instanceId + " :ThisObj.blockIndex: " + ThisObj.blockIndex);

                                                  //conditional set btnDelete & btnExpand for "SinglePlayerMode":disabled;
                                                  if (ThisObj.currentInstance.ThisObj.blkWindow.BlkJSX.getZIndex() == ThisObj.currentInstance.highZIndex ||
                                                      getAssociativeArrayLength(ThisObj.currentInstance.arrBlkWindow) == 1) {
                                                    ThisObj.currentInstance.btnExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                    ThisObj.currentInstance.btnDelete.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                  }
                                                  else {
                                                    ThisObj.currentInstance.btnExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                    ThisObj.currentInstance.btnDelete.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                  }
                                                  //publish 'srcEntityName' to 'lblSelectedWindow'
                                                  var LabelSelectedWindow = ThisObj.EllipsisLabelSelectedWindow(ThisObj.currentInstance.arrVideoBlockList[instanceId].connSrcEntityName, 20);

                                                  //topic_Publish(WindowingPlayerHeader.tWinHeader, "LabelSelectedWindow", arrVideoBlockList[instanceId].connSrcEntityName);
                                                  topic_Publish(this.tWinHeader, "LabelSelectedWindow", LabelSelectedWindow);

                                                  //btnKBM ENABLED/STATEDISABLED based on [1]cmpBtnRemoteKBMFlag(profile)[2]IsRFBReady
                                                  if (ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer && ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.cmpBtnRemoteKBMFlag
                                                      && (ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer && ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady)
                                                      && topic_Get(this.tWinHeader, 'CollaborationTimeState') == 'LIVE') {
                                                      if (ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer) ThisObj._LOG.info("::addBlkWinToLytWinSeletor:instanceId: " + instanceId + " :IsRFBReady: " +ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.IPVideoPlayer.IPVideoPlayer.IsRFBReady);
                                                      ThisObj.currentInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                  }
                                                  else {
                                                    ThisObj.currentInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                  }
                                                  
                                                  //set btnMute STATE_OFF/STATE_ON
                                                  if (ThisObj.currentInstance.arrVideoBlockList[instanceId].cmpPlayer.muteState == 1) {
                                                    ThisObj.currentInstance.btnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                  }
                                                  else {
                                                    ThisObj.currentInstance.btnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                  }

                                                  //set other buttons ENABLED
                                                  ThisObj.setBlkHeaderBtnEnabled("ENABLED");
                                                  //restore buttons status based on previous state
                                                  ThisObj.setBlkHeaderBtnStates();
                                                  var expandStatus = topic_Get(this.tWinHeader, "ExpandStatus");
                                                  if(expandStatus == "ON" && topic_Get(this.tWinHeader, "SinglePlayerMode") != "true"){
                                                    if(getAssociativeArrayLength(ThisObj.currentInstance.arrBlkWindow) == 1){
                                                      ThisObj.currentInstance.btnExpand.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
                                                      ThisObj.currentInstance.btnExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                    }
                                                    else{
                                                      ThisObj.currentInstance.btnExpand.BlkJSX.setState(jsx3.gui.ImageButton.STATE_ON, true);
                                                      ThisObj.currentInstance.btnExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
                                                    }
                                                  }
                                                  ThisObj.streamURL = ThisObj.currentInstance.arrVideoBlockList[instanceId].streamURL;
                                                  if (ThisObj.streamURL.match(/v2d:/)) {
                                                    if (ThisObj.streamURL.match(/avoption=/)) {
                                                      var avOption = ThisObj.streamURL.replace(/^.*avoption=(\w+).*$/, "$1");
                                                      if(avOption == "Audio"){
                                                        ThisObj.currentInstance.btnFullscreen.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                        ThisObj.currentInstance.btn1To1.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                        ThisObj.currentInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                      }
                                                      else if(avOption == "Video"){
                                                        ThisObj.currentInstance.btnMute.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
                                                      }
                                                    }
                                                  }
                                               }
                                             }, null, null);

    blkWindow.init();
    this.blkWindow = blkWindow;
    this.currentInstance.arrBlkWindow[instanceId] = this;
    this.currentInstance.cntWinSelectorPanel.BlkJSX.adoptChild(blkWindow.BlkJSX, true);
    this.currentInstance.cntWinSelectorPanel.SubCmpArr.push({ cmp: blkWindow });

    //as a requirement set default click to the first block
    if (this.currentInstance.selectDefaultBlkWindow) {
      //put this outside of 'sleep' to ensure 'onSelect' only once across class scope
      this.currentInstance.selectDefaultBlkWindow = false;
      this.setBlkHeaderBtnEnabled("DISABLED");
      this.focusDefaultBlkWindow();
    }
    else if (this.currentInstance.btnDelete.BlkJSX.getEnabled() == jsx3.gui.Form.STATEDISABLED &&
             getAssociativeArrayLength(this.currentInstance.arrBlkWindow) > 1) {
      //USER CASE:delete all conn from srcPanel and then add them back, 'btnDelete' & 'btnExpand' should be enabled
      this.focusDefaultBlkWindow();
    }
    else {//deselect other windows
      ThisObj.blkWindow.BlkJSX.setClassName(CSSpvrWindow).repaint();
    }
  }


  /**
  * Update windoing controller labale name: 
  * @param labelname {String}
  * @param labellength {String} 
  */
  WindowingPlayerHeader_prototype.EllipsisLabelSelectedWindow = function (text, textWidth) {
    if (!jsx3.util.strEmpty(textWidth)) {
      if (text.length > textWidth) {
        var i = 1;
        var returnText = '';
        while (returnText.length < (textWidth) && i < text.length) {
          returnText = text.substr(0, i) + '...';
          i++;
        }
        text = returnText;
      }
    }
    return text;
  }

  /**
  * focusDefaultBlkWindow check and select one default blkWindow while update lytWinSelector: 
  */
  WindowingPlayerHeader_prototype.focusDefaultBlkWindow = function() {
    //if (topic_Get(WindowingPlayerHeader.tWinHeader, "SinglePlayerMode") == "true") return;
    //as a requirement set default click to the first block
    var firstElement = getAssociativeArrayFirstElement(this.currentInstance.arrBlkWindow)
    if (!jsx3.util.strEmpty(firstElement)) {
      this.currentInstance.arrBlkWindow[firstElement].blkWindow.onSelect.call(this.currentInstance.arrBlkWindow[firstElement].blkWindow);
    }
  }
  /**
  * repaintBlkWindowSelector: repaint blkWindowSelector after delete connection
  * @param playerInstanceId {String}
  * @param eventTopic {Object}
  */
  WindowingPlayerHeader_prototype.updateBlkWindowSelectorAfterDelete = function(deletedInstance) {
    //SinglePlayerMode only have one 'Add_Player' in life span, it's different with non-SinglePlayerMode
    //For non-SinglePlayerMode WindowingPlayerHeader.arrBlkWindow can be created on the fly
    var lastEleInArrBlkWindow = getAssociativeArrLastElement(this.currentInstance.arrBlkWindow);
    if (topic_Get(this.tWinHeader, "SinglePlayerMode") == "true") {
      //deleteConn of SinglePlayerMode disable buttons,unselect header block,reset name
      this.currentInstance.ThisObj.setBlkHeaderBtnEnabled("DISABLED");
      this.currentInstance.ThisObj.resetCSSpvrWindowForOtherWindows(lastEleInArrBlkWindow);
      topic_Publish(this.tWinHeader, "LabelSelectedWindow", "");
    }
    else {
      //1. Remove the last element 
      var childrenTotal = getAssociativeArrayLength(this.currentInstance.arrVideoBlockList);
      var lastElement = this.currentInstance.cntWinSelectorPanel.BlkJSX.getChild(childrenTotal - 1);

      if (this.currentInstance.ThisObj.blockIndex == childrenTotal ||
          deletedInstance == lastEleInArrBlkWindow ||
          deletedInstance == getAssociativeArrLastElement(this.currentInstance.arrVideoBlockList)) {
        //2. Deleted is the last, remove it from DOM and array
        this.currentInstance.cntWinSelectorPanel.BlkJSX.removeChild(lastElement);
        //remove from the container cntWinSelectorPanel
        var toBeRemoveSubCmp = this.currentInstance.cntWinSelectorPanel.SubCmpArr[childrenTotal - 1];
        this.currentInstance.cntWinSelectorPanel.SubCmpArr.remove(toBeRemoveSubCmp);
        var expandStatus = topic_Get(this.tWinHeader, "ExpandStatus");
        if(expandStatus == "ON" && topic_Get(this.tWinHeader, "SinglePlayerMode") != "true"){
          this.currentInstance.ThisObj.blkWindow.BlkJSX.setZIndex(this.currentInstance.normalZIndex, true);
          topic_Set(this.tWinHeader, "ExpandStatus", "OFF");
          this.currentInstance.ThisObj._toggleViewMode(this.currentInstance.ThisObj.cmpPlayer);
          topic_Publish(this.tWinHeader, "MultiPlayerViewUpdate", (new Date));
        }
      }
      else {
        //trasfer 1:1 state as this property inheritance from player 
        this.currentInstance.ThisObj.stateBtn1To1 = this.currentInstance.ThisObj.cmpPlayer.NativeMode;
        //3. Deleted is not the last,get current to be deleted element, swap it with the last
        var toBeDeletedElement = this.currentInstance.cntWinSelectorPanel.BlkJSX.getChild(this.currentInstance.ThisObj.blockIndex - 1);
        //insert the last before the one to be deleted
        this.currentInstance.cntWinSelectorPanel.BlkJSX.insertBefore(lastElement, toBeDeletedElement);
        //remove toBeDeletedElement from DOM after inserting
        this.currentInstance.cntWinSelectorPanel.BlkJSX.removeChild(toBeDeletedElement);
        //remove from the container cntWinSelectorPanel
        var toBeRemoveSubCmp = this.currentInstance.cntWinSelectorPanel.SubCmpArr[this.currentInstance.ThisObj.blockIndex - 1];
        this.currentInstance.cntWinSelectorPanel.SubCmpArr.remove(toBeRemoveSubCmp);
        //assign the last to this,delete the last from arrBlkWindow
        this.currentInstance.arrBlkWindow[this.currentInstance.ThisObj.playerInstanceId] = this.currentInstance.arrBlkWindow[lastEleInArrBlkWindow].clone();
        //set moved blkWindow's instanceId so as to update its blkIndex while clicking this block/ the deleted element(last)
        topic_Set(this.tWinHeader, "UpdateBlockIndex", lastEleInArrBlkWindow);
      }
      //4. Reset all buttons to disabled & window name to ""
      this.currentInstance.ThisObj.setBlkHeaderBtnEnabled("DISABLED");
      topic_Publish(this.tWinHeader, "LabelSelectedWindow", "");

      //5. delete last element for non-SinglePlayerMode
      delete this.currentInstance.arrBlkWindow[lastEleInArrBlkWindow];

      //6. Set focus to the first blkWindow
      if (getAssociativeArrayLength(this.currentInstance.arrBlkWindow) > 0) {
        this.currentInstance.ThisObj.focusDefaultBlkWindow();
      }
      //7. reset while arrVideoBlockList is empty
      if (getAssociativeArrayLength(this.currentInstance.arrBlkWindow) == 0) {
        this.currentInstance.cntWinSelectorPanel.SubCmpArr = [];
        this.currentInstance.cntWinSelectorPanel.BlkJSX.removeChildren();
        WindowingPlayerHeader_prototype.blockIndex = 0
        //while deleting the last elements reset selectDefaultBlkWindow for auto foucs
        this.currentInstance.selectDefaultBlkWindow = true;
      }
    }
  };
  /**
  * Update windoing plyaer header layout, this happen when: 
  * 1. expand/unexand & fullscreen/unfullscreen, only need update layout UI
  * 2. delete, this need change both UI,cntWinSelectorPanel's children and arrVideoBlockList
  * @param rowStr {String}
  * @param colStr {String} 
  * @param arrVideoBlockList {Associative array}
  */
  WindowingPlayerHeader_prototype.updateLytWinSeletor = function(rowStr, colStr, arrVideoBlockList) {
    if (!jsx3.util.strEmpty(rowStr)) this.setRows(rowStr, true);
    if (!jsx3.util.strEmpty(colStr)) this.setCols(colStr, true);
    if (arrVideoBlockList) this.currentInstance.arrVideoBlockList = arrVideoBlockList;
  }

  /**
  * Set states to blkHeaderButtons, restor buttons states to buttons;
  */
  WindowingPlayerHeader_prototype.setBlkHeaderBtnStates = function(initialState) {
    if (!initialState) {
      this.currentInstance.btnFullscreen.BlkJSX.setState(this.stateBtnFullscreen, true);
      this.currentInstance.btnExpand.BlkJSX.setState(this.stateBtnExpand, true);
      this.currentInstance.btnDelete.BlkJSX.setState(this.stateBtnDelete, true);
      this.currentInstance.btnKBM.BlkJSX.setState(this.stateBtnKBM, true);
      //some test cases: after deleting video,the initial 1:1 state need update from player,it may change during layout re-org
      //this.stateBtn1To1 = this.currentInstance.ThisObj.cmpPlayer.NativeMode? 1: 0;
      this.currentInstance.btn1To1.BlkJSX.setState(this.stateBtn1To1, true);
      //set btnMute ImageButton.STATE conditionally when clicking windowing header block
    }
    else {
      this.currentInstance.btnFullscreen.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
      this.currentInstance.btn1To1.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
      this.currentInstance.btnExpand.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
      this.currentInstance.btnMute.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
      this.currentInstance.btnDelete.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
      this.currentInstance.btnKBM.BlkJSX.setState(jsx3.gui.ImageButton.STATE_OFF, true);
    }
  };


  /**
  * Set blkHeaderButtons initial to DISABLED until select a blkWindow
  */
  WindowingPlayerHeader_prototype.setBlkHeaderBtnEnabled = function(enabled) {
    if (enabled != "ENABLED") {
      this.currentInstance.btnFullscreen.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.currentInstance.btn1To1.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.currentInstance.btnExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.currentInstance.btnMute.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.currentInstance.btnDelete.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
      this.currentInstance.btnKBM.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    else {
      this.currentInstance.btnFullscreen.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.currentInstance.btn1To1.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      this.currentInstance.btnMute.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      //this.currentInstance.btnKBM.BlkJSX.setEnabled(this.currentInstance.ThisObj.stateBtnKBM, true);
      //this.currentInstance.btnDelete.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED, true);
      //set btnExpand,btnKBM STATEENABLED conditionallly when clicking windowing header block
    }
  };

  /**
  * _toggleViewMode : set status for cmpLytVideoBlocksGrid adjust
  */
  WindowingPlayerHeader_prototype._toggleViewMode = function(player) {
    player.displaymode.maxviewtoggle = 1;
    player.displaymode.maxview = (player.displaymode.maxview == 0) ? 1 : 0;
    this._LOG.info("::_toggleViewMode: player.displaymode.maxview = " + player.displaymode.maxview);
  };

  /**
  * resetCSSpvrWindowForOtherWindows : set status for cmpLytVideoBlocksGrid adjust
  */
  WindowingPlayerHeader_prototype.resetCSSpvrWindowForOtherWindows = function(instanceId) {
    for (var lcInstanceId in this.currentInstance.arrBlkWindow) { 
      var blkWindow = this.currentInstance.arrBlkWindow[lcInstanceId].blkWindow;
      if (lcInstanceId != instanceId) {
        blkWindow.BlkJSX.setClassName(CSSpvrWindow).repaint();
      }
    }
  };
  
  /**
  * destroy : used for unload view and destroy static proerties and function
  * this is called while player window finally being closed
  * @param instanceId {String}
  * @param tHeader {String}
  */
  WindowingPlayerHeader_prototype.destroy = function(instanceId, tHeader) {
    if (WindowingPlayerHeaderInstanceArr[tHeader]) {
      var currentInstance = WindowingPlayerHeaderInstanceArr[tHeader];
      currentInstance.ThisObj = null;
      currentInstance.highZIndex = null;
      currentInstance.strLytClosShow = null;
      currentInstance.strLytClosHide = null;
      currentInstance.arrBlkWindow = null;
      currentInstance.arrVideoBlockList = null;
      currentInstance.selectDefaultBlkWindow = null;
      currentInstance.btnFullscreen = null;
      currentInstance.btn1To1 = null;
      currentInstance.btnExpand = null;
      currentInstance.btnDelete = null;
      currentInstance.btnMute = null;
      currentInstance.btnKBM = null;
      currentInstance.lblSelectedWindow = null;
      currentInstance.cntWinSelectorPanel = null;
      currentInstance.cntWindowingHeaderContols = null;
      currentInstance.lytSelectedWindow = null;
      currentInstance.initView = null;
      currentInstance.eventWrap = null;
      delete WindowingPlayerHeaderInstanceArr[tHeader];
    }
  };

});

