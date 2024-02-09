/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * SessionPlayer - Class definition 
 * - registered as a jQuery plugin
 */
var SessionPlayer = Class.extend({
    init: function (options, elem) {
        // Mix in the passed-in options with the default options
        this.options = jQuery.extend({}, this._defaults, options);
        this.log(bDEBUG, "init", "call", "");
        // Init state
        this.state = {
            isLoaded: false
        };
        // Save the element reference, both as a jQuery
        // reference and a normal reference
        this.elem = elem;
        this.$elem = jQuery(elem);
        // Build the DOM's initial structure
        this._build();
    },
    _defaults: {
        controlBarHeight: 64,
        playerPref: 'AUTO',
        playerURL: '',
        portNumberStart: 6070,
        portNumberRange: 990,
        portNumberSpacing: 4
    },
    _build: function () {
        this.log(bDEBUG, "_build", "call", "");
        // derive an options element for the stream player
        var options = this.options;
        // if (this.options.showControlBar != false) {
        //     options.height = this.options.height - this.options.controlBarHeight;
        // }
        if (this.options.playerUsed == 'streamPlayerChromelessHLS') {
            this.$streamPlayer = this.$elem.streamPlayerChromelessHLS(options);
        }
        else if (this.options.playerUsed == 'streamPlayerHTML5videoHLS') {
            this.$streamPlayer = this.$elem.streamPlayerHTML5videoHLS(options);
        }
        else if (this.options.playerUsed == 'streamPlayerIPV') {
            this.$streamPlayer = this.$elem.streamPlayerIPV(options);
        }

        // Add in a session control bar
        // derive an options element for the session control bar
        var options = this.options;
        if (this.options.showControlBar != false) {
            options.height = this.options.controlBarHeight;
        }
        else {
            options.height = 0;
        }
        this.$sessionController = this.$elem.sessionController(options);

        // Give them a ref to each other so they can interact
        var objStreamPlayer = this.$elem.data(this.options.playerUsed);
        var objSessionController = this.$elem.data('sessionController');

        objStreamPlayer.objSessionController = objSessionController;
        objSessionController.objStreamPlayer = objStreamPlayer;

        this.log(bINFO, "_build", "return", "Done");
    },
    destroy: function (pluginName) {
        this.log(bDEBUG, "destroy", "call", pluginName);
        // unsubscribe to events
        this.$elem.unbind("myEvent", function (e) {
            this.log(bDEBUG, "destroy", "call",
              "unsubscribed to this event");
        });

        // clean out DOM structure - needs the plugin name to be passed in
        // destroy child classes
        this.$elem.sessionController('destroy', 'sessionController');
        var objStreamPlayer = this.$elem.data(this.options.playerUsed);
        objStreamPlayer.destroy(this.options.playerUsed);
        // destroy self
        this.$elem.find('.SessionPlayer').remove();
        this.$elem.removeData(pluginName);
        this.log(bDEBUG, "destroy", "call", pluginName);
     },
    play: function (url) {
        this.log(bDEBUG, "play", "call", url);
        // send a Play command to the session controller
        var mediaURL = (url) ? url : this.options.playerURL;
        this.mediaURLObj = BarcoUtil.parseURL(mediaURL);
        if (this.mediaURLObj['DstPort.portNumber'] == null) {
          // We need to get every Nth port in this range;
          var portNumberStartSp = Math.ceil(this.options.portNumberStart / this.options.portNumberSpacing);
          var portNumberRangeSp = Math.floor(this.options.portNumberRange / this.options.portNumberSpacing);
          var portNumberSp = portNumberStartSp + Math.round(Math.random() * portNumberRangeSp);
          var portNumber = this.options.portNumberSpacing * portNumberSp;
          mediaURL += '&DstPort.portNumber=' + portNumber;
          this.log(bDEBUG, "play", "A",
            'DstPort.portNumber=' + portNumber);
        }
        if (this.mediaURLObj.requestName === 'View') {
            if (this.mediaURLObj['SrcPort.pvrEnable'] == null) {
                mediaURL += '&SrcPort.pvrEnable=true';
            }
        }
        this.log(bDEBUG, "play", "B", 'URL: ' + mediaURL);
        this.$elem.sessionController('doPlay', mediaURL);
    },
    stop: function () {
      this.log(bDEBUG, "play", "call", "");
        // send a Stop command to the session controller
      this.$elem.sessionController('doStop');
    },
    pause: function () {
        this.log(bDEBUG, "pause", "call", "");
        this.$elem.sessionController('doPause');
    },
    resume: function () {
        this.log(bDEBUG, "resume", "call", "");
        this.$elem.sessionController('doResume');
    },
    catchup: function () {
        this.log(bDEBUG, "catchup", "call", "");
        this.$elem.sessionController('doCatchup');
    },
    skipforward: function (offSet) {
        this.log(bDEBUG, "skipforward", "call", offSet);
        this.$elem.sessionController('doSkipFwd', offSet);
    },
    skipbackward: function (offSet) {
        this.log(bDEBUG, "skipbackward", "call", offSet);
        this.$elem.sessionController('doSkipBack', offSet);
    },
    seekTo: function (toOffSet) {
        this.log(bDEBUG, "seekTo", "call", toOffSet);
        this.$elem.sessionController('doSeekTo', toOffSet);
    },
    mute: function () {
        this.log(bDEBUG, "mute", "call", "");
        this.$elem.sessionController('doMute');
    },
    unmute: function () {
        this.log(bDEBUG, "unmute", "call", "");
        this.$elem.sessionController('doUnmute');
    },
    zoom: function (size) {
      this.log(bDEBUG, "zoom", "call", size);
      this.$elem.sessionController('doZoom', size);
    },
    setLocalKBMState: function (bvalue) {
      this.log(bDEBUG, "setLocalKBMState", "call", bvalue);
      this.$elem.sessionController('setLocalKBMState', bvalue);
    },
    toggleScreen: function () {
      this.log(bDEBUG, "toggleScreen", "call", "");
      this.$elem.sessionController('doToggleScreen');
    },
    snapShot: function () {
      this.log(bDEBUG, "snapShot", "call", "");
      this.$elem.sessionController('doSnapshot');
    },
    getcliplength: function () {
        this.log(bDEBUG, "getcliplength", "call", "");
        return this.$elem.sessionController('getcliplength');
    },
    isPlaying: function () {
        this.log(bDEBUG, "isPlaying", "call", "");
        return this.$elem.sessionController('isPlaying');
    },
    isPaused: function () {
        this.log(bDEBUG, "isPaused", "call", "");
        return this.$elem.sessionController('isPaused');
    },
    isMuted: function () {
        this.log(bDEBUG, "isMuted", "call", "");
        return this.$elem.sessionController('isMuted');
    },
    //doEventPublish: function (eventType, details) {
    //    this.$elem.player('doEventPublish', eventType, details);
    //}
});

// expose Objects as jQuery plugins
jQuery.plugin('sessionPlayer', SessionPlayer);