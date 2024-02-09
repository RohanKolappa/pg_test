/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * StreamPlayer - Class definition 
 * - registered as a jQuery plugin
 */
var StreamPlayer = Class.extend({
    init: function (options, elem) {
        // Mix in the passed-in options with the default options
        this.options = jQuery.extend({}, this._defaults, options);
        this.log(bDEBUG, "init", "StreamPlayer", "");
        // Init state
        this.state = {
            isLoaded: false
        };
        this.IPVideoPlayer = null;
        this.nativeSize = {};
        this.isNativeRes = false;
        // Save the element reference, both as a jQuery
        // reference and a normal reference
        this.elem = elem;
        this.$elem = jQuery(elem);

        // function to check for load to complete
        var thisObj = this;
        var checkPlayer = function () {
            // try to get a handle to the player HTML object
            thisObj.player = document.getElementById(thisObj.options.playerId)
            // if handle is valid = load complete = fire callback
            if (thisObj.player) {
                // do first time load actions
                thisObj.state.isLoaded = true;
                //Event onLoad
                jQuery.publish("onLoad", { id: thisObj.options.divId, playerId: thisObj.options.playerId, type: "Event", message: 'On Load' });
            }
            else {
                // if div is still valid but player not ready - setTimeout and repeat - till max retries
                if (thisObj.$elem.find('.StreamPlayer') && this.options.playerCheckMaxRetry--) {
                    setTimeout(function () { checkPlayer.call(thisObj); }, thisObj.options.playerCheckTimeout);
                }
                else {
                    //raise up and handle player loading error
                    var errObj = { id: thisObj.options.divId, playerId: thisObj.options.playerId, type: "Player", message: 'Fail to Load ActiveX Object' };
                    jQuery.publish("onError", errObj);
                }
            }
        }
        this.checkPlayer = checkPlayer;
        // Build the DOM's initial structure
        this._build();
    },
    _defaults: {
        playerURL: '',
        playerCheckTimeout: 100,
        playerCheckMaxRetry: 5
    },
    _build: function () {
        this.log(bDEBUG, "_build", "StreamPlayer", "");
        // This method should be overridden by sub classes
    },
    destroy: function (pluginName) {
        this.log(bDEBUG, "destroy", "StreamPlayer call", pluginName);
        // unsubscribe to events

        // clean out DOM structure
        // destroy self
        this.$elem.find('.StreamPlayer').remove();
        this.$elem.removeData(pluginName);
        this.IPVideoPlayer = null;
        this.log(bDEBUG, "destroy", "StreamPlayer return", pluginName);
    },
    play: function (streamURL) {
        this.log(bDEBUG, "play", "StreamPlayer", "");
        // This method should be overridden by sub classes
    },
    doResize: function (subWidth, subHeight) {
        this.log(bDEBUG, "doResize", "StreamPlayer", subWidth + ', ' + subHeight);
        var width = (typeof subWidth === 'number') ? (subWidth + "px") : subWidth;
        var height = (typeof subHeight === 'number') ? (subHeight + "px") : subHeight;
        this.player.css({ 'width': width, 'height': height });
        this.log(bDEBUG, "doResize", "super StreamPlayer",
          "width " + width + ", height:" + height);
    }
});

// StreamPlayerIPV - an object that extends/inherit from StreamPlayer
var StreamPlayerIPV = StreamPlayer.extend({
  init: function (options, elem) {
    // extend the defaults
    this.log(bDEBUG, "init", "call", "");
    this._defaults = jQuery.extend(this._defaults, {
      playerVersion: '%PLAYER.VERSION2%',
      Fps: 10,
      useHTTP: true,
      useMMS: 0,
      ServerPort: 6060,
      EnableKbm: false,
      Multicast: false,
      BandwidthKbps: 10240,
      AVOption: 3,
      Settings: '<ParameterList><toolbar>Preserve</toolbar><doubleclick>Disable</doubleclick><shortcutkeysforkbm>Disable</shortcutkeysforkbm></ParameterList>',
      SliceReduction: 1,
      Priority: false,
      LocalKBMSState: 0,
      authscheme: 'TOKEN',
      authdata: ''
    });
    this.isNativeRes = false;
    // call the base method
    this._super(options, elem);
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    // override base method.
    // insert an IPV stream player object tags
    var child = jQuery('<div id="' + this.options.divId + '_objectdiv" class="objectdiv" style="position: absolute; display: block; width: 100%; top: 0px;">' +
      '<div id="' + this.options.playerId + '" style="display: block; width: 100%; height: 100%;">' +
      '  <OBJECT id="' + this.options.playerId + '_emb" codeBase=../streamplayers/ipvplayer/IPVPlayerCtrl.cab#Version=' + this.options.playerVersion +
      '    classid=CLSID:A2A6189E-44F4-4D69-9CE4-8329312D2E3A autosize="true" style="width:100%; height:100%; margin:auto; top:0; left:0; bottom:0; right:0"> ' +
      '    <PARAM NAME=\"BackColor\" VALUE=\"7874560\"><PARAM NAME=\"BorderColor\" VALUE=\"0\"><PARAM NAME=\"AutoSize\" VALUE=\"0\"> ' +
      '    <PARAM NAME=\"AutoStart\" VALUE=\"0\"><PARAM NAME=\"SessionId\" VALUE=\"0\"><PARAM NAME=\"Windowless\"  VALUE=\"1\"> ' +
      '  </OBJECT>' +
      '</div></div> '
      ).addClass('StreamPlayer');
    this.$elem.append(child);

    // subscribe to player callbacks
    //jQuery.subscribe("onFullscreen", this._setFullscreenStatus.call(this, this.$elem));

    // call the load checker - do in sleep to increase chance of success on first try
    if (!this.player) {
      var thisObj = this;
      setTimeout(function () { thisObj.checkPlayer.call(thisObj); }, thisObj.options.playerCheckTimeout);
    }
    jQuery.subscribe("onLoad", jQuery.proxy(this.checkMediaPlayer, this));
    this.log(bINFO, "_build", "return", "Done");
  },
  getRendered: function() {
    this.log(bDEBUG, "getRendered", "call", "");
    return this.elem;
  },
  checkMediaPlayer: function(evt) {
    this.log(bDEBUG, "checkMediaPlayer", "call", "{id:" + evt.id + ", playerId:" + evt.playerId + ", type:" + evt.type + ", message:" + evt.message + "}");
    if (this.player) {
      this.IPVideoPlayer = document.getElementById(this.options.playerId + "_emb");
      this.objectdiv = document.getElementById(this.options.divId + "_objectdiv");
    }
    jQuery.unsubscribe("onLoad", jQuery.proxy(this.checkMediaPlayer, this));
  },
  setup: function () {
    this.log(bDEBUG, "setup", "call", "");
    //this.player.Fps = this.options.Fps;
    //this.player.useHTTP = this.options.useHTTP;
    //this.setScaleToWindow();
    //compare two versions from activeX and configuration
    var currVerionArr = this.player.Version.split('.');
    var confVerionArr = this.options.playerVersion.split('.');
    if (currVerionArr[0] !== confVerionArr[0] ||
		  currVerionArr[1] !== confVerionArr[1] ||
		  currVerionArr[2] !== confVerionArr[2] ||
		  currVerionArr[3] !== confVerionArr[3]) {
      //	jQuery.publish("onError", { id: this.optionsdivId, playerId: this.optionsplayerId, type: "Player", message: 'Player version config error: run time version different with the config one. ' });
    }
  },
  setFocus: function() {
    this.log(bDEBUG, "setFocus", "call", "");
    if (this.IPVideoPlayer) {
      this.log(bINFO, "setFocus", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID);
      this.IPVideoPlayer.focus();
    }
    else {
      this.log(bWARNING, "setFocus", "block", "Player not loaded.");
      return;
    }
  },
  setFps: function(Fps) {
    this.log(bDEBUG, "setFps", "call", Fps);
    if (this.IPVideoPlayer) {
      if (this.IPVideoPlayer.PlayState == StreamPlayerIPV.IPVSPlayerState_PLAYING) {
        this.log(bINFO, "setFps", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId + ": Fps: " + Fps);
        this.IPVideoPlayer.Fps = Fps;
      }
    }
    else {
      this.log(bWARNING, "setFps", "block", "Player not loaded.");
      return;
    }
  },
  setPriority: function(priority) {
    this.log(bDEBUG, "setPriority", "call", priority);
    if (this.IPVideoPlayer) {
      this.log(bINFO, "setPriority", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + ": Priority: " + priority);
      this.IPVideoPlayer.Priority = priority;
    }
    else {
      this.log(bWARNING, "setPriority", "block", "Player not loaded.");
      return;
    }
  },
  start: function(StreamURL) {
    this.log(bDEBUG, "start", "call", StreamURL);
    if (this.IPVideoPlayer) {
      this.$elem.player('doEventPublish', 'onStreamPlayerReady', {});
      //get URL when session started
      this.log(bINFO, "start", "block",
        "StreamURL: " + StreamURL + " :uniqueID: " + this.IPVideoPlayer.uniqueID);
      // UseHTTP
      var useHTTP = this.options.useHTTP;
      var useMMS = this.options.useMMS;
      // Parse Options 
      if (StreamURL.match(/^v2d/)) {
        // Host
        if (useHTTP === 1) {
          useMMS = this.options.useHTTP;
        }
        var StreamURLHostPart = StreamURL.replace(/\/[^\/]*$/, "");
        if (useMMS === 1) {//TBD
          // var mmsIPAddress = com.ipvs.agent.ServiceAgent.LOGIN_SERVER);
          // StreamURLHostPart = "v2d://" + mmsIPAddress;
        }

        // Port
        var ServerPort = this.options.serverPort;
        if (StreamURL.match(/port=/)) {
          ServerPort = parseInt(StreamURL.replace(/^.*port=(\d+).*$/, "$1"));
        }

        // V2D set values on player ctrl
        this.IPVideoPlayer.Filename = StreamURLHostPart;
        this.IPVideoPlayer.ServerPort = ServerPort;

        //V2D authentication (?authscheme=TOKEN?authdata=47f10529-f6b7-44ee-9011-c08b942122e2")
        if (StreamURL.match(/authscheme=/) && StreamURL.match(/authdata=/)) {
          var authScheme = StreamURL.replace(/^.*authscheme=(\w+).*$/, "$1");
          if (authScheme === "TOKEN") {
            var authdata = StreamURL.replace(/^.*authdata=(\S+).*$/, "$1");
            this.IPVideoPlayer.SetAuthParameters(authScheme, authdata); //Not supported??
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
      var BandwidthKbps = this.options.BandwidthKbps;
      if (StreamURL.match(/bandwidth=/)) {
        var Bandwidthbps = parseInt(StreamURL.replace(/^.*bandwidth=(\d+).*$/, "$1"));
        //"bandwidth in URL is in bps - need to convert to Kbps for player API"
        BandwidthKbps = Bandwidthbps / 1024;
      }
      // AVOption
      var AVOption = this.options.AVOption;
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
      var Fps = this.options.Fps;
      if (StreamURL.match(/fps=/)) {
        Fps = StreamURL.replace(/^.*fps=(\d+).*$/, "$1");
      }
      // Multicast
      var Multicast = this.options.Multicast;
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
      if (Multicast === 0) {
        this.IPVideoPlayer.useHTTP = useHTTP;
      } else {
        this.IPVideoPlayer.useHTTP = 0;
      }
      // SRD
      var SliceReduction = this.options.SliceReduction;
      // Enable Kbm
      var EnableKbm = this.options.EnableKbm;
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
      var LocalKBMSState = this.options.LocalKBMSState;
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
      this.IPVideoPlayer.Settings = this.options.Settings;

      // player priority
      var Priority = this.options.Priority;
      this.log(bDEBUG, "start", "block",
        "priority: " + Priority + " :uniqueID: " + this.IPVideoPlayer.uniqueID + ": fps: " + Fps);
      Priority = (Priority === "true") ? 1 : 0;
      this.IPVideoPlayer.Priority = Priority;

      // And finally play
      this.IPVideoPlayer.Play();
    }
    else {
      this.log(bWARNING, "start", "block", "Player not loaded.");
      return;
    }
  },
  play: function(StreamURL) {
    this.log(bDEBUG, "play", "call", "");
    this.start(StreamURL);
  },
  stop: function () {
    this.log(bDEBUG, "stop", "call", "");
    if (this.IPVideoPlayer) {
      this.log(bINFO, "stop", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
      if (this.IPVideoPlayer.PlayState == StreamPlayerIPV.IPVSPlayerState_PLAYING || this.IPVideoPlayer.PlayState == StreamPlayerIPV.IPVSPlayerState_PAUSED) {
        this.IPVideoPlayer.Stop();
      }
    }
    else {
      this.log(bINFO, "stop", "block","Player not loaded.");
      return;
    }
  },
  pause: function() {
    this.log(bDEBUG, "pause", "call", "");
    if (this.IPVideoPlayer) {
      if (this.IPVideoPlayer.PlayState != StreamPlayerIPV.IPVSPlayerState_PAUSED) {
        this.log(bINFO, "pause", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
        this.IPVideoPlayer.Pause();
      }
    }
    else {
      this.log(bWARNING, "pause", "block", "Player not loaded.");
      return;
    }
  },
  resume: function() {
    this.log(bDEBUG, "resume", "call", "");
    if (this.IPVideoPlayer) {
      //from bookmark playFrom
      if (this.IPVideoPlayer.PlayState == StreamPlayerIPV.IPVSPlayerState_PLAYING) {
        this.IPVideoPlayer.Pause();
        this.IPVideoPlayer.Resume();
      }
      if (this.IPVideoPlayer.PlayState == StreamPlayerIPV.IPVSPlayerState_PAUSED) {
        this.IPVideoPlayer.Resume();
      }
      this.log(bINFO, "resume", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
    }
    else {
      this.log(bWARNING, "resume", "block", "Player not loaded.");
      return;
    }
  },
  skipBackward: function(backwardSeconds) {
    this.log(bDEBUG, "skipBackward", "call", "");
    if (this.IPVideoPlayer) {
      this.log(bINFO, "skipBackward", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
      this.IPVideoPlayer.SkipBack(backwardSeconds);
    }
    else {
      this.log(bWARNING, "skipBackward", "block", "Player not loaded.");
      return;
    }
  },
  skipForward: function(forwardSeconds) {
    this.log(bDEBUG, "skipForward", "call", "");
    if (this.IPVideoPlayer) {
      this.log(bINFO, "skipForward", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
      this.IPVideoPlayer.SkipFwd(forwardSeconds);
    }
    else {
      this.log(bWARNING, "skipForward", "block", "Player not loaded.");
      return;
    }
  },
  catchUp: function() {
    this.log(bDEBUG, "catchUp", "call", "");
    if (this.IPVideoPlayer) {
      this, log(bINFO, "catchUp", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
      this.IPVideoPlayer.CatchUp();
    }
    else {
      this.log(bWARNING, "catchUp", "block", "Player not loaded.");
      return;
    }
  },
  _mute: function(muteType) {
    this.log(bDEBUG, "_mute", "call", muteType);
    if (this.IPVideoPlayer) {
      this.IPVideoPlayer.Mute = muteType;
    }
    else {
      this.log(bWARNING, "_mute", "block", "Player not loaded.");
      return;
    }
  },
  mute: function() {
    this.log(bDEBUG, "mute", "call", "");
    this._mute(true);
  },
  unmute: function() {
    this.log(bDEBUG, "unmute", "call", "");
    this._mute(false);
  },
  isMuted: function() {
    this.log(bDEBUG, "isMuted", "call", "");
    if (this.player)
      return this.IPVideoPlayer.Mute;
    else
      return false;
  },
  restoreFPS: function() {
    this.log(bDEBUG, "restoreFPS", "call", "");
    if (!this.IPVideoPlayer)
      return false;
    this.IPVideoPlayer.Fps = this.options.Fps;
    return true;
  },
  setLocalKBMSState: function(lKBMSState) {
    this.log(bDEBUG, "setLocalKBMSState", "call", lKBMSState);
    if (this.IPVideoPlayer) {
      this.log(bINFO, "setLocalKBMSState", "block", "lKBMSState:" + lKBMSState + " uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
      this.IPVideoPlayer.LocalKBMSState = lKBMSState;
      if (lKBMSState) this.IPVideoPlayer.focus();
    }
    else {
      this.log(bWARNING, "setLocalKBMSState", "block", "Player not loaded.");
      return;
    }
  },
  getKBMSState: function() {
    this.log(bDEBUG, "getKBMSState", "call", "");
    if (this.IPVideoPlayer) {
      return this.IPVideoPlayer.IsKBMSSuccess;
    }
    else {
      this.log(bWARNING, "getKBMSState", "block", "Player not loaded.");
      return 0;
    }
  },
  isKBMEnabled: function() {
    this.log(bDEBUG, "isKBMEnabled", "call", "");
    return this.getKBMSState();
  },
  setNativeSize: function() {
    this.log(bDEBUG, "setNativeSize", "call", "");
    if (this.IPVideoPlayer) {
      this.getNativeSize();
      this.log(bDEBUG, "setNativeSize", "block", "setNativeSize:Player(" + this.IPVideoPlayer.uniqueID + "):native width: " + this.nativeSize.width + ":nativeSize.height: " + this.nativeSize.height);
      if (this.nativeSize.width <= 0 || this.nativeSize.height <= 0) {
        //Fix bug 30225. Not able to come out of full screen mode for blank video player;
        //Analysis: The player is going to "0" size in this case. Before going to 1:1 mode,
        //validate the nativeSize.width and height. If any of two <= '0', then do not resize the container.
        return;
      }
      //set the outer DIV size to actual video value
      this.objectdiv.style.overflow = 'auto';
      this.IPVideoPlayer.style.width = this.nativeSize.width.toString() + 'px';
      this.IPVideoPlayer.style.height = this.nativeSize.height.toString() + 'px';
      //      this.IPVideoPlayer.ShowNativeVideoResolution();
      this.isNativeRes = true;
    }
    else {
      this.log(bWARNING, "setNativeSize", "block", "Player not loaded.");
      return;
    }
  },
  getNativeSize: function() {
    this.log(bDEBUG, "setNativeSize", "call", "");
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
      this.log(bWARNING, "setNativeSize", "block", "Player not loaded.");
      return null;
    }
  },
  toggleDisplay: function() {
    this.log(bDEBUG, "toggleDisplay", "call", "");
    if (this.IPVideoPlayer) {
      try {
        this.log(bINFO, "toggleDisplay", "block", "Player toggleDisplay: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this.options.playerId);
        this.IPVideoPlayer.ToggleDisplay();
      }
      catch (er) {
        return false;
      }
      return true;
    }
    else {
      this.log(bWARNING, "toggleDisplay", "block", "Player not loaded.");
      return false;
    }
  },
  setScaleToZoom: function(zoomSize) {
    this.log(bDEBUG, "setScaleToZoom", "call", zoomSize);
    if (this.IPVideoPlayer) {
      this.getNativeSize();
      this.log(bINFO, "setScaleToZoom", "block", "Player(" + this.IPVideoPlayer.uniqueID + "):native width: " + this.nativeSize.width + ":nativeSize.height: " + this.nativeSize.height);
      if (this.nativeSize.width <= 0 || this.nativeSize.height <= 0) {
        return;
      }
      if (jQuery.isBlank(zoomSize))
        zoomSize = 100;
      //set the outer DIV size to actual video value
      this.objectdiv.style.overflow = 'auto';
      this.IPVideoPlayer.style.width = ((this.nativeSize.width * zoomSize) / 100).toString() + 'px';
      this.IPVideoPlayer.style.height = ((this.nativeSize.height * zoomSize) / 100).toString() + 'px';
      // set player 'border'
      this.IPVideoPlayer.style.border = '0'; // should be from preferences
      /* My attempt to do vertical centering based on this article
       * http://www.smashingmagazine.com/2013/08/09/absolute-horizontal-vertical-centering-css/
       * Question is how to deal with dynamic resize.
       */
      if (this.IPVideoPlayer.offsetHeight < this.objectdiv.offsetHeight) {
        this.IPVideoPlayer.style.position = 'absolute';
      } else {
        this.IPVideoPlayer.style.position = '';
      }
      this.isNativeRes = (zoomSize === 100);
    }
    else {
      this.log(bWARNING, "setScaleToZoom", "block", "Player not loaded.");
      return;
    }
  },
  setScaleToWindow: function() {
    this.log(bDEBUG, "setScaleToWindow", "call", "");
    if (this.IPVideoPlayer) {
      this.log(bINFO, "setScaleToWindow", "block", "Player(" + this.IPVideoPlayer.uniqueID + ")");
      //set the outer DIV size to actual video value
      this.objectdiv.style.overflow = 'visible';
      this.IPVideoPlayer.style.width = "100%";
      this.IPVideoPlayer.style.height = "100%";
      // set player 'border'
      this.IPVideoPlayer.style.border = '0'; // should be from preferences
      this.isNativeRes = false;
    }
    else {
      this.log(bWARNING, "setScaleToWindow", "block", "Player not loaded.");
      return;
    }
  },
  snapShot: function(path) {
    this.log(bDEBUG, "snapShot", "call", path);
    if (this.IPVideoPlayer) {
      try {
        this.log(bINFO, "snapShot", "block", "uniqueID: " + this.IPVideoPlayer.uniqueID);
        this.IPVideoPlayer.Snapshot(path, "");
      }
      catch (er) {
        return false;
      }
      return true;
    }
    else {
      this.log(bWARNING, "snapShot", "block", "Player not loaded.");
      return false;
    }
  },
  zoom: function(currentZoomX, currentZoomY) {
    this.log(bDEBUG, "zoom", "call", currentZoomX + ", " + currentZoomY);
    if (!this.IPVideoPlayer) {
      return false;
    }
    //get s pairs(2 dimensions) of zoom settings
    var lastZoomX = this.$elem.width();
    var lastZoomY = this.$elem.height();
    //dimensions before zooming
    var blkWinHtmlElement = this.getRendered();
    var blkWinWidth = blkWinHtmlElement.clientWidth;
    var blkWinHeight = blkWinHtmlElement.clientHeight;
    var blkWinScrollLeft = blkWinHtmlElement.scrollLeft;
    var blkWinScrollTop = blkWinHtmlElement.scrollTop;
    // calulate current center offset
    var offX = parseInt(blkWinScrollLeft) + (blkWinWidth / 2);
    var offY = parseInt(blkWinScrollTop) + (blkWinHeight / 2);
    // calulate new scrollLeft/scrollTop
    blkWinScrollLeft = (offX * parseInt(currentZoomX) / parseInt(lastZoomX)) - (blkWinWidth / 2);
    blkWinScrollTop = (offY * parseInt(currentZoomY) / parseInt(lastZoomY)) - (blkWinHeight / 2);
    this.blkPlayer.setWidth(currentZoomX, true);
    this.blkPlayer.setHeight(currentZoomY, true);
    blkWinHtmlElement.scrollLeft = blkWinScrollLeft;
    blkWinHtmlElement.scrollTop = blkWinScrollTop;
    return {zoomX: currentZoomX, zoomY: currentZoomY, scrollLeft: blkWinScrollLeft, scrollTop: blkWinScrollTop};
  },
  pan: function(blkWinScrollLeft, blkWinScrollTop) {
    this.log(bDEBUG, "pan", "call", blkWinScrollLeft + ", " + blkWinScrollTop);
    if (!this.IPVideoPlayer) {
      return false;
    }
    //dimensions before zooming
    var blkWinHtmlElement = this.getRendered();
    blkWinHtmlElement.scrollLeft = blkWinScrollLeft;
    blkWinHtmlElement.scrollTop = blkWinScrollTop;
    return true;
  },
  changeDisplayMode: function() {
    this.log(bDEBUG, "changeDisplayMode", "call", "");
    if (this.IPVideoPlayer) {
      return true;
    }
    else {
      this.log(bWARNING, "changeDisplayMode", "block", "Player not loaded.");
      return false;
    }
  },
  isNative: function () {
    this.log(bDEBUG, "isNative", "call", "");
    return this.isNativeRes;
  },
  isRFBReady: function() {
    this.log(bDEBUG, "IsRFBReady", "call", "");
    if (this.IPVideoPlayer) {
      return this.IPVideoPlayer.IsRFBReady;
    }
    else {
      this.log(bWARNING, "IsRFBReady", "block", "Player not loaded.");
      return false;
    }
  },
  enableKbm: function() {
    this.log(bDEBUG, "IsRFBReady", "call", "");
    if (this.IPVideoPlayer) {
      return this.IPVideoPlayer.EnableKbm;
    }
    else {
      this.log(bWARNING, "EnableKbm", "block", "Player not loaded.");
      return false;
    }
  }
});

/**
 * Static constant varibles for player modes
 */
StreamPlayerIPV.WINDOWLESS_MODE_PLAYER = 1;
StreamPlayerIPV.WINDOW_MODE_PLAYER = 0;

/**
 * {ActiveX object} media player state 
 */
StreamPlayerIPV.LOWEST_FPS = 1;
StreamPlayerIPV.DEFAULT_FPS = 15;
StreamPlayerIPV.IPVSPlayerState_PAUSED = 1;
StreamPlayerIPV.IPVSPlayerState_PLAYING = 2;

// expose Objects as jQuery plugins
jQuery.plugin('streamPlayerIPV', StreamPlayerIPV);
