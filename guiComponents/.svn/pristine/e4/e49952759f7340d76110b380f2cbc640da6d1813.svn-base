/* ============================================================================
 * Source List Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var RecordingsList = Class.extend({
  _defaults: {
  },

  viewState: false,
  recordingsList: null,
  selectedRec: null,
  defaultThumbnail: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALIAAABkCAIAAACtj/0NAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAAZtJREFUeNrs2q9LA2EcwOHdj6hmxagMtAxWFgwLYhCryX/UIIaFCRoGJkGMghj1jHc3NYhM5F0RfG/v81gvfH334d6X481OTs96sCi3BMgCWSALZIEskAWyQBbIAlkgC2RBmsrYBjo+HKew7ueXE28LbCLIAlngyBmHi8m0ruvQP1CWR+MDQ3pbIAtkgSyQBbJAFsgCWSAL+Bb7x+9OfDPu1odtbwtkgSyQBY6ci65nt03dBB4oymI0HBgyrSxeq7elN1wMaRNBFsgCWSALZIEskAWyIGWxf30bDQfz3jzwQPbxZ8jUsthYX4t/ETsxpE0EWSALnC0+uW8hi1+4b2ETQRbIAlkgC2SBLJAFyIIw9y1SGXKlsnDfwiaCLJAFzhZ/6e7+oWnbwANFnu/1dw2ZVhaPT89LrzL8+4p3YkibCLJAFsgCWSALZIEskAWrKvavnPv9nbYNXWXI88yQyWWxvbUZ/yJ2YkibCLJAFsgCR84vVzczv4osfnqpKr+KTQRZIAtkgSyQBbIAWSALZIEskAWyQBZE412AAQAWfHHCcCNq0gAAAABJRU5ErkJggg==",

  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.subscriptionID = "File_" + new Date().getTime();
    this.log(bDEBUG, "init", "success", "");
  },

  destroy: function () {
    this.resetList();
    this.$elem.removeData('edp_sourceList');
  },

  setList: function () {
    if (!this.viewState) {
      this.viewState = true;
      this.selectedRec = null;
      //jQuery.subscribe("FileUpdate", jQuery.proxy(this._fileUpdateHandler, this));
      this.getFileList();
    }
  },

  resetList: function () {
    this.viewState = false;
    this.recordingsList = null;
    this.selectedRec = null;
    //jQuery.unsubscribe("FileUpdate", this._fileUpdateHandler);
    this.$elem.find(".source-list-row").remove();
  },

  /**
  * show function 
  */
  show: function () {
    this.$elem.removeClass('hidden');
  },

  /**
  * hide function 
  */
  hide: function () {
    this.$elem.addClass('hidden');
  },

  getFileList: function (whereClause) {
    if (!this.viewState) return;
    this.log(bDEBUG, "getFileList", "call", "");
    var urlStr = this.options.protocol + "//" + this.options.ipAddress + "/controller/Resource/File?whereType=MediaClip,MediaImage&whereState=Ready&sortBy=dateCreated&sortOrder=descending&subscriptionID=" + this.subscriptionID + "&selectFormat=BASICURL&wherePermissions=PLAYBACK";
    if (whereClause) urlStr += whereClause;
    this.options.barcoAPI.getFileList(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, "getFileList", "success", "getFileList success");
        this._onGetFileListSuccess(data);
      } else {
        this.log(bERROR, "getFileList", "fail", "getFileList failed");
      }
    }, this));
  },

  _onGetFileListSuccess: function (objRresponse) {
    var objFileList = {};
    if (objRresponse.Resource && objRresponse.Resource.File) {
      var fileList = objRresponse.Resource.File;
      if ((fileList) instanceof Array) {
        for (var i = 0; i < fileList.length; i++) {
          var objFile = fileList[i];
          objFileList[objFile['@NID']] = {
            id: objFile['@NID'],
            title: objFile['@title'],
            fileType: objFile['@fileType'],
            streamType: (objFile['@fileType'] == "MediaImage") ? "IMG" : objFile['@streamType'],
            mediaURL: objFile['@mediaURL'],
            thumbnailURL: (objFile['@thumbnailURL']) ? objFile['@thumbnailURL'] : this.defaultThumbnail,
            playlength: BarcoUtil.msToStr(objFile['@playlength']),
            size: BarcoUtil.convertSizeToStr(objFile['@size']),
            state: objFile['@state'],
            dateCreated: jQuery.formatDateTime('M dd, yy g:ii a', new Date(objFile['@dateCreated']), { ampmNames: ['am', 'pm'] }),
            hls: objFile['@hls']
          };
        }
      }
      else {
        objFileList[fileList['@NID']] = {
          id: fileList['@NID'],
          title: fileList['@title'],
          fileType: fileList['@fileType'],
          streamType: (fileList['@fileType'] == "MediaImage") ? "IMG" : fileList['@streamType'],
          mediaURL: fileList['@mediaURL'],
          thumbnailURL: (fileList['@thumbnailURL']) ? fileList['@thumbnailURL'] : this.defaultThumbnail,
          playlength: BarcoUtil.msToStr(fileList['@playlength']),
          size: BarcoUtil.convertSizeToStr(fileList['@size']),
          state: fileList['@state'],
          dateCreated: jQuery.formatDateTime('M dd, yy g:ii a', new Date(fileList['@dateCreated']), { ampmNames: ['am', 'pm'] }),
          hls: fileList['@hls']
        }
      }
    }
    this.displayList(objFileList);
  },

  displayList: function (objFileList) {
    this.$elem.find(".source-list-row").remove();
    this.$elem.find("#ctnNoData").remove();
    this.recordingsList = objFileList;
    if (!jQuery.isEmptyObject(objFileList)) {
      jQuery.each(objFileList, jQuery.proxy(function (key, obj) {
        var strPlaylength = "";
        if (obj.fileType == 'MediaClip') {
          strPlaylength = '<span class="source-item-playlength">' + obj.playlength + '</span>';
        }
        //keep the selection if item is selected  before refresh
        var rowClass = "source-list-row display-normal";
        if (obj.id == this.selectedRec) {
          rowClass += " selected";
        }

      var html = '<div class="' + rowClass + '" id="src_' + obj.id + '">' +
                   '<div class="source-list-cell source-item-container">' +
                     '<div class="source-list-cell-img rec-item-thumb">' +
                        strPlaylength +
                        '<img id="' + obj.id + '" title="' + obj.title + '" src="' + obj.thumbnailURL + '" style="width:100%; height:100%;">' +
                      '</div>' +
                      '<div class="source-item-details">' +
                        '<span class="source-item-title">' + obj.title + '</span>' +
                        '<span class="source-item-date">' + obj.dateCreated + '</span>' +
                        '<span class="source-item-filetype">' + obj.streamType + '</span>' +
                      '</div>' +
                    '</div>' +
                  '</div>';
        jQuery(html).appendTo(this.$elem);
      }, this));
    } else {
      this.$elem.html("<p  id='ctnNoData' class='pull-center'>  No Data Found  </p>"); 
    }
    this.log(bINFO, "displayList", "success", "Recordings list is ready");
    this.$elem.find(".source-list-row").on('click', jQuery.proxy(this._onClick_File, this));
    if (this.options.draggable) this.$elem.find(".source-list-cell-img").draggable({ appendTo: "body", helper: "clone" });
  },

  _onClick_File: function (evt) {
    var itemId = evt.currentTarget.id;
    var fileNID = itemId.substring(itemId.indexOf("_") + 1);
    this.selectedRec = fileNID;
    if (!this.options.draggable) {
      this.$elem.find(".source-list-row").removeClass("selected");
      jQuery(evt.currentTarget).addClass("selected");
    }
    if (this.options.callBack) this.options.callBack(this.recordingsList[fileNID]);
  },

  deselect: function () {
    this.$elem.find(".source-list-row").removeClass("selected");
  },

  _fileUpdateHandler: function (event, notofyFile) {
    //TBD decide what kind of file updates need to handle
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('edp_recordingsList', RecordingsList);
