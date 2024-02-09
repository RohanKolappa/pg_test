/*
* Copyright (c) Barco Inc.
* Use, modification, and distribution subject to terms of license.
*/
/**
* Request class provides async methods for making requests.
*/
Request = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "", "constructor", "");
};

Request.cls = "Request";
Request.httpHeader = null;

/**
* Generic Get/Post methods
*/
Request.getHTTPHeader = function () {
  BarcoUtil.logger(bDEBUG, "", this.cls, "getHTTPHeader", "call", "");
  if (Request.authUserPass && Request.authResource) {
    //Consolidate GUI
    var encAuth = 'Basic ' + BarcoUtil.b64encode(Request.authUserPass);
    Request.httpHeader = {
      "Content-Type": "application/json; charset=utf-8",
      'Authorization': encAuth,
      'Accept': 'application/json',
      'X-Barco-resource': Request.authResource
    };
  }
  else {
    //SDK player
    Request.httpHeader = {
      "Content-Type": "application/json; charset=utf-8",
      'Accept': 'application/json'
    };
  }
  return Request.httpHeader;
};

Request.doGET = function (RequestURL, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doGET", "call", RequestURL);
  Request.doGETCustomHeader(RequestURL, null, cb);
};

Request.doGETCustomHeader = function (RequestURL, hdrObj, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doGETCustomHeader", "call", RequestURL);
  var headers = Request.getHTTPHeader();
  for (var key in hdrObj) {
    headers[key] = hdrObj[key];
  }
	jQuery.ajax({
		type: "GET",
		headers: headers,
		url: RequestURL,
		complete: cb,
		cache: false,
		async: true
	});
};
Request.doGETjsonp = function (url, objData, cb) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "doGETjsonp", "call", url);
	jQuery.support.cors = true;
	jQuery.ajax({
        type: 'GET',
        url: url,
        data: objData,
		dataType: 'jsonp',
		cache: false,
		async: true
    }).done(function(data, status, xhr) {
        if (cb) cb(data, status);
    }).fail(function(xhr, status) {
        if (cb) cb(xhr, status);
    });
};
Request.doPOST = function (url, objData, cb) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "doPOST", "call", url);
    Request.doPOSTCustomHeader(url, null, objData, cb);
};
Request.doPOSTCustomHeader = function (url, hdrObj, objData, cb) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "doPOSTCustomHeader", "call", url);
    jQuery.support.cors = true;
    var headers = Request.getHTTPHeader();
    for (var key in hdrObj) {
      headers[key] = hdrObj[key];
    }
    jQuery.ajax({
      type: "POST",
      contentType: "application/json; charset=utf-8",
      headers: headers,
      url: url,
      data: JSON.stringify(objData),
      dataType: 'json',
      // complete: cb,
      async: true
    }).done(function (data, status, xhr) {
        if (cb) cb(xhr, status, data);
    }).fail(function (xhr, status) {
        if (cb) cb(xhr, status);
    });
};
Request.doDELETE = function (RequestURL, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "doDELETE", "call", RequestURL);
  var headers = Request.getHTTPHeader();
  jQuery.ajax({
    type: "DELETE",
    headers: headers,
    url: RequestURL,
    complete: cb,
    cache: false,
    async: true
  });
};

/**
 * setAuthInfo method will sets the authentication information.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param userName {String} 
 * @param password {String}
 * @param protocol {String} 
 * @param ipAddress {String}
 * @param clientId {String}
 * @return Object {JSON}.
 */
Request.setAuthInfo = function (userName, password, protocol, ipAddress, clientId) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "setAuthInfo", "call", clientId);
  Request.authUserPass = userName + ':' + password;
  Request.authResource = clientId + '_' + userName;
  Request.baseURL = protocol + '//' + ipAddress + '/controller';

  return Request.getHTTPHeader();
};

/**
 * Login method will send asynchronous request for XMPP login.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param userName {String} 
 * @param password {String}
 * @param protocol {String} 
 * @param ipAddress {String}
 * @param clientId {String}
 * @param serviceVersion {String}
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */
Request.Login = function (userName, password, protocol, ipAddress, clientId, serviceVersion, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "Login", "call",
    clientId + ', ' + serviceVersion);
  Request.authUserPass = userName + ':' + password;
  Request.authResource = clientId + '_' + userName;
  Request.baseURL = protocol + '//' + ipAddress + '/controller';
  
  var url = Request.baseURL + '/Service/Login';
  var jsonData = { "ServiceLogin": { "clientPrivateKey": "", 
									"@clientVersion": serviceVersion, 
									"clientTemplateTitle": clientId } };
  Request.doPOST(url, jsonData, cb);
};

/**
 * Logout method will send asynchronous request for Logout.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */ 
Request.Logout = function (cb) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "Logout", "call", "");
  	var url = Request.baseURL + '/Service/Logout';
	var jsonData = {};
	Request.doPOST(url, jsonData, cb);
};

/**
 * Kickout method will send asynchronous request for Logout.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */ 
Request.Kickout = function (cb) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "Kickout", "call", "");
  var url = Request.baseURL + '/Service/Kickout';
  var jsonData = {};
	Request.doPOST(url, jsonData, cb);
};

/**
 * View method will send asynchronous request for start Session (Live).
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param reqParams {Object}
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */
Request.View = function (reqParams, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "View", "call", "");
  var url = Request.baseURL + '/Session';
  var jsonData = {
    "View": {
      "@id": "",
      "@name": "",      
      "@canvasId": "",      
      "@windowId": "",
      "@sessionId": "",
      "@sessionName": "",
      "@srcEntityId": "",
      "@dstEntityId": "",
      "@restartSession": "",
      "@profileId": "",
      "Profile": {
        "@profileTitle": "",
        "@profileNID": "",
        "@id": ""
      },
      "SrcPort": {
        "@portNID": "",
        "@portTitle": "",
        "@pvrEnable": "",
        "@trimLength": "3600000",
        "@id": ""
      },
      "DstPort": {
        "@portNID": "",
        "@portTitle": "",
        "@id": "",
        "@portIP": ""
      }
    }
  };
  jsonData.View['@canvasId'] = reqParams.canvasId ||"";
  jsonData.View['@windowId'] = reqParams.windowId ||"";
  jsonData.View['@sessionId'] = reqParams.sessionId ||"";
  jsonData.View['@dstEntityId'] = reqParams.dstEntityId ||"";
  jsonData.View['@restartSession'] = reqParams.restartSession ||"";
 
  jsonData.View.SrcPort['@portNID'] = reqParams.srcPortNID ||"";
  jsonData.View.SrcPort['@portTitle'] = reqParams.srcPortTitle ||"";
  jsonData.View.SrcPort['@pvrEnable'] = reqParams.pvrEnable ||"";
  jsonData.View.DstPort['@portNID'] = reqParams.dstPortNID ||"";
  jsonData.View.DstPort['@portTitle'] = reqParams.dstPortTitle || "";
  jsonData.View.DstPort['@portIP'] = reqParams.dstPortIP || "";
  
  Request.doPOST(url, jsonData, cb);
};

/**
 * Playback method will send asynchronous request for start Session (Playback).
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param reqParams {Object}
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */
Request.Playback = function (reqParams, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "Playback", "call", "");
  var url = Request.baseURL + '/Session';
  var jsonData = {
    "Playback": {
      "@id": "",      
      "@canvasId": "",
      "@windowId": "",
      "@sessionId": "",      
      "@srcEntityId": "",
      "@dstEntityId": "",
      "@restartSession": "",
      "GrpFile": {
          "@fileNID": "",
          "@fileTitle": "",
          "@id": ""
      },
      "SrcFile": {
          "@fileNID": "",
          "@fileTitle": "",
          "@grpFileId": "",
          "@id": ""
      },
      "DstPort": {
          "@portNID": "",
          "@portTitle": "",
          "@id": ""
      }
    }
  };
  jsonData.Playback['@windowId'] = reqParams.windowId ||"";
  jsonData.Playback['@sessionId'] = reqParams.sessionId ||"";
  jsonData.Playback['@restartSession'] = reqParams.restartSession ||"";
  jsonData.Playback.SrcFile['@fileNID'] = reqParams.fileNID ||"";
  jsonData.Playback.SrcFile['@fileTitle'] = reqParams.fileTitle ||"";
  jsonData.Playback.DstPort['@portNID'] = reqParams.portNID ||"";
  jsonData.Playback.DstPort['@portTitle'] = reqParams.portTitle ||"";
  
  Request.doPOST(url, jsonData, cb);
};

/**
 * StartURL method will send asynchronous request for start Session (StartURL).
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param reqParams {Object}
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */
Request.StartURL = function (reqParams, cb) {
	BarcoUtil.logger(bDEBUG, "", this.cls, "StartURL", "call", "");
	//jQuery.getJSON(reqParams.sessionURL, cb);
	/*
	jQuery.ajax({
      url: reqParams.sessionURL,
      dataType: 'jsonp',
      data: reqParams.data,
      complete: cb
    });
	*/
	
	 jQuery.ajax({
        type: 'GET',
        url: reqParams.sessionURL,
        data: reqParams.data,
		dataType: 'jsonp'
     }).done(function(data, status, xhr) {
        if (cb) cb(data, status);
     }).fail(function(xhr, status) {
        if (cb) cb(xhr, status);
     });
	 
	//var url = reqParams.sessionURL;
    //var data = reqParams.data;
	//Request.doGETjsonp(url, jsonData, cb);
};

/**
 * StopSession method will send asynchronous request for Stop Session.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @param sessionId {String}
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */
Request.StopSession = function (sessionId, cb) {
  BarcoUtil.logger(bDEBUG, "", this.cls, "StopSession", "call", sessionId);
  var url = Request.baseURL + '/Session';
  var jsonData = { "StopSession": { "@sessionId": sessionId } };
  
  Request.doPOST(url, jsonData, cb);
};

/**
 * SessionPvr method will send asynchronous request for Pause/Resume/Skipback/Skipfwd/Catchup Session.
 * <p/>
 * The arguments and the order of those arguments for this method are:;
 * <p/>
 * @reqParams {Object}
 * @param Callback {Object}
 * @return objResponse (asyncRV).
 */
Request.SessionPvr = function (reqParams, cb) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "SessionPvr", "call", "");
    var url = Request.baseURL + '/Session';
	var jsonData = {};
    if (reqParams.action == "Pause") {
        jsonData = { "Pause": { "@sessionId": reqParams.sessionId} };
    }
    else if (reqParams.action == "Resume") {
        jsonData = { "Resume": { "@sessionId": reqParams.sessionId, "@timeScale": reqParams.timeScale} };
    }
    else if (reqParams.action == "SkipBack") {
        jsonData = { "SkipBack": { "@sessionId": reqParams.sessionId, "@timeScale": reqParams.timeScale, "@skipOffset": reqParams.skipOffset} };
    }
    else if (reqParams.action == "SkipFwd") {
        jsonData = { "SkipFwd": { "@sessionId": reqParams.sessionId, "@timeScale": reqParams.timeScale, "@skipOffset": reqParams.skipOffset} };
    }
    else if (reqParams.action == "Catchup") {
        jsonData = { "Catchup": { "@sessionId": reqParams.sessionId} };
    }
	
	Request.doPOST(url, jsonData, cb);
};

/**
* StartRecord method will send asynchronous request to add a Record connection.
* <p/>
* The arguments and the order of those arguments for this method are:;
* <p/>
* @param  JSON object
* @param Callback {Object}
* @return objResponse (asyncRV).
*/
Request.StartRecord = function (reqParams, cb) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "StartRecord", "call", "");
    var base64Encord = 'Basic ' + BarcoUtil.b64encode('u1:Ipvs1234');
    Request.httpHeader = {
        "Content-Type": "application/json; charset=utf-8",
        'Authorization': base64Encord,
        //if Accept header then return **, bydefault get JSON response back, 
        //if fill then following application/vnd.barco.nimbus.Device+json except serviceLogin
        'Accept': 'application/json',
        'X-Barco-resource': 'web-client_u1'
    };
    Request.baseURL = 'http://10.51.51.73/controller';

    var sessionId = reqParams.sessionId;
    var srcEntityId = reqParams.srcEntityId;
    var srcTitle = reqParams.srcTitle ? reqParams.srcTitle : "";

    var jsonData = { "Record": {
                            "@name": "",
                            "DstFile": {
                                "MetaData": "",
                                "@fileTitle": srcTitle,
                                "@id": ""
                            },
                            "@sessionId": sessionId,
                            "@id": "",
                            "@srcEntityId": srcEntityId,
                            "@resetTimeline": "true",
                            "@sessionName": "",
                            "SrcPort": {
                                "@portNID": "",
                                "@portTitle": "",
                                "@id": ""
                            }
                        }
                    };
    jQuery.support.cors = true;
    jQuery.ajax({
        url: Request.baseURL + '/Session',
        type: "POST",
        dataType: 'json',
        headers: Request.httpHeader,
        data: JSON.stringify(jsonData),
        contentType: "application/json; charset=utf-8"
    }).done(function (data, status, xhr) {
        if (cb) cb(data, status);
    }).fail(function (xhr, status) {
        if (cb) cb(xhr, status);
    });
};


/**
* StopRecordRequest method will send asynchronous request to delete a Record connection
* <p/>
* The arguments and the order of those arguments for this method are:;
* <p/>
* @param reqParams JSON object
* @param Callback {Object}
* @return objResponse (asyncRV).
*/
Request.StopRecord = function (reqParams, cb) {
    BarcoUtil.logger(bDEBUG, "", this.cls, "StopRecord", "call", "");
    var base64Encord = 'Basic ' + BarcoUtil.b64encode('u1:Ipvs1234');
    Request.httpHeader = {
        "Content-Type": "application/json; charset=utf-8",
        'Authorization': base64Encord,
        //if Accept header then return **, bydefault get JSON response back, 
        //if fill then following application/vnd.barco.nimbus.Device+json except serviceLogin
        'Accept': 'application/json',
        'X-Barco-resource': 'web-client_u1'
    };
    Request.baseURL = 'http://10.51.51.73/controller';
    var sessionId = reqParams.sessionId;
    var connectionId = reqParams.connectionId;
    var jsonData = { "DeleteConn": {
                        "@sessionId": sessionId,
                        "@connId": connectionId
                        }
                   };
    jQuery.support.cors = true;
    jQuery.ajax({
        url: Request.baseURL + '/Session',
        type: "POST",
        dataType: 'json',
        headers: Request.httpHeader,
        data: JSON.stringify(jsonData),
        contentType: "application/json; charset=utf-8"
    }).done(function (data, status, xhr) {
        if (cb) cb(data, status);
    }).fail(function (xhr, status) {
        if (cb) cb(xhr, status);
    });
};


/* working version
Request.SessionPvr = function (reqParams, cb) {
    var requestURL = ""; // reqParams.sessionURL + "&requestName=" + reqParams.action + "&" + request + ".sessionId=" + reqParams.sessionId + "&" + requestParam + '&callback=?';

    if (reqParams.action == "Pause") {
        requestURL = reqParams.sessionURL + "&requestName=" + reqParams.action + "&" + reqParams.action + ".sessionId=" + reqParams.sessionId + '&callback=?';
    }
    else if (reqParams.action == "Resume") {
        requestURL = reqParams.sessionURL + "&requestName=" + reqParams.action + "&" + reqParams.action + ".sessionId=" + reqParams.sessionId + '&callback=?';
    }
    else if (reqParams.action == "Catchup") {
        requestURL = reqParams.sessionURL + "&requestName=" + reqParams.action + "&" + reqParams.action + ".sessionId=" + reqParams.sessionId + '&callback=?';
    } //
    else if (reqParams.action == "SkipBack") {
        requestURL = reqParams.sessionURL + "&requestName=" + reqParams.action + "&" + reqParams.action + ".sessionId=" + reqParams.sessionId + "&SkipBack.skipOffset=" + reqParams.skipOffset + '&callback=?';
    }
    else if (reqParams.action == "SkipFwd") {
        requestURL = reqParams.sessionURL + "&requestName=" + reqParams.action + "&" + reqParams.action + ".sessionId=" + reqParams.sessionId + "&SkipFwd.skipOffset=" + reqParams.skipOffset + '&callback=?';
    }

	var jsonData = {};
	
	Request.doGETjsonp(requestURL, jsonData, cb);
};
*/