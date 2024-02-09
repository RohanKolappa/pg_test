SessionAgent = function() {
  BarcoUtil.logger(bDEBUG, "", this.cls, "", "constructor", "");
}

SessionAgent.cls = "SessionAgent";
SessionAgent.sessionlist = new Array();
SessionAgent.id = 0;

SessionAgent.sendSessionRequest = function(reqMethod, url, objData) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "sendSessionRequest", "call",
	  "URL:" + url+ " data: " + JSON.stringify(objData));
	Request[reqMethod](url, objData, jQuery.proxy(function(response, status) {
		if( status != 0 ) {
			SessionAgent.handleSessionResponse(response);
		}
		else {
			//console.log("sendSessionRequest: Failed " + url+ " data: " + JSON.stringify(objData));
		}
	}, SessionAgent));
};

SessionAgent.handleSessionResponse = function(response) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "handleSessionResponse", "call",
	  "");
	//console.log("handleSessionResponse:" + response.requestName + "." + response.clientData);
	var clsSession = null;
	if(response.clientData) {
		clsSession = SessionAgent.getSessionFromList(response.clientData);
	}
  //Check View/Playback/RECORD requests which will not use startUrl
	else if (response.responseJSON) {
	  if (response.responseJSON.Record) {
	    response.requestName = "Record";
	    response.data = JSON.stringify(response.responseJSON.Record);
	    response.sessionId = response.responseJSON.Record.Session['@id'];
	    //clsSession = SessionAgent.getSessionFromListById(response.responseJSON.Record.Session['@id']);
	  }
	  else if (response.responseJSON.View) {
	    response.requestName = "View";
	    response.data = JSON.stringify(response.responseJSON.View);
	    response.sessionId = response.responseJSON.View.Session['@id'];
	    //clsSession = SessionAgent.getSessionFromListById(response.responseJSON.View.Session['@id']);
	  }
	  else if (response.responseJSON.Playback) {
	    response.requestName = "Playback";
	    response.data = JSON.stringify(response.responseJSON.Playback);
	    response.sessionId = response.responseJSON.Playback.Session['@id'];
	    //clsSession = SessionAgent.getSessionFromListById(response.responseJSON.Playback.Session['@id']);
	  }
	  if (response.sessionId) {
	    response.errorCode = (response.status == 200) ? 0 : response.status;
	    clsSession = SessionAgent.getSessionFromListById(response.sessionId);
	  }
	}
	//TBD figue out how to get client data in response for view/pause/ etc..
	//else find session by sessionId or something else in response
	if(clsSession != null) {
		clsSession.handleResponse.call(clsSession, response);
	}
	else { // Stop stale session
		if(response.requestName && 
			response.requestName != "StopSession" && response.requestName != "KeepAlive") {
			// TBD - add code to deal with View/Playback based stale session
			if(response.sessionURL ) {
				//console.log("StopI:" + response.clientData + ":" + "no matching session");
				var objData = { "StopSession": { "@sessionId": response.sessionId } };
				SessionAgent.sendSessionRequest('doPOST', response.sessionURL, objData);
			}
		}
	}
};

SessionAgent.addSessionToList = function(session) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "addSessionToList", "call",
	  "");
	SessionAgent.id++;
	SessionAgent.sessionlist.push(session);
	//console.debug("Length=" + SessionAgent.sessionlist.length);
	return SessionAgent.id;
};

SessionAgent._getSessionIndexInList = function(id) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "_getSessionIndexInList", "call",
	  id);
	for(i=0;i<SessionAgent.sessionlist.length;i++)
		if(SessionAgent.sessionlist[i].clientData == id)
			return i;
	return -1;
};
SessionAgent.getSessionFromList = function(id) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionFromList", "call",
	  id);
	index = SessionAgent._getSessionIndexInList(id);
	if(index == -1)
		return null;
	return SessionAgent.sessionlist[index];
};

SessionAgent.getSessionFromListById = function (sessionId) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionFromListById", "call",
    sessionId);
  for (i = 0; i < SessionAgent.sessionlist.length; i++) {
    if (SessionAgent.sessionlist[i].sessionId == sessionId || SessionAgent.sessionlist[i].reqestedSessionId == sessionId) {
      return SessionAgent.sessionlist[i];
    }
  }
  return null;
};

SessionAgent.removeSessionFromList = function(id) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "removeSessionFromList", "call",
	  id);
	index = SessionAgent._getSessionIndexInList(id);
	if(index == -1)
		return;
	SessionAgent.sessionlist.splice(index, 1);
	//console.debug("Length=" + SessionAgent.sessionlist.length);
};


