/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var SessionSlider = Class.extend({
	_defaults: {
	},
	init: function(options, elem) {
		this.options = jQuery.extend({}, this._defaults, options);
		this.log(bDEBUG, "init", "call", "");
		// this.sessionController = this.options.sessionController;
		this.serviceAgent = this.options.barcoAPI.clsServiceAgent;
		this.elem = elem;
		this.$elem = jQuery(elem);
		// time and offset of start of slider bar
		// startmediatime is used as a flag to control repaint execution
		this.StartMediaTime = null;
		this.SliderStartOffset = null;
		// last known time and offset of media
		this.LastMediaTime = null;
		this.LastMediaOffset = null;
		// mode and scale of time since last known media offset
		this.TimeMode = null;
		this.TimeScale = null;
		this.TimeLength = null;
		// current media offset
		this.CurrMediaOffset = null;
		this.LiveStartMediaTime = null;
		// PVR media offsets
		this.PVRStartMediaOffset = null;
		this.LiveCurrMediaOffset = null;
		// custom offset for starting file media at non-zero offset
		this.StreamStartOffset = null;
		// pointer to interval timer for continuous update
		this.timerHandle = null;
		this.DefaultTimeLength = 3600000;
		this.isTimeshifted = false;
		this.onDragSlider = false;
		this.timeOutId = null;
		this.DeltaServerLocalTime = null;
		
	  // init the offset and position
		this.$sessionCtl = this.$elem.closest('.SessionController').parent();
		this.scrubberState = {};
		this._build();
		this.initMediaOffset();
		this.updateView();
	},
	destroy: function (pluginName) {
		if (this.options.showBookmarkPlayback) {
		  this.$bookmarksListElem.sessionBookmarksList('destroy', 'sessionBookmarksList');
		}
		this.$elem.find('.SessionSlider').remove();
		this.$elem.removeData(pluginName);
		this.log(bDEBUG, "destroy", "call", pluginName);
 	},
	_build: function () {
	  var html = '<div class="controller-top" id="blkSlider">';
	  html += '<div class="tooltip scrubber_tooltip"></div>';
	  html += '<div class="controller-time1"><span class="lblTimeCode"></span></div>';
		html += '<div class="scrubber">';
		html += '<div id="progressbar" class="ui-slider-progressbar"></div></div>';
		html += '<div class="controller-time2"><span class="lblTimeLength"></span></div>';
		if (this.options.showBookmarkPlayback) {
		  html += '<div id="' + this.options.divId + '_bookmarksbar" class="ui-slider-bookmarksbar"></div>';
		}
		html += '</div>';
		this.$elem.append(html);
		this.$lblTimeCode = this.$elem.find(".lblTimeCode").first();
		this.$blkSlider = this.$elem.find("#blkSlider").first();
		this.$scrubber = this.$elem.find(".scrubber").first();
		this.$blkProgress = this.$elem.find(".ui-slider-progressbar").first();
		this.$scrubber_tooltip = this.$blkSlider.children(".scrubber_tooltip").first();
		this._createScrubberUI();
		this.$scrubber_tooltip.hide();
		jQuery("#progressbar").position({
		  of: ".scrubber"
		});	
		this.$lblTimeLength = this.$elem.find(".lblTimeLength").first();
		if (this.options.showBookmarkPlayback) {
		  this.$bookmarksListElem = this.$elem.find('#' + this.options.divId + '_bookmarksbar');
		  this.$bookmarksListElem.sessionBookmarksList(this.options);
		}
	},
	_createScrubberUI: function() {
		var self = this;
		this.scrubberState.pos = 0;
		this.scrubberState.min = 0;
		this.scrubberState.range = 100;
		this.scrubberState.tooltipTextOffset = 39;
		this.$scrubber.slider({
			//range: "max",
			value: this.scrubberState.pos,
			//min: this.scrubberState.min,
			//max: this.scrubberState.min + this.scrubberState.range,
			slide: function(evt, ui) {
				self.onDrag(ui.value);
				self.scrubberState.pos = ui.value;
			},
			start: function(evt, ui) {
			},
			stop: function(evt, ui) {
			  self.onSelect(ui.value, self.scrubberState.pos);
			}
		});
		this.$scrubber_handle = this.$scrubber.children(".ui-slider-handle").first();
    //handle mouse events to show/hide tooltip
		jQuery(".ui-slider").mouseleave(function () {
		  self.$scrubber_tooltip.hide();
		});
		jQuery(".ui-slider").mouseenter(jQuery.proxy(this._handleTooltipOnMouseAction, this));
		jQuery(".ui-slider").mousemove(jQuery.proxy(this._handleTooltipOnMouseAction, this));
	},
	// extend show
	show: function(EventVar, EventVal) {
	  this.$scrubber.show();
	},
	// override hide
	hide: function () {
	  jQuery("#progressbar").empty();
	  this.$scrubber.hide();
	},
	//check mediaOffset before set it, to ensure this.StartMediaTime not null for join mode
	checkAndSetMediaOffset: function(objCollabInfo) {
		if (this.timerHandle == null) {
			this.startMediaOffset(objCollabInfo);
		}
		this.setMediaOffset(objCollabInfo);
	},
	applyCollaborationInfo: function(objCollabInfo) {
		this.isTimeshifted = objCollabInfo.isTimeshifted;
		// process action
		switch (objCollabInfo.action) {
			case "Start":
			  this.startMediaOffset(objCollabInfo);
			  if (this.options.showBookmarkPlayback) {
			    this.$bookmarksListElem.sessionBookmarksList('applyCollaborationInfo', objCollabInfo);
			  }
				break;
			case "Stop":
			  this.initMediaOffset();
			  if (this.options.showBookmarkPlayback) {
			    this.$bookmarksListElem.sessionBookmarksList('applyCollaborationInfo', objCollabInfo);
			  }
				break;
			case "Pause":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			case "Resume":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			case "ResumeFrom":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			case "ResumeFromMediaTC":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			case "Catchup":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			case "SkipBackward":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			case "SkipForward":
				this.checkAndSetMediaOffset(objCollabInfo);
				break;
			default:
				// just log an error
				//BarcoUtil.log("SessionSlider::Unknown Action:" + objCollabInfo.action);
				//        this.updateMediaOffset();
				break;
		}
		if (objCollabInfo.isSourceSelected === "IMAGE") {
			this.initMediaOffset();
		}
		this.updateView();

	},
	initMediaOffset: function() {
		// default time mode = live
		this.TimeMode = "NONE";
		// default time length = 1 hour
		this.TimeLength = this.DefaultTimeLength; //3600000;
		// default timescale is 1:1 play
		this.TimeScale = 1;
		// default stream start offset
		this.StreamStartOffset = 0;

		this.StartMediaTime = null;
		this.LiveStartMediaTime = null;
		this.PVRStartMediaTime = null;
		this.LastMediaTime = null;
		this.LastMediaOffset = 0;
		this.onDragSlider = false;
		this.DeltaServerLocalTime = null;

		this.SliderStartOffset = 0;
		this.CurrMediaOffset = 0;
		this.StreamLatency = 0;

		if (this.$lblTimeCode != null) {
		  this.$lblTimeCode.text("");
		}
		
		if (this.$lblTimeLength != null) {
			this.$lblTimeLength.text("");
		}

		if (this.lblPvrBuffer != null) {
			this.lblPvrBuffer.text("");
		}
		this.hide();
	},
	startMediaOffset: function(objCollabInfo) {
		/*
		 //make sure the collab state is Streaming
		 if (com.ipvs.gui.SessionControllerI.CONNSTATE[objCollabInfo.state] < com.ipvs.gui.SessionControllerI.CONNSTATE['STREAMING']) return;
		 */
		// default time mode = live
		this.TimeMode = "LIVE";
		if (objCollabInfo.pvrInfo.canPVR) {
			this.TimeMode = objCollabInfo.timeMode;
		}

		if (this.TimeMode == null || this.TimeMode === "") {
			this.TimeMode = "NONE";
		}
		// default time length = 1 hour
		this.TimeLength = objCollabInfo.pvrInfo.timeLength;

		if (jQuery.isBlank(this.TimeLength)) {
			this.TimeLength = this.DefaultTimeLength;
		}
		//BarcoUtil.log("Slider: startMediaOffset 0 :: this.TimeLength: " + this.TimeLength);

		this._handleLblTimeLength(objCollabInfo);

		// default timescale is 1:1 play
		this.TimeScale = objCollabInfo.timeInfo.timeScale;
		if (this.TimeScale == null || this.TimeScale === "") {
			this.TimeScale = 1;
		}
		this.StreamStartOffset = objCollabInfo.timeInfo.startOffset;
		if (this.StreamStartOffset == null || this.StreamStartOffset === "") {
			this.StreamStartOffset = 0;
		}
	  //BarcoUtil.log("Slider: startMediaOffset 1 :: this.StreamStartOffset: " + this.StreamStartOffset);
		if (this.TimeMode === "PLAYBACK_BUSY") {
		  this.DeltaServerLocalTime = 0;
		}
		if (this.DeltaServerLocalTime == null) {
		  this.DeltaServerLocalTime = parseInt((new Date()).getTime()) - parseInt(objCollabInfo.timeInfo.startWC);
		}
		var CurrDateTime = this.serviceAgent.getCurrentServerTime() - this.DeltaServerLocalTime;
		//BarcoUtil.log("Slider: startMediaOffset 2 :: CurrDateTime: " + CurrDateTime);

		// assume we are starting now
		//and media offset 0 (unless mode is LIVE)
		// in LIVE mode Media Offset = MediaTime
		this.StartMediaTime = objCollabInfo.timeInfo.startWC;
	  //BarcoUtil.log("Slider: startMediaOffset 3 :: this.StartMediaTime: " + this.StartMediaTime);

		if (this.StartMediaTime == null || this.StartMediaTime === "") {
			this.StartMediaTime = CurrDateTime;
		}
		else if (this.StartMediaTime > CurrDateTime && this.TimeMode != "PLAYBACK_BUSY") {
			this.StartMediaTime = CurrDateTime;
		}
    
		//BarcoUtil.log("Slider: startMediaOffset 4 :: this.StartMediaTime: " + this.StartMediaTime + " ::CurrDateTime:" + CurrDateTime);

		if (this.TimeMode === "PERSISTENT_PVR") {
			this.PVRStartMediaTime = objCollabInfo.pvrInfo.startTC;
			//BarcoUtil.log("Slider: startMediaOffset 5 :: this.PVRStartMediaTime: " + this.PVRStartMediaTime + " ::(CurrDateTime - this.TimeLength):" + CurrDateTime - this.TimeLength);
			if (this.PVRStartMediaTime < this.StartMediaTime - this.TimeLength) {
				this.PVRStartMediaTime = this.StartMediaTime - this.TimeLength;
			}
			//BarcoUtil.log("Slider: startMediaOffset 6 :: this.PVRStartMediaTime: " + this.PVRStartMediaTime);
		}

		
		if (this.TimeMode != "PLAYBACK" && this.TimeMode != "PLAYBACK_BUSY") {
		  this.$lblTimeCode.css("display", "none");
	  }
		else {
		  this.$lblTimeCode.css("display", "block");
		}

		// get the last media time and offset
		this.LastMediaTime = objCollabInfo.timeInfo.lastWC;
		if (this.LastMediaTime == null || this.LastMediaTime === "") {
			this.LastMediaTime = CurrDateTime;
		}
		else if (this.LastMediaTime > CurrDateTime) {
			this.LastMediaTime = CurrDateTime;
		}
		this.LastMediaOffset = objCollabInfo.timeInfo.lastOffset;
		if (this.LastMediaOffset == null || this.LastMediaOffset === "") {
			this.LastMediaOffset = this.LastMediaTime - this.StartMediaTime;
		}

		this.StreamLatency = objCollabInfo.streamLatency;

		this.show();
		// done with start - rest is handled by update
		this.updateMediaOffset();
	},
	setMediaOffset: function(objCollabInfo) {
		// get curr media offset from the offset that came with the action event
		// set timescale
		this.TimeScale = objCollabInfo.timeInfo.timeScale;
		this.LastMediaOffset = objCollabInfo.timeInfo.lastOffset;
		this.LastMediaTime = objCollabInfo.timeInfo.lastWC;
		this._handleLblTimeLength(objCollabInfo);
	},
	updateMediaOffset: function() {
		if (this.StartMediaTime != null) {
			var CurrDateTime = this.serviceAgent.getCurrentServerTime() - this.DeltaServerLocalTime;
			// calc media offset where live is
			this.LiveCurrMediaOffset = CurrDateTime - this.StartMediaTime;
			// calc curr media offset (in milliseconds) based on milliseconds since media time and timescale (ie play mode)    
			this.CurrMediaOffset = Math.round(parseInt(this.LastMediaOffset) + (parseInt(CurrDateTime) - parseInt(this.LastMediaTime)) * this.TimeScale);

			if (this.TimeMode === "PLAYBACK" && this.CurrMediaOffset > this.TimeLength) {
			  this.CurrMediaOffset = this.TimeLength;
			}
			
			if (this.TimeMode === "PLAYBACK_BUSY") {
			  this.TimeLength = this.LiveCurrMediaOffset;
			}
		  //BarcoUtil.log("Slider: updateMediaOffset:: 0 ::CurrDateTime: " + CurrDateTime + " \n::this.LiveCurrMediaOffset:  " + this.LiveCurrMediaOffset + "\n::this.CurrMediaOffset:" + this.CurrMediaOffset + "\n::this.StartMediaTime:" + this.StartMediaTime);
			// by default slider starts at offset 0
			this.SliderStartOffset = 0;
			// if time mode is still NONE
			if (this.TimeMode === "NONE") {
				// calc the slider start offset
				this.SliderStartOffset = this.LiveCurrMediaOffset;
			}
			// if in LIVE mode 
			if (this.TimeMode === "LIVE") {
				// if time mode just changed to LIVE - init LiveStartTime
				if (this.LiveStartMediaTime == null) {
					//this.LiveStartMediaTime = CurrDate.getTime();
					this.LiveStartMediaTime = this.StartMediaTime;
					this.LastMediaTime = CurrDateTime;
					this.LastMediaOffset = this.CurrMediaOffset;
				}
				// calc slider start offset
				this.SliderStartOffset = this.LiveCurrMediaOffset - this.TimeLength;
			}
			// if in PVR mode
			if (this.TimeMode === "PERSISTENT_PVR" || this.TimeMode === "PVR") {
				// if time mode just changed to PVR - init PVRStartTime
				if (this.PVRStartMediaTime == null) {
					//this.PVRStartMediaTime = CurrDate.getTime();
					this.PVRStartMediaTime = this.StartMediaTime;
					this.LastMediaTime = CurrDateTime;
					this.LastMediaOffset = this.CurrMediaOffset;
				}
				// calc the start of the PVR bar fill
				this.PVRStartMediaOffset = this.LiveCurrMediaOffset - this.TimeLength;
				//BarcoUtil.log("Slider: updateMediaOffset PVR :: 1 ::this.PVRStartMediaTime:  " + this.PVRStartMediaTime + "::this.PVRStartMediaOffset:" + this.PVRStartMediaOffset + "::TimeLength:" + this.TimeLength + "::(" + this.LiveCurrMediaOffset + " - " + this.TimeLength + "):");
				// adjust if trimming has kicked in
				if (this.PVRStartMediaOffset < (this.PVRStartMediaTime - this.StartMediaTime)) {
					this.PVRStartMediaOffset = this.PVRStartMediaTime - this.StartMediaTime;
				}
				//BarcoUtil.log("Slider: updateMediaOffset PVR :: 2 ::this.PVRStartMediaOffset: " + this.PVRStartMediaOffset + " ::(" + this.PVRStartMediaTime + " - " + this.StartMediaTime + "):  ");
				// calc the slider start offset
				this.SliderStartOffset = this.LiveCurrMediaOffset - this.TimeLength;
				//BarcoUtil.log("Slider: updateMediaOffset PVR :: 3 ::this.SliderStartOffset: " + this.SliderStartOffset + " ::(" + this.LiveCurrMediaOffset + " - " + this.TimeLength + "):  " + this.LiveCurrMediaOffset - this.TimeLength);
				// adjust if paused for longer than trim
				if (this.SliderStartOffset > this.CurrMediaOffset) {
					this.SliderStartOffset = this.CurrMediaOffset;
				}
				//BarcoUtil.log("Slider: updateMediaOffset PVR :: 4 ::this.SliderStartOffset: " + this.SliderStartOffset + " ::this.CurrMediaOffset:  " + this.CurrMediaOffset);
			}

		}
	},
	updateView: function() {
		// update view of all time related things
	  this.updateSliderHandle();
	  this.updateProgressBar();
	  this.updateMediaOffsetString(this.CurrMediaOffset);
    // make sure increment timer is stopped/started as needed
		if (this.StartMediaTime == null) {
			this.incrementStop();
		} else {
			this.incrementStart();
		}
	},
	updateSliderHandle: function() {
		var Position = 0;
		//We are getting unexpected exception "Microsoft Jscript runtime error:object required".
		//Not able to find exact reason for this exception
		//To work around this issue using try/catch.
		try {
			if (this.StartMediaTime != null) {
				var Position = Math.round((parseInt(this.CurrMediaOffset) - parseInt(this.SliderStartOffset)) / (this.TimeLength / 100));
				// check for bounds and set the position
				if (isNaN(Position))
					Position = 100;
				if (Position > 100)
					Position = 100;
				if (Position < 0)
					Position = 0;
			}
			//BarcoUtil.log("\nSlider: updateSliderHandle::: sliderPointer: " + Position);
			// Slider mark position --
			this.scrubberState.pos = Position;
			this.$scrubber.slider('value', Position);
			//this.SessionSliderToolTipTime.updatePositionToolTip(Position + "%");
		} catch (e) {
			//BarcoUtil.log("Slider: updateSliderHandle: " + e.description);
		}
	},
	
  updateProgressBar: function () {
    var ProgressStart = 0;
    var ProgressSize = 0;
	 
    if (this.StartMediaTime != null) {
      if (this.TimeMode == "PERSISTENT_PVR" || this.TimeMode == "PVR" && this.PVRStartMediaOffset != null) {
        ProgressStart = Math.round((parseInt(this.PVRStartMediaOffset) - parseInt(this.SliderStartOffset)) / (this.TimeLength / 100));
        if (isNaN(ProgressStart)) ProgressStart = 100;
        if (ProgressStart > 100) ProgressStart = 100;
        ProgressSize = Math.round((parseInt(this.LiveCurrMediaOffset) - parseInt(this.PVRStartMediaOffset)) / (this.TimeLength / 100));
        if (isNaN(ProgressSize)) ProgressSize = 100;
        if (ProgressStart + parseInt(ProgressSize) > 100) ProgressSize = 100 - ProgressStart;
      }
      else if (this.TimeMode == "PLAYBACK" || this.TimeMode == "PLAYBACK_BUSY") {
        ProgressStart = 0;
        ProgressSize = 100;
      }
    }
    //BarcoUtil.log("Slider: updateProgressBar 5 :: ProgressStart: " + ProgressStart + " ::ProgressSize:  " + ProgressSize);
    // Slider progress block size --
    this.$blkProgress.css({"left":ProgressStart + "%","width":ProgressSize + "%"});
    //this.blkProgress.repaint();
  },	 

	updateMediaOffsetString: function(Offset) {
		var MediaOffsetStr = "";
		//BarcoUtil.log("Slider: updateMediaOffsetString 0 :: Offset: " + Offset + " ::PVRStartMediaTime:  " + this.PVRStartMediaTime + " ::StartMediaTime: " + this.StartMediaTime);
		if (this.StartMediaTime != null) {
			// Slider Tooltip & Time Label -- set curr offset time as tool tip
			if (this.TimeMode === "NONE") {
				Offset = Offset - this.SliderStartOffset;
			}
			if (this.TimeMode === "LIVE") {
				Offset = Offset - (this.LiveStartMediaTime - parseInt(this.StartMediaTime));
			}
			if (this.TimeMode === "PVR") {
				if (this.isTimeshifted) {
					Offset = (Offset - (this.PVRStartMediaTime - parseInt(this.StartMediaTime)));
					Offset = (this.LiveCurrMediaOffset - Offset);
				}
				else {
					Offset = (this.PVRStartMediaTime - parseInt(this.StartMediaTime));
				}
				//BarcoUtil.log("Slider: updateMediaOffsetString 1 PVR::Offset:  " + Offset);
			}
			if (this.TimeMode === "PERSISTENT_PVR") {
				if (this.onDragSlider || this.isTimeshifted) {
					Offset = (this.LiveCurrMediaOffset - Offset);
				}
				else {
					var x1 = this.StartMediaTime - parseInt(this.PVRStartMediaTime);
					if (x1 < this.TimeLength) {
						Offset = (this.StartMediaTime - (parseInt(this.PVRStartMediaTime) + x1));
					}
					else {
						Offset = ((this.StartMediaTime - parseInt(this.PVRStartMediaTime)) - x1);
					}
				}
				//BarcoUtil.log("Slider: updateMediaOffsetString 1 PersistPVR::Offset:  " + Offset);
			}
			if (this.TimeMode === "PLAYBACK" && Offset > this.TimeLength) {
				Offset = this.TimeLength;
			}
			MediaOffsetStr = BarcoUtil.msToHMS(Offset);
		}
		// slider tool tip
		// this.sliderPointer.setTip("");
		// time label
		//BarcoUtil.log("Slider: updateMediaOffsetString 2 :: lblTimeCode: " + this.lblTimeCode + ":: MediaOffsetStr:" + MediaOffsetStr);
		if (this.$lblTimeCode != null && (this.TimeMode != "LIVE" && this.TimeMode !== "NONE")) {
			//BarcoUtil.log("Slider: updateMediaOffsetString 3_1 :: isTimeshifted:" + this.isTimeshifted);
		  if (this.onDragSlider || this.isTimeshifted || this.TimeMode === "PLAYBACK" || this.TimeMode === "PLAYBACK_BUSY") {
				this.$lblTimeCode.css("display", "block");
			}
			else {
			  this.$lblTimeCode.css("display", "none");
			}
			 this.$lblTimeCode.text(MediaOffsetStr);
		}
	},
	// extend onselect - this will handle user moving the slider position
	onSelect: function (fpVALUE, fpPREVIOUS) {
	  //Disbale the onSelect as IPAD/HTML5 has issue with seek incase PVR
	  if ((this.TimeMode === "PVR" || this.TimeMode === "PERSISTENT_PVR") && this.options.playerUsed == "streamPlayerHTML5videoHLS") {
	    return false;
	  }

		this.onDragSlider = true;
		if (!jQuery.isBlank(this.timeOutId)) {
			this._clearTimeOutId();
		}

		// in LIVE mode return false to reset the hanldle position
		if (this.TimeMode === "LIVE") {
			return false;
		}
		else {
			// calc the target media offset based on the current position
			var TgtOffset = parseInt(this.SliderStartOffset) + Math.round(fpVALUE * (this.TimeLength / 100));

			//BarcoUtil.log("Slider: onSelect:: fpVALUE: " + fpVALUE + "\n");

			//this._showSliderToolTipTimeOnDrag.call(this, TgtOffset);
			/*
			 this._hideSliderToolTipTime();
			 */

			this.goToOffset.call(this, TgtOffset, fpVALUE);

			this.onDragSlider = false;
			// return true to allow the handle to be left in the new position
			return true;
		}
	},
	onDrag: function(fpVALUE) {
		// in LIVE mode - do nothing
		//var this = this.getAncestorOfType("com.ipvs.gui.Slider");
		if (this.TimeMode === "LIVE") {
			return;
		}
    //Disbale the drag as IPAD/HTML5 has issue with seek incase PVR
		else if ((this.TimeMode === "PVR" || this.TimeMode === "PERSISTENT_PVR") && this.options.playerUsed == "streamPlayerHTML5videoHLS") {
      return;
		}
		else {
			if (!jQuery.isBlank(this.timeOutId)) {
				this._clearTimeOutId();
			}

			this.onDragSlider = true;
			// turn off position update
			this.incrementStop.call(this);
			// calc the target media offset based on the current position
			var TgtOffset = parseInt(this.SliderStartOffset) + Math.round(fpVALUE * (this.TimeLength / 100));
			//BarcoUtil.log("\nSlider: onDrag :: fpVALUE ::" + fpVALUE);
			//BarcoUtil.log("Slider: onDrag :: CALLING ::  this_updateMediaOffsetString:" + TgtOffset);
			// update the display string to show what the media offset would be
			this.updateMediaOffsetString.call(this, TgtOffset);
			//this._showSliderToolTipTimeOnDrag.call(this, TgtOffset);
		}
	},
	goToOffset: function (TgtOffset, fpVALUE) {
		// re calc current offset - for accuracy
		this.incrementStop();
		this.updateMediaOffset();
		if (this.TimeMode === "PLAYBACK" || this.TimeMode === "PLAYBACK_BUSY") {
		  this.$sessionCtl.sessionController('doSeekTo', TgtOffset);
		}
		else {
		  if (fpVALUE >= 99) {
		    this.$sessionCtl.sessionController('doCatchup');
		  }
		  else {
		    // calc the offset from current position
		    //HACK: if the Tgt Offset is 0 - the user is trying to get to the start
		    // - probably the request will fail - because we will be off by a few milliseconds
		    // - the right fix is for the middleware to truncate negative offsets to zero
		    // - for other offsets reduce the TgtOffset by 2 seconds to improve usability for now
		    if (TgtOffset > 2000) {
		      TgtOffset = parseInt(TgtOffset) - 2000;
		    }
		    else {
		      if (this.TimeMode !== "PERSISTENT_PVR") {
		        TgtOffset = 0;
		      }
		    }
		    var Offset = TgtOffset - (this.CurrMediaOffset - this.StreamLatency);
		    if (Offset > 0) {
		      this.$sessionCtl.sessionController('doSkipFwd', Offset);
		    }
		    else {
		      this.$sessionCtl.sessionController('doSkipBack', (Offset * -1));
		    }
		  }
		}
		// fire event which will result in skip fwd/back by this offset
		/*
		 this.publish({ subject: "ONSLIDERSELECT", context: Offset });
		 */
	},
	// setup interval timer to call increment function every length/100 times
	incrementStart: function() {
		 if (this.timerHandle == null) {
			 // slider has 100 discrete positions
			 var context = this;
			 this.timerHandle = setInterval(function () {context.increment(context); }, 1000);
		 }
	},
	// stop interval timer
	incrementStop: function() {
		 clearInterval(this.timerHandle);
		 this.timerHandle = null;
	},
	increment: function(context) {
		// this function is called in a global context (due to setInterval)
		 context.updateMediaOffset.call(context);
		 context.updateView.call(context);
	},

  //Show the tooltip to show the current offset
	_handleTooltipOnMouseAction: function (evt) {
	  if (this.options.playerUsed == "streamPlayerHTML5videoHLS") return;
	  var leftPos = evt.clientX;
	  var xPosition = evt.clientX - (this.$scrubber.offset().left + 1);
	  var sliderWidth = this.$scrubber.innerWidth();
	  var fpValue = Math.round((xPosition * 100) / sliderWidth);
	  var Offset = parseInt(this.SliderStartOffset) + Math.round(fpValue * (this.TimeLength / 100));
	  this.$scrubber_tooltip.html(this.getMediaOffsetStr(Offset));
	  var tooltipPos = Math.max(xPosition + this.scrubberState.tooltipTextOffset, 1);
	  this.$scrubber_tooltip.css('left', Math.round(tooltipPos)).show();
	},

	_handleLblTimeLength: function(objCollabInfo) {
		if (this.$lblTimeLength != null) {
		  if (this.TimeMode === "PLAYBACK") {
		    this.$lblTimeLength.text(BarcoUtil.msToHMS(this.TimeLength));
		  }
		  else if (this.TimeMode === "PLAYBACK_BUSY") {
		    this.$lblTimeLength.text("BUSY");
		  }
		  else {
		    this.$lblTimeLength.text("LIVE");
				if (objCollabInfo.isTimeshifted) {
				}
				else {
				}
				if (this.TimeMode != "LIVE" && this.TimeMode != "NONE") {
				}
			}
		}
	},
	/**
	 * _onClick_CatchUp:
	 */
	/*
	 SessionSlider_prototype._onClick_CatchUp = function () {
	 var Offset = "CATCHUP";
	 // fire event which will result in skip fwd/back by this offset
	 this.publish({ subject: "ONSLIDERSELECT", context: Offset });
	 }
	 */

	/**
	 * _getSliderWidth:
	 */
	/*
	 SessionSlider_prototype._getSliderWidth = function () {
	 var sliderHtmlElement = this.sliderPointer.getRendered();
	 var sliderWidth = parseInt(sliderHtmlElement.clientWidth - 14);
	 return sliderWidth;
	 }
	 */

	/**
	 * _clearTimeOutId:
	 */
	_clearTimeOutId: function() {
		/*
		 this.SessionSliderToolTipTime.cmpHide();
		 clearTimeout(this.timeOutId);
		 */
		this.timeOutId = null;
	},
	/**
	 * _hideSliderToolTipTime:
	 */
	/*
	 SessionSlider_prototype._hideSliderToolTipTime = function () {
	 var ThisClass = this;
	 this.timeOutId = setTimeout(function () {
	 ThisClass.SessionSliderToolTipTime.cmpHide();
	 ThisClass.timeOutId = null;
	 }, 1000);
	 }
	 */

	/**
	 * _showSliderToolTipTime:
	 */
	/*
	 SessionSlider_prototype._showSliderToolTipTime = function (MediaOffsetStr, xPosition) {
	 this.SessionSliderToolTipTime.updateToolTip(MediaOffsetStr);
	 this.SessionSliderToolTipTime.cmpShow();
	 this.SessionSliderToolTipTime.updatePositionToolTip(xPosition);
	 this._hideSliderToolTipTime();
	 }
	 */


	/**
	 * _getOffsetSliderToolTipTime:
	 */
	/*
	 SessionSlider_prototype._getOffsetSliderToolTipTime = function (xPosition) {
	 var sliderWidth = this._getSliderWidth();
	 var fpValue = Math.round((xPosition * 100) / sliderWidth);
	 //BarcoUtil.log("\nSlider:_getOffsetSliderToolTipTime:  Calculated fpValue:" + fpValue + "\n");
	 var Offset = parseInt(this.SliderStartOffset) + Math.round(fpValue * (this.TimeLength / 100));
	 return Offset;
	 }
	 */

	/**
	 * _getOffsetSliderToolTipTime:
	 */
	/*
	 SessionSlider_prototype._getPxOnSlider = function (fpValue) {
	 var sliderWidth = this._getSliderWidth();
	 var xPosition = Math.round((fpValue * sliderWidth) / 100);
	 return xPosition;
	 }
	 */

	/**
	 * getMediaOffsetStr:
	 */
	getMediaOffsetStr: function(offSet) {
		if (this.TimeMode === "PVR") {
			offSet = (offSet - (this.PVRStartMediaTime - parseInt(this.StartMediaTime)));
			offSet = (this.LiveCurrMediaOffset - offSet);
			//BarcoUtil.log("Slider: updateMediaOffsetString 1 PVR::Offset:  " + Offset);
		}
		if (this.TimeMode === "PERSISTENT_PVR") {
			offSet = (this.LiveCurrMediaOffset - offSet);
		}
		return BarcoUtil.msToHMS(offSet);
	},
	/**
	 * _onSpy:
	 */
	/*
	 SessionSlider_prototype.onSpy = function (objGUI, objEvent) {
	 var eventType = objEvent.getType();
	 if (eventType != "mouseover") return;
	 if (this.TimeMode == "NONE" || this.TimeMode == "LIVE" ) {
	 return;
	 }
	 
	 //hide and clear 
	 if (!jQuery.isBlank(this.timeOutId)) {
	 this._clearTimeOutId();
	 }
	 
	 var blkName = objGUI.jsxname;
	 var imageObj = objGUI.getFirstChild();
	 var imgName = imageObj.jsxname;
	 var fpVal = imageObj.getValue();
	 var imgVal = this._getPxOnSlider(fpVal);
	 var mouseOverXPosition = objEvent.getOffsetX();
	 var clientX = objEvent.clientX();
	 var sliderWidth = this._getSliderWidth();
	 var adjustPos = false;
	 
	 //BarcoUtil.log("\nSlider:onSpy: BEFORE mouseOverXPosition: " + mouseOverXPosition + " clientX: " + clientX + "\n");
	 clientX = parseInt(clientX - 518) < 0 ? 0 : parseInt(clientX - 518);
	 
	 if (clientX >= imgVal || parseInt(clientX - 1) >= imgVal) {
	 if (mouseOverXPosition >= 0 && mouseOverXPosition < 15) {
	 mouseOverXPosition = imgVal;
	 adjustPos = true;
	 }
	 }
	 
	 if (mouseOverXPosition >= 0 && mouseOverXPosition < 10 && imgVal <= 5) {
	 mouseOverXPosition = mouseOverXPosition;
	 }
	 
	 mouseOverXPosition = mouseOverXPosition < 0 ? 0 : mouseOverXPosition > sliderWidth ? sliderWidth : mouseOverXPosition;
	 
	 //BarcoUtil.log("\nSlider:onSpy:  eventType: " + eventType + " objGUI Name: " + blkName + " AFTER mouseOverXPosition: " + mouseOverXPosition + " imgName: " + imgName + " imgVal:" + imgVal + " fpVal:" + fpVal + " clientX:" + clientX + "\n");
	 
	 var offSet = this._getOffsetSliderToolTipTime(mouseOverXPosition);
	 var MediaOffsetStr = this.getMediaOffsetStr(offSet);
	 
	 if (adjustPos) {
	 mouseOverXPosition = parseInt(mouseOverXPosition + 4);
	 }
	 mouseOverXPosition = mouseOverXPosition == sliderWidth ? parseInt(sliderWidth + 4) : mouseOverXPosition;
	 
	 //show the block
	 this._showSliderToolTipTime(MediaOffsetStr, mouseOverXPosition);
	 }
	 */


	/**
	 * _showSliderToolTipTimeOnDrag:
	 */
	//_showSliderToolTipTimeOnDrag: function(Offset) {
	//	var MediaOffsetStr = "";
	//	if (this.StartMediaTime != null) {
	//		// Slider Tooltip & Time Label -- set curr offset time as tool tip
	//		if (this.TimeMode === "NONE") {
	//			Offset = Offset - this.SliderStartOffset;
	//		}
	//		if (this.TimeMode === "LIVE") {
	//			Offset = Offset - (this.LiveStartMediaTime - parseInt(this.StartMediaTime));
	//		}
	//		if (this.TimeMode === "PVR") {
	//			if (this.isTimeshifted) {
	//				Offset = (Offset - (this.PVRStartMediaTime - parseInt(this.StartMediaTime)));
	//				Offset = (this.LiveCurrMediaOffset - Offset);
	//			}
	//			else {
	//				Offset = (this.PVRStartMediaTime - parseInt(this.StartMediaTime));
	//			}
	//		}
	//		if (this.TimeMode === "PERSISTENT_PVR") {
	//			if (this.onDragSlider || this.isTimeshifted) {
	//				Offset = (this.LiveCurrMediaOffset - Offset);
	//			}
	//			else {
	//				var x1 = this.StartMediaTime - parseInt(this.PVRStartMediaTime);
	//				if (x1 < this.TimeLength) {
	//					Offset = (this.StartMediaTime - (parseInt(this.PVRStartMediaTime) + x1));
	//				}
	//				else {
	//					Offset = ((this.StartMediaTime - parseInt(this.PVRStartMediaTime)) - x1);
	//				}
	//			}
	//		}
	//		if (this.TimeMode === "PLAYBACK" && Offset > this.TimeLength) {
	//			Offset = this.TimeLength;
	//		}
	//		MediaOffsetStr = BarcoUtil.msToHMS(Offset);
	//	}
	//	if (this.lblTimeCode != null) {
	//		if (this.TimeMode !== "NONE") {
	//			/*
	//			 var actualBlk2Width = this.SessionSliderToolTipTime.getBlkToolTipWidth();
	//			 var blkToolTipPosition = Math.round((actualBlk2Width / this.TimeLength) * Offset);
	//			 if (this.TimeMode != "PLAYBACK") {
	//			 blkToolTipPosition = actualBlk2Width - blkToolTipPosition;
	//			 }
	//			 blkToolTipPosition = parseInt(blkToolTipPosition + 4);
	//			 this.SessionSliderToolTipTime.updatePositionToolTip(blkToolTipPosition);
	//			 */
	//		}
	//		/*
	//		 this.SessionSliderToolTipTime.updateToolTip(MediaOffsetStr);
	//		 this.SessionSliderToolTipTime.cmpShow();
	//		 */
	//		//BarcoUtil.log("\nSlider:_showSliderToolTipTimeOnDrag:  blkToolTipPosition: " + blkToolTipPosition + " MediaOffsetStr: " + MediaOffsetStr + "\n");
	//	}
  //}

  //*************Edit Media supporting methods***********
	getCurrentOffset: function () {
	  return this.CurrMediaOffset;
	},

  //Add a pointer as soon as user selects start point
	addSegmentStart: function (objSegment) {
	  var segment = jQuery('<div id="' + objSegment.id + '" class="segmentInProgress"></div>');
	  this.$scrubber.append(segment);
	  var segmentLeft = Math.round(objSegment.Start / (this.TimeLength / 100));
	  segment.css('left', segmentLeft + '%');
	  segment.css('width', '3px');
	},

  //Remove the start pointer and add the segment
	addSegment: function (objSegment) {
	  this.deleteSegment(objSegment.id);
	  var segmentLeft = Math.round(objSegment.Start / (this.TimeLength / 100));
	  var segmentWidth = Math.round((objSegment.End - objSegment.Start) / (this.TimeLength / 100));
	  var segment = jQuery('<div id="' + objSegment.id + '" class="segmentAdd"></div>');
	  this.$scrubber.append(segment);
	  segment.css('left', segmentLeft + '%');
	  segment.css('width', segmentWidth + '%');
	  segment.on('click', { id: objSegment.id }, jQuery.proxy(this.onSegmentSelected, this));
	},

	onSegmentSelected: function (eventObj) {	  
	  if (this.$sessionCtl.sessionController('getEditMediaPanelRef').editMediaPanel('onSegmentSelected', eventObj.data.id)) {
	    this.$elem.find('#' + eventObj.data.id).addClass('segmentDelete');
	  }
	},

	deleteSegment: function (segmentId) {
	  this.$elem.find('#' + segmentId).remove();
	},

	deleteAllSegments: function () {
	  this.$elem.find('.segmentInProgress').remove();
	  this.$elem.find('.segmentAdd').remove();
	},

	cancelDeleteSegment: function (segmentId) {
	  this.$elem.find('#' + segmentId).removeClass('segmentDelete').addClass('segmentAdd');
	}
  //*************End*************************************
});
jQuery.plugin('sessionSlider', SessionSlider);
