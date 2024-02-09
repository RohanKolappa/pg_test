/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var ImageViewerButtonPanel = Class.extend({
  //---------Start-------------
  init: function (options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");

    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$ImageViewer = this.$elem.closest('.ImageViewer').parent();

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
    html += '<a href="#" id="' + this.options.divId + '_zoom" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_fullscr" class="hidden"></a>';
    html += '<a href="#" id="' + this.options.divId + '_pinUnpin" class="hidden"></a>';

    //html += '</div> <!-- blkSessionButtonContainer -->';
    this.$elem.append(html);
    // Stash refs to buttons and Hook event handlers to buttons
    this.ctrls = {};
    this.ctrls.$fullscr = this.$elem.find('#' + this.options.divId + '_fullscr');
    this.ctrls.$fullscr.on('click', jQuery.proxy(this._onClick_fullscr, this));
    this.ctrls.$zoom = this.$elem.find('#' + this.options.divId + '_zoom');
    this.ctrls.$zoom.on('click', jQuery.proxy(this._onClick_zoom, this));
    this.ctrls.$pinUnpin = this.$elem.find('#' + this.options.divId + '_pinUnpin');
    this.ctrls.$pinUnpin.on('click', jQuery.proxy(this._onClick_pinUnpin, this));
    if (this.ctrls.$fullscr.hasClass('hidden'))
      this.ctrls.$fullscr.removeClass('hidden').addClass("fullscreenOnBtn-enabled");
    if (this.ctrls.$zoom && this.ctrls.$zoom.hasClass('hidden'))
      this.ctrls.$zoom.removeClass('hidden').addClass("zoomBtn-enabled");
    if (!this.options.controllerBarFixed && this.ctrls.$pinUnpin.hasClass('hidden'))
      this.ctrls.$pinUnpin.removeClass('hidden').addClass('pinBtn-enabled');
    this.log(bINFO, "_build", "return", "Done");
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.find('.ImageViewerButtonPanel').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
  },
  // button click pin/Unpin
  _onClick_pinUnpin: function (e) {
    this.log(bDEBUG, "_onClick_pinUnpin", "call", "");
    this.$ImageViewer.edp_imageViewer('doPinUnpin');
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
    this.$ImageViewer.edp_imageViewer('doToggleScreen');
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
  _onClick_zoom: function (e) {
    this.log(bDEBUG, "_onClick_zoom", "call", "");
    var pos = this.ctrls.$zoom.position();
    var imgWidth = this.ctrls.$zoom.width();
    pos.left = pos.left + Math.floor(imgWidth / 2) - 3;
    this.log(bDEBUG, "_onClick_zoom", "block", "@@@@@@ pos.left=" + pos.left + "pos.top=" + pos.top);
    this.$ImageViewer.edp_imageViewer('doZoomPopup', pos);
    e.preventDefault();
  },
  //------End------------------

});
jQuery.plugin('edp_imageViewerButtonPanel', ImageViewerButtonPanel);
