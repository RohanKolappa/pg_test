/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * StreamPlayerHTML5videoHLS - Class definition 
 * - registered as a jQuery plugin
 */
var StreamPlayerHTML5videoHLS = StreamPlayer.extend({
    init: function(options, elem) {
        // extend the defaults
        this.log(bDEBUG, "init", "call", "");
        this._super(options, elem);
        this.state.buildingInnerPlayerDiv = false;
        this.state.isVideoPlayerReady = false;
        this.state.playRequested = false;
        this.state.stopRequested = false;
        this.state.isPlaying = false;
        this.state.suppressPausePlay = false;
        this.state.progPlayTimeoutRef = null;
        this.lastActionRequested = null;
        this.isDataLoaded = false;
        jQuery.subscribe("onLoad", jQuery.proxy(this._buildSucceed, this));
    },
    _build: function() {
        this.log(bDEBUG, "_build", "call","");
        // override base method.
        this.state.initialBuildPending = true;
        // Temporary until I'm certain this works across all devices; should
        // move permanently into EmbeddedPlayer.css once confident this works.
//        this.options.width = '100%';
//        this.options.height = '100%';
//        var cssWidth = (typeof this.options.width === 'number') ? (this.options.width + "px") : this.options.width;
//        var cssHeight = (typeof this.options.height === 'number') ? (this.options.height + "px") : this.options.height;
        var playerHTML = '<div id="' + this.options.divId + '_objectdiv" ' + 'class="objectdiv StreamPlayer" ' + 'style="position: absolute; display: block; width: 100%; top: 0px;">';
        playerHTML += '<div id="' + this.options.playerId + '" style="display: block; width: 100%; height: 100%;"></div></div>';
        this.$elem.append(playerHTML);
        if (!this.player) {
            var thisObj = this;
            setTimeout(function() {
                thisObj.checkPlayer.call(thisObj);
            }, thisObj.options.playerCheckTimeout);
        }
    },
    destroy: function(pluginName) {
      this.log(bDEBUG, "destroy", "call", "");
      var inner = this._getInner();
      if (inner != null) {
        this._destroyInnerBegin();
      }
      this._super(pluginName);
      this.log(bDEBUG, "destroy", "return", "");
    },
    _buildSucceed: function() {
        this.log(bDEBUG, "_buildSucceed", "call", "");
        this.player = this.$elem.find('#' + this.options.playerId).first();
        this.options.playerCheckMaxRetry = this._defaults.playerCheckMaxRetry;
        this.state.initialBuildPending = false;
        if (this.state.playRequested) {
            this._buildInnerBegin();
        }
    },
    _getInner: function() {
        this.log(bDEBUG, "_getInner", "call", "");
        var inner = null;
        var innerElems = this.$elem.find('video');
        if (innerElems.length > 0) inner = innerElems.first();
        return inner;
    },
    _getVideoPlayer: function() {
        this.log(bDEBUG, "_getVideoPlayer", "call", "");
        return document.getElementById(this.options.playerId + "_vid");
    },
    _destroyInnerBegin: function() {
        this.log(bDEBUG, "_destroyInnerBegin", "call", "...");
        this.state.isVideoPlayerReady = false;
        var inner = this._getInner();
        var videoPlayer = this._getVideoPlayer();
        if (inner != null) {
            videoPlayer.removeEventListener('ended', this._onPlayEnded);
            videoPlayer.removeEventListener('pause', this._onPlayPaused);
            videoPlayer.removeEventListener('play', this._onPlayStarted);
            videoPlayer.removeEventListener('seeked', this._onPlaySeeked);
            videoPlayer.removeEventListener('loadedmetadata', this._onDataLoaded);
            videoPlayer.removeEventListener('canplaythrough', this._canplaythrough);
            inner.remove();
            var self = this;
            this.state._checkInnerDestroyTries = 0;
            this.state._checkInnerDestroyRef = window.setInterval(function() {
                self._destroyInnerCheck.call(self);
            }, this.options.playerCheckTimeout);
        }
    },
    _destroyInnerCheck: function() {
        this.log(bDEBUG, "_destroyInnerCheck", "call",
          "Try:" + this.state._checkInnerDestroyTries);
        var inner = this._getInner();
        if (inner == null || this.state._checkInnerDestroyTries == this.options.playerCheckMaxRetry) {
            window.clearInterval(this.state._checkInnerDestroyRef);
            this.state._checkInnerDestroyRef = null;
            this._destroyInnerEnd();
        }
        this.state._checkInnerDestroyTries++;
    },
    _destroyInnerEnd: function() {
      var inner = this._getInner();
      if (inner == null) {
        this.log(bDEBUG, "_destroyInnerEnd", "call", "");
        if (this.state.buildingInnerPlayerDiv) {
          this._buildInnerContinued();
        } else {
          this._destroyCompleted();
        }
      } else {
        this.log(bERROR, "_destroyInnerEnd", "call", "FALIED");
      }
    },
    _destroyCompleted: function() {
        this.log(bDEBUG, "_destroyCompleted", "call", "...");
    },
    _buildInnerBegin: function() {
        this.log(bDEBUG, "_buildInnerBegin", "call", "...");
        var inner = this._getInner();
        this.state.buildingInnerPlayerDiv = true;
        if (inner != null) {
            this._destroyInnerBegin();
        } else {
            this._buildInnerContinued();
        }
    },
    _buildInnerContinued: function() {
        this.log(bDEBUG, "_buildInnerContinued", "call", "...");
        var innerHTML = '<video ' +
                'id="' + this.options.playerId + '_vid" ' +
                'width="100%" height="100%">' +
              '<source src="' + this.options.streamURL + '" type="application/vnd.apple.mpegURL">' +
            '</video>';
        this.player.append(innerHTML);
        var self = this;
        this.state._checkInnerBuildTries = 0;
        this.state._checkInnerBuildRef = window.setInterval(function() {
            self._checkInnerBuild.call(self);
        }, this.options.playerCheckTimeout);
    },
    _checkInnerBuild: function() {
        this.log(bDEBUG, "_checkInnerBuild", "call",
          "Try: " + this.state._checkInnerBuildTries);
        var videoPlayer = this._getVideoPlayer();
        if ((videoPlayer.readyState > 0 && this.state._checkInnerBuildTries === this.options.playerCheckMaxRetry) || this.state._checkInnerBuildTries === this.options.playerCheckMaxRetry) {
            window.clearInterval(this.state._checkInnerBuildRef);
            this.state._checkInnerBuildRef = null;
            this._buildInnerEnd();
        }
        this.state._checkInnerBuildTries++;
    },
    _buildInnerEnd: function() {
        this.state.buildingInnerPlayerDiv = false;
        var videoPlayer = this._getVideoPlayer();
        if (videoPlayer != null) {
            this.log(bDEBUG, "_buildInnerEnd", "call", "");
            this.state.isVideoPlayerReady = true;
            this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPlayerReady', {});
            this.state.videoHLS = videoPlayer;
            videoPlayer.addEventListener('loadedmetadata', jQuery.proxy(this._onDataLoaded, this));
            videoPlayer.addEventListener('play', jQuery.proxy(this._onPlayStarted, this));
            videoPlayer.addEventListener('pause', jQuery.proxy(this._onPlayPaused, this));
            videoPlayer.addEventListener('ended', jQuery.proxy(this._onPlayEnded, this));
            videoPlayer.addEventListener('seeked', jQuery.proxy(this._onPlaySeeked, this));
            videoPlayer.addEventListener('canplaythrough', jQuery.proxy(this._canplaythrough, this));
            
            if (this.state.playRequested)
                this._playContinued();
        } else {
            this.log(bERROR, "_buildInnerEnd", "call", "FAILED");
        }
    },
    setup: function() {
        this.log(bDEBUG, "setup", "call", "");
    },
    play: function(streamURL) {
        this.log(bDEBUG, "play", "call", streamURL);
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
          this.stop();
        }
        this._playSetup();
    },
    _playSetup: function() {
        this.log(bDEBUG, "_playSetup", "call", "");
        this.state.isPlaying = false;
        this.state.isBuffering = false;
        this.state.isPaused = false;
        this.state.isMuted = false;
        this.state.stopRequested = false;
        this.lastActionRequested = null;
        this.isDataLoaded = false;
        if (this.state.initialBuildPending || this.state.buildingInnerPlayerDiv)
            return;
        this._buildInnerBegin();
    },
    _playContinued: function() {
        this.log(bDEBUG, "_playContinued", "call", "");
        this.state.videoHLS.play();
        this.state.progPlayTimeoutRef = window.setTimeout(jQuery.proxy(this._onPlayBlocked, this), 1000);
        this.state.playRequested = false;
    },
    stop: function() {
        this.log(bDEBUG, "stop", "call", "");
        this.state.videoHLS.pause();
        this.state.stopRequested = true;
        if (this.state.isPaused) {
          this._onPlayPaused();
        }
        this._destroyInnerBegin();
    },
    pause: function() {
      this.log(bDEBUG, "pause", "call", "");
      if (this.state.isPlaying) {
        this.state.videoHLS.pause();
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: 'Pause', timeScale: 0, offset: this.getPosition(), playLength: this.getcliplength() });
      }
    },
    resume: function() {
      this.log(bDEBUG, "resume", "call", "");
      if (this.state.isPlaying) {
        this.state.videoHLS.play();
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: 'Resume', timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
      }
    },
    skipforward: function(ms) {
      this.log(bDEBUG, "skipforward", "call", ms);
      if (this.state.isPaused) {
        this.state.videoHLS.play();
      }
      this.lastActionRequested = "SkipForward"
      var playerPosition = this.state.videoHLS.currentTime;
      var offset = this.options.skipforwardOffset;
      if (ms != null) offset = ms;
      this.state.videoHLS.currentTime = (playerPosition + (offset / 1000.0)).toFixed(1);
      //this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: 'SkipForward', timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
    },
    skipbackward: function(ms) {
      this.log(bDEBUG, "skipbackward", "call", ms);
      if (this.state.isPaused) {
        this.state.videoHLS.play();
      }
      this.lastActionRequested = "SkipBackward"
      var playerPosition = this.state.videoHLS.currentTime;
      var offset = this.options.skipbackwardOffset;
      if (ms != null) offset = ms;
      this.state.videoHLS.currentTime = (playerPosition - (offset / 1000.0)).toFixed(1);
      //this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: 'SkipBackward', timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
    },
    catchup: function() {
      this.log(bDEBUG, "catchup", "call", "");
      if (this.state.isPaused) {
        this.state.videoHLS.play();
      }
      this.lastActionRequested = "Catchup";
      var newCurrentTime = this.state.videoHLS.seekable.end(0);      
      this.state.videoHLS.currentTime = newCurrentTime.toFixed(1);
    },
    seekTo: function(ms) {
      this.log(bDEBUG, "seekTo", "call", ms);
      if (this.state.isPaused) {
        this.state.videoHLS.play();
      }
      this.lastActionRequested = "ResumeFrom";
      this.state.videoHLS.currentTime = (ms / 1000.0).toFixed(1);
      //this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: 'ResumeFrom', timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
    },
    mute: function() {
      this.log(bDEBUG, "mute", "call", "");
      this.state.videoHLS.muted = true;
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onMute', {});
    },
    unmute: function() {
      this.log(bDEBUG, "unmute", "call", "");
      this.state.videoHLS.muted = false;
      this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onUnmute', {});
    },
    getPosition: function() {
      this.log(bDEBUG, "getPosition", "call", "");
      var playerPosition = this.state.videoHLS.currentTime;
      return Math.round(1000 * playerPosition);
    },
    getcliplength: function () {
      this.log(bDEBUG, "getcliplength", "call", "");
      var duration = this.state.videoHLS.duration;
      //return BarcoUtil.msToStr(Math.round(1000 * duration));
      if(duration == "Infinity") {
        duration = this.state.videoHLS.seekable.end(0) - this.state.videoHLS.seekable.start(0);
      }
      return Math.round(1000 * duration);
    },
    getEndTimeRange: function () {
      this.log(bDEBUG, "getEndTimeRange", "call", "");
      return Math.round(1000 * this.state.videoHLS.seekable.end(0));
    },
    getVersion: function() {
      this.log(bDEBUG, "getVersion", "call", "");
      return navigator.appVersion;
    },
    isPlaying: function() {
        this.log(bDEBUG, "isPlaying", "call", "");
        return this.state.isPlaying;
    },
    isPaused: function() {
      this.log(bDEBUG, "isPaused", "call", "");
      return this.state.videoHLS.paused;
    },
    isMuted: function() {
      this.log(bDEBUG, "isMuted", "call", "");
      return this.state.videoHLS.muted;
    },
    _onPlayStarted: function() {
      this.log(bDEBUG, "_onPlayStarted", "call", "");
      window.clearTimeout(this.state.progPlayTimeoutRef);
      this.state.progPlayTimeoutRef = undefined;
      if (!this.state.isPlaying) {
        if (this.state.playBlocked) {
          this.state.playBlocked = false;
          // this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onPlayUnblocked', {});
          this.state.suppressPausePlay = true;
          this.state.videoHLS.pause();
          this.state.videoHLS.controls = false;
          this.state.videoHLS.play();
          this.state.isPaused = false;
        } else {
          this.state.isPaused = false;
        }
        if(this.isDataLoaded){
          this.state.isPlaying = true;
        }
      }
      if (this.state.isPaused) {
        this.state.isPaused = false;
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamResume', { offset: this.getPosition(), playLength: this.getcliplength() });
      }
    },
    _onPlayBlocked: function () {
      this.log(bDEBUG, "_onPlayBlocked", "call", "");
      /*
       * On some platforms play may be blocked unless invoked directly by a
       * "real" touch event. This is detected indirectly when the play action
       * is not followed by a play event within 1 second. Based on our tests,
       * the play action, when successful, always happens within 1/3 sec.
       * If it doesn't happen within 3X of that time window, we come here.
       * So far this has been 100% reliable but there's always the chance that
       * we missed a case. Thus far, no better solution has been found.
       */
      this.state.videoHLS.controls = true;
      this.state.playBlocked = true;
      this.state.isPlaying = false;
      // this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onPlayBlocked', {});
    },
    _onDataLoaded: function() {
      this.log(bDEBUG, "_onDataLoaded", "call", "");
      var intiTimerRef = window.setInterval(jQuery.proxy(function() {
        if(this.state.videoHLS.seekable.length > 0) {
          window.clearInterval(intiTimerRef);
          this.isDataLoaded = true;
          this.state.isPlaying = true;
          this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStart', { offset: this.getPosition(), playLength: this.getcliplength() });
        }
      }, this), 250);
      //this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStart', { offset: this.getPosition(), playLength: this.getcliplength() });
    },
    _onPlayPaused: function() {
      this.log(bDEBUG, "_onPlayPaused", "call", "");
      if (this.state.stopRequested) {
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStop', {});
        this.state.isPlaying = false;
        this.state.isPaused = false;
        this.state.stopRequested = false;
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPlayerNotReady', {});
      } else if (!this.state.isPaused && !this.state.suppressPausePlay) {
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: 'Pause', timeScale: 0, offset: this.getPosition(), playLength: this.getcliplength()});
        this.state.isPaused = true;
      }
      this.state.suppressPausePlay = false;
    },
    _onPlayEnded: function() {
      this.log(bDEBUG, "_onPlayEnded", "call", "");
      if (this.state.isPlaying)
        this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamStop', {});
      this.state.isPlaying = false;
      this.state.isPaused = false;
    },
    _onPlaySeeked: function() {
      this.log(bDEBUG, "_onPlaySeeked", "call", "");
      if(this.lastActionRequested) {
        window.setTimeout(jQuery.proxy(function() {
          if (this.state.videoHLS.readyState === 4) {
            this.log(bDEBUG, "_onPlaySeeked", "setTimeout",
              "End:" + this.state.videoHLS.seekable.end(0) + " Pos:" + this.state.videoHLS.currentTime);
            this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: this.lastActionRequested, timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
            this.lastActionRequested = null;
          }
        }, this), 250);
      }
    },
   _canplaythrough: function(evt) {
      this.log(bDEBUG, "_canplaythrough", "call", "");
       if(this.lastActionRequested) {
        window.setTimeout(jQuery.proxy(function() {
           if(this.state.videoHLS.readyState == 4){
              this.log(bDEBUG, "_canplaythrough", "setTimeout",
                "End:" + this.state.videoHLS.seekable.end(0) + " Positon:" + this.state.videoHLS.currentTime + " readyState:" + this.state.videoHLS.readyState);
              this.objSessionController && this.objSessionController.onHLSPlayerStateChange('onStreamPVRAction', { action: this.lastActionRequested, timeScale: 1, offset: this.getPosition(), playLength: this.getcliplength() });
              this.lastActionRequested = null;
           }
        }, this), 250);
      }
   }
});
// expose Objects as jQuery plugins
jQuery.plugin('streamPlayerHTML5videoHLS', StreamPlayerHTML5videoHLS);
