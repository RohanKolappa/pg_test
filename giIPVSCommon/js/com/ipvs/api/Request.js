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
jsx3.Class.defineClass("com.ipvs.api.Request", jsx3.net.Request, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(IpvsRequest, IpvsRequest_prototype) {

  /** @private @jsxobf-clobber */
  IpvsRequest_prototype._LOG = jsx3.util.Logger.getLogger("com.ipvs.api.Request");

  IpvsRequest_prototype.init = function() {
    // get a handle to the app
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    
    var UserJID = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_USERJID);
    if( jsx3.util.strEmpty(UserJID) ) {
      UserJID = "NONE";
    }
    this.UserJID = UserJID;
    // Get a random requestID and use it to create unique topic name
    this.TopicName = "Request_" + jsx3.xml.CDF.getKey();
    // set up subscription for this req related events
    this.clsApp.clsNotification.subscribe(this.TopicName, this, "_onRequestResponseState");
    
    // setup basic request vars
    this.LogLevel = 0;
    this.ClientData = this.TopicName;
    this.ServiceName = "NONE";
    this.ObjectType = "NONE";
    this.OpType = "NONE";
    this.OpSpec = "NONE";
    this.Op_Where_NID = "";
    this.Op_Where_ParentNID = "";
    this.Op_Where_WhereAssistXML = null;
    this.Op_Where_WhereXPath = "";
    this.Op_Where_XQueryPrefix = "";
    this.Op_In_ChildXPath = "";
    this.Op_Select_ListSpec_StartFrom = 0;
    this.Op_Select_ListSpec_CountToFetch = -1;
    this.Op_Select_ListSpec_SortOrder = "";
    this.Op_Select_ListSpec_SortByXPathList_SortByXpath = "";
    this.Op_Select_ItemSpec_NodeDepth = -1;
    this.Op_Select_ItemSpec_SelectXML = "";
    this.Op_Select_ItemSpec_SelectXQuery = "";
    this.Op_ReadQueryRequest_Query = "";
    
    this.RequestName = null;
    this.RequestDataRuleName = null; 
    this.RequestXML = null;
    this.ReturnXML = null;
    this.ResponseXML = null;
    this.ResponseNode = null;

    //session API parameters
    //-- rule:SessionIQ.xml
    this.iq_id = this.TopicName;
    this.iq_from = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_USERJID);
    this.iq_to = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_APPSERVERAGENTJID); 
    //-- rule:SessionJoin.xml
    this.session_id = 0;
    this.bookmark_id = 0;
    this.ObjectList_Type = "NONE";
    this.ObjectList_StartFrom = "";
    this.ObjectList_CountToFetch = "";
    this.ObjectList_SortOrder = "";
    this.ObjectList_SortBy = "";
    this.ObjectList_SortByMetaData = "";
    this.ObjectList_WhereNID = "";
    this.ObjectList_WhereID = "";
    this.ObjectList_WhereParentNID = "";
    this.ObjectList_WhereSessionConfigNID = "";
    this.ObjectList_WhereTextSubStr = "";
    this.ObjectList_SearchTextSubStrIn = "";
    this.ObjectList_WhereDateRange = "";
    this.ObjectList_WhereTagExpr = "";
    this.ObjectList_SelectDetailLevel = "";
    this.ObjectList_SelectFormat = "BASIC";
    this.ObjectList_SubscriptionID = "";
    this.ObjectList_WhereDateCreated = "";
    this.ObjectList_WhereState = "";
    this.ObjectList_WhereOwnerJID = "";
    this.ObjectList_WhereType = "";
    this.ObjectList_whereContactJID = "";
    this.ObjectList_WherePriority = "";
    this.ObjectList_WhereStateDescription = "";
    this.ObjectList_WhereTCRange = "";
    this.ObjectList_WhereFileNID = "";
    this.ObjectList_WhereSessionInfo = "";
    this.ObjectList_WhereNameTextSubStr = "";
    this.ObjectList_WhereValueTextSubStr = "";
    this.ObjectList_WhereMetaDataExpr = "";
    this.ObjectList_WhereParentDirNID = "";
    this.ObjectList_WherePermissions = "";

    //Switch Command Parameters
    this.ObjectList_newSrcEntityId = ""; 
    //View Command Parameters
    this.ObjectList_sessionId = "";
    this.ObjectList_sessionName = "";
    this.ObjectList_connId = "";
    this.ObjectList_connName = "";
    this.ObjectList_SrcEntityId = "";
    this.ObjectList_DstEntityId = "";
    this.ObjectList_dstEntityId = "";
    this.ObjectList_dstPortNID = "";
    this.ObjectList_dstPortTitle = "";
    this.ObjectList_dstDirNID = "";
    this.ObjectList_dstFileTitle = "";
    this.ObjectList_srcEntityId = "";
    this.ObjectList_srcPortNID = "";
    this.ObjectList_srcPortTitle = "";
    this.ObjectList_pvrEnable = "";
    this.ObjectList_fileNID = "";
    this.ObjectList_fileEntityId = "";
    //Windowind Commands Parameter
    this.ObjectList_windowId = "";
    this.ObjectList_canvasId = "";
    this.ObjectList_restartSession = "";
    this.ObjectList_resetTimeline = "";
    this.ObjectList_profileId = "";
    this.ObjectList_profileNID = "";
    this.ObjectList_profileTitle = "";
    this.ObjectList_grpFileId = "";
    this.ObjectList_grpFileNID = "";
    this.ObjectList_dirId = "";
    this.ObjectList_Key = "";
    this.ObjectList_Action = "";
    this.ObjectList_keyMetaDataKeyName = "";
    this.cb = null;
    this.RequestDataObjectArr = [];
  }
 
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
    this._status = 0;
    this._async = Boolean(bAsync);
    this._method = strMethod;
    var reqName = strURL.getPath();
    var ReqParamsArr = strURL.getQueryParams();
    try {
      if( !jsx3.util.strEmpty(ReqParamsArr) ) {
        for(var rpName in ReqParamsArr) {
          this[rpName] = unescape(ReqParamsArr[rpName]);
        }
      }
      this._LOG.debug("New ServiceAppletRequest for : " + reqName);
    }
    catch(e) {
      this._status = jsx3.net.Request.STATUS_ERROR;
    }
    return this;
  };
  
  IpvsRequest_prototype.async_send = jsx3.$Y(function(cb) { 
    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(encodeURI(cb.args()[0])); 
    this.open("GET", uri, true);
    this.cb = cb;
    this.send(); 
  }) 


  IpvsRequest_prototype.send = function() {
    // Get request XML if it does not exist
    if( this.RequestXML == null ) {
      // user request params to get requestname if not already specd
      if( this.RequestName == null ) {
        var ReqName = "";
        if( this.ServiceName == "xmpp" || this.ServiceName == "systemservice" || this.ServiceName == "mediaroomservice" ) {
          if(this.OpType != "Operation") {
            ReqName += this.OpType;
          }
          ReqName += this.ObjectType;
          if(this.OpSpec != "NONE") {
            ReqName += this.OpSpec;
          }
          ReqName += "Request";
        }
        else if( this.ServiceName == "assetadminservice" ) {
          if(this.ObjectType == "EmptyRecycleBin") {
            ReqName = "EmptyRecycleBinRequest";
          } else {
            ReqName = "AssetManagementRequest";
          }
        }
        else if( this.ServiceName == "policyadminservice" ) {
          ReqName = "PolicyManagementRequest";
        }
        this.RequestName = ReqName;
      }
      if( this.RequestName == "" ) {
        this._onResponse("REQUESTFAIL", "Bad Request Info: \n" +
                            "ServiceName :" + this.ServiceName + "\n" + 
                            "ObjectType :" + this.ObjectType + "\n" + 
                            "OpType :" + this.OpType + "\n", null);
        return;
      }
      // load requestheader rule - everything else should cascade from there
      
      var requestRuleName = "ServiceAppletRequest";
      //check if agent meesage need to be sent
      if( this.AgentMessageHeader != null ) {
        //agent message with Request
        if( this.AgentMessageHeader.agentmessagetype == "Request" ) {
          requestRuleName = "AgentMessage";
        }
        //agent message with AgentEvent
        else if( this.AgentMessageHeader.agentmessagetype == "AgentEvent" ){
          requestRuleName = "AgentEventMessage";
        }
      }

      //check if session service - use IQ to send
      if( this.ServiceName == "sessionservice" ) {
        requestRuleName = "SessionIQ";
      }
      //avoid resolve XML rules from GI for the following 4 requests
      if (this.RequestName == 'LoginRequest' || this.RequestName == 'SetServiceDomainRequest' ||
                this.RequestName == 'GetServiceClientRequest' || this.RequestName == 'GetServerTimeRequest' ||
                this.RequestName == 'GetFileRequest' || this.RequestName == 'GetPortRequest' || this.RequestName == 'PauseRequest' || 
                this.RequestName == 'ResumeRequest' || this.RequestName == 'SkipBackRequest' || this.RequestName == 'SkipFwdRequest' ||
                this.RequestName == 'CatchupRequest' || this.RequestName == 'StopRequest' || this.RequestName == 'ViewRequest' || 
                this.RequestName == 'RecordRequest' || this.RequestName == 'DeleteConnRequest' || this.RequestName == 'PlayBackSrcRequest' ||
                this.RequestName == 'PlayBackGrpRequest' || this.RequestName == 'GetBookmarkRequest' || this.RequestName == 'NewBookmarkRequest' ||
                this.RequestName == 'AddBookmarkRequest' || this.RequestName == 'UpdateBookmarkRequest' || this.RequestName == 'ResumeFromBookmarkRequest' ||
                this.RequestName == 'DeleteBookmarkRequest' || this.RequestName == 'AuditEventRequest' || this.RequestName == 'LogoutRequest' ) {
          //get rules from RequestRules.js
          var requestJSON = com.ipvs.api.RequestRules.getRule(this);
          this.RequestXML = xmlJsonClass.json2xml(requestJSON, "\t");
      }
      else {
          //other than above request still using same way to get rules XML
          var requestDoc = utilRuleGetOutboundDoc.call(this, requestRuleName);
          if (requestDoc != null) {
              this.RequestXML = requestDoc.toString();
          }
      }
      if (this.ServiceName == "sessionservice" && !jsx3.util.strEmpty(this.RequestXML)) {
          if (this.QueryType != "XMPPQuery") {
              // add 'xmlns' to IPVS query node
              var serviceVersion = this.getServiceVersion();
              this.RequestXML = (this.RequestXML).replace(/<query>/, "<query xmlns=\"com.ipvs.session\" serviceVer=\"" + serviceVersion + "\">");
          }
      }
    }
    this.REQUESTSEND();
  }

  // default handling - can be overridden by specific request to do some processing
  IpvsRequest_prototype.REQUESTSEND = function() {
    if( this.RequestXML == null ) {
      this._onResponse("REQUESTFAIL", "No Request XML", null);
      return;
    }
    // log the request
    this._LOG.info("Request: " + (this.RequestXML).replace(/[\n\r]/g, " ").replace(/>\s+</g, "><"));
    // send the request
    this.ReturnXML = this.clsApp.clsApplet.applet_sendRequest(this.UserJID, this.RequestXML);
    if( this.ReturnXML == null ) {
      this._onResponse("REQUESTFAIL", "NULL Return from Applet SendRequest", null);
      return;
    }
    // stash the return value in the topic 
    var returnDoc = new jsx3.xml.Document();
    returnDoc.loadXML(this.ReturnXML);
    var returnNode = returnDoc.getRootNode(); 
    // check for Error return
    if (returnNode != null && returnNode.selectSingleNode("/Error")!=null) {
      this._onResponse("REQUESTFAIL", "REQUEST ERROR RETURN: " + returnNode.toString(), null);
      return;  
    }
  }

  IpvsRequest_prototype.RESPONSERECEIVE = function() {
    var responseNode = this.ResponseNode;
    if (responseNode == null) {
      this._onResponse("RESPONSEFAIL", "No Response In Topic: " + this.TopicName, null);
    }
    this.ResponseXML = responseNode.toString();
      
    //CASE 1. for all new session API which response with iq node
    //iq is the root node of the responseNode
    if (responseNode.getNodeName() == "iq") {
      //remove xmlns from iqNode
      this.ResponseXML = (this.ResponseXML).replace(/xmlns=\"com.ipvs.session\"/g, "");
      responseNode = new jsx3.xml.Document();
      responseNode.loadXML(this.ResponseXML);
    }
    //CASE 2. for new session error handler which response with query and error nodes inside of iq node
    if (responseNode.selectSingleNode("./error") != null) {
      //if error node is a direct child of iq node
      var ipvsErrCodeNode = responseNode.selectSingleNode(".//ipvserror");
      var ipvsErrStrNode = responseNode.selectSingleNode(".//description");
      var objError = new Object();
      objError.errorCode = ipvsErrCodeNode == null ? "" : ipvsErrCodeNode.getAttribute("code");
      objError.errorData = ipvsErrStrNode == null ? "" : ipvsErrStrNode.getValue();
      this._onResponse("RESPONSEFAIL", "Bad Response State: IPVS error code:" + objError.errorCode + "; Error:" + objError.errorData + "." + "\n" + " ResponseXML: " + this.ResponseXML, objError);
      return;
    }    
    //CASE 3. for all none session API responses which without any iq node
    if (responseNode.selectSingleNode(".//header/state") != null) {
      // check the state in the response - if 0 - simple pass the raw data thru to the caller
      var ResponseHeaderState = responseNode.selectSingleNode(".//header/state").getValue();
      if (ResponseHeaderState == null || ResponseHeaderState == 0) {
        this._onResponse("RESPONSEFAIL", "Bad Response State: " + ResponseHeaderState + "\n" + " ResponseXML: " + this.ResponseXML, null);
        return;
      }
    }
    //process response
    this._handleResponse(responseNode);
  }

  /** @private @jsxobf-clobber */
  /*
    common response handler
  */
  // this should be split across specific requests that override a base
  IpvsRequest_prototype._handleResponse = function(responseNode) {
    var response = null;
    // check if this was a agent request
    // if so - just pass the response up
    if( this.AgentMessageHeader != null) {
      response = responseNode.selectSingleNode(".//data/*").cloneNode(true);
    }
    else {
      // get the original requestname - and handle the response
      switch (this.RequestName) {
        case "LoginRequest":
        case "LogoutRequest":
          response = responseNode.selectSingleNode(".//header/userJID").getValue();
          break;
        default:
          if( responseNode.selectSingleNode(".//data") != null ) {
            responseNode = responseNode.selectSingleNode(".//data/*");
          }
          else if (responseNode.selectSingleNode(".//query") != null) {
            responseNode = responseNode.selectSingleNode(".//query/*"); //".//jsxnsIPVSSessionService:query/*"
          }
          response = (new jsx3.xml.Document()).loadXML(responseNode.toString());
          break;
      }
    }
    this._onResponse("SUCCESS", response, null);
  }

  /** @private @jsxobf-clobber */
  IpvsRequest_prototype._onResponse = function (respCode, response, objError) {
    this._response = response;
    if( respCode == "SUCCESS" ) {
      this._status = jsx3.net.Request.STATUS_OK;
      this._LOG.debug("IpvsRequest sending request Success");
    }
    else if( respCode == "RESPONSEFAIL" ) {
      this._status = this._getErrorCode(response);
      this._LOG.warn("IpvsRequest sending request Error");
    }
    else {
      this._status = respCode;
      var ErrStr = "Request: " + this.RequestName + " : " + response + "\n" +
                   " RequestXML: " + this.RequestXML + "\n" +
                   " ReturnXML: " + this.ReturnXML + "\n" +
                   " ResponseXML: " + this.ResponseXML + "\n";
      this._LOG.error(respCode + ":" + ErrStr);
      alert(respCode + ":" + ErrStr);
    }

    if (this.cb != null) this.cb.done({ Status: this._status, Val: this._response, ObjError:objError });
    this.publish({subject:jsx3.net.Request.EVENT_ON_RESPONSE, context:{ objEVENT: this }});
    this.endRequest();
  }

  /** @private @jsxobf-clobber */
  IpvsRequest_prototype._onRequestResponseState = function(objEvent) {
    if( !objEvent ) return;
    if( objEvent.stateVar == "RequestResponseState" ) {
      this.ResponseNode = objEvent.responseNode;
      this.RESPONSERECEIVE();
    }
  }

  /**
   * This will destroy the request.
   */
  IpvsRequest_prototype.endRequest = function() {
    this.cb = null;
    this.clsApp.clsNotification.unsubscribe(this.TopicName, this, "_onRequestResponseState");
  }

  /**
   * Gets the ServiceAppletRequest response code (e.g. 200, 1003, 1004, etc).
   * @return {int}
   */
  IpvsRequest_prototype.getStatus = function() {
    return this._status == 0 ? jsx3.net.Request.STATUS_OK : this._status;
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

  /** @private @jsxobf-clobber */
  IpvsRequest_prototype._getErrorCode = function(respVal) {
    var ErrCode = jsx3.net.Request.STATUS_ERROR;
    if( respVal.match(/ResponseXML/) ) {
      var ResponseXML = (respVal.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
      //this._response = ResponseXML;
      var responseDoc = new jsx3.xml.Document;
      responseDoc.loadXML(ResponseXML);
      this._response = responseDoc;
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

  IpvsRequest_prototype.appendQueryNode = function(requestNode) {
    // create and append a 'query' node
    var queryNode = requestNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "query");
    requestNode.appendChild(queryNode);
    // cascade call to add query request date
    this.appendRequestDataNode(queryNode);
  }

  IpvsRequest_prototype.appendRequestDataNode = function(requestNode){
    if( this.RequestDataRuleName == null ) {
      if (this.ServiceName == "sessionservice") {
        // select requestdatarule based on requestname
        this.RequestDataRuleName = "Session" + this.RequestName;
      }
      else {      
        this.RequestDataRuleName = this.RequestName + "Data";
      }
    }
    var requestDataDoc = utilRuleGetOutboundDoc.call(this, this.RequestDataRuleName);
    if( requestDataDoc != null ) {
      this.requestDataNode = requestDataDoc.selectSingleNode("/*").cloneNode(true);
      requestNode.appendChild(this.requestDataNode); 
    }  
  }

  IpvsRequest_prototype.replaceRequestDataObjectNode = function(requestNode) {
    var NodeName = requestNode.getNodeName();
    var objectNode = this.RequestDataObjectArr[NodeName];
    if( objectNode != null ) {
      //as per Documentaion Iterating over the attribute names is 
      //more performant than using the getAttributes() method.
      var arrAttributeNames = objectNode.getAttributeNames();
      var attrCount = arrAttributeNames.length;
      for( var nameIndex = 0; nameIndex < attrCount; nameIndex++ ) {
        var objAttrNode = objectNode.getAttributeNode(arrAttributeNames[nameIndex]);
        objectNode.removeAttributeNode(objAttrNode);
        requestNode.setAttributeNode(objAttrNode);
      }

      var objectNodeIter = objectNode.getChildIterator();
      if( objectNodeIter._next != null ) {
        while( objectNodeIter.hasNext() ) {
          requestNode.appendChild(objectNodeIter.next().cloneNode(true));
        }
      }
      else {
        requestNode.setValue("");
      }
    }
    else {
      requestNode.setValue("");
    }
  }  
  
  IpvsRequest_prototype.getServiceVersion = function() {
    //Use singleton accessor method of App class and get service version
    return this.clsApp.clsApplet.getServiceVersion();
  }
});
jsx3.net.Request.addSchemeHandler("ipvs", com.ipvs.api.Request.jsxclass);