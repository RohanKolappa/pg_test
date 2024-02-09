
var sessionList = new Array();
var id = 0;
var sessionTimeout = 5000;

function sendRequestInternal(requestURL) {
	// alert("sendRequestInternal:" + requestURL);
	requestURL += '&callback=?';	
	$.getJSON(requestURL, function(response) {
		handleSessionResponseInternal(response);
	});
};

function stopStaleSessionInternal(response, reason) {
	if(response.sessionURL == undefined)
		return;
	log("StopI:" + response.clientData + ":" + reason);
	var requestURL = response.sessionURL + "&requestName=StopSession";
	sendRequestInternal(requestURL);
};


function handleSessionResponseInternal(response) {
	log("response:" + response.requestName + "." + response.clientData);
	session = getSessionInternal(response.clientData);
	if(session != null)
		session.handleResponse(response);
	else { // Stop stale session
		if(response.requestName != "StopSession") {
			stopStaleSessionInternal(response, "No matching session");
		}
	}
};

function log(logdata) {
	if(window.console && window.console.log)
		console.log(logdata);
};

function addSessionInternal(session) {
	id++;
	sessionList.push(session);
	// alert("Length=" + sessionList.length);
	return id;
};

function getSessionIndexInternal(id) {
	for(i=0;i<sessionList.length;i++)
		if(sessionList[i].clientData == id)
			return i;
	return -1;
};

function getSessionInternal(id) {
	index = getSessionIndexInternal(id);
	if(index == -1)
		return null;
	return sessionList[index];
};

function removeSessionInternal(id) {
	index = getSessionIndexInternal(id);
	if(index == -1)
		return;
	sessionList.splice(index, 1);
	// alert("Length=" + sessionList.length);
};

var Session = function(startedCB, responseCB, eventCB, errorCB) {
    this.startedCB = startedCB;
    this.responseCB = responseCB;
    this.eventCB = eventCB;
    this.errorCB = errorCB;
    this.reset();
};

Session.prototype.reset = function() {
	this.clientData = "";
	this.sessionId = "";
	this.sessionURL = "";
	this.nextControlURL = "";
    this.startTS = 0;
};

Session.prototype.sendRequestOld = function(request) {
	// alert("handleSendRequest:" + request);
	request += '&'
	request += new Date().getTime().toString()
	var response = request;
	var script = document.createElement('script');
    script.setAttribute('src', request);
    document.getElementsByTagName('head')[0].appendChild(script); 	
};

Session.prototype.sendRequest = function(requestURL) {
	sendRequestInternal(requestURL);
};

//11/2/2012
Session.prototype.handleResponse = function(response) {
	// alert("handleResponse requestName=" + response.requestName);
	// console.log("handleResponse requestName=" + response.requestName);
	if((response.requestName == "View") || (response.requestName == "Playback")) {
		if(this.sessionId != "") { // Clean out previous session
			log("Session:handleResponse " + response.requestName + "." + response.clientData + "Assert sessionId not empty handling StartResponse");
			this.doStop();
		}
		if(response.errorCode != 0) {
			log("Session:handleResponse " + response.requestName + "." + response.clientData + " error=" + response.errorCode + ":" + response.data);
			this.doStop();
			this.errorCB(response);
			return;
		}
		this.sessionId = response.sessionId;
		if(this.nextControlURL != "") { // Next start request is ready
			log("Session:handleResponse " + response.requestName + "." + response.clientData + " next controlURL ready");
			this.sessionURL = response.sessionURL;
			this.doStart(this.nextControlURL); // Send out the next start
			return;
		}
		
		var duration = (new Date()).getTime() - this.startTS;
		this.startedCB(response);//!!
		this.doKeepAlive(response.sessionURL);
		log("Session:handleResponse " + response.requestName + ":" + response.clientData + ":" + response.duration + "/" + duration + ":" + response.mediaURL);
	}
	else if(response.requestName == "KeepAlive") {
		if(this.sessionId != response.sessionId) { // Clean out stale session
			stopStaleSessionInternal(response, "keepalive sessionId mismatch errorCode=" + response.errorCode + " sessionId=" + response.sessionId);
			return;		
		}
		if(response.errorCode != 0) {
			log("Session:handleResponse KeepAlive" + ":" + response.clientData + " errorCode=" + response.errorCode + "data=" + response.data);
			this.doStop();
			this.errorCB(response);
			return;
		}
		log("Session:handleResponse " + response.requestName + ":" + response.clientData + ": KeepAlive data:" + response.data);
		this.doKeepAlive(response.sessionURL);
		if(response.data == "") {
			// log("Session:handleResponse KeepAlive" + ":" + response.clientData + " no events");
			return;
		}
		this.eventCB(response);
	}
	else {
		this.responseCB(response);//!!
	}
};

Session.prototype.doKeepAlive = function(sessionURL) {
	log("Session:doKeepAlive:" + this.clientData);
	this.sessionURL = sessionURL;
	this.sendRequest(this.sessionURL + "&requestName=KeepAlive");
};

Session.prototype.clearSession = function() {
	removeSessionInternal(this.clientData);
	this.reset();
};


Session.prototype.getSessionId = function() {
	return this.sessionId;
};

Session.prototype.doStart = function(controlURL) {
	if(this.clientData != "") { // In Progress
		var duration = (new Date()).getTime() - this.startTS;
		if(this.sessionId != "") { // Session in progress
			log("Session:doStart stopping current");
			this.doStop();
		}
		else if(duration > sessionTimeout) { // Start Request timeout
			log("Session:doStart start timed out");
			this.clearSession();
		}
		else { // Start Request In Progress, have to wait until it returns
			log("Session:doStart queuing nextControlURL");
			this.nextControlURL = controlURL;
			return; // Will be picked up when the Start Request returns
		}
	}
	this.clientData = addSessionInternal(this);
	this.startTS = (new Date()).getTime();
	this.sendRequest(controlURL + "&clientData=" + this.clientData);	
	log("Session:doStart starting " + this.clientData + " url=" + controlURL);
};

Session.prototype.doStop = function() {
	if(this.sessionURL != "") {
		log("Stop:" + this.clientData);
		var request = this.sessionURL + "&requestName=StopSession";
		this.sendRequest(request);
	}
	this.clearSession();	
};

Session.prototype.sendSessionRequest = function(request, requestParam) {
	if(this.sessionURL == "")
		return;
	request = this.sessionURL + "&requestName=" + request + "&" + request + ".sessionId=" + this.sessionId + "&" + requestParam;
	//alert("sendSessionRequest:" + request);
	this.sendRequest(request);
};