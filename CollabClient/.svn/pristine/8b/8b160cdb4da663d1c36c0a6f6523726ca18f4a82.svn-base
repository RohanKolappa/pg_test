/* ============================================================================
 * DestPanel Class
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */
var DestPanel = Class.extend({
  /**
  * _defaults options 
  */
  _defaults: {
  },

  /**
  * instance variables to manage session data, windowing control status
  */

  //plugin class instance
  $sourcePanelInst: null,
  $destListInst: null,
  $windowCtrlPanelInst: null,

  //layout variable
  //$windowCtrlLayoutPanel: null,
  $displayListPanel: null,

  //other instance variables
  viewState: false,

  /**
  * init function 
  */
  init: function (options, elem) {
    this.options = jQuery.extend({}, this._defaults, options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this.log(bINFO, "init", "call", "");
    this._build();
    this.$destListInst = this.$elem.find("#divDisplayList").edp_destList(jQuery.extend({}, this.options, { callBack: jQuery.proxy(this._onListItemSelected, this) })).data("edp_destList");
    this.$sourcePanelInst = this.$elem.find("#source-panel-container").edp_sourcePanel(jQuery.extend({}, this.options, { callBack: jQuery.proxy(this._onSrcSelected, this) })).data('edp_sourcePanel');
    //this.$windowCtrlLayoutPanel = this.$elem.find("#divWindowCtrlLayout");
    this.$displayListPanel = this.$elem.find("#divDestList"); //divDisplayList
    this._resetVariables();
  },

  /**
  * _resetVariables function 
  */
  _resetVariables: function () {
    this.selDestNid = '';
    this.selDestTitle = '';
    this.selDestFeatureType = '';
    this.selDestStreamType = '';
    this.selDstWinId = '';
    this.selSrcNid = '';
    this.selSrcTitle = '';
    this.selSrcType = '';
    this.sessionData = null;
  },

  /**
  * _build function 
  */
  _build: function () {
    var html =
    //    '<div id="source-panel-container" class="col left panel">' +
    //    '</div>' +
    //    '<div class="col center panel">' +
    //	  ' <div id="divDestList" style="width:100%; height:100%;">' +
    //    '   <div class="topic">Select a display to control</div>' +
    //    ' <div id="divDisplayList" class="row col display-list grid overflow-y" style="top:80px;"></div>' +
    //    '</div>' +
    //	  '<div id="divControllerRightPanel" style="width:100%; height:100%;" class="hidden"></div>' +
    //    '</div>';
    '<div class="col left panel">' +
	  ' <div id="divDestList" style="position:absolute; top:0px; bottom:360px; width:100%">' +
      '   <div class="toolbar-panel"><h5>Select a display to control</h5></div>' +
      '   <div id="divDisplayList" class="body-panel scroll-y display-list" style="top:40px;bottom:0px"></div>' +
      ' </div>' +
      ' <div id="divControllerRightPanel" style="position:absolute; width:100%; height:350px; bottom:10px; border-top:1px solid #485157;"></div>' +
    '</div>' +
    '<div id="source-panel-container" class="col center panel">' +
    '</div>';
    jQuery(html).appendTo(this.$elem);
  },

  /**
  * setPanel function 
  */
  setPanel: function () {
    this.log(bINFO, "setPanel", "call", "");
    if (!this.viewState) {
      //subscribe notifications from BarcoAPI
      jQuery.subscribe("PortConnectionUpdate", jQuery.proxy(this._portConnectionUpdateHandler, this));
      jQuery.subscribe("PortConnectionStateUpdate", jQuery.proxy(this._portConnectionStateUpdateHandler, this));
      //subscribe notifications within application
      jQuery.subscribe('onPortSessionInfoUpdate', jQuery.proxy(this._processPortSessionInfo, this));
      jQuery.subscribe('onPortUpdateDelete', jQuery.proxy(this._handleDestDelete, this));
      jQuery.subscribe('onStopSessionRequest', jQuery.proxy(this._handleStopSessionRequest, this));
      jQuery.subscribe('onCloseDestCtrlWindow', jQuery.proxy(this._closeDestCtrlWindow, this));
      this.viewState = true;
      this.sessionData = {};
      var whereClause = "^media-client$";
      if (this.options.userPermissions) {
        var strPrivateData = this.options.userPermissions.getPrivateData();
        if (!jQuery.isBlank(strPrivateData)) {
          var $privateDataNode = jQuery(jQuery.parseXML(strPrivateData));
          $privateDataNode.find("EndPointEntry").each(function (index, elem) {
            var $endPoint = jQuery(elem).attr('Title');
            if ($endPoint) whereClause = whereClause + ",^" + $endPoint + "$";
          });
        }
      }
      whereClause = "whereTextSubStr=" + encodeURIComponent(whereClause);
      this.$destListInst.setList(whereClause);
      this.$sourcePanelInst.setPanel();
    }
  },

  /**
  * resetPanel function 
  */
  resetPanel: function () {
    if (this.viewState) {
      jQuery.unsubscribe("PortConnectionUpdate", jQuery.proxy(this._portConnectionUpdateHandler, this));
      jQuery.unsubscribe("PortConnectionStateUpdate", jQuery.proxy(this._portConnectionStateUpdateHandler, this));
      jQuery.unsubscribe('onPortSessionInfoUpdate', jQuery.proxy(this._processPortSessionInfo, this));
      jQuery.unsubscribe('onPortUpdateDelete', jQuery.proxy(this._handleDestDelete, this));
      jQuery.unsubscribe('onStopSessionRequest', jQuery.proxy(this._handleStopSessionRequest, this));
      jQuery.unsubscribe('onCloseDestCtrlWindow', jQuery.proxy(this._closeDestCtrlWindow, this));
      if (this.$windowCtrlPanelInst) {
        this._closeDestCtrlWindow();
      }
      this.$destListInst.resetList();
      this.$sourcePanelInst.resetPanel();
      this.viewState = false;
      this._resetVariables();
    }
  },

  /**
  * destroy function 
  */
  destroy: function () {
    this.resetPanel();
    this.$destListInst.destroy();
    this.$sourcePanelInst.destroy();
    this.$elem.removeData('edp_destPanel');
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
  * _onListItemSelected function
  * call back from displayList
  * @param destNID {String}
  */
  _onListItemSelected: function (objInfo) {
    this.selDestNid = objInfo.id;
    this.selDestTitle = objInfo.title;
    this.selDestStreamType = objInfo.type;
    this.selDestFeatureType = objInfo.featureType;
    //#divDisplayController
    this.$elem.find('.display-item-card').remove();
    if (this.$windowCtrlPanelInst) this._closeDestCtrlWindow();
    this.$windowCtrlPanelInst = this.$elem.find('#divControllerRightPanel').edp_windowControlPanel({ barcoAPI: this.options.barcoAPI, ipAddress: this.options.ipAddress, protocol: this.options.protocol, sessionData: this.sessionData, nid: this.selDestNid, title: this.selDestTitle, streamType: this.selDestStreamType, featureType: this.selDestFeatureType, selectDestCallBack: jQuery.proxy(this._onDestSelected, this) }).data('edp_windowControlPanel');
    this.$windowCtrlPanelInst.setPanel();
    //this.$displayListPanel.hide();
    //this.$windowCtrlLayoutPanel.removeClass('hidden');
    this.$windowCtrlPanelInst.show();
    this.$sourcePanelInst.show();
  },

  /**
  * _closeDestCtrlWindow function
  */
  _closeDestCtrlWindow: function () {
    this.$windowCtrlPanelInst.destroy();
    this.$windowCtrlPanelInst = null;
    //this.$windowCtrlLayoutPanel.addClass('hidden');
    //this.$displayListPanel.show();
  },

  /**
  * _handleStopSessionRequest function
  * @param event {Event Obj}
  * @param data {String}, sessionID
  */
  _handleStopSessionRequest: function (event, data) {
    this.options.barcoAPI.stop(data);
  },

  /**
  * _onSrcSelected function:
  * get call back from both sourceList item click;
  * do valid check and send out corresponding request 
  * @param type {String|LIVE|RECORDING}
  * @param objInfo {JSON}
  */
  _onSrcSelected: function (type, objInfo) {
    //source selected
    this.selSrcNid = objInfo.id;
    this.selSrcTitle = objInfo.title;
    this.selSrcType = (type == "LIVE") ? "VIEW" : "PLAYBACK";
    if (this.$windowCtrlPanelInst) {
      //if a window selected 
      if (this.selDstWinId) this._startSession();
      this.log(bINFO, '_onSrcSelected', 'call', ' this.selDstWinId:' + this.selDstWinId + ', this.selSrcNid:' + this.selSrcNid);
    }
  },

  /**
  * _onDestSelected function:
  * get call back from destList item click;
  * do valid check and send out corresponding request 
  * @param objInfo {JSON}
  */
  _onDestSelected: function (winId) {
    this.selDstWinId = winId;
    if (!winId) return;
    //selected dest
    if (this.selSrcNid) {
      //source already selected
      this._startSession();
      this.log(bINFO, '_onDestSelected', 'call', ' this.selDstWinId:' + this.selDstWinId + ', selectedSrcNid:' + this.selSrcNid);
    }
  },

  /**
  * _startSession function: 
  * 
  */
  _startSession: function () {
    if (this.selSrcType == "VIEW") {
      this._handleSrcDestSelectionForView();
    } else {
      this._handleSrcDestSelectionForPlayback();
    }
  },

  /**
  * _handleSrcDestSelectionForView function, compose VIEW reqObj and respCb
  */
  _handleSrcDestSelectionForView: function () {
    var selSrcDstSessionId = null;
    var isSrcDstAlreadyConnected = false;
    for (var k in this.sessionData) {
      var sdata = this.sessionData[k];
      if (sdata.PortConnectionInfo['@dstNID'] == this.selDestNid && sdata.PortConnectionInfo['@windowId'] == this.selDstWinId) {
        //check if selected src and dst already connected
        if (sdata.PortConnectionInfo['@srcNID'] == this.selSrcNid) {
          isSrcDstAlreadyConnected = true;
          this.log(bINFO, "_handleSrcDestSelectionForView", "call", "selectedSrcNid=" + this.selSrcNid + "and selectedDestNid (" + this.selDestNid + "_" + this.selDstWinId + ") already connected.");
          break;
        }
        //keep the sessionId
        selSrcDstSessionId = sdata["@id"];
      }
    };
    this.log(bINFO, "_handleSrcDestSelectionForView", "enter", "selSrcTitle=" + this.selSrcTitle + ",  dstPortTitle= " + this.selDestTitle + ", and  this.selDstWinId = " + this.selDstWinId + ". ");
    if (!isSrcDstAlreadyConnected) {
      var defaultsTrimlength = "3600000";
      objData = {
        "View": {
          "@id": "", "@name": "", "@restartSession": "",
          "@canvasId": "", "@windowId": "", "@sessionId": "", "@sessionName": "",
          "@srcEntityId": "", "@dstEntityId": "", "@profileId": "",
          "Profile": {
            "@profileTitle": "", "@profileNID": "", "@id": ""
          },
          "SrcPort": {
            "@portNID": "", "@portTitle": "", "@pvrEnable": "", "@trimLength": "3600000", "@id": ""
          },
          "DstPort": {
            "@portNID": "", "@portTitle": "", "@id": "", "@portIP": ""
          }
        }
      };
      var sessionId = (selSrcDstSessionId) ? selSrcDstSessionId : this._getSessionIdByDestWinId(this.selDestNid, this.selDstWinId);
      objData.View.SrcPort['@portNID'] = this.selSrcNid;
      objData.View.SrcPort['@portTitle'] = this.selSrcTitle;
      objData.View['@windowId'] = this.selDstWinId;
      objData.View.DstPort['@portNID'] = this.selDestNid;
      objData.View.DstPort['@portTitle'] = this.selDestTitle;
      objData.View['@sessionId'] = sessionId;
      objData.View['@restartSession'] = true;
      objData.View.SrcPort['@pvrEnable'] = false;
      objData.View.SrcPort['@trimLength'] = defaultsTrimlength;
      this.log(bINFO, '_handleSrcDestSelectionForView', 'call', ' fileTitle: ' + this.selSrcTitle + ', dest:' + this.selDestTitle + ', sessionId:' + sessionId);
      Request['doPOST'](Request.baseURL + '/Session', objData, jQuery.proxy(function (xhr, status, data) {
        this._handleSessionRespCallback(xhr, status, data);
      }, this));
    }
    else {
      //deselect list 
      this.$destListInst.deselect();
      this.$sourcePanelInst.deselect();
      this.log(bINFO, "_handleSrcDestSelectionForView", "call", " already connected by selSrcTitle=" + this.selSrcTitle + ",  dstPortTitle= " + this.selDestTitle + ", and  windowId = " + this.selDstWinId + ". ");
    }
    this.selSrcNid = "";
    this.selSrcTitle = "";
    this.selDstWinId = "";
  },

  /**
  * _handleSrcDestSelectionForPlayback function, compose PLAYBACK reqObj and respCb
  */
  _handleSrcDestSelectionForPlayback: function () {
    var selSrcDstSessionId = null;
    var isSrcDstAlreadyConnected = false;
    for (var k in this.sessionData) {
      var sdata = this.sessionData[k];
      if (sdata.PortConnectionInfo['@dstNID'] == this.selDestNid && sdata.PortConnectionInfo['@windowId'] == this.selDstWinId) {
        //check if selected src and dst already connected
        if (sdata.PortConnectionInfo['@srcNID'] == this.selSrcNid) {
          isSrcDstAlreadyConnected = true;
          this.log(bINFO, "_handleSrcDestSelectionForPlayback", "call", "selectedSrcNid=" + this.selSrcNid + "and selectedDestNid (" + this.selDestNid + "_" + this.selDstWinId + ") already connected.");
          return false;
        }
        //keep the sessionId
        selSrcDstSessionId = sdata["@id"];
      }
    };

    if (!isSrcDstAlreadyConnected) {
      objData = {
        "Playback": {
          "@id": "", "@canvasId": "", "@windowId": "",
          "@sessionId": "", "@srcEntityId": "", "@dstEntityId": "", "@restartSession": "",
          "GrpFile": {
            "@fileNID": "", "@fileTitle": "", "@id": ""
          },
          "SrcFile": {
            "@fileNID": "", "@fileTitle": "", "@grpFileId": "", "@id": ""
          },
          "DstPort": {
            "@portNID": "", "@portTitle": "", "@id": ""
          }
        }
      };
      var sessionId = (selSrcDstSessionId) ? selSrcDstSessionId : this._getSessionIdByDestWinId(this.selDestNid, this.selDstWinId);
      objData.Playback['@windowId'] = this.selDstWinId;
      objData.Playback['@sessionId'] = sessionId;
      objData.Playback['@restartSession'] = true;
      objData.Playback.SrcFile['@fileNID'] = this.selSrcNid;
      objData.Playback.SrcFile['@fileTitle'] = this.selSrcTitle;
      objData.Playback.DstPort['@portNID'] = this.selDestNid;
      objData.Playback.DstPort['@portTitle'] = this.selDestTitle;
      this.log(bINFO, '_handleSrcDestSelectionForPlayback', 'call', ' fileTitle: ' + this.selSrcTitle + ', dest:' + this.selDestTitle + ', sessionId:' + sessionId);
      Request['doPOST'](Request.baseURL + '/Session', objData, jQuery.proxy(function (xhr, status, data) {
        this._handleSessionRespCallback(xhr, status, data);
      }, this));
    }
    else {
      this.log(bINFO, "_handleSrcDestSelectionForPlayback", "call", " already connected by selSrcTitle=" + this.selSrcTitle + ",  dstPortTitle= " + this.selDestTitle + ", and  windowId = " + this.selDstWinId + ". ");
      //deselect list 
      this.$destListInst.deselect();
      this.$sourcePanelInst.deselect();
    }
    this.selSrcNid = "";
    this.selSrcTitle = "";
    this.selDstWinId = "";
  },

  /**
  * _getSessionIdByDestWinId function:
  * compose a new  a sessionId by given destination NID and windowId
  * @param destNid {String}
  * @param winId {String}
  **/
  _getSessionIdByDestWinId: function (destNid, winId) {
    var sessioId = "Session" + destNid.substring(destNid.indexOf("_"));
    if (winId.indexOf("No_") != 0) {
      sessioId = sessioId + "_" + winId;
    }
    return sessioId;
  },
  /**
  * _handleSessionRespCallback function
  */
  _handleSessionRespCallback: function (response, status, data) {
    this.log(bINFO, "_handleSessionRespCallback", "response", "selSrcTitle=" + this.selSrcTitle + ",  dstPortTitle= " + this.selDestTitle + ", and  this.selDstWinId = " + this.selDstWinId + ". ");
    if (status == "success") {
      var connObj, sessionResp, respData = response.responseJSON;
      if (data.View) {
        sessionResp = data.View.Session;
        connObj = sessionResp.ConnectionList.Connection;
      } else if (data.Playback) {
        sessionResp = data.Playback.Session;
        connObj = sessionResp.ConnectionList.Connection;
      }
      this.$sourcePanelInst.deselect();

      var errMsg = null;
      if (connObj instanceof Array) {
        jQuery.each(connObj, function (connection) {
          if (connection['@state'].search("ERROR") >= 0) {
            errMsg = connection['@errorData'];
          }
          return false;
        });
      }
      else if (connObj['@state'].search("ERROR") >= 0) {
        //if only one connection return an object
        errMsg = connObj['@errorData'];
      }

      if (errMsg) {
        this.log(bERROR, "_handleSrcDestSelectionFor" + this.selSrcType, "", " session request error: " + errMsg);
      }
      this.log(bINFO, "_handleSessionRespCallback", "call", "selSrcTitle=" + this.selSrcTitle + ",  dstPortTitle= " + this.selDestTitle + ", and  windowId = " + this.selDstWinId + ". ");
      this._handleSessionResponse(sessionResp);
    }
    else {
      //fail
      var errMsg = response.responseJSON ? response.responseJSON.Description : response.statusText;
      this.log(bERROR, "_handleSrcDestSelectionFor" + this.selSrcType, "", "session request fail: " + errMsg);
    }

    //deselect dest list 
    this.$destListInst.deselect();
  },

  /**
  * _handleSessionResponse function
  * Handle session response from VIEW or PLAYBACK
  * Called by: _handleSrcDestSelectionForView or _handleSrcDestSelectionForPlayback
  * @param sessionInfo {JSON}
  **/
  _handleSessionResponse: function (sessionInfo) {
    //bug 321 fix:  response/notification sequence are different: 
    //4.2 sequency : [1] Session response (no thumbnail) [2]connection update notification (has valid thumbnail URL)[3]connection state update notification
    //4.3 sequency : [1] connection update notification (has valid thumbnail URL) [2] Session response [3]connection state update notification
    if (this.sessionData[sessionInfo['@id']]) return;
    var portSessionInfo = {
      "@id": "",
      "@name": "",
      "PortConnectionInfo": {
        "@canvasId": "",
        "@dstNID": "",
        "@dstTitle": "",
        "@errorCode": "",
        "@errorData": "",
        "@id": "",
        "@name": "",
        "@srcNID": "",
        "@srcTitle": "",
        "@state": "",
        "@type": "",
        "@windowId": ""
      }
    }
    portSessionInfo["@id"] = sessionInfo['@id'];
    portSessionInfo["@name"] = sessionInfo['@name'];
    portSessionInfo.PortConnectionInfo["@canvasId"] = sessionInfo.ConnectionList.Connection['@canvasId'];
    portSessionInfo.PortConnectionInfo["@id"] = sessionInfo.ConnectionList.Connection['@id'];
    portSessionInfo.PortConnectionInfo["@name"] = sessionInfo.ConnectionList.Connection['@name'];
    portSessionInfo.PortConnectionInfo["@state"] = sessionInfo.ConnectionList.Connection['@state'];
    portSessionInfo.PortConnectionInfo["@type"] = sessionInfo.ConnectionList.Connection['@type'];
    portSessionInfo.PortConnectionInfo["@windowId"] = sessionInfo.ConnectionList.Connection['@windowId'];
    var arrEntity = sessionInfo.EntityList.Entity;

    jQuery.each(arrEntity, function (index, entity) {
      if (entity['@id'] == sessionInfo.ConnectionList.Connection['@dstEntityId']) {
        portSessionInfo.PortConnectionInfo["@dstNID"] = entity.Port["@resourceNID"];
        portSessionInfo.PortConnectionInfo["@dstTitle"] = entity.Port["@resourceTitle"];
      }
      else if (entity['@id'] == sessionInfo.ConnectionList.Connection['@srcEntityId']) {
        var srcEntity = (entity["@type"] == "PlyFile") ? entity.File : entity.Port;
        portSessionInfo.PortConnectionInfo["@srcNID"] = srcEntity["@resourceNID"];
        portSessionInfo.PortConnectionInfo["@srcTitle"] = srcEntity["@resourceTitle"];
      }
    });
    this.sessionData[sessionInfo['@id']] = portSessionInfo;
    if (this.$windowCtrlPanelInst) {
      //update $windowCtrlPanelInst sessionData
      this.log(bINFO, "_handleSessionRespCallback", "call", "selSrcTitle=" + this.selSrcTitle + ",  dstPortTitle= " + this.selDestTitle + ", and  this.selDstWinId  = " + this.selDstWinId + ', PortConnectionInfo["@windowId"]' + portSessionInfo.PortConnectionInfo["@windowId"]);
      this.$windowCtrlPanelInst.updatePortSessionInfo(portSessionInfo, 'UPDATE');
    }
  },

  /**
  * _portConnectionUpdateHandler function
  * handler for "PortConnectionUpdate" notoification
  * @event {Event Object}
  * @notifyNode {JSON}, a port connection update notification Node
  * **/
  _portConnectionUpdateHandler: function (event, notifyNode) {
    this.log(bINFO, '_portConnectionUpdateHandler', 'call', ' notification ' + event.type);
    var portSessionInfo = notifyNode.Port.ConnectionStatus.PortSessionInfo;
    var portConnectionInfo = portSessionInfo.PortConnectionInfo;
    // In 4.3, there are multiple connectionUpdate notifications which same sessionId, same subsciptionId but the invalid one '@NID' is different to '@dstNID'. 
    if (portConnectionInfo['@dstNID'] == "" || portConnectionInfo['@dstNID'] == null || portConnectionInfo['@type'] == "PVR" || portConnectionInfo['@dstNID'] != notifyNode.Port['@NID']) return;
    var notifyType = "UPDATE";
    this.log(bINFO, '_portConnectionUpdateHandler', 'call', 'portConnectionUpdateHandler:: Session id:' + portSessionInfo['@id'] + ' State:' + portConnectionInfo['@state'] + ' Operation:' + notifyNode.Port['@operation']);

    if (notifyNode.Port['@operation'] == "Delete") { //'Add'
      if (this.sessionData[portSessionInfo['@id']]) {
        delete this.sessionData[portSessionInfo['@id']];
      }
      if (this.selDstWinId == portConnectionInfo['@windowId']) {
        this.selDstWinId = '';
      }
      notifyType = "DELETE";
    }
    else {
      this.sessionData[portSessionInfo['@id']] = portSessionInfo;
    }
    if (this.$windowCtrlPanelInst) {
      this.$windowCtrlPanelInst.updatePortSessionInfo(portSessionInfo, notifyType);
    }
  },

  /**
  * _portConnectionStateUpdateHandler function
  * handler for "PortConnectionStateUpdate" notoification
  * @event {Event Object}
  * @notifyNode {JSON}, a port connection state update notification Node
  * **/
  _portConnectionStateUpdateHandler: function (event, notifyNode) {
    var portNode = notifyNode.Port,
      	portNID = portNode['@NID'],
      	connStateNode = portNode.ConnectionState,
      	sessionId = connStateNode['@sessionId'],
      	value = connStateNode['@value'],
        connStatusNode = portNode.ConnectionStatus;
    this.log(bINFO, '_portConnectionStateUpdateHandler', 'call', ' notification ' + event.type + ', Session id:' + sessionId + ' State:' + value);
    var sessionData = this.sessionData[sessionId];
    if (sessionData) {
      sessionData.PortConnectionInfo['@state'] = value;
      if (this.$windowCtrlPanelInst) {
        //sessionData is connStatusNode.PortSessionInfo
        this.$windowCtrlPanelInst.updatePortSessionInfo(sessionData, "UPDATE");
      }
    }
  },

  /**
  * _handleDestDelete function
  * Called by: portUpdateHandler
  * @event {JSON}
  * @nid {String} 
  * **/
  _handleDestDelete: function (event, nid) {
    if (this.$windowCtrlPanelInst && nid === this.selDestNid) {
      this._closeDestCtrlWindow();
    }
  },

  /**
  * _processPortSessionInfo function 
  * update sessionDate based on portSessionInfo
  * @event {JSON}
  * @portSessionInfo {JSON}
  * **/
  _processPortSessionInfo: function (event, portSessionInfo) {
    var portConnectionInfo = portSessionInfo.PortConnectionInfo;
    if (portConnectionInfo['@dstNID'] == "" || portConnectionInfo['@dstNID'] == null || portConnectionInfo['@type'] == "PVR") return;
    var connState = portConnectionInfo['@state'];
    this.sessionData[portSessionInfo['@id']] = portSessionInfo;
    this.log(bINFO, '_processPortSessionInfo', 'call', ' Session id:' + portSessionInfo['@id'] + ' State:' + portConnectionInfo['@state']);

    if (this.$windowCtrlPanelInst && this.selDestNid == portConnectionInfo['@dstNID']) {
      this.$windowCtrlPanelInst.updatePortSessionInfo(portSessionInfo, "UPDATE");
    }
  }

});
// expose Objects as jQuery plugins
jQuery.plugin('edp_destPanel', DestPanel);