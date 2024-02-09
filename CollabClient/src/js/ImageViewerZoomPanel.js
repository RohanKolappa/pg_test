/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var ImageViewerZoomPanel = Class.extend({
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$ImageViewer = this.$elem.closest('.ImageViewer').parent();
    this._build();
  },
  _defaults: {
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    //Zoom Panel
    var child = jQuery('<div id="' + this.options.divId + '_zoomPopup' +
      '" class="zoomPopup hidden"><ul>' +
      '<li><a id="' + this.options.divId + '_zoom025" href="#">&nbsp;25%</a></li>' +
      '<li><a id="' + this.options.divId + '_zoom050" href="#">&nbsp;50%</a></li>' +
      '<li><a id="' + this.options.divId + '_zoom100" href="#">100%</a></li>' +
      '<li><a id="' + this.options.divId + '_zoom200" href="#">200%</a></li>' +
      '<li><a id="' + this.options.divId + '_zoomFit" href="#">Fit to viewport</a></li>' +
      '</ul><span class="zoomNotch"><span></div>').addClass('ImageViewerZoomPanel');
    this.$elem.append(child);
    this.ctrls = {};
    this.ctrls.$zoom025 = this.$elem.find('#' + this.options.divId + '_zoom025');
    this.ctrls.$zoom050 = this.$elem.find('#' + this.options.divId + '_zoom050');
    this.ctrls.$zoom100 = this.$elem.find('#' + this.options.divId + '_zoom100');
    this.ctrls.$zoom200 = this.$elem.find('#' + this.options.divId + '_zoom200');
    this.ctrls.$zoomFit = this.$elem.find('#' + this.options.divId + '_zoomFit');
    this.ctrls.$zoom025.click(jQuery.proxy(this._onClick_025, this));
    this.ctrls.$zoom050.click(jQuery.proxy(this._onClick_050, this));
    this.ctrls.$zoom100.click(jQuery.proxy(this._onClick_100, this));
    this.ctrls.$zoom200.click(jQuery.proxy(this._onClick_200, this));
    this.ctrls.$zoomFit.click(jQuery.proxy(this._onClick_Fit, this));
    this.log(bINFO, "_build", "return", "Done");
  },
  _onClick_025: function (event) {
    this.log(bDEBUG, "_onClick_025", "call", "");
    this.$ImageViewer.edp_imageViewer('doZoom', '25%');
    this.popupZoom();
    event.preventDefault();
  },
  _onClick_050: function (event) {
    this.log(bDEBUG, "_onClick_050", "call", "");
    this.$ImageViewer.edp_imageViewer('doZoom', '50%');
    this.popupZoom();
    event.preventDefault();
  },
  _onClick_100: function (event) {
    this.log(bDEBUG, "_onClick_100", "call", "");
    this.$ImageViewer.edp_imageViewer('doZoom', '100%');
    this.popupZoom();
    event.preventDefault();
  },
  _onClick_200: function (event) {
    this.log(bDEBUG, "_onClick_200", "call", "");
    this.$ImageViewer.edp_imageViewer('doZoom', '200%');
    this.popupZoom();
    event.preventDefault();
  },
  _onClick_Fit: function (event) {
    this.log(bDEBUG, "_onClick_Fit", "call", "");
    this.$ImageViewer.edp_imageViewer('doZoom', 'Fit');
    this.popupZoom();
    event.preventDefault();
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.find('.ImageViewerZoomPanel').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
  },
  popupZoom: function (pos, hide) {
    this.log(bDEBUG, "popupZoom", "call", hide);
    var $elemForm = this.$elem.find('.ImageViewerZoomPanel');
    if ($elemForm.is(':visible') || hide === true) {
      $elemForm.addClass('hidden');
      // $elemForm.fadeOut();
    } else {
      $elemForm.removeClass('hidden');
      // $elemForm.fadeIn();
      var toCenter = Math.round($elemForm.width() / 2);
      $elemForm.css('left', (pos.left - toCenter) + 'px');
    }
  },
  zoomSelected: function (sel) {
    this.log(bDEBUG, "zoomSelected", "call", sel);
    this.ctrls.$zoom025.parent().removeClass('zoomItemSelected');
    this.ctrls.$zoom050.parent().removeClass('zoomItemSelected');
    this.ctrls.$zoom100.parent().removeClass('zoomItemSelected');
    this.ctrls.$zoom200.parent().removeClass('zoomItemSelected');
    this.ctrls.$zoomFit.parent().removeClass('zoomItemSelected');
    switch (sel) {
      case '25%':
        this.ctrls.$zoom025.parent().addClass('zoomItemSelected');
        break;
      case '50%':
        this.ctrls.$zoom050.parent().addClass('zoomItemSelected');
        break;
      case '100%':
        this.ctrls.$zoom100.parent().addClass('zoomItemSelected');
        break;
      case '200%':
        this.ctrls.$zoom200.parent().addClass('zoomItemSelected');
        break;
      default:
        this.ctrls.$zoomFit.parent().addClass('zoomItemSelected');
    }
  }
});
jQuery.plugin('edp_imageViewerZoomPanel', ImageViewerZoomPanel);
