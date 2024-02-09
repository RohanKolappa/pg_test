/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* ZoomPanel class.
* <p/>
*/
jsx3.require("com.ipvs.gui.CustomAlignBlock");
jsx3.Class.defineClass("com.ipvs.ipv.ZoomPanel", com.ipvs.gui.CustomAlignBlock, [com.ipvs.util.IPVSComponentI], function (ZoomPanel, ZoomPanel_prototype) {

  /** @private @jsxobf-clobber */
  ZoomPanel_prototype._LOG = jsx3.util.Logger.getLogger(ZoomPanel.jsxclass.getName());

  /*
  * onAfterAttach
  * - hook into GI DOM building to do init
  * - code before 'jsxsuper' - pre-init - executes during top-down traverse
  * - code after 'jsxsuper' - post-init - executes on the way back
  */
  ZoomPanel_prototype.onAfterAttach = function () {
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

  /**
  * Extended initializer.
  */
  ZoomPanel_prototype._onAppInit = function () {
    this.setBottom(31);
    this.hidePanel();
    this.lbl25 = this.getDescendantOfName("lbl25");
    this.lbl50 = this.getDescendantOfName("lbl50");
    this.lbl100 = this.getDescendantOfName("lbl100");
    this.lbl200 = this.getDescendantOfName("lbl200");
    this.lblFitToWindow = this.getDescendantOfName("lblFitToWindow");

    this.lbl25.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lbl25.setEvent("this.onClick('25');", jsx3.gui.Interactive.JSXCLICK);
    this.lbl25.onClick = jsx3.$F(this._onClick_Zoom).bind(this);

    this.lbl50.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lbl50.setEvent("this.onClick('50');", jsx3.gui.Interactive.JSXCLICK);
    this.lbl50.onClick = jsx3.$F(this._onClick_Zoom).bind(this);

    this.lbl100.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lbl100.setEvent("this.onClick('100');", jsx3.gui.Interactive.JSXCLICK);
    this.lbl100.onClick = jsx3.$F(this._onClick_Zoom).bind(this);

    this.lbl200.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lbl200.setEvent("this.onClick('200');", jsx3.gui.Interactive.JSXCLICK);
    this.lbl200.onClick = jsx3.$F(this._onClick_Zoom).bind(this);

    this.lblFitToWindow.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.lblFitToWindow.setEvent("this.onClick('FITTOWINDOW');", jsx3.gui.Interactive.JSXCLICK);
    this.lblFitToWindow.onClick = jsx3.$F(this._onClick_Zoom).bind(this);
  }

  /**
  * Display the panel with zoom options.
  */
  ZoomPanel_prototype.showPanel = function () {
    this.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.repaint();
    this._subscribeClick();
  }

  /**
  * Hides the panel.
  */
  ZoomPanel_prototype.hidePanel = function () {
    this._unsubscribeClick();
    this.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.repaint();
  }

  /**
  * Resets the zoom to default (FITTOWINDOW);
  */
  ZoomPanel_prototype.resetZoom = function () {
    this._onClick_Zoom("FITTOWINDOW");
  }

  /** @private @jsxobf-clobber */
  ZoomPanel_prototype._onClick_Zoom = function (zoomSize) {
    this.hidePanel();
    this.publish({ subject: "ONZOOMSELECT", context: { ZoomSize: zoomSize} });
  }

  /** @private @jsxobf-clobber */
  //Zoom btn size is 30X36 and zoom menu width is 75
  //Menu will be closed if mouse moved out of image ase well ass zoom menu area.
  ZoomPanel_prototype._cancelZoom = function (objEvent) {
    var menuRenderedPos = this.getDescendantOfName("blkZoomMenuBlk").getRendered().getBoundingClientRect();
    var mouseClientX = objEvent.event.getTrueX();
    var mouseClientY = objEvent.event.getTrueY();
    if (mouseClientX < menuRenderedPos.left || mouseClientX > menuRenderedPos.right || mouseClientY < menuRenderedPos.top || mouseClientY > menuRenderedPos.bottom) {
      if (mouseClientY > parseInt(menuRenderedPos.bottom) + 36 || mouseClientX < parseInt(menuRenderedPos.left) + 27 || mouseClientX > parseInt(menuRenderedPos.right) - 22) {
        this._onClick_Zoom("CANCEL");
      }
    }
  }


  /** @private @jsxobf-clobber */
  ZoomPanel_prototype._subscribeClick = function () {
    jsx3.gui.Event.subscribe(jsx3.gui.Event.MOUSEMOVE, this, this._cancelZoom);
  }

  /** @private @jsxobf-clobber */
  ZoomPanel_prototype._unsubscribeClick = function () {
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.MOUSEMOVE, this);
  }

});