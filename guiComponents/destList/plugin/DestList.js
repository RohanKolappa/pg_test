/* ============================================================================
 * Destination List Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var DestList = Class.extend({
  _defaults: {

  },

  /**
  * instance variables 
  */
  viewState: false,
  destinationList: null,
  subscriptionId: 'Dest_' + new Date().getTime(),

  /**
  * init function 
  */
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.whereClause = "";
    this.log(bDEBUG, "init", "success", "");
  },

  /**
  * destroy function 
  */
  destroy: function () {
    this.$elem.find(".display-list-item").off('click', jQuery.proxy(this._onSelect, this));
    this.resetList();
    this.$elem.removeData('edp_destList');
  },

  /**
  * setList function 
  */
  setList: function (whereClause) {
    if (!this.viewState) {
      jQuery.subscribe("PortUpdate", jQuery.proxy(this._portUpdateHandler, this));
      this.viewState = true;
      this.whereClause = whereClause;
      this.getDestinationList();
    }
  },

  /**
  * resetList function 
  */
  resetList: function () {
    this.viewState = false;
    this.destinationList = null;
    this.whereClause = "";
    jQuery.unsubscribe("PortUpdate", jQuery.proxy(this._portUpdateHandler, this));
    this.$elem.find(".display-list-item").remove();
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

  /**
  * getDestinationList: send getPort(Dst) request 
  */
  getDestinationList: function () {
    this.log(bDEBUG, "getDestinationList", "call", "");
    var urlStr = this.options.protocol + '//' + this.options.ipAddress + '/controller/Resource/Port?type=DstPort&subscriptionID=' + this.subscriptionId + '&subscriptionScope=ALLNODES&whereState=Ready&selectFormat=BASICCONNECTIONINFO&wherePermissions=STREAMTO&whereOperationMode=Remote';
    if (this.whereClause) urlStr = urlStr + "&" + this.whereClause;
    this.options.barcoAPI.getPortList(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, "getDestinationList", "success", "getPortList type DestPort success");
        this.$elem.find(".display-list-item").remove();
        this._onResponseSuccess(data);
      } else {
        this.log(bERROR, "getDestinationList", "fail", "getPortList type DestPort failed");
      }
    }, this));
  },

  /**
  * _onResponseSuccess function
  * @param responseObj {JSON}
  */
  _onResponseSuccess: function (responseObj) {
    if (responseObj.Resource && responseObj.Resource.Port) {
      var portList = responseObj.Resource.Port;
      if (!((portList) instanceof Array)) {
        portList = new Array(portList);
      }
      //fix problem of getPort(dest) with &whereOperationMode=Remote return media client as well
      this.destinationList = $.grep(portList, function (port, index) {
        var featureList = port.FeatureList;
        var returnFlag = false;
        if (featureList.Feature) {
          for (var i = 0; i < featureList.Feature.length; i++) {
            var obj = featureList.Feature[i];
            if (obj['@name'] == "IPVSClientDisplayConfig" || obj['@name'] == "XP200DisplayConfig") {
              returnFlag = true;
              break;
            }
          }
        }
        return returnFlag;
      });

      this.displayList(this.destinationList);

      for (var i = 0; i < this.destinationList.length; i++) {
        var portNode = this.destinationList[i];
        if (portNode.ConnectionStatus) {
          //process PortInfo which may have ConnectionStatus & PortSessionInfo
          var portSessionInfo = portNode.ConnectionStatus.PortSessionInfo;

          if (!((portSessionInfo) instanceof Array)) {
            portSessionInfo = new Array(portSessionInfo);
          }
          this.log(bINFO, "getDestinationList._onResponseSuccess", "call", "  portSessionInfo.length:" + portSessionInfo.length);
          for (var j = 0; j < portSessionInfo.length; j++) {
            jQuery.publish('onPortSessionInfoUpdate', portSessionInfo[j]);
          }
        }
      }
    }
  },

  /**
  * displayList function
  * @param responseObj {JSON}
  */
  displayList: function (responseObj) {
    this.$elem.find(".source-list-row").remove();
    jQuery.each(responseObj, jQuery.proxy(function (key, obj) {
      jQuery('<div id="displayItem_' + obj['@NID'] + '" class="display-list-item" style="height:50px;">' +
//					  ' <div id="displaylayout_' + obj['@NID'] + '" class="display-list-item-layout">' +
//					  '	  <div id="displayWindow_' + obj['@NID'] + '" class="display-window" style="width:100%; height:100%"></div>' +
//					  ' </div>' +
					  ' <div id="portTitle' + obj['@NID'] + '" class="display-list-item-title"><i class="fa fa-desktop" style="font-size:20px;margin-right:10px;"></i>' + obj['@title'] + '</div>	' +
					  '</div>').appendTo(this.$elem);
    }, this));
    this.log(bINFO, "displayList", "success", "destinationList is ready");
    this.$elem.find(".display-list-item").on('click', jQuery.proxy(this._onSelect, this));
  },

  /**
  * _portUpdateHandler function
  * handler for "PortUpdate" notoification
  * @event {Event Object}
  * @notifyNode {JSON}
  * **/
  _portUpdateHandler: function (event, notifyNode) {
    //call _portAddOperation in DestPanel 
    var portNode = notifyNode.Port,
    //basic common part for all ports
        nid = portNode['@NID'],
        portType = portNode['@portType'],
        presence = portNode['@presence'],
        operation = portNode['@operation'],
        title = portNode['@title'],
        state = portNode['@state'];
    this.log(bINFO, 'displayList', 'success', 'portUpdateHandler notification: presence:' + presence + ', operation:' + operation + ', state:' + state + ', portType:' + portType + ', title:' + title + ', nid:' + nid);
    if (portType == "StreamDst" && presence == "unavailable" && state == "Offline") {
      jQuery.publish('onPortUpdateDelete', nid);
    } else if (portType == "StreamSrc" && operation == "PortTypeChange") {
      //change type, presence "ready"
      jQuery.publish('onPortUpdateDelete', nid);
    }
    this.getDestinationList();
  },

  /**
  * _onSelect function
  * @param evt {Object}
  */
  _onSelect: function (evt) {
    var itemId = evt.currentTarget.id;
    var title = evt.currentTarget.innerText ? evt.currentTarget.innerText :  evt.currentTarget.textContent;
    var destNID = itemId.substring(itemId.indexOf("_") + 1);
    this.$elem.find(".display-list-item").removeClass("selected");
    jQuery(evt.currentTarget).addClass("selected");
    var destType = '', featureType = '';
    jQuery.each(this.destinationList, jQuery.proxy(function (key, obj) {
      if (obj['@NID'] == destNID) {
        destType = obj['@streamType'];
        featureType = obj.FeatureList.Feature[0]['@name']
      }
    }, this));
    if (this.options.callBack) this.options.callBack({ id: destNID, title: title, type: destType, featureType: featureType });
  },

  /**
  * deselect function 
  */
  deselect: function () {
    this.$elem.find(".display-list-item").removeClass("selected");
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('edp_destList', DestList);
