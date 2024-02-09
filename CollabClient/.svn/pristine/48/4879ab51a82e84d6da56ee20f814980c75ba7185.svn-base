/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var SessionBookmarksList = Class.extend({
  //---------Start-------------
  init: function (options, elem) {
    // Mix in the passed-in options with the default options
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");

    // Init state
    this.objCollabInfo = {
      sessionId: '',
      state: 'NONE',
      action: 'NONE',
      recState: 'NONE',
      recAction: 'NONE',
      isTimeshifted: false,
      timeMode:'NONE'
    };

    // Save the element reference, both as a jQuery
    // reference and a normal reference
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$sessionCtl = this.$elem.closest('.SessionController').parent();

    this._bookmarkFileNID = "NONE";
    this._playlength = null;
    this._lastCollabAction = 'Stop';

    // Build the DOM's initial structure
    this._build();
  },
  _defaults: {
    
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    // Add a container for bookmarks 
    if (this.options.showBookmarkPlayback) {
      jQuery.subscribe(this.options.divId + "_onBookmarkUpdate", jQuery.proxy(this._updateBookmarkList, this));
    }
    this.log(bINFO, "_build", "call", "Done");
  },
  destroy: function (pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    if (this.options.showBookmarkPlayback) {
      jQuery.unsubscribe(this.options.divId + "_onBookmarkUpdate", jQuery.proxy(this._updateBookmarkList, this));
    }
    // destroy self
    this.$elem.find('.SessionBookmarksList').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
   },

  //add a new bookmark into the marks list
  addMark:  function (nodeBookmark) {
    this.log(bDEBUG, "addMark", "call", "");
    var BookmarkNID = nodeBookmark["@NID"];
    var BookmarkAtTC = nodeBookmark["@TC"];
    var BookmarkTitle = nodeBookmark["@title"];
    var BookmarkComment = nodeBookmark.Description;
    if (jQuery.isBlank(BookmarkTitle)) BookmarkTitle = BookmarkComment;
    var BookmarkOffset = nodeBookmark["@offset"];
    if (BookmarkOffset != null) {
      var BookmarkPosition = Math.round(BookmarkOffset / (this._playlength / 100));
      // check value for bounds
      if (BookmarkPosition > 100) BookmarkPosition = 100;
      if (BookmarkPosition < 0) BookmarkPosition = 0;
      var markId = BookmarkNID.replace(/\./g, '')
      var markClass = "bookmark-off";
      if (this.LastSelectedBookmarkId == markId) markClass = "bookmark-on";
      var mark = jQuery('<a href="#" id="' + markId + '" class="' + markClass + '" title="' + BookmarkTitle + '"></a>');
      mark.on('click', { id:markId, bookmarkNID: BookmarkNID, offSet: BookmarkOffset }, jQuery.proxy(this.onBookmarkSelected, this));
      this.$elem.append(mark);
      mark.css('left', BookmarkPosition + '%');
    }
  },

  applyCollaborationInfo: function (objCollabInfo) {
    this.log(bDEBUG, "applyCollaborationInfo", "call", "");
    this.objCollabInfo.sessionId = objCollabInfo.sessionId;
    this.objCollabInfo.state = objCollabInfo.state;
    this.objCollabInfo.action = objCollabInfo.action;
    this.objCollabInfo.recState = objCollabInfo.recState;
    this.objCollabInfo.recAction = objCollabInfo.recAction;
    this.objCollabInfo.isTimeshifted = objCollabInfo.isTimeshifted;
    this.objCollabInfo.timeMode = objCollabInfo.timeMode;
    
    //check showBookmarkPlayback
    if (!this.options.showBookmarkPlayback || objCollabInfo.timeMode != 'PLAYBACK') {
      jQuery(".ui-slider-bookmarksbar").empty();
      return;
    }
    // if Stop
    if (objCollabInfo.action == 'Stop') {
      // save action as last action
      this._lastCollabAction = objCollabInfo.action;
      jQuery(".ui-slider-bookmarksbar").empty();
      this.LastSelectedBookmarkId = null;
    }
      // if Start after Stop
    else if (objCollabInfo.action == 'Start' && this._lastCollabAction == 'Stop' && objCollabInfo.state == 'STREAMING') {
      // save action as last action
      this._lastCollabAction = objCollabInfo.action;
      jQuery(".ui-slider-bookmarksbar").empty();
      this._bookmarkFileNID = objCollabInfo.bookmarkFileNID;
      this._playlength = objCollabInfo.pvrInfo.timeLength;
      this._getBookmarks();
    }
  },

  _getBookmarks: function () {
    this.log(bDEBUG, "_getBookmarks", "call", "");
    var objData = {
      "GetBookmark": {
        "@whereSessionInfo": "sessionId:" + this.objCollabInfo.sessionId + ",connGroupId:,connId:,entityId:",
        "@whereTCRange": "",
        "@sortBy": "",
        "@whereNID": "",
        "@sortOrder": "",
        "@startFrom": "",
        "@countToFetch": "",
        "@whereFileNID": "",
        "@whereTextSubStr": "",
        "@selectFormat": "BASIC/EXTENDED"
      }
    }
    var url = this.$sessionCtl.sessionController('getSessionPostBaseURL');
    Request['doPOST'](url, objData, jQuery.proxy(function (response, status) {
      if (status != 0 && status != "error") {
        if (response.responseJSON) {
          var bookmarksList = response.responseJSON.GetBookmarkResponseData.GetBookmark.Bookmark;
          if (bookmarksList) {
            if ((bookmarksList) instanceof Array) {
              jQuery.each(bookmarksList, jQuery.proxy(function (key, bookmarkObj) {
                this.addMark(bookmarkObj);
              }, this))
            }
            else {
              this.addMark(bookmarksList);
            }
          }
        }
      }
      else {
        this.log(bERROR, "_getBookmarks", "call",
          "FAILED Session Info:" + " sessionId:" + this.objCollabInfo.sessionId + ",connGroupId:,connId:,entityId:'");
      }
    }, this));
  },

  // change the highlighting to reflect selected bookmark and fire select event
  onBookmarkSelected : function (event) {
    this.log(bDEBUG, "onBookmarkSelected", "call", "");
    // un highlight the last selection
    if (this.LastSelectedBookmarkId != null) {
      jQuery("#" + this.LastSelectedBookmarkId).removeClass("bookmark-on").addClass("bookmark-off");
    }
    // highlight new selection
    jQuery('#' + event.data.id).removeClass("bookmark-off").addClass("bookmark-on");
    // save as the last selection - for next time
    this.LastSelectedBookmarkId = event.data.id;
    this.$sessionCtl.sessionController('doSeekTo', event.data.offSet);
    this.$sessionCtl.sessionController('onBookmarkSelected', event.data.bookmarkNID);
  },

  _updateBookmarkList: function (event, eventData) {
    this.log(bDEBUG, "_updateBookmarkList", "call", "");
    if (this.objCollabInfo.state == "STREAMING" && this.objCollabInfo.timeMode == 'PLAYBACK') {
      if (eventData.action == "DELETE") {
        jQuery('#' + eventData.bookmarkNID.replace(/\./g, '')).remove();
        this.LastSelectedBookmarkId = null;
        this.$sessionCtl.sessionController('onBookmarkSelected', null);
      }
      else {
        jQuery(".ui-slider-bookmarksbar").empty();
        this._getBookmarks();
      }
    }
  }
  //------End------------------	
});
jQuery.plugin('sessionBookmarksList', SessionBookmarksList);
