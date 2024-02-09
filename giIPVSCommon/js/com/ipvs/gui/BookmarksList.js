/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* BookmarksList class to display boomarks on slider
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.BookmarksList", jsx3.gui.Block, null, function (BookmarksList, BookmarksList_prototype) {

  /** @private @jsxobf-clobber */
  BookmarksList_prototype._LOG = jsx3.util.Logger.getLogger(BookmarksList.jsxclass.getName());

  BookmarksList_prototype.onAfterAttach = function () {
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
  BookmarksList_prototype._onAppInit = function () {
    // get handle to the sample mark and setup onselect handler
    this.blkMark = this.getDescendantOfName("blkMark", false, false);
    // we want it to call our onSelect function
    // but the fnc call will exec in the context of the GI block
    // so later (after cloning) we will need to set the "onSelect" property in its context to point to our "onSelect" fnc
    this.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.blkMark.setEvent("this.onSelect();", jsx3.gui.Interactive.JSXCLICK);
    this.blkMark.setAttribute("onmouseover", "com.ipvs.gui.BookmarksList.onMouseOver(this)");
    this.blkMark.setAttribute("onmouseout", "com.ipvs.gui.BookmarksList.onMouseOut(this)");

    // get handle to list area and clear it - just in case
    this.blkMarkList = this.getDescendantOfName("blkMarkList", false, false);
    this.blkMarkList.removeChildren();
    this.clsApp.clsNotification.subscribe("DBUpdate", this, "_handleBookmarkNotification");
    this._bookmarkFileNID = "NONE";
    this._playlength = null;
    this._lastCollabAction = 'Stop';
  }

  // extend show
  BookmarksList_prototype.show = function (EventVar, EventVal) {
    if (this.setEnabled != null) {
      this.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  // override hide
  BookmarksList_prototype.hide = function () {
    if (this.setEnabled != null) {
      this.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
  }

  //called from controller to show/hide bookmarks list
  BookmarksList_prototype.applyCollaborationInfo = function (objCollabInfo) {
    //check enableViewBookmarks
    if(!objCollabInfo.enableViewBookmarks) {
      if(objCollabInfo.timeMode == 'PLAYBACK') {
        this.blkMarkList.removeChildren();
      }
      return;
    }
    // if Stop
    if (objCollabInfo.action == 'Stop') {
      // save action as last action
      this._lastCollabAction = objCollabInfo.action;
      this.blkMarkList.removeChildren();
      this.LastBookmarkSelectedBlkJSX = null;
      this.publish({ subject: "BOOKMARKSELECTED", context: "NONE" });
      this.hide();
    }
    // if Start after Stop
    else if (objCollabInfo.action == 'Start' && this._lastCollabAction == 'Stop' && objCollabInfo.state == 'STREAMING') {
      // save action as last action
      this._lastCollabAction = objCollabInfo.action;
      this.blkMarkList.removeChildren();
      this._bookmarkFileNID = objCollabInfo.bookmarkFileNID;
      this._playlength = objCollabInfo.pvrInfo.timeLength;;
      var reqParams = new Object();
      reqParams["WhereSessionInfo"] = "sessionId:" + objCollabInfo.sessionId + ",connGroupId:,connId:,entityId:";
      reqParams["SubscriptionID"] = "Subscription_" + jsx3.xml.CDF.getKey();
      com.ipvs.api.API.GetBookmark(reqParams).when(jsx3.$F(function (objResponse) {
        var respStatus = objResponse.Status;
        var respVal = objResponse.Val;
        if (respStatus == jsx3.net.Request.STATUS_OK) {
          this._LOG.info(":GetBookmark successful: response status:" + respStatus);
          var nodeBookmarkIter = respVal.selectNodeIterator(".//Bookmark");
          while (nodeBookmarkIter.hasNext()) {
            var nodeBookmark = nodeBookmarkIter.next();
            this.addMark(nodeBookmark, false);
          }
          this.show();
          this.blkMarkList.repaint();
        }
        else {
          this._LOG.error(":GetBookmark fail: response status:" + respStatus);
        }
      }).bind(this));
    }
  }

  //add a new bookmark into the marks list
  BookmarksList_prototype.addMark = function (nodeBookmark, bUpdateView) {
    var BookmarkNID = nodeBookmark.getAttribute("NID");
    var BookmarkAtTC = nodeBookmark.getAttribute("TC");
    var BookmarkTitle = nodeBookmark.getAttribute("title");
    var BookmarkComment = nodeBookmark.selectSingleNode(".//Description").getValue();
    if(!jsx3.util.strEmpty(BookmarkTitle)) BookmarkComment = BookmarkTitle + "-" + BookmarkComment;
    var BookmarkOffset = nodeBookmark.getAttribute("offset");
    if (BookmarkOffset != null) {
      var BookmarkPosition = Math.round(BookmarkOffset / (this._playlength / 100));
      // check value for bounds
      if (BookmarkPosition > 100) BookmarkPosition = 100;
      if (BookmarkPosition < 0) BookmarkPosition = 0;
      // clone a new bookmark and make it a child of the bookmark list block
      var newBookMarkBlkJSX = this.blkMark.doClone(jsx3.app.Model.PERSISTEMBED);
      this.blkMarkList.adoptChild(newBookMarkBlkJSX, bUpdateView);
      // set a "onSelect" property = the curr instance onSelect function to call on click
      newBookMarkBlkJSX.onSelect = this.onSelect;
      // set a MarkID property = the bookmark ID so we can get it back when selected
      newBookMarkBlkJSX.MarkID = BookmarkNID;
      // set its position
      newBookMarkBlkJSX.setDimensions(BookmarkPosition + '%', 0, 8, '100%', true);
      // set its visibility
      newBookMarkBlkJSX.setVisibility(jsx3.gui.Block.VISIBILITYVISIBLE, true);
      // set the comment as tooltip
      newBookMarkBlkJSX.setTip(BookmarkComment);
      // also save the offset value for later retrieval
      newBookMarkBlkJSX.MarkOffset = BookmarkOffset;
      if (this.LastBookmarkSelectedBlkJSX != null) {
        if (newBookMarkBlkJSX.MarkID == this.LastBookmarkSelectedBlkJSX.MarkID) {
          newBookMarkBlkJSX.setBackgroundColor("#2ca0e6", true);
          this.LastBookmarkSelectedBlkJSX = newBookMarkBlkJSX;
        }
      }
    }
  }

  //update comment of the mark
  BookmarksList_prototype.updateMark = function (nodeBookmark) {
    //get the NID and commnet for bookmark
    var BookmarkNID = nodeBookmark.getAttribute("NID");
    var BookmarkComment = nodeBookmark.selectSingleNode(".//Description").getValue();
    //find the block for this bookmark
    var blkMark = this._getMark(BookmarkNID);
    if (blkMark) {
      // set the comment as the tip
      blkMark.setTip(BookmarkComment);
    }
  }

  //delete the mark from mark list
  BookmarksList_prototype.deleteMark = function (BookmarkNID) {
    //find the block for this bookmark
    var blkMark = this._getMark(BookmarkNID);
    if (blkMark) {
      // delete it from the list
      blkMark.getParent().removeChild(blkMark);
    }
  }

  //get the mark by NID
  BookmarksList_prototype._getMark = function (BookmarkNID) {
    //find the block for this bookmark
    // iterate thru bookmark list
    var arrMark = this.blkMarkList.getChildren();
    var iterMark = (new jsx3.util.List(arrMark)).iterator();
    while (iterMark.hasNext()) {
      // for each mark
      var blkMark = iterMark.next();
      // compare markID with bookmark id
      if (blkMark.MarkID == BookmarkNID) {
        return blkMark;
      }
    }
    return null;
  }

  // extend onMouserOver - this will handle 
  // this function executes in the context of the GI component (not "this")
  BookmarksList.onMouseOver = function (objGUI) {
    // mark this bookmark as the currently selected one
    var strId = objGUI.getAttribute("id");
    var objJSX = jsx3.GO(strId);
    if (!jsx3.util.strEmpty(objJSX)) {
      objJSX.setBackgroundColor("#2ca0e6", true);
    }
  }

  // extend onMouserOver - this will handle 
  // this function executes in the context of the GI component (not "this")
  BookmarksList.onMouseOut = function (objGUI) {
    var strId = objGUI.getAttribute("id");
    var objJSX = jsx3.GO(strId);
    if (!jsx3.util.strEmpty(objJSX)) {
      var lastSelectedBlkJSX = objJSX.getAncestorOfType("com.ipvs.gui.BookmarksList");
      if (lastSelectedBlkJSX.LastBookmarkSelectedBlkJSX != null) {
        if (objJSX.MarkID == lastSelectedBlkJSX.LastBookmarkSelectedBlkJSX.MarkID) {
          objJSX.setBackgroundColor("#2ca0e6", true);
        }
        else {
          objJSX.setBackgroundColor("#9898a5", true);
        }
      }
      else {
        objJSX.setBackgroundColor("#9898a5", true);
      }
    }
  }

  // extend onselect - this will handle selection of bookmark for delete
  // this function executes in the context of the GI component (not "this")
  BookmarksList_prototype.onSelect = function () {
    // mark this bookmark as the currently selected one
    var ThisClass = this.getAncestorOfType("com.ipvs.gui.BookmarksList");
    ThisClass.markSelected.call(ThisClass, this);
  }

  // change the highlighting to reflect selected bookmark and fire select event
  BookmarksList_prototype.markSelected = function (BookmarkSelectedBlkJSX) {
    // un highlight the last selection
    if (this.LastBookmarkSelectedBlkJSX != null) {
      this.LastBookmarkSelectedBlkJSX.setBackgroundColor("#9898a5", true);
    }
    // highlight new selection
    BookmarkSelectedBlkJSX.setBackgroundColor("#2ca0e6", true);
    // save as the last selection - for next time
    this.LastBookmarkSelectedBlkJSX = BookmarkSelectedBlkJSX;
    this.publish({ subject: "BOOKMARKSELECTED", context: BookmarkSelectedBlkJSX.MarkID });
  }

  BookmarksList_prototype._handleBookmarkNotification = function (objEvent) {
    if (objEvent == null) return;
    var eventType = objEvent.type;
    if (objEvent.objType == "bookmarklist") {
      if (eventType == "replace") {
        var reqParams = new Object();
        reqParams["WhereNID"] = objEvent.nodeID;
        com.ipvs.api.API.GetBookmark(reqParams).when(jsx3.$F(function (objResponse) {
          var respStatus = objResponse.Status;
          var response = objResponse.Val;
          if (respStatus == jsx3.net.Request.STATUS_OK) {
            var nodeBookmark = response.selectSingleNode(".//Bookmark");
            var fileNID = nodeBookmark.getAttribute("mediaStreamFileNID");
            if (this._bookmarkFileNID == fileNID) {
              //don't know it is Add or update.
              //delete and add.
              this.deleteMark(nodeBookmark.getAttribute("NID"));
              this.addMark(nodeBookmark, true);
            }
          }
          else {
            this._LOG.error(":GetBookmark failed: response status:" + respStatus);
          }
        }).bind(this));
      }
      else if (eventType == "delete") {
        this.deleteMark(objEvent.nodeID);
      }
    }
  }

});