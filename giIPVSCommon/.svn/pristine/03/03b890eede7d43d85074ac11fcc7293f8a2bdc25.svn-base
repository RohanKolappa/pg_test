/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* SessionSlider class.
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.SessionSlider", jsx3.gui.Block, null, function (SessionSlider, SessionSlider_prototype) {

  /** @private @jsxobf-clobber */
  SessionSlider_prototype._LOG = jsx3.util.Logger.getLogger(SessionSlider.jsxclass.getName());

  SessionSlider_prototype.onAfterAttach = function () {
    this.jsxsuper();
    this.clsApp = com.ipvs.app.App.clsApp;
    if (!this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    }
  }
  SessionSlider_prototype._onAppInit = function () {
    this.sliderPointer = this.getDescendantOfName("sliderPointer");
    this.blkProgress = this.getDescendantOfName("sliderTrackProgress");
    this.lblTimeCode = this.getDescendantOfName("lblTimeCode");
    this.lblPvrBuffer = this.getDescendantOfName("lblPvrBuffer");
    this.lblTimeLength = this.getDescendantOfName("lblTimeLength");
    this.blkBookmarksListContainer = this.getDescendantOfName("blkBookmarksListContainer");
    this.blkTrackBG = this.getDescendantOfName("blkTrackBG");
    this.sliderTrackContainer = this.getDescendantOfName("sliderTrackContainer");
    this.blkSliderContainer = this.getDescendantOfName("blkSliderContainer");
    this.blkSlider = this.getDescendantOfName("blkSlider");

    this.lblTimeLength.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lblTimeLength.setEvent("this.onClick();", jsx3.gui.Interactive.JSXCLICK);
    this.lblTimeLength.onClick = jsx3.$F(this._onClick_CatchUp).bind(this);

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

    if (!jsx3.util.strEmpty(jsx3.gui.Slider.BRIDGE_EVENTS)) {
      jsx3.gui.Slider.BRIDGE_EVENTS[jsx3.gui.Event.MOUSEWHEEL] = false;
    }
    else {
      jsx3.gui.Slider.yg[jsx3.gui.Event.MOUSEWHEEL] = false;
    }

    this.sliderPointer.removeEvent(jsx3.gui.Interactive.CHANGE);
    this.sliderPointer.setEvent("this.onSelect(fpVALUE,fpPREVIOUS);", jsx3.gui.Interactive.CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.sliderPointer.onSelect = jsx3.$F(this.onSelect).bind(this);
    // setup the select event handler
    this.sliderPointer.removeEvent(jsx3.gui.Interactive.INCR_CHANGE);
    this.sliderPointer.setEvent("this.onDrag(fpVALUE);", jsx3.gui.Interactive.INCR_CHANGE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.sliderPointer.onDrag = jsx3.$F(this.onDrag).bind(this);

    this.blkSlider.setCanSpy(jsx3.Boolean.TRUE);
    this.blkSlider.removeEvent(jsx3.gui.Interactive.SPYGLASS);
    this.blkSlider.setEvent("this.onSpy(this, objEVENT);", jsx3.gui.Interactive.SPYGLASS);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.blkSlider.onSpy = jsx3.$F(this.onSpy).bind(this);

    this.SessionSliderToolTipTime = this.clsApp.getDescendantsOfType("com.ipvs.gui.SessionSliderToolTipTime")[0];
    // init the offset and position
    this.initMediaOffset();
    this.updateView();
  }


  // extend show
  SessionSlider_prototype.show = function (EventVar, EventVal) {
    if (this.TimeMode != "NONE" && this.sliderPointer.setEnabled != null) {
      this.sliderPointer.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this.sliderPointer.setHandleImage("images/slider/handleEnabled.gif");
    //this.sliderPointer.repaint();
    this.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.repaint();
  }

  // override hide
  SessionSlider_prototype.hide = function () {
    if (this.sliderPointer.setEnabled != null) {
      this.sliderPointer.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    this.sliderPointer.setHandleImage("images/slider/handleDisabled.gif");
    //this.sliderPointer.repaint();
    this.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.repaint();
  }


  //check mediaOffset before set it, to ensure this.StartMediaTime not null for join mode
  SessionSlider_prototype.checkAndSetMediaOffset = function (objCollabInfo) {
    if (this.timerHandle == null) {
      this.startMediaOffset(objCollabInfo);
    }
    this.setMediaOffset(objCollabInfo);
  }

  SessionSlider_prototype.applyCollaborationInfo = function (objCollabInfo) {
    this.isTimeshifted = objCollabInfo.isTimeshifted;
    // process action
    switch (objCollabInfo.action) {
      case "Start":
        this.startMediaOffset(objCollabInfo);
        break;
      case "Stop":
        this.initMediaOffset();
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
        this._LOG.error("SessionSlider::Unknown Action:" + objCollabInfo.action);
        //        this.updateMediaOffset();
        break;
    }

    if(objCollabInfo.isSourceSelected == "IMAGE") {
      this.initMediaOffset();
    }
    this.updateView();

  }
  SessionSlider_prototype.initMediaOffset = function () {
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

    this.SliderStartOffset = 0;
    this.CurrMediaOffset = 0;
    if (this.lblTimeLength != null) {
      this.lblTimeLength.setText("", true);
    }

    if (this.lblPvrBuffer != null) {
      this.lblPvrBuffer.setText("", true);
    }
    this.hide();
  }

  SessionSlider_prototype.startMediaOffset = function (objCollabInfo) {
    //make sure the collab state is Streaming
    if (com.ipvs.gui.SessionControllerI.CONNSTATE[objCollabInfo.state] < com.ipvs.gui.SessionControllerI.CONNSTATE['STREAMING']) return;
    // default time mode = live
    this.TimeMode = "LIVE";
    if (objCollabInfo.pvrInfo.canPVR) {
      this.TimeMode = objCollabInfo.timeMode;
    }

    if (this.TimeMode == null || this.TimeMode == "") {
      this.TimeMode = "NONE";
    }
    // default time length = 1 hour
    this.TimeLength = objCollabInfo.pvrInfo.timeLength;

    if (jsx3.util.strEmpty(this.TimeLength)) {
      this.TimeLength = this.DefaultTimeLength;
    }
    //this._LOG.warn("Slider: startMediaOffset 0 :: this.TimeLength: " + this.TimeLength);

    this._handleLblTimeLength(objCollabInfo);

    // default timescale is 1:1 play
    this.TimeScale = objCollabInfo.timeInfo.timeScale;
    if (this.TimeScale == null || this.TimeScale == "") {
      this.TimeScale = 1;
    }
    this.StreamStartOffset = objCollabInfo.timeInfo.startOffset;
    if (this.StreamStartOffset == null || this.StreamStartOffset == "") {
      this.StreamStartOffset = 0;
    }
    //this._LOG.warn("Slider: startMediaOffset 1 :: this.StreamStartOffset: " + this.StreamStartOffset);

    var CurrDateTime = this.clsApp.clsServiceAgent.getCurrentServerTime();
    //this._LOG.warn("Slider: startMediaOffset 2 :: CurrDateTime: " + CurrDateTime);

    // assume we are starting now
    //and media offset 0 (unless mode is LIVE)
    // in LIVE mode Media Offset = MediaTime
    this.StartMediaTime = objCollabInfo.timeInfo.startWC;
    //this._LOG.warn("Slider: startMediaOffset 3 :: this.StartMediaTime: " + this.StartMediaTime);

    if (this.StartMediaTime == null || this.StartMediaTime == "") {
      this.StartMediaTime = CurrDateTime;
    }
    else if (this.StartMediaTime > CurrDateTime) {
      this.StartMediaTime = CurrDateTime;
    }
    //this._LOG.warn("Slider: startMediaOffset 4 :: this.StartMediaTime: " + this.StartMediaTime + " ::CurrDateTime:" + CurrDateTime);

    if (this.TimeMode == "PERSISTENT_PVR") {
      this.PVRStartMediaTime = objCollabInfo.pvrInfo.startTC;
      //this._LOG.warn("Slider: startMediaOffset 5 :: this.PVRStartMediaTime: " + this.PVRStartMediaTime + " ::(CurrDateTime - this.TimeLength):" + CurrDateTime - this.TimeLength);
      if (this.PVRStartMediaTime < this.StartMediaTime - this.TimeLength) {
        this.PVRStartMediaTime = this.StartMediaTime - this.TimeLength;
      }
      //this._LOG.warn("Slider: startMediaOffset 6 :: this.PVRStartMediaTime: " + this.PVRStartMediaTime);
    }

    if (this.TimeMode != "PLAYBACK") {
      this.lblTimeCode.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      this.lblTimeCode.repaint();
    }
    else {
      this.lblTimeCode.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      this.lblTimeCode.repaint();
    }

    // get the last media time and offset
    this.LastMediaTime = objCollabInfo.timeInfo.lastWC;
    if (this.LastMediaTime == null || this.LastMediaTime == "") {
      this.LastMediaTime = CurrDateTime;
    }
    else if (this.LastMediaTime > CurrDateTime) {
      this.LastMediaTime = CurrDateTime;
    }
    this.LastMediaOffset = objCollabInfo.timeInfo.lastOffset;
    if (this.LastMediaOffset == null || this.LastMediaOffset == "") {
      this.LastMediaOffset = this.LastMediaTime - this.StartMediaTime;
    }

    this.show();
    // done with start - rest is handled by update
    this.updateMediaOffset();
  }

  SessionSlider_prototype.setMediaOffset = function (objCollabInfo) {
    // get curr media offset from the offset that came with the action event
    // set timescale
    this.TimeScale = objCollabInfo.timeInfo.timeScale;
    this.LastMediaOffset = objCollabInfo.timeInfo.lastOffset;
    this.LastMediaTime = objCollabInfo.timeInfo.lastWC;
    this._handleLblTimeLength(objCollabInfo);
  }

  SessionSlider_prototype.updateMediaOffset = function () {
    if (this.StartMediaTime != null) {
      var CurrDateTime = this.clsApp.clsServiceAgent.getCurrentServerTime();
      // calc media offset where live is
      this.LiveCurrMediaOffset = CurrDateTime - this.StartMediaTime;
      // calc curr media offset (in milliseconds) based on milliseconds since media time and timescale (ie play mode)    
      this.CurrMediaOffset = Math.round(parseInt(this.LastMediaOffset) + (parseInt(CurrDateTime) - parseInt(this.LastMediaTime)) * this.TimeScale);
      //this._LOG.warn("Slider: updateMediaOffset:: 0 ::CurrDateTime: " + CurrDateTime + " \n::this.LiveCurrMediaOffset:  " + this.LiveCurrMediaOffset + "\n::this.CurrMediaOffset:" + this.CurrMediaOffset + "\n::this.StartMediaTime:" + this.StartMediaTime);

      // by default slider starts at offset 0
      this.SliderStartOffset = 0;
      // if time mode is still NONE
      if (this.TimeMode == "NONE") {
        // calc the slider start offset
        this.SliderStartOffset = this.LiveCurrMediaOffset;
      }
      // if in LIVE mode 
      if (this.TimeMode == "LIVE") {
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
      if (this.TimeMode == "PERSISTENT_PVR" || this.TimeMode == "PVR") {
        // if time mode just changed to PVR - init PVRStartTime
        if (this.PVRStartMediaTime == null) {
          //this.PVRStartMediaTime = CurrDate.getTime();
          this.PVRStartMediaTime = this.StartMediaTime;
          this.LastMediaTime = CurrDateTime;
          this.LastMediaOffset = this.CurrMediaOffset;
        }
        // calc the start of the PVR bar fill
        this.PVRStartMediaOffset = this.LiveCurrMediaOffset - this.TimeLength;
        //this._LOG.warn("Slider: updateMediaOffset PVR :: 1 ::this.PVRStartMediaTime:  " + this.PVRStartMediaTime + "::this.PVRStartMediaOffset:" + this.PVRStartMediaOffset + "::TimeLength:" + this.TimeLength + "::(" + this.LiveCurrMediaOffset + " - " + this.TimeLength + "):");
        // adjust if trimming has kicked in
        if (this.PVRStartMediaOffset < (this.PVRStartMediaTime - this.StartMediaTime)) {
          this.PVRStartMediaOffset = this.PVRStartMediaTime - this.StartMediaTime;
        }
        //this._LOG.warn("Slider: updateMediaOffset PVR :: 2 ::this.PVRStartMediaOffset: " + this.PVRStartMediaOffset + " ::(" + this.PVRStartMediaTime + " - " + this.StartMediaTime + "):  ");
        // calc the slider start offset
        this.SliderStartOffset = this.LiveCurrMediaOffset - this.TimeLength;
        //this._LOG.warn("Slider: updateMediaOffset PVR :: 3 ::this.SliderStartOffset: " + this.SliderStartOffset + " ::(" + this.LiveCurrMediaOffset + " - " + this.TimeLength + "):  " + this.LiveCurrMediaOffset - this.TimeLength);
        // adjust if paused for longer than trim
        if (this.SliderStartOffset > this.CurrMediaOffset) {
          this.SliderStartOffset = this.CurrMediaOffset;
        }
        //this._LOG.warn("Slider: updateMediaOffset PVR :: 4 ::this.SliderStartOffset: " + this.SliderStartOffset + " ::this.CurrMediaOffset:  " + this.CurrMediaOffset);
      }

    }
  }

  SessionSlider_prototype.updateView = function () {
    // update view of all time related things
    this.updateSliderHandle();
    this.updateProgressBar();
    this.updateMediaOffsetString(this.CurrMediaOffset);
    // make sure increment timer is stopped/started as needed
    if (this.StartMediaTime == null) {
      this.incrementStop();
    }
    else {
      this.incrementStart();
    }
  }

  SessionSlider_prototype.updateSliderHandle = function () {
    var Position = 0;
    //We are getting unexpected exception "Microsoft Jscript runtime error:object required".
    //Not able to find exact reason for this exception
    //To work around this issue using try/catch.
    try {
      if (this.StartMediaTime != null) {
        var Position = Math.round((parseInt(this.CurrMediaOffset) - parseInt(this.SliderStartOffset)) / (this.TimeLength / 100));
        // check for bounds and set the position
        if (isNaN(Position)) Position = 100;
        if (Position > 100) Position = 100;
        if (Position < 0) Position = 0;
      }
      //this._LOG.error("\nSlider: updateSliderHandle::: sliderPointer: " + Position);
      // Slider mark position --
      this.sliderPointer.setValue(Position);
      this.sliderPointer.repaint();
      //this.SessionSliderToolTipTime.updatePositionToolTip(Position + "%");
    } catch (e) {
      this._LOG.warn("Slider: updateSliderHandle: " + e.description);
    }
  }

  SessionSlider_prototype.updateProgressBar = function () {
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
      else if (this.TimeMode == "PLAYBACK") {
        ProgressStart = 0;
        ProgressSize = 100;
      }
    }
    //this._LOG.warn("Slider: updateProgressBar 5 :: ProgressStart: " + ProgressStart + " ::ProgressSize:  " + ProgressSize);
    // Slider progress block size -- 
    this.blkProgress.setLeft(ProgressStart + "%");
    this.blkProgress.setWidth(ProgressSize + "%");
    this.blkProgress.repaint();
  }

  SessionSlider_prototype.updateMediaOffsetString = function (Offset) {
    var MediaOffsetStr = "";
    //this._LOG.warn("Slider: updateMediaOffsetString 0 :: Offset: " + Offset + " ::PVRStartMediaTime:  " + this.PVRStartMediaTime + " ::StartMediaTime: " + this.StartMediaTime);
    if (this.StartMediaTime != null) {
      // Slider Tooltip & Time Label -- set curr offset time as tool tip
      if (this.TimeMode == "NONE") {
        Offset = Offset - this.SliderStartOffset;
      }
      if (this.TimeMode == "LIVE") {
        Offset = Offset - (this.LiveStartMediaTime - parseInt(this.StartMediaTime));
      }
      if (this.TimeMode == "PVR") {
        if (this.isTimeshifted) {
          Offset = (Offset - (this.PVRStartMediaTime - parseInt(this.StartMediaTime)));
          Offset = (this.LiveCurrMediaOffset - Offset);
        }
        else {
          Offset = (this.PVRStartMediaTime - parseInt(this.StartMediaTime));
        }
        //this._LOG.warn("Slider: updateMediaOffsetString 1 PVR::Offset:  " + Offset);
      }
      if (this.TimeMode == "PERSISTENT_PVR") {
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
        //this._LOG.warn("Slider: updateMediaOffsetString 1 PersistPVR::Offset:  " + Offset);
      }
      if (this.TimeMode == "PLAYBACK" && Offset > this.TimeLength) {
        Offset = this.TimeLength;
      }
      MediaOffsetStr = sec2HMS(Offset);
    }
    // slider tool tip
    this.sliderPointer.setTip("");
    // time label
    //this._LOG.warn("Slider: updateMediaOffsetString 2 :: lblTimeCode: " + this.lblTimeCode + ":: MediaOffsetStr:" + MediaOffsetStr);
    if (this.lblTimeCode != null && (this.TimeMode != "LIVE" && this.TimeMode != "NONE")) {
      //this._LOG.warn("Slider: updateMediaOffsetString 3_1 :: isTimeshifted:" + this.isTimeshifted);
      if (this.onDragSlider || this.isTimeshifted || this.TimeMode == "PLAYBACK") {
        this.lblTimeCode.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
      }
      else {
        this.lblTimeCode.setDisplay(jsx3.gui.Block.DISPLAYNONE);
      }
      this.lblTimeCode.setText(MediaOffsetStr);
      this.lblTimeCode.repaint();
      //this._LOG.warn("Slider: updateMediaOffsetString 3 :: lblTimeCode: UPDTED");
    }
  }

  // extend onselect - this will handle user moving the slider position
  SessionSlider_prototype.onSelect = function (fpVALUE, fpPREVIOUS) {
    this.onDragSlider = true;
    if (!jsx3.util.strEmpty(this.timeOutId)) {
      this._clearTimeOutId();
    }

    // in LIVE mode return false to reset the hanldle position
    if (this.TimeMode == "LIVE") {
      return false;
    }
    else {
      // calc the target media offset based on the current position
      var TgtOffset = parseInt(this.SliderStartOffset) + Math.round(fpVALUE * (this.TimeLength / 100));

      //this._LOG.error("Slider: onSelect:: fpVALUE: " + fpVALUE + "\n");

      this._showSliderToolTipTimeOnDrag.call(this, TgtOffset);
      this._hideSliderToolTipTime();

      this.goToOffset.call(this, TgtOffset);

      this.onDragSlider = false;
      // return true to allow the handle to be left in the new position
      return true;
    }
  }

  SessionSlider_prototype.onDrag = function (fpVALUE) {
    // in LIVE mode - do nothing
    //var this = this.getAncestorOfType("com.ipvs.gui.Slider");
    if (this.TimeMode == "LIVE") {
      return;
    }
    else {
      if (!jsx3.util.strEmpty(this.timeOutId)) {
        this._clearTimeOutId();
      }

      this.onDragSlider = true;
      // turn off position update
      this.incrementStop.call(this);
      // calc the target media offset based on the current position
      var TgtOffset = parseInt(this.SliderStartOffset) + Math.round(fpVALUE * (this.TimeLength / 100));
      //this._LOG.warn("\nSlider: onDrag :: fpVALUE ::" + fpVALUE);
      //this._LOG.warn("Slider: onDrag :: CALLING ::  this_updateMediaOffsetString:" + TgtOffset);
      // update the display string to show what the media offset would be
      this.updateMediaOffsetString.call(this, TgtOffset);
      this._showSliderToolTipTimeOnDrag.call(this, TgtOffset);
    }
  }

  SessionSlider_prototype.goToOffset = function (TgtOffset) {
    // re calc current offset - for accuracy
    this.incrementStop();
    this.updateMediaOffset();
    // calc the offset from current position
    //HACK: if the Tgt Offset is 0 - the user is trying to get to the start
    // - probably the request will fail - because we will be off by a few milliseconds
    // - the right fix is for the middleware to truncate negative offsets to zero
    // - for other offsets reduce the TgtOffset by 2 seconds to improve usability for now
    if (TgtOffset > 2000) {
      TgtOffset = parseInt(TgtOffset) - 2000;
    }
    else {
      if (this.TimeMode != "PERSISTENT_PVR") {
        TgtOffset = 0;
      }
    }
    var Offset = TgtOffset - this.CurrMediaOffset;
    // fire event which will result in skip fwd/back by this offset
    this.publish({ subject: "ONSLIDERSELECT", context: Offset });
  }

  // setup interval timer to call increment function every length/100 times
  SessionSlider_prototype.incrementStart = function () {
    if (this.timerHandle == null) {
      // slider has 100 discrete positions
      var ThisClass = this;
      this.timerHandle = setInterval(function () { SessionSlider_prototype.increment(ThisClass); }, 1000);
    }
  }
  // stop interval timer
  SessionSlider_prototype.incrementStop = function () {
    clearInterval(this.timerHandle);
    this.timerHandle = null;
  }

  // this function is called in a global context (due to setInterval)
  // the value of "this" in here is not our "this" - so we use the special var ThisClass instead
  SessionSlider_prototype.increment = function (context) {
    context.updateMediaOffset.call(context);
    context.updateView.call(context);
  }

  SessionSlider_prototype._handleLblTimeLength = function (objCollabInfo) {
    if (this.lblTimeLength != null) {
      if (this.TimeMode == "PLAYBACK") {
        this.lblTimeLength.removeEvent(jsx3.gui.Interactive.JSXCLICK);

        this.lblTimeLength.setText(sec2HMS(this.TimeLength), true);
        this.lblTimeLength.setCursor("default", true);
        this.lblTimeLength.setClassName("liveOff");
        this.lblTimeLength.repaint();

        this.lblPvrBuffer.setText("", true);
      }
      else {
        if (objCollabInfo.isTimeshifted) {
          this.lblTimeLength.removeEvent(jsx3.gui.Interactive.JSXCLICK);
          this.lblTimeLength.setEvent("this.onClick();", jsx3.gui.Interactive.JSXCLICK);
          this.lblTimeLength.onClick = jsx3.$F(this._onClick_CatchUp).bind(this);

          this.lblTimeLength.setText("LIVE", true);
          this.lblTimeLength.setCursor("hand", true);
          this.lblTimeLength.setClassName("liveOn");
          this.lblTimeLength.repaint();
        }
        else {
          this.lblTimeLength.removeEvent(jsx3.gui.Interactive.JSXCLICK);

          this.lblTimeLength.setText("LIVE", true);
          this.lblTimeLength.setCursor("default", true);
          this.lblTimeLength.setClassName("liveOff");
          this.lblTimeLength.repaint();
        }
        if (this.TimeMode != "LIVE" && this.TimeMode != "NONE") {
          this.lblPvrBuffer.setText(sec2HMS(this.TimeLength), true);
        }
      }
    }
  }

  /**
  * _onClick_CatchUp:
  */
  SessionSlider_prototype._onClick_CatchUp = function () {
    var Offset = "CATCHUP";
    // fire event which will result in skip fwd/back by this offset
    this.publish({ subject: "ONSLIDERSELECT", context: Offset });
  }

  /**
  * _getSliderWidth:
  */
  SessionSlider_prototype._getSliderWidth = function () {
    var sliderHtmlElement = this.sliderPointer.getRendered();
    var sliderWidth = parseInt(sliderHtmlElement.clientWidth - 14);
    return sliderWidth;
  }

  /**
  * _clearTimeOutId:
  */
  SessionSlider_prototype._clearTimeOutId = function () {
    this.SessionSliderToolTipTime.cmpHide();
    clearTimeout(this.timeOutId);
    this.timeOutId = null;
  }

  /**
  * _hideSliderToolTipTime:
  */
  SessionSlider_prototype._hideSliderToolTipTime = function () {
    var ThisClass = this;
    this.timeOutId = setTimeout(function () {
      ThisClass.SessionSliderToolTipTime.cmpHide();
      ThisClass.timeOutId = null;
    }, 1000);
  }

  /**
  * _showSliderToolTipTime:
  */
  SessionSlider_prototype._showSliderToolTipTime = function (MediaOffsetStr, xPosition) {
    this.SessionSliderToolTipTime.updateToolTip(MediaOffsetStr);
    this.SessionSliderToolTipTime.cmpShow();
    this.SessionSliderToolTipTime.updatePositionToolTip(xPosition);
    this._hideSliderToolTipTime();
  }


  /**
  * _getOffsetSliderToolTipTime:
  */
  SessionSlider_prototype._getOffsetSliderToolTipTime = function (xPosition) {
    var sliderWidth = this._getSliderWidth();
    var fpValue = Math.round((xPosition * 100) / sliderWidth);
    //this._LOG.error("\nSlider:_getOffsetSliderToolTipTime:  Calculated fpValue:" + fpValue + "\n");
    var Offset = parseInt(this.SliderStartOffset) + Math.round(fpValue * (this.TimeLength / 100));
    return Offset;
  }

  /**
  * _getOffsetSliderToolTipTime:
  */
  SessionSlider_prototype._getPxOnSlider = function (fpValue) {
    var sliderWidth = this._getSliderWidth();
    var xPosition = Math.round((fpValue * sliderWidth) / 100);
    return xPosition;
  }

  /**
  * getMediaOffsetStr:
  */
  SessionSlider_prototype.getMediaOffsetStr = function (offSet) {
    if (this.TimeMode == "PVR") {
      offSet = (offSet - (this.PVRStartMediaTime - parseInt(this.StartMediaTime)));
      offSet = (this.LiveCurrMediaOffset - offSet);
      //this._LOG.warn("Slider: updateMediaOffsetString 1 PVR::Offset:  " + Offset);
    }
    if (this.TimeMode == "PERSISTENT_PVR") {
      offSet = (this.LiveCurrMediaOffset - offSet);
    }
    return sec2HMS(offSet);
  }

  /**
  * _onSpy:
  */
  SessionSlider_prototype.onSpy = function (objGUI, objEvent) {
    var eventType = objEvent.getType();
    if (eventType != "mouseover") return;
    if (this.TimeMode == "NONE" || this.TimeMode == "LIVE" ) {
      return;
    }

    //hide and clear 
    if (!jsx3.util.strEmpty(this.timeOutId)) {
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

    //this._LOG.error("\nSlider:onSpy: BEFORE mouseOverXPosition: " + mouseOverXPosition + " clientX: " + clientX + "\n");
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

    //this._LOG.error("\nSlider:onSpy:  eventType: " + eventType + " objGUI Name: " + blkName + " AFTER mouseOverXPosition: " + mouseOverXPosition + " imgName: " + imgName + " imgVal:" + imgVal + " fpVal:" + fpVal + " clientX:" + clientX + "\n");

    var offSet = this._getOffsetSliderToolTipTime(mouseOverXPosition);
    var MediaOffsetStr = this.getMediaOffsetStr(offSet);

    if (adjustPos) {
      mouseOverXPosition = parseInt(mouseOverXPosition + 4);
    }
    mouseOverXPosition = mouseOverXPosition == sliderWidth ? parseInt(sliderWidth + 4) : mouseOverXPosition;

    //show the block
    this._showSliderToolTipTime(MediaOffsetStr, mouseOverXPosition);
  }


  /**
  * _showSliderToolTipTimeOnDrag:
  */
  SessionSlider_prototype._showSliderToolTipTimeOnDrag = function (Offset) {
    var MediaOffsetStr = "";
    if (this.StartMediaTime != null) {
      // Slider Tooltip & Time Label -- set curr offset time as tool tip
      if (this.TimeMode == "NONE") {
        Offset = Offset - this.SliderStartOffset;
      }
      if (this.TimeMode == "LIVE") {
        Offset = Offset - (this.LiveStartMediaTime - parseInt(this.StartMediaTime));
      }
      if (this.TimeMode == "PVR") {
        if (this.isTimeshifted) {
          Offset = (Offset - (this.PVRStartMediaTime - parseInt(this.StartMediaTime)));
          Offset = (this.LiveCurrMediaOffset - Offset);
        }
        else {
          Offset = (this.PVRStartMediaTime - parseInt(this.StartMediaTime));
        }
      }
      if (this.TimeMode == "PERSISTENT_PVR") {
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
      }
      if (this.TimeMode == "PLAYBACK" && Offset > this.TimeLength) {
        Offset = this.TimeLength;
      }
      MediaOffsetStr = sec2HMS(Offset);
    }

    if (this.lblTimeCode != null) {
      if (this.TimeMode != "NONE") {
        var actualBlk2Width = this.SessionSliderToolTipTime.getBlkToolTipWidth();
        var blkToolTipPosition = Math.round((actualBlk2Width / this.TimeLength) * Offset);
        if (this.TimeMode != "PLAYBACK") {
          blkToolTipPosition = actualBlk2Width - blkToolTipPosition;
        }


        blkToolTipPosition = parseInt(blkToolTipPosition + 4);
        this.SessionSliderToolTipTime.updatePositionToolTip(blkToolTipPosition);
      }

      this.SessionSliderToolTipTime.updateToolTip(MediaOffsetStr);
      this.SessionSliderToolTipTime.cmpShow();
      //this._LOG.error("\nSlider:_showSliderToolTipTimeOnDrag:  blkToolTipPosition: " + blkToolTipPosition + " MediaOffsetStr: " + MediaOffsetStr + "\n");
    }

  }



});