/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* ConnError class provides UI to give feedback about connection errors.
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.ipv.ConnError", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function (ConnError, ConnError_prototype) {

  /** @private @jsxobf-clobber */
  ConnError_prototype._LOG = jsx3.util.Logger.getLogger(ConnError.jsxclass.getName());

  /**
  * The instance initializer.
  */
  ConnError_prototype.onAfterAttach = function () {
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
  ConnError_prototype._onAppInit = function () {
    this.lblErrorName = this.getDescendantOfName("lblConnErrorName");
    this.lblErrorDetails = this.getDescendantOfName("lblConnErrorDetails");
    this.btnClose = this.getDescendantOfName("btnConnErrorClose");
    this.btnClose.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._resetError);
    this._resetError();
  }

  /** @private @jsxobf-clobber */
  ConnError_prototype._resetError = function () {
    this.lblErrorName.setText("", true);
    this.lblErrorDetails.setText("", true);
    this.cmpHide();
  }

  /** @private @jsxobf-clobber */
  ConnError_prototype.applyCollaborationInfo = function (objCollabInfo) {
    if (objCollabInfo.isSourceSelected == "IMAGE") {
      this._resetError();
      return;
    }
    // process based on state
    switch (objCollabInfo.state) {
      case 'CREATED':
      case 'AUTHORIZING':
      case 'CHECKING_AVAILABILITY':
      case 'WAITING_FOR_DEST':
      case 'WAITING_FOR_SOURCE-DEST_CONTACTED':
      case 'WAITING_FOR_SOURCE-DEST_ACKNOWLEDGED':
        this._setLoading();
        break;
      case 'WAITING_FOR_STREAMURL':
      case 'WAITING_FOR_STREAMSTART':
      case 'STREAMING':
        this._resetError();
        break;
      case 'DENIED_XAUTH':
      case 'REVOKED_XAUTH':
      case 'END_OF_MEDIA':
      case 'SETUP_ERROR':
      case 'START_ERROR':
      case 'SOURCE_ERROR':
      case 'MEDIA_ERROR':
      case 'DEST_ERROR':
      case 'DELETED':
        this._setError(objCollabInfo.state, objCollabInfo.connErrorInfo);
        break;
      case 'NONE':
        if (jsx3.util.strEmpty(objCollabInfo.connErrorInfo)) {
          this._resetError();
        }
        else {
          //This will be called when offline source is selected
          this._setError(objCollabInfo.connErrorInfo.errorCode, objCollabInfo.connErrorInfo);
        }
        break;
      default:
        this._resetError();
        break;
    }
  }

  /** @private @jsxobf-clobber */
  ConnError_prototype._setError = function (connState, errorInfo) {
    if (!jsx3.util.strEmpty(errorInfo)) {
      this.cmpShow();
      this.lblErrorName.setText(connState, true);
      this.lblErrorDetails.setText(errorInfo.errorData, true);
      this.btnClose.setDisplay(jsx3.gui.Block.DISPLAYBLOCK, true);
    }
  }

  /** @private @jsxobf-clobber */
  ConnError_prototype._setLoading = function () {
    this.cmpShow();
    this.lblErrorName.setText("Loading...", true);
    this.lblErrorDetails.setText("", true);
    this.btnClose.setDisplay(jsx3.gui.Block.DISPLAYNONE, true);
  }

});