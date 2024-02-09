/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Bookmark class provides methods to Add/Edit bookmark.
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.Bookmark", jsx3.gui.Block, [com.ipvs.util.IPVSComponentI], function (Bookmark, Bookmark_prototype) {

  /** @private @jsxobf-clobber */
  Bookmark_prototype._LOG = jsx3.util.Logger.getLogger(Bookmark.jsxclass.getName());

  Bookmark_prototype.onAfterAttach = function () {
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

  Bookmark_prototype._onAppInit = function () {
    this._bookMarkID = "NONE";
    this.txtBookmark = this.getDescendantOfName("txtBookmarkComments");
    this.txtBookmark.setValue("");
    this.btnCancel = this.getDescendantOfName("btnCancelBookmark");
    this.btnSave = this.getDescendantOfName("btnSaveBookmark");

    this.btnCancel.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Cancel);
    this.btnSave.subscribe(jsx3.gui.Interactive.EXECUTE, this, this._onClick_Save);
    this._LOG.info("Bookmark: Initiated");
    this._whereSessionInfo = "NONE";
  }

  //called from controller to show/hide bookmarks list
  Bookmark_prototype.applyCollaborationInfo = function (objCollabInfo) {
    //Hide Bookmark panel on End of media
    if (objCollabInfo.action == "Stop") {
      this.destroy();
    }
  }

  Bookmark_prototype.newBookmark = function (sessionId) {
    this._mode = "ADD";
    this.txtBookmark.setValue("");
    this._subscribeEnterKeyAction();
    var whereSessionInfo = "sessionId:" + sessionId + ",connGroupId:,connId:,entityId:";
    this._whereSessionInfo = whereSessionInfo;
    var reqParams = new Object();
    reqParams["WhereSessionInfo"] = this._whereSessionInfo;
    com.ipvs.api.API.NewBookmark(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._setBookmarkResponse(response);
        this.cmpShow();
        this.txtBookmark.focus();
        var ThisClass = this;
        jsx3.sleep(function () {
          ThisClass.txtBookmark.focus();
        }, null, this);
      }
      else {
        this._LOG.error(":NewBookmark failed: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "New bookmark failed");
        }
      }
    }).bind(this));
  }

  Bookmark_prototype.editBookmark = function (bookmarkId) {
    this._mode = "EDIT";
    this._subscribeEnterKeyAction();
    var reqParams = new Object();
    reqParams["WhereNID"] = bookmarkId;
    com.ipvs.api.API.GetBookmark(reqParams).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        var title = response.selectSingleNode(".//Bookmark/Description").getValue();
        this.txtBookmark.setValue(title);
        this._setBookmarkResponse(response);
        this.cmpShow();
        this.txtBookmark.focus();
        var ThisClass = this;
        jsx3.sleep(function () {
          ThisClass.txtBookmark.focus();
        }, null, this);
      }
      else {
        this._LOG.error(":GetBookmark for edit failed: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "Get bookmarks failed");
        }
      }
    }).bind(this));
  }

  Bookmark_prototype.deleteBookmark = function (bookmarkId) {
    var deleteBookmarkNode = loadXMLFile("JSXAPPS/IPVS_Viewer/xml/defaults/DeleteBookmarkRequest.xml");
    deleteBookmarkNode.setAttribute("whereNID", bookmarkId);
    com.ipvs.api.API.DeleteBookmark("DeleteBookmark", deleteBookmarkNode).when(jsx3.$F(function (objResponse) {
      var respStatus = objResponse.Status;
      var response = objResponse.Val;
      if (respStatus == jsx3.net.Request.STATUS_OK) {
        this._LOG.info(":Delete Bookmark Sucess: BookmarkID:" + bookmarkId);
      }
      else {
        this._LOG.error(":Delete Bookmark failed: response status:" + respStatus);
        if (!jsx3.util.strEmpty(objResponse.ObjError)) {
          this.clsApp.setError(objResponse.ObjError, "Delete bookmark failed");
        }
      }
      this.cmpHide();
      this.txtBookmark.setValue("");
    }).bind(this));
  }

  Bookmark_prototype.destroy = function () {
    this._unsubscribeEnterKeyAction();
    this.cmpHide();
  }

  Bookmark_prototype._setBookmarkResponse = function (responseXML) {
    this._bookmarkResponseXML = responseXML;
  }

  Bookmark_prototype._onClick_Cancel = function () {
    this.destroy();
  }

  Bookmark_prototype._onClick_Save = function () {
    var bookmarkTitle = jsx3.util.strTrim(this.txtBookmark.getValue());
    if(jsx3.util.strEmpty(bookmarkTitle)) {
      alert("Please enter a  Bookmark Comment");
      this.txtBookmark.focus();
      return;
    }
    var newBookmarkNode = this._bookmarkResponseXML.selectSingleNode("//Bookmark").cloneNode(true);
    var bookmarkNID = newBookmarkNode.getAttribute("NID");
    var bookmarkOffset = newBookmarkNode.getAttribute("offset");
    var bookmarkTC = newBookmarkNode.getAttribute("TC");
    if (this._mode == "ADD") {
      var addBookmarkNode = loadXMLFile("JSXAPPS/IPVS_Viewer/xml/defaults/AddBookmarkRequest.xml");
      var bookmarkNode = addBookmarkNode.selectSingleNode(".//Bookmark");
      bookmarkNode.setAttribute("title", "");
      bookmarkNode.setAttribute("offset", bookmarkOffset);
      bookmarkNode.setAttribute("TC", bookmarkTC);
      addBookmarkNode.setAttribute("whereSessionInfo", this._whereSessionInfo);
      addBookmarkNode.selectSingleNode(".//Bookmark/Description").setValue(bookmarkTitle);
      com.ipvs.api.API.AddBookmark("AddBookmark", addBookmarkNode).when(jsx3.$F(function (objResponse) {
        var respStatus = objResponse.Status;
        var response = objResponse.Val;
        if (respStatus == jsx3.net.Request.STATUS_OK) {
          this._bookMarkID = response.selectSingleNode("//Bookmark/@NID").getValue();
          this._whereSessionInfo = "NONE";
          this._setBookmarkResponse(null);
          this._LOG.info(":" + this._mode + "Bookmark Sucess");
        }
        else {
          this._LOG.error(":" + this._mode + "Bookmark failed: response status:" + respStatus);
          if (!jsx3.util.strEmpty(objResponse.ObjError)) {
            this.clsApp.setError(objResponse.ObjError, "Add bookmark failed");
          }
        }
        this.txtBookmark.setValue("");
      }).bind(this));
    }
    else if (this._mode == "EDIT") {
      var editBookmarkNode = loadXMLFile("JSXAPPS/IPVS_Viewer/xml/defaults/UpdateBookmarkRequest.xml");
      var bookmarkNode = editBookmarkNode.selectSingleNode(".//Bookmark");
      bookmarkNode.setAttribute("offset", bookmarkOffset);
      bookmarkNode.setAttribute("TC", bookmarkTC);
      editBookmarkNode.setAttribute("whereNID", bookmarkNID);
      editBookmarkNode.selectSingleNode(".//Bookmark/Description").setValue(bookmarkTitle);
      com.ipvs.api.API.UpdateBookmark("UpdateBookmark", editBookmarkNode).when(jsx3.$F(function (objResponse) {
        var respStatus = objResponse.Status;
        var response = objResponse.Val;
        if (respStatus == jsx3.net.Request.STATUS_OK) {
          this._setBookmarkResponse(null);
          this._LOG.info(":" + this._mode + "Bookmark Sucess");
          this._bookmarkMsg = "Edit Bookmark saved";
        }
        else {
          this._LOG.error(":" + this._mode + "Bookmark failed: response status:" + respStatus);
          this._bookmarkMsg = "Edit Bookmark failed";
          if (!jsx3.util.strEmpty(objResponse.ObjError)) {
            this.clsApp.setError(objResponse.ObjError, "Edit bookmark failed");
          }
        }
        this.txtBookmark.setValue("");
      }).bind(this));
    }
    this.destroy();
  }

  Bookmark_prototype._subscribeEnterKeyAction = function () {
    jsx3.gui.Event.subscribe(jsx3.gui.Event.KEYPRESS, this, this._handleEnterKeyAction);
  }

  Bookmark_prototype._unsubscribeEnterKeyAction = function () {
    jsx3.gui.Event.unsubscribe(jsx3.gui.Event.KEYPRESS, this, this._handleEnterKeyAction);
  }

  Bookmark_prototype._handleEnterKeyAction = function (objEvent) {
    if (objEvent.event.enterKey()) {
      this._onClick_Save();
    }
  }
});