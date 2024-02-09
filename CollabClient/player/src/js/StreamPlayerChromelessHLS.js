/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/*
 * 
 * Plug-ins and add-ons can only call functions in the global context. So, we
 * need to setup a set of trampoline functions to get back into the class
 * instance context where our player state is maintained.
 */

function wspOnPlayerReady(objId) {
  var flashPlayer = null;
  if (typeof window.ActiveXObject === 'undefined') {
    if (document.embeds && document.embeds[objId]) {
      flashPlayer = document.embeds[objId];
    }
  } else {
    flashPlayer = document.getElementById(objId);
  }
  if (flashPlayer) {
    // WebStreamPlayer.swf wants callback registration within the same
    // call stack overwise it complaints about no callbacks registered.
    flashPlayer.registerCallback("OnStateChanged", "barco_wspOnStateChanged");
    flashPlayer.registerCallback("OnCursorChange", "barco_wspOnCursorChange");
    flashPlayer.registerCallback("OnStreamComplete", "barco_wspOnStreamComplete");
    flashPlayer.registerCallback("OnError", "barco_wspOnError");
    window.setTimeout(function () {
      jQuery.publish("OnPlayerReady_" + objId, {});
    }, 0);
  } else {
    barco_wspOnError("OnError_" + objId, { message: "Callback registration failed" });
  }
}

function barco_wspOnStateChanged(objId, state) {
  window.setTimeout(function () {
    jQuery.publish("OnStateChanged_" + objId, { state: state });
  }, 0);
}

function barco_wspOnCursorChange(objId, position) {
  window.setTimeout(function () {
    jQuery.publish("OnCursorChange_" + objId, position);
  }, 0);
}

function barco_wspOnStreamComplete(objId) {
  window.setTimeout(function () {
    jQuery.publish("OnStreamComplete_" + objId, "OnStreamComplete");
  }, 0);
}

function barco_wspOnError(objId, message) {
  window.setTimeout(function () {
    jQuery.publish("OnError_" + objId, { message: message });
  }, 0);
}

/*
 * StreamPlayerChromelessHLS - Class definition 
 * - registered as a jQuery plugin
 */

var StreamPlayerChromelessHLS = StreamPlayer.extend({
  init: function (options, elem) {
    // extend the defaults
    this.log(bDEBUG, "init", "call", "");
    this._super(options, elem);
    // find the URL to this script
    var scripts = document.getElementsByTagName("script");
    var scriptURL = null;
    for (var i = 0; i < scripts.length; i++) {
      var src = scripts[i].src;
      if (src.indexOf('StreamPlayerChromelessHLS') > 0) {
        scriptURL = src;
      }
    }
    // derive SWF URL from our URL path. We cannot rely on a
    // relative path or the base path.
    if (scriptURL) {
      var parts = scriptURL.split('/js/StreamPlayerChromelessHLS');
      this.state.swfChromelessURL = parts[0] +
        '/streamplayers/chromelessplayer/WebStreamPlayer.swf'
    } else { // fallback case
      this.state.swfChromelessURL = './streamplayers/chromelessplayer/WebStreamPlayer.swf';
    }
    this.state.buildingInnerPlayerDiv = false;
    this.state.playRequested = false;
    this.state.idleAfterPlaying = false;
    this.state.isVideoPlayerReady = false;
    this.state.unloadRequested = false;
    this.state.isPlaying = false;
    this.state.isPaused = false;
    this.state.volume = 100;
    this.state.muted = (this.state.volume === 0);
    this.state.actionRequested = null;
    jQuery.subscribe("onLoad", jQuery.proxy(this._buildSucceed, this));
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    // override base method.
    this.state.initialBuildPending = true;
    // Temporary until I'm certain this works across all devices; should
    // move permanently into EmbeddedPlayer.css once confident this works.
    //        this.options.width = '100%';
    //        this.options.height = '100%';
    //        var cssWidth = (typeof this.options.width === 'number') ?
    //          (this.options.width + "px") : this.options.width;
    //        var cssHeight = (typeof this.options.height === 'number') ?
    //          (this.options.height + "px") : this.options.height;
    var playerHTML = '<div id="' + this.options.divId + '_objectdiv" ' +
      'class="objectdiv StreamPlayer" ' +
      'style="position: absolute; display: block; width: 100%; top: 0px;">';
    playerHTML += '<div id="' + this.options.playerId +
      '" style="display: block; width: 100%; height: 100%;"></div></div>';
    this.$elem.append(playerHTML);
    if (!this.player) {
      var thisObj = this;
      setTimeout(function () {
        thisObj.checkPlayer.call(thisObj);
      }, thisObj.options.playerCheckTimeout);
    }
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", "");
    var inner = this._getInner();
    if (inner != null) {
      this._destroyInnerBegin();
    }
    this._super(pluginName);
    this.log(bDEBUG, "destroy", "return", "");
  },
  _buildSucceed: function () {
    this.log(bDEBUG, "_buildSucceed", "call", "");
    this.player = this.$elem.find('#' + this.options.playerId).first();
    this.options.playerCheckMaxRetry = this._defaults.playerCheckMaxRetry;
    this.state.initialBuildPending = false;
    if (this.state.playRequested) {
      this._buildInnerBegin();
    }
  },
  _getInner: function () {
    this.log(bDEBUG, "_getInner", "call", "");
    var inner = null;
    if (typeof window.ActiveXObject === 'undefined') {
      var innerElems = this.$elem.find('embed');
    } else {
      var innerElems = this.$elem.find('object');
    }
    if (innerElems.length > 0) inner = innerElems.first();
    return inner;
  },
  _getFlashPlayer: function () {
    this.log(bDEBUG, "_getFlashPlayer", "call", "");
    var flashPlayer = null;
    if (typeof window.ActiveXObject === 'undefined') {
      if (document.embeds && document.embeds[this.options.playerId + "_emb"]) {
        flashPlayer = document.embeds[this.options.playerId + "_emb"];
      }
    } else {
      flashPlayer = document.getElementById(this.options.playerId + "_emb");
    }
    return flashPlayer;
  },
  _ActiveXRemoveFunctionRefs: function (addOn) {
    this.log(bDEBUG, "_ActiveXRemoveFunctionRefs", "call", "");
    /*
     * Internet Explorer uses a dynamically generated proxy to connect
     * JavaScript to an ActiveX control. The proxy actually marshalls
     * Javascript requests via COM invoke methods on ActiveX. (This allows
     * IE to work with multiple scripting languages such as VBScript and
     * JavaScript in a uniform way.) However, if there are any active
     * references still in existance to the proxy from JavaScript, IE's
     * garbage collector (GC) will not release the proxy or the
     * underlying ActiveX control. Therefore, it is necessary to unlink all
     * the functions in the proxy so the GC can cleanup properly and, thus,
     * avoid memory leaks and sometimes wierd ActiveX behavior.
     */
    if (typeof addOn === 'object')
      for (var prop in addOn) {
        if (typeof addOn[prop] === "function") {
          addOn[prop] = null;
          delete addOn[prop];
        }
      }
  },
  _flashCallbackSubscribe: function () {
    this.log(bDEBUG, "_flashCallbackSubscribe", "call", "");
    jQuery.subscribe("OnPlayerReady_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnPlayerReady, this));
    jQuery.subscribe("OnStateChanged_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnStateChanged, this));
    //jQuery.subscribe("OnPositionChanged_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnPositionChanged, this));
    jQuery.subscribe("OnStreamComplete_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnStreamComplete, this));
    jQuery.subscribe("OnError_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnError, this));
    jQuery.subscribe("OnCursorChange_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnCursorChange, this));
  },
  _flashCallbackUnsubscribe: function () {
    this.log(bDEBUG, "_flashCallbackUnsubscribe", "call", "");
    jQuery.unsubscribe("OnPlayerReady_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnPlayerReady, this));
    jQuery.unsubscribe("OnStateChanged_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnStateChanged, this));
    //jQuery.unsubscribe("OnPositionChanged_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnPositionChanged, this));
    jQuery.unsubscribe("OnStreamComplete_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnStreamComplete, this));
    jQuery.unsubscribe("OnError_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnError, this));
    jQuery.unsubscribe("OnCursorChange_" + this.options.playerId + '_emb', jQuery.proxy(this._subscribeOnCursorChange, this));
  },
  _destroyInnerBegin: function () {
    this.log(bDEBUG, "_destroyInnerBegin", "call", "");
    var inner = this._getInner();
    if (inner != null) {
      inner.remove();
      var self = this;
      this._ActiveXRemoveFunctionRefs(this.state.chromelessHLS);
      this.state.chromelessHLS = null;
      this._flashCallbackUnsubscribe();
      this.state._checkInnerDestroyTries = 0;
      this.state._checkInnerDestroyRef = window.setInterval(function () {
        self._destroyInnerCheck.call(self);
      }, this.options.playerCheckTimeout);
    }
  },
  _destroyInnerCheck: function () {
    this.log(bDEBUG, "_destroyInnerCheck", "call", "");
    var inner = this._getInner();
    if (inner == null || this.state._checkInnerDestroyTries == this.options.playerCheckMaxRetry) {
      window.clearInterval(this.state._checkInnerDestroyRef);
      this.state._checkInnerDestroyRef = null;
      this._destroyInnerEnd();
    }
    this.state._checkInnerDestroyTries++;
  },
  _destroyInnerEnd: function () {
    this.log(bDEBUG, "_destroyInnerEnd", "call", "");
    var inner = this._getInner();
    if (inner == null) {
      if (this.state.buildingInnerPlayerDiv) {
        this._buildInnerContinued();
      } else {
        this._destroyCompleted();
      }
    } else {
      this.log(bERROR, "_destroyInnerEnd", "call", "FAILED");
    }
  },
  _destroyCompleted: function () {
    this.log(bDEBUG, "_destroyCompleted", "call", "...");
  },
  _buildInnerBegin: function () {
    this.log(bDEBUG, "_buildInnerBegin", "call", "");
    var inner = this._getInner();
    this.state.buildingInnerPlayerDiv = true;
    if (inner != null) {
      this._destroyInnerBegin();
    } else {
      this._buildInnerContinued();
    }
  },
  _buildInnerContinued: function () {
    this.log(bDEBUG, "_buildInnerBegin", "call", "");
    var innerHTML = '';
    this._flashCallbackSubscribe();
    if (typeof window.ActiveXObject === 'undefined') {
      var wMode = (/chrom(e|ium)/.test(navigator.userAgent.toLowerCase())) ? "direct" : "transparent";
      innerHTML += '<embed ' +
          'name="' + this.options.playerId + '_emb" ' +
          'type="application/x-shockwave-flash" ' +
          'src="' + this.state.swfChromelessURL + '" ' +
          'width="100%" height="100%" ' +
          'quality="autohigh" ' +
          'swliveconnect="true" ' +
          'allowScriptAccess="sameDomain" ' +
          'bgcolor="0" ' +
          'allowFullScreen="false" ' +
          'wmode="' + wMode + '">' +
      '</embed>';
    } else {
      innerHTML += '<object id="' + this.options.playerId + '_emb" ' +
              'classid="clsid:d27cdb6e-ae6d-11cf-96b8-444553540000" ' +
              'codebase="" width="100%" height="100%">' +
          '<param name="movie" value="' + this.state.swfChromelessURL + '">' +
          '<param name="quality" value="autohigh">' +
          '<param name="swliveconnect" value="true">' +
          '<param name="allowScriptAccess" value="sameDomain">' +
          '<param name="bgcolor" value="0">' +
          '<param name="allowFullScreen" value="false">' +
          '<param name="wmode" value="opaque">' +
      '</object>';
    }
    this.player.append(innerHTML);
    var self = this;
    this.state._checkInnerBuildTries = 0;
    this.state._checkInnerBuildRef = window.setInterval(function () {
      self._checkInnerBuild.call(self);
    }, this.options.playerCheckTimeout);
  },
  _checkInnerBuild: function () {
    this.log(bDEBUG, "_checkInnerBuild", "call", "");
    var flashPlayer = this._getFlashPlayer();
    var playerLoadDef;
    if (flashPlayer != null) {
      playerLoadDef = typeof flashPlayer.playerLoad;
    }
    if ((playerLoadDef === 'function' && this.state._checkInnerBuildTries === this.options.playerCheckMaxRetry) || this.state._checkInnerBuildTries === this.options.playerCheckMaxRetry) {
      window.clearInterval(this.state._checkInnerBuildRef);
      this.state._checkInnerBuildRef = null;
      this._buildInnerEnd();
    }
    this.state._checkInnerBuildTries++;
  },
  _buildInnerEnd: function () {
    this.log(bDEBUG, "_buildInnerEnd", "call", "");
    this.state.buildingInnerPlayerDiv = false;
    var flashPlayer = this._getFlashPlayer();
    /*
     * At this point we wait for the wspOnPlayerReady callback from the
     * WebStreamPlayer.swf to occur and then call this._playContinued()
     * from there. We had timing issues with IE doing it from here.
     */
    if (!flashPlayer) {
      this.log(bERROR, "_buildInnerEnd", "call", "FALIED");
    }
  },
  setup: function () {
    this.log(bDEBUG, "setup", "call", "");
  },
  play: function (streamURL) {
    this.log(bDEBUG, "play", "call", streamURL);
    this.state.actionRequested = null;
    // override base method.
    if (this.state.playRequested) {
      this.log(bWARNING, "play", "A", "TOO MANY PLAY REQUESTS");
      return;
    }
    if (this.options.streamURL && streamURL == null)
      streamURL = this.options.streamURL;
    if (streamURL == null) {
      this.log(bERROR, "play", "B", "NO STREAM URL");
      return;
    }
    var sdpCharPos = streamURL.indexOf('.sdp');
    if (sdpCharPos > 0) {
      this.options.streamURL = streamURL.substring(0, sdpCharPos) + '.m3u8';
    } else {
      this.options.streamURL = streamURL;
    }
    this.state.playRequested = true;
    if (this.state.isPlaying) {
      this.state.chromelessHLS.doStop();
    } else {
      this._playSetup();
    }
  },
  _playSetup: function () {
    this.log(bDEBUG, "_playSetup", "call", "");
    this.state.isPlaying = false;
    this.state.isBuffering = false;
    this.state.isPaused = false;
    this.state.isMuted = false;
    this.state.progressMs = 0;
    if (this.state.initialBuildPending || this.state.buildingInnerPlayerDiv)
      return;
    this._buildInnerBegin();
  },
  _playContinued: function () {
    this.log(bDEBUG, "_playContinued", "call", "");
    this.state.chromelessHLS = this._getFlashPlayer();
    this.state.chromelessHLS.doLoad(this.options.streamURL);
    //this.state.chromelessHLS.setLogLevel(0);
    this.state.chromelessHLS.doPlay();
    this.state.volume = 100;
    this.state.muted = (this.state.volume === 0);
    this.state.chromelessHLS.playerVolume(this.state.volume);
    /*
    var self = this;
    this.state._checkChromelessDuringPlayRef = window.setInterval(function () {
        self._statusDuringPlayCheck.call(self);
    }, 563);
    */
    this.state.playRequested = false;
  },
  stop: function () {
    this.log(bDEBUG, "stop", "call", "");
    this.state.actionRequested = null;
    this.state.chromelessHLS.doStop();
    this.state.unloadRequested = true;
    this.state.chromelessHLS.doKill();
    /*
    window.clearInterval(this.state._checkChromelessDuringPlayRef);
    this.state._checkChromelessDuringPlayRef = null;
    */
  },
  pause: function () {
    this.log(bDEBUG, "pause", "call", "");
    this.state.actionRequested = "Pause";
    this.state.chromelessHLS.doPause();
  },
  resume: function () {
    this.log(bDEBUG, "resume", "call", "");
    this.state.actionRequested = "Resume";
    this.state.chromelessHLS.doResume();
  },
  skipforward: function (ms) {
    this.log(bDEBUG, "skipforward", "call", ms);
    this.state.actionRequested = "SkipForward";
    if (this.state.isPaused) {
      this.state.chromelessHLS.doResume();
    }
    var playerPosition = this.state.chromelessHLS.getCursorPosition();
    var offset = this.options.skipforwardOffset;
    if (ms != null) offset = ms;
    var newPosition = playerPosition + (offset / 1000.0);
    if (newPosition > this.state.chromelessHLS.getStreamDuration()) newPosition = this.state.chromelessHLS.getStreamDuration();
    this.state.chromelessHLS.doSeek(newPosition);
  },
  skipbackward: function (ms) {
    this.log(bDEBUG, "skipbackward", "call", ms);
    this.state.actionRequested = "SkipBackward";
    if (this.state.isPaused) {
      this.state.chromelessHLS.doResume();
    }
    var playerPosition = this.state.chromelessHLS.getCursorPosition();
    var offset = this.options.skipbackwardOffset;
    if (ms != null) offset = ms;
    var newPosition = playerPosition - (offset / 1000.0);
    if (newPosition < 0) newPosition = 0.00;
    this.state.chromelessHLS.doSeek(newPosition);
  },
  catchup: function () {
    this.log(bDEBUG, "catchup", "call", "");
    this.state.actionRequested = "Catchup";
    if (this.state.isPaused) {
      this.state.chromelessHLS.doResume();
    }
    this.state.chromelessHLS.doCatchUp();
  },
  seekTo: function (ms) {
    this.log(bDEBUG, "seekTo", "call", ms);
    this.state.actionRequested = "ResumeFrom";
    if (this.state.isPaused) {
      this.state.chromelessHLS.doResume();
    }
    var newPosition = (ms / 1000.0);
    if (newPosition < 0) newPosition = 0.00;
    else if (newPosition > this.state.chromelessHLS.getStreamDuration()) newPosition = this.state.chromelessHLS.getStreamDuration();
    this.state.chromelessHLS.doSeek(newPosition);
  },
  mute: function () {
    this.log(bDEBUG, "mute", "call", "");
    this.state.volume = 0;
    this.state.muted = (this.state.volume === 0);
    this.state.chromelessHLS.playerVolume(this.state.volume);
    this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onMute', {});
  },
  unmute: function () {
    this.log(bDEBUG, "unmute", "call", "");
    this.state.volume = 100;
    this.state.muted = (this.state.volume === 0);
    this.state.chromelessHLS.playerVolume(this.state.volume);
    this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onUnmute', {});
  },
  getPosition: function () {
    this.log(bDEBUG, "getPosition", "call", "");
    var playerPosition = this.state.chromelessHLS.getCursorPosition();
    if (window.isNaN(playerPosition)) playerPosition = 0;
    this.state.progressMs = Math.round(1000 * playerPosition);
    return this.state.progressMs;
  },
  getcliplength: function () {
    this.log(bDEBUG, "getcliplength", "call", "");
    var duration = this.state.chromelessHLS.getStreamDuration();
    if (window.isNaN(duration)) duration = 360000;
    return Math.round(1000 * duration);
  },
  getVersion: function () {
    this.log(bDEBUG, "getVersion", "call", "");
    var verArray = this.state.chromelessHLS.getPlayerVersion();
    var verStr = '';
    if (verArray && verArray.length > 0) {
      verStr += verArray[0];
      for (var i = 1; i < verArray.length; i++) {
        verStr += ',' + verArray[i];
      }
    }
    return verStr;
  },
  isPlaying: function () {
    this.log(bDEBUG, "isPlaying", "call", "");
    return this.state.isPlaying;
  },
  isPaused: function () {
    this.log(bDEBUG, "isPaused", "call", "");
    return this.state.isPaused;
  },
  isMuted: function () {
    this.log(bDEBUG, "isMuted", "call", "");
    /*
    * There is no way to get the volume level or mute state of the player.
    * We have to assume that our commands are successful and that the
    * volume is always enabled (by forcing it to be max) when we start
    * playing again  Otherwise, things can get out of sync.
    */
    return this.state.muted;
  },
  /*
  _statusDuringPlayCheck: function () {
      var playerStatus = this.state.chromelessHLS.getPlayerState();
      var localState = {}
      if (playerStatus != null) {
          localState.isPlaying = playerStatus === 'PLAYING_BUFFERING' ||
                  playerStatus === 'PLAYING' ||
                  playerStatus === 'PAUSED';
          localState.isBuffering = playerStatus === 'PLAYING_BUFFERING';
          localState.isPaused = playerStatus === 'PAUSED';
      }
      var playerPosition = this.state.chromelessHLS.getCursorPosition();
      if (playerPosition != null) {
          localState.progressMs = Math.round(1000 * playerPosition);
      }
      this._changeOfState(localState);
      if (playerStatus == null || !localState.isPlaying || this.state.unloadRequested) {
          window.clearInterval(this.state._checkChromelessDuringPlayRef);
          this.state._checkChromelessDuringPlayRef = null;
          this.state.unloadRequested = false;
          this._isNotBufferingOrPlaying();
      }
  },
*/
  _changeOfState: function (localState) {
    this.log(bDEBUG, "_changeOfState", "call", "");
    if (localState.isPlaying !== this.state.isPlaying) {
      this.state.isPlaying = localState.isPlaying;
      if (this.state.isPlaying)
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStart', { offset: this.getPosition(), playLength: this.getcliplength() });
      else
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStop', {});
    }
    if (localState.isBuffering !== this.state.isBuffering && this.state.isPlaying) {
      this.state.isBuffering = localState.isBuffering;
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onBuffering', { isBuffering: localState.isBuffering, timeline: localState.progressMs });
      this.state.idleAfterPlaying = true;
    }
    if (localState.isPaused !== this.state.isPaused && this.state.isPlaying) {
      this.state.isPaused = localState.isPaused;
      if (this.state.isPaused)
        var objData = { action: 'Pause', timeScale: 0, offset: this.getPosition(), playLength: this.getcliplength() };
      else
        var objData = { action: 'Resume', timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() };

      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', objData);
    }
    /*
    if (localState.progressMs !== this.state.progressMs) {
        this.state.progressMs = localState.progressMs;
    }
    */
  },
  _isNotBufferingOrPlaying: function () {
    this.log(bDEBUG, "_isNotBufferingOrPlaying", "call", "");
    if (this.state.isPlaying) {
      this.state.isPlaying = false;
      this.state.isBuffering = false;
      this.state.isPaused = false;
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStop', {});
    }
    this.state.progressMs = 0;
    if (this.state.idleAfterPlaying) {
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPlayerNotReady', {});
      this.state.idleAfterPlaying = false;
      this.state.isVideoPlayerReady = false;
    }
    if (this.state.playRequested) this._playSetup();
  },
  _subscribeOnPlayerReady: function (topic, data) {
    this.log(bDEBUG, "_subscribeOnPlayerReady", "call", "");
    this.state.isVideoPlayerReady = true;
    this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPlayerReady', {});
    if (this.state.playRequested) this._playContinued();
  },
  _subscribeOnStateChanged: function (topic, data) {
    this.log(bDEBUG, "_subscribeOnStateChanged", "call", "");
    var playerStatus = data.state;
    var localState = {};
    if (playerStatus != null) {
      localState.isPlaying =
        playerStatus === (this.state.isPlaying && 'PLAYING_BUFFERING') ||
        playerStatus === 'PLAYING' ||
        playerStatus === 'PAUSED' ||
        playerStatus === 'PAUSED_BUFFERING';
      localState.isBuffering =
        playerStatus === 'PLAYING_BUFFERING' ||
        playerStatus === 'PAUSED_BUFFERING';
      localState.isPaused =
        playerStatus === 'PAUSED' ||
        playerStatus === 'PAUSED_BUFFERING';
      if (localState.isPlaying) localState.progressMs = this.getPosition();
      else localState.progressMs = 0;
      this._changeOfState(localState);
      if (playerStatus == null || this.state.unloadRequested ||
          !localState.isPlaying) {
        this.state.unloadRequested = false;
        this._isNotBufferingOrPlaying();
      }
    }
  },
  _subscribeOnCursorChange: function (topic, position) {
    this.log(bDEBUG, "_subscribeOnCursorChange", "call", "");
    if (jQuery.isBlank(position) || jQuery.isBlank(this.state.actionRequested)) return;
    window.setTimeout(jQuery.proxy(function () {
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: this.state.actionRequested, timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
      this.state.actionRequested = null;
    }, this), 250);
  },
  _subscribeOnStreamComplete: function (topic) {
    this.log(bDEBUG, "_subscribeOnStreamComplete", "call", "");
    window.setTimeout(jQuery.proxy(function () {
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onError', { state: "SOURCE_ERROR", connErrorInfo: { errorCode: 112, errorData: "End of media" } });
      this.state.actionRequested = null;
    }, this), 250);
  },
  _subscribeOnError: function (topic, data) {
    this.log(bDEBUG, "_subscribeOnError", "call", "");
    this.log(bERROR, "_subscribeOnError", "call",
      "Flash Player Error: " + data.message);
    this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onError', { state: "SOURCE_ERROR", connErrorInfo: { errorCode: 0005, errorData: "Ooops..Encountered error in streaming. Please contact administrator to check sessions tab in AdminUI for more info." } });
  }
});
// expose Objects as jQuery plugins
jQuery.plugin('streamPlayerChromelessHLS', StreamPlayerChromelessHLS);