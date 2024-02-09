/* ============================================================================
 * ImageViewer Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var ImageViewer = Class.extend({
  _defaults: {
    imgWidth: 0,
    imgHeight: 0,
    divId: "ifp",
    platform: "PC",
    controlBarHeight: 32,
    showControlBar: true,
    controllerBarFixed:true
  },
  viewState: false,
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bINFO, "init", "call", "");
    if (jQuery.isBlank(this.options.showControlBar)) {
      this.options.showControlBar = true;
    }
    else {
      this.options.showControlBar = (String(this.options.showControlBar).toLowerCase() === 'true') ? true : false;
    }
    if (this.options.showControlBar != false) {
      options.height = this.options.controlBarHeight;
    }
    else {
      options.height = 0;
    }
    if (jQuery.isBlank(this.options.controllerBarFixed)) {
      this.options.controllerBarFixed = false;
    }
    else {
      this.options.controllerBarFixed = (String(this.options.controllerBarFixed).toLowerCase() === 'true') ? true : false;
    }
    this.state = {
      isFullScr: false,
      isResFit: true,
      zoomSel: "Fit",
      isBarPinned: false,
      barAnimationIdleTime: 0,
      barOpenState: true,
      barTimerId: null,
      barForceOpen: false,
    };
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$rowTop = null;
    this.$img = null;
    this._build();
  },
  _build: function () {
    var html = '<div id="' + this.options.divId + '" class="img-view-frame">' +
      '<div id="' + this.options.divId + '_objectdiv' +
      '" class="img-view-div"><span class="img-view-helper"></span><img class="img-view-image"></div>' +
      '</div>';
    jQuery(html).appendTo(this.$elem);
    if (this.options.showControlBar) {
      // Add a container for session controls
      var child = jQuery('<div id="' + this.options.divId + '_buttonOverlay' +
        '" class="button-overlay" style="height: ' +
        this.options.controlBarHeight + 'px;"></div>').addClass('ImageViewer');
      this.$elem.append(child);
      var html = '<div style="float:left;"><span class="image-viewer-title"></span><span class="image-viewer-date"></div>';
      child.append(html);
      html = '<div id="' + this.options.divId + 'blkWindowButtonContainer" class="blkWindowButtonContainer"></div>';
      child.append(html);
      this.$windowBtnElem = this.$elem.find('#' + this.options.divId + 'blkWindowButtonContainer');
      this.$windowBtnElem.edp_imageViewerButtonPanel(this.options);
      html = '<div id="' + this.options.divId + 'blkZoomPanelContainer" class="blkZoomPanelContainer"></div>';
      child.append(html);
      this.$zoomPanelElem = this.$elem.find('#' + this.options.divId + 'blkZoomPanelContainer');
      this.$zoomPanelElem.edp_imageViewerZoomPanel(this.options);
    }
    jQuery(document).bind('fscreenchange', jQuery.proxy(this._onToggleScreen, this));
    this._buttonOverlayAnimationSetup();
    this.$objectdiv = this.$elem.find('div#' + this.options.divId + '_objectdiv');
    this.$img = this.$objectdiv.find('img');
    var self = this;
    this.$img.load(function (evt) {
      self.imgWidth = evt.target.naturalWidth;
      self.imgHeight = evt.target.naturalHeight;
      self._fitToWindow();
    });
  },
  setViewer: function (data) {
    this.log(bINFO, "setViewer", "call", "");
    if (!this.viewState) {
      this.viewState = true;
    }
    this.$img.attr('src', data.mediaURL);
    var $title = this.$elem.find('span.image-viewer-title');
    var $created = this.$elem.find('span.image-viewer-date');
    $title.text(data.title);
    $created.text(data.dateCreated);
  },
  resetViewer: function () {
    if (this.viewState) {
      this.viewState = false;
    }
  },
  destroy: function () {
    this.resetViewer();
    this.$elem.removeData('edp_imageViewer');
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
  _buttonOverlayAnimationSetup: function () {
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
          if (this.state.barTimerId != null)
            clearInterval(this.state.barTimerId);
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
  doToggleScreen: function () {
    this.log(bDEBUG, "doToggleScreen", "call", "");
    if (!this.state.isFullScr) {
      this.$elem.fullscreen();
    } else {
      jQuery.fullscreen.exit();
    }
  },
  doZoomPopup: function (pos, hide) {
    this.log(bDEBUG, "doZoomPopup", "call", "");
    this.$zoomPanelElem && this.$zoomPanelElem.edp_imageViewerZoomPanel('popupZoom', pos, hide);
  },
  doZoom: function (sel) {
    this.log(bDEBUG, "doZoom", "call", "sel=" + sel);
    this.state.zoomSel = sel;
    this.state.isBarPinned = (this.state.zoomSel == "Fit");
    this.doPinUnpin();
    this.$zoomPanelElem && this.$zoomPanelElem.edp_imageViewerZoomPanel('zoomSelected', sel);
    var zoomSize = 0;
    switch (sel) {
      case  '25%':
        zoomSize = 25;
        break;
      case  '50%':
        zoomSize = 50;
        break;
      case '100%':
        zoomSize = 100;
        break;
      case '200%':
        zoomSize = 200;
        break;
      default:
        this._fitToWindow();
        return;
    }
    this._setScaleToZoom(zoomSize);
  },
  doPinUnpin: function () {
    this.log(bDEBUG, "doPinUnpin", "call", "");
    if (this.options.controllerBarFixed)
      return;
    if (this.state.isBarPinned) {
      //check zoom mode is not FitToWindow or KBMEnabled if true then don't change pin mode
      if (this.state.zoomSel != "Fit")
        return;
      var bottom = 0;
      this.state.isBarPinned = false;
    }
    else {
      var bottom = jQuery('div#' + this.options.divId + '_buttonOverlay').outerHeight(true);
      this.state.isBarPinned = true;
    }
    jQuery('div#' + this.options.divId + '_objectdiv').css("bottom", bottom);
    this.$windowBtnElem && this.$windowBtnElem.edp_imageViewerButtonPanel('onPinUnpin', this.state.isBarPinned);
  },
  _fitToWindow: function () {
    this.state.zoomSel = "Fit";
    this.$img.css("width", "100%");
    this.$img.css("height", "100%");
  },
  _setScaleToZoom: function(zoomSize) {
    this.state.zoomSel = zoomSize.toString() + "%";
    this.$img.css("width", zoomSize*this.imgWidth/100.0);
    this.$img.css("height", zoomSize*this.imgHeight/100.0);
  },
  _onToggleScreen: function () {
    this.state.isFullScr = jQuery.fullscreen.isFullScreen();
    this.$windowBtnElem && this.$windowBtnElem.edp_imageViewerButtonPanel('onToggleScreen', this.state.isFullScr);
  },
});
// expose Objects as jQuery plugins
jQuery.plugin('edp_imageViewer', ImageViewer);