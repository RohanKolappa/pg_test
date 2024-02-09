/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var SessionCtrlButtonPanel = Class.extend({
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
      isTimeshifted : false
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
    width: 440,
    height: 60
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    // Add a container for session controls
    //var html = '<div class="blkSessionButtonContainer">';
    var html = '';
	if( this.options.demoMode ) {
		html += '<a href="#" id="' + this.options.divId + '_share" class="hidden"></a>';
	}
	html += '<a href="#" id="' + this.options.divId + '_stopStart" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_skipBack" class="hidden"></a>';
	html += '<a href="#" id="' + this.options.divId + '_pauseResume" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_skipFwd" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_record" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_catchup" class="hidden"></a>';
    if (this.options.showBookmarkCreation) {
      html += '<a href="#" id="' + this.options.divId + '_addBookmark" class="hidden"></a>';
      html += '<a href="#" id="' + this.options.divId + '_editBookmark" class="hidden"></a>';
      html += '<a href="#" id="' + this.options.divId + '_deleteBookmark" class="hidden"></a>';
    }
    if (this.options.isEditAllowed) {
      html += '<a href="#" id="' + this.options.divId + '_editMedia" class="hidden"></a>';
    }
    this.$elem.append(html);
    // Stash refs to buttons and Hook event handlers to buttons
    this.ctrls = {};
	if( this.options.demoMode ) {
		// Share
		this.ctrls.$share = this.$elem.find('#' + this.options.divId + '_share');
		this.ctrls.$share.on('click', jQuery.proxy(this._onClick_share, this));
		this.ctrls.$share.removeClass('hidden').addClass('shareBtn-enabled');
	}
    // Stop/Start
    this.ctrls.$stopStart = this.$elem.find('#' + this.options.divId + '_stopStart');
    this.ctrls.$stopStart.on('click', jQuery.proxy(this._onClick_stopStart, this));
    // Pause/Resume
    this.ctrls.$pauseResume = this.$elem.find('#' + this.options.divId + '_pauseResume');
    this.ctrls.$pauseResume.on('click', jQuery.proxy(this._onClick_pauseResume, this));
    // Skipbackward
    this.ctrls.$skipBack = this.$elem.find('#' + this.options.divId + '_skipBack');
    this.ctrls.$skipBack.on('click', jQuery.proxy(this._onClick_skipBack, this));
    // Skipfwd
    this.ctrls.$skipFwd = this.$elem.find('#' + this.options.divId + '_skipFwd');
    this.ctrls.$skipFwd.on('click', jQuery.proxy(this._onClick_skipFwd, this));
    // IPVPlayer
    //if (this.options.playerUsed === 'streamPlayerIPV') {
      // Catchup
      this.ctrls.$catchup = this.$elem.find('#' + this.options.divId + '_catchup');
      this.ctrls.$catchup.on('click', jQuery.proxy(this._onClick_catchup, this));
   // }
    // Record
    this.ctrls.$record = this.$elem.find('#' + this.options.divId + '_record');
    this.ctrls.$record.on('click', jQuery.proxy(this._onClick_record, this));
    //bookmarks
    if (this.options.showBookmarkCreation) {
      this.ctrls.$addBookmark = this.$elem.find('#' + this.options.divId + '_addBookmark');
      this.ctrls.$addBookmark.on('click', jQuery.proxy(this._onClick_addBookmark, this));

      this.ctrls.$editBookmark = this.$elem.find('#' + this.options.divId + '_editBookmark');
      this.ctrls.$editBookmark.on('click', jQuery.proxy(this._onClick_editBookmark, this));

      this.ctrls.$deleteBookmark = this.$elem.find('#' + this.options.divId + '_deleteBookmark');
      this.ctrls.$deleteBookmark.on('click', jQuery.proxy(this._onClick_deleteBookmark, this));
    }
    if (this.options.isEditAllowed) {
      this.ctrls.$editMedia = this.$elem.find('#' + this.options.divId + '_editMedia');
      this.ctrls.$editMedia.on('click', jQuery.proxy(this._onClick_editMedia, this));
    }

    this.log(bINFO, "_build", "return", "Done");
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.find('.SessionCtrlButtonPanel').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "return", pluginName);
   },

  // button click Share 
  _onClick_share: function (e) {
    this.log(bDEBUG, "_onClick_share", "call", "");
    this.$sessionCtl.sessionController('doShare');
    e.preventDefault();
  },
  
  // button click Stop/Start 
  _onClick_stopStart: function (e) {
    this.log(bDEBUG, "_onClick_stopStart", "call", "");
    if (this.objCollabInfo.state == "STREAMING" || this.objCollabInfo.state == "AUTHORIZING") {
      this.$sessionCtl.sessionController('doStop');
    } else {
      this.$sessionCtl.sessionController('doPlay', null);
    }
    e.preventDefault();
  },

  // button click Pause/Resume
  _onClick_pauseResume: function (e) {
    this.log(bDEBUG, "_onClick_pauseResume", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      if (this.objCollabInfo.action == "Pause") {
        this.$sessionCtl.sessionController('doResume');
      } else {
        this.$sessionCtl.sessionController('doPause');
      }
    }
    e.preventDefault();
  },

  // button click SkipBack
  _onClick_skipBack: function (e) {
    this.log(bDEBUG, "_onClick_skipBack", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      this.$sessionCtl.sessionController('doSkipBack');
    }
    e.preventDefault();
  },

  // button click Skipfwd 
  _onClick_skipFwd: function (e) {
    this.log(bDEBUG, "_onClick_skipFwd", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      this.$sessionCtl.sessionController('doSkipFwd');
    }
    e.preventDefault();
  },

  // button click Catchup
  _onClick_catchup: function (e) {
    this.log(bDEBUG, "_onClick_catchup", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      this.$sessionCtl.sessionController('doCatchup');
    }
    e.preventDefault();
  },

  // button click Record/Stop
  _onClick_record: function (e) {
    this.log(bDEBUG, "_onClick_record", "call", "");
    if (this.objCollabInfo.state == "STREAMING") {
      if (this.objCollabInfo.recState == "STREAMING") {
        this.$sessionCtl.sessionController('doStopRecord');
      }
      else {
        this.$sessionCtl.sessionController('doRecord');
      }
    }
    e.preventDefault();
  },

  _onClick_addBookmark: function (e) {
    this.log(bDEBUG, "_onClick_addBookmark", "call", "");
    this.$sessionCtl.sessionController('doAddBookmark');
    e.preventDefault();
  },

  _onClick_editBookmark: function (e) {
    this.log(bDEBUG, "_onClick_editBookmark", "call", "");
    this.$sessionCtl.sessionController('doEditBookmark');
    e.preventDefault();
  },

  _onClick_deleteBookmark: function (e) {
    this.log(bDEBUG, "_onClick_deleteBookmark", "call", "");
    this.$sessionCtl.sessionController('doDeleteBookmark');
    e.preventDefault();
  },

  onBookmarkSelected: function (bookmarkNID) {
    if (this.options.showBookmarkCreation && !jQuery.isBlank(bookmarkNID)) {
      this.ctrls.$editBookmark.removeClass('hidden').addClass("editBookmarkBtn-enabled");
      this.ctrls.$deleteBookmark.removeClass('hidden').addClass("deleteBookmarkBtn-enabled");
    }
    else {
      this.ctrls.$editBookmark.removeClass('editBookmarkBtn-enabled').addClass("hidden");
      this.ctrls.$deleteBookmark.removeClass('deleteBookmarkBtn-enabled').addClass("hidden");
    }
  },

  _onClick_editMedia: function (e) {
    this.log(bDEBUG, "_onClick_editMedia", "call", "");
    this.$sessionCtl.sessionController('editMedia');
    e.preventDefault();
  },

  applyCollaborationInfo: function (objCollabInfo) {
    this.log(bDEBUG, "applyCollaborationInfo", "call", "");
    this.objCollabInfo.sessionId = objCollabInfo.sessionId;
    this.objCollabInfo.state = objCollabInfo.state;
    this.objCollabInfo.action = objCollabInfo.action;
    this.objCollabInfo.recState = objCollabInfo.recState;
    this.objCollabInfo.recAction = objCollabInfo.recAction;
    this.objCollabInfo.isTimeshifted = objCollabInfo.isTimeshifted;
    //btns state
    if (objCollabInfo.state == 'STREAMING') {
      //Play/Stop btn
      this.ctrls.$stopStart.removeClass('hidden playBtn-enabled').addClass('stopBtn-enabled');
      if (objCollabInfo.pvrInfo.canPVR) {
        //Paues/resume btn
        if (objCollabInfo.action != "Pause") {
          this.ctrls.$pauseResume.removeClass("hidden resumeBtn-enabled").addClass("pauseBtn-enabled");
        }
        else {
          this.ctrls.$pauseResume.removeClass("hidden pauseBtn-enabled").addClass("resumeBtn-enabled");
        }

        //SkipBack btn
        if (this.options.playerUsed != "streamPlayerHTML5videoHLS" || objCollabInfo.timeMode == "PLAYBACK_BUSY" || objCollabInfo.timeMode == "PLAYBACK") {
          this.ctrls.$skipBack.removeClass('hidden').addClass("skipBackBtn-enabled");
          this.ctrls.$skipFwd.removeClass('hidden').addClass("skipForwardBtn-enabled");
        }
      }
      //Catchup btn
      if (objCollabInfo.timeMode != "PLAYBACK") {
        if (objCollabInfo.pvrInfo.canPVR) {
          if (this.ctrls.$catchup) {
            //Check for time shifted to show/hide catchup btn
            //In IPAD/HTML5 seek is not working consistently
            //Hide Catchup btn on IPAD even for for LIVE/PVR
            if ((objCollabInfo.isTimeshifted && this.options.playerUsed != "streamPlayerHTML5videoHLS") ||
                (objCollabInfo.isTimeshifted && objCollabInfo.timeMode == "PLAYBACK_BUSY")) {
              this.ctrls.$catchup.removeClass('hidden').addClass('catchupBtn-enabled');
            }
            else {
              this.ctrls.$catchup.removeClass('catchupBtn-enabled').addClass('hidden');
            }
          }
        }
      }
      else {
        if (this.ctrls.$catchup) this.ctrls.$catchup.addClass('hidden');
      }
      //Catch Up/Record btn
      if (objCollabInfo.timeMode != "PLAYBACK" && objCollabInfo.timeMode != "PLAYBACK_BUSY") {
        if (this.options.showRecordingControls) {
          //Check for recording state
          if (this.objCollabInfo.recState == 'STREAMING') {
            this.ctrls.$record.removeClass('hidden recordingBtn-off').addClass('recordingBtn-on');
            if (this.options.showBookmarkCreation)
              this.ctrls.$addBookmark.removeClass('hidden').addClass("addBookmarkBtn-enabled");
          }
          else {
              this.ctrls.$record.removeClass('hidden recordingBtn-on').addClass('recordingBtn-off');
              if (this.options.showBookmarkCreation)
                this.ctrls.$addBookmark.removeClass('addBookmarkBtn-enabled').addClass("hidden");
          }
        }
      }
      else {
        this.ctrls.$record.removeClass('recordingBtn-off recordingBtn-on').addClass('hidden');
        if (this.options.showBookmarkCreation) {
          this.ctrls.$addBookmark.removeClass('hidden').addClass("addBookmarkBtn-enabled");
          if (objCollabInfo.timeMode == "PLAYBACK" && this.options.isEditAllowed) {
            this.ctrls.$editMedia.removeClass('hidden').addClass("trimBtn-enabled");
          }
        }
      }
    }
    else if (objCollabInfo.state == 'AUTHORIZING') {
      this.ctrls.$stopStart.removeClass('hidden playBtn-enabled').addClass('stopBtn-enabled');
    }
    else {
      this.ctrls.$stopStart.removeClass('hidden stopBtn-enabled').addClass('playBtn-enabled');
      this.ctrls.$pauseResume.removeClass("pauseBtn-enabled resumeBtn-enabled").addClass("hidden");
      this.ctrls.$skipBack.removeClass("skipBackBtn-enabled").addClass('hidden');
      this.ctrls.$skipFwd.removeClass("skipForwardBtn-enabled").addClass('hidden');
      this.ctrls.$catchup.removeClass("catchupBtn-enabled").addClass('hidden');
      this.ctrls.$record.removeClass('recordingBtn-off recordingBtn-on').addClass('hidden');
      if (this.options.showBookmarkCreation) {
        this.ctrls.$addBookmark.removeClass('addBookmarkBtn-enabled').addClass('hidden');
        this.ctrls.$editBookmark.removeClass('editBookmarkBtn-enabled').addClass('hidden');
        this.ctrls.$deleteBookmark.removeClass('deleteBookmarkBtn-enabled').addClass('hidden');
      }
      if (this.options.isEditAllowed) {
        this.ctrls.$editMedia.removeClass('trimBtn-enabled').addClass("hidden");
      }
    }
  }
  //------End------------------
	
});
jQuery.plugin('sessionCtrlButtonPanel', SessionCtrlButtonPanel);
