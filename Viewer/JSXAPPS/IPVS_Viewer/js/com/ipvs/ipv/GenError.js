/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* GenError class to provide UI for error display.
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.ipv.GenError", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function (GenError, GenError_prototype) {

  /** @private @jsxobf-clobber */
  GenError_prototype._LOG = jsx3.util.Logger.getLogger(GenError.jsxclass.getName());

  GenError_prototype.onAfterAttach = function () {
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

  /** @private @jsxobf-clobber */
  GenError_prototype._onAppInit = function () {
    this.blkGenErrorName = this.getDescendantOfName("lblGenErrorName");
    this.blkGenErrorName.setCanSpy(true);
    this.blkGenErrorName.removeEvent(jsx3.gui.Interactive.SPYGLASS);
    this.blkGenErrorName.setEvent("this.showErrorDetails()", jsx3.gui.Interactive.SPYGLASS);
    this.blkGenErrorName.showErrorDetails = jsx3.$F(this.showErrorDetails).bind(this);
    this.btnGenErrorClose = this.getDescendantOfName("btnGenErrorClose");
    this.btnGenErrorClose.subscribe(jsx3.gui.Interactive.EXECUTE, this, this.resetError);
    this.clsApp.subscribe(com.ipvs.app.App.ON_ERROR, this, this._onError);

  }

  /** @private @jsxobf-clobber */
  GenError_prototype._onError = function () {
    var objError = this.clsApp.getError();
    if (jsx3.util.strEmpty(objError)) {
      this.resetError();
    }
    else {
      this.setError(objError);
    }
  }

  /* setError
  * Display error panel and shows error name
  */
  GenError_prototype.setError = function (objError) {
    this.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    var txtStr = objError.errorName;
    this.blkGenErrorName.setText(txtStr, true);
  }

  /* resetError
  * hides error panel
  */
  GenError_prototype.resetError = function () {
    this.blkGenErrorName.setText("", true);
    this.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

  /* showErrorDetails
  * show error details in spy glass
  */
  GenError_prototype.showErrorDetails = function () {
    var errorDetails = this.clsApp.getError();
    return '<div id="innerDiv" style="background-color: #ffffcc;"><table><tr><td>' + errorDetails.errorData + '</td></tr></table></div>';
  }
});