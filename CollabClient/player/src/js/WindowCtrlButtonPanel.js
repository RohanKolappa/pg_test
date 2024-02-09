/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var WindowCtrlButtonPanel = Class.extend({
  //---------Start-------------
  init: function (options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");

    // Init state
    this.objCollabInfo = {
      sessionId: '',
      state: 'NONE',
      action: 'NONE',
      recState: 'NONE',
      recAction: 'NONE',
      isTimeshifted: false,
      isSrcKbmCtrlEnabled: false
    };

    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$sessionCtl = this.$elem.closest('.SessionController').parent();

    // Build the DOM's initial structure
    this._build();
  },
  _defaults: {
    width: 200,
    height: 60
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    // Add window controls
    var html = "";
    if (this.options.playerUsed === 'streamPlayerIPV') {
      html += '<a href="#" id="' + this.options.divId + '_kbm" class="hidden"></a>';
      html += '<a href="#" id="' + this.options.divId + '_snapshot" class="hidden"></a>';
      html += '<a href="#" id="' + this.options.divId + '_zoom" class="hidden"></a>';
    }
    html += '<a href="#" id="' + this.options.divId + '_muteUnmute" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_fullscr" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_pinUnpin" class="hidden"></a>';

    //html += '</div> <!-- blkSessionButtonContainer -->';
    this.$elem.append(html);
    // Stash refs to buttons and Hook event handlers to buttons
    this.ctrls = {};
    // Fullscreen
    this.ctrls.$fullscr = this.$elem.find('#' + this.options.divId + '_fullscr');
    this.ctrls.$fullscr.on('click', jQuery.proxy(this._onClick_fullscr, this));
    // IPVPlayer
    if (this.options.playerUsed === 'streamPlayerIPV') {
      // Snapshot
      this.ctrls.$snapshot = this.$elem.find('#' + this.options.divId + '_snapshot');
      this.ctrls.$snapshot.on('click', jQuery.proxy(this._onClick_snapshot, this));
      // zoom display
      this.ctrls.$zoom = this.$elem.find('#' + this.options.divId + '_zoom');
      this.ctrls.$zoom.on('click', jQuery.proxy(this._onClick_zoom, this));
      //KBM control
      this.ctrls.$kbm = this.$elem.find('#' + this.options.divId + '_kbm');
      this.ctrls.$kbm.on('click', jQuery.proxy(this._onClick_kbm, this));
    }
    // mute/unmute
    this.ctrls.$muteUnmute = this.$elem.find('#' + this.options.divId + '_muteUnmute');
    this.ctrls.$muteUnmute.on('click', jQuery.proxy(this._onClick_muteUnmute, this));

    //pin/unpin
    this.ctrls.$pinUnpin = this.$elem.find('#' + this.options.divId + '_pinUnpin');
    this.ctrls.$pinUnpin.on('click', jQuery.proxy(this._onClick_pinUnpin, this));

    this.log(bINFO, "_build", "return", "Done");
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.find('.WindowCtrlButtonPanel').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
  },

  // button click mute/Unmute
  _onClick_muteUnmute: function (e) {
    this.log(bDEBUG, "_onClick_muteUnmute", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      this.$sessionCtl.sessionController('doMuteUnmute');
    }
    e.preventDefault();
  },

  onMuteUnmute: function (isMuted) {
    this.log(bDEBUG, "onMuteUnmute", "call", "");
    if (isMuted) {
      this.ctrls.$muteUnmute.removeClass('muteBtn-enabled').addClass("unmuteBtn-enabled");
    } else {
      this.ctrls.$muteUnmute.removeClass('unmuteBtn-enabled').addClass("muteBtn-enabled");      
    }
  },

  // button click pin/Unpin
  _onClick_pinUnpin: function (e) {
    this.log(bDEBUG, "_onClick_pinUnpin", "call", "");
    this.$sessionCtl.sessionController('doPinUnpin');
    e.preventDefault();
  },

  onPinUnpin: function (isPinned) {
    this.log(bDEBUG, "onPinUnpin", "call", "");
    if (isPinned) {
      this.ctrls.$pinUnpin.removeClass('pinBtn-enabled').addClass("unpinBtn-enabled");
    } else {
      this.ctrls.$pinUnpin.removeClass('unpinBtn-enabled').addClass("pinBtn-enabled");
    }
  },

  // button click FullScreen 
  _onClick_fullscr: function (e) {
    this.log(bDEBUG, "_onClick_fullscr", "call", "");
    this.$sessionCtl.sessionController('doToggleScreen');
    e.preventDefault();
  },

  onToggleScreen: function (isFullScreen) {
    this.log(bDEBUG, "onToggleScreen", "call", "");
    if (isFullScreen) {
      this.ctrls.$fullscr.removeClass('fullscreenOnBtn-enabled').addClass("fullscreenOffBtn-enabled");
    } else {
      this.ctrls.$fullscr.removeClass('fullscreenOffBtn-enabled').addClass("fullscreenOnBtn-enabled");
    }
  },

  // button click SkipBack
  _onClick_snapshot: function (e) {
    this.log(bDEBUG, "_onClick_snapshot", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      this.$sessionCtl.sessionController('doSnapshot');
    }
    e.preventDefault();
  },

  _onClick_zoom: function (e) {
    this.log(bDEBUG, "_onClick_zoom", "call", "");
    if (this.objCollabInfo.state === "STREAMING") {
      var pos = this.ctrls.$zoom.position();
      var imgWidth = this.ctrls.$zoom.width();
      pos.left = pos.left + Math.floor(imgWidth/2) - 3;
      this.log(bDEBUG, "_onClick_zoom", "block", "@@@@@@ pos.left=" + pos.left + "pos.top=" + pos.top);
      this.$sessionCtl.sessionController('doZoomPopup', pos);
    }
    e.preventDefault();
  },
  //click KBM control
  _onClick_kbm: function (e) {
    this.log(bDEBUG, "_onClick_kbm", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      this.$sessionCtl.sessionController('doKBM');
    }
    e.preventDefault();
  },
  
  onKbmOnOff: function (isKbmSuccess) {
    this.log(bDEBUG, "onKbmOnOff", "call", "");
    if (isKbmSuccess) {
      this.ctrls.$kbm.removeClass('kbmBtn-On-enabled').addClass('kbmBtn-Off-enabled');
    }
    else {
      this.ctrls.$kbm.removeClass('kbmBtn-Off-enabled').addClass('kbmBtn-On-enabled');
    }
  },
  
  applyCollaborationInfo: function (objCollabInfo) {
    this.log(bDEBUG, "applyCollaborationInfo", "call", "");
    this.objCollabInfo.sessionId = objCollabInfo.sessionId;
    this.objCollabInfo.state = objCollabInfo.state;
    this.objCollabInfo.action = objCollabInfo.action;
    this.objCollabInfo.recState = objCollabInfo.recState;
    this.objCollabInfo.recAction = objCollabInfo.recAction;
    this.objCollabInfo.isTimeshifted = objCollabInfo.isTimeshifted;
    if (objCollabInfo.srcKbmCtrl) this.objCollabInfo.isSrcKbmCtrlEnabled = objCollabInfo.srcKbmCtrl;

    //btns state
    if (objCollabInfo.state == 'STREAMING') {
      if (this.ctrls.$fullscr.hasClass('hidden')) this.ctrls.$fullscr.removeClass('hidden').addClass("fullscreenOnBtn-enabled");
      if (this.ctrls.$snapshot && this.ctrls.$snapshot.hasClass('hidden'))
        this.ctrls.$snapshot.removeClass('hidden').addClass("snapBtn-enabled");
      if (this.ctrls.$zoom && this.ctrls.$zoom.hasClass('hidden'))
        this.ctrls.$zoom.removeClass('hidden').addClass("zoomBtn-enabled");
      if (this.objCollabInfo.isSrcKbmCtrlEnabled && this.ctrls.$kbm && this.ctrls.$kbm.hasClass('hidden'))
        this.ctrls.$kbm.removeClass('hidden').addClass("kbmBtn-On-enabled");
      if (this.options.platform !== 'iOS') {
        if (this.ctrls.$muteUnmute.hasClass('hidden')) this.ctrls.$muteUnmute.removeClass('hidden').addClass("muteBtn-enabled");
      }
      if (!this.options.controllerBarFixed && this.ctrls.$pinUnpin.hasClass('hidden')) this.ctrls.$pinUnpin.removeClass('hidden').addClass('pinBtn-enabled');
    }
    else {
      this.ctrls.$fullscr.removeClass('fullscreenOnBtn-enabled fullscreenOffBtn-enabled').addClass('hidden');
      if (this.ctrls.$snapshot)
        this.ctrls.$snapshot.removeClass('snapBtn-enabled').addClass('hidden');
      if (this.ctrls.$zoom) {
        this.ctrls.$zoom.removeClass('zoomBtn-enabled').addClass("hidden");
        this.$sessionCtl.sessionController('doZoomPopup', 0, true);
      }
      if (this.ctrls.$kbm)
        this.ctrls.$kbm.removeClass('kbmBtn-On-enabled kbmBtn-Off-enabled').addClass('hidden');
      this.ctrls.$muteUnmute.removeClass('unmuteBtn-enabled muteBtn-enabled').addClass('hidden');
      this.ctrls.$pinUnpin.removeClass('pinBtn-enabled unpinBtn-enabled').addClass('hidden');
    }
  }
  //------End------------------

});
jQuery.plugin('windowCtrlButtonPanel', WindowCtrlButtonPanel);
