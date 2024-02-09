/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Ipvs request which handles applet request.
* <p/>
* This class uses jsx3.net.Request.
* <p/>
* This class adds handler with 'ipvs' scheme to use applet request.
* If XMLurl for a document is with 'ipvs:///' then Document.load will use this request.
*/
jsx3.Class.defineClass("com.ipvs.api.Request", jsx3.net.Request, null, function(IpvsRequest, IpvsRequest_prototype) {

  /** @private @jsxobf-clobber */
  IpvsRequest._LOG = jsx3.util.Logger.getLogger("com.ipvs.api.Request");

  /**
  * The instance initializer.
  * @param RequestType {String} Request type to use.Default is 'ServiceAppletRequest'.
  */
  IpvsRequest_prototype.init = function(RequestType) {
    if( RequestType != null ) {
      this._requestType = RequestType;
      this._request = null;
      this._response = null;
    }
  };
  
  /**
   * Initializes the ServiceAppletRequest
   * @param strMethod {String} The HTTP method used to open the connection. Valid values include: GET, POST, or PUT.
   * @param strURL {String|jsx3.net.URI} The requested URL.
   * Note:strURL should start with 'ipvs:///<RequestName>' and requsest parameters as query string.
   * Eg:'ipvs:///RequestName?StartFrom=1&CountToFetch=25&OrderBy=....'.
   * @param bAsync {boolean} whether to issue the request asynchronously, if true this class will use the EventDispatcher interface to publish an event on response.
   * @return {com.ipvs.api.Request} this object.
   */
  IpvsRequest_prototype.open = function(strMethod, strURL, bAsync) {
    if( this._request != null ) return null;
    if( jsx3.util.strEmpty(this._requestType) ) this._requestType = "ServiceAppletRequest";
    this._status = 0;
    this._async = Boolean(bAsync);
    this._method = strMethod;
    var reqName = strURL.getPath();
    var ReqParamsArr = strURL.getQueryParams();

    // get new request handle
    var UserJID = topic_Get(tApp, "LoginUserJID");
    if( UserJID == null ) {
      UserJID = "NONE";
    }
    //this._requestEvent = ReqEvent;
    try {
      this._request = eval("new " + this._requestType + "(UserJID)");
      if( !jsx3.util.strEmpty(ReqParamsArr) ) {
        for(var rpName in ReqParamsArr) {
          this._request[rpName] = unescape(ReqParamsArr[rpName]);
        }
      }
      IpvsRequest._LOG.info("New ServiceAppletRequest for : " + reqName);
    }
    catch(e) {
      this._status = jsx3.net.Request.STATUS_ERROR;
    }
    if( this.RequestDataObjectArr != null){
      this._request.RequestDataObjectArr = this.RequestDataObjectArr;
    }
    return this;
  };
  
  /**
   * Sets the call back for ServiceAppletRequest and sends the request.
   * @param strContent {String} The content to send for a POST request.
   * @param intTimeout {int}  the number milliseconds to wait before publishing a timeout event. This only applies
   *    to asynchronous requests. If used, subscribe to the <code>jsx3.net.Request.EVENT_ON_TIMEOUT</code> event to
   *    be notified of a timeout.
   * Note:Current version of ServiceAppletRequest is not supporting strContent and intTimeout.
   */
  IpvsRequest_prototype.send = function(strContent, intTimeout) {
    // setup callbacks
    this._request.setCallback("SUCCESS", this, IpvsRequest_prototype._onResponse);
    this._request.setCallback("RESPONSEFAIL", this, IpvsRequest_prototype._onResponse);
    // send req
    IpvsRequest._LOG.info("IpvsRequest sending request..");
    this._request.send();
    
  };

  /**
   * This will attach the agentMessageheader object request.
   */
  IpvsRequest_prototype.setAgentMessageHeader = function(agentMsgHeaderObj) {
    if( agentMsgHeaderObj ){
      this._request.AgentMessageHeader = agentMsgHeaderObj;
    }    
  };

  /**
  * This will attach the agentMessageheader object request.
  */
  IpvsRequest_prototype.setAgentMessageDataObjectArr = function (agentMessageDataObjectArr) {
    if (agentMessageDataObjectArr) {
      this._request.AgentMessageDataObjectArr = agentMessageDataObjectArr;
    }
  };
  
  /**
  * This will attach the RequestDataObject object request.
  */
  IpvsRequest_prototype.setRequestDataObjectArr = function (requestDataObjectArr) {
    if (requestDataObjectArr) {
      this._request.RequestDataObjectArr = requestDataObjectArr;
    }
  };

  /**
   * Gets the ServiceAppletRequest response code (e.g. 200, 1003, 1004, etc).
   * @return {int}
   */
  IpvsRequest_prototype.getStatus = function() {
    var s = this._status != null ? this._status : this._request.status;
    return s == 0 ? jsx3.net.Request.STATUS_OK : s;
  }
  
  /**
   * Gets the content of the response as string.
   * @return {String}
   */
  IpvsRequest_prototype.getResponseText = function() {
    var responseText = null;
    if (!jsx3.util.strEmpty(this._response)) {
      responseText = this._response.toString();
    }
    return responseText;
  };

  /**
   * Gets the content of the response as an XML document. If the response is not a valid XML document,
   * <code>null</code> is returned.
   * @return {jsx3.xml.Document}
   */
  IpvsRequest_prototype.getResponseXML = function() {
    var objDoc = new jsx3.xml.Document();
    objDoc.loadXML(this.getResponseText());
    return objDoc;
  }

  /**
   * This will destroy the request.
   */
  IpvsRequest_prototype.endRequest = function() {
    if( this._request != null ) {
      this._request.destroy();
      this._request = null;
    } 
  }

  /** @private @jsxobf-clobber */
  IpvsRequest_prototype._onResponse = function(req, ErrCode, response) {
    this._response = response;
    if( ErrCode == "SUCCESS" ) {
      this._status = jsx3.net.Request.STATUS_OK;
      IpvsRequest._LOG.info("IpvsRequest sending request Success");
    }
    else {
      this._status = this._getErrorCode(response);
      IpvsRequest._LOG.info("IpvsRequest sending request Error");
    }    
    this.publish({subject:jsx3.net.Request.EVENT_ON_RESPONSE});
    this.endRequest();
  }

  /** @private @jsxobf-clobber */
  IpvsRequest_prototype._getErrorCode = function(respVal) {
    var ErrCode = jsx3.net.Request.STATUS_ERROR;
    if( respVal.match(/ResponseXML/) ) {
      var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
      var responseDoc = new jsx3.xml.Document;
      responseDoc.loadXML(ResponseXML);
      try {
        ErrCode = responseDoc.selectSingleNode(".//Code").getValue();        
      } catch(e) {
      }
    }
    return ErrCode;    
  }
  
   /**
   * Gets the value of a specific ServiceAppletRequest response header.
   * @return {String}
   */
  IpvsRequest_prototype.getResponseHeader = function() {
    var respHeader = "";
    if( this.getStatus() != jsx3.net.Request.STATUS_OK) {
      respHeader = "ipvs";
    }
    return respHeader;
  }
});

jsx3.net.Request.addSchemeHandler("ipvs", com.ipvs.api.Request.jsxclass);