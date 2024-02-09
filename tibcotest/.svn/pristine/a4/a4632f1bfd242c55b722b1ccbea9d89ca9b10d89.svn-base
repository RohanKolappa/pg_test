//-------------------------------------
// TBD - instead of using this global - tack it on to the service object itself
// in the rule refer to is as "this.requestHandle"
//var requestHandle = null;
/*
  common request sender -
    - creates requestspecifc topicdoc
    - calls the correct mapping rule to generate the request data+header XML
    - sends the requestXML to the applet
    Inputs:
      - UserJID = callers full JID
      - ServiceName = mediaroom, assetadmin, policyadmin ...
      - ObjectType = Source/StreamProfile/
                    CreateConnection/AddStreamAction/
                    MediaRoomNID/MediaRoom/
                    AssetResource/
                    V2DMediaStreamProfile/MPEGTSMediaStreamProfile/RTPMediaStreamProfile/
                    V2D-Tx-Device/V2D-Rx-Device/DMS-120-Device/DMS-1000-Device/IPVSClient-Device
      - RequestOpType = Get/Add/Delete/Replace/Operation
      - RequestOpSpec = List/NID/NONE
      - RequestData = NONE, getList-startcountArray, get-delete-replace-NID, add-operationdataNode, mediaroomrequestdataNode
*/
function ServiceAppletRequest(UserJID) {
  if( UserJID != null ) {
    this.UserJID = UserJID;
    // Get a random requestID and use it to create unique topic name
    this.TopicName = "Request_" + jsx3.xml.CDF.getKey();
    topic_Create(this.TopicName);
    // set up subscription for this req related events
    topic_Subscribe(this.TopicName, this, "eventHandler");
    // setup callbackArr
    this.callbackArr = new Object;
    // setup basic request vars
    this.LogLevel = 0;
    this.ClientData = this.TopicName;
    this.ServiceName = "NONE";
    this.ObjectType = "NONE";
    this.OpType = "NONE";
    this.OpSpec = "NONE";
    this.Op_Where_NID = "";
    this.Op_Where_AssetNID = "";
    this.Op_Where_Title = "";
    this.Op_Where_Type = "";
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
    this.requestData = "NONE";
    this.responseData = "NONE";
    
    this.RequestName = null;
    this.RequestDataRuleName = null; 
    this.RequestXML = null;
    this.ReturnXML = null;
    this.ResponseXML = null;
    this.SimResponseXMLFileName = null;

    //session API parameters
    //-- rule:SessionIQ.xml
    this.iq_id = this.TopicName;
    //-- rule:SessionJoin.xml
    this.session_id = 0;
    this.bookmark_id = 0;
    this.timeScale = "";
    this.ObjectList_Type = "NONE";
    this.ObjectList_StartFrom = "";
    this.ObjectList_CountToFetch = "";
    this.ObjectList_SortOrder = "";
    this.ObjectList_SortBy = "";
    this.ObjectList_SortByMetaData = "";
    this.ObjectList_SortByTagName = "";
    this.ObjectList_WhereNID = "";
    this.ObjectList_WhereID = "";
    this.ObjectList_WhereParentNID = "";
    this.ObjectList_WhereSessionConfigNID = "";
    this.ObjectList_WhereTextSubStr = "";
    this.ObjectList_SearchTextSubStrIn = "";
    this.ObjectList_WhereDateRange = "";
    this.ObjectList_WhereTagExpr = "";
    this.ObjectList_WhereMetaDataExpr = "";
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
    this.ObjectList_WhereConfig = "";
    this.ObjectList_WherePermissions = "";
    this.ObjectList_WhereFilePermissions = "";
    this.ObjectList_WhereParentDirNID = "";
    this.ObjectList_WhereTitle = "";
    this.ObjectList_WhereStreamType = "";
    this.ObjectList_WhereDownloadState = "";
    this.ObjectList_subscriptionID = "";
    this.whereTCRange = "";
    this.whereFileNID = "";
    this.whereSessionInfo = "";

    this.ObjectList_WhereNameTextSubStr = "";
    this.ObjectList_WhereValueTextSubStr = "";

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
    this.ObjectList_srcEntityId = "";
    this.ObjectList_srcPortNID = "";
    this.ObjectList_srcPortTitle = "";
    this.ObjectList_srcPVREnable = "false";
    this.ObjectList_srcPVRTrimLength = "3600000";
    this.ObjectList_fileNID = "";
    this.ObjectList_fileEntityId = "";
    //Windowind Commands Parameter
    this.ObjectList_windowId = "";
    this.ObjectList_canvasId = "";
    this.ObjectList_restartSession = "";
    //Report Parameters
    this.ObjectList_name = "";
    this.ObjectList_description = "";
    this.ObjectList_format = "";
    this.ObjectList_enableExport = "";
    this.ObjectList_exportSchedule = "";
    this.ObjectList_exportURL = "";
    this.ObjectList_policyProperties = "";
    this.ObjectList_auditProperties = "";
    this.ObjectList_policyPermissions = "";
    this.ObjectList_WhereTarget = "";
    this.ObjectList_policyFormatXSL = "";
    this.ObjectList_auditFormatXSL = "";
    this.ObjectList_toFileName = "";
    // Add Device
    this.ObjectList_Title = "";
    this.ObjectList_DevicePrivateData = "";
    this.ObjectList_Password = "";
    this.ObjectList_UnManaged = "";
    this.ObjectList_UnManned = "";
    this.ObjectList_ApplyTagsToPorts = "";
    // new Set calls attributes
    this.ObjectList_ByNID = "";
    this.ObjectList_ByName = "";
    this.ObjectList_ByTitle = "";
    this.ObjectList_StreamURL = "";
    this.ObjectList_UserNID = "";
    this.ObjectList_ResourceNID = "";
    this.ObjectList_XAuth = "";
    this.ObjectList_Monitor = "";
    this.ObjectList_Enable = "";
    this.ObjectList_TrimLength = "";
    this.ObjectList_MediaStreamDirResourceNID = "";
    this.ObjectList_Path = "";
    this.ObjectList_StartIPAddress = "";
    this.ObjectList_EndIPAddress = "";
    //User
    this.ObjectList_WhereUsername = "";
    this.ObjectList_BypassAutoStorageRules = "";
    this.ExportPriority = "2";
  }
  
  this.setCallback = function(Event, callbackContext, callbackFunction) {
    if( callbackContext == null ) callbackContext = this;
    var cb = new Object;
    cb.context = callbackContext;
    cb.fnc = callbackFunction;
    this.callbackArr[Event] = cb;
  }
  
  this.destroy = function() {
    topic_Destroy(this.TopicName);
//    requestHandle = null;
  }
  this.appendQueryNode = function(requestNode) {
    // create and append a 'query' node
    var queryNode = requestNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "query");
    requestNode.appendChild(queryNode);
    // cascade call to add query request date
    this.appendRequestDataNode(queryNode);
  }
  this.replaceRequestDataObjectNode = function(requestNode) {
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
  
  this.appendRequestDataObjectNode = function(requestNode) {
    var NodeName = requestNode.getNodeName();
    var objectNode = this.RequestDataObjectArr[NodeName];
    if( objectNode != null ) {
      requestNode.appendChild(objectNode.cloneNode(true));
    }
    else {
      requestNode.setValue("");
    }
  }
  
  this.appendRequestDataOpWhereAssistXMLNode = function(requestNode) {
    var objectNode = this.Op_Where_WhereAssistXML;
    if( objectNode != null ) {
      requestNode.appendChild(objectNode.cloneNode(true));
    }
    else {
      requestNode.setValue("");
    }
  }

  this.appendRequestDataOpTypeNode = function(requestNode) {
    this.RequestDataOpTypeRuleName = this.OpType + "Request";
    var requestDataOpTypeDoc = utilRuleGetOutboundDoc.call(this, this.RequestDataOpTypeRuleName);
    if( requestDataOpTypeDoc != null ) {
      this.requestDataOpTypeNode = requestDataOpTypeDoc.selectSingleNode("/*").cloneNode(true);
      requestNode.appendChild(this.requestDataOpTypeNode); 
    }  
  }
  
  this.appendRequestDataNode = function(requestNode){
    if( this.RequestDataRuleName == null ) {
    /*
      // select request data rule
      if(this.ServiceName == "xmpp" || this.ServiceName == "mediaroomservice") {
        // select requestdatarule based on requestname
        this.RequestDataRuleName = this.RequestName + "Data";
      }
      else if( (this.ServiceName == "assetadminservice") || (this.ServiceName == "policyadminservice") ) {
        // select assetadmin requestdatarule based on op-type
        this.RequestDataRuleName = this.RequestName + "Data";
      }*/
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
  
  this.appendRequestNode = function(agentMessageDataNode){
    if( this.RequestRuleName == null ) {
      this.RequestRuleName = "ServiceAppletRequest";
    }
    var requestDoc = utilRuleGetOutboundDoc.call(this, this.RequestRuleName);
    if( requestDoc != null ) {
      this.requestNode = requestDoc.selectSingleNode("/*").cloneNode(true);
      agentMessageDataNode.appendChild(this.requestNode); 
    }
  }

  this.appendAgentEventNode = function (agentMessageDataNode) {
    var NodeName = agentMessageDataNode.getNodeName();
    var objectNode = this.AgentMessageDataObjectArr[NodeName];
    if (objectNode != null) {
      agentMessageDataNode.appendChild(objectNode);
    }
    else {
      agentMessageDataNode.setValue("");
    }
  }

  this.appendAgentMessageNode = function (agentMessageDataNode) {
    var NodeName = agentMessageDataNode.getNodeName();
    var objectNode = this.AgentMessageDataObjectArr[NodeName];
    if (objectNode != null) {
      agentMessageDataNode.appendChild(objectNode);
    }
    else {
      agentMessageDataNode.setValue("");
    }
  }

  this.send = function (requestor) {
      // Get request XML if it does not exist
      if (this.RequestXML == null) {
          if (AnimationMode == true) {
              this.RequestXML = '<AnimatedRequest/>';
          }
          else {
              // check and set requesthandle - possible race here
              //        if( requestHandle != null ) {
              //          this.callbackError("REQUESTFAIL", "Request Handle Locked: " + requestHandle);
              //          return;
              //        }
              //        requestHandle = this;
              // user request params to get requestname if not already specd
              if (this.RequestName == null) {
                  var ReqName = "";
                  if (this.ServiceName == "xmpp" ||
              this.ServiceName == "Discovery" ||
              this.ServiceName == "systemservice" ||
               this.ServiceName == "sessionservice.resource" ||
              this.ServiceName == "sessionservice.service" ||
              this.ServiceName == "mediaroomservice") {
                      if (this.OpType != "Operation") {
                          ReqName += this.OpType;
                      }
                      ReqName += this.ObjectType;
                      if (this.OpSpec != "NONE") {
                          ReqName += this.OpSpec;
                      }
                      ReqName += "Request";
                  }
                  else if (this.ServiceName == "assetadminservice") {
                      if (this.ObjectType == "EmptyRecycleBin") {
                          ReqName = "EmptyRecycleBinRequest";
                      } else {
                          ReqName = "AssetManagementRequest";
                      }
                  }
                  else if (this.ServiceName == "policyadminservice") {
                      ReqName = "PolicyManagementRequest";
                  }
                  this.RequestName = ReqName;
              }
              if (this.RequestName == "") {
                  this.callbackError("REQUESTFAIL", "Bad Request Info: \n" +
                              "ServiceName :" + this.ServiceName + "\n" +
                              "ObjectType :" + this.ObjectType + "\n" +
                              "OpType :" + this.OpType + "\n");
                  return;
              }
              // load requestheader rule - everything else should cascade from there

              var requestRuleName = "ServiceAppletRequest";
              //check if agent meesage need to be sent
              if (this.AgentMessageHeader != null) {
                  //agent message with Request
                  if (this.AgentMessageHeader.type == "Request") {
                      requestRuleName = "AgentMessage";
                      if ((this.ObjectType == "GetDevice" && this.RequestName == "GetDeviceRequest") || (this.ObjectType == "SetDeviceConfig" && this.RequestName == "SetDeviceConfigRequest")) {
                          this.RequestName = "AgentEventRequest";
                      }
                  }
                  //agent message with AgentEvent
                  else if (this.AgentMessageHeader.type == "Event") {
                      requestRuleName = "AgentEventMessage";
                      this.RequestName = "AgentEventRequest";
                  }
                  //agent message with Response
                  else if (this.AgentMessageHeader.type == "Response") {
                      requestRuleName = "AgentResponseMessage";
                      this.RequestName = "AgentEventRequest";
                  }
              }

              //check if session service - use IQ to send
              if (this.ServiceName == "sessionservice") {
                  requestRuleName = "SessionIQ";
              }

              // 5 login/logout requests   
              var loginReqtList = ['LoginRequest', 'SetServiceDomainRequest', 'GetServiceClientRequest', 'GetServerTimeRequest', 'LogoutRequest'];

              // 2 asset/policy management requests
              var managementReqList = ['AssetManagementRequest', 'PolicyManagementRequest'];
               
              // 2 getuser/getport requests
              var getUserPortReqList = ['GetUserRequest', 'GetPortRequest'];

              // 26 media tab requests
              var mediaReqList = [
                    'GetDirRequest', 'GetFileRequest', 'GetPVRFileRequest', 'QueueStatusExportImportFileRequest', 'SuspendExportImportFileRequest',
                    'ResumeExportImportFileRequest', 'ClearQueueExportImportFileRequest', 'DeleteExportImportFileRequest', 'UpdateExportImportFileRequest', 'RestoreMediaStreamFileRequest',
                    'DetachDirRequest', 'AttachDirRequest', 'EmptyDirRequest', 'CleanOrphanDirRequest', 'RepairMediaStreamFileRequest',
                    'UpgradeDirRequest', 'UpgradeFileRequest', 'SetFileGroupInfoRequest', 'SetFileInfoRequest', 'SetFileMetaDataRequest',
                    'SetFileBypassAutoStorageRulesRequest', 'DeleteFileRequest', 'ClearDownloadCacheRequest', 'GetBookmarkListRequest', 'SetDirStorageConfigRequest', 'AddExportFileRequest', 'AddImportFileRequest'
              ];

              // 18 device tab requests
              var deviceReqList = [
                    'GetDeviceRequest', 'GetDeviceConfigRequest', 'GetDeviceStatusRequest', 'AddDeviceRequest', 'SetDeviceInfoRequest', 'DeleteDeviceRequest',
                    'UpgradeDeviceRequest', 'StartDiscoveryServiceRequest', 'StopDiscoveryServiceRequest', 'SetPortInfoRequest', 'SetPortPVRInfoRequest', 'SetPortAutoStartRequest',
                    'SetPortObserverInfoRequest', 'SetPortStreamURLRequest', 'RebootDeviceRequest', 'RevertToDefaultDeviceRequest', 'SaveConfigDeviceRequest', 'ShutdownDeviceRequest', 'HATakeoverDeviceRequest'
              ];

              // 2 session tab requests
              var sessionReqList = ['GetSessionRequest', 'StopRequest'];

              // 11 system tab requests
              var systemReqLlist = [
                    'GetReportRequest', 'AddReportRequest', 'SetReportRequest', 'DeleteReportRequest', 'DownloadReportRequest', //report
                    'GetSystemConfigRequest', 'UpgradeRequest', 'SetMulticastIPAddressPoolRequest', //preferences
                    'RestoreDBRequest', 'ImportServiceDBRequest', 'BackupDatabaseRequest', 'SaveDBRequest', 'ExportServiceDBRequest', 'SetAutoDiscoveryRequest', 'SetWriterOptionsRequest' // backup database
              ];

              // 25 IPVSClient requests
              var ipvsclientReqList = [
                    'GetSessionConfigRequest', 'StartRequest', 'GetMediaRoomListRequest', 'GetBookmarkRequest', 'AddBookmark',
                    'DeleteBookmark', 'ResumeFromBookmarkRequest', 'UpdateBookmark', 'DeleteConnRequest', 'PauseRequest',
                    'PlayRequest', 'SkipBackRequest', 'SkipFwdRequest', 'CatchupRequest', 'PauseRecordRequest',
                    'ResumeRecordRequest', 'JoinRequest', 'GetPortDisplayStatusRequest', 'SetPortDisplayConfig', 'ViewRequest',
                    'GetPortTagsRequest', 'GetPortDisplayConfigRequest', 'PlayBackSrcRequest', 'PlayBackGrpRequest', 'SetPortWindowProperties'
              ];

              // 3 umc client requests
              var umcReqList = [
                    'SetServiceClientDisplayStatus', 'SetServiceClientDisplayConfig', 'AgentEventRequest'
              ];

              // 9 other qequests
              var otherReqList = [
                    'FileXferSetupRequest', 'FileXferStopRequest', 'DeviceConfigurationRequest', 'SetDeviceConfigRequest', 'DeviceCommandRequest',
                    'SetDeviceNetworkInterfaceRequest','SetDeviceXMPPAgentRequest','AuditEventRequest','GetTagsRequest'
              ];

              var allReqList = loginReqtList.concat(managementReqList, getUserPortReqList, mediaReqList, deviceReqList, sessionReqList, systemReqLlist, ipvsclientReqList, umcReqList, otherReqList);

              var allReqStr = allReqList.toString();

              if (allReqStr.search(this.RequestName) >= 0) {
                  //get rules from RequestRules.js
                  var requestJSON = com.ipvs.api.RequestRules.getRule(this);
                  this.RequestXML = xmlJsonClass.json2xml(requestJSON, "\t");
              }
              else {
                  log.error(' request ' + this.RequestName + 'is not existing!');
                  //old get rules XML
                  //var requestDoc = utilRuleGetOutboundDoc.call(this, requestRuleName);
                  //if (requestDoc != null) {
                  //  this.RequestXML = requestDoc.toString();
                  //}
              }

              if (this.ServiceName == "sessionservice") {
                  if (this.QueryType != "XMPPQuery") {
                      // add 'xmlns' to IPVS query node
                      var serviceVersion = topic_Get(tApp, "ServiceVersion");
                      this.RequestXML = (this.RequestXML).replace(/<query>/, "<query xmlns=\"com.ipvs.session\" serviceVer=\"" + serviceVersion + "\">");
                  }
              }

          }
      }
      // Send it - use pubsub to make this async
      topic_Publish(this.TopicName, "RequestResponseState", "REQUESTSEND");
  }
  
  this.eventHandler = function(objPub) {
    if(! objPub ) return;
    if( objPub.subject != this.TopicName ) return; 
    var EventType = objPub.stateVar;
    var EventVal = objPub.stateVal;
    if(EventType == "RequestResponseState") {
      // one last chance for hacks before the handler/callback function
      switch (EventVal) {
        case "REQUESTSEND":
          break;
        case "RESPONSERECEIVE":
          break;
        default:
          break;
      }
      // call the handler for the state - if no handler - barf    
      var handler = eval("this" + "." + EventVal);
      if( handler != null ) {
        handler.call(this, EventVal);
      }
      else {
        handleFatalError( "InternalError: Unhandled Event: " + EventType + " : " + EventVal + "\n" +
                               "RequestTopic: " + this.TopicName);
      }
    }
  }

  // default handling - can be overridden by specific request to do some processing
  this.REQUESTSEND = function() {
    if( this.RequestXML == null ) {
      this.callbackError("REQUESTFAIL", "No Request XML" );  
      return;
    }
    if( AppletRequestAlertMode == true ) {    
      log.info((this.RequestXML).replace(/[\n\r]/g," ").replace(/>\s+</g, "><"));
    } 
    this.ReturnXML = null;
    if( AnimationMode == true ) {
      this.ReturnXML = '<AnimatedReturn/>';
    }
    else if( SimulationMode == true ) {
      this.ReturnXML = '<SimulatedReturn/>';
    }
    else if(this.SimulateResponse == true){
      this.ReturnXML = this.SimulateResponseXML;
    }
    else {
      //      this.ReturnXML = applet_sendRequest(this.UserJID, this.RequestXML);
      this.ReturnXML = clsApplet.applet_sendRequest(this.UserJID, this.RequestXML);
    }
    if( this.ReturnXML == null ) {
      this.callbackError("REQUESTFAIL", "NULL Return from Applet SendRequest");  
      return;
    }
    // stash the return value in the topic 
    var returnDoc = new jsx3.xml.Document();
    returnDoc.loadXML(this.ReturnXML);
    var returnNode = returnDoc.getRootNode(); 
    // check for Error return
    //var returnErrorNode = returnNode.selectSingleNode("/Error");
    if (returnNode != null && returnNode.selectSingleNode("/Error")!=null) {
      this.callbackError("REQUESTFAIL", "REQUEST ERROR RETURN: " +  returnNode.toString() );  
      return;  
    }  
    if( AnimationMode == true ) {
      topic_Publish(this.TopicName, "RequestResponseState", "RESPONSERECIEVE");
    }
    else if( SimulationMode == true ) {
      var SimResponseName = "";
      if( this.ObjectType != "NONE" ) {        
        if(this.OpType != "Operation") {
          SimResponseName += this.OpType;
        }
        SimResponseName += this.ObjectType;
        if(this.OpSpec != "NONE") {
          SimResponseName += this.OpSpec;
        }
        SimResponseName += "Response";
      }
      else {
        SimResponseName = this.RequestName.replace(/Request$/, "Response");
      }
      if( fncSimulateAppletResponseByName(SimResponseName, this.TopicName) != true ) {
        this.callbackError("RESPONSEFAIL", "Error Simulating Response");
      }
    }
    else if(this.SimulateResponse == true){
      // set the current topicname in the xml
      var responseDoc = new jsx3.xml.Document();
      responseDoc.loadXML(this.SimulateResponseXML);
      // check if its a iq response
      var iqResponse = responseDoc.selectSingleNode("/iq");
      if( !jsx3.util.strEmpty(iqResponse) ){
        responseDoc.selectSingleNode("/iq").setAttribute("id",this.TopicName);
      }
      // check if its a regular response
      var regResponse = responseDoc.selectSingleNode(".//clientdata");
      if( !jsx3.util.strEmpty(regResponse) ){
        responseDoc.selectSingleNode(".//clientdata").setValue(this.TopicName);
      }        
    
      if( fncSimulateAppletResponse(responseDoc.toString(), this.TopicName) != true ) {
        this.callbackError("RESPONSEFAIL", "Error Simulating Response");
      }      
    }
  }
  this.RESPONSERECEIVE = function() {
    var responseNode = topic_GetNode(this.TopicName, "//ResponseNode/*");
    if (responseNode == null) {
      this.callbackError("RESPONSEFAIL", "No Response In Topic: " + this.TopicName);
    }
    this.ResponseXML = responseNode.toString();
      
    //CASE 1. for all new session API which response with iq node
    //iq is the root node of the responseNode
    if (responseNode.selectSingleNode("//iq") != null) {
      //remove xmlns from iqNode
      this.ResponseXML = (this.ResponseXML).replace(/xmlns=\"com.ipvs.session\"/g, "");
      responseNode = new jsx3.xml.Document();
      responseNode.loadXML(this.ResponseXML);
    }
    //CASE 2. for new session error handler which response with query and error nodes inside of iq node
    if (responseNode.selectSingleNode("./error") != null) {
      //if error node is a direct child of iq node
      var ipvsErrCodeNode = responseNode.selectSingleNode(".//ipvserror");
      var ipvsErrCode = ipvsErrCodeNode == null ? "" : ipvsErrCodeNode.getAttribute("code");
      var ipvsErrStrNode = responseNode.selectSingleNode(".//description");
      var ipvsErrStr = ipvsErrStrNode == null ? "" : ipvsErrStrNode.getValue();
      this.callbackError("RESPONSEFAIL", "Bad Response State: IPVS error code:" + ipvsErrCode + "; Error:" + ipvsErrStr + ".");
      return;
    }    
    //CASE 3. for all none session API responses which without any iq node
    if (responseNode.selectSingleNode(".//header/state") != null) {
      // check the state in the response - if 0 - simple pass the raw data thru to the caller
      var ResponseHeaderState = responseNode.selectSingleNode(".//header/state").getValue();
      if (ResponseHeaderState == null || ResponseHeaderState == 0) {
        this.callbackError("RESPONSEFAIL", "Bad Response State: " + ResponseHeaderState);
        return;
      }
    }
    //process response
    this.handleResponse(responseNode);
  }
  
  this.callbackError = function(ErrCode, ErrStr) {
    ErrStr = "Request: " + this.RequestName + " : " + ErrStr + "\n" +
            " RequestXML: " + this.RequestXML + "\n" +
            " ReturnXML: " + this.ReturnXML + "\n" +
            " ResponseXML: " + this.ResponseXML + "\n";   
    this.callback(ErrCode, ErrStr);  
  }

  this.callback = function(Code, Data) {
    if( this.callbackArr[Code] != null ) { 
      this.callbackArr[Code].fnc.call(this.callbackArr[Code].context, this, Code, Data);
    }
    else {
      handleFatalError(Code + " : " + Data);
      this.destroy();
    }  
  }

/*
  common response handler
*/
// this should be split across specific requests that override a base
  this.handleResponse = function(responseNode) {
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
          // process response based on responsetype .//GetResponse 
          if (responseDataNode = responseNode.selectSingleNode(".//GetResponse")) {
            response = this.handleGetResponse(responseDataNode);
          }
          else if( responseDataNode = responseNode.selectSingleNode(".//AddResponse") ) {
            response = this.handleAddResponse(responseDataNode);
          }
          else if( responseDataNode = responseNode.selectSingleNode(".//DeleteResponse") ) {
            response = this.handleDeleteResponse(responseDataNode);
          }
          else if( responseDataNode = responseNode.selectSingleNode(".//ReplaceResponse") ) {
            response = this.handleReplaceResponse(responseDataNode);
          }
          else {
            if( responseNode.selectSingleNode(".//data") != null ) {
              responseNode = responseNode.selectSingleNode(".//data/*");
            }
            else if (responseNode.selectSingleNode(".//query") != null) {
              responseNode = responseNode.selectSingleNode(".//query/*"); //".//jsxnsIPVSSessionService:query/*"
            }
            response = (new jsx3.xml.Document()).loadXML(responseNode.toString());
//            response = responseNode.cloneNode(true);
          }
          break;
      }
    }
    this.callback("SUCCESS", response);
  }

/*
  update the cache with the elements returned
  push the list of NIDs in the request-specific topic cache
  push any non-NID type return elements in topiccache as well
*/
  this.handleGetResponse = function(responseDataNode) {
    //check that this get response is for session API
    var ObjectListNode = responseDataNode.selectSingleNode(".//ObjectList");
    if( ObjectListNode != null ) return ObjectListNode;
  // create root element for NIDlist 
    var objectNIDListNode = new jsx3.xml.Document().createDocumentElement("ResponseObjectNIDList");
    //add objectcount and counttofetch as attributes to root element
    var objectNIDListCountNode = responseDataNode.selectSingleNode(".//GetListObjectResponseData/ObjectCount");
    var objectNIDListStartFromNode = responseDataNode.selectSingleNode(".//GetListObjectResponseData/ObjectLastWhereEndCount");
    if( objectNIDListCountNode != null ) {
      var objectNIDListCount = objectNIDListCountNode.getValue();
      var objectNIDListStartFrom = objectNIDListStartFromNode.getValue();
      objectNIDListNode.setAttribute("ObjectCount", objectNIDListCount);
      objectNIDListNode.setAttribute("ListStartFrom", objectNIDListStartFrom);
    }
  // for each element in response - add it to cache and add its NID to NIDList  
    var elementIter = responseDataNode.selectNodeIterator(".//ResponseXML/*");
    while(elementIter.hasNext()) {
      var elementNode = elementIter.next().cloneNode(true);
      var ElementNID = elementNode.getAttribute("NID");
      if( ElementNID != null ) {
        setObjectCacheNode(elementNode);
        var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
        objectNIDListNode.appendChild(objectNIDNode);
      }
      else {
        objectNIDListNode.appendChild(elementNode);
      }
    }
    return objectNIDListNode;
  }

/*
  ideally the add response should not touch the cache
  instead update notifications from the DB should make that happen
  for now we cheat and treat the Add request as if it was followed by an automatic Get request
*/
  this.handleAddResponse = function(responseDataNode) {
    var objectNIDListNode = this.handleGetResponse(responseDataNode);
  /*
  // create root element for NIDlist 
    var objectNIDListNode = new jsx3.xml.Document().createDocumentElement("ResponseObjectNIDList");
  // for each element in response - add it to cache and add its NID to NIDList  
    var elementIter = responseDataNode.selectNodeIterator(".//ResponseXML/*");
    while(elementIter.hasNext()) {
      var elementNode = elementIter.next().cloneNode(true);
      var ElementNID = elementNode.getAttribute("NID");
      if( ElementNID != null ) {
        var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
        objectNIDListNode.appendChild(objectNIDNode);
      }
    }
  */  
    return objectNIDListNode;
  }
/*
  delete the elements from the cache
  push the list of deleted NIDs in the request-specific topic
*/
  this.handleDeleteResponse = function(responseDataNode) {
  // create root element for NIDlist
    var objectNIDListNode = new jsx3.xml.Document().createDocumentElement("ResponseObjectNIDList");
  // for each element in response - add it to cahce and add its NID to requestspecific topiccache  
    var elementIter = responseDataNode.selectNodeIterator(".//ResponseXML/*");
    while(elementIter.hasNext()) {
      var elementNode = elementIter.next().cloneNode(true);
      var ElementNID = elementNode.getAttribute("NID");
      if( ElementNID != null ) {
        var elementCacheNode = topic_GetNIDNode(tCache, ElementNID);
        if(elementCacheNode != null) {
         // elementCacheNode.getParent().removeChild(elementCacheNode);
        }
        var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
        objectNIDListNode.appendChild(objectNIDNode);
      }
    }
    return objectNIDListNode;
  }
/*
  update the cache with the elements returned
  push the list of NIDs in the request-specific topic cache
*/
  this.handleReplaceResponse = function(responseDataNode) {
  // create root element for NIDlist
    var objectNIDListNode = new jsx3.xml.Document().createDocumentElement("ResponseObjectNIDList");
  // for each element in response - add it to cahce and add its NID to requestspecific topiccache  
    var elementIter = responseDataNode.selectNodeIterator(".//ResponseXML/*");
    while(elementIter.hasNext()) {
      var elementNode = elementIter.next().cloneNode(true);
      var ElementNID = elementNode.getAttribute("NID");
      if( ElementNID != null ) {
        var elementCacheNode = topic_GetNIDNode(tCache, ElementNID);
        if(elementCacheNode != null) {
          elementCacheNode.getParent().replaceNode(elementNode, elementCacheNode);
        }
        var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
        objectNIDListNode.appendChild(objectNIDNode);
      }
    }
    return objectNIDListNode;
  }

}


