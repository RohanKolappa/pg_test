/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * BarcoPlayerInterface - Class definition 
 * - registered as a jQuery plugin
 */
var BarcoPlayerInterface = Class.extend({
  init: function(options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);

    // Init state
    this.playerIFrame = null;
    this.playerIFrameHost = null;
    this.playerURL = null;
    this.waitedPlayerURL = null;
    this.playerURLinProcess = false;
    this.playerState = {
      version: '',
      clipLength: 0,
      isPlaying: false,
      isPaused: false,
      isMuted: false
    };
    this.authData = null;

    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.options.divId = this.$elem.attr('id');
    this.log(bDEBUG, "init", "Inf return", "");

    // Build the DOM's initial structure
    this._build();
  },
  _defaults: {
  },
  _build: function() {
    this.log(bDEBUG, "_build", "Inf call", "");
    // stash divId, generate a new random playerId
    this.options.playerId = 'player_' + Math.floor((Math.random() * 100000) + 1);

    // stash current Host
    this.options.playerParentHost = window.location.href.match(/:\/\/(?:\.)?(.[^/]+)(.*)/)[1];

    // set player background color to black
    this.$elem.css({'background-color': 'black'});
    // set player position - needed for overlay control bar
    if (jQuery.isBlank(this.$elem.attr('position'))) {
      this.$elem.css({position: 'relative'});
    }
    // set player dimensions
    // remove from options before passing to iframe player
    if (this.options.width != null) {
      this.$elem.css({width: this.options.width});
      this.options.width = null;
    }
    if (this.options.height != null) {
      this.$elem.css({height: this.options.height});
      this.options.height = null;
    }

    if ((this.playerIFrame = jQuery('iframe#' + this.options.divId + '_iFrame')[0]) == null) {
      // add an iframe to host the remote player
      var child = jQuery('<iframe src="" id="' + this.options.divId + '_iFrame" style="width:100%;height:100%" frameBorder="0" scrolling="no" allowfullscreen></iframe>')
        .addClass('BarcoPlayerInterface');
      this.$elem.append(child);
      // get a handle to the iframe for sending messages
      this.playerIFrame = jQuery('iframe#' + this.options.divId + '_iFrame')[0];
    }

    // add an event listener to receive messages
    if (window.addEventListener) {
      addEventListener("message", jQuery.proxy(this._receiveMessage, this), false);
    }
    else {
      attachEvent("onmessage", jQuery.proxy(this._receiveMessage, this));
    }

    //jQuery(document).bind('fscreenchange', jQuery.proxy(this._onToggleMainScreen, this));
    this.log(bINFO, "_build", "Inf return", "Done");
  },
  destroy: function(pluginName) {
    this.log(bDEBUG, "destroy", "Inf call", pluginName);
    // unsubscribe to events
    if (window.removeEventListener) {
      removeEventListener("message", jQuery.proxy(this._receiveMessage, this), false)
    }
    else {
      detachEvent("onmessage", jQuery.proxy(this._receiveMessage, this))
    }
    jQuery(document).unbind('fscreenchange');

    // destroy child classes; destroy self
    if (!this.$elem)
      return;
    // thisObj.$elem.find('.BarcoPlayerInterface').remove(); cause HLS flash destroy problem in IE 9
    //i.e. __flash__removeCallback(document.getElementById("player_93804_emb"), "playerSetseekFromLowestLevel"); undefined
    // remove plugin
    this.$elem.data('player', null);
    this._unloadIFrame();
    this.log(bDEBUG, "destroy", "Inf return", pluginName);
  },

  _onToggleMainScreen:function(){
    jQuery.publish("onFullscreen", { id: this.options.divId, playerId: this.options.playerId, type: "Event", message: "On Fullscreen", isFullScreen: jQuery.fullscreen.isFullScreen() });
  },
  /**
  * show function 
  */
  show: function () {
    this.$elem.removeClass('hidden');
  },

  /**
  * hide function 
  */
  hide: function () {
    this.$elem.addClass('hidden');
  },

  // send messages to Player
  _sendMessage: function() {
    this.log(bDEBUG, "_sendMessage", "call", "");
    if (this.playerIFrame && this.playerIFrame.src != '') {
      this.playerIFrame.contentWindow.postMessage(JSON.stringify(arguments), '*');
      this.log(bDEBUG, "_sendMessage", "call",
        JSON.stringify(arguments));
    }
  },
  // receive events from Player
  _receiveMessage: function(event) {
    if(!this.playerIFrameHost) return;
    this.log(bDEBUG, "_receiveMessage", "call", "");
    //Incase port 80/443 browser/frame treats it as defult
    //So event.origin will not have :80/443
    var frameHost = this.playerIFrameHost.replace(/:80$/, "").replace(/:443$/, "");
    // make sure message is coming from iframe
    if (event.origin.indexOf(frameHost) < 0) {
      this.log(bDEBUG, "_receiveMessage", "Inf A",
        'skip ' + event.origin);
      return;
    }
    this.log(bDEBUG, "_receiveMessage", "Inf B",
      'from: ' + event.origin + ', event.data: ' + event.data);
    var evt = JSON.parse(event.data);
    if (evt[0] && evt[0].type) {
      var cmd = evt[0].type;
      var data = evt[1];
      if (data.id != this.options.divId) {
        this.log(bDEBUG, "_receiveMessage", "Inf C", 'skip ' + data.id);
        return;
      }
      if (cmd == 'onFullscreen' && !jQuery.fullscreen.isNativelySupported()) {
        // toggle fullscreen based on current state
        if (jQuery.fullscreen.isFullScreen()) {
          jQuery.fullscreen.exit();
        }
        else {
          this.$elem.fullscreen();
        }
      }
      else if (cmd == 'onStart' || cmd == 'onAuthorizing') {
        // stash player state 
        this.playerState.isPlaying = true;
        this.playerURLinProcess = false;
      }
      else if (cmd == 'onPause') {
        // stash player state 
        this.playerState.isPaused = true;
      }
      else if (cmd == 'onMute') {
        // stash player state 
        this.playerState.isMuted = true;
      }
      else if (cmd == 'onUnmute') {
        // stash player state 
        this.playerState.isMuted = false;
      }
      else if (cmd == 'onResume') {
        // stash player state 
        this.playerState.isPaused = false;
      }
      else if (cmd == 'onCatchup') {
        // stash player state 
        this.playerState.isPaused = false;
      }
      else if (cmd == 'onSeek') {
        // stash player state 
        this.playerState.isPaused = false;
      }
      else if (cmd == 'onStop') {
        // stash player state 
        this.playerState.isMuted = false;
        this.playerState.isPaused = false;
        this.playerState.isPlaying = false;
        if(this.waitedPlayerURL) {
          this.waitedPlayerURL = null;
          this.playerURLinProcess = true;
          this._loadIFrame();
          this.log(bINFO, "_receiveMessage", "Inf D", 'onStop, play this.options.playerURL:' + this.options.playerURL);
        }
      }
      else if (cmd == 'onError') {
         this.playerURLinProcess = false;
         this.log(bINFO, "_receiveMessage",  "Inf E", 'onError, reset this.playerURLinProcess:' + this.playerURLinProcess);
      }
      jQuery.publish(cmd, data);
    }
    else if (evt[0] && evt[0].version) {
      this.playerState.version = evt[0].version;
    }
    else if (evt[0] && evt[0].ready && evt[0].id === this.options.divId) {
      this._sendMessage('conf', this.options);
      if (this.authData)
        this._sendMessage('authenticate', this.authData);
      this._sendMessage('play');
    }
  },
  _loadIFrame: function() {
    if (this.options.playerURL != null) {
      this.log(bDEBUG, "_loadIFrame", "Inf call", "");
      var urlObj = BarcoUtil.parseURL(this.options.playerURL);
      var simplifiedPlayerURL;
      this.playerURL = this.options.playerURL;
      if (urlObj.http) {
        simplifiedPlayerURL = "http:/" + urlObj.http;
      } else if (urlObj.https) {
        simplifiedPlayerURL = "https:/" + urlObj.https;
      } else {
        this.log(bERROR, "_loadIFrame", "Inf", "Bad protocol: " + this.options.playerURL);
        this.playerURL = null;
      }
      if (this.playerURL) {
        this.playerIFrameHost = simplifiedPlayerURL.match(/:\/\/([^/]+)/)[1];
        this.playerIFrame.src = simplifiedPlayerURL + "?id=" + this.options.divId;
      }
    }
  },
// _loadIFrameAndPlay: No longer called
// Instead we only load the player. 
// Once loaded the player will fire a 'ready' event. 
// When we get this we will pass the conf and issue a play request
  _loadIFrameAndPlay: function() {
    this.log(bDEBUG, "_loadIFrameAndPlay", "call", "");
    var optionsQueryStr = '';
    for (var key in this.options) {
      if (key == 'playerURL')
        continue;
      optionsQueryStr += '&' + 'confOpt_' + key + '=' + this.options[key];
    }
    if (this.options.playerURL != null) {
      this.playerURL = this.options.playerURL;
    }
    this.playerIFrameHost = this.playerURL.match(/:\/\/(?:\.)?(.[^/]+)(.*)/)[1];
    this.playerIFrame.src = this.playerURL + optionsQueryStr;
  },
  _unloadIFrame: function() {
    //empty iframe source after logout
    this.playerIFrame.src = '';
  },
  conf: function(options) {
    this.log(bDEBUG, "conf", "Inf call", "");
    this.options = jQuery.extend({}, this.options, options);
    // set dimensions of top level div
    // remove from options before passing to iframe player
    if (this.options.width != null) {
      this.$elem.css({width: this.options.width});
      this.options.width = null;
    }
    if (this.options.height != null) {
      this.$elem.css({height: this.options.height});
      this.options.height = null;
    }
  },
  authenticate: function(data) {
    this.log(bDEBUG, "authenticate", "Inf call", "");
    this.authData = data;
  },
  play: function(mediaURL) {
    this.log(bDEBUG, "play", "Inf call", mediaURL);
    if (mediaURL != null) {
      this.options.playerURL = mediaURL;
    }
    //check whether isPlaying, if playing,send stop then play when stop success
    if(this.isPlaying()){
      this.waitedPlayerURL = this.options.playerURL;
      this.stop();
      this.log(bINFO, "play", "Inf A", ' A. this.playerState.isPlaying:'+this.playerState.isPlaying+' , this.playerURLinProcess:'+this.playerURLinProcess);
    }
    else if(!this.playerURLinProcess) {
      this.log(bINFO, "play", "Inf B", ' B. this.playerState.isPlaying:'+this.playerState.isPlaying+' , this.playerURLinProcess:'+this.playerURLinProcess);
      this.playerURLinProcess = true;
      this._loadIFrame();
    }
  },
  stop: function() {
    this.log(bDEBUG, "stop", "Inf call", "");
    this._sendMessage('stop');
    if (this.options.playerURL && this.options.playerURL.indexOf("requestName=Stream") > 0) {
      //for stream player, no player status update thus need force to set this.playerURLinProcess to false otherwise the player can't stop and play again(ref.line 261).
      this.playerURLinProcess = false;
    }
  },
  pause: function() {
    this.log(bDEBUG, "pause", "Inf call", "");
    this._sendMessage('pause');
  },
  resume: function() {
    this.log(bDEBUG, "resume", "Inf call", "");
    this._sendMessage('resume');
  },
  skipforward: function(ms) {
    this.log(bDEBUG, "skipforward", "Inf call", ms);
    this._sendMessage('skipforward', ms);
  },
  skipbackward: function(ms) {
    this.log(bDEBUG, "skipbackward", "Inf call", ms);
    this._sendMessage('skipbackward', ms);
  },
  catchup: function() {
    this.log(bDEBUG, "catchup", "Inf call", "");
    this._sendMessage('catchup');
  },
  seekTo: function(ms) {
    this.log(bDEBUG, "seekTo", "Inf call", ms);
    this._sendMessage('seekTo', ms);
  },
  mute: function() {
    this.log(bDEBUG, "mute", "Inf call", "");
    this._sendMessage('mute');
  },
  unmute: function() {
    this.log(bDEBUG, "unmute", "Inf call", "");
    this._sendMessage('unmute');
  },
  zoom: function(size) {
    this.log(bDEBUG, "zoom", "Inf call", size);
    this._sendMessage('zoom', size);
  },
  setLocalKBMState: function(bvalue) {
    this.log(bDEBUG, "setLocalKBMState", "Inf call", bvalue);
    this._sendMessage('setLocalKBMState', bvalue);
  },
  toggleScreen:function(){
    this.log(bDEBUG, "toggleScreen", "Inf call", "");
    //this._sendMessage('toggleScreen');
    jQuery(document).unbind('fscreenchange');
    jQuery(document).bind('fscreenchange', jQuery.proxy(this._onToggleMainScreen, this));
    if (jQuery.fullscreen.isFullScreen()) {
          jQuery.fullscreen.exit();
        }
        else {
          this.$elem.fullscreen();
        }
  },
  snapShot:function(){
    this.log(bDEBUG, "snapShot", "Inf call", "");
    this._sendMessage('snapShot');
  },
  id: function() {
    this.log(bDEBUG, "id", "Inf call", "");
    return this.options.divId;
  },
  getVersion: function() {
    this.log(bDEBUG, "getVersion", "Inf call", "");
    return this.playerState.version;
  },
  getMediaURL: function() {
    this.log(bDEBUG, "getMediaURL", "Inf call", "");
    return this.playerURL;
  },
  isPlaying: function() {
    this.log(bDEBUG, "isPlaying", "Inf call", "");
    return this.playerState.isPlaying;
  },
  isPaused: function() {
    this.log(bDEBUG, "isPaused", "Inf call", "");
    return this.playerState.isPaused;
  },
  isMuted: function() {
    this.log(bDEBUG, "isMuted", "Inf call", "");
    return this.playerState.isMuted;
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('player', BarcoPlayerInterface);