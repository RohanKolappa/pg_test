/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* ImagePanel class.
* <p/>
* This class uses jsx3.gui.Block, com.ipvs.util.IPVSComponentI.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.ipv.ImagePanel", jsx3.gui.Block, null, function (ImagePanel, ImagePanel_prototype) {

  /** @private @jsxobf-clobber */
  ImagePanel_prototype._LOG = jsx3.util.Logger.getLogger(ImagePanel.jsxclass.getName());

  /*
  * onAfterAttach
  * - hook into GI DOM building to do init
  * - code before 'jsxsuper' - pre-init - executes during top-down traverse
  * - code after 'jsxsuper' - post-init - executes on the way back
  */
  ImagePanel_prototype.onAfterAttach = function () {
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


  /*
  * onAfterPaint
  * - get handle for rendered img object
  */
  ImagePanel_prototype.onAfterPaint = function (objGUI) {
    this.imgSnapShot = objGUI.ownerDocument.getElementById("imgSnapShot");
  }

  /**
  * Extended initializer.
  */
  ImagePanel_prototype._onAppInit = function () {
    this.imgSnapShot = null;
    this.snapShotHeight = null;
    this.snapShotWidth = null;
    this._LOG.info("ImagePanel: Initiated");
    this.subscribe(jsx3.gui.Interactive.AFTER_RESIZE_VIEW, this, "_setFitToWindow");
  }

  /*
  * showSnapShot
  * - set snap shot src
  * - onLoad get the image height and width
  * - resizes image to FitToWindow
  * @param imageURL {String}
  */
  ImagePanel_prototype.showSnapShot = function (imageURL) {
    this.resetSnapShot();
    this.imgSnapShot.onload = jsx3.$F(function () {
      this.snapShotWidth = this.imgSnapShot.width;
      this.snapShotHeight = this.imgSnapShot.height;
      this._zoomMode = "FITTOWINDOW";
      this._setFitToWindow();
    }).bind(this);
    this.imgSnapShot.src = imageURL;
  }

  /*
  * doZoom
  * - Calculates height and width based on zoomsize and applies those values to image 
  * @param zoomSize {String}
  */
  ImagePanel_prototype.doZoom = function (zoomSize) {
    this._zoomMode = zoomSize;
    if (zoomSize == "FITTOWINDOW") {
      this._setFitToWindow();
      return;
    }
    else {
      var newWidth = this.imgSnapShot.style.width;
      var newHeight = this.imgSnapShot.style.height;
      if (zoomSize == "NATIVE" || zoomSize == "100") {
        newWidth = this.snapShotWidth;
        newHeight = this.snapShotHeight;
      }
      else {
        zoomSize = parseInt(zoomSize);
        if (!isNaN(zoomSize) && zoomSize != 0) {
          newWidth = (this.snapShotWidth * zoomSize) / 100;
          newHeight = (this.snapShotHeight * zoomSize) / 100;
        }
      }
      this.imgSnapShot.style.width = newWidth;
      this.imgSnapShot.style.height = newHeight;
    }
  }

  /** @private @jsxobf-clobber */
  ImagePanel_prototype._setFitToWindow = function () {
    if (!jsx3.util.strEmpty(this.imgSnapShot) && !jsx3.util.strEmpty(this.snapShotWidth) && !jsx3.util.strEmpty(this.snapShotHeight) && this._zoomMode == "FITTOWINDOW") {
      var blkRendered = this.getRendered();
      var newWidth = blkRendered.clientWidth;
      var newHeight = (blkRendered.clientWidth / this.snapShotWidth) * this.snapShotHeight;
      if (newHeight > blkRendered.clientHeight) {
        newHeight = blkRendered.clientHeight;
        newWidth = (blkRendered.clientHeight / this.snapShotHeight) * this.snapShotWidth;
      }
      this.imgSnapShot.style.width = newWidth;
      this.imgSnapShot.style.height = newHeight;
    }
  }

  /*
  * resetSnapShot
  * - reset snap shot image src
  * - removes style attribute of snap shot image
  */
  ImagePanel_prototype.resetSnapShot = function () {
    this.snapShotWidth = null;
    this.snapShotHeight = null;
    this.imgSnapShot.removeAttribute('style');
    this.imgSnapShot.src = "";
  }

});
