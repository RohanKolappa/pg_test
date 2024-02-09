/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* PvrRecordSlider class.
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.PvrRecordSlider", jsx3.gui.Block, null, function (PvrRecordSlider, PvrRecordSlider_prototype) {

  /** @private @jsxobf-clobber */
  PvrRecordSlider_prototype._LOG = jsx3.util.Logger.getLogger(PvrRecordSlider.jsxclass.getName());

  PvrRecordSlider_prototype.init = function () {
    this.jsxsuper();
    this._onAppInit();
  }

  PvrRecordSlider_prototype._onAppInit = function () {
    this.sliderPointer = this.getDescendantOfName("sliderPointer");
    this.blkProgress = this.getDescendantOfName("sliderTrackProgress");
    this.blkPvrSelectTrack = this.getDescendantOfName("sliderPvrSelectTrack");
    this.lblTimeCode = this.getDescendantOfName("lblTimeCode");
    this.lblTimeLength = this.getDescendantOfName("lblTimeLength");
    this.blkSliderContainer = this.getDescendantOfName("blkSliderContainer");
    this.blkInPointMarker = this.getDescendantOfName("blkInPointMarker");
    this.blkOutPointMarker = this.getDescendantOfName("blkOutPointMarker");

    this.txtHiddenStartOffset = this.getDescendantOfName("txtHiddenStartOffset");
    this.txtHiddenEndOffset = this.getDescendantOfName("txtHiddenEndOffset");
    this.txtAvailablePvr = this.getDescendantOfName("txtAvailablePvr");
    this.txtSelectedDuration = this.getDescendantOfName("txtSelectedDuration");

    this.lblTimeTypeInfoTip = this.getDescendantOfName("lblTimeTypeInfoTip");
    this.btnPvrTimeType = this.getDescendantOfName("btnPvrTimeType");

    this.blkSliderTimeThumbNail = this.getDescendantOfName("blkSliderTimeThumbNail");
    this.lblSliderTimeThumbNail = this.getDescendantOfName("lblSliderTimeThumbNail");

    this.sliderCoverForOnMouseOver = this.getDescendantOfName("sliderCoverForOnMouseOver");
    //    this.lblTimeLength.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    //    this.lblTimeLength.setEvent("this.onClick();", jsx3.gui.Interactive.JSXCLICK);
    //    this.lblTimeLength.onClick = jsx3.$F(this._onClick_CatchUp).bind(this);

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
    this.pvrPlayLength = null;
    this.userSelectedPVR = false;
    this.blkProgressStartLeft = 0;
    this.blkProgressEndLeft = 0;
    this.deltaStartLeft = 0;
    this.deltaEndLeft = 0;
    this.deltaSet = true;

    this.SliderLeftPosition = 0;
    this.SliderRightPosition = 484;

    this.PvrTimeType = "PLAYLENGTH";
    this.timeOutInfoId = null; 
    this.lblTimeOutInfoId = null;

    this.whichMarkerMoved = null;
    this.inPointMarkerOffset = null;
    this.outPointMarkerOffset = null;
    this.pvrStartOffset = null;
    this.pvrEndOffset = null;
    this.mouseOverXPosition = 0;
    this.mouseOverOffset = null;
    this.sliderWidth = null;

    // for the this.blkInPointMarker moving
    this.blkInPointMarker.removeEvent(jsx3.gui.Interactive.BEFORE_MOVE);
    this.blkInPointMarker.setEvent("this.onBlkBeforeMove(this, objEVENT);", jsx3.gui.Interactive.BEFORE_MOVE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.blkInPointMarker.onBlkBeforeMove = jsx3.$F(this.onBlkBeforeMove).bind(this);

    this.blkInPointMarker.removeEvent(jsx3.gui.Interactive.AFTER_MOVE);
    this.blkInPointMarker.setEvent("this.onBlkAfterMove(this, objEVENT);", jsx3.gui.Interactive.AFTER_MOVE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.blkInPointMarker.onBlkAfterMove = jsx3.$F(this.onBlkAfterMove).bind(this);

    // for the this.blkInPointMarker moving
    this.blkOutPointMarker.removeEvent(jsx3.gui.Interactive.BEFORE_MOVE);
    this.blkOutPointMarker.setEvent("this.onBlkBeforeMove(this, objEVENT);", jsx3.gui.Interactive.BEFORE_MOVE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.blkOutPointMarker.onBlkBeforeMove = jsx3.$F(this.onBlkBeforeMove).bind(this);

    this.blkOutPointMarker.removeEvent(jsx3.gui.Interactive.AFTER_MOVE);
    this.blkOutPointMarker.setEvent("this.onBlkAfterMove(this, objEVENT);", jsx3.gui.Interactive.AFTER_MOVE);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.blkOutPointMarker.onBlkAfterMove = jsx3.$F(this.onBlkAfterMove).bind(this);


    this.btnPvrTimeType.removeEvent(jsx3.gui.Interactive.TOGGLE);
    this.btnPvrTimeType.setEvent("this.onClick(intSTATE);", jsx3.gui.Interactive.TOGGLE);
    this.btnPvrTimeType.onClick = jsx3.$F(this._getPvrTimeTypeButtonState).bind(this);


    this.lblTimeCode.setAttribute("onmouseover", "com.ipvs.gui.PvrRecordSlider.lblTimeCodeOnMouseOver(this)");
    //this.sliderCoverForOnMouseOver.setAttribute("onmouseover", "com.ipvs.gui.PvrRecordSlider.trackOnMouseOver(this)");
    this.blkInPointMarker.setAttribute("onmouseover", "com.ipvs.gui.PvrRecordSlider.trackOnMouseOver(this)");
    this.blkOutPointMarker.setAttribute("onmouseover", "com.ipvs.gui.PvrRecordSlider.trackOnMouseOver(this)");

    this.sliderCoverForOnMouseOver.removeEvent(jsx3.gui.Interactive.SPYGLASS);
    this.sliderCoverForOnMouseOver.setEvent("this.onspyGlass(this, objEVENT);", jsx3.gui.Interactive.SPYGLASS);
    // the above event will fire in the context of the blk - so we setup its onSelect to point to ours
    this.sliderCoverForOnMouseOver.onspyGlass = jsx3.$F(this.onspyGlass).bind(this);

    //this.SessionSliderToolTipTime = this.clsApp.getDescendantsOfType("com.ipvs.gui.SessionSliderToolTipTime")[0];
    // init the offset and position
    this.initMediaOffset();
    this.updateView();
  }

  // extend show
  PvrRecordSlider_prototype.onBlkBeforeMove = function (obj, objEvent) {
    var eventType = objEvent.getType();
    if (eventType == "mousedown") {
    }
    else if (eventType == "mouseup") {
    }

    var blkName = obj.jsxname;

    this._hideBlkSliderTimeThumbNail();
    clearTimeout(this.timeOutInfoId);
    this.timeOutInfoId = null;
    //stop the update
    this.incrementStop();
    return { 'bCONSTRAINY': true };
  }

  PvrRecordSlider_prototype.onBlkAfterMove = function (obj, objEvent) {
    //return { 'bCONSTRAINY': true };
    this.userSelectedPVR = true;
    this.deltaStartLeft = 0;
    this.deltaEndLeft = 0;

    var blkName = obj.jsxname;
    //var xPosition = objEvent.clientX();
    var xPosition = 0;
    if (blkName == "blkInPointMarker") {
      xPosition = this.getPointMarkerxPosition(this.blkInPointMarker);
    }
    else if (blkName == "blkOutPointMarker") {
      xPosition = this.getPointMarkerxPosition(this.blkOutPointMarker);
    }
    this.adjustBlkPointMarker(blkName, xPosition);

    //check if after moving the In and OUt markers are at the end point like when dialog open
    var inPointMark = this.getPointMarkerxPosition(this.blkInPointMarker);
    var inPositionOnSlider = Math.round((inPointMark * 100) / this.sliderWidth);

    var outpointMark = this.getPointMarkerxPosition(this.blkOutPointMarker);
    var outPositionOnSlider = Math.round((parseInt(outpointMark - 16, 10) * 100) / this.sliderWidth);
    if ((inPointMark == this.SliderLeftPosition && outpointMark == this.SliderRightPosition) ||
         (this.blkProgressStartLeft == inPositionOnSlider && this.blkProgressEndLeft == outPositionOnSlider)) {
      this.userSelectedPVR = false;
    }

    //start the update timer
    this.incrementStart();
  }

  PvrRecordSlider_prototype.adjustBlkPointMarker = function (blkName, xposition) {
    if (blkName == "blkInPointMarker") {
      this.whichMarkerMoved = "IN";
      var progressPosition = Math.round((this.blkProgressStartLeft * this.sliderWidth) / 100);
      var xPositionOnSlider = Math.round((xposition * 100) / this.sliderWidth);

      var outPointMarkerXPosition = this.getPointMarkerxPosition(this.blkOutPointMarker);
      outPointMarkerXPosition = parseInt(outPointMarkerXPosition - 16, 10);
      if (xposition <= this.SliderLeftPosition || xposition > this.sliderWidth ||
          xposition > outPointMarkerXPosition || xPositionOnSlider < this.blkProgressStartLeft) {
        xposition = progressPosition;
        this.setPointMarkerxPosition(this.blkInPointMarker, xposition);
      }
    }
    else if (blkName == "blkOutPointMarker") {
      this.whichMarkerMoved = "OUT";
      var inPointMarkerXPosition = this.getPointMarkerxPosition(this.blkInPointMarker);
      inPointMarkerXPosition = parseInt(inPointMarkerXPosition + 16, 10);
      if (xposition <= this.SliderLeftPosition || xposition > this.SliderRightPosition ||
          xposition < inPointMarkerXPosition) {
        xposition = this.SliderRightPosition;
        this.setPointMarkerxPosition(this.blkOutPointMarker, xposition);
      }
    }

  }

  PvrRecordSlider_prototype.setPointMarkerxPosition = function (blkPointMarkerJSX, setleftpx) {
    blkPointMarkerJSX.setLeft(setleftpx);
    blkPointMarkerJSX.repaint();
  }

  PvrRecordSlider_prototype.getPointMarkerxPosition = function (blkPointMarkerJSX) {
    return blkPointMarkerJSX.getLeft();
  }

  PvrRecordSlider_prototype.getMouseOverxPosition = function () {
    return this.mouseOverXPosition;
  }



  PvrRecordSlider_prototype.getBlkSliderWidthPx = function () {
    var blkSliderWidthPx = this.blkSliderContainer.getRendered();
    // subtract width of the inblkmark(16) and outblkmark(16)
    var blkJsxWidth = blkSliderWidthPx.clientWidth - 16 - 16;
    return blkJsxWidth;
  }

  // extend show
  PvrRecordSlider_prototype.show = function (EventVar, EventVal) {
    if (this.TimeMode != "NONE" && this.sliderPointer.setEnabled != null) {
      this.sliderPointer.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
    this.sliderPointer.setHandleImage("images/slider/handleEnabled.gif");
    //this.sliderPointer.repaint();
    this.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.repaint();
  }

  // override hide
  PvrRecordSlider_prototype.hide = function () {
    if (this.sliderPointer.setEnabled != null) {
      this.sliderPointer.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
    this.sliderPointer.setHandleImage("images/slider/handleDisabled.gif");
    //this.sliderPointer.repaint();
    this.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.repaint();
  }


  //check mediaOffset before set it, to ensure this.StartMediaTime not null for join mode
  PvrRecordSlider_prototype.checkAndSetMediaOffset = function (objCollabInfo) {
    if (this.timerHandle == null) {
      this.startMediaOffset(objCollabInfo);
    }
    this.setMediaOffset(objCollabInfo);
  }

  PvrRecordSlider_prototype.applyCollaborationInfo = function (objCollabInfo) {
    this.isTimeshifted = objCollabInfo.isTimeshifted;
    this.pvrPlayLength = objCollabInfo.playlength;
    // process action
    switch (objCollabInfo.action) {
      case "Start":
        this.startMediaOffset(objCollabInfo);
        break;
      case "Stop":
        this.initMediaOffset();
        break;
      default:
        // just log an error
        this._LOG.error("PvrRecordSlider::Unknown Action:" + objCollabInfo.action);
        //        this.updateMediaOffset();
        break;
    }
    this.updateView();

  }
  PvrRecordSlider_prototype.initMediaOffset = function () {
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

    this.SliderStartOffset = 0;
    this.CurrMediaOffset = 0;
    if (this.lblTimeLength != null) {
      this.lblTimeLength.setText("", true);
    }

    this.txtHiddenStartOffset.setValue("", true);
    this.txtHiddenEndOffset.setValue("", true);
    this.txtAvailablePvr.setValue("", true);
    this.txtSelectedDuration.setValue("", true);

    this.sliderWidth = this.getBlkSliderWidthPx();
    this.hide();
  }

  PvrRecordSlider_prototype.startMediaOffset = function (objCollabInfo) {
    // default time mode = live
    this.TimeMode = objCollabInfo.timeMode;
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

    var CurrDateTime = getCurrServerTime();
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

    if (this.TimeMode == "PERSISTENT_PVR" || this.TimeMode == "PVR") {
      this.PVRStartMediaTime = objCollabInfo.pvrInfo.startTC;
      //this._LOG.warn("Slider: startMediaOffset 5 :: this.PVRStartMediaTime: " + this.PVRStartMediaTime + " ::(CurrDateTime - this.TimeLength):" + CurrDateTime - this.TimeLength);
      if (this.PVRStartMediaTime < this.StartMediaTime - this.TimeLength) {
        this.PVRStartMediaTime = this.StartMediaTime - this.TimeLength;
      }
      //this._LOG.warn("Slider: startMediaOffset 6 :: this.PVRStartMediaTime: " + this.PVRStartMediaTime);
    }

    this.lblTimeCode.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.lblTimeCode.setText(convertMMtoHrMin(this.TimeLength));
    this.lblTimeCode.repaint();

    this.btnPvrTimeType.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.btnPvrTimeType.repaint();

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

  PvrRecordSlider_prototype.setMediaOffset = function (objCollabInfo) {
    // get curr media offset from the offset that came with the action event
    // set timescale
    this.TimeScale = objCollabInfo.timeInfo.timeScale;
    this.LastMediaOffset = objCollabInfo.timeInfo.lastOffset;
    this.LastMediaTime = objCollabInfo.timeInfo.lastWC;
    this._handleLblTimeLength(objCollabInfo);
  }

  PvrRecordSlider_prototype.updateMediaOffset = function () {
    if (this.StartMediaTime != null) {
      var CurrDateTime = getCurrServerTime();
      // calc media offset where live is
      this.LiveCurrMediaOffset = CurrDateTime - this.StartMediaTime;
      // calc curr media offset (in milliseconds) based on milliseconds since media time and timescale (ie play mode)    
      this.CurrMediaOffset = Math.round(parseInt(this.LastMediaOffset) + (parseInt(CurrDateTime) - parseInt(this.LastMediaTime)) * this.TimeScale);
      //this._LOG.warn("Slider: updateMediaOffset:: 0 ::CurrDateTime: " + CurrDateTime + " \n::this.LiveCurrMediaOffset:  " + this.LiveCurrMediaOffset + "\n::this.CurrMediaOffset:" + this.CurrMediaOffset + "\n::this.StartMediaTime:" + this.StartMediaTime);

      // by default slider starts at offset 0
      this.SliderStartOffset = 0;

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

  PvrRecordSlider_prototype.updateView = function () {
    // update view of all time related things
    this.updateSliderHandle();
    this.updateProgressBar();
    this.updateMediaOffsetString();

    // make sure increment timer is stopped/started as needed
    if (this.StartMediaTime == null) {
      this.incrementStop();
    }
    else {
      this.incrementStart();
    }
  }

  PvrRecordSlider_prototype.updateSliderHandle = function () {
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
      // Slider mark position --
      this.sliderPointer.setValue(Position);
      this.sliderPointer.repaint();
      //this.SessionSliderToolTipTime.updatePositionToolTip(Position + "%");
    } catch (e) {
      this._LOG.warn("Slider: updateSliderHandle: " + e.description);
    }
  }


  PvrRecordSlider_prototype.updateProgressBar = function () {
    var pvrProgressStart = 0;
    var pvrProgressSize = 0;
    var pvrStartOffset = 0;
    var pvrEndOffset = 0;
    var pvrAvailable = 0;
    var pvrProgressEnd = 100;

    var selectProgressStart = 0;
    var selectProgressSize = 0;
    var selectProgressEnd = 0;
    var selectPVR = 0;
    var selectStartOffset = 0;
    var selectEndOffset = 0;

    if (this.StartMediaTime != null) {
      if (this.TimeMode == "PERSISTENT_PVR" || this.TimeMode == "PVR" && this.PVRStartMediaOffset != null) {
        //this._LOG.warn("\n\nSlider: updateProgressBar 1 :: PVRStartMediaOffset: " + this.PVRStartMediaOffset + " ::SliderStartOffset:  " + this.SliderStartOffset + " ::TimeLength:  " + this.TimeLength);

        pvrProgressStart = Math.round((parseInt(this.PVRStartMediaOffset) - parseInt(this.SliderStartOffset)) / (this.TimeLength / 100));
        if (isNaN(pvrProgressStart)) pvrProgressStart = 100;
        if (pvrProgressStart > 100) pvrProgressStart = 100;
        pvrProgressSize = Math.round((parseInt(this.LiveCurrMediaOffset) - parseInt(this.PVRStartMediaOffset)) / (this.TimeLength / 100));
        if (isNaN(pvrProgressSize)) pvrProgressSize = 100;
        if (pvrProgressStart + parseInt(pvrProgressSize) > 100) pvrProgressSize = 100 - pvrProgressStart;
        pvrProgressEnd = 100;
        pvrStartOffset = parseInt(this.PVRStartMediaOffset) + Math.round(pvrProgressStart * (this.TimeLength / 100));
        pvrEndOffset = parseInt(this.PVRStartMediaOffset) + Math.round(pvrProgressEnd * (this.TimeLength / 100));

        pvrAvailable = parseInt(pvrEndOffset - pvrStartOffset, 10);
        //this._LOG.warn("Slider: updateProgressBar 2 :: ProgressStart: " + pvrProgressStart + " ::ProgressSize:  " + pvrProgressSize + " ::pvrStartOffset:  " + pvrStartOffset);

        if (pvrAvailable > this.TimeLength) {
          pvrAvailable = this.TimeLength;
        }
        else if (pvrAvailable <= 0) {
          pvrAvailable = 0;
        }
        //this._LOG.warn("Slider: updateProgressBar 3 :: pvrStartOffset: " + pvrStartOffset + " ::pvrEndOffset:  " + pvrEndOffset + " ::pvrAvailable:  " + pvrAvailable + "\n\n");

        if (this.userSelectedPVR) {
          var tempPvrProgressStart = pvrProgressStart;

          if (this.deltaStartLeft == 0 && this.deltaEndLeft == 0) {
            var inBlkMarkerPx = this.getPointMarkerxPosition(this.blkInPointMarker);
            var outBlkMarkerPx = this.getPointMarkerxPosition(this.blkOutPointMarker);
            outBlkMarkerPx = parseInt(outBlkMarkerPx - 16, 10);
            selectProgressStart = Math.round((inBlkMarkerPx * 100) / this.sliderWidth);
            selectProgressEnd = Math.round((outBlkMarkerPx * 100) / this.sliderWidth);

            this.deltaStartLeft = selectProgressStart - tempPvrProgressStart;
            this.deltaEndLeft = selectProgressEnd - tempPvrProgressStart;
            //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar DELTASTART=0 DELTAEND=0 :: deltaStartLeft: " + this.deltaStartLeft + " ::deltaEndLeft:  " + this.deltaEndLeft);
            //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar DELTASTART=0 DELTAEND=0 :: selectProgressStart: " + selectProgressStart + " ::selectProgressEnd:  " + selectProgressEnd + "\n\n");

            if (tempPvrProgressStart == 0) {
              this.deltaSet = false;
            }
          }
          else {
            this.deltaSet = true;
          }

          //re calculate the delta
          if (tempPvrProgressStart == 0 && this.deltaSet) {
            var CurrDateTime = getCurrServerTime();
            var x1 = CurrDateTime - this.PVRStartMediaTime;
            if (x1 > this.TimeLength) {
              var x2 = x1 - this.TimeLength;
              var x3 = Math.round((100 / this.TimeLength) * x2);
              x3 = parseInt(x3 * -1);
              tempPvrProgressStart = x3;
              //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar tempPvrProgressStart=0:: x1: " + x1 + " ::TimeLength:  " + this.TimeLength + "\n");
              //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar tempPvrProgressStart=0 :: x2: " + x2 + " ::x3:  " + x3 + "\n");
              //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar tempPvrProgressStart=0 :: deltaStartLeft: " + this.deltaStartLeft + " ::deltaEndLeft:  " + this.deltaEndLeft);
              //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar tempPvrProgressStart=0 :: selectProgressStart: " + selectProgressStart + " ::selectProgressEnd:  " + selectProgressEnd + "\n\n");
            }
          }

          selectProgressStart = tempPvrProgressStart + this.deltaStartLeft;
          selectProgressStart = (selectProgressStart <= 0 ? 0 : selectProgressStart >= 100 ? 100 : selectProgressStart);
          selectProgressEnd = tempPvrProgressStart + this.deltaEndLeft;
          selectProgressEnd = (selectProgressEnd <= 0 ? 0 : selectProgressEnd >= 100 ? 100 : selectProgressEnd);

          selectProgressSize = selectProgressEnd - selectProgressStart;
          selectStartOffset = parseInt(this.PVRStartMediaOffset) + Math.round(selectProgressStart * (this.TimeLength / 100));
          selectEndOffset = parseInt(this.PVRStartMediaOffset) + Math.round(selectProgressEnd * (this.TimeLength / 100));
          //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar 4 :: deltaStartLeft: " + this.deltaStartLeft + " ::deltaEndLeft:  " + this.deltaEndLeft + " ::tempPvrProgressStart:  " + tempPvrProgressStart);
          //this._LOG.warn("Slider: updateSELECTPVR_ProgressBar 5 :: selectProgressStart: " + selectProgressStart + " ::selectProgressEnd:  " + selectProgressEnd + " ::selectProgressSize:  " + selectProgressSize + "\n\n");
        }
        else {
          selectProgressStart = pvrProgressStart;
          selectProgressEnd = pvrProgressEnd;
          selectProgressSize = pvrProgressSize;
          selectStartOffset = pvrStartOffset;
          selectEndOffset = pvrEndOffset;
          this.deltaStartLeft = 0;
          this.deltaEndLeft = 0;
        }
      }
    }

    pvrAvailable = convertMMtoHrMinText(pvrAvailable);
    this.txtAvailablePvr.setValue(pvrAvailable, true);

    this.blkProgressStartLeft = pvrProgressStart;
    this.blkProgressEndLeft = pvrProgressEnd;

    // Slider progress block size -- 
    this.blkProgress.setLeft(pvrProgressStart + "%");
    this.blkProgress.setWidth(pvrProgressSize + "%");
    this.blkProgress.repaint();

    //updat ethe selected pvr
    this.blkPvrSelectTrack.setLeft(selectProgressStart + "%");
    this.blkPvrSelectTrack.setWidth(selectProgressSize + "%");
    this.blkPvrSelectTrack.repaint();

    var inMarkLeftpx = Math.round(parseInt(this.sliderWidth, 10) * (selectProgressStart / 100));
    this.blkInPointMarker.setLeft(inMarkLeftpx);
    this.blkInPointMarker.repaint();

    var outMarkLeftpc = Math.round(parseInt(this.sliderWidth, 10) * (selectProgressEnd / 100) + 16);
    this.blkOutPointMarker.setLeft(outMarkLeftpc);
    this.blkOutPointMarker.repaint();

    selectPVR = parseInt(selectEndOffset - selectStartOffset, 10);
    if (selectPVR > this.TimeLength) {
      selectPVR = this.TimeLength;
    }
    else if (selectPVR <= 0) {
      selectPVR = 0;
    }

    var convertSelectPVR = convertMMtoHrMinText(selectPVR);
    this.txtSelectedDuration.setValue(convertSelectPVR, true);

    var reqStartOffset = 0;
    var reqEndOffset = 0;

    reqStartOffset = Math.round(parseInt(selectStartOffset, 10) - parseInt(pvrStartOffset, 10));
    if (reqStartOffset < 0) {
      reqStartOffset = 0;
    }
    this.inPointMarkerOffset = selectStartOffset;

    reqEndOffset = Math.round(parseInt(selectEndOffset, 10) - parseInt(pvrStartOffset, 10));
    if (reqEndOffset == 0) {
      reqEndOffset = selectPVR;
    }
    this.outPointMarkerOffset = selectEndOffset;
    this.pvrStartOffset = pvrStartOffset;
    this.pvrEndOffset = pvrEndOffset;

    this.txtHiddenStartOffset.setValue(reqStartOffset, true);
    this.txtHiddenEndOffset.setValue(reqEndOffset, true);

    if (this.whichMarkerMoved != null) {
      this._getBlkSliderTimeThumbNailData();
      this.whichMarkerMoved = null;
    }
  }

  PvrRecordSlider_prototype.updateMediaOffsetString = function () {
    if (this.LiveCurrMediaOffset == null || this.PVRStartMediaOffset == null) return;
    var CurrDateTime = getCurrServerTime();

    if (this.PvrTimeType == "DATETIME") {
      var xx = getDateTimeHHMMFromTimeCode(CurrDateTime.toString());
      this.lblTimeLength.setText(xx);
      var enddateTime = Math.round(parseInt((CurrDateTime - this.TimeLength)));

      var yy = getDateTimeHHMMFromTimeCode(enddateTime.toString());
      this.lblTimeCode.setText(yy);
    }
    else if (this.PvrTimeType == "PLAYLENGTH") {
      this.lblTimeCode.setText(convertMMtoHrMin(this.TimeLength));
      this.lblTimeLength.setText("LIVE", true);
    }
    this.lblTimeCode.repaint();
    this.lblTimeLength.repaint();

  }


  // setup interval timer to call increment function every length/100 times
  PvrRecordSlider_prototype.incrementStart = function () {
    if (this.timerHandle == null) {
      // slider has 100 discrete positions
      var ThisClass = this;
      this.timerHandle = setInterval(function () { PvrRecordSlider_prototype.increment(ThisClass); }, 1000);
    }
  }
  // stop interval timer
  PvrRecordSlider_prototype.incrementStop = function () {
    clearInterval(this.timerHandle);
    this.timerHandle = null;
  }

  // this function is called in a global context (due to setInterval)
  // the value of "this" in here is not our "this" - so we use the special var ThisClass instead
  PvrRecordSlider_prototype.increment = function (context) {
    context.updateMediaOffset.call(context);
    context.updateView.call(context);
  }

  PvrRecordSlider_prototype._handleLblTimeLength = function (objCollabInfo) {
    //    if (this.lblTimeLength != null) {
    //      this.lblTimeLength.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    //      this.lblTimeLength.setText("LIVE", true);
    //      this.lblTimeLength.setCursor("default", true);
    //      this.lblTimeLength.setClassName("liveOff");
    //      this.lblTimeLength.repaint();
    //    }
  }

  PvrRecordSlider_prototype.getPvrOffset = function (type) {
    var offset = "";
    if (type == "START") {
      offset = this.txtHiddenStartOffset.getValue();
    }
    else if (type == "END") {
      offset = this.txtHiddenEndOffset.getValue();
    }
    return offset;
  }


  PvrRecordSlider_prototype._getPvrTimeTypeButtonState = function (btnState) {
    var hideBlk = false;
    // show the PVR time and date of the day
    if (btnState == "1") {
      this.PvrTimeType = "DATETIME";
      this.lblTimeTypeInfoTip.setText("PVR time of day", true);
      this._showTimeTypeInfoTip();
      hideBlk = true;
    }
    //show the time length of the pvr
    else if (btnState == "0") {
      this.PvrTimeType = "PLAYLENGTH";
      this.lblTimeTypeInfoTip.setText("PVR Length", true);
      this._showTimeTypeInfoTip();
      hideBlk = true;
    }

    if (hideBlk) {
      if (jsx3.util.strEmpty(this.timeOutInfoId)) {
        var ThisClass = this;
        this.timeOutInfoId = setTimeout(function () {
          ThisClass._hideTimeTypeInfoTip();
          ThisClass.timeOutInfoId = null;
        }, 2000);
      }
    }

  }


  PvrRecordSlider_prototype._showTimeTypeInfoTip = function () {
    this.lblTimeTypeInfoTip.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.lblTimeTypeInfoTip.repaint();
  }

  PvrRecordSlider_prototype._hideTimeTypeInfoTip = function () {
    this.lblTimeTypeInfoTip.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.lblTimeTypeInfoTip.repaint();
  }


  PvrRecordSlider.lblTimeCodeOnMouseOver = function (objGUI) {
    var strId = objGUI.getAttribute("id");
    var objJSX = jsx3.GO(strId);
    var ThisClass = objJSX.getAncestorOfType("com.ipvs.gui.PvrRecordSlider");
    if (!jsx3.util.strEmpty(ThisClass.lblTimeOutInfoId)) return;

    ThisClass._showTimeTypeInfoTip();
    ThisClass.lblTimeOutInfoId = setTimeout(function () {
      ThisClass._hideTimeTypeInfoTip();
      ThisClass.lblTimeOutInfoId = null;
    }, 2000);

  }

  PvrRecordSlider_prototype._getBlkSliderTimeThumbNailData = function () {
    var lblValue = "00:00";
    if (this.PvrTimeType == "DATETIME") {
      var CurrDateTime = getCurrServerTime();
      var timetoShow = "00:00";
      if (this.whichMarkerMoved == "IN") {
        //timetoShow = Math.round(parseInt(CurrDateTime + this.PVRStartMediaOffset) + parseInt((this.inPointMarkerOffset - this.pvrStartOffset)));
        timetoShow = Math.round(parseInt(CurrDateTime - parseInt(this.pvrEndOffset - this.inPointMarkerOffset)));
      }
      else if (this.whichMarkerMoved == "OUT") {
        //timetoShow = Math.round(parseInt(CurrDateTime + this.PVRStartMediaOffset) + parseInt((this.outPointMarkerOffset - this.pvrStartOffset)));
        timetoShow = Math.round(parseInt(CurrDateTime - parseInt(this.pvrEndOffset - this.outPointMarkerOffset)));
      }
      else if (this.whichMarkerMoved == "MOUSEOVER") {
        // timetoShow = Math.round(parseInt(CurrDateTime + this.PVRStartMediaOffset) + parseInt((this.mouseOverOffset - this.pvrStartOffset)));
        timetoShow = Math.round(parseInt(CurrDateTime - parseInt(this.pvrEndOffset - this.mouseOverOffset)));
      }
      lblValue = getTimeHHMMFromTimeCode(timetoShow.toString());
    }
    else if (this.PvrTimeType == "PLAYLENGTH") {
      var timetoShow = "00:00";
      if (this.whichMarkerMoved == "IN") {
        timetoShow = parseInt(this.pvrEndOffset - this.inPointMarkerOffset);
      }
      else if (this.whichMarkerMoved == "OUT") {
        timetoShow = parseInt(this.pvrEndOffset - this.outPointMarkerOffset);
      }
      else if (this.whichMarkerMoved == "MOUSEOVER") {
        timetoShow = parseInt(this.pvrEndOffset - this.mouseOverOffset);
      }
      lblValue = convertMMtoHrMin(timetoShow.toString());
    }

    var leftPosition = "0";
    if (this.whichMarkerMoved == "IN") {
      leftPosition = this.blkInPointMarker.getLeft();
    }
    else if (this.whichMarkerMoved == "OUT") {
      leftPosition = parseInt(this.blkOutPointMarker.getLeft() - 16);
    }
    else if (this.whichMarkerMoved == "MOUSEOVER") {
      leftPosition = this.getMouseOverxPosition();
    }

    this._showBlkSliderTimeThumbNail(lblValue, leftPosition);
  }

  PvrRecordSlider_prototype._showBlkSliderTimeThumbNail = function (lblData, lblPosition) {
    this.lblSliderTimeThumbNail.setText(lblData, true);
    this.blkSliderTimeThumbNail.setLeft(lblPosition);

    this.blkSliderTimeThumbNail.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.blkSliderTimeThumbNail.repaint();

    // hide the block
    if (jsx3.util.strEmpty(this.timeOutInfoId)) {
      var ThisClass = this;
      this.timeOutInfoId = setTimeout(function () {
        ThisClass._hideBlkSliderTimeThumbNail();
        //ThisClass.whichMarkerMoved = null;
        ThisClass.timeOutInfoId = null;
      }, 1000);
    }
  }

  PvrRecordSlider_prototype._hideBlkSliderTimeThumbNail = function () {
    this.blkSliderTimeThumbNail.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.blkSliderTimeThumbNail.repaint();
  }



  PvrRecordSlider.trackOnMouseOver = function (objGUI) {
    var strId = objGUI.getAttribute("id");
    var objJSX = jsx3.GO(strId);
    var blkJSXName = objJSX.jsxname;
    var ThisClass = objJSX.getAncestorOfType("com.ipvs.gui.PvrRecordSlider");

    if (blkJSXName == "blkInPointMarker") {
      ThisClass.whichMarkerMoved = "IN";
    }
    else if (blkJSXName == "blkOutPointMarker") {
      ThisClass.whichMarkerMoved = "OUT";
    }

    ThisClass._hideBlkSliderTimeThumbNail();
    clearTimeout(ThisClass.timeOutInfoId);
    ThisClass.timeOutInfoId = null;

    ThisClass._getBlkSliderTimeThumbNailData();

    ThisClass.timeOutInfoId = setTimeout(function () {
      ThisClass._hideBlkSliderTimeThumbNail();
      ThisClass.timeOutInfoId = null;
    }, 1000);

  }


  PvrRecordSlider_prototype.onspyGlass = function (objGUI, objEvent) {
    this.mouseOverXPosition = objEvent.getOffsetX();
    this.mouseOverXPosition = parseInt(this.mouseOverXPosition-16,10);
    this.mouseOverXPosition = this.mouseOverXPosition < 0 ? 0 : this.mouseOverXPosition > this.sliderWidth ? this.sliderWidth : this.mouseOverXPosition;
    var mouserOverOnSlider = Math.round((this.mouseOverXPosition * 100) / this.sliderWidth);
    this.mouseOverOffset = parseInt(this.PVRStartMediaOffset) + Math.round(mouserOverOnSlider * (this.TimeLength / 100));

    this._hideBlkSliderTimeThumbNail();
    clearTimeout(this.timeOutInfoId);
    this.timeOutInfoId = null;

    this.whichMarkerMoved = "MOUSEOVER";
    this._getBlkSliderTimeThumbNailData();

    var ThisClass = this;
    this.timeOutInfoId = setTimeout(function () {
      ThisClass._hideBlkSliderTimeThumbNail();
      ThisClass.timeOutInfoId = null;
    }, 1000);


  }

});
