/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* SessionSliderToolTipTime class.
* <p/>
*/
jsx3.require("com.ipvs.gui.CustomAlignBlock");
jsx3.Class.defineClass("com.ipvs.gui.SessionSliderToolTipTime", com.ipvs.gui.CustomAlignBlock, [com.ipvs.util.IPVSComponentI], function (SessionSliderToolTipTime, SessionSliderToolTipTime_prototype) {

  /** @private @jsxobf-clobber */
  SessionSliderToolTipTime_prototype._LOG = jsx3.util.Logger.getLogger(SessionSliderToolTipTime.jsxclass.getName());

  SessionSliderToolTipTime_prototype.onAfterAttach = function () {
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


  SessionSliderToolTipTime_prototype._onAppInit = function () {
    this.setBottom(71);
    this.blkSliderTime = this.getDescendantOfName("blkSliderTime");
    this.lblSliderTime = this.getDescendantOfName("lblSliderTime");

    this.lytBlkPanel1 = this.getDescendantOfName("lytBlkPanel1");
    this.lytBlkPanel2 = this.getDescendantOfName("lytBlkPanel2");
    this.lytBlkPanel3 = this.getDescendantOfName("lytBlkPanel3");

    this.blkSliderTime.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.blkSliderTime.repaint();
    //this.setAttribute("onmouseover", "com.ipvs.gui.SessionSliderToolTipTime.onMouseOver(this)");
    //this.setAttribute("onmouseout", "com.ipvs.gui.SessionSliderToolTipTime.onMouseOut(this)");
  }

  SessionSliderToolTipTime_prototype.onMouseOut = function (objGUI) {
    //    if (com.ipvs.app.App.clsApp.clsPreferences.getLocalPreferences("PinnedMode", true) == "true") return;
    //    var strId = objGUI.getAttribute("id");
    //    var objJSX = jsx3.GO(strId);
    //    if (!jsx3.util.strEmpty(objJSX)) {
    //      objJSX._setTimer();
    //    }
  }

  SessionSliderToolTipTime_prototype.onMouseOver = function (objGUI) {
    //    var strId = objGUI.getAttribute("id");
    //    var objJSX = jsx3.GO(strId);
    //    if (!jsx3.util.strEmpty(objJSX)) {
    //      objJSX._clearTimer();
    //    }
  }

  SessionSliderToolTipTime_prototype.updateToolTip = function (strToolTip) {
    if (jsx3.util.strEmpty(strToolTip)) return;
    this.lblSliderTime.setText(strToolTip);
    this.lblSliderTime.repaint();
  }

  SessionSliderToolTipTime_prototype.updatePositionToolTip = function (position) {
    if (jsx3.util.strEmpty(position)) return;
    if (!this.blkSliderTime) return;
    this.blkSliderTime.setLeft(position);
    this.blkSliderTime.repaint();
  }

  SessionSliderToolTipTime_prototype.getLytPanel1Width = function () {
    var blkContSliderHtmlElement = this.lytBlkPanel1.getRendered();
    var blkJsxWidth = blkContSliderHtmlElement.clientWidth;
    return blkJsxWidth;
  }

  SessionSliderToolTipTime_prototype.getLytPanel2Width = function () {
    var blkContSliderHtmlElement = this.lytBlkPanel2.getRendered();
    var blkJsxWidth = blkContSliderHtmlElement.clientWidth;
    return blkJsxWidth;
  }

  SessionSliderToolTipTime_prototype.getLytPanel3Width = function () {
    var blkContSliderHtmlElement = this.lytBlkPanel3.getRendered();
    var blkJsxWidth = blkContSliderHtmlElement.clientWidth;
    return blkJsxWidth;
  }

  SessionSliderToolTipTime_prototype.getBlkToolTipWidth = function () {
    var panel2Width = this.getLytPanel2Width();
    var panel3Width = this.getLytPanel3Width();
    var actualBlk2Width = panel2Width - panel3Width - 14;
    return actualBlk2Width;
  }

  SessionSliderToolTipTime_prototype.getBlkSliderLeft = function () {
    var leftPx = this.blkSliderTime.getLeft();
    return leftPx;
  }

  SessionSliderToolTipTime_prototype.cmpShow = function () {
    if (!this.blkSliderTime) return;
    this.blkSliderTime.setDisplay(jsx3.gui.Block.DISPLAYBLOCK);
    this.blkSliderTime.repaint();
  }

  SessionSliderToolTipTime_prototype.cmpHide = function () {
    if (!this.blkSliderTime) return;
    this.blkSliderTime.setDisplay(jsx3.gui.Block.DISPLAYNONE);
    this.blkSliderTime.repaint();
  }

});