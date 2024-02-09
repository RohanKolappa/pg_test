/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var EditMediaPanel = Class.extend({
  //---------Start-------------
  init: function (options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");

    // Init state
    this.objSegmentsInfo = {
      FileNID: "",
      currStartPoint: null,
      currEndPoint: null,
      segmentID: null,
      selectedSegmentID: null
    };

    this.totalLength = 0;

    this.objSegmentList = {};
    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$sessionCtl = this.$elem.closest('.SessionController').parent();
    // Build the DOM's initial structure
    this._build();
  },

  _defaults: {
  },

  _build: function () {
    this.log(bDEBUG, "_build", "call","");
    // Add edit controls
    var html = '<div class="cutlistLength" id="' + this.options.divId + '_totalLength"></div>';
    html += '<a href="#" id="' + this.options.divId + '_setTrimStart" class="setTrimStartBtn-enabled"></a>';
    html += '<a href="#" id="' + this.options.divId + '_setTrimEnd" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_deleteTrimSegment" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_undoTrimPoints" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_cancelTrimProcess" class="cancelTrimBtn-enabled"></a>';
    html += '<a href="#" id="' + this.options.divId + '_saveTrimAsNew" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_saveTrim" class="hidden"></a>';

    this.$elem.append(html);

    var childSaveAs = jQuery('<div id="' + this.options.divId + '_newMediaTitle' +
      '" class="saveAsNewContainer hidden">' +
      '<input id="' + this.options.divId + '_mediaTitle" type="text" size="20" placeholder="Title" class="saveAsNewText">' +
      '<a id="' + this.options.divId + '_cancelSaveAs" class="backTrimBtn-enabled"></a>' +
      '<a id="' + this.options.divId + '_saveAs" class="saveTrimContinueBtn-enabled"></a>' +      
      '</div>').addClass('EditMediaAsNewFilePanel');
    this.$elem.append(childSaveAs);

    var childSave = jQuery('<div id="' + this.options.divId + '_newMedia' +
      '" class="saveAsNewContainer hidden">' +
      '<label id="' + this.options.divId + '_mediaMsg" class="saveAsNewText">You will be overwriting the original media file..</label>' +
      '<a id="' + this.options.divId + '_cancelSave" class="backTrimBtn-enabled"></a>' +
      '<a id="' + this.options.divId + '_save" class="saveTrimContinueBtn-enabled"></a>' +      
      '</div>').addClass('EditMediaNewFilePanel');
    this.$elem.append(childSave);

    // Stash refs to buttons and Hook event handlers to buttons
    this.ctrls = {};
    // Set Start Point
    this.ctrls.$setTrimStart = this.$elem.find('#' + this.options.divId + '_setTrimStart');
    this.ctrls.$setTrimStart.on('click', jQuery.proxy(this._onClick_setTrimStart, this));

    // Set End Point
    this.ctrls.$setTrimEnd = this.$elem.find('#' + this.options.divId + '_setTrimEnd');
    this.ctrls.$setTrimEnd.on('click', jQuery.proxy(this._onClick_setTrimEnd, this));

    // Delete Segment
    this.ctrls.$deleteTrimSegment = this.$elem.find('#' + this.options.divId + '_deleteTrimSegment');
    this.ctrls.$deleteTrimSegment.on('click', jQuery.proxy(this._onClick_deleteTrimSegment, this));

    // Undo setpoints
    this.ctrls.$undoTrimPoints = this.$elem.find('#' + this.options.divId + '_undoTrimPoints');
    this.ctrls.$undoTrimPoints.on('click', jQuery.proxy(this._onClick_undoTrimPoints, this));

    // Cancel Trim process
    this.ctrls.$cancelTrimProcess = this.$elem.find('#' + this.options.divId + '_cancelTrimProcess');
    this.ctrls.$cancelTrimProcess.on('click', jQuery.proxy(this._onClick_cancelTrimProcess, this));

    //Trim as new
    this.ctrls.$saveTrimAsNew = this.$elem.find('#' + this.options.divId + '_saveTrimAsNew');
    this.ctrls.$saveTrimAsNew.on('click', jQuery.proxy(this._onClick_saveTrim, this));

    //Trim
    this.ctrls.$saveTrim = this.$elem.find('#' + this.options.divId + '_saveTrim');
    this.ctrls.$saveTrim.on('click', jQuery.proxy(this._onClick_saveTrim, this));

    this.ctrls.$txtFileTitle = childSaveAs.find('#' + this.options.divId + '_mediaTitle');
    childSaveAs.find('#' + this.options.divId + '_saveAs').click(jQuery.proxy(this._onClick_SaveAs, this));
    childSaveAs.find('#' + this.options.divId + '_cancelSaveAs').click(jQuery.proxy(this._onClick_CancelSave, this));

    childSave.find('#' + this.options.divId + '_save').click(jQuery.proxy(this._onClick_Save, this));
    childSave.find('#' + this.options.divId + '_cancelSave').click(jQuery.proxy(this._onClick_CancelSave, this));

    this.log(bINFO, "_build", "return", "Done");
  },

  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
  },

  _onClick_setTrimStart: function (e) {
    this.log(bDEBUG, "_onClick_setTrimStart", "call", "");
    //Get the current offset from slider
    this.currStartPoint = this.$sessionCtl.sessionController('getSliderRef').sessionSlider('getCurrentOffset');
    //Create id for segment
    this.segmentID = 'seg_' + Math.floor((Math.random() * 100000) + 1);
    //Add & display start pointer on slider
    this.$sessionCtl.sessionController('getSliderRef').sessionSlider('addSegmentStart', { 'id': this.segmentID, 'Start': this.currStartPoint});
    this.ctrls.$setTrimStart.removeClass('setTrimStartBtn-enabled').addClass('hidden');
    this.ctrls.$setTrimEnd.removeClass('hidden').addClass('setTrimEndBtn-enabled');
    this.ctrls.$undoTrimPoints.removeClass('hidden').addClass('undoTrimBtn-enabled');
    e.preventDefault();
  },

  _onClick_setTrimEnd: function (e) {
    this.log(bDEBUG, "_onClick_setTrimEnd", "call", "");
    //Get the current offset from slider
    this.currEndPoint = this.$sessionCtl.sessionController('getSliderRef').sessionSlider('getCurrentOffset');
    //Validate start and end points
    if (this.currEndPoint <= this.currStartPoint) {
      alert("Select Valid StartPoint/EndPoints \nEndPoint should be greater than StartPoint");
      this.currEndPoint = null;
      return false;
    }
    //Check for overlapping
    if (this._chkOverLapping()) {
      alert("Overlapping of segments is not allowed");
      this.currEndPoint = null;
      return false;
    }
    //Add segment info to list
    this.objSegmentList[this.segmentID] = { "Start": this.currStartPoint, "End": this.currEndPoint };
    //Add & display segment on slider
    this.$sessionCtl.sessionController('getSliderRef').sessionSlider('addSegment', { 'id': this.segmentID, 'Start': this.currStartPoint, 'End': this.currEndPoint });
    this._updateTotalLength("ADD", (this.currEndPoint - this.currStartPoint));
    this.currStartPoint = null;
    this.currEndPoint = null;
    this.segmentID = null;
    this.ctrls.$setTrimEnd.removeClass('setTrimEndBtn-enabled').addClass('hidden');
    this.ctrls.$setTrimStart.removeClass('hidden').addClass('setTrimStartBtn-enabled');
    this.ctrls.$undoTrimPoints.removeClass('undoTrimBtn-enabled').addClass('hidden');
    this.ctrls.$saveTrim.removeClass('hidden').addClass('saveTrimBtn-enabled');
    this.ctrls.$saveTrimAsNew.removeClass('hidden').addClass('saveTrimNewFileBtn-enabled');
    e.preventDefault();
  },

  _updateTotalLength: function (mode, segmentLength) {
    if (mode == "ADD") {
      this.totalLength = this.totalLength + parseInt(segmentLength);
    }
    else if (mode == "DELETE") {
      this.totalLength = this.totalLength - parseInt(segmentLength);
    }
    if (this.totalLength <= 0) {
      this.$elem.find('#' + this.options.divId + '_totalLength').text("").addClass('hidden');
    }
    else {
      this.$elem.find('#' + this.options.divId + '_totalLength').removeClass('hidden').text("Length: " + BarcoUtil.msToHMS(this.totalLength));
    }
  },

  _onClick_deleteTrimSegment: function (e) {
    this.log(bDEBUG, "_onClick_deleteTrimSegment", "call", "");
    //Check if segment selected then remove from slider and from list
    if (this.selectedSegmentID) {
      this.$sessionCtl.sessionController('getSliderRef').sessionSlider('deleteSegment', this.selectedSegmentID);
      var segmentLength = this.objSegmentList[this.selectedSegmentID].End - this.objSegmentList[this.selectedSegmentID].Start;
      this._updateTotalLength("DELETE", segmentLength);
      delete this.objSegmentList[this.selectedSegmentID];
      this.selectedSegmentID = null;
    }
    //Check if no segments then hide Save and SaveAs buttons
    if (jQuery.isEmptyObject(this.objSegmentList)) {
      this.ctrls.$saveTrim.removeClass('saveTrimBtn-enabled').addClass('hidden');
      this.ctrls.$saveTrimAsNew.removeClass('saveTrimNewFileBtn-enabled').addClass('hidden');
    }

    this.ctrls.$deleteTrimSegment.removeClass('deleteTrimBtn-enabled').addClass('hidden');
    this.ctrls.$setTrimStart.removeClass('hidden').addClass('setTrimStartBtn-enabled');    
    this.ctrls.$undoTrimPoints.removeClass('undoTrimBtn-enabled').addClass('hidden');
    e.preventDefault();
  },

  _onClick_undoTrimPoints: function (e) {
    this.log(bDEBUG, "_onClick_undoTrimPoints", "call", "");
    //Check add segment process is inprogress then cancel the same
    //Otherwise check if delete segment process is inprogress then cancle the same
    if (this.segmentID) {
      this.$sessionCtl.sessionController('getSliderRef').sessionSlider('deleteSegment', this.segmentID);
    }
    else if (this.selectedSegmentID) {
      this.$sessionCtl.sessionController('getSliderRef').sessionSlider('cancelDeleteSegment', this.selectedSegmentID);
      this.selectedSegmentID = null;
    }
    this.currStartPoint = null;
    this.currEndPoint = null;
    this.segmentID = null;
    this.ctrls.$setTrimStart.removeClass('hidden').addClass('setTrimStartBtn-enabled');
    this.ctrls.$setTrimEnd.removeClass('setTrimEndBtn-enabled').addClass('hidden');
    this.ctrls.$deleteTrimSegment.removeClass('deleteTrimBtn-enabled').addClass('hidden');
    this.ctrls.$undoTrimPoints.removeClass('undoTrimBtn-enabled').addClass('hidden');
    e.preventDefault();
  },

  _onClick_cancelTrimProcess: function (e) {
    //Cancel and reset every thing and close the panel;
    this.log(bDEBUG, "_onClick_cancelTrimProcess", "call", "");
    this.resetEditMedia();
    e.preventDefault();
  },

  _onClick_saveTrim: function (e) {
    this.log(bDEBUG, "_onClick_saveTrim", "call", "");
    //Check if StartPoint is set but EndPoint is not set
    if (this.currStartPoint != null) {
      alert("Set the End point before proceed");
      return;
    }
    //Check if segment is selected for delete then undo the same and proceed.
    if (this.selectedSegmentID) {
      this.$sessionCtl.sessionController('getSliderRef').sessionSlider('cancelDeleteSegment', this.selectedSegmentID);
      this.selectedSegmentID = null;
    }
    this.$elem.find('#' + this.options.divId + '_totalLength').text("").addClass('hidden');
    this.ctrls.$setTrimStart.removeClass('setTrimStartBtn-enabled').addClass('hidden');
    this.ctrls.$setTrimEnd.removeClass('setTrimEndBtn-enabled').addClass('hidden');
    this.ctrls.$deleteTrimSegment.removeClass('deleteTrimBtn-enabled').addClass('hidden');
    this.ctrls.$undoTrimPoints.removeClass('undoTrimBtn-enabled').addClass('hidden');
    this.ctrls.$saveTrim.removeClass('saveTrimBtn-enabled').addClass('hidden');
    this.ctrls.$saveTrimAsNew.removeClass('saveTrimNewFileBtn-enabled').addClass('hidden');
    if (e.target.id == this.ctrls.$saveTrim.attr('id')) {
      this.$elem.find('.EditMediaNewFilePanel').removeClass('hidden');
    }
    else {
      this.$elem.find('.EditMediaAsNewFilePanel').removeClass('hidden');
      this.ctrls.$txtFileTitle.val('');
    }
    e.preventDefault();
  },

  _onClick_SaveAs: function (e) {
    this.log(bDEBUG, "_onClick_Create", "call", "");
    var newMediaTitle = jQuery.trim(this.ctrls.$txtFileTitle.val());
    if (jQuery.isBlank(newMediaTitle)) {
      alert("Enter title for new media");
      this.ctrls.$txtFileTitle.focus();
    }
    else {
      var objTrimAsNew = {
        "CreateMediaFromCutList": {
          "@byNID": this.fileNID,
          "@cutList": this._getCutList(),
          "@title": newMediaTitle,
          "Description": newMediaTitle
        }
      }
      var url = this.$sessionCtl.sessionController('getSessionPostBaseURL').split('Session')[0] + "Resource/File/CreateMediaFromCutList";
      Request.doPOSTCustomHeader(url, this.options.authHeaderObj, objTrimAsNew, jQuery.proxy(function (response, status) {
        if (status == "success") {
          this.log(bDEBUG, "CreateMediaFromCutList", "", "CreateMediaFromCutList Success fileNID:" + this.fileNID);
          this.ctrls.$txtFileTitle.val('');
        }
        else {
          this.log(bERROR, "CreateMediaFromCutList", "", "CreateMediaFromCutList Failed for fileNID:" + this.fileNID + " " + response.responseText.toString());
          alert("CreateMediaFromCutList failed");
        }
        this.resetEditMedia();
      }, this));
    }
    e.preventDefault();
  },

  _onClick_Save: function (e) {
    this.log(bDEBUG, "_onClick_Create", "call", "");
    var objTrim = {
      "TrimFile": {
        "@byNID": this.fileNID,
        "@cutList": this._getCutList()
      }
    }
    var url = this.$sessionCtl.sessionController('getSessionPostBaseURL').split('Session')[0] + "Resource/File/Trim";
    Request.doPOSTCustomHeader(url, this.options.authHeaderObj, objTrim, jQuery.proxy(function (response, status) {
      if (status == "success") {
        this.$sessionCtl.sessionController('doStop');
      }
      else {
        this.log(bERROR, "TrimFile", "", "Trim File Failed for fileNID:" + this.fileNID + " " + response.responseText.toString());
        alert("Trim file failed");
      }
      this.resetEditMedia();
    }, this));
    e.preventDefault();
  },

  _onClick_CancelSave: function (e) {
    this.log(bDEBUG, "_onClick_CancelSaveAs", "call", "");
    this.$elem.find('.EditMediaAsNewFilePanel').addClass('hidden');
    this.$elem.find('.EditMediaNewFilePanel').addClass('hidden');
    this.ctrls.$setTrimStart.removeClass('hidden').addClass('setTrimStartBtn-enabled');
    this.ctrls.$saveTrim.removeClass('hidden').addClass('saveTrimBtn-enabled');
    this.ctrls.$saveTrimAsNew.removeClass('hidden').addClass('saveTrimNewFileBtn-enabled');
    this._updateTotalLength("UPDATE", 0);
    e.preventDefault();
  },

  onSegmentSelected: function (segmentID) {
    //Check if Add segment is in progress
    //Check Save or SaveAs is in progress then do nothing
    if (this.currStartPoint == null && this.currEndPoint == null &&
        this.$elem.find('.EditMediaNewFilePanel').hasClass('hidden') && this.$elem.find('.EditMediaAsNewFilePanel').hasClass('hidden')) {
      if (this.selectedSegmentID != null) {
        this.$sessionCtl.sessionController('getSliderRef').sessionSlider('cancelDeleteSegment', this.selectedSegmentID);
      }
      this.selectedSegmentID = segmentID;
      this.ctrls.$setTrimStart.removeClass('setTrimStartBtn-enabled').addClass('hidden');
      this.ctrls.$deleteTrimSegment.removeClass('hidden').addClass('deleteTrimBtn-enabled');
      this.ctrls.$undoTrimPoints.removeClass('hidden').addClass('undoTrimBtn-enabled');
      return true;
    }
    return false;
  },

  initEditMedia: function (fileNID) {
    if (this.$elem.hasClass('hidden')) {
      this.fileNID = fileNID;
      this.$elem.parent().css('height', 100);
      this._reset();
      this.$elem.removeClass('hidden').addClass("trimMediaContainer");
    }
  },

  _reset: function () {
    this.currStartPoint = null;
    this.currEndPoint = null;
    this.segmentID = null;
    this.selectedSegmentID = null;
    this.objSegmentList = {};
    this.totalLength = 0;
    this.$elem.find('#' + this.options.divId + '_totalLength').text("").addClass('hidden');
    this.ctrls.$setTrimStart.removeClass('hidden').addClass('setTrimStartBtn-enabled');
    this.ctrls.$setTrimEnd.removeClass('setTrimEndBtn-enabled').addClass('hidden');
    this.ctrls.$deleteTrimSegment.removeClass('deleteTrimBtn-enabled').addClass('hidden');
    this.ctrls.$undoTrimPoints.removeClass('undoTrimBtn-enabled').addClass('hidden');
    this.ctrls.$saveTrim.removeClass('saveTrimBtn-enabled').addClass('hidden');
    this.ctrls.$saveTrimAsNew.removeClass('saveTrimNewFileBtn-enabled').addClass('hidden');
    this.ctrls.$txtFileTitle.val("");
    this.$elem.find('.EditMediaAsNewFilePanel').addClass('hidden');
    this.$elem.find('.EditMediaNewFilePanel').addClass('hidden');
  },

  resetEditMedia: function () {
    this.$sessionCtl.sessionController('getSliderRef').sessionSlider('deleteAllSegments');
    this._reset();
    this.fileNID = "";
    this.$elem.parent().css('height', this.options.controlBarHeight);
    this.$elem.removeClass('trimMediaContainer').addClass('hidden');  
  },
  
  applyCollaborationInfo: function (objCollabInfo) {
    this.log(bDEBUG, "applyCollaborationInfo", "call", "");
    if (objCollabInfo.action == 'Stop' && objCollabInfo.state != 'STREAMING') {
      this.resetEditMedia();
    }
  },

  _chkOverLapping: function () {
    var isOverLapped = false;
    jQuery.each(this.objSegmentList, jQuery.proxy(function (key, value) {
      var startPoint = value.Start;
      var endPoint = value.End;
      if ((startPoint >= this.currStartPoint && startPoint <= this.currEndPoint) ||
          (endPoint >= this.currStartPoint && endPoint <= this.currEndPoint) ||
          (this.currStartPoint >= startPoint && this.currStartPoint <= endPoint) ||
          (this.currEndPoint >= startPoint && this.currEndPoint <= endPoint)) {
        isOverLapped = true;
        return false;
      }
    }, this));
    return isOverLapped;
  },

  _getCutList: function () {
    var strCutlist = "";
    var arrCutlist = [];
    jQuery.each(this.objSegmentList, function (key, value) {
      arrCutlist.push(value);
    });
    //Sort the array as MW expecting cutlist in ascending order
    var arrCutlist = arrCutlist.sort(function (a, b) {
      return a.Start - b.Start;
    });

    jQuery.each(arrCutlist, function (key, value) {
      if (strCutlist == "") {
        strCutlist = value.Start + ":" + value.End;
      }
      else {
        strCutlist = strCutlist + "," + value.Start + ":" + value.End;
      }
    });
    return strCutlist;
  }

  //------End------------------

});
jQuery.plugin('editMediaPanel', EditMediaPanel);
