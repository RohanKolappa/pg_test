/* ============================================================================
 * WindowControlPanel jQuery plugin
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */

var WindowControlPanel = Class.extend({
  /**
  * _defaults options 
  */
  _defaults: {
  },

  /**
  * init function 
  * @param options {JSON}, 
  * @param elem {Object}
  */

  defaultThumbnail: "data:image/png;base64,iVBORw0KGgoAAAANSUhEUgAAALIAAABkCAIAAACtj/0NAAAAGXRFWHRTb2Z0d2FyZQBBZG9iZSBJbWFnZVJlYWR5ccllPAAAA31JREFUeNrs3NlOGlEAgGGHYRGrIKtLK1asYkWk1jR9iN72qi/c1NiKS92qsRpFGECBVpBF6JgmJlBFqZlxlv8L8cIYhyO/55wZcYQPHz/1AK0s/AhAFiALkAXIAmQBsgBZgCxAFiALkAXIAmQBkAXIAmQBsgBZgCxAFiALkAXIAmQBo7KqebA+pzM8Phbwe3sdDiP9EPOF4mlaOjpJNhpNsuhOwO9biM2KFgPOT4Nul/wYHR5aWlmr1+ssIl3ME0Zt4obbNRCPzrC36MKriZCxm/gr6PfJcZDFQ/m8HpNs1vyGGKlKewuH3d75Cy5KJSmTuyiXlR2tKLpdrpGhgIIjddgf/02cvY7weEgQhA5fs7G9q/ssOo8wd55fTqypto2XssF49LVSI+0RHr9HWYzP3fuLpGgWmljv9w4O1Ty1S6ak3xclbc7ew8HA+8U39zZhqOsWdykrvHbcesT+Z31aayL8MhSZnDDXdQt0mrQtwtzM9PORYXNtOdGBzWZ9G4t6PYOmOxPBXfqczncLMfmjGU9QcSt5hpDnCXm2MOl1CzOz222RybDX49balEAWT2bQ7Vqcn5PL0NkWmFdOOaIoxqMzumuCLJQVejGqo4WDLFQy5Pfp9JmThZIbN6uVLGAcZAHNZiEIxj8iWXQr6FN1a2azWT1uF699p12RFp5EZCpcv7pKpaWrRkPpY7kG+qORKf1uBk2UhWixzM9G5AevB4sIyAJkAbIAWaAbzWaTLNDuslIhC7RLptJkgXan6Uw6kyULtFvf2smd53X3tLkGrKxarb70bTXo9wV83ut7HLT+hc5ht2vzxkBkoQYpm5Mf/35eg/84xCKiiblkObGekjJkgRZXjUZiY+voOEkWaNFsNr/v/NjdPyALtNv/ebS2uaORC6NkoSEnp6mvqxsqvBeJLHQmkzv78jVRrdZMcYIqz40C76p9mELx1+fllfD4mPGzqFSrBruhc4eRPv6blMplRe+YppVFRI8XgP9P9uzcAKNQKYvdvYNarW74Jk5S6XyhSBYPdVmprG5uG+M26XeRg9ja3TPGWMTp2Zg6RyqVysmUZBWv6fGWD3eRzycLheLB4bG8G2ho4NxST1vOmznjaXdS0NYiArIAWYAsQBYgC4AsQBYgC5AFyAJkAbIAWYAsQBYgC5AFyAIgC5AFyAJkAbIAWUB9fwQYAO9l8hfJYUQdAAAAAElFTkSuQmCC",
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._build();
    this._resetVariables();
    this.sessionData = this.options.sessionData;
    this.sourceTitle = "";   
    this.log(bINFO, "init", "return", "Done");
  },

  /**
  * _resetVariables  
  */
  _resetVariables: function () {
    this.destTypeRx = false;
    this.destWinArr = [];
    this.destTnTimerArr = {};
    this.displayConfigRecord = null;
    this.displayStatusNode = null;
    this.displayWinArr = null;
    this.selDestNid = '';
    this.selDstWin = '';
    this.selDstWinCtrls = {};
    this.sourceTitle = "";
    this.sessionData = null;
    this.setPortWinPropReqUrl = null;
    this.subscriptionId = 'Dest_' + new Date().getTime();
  },

  /**
  * _build 
  */
  _build: function () {
  },

  /**
  * setPanel function 
  */
  setPanel: function () {
    this.log(bINFO, "setPanel", "call", "");
    this.destTypeRx = (this.options.featureType == "XP200DisplayConfig") ? true : false;
    jQuery.subscribe("PortDisplayConfigUpdate", jQuery.proxy(this._portDisplayConfigUpdateHandler, this));
    jQuery.subscribe("PortDisplayStatusUpdate", jQuery.proxy(this._portDisplayStatusUpdateHandler, this));
    this.$elem.parent().on('click', jQuery.proxy(this._onClickPanel, this));
    this.selDestNid = this.options.nid;
    this._getSourceTitle();
    this.getDisplayConfig();
    if (!this.destTypeRx) {
      //for ROC only
      this.getDisplayStatus();
    }
  },

  /**
  * resetPanel function 
  */
  resetPanel: function () {
    jQuery.unsubscribe("PortDisplayConfigUpdate", jQuery.proxy(this._portDisplayConfigUpdateHandler, this));
    jQuery.unsubscribe("PortDisplayStatusUpdate", jQuery.proxy(this._portDisplayStatusUpdateHandler, this));
    this.$elem.parent().off('click', jQuery.proxy(this._onClickPanel, this));
    this._clearUpTnUrlTimer();
    this.options.nid = null;
    this._resetVariables();

  },

  /**
  * destroy function 
  */
  destroy: function () {
    this.resetPanel();
    this.hide();
    this.$elem.find(".display-item-container").remove();
    this.$elem.removeData('edp_windowControlPanel');
    this.log(bINFO, "destroy", "return", "Done");
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

  _getSourceTitle: function () {
    jQuery.each(this.sessionData, jQuery.proxy(function (index, data) {
      if (data && data.PortConnectionInfo && data.PortConnectionInfo['@dstNID'] == this.options.nid && data.PortConnectionInfo['@srcTitle']) {
        this.sourceTitle = data.PortConnectionInfo['@srcTitle'];
      }
    }, this));
  },

  /**
  * getDisplayConfig: send getDisplayConfig request 
  */
  getDisplayConfig: function () {
    var urlStr = this.options.protocol + '//' + this.options.ipAddress + '/controller/Resource/Port?byNID=' + this.options.nid + '&&subscriptionID=DisplyConfig_' + this.subscriptionId;
    this.log(bINFO, "getDisplayConfig", "call", "url: " + urlStr);
    this.options.barcoAPI.getDisplayConfig(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, 'getDisplayConfig', 'success', 'getDisplayConfig success ');
        this._applyPortDisplayConfig(data);
        this._applyPortSessionInfo();
        if (this.destTypeRx) {
          //for RX XP100/XP200/NGSD200, get window info from displayConfig as NGSD200 no displayStatus
          var displayConfigNode = data.Resource ? data.Resource.PortDisplayConfig : null;
          this._applyPortDisplayStatus(displayConfigNode);
        }
        if (this.destWinArr && this.destWinArr[0]) {
          this.selDstWin = this.destWinArr[0].windowid;
          this._handleDstWindowClick();
        }

      } else {
        this.log(bERROR, 'getDisplayConfig', 'fail', 'getDisplayConfig fail');
      }
    }, this));
  },

  /**
  * _applyPortDisplayConfig function
  * apply port display config from get DisplayConfig response or update notification
  * format of response: 'Resource/PortDisplayConfig'
  * format of notification 'PortDisplayConfig'
  * @param nodeObj {JSON}
  */
  _applyPortDisplayConfig: function (nodeObj) {
    this.log(bINFO, "_applyPortDisplayConfig", "call", "with:" + nodeObj);
    this.displayConfigRecord = (nodeObj.Resource) ? nodeObj.Resource.PortDisplayConfig : nodeObj.PortDisplayConfig;
    if (this.displayConfigRecord) {
      this._detachEvents();
      this.$elem.find('.display-item-container').parent().empty();
      // nodeObj.Resource['@title'], but node from notification no title thus using dest title
      var title = this.options.title;
      var nid = this.displayConfigRecord['@NID'];
      var canvasNode = this.displayConfigRecord.Canvas;
      this._applyCanvas(nid, title, canvasNode);
      this._attachEvents();
    } else {
      this.log(bINFO, '_applyPortDisplayConfig', 'return', 'no DisplayConfig');
    }
  },

  /**
  * _applyCanvas function
  * @param nid {string}
  * @param canvasNode {JSON}
  */
  _applyCanvas: function (nid, title, canvasNode) {
    var cvsWidth = canvasNode['@hres'],
        cvsHeight = canvasNode['@vres'],
        divWidth = (this.$elem.innerWidth() == 0) ? 400 : this.$elem.innerWidth(),
        divHeight = (this.$elem.innerHeight() == 0) ? 230 : this.$elem.innerHeight(),
        windowNode = canvasNode.Window,
        layoutCssCls = '',
        winId = '',
        dominatWidth = 0,
        dominatHeight = 0,
        convertRatio = 0;
    if (cvsWidth / cvsHeight < divWidth / divHeight) {
      // adjust divWidth to meet cvsWidth/cvsHeight ration
      dominatWidth = parseInt(divHeight * cvsWidth / cvsHeight);
      dominatHeight = divHeight;
    }
    else {
      //adjust cvsHeight to meet cvsWidth/cvsHeight ration
      dominatWidth = divWidth;
      dominatHeight = parseInt(divWidth * cvsHeight / cvsWidth);
    }
    convertRatio = dominatWidth / cvsWidth; //same as dominatHeight/cvsHeight
    this.$elem.innerWidth(dominatWidth);
    //this.$elem.innerHeight(dominatHeight);
    this.destWinArr = [];
    var dstStrArr = [];
    if (windowNode && (windowNode instanceof Array)) {
      var i;
      for (i = 0; i < windowNode.length; ++i) {
        var winObj = windowNode[i],
            winId = winObj['@id'],
            //tnUrl = winObj.ThumbnailContent['@imageURL'],
            tnUrl = this.defaultThumbnail,
            propertyNode = winObj.Properties,
            hpos = propertyNode['@hpos'] + '',
            hres = propertyNode['@hres'] + '',
            vpos = propertyNode['@vpos'] + '',
            vres = propertyNode['@vres'] + '',
            zIndex = propertyNode['@zIndex'] + '',
            visibility = propertyNode['@visibiity'],
            scale = propertyNode['@scale'];
        //tnUrl = tnUrl || "./resource/images/lightbg.png"; //src-thumb-video-online.png";
        //bind destNID and winId for ROC   	
        this.destWinArr.push({ "nid": nid, "windowid": winId, "winNode": {} });
        hpos = (hpos == 0) || (hpos.indexOf('%') > 0) ? hpos : parseInt(parseInt(hpos) * convertRatio) + 'px',
        vpos = (vpos == 0) || (vpos.indexOf('%') > 0) ? vpos : parseInt(parseInt(vpos) * convertRatio) + 'px',
        hres = (hres == 0) || (hres.indexOf('%') > 0) ? hres : parseInt(parseInt(hres) * convertRatio) + 'px',
        vres = (vres == 0) || (vres.indexOf('%') > 0) ? vres : parseInt(parseInt(vres) * convertRatio) + 'px',
        inlineStyle = 'position:absolute;top:' + vpos + ';left:' + hpos + ';height:' + vres + ';width:' + hres + ';z-index:' + zIndex + ';';
        dstStrArr.push(' <div id="' + winId + '" class="display-window\" style=\"' + inlineStyle + '\" >');
        dstStrArr.push(' <img src="' + tnUrl + '" style="width:100%; height:100%; !important" id=\"img_' + nid + '\">');
        dstStrArr.push(' </div>');
      }
    }
    else {
      var winId = windowNode['@id'] != "id" ? windowNode['@id'] : 'No_' + nid;
      //bind destNID and winId for RX
      this.destWinArr.push({ "nid": nid, "windowid": winId, "winNode": {} });
      var imgSrc = this.defaultThumbnail; //windowNode.ThumbnailContent ? 'src="' + windowNode.ThumbnailContent['@imageURL'] + '"' : 'src=""';
      //if one window like Rx/Xp
      var styleCSS = 'position:absolute;top:0;left:0;height:100%;width:100%;zIndex:1';
      dstStrArr.push(' <div class=\"display-window\" id=\"' + winId + '\" style=\"' + styleCSS + '\">');
      dstStrArr.push(' <img ' + imgSrc + ' style="width:100%; height:100%; !important" id=\"img_' + nid + '\">');
      dstStrArr.push(' </div>');
    }

    var displayHtmlStr = '';
    if (this.destTypeRx) {
      //All buttons default are disabled with separate css 'dsbl', when available change 'dsbl' to 'on', OFF css is as it is 'display-item-delete'
      displayHtmlStr = '<div id="container_' + nid + '" class="display-item-container">' +
				                 '  <div id="lblDisplayItemTitle" class="display-item-title"><i class="fa fa-angle-left hidden" style="font-size:24px;margin-right:8px"></i>' + title + '</div>' +
                         '  <div class="display-item-card">' +
                         '    <div class="display-item-thumb-container" id="' + nid + '">' + dstStrArr.join('') + '<label id="srcTitle" class="display-list-item-title" style="position:absolute;"> ' + this.sourceTitle + '</label></div>' +
                         '  	 <div class="display-item-controls"> ' +
			                   '     <div id="delBtn" class="display-item-control-btn display-item-delete dsbl "><span>Disconnect</span></div>' +
                         '    </div>' +
                         '  </div>' +
                         '</div>	';
    } else {
      //All buttons default are disabled with separate css 'dsbl', when available change 'dsbl' to 'on', OFF css is as it is 'display-item-delete'
      displayHtmlStr = '<div id="container_' + nid + '" class="display-item-container">' +
				                 '  <div id="lblDisplayItemTitle" class="display-item-title"><i class="fa fa-angle-left hidden" style="font-size:24px;margin-right:8px"></i>' + title + '</div>' +
                         '  <div class="display-item-card">' +
                         '    <div class="display-item-thumb-container" id="' + nid + '">' + dstStrArr.join('') + '<label id="srcTitle" class="display-list-item-title" style="position:absolute;"> ' + this.sourceTitle + '</label> </div>' +
                         '  	 <div class="display-item-controls"> ' +
			                   '     <div id="delBtn" class="display-item-control-btn display-item-delete dsbl "><span>Disconnect</span></div>' +
			                   '     <div id="muteBtn" class="display-item-control-btn display-item-mute dsbl"><span>Audio</span></div>' +
			                   '     <div id="kbmBtn" class="display-item-control-btn display-item-kbm dsbl"><span>Remote</span></div>' +
			                   '     <div id="1To1Btn" class="display-item-control-btn display-item-1to1 dsbl"><span>Native</span></div>' +
			                   '     <div id="fsBtn" class="display-item-control-btn display-item-fullscreen dsbl"><span>Fullscreen</span></div>' +
			                   '     <div id="hiddenBtn" class="display-item-control-btn display-item-hidden dsbl"><span>Hide</span></div>' +
                         '    </div>' +
                         '  </div>' +
                         '</div>	';
    }
    this.$elem.html(displayHtmlStr); //'#divControllerRightPanel'
    this.log(bINFO, "_applyCanvas", "return", "DONE");
  },

  /**
  * _applyPortSessionInfo function
  * apply port session info to display config 
  * @param nodeObj {JSON}
  */
  _applyPortSessionInfo: function () {
    this.log(bINFO, "_applyPortSessionInfo", "call", "");
    if (this.sessionData) {
      //check dst is already connected
      for (var i = 0; i < this.destWinArr.length; i++) {
        jQuery.each(this.sessionData, jQuery.proxy(function (key, sdata) {
          //check dst is already connected
          if (sdata.PortConnectionInfo['@dstNID'] == this.destWinArr[i].nid && sdata.PortConnectionInfo['@windowId'] == this.destWinArr[i].windowid) {
            this.updateDstOnConnState(sdata.PortConnectionInfo, sdata.PortConnectionInfo['@state']);
            return false;
          }
        }, this));
      }
    }
  },

  /**
  * getDisplayStatus: send getDisplayStatus request 
  */
  getDisplayStatus: function () {
    var urlStr = this.options.protocol + '//' + this.options.ipAddress + '/controller/Resource/Port?byNID=' + this.options.nid + '&&subscriptionID=DisplyStatus_' + this.subscriptionId;
    this.log(bINFO, "getDisplayStatus", "call", "url: " + urlStr);
    this.options.barcoAPI.getDisplayStatus(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, 'getDisplayStatus', 'success', 'response success ');
        var displayStatusNode = data.Resource ? data.Resource.PortDisplayStatus : null;
        this._applyPortDisplayStatus(displayStatusNode);
      } else {
        this.log(bERROR, 'getDisplayStatus', 'fail', 'request fail');
      }
    }, this));
  },

  /**
  * _applyPortDisplayStatus function
  * apply port display status by response or update notification
  * @param nodeObj {JSON}
  */
  _applyPortDisplayStatus: function (displayStatusNode) {
    this.log(bINFO, "_applyPortDisplayStatus", "call", "with displayStatusNode:" + displayStatusNode);
    if (displayStatusNode) {
      this.displayStatusNode = displayStatusNode;
      this.displayWinArr = this.displayStatusNode.Canvas.Window;
      if (!((this.displayWinArr) instanceof Array)) {
        this.displayWinArr = new Array(this.displayWinArr);
      }
      for (i = 0; i < this.displayWinArr.length; i++) {
        if (this.displayWinArr[i]['@id'] == this.selDstWin) {
          this._updateWindowingControlByProps(this.displayWinArr[i]);
          this.log(bINFO, "_applyPortDisplayStatus", "call", "_updateWindowingControlByProps to window: " + this.selDstWin);
          break;
        }
      }
      //keep previous selected window 
      if (this.selDstWin) {
        var associatedWinDOM = this.$elem.find('#' + this.selDstWin);
        associatedWinDOM.addClass("display-window-selected");
      }
    } else {
      this.log(bINFO, '_applyPortDisplayStatus', 'return', 'no DisplayStatus node');
    }
  },

  /**
  * _attachEvents function
  */
  _attachEvents: function () {
    //add click to control window
    this.$elem.find(".display-window").on('click', jQuery.proxy(this._onClickDstWindow, this));
    //add click to window buttons
    this.$elem.find('.display-item-control-btn').each(jQuery.proxy(function (index, elem) {
      jQuery(elem).on('click', jQuery.proxy(this._processWindowControlActions, this));
    }, this));
    //this need callback to DisplayListPanel
    //this.$elem.find('#lblDisplayItemTitle').on('click', jQuery.proxy(this._onCloseDestCtrlWindow, this));
  },

  /**
  * _detachEvents function
  */
  _detachEvents: function () {
    //add click to control window
    this.$elem.find(".display-window").off('click', jQuery.proxy(this._onClickDstWindow, this));
    //add click to window buttons
    this.$elem.find('.display-item-control-btn').each(jQuery.proxy(function (index, elem) {
      jQuery(elem).off('click', jQuery.proxy(this._processWindowControlActions, this));
    }, this));
    //this need callback to DisplayListPanel
    //this.$elem.find('#lblDisplayItemTitle').off('click', jQuery.proxy(this._onCloseDestCtrlWindow, this));
  },

  /**
  * _onClickPanel function, click window
  * @param evt {JSON}
  */
  _onClickPanel: function (evt) {
    if (evt.target.className.indexOf("display-item-control-btn") >= 0) return; //"display-item-title"
    this._disableWindowControls();
    this.$elem.find(".display-window").removeClass("display-window-selected");
    if (this.selDstWin) this.selDstWin = '';
    if (this.options.selectDestCallBack) this.options.selectDestCallBack(this.selDstWin);
    this.log(bINFO, '_onClickPanel', 'call', ' selectedDstWin:' + this.selDstWin);
  },

  /**
  * _onClickDstWindow function, click window
  * @param evt {JSON}
  */
  _onClickDstWindow: function (evt) {
    //img_default.mediastreamdstportresourcelist.msdprl_0b12adbe-8e5c-43fe-92dd-2bd8f889d619
    evt.stopPropagation();
    this.selDstWin = evt.currentTarget.id;
    this._handleDstWindowClick();
    //if (this.options.selectDestCallBack) this.options.selectDestCallBack(this.selDstWin);
    //var sessionId = this._getSessionByDestWinId(this.selDestNid, this.selDstWin);
    //if (sessionId) {
    //  this._enableWindowControls();
    //} else {
    //  this._disableWindowControls();
    //}
    //this.$elem.find(".display-window").removeClass("display-window-selected");
    //jQuery(evt.currentTarget).addClass("display-window-selected");
    //this.log(bINFO, '_onClickDstWindow', 'call', ' selectedDstWin:' + this.selDstWin);
  },

  _handleDstWindowClick: function () {
    if (this.options.selectDestCallBack) this.options.selectDestCallBack(this.selDstWin);
    var sessionId = this._getSessionByDestWinId(this.selDestNid, this.selDstWin);
    if (sessionId) {
      this._enableWindowControls();
    } else {
      this._disableWindowControls();
    }
    this.$elem.find(".display-window").removeClass("display-window-selected");
    this.$elem.find("#" + this.selDstWin.replace(/(:|\.|\[|\]|,)/g, "\\$1")).addClass("display-window-selected");
    this.log(bINFO, '_onClickDstWindow', 'call', ' selectedDstWin:' + this.selDstWin);
  },

  /**
  * _onCloseDestCtrlWindow function
  * @param evt {JSON}
  */
  _onCloseDestCtrlWindow: function (evt) {
    //click back to DestPanel
    jQuery.publish('onCloseDestCtrlWindow', "Close");
    this.destroy();
  },

  /**
  * _portDisplayConfigUpdateHandler function
  * handler for "PortDisplayConfigUpdate" notoification
  * @event {Event Object}
  * @PortDisplayConfigNode {JSON}, DisplayConfig update notification, root is 'PortDisplayConfig' 
  * **/
  _portDisplayConfigUpdateHandler: function (event, PortDisplayConfigNode) {
    if (PortDisplayConfigNode) this._applyPortDisplayConfig(PortDisplayConfigNode);
    this._applyPortSessionInfo();
    this._enableWindowControls();
  },

  /**
  * _portDisplayStatusUpdateHandler function
  * handler for "PortDisplayStatusUpdate" notoification
  * @event {Event Object}
  * @notifyNode {JSON}, DisplayStatus update notification 
  * **/
  _portDisplayStatusUpdateHandler: function (event, notifyNode) {
    var displayStatusNode = notifyNode.PortDisplayStatus;
    if (displayStatusNode) this._applyPortDisplayStatus(displayStatusNode);
    this._enableWindowControls();
  },

  /**
  * _updateWindowingControlByProps function
  * update windowing control by window properties
  * @windowObj {JSON}
  * **/
  _updateWindowingControlByProps: function (windowObj) {
    //if no sessionId then return (i.e. delete case)
    if (!this._getSessionByDestWinId(this.selDestNid, this.selDstWin)) return;
    var winProperties = windowObj['Properties'];
    var connContent = windowObj['ConnectionContent'];
    var v2dDest = (this.options.streamType.indexOf('V2D') >= 0) || (this.options.streamType.indexOf('RTP') >= 0);
    //winProperties may be null (i.e. rx)
    if (winProperties) {
      var mute = winProperties['@mute'];
      var kbm = winProperties['@remoteKbmCtrl'];
      var srcKbm = connContent['@srcKbmCtrl'];
      var sc = winProperties['@scale']; //'fitToWindow','fitNativeSize'
      var fs = winProperties['@maximize'];
      var vis = winProperties['@visibility'];

      this.log(bINFO, '_updateWindowingControlByProps', 'call', ' properties, mute:' + mute + ' srcKbm:' + srcKbm + ' kbm:' + kbm + ' sc:' + sc + ' fs:' + fs + ' vis:' + vis + ' V2D-source=' + v2dDest + ',this.destTypeRx=' + this.destTypeRx);
      //delte
      this.$elem.find('#delBtn').removeClass('dsbl');
      //mute
      if (mute) {
        this.$elem.find('#muteBtn').removeClass('dsbl').addClass('on');
      }
      else {
        this.$elem.find('#muteBtn').removeClass('dsbl').removeClass('on');
      }
      //kbm
      if (v2dDest && srcKbm) {
        if (kbm) {
          this.$elem.find('#kbmBtn').removeClass('dsbl').addClass('on');
        } else {
          this.$elem.find('#kbmBtn').removeClass('dsbl').removeClass('on');
        }
      } else {
        this.$elem.find('#kbmBtn').removeClass('on').addClass('dsbl');
      }
      //1To1
      if (v2dDest) {
        if (sc === 'fitWindow') {
          this.$elem.find('#1To1Btn').removeClass('dsbl').removeClass('on');
        }
        else {
          this.$elem.find('#1To1Btn').removeClass('dsbl').addClass('on');
        }
      }
      //fullscreen
      if (fs) {
        this.$elem.find('#fsBtn').removeClass('dsbl').addClass('on');
      }
      else {
        this.$elem.find('#fsBtn').removeClass('dsbl').removeClass('on');
      }
      //hidden
      if (vis) {
        this.$elem.find('#hiddenBtn').removeClass('dsbl').removeClass('on');
      }
      else {
        this.$elem.find('#hiddenBtn').removeClass('dsbl').addClass('on');
      }
    } else if (this.destTypeRx) {
      //keep only disconnect for Rx
      this.$elem.find('#delBtn').removeClass('dsbl');
    }
  },

  /**
  * _processWindowControlActions function
  * @param evt {JSON}
  * **/
  _processWindowControlActions: function (evt) {
    var btnId = evt.target.id;
    var selectOnConn = false;
    if (this.sessionData) {
      jQuery.each(this.sessionData, jQuery.proxy(function (key, sdata) {
        //check window is already connected
        if (sdata.PortConnectionInfo['@windowId'] == this.selDstWin && sdata.PortConnectionInfo['@dstNID'] == this.selDestNid) {
          selectOnConn = true;
          return false;
        }
      }, this));
    }
    //{ connContent: connContent, connId: connId, srcKbm: srcKbm, mute: mute, kbm: kbm, sc: sc, fs: fs, vis: vis };
    if (selectOnConn) {
      if (!this.setPortWinPropReqUrl) this.setPortWinPropReqUrl = this.options.protocol + '//' + this.options.ipAddress + '/controller/Resource/Port?NID=' + this.options.nid;
      var btnStatus = this._getBtnStatusByWinProps();
      this.log(bINFO, '_processWindowControlActions', 'setPortWindowPropertiesRequest', ' btnId:' + btnId);
      if (btnId == "delBtn") {
        var sessionId = this._getSessionByDestWinId(this.selDestNid, this.selDstWin);
        //Remove Source Title
        if (this.$elem.find('#srcTitle')) this.$elem.find('#srcTitle').empty();
        this._sendStopRequest(sessionId);
      } else if (btnId == "muteBtn") {
        if (btnStatus.mute) { //evt.currentTarget.className.indexOf('display-item-mute on') >= 0
          //current state is audio on
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@mute', 'false', null); // mute on
        } else {
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@mute', 'true', null); // unmute on
        }
      } else if (btnId == "kbmBtn") {
        if (btnStatus.kbm) {//'on' === evt.currentTarget.classList[2]
          //current state is kbm on
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@remoteKbmCtrl', 'false', null); //nokbm on
        } else {
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@remoteKbmCtrl', 'true', null); //kbm on
        }
      } else if (btnId == "1To1Btn") {
        if (btnStatus.sc == 'fitNativeSize') {//display-item-1to1 on/.test(evt.currentTarget.className)
          //current state is 1:1
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@scale', 'fitWindow', null); //no1to1 on
        } else {
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@scale', 'fitNativeSize', null); //1to1 on
        }
      } else if (btnId == "fsBtn") {
        if (btnStatus.fs) {
          //current state is fullscreen
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@maximize', 'false', null); //fullscreen off
        } else {
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@maximize', 'true', null); //fullscreen on
        }
      } else if (btnId == "hiddenBtn") {
        if (btnStatus.vis) {
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@visibility', 'false', null); //hide on
        } else {
          //current state is hidden
          this.options.barcoAPI.setPortWindowPropertiesRequest(this.setPortWinPropReqUrl, this.selDstWin, '@visibility', 'true', null); //hide off
        }
      }
    }
  },

  /**
  * _getBtnStatusByWinProps function
  * @return button status {JSON}
  * **/
  _getBtnStatusByWinProps: function () {
    this.log(bINFO, '_getBtnStatusByWinProps', 'call', ' ');
    var winBtnProps = null;
    if (this.displayWinArr && this.displayWinArr.length > 0) {
      for (i = 0; i < this.displayWinArr.length; i++) {
        var windowObj = this.displayWinArr[i];
        if (windowObj['@id'] == this.selDstWin) {
          //XP has no window Properties and ConnectionContent
          var winProperties = windowObj['Properties'];
          if (winProperties) {
            var connContent = windowObj['ConnectionContent'];
            var connId = connContent['@connId'];
            var srcKbm = connContent['@srcKbmCtrl'];
            var mute = winProperties['@mute'];
            var kbm = winProperties['@remoteKbmCtrl'];
            var sc = winProperties['@scale']; //'fitToWindow','fitNativeSize'
            var fs = winProperties['@maximize'];
            var vis = winProperties['@visibility'];
            this.log(bINFO, '_getBtnStatusByWinProps', 'call', ' properties, mute:' + mute + ' connId:' + connId + ' srcKbm:' + srcKbm + ' kbm:' + kbm + ' sc:' + sc + ' fs:' + fs + ' vis:' + vis);
            winBtnProps = { connContent: connContent, connId: connId, srcKbm: srcKbm, mute: mute, kbm: kbm, sc: sc, fs: fs, vis: vis };
            break;
          } else {
            this.log(bINFO, '_getBtnStatusByWinProps', 'call', 'one window - XP  ');
          }
        }
      }
    } else {
      this.log(bERROR, '_getBtnStatusByWinProps', 'call', ' no windows, displayWinArr is empty');
    }
    return winBtnProps;
  },

  /**
  * _sendStopRequest function
  * @param sessionId{String}
  * **/
  _sendStopRequest: function (sessionId) {
    jQuery.publish('onStopSessionRequest', sessionId);
    this._disableWindowControls();
  },

  /**
  * _enableWindowControls function
  *
  * **/
  _enableWindowControls: function () {
    if (this.displayWinArr instanceof Array) {
      for (i = 0; i < this.displayWinArr.length; i++) {
        var winId = this.displayWinArr[i]['@id'];
        this.log(bINFO, '_enableWindowControls', 'call', ' winId:' + winId);
        if (winId && winId == this.selDstWin) {
          this._updateWindowingControlByProps(this.displayWinArr[i]);
          break;
        }
        else {
          if ((this.destTypeRx) || (this.displayWinArr['@id'] == this.selDstWin)) {
            this._updateWindowingControlByProps(this.displayWinArr);
          }
        }
      }
    }
    else {
      this.log(bERROR, '_enableWindowControls', 'call', ' this.displayWinArr is empty');
    }
  },

  /**
  * _disableWindowControls function
  * **/
  _disableWindowControls: function () {
    this.$elem.find('#delBtn') && this.$elem.find('#delBtn').removeClass('on').addClass('dsbl');
    this.$elem.find('#muteBtn') && this.$elem.find('#muteBtn').removeClass('on').addClass('dsbl');
    this.$elem.find('#kbmBtn') && this.$elem.find('#kbmBtn').removeClass('on').addClass('dsbl');
    this.$elem.find('#1To1Btn') && this.$elem.find('#1To1Btn').removeClass('on').addClass('dsbl');
    this.$elem.find('#fsBtn') && this.$elem.find('#fsBtn').removeClass('on').addClass('dsbl');
    this.$elem.find('#hiddenBtn') && this.$elem.find('#hiddenBtn').removeClass('on').addClass('dsbl');
  },

  /**
  * updateDstOnConnState function  
  * Changes Window/Dest style based on connection state notification
  * Called by: _processPortSessionInfo, _portConnectionUpdateHandler & _portConnectionStateUpdateHandler
  * @portConnectionInfo {JSON}
  * @state {string}
  **/
  updateDstOnConnState: function (portConnectionInfo, state) {
    var dstId = portConnectionInfo['@dstNID'];
    var winId = portConnectionInfo['@windowId'];
    var dstWinDom = this.$elem.find('div[id="' + winId + '"]');
    if (dstWinDom.length > 0) {
      dstWinDom.addClass("display-window");
      var thumbnailSrc = ''; // this.blankTnSrc;
      if (state == "STREAMING" || state == "WAITING_FOR_STREAMSTART") {
        if (winId == this.selDstWin) {
          dstWinDom.addClass("display-window-selected");
        }
        thumbnailSrc = this.defaultThumbnail; // portConnectionInfo['@thumbnailURL'];
        this._enableWindowControls();
      } else if (state.indexOf('_ERROR') >= 0) {
        dstWinDom.addClass("error");
        thumbnailSrc = "./resource/images/icon_error.png";
      } else if (state == "None" && winId == this.selDstWin) { // STOP
        dstWinDom.removeClass("error");
        this._disableWindowControls();
      }

      if (dstWinDom.get(0).firstElementChild.src != thumbnailSrc) {
        dstWinDom.get(0).firstElementChild.src = thumbnailSrc;
        if (state == "STREAMING" || state == "WAITING_FOR_STREAMSTART") {
          //start tn timer for thumbnailSrc 
          //this._setDestTnUrlTimer(thumbnailSrc, dstId, winId, dstWinDom.get(0));
        }
        else if (state != "END_OF_MEDIA") {
          //stop dest thumbnail request timer by destNID and winId
          this._clearUpTnUrlTimer(dstId + '_' + winId);
          this.$elem.find(".display-window[id='" + winId + "'] ").removeClass("display-window-selected");
        }
      }
      this.log(bINFO, 'updateDstOnConnState', 'return', ' destId: ' + dstId + ', winId: ' + winId + ', state:' + state);
    }
  },

  /**
  * _setDestTnUrlTimer function
  * @tnUrl {String}: thumbnail URL
  * @dstId {String}
  * @winId {String}
  * @dstWinDom {DOM}
  * **/
  _setDestTnUrlTimer: function (tnUrl, nid, winId, dstWinDom) {
    var destId = nid + '_' + winId;
    if (destId in this.destTnTimerArr) {
      this._clearUpTnUrlTimer(destId);
    }
    var imgEl = dstWinDom.firstElementChild; //getElementById('#img_' + nid); 
    if (imgEl && tnUrl) {
      if (tnUrl.indexOf('mediajs') >= 0) {
        var timerId = setInterval(
        			function () {
        			  var suffix = new Date().getTime();
        			  imgEl.src = "";
        			  imgEl.src = tnUrl + '&suffixTimeId=' + suffix;
        			}, 4000);

        this.destTnTimerArr[destId] = timerId;
        this.log(bDEBUG, '_setDestTnUrlTimer', 'return', '  destId: ' + destId + ', timerId:' + timerId + ', tnUrl: ' + tnUrl);
      }
      else {
        imgEl.src = tnUrl;
        this.log(bDEBUG, '_setDestTnUrlTimer', 'return', '  setDestTnUrl with a static img: destId: ' + destId + ', tnUrl: ' + tnUrl);
      }
    }
  },

  /**
  * _clearUpTnUrlTimer function
  * @id {String}, nid_winId
  * **/
  _clearUpTnUrlTimer: function (id) {
    //clean thumbnail url timer
    if (this.destTnTimerArr instanceof Object) {
      if (typeof id !== 'undefined') {
        //clean single tn url timer by nid
        for (var j in this.destTnTimerArr) {
          if (j == id) {
            clearInterval(this.destTnTimerArr[j]);
            delete this.destTnTimerArr[j];
            this.log(bDEBUG, '_clearUpTnUrlTimer', 'return', ' timerId: ' + this.destTnTimerArr[j] + ',  id: ' + id);
            break;
          }
        };
      }
      else {
        //clean all tn url timer
        for (var i in this.destTnTimerArr) {
          clearInterval(this.destTnTimerArr[i]);
          delete this.destTnTimerArr[i];
          this.log(bDEBUG, '_setDestTnUrlTimer', 'return', ' clear all timerId: ' + this.destTnTimerArr[j] + ',  id: ' + i);
        };
      }
    }
  },

  /**
  * _getSessionByDestWinId function
  * @sessiondata {String}
  * @destNid {String}
  * @destWinId {DOM}
  * @return {sessionId|String}
  * **/
  _getSessionByDestWinId: function (destNid, destWinId) {
    var selSrcDstSessionId = null;
    for (var k in this.sessionData) {
      var sdata = this.sessionData[k];
      if (sdata.PortConnectionInfo['@dstNID'] == destNid && sdata.PortConnectionInfo['@windowId'] == destWinId) {
        //get sessionId from sessionData
        selSrcDstSessionId = sdata["@id"];
        break;
      }
    };
    return selSrcDstSessionId;
  },

  /**
  * updatePortSessionInfo function
  * @portSessionInfo {JSON}
  * @type {String|'DELETE'|'UPDATE'}
  * **/
  updatePortSessionInfo: function (portSessionInfo, type) {
    if (!portSessionInfo) {
      this.log(bERROR, 'updatePortSessionInfo', 'return', ' portSessionInfo node is empty ');
      return;
    }
    if (portSessionInfo.PortConnectionInfo && portSessionInfo.PortConnectionInfo['@srcTitle'] && type.toString().toUpperCase() == "UPDATE") {
      this.sourceTitle = portSessionInfo.PortConnectionInfo['@srcTitle'];
      if (this.$elem.find('#srcTitle')) this.$elem.find('#srcTitle').text(this.sourceTitle);
    }
    var sessionId = portSessionInfo['@id'];
    var portConnInfoNode = portSessionInfo.PortConnectionInfo;
    var connState = portConnInfoNode['@state'];
    if (type == 'DELETE') {
      delete this.sessionData[sessionId];
      if (this.selDstWin == portConnInfoNode['@windowId']) {
        this.selDstWin = '';
      }
      connState = "None";
    } else {
      this.sessionData[sessionId] = portSessionInfo;
    }

    this.updateDstOnConnState(portConnInfoNode, connState);
  }

});

// expose Objects as jQuery plugins
jQuery.plugin('edp_windowControlPanel', WindowControlPanel);
