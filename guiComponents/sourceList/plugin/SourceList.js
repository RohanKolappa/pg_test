/* ============================================================================
 * Source List Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var SourceList = Class.extend({
  _defaults: {
  },

  viewState: false,
  sourceList: null,
  selectedSrc: null,
  defaultThumbnail: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALIAAABkCAIAAACtj/0NAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAA31JREFUeNrs3NlOGlEAgGGHYRGrIKtLK1asYkWk1jR9iN72qi/c1NiKS92qsRpFGECBVpBF6JgmJlBFqZlxlv8L8cIYhyO/55wZcYQPHz/1AK0s/AhAFiALkAXIAmQBsgBZgCxAFiALkAXIAmQBkAXIAmQBsgBZgCxAFiALkAXIAmQBo7KqebA+pzM8Phbwe3sdDiP9EPOF4mlaOjpJNhpNsuhOwO9biM2KFgPOT4Nul/wYHR5aWlmr1+ssIl3ME0Zt4obbNRCPzrC36MKriZCxm/gr6PfJcZDFQ/m8HpNs1vyGGKlKewuH3d75Cy5KJSmTuyiXlR2tKLpdrpGhgIIjddgf/02cvY7weEgQhA5fs7G9q/ssOo8wd55fTqypto2XssF49LVSI+0RHr9HWYzP3fuLpGgWmljv9w4O1Ty1S6ak3xclbc7ew8HA+8U39zZhqOsWdykrvHbcesT+Z31aayL8MhSZnDDXdQt0mrQtwtzM9PORYXNtOdGBzWZ9G4t6PYOmOxPBXfqczncLMfmjGU9QcSt5hpDnCXm2MOl1CzOz222RybDX49balEAWT2bQ7Vqcn5PL0NkWmFdOOaIoxqMzumuCLJQVejGqo4WDLFQy5Pfp9JmThZIbN6uVLGAcZAHNZiEIxj8iWXQr6FN1a2azWT1uF699p12RFp5EZCpcv7pKpaWrRkPpY7kG+qORKf1uBk2UhWixzM9G5AevB4sIyAJkAbIAWaAbzWaTLNDuslIhC7RLptJkgXan6Uw6kyULtFvf2smd53X3tLkGrKxarb70bTXo9wV83ut7HLT+hc5ht2vzxkBkoQYpm5Mf/35eg/84xCKiiblkObGekjJkgRZXjUZiY+voOEkWaNFsNr/v/NjdPyALtNv/ebS2uaORC6NkoSEnp6mvqxsqvBeJLHQmkzv78jVRrdZMcYIqz40C76p9mELx1+fllfD4mPGzqFSrBruhc4eRPv6blMplRe+YppVFRI8XgP9P9uzcAKNQKYvdvYNarW74Jk5S6XyhSBYPdVmprG5uG+M26XeRg9ja3TPGWMTp2Zg6RyqVysmUZBWv6fGWD3eRzycLheLB4bG8G2ho4NxST1vOmznjaXdS0NYiArIAWYAsQBYgC4AsQBYgC5AFyAJkAbIAWYAsQBYgC5AFyAIgC5AFyAJkAbIAWUB9fwQYAO9l8hfJYUQdAAAAAElFTkSuQmCC",

  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.thumbnailTimerArr = {};
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.subscriptionID = "Src_" + new Date().getTime();
    this.log(bDEBUG, "init", "success", "");
  },

  destroy: function () {
    this.resetList();
    this.$elem.removeData('edp_sourceList');
  },

  setList: function () {
    if (!this.viewState) {
      this.viewState = true;
      this.selectedSrc = null;
      this.thumbnailTimerArr = {};
      jQuery.subscribe("PortUpdate", jQuery.proxy(this._portUpdateHandler, this));
      this.getSourceList();
    }
  },

  resetList: function () {
    this._clearThumbnailRefresh();
    this.viewState = false;
    this.sourceList = null;
    this.selectedSrc = null;
    this.thumbnailTimerArr = {};
    jQuery.unsubscribe("PortUpdate", this._portUpdateHandler);
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

  getSourceList: function (whereClause) {
    if (!this.viewState) return;
    this.log(bDEBUG, "getSourceList", "call", "");
    var urlStr = this.options.protocol + "//" + this.options.ipAddress + "/controller/Resource/Port?type=SrcPort&subscriptionID=" + this.subscriptionID + "&subscriptionScope=ALLNODES&whereState=Ready,Unmanaged&selectFormat=BASICCONNECTIONINFO&wherePermissions=VIEWFROM";
    if (whereClause) urlStr += whereClause;
    this.options.barcoAPI.getPortList(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, "getSourceList", "success", "getPortList type SrcPort success");
        this._onGetSourceListSuccess(data);
      } else {
        this.log(bERROR, "getSourceList", "fail", "getPortList type SrcPort failed");
      }
    }, this));
  },

  _onGetSourceListSuccess: function (responseObj) {
    var portListObj = {};
    if (responseObj.Resource && responseObj.Resource.Port) {
      var portList = responseObj.Resource.Port;
      if ((portList) instanceof Array) {
        for (var i = 0; i < portList.length; i++) {
          var port = portList[i];
          portListObj[port['@NID']] = { id: port['@NID'], title: port['@title'], streamType: port['@streamType'], mediaURL: port['@mediaURL'], thumbnailURL: (port['@thumbnailURL']) ? port['@thumbnailURL'] : "" };
        }
      }
      else {
        portListObj[portList['@NID']] = { id: portList['@NID'], title: portList['@title'], streamType: portList['@streamType'], mediaURL: portList['@mediaURL'], thumbnailURL: (portList['@thumbnailURL']) ? portList['@thumbnailURL'] : "" };
      }
    }
    this.displayList(portListObj);
  },

  displayList: function (responseObj) {
    this._clearThumbnailRefresh();
    this.$elem.find(".source-list-row").remove();
    this.$elem.find("#ctnNoData").remove();
    this.sourceList = responseObj;
    if (!jQuery.isEmptyObject(responseObj)) {
      jQuery.each(responseObj, jQuery.proxy(function (key, obj) {
        var timerCount = 0;
        //keep the selection if item is selected  before refresh
        var rowClass = "source-list-row display-normal";
        if (obj.id == this.selectedSrc) {
          rowClass += " selected";
        }
        var thumbnailURL = jQuery.isBlank(obj.thumbnailURL) ? this.defaultThumbnail : this.defaultThumbnail;
        jQuery('<div class="' + rowClass + '" id="src_' + obj.id + '">' +
                '<div class="source-list-cell source-item-container">' +
                  '<div class="source-list-cell-img live-item-thumb">' +
                     '<img id="' + obj.id + '" title="' + obj.title + '" src="' + thumbnailURL + '" style="width:100%; height:100%;">' +
                  '</div>' +
                  '<div class="source-item-details">' +
                    '<span class="source-item-title">' + obj.title + ' </span>' +
                    '<span class="source-item-filetype">' + obj.streamType + '</span>' +
                  '</div>' +
                '</div>' +
              '</div>').appendTo(this.$elem);
        //start thumbnail refresh
        //if (!jQuery.isBlank(obj.thumbnailURL)) {
          //set the delay for each port to refresh the thumbnails
        //  setTimeout(jQuery.proxy(this._startThumbnailRefresh, this, obj.id, obj.thumbnailURL), timerCount * 1000);
        //  timerCount++;
        //}
      }, this));
    } else {
      this.$elem.html("<p id='ctnNoData' class='pull-center'> No Data Found </p>"); 
    }

    this.log(bINFO, "displayList", "success", "Sourcelist is ready");
    this.$elem.find(".source-list-row").on('click', jQuery.proxy(this._onClick_Source, this));
    if (this.options.draggable) this.$elem.find(".source-list-cell-img").draggable({ appendTo: "body", helper: "clone" });
  },

  _onClick_Source: function (evt) {
    var itemId = evt.currentTarget.id;
    var srcNID = itemId.substring(itemId.indexOf("_") + 1);
    this.selectedSrc = srcNID;
    if (!this.options.draggable) {
      this.$elem.find(".source-list-row").removeClass("selected");
      jQuery(evt.currentTarget).addClass("selected");
    }
    if (this.options.callBack) this.options.callBack(this.sourceList[srcNID]);
  },

  deselect: function () {
    this.$elem.find(".source-list-row").removeClass("selected");
  },

  _portUpdateHandler: function (event, notifyNode) {
    var portNode = notifyNode.Port;
    if (portNode && portNode['@portType'] == "StreamSrc") {
      this.getSourceList();
    }
  },

  _startThumbnailRefresh: function (nid, imgURL) {
    var timerId = setInterval(jQuery.proxy(function (imgId, imgURL) {
      var $thumbImage = this.$elem.find("#" + imgId.replace(/(:|\.|\[|\]|,)/g, "\\$1"));
      if ($thumbImage.length) {
        //detach the element before manuplating (suggested by jquery)
        var parentDiv = $thumbImage.parent();
        var newThumbnailImg = $thumbImage.detach();
        newThumbnailImg.attr("src", imgURL + '&suffixTimeId=' + new Date().getTime());
        parentDiv.append(newThumbnailImg);
      }
      else {
        this._clearThumbnailRefresh(imgId);
      }
    }, this, nid, imgURL), 4000);
    this.thumbnailTimerArr[nid] = timerId;
  },

  _clearThumbnailRefresh: function (nid) {
    //if nid is null clear all timers
    if (jQuery.isBlank(nid)) {
      jQuery.each(this.thumbnailTimerArr, jQuery.proxy(function (nid, value) {
        clearInterval(this.thumbnailTimerArr[nid]);
        delete this.thumbnailTimerArr[nid];
      }, this));
    }
    else if (this.thumbnailTimerArr[nid]) {
      clearInterval(this.thumbnailTimerArr[nid]);
      delete this.thumbnailTimerArr[nid];
    }
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('edp_sourceList', SourceList);
