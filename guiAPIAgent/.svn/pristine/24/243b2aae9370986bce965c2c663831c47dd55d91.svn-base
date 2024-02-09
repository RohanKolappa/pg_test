var SessionAPI = function(startedCB, actionCB, eventCB, errorCB) {
  this.cls = "SessionAPI";
  this.startedCB = startedCB;
  this.actionCB = actionCB;
  this.eventCB = eventCB;
  this.errorCB = errorCB;
  this.reset();
};

SessionAPI.prototype.MSG_SESSIONUPDATE = "SessionUpdate";
//SessionAPI.prototype.MSG_CONTROLLERUPDATE: "SessionControllerUpdate";
SessionAPI.prototype.MSG_SESSIONERROR = "SessionError";

//SessionAPI.prototype.MSG_ENTITYPRESENCEEVENT = "EntityPresence";
SessionAPI.prototype.MSG_SESSIONSTATEEVENT = "SessionState";
SessionAPI.prototype.MSG_CONNSTATEEVENT = "ConnState";
SessionAPI.prototype.MSG_TIMELINEEVENT = "Timeline";
	
SessionAPI.prototype.reset = function () {
	this.clientData = "";
	this.sessionId = "";
	this.reqestedSessionId = "";
	this.sessionURLPostBase = null;
	this.docSession = null;
	this.objCollabInfo = null;
	this.nextArgs = "";
	this.keepAliveURL = null;

	this.startTS = 0;
	this.sessionTimeout = 5000;
	
	this.defaults = {};
	this.defaults.timeScale = "1";
	this.defaults.skipBackOffset = "10000";	// TBD - string or int??
	this.defaults.skipFwdOffset = "10000";
	this.defaults.trimlength = "3600000";
};

SessionAPI.prototype.getSessionId = function () {
	return this.sessionId;
};
SessionAPI.prototype._clearSession = function () {
	SessionAgent.removeSessionFromList(this.clientData);
	this.reset();
};

SessionAPI.prototype.handleResponse = function (response) {
  //console.log("handleResponse requestName=" + response.requestName);
  // TBD - deal with sessions started with View/Playback v/s StartURL
  // - check / normalize the response object in those cases to work thru this logic as well
  if ((response.requestName == "View") || (response.requestName == "Playback")) {
    if (this.sessionId != "" && this.sessionId != response.sessionId) { // Clean out previous session
      //jQuery.log("SessionAPI:handleResponse " + response.requestName + "." + response.clientData + "Assert sessionId not empty handling StartResponse");
      this.doStop();
    }
    if (response.errorCode != 0) {
      //jQuery.log("SessionAPI:handleResponse " + response.requestName + "." + response.clientData + " error=" + response.errorCode + ":" + response.data);
      this.doStop();
      this.errorCB(response);
      return;
    }
    this.sessionId = response.sessionId;
    if (response.sessionURL) {
      if (!this.sessionURLPostBase) {
        this.sessionURLPostBase = this.sessionCtrlURL + '?' + 'sessionId=' + this.sessionId;
      }
      /*
       * The server doesn't not change the protocol of stream URLs to HTTPS
       * even when the sessionURL is HTTPS. Therefore, we need to do it here.
       */
      if (response.sessionURL.match(/^https:/i) && response.mediaURL.match(/^http:/i)) {
        response.mediaURL = response.mediaURL.replace(/^http:/i, "https:");
      }
    }
    if (!this.sessionURLPostBase) {
      this.sessionURLPostBase = this.sessionCtrlURL + '?' + 'sessionId=' + this.sessionId;
    }
    if (this.nextArgs != "") { // Next start request is ready
        //console.log("SessionAPI:handleResponse " + response.requestName + "." + response.clientData + " next controlURL ready");
        //this.sessionURL = response.sessionURL; //?? TBD - Why??
        this._doStart.apply(this, this.nextArgs); // Send out the next start
        return;
    }

    this._handleStartResponse(response.mediaURL, response.data);

    if (response.sessionURL) {
      this._doKeepAlive(response.sessionURL);
    }
    //console.log("SessionAPI:handleResponse " + response.requestName + ":" + response.clientData + ":" + response.duration + "/" + duration + ":" + response.mediaURL);
  }
  else if (response.requestName == "KeepAlive") {
    if (this.sessionId != response.sessionId) { // Clean out stale session
        this._stopStaleSessionInternal(response, "keepalive sessionId mismatch errorCode=" + response.errorCode + " sessionId=" + response.sessionId);
        return;
    }
    if (response.errorCode != 0) {
        //JQuery.log("Session:handleResponse KeepAlive" + ":" + response.clientData + " errorCode=" + response.errorCode + "data=" + response.data);
        this.doStop();
        this.errorCB(response);
        return;
    }
    //console.log("Session:handleResponse " + response.requestName + ":" + response.clientData + ": KeepAlive data:" + response.data);
    this._doKeepAlive(response.sessionURL);
    if (response.data == "{}") {
        //console.log("Session:handleResponse KeepAlive" + ":" + response.clientData + " no events");
        return;
    }
    this._handleNotificationResponse(response.data);
  }
  else {
    // all other session action response here
    this._handleActionResponse(response.data);
  }
};

SessionAPI.prototype._stopStaleSessionInternal = function (response, reason) {
  if (response.sessionURL === undefined) return;
  BarcoUtil.logger(bDEBUG, "", this.cls, "_stopStaleSessionInternal", "call",
    response.clientData + ":" + reason);
  SessionAgent.sendSessionRequest('doGETjsonp', response.sessionURL + "&requestName=StopSession", objData);
};

SessionAPI.prototype._handleStartResponse = function (streamURL, responseData) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "_handleStartResponse", "call",
	  streamURL + " response: " + responseData);
	var objResponse = JSON.parse(responseData);
	if(objResponse.Session) {
		this.docSession = new SessionInfo(objResponse);
		var objSessionCtrlInfo = this.docSession.getSessionCtrlInfo();
		this.startedCB(streamURL, objSessionCtrlInfo);
		//jQuery.publish(this.MSG_SESSIONUPDATE, objSessionCtrlInfo);
	}
	else {
		//JQuery.log("Session:handleStartResponse: Missing Session in Response" + responseData); 
		this.errorCB(responseData);
		//jQuery.publish(this.MSG_SESSIONERROR, responseData);
		return;
	}
};

SessionAPI.prototype._handleActionResponse = function (responseData) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "_handleActionResponse", "call",
	  responseData);
	var objResponse = JSON.parse(responseData);
	if(objResponse) {
	  this.docSession = new SessionInfo(objResponse);
	}
	else if(objResponse.ipvserror) {
		//console.log("Session:handleActionResponse: Error in Response" + responseData); 
		this.errorCB(responseData);
		//jQuery.publish(this.MSG_SESSIONERROR, responseData);
		return;
	}
	var objSessionCtrlInfo = this.docSession.getSessionCtrlInfo();
	this.actionCB(objSessionCtrlInfo);
	//jQuery.publish(this.MSG_SESSIONUPDATE, objSessionCtrlInfo);
};

SessionAPI.prototype._handleNotificationResponse = function (responseData) {
  var objResponse = JSON.parse(responseData);
	var nArray = null;
	// session keepalive based notifications - TBD ** Need to verify this format **
	if (objResponse.notificationList) {
		if ((objResponse.notificationList.x) instanceof Array) {
			nArray = objResponse.notificationList.x;
		}
		else {
			var nNode = objResponse.notificationList.x ? objResponse.notificationList.x : objResponse.notificationList;
			nArray = [];
			nArray.push(nNode);
		}
	}
	// login keepalive based notifications
	else if (objResponse.NotificationList) {
		if ((objResponse.NotificationList.Notification) instanceof Array) {
			nArray = objResponse.NotificationList.Notification
		}
		else {
			var nNode = objResponse.NotificationList.Notification ?
						objResponse.NotificationList.Notification : objResponse.NotificationList;
			nArray = [];
			nArray.push(nNode);
		}
	}
	else if(objResponse.ipvserror) {
		//JQuery.log("Session:handleNotificationResponse: Error in Response" + responseData); 
		this.errorCB(responseData);
		//jQuery.publish(this.MSG_SESSIONERROR, responseData);
		return;
	}
	if( nArray ) {
		var self = this;
		jQuery.each (nArray, function (key, nNode) {
		  for (propertyName in nNode) {
			if (propertyName == self.MSG_CONNSTATEEVENT) {
				self._handleConnStateEvent(nNode[propertyName]);
			}
			else if (propertyName == self.MSG_TIMELINEEVENT) {
				self._handleTimelineEvent(nNode[propertyName]);
			}
			else if (propertyName == self.MSG_SESSIONSTATEEVENT) {
			  self._handleSessionStateEvent(nNode[propertyName]);
			}
			//jQuery.publish(propertyName, nNode[propertyName]);
		  }
		});
	}
};
SessionAPI.prototype._handleConnStateEvent = function (objConnState) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "_handleConnStateEvent", "call",
	  "objConnState:" + JSON.stringify(objConnState));
	if (! this.docSession ) {
		this.errorCB(objConnState);
		return;
	}
  //When DMS rebooted state will be changed as WAITING_FOR_SOURCE-DEST_ACKNOWLEDGED
  //As there is no reliability(recovery) in SDK player need to stop the session and show the error msg
  //VNG0001-3167
	if (objConnState['@value'] == "WAITING_FOR_SOURCE-DEST_ACKNOWLEDGED") {
	  objConnState['@value'] = "SOURCE_ERROR";
	  objConnState['@errorCode'] = "0005";
	  objConnState['@errorData'] = "Source unavailable";
	}
	if (objConnState['@value'] == "REVOKED_XAUTH") {
	  objConnState['@value'] = "SOURCE_ERROR";
	  objConnState['@errorCode'] = "0006";
	  objConnState['@errorData'] = "Xauth Revoked";
	}
	if (objConnState['@value'] == "DENIED_XAUTH") {
	  objConnState['@value'] = "SOURCE_ERROR";
	  objConnState['@errorCode'] = "0007";
	  objConnState['@errorData'] = "Xauth Denied";
	}
	var connObj = this.docSession.getConnection(objConnState['@connId']);
	var streamURL = null;
	if (connObj && connObj["@state"] == "AUTHORIZING" && objConnState["@streamURL"])
	  streamURL = objConnState["@streamURL"];
	// update the conn state to the session doc
	if (! this.docSession.updateConnState(objConnState['@connId'], objConnState)) {
		// if not exist - eg: start / stop record
		//TBD - need to getSession - and do update event after that
		//this.doGetSession();
		this.errorCB(objConnState);	//temporary till TBD
		return;
	}
	var objSessionCtrlInfo = this.docSession.getSessionCtrlInfo();
	if (streamURL) {
	  this.startedCB(streamURL, objSessionCtrlInfo);
	}
	else {
	  this.eventCB(objSessionCtrlInfo);
	}
	//jQuery.publish(this.MSG_SESSIONUPDATE, this.docSession.getSessionCtrlInfo());
	//this.doEvent(SessionAgent.MSG_CONTROLLERUPDATE + '_' + objConnState.sessionId, {objCONNSTATE: objConnState });
};
SessionAPI.prototype._handleTimelineEvent = function (objTimeline) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "_handleTimelineEvent", "call",
	  "objTimeline:" + JSON.stringify(objTimeline));
	if (! this.docSession ) {
		this.errorCB(objTimeline);
		return;
	}
	// update the timeline to the session doc
	if (! this.docSession.updateTimeline(objTimeline['@id'], objTimeline)) {
		// if not exist - add
		this.docSession.addTimeline(objTimeline);
	}
	var objSessionCtrlInfo = this.docSession.getSessionCtrlInfo();
	this.eventCB(objSessionCtrlInfo);
	//jQuery.publish(this.MSG_SESSIONUPDATE, objSessionCtrlInfo);
	//this.doEvent(SessionAgent.MSG_CONTROLLERUPDATE + '_' + objTimeline.sessionId, { objTIMELINE: objTimeline });
};

SessionAPI.prototype._handleSessionStateEvent = function (objSessionState) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "_handleSessionStateEvent", "call",
    "objSessionState:" + JSON.stringify(objSessionState));
  //handle session DELETE
  if (objSessionState['@value'] != "DELETED") return;
  if (!this.docSession) {
    this._clearSession();
    this.errorCB(objSessionState);
    return;
  }
  var objSessionCtrlInfo = this.docSession.getSessionCtrlInfo();
  objSessionCtrlInfo.action = "Stop";
  objSessionCtrlInfo.state = "DELETED";
  if (this.reqestedSessionId == objSessionState['@sessionId']) {
    this._unSubscribeSessionEvents();
  }
  this._clearSession();
  this.eventCB(objSessionCtrlInfo);
};
	
SessionAPI.prototype.doView = function (reqParams) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doView", "call", "");
	//console.info("View: sessionId:" + reqParams.SessionId);
  if (!this.sessionCtrlURL) this.sessionCtrlURL = Request.baseURL + '/Session';
	var objData = {
		"View": {
			"@id": "", "@name": "", "@restartSession": "",      
			"@canvasId": "", "@windowId": "", "@sessionId": "", "@sessionName": "", 
			"@srcEntityId": "", "@dstEntityId": "", "@profileId": "",
			"Profile": {
				"@profileTitle": "", "@profileNID": "", "@id": ""
			},
			"GrpSrcPort": {
			  "@portGrpNID": "", "@portGrpTitle": "", "@pvrEnable": "", "@trimLength": "3600000", "@id": "",
        "Canvas": ""
			},
			"SrcPort": {
				"@portNID": "", "@portTitle": "", "@pvrEnable": "", "@trimLength": "3600000", "@id": ""
			},
			"DstPort": {
				"@portNID": "", "@portTitle": "", "@id": "", "@portIP": ""
			}
		}
	};
	objData.View['@canvasId'] = reqParams.canvasId ||"";
	objData.View['@windowId'] = reqParams.windowId ||"";
	objData.View['@sessionId'] = reqParams.sessionId ||"";
	objData.View['@dstEntityId'] = reqParams.dstEntityId ||"";
	objData.View['@restartSession'] = reqParams.restartSession || "";

	objData.View.GrpSrcPort['@portGrpNID'] = reqParams.portGrpNID || "";
	objData.View.GrpSrcPort['@portGrpTitle'] = reqParams.portGrpTitle || "";
	objData.View.GrpSrcPort['@pvrEnable'] = reqParams.grpPVREnable || "";
	objData.View.GrpSrcPort['@trimLength'] = reqParams.grpTrimLength || this.defaults.trimlength;

	objData.View.SrcPort['@portNID'] = reqParams.srcPortNID ||"";
	objData.View.SrcPort['@portTitle'] = reqParams.srcPortTitle ||"";
	objData.View.SrcPort['@pvrEnable'] = reqParams.pvrEnable ||"";
	objData.View.SrcPort['@trimLength'] = reqParams.trimLength || this.defaults.trimlength;

	objData.View.DstPort['@portNID'] = reqParams.dstPortNID ||"";
	objData.View.DstPort['@portTitle'] = reqParams.dstPortTitle || "";
	objData.View.DstPort['@portIP'] = reqParams.dstPortIP || "";

	this.reqestedSessionId = reqParams.sessionId || "";
	this._subscribeSessionEvents();
	this._doStart('doPOST', this.sessionCtrlURL, objData);
};
SessionAPI.prototype.doPlayback = function (reqParams) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doPlayback", "call", "");
	//console.info("Playback: sessionId:" + reqParams.SessionId);
  if (!this.sessionCtrlURL) this.sessionCtrlURL = Request.baseURL + '/Session';
	var objData = {
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
	objData.Playback['@windowId'] = reqParams.windowId ||"";
	objData.Playback['@sessionId'] = reqParams.sessionId ||"";
	objData.Playback['@restartSession'] = reqParams.restartSession || "";

	objData.Playback.GrpFile['@fileNID'] = reqParams.fileGrpNID || "";
	objData.Playback.GrpFile['@fileTitle'] = reqParams.fileGrpTitle || "";
	
	objData.Playback.SrcFile['@fileNID'] = reqParams.fileNID ||"";
	objData.Playback.SrcFile['@fileTitle'] = reqParams.fileTitle ||"";
	
	objData.Playback.DstPort['@portNID'] = reqParams.dstPortNID || "";
	objData.Playback.DstPort['@portTitle'] = reqParams.dstPortTitle || "";

	this.reqestedSessionId = reqParams.sessionId || "";
	this._subscribeSessionEvents();
	this._doStart('doPOST', this.sessionCtrlURL, objData);
};

SessionAPI.prototype._subscribeSessionEvents = function () {
  jQuery.subscribe(this.MSG_TIMELINEEVENT, jQuery.proxy(this._processSessionEvents, this));
  jQuery.subscribe(this.MSG_CONNSTATEEVENT, jQuery.proxy(this._processSessionEvents, this));
  jQuery.subscribe(this.MSG_SESSIONSTATEEVENT, jQuery.proxy(this._processSessionEvents, this));
};

SessionAPI.prototype._unSubscribeSessionEvents = function () {
  jQuery.unsubscribe(this.MSG_TIMELINEEVENT, jQuery.proxy(this._processSessionEvents, this));
  jQuery.unsubscribe(this.MSG_CONNSTATEEVENT, jQuery.proxy(this._processSessionEvents, this));
  jQuery.unsubscribe(this.MSG_SESSIONSTATEEVENT, jQuery.proxy(this._processSessionEvents, this));
};

SessionAPI.prototype._processSessionEvents = function (eventType, eventData) {
  if (eventType.type == this.MSG_TIMELINEEVENT) {
    this._handleTimelineEvent(eventData);
  }
  else if (eventType.type == this.MSG_CONNSTATEEVENT) {
    this._handleConnStateEvent(eventData);
  }
  else if (eventType.type == this.MSG_SESSIONSTATEEVENT) {
    this._handleSessionStateEvent(eventData);
  }
};

SessionAPI.prototype.doStartURL = function (sessionStartURL) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doStartURL", "call", sessionStartURL);
  var url = sessionStartURL;
  this.sessionCtrlURL = url.split('session?')[0] + "Session";
  var objData = {};
  //console.info("StartURL: sessionURL:" + url);
  this._doStart('doGETjsonp', url, objData);
};
SessionAPI.prototype.doStartSession = function (objSession) {
	var url =  Request.baseURL + '/Session';
	this.sessionCtrlURL = url;
	var objData = objSession || {"Session": {"@id": ""}};
	//console.info("StartSession: sessionURL:" + reqParams.sessionURL);
	this._doStart('doPOST', url, objData);
};

SessionAPI.prototype._doStart = function (reqMethod, url, objData) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "_doStart", "call", url);
   this.nextArgs = "";
	if(this.clientData != "" && this.reqestedSessionId == "") { // In Progress
	  var duration = (new Date()).getTime() - this.startTS;
		if (this.sessionId != "" && this.sessionURLPostBase != null) { // Session in progress
		  //console.log("SessionAPI:_doStart stopping current");
			this.doStop();
		}
		else if(duration > this.sessionTimeout) { // Start Request timeout
		  //console.log("SessionAPI:_doStart start timed out");
			this._clearSession();
		}
		else { // Start Request In Progress, have to wait until it returns
		  //console.log("SessionAPI:_doStart queuing nextControlURL");
			this.nextArgs = arguments;
			return; // Will be picked up when the Start Request returns
		}
	}
	this.clientData = SessionAgent.addSessionToList(this);
	if( reqMethod == 'doGETjsonp' ) {
		url += "&clientData=" + this.clientData;
	}
	// TBD - figure out how to send client data for View/Playback/SessionCtrlURL as well - so it will come back in response	
	this.startTS = (new Date()).getTime();    
	SessionAgent.sendSessionRequest(reqMethod, url, objData);
	//console.log("SessionAPI:_doStart starting " + this.clientData + " url=" + url);
};
SessionAPI.prototype._doKeepAlive = function (sessionURL) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "_doKeepAlive", "call", sessionURL);
  this.keepAliveURL = sessionURL;
  var objData = {};
  SessionAgent.sendSessionRequest('doGETjsonp', sessionURL + "&requestName=KeepAlive", objData);
};

SessionAPI.prototype.doStop = function (sessionId) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doStop", "call", sessionId);
//    var id = sessionId ? sessionId : this.sessionId;
//    var sessionInstance = null;

//    if (id) {
//        sessionInstance = SessionAgent.getSessionFromList(id);
//    }
//    else {
//        jQuery.log("Error: SessionAPI.doStop without any valid sessionId.");
//        return;
//    }
//    if (!sessionInstance) {
//        jQuery.log("Error: SessionAPI.doStop: there is not any valid session for sessionId: " + id);
//        return;
//    }
  if (!this.sessionCtrlURL) this.sessionCtrlURL = Request.baseURL + '/Session';
  if (sessionId) {
      BarcoUtil.logger(bDEBUG, "", this.cls, "doStop", "if sessionId",
        "sessionId:" + sessionId);
      var objData = { "StopSession": { "@sessionId": sessionId} };
      SessionAgent.sendSessionRequest('doPOST', this.sessionCtrlURL, objData);
  }
  else if(this.sessionId != "") {
      BarcoUtil.logger(bDEBUG, "", this.cls, "doStop", "if this.sessionId",
        "sessionId:" + this.sessionId);
      var objData = { "StopSession": { "@sessionId": this.sessionId} };
      SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
      //Generate fake SessionState notification for early cleanup
      this._handleSessionStateEvent({ "@value": "DELETED", "@sessionId": objData.StopSession['@sessionId'] });
  }
};
SessionAPI.prototype.doPause = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doPause", "call", "");
  if (this.sessionId != "") {
		//console.info("doPause: sessionId:" + this.sessionId);
		var objData = { "Pause": { "@sessionId": this.sessionId} };
		SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};

SessionAPI.prototype.getSessionPostBaseURL = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionPostBaseURL", "call", "");
  return this.sessionURLPostBase;
};

SessionAPI.prototype.doResume = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doResume", "call", "");
  if (this.sessionId != "") {
	  //console.info("doResume: sessionId:" + this.sessionId);
	  var objData = { "Resume": { "@sessionId": this.sessionId, //TBD - should this be 'Play'??
							"@timeScale": this.defaults.timeScale} };
    SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};
SessionAPI.prototype.doSkipBack = function (skipOffset) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doSkipBack", "call", skipOffset);
  if (this.sessionId != "") {
	  //console.info("doSkipBack: sessionId:" + this.sessionId);
	  var objData = { "SkipBack": { "@sessionId": this.sessionId, 
							"@timeScale": this.defaults.timeScale, 
							"@skipOffset": (skipOffset ? skipOffset : this.defaults.skipBackOffset)} };
    SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};
SessionAPI.prototype.doSkipFwd = function (skipOffset) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doSkipFwd", "call", skipOffset);
  if (this.sessionId != "") {
		//console.info("doSkipFwd: sessionId:" + this.sessionId);
		var objData = { "SkipFwd": { "@sessionId": this.sessionId, 
									"@timeScale": this.defaults.timeScale, 
									"@skipOffset": (skipOffset ? skipOffset : this.defaults.skipFwdOffset)} };
		SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};
SessionAPI.prototype.doCatchup = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doCatchup", "call", "");
  if (this.sessionId != "") {
		//console.info("doCatchup: sessionId:" + this.sessionId);
		var objData = { "Catchup": { "@sessionId": this.sessionId} };
		SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};
SessionAPI.prototype.doPlayFrom = function (mediaOffset) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doPlayFrom", "call", mediaOffset);
  if (this.sessionId != "") {
    //console.info("doSkipFwd: sessionId:" + this.sessionId);
    var objData = {
      "Play": {
        "@sessionId": this.sessionId,
        "@timeScale": this.defaults.timeScale,
        "@mediaOffset": (mediaOffset ? mediaOffset : 0)
      }
    };
    SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
  }
};
//TBD get args from session obj iso param
SessionAPI.prototype.doStartRecord = function (srcEntityId, srcTitle) {
  if (this.sessionId != "") {
    if (srcEntityId == null && this.docSession) srcEntityId = this.docSession.getSessionCtrlSrcEntityId();
    if (!srcEntityId) return false;
    if (srcTitle == null && this.docSession) srcTitle = this.docSession.getEntityNameById(srcEntityId);
    BarcoUtil.logger(bDEBUG, "", this.cls, "doStartRecord", "call", srcEntityId + ', ' + srcTitle);
    var objData = { "Record": { "@name": "",
      "DstFile": { "MetaData": "", "@fileTitle": srcTitle, "@id": "" },
      "@sessionId": this.sessionId, "@sessionName": "", "@id": "", "@resetTimeline": "true",
      "@srcEntityId": srcEntityId,
      "SrcPort": { "@portNID": "", "@portTitle": "", "@id": "" }
    }
    };
    //		SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
  Request.doPOST(this.sessionURLPostBase, objData, jQuery.proxy(function (response, status) {
      if (status == "success") {
        BarcoUtil.logger(bDEBUG, "doStartRecord", "", "doStartRecord Success fileTitle:" + srcTitle);
        if (response.responseJSON && response.responseJSON.Record) {
          response.requestName = "Record";
          response.data = JSON.stringify(response.responseJSON.Record);
          var clsSession = SessionAgent.getSessionFromListById(response.responseJSON.Record.Session['@id']);
          clsSession.handleResponse(response);
        }
      }
      else {
        var errDescription = ' ';
        if (response.responseJSON) {
          errDescription = response.responseJSON.Description;
        }
        else {
          var msg = response.responseText ? response.responseText.toString() : "";
          errDescription = msg.substring(msg.indexOf('<Description>'), msg.indexOf('</Description>'));
        }
        BarcoUtil.logger(bERROR, "doStartRecord", "", "doStartRecord Failed for fileTitle:" + srcTitle + ", error description: " + errDescription);
        var dataStr = JSON.stringify({ ipvserror: { description: errDescription} });
        var respVal = { requestName: 'Record', data: dataStr };
        this.errorCB(respVal);
      }
    }, this));
  }
};
//TBD get args from session obj iso param
SessionAPI.prototype.doStopRecord = function (connectionId) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doStopRecord", "call",
    connectionId);
  if (this.sessionId != "") {
    if (connectionId == null) {
      if (this.docSession) connectionId = this.docSession.getSessionCtrlRecordConnId();
    }
		var objData = {"DeleteConn": {                      
					"@sessionId": this.sessionId,
					"@connId": connectionId                    	
                  }};
		SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};
//TBD - fix this it probably needs to be a doGET with whereId in URL
SessionAPI.prototype.doGetSession = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doGetSession", "call", "");
    if (this.sessionId != "") {
		var objData = {"GetSession": {                      
					"@sessionId": this.sessionId               	
                  }};
		SessionAgent.sendSessionRequest('doPOST', this.sessionURLPostBase, objData);
	}
};
SessionAPI.prototype.getKeepAliveURL = function() {
  BarcoUtil.logger(bDEBUG, "", this.cls, "getKeepAliveURL", "call", "");
  return this.keepAliveURL;
};
SessionAPI.prototype.getSessionObj = function() {
  BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionObj", "call", "");
  if (this.docSession && this.docSession.objSession)
    return this.docSession.objSession;
  return null;
};
SessionAPI.prototype.setSessionObj = function (playerInfo) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "setSessionObj", "call", "");
  if (playerInfo.sessionInfo == null) return;
  this.clientData = SessionAgent.addSessionToList(this);
  // Need to replace clientData ref in old keep alive to match our new entry
  var sessionURL = playerInfo.keepAliveURL.replace(/&clientData=\d&/, "&clientData="+this.clientData+"&");
  // TBD - figure out how to send client data for View/Playback/SessionCtrlURL as well - so it will come back in response	
  this.startTS = (new Date()).getTime();
  this.sessionId = playerInfo.sessionInfo['@id'];
  this.sessionCtrlURL = sessionURL.split('session?')[0] + "Session";
  this.sessionURLPostBase = this.sessionCtrlURL + '?' + 'sessionId=' + this.sessionId;
  this.keepAliveURL = sessionURL;
  this.docSession = new SessionInfo({Session:playerInfo.sessionInfo});
  var objSessionCtrlInfo = this.docSession.getSessionCtrlInfo();
  this.startedCB(playerInfo.streamURL, objSessionCtrlInfo);
  this._doKeepAlive(sessionURL);
};
SessionAPI.prototype.leaveSession = function(sessionId) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "leaveSession", "call", "");
  var objData;
  if (sessionId) {
    objData = { "StopSession": { "@sessionId": sessionId} };
  } else if (this.sessionId != "") {
    objData = { "StopSession": { "@sessionId": this.sessionId} };
  }
  this._handleSessionStateEvent({ "@value": "DELETED", "@sessionId": objData.StopSession['@sessionId'] });
}
