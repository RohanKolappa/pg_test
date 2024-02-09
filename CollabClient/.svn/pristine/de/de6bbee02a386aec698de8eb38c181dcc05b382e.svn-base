/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * BarcoPlayer - Class definition 
 * - registered as a jQuery plugin
 */
var BarcoPlayer = Class.extend({
  init: function (options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    // Init state
    this.player = null;
    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.options.divId = this.$elem.attr('id');
    this.log(bDEBUG, "init", "call", "");

    // Build the DOM's initial structure
    this._build();
  },
  _defaults: {
    sessionMessagePanelType: null,
    sessionMessagePanel: null,
    playerURL: '',
    playerPref: 'AUTO',
    playerUsed: null, // Do not change this manually - set automatically
    isLoaded: null,
    skipforwardOffset: 10000,
    skipbackwardOffset: 10000,
    controllerBarFixed: false
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    var thisObj = this;
    // stash divId, generate a new random playerId
    this.options.playerId = 'player_' + Math.floor((Math.random() * 100000) + 1);
    // set player background color to black
    this.$elem.css({ 'background-color': 'black' });
    // set player position - needed for overlay control bar
    if (jQuery.isBlank(this.$elem.attr('position'))) {
      this.$elem.css({ position: 'relative' });
    }
    // set player dimensions
    if (this.options.width != null) {
      this.$elem.css({ width: this.options.width });
    }
    if (this.options.height != null) {
      this.$elem.css({ height: this.options.height });
    }
    this.log(bINFO, "_build", "return", "Done");
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events

    // clean out DOM structure
    // destroy child classes
    if (this.options.isLoaded) {
      if (this.options.streamURL) {
        this.objPlayer('destory', this.options.playerUsed);
      } else {
        this.$elem.sessionPlayer('destroy', 'sessionPlayer');
      }
    }
    if (this.options.sessionMessagePanelType === 'sessionMessagePanelDefault') {
      this.options.sessionMessagePanel('destroy', this.options.sessionMessagePanelType);
    }
    if (this.skinner) {
      this.$elem.skinner('destroy', 'skinner');
      this.skinner = null;
    }
    if (this.playerSelection) {
      this.$elem.playerSelection('destroy', 'playerSelection');
      this.playerSelecction = null;
    }
    // destroy self
    this.$elem.find('.BarcoPlayer').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
  },
  _loadSessionPlayer: function () {
    this.log(bDEBUG, "_loadSessionPlayer", "call", "");
    // instantiate session player
    this.$elem.sessionPlayer(this.options);
    this.objPlayer = this.$elem.data('sessionPlayer');
    this.options.isLoaded = 'SessionPlayer';
  },
  _loadStreamPlayer: function () {
    this.log(bDEBUG, "_loadStreamPlayer", "call", "");
    if (this.options.playerUsed == 'streamPlayerChromelessHLS') {
      this.$elem.streamPlayerChromelessHLS(this.options);
      this.objPlayer = this.$elem.data('streamPlayerChromelessHLS');
    }
    else if (this.options.playerUsed == 'streamPlayerHTML5videoHLS') {
      this.$elem.streamPlayerHTML5videoHLS(this.options);
      this.objPlayer = this.$elem.data('streamPlayerHTML5videoHLS');
    }
    else if (this.options.playerUsed == 'streamPlayerIPV') {
      this.$elem.streamPlayerIPV(this.options);
      this.objPlayer = this.$elem.data('streamPlayerIPV');
    }
    this.options.isLoaded = 'SteamPlayer';
  },
  conf: function (options) {
    this.log(bDEBUG, "conf", "call", "");
    this.options = jQuery.extend({}, this.options, options);
    // set player dimensions
    if (this.options.width != null) {
      this.$elem.css({ width: this.options.width });
    }
    if (this.options.height != null) {
      this.$elem.css({ height: this.options.height });
    }
  },
  authenticate: function (data) {
    this.log(bDEBUG, "authenticate", "call", "");
    var base64Encode = 'Basic ' + BarcoUtil.b64encode(data.username + ':' + data.password);
    var barcoResource = data.resource + '_' + data.username;
    var options = { authHeaderObj: {
      'Content-Type': 'application/json',
      'Authorization': base64Encode,
      'Accept': 'application/json',
      'X-Barco-resource': barcoResource
    }
    };
    this.options = jQuery.extend({}, this.options, options);
  },
  play: function (mediaURL) {
    this.log(bDEBUG, "play", "call", mediaURL);
    // unload previous session or stream player, if any. Not necessary when
    // switching between the same stream type, But, required when switching to
    // different stream types that require an different stream player.
    if (this.options.isLoaded) {
      if (this.options.streamURL) {
        this.objPlayer('destory', this.options.playerUsed);
      } else {
        this.$elem.sessionPlayer('destroy', 'sessionPlayer');
      }
      this.options.isLoaded = false;
    }
    // clear any messages from previous player detection
    this.$elem.find('.PlayerDetection').remove();
    // if mediaURL is passed use it - else specified before in conf
    if (mediaURL != null) {
      this.options.playerURL = mediaURL;
    }
    // if no playerURL = error
    if (!this.options.playerURL) {
      this.log(bERROR, "_loadStreamPlayer", "call",
        "No playerURL: " + this.options.playerURL);
      this.doEventPublish("onError", { type: "Error", message: ' No player URL ' });
      return;
    }
    if (this.skinner == null && this.options.customSkin != null) {
      this.$elem.skinner(this.options);
      this.skinner = this.$elem.data('skinner');
    }
    if (this.playerSelection == null) {
      this.$elem.playerSelection(this.options);
      this.playerSelection = this.$elem.data('playerSelection');
    } else {
      this.playerSelection.options.playerURL = this.options.playerURL;
    }
    this.options.platform = this.playerSelection.getPlatform();
    var abortPlay = this.playerSelection.doPlayerSelectionActions();
    if (abortPlay) {
      this.playerSelection.showMessage();
      this.doEventPublish("onError", this.playerSelection.getErrorEvent());
      return;
    }
    this.options.playerUsed = this.playerSelection.getPlayerUsed();
    // select messagePanel - unless forced by user
    if (this.options.sessionMessagePanel == null) {
      // Use our popup SessionMessagePanel by default if not previously defined
      this.$elem.sessionMessagePanelDefault(this.options);
      this.options.sessionMessagePanelType = 'sessionMessagePanelDefault';
      this.options.sessionMessagePanel = this.$elem.data(this.options.sessionMessagePanelType);
    }
    if (this.options.playerURL.indexOf("/clients/player?") > 0) {
      if (!this.options.isLoaded) {
        // load session player
        this._loadSessionPlayer();
      }
    }
    // if streamingURL
    else {
      this.options.streamURL = this.options.playerURL;
      if (!this.options.isLoaded) {
        // load stream player if don't load any
        this._loadStreamPlayer();
      }
    }
    // do play
    this.objPlayer.play(this.options.playerURL);
  },
  stop: function () {
    this.log(bDEBUG, "stop", "call", "");
    this.objPlayer.stop();
  },
  pause: function () {
    this.log(bDEBUG, "pause", "call", "");
    this.objPlayer.pause();
  },
  resume: function () {
    this.log(bDEBUG, "resume", "call", "");
    this.objPlayer.resume();
  },
  skipforward: function (ms) {
    this.log(bDEBUG, "skipforward", "call", ms);
    this.objPlayer.skipforward(ms);
  },
  skipbackward: function (ms) {
    this.log(bDEBUG, "skipbackward", "call", ms);
    this.objPlayer.skipbackward(ms);
  },
  catchup: function () {
    this.log(bDEBUG, "skipbackward", "call", "");
    this.objPlayer.catchup();
  },
  seekTo: function (ms) {
    this.log(bDEBUG, "seekTo", "call", ms);
    this.objPlayer.seekTo(ms);
  },
  mute: function () {
    this.log(bDEBUG, "mute", "call", "");
    this.objPlayer.mute();
  },
  unmute: function () {
    this.log(bDEBUG, "unmute", "call", "");
    this.objPlayer.unmute();
  },
  zoom: function (size) {
    if (this.options.playerUsed === 'streamPlayerIPV') {
      this.log(bDEBUG, "zoom", "call", size);
      this.objPlayer.zoom(size);
    }
  },
  setLocalKBMState: function (bvalue) {
    if (this.options.playerUsed === 'streamPlayerIPV') {
      this.log(bDEBUG, "setLocalKBMState", "call", bvalue);
      this.objPlayer.setLocalKBMState(bvalue);
    }
  },
  toggleScreen: function () {
    this.log(bDEBUG, "toggleScreen", "call", "");
    this.objPlayer.toggleScreen();
  },
  snapShot: function () {
    if (this.options.playerUsed === 'streamPlayerIPV') {
      this.log(bDEBUG, "snapShot", "call", "");
      this.objPlayer.snapShot();
    }
  },
  id: function () {
    this.log(bDEBUG, "id", "call", "");
    return this.options.divId;
  },
  getVersion: function () {
    this.log(bDEBUG, "getVersion", "call", "");
    return "%SERVER.VERSION%";
  },
  getPlayerId: function () {
    this.log(bDEBUG, "getPlayerId", "call", "");
    return this.options.playerId;
  },
  getMediaURL: function () {
    this.log(bDEBUG, "getMediaURL", "call", "");
    return this.options.playerURL;
  },
  getcliplength: function () {
    this.log(bDEBUG, "getcliplength", "call", "");
    return this.objPlayer.getcliplength();
  },
  isPlaying: function () {
    this.log(bDEBUG, "isPlaying", "call", "");
    if (!this.options.isLoaded)
      return false;
    return this.objPlayer.isPlaying();
  },
  isPaused: function () {
    this.log(bDEBUG, "isPaused", "call", "");
    if (!this.options.isLoaded)
      return false;
    return this.objPlayer.isPaused();
  },
  isMuted: function () {
    this.log(bDEBUG, "isMuted", "call", "");
    return this.objPlayer.isMuted();
  },
  doEventPublish: function (eventType, details) {
    this.log(bDEBUG, "doEventPublish", "call", eventType);
    var data = jQuery.extend({ id: this.options.divId, playerId: this.options.playerId, type: "Event" }, details);
    jQuery.publish(eventType, data);
  }
});
// expose Objects as jQuery plugins
jQuery.plugin('player', BarcoPlayer);