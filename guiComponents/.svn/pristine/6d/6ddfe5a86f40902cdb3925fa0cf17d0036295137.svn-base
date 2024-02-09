/* ============================================================================
 * Device List Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var DeviceList = Class.extend({
  _defaults: {
  },

  viewState: false,
  deviceList: null,

  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.subscriptionID = "Device_" + new Date().getTime();
    this._build();
    this.log(bDEBUG, "init", "success", "");
  },

  /**
  * _build function 
  */
  _build: function () {
    var html =
      '<table>' +
      '  <thead>' +
      '   <tr class="toolbar-panel">' +
      '     <td class="title-cell" title="">Name</td><td class="option-cell"><a href="#" class="icon-sortDown" title="View Option"></a></td>' +
//      '     <td class="type-cell" title="">Type</td>' +
      '     <td class="ip-cell" title="">IP Address</td>' +
      '     <td class="actions-cell" title="">Actions</td>' +
      '   </tr>' +
      '  </thead>' +
      '  <tbody id="tbodyId">' +
      '  </tbody>' +
      '</table>';
    jQuery(html).appendTo(this.$elem);
    this.$tbody = this.$elem.find('#tbodyId');
  },

  destroy: function () {
    this.resetList();
    this.$elem.removeData('edp_deviceList');
  },

  setList: function () {
    if (!this.viewState) {
      this.viewState = true;
      jQuery.subscribe("PortUpdate", jQuery.proxy(this._portUpdateHandler, this));
      this.getDeviceList();
    }
  },

  resetList: function () {
    this.viewState = false;
    jQuery.unsubscribe("PortUpdate", this._portUpdateHandler);
    this.deviceList = null;
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
  * getDeviceList function 
  */
  getDeviceList: function () {
    if (!this.viewState) return;
    this.log(bDEBUG, "getDeviceList", "call", "");
    //    var urlStr = this.options.protocol + "//" + this.options.ipAddress + "/controller/Resource/Device?sortBy=title&sortOrder=ascending&whereType=RTP-Tx-Device&selectFormat=BASIC&subscriptionID=" + this.subscriptionID + "&subscriptionScope=ALLNODES";
    var urlStr = this.options.protocol + '//' + this.options.ipAddress + '/controller/Resource/Port?type=SrcPort&sortOrder=ascending&selectFormat=BASICCONNECTIONINFO&whereStreamType=RTP&whereState=Unmanaged';
    this.options.barcoAPI.getPortList(urlStr, jQuery.proxy(function (response, data) {
      if (response == "success") {
        this.log(bINFO, "getDeviceList", "success", "getPortList type RTP-Tx-Device Unmanaged success");
        this._onGetDeviceListSuccess(data);
      } else {
        this.log(bERROR, "getDeviceList", "fail", "getPortList type RTP-Tx-Device Unmanaged failed");
      }
    }, this));
  },

  /**
  * _onGetDeviceListSuccess function 
  */
  _onGetDeviceListSuccess: function (responseObj) {
    var deviceListObj = {};
    //    if (responseObj.Resource && responseObj.Resource.Device) {
    if (responseObj.Resource && responseObj.Resource.Port) {
      var deviceList = responseObj.Resource.Port;
      if (!((deviceList) instanceof Array)) {
        deviceList = new Array(deviceList);
      }

      for (var i = 0; i < deviceList.length; i++) {
        var port = deviceList[i];
        deviceListObj[port['@NID']] = { id: port['@NID'], title: port['@title'], type: port.Asset['@deviceType'], deviceNID: port.Asset['@NID'], pvrenable: port.PVRInfo['@enable'], trimlength: port.PVRInfo['@trimLength'], SDP: port.InstanceList.Instance['@streamURL'], SDP: port.InstanceList.Instance['@streamURL'], description: port.Description };
      }
    }
    this.displayList(deviceListObj);
  },

  /**
  * displayList function 
  */
  displayList: function (responseObj) {
    this.$tbody.empty();
    this.deviceList = responseObj;
    jQuery.each(responseObj, jQuery.proxy(function (key, obj) {
      var rowClass = "display-normal";
      jQuery('<tr class="">' +
             ' <td class="title-cell" title="">' + obj.title + '</td><td class="option-cell"><a href="#" class="icon-sortDown" title="View Option"></a></td>' +
//             ' <td class="type-cell" title="">' + obj.type + '</td>' +
             ' <td class="ip-cell" title="">' + this._formatDescription(obj.description) + '</td>' +
             ' <td class="actions-cell" title=""><a href="#"><i id=edit_' + obj.id + ' class="fa fa-pencil"></i></a> <a href="#" title="Delete device"><i id=delete_' + obj.id + ' class="fa fa-times" style="margin-left:20px;"></i></a></td>' +
          '</tr>').appendTo(this.$tbody);
    }, this));
    this.log(bINFO, "displayList", "success", "deviceList is ready");
    this.$elem.find('.fa-pencil').on('click', jQuery.proxy(this._onClickDeviceAction, this));
    this.$elem.find('.fa-times').on('click', jQuery.proxy(this._onClickDeviceAction, this));
  },

  /**
  * _onClickDeviceAction function 
  */
  _onClickDeviceAction: function (evt) {
    var devicePortInfoObj = null, action='';
    var itemId = evt.currentTarget.id;
    if(itemId.indexOf('edit')!=-1){
        itemId = itemId.replace(/edit_/,'');
        action = 'EDIT';
    } else {
        itemId = itemId.replace(/delete_/,'');
        action = 'DELETE';
    }
    for (var key in this.deviceList) {
      if (key == itemId) {
        devicePortInfoObj = this.deviceList[key];
        devicePortInfoObj.action = action;
        break;
      }
    }
    this.options.callBack(devicePortInfoObj);
  },

  /**
  * _portUpdateHandler function 
  */
  _portUpdateHandler: function (event, notifyNode) {
    var portNode = notifyNode.Port;
    if (portNode && portNode['@portType'] == "StreamSrc" && portNode['@streamType'] == "RTP") {
      this.getDeviceList();
    }
  },
  _formatDescription: function (description) {
    var ipV4Pat = /^((([01]?[0-9]{1,2})|(2[0-4][0-9])|(25[0-5]))[.]){3}(([0-1]?[0-9]{1,2})|(2[0-4][0-9])|(25[0-5]))$/;
    var ipMatch = description.match(ipV4Pat);
    if (ipMatch != null)
      return '<a href="http://' + description + '/" target="_blank" class="source-ip-addr">' + description + '</a>';
    else
      return description;
  }
});

// expose Objects as jQuery plugins
jQuery.plugin('edp_deviceList', DeviceList);
