/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */


/**
 * This class provides a block-based, object-oriented approach to creating media player objects
 */
jsx3.Class.defineClass("com.ipvs.gui.MediaPlayerIE", null, null, function (MediaPlayerIE, MediaPlayerIE_prototype) {

  /**
  * Static constant varibles for player modes
  */
  MediaPlayerIE.WINDOWLESS_MODE_PLAYER = 1;
  MediaPlayerIE.WINDOW_MODE_PLAYER = 0;

  // create Logger for this class
  MediaPlayerIE_prototype._LOG = jsx3.util.Logger.getLogger("com.ipvs.gui.MediaPlayerIE");

  /**
  * {String} plalyer ID 
  */
  MediaPlayerIE_prototype._playerId = "";

  /**
  * {Object}BlkJSX 
  */
  MediaPlayerIE_prototype.BlkJSX = null;

  /**
  * Default value WINDOW_MODE_PLAYER:normal player
  * {int} player type WINDOWLESS_MODE_PLAYER(1):windowless player, WINDOW_MODE_PLAYER(0):normal player;
  */
  MediaPlayerIE_prototype._playerType = MediaPlayerIE.WINDOWLESS_MODE_PLAYER;

  /**
  * {ActiveX object} media player
  */
  MediaPlayerIE_prototype.IPVideoPlayer = null;

  /**
  * {ActiveX object} media player
  */
  MediaPlayerIE_prototype._IteratePlayerLoadDelay = 50;

  /**
  * {ActiveX object} media player state 
  */
  MediaPlayerIE_prototype.IPVSPlayerState_PAUSED = 1;
  MediaPlayerIE_prototype.IPVSPlayerState_PLAYING = 2;

  /**
  * store player fps value 
  */
  MediaPlayerIE_prototype.firstFps = null;

  /**
  * native size of the media player
  */
  MediaPlayerIE_prototype.nativeSize = new Object();

  /**
  * topic tMediaPlayerIE, this property need dynamic inject after player loaded
  */
  MediaPlayerIE_prototype.tMediaPlayerIE = null;

  /**
  * Method to set media player type
  * @param value {int} 0:normal,1:windowless; Default value 1.
  */
  MediaPlayerIE_prototype.setPlayerType = function (value) {
    this._playerType = value;
  }

  /**
  * Method to get media player type
  * @return {int}
  */
  MediaPlayerIE_prototype.getPlayerType = function () {
    return this._playerType;
  }
  /**
  * Method to get media player 
  * @return {ActiveX Object}
  */
  MediaPlayerIE_prototype.getMediaPlayer = function () {
    return this.IPVideoPlayer;
  }
  /**
  * Method to get a non-zero fps 
  * @return {int}
  */
  MediaPlayerIE_prototype.readFirstFps = function () {
    return this.firstFps;
  }
  /**
  * Method to save a non-zero fps  
  * @param {int}
  */
  MediaPlayerIE_prototype.saveFirstFps = function (fps) {
    this.firstFps = fps;
  }
  /**
  * The instance initializer.
  *
  * @param instanceID {String}  
  * @param BlkJSX {Object} 
  * @param windowlessPlayer {int} 
  */
  MediaPlayerIE_prototype.init = function (instanceID, BlkJSX, playerType) {
    this._LOG.trace("initialize MediaPlayer instance.");
    this._playerId = instanceID;
    this.BlkJSX = BlkJSX;
    //check and set parameters
    if (typeof playerType != 'undefined' && !jsx3.util.strEmpty(playerType)) {
      this._playerType = playerType;
    }
  }
  //------------------------------------------------------------------------------------------
  /**
  * Instance call back function OnFullScreen(bFullScreen)
  * @param bFullScreen {boolean}
  */
  MediaPlayerIE_prototype.doFullScreen = function (bFullScreen) {
    var strPub = this._playerId;
    var callBackMsg = bFullScreen + '';
    var pubMsg = strPub + ":" + callBackMsg;
    if (!this.tMediaPlayerIE) {
      this._LOG.info("::doFullScreen:this.doEvent:'bFullScreen' playerId=" + strPub + " :fullscreenMsg= " + pubMsg);
      this.doEvent(com.ipvs.gui.MediaPlayerWindow.MSG_ONFULLSCREEN, { objEVENT: pubMsg });
    }
    else {
      this._LOG.info("::doFullScreen: topic_Publish:'bFullScreen' playerId=" + strPub + " :fullscreenMsg= " + pubMsg);
      topic_Publish(this.tMediaPlayerIE, "bFullScreen", pubMsg);
    }
  }

  /**
  * Instance call back function doKBMSEnable(bKBMSEnabled)
  */
  MediaPlayerIE_prototype.doKBMSEnable = function (bKBMSEnabled) {
    var strPub = this._playerId;
    var callBackMsg = bKBMSEnabled + '';
    var pubMsg = strPub + ":" + callBackMsg;
    if (!this.tMediaPlayerIE) {
      this._LOG.info("::doKBMSEnable:this.doEvent: 'bKBMSEnabled' playerId=" + strPub + " :pubMsg= " + pubMsg);
      this.doEvent(com.ipvs.gui.MediaPlayerWindow.MSG_ONKBMSENABLED + strPub, { objEVENT: pubMsg });
    }
    else {
      this._LOG.info("::doFullScreen: topic_Publish:'bKBMSEnabled' playerId=" + strPub + " :pubMsg= " + pubMsg);
      topic_Publish(this.tMediaPlayerIE, "bKBMSEnabled", pubMsg);
    }
  }

  /**
  * Instance call back function doRFBEnabled(bRFBEnabled)
  */
  MediaPlayerIE_prototype.doRFBEnabled = function (bRFBEnabled) {
    var strPub = this._playerId;
    var callBackMsg = bRFBEnabled + '';
    var pubMsg = strPub + ":" + callBackMsg;
    if (!this.tMediaPlayerIE) {
      this._LOG.info("::doRFBEnabled:this.doEvent: 'bRFBEnabled' playerId=" + strPub + " :pubMsg= " + pubMsg);
      this.doEvent(com.ipvs.gui.MediaPlayerWindow.MSG_ONRFBENABLED + strPub, { objEVENT: pubMsg });
    }
    else {
      this._LOG.info("::doFullScreen: topic_Publish:'bRFBEnabled' playerId=" + strPub + " :pubMsg= " + pubMsg);
      topic_Publish(this.tMediaPlayerIE, "bRFBEnabled", pubMsg);
    }

  }

  /**
  * Instance call back function doNativeSizeChange(bFullScreen)
  */
  MediaPlayerIE_prototype.doNativeSizeChange = function (a_lWidth, a_lHeight) {
    var strPub = this._playerId;
    var callBackMsg = a_lWidth + "*" + a_lHeight;
    var pubMsg = strPub + ":" + callBackMsg;
    if (!this.tMediaPlayerIE) {
      this._LOG.info("::doNativeSizeChange:this.doEvent: 'bNativeSizeChange' playerId=" + strPub + " :pubMsg= " + pubMsg);
      this.doEvent(com.ipvs.gui.MediaPlayerWindow.MSG_ONNATIVESIZECHANGE + strPub, { objEVENT: strPub });
    }
    else {
      this._LOG.info("::doFullScreen: topic_Publish:'bNativeSizeChange' playerId=" + strPub + " :pubMsg= " + pubMsg);
      topic_Publish(this.tMediaPlayerIE, "bNativeSizeChange", pubMsg);
    }

  }

  //-------------------------------------------------------------------------------------------
  /**
  * Method Asynchronous load player; shared by both IPVS_ClientUI and IPVS_AuthMediaClient
  * @return {jsx3.$AsyncRV} jsx3._eb/jsx3.dl
  */
  MediaPlayerIE_prototype.load = jsx3.$Y(function (cb) {
    this._LOG.info("Start to load MediaPlayer:jsxid: " + this._playerId + " .");
    // set the ActiveX Object HTML as it's text
    var playerTextHtml = this.getPlayerTextHtml(this._playerId, this._playerType, this.BlkJSX.getId());

    //    this.BlkJSX.doFullScreen = this.doFullScreen;
    //    this.BlkJSX.doKBMSEnable = this.doKBMSEnable;
    //    this.BlkJSX.doRFBEnabled = this.doRFBEnabled;
    //    this.BlkJSX.doNativeSizeChange = this.doNativeSizeChange;

    this.BlkJSX.setText(playerTextHtml, true);

    // function to check for load to complete
    var checkMediaPlayer = function () {
      var playerLoadTimer = null;
      var BlkJSXDocument = PNameSpace.getDocumentOf(this.BlkJSX);
      if (BlkJSXDocument != null) {
        this.IPVideoPlayer = BlkJSXDocument.getElementById(this._playerId);
        if (this.IPVideoPlayer != null) {
          this._LOG.info("MediaPlayer (uniqueID: " + this.IPVideoPlayer.uniqueID + ", playerId: " + this._playerId + ") is loaded .");
          //player border is "", it should be initialize as '0'
          this.IPVideoPlayer.style.border = 0;
          cb.done();
        }
        else {
          var ThisObj = this;
          // setup a timer to check for load completion
          setTimeout(checkMediaPlayer.call(ThisObj), this._IteratePlayerLoadDelay);
        }
      }
      else {
        this._LOG.error("load MediaPlayer error:" + this.BlkJSX.jsxname + " can't be loaded.");
      }
    }
    jsx3.sleep(function () {
      checkMediaPlayer.call(this);
    }, null, this);
  });

  /**
  * Method getPlayerTextHtml
  * @param playerId:an Id of a player
  * @param playerType:a type of player 
  * @param blkId:an Id of a block where player attatch to 
  * @return playerTextHtml
  */
  MediaPlayerIE_prototype.getPlayerTextHtml = function (playerId, playerType, blkId) {
    this._LOG.info("getPlayerTextHtml:jsxid: " + playerId + " :playerType :" + playerType + " :blkId :" + blkId);
    var playerTextHtml = "<table border=\"0\" cellspacing=\"0\" cellpadding=\"0\" width=\"100%\" height=\"100%\"><tr><td align=\"center\" valign=\"middle\">" +
                             "<OBJECT id=" + playerId + " codeBase=\"//player//IPVPlayerCtrl.cab#Version=3,8,2,2\" width=\'100%\' height=\'100%\' autosize=\"true\" border=\"8px\" bordercolor=\"#222\" classid=CLSID:A2A6189E-44F4-4D69-9CE4-8329312D2E3A VIEWASTEXT>" +
                             "<PARAM NAME=\"BackColor\" VALUE=\"7874560\"><PARAM NAME=\"BorderColor\" VALUE=\"0\">" +
                             "<PARAM NAME=\"AutoSize\" VALUE=\"0\"><PARAM NAME=\"AutoStart\" VALUE=\"0\">" +
                             "<PARAM NAME=\"SessionId\" VALUE=\"0\"><PARAM NAME=\"Windowless\"  VALUE=\"" + playerType + "\">" +
                             "</OBJECT><SPAN style=\"FILTER: alpha(opacity=0)\"></SPAN></td></tr></table>" +
                             "<SCRIPT for=" + playerId + " event=\"OnNativeSizeChange(a_lWidth,a_lHeight)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doNativeSizeChange(a_lWidth,a_lHeight); " +
                             "</SCRIPT>" +
                             "<SCRIPT for=" + playerId + " event=\"OnFullScreen(bFullScreen)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doFullScreen(bFullScreen); " +
                            "</SCRIPT>" +
                            "<SCRIPT for=" + playerId + " event=\"OnKBMSEnable(bKBMSEnabled)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doKBMSEnable(bKBMSEnabled); " +
                            "</SCRIPT>" +
                            "<SCRIPT for=" + playerId + " event=\"OnRFBEnable(bRFBEnabled)\" LANGUAGE=\"JavaScript\"> " +
                                 "var jsx3BridgeModel = jsx3.GO('" + blkId + "'); " +
                                 "jsx3BridgeModel.doRFBEnabled(bRFBEnabled); " +
                            "</SCRIPT>";
    return playerTextHtml;
  }

  /**
  * Method player destroy to break cycle between DOM oject and Javascript
  * @param IPVideoPlayer {ActiveX Object}  
  */
  MediaPlayerIE_prototype.destroy = function () {
    if (this.IPVideoPlayer) {
      var attributes = this.IPVideoPlayer.attributes, i, l, n;
      this._LOG.trace("MediaPlayerIE_prototype.destroy:" + attributes.length + " player attributes.");
      if (attributes) {
        l = attributes.length;
        for (i = 0; i < l; i += 1) {
          n = attributes[i].name;
          if (typeof this.IPVideoPlayer[n] === 'function') {
            this.IPVideoPlayer[n] = null;
          }
        }
      }
      this.IPVideoPlayer = null;
    }
  }

  /**
  * Method player start
  * @param StreamURL {String} 
  */
  MediaPlayerIE_prototype.start = function (StreamURL) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player Start: uniqueID: " + this.IPVideoPlayer.uniqueID + ": StreamURL: " + StreamURL + " :playerId:" + this._playerId);
      // UseHTTP
      var useHTTP = 0;
      if (typeof tApp != "undefined" && tApp) {
        useHTTP = topic_Get(tApp, "IPVSPlayerDefaultUseHTTP");
      }
      var useMMS = 0;
      // Parse Options 
      // ??? - should we be getting all this as part of URL or should some of this come from Profile?
      if (StreamURL.match(/^v2d/)) {
        // Host
        if (useHTTP == 1) {
          if (typeof tApp != "undefined" && tApp) {
            useMMS = topic_Get(tApp, "IPVSPlayerDefaultUseMMS");
          }
        }
        var StreamURLHostPart = StreamURL.replace(/\/[^\/]*$/, "");
        if (useMMS == 1) {
          if (typeof tApp != "undefined" && tApp) {
            var mmsIPAddress = topic_Get(tApp, "LoginServer");
            StreamURLHostPart = "v2d://" + mmsIPAddress;
          }
        }
        // Port
        var ServerPort = 6060;
        if (typeof tApp != "undefined" && tApp) {
          ServerPort = topic_Get(tApp, "IPVSPlayerDefaultServerPort");
        }

        if (StreamURL.match(/port=/)) {
          ServerPort = parseInt(StreamURL.replace(/^.*port=(\d+).*$/, "$1"));
        }

        // V2D set values on player ctrl
        this.IPVideoPlayer.Filename = StreamURLHostPart;
        this.IPVideoPlayer.ServerPort = ServerPort;

        //V2D authentication (?authscheme=TOKEN?authdata=47f10529-f6b7-44ee-9011-c08b942122e2")
        if (StreamURL.match(/authscheme=/) && StreamURL.match(/authdata=/)) {
          var authScheme = StreamURL.replace(/^.*authscheme=(\w+).*$/, "$1");
          if (authScheme == "TOKEN") {
            var authdata = StreamURL.replace(/^.*authdata=(\S+).*$/, "$1");
            this.IPVideoPlayer.SetAuthParameters(authScheme, authdata)
          }
        }
      }
      else if (StreamURL.match(/^http/)) {
        this.IPVideoPlayer.Filename = StreamURL;
      }
      else if (StreamURL.match(/^udp/) || StreamURL.match(/^igmp/)) {
        this.IPVideoPlayer.Filename = StreamURL.replace("://", "://@");
      }
      //common player parameters used for all types of StreamURL
      // Bandwidth
      var BandwidthKbps = 10240;
      if (typeof tApp != "undefined" && tApp) {
        BandwidthKbps = topic_Get(tApp, "IPVSPlayerDefaultBandwidthKbps");
      }
      if (StreamURL.match(/bandwidth=/)) {
        var Bandwidthbps = parseInt(StreamURL.replace(/^.*bandwidth=(\d+).*$/, "$1"));
        //"bandwidth in URL is in bps - need to convert to Kbps for player API"
        BandwidthKbps = Bandwidthbps / 1024;
      }
      // AVOption
      var AVOption = 3;
      if (typeof tApp != "undefined" && tApp) {
        AVOption = topic_Get(tApp, "IPVSPlayerDefaultAVOption");
      }
      if (StreamURL.match(/avoption=/)) {
        AVOption = StreamURL.replace(/^.*avoption=(\w+).*$/, "$1");
        switch (AVOption) {
          case "Video":
            AVOption = 1;
            break;
          case "Audio":
            AVOption = 2;
            break;
          case "Both":
            AVOption = 3;
            break;
          default:
            AVOption = 3;
            break;
        }
      }
      // Fps
      var Fps = 15;
      if (typeof tApp != "undefined" && tApp) {
        Fps = topic_Get(tApp, "IPVSPlayerDefaultFps");
      }
      if (StreamURL.match(/fps=/)) {
        Fps = StreamURL.replace(/^.*fps=(\d+).*$/, "$1");
      }
      this.saveFirstFps(Fps);
      // Multicast
      var Multicast = "false";
      if (typeof tApp != "undefined" && tApp) {
        Multicast = topic_Get(tApp, "IPVSPlayerDefaultMulticast");
      }
      if (StreamURL.match(/multicast=/)) {
        Multicast = StreamURL.replace(/^.*multicast=(\w+).*$/, "$1");
        switch (Multicast) {
          case "false":
            Multicast = 0;
            break;
          case "true":
            Multicast = 1;
            break;
          default:
            Multicast = 0;
            break;
        }
      }
      if (Multicast == 0) {
        this.IPVideoPlayer.useHTTP = useHTTP;
      } else {
        this.IPVideoPlayer.useHTTP = 0;
      }
      // SRD
      var SliceReduction = 1;
      if (typeof tApp != "undefined" && tApp) {
        SliceReduction = topic_Get(tApp, "IPVSPlayerDefaultSliceReduction");
      }
      // Enable Kbm
      var EnableKbm = "false";
      if (typeof tApp != "undefined" && tApp) {
        EnableKbm = topic_Get(tApp, "IPVSPlayerDefaultEnableKbm");
      }
      if (StreamURL.match(/enablekbm=/)) {
        EnableKbm = StreamURL.replace(/^.*enablekbm=(\w+).*$/, "$1");
        switch (EnableKbm) {
          case "false":
            EnableKbm = 0;
            break;
          case "true":
            EnableKbm = 1;
            break;
          default:
            EnableKbm = 0;
            break;
        }
      }
      // Local KBMS State
      var LocalKBMSState = 0;
      if (typeof tApp != "undefined" && tApp) {
        LocalKBMSState = topic_Get(tApp, "IPVSDefaultLocalKBMSState");
      }
      this.IPVideoPlayer.MaxBandwidth = BandwidthKbps;
      this.IPVideoPlayer.AVMode = AVOption;
      this.IPVideoPlayer.Fps = Fps;
      this.IPVideoPlayer.Multicast = Multicast;
      // Optional  
      this.IPVideoPlayer.SliceReduction = SliceReduction;
      // not used by activeX control
      this.IPVideoPlayer.EnableKbm = EnableKbm;
      this.IPVideoPlayer.LocalKBMSState = LocalKBMSState;

      //set toolbar to preserve in togglescreen::
      this.IPVideoPlayer.Settings = "<ParameterList><toolbar>Preserve</toolbar><doubleclick>Enable</doubleclick></ParameterList>"; //Disable

      // palyer priority
      var priority = "false";
      if (typeof tApp != "undefined" && tApp) {
        priority = topic_Get(tApp, "IPVSPlayerDefaultPriority");
      }
      this._LOG.info("Player play with priority: " + priority + " :uniqueID: " + this.IPVideoPlayer.uniqueID + ": fps: " + Fps);
      priority = (priority == "true") ? 1 : 0;
      this.IPVideoPlayer.Priority = priority;

      // start playback
      this.IPVideoPlayer.Play();
    }
    else {
      this._LOG.warn("start:Player not loaded.");
      return;
    }
  }

  /**
  * Method player stop
  */
  MediaPlayerIE_prototype.stop = function playerStop() {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player Stop: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
      this.IPVideoPlayer.Stop();
    }
    else {
      this._LOG.info("stop:Player not loaded.");
      return;
    }
  }

  /**
  * Method player pause
  */
  MediaPlayerIE_prototype.pause = function () {
    if (this.IPVideoPlayer) {
      if (this.IPVideoPlayer.PlayState != this.IPVSPlayerState_PAUSED) {
        this._LOG.info("Player pause: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
        this.IPVideoPlayer.Pause();
      }
    }
    else {
      this._LOG.warn("pause:Player not loaded.");
      return;
    }
  }

  /**
  * Method player resume
  */
  MediaPlayerIE_prototype.resume = function () {
    if (this.IPVideoPlayer) {
      //from bookmark playFrom
      if (this.IPVideoPlayer.PlayState == this.IPVSPlayerState_PLAYING) {
        this.IPVideoPlayer.Pause();
        this.IPVideoPlayer.Resume();
      }
      if (this.IPVideoPlayer.PlayState == this.IPVSPlayerState_PAUSED) {
        this.IPVideoPlayer.Resume();
      }
      this._LOG.info("Player resume: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
    }
    else {
      this._LOG.warn("resume:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSkipBackward
  * @param backwardSeconds {int} (millisecond)
  */
  MediaPlayerIE_prototype.skipBackward = function (backwardSeconds) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player skipBackward: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
      this.IPVideoPlayer.SkipBack(backwardSeconds);
    }
    else {
      this._LOG.warn("skipBackward:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSkipForward
  * @param forwardSeconds {int} (millisecond)
  */
  MediaPlayerIE_prototype.skipForward = function (forwardSeconds) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player skipForward: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
      this.IPVideoPlayer.SkipFwd(forwardSeconds);
    }
    else {
      this._LOG.warn("skipForward:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerCatchUp  
  * (RTP requires this for synchronizing clock with streming server,
  *  V2D has no timestamp, so it's simple)
  */
  MediaPlayerIE_prototype.catchUp = function () {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player catchup: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
      this.IPVideoPlayer.CatchUp();
    }
    else {
      this._LOG.warn("catchup:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSetFps  
  * @param Fps(frame per second) {int} 
  */
  MediaPlayerIE_prototype.setFps = function (Fps) {
    if (this.IPVideoPlayer) {
      if (this.IPVideoPlayer.PlayState == this.IPVSPlayerState_PLAYING) {
        this._LOG.info("Player SetFps: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId + ": Fps: " + Fps);
        this.IPVideoPlayer.Fps = Fps;
      }
    }
    else {
      this._LOG.warn("setFps:Player not loaded.");
      return;
    }
  }

  /**
  * Method setTrickMode  
  * @param TrickMode {int} 
  */
  MediaPlayerIE_prototype.setTrickMode = function (TrickMode) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player setTrickMode: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId + ": TrickMode: " + TrickMode);
      this.IPVideoPlayer.SetTrickMode(TrickMode);
    }
    else {
      this._LOG.warn("setTrickMode:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSetLocalKBMSState  
  * @param lKBMSState {int} 
  */
  MediaPlayerIE_prototype.setLocalKBMSState = function (lKBMSState) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player setLocalKBMSState: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId + ":lKBMSState:" + lKBMSState);
      this.IPVideoPlayer.LocalKBMSState = lKBMSState;
    }
    else {
      this._LOG.warn("setLocalKBMSState:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSetFocus  
  */
  MediaPlayerIE_prototype.setFocus = function () {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player setFocus: uniqueID: " + this.IPVideoPlayer.uniqueID);
      this.IPVideoPlayer.focus();
    }
    else {
      this._LOG.warn("setFocus:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerGetKBMSState to check KBMSState is success or not
  */
  MediaPlayerIE_prototype.getKBMSState = function () {
    if (this.IPVideoPlayer) {
      return this.IPVideoPlayer.IsKBMSSuccess;
    }
    else {
      this._LOG.warn("getKBMSState:Player not loaded.");
      return 0;
    }
  }

  /**
  * Method playerMute to set Player mute ON or OFF
  * @param muteType {int} 
  */
  MediaPlayerIE_prototype.mute = function (muteType) {
    if (this.IPVideoPlayer) {
      this.IPVideoPlayer.Mute = muteType;
    }
    else {
      this._LOG.warn("mute:Player not loaded.");
      return;
    }
  }

  /**
  * Method remoteKBMSState 
  * HACK: playerID is being passed as 0.
  * - for now this is only used in tView
  * @param PlayerID {String} 
  * @param rKBMSState {int} 
  */
  MediaPlayerIE_prototype.remoteKBMSState = function (PlayerID, rKBMSState) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player Callback: remoteKBMSState: " + rKBMSState);
      //topic_Publish(PlayerID, "PlayerEvent", "REMOTESTATE");
      topic_Publish(tView, "PlayerEvent", "REMOTESTATE");
    }
    else {
      this._LOG.warn("remoteKBMSState:Player not loaded.");
      return;
    }
  }

  /**
  * Method ChangeDisplayMode 
  * @return {boolean}
  */
  MediaPlayerIE_prototype.changeDisplayMode = function () {
    if (this.IPVideoPlayer) {
      return true;
    }
    else {
      this._LOG.warn("changeDisplayMode:Player not loaded.");
      return false;
    }
  }

  /**
  * Method playerToggleDisplay 
  * @return {boolean}
  */
  MediaPlayerIE_prototype.toggleDisplay = function () {
    if (this.IPVideoPlayer) {
      try {
        this._LOG.info("Player toggleDisplay: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._playerId);
        this.IPVideoPlayer.ToggleDisplay();
      }
      catch (er) {
        return false;
      }
      return true;
    }
    else {
      this._LOG.warn("toggleDisplay:Player not loaded.");
      return false;
    }
  }

  /**
  * Method playerGetNativeSize 
  * @return {Object}
  */
  MediaPlayerIE_prototype.getNativeSize = function () {
    if (this.IPVideoPlayer) {
      this.nativeSize.width = this.IPVideoPlayer.GetNativeWidth();
      this.nativeSize.height = this.IPVideoPlayer.GetNativeHeight();
      if (isNaN(this.nativeSize.width) || (this.nativeSize.width <= 0) || (this.nativeSize.width == '')) {
        this.nativeSize.width = 0;
      }
      if (isNaN(this.nativeSize.height) || (this.nativeSize.height <= 0) || (this.nativeSize.height == '')) {
        this.nativeSize.height = 0;
      }
      return this.nativeSize;
    }
    else {
      this._LOG.warn("getNativeSize:Player not loaded.");
      return null;
    }
  }

  /**
  * Method playerSetNativeSize 
  */
  MediaPlayerIE_prototype.setNativeSize = function () {
    if (this.IPVideoPlayer) {
      this.getNativeSize();
      this._LOG.info("setNativeSize:Player(" + this.IPVideoPlayer.uniqueID + "):native width: " + this.nativeSize.width + ":nativeSize.height: " + this.nativeSize.height);
      if (this.nativeSize.width <= 0 || this.nativeSize.height <= 0) {
        //Fix bug 30225. Not able to come out of full screen mode for blank video player;
        //Analysis: The player is going to "0" size in this case. Before going to 1:1 mode,
        //validate the nativeSize.width and height. If any of two <= '0', then do not resize the container.
        return;
      }
      //set the outer DIV size to actual video value
      this.IPVideoPlayer.style.width = this.nativeSize.width;
      this.IPVideoPlayer.style.height = this.nativeSize.height;
      //      this.IPVideoPlayer.ShowNativeVideoResolution();
    }
    else {
      this._LOG.warn("setNativeSize:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSetScaleToWindow 
  */
  MediaPlayerIE_prototype.setScaleToWindow = function () {
    if (this.IPVideoPlayer) {
      this._LOG.info("setScaleToWindow:Player(" + this.IPVideoPlayer.uniqueID + ")");
      //set the outer DIV size to actual video value
      this.IPVideoPlayer.style.width = "100%";
      this.IPVideoPlayer.style.height = "100%";
      // set player 'border'
      this.IPVideoPlayer.style.border = 0;
      if (typeof tApp != "undefined" && tApp) {
        this.IPVideoPlayer.style.border = topic_Get(tApp, "IPVSPlayerDefaultBorder");
      }
    }
    else {
      this._LOG.warn("setScaleToWindow:Player not loaded.");
      return;
    }
  }

  /**
  * Method playerSnapShot 
  * @return {boolean}
  */
  MediaPlayerIE_prototype.snapShot = function () {
    if (this.IPVideoPlayer) {
      try {
        this._LOG.info("Player snapShot: uniqueID: " + this.IPVideoPlayer.uniqueID);
        this.IPVideoPlayer.Snapshot("", "");
      }
      catch (er) {
        return false;
      }
      return true;
    }
    else {
      this._LOG.warn("snapShot:Player not loaded.");
      return false;
    }
  }

  /**
  * Method playerSetPriority:set player priority [true|false]:[normal|low] 
  * @param priority {boolean} 
  */
  MediaPlayerIE_prototype.setPriority = function (priority) {
    if (this.IPVideoPlayer) {
      this._LOG.info("Player setPriority: uniqueID: " + this.IPVideoPlayer.uniqueID + ": Priority: " + priority);
      this.IPVideoPlayer.Priority = priority;
    }
    else {
      this._LOG.warn("setPriority:Player not loaded.");
      return;
    }
  }

  /**
  * Method setTopic 
  * @param tMediaPlayerIE {string} 
  */
  MediaPlayerIE_prototype.setTopic = function (tMediaPlayerIE) {
    this.tMediaPlayerIE = tMediaPlayerIE;
    //stash player context properties to its model for call back functions usage
    this.BlkJSX.tMediaPlayerIE = this.tMediaPlayerIE;
    this.BlkJSX._LOG = this._LOG;
    this.BlkJSX._playerId = this._playerId;
    this.BlkJSX.doFullScreen = this.doFullScreen;
    this.BlkJSX.doKBMSEnable = this.doKBMSEnable;
    this.BlkJSX.doRFBEnabled = this.doRFBEnabled;
    this.BlkJSX.doNativeSizeChange = this.doNativeSizeChange;
  }

  /**
  * Method getTopic
  * @return string 
  */
  MediaPlayerIE_prototype.getTopic = function () {
    return this.tMediaPlayerIE;
  }
});
