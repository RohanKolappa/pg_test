/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

var SessionCtrlBookmarkPanel = Class.extend({
  init: function(options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.log(bDEBUG, "init", "call", "");
    this.state = {
      mode: null,
      bookmarkResponseObj: null
    };
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.$sessionCtl = this.$elem.closest('.SessionController').parent();
    this._build();
  },
  _defaults: {
    bookmarkPopupHeight: 30
  },
  _build: function () {
    this.log(bDEBUG, "_build", "call", "");
    //Bookmark Panel
    var child = jQuery('<div id="' + this.options.divId + '_bookmarkPopup' +
      '" class="bookmarkPopup hidden">' +
      '<input id="' + this.options.divId + '_bmOffset" type="number" size="10" placeholder="Time (ms)">' +
      '<input id="' + this.options.divId + '_bmTitle" type="text" size="20" placeholder="Title">' +
      '<input id="' + this.options.divId + '_save" type="button" value="Save">' +
      '<input id="' + this.options.divId + '_cancel" type="button" value="Cancel">' +
      '</div>').addClass('SessionCtrlBookmarkPanel');
    this.$elem.append(child);
    this.ctrls = {};  
    this.ctrls.$bookmarkOffset = this.$elem.find('#' + this.options.divId + '_bmOffset');
    this.ctrls.$bookmarkTitle = this.$elem.find('#' + this.options.divId + '_bmTitle');

    this.ctrls.$bookmarkOffset.on("keyup", this._doOffsetValidate);

    child.find('#' + this.options.divId + '_save').click(jQuery.proxy(this._onClick_Save, this));
    child.find('#' + this.options.divId + '_cancel').click(jQuery.proxy(this._doCancel, this));
    this.log(bINFO, "_build", "return", "Done");
  },
  _doOffsetValidate: function() {
    this.log(bDEBUG, "_doOffsetValidate", "call", "");
    var self = jQuery(this);
    var val = self.val();
    if (val.match(/^[^0-9]/)) {
      self.val('');
      return;
    }
    var filter = val.replace(/[^0-9]/g, '' );
    self.val(filter);
  },
  _onClick_Save: function (event) {
    this.log(bDEBUG, "_onClick_Save", "call", "");
    event.preventDefault();
    this._doSave();
    this.state.mode = null;
    this.popupBookmark();
  },
  _doCancel: function (event) {
    this.log(bDEBUG, "_doCancel", "call", "");
    this.state.mode = null;
    event.preventDefault();
    this.popupBookmark();
    },
  destroy: function(pluginName) {
    this.log(bDEBUG, "destroy", "call", pluginName);
    // unsubscribe to events
    // destroy self
    this.$elem.find('.SessionCtrlBookmarkPanel').remove();
    this.$elem.removeData(pluginName);
    this.log(bDEBUG, "destroy", "call", pluginName);
   },
  popupBookmark: function(hide) {
    this.log(bDEBUG, "popupBookmark", "call", hide);
    var $elemForm = this.$elem.find('.SessionCtrlBookmarkPanel');
    if ($elemForm.is(':visible') || hide == true) {
      $elemForm.addClass('hidden');
      this.ctrls.$bookmarkOffset.show();
      this.ctrls.$bookmarkOffset.val("");
      this.ctrls.$bookmarkTitle.val("");
    } else {
      $elemForm.removeClass('hidden');
    }
  },

  //whereId -String. FileNID in case of HLS Playback. In all other cases it will be sessionId
  //offSet - Milliseconds in case of HLS Playback.In all other cases this value is null
  addBookmark: function (whereId, isHLSPlayback, offSet) {
    this.log(bDEBUG, "addBookmark", "call", offSet + ', ' + whereId);
    this.state.mode = "ADD";
    this.state.bookmarkResponseObj = {
      "AddBookmark": {
        "@whereSessionInfo": "",
        "Bookmark": {
          "Description": "",
          "@offset": "",
          "MetaData": { "Tag": { "@Name": "" } },
          "@title": "",
          "@TC": ""
        },
        "@whereFileNID": ""
      }
    };
    //HLS Playback case
    if (isHLSPlayback) {
      this.state.bookmarkResponseObj.AddBookmark['@whereFileNID'] = whereId;
      this.popupBookmark();
      this.ctrls.$bookmarkOffset.val(offSet);
    }
    else {
      var url = this.$sessionCtl.sessionController('getSessionPostBaseURL');
      var newBookmarkObj = {"NewBookmark": {
        "@whereSessionInfo": "sessionId:" + whereId,
        "@whereFileNID": ""
        }
      } 
      Request['doPOST'](url, newBookmarkObj, jQuery.proxy(function (response, status) {
        if (status != 0) {
          this.log(bINFO, "NewBookmark", "callback", "Success sessionId:" + whereId);
          var newBookmark = response.responseJSON.NewBookmarkResponseData.NewBookmark;
          if (newBookmark) {
            this.state.bookmarkResponseObj.AddBookmark['@whereSessionInfo'] = "sessionId:" + whereId;
            this.state.bookmarkResponseObj.AddBookmark.Bookmark['@TC'] = newBookmark.Bookmark['@TC'];
            this.popupBookmark();
            //In live mode while recording 'offset' is empty for NewBookmark and it will have only TC
            //Hide the offset text bax and proceed with TC
            if (jQuery.isBlank(newBookmark.Bookmark['@offset'])) {
              this.ctrls.$bookmarkOffset.hide();
            }
            else {
              this.ctrls.$bookmarkOffset.val(newBookmark.Bookmark['@offset']);
            }
          }
        }
        else {
          this.log(bINFO, "NewBookmark", "callback", "Failed sessionId:" + whereId);
        }
      }, this));
    }
  },
  editBookmark: function (bookmarkNID) {
    this.log(bDEBUG, "editBookmark", "call", bookmarkNID);
    this.state.mode = "EDIT";
    var objData = {
      "GetBookmark": {
        "@whereSessionInfo": "",
        "@whereTCRange": "",
        "@sortBy": "",
        "@whereNID": bookmarkNID,
        "@sortOrder": "",
        "@startFrom": "",
        "@countToFetch": "",
        "@whereFileNID": "",
        "@whereTextSubStr": "",
        "@selectFormat": "BASIC/EXTENDED"
      }
    };
    var url = this.$sessionCtl.sessionController('getSessionPostBaseURL');
    Request['doPOST'](url, objData, jQuery.proxy(function(response, status) {
      if( status != 0 ) {
        var bookmarkObj = response.responseJSON.GetBookmarkResponseData.GetBookmark;
        if (bookmarkObj) {
          this.state.bookmarkResponseObj = bookmarkObj;
          this.popupBookmark();
          this.ctrls.$bookmarkOffset.val(bookmarkObj.Bookmark["@offset"]);
          this.ctrls.$bookmarkTitle.val(bookmarkObj.Bookmark["@title"]);
        }
        else {
          this.log(bWARNING, "editBookmark", "A",
            "No Bookmark found with BookmarkNID:" + bookmarkNID);
        }
      }
      else {
        this.log(bERROR, "editBookmark", "B",
          "GetBookmarks Failed for BookmarkNID:" + bookmarkNID);
      }
    }, this));
  },
  deleteBookmark: function (bookmarkNID) {
    this.log(bDEBUG, "deleteBookmark", "call", bookmarkNID);
    var bookmarkObj = {
      "DeleteBookmark": {
        "@whereNID": bookmarkNID,
      }
    };
    var url = this.$sessionCtl.sessionController('getSessionPostBaseURL');
    Request['doPOST'](url, bookmarkObj, jQuery.proxy(function (response, status) {
      this.log(bINFO, "deleteBookmark", "callback",
        (status != 0) ? "Success" : "Failed" + ". BookmarkNID:" + bookmarkNID);
      var data = { action: 'DELETE', bookmarkNID: bookmarkNID };
      if (status == "success") jQuery.publish(this.options.divId + "_onBookmarkUpdate", data);
    }, this));
  },
  
  _doSave: function () {
    this.log(bDEBUG, "_doSave", "call", "");
    var bookmarkOffset = jQuery.trim(this.ctrls.$bookmarkOffset.val());
    var bookmarkTC = null;
    //Incase of Live and recording is on then there is no offset for newbookmark
    if (this.state.mode == "ADD") {
      bookmarkTC = this.state.bookmarkResponseObj.AddBookmark.Bookmark['@TC'];
    }
    if (jQuery.isBlank(bookmarkOffset) && jQuery.isBlank(bookmarkTC)) {
      alert("Please enter Bookmark offset");
      this.ctrls.$bookmarkOffset.focus();
      return;
    }
    var bookmarkTitle = jQuery.trim(this.ctrls.$bookmarkTitle.val());
    if(jQuery.isBlank(bookmarkTitle)) {
      alert("Please enter Bookmark Title");
      this.ctrls.$bookmarkTitle.focus();
      return;
    }
    
    if (this.state.mode == "ADD") {
      this.state.bookmarkResponseObj.AddBookmark.Bookmark['@offset'] = bookmarkOffset;
      this.state.bookmarkResponseObj.AddBookmark.Bookmark['@title'] = bookmarkTitle;
      var url = this.$sessionCtl.sessionController('getSessionPostBaseURL');
      Request['doPOST'](url, this.state.bookmarkResponseObj, jQuery.proxy(function (response, status) {
        this.log(bINFO, "_doSave", "callback A",
          (status != 0) ? "Success" : "Failed" + ".");
        this.state.bookmarkResponseObj = null;
        if (status != 0) {
          var data = { action: 'ADD' };
          jQuery.publish(this.options.divId + "_onBookmarkUpdate", data);
        }
      }, this));
    }
    else if (this.state.mode == "EDIT") {
      var bookMarkNID = this.state.bookmarkResponseObj.Bookmark["@NID"];
      var bookmarkObj = {
        "UpdateBookmark": {
          "@whereNID": bookMarkNID,
          "Bookmark": {
            "@offset": bookmarkOffset,
            "@title": bookmarkTitle,
          }
        }
      };
      var url = this.$sessionCtl.sessionController('getSessionPostBaseURL');
      Request['doPOST'](url, bookmarkObj, jQuery.proxy(function (response, status) {
        this.log(bINFO, "_doSave", "callback B",
          (status != 0) ? "Success" : "Failed" + ". BookmarkNID:" + bookMarkNID);
        this.state.bookmarkResponseObj = null;
        if (status != 0) {
          //this.$sessionCtl.sessionController('updateBookmarksList');
          var data = { action: 'EDIT', bookmarkNID: bookMarkNID };
          jQuery.publish(this.options.divId + "_onBookmarkUpdate", data);
        }
      }, this));
    }
  },

  applyCollaborationInfo: function (objCollabInfo) {
    this.log(bDEBUG, "applyCollaborationInfo", "call", "");
    if (objCollabInfo.state != "STREAMING" || objCollabInfo.action == 'Stop') this.popupBookmark(true);
  }
  
});
jQuery.plugin('sessionCtrlBookmarkPanel', SessionCtrlBookmarkPanel);
