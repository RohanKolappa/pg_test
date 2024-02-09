/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the Player Video Widget
*/

jsx3.Class.defineClass('com.ipvs.gui.PlayerVideoWidget', null, null, function(PlayerVideoWidget, PlayerVideoWidget_prototype) {

  // create Logger for this class
  PlayerVideoWidget_prototype._LOG = jsx3.util.Logger.getLogger(PlayerVideoWidget.jsxclass.getName());

  /**
  * Instance varibales
  */

  //default delay time, miliseconds
  PlayerVideoWidget_prototype.videoHeaderDelayTime = 5000;
  PlayerVideoWidget_prototype.eventPly = null;
  PlayerVideoWidget_prototype.topicPly = null;
  PlayerVideoWidget_prototype.winPlyBlkJSX = null;
  PlayerVideoWidget_prototype.instanceId = null;
  //container of the player header and viedo block
  PlayerVideoWidget_prototype.cmpCtrVideoBlock = null;

  /**
  * The instance initializer.
  * @param topicPly {Object}
  * @param eventApp {Object}
  */
  PlayerVideoWidget_prototype.init = function(topicPly, eventPly, winPlyBlkJSX, instanceId) {
    this._LOG.debug("PlayerVideoWidget.init");
    //stash varibles
    this.eventPly = eventPly;
    this.topicPly = topicPly;
    this.winPlyBlkJSX = winPlyBlkJSX;
    this.instanceId = instanceId;
    this.eventWrap(eventPly);
    var ThisClass = this;
    var ThisInstanceId = instanceId;

    // get the template node
    var cmpBlkVideoBlockTemplate = getJsxByName([winPlyBlkJSX, "blkVideoBlock"]);
    // clone it
    var cmpBlkVideoBlock = cmpBlkVideoBlockTemplate.doClone(jsx3.app.Model.PERSISTREF, 0);
    // give it a unique name
    cmpBlkVideoBlock.setName(instanceId);

    // attach a header in blkVideoBlock which contain fullscreen,expand,1to1, and delete buttons
    var cmpLblVideoSrcEntity = new CmpLabel(([cmpBlkVideoBlock, "lblVideoSrcName"]),
                                          null,
                                          null);

    var cmpBtnVideoFullscreen = new CmpButton([cmpBlkVideoBlock, "btnVideoFullscreen"], null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function(EventVar, EventVal) {
                                               ThisClass._LOG.info(" Clicking button Fullscreen.");
                                               //drop fps 
                                               topic_Publish(topicPly, "DropFPS", "DROP");

                                               //change status 'ON/OFF' while clicking
                                               if (cmpBtnVideoFullscreen.BlkJSX.getState() == jsx3.gui.ImageButton.STATE_ON) {
                                                 cmpBtnVideoExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEDISABLED);
                                                 //hide 'Expand' button
                                                 topic_Publish(topicPly, "FullScreenStatus", "ON");
                                               }
                                               else {
                                                 cmpBtnVideoExpand.BlkJSX.setEnabled(jsx3.gui.Form.STATEENABLED);
                                                 //show 'Expand' button
                                                 topic_Publish(topicPly, "FullScreenStatus", "OFF");
                                               }
                                               topic_Publish(topicPly, "ToggleDisplay", instanceId);
                                               
                                               //to avoid duplicate call cmpBtnVideoExpand while clicking 'btnVideoExpand' then following 'btnVideoFullscreen'
                                               var expandStatus = topic_Get(topicPly, "ExpandStatus");
                                               // expandStatus 'OFF', adjust layout
                                               if (expandStatus != "ON" ) {
                                                ThisClass.toggleViewMode();
                                               }
                                               else {
                                                 //if expandStatus 'OFF', 'lytVideoBlocksContainerGrid' no change but need restore fps 
                                                topic_Publish(topicPly, "MultiPlayerViewUpdate", (new Date));
                                               }
                                            
                                             }
                                           });
    var cmpBtnVideoExpand = new CmpButton([cmpBlkVideoBlock, "btnVideoExpand"], null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function(EventVar, EventVal) {
                                               ThisClass._LOG.info(" Clicking button Expand widnow.");
                                               //drop fps 
                                               topic_Publish(topicPly, "DropFPS", "DROP");
                                               if (topic_Get(topicPly, "ExpandStatus") != "ON") {
                                                 topic_Set(topicPly, "ExpandStatus", "ON");
                                               }
                                               else {
                                                 topic_Set(topicPly, "ExpandStatus", "OFF");
                                               }
                                               ThisClass.toggleViewMode();
                                             }
                                           });

    var cmpBtnVideo1to1 = new CmpButton([cmpBlkVideoBlock, "btnVideo1to1"], null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function() {
                                               //video 1 to 1
                                               var nativeSizeMode = topic_Get(topicPly, "NativeSizeMode");
                                               if (ThisClass.cmpCtrVideoBlock.cmpPlayer.displaymode.one2one == 0) {
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.displaymode.one2one = 1;
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.doAction("NativesizeOn");
                                               }
                                               else {
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.displaymode.one2one = 0;
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.doAction("NativesizeOff");
                                               }
                                             }
                                           });

    var cmpBtnVideoDelete = new CmpButton([cmpBlkVideoBlock, "btnVideoDelete"], null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function() {
                                               //video delete
                                               topic_Publish(topicPly, "DeleteSessionConnection", instanceId);
                                             }
                                           });

    var cmpBtnMute = new CmpButton([cmpBlkVideoBlock, "btnAudio"], null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function() {
                                             if (ThisClass.cmpCtrVideoBlock.cmpPlayer.muteState == 0) {
                                                 //mute
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.doAction("MuteOn");
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.muteState = 1;
                                               }
                                               else {
                                                 //unmute
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.doAction("MuteOff");
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.muteState = 0;
                                               }
                                               topic_Publish(topicPly, "OneMuteEvent", "Update");
                                             }
                                           });

    var cmpBtnRemoteKBM = new CmpButton([cmpBlkVideoBlock, "btnKBM"], null,
                                           { action: "FNCCALL", context: null,
                                             fnc: function() {
                                               if (ThisClass.cmpCtrVideoBlock.cmpPlayer.remoteEnable == 0) {
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.remoteEnable = 1;
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.doAction("RemoteOn");
                                               }
                                               else {
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.remoteEnable = 0;
                                                 ThisClass.cmpCtrVideoBlock.cmpPlayer.doAction("RemoteOff");
                                               }
                                             }
                                           });

    var cmpBlkVideoHeader = new CmpContainer([cmpBlkVideoBlock, "blkVideoHeader"],
                                                 [{ cmp: cmpLblVideoSrcEntity },
                                                  { cmp: cmpBtnVideoFullscreen },
                                                  { cmp: cmpBtnVideoExpand, sel: "topic_Get('" + topicPly + "', 'FullScreenStatus') != 'ON'" },
                                                  { cmp: cmpBtnVideo1to1 },
                                                  { cmp: cmpBtnVideoDelete },
                                                  { cmp: cmpBtnMute },
                                                  { cmp: cmpBtnRemoteKBM, sel: "topic_Get('" + topicPly + "', 'EnableInstanceKbm_'+'" + ThisInstanceId + "') == '" + ThisInstanceId + "'"}],
                                                  [{ action: "TOPICEVENT", topic: topicPly, eventvar: "EnableInstanceKbm_" + ThisInstanceId, eventval: ThisInstanceId },
                                                   { action: "TOPICEVENT", topic: topicPly, eventvar: "FullScreenStatus", eventval: null}]);
    //assign 'autoHide' time 
    cmpBlkVideoHeader.autoHide = 0;
    // create a player component attached to the video object in it
    var cmpPlayer = new CmpPlayer([cmpBlkVideoBlock, "videoObjectContainer"], instanceId, 
                                null,
                                { action: "FNCCALL",
                                  context: null,
                                  fnc: function() {
                                    // on player load - fire only 'Start'
                                    if (this.lastAction != null) {
                                      this.doAction("Start");
                                      if(this.muteState == 1) this.doAction("MuteOn");
                                      this.lastAction = null;
                                    }
                                  }
                                }, topicPly);


    // create a container component for the block
    this.cmpCtrVideoBlock = new CmpContainer([cmpBlkVideoBlock],
                                  [
                                   { cmp: cmpPlayer }
                                  ],
                                  null,
                                  { action: "FNCCALL",
                                    context: null,
                                    fnc: function() {
                                      this.Destroy();
                                    }
                                  });

    // set its cmpPlayer property to point to the player component
    this.cmpCtrVideoBlock.cmpPlayer = cmpPlayer;
    this.cmpCtrVideoBlock.cmpVideoHeader = cmpBlkVideoHeader;

    //set for expand player
    this.cmpCtrVideoBlock.cmpPlayer.displaymode = { maxview: 0, maxviewtoggle: 0, one2one: 0 };

    //set for mute player
    this.cmpCtrVideoBlock.cmpPlayer.muteState = 0;
    this.cmpCtrVideoBlock.cmpPlayer.cmpBtnMute = cmpBtnMute;
    this.cmpCtrVideoBlock.cmpPlayer.cmpBtnVideoExpand = cmpBtnVideoExpand;
    //set connSrcEntityName outside
    this.cmpCtrVideoBlock.cmpLblVideoSrcEntity = cmpLblVideoSrcEntity;
    //hook remoteKBM button
    this.cmpCtrVideoBlock.cmpPlayer.cmpBtnRemoteKBM = cmpBtnRemoteKBM;
    this.cmpCtrVideoBlock.cmpPlayer.cmpBtnRemoteKBMFlag = false;
    //set for remote control enable
    this.cmpCtrVideoBlock.cmpPlayer.remoteEnable = 0;
    
    // onMouseOver
    this.cmpCtrVideoBlock.onMouseOver = function() {
      this.cmpVideoHeader.show();
    };

    // show/hide custom mask
    this.cmpCtrVideoBlock.handleMask = function (bolShowHide) {
      this.BlkJSX.getDescendantOfName("blkMask").setDisplay(bolShowHide, true);
    };

    
    //onSelect Destroy (clean up before component destory)
    this.cmpCtrVideoBlock.Destroy = function() {
      ThisClass._LOG.debug("PlayerVideoWidget.cmpCtrVideoBlock.Destroy");
    };

    this.cmpCtrVideoBlock.init();


  };

  /**
  * toggleViewMode
  */
  PlayerVideoWidget_prototype.toggleViewMode = function(expandStatus) {
      this.cmpCtrVideoBlock.cmpPlayer.displaymode.maxviewtoggle = 1;
      if (this.cmpCtrVideoBlock.cmpPlayer.displaymode.maxview == 0) {
        this.cmpCtrVideoBlock.cmpPlayer.displaymode.maxview = 1;
      }
      else {
        this.cmpCtrVideoBlock.cmpPlayer.displaymode.maxview = 0;
      }
      //update grid view
      topic_Publish(this.topicPly, "MultiPlayerViewUpdate", (new Date));
  };

  /**
  * get cmpCtrVideoBlock
  */
  PlayerVideoWidget_prototype.getVideoWidgetBlock = function() {
    return this.cmpCtrVideoBlock;
  };

  /**
  * Envent listener sections
  * @param eventApp {Object}
  */
  PlayerVideoWidget_prototype.eventWrap = function(eventPly) {
    var ThisClass = this;
    //Section for future event listener
  }
 });





  