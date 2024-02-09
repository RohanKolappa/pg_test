/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Bookmarks class to display boomarks on slider
* <p/>
*/
jsx3.require("jsx3.gui.Block");
jsx3.Class.defineClass("com.ipvs.gui.Bookmarks", jsx3.gui.Block, null, function(Bookmarks, Bookmarks_prototype) {

  /** @private @jsxobf-clobber */
  Bookmarks_prototype._LOG = jsx3.util.Logger.getLogger(Bookmarks.jsxclass.getName());

  Bookmarks_prototype.onAfterAttach = function() {
    this.jsxsuper();
    this._onAppInit();
  }

  /** @private @jsxobf-clobber */
  Bookmarks_prototype._onAppInit = function() {
    // get handle to the sample mark and setup onselect handler
    this.blkMark = this.getDescendantOfName("blkMark", false, false);
    this.removeEvent(jsx3.gui.Interactive.JSXCLICK);
    this.blkMark.setEvent("this.onMarkClick();", jsx3.gui.Interactive.JSXCLICK);
    this.blkMark.onMarkClick = jsx3.$F(this.onBookmarkClick).bind(this)
    // get handle to list area and clear it - just in case
    this.blkMarkList = this.getDescendantOfName("blkMarkList", false, false);
    this.blkMarkList.removeChildren();
    this._bookmarkFileNID = "NONE";
    this._playlength = null;
  }

  // extend show
  Bookmarks_prototype.show = function(EventVar, EventVal) {
    if(this.setEnabled != null) {
      this.setEnabled(jsx3.gui.Form.STATEENABLED, true);
    }
  }

  // override hide
  Bookmarks_prototype.hide = function() {
    if(this.setEnabled != null) {
      this.setEnabled(jsx3.gui.Form.STATEDISABLED, true);
    }
  }


  /**
  * Displays the bookmarks on bar.
  * @param bookmarksNode {Entity} Bookmarks node
  * @param mediaLength{String} length of media in milliseconds
  */
  Bookmarks_prototype.displayBookmarks = function(bookmarksNode, mediaLength) {
    this._playlength = mediaLength;
    if(!jsx3.util.strEmpty(bookmarksNode)) {
      var nodeBookmarkIter = bookmarksNode.selectNodeIterator(".//Bookmark");
      while(nodeBookmarkIter.hasNext()) {
        var nodeBookmark = nodeBookmarkIter.next();
        this.addMark(nodeBookmark, false);
      }
    }
    this.show();
    this.blkMarkList.repaint();
  }

  /**
  * Adds and Place a new bookmark on to the marks list bar.
  * @param nodeBookmark {Entity} Bookmark node
  * @param bUpdateView{Boolean}
  */
  Bookmarks_prototype.addMark = function(nodeBookmark, bUpdateView) {
    var BookmarkNID = nodeBookmark.getAttribute("NID");
    var BookmarkAtTC = nodeBookmark.getAttribute("TC");
    var BookmarkCommentNode = nodeBookmark.selectSingleNode(".//Description");
    if(jsx3.util.strEmpty(BookmarkCommentNode)) BookmarkCommentNode = nodeBookmark.selectSingleNode(".//Comment");
    var BookmarkComment = BookmarkCommentNode.getValue();
    var BookmarkOffset = nodeBookmark.selectSingleNode(".//Offset").getValue();
    if(BookmarkOffset != null) {
      var BookmarkPosition = Math.round(BookmarkOffset / (this._playlength / 100));
      // check value for bounds
      if(BookmarkPosition > 100) BookmarkPosition = 100;
      if(BookmarkPosition < 0) BookmarkPosition = 0;
      // clone a new bookmark and make it a child of the bookmark list block
      var newBookMarkBlkJSX = this.blkMark.doClone(jsx3.app.Model.PERSISTEMBED);
      this.blkMarkList.adoptChild(newBookMarkBlkJSX, bUpdateView);
      // set a "onSelect" property = the curr instance onSelect function to call on click
      newBookMarkBlkJSX.onMarkClick = jsx3.$F(this.onBookmarkClick).bind(this, [newBookMarkBlkJSX]);
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
      if(this.LastBookmarkSelectedBlkJSX != null) {
        if(newBookMarkBlkJSX.MarkID == this.LastBookmarkSelectedBlkJSX.MarkID) {
          newBookMarkBlkJSX.setBackgroundColor("#336666", true);
          this.LastBookmarkSelectedBlkJSX = newBookMarkBlkJSX;
        }
      }
    }
  }

  /**
  * Change the highlighting to reflect selected bookmark and fire select event.
  * @param objMark {OBJECT}
  */
  Bookmarks_prototype.onBookmarkClick = function(objMark) {
    // unhighlight the last selection
    this.clearSelection();
    // save as the last selection - for next time
    this.LastBookmarkSelectedBlkJSX = objMark;
    this.publish({ subject: "BOOKMARKSELECTED", context: objMark });
  }

  /**
  * Clears the bookmark selection.
  */
  Bookmarks_prototype.clearSelection = function() {
    if(this.LastBookmarkSelectedBlkJSX != null) {
      this.LastBookmarkSelectedBlkJSX.getChild(0).setState(0);
    }
    this.LastBookmarkSelectedBlkJSX = null;
  }

  /**
  * Removes all marks from marks list bar
  */
  Bookmarks_prototype.clearList = function() {
    this.blkMarkList.removeChildren();
  }
});