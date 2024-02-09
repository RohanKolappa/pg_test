/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */
/*
 * SessionController - Class definition 
 * - registered as a jQuery plugin
 */
var SessionController = Class.extend({
  init: function (options, elem) {
      // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");
    //Check options to show/hide controllerbar/record/bookmarks btns
    //While sending these options as query paramenters those are String instead of Boolean
    //Checking the String and also handling case sensitive value to make sure final value will be Boolean
    if (jQuery.isBlank(this.options.showControlBar)) {
      this.options.showControlBar = true;
    }
    else {
      this.options.showControlBar = (String(this.options.showControlBar).toLowerCase() === 'true') ? true : false;
    }

    if (jQuery.isBlank(this.options.showRecordingControls)) { 
      this.options.showRecordingControls = false;
    }
    else {
      this.options.showRecordingControls = (String(this.options.showRecordingControls).toLowerCase() === 'true') ? true : false;
    }

    if (jQuery.isBlank(this.options.showBookmarkCreation)) {
      this.options.showBookmarkCreation = true;
    }
    else {
      this.options.showBookmarkCreation = (String(this.options.showBookmarkCreation).toLowerCase() === 'true') ? true : false;
    }

    if (jQuery.isBlank(this.options.showBookmarkPlayback)) {
      this.options.showBookmarkPlayback = true;
    }
    else {
      this.options.showBookmarkPlayback = (String(this.options.showBookmarkPlayback).toLowerCase() === 'true') ? true : false;
    }

    this.options.isEditAllowed = false;
    if (!jQuery.isBlank(this.options.authHeaderObj)) this.options.isEditAllowed = true;

    if (jQuery.isBlank(this.options.controllerBarFixed)) {
      this.options.controllerBarFixed = false;
    }
    else {
      this.options.controllerBarFixed = (String(this.options.controllerBarFixed).toLowerCase() === 'true') ? true : false;
    }
    

    // Init state
    this.state = {
      sessionId: '',
      isPlaying: false,
      streamURL: '',
      isMuted: false,
      isFullScr: false,
      isResFit: true,
      zoomSel: "Fit",
      isKbmEnabled: false,
      bookmarkNID: null,
      errorStates: ['END_OF_MEDIA', 'SETUP_ERROR', 'START_ERROR',
                    'SOURCE_ERROR', 'MEDIA_ERROR', 'DEST_ERROR', 'WAITING_FOR_SOURCE-DEST_ACKNOWLEDGED'],
      errorState: null,
      isBarPinned: false,
      barAnimationIdleTime: 0,
      barOpenState: true,
      barTimerId: null,
      barForceOpen: false,
      streamLatency: 0
    };
    this.END_OF_MEDIA = 112;

    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);

    // Instantiate an Agent and register notification callback handler
    this.sessionInstance = new SessionAPI(jQuery.proxy(this.handleStartedCallback, this), 
										jQuery.proxy(this.handleActionCallback, this),
										jQuery.proxy(this.handleEventCallback, this), 
										jQuery.proxy(this.handleErrorCallback, this));
    this.options.barcoAPI = new BarcoAPI();
    //this.options.barcoAPI.registerCallback(this.handleAgentNotification); //reqParams.sessionURL	
    //this.sessionAgent = new SessionAgent();
    //this.sessionAgent.StartURL({sessionURL:});
    //
    // Build the DOM's initial structure
    this._build();
  },
  _defaults: {
    //clip 10000 ms
    skipOffset: 10000,
    timeScale: "1"
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    if (this.options.showControlBar) {
      // Add a container for session controls
      var child = jQuery('<div id="' + this.options.divId + '_buttonOverlay' +
        '" class="button-overlay" style="height: ' +
        this.options.controlBarHeight + 'px;"></div>').addClass('SessionController');
      this.$elem.append(child);
      // Add the session controls inside it
      var html = '<div id="' + this.options.divId + 'blkSessionSlider" class="blkSessionSlider"></div>';
      child.append(html);
      this.$sliderElem = this.$elem.find('#' + this.options.divId + 'blkSessionSlider');
      this.$sliderElem.sessionSlider(this.options);

      if (this.options.isEditAllowed) {
        var html = '<div id="' + this.options.divId + 'blkEditMediaPanelContainer" class="hidden"></div>';
        child.append(html);
        this.$editMediaPanelElem = this.$elem.find('#' + this.options.divId + 'blkEditMediaPanelContainer');
        this.$editMediaPanelElem.editMediaPanel(this.options);
      }

      var html = '<div id="' + this.options.divId + 'blkSessionButtonContainer" class="blkSessionButtonContainer"></div>';
      child.append(html);
      this.$sessionBtnElem = this.$elem.find('#' + this.options.divId + 'blkSessionButtonContainer');
      this.$sessionBtnElem.sessionCtrlButtonPanel(this.options);

      var html = '<div id="' + this.options.divId + 'blkWindowButtonContainer" class="blkWindowButtonContainer"></div>';
      child.append(html);
      this.$windowBtnElem = this.$elem.find('#' + this.options.divId + 'blkWindowButtonContainer');
      this.$windowBtnElem.windowCtrlButtonPanel(this.options);

      if (this.options.showBookmarkCreation) {
        var html = '<div id="' + this.options.divId + 'blkBookmarkPanelContainer" class="blkBookmarkPanelContainer"></div>';
        child.append(html);
        this.$bookmarkPanelElem = this.$elem.find('#' + this.options.divId + 'blkBookmarkPanelContainer');
        this.$bookmarkPanelElem.sessionCtrlBookmarkPanel(this.options);
      }
      if (this.options.playerUsed === 'streamPlayerIPV') {
        var html = '<div id="' + this.options.divId + 'blkZoomPanelContainer" class="blkZoomPanelContainer"></div>';
        child.append(html);
        this.$zoomPanelElem = this.$elem.find('#' + this.options.divId + 'blkZoomPanelContainer');
        this.$zoomPanelElem.sessionCtrlZoomPanel(this.options);
      }
    }
    //jQuery(document).bind('fscreenchange', jQuery.proxy(this.onToggleScreen, this));
//    jQuery(document).bind('onRFBEnable', jQuery.proxy(this.onToggleKBMButton, this));
    this.buttonOverlayAnimationSetup();
    this.log(bINFO, "_build", "return", "Done");
  },

  buttonOverlayAnimationSetup: function () {
    this.state.isBarPinned = true;
    this.state.barAnimationIdleTime = 0;
    this.state.barOpenState = true;
    this.state.barTimerId = null;
    this.state.barForceOpen = false;
    this.options.controllerBarFixed
    //if config for controller bar pinned mode is true then dont subscribe any mouse events 
    if (this.options.controllerBarFixed) {
      var bottom = jQuery('div#' + this.options.divId + '_buttonOverlay').outerHeight(true);
      jQuery('div#' + this.options.divId + '_objectdiv').css("bottom", bottom);
      return;
    }
    this.doPinUnpin();
    if (this.options.platform !== 'PC') {
      /*
       * Touch devices need a touch event as there is no concept of a mouse
       * enter/leave. The 3 second delay doesn't work on touch device either
       * because there is no concept of the cursor remaining in a region.
       * Therefore the behavior on touch devices is to touch to get the
       * control bar to appear and touch outside the control bar to get it to
       * hide again.
       */
      var $sessionContr = jQuery('div#' + this.options.divId + '_buttonOverlay');
	    $sessionContr.addClass('hidden');
	    jQuery('div#' + this.options.divId + '_objectdiv').on('click', function () {
        var vis = $sessionContr.is(':visible');
        if (vis) {
          $sessionContr.addClass('hidden');
        } else {
          $sessionContr.removeClass('hidden');
        }
      });
    } else {
      jQuery('div#' + this.options.divId + '_buttonOverlay').slideUp();
      this.state.barOpenState = false;
      jQuery('div#' + this.options.divId + '_objectdiv')
      //if mouse movement- zero the idle timer
      .mousemove(jQuery.proxy(function (evt) {
        //hack for when mousemove is triggered even cursor did not move.
        if (this.lastClientX == evt.clientX && this.lastClientY == evt.clientY) {
          return;
        }
        else {
          this.lastClientX = evt.clientX;
          this.lastClientY = evt.clientY;
        }
        this.state.barAnimationIdleTime = 0;
      }, this))
      // if mouse click - toggle open/close
      .mousedown(jQuery.proxy(function () {
        if (this.state.barOpenState != true) {
          jQuery('div#' + this.options.divId + '_buttonOverlay').slideDown();
          this.state.barOpenState = true;
        }
        this.state.barAnimationIdleTime = 0;
      }, this))
      .mouseenter(jQuery.proxy(function () {
        jQuery('div#' + this.options.divId + '_buttonOverlay').slideDown();
        this.state.barOpenState = true;
        //Check and Increment the idle time counter every second.
        if (this.state.barTimerId != null) clearInterval(this.state.barTimerId);
        this.state.barTimerId = setInterval(jQuery.proxy(function () {
          // If not idle and closed -> open it
          //this.log(bINFO, "buttonOverlayAnimationSetup", "call", "this.barAnimationIdleTime::" + this.state.barAnimationIdleTime + ";;this.barOpenState::" + this.state.barOpenState + ";;this.barForceOpen::" + this.state.barForceOpen);
          if (this.state.barAnimationIdleTime == 0 && this.state.barOpenState == false) {
            jQuery('div#' + this.options.divId + '_buttonOverlay').slideDown();
            this.state.barOpenState = true;
          }
            // if idle and open and not forced open -> close it
          else if (this.state.barAnimationIdleTime > 3 && this.state.barOpenState == true && this.state.barForceOpen != true && this.state.isBarPinned != true) {
            jQuery('div#' + this.options.divId + '_buttonOverlay').slideUp();
            this.state.barOpenState = false;
          }
          this.state.barAnimationIdleTime++;
        }, this), 1000);
      }, this));
      // if mouse in to control bar - override auto hide
      // if mouse out of control bar - back to auto hide
      jQuery('div#' + this.options.divId + '_buttonOverlay')
      .mouseenter(jQuery.proxy(function () {
        this.state.barForceOpen = true;
      }, this))
      .mouseleave(jQuery.proxy(function () {
        this.state.barForceOpen = false;
      }, this))
    }
  },
  
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // clean out DOM structure
    // destroy child classes
    if (this.options.showControlBar) {
      this.$sliderElem.sessionSlider('destroy', 'sessionSlider');
      this.$sessionBtnElem.sessionCtrlButtonPanel('destroy', 'sessionCtrlButtonPanel');
      // destroy self
      this.$elem.find('.SessionController').remove();
    }
    jQuery(document).unbind('fscreenchange');
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "return", pluginName);
   },
 
  _getSessionStartURL: function (mediaURL) {
    this.log(bDEBUG, "_getSessionStartURL", "call", "");
    var sessionStartURL = mediaURL;
    if (mediaURL.search("/clients/player?") >= 0) {
      sessionStartURL = mediaURL.replace("/clients/player?", "/controller/session?");
    }
    return sessionStartURL;
  },

  handleStartedCallback: function (streamURL, objSessionCtrlInfo) {
    this.log(bDEBUG, "handleStartedCallback", "call",
      "streamURL: " + streamURL + " objSessionCtrlInfo: " + JSON.stringify(objSessionCtrlInfo));
    if (streamURL) {
      // get the returned streamURL
      this.state.streamURL = streamURL;
      this.state.isPlaying = true;
      this.state.streamLatency = 0;
      // parse session JSON - get sessionId
      // call the play method on the assoc streamPlayer    
      this.objStreamPlayer && this.objStreamPlayer.play(this.state.streamURL);
    }

    // publish event and update state; comment as scope problem temparay
    //jQuery.publish("onStart", { id: this.options.divId, playerId: this.options.playerId, type: "Event", message: "On Start" });
    //this.state.isPlaying = true;
    if (this.options.playerUsed == "streamPlayerChromelessHLS" || this.options.playerUsed == "streamPlayerHTML5videoHLS") {
      if (objSessionCtrlInfo.state == "WAITING_FOR_STREAMSTART" && this.objStreamPlayer.isPlaying()) {
        objSessionCtrlInfo.state = "STREAMING";
      }
      if (this.objStreamPlayer.isPlaying()) {
        objSessionCtrlInfo = this.getStreamCtrlinfo(objSessionCtrlInfo, "handleStart");
      }
    }
    else if (objSessionCtrlInfo.state == "STREAMING" || objSessionCtrlInfo.state == "WAITING_FOR_STREAMSTART") {
      var dataToPublish = {};
      dataToPublish.offset = objSessionCtrlInfo.timeInfo.lastOffset;
      dataToPublish.playLength = objSessionCtrlInfo.playlength;
      this.$elem.player('doEventPublish', 'onStart', dataToPublish);
    }
    else if (objSessionCtrlInfo.state == "AUTHORIZING") {
      var dataToPublish = {};
      this.$elem.player('doEventPublish', 'onAuthorizing', dataToPublish);
    }
    this.state.objSessionCtrlInfo = objSessionCtrlInfo;
    this.state.sessionId = objSessionCtrlInfo.sessionId;
    // update slider with sessionCtrlInfo
    if (this.options.showControlBar && (objSessionCtrlInfo.state == "STREAMING" ||  objSessionCtrlInfo.state == "AUTHORIZING")) {
      this.applyCollaborationInfo(objSessionCtrlInfo);
    }
  },

  getStreamCtrlinfo: function (objStreamCtrlInfo, handleMode) {
    this.log(bDEBUG, "getStreamCtrlinfo", "call", "");
    if (this.objStreamPlayer.isPlaying() && objStreamCtrlInfo.state == "STREAMING") {
      //Update the WC and offset to generate timeline info same as streaming server which is used to drive the slider
      var currTime = (new Date()).getTime();
      var currOffset = this.getPosition(objStreamCtrlInfo);
      if (objStreamCtrlInfo.timeMode != "PLAYBACK" && objStreamCtrlInfo.timeMode != "PLAYBACK_BUSY") {
        if (objStreamCtrlInfo.action == "Start" && handleMode == "handleStart") {
          objStreamCtrlInfo.timeInfo.startOffset = 0;
          objStreamCtrlInfo.timeInfo.startWC = currTime;
          objStreamCtrlInfo.pvrInfo.startTC = currTime - this.getClipLength(objStreamCtrlInfo);
          objStreamCtrlInfo.pvrInfo.timeLength = objStreamCtrlInfo.pvrInfo.timeLength - this.state.streamLatency;
        }
        else {
          //If any connstate update timlineInfo/PVRInfo will be from session
          //Update the object and keep the information from StreamPlayer
          objStreamCtrlInfo.timeInfo.startOffset = this.state.objSessionCtrlInfo.timeInfo.startOffset;
          objStreamCtrlInfo.timeInfo.startWC = this.state.objSessionCtrlInfo.timeInfo.startWC;
          objStreamCtrlInfo.pvrInfo.startTC = this.state.objSessionCtrlInfo.pvrInfo.startTC;
          objStreamCtrlInfo.pvrInfo.timeLength = this.state.objSessionCtrlInfo.pvrInfo.timeLength;
        }
        //Calculate the last offset w.r.t session start
        objStreamCtrlInfo.timeInfo.lastOffset = (currTime - objStreamCtrlInfo.timeInfo.startWC) - (this.getClipLength(objStreamCtrlInfo) - currOffset);
        objStreamCtrlInfo.timeInfo.lastWC = currTime;
      }
      else {
        if (jQuery.isBlank(objStreamCtrlInfo.playlength)) objStreamCtrlInfo.timeMode = "PLAYBACK_BUSY";
        objStreamCtrlInfo.playlength = this.getClipLength(objStreamCtrlInfo);
        objStreamCtrlInfo.pvrInfo.timeLength = this.getClipLength(objStreamCtrlInfo);
        if (objStreamCtrlInfo.action == "Start" && handleMode == "handleStart") {
          objStreamCtrlInfo.timeInfo.startOffset = 0;
          objStreamCtrlInfo.timeInfo.startWC = currTime - currOffset;
        }
        else {
          //If any connstate update timlineInfo/PVRInfo in object is from session
          //Update the object and keep the information from StreamPlayer
          objStreamCtrlInfo.timeInfo.startOffset = this.state.objSessionCtrlInfo.timeInfo.startOffset;
          objStreamCtrlInfo.timeInfo.startWC = this.state.objSessionCtrlInfo.timeInfo.startWC;
        }
        objStreamCtrlInfo.timeInfo.lastOffset = currOffset;
        objStreamCtrlInfo.timeInfo.lastWC = currTime;
      }
      objStreamCtrlInfo.isTimeshifted = this.state.objSessionCtrlInfo.isTimeshifted;
      objStreamCtrlInfo.timeInfo.timeScale = this.state.objSessionCtrlInfo.timeInfo.timeScale;
      if (objStreamCtrlInfo.action != "Stop") {
        objStreamCtrlInfo.action = this.state.objSessionCtrlInfo.action;
      }
    }
    return objStreamCtrlInfo;
  },

 	handleActionCallback: function (objSessionCtrlInfo) {
	  this.log(bDEBUG, "handleActionCallback", "call",
	    "objSessionCtrlInfo: " + JSON.stringify(objSessionCtrlInfo));
	  if (this.options.playerUsed == "streamPlayerChromelessHLS" || this.options.playerUsed == "streamPlayerHTML5videoHLS") {
	    if (objSessionCtrlInfo.state == "WAITING_FOR_STREAMSTART" && this.objStreamPlayer.isPlaying()) {
	      objSessionCtrlInfo.state = "STREAMING";
	    }
	    else if (!this.objStreamPlayer.isPlaying()) {
	      objSessionCtrlInfo.state = "NONE";
	    }
	    objSessionCtrlInfo = this.getStreamCtrlinfo(objSessionCtrlInfo);
	  }
	  this.state.objSessionCtrlInfo = objSessionCtrlInfo;
	},

	handleEventCallback: function (objSessionCtrlInfo) {
	  this.log(bDEBUG, "handleEventCallback", "call",
	    "objSessionCtrlInfo: " + JSON.stringify(objSessionCtrlInfo));
	  if (this.options.playerUsed == "streamPlayerChromelessHLS" || this.options.playerUsed == "streamPlayerHTML5videoHLS") {
	    if (objSessionCtrlInfo.state == "WAITING_FOR_STREAMSTART" && this.objStreamPlayer.isPlaying()) {
	      objSessionCtrlInfo.state = "STREAMING";
	    }
	    else if (objSessionCtrlInfo.state == "DELETED" && this.objStreamPlayer.isPlaying()) {
	      this.objStreamPlayer.stop();
	    }
	    else if (!this.objStreamPlayer.isPlaying()) {
	      objSessionCtrlInfo.state = "NONE";
	    }
	    objSessionCtrlInfo = this.getStreamCtrlinfo(objSessionCtrlInfo);
	  }
	  else if (this.options.playerUsed == "streamPlayerIPV") {
	    if (objSessionCtrlInfo.state == "STREAMING") {
	      objSessionCtrlInfo.srcKbmCtrl = (this.objStreamPlayer.isRFBReady() && this.objStreamPlayer.enableKbm()) ? true : false;
	      this.log(bDEBUG, "handleEventCallback", " objSessionCtrlInfo.srcKbmCtrl=" + objSessionCtrlInfo.srcKbmCtrl);
          if(objSessionCtrlInfo.action != "Start") {
	        var eventToPublish = "on" + objSessionCtrlInfo.action;
	        if (objSessionCtrlInfo.action == "SkipForward" || objSessionCtrlInfo.action == "SkipBackward" || objSessionCtrlInfo.action == "ResumeFrom")
	          eventToPublish = "onSeek";
	        var dataToPublish = {};
	        dataToPublish.offset = objSessionCtrlInfo.timeInfo.lastOffset;
	        dataToPublish.playLength = objSessionCtrlInfo.playlength;
	        this.$elem.player('doEventPublish', eventToPublish, dataToPublish);
          }
	    }
	    else if (objSessionCtrlInfo.state == "DELETED") {
	      this.objStreamPlayer && this.objStreamPlayer.stop();
	      this.state.isPlaying = false;
	      this.state.isPaused = false;
	      this.$elem.player('doEventPublish', 'onStop', { value: true });
	    }
    }
	  this.state.objSessionCtrlInfo = objSessionCtrlInfo;
	  // update slider with sessionCtrlInfo
	  if (this.options.showControlBar && (objSessionCtrlInfo.state != "NONE" || objSessionCtrlInfo.action == "Stop")) {
	    this.applyCollaborationInfo(objSessionCtrlInfo);
	  }

	  if (this.state.errorStates.indexOf(objSessionCtrlInfo.state) > -1) {
	    var dataToPublish = {};
	    dataToPublish.errorType = objSessionCtrlInfo.state;
	    if (objSessionCtrlInfo.connErrorInfo != null) {
	      dataToPublish.errorCode = objSessionCtrlInfo.connErrorInfo.errorCode;
	      dataToPublish.errorData = objSessionCtrlInfo.connErrorInfo.errorData;
	    }
	    this.$elem.player('doEventPublish', 'onError', dataToPublish);
	    this.doStop();
	  }
  },

  handleErrorCallback: function (response) {
    this.log(bDEBUG, "handleErrorCallback", "call", response);
    var jsonData = response['data'];
    if (jsonData) {
      var name = response['requestName'];
      if (name == 'Playback' || name === 'View' || name === 'Record') {
        var data = JSON.parse(jsonData);
        var summary = data.ipvserror['description'];
        this.options.sessionMessagePanel.display('ERROR', 'ERROR', summary);
        if(name != 'Record') this.$elem.player('doEventPublish', 'onError', { value: true });
      }
    }
  },
	
  handleAgentNotification: function (objEvent, objSessionCtrlInfo) {
    this.log(bDEBUG, "handleAgentNotification", "call",
      objEvent + " :CollabInfo: " + JSON.stringify(objSessionCtrlInfo));
    //$sessionSliderElem.sessionSlider('applyCollaborationInfo', objSessionCtrlInfo);
  },
  _maybeDisplayError: function (objSessionCtrlInfo) {
    this.log(bDEBUG, "_maybeDisplayError", "call", "");
    var state = objSessionCtrlInfo.state;
    if (state == "AUTHORIZING") {
      this.options.sessionMessagePanel.display('INFO', "", "AUTHORIZING...");
    }
    else {
      var errorState = null;
      //check if it is deleted because of error
      if (objSessionCtrlInfo.state == "DELETED" && objSessionCtrlInfo.connErrorInfo != null) {
        errorState = this.state.errorState;
      } else if (this.state.errorStates.indexOf(state) > -1) {
        errorState = state;
      }
      if (errorState != null) {
        // Error in play
        var desc = '';
        var errCode = null;
        if (objSessionCtrlInfo.connErrorInfo != null) {
          errCode = objSessionCtrlInfo.connErrorInfo.errorCode;
          desc = objSessionCtrlInfo.connErrorInfo.errorData;
        }
        // SOURCE_ERROR End of Media should not display error
        if (errCode === this.END_OF_MEDIA) errorState = null;
      }
      if (errorState === null) {
        // Check rec state; but only if no error in play
        var recState = objSessionCtrlInfo.recState;
        if (this.state.errorStates.indexOf(recState) > -1) {
          errorState = recState;
          desc = objSessionCtrlInfo.recErrorData;
        }
      }
      if (errorState === null) {
        this.options.sessionMessagePanel.clear();
      } else {
        this.options.sessionMessagePanel.display('ERROR', errorState, desc);
      }
      this.state.errorState = errorState;
    }
  },
  onHLSPlayerStateChange: function (eventType, eventData) {
    this.log(bDEBUG, "onHLSPlayerStateChange", "call", eventType);
    var applyInfo = false;
    var eventToPublish = eventType;
    var dataToPublish = {};
    if (eventType == "onStreamStart") {
      applyInfo = true;
      eventToPublish = "onStart";
      dataToPublish.offset = eventData.offset;
      dataToPublish.playLength = eventData.playLength;
      if (this.state.objSessionCtrlInfo.timeMode != "PLAYBACK" && this.state.objSessionCtrlInfo.timeMode != "PLAYBACK_BUSY") {
        this.state.streamLatency = eventData.playLength - eventData.offset;
      }
      this.state.objSessionCtrlInfo.state = "STREAMING";
      this.state.objSessionCtrlInfo = this.getStreamCtrlinfo(this.state.objSessionCtrlInfo, "handleStart");
    }
    else if (eventType == "onStreamPVRAction") {
      applyInfo = true;
      eventToPublish = "on" + eventData.action;
      if (eventData.action == "SkipForward" || eventData.action == "SkipBackward" || eventData.action == "ResumeFrom")
        eventToPublish = "onSeek";
      dataToPublish.offset = eventData.offset;
      dataToPublish.playLength = eventData.playLength;
      this.state.objSessionCtrlInfo.action = eventData.action;
      this.state.objSessionCtrlInfo.timeInfo.timeScale = eventData.timeScale;
      if (eventData.action == "Catchup" || (dataToPublish.offset >= (dataToPublish.playLength - this.state.streamLatency) && eventToPublish == "onSeek")) {
        this.state.objSessionCtrlInfo.isTimeshifted = false;
      }
      else {
        this.state.objSessionCtrlInfo.isTimeshifted = true;
      }
    }
    else if (eventType == "onStreamStop") {
      applyInfo = true;
      eventToPublish = "onStop";
      if (this.state.isPlaying && this.state.objSessionCtrlInfo.action != "Stop") {
        this.sessionInstance && this.sessionInstance.doStop();
      }
      this.state.isPlaying = false;
      this.state.isPaused = false;
      this.state.streamLatency = 0;
    }
    else if (eventType == "onError") {
      this.state.objSessionCtrlInfo.state = eventData.state;
      this.state.objSessionCtrlInfo.connErrorInfo = eventData.connErrorInfo;
      this._maybeDisplayError(this.state.objSessionCtrlInfo);
    }
    if (this.options.showControlBar && applyInfo == true) {
      this.state.objSessionCtrlInfo = this.getStreamCtrlinfo(this.state.objSessionCtrlInfo);
      this.applyCollaborationInfo(this.state.objSessionCtrlInfo);
    }
    this.$elem.player('doEventPublish', eventToPublish, dataToPublish);
  },

  getClipLength: function (objStreamCtrlInfo) {
    this.log(bDEBUG, "getClipLength", "call", "");
    var clipLength = this.objStreamPlayer.getcliplength();
    //HTML5 will return duration as "Infinity" in case of LIVE/PVR
    //Calculating using seekable.end and seekable.start
    //seekable.start is always 0 and seekable.end will keep increases.
    if (this.options.playerUsed == "streamPlayerHTML5videoHLS") {
      if (objStreamCtrlInfo.timeMode != "PLAYBACK" && objStreamCtrlInfo.timeMode != "PLAYBACK_BUSY") {
        //BarcoUtil.log("SessionController : getClipLength: clipLength = " + clipLength );
        if (clipLength > objStreamCtrlInfo.pvrInfo.timeLength) clipLength = objStreamCtrlInfo.pvrInfo.timeLength;
        //BarcoUtil.log("SessionController : getClipLength: clipLength = " + clipLength );
      }
    }
    return clipLength - this.state.streamLatency;
  },

  getPosition: function (objStreamCtrlInfo) {
    this.log(bDEBUG, "getPosition", "call", "");
    var currPosition = this.objStreamPlayer.getPosition();
    //HTML5 will return duration as "Infinity" in case of LIVE/PVR
    //Calculating using seekable.end and seekable.start
    //seekable.start is always 0 and seekable.end will keep increases.
    //Position is  w.r.t seekable.end so adjusting if seekable.end is greaterthan PVR length
    if (this.options.playerUsed == "streamPlayerHTML5videoHLS") {
      if (objStreamCtrlInfo.timeMode != "PLAYBACK" && objStreamCtrlInfo.timeMode != "PLAYBACK_BUSY") {
        //BarcoUtil.log("SessionController : getPosition: currPosition = " + currPosition );
        var endTimeFromPlayer = this.objStreamPlayer.getEndTimeRange();
        if (endTimeFromPlayer > (objStreamCtrlInfo.pvrInfo.timeLength - 5)) {          
          currPosition = parseInt(currPosition) - parseInt(endTimeFromPlayer - objStreamCtrlInfo.pvrInfo.timeLength);
          //BarcoUtil.log("SessionController : getPosition: endTimeFromPlayer = " + endTimeFromPlayer + " currPosition = " + currPosition );
        }
      }
    }
    return currPosition;
  },

  applyCollaborationInfo: function (objSessionCtrlInfo) {
    this.log(bDEBUG, "applyCollaborationInfo", "call", "");
    //check if action is stop and player is fullscrren mode then exit from fullscreen
    if (objSessionCtrlInfo.action == "Stop" && this.state.isFullScr) {
      this.doToggleScreen();
    }
    objSessionCtrlInfo.streamLatency = this.state.streamLatency;
    if (objSessionCtrlInfo.state != "AUTHORIZING") {
      this.$sliderElem.sessionSlider('applyCollaborationInfo', objSessionCtrlInfo);
      this.$editMediaPanelElem && this.$editMediaPanelElem.editMediaPanel('applyCollaborationInfo', objSessionCtrlInfo);
      this.$windowBtnElem.windowCtrlButtonPanel('applyCollaborationInfo', objSessionCtrlInfo);
      this.$bookmarkPanelElem && this.$bookmarkPanelElem.sessionCtrlBookmarkPanel('applyCollaborationInfo', objSessionCtrlInfo);
    }
    this.$sessionBtnElem.sessionCtrlButtonPanel('applyCollaborationInfo', objSessionCtrlInfo);    
    this._maybeDisplayError(objSessionCtrlInfo);
  },

  //set local KBM control
  doKBM: function () {
    this.log(bDEBUG, "doKBM", "call", "");
    this.state.isBarPinned = this.state.isKbmEnabled;
    if (this.state.isKbmEnabled) {
      this.setLocalKBMState(0);
    }
    else {
      this.setLocalKBMState(1);
    }
    this.doPinUnpin();
  },

  doMuteUnmute: function () {
    this.log(bDEBUG, "doMuteUnmute", "call", "");
    if (this.state.isMuted) {
      this.doUnmute();
    }
    else {
      this.doMute();
    }
  },

  doPinUnpin: function () {
    this.log(bDEBUG, "doPinUnpin", "call", "");
    if (this.options.controllerBarFixed) return;
    if (this.state.isBarPinned) {
      //check zoom mode is not FitToWindow or KBMEnabled if true then don't change pin mode
      if (this.state.zoomSel != "Fit" || this.state.isKbmEnabled) return;
      var bottom = 0;
      this.state.isBarPinned = false;
    }
    else {
      var bottom = jQuery('div#' + this.options.divId + '_buttonOverlay').outerHeight(true);
      this.state.isBarPinned = true;
    }
    jQuery('div#' + this.options.divId + '_objectdiv').css("bottom", bottom);
    this.$windowBtnElem && this.$windowBtnElem.windowCtrlButtonPanel('onPinUnpin', this.state.isBarPinned);
  },

  doShare: function () {
    this.log(bDEBUG, "doShare", "call", "");
    if (this.state.mediaURL) {
		prompt("URL to launch this media in a browser", this.options.playerURL) ;
    }
  },
  
  doPlay: function (mediaURL) {
    this.log(bDEBUG, "doPlay", "call", mediaURL);
    if (mediaURL) this.state.mediaURL = mediaURL;
    this.state.isPlaying = true;
		this.state.isPaused = false;
		// start a new session
    var sessionStartURL = this._getSessionStartURL(this.state.mediaURL);
    this.log(bDEBUG, "doPlay", "call",
      "doStart time: " + new Date().getTime() + " ||  (" + sessionStartURL + ")");
    this.sessionInstance && this.sessionInstance.doStartURL(sessionStartURL);
  },

  doStop: function () {
    this.log(bDEBUG, "doStop", "call", "");
    //this.log(TRACE_METH, "doStop", "call",
    //  "objControllerState:" + this.objSessionCtrlInfo.state);
    //if (this.objSessionCtrlInfo.state == "NONE") return;
    if (this.options.playerUsed !== 'streamPlayerIPV') {
      this.objStreamPlayer.stop();
    }
    else {
      this.state.isPlaying = false;
      this.state.isPaused = false;
      // send stop request
      this.sessionInstance && this.sessionInstance.doStop();
    }
  },
  doPause: function () {
    this.log(bDEBUG, "doPause", "call", "");
    this.state.isPaused = true;
    if (this.options.playerUsed !== 'streamPlayerIPV') {
      this.objStreamPlayer.pause();
    }
    else {
      // send pause request
      this.sessionInstance && this.sessionInstance.doPause();
    }
  },
  doResume: function () {
    this.log(bDEBUG, "doResume", "call", "");
    this.state.isPaused = false;
    // send resume request
    if (this.options.playerUsed !== 'streamPlayerIPV') {
      this.objStreamPlayer.resume();
    }
    else {
      this.sessionInstance && this.sessionInstance.doResume();
    }
  },
  doSkipFwd: function (skipOffset) {
    this.log(bDEBUG, "doSkipFwd", "call", skipOffset);
    //Not supported as IPAD/HTML5 has issue with seek incase of PVR
    if (this.options.playerUsed == "streamPlayerHTML5videoHLS" && (this.state.objSessionCtrlInfo.timeMode == "PVR" || this.state.objSessionCtrlInfo.timeMode == "PERSISTENT_PVR")) return;
		this.state.isPaused = false;
    // send skipFwd request
		if (jQuery.isBlank(skipOffset)) skipOffset = this.options.skipforwardOffset;
		if (this.options.playerUsed !== 'streamPlayerIPV') {
		  this.objStreamPlayer.skipforward(skipOffset);
		}
		else {
		  this.sessionInstance && this.sessionInstance.doSkipFwd(skipOffset);
		}
  },
  doSkipBack: function (skipOffset) {
    this.log(bDEBUG, "doSkipBack", "call", skipOffset);
    //Not supported as IPAD/HTML5 has issue with seek incase of PVR
    if (this.options.playerUsed == "streamPlayerHTML5videoHLS" && (this.state.objSessionCtrlInfo.timeMode == "PVR" || this.state.objSessionCtrlInfo.timeMode == "PERSISTENT_PVR")) return;
		this.state.isPaused = false;
    // send skipBack request
		if (jQuery.isBlank(skipOffset)) skipOffset = this.options.skipbackwardOffset;
		if (this.options.playerUsed !== 'streamPlayerIPV') {
		  this.objStreamPlayer.skipbackward(skipOffset);
		}
		else {
		  this.sessionInstance && this.sessionInstance.doSkipBack(skipOffset);
		}
  },
  doSeekTo: function (mediaOffset) {
    this.log(bDEBUG, "doSeekTo", "call", mediaOffset);
    if (this.state.objSessionCtrlInfo.timeMode != "PLAYBACK" && this.state.objSessionCtrlInfo.timeMode != "PLAYBACK_BUSY") return;
		this.state.isPaused = false;
    // send skipBack request
		if (jQuery.isBlank(mediaOffset)) mediaOffset = "";
		if (this.options.playerUsed !== 'streamPlayerIPV') {
		  this.objStreamPlayer.seekTo(mediaOffset);
		}
		else {
		  this.sessionInstance && this.sessionInstance.doPlayFrom(mediaOffset);
		}
  },
  doCatchup: function () {
    this.log(bDEBUG, "doCatchup", "call", "");
    //Not supported as IPAD/HTML5 has issue with seek incase of PVR
    if (this.state.objSessionCtrlInfo.timeMode == "PLAYBACK" ||
      (this.options.playerUsed == "streamPlayerHTML5videoHLS" && (this.state.objSessionCtrlInfo.timeMode == "PVR" || this.state.objSessionCtrlInfo.timeMode == "PERSISTENT_PVR"))) return;
    this.state.isPaused = false;
    if (this.options.playerUsed !== 'streamPlayerIPV') {
      this.objStreamPlayer.catchup();
    }
    else {
      // send catchup request
      this.sessionInstance && this.sessionInstance.doCatchup();
    }
  },
  doUnmute: function () {
    this.log(bDEBUG, "doUnmute", "call", "");
    this.objStreamPlayer.unmute();
    this.$windowBtnElem && this.$windowBtnElem.windowCtrlButtonPanel('onMuteUnmute', this.isMuted());
  },
  doMute: function () {
    this.log(bDEBUG, "doMute", "call", "");
    this.objStreamPlayer.mute();
    this.$windowBtnElem && this.$windowBtnElem.windowCtrlButtonPanel('onMuteUnmute', this.isMuted());
  },
  doRecord: function () {
    this.log(bDEBUG, "doRecord", "call", "");
    // send startRecord request
    this.sessionInstance && this.sessionInstance.doStartRecord();
  },
  doStopRecord: function () {
    this.log(bDEBUG, "doStopRecord", "call", "");
    // send stopRecord request
    this.sessionInstance && this.sessionInstance.doStopRecord();
  },

  onBookmarkSelected: function (bookmarkNID) {
    this.log(bDEBUG, "onBookmarkSelected", "call", "");
    this.state.bookmarkNID = bookmarkNID;
    this.$bookmarkPanelElem && this.$sessionBtnElem.sessionCtrlButtonPanel('onBookmarkSelected', bookmarkNID);
  },

  doAddBookmark: function () {
    this.log(bDEBUG, "doAddBookmark", "call", "");
    if(jQuery.isBlank(this.state.objSessionCtrlInfo.bookmarkFileNID)) return;
    var bookmarkOffset = null;
    var isHLSPlayback = false;
    var whereId = this.state.objSessionCtrlInfo.sessionId;
    if (this.options.playerUsed !== 'streamPlayerIPV' && this.state.objSessionCtrlInfo.timeMode == "PLAYBACK") {
      bookmarkOffset = this.getPosition(this.state.objSessionCtrlInfo);
      whereId = this.state.objSessionCtrlInfo.bookmarkFileNID;
      isHLSPlayback = true;
    }
    this.$bookmarkPanelElem && this.$bookmarkPanelElem.sessionCtrlBookmarkPanel('addBookmark', whereId, isHLSPlayback, bookmarkOffset);
  },

  doEditBookmark: function () {
    this.log(bDEBUG, "doEditBookmark", "call", "");
    if (!jQuery.isBlank(this.state.bookmarkNID)) {
      this.$bookmarkPanelElem && this.$bookmarkPanelElem.sessionCtrlBookmarkPanel('editBookmark', this.state.bookmarkNID);
    }
    else {
      alert("Select Bookmark to Edit");
    }
  },

  doDeleteBookmark: function () {
    this.log(bDEBUG, "doDeleteBookmark", "call", "");
    if (!jQuery.isBlank(this.state.bookmarkNID)) {
      this.$bookmarkPanelElem && this.$bookmarkPanelElem.sessionCtrlBookmarkPanel('deleteBookmark', this.state.bookmarkNID);
    }
    else {
      alert("Select Bookmark to Delete");
    }
  },
  //Cutlist(Edit Media) support
  editMedia: function () {
    if (this.state.objSessionCtrlInfo.timeMode == "PLAYBACK") {
      if (this.$editMediaPanelElem) {
        this.$editMediaPanelElem.editMediaPanel('initEditMedia', this.state.objSessionCtrlInfo.bookmarkFileNID);
      }
    }
  },

  getSliderRef: function () {
    return this.$sliderElem;
  },

  getEditMediaPanelRef: function () {
    return this.$editMediaPanelElem;
  },
  //End Cutlist(Edit Media) support

  getSessionPostBaseURL: function () {
    this.log(bDEBUG, "getSessionPostBaseURL", "call", "");
    var sessionURLPostBase = null;
    if(this.sessionInstance)
      sessionURLPostBase = this.sessionInstance.getSessionPostBaseURL();
    return sessionURLPostBase;
  },

  doToggleScreen: function () {
    this.log(bDEBUG, "doToggleScreen", "call", "");
    jQuery(document).unbind('fscreenchange');
    jQuery(document).bind('fscreenchange', jQuery.proxy(this.onToggleScreen, this));
    if (!this.state.isFullScr) {
      this.$elem.fullscreen();
    } else {
      jQuery.fullscreen.exit();
    }
  },

  onToggleScreen: function () {
    this.state.isFullScr = jQuery.fullscreen.isFullScreen();
    this.$windowBtnElem && this.$windowBtnElem.windowCtrlButtonPanel('onToggleScreen', this.state.isFullScr);
    this.$elem.player('doEventPublish', "onFullscreen", { id: this.options.divId, playerId: this.options.playerId, type: "Event", message: "On Fullscreen", isFullScreen: this.state.isFullScr });
  },
  
  doSnapshot: function () {
    this.log(bDEBUG, "doSnapshot", "call", "");
    var path = ""; // provide timecoded filename
    this.objStreamPlayer.snapShot(path);
  },
  doZoomPopup: function (pos, hide) {
    this.log(bDEBUG, "doZoomPopup", "call", "");
    this.$zoomPanelElem && this.$zoomPanelElem.sessionCtrlZoomPanel('popupZoom', pos, hide);
  },
  doZoom: function(sel) {
    this.log(bDEBUG, "doZoom", "call", "sel=" + sel);
    var zoomSize = null;
    if (sel == "Fit") {
      zoomSize = sel;
    }
    else {
      zoomSize = parseInt(sel);
      if (isNaN(zoomSize) || zoomSize > 400) zoomSize = null;
    }
    if (zoomSize) {
      this.state.zoomSel = sel;
      this.state.isBarPinned = (this.state.zoomSel == "Fit");
      this.doPinUnpin();
      this.$zoomPanelElem && this.$zoomPanelElem.sessionCtrlZoomPanel('zoomSelected', sel);
      if (zoomSize == "Fit") {
        this.doResFit();
      }
      else {
        this.objStreamPlayer.setScaleToZoom(zoomSize);
        this.doesResFit();
      }      
    }
    else {
      this.log(bERROR, "doZoom", "fail", "zoom=" + sel);
    }
  },
  doResFit: function () {
    this.log(bDEBUG, "doResFit", "call", "");
    this.objStreamPlayer.setScaleToWindow()
    this.doesResFit();
    this.state.zoomSel = 'Fit';
    this.$zoomPanelElem && this.$zoomPanelElem.sessionCtrlZoomPanel('zoomSelected', this.state.zoomSel);
  },
  isPlaying: function () {
    this.log(bDEBUG, "isPlaying", "call", "");
    if (this.options.playerUsed !== 'streamPlayerIPV') {
      return this.objStreamPlayer.isPlaying();
    }
    return this.state.isPlaying;
  },
  isPaused: function () {
    this.log(bDEBUG, "isPaused", "call", "");
    if (this.options.playerUsed !== 'streamPlayerIPV') {
      return this.objStreamPlayer.isPaused();
    }
    return this.state.isPaused;
  },
  isMuted: function() {
    this.log(bDEBUG, "isMuted", "call", "");
    this.state.isMuted = this.objStreamPlayer.isMuted();
    return this.state.isMuted;
  },
  doesResFit: function() {
    this.log(bDEBUG, "doesResFit", "call", "");
    this.state.isResFit = !this.objStreamPlayer.isNative();
    return this.state.isResFit;
  },
  isKBMEnabled: function () {
    this.log(bDEBUG, "isKBMEnabled", "call", "");
    this.state.isKbmEnabled = this.objStreamPlayer.isKBMEnabled();
    return this.state.isKbmEnabled;
  },
  setLocalKBMState: function (state) {
    this.log(bDEBUG, "setLocalKBMState", "call", "");
    this.objStreamPlayer.setLocalKBMSState(state);
    this.$windowBtnElem && this.$windowBtnElem.windowCtrlButtonPanel('onKbmOnOff', this.isKBMEnabled());
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('sessionController', SessionController);