/* ============================================================================
 * MediaPlayerPanel Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var MediaPlayerPanel = Class.extend({
  _defaults: {
    playerPref: 'AUTO',
    showRecordingControls: true,
    enableShare: true
  },
  $sourcePanel: null,
  $mediaPlayer: null,
  $imageViewer: null,
  viewState: false,
  authParams:null,
 
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bINFO, "init", "call", "");
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._build();
    this.$sourcePanel = this.$elem.find("#source-panel-container").edp_sourcePanel(jQuery.extend({}, this.options, { callBack: jQuery.proxy(this._onSrcSelected, this) })).data('edp_sourcePanel');
    this.$mediaPlayer = this.$elem.find("div.barcoplayer").player(this._defaults).data('player');
    this.$imageViewer = this.$elem.find("div.imageviewer").edp_imageViewer(this._defaults).data('edp_imageViewer');
  },

  _build: function () {
    var html = '<div id="source-panel-container" class="col left panel">' +
      '</div>' +
      '<div class="col center panel">' +
        '<div id="media-player-container" class="barcoplayer hidden" style="width:100%; height:100%;"></div>' +
        '<div id="media-image-container" class="imageviewer hidden" style="width:100%; height:100%;"></div>' +
      '</div>';
    jQuery(html).appendTo(this.$elem);
  },

  setPanel: function () {
    this.log(bINFO, "setPanel", "call", "");
    if (!this.viewState) {
      this.viewState = true;
      this.$mediaPlayer.authenticate(this.authParams);
      this.$sourcePanel.setPanel();
    }
  },

  resetPanel: function () {
    if (this.viewState) {
      this.viewState = false;
      this.authParams = null;
      this.$sourcePanel.resetPanel();
      this.$mediaPlayer.stop();
      this.$imageViewer.resetViewer();
    }
  },

  destroy: function () {
    this.resetPanel();
    this.$sourcePanel.destroy();
    this.$mediaPlayer.destroy();
    this.$imageViewer.destroy();
    this.$elem.removeData('edp_mediaPlayerPanel');
    this.log(bINFO, "destroy", "return", "Done");
  },

  /**
  * show function 
  */
  show: function () {
    this.$elem.removeClass('hidden');
  },

  /**
  * hide function 
  */
  hide: function () {
    this.$elem.addClass('hidden');
  },

  setAuthParams: function (authParams) {
    this.authParams = authParams;
  },

  _onSrcSelected: function (srcType, data) {
    if (srcType === "RECORDED" && data.fileType === "MediaImage") {
      this.$mediaPlayer.hide();
      this.$mediaPlayer.stop();
      this.$imageViewer.setViewer(data);
      this.$imageViewer.show();
    }
    else {
      this.$imageViewer.hide();
      this.$imageViewer.resetViewer();
      this.$mediaPlayer.play(data.mediaURL);
      this.$mediaPlayer.show();
    }
  },
});
// expose Objects as jQuery plugins
jQuery.plugin('edp_mediaPlayerPanel', MediaPlayerPanel);