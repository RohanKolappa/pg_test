//---------------------------------------
// base request class - sub clss for different types
function Request (RequestType, SuccessEvent, FailEvent) {
  if( RequestType != null ) {
    this.RequestType = RequestType;
    this.SuccessEvent = SuccessEvent;
    this.FailEvent = FailEvent;
    // setup request lock
    this.req = null;
    this.RetryTimeout = null;
    this.TimerHandle = null;
  }

  // stash value of this in local var for access from timer function
  var ThisClass = this;
  
  // request cleanup code
  this.endRequest = function() {
    if( this.req != null ) {
      this.req.destroy();
      this.req = null;
    } 
  }

  // define init
  this.initRequest = function() {
    // get new request handle
    var UserJID = topic_Get(tApp, "LoginUserJID");
    if( UserJID == null ) {
      UserJID = "NONE";
    }
    this.req = eval("new " + this.RequestType + "(UserJID)");
    if( this.ReqEvent != null ) {
      if( this.ReqEvent.action == "APPEVENT" ) {
        this.ReqEvent.topic = tApp;
      }
    }
    if( this.SuccessEvent != null ) {
      if( this.SuccessEvent.action == "APPEVENT" ) {
        this.SuccessEvent.topic = tApp;
      }
      if( this.SuccessEvent.action == "APPEVENT" || this.SuccessEvent.action == "TOPICEVENT" ) {
       // if( topic_Get(this.SuccessEvent.topic, this.SuccessEvent.eventvar) == null ) {
       //   topic_Set(this.SuccessEvent.topic, this.SuccessEvent.eventvar, "NONE");
       // }
      }
    }
    if( this.FailEvent != null ) {
      if( this.FailEvent.action == "APPEVENT" ) {
        this.FailEvent.topic = tApp;
      }
    }
    return true;
  }
   
  // define send
  this.sendRequest = function() {
    if( this.RetryTimeout != null ) {
      this.TimerHandle = setInterval(this.handleTimeout, this.RetryTimeout);
    }
    // setup callbacks
    this.req.setCallback("SUCCESS", this, this.onSuccess);
    this.req.setCallback("RESPONSEFAIL", this, this.onFail);
    // send req
    this.req.send();
    // potentially need to cleanup here - if send fails such that no callback will happen 
  }

  // to resend the same request
  this.resendRequest = function() {
    if( this.req != null ) {
      // send req
      this.req.send();
    }
  }

  this.handleTimeout = function() {
    ThisClass.resendRequest.call(ThisClass);
  }
  
  // define onSuccess - should be overridden by sub class
  this.onSuccess = function(req, ErrCode, response) {
    // fire request and/or success events
    var handled = false;
    if( this.TimerHandle != null ) {
      clearInterval(this.TimerHandle);
    }
    if( this.ReqEvent != null ) {
      this.doEvent(this.ReqEvent, ErrCode, response);
      handled = true;
    }
    if( this.SuccessEvent != null ) {
      this.doEvent(this.SuccessEvent, ErrCode, response);
      handled = true;
    }
    // log only if unhandled
    if( handled != true ) {
      log.warn("Unhandled Success Response: \n" + response.toString());
    } 
    // making sure the topic is still valid. This is being done for quick click/select switching 
    if( (req.TopicName == null) || (! topic_Exists(req.TopicName)) ) {
      log.warn("clsRequest::Request::ONSUCESS:: TopicName :" + req.TopicName + " doesnt not exist, drop its response.\n");
      return ;
    }            
    this.endRequest();
  }
  //define onFail - may be overridden by sub class
  this.onFail = function(req, ErrCode, response) {
    if( this.TimerHandle != null ) {
      clearInterval(this.TimerHandle);
    }
    // log the raw failure
    log.error("Fail Response: \n" + ErrCode + " : " + response);
    
    // TBD - parse the response and get the error "code" an look up errcode table
    //     - based on that set ErrCode to one of ERROR_REQUEST, ERROR_DB, ERROR_POLICY
    ErrCode = "ERROR";
    if( response.match(/ResponseXML/) ) {
      var ResponseXML = (response.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
      var responseDoc = new jsx3.xml.Document;
      responseDoc.loadXML(ResponseXML);
      try {
        var codeNode = responseDoc.selectSingleNode(".//Code");
        if( jsx3.util.strEmpty(codeNode) ){
          var ipvsErrorNode = responseDoc.selectSingleNode(".//ipvserror");
          if( !jsx3.util.strEmpty(ipvsErrorNode) ){
            ErrCode = ipvsErrorNode.getAttribute("code");
          }       
        }
        else {
          ErrCode = codeNode.getValue();
        }
         
      } catch(e) {
      }
    }    
    
    // fire request and/or fail events
    if( this.ReqEvent != null ) {
      this.doEvent(this.ReqEvent, ErrCode, response);
    }
    if( this.FailEvent != null ) {
      this.doEvent(this.FailEvent, ErrCode, response);
    }
    // log the request
    log.warn("ONFAIL ENDING RequestName: \n" );    
    this.endRequest();
  }
  // define doevent - similar to onSelect in component
  this.doEvent = function(Event, RespCode, RespVal) {
    if( Event == null ) return;
    // call/fire event
    if( Event.action == "APPEVENT" || Event.action == "TOPICEVENT" ) {
      var eventval = Event.eventval;
      if( Event.eventval == "DATE" ) {
        eventval = newServerDate();
      }
      topic_Publish(Event.topic, Event.eventvar, eventval);
    }
    else if( Event.action == "FNCCALL" ) {
      //jsx3.sleep((Event.fnc), null, Event.context);
      if( Event.context != null ) {
        (Event.fnc).call(Event.context, RespCode, RespVal);
      }
      else {
        (Event.fnc).call(this, RespCode, RespVal);
      }
    }  
  }
}
//---------------------------------------
ReqLogin.prototype = new Request();
function ReqLogin () {
// setup inheritence and call base constructor
  ReqLogin.BaseClass = ReqLogin.prototype;
  ReqLogin.prototype.constructor = ReqLogin;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.obj = arrServiceObjectInfo["Login"];
  if( this.obj != null ) {
    this.SuccessEvent = this.obj.successevent;
    this.FailEvent = this.obj.failevent;
  }
 
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "xmpp";
    this.req.ObjectType = "Login";
    this.req.OpType = "Operation";
    this.req.User = topic_Get(tApp, "LoginUser");
    this.req.Password = topic_Get(tApp, "LoginPassword");
    this.req.Domain = topic_Get(tApp, "LoginDomain");
    this.req.Resource = topic_Get(tApp, "LoginResource");
    this.req.Server = topic_Get(tApp, "LoginServer");
    this.req.Port = topic_Get(tApp, "LoginPort");
    ReqLogin.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    topic_Set(tApp, "LoginUserJID", response);
    // extracting domain from the login bug 18945
    var LoginUserDomain = response.substring(response.indexOf("@")+1, response.indexOf("/"));
    topic_Set(tApp, "LoginUserDomain", LoginUserDomain);
    ReqLogin.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  
  this.onFail = function(req, ErrCode, response) {
    // check for duplicate login - if so change fail eventval
    if( response.match(/ResponseXML/) ) {
      var ResponseXML = (response.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
      var responseDoc = new jsx3.xml.Document;
      responseDoc.loadXML(ResponseXML);
      var nodeResponseLogCode = responseDoc.selectSingleNode(".//log/Code");
      if( nodeResponseLogCode != null && nodeResponseLogCode.getValue() == "1004" ) {
        var ResponseLog = responseDoc.selectSingleNode(".//log").getValue();
        ResponseLog = ResponseLog.split("Duplicate");
        var LoginUserJID = ResponseLog[0].replace(/\d+\s(.*)\s/, "$1");
        topic_Set(tApp, "LoginUserJID", LoginUserJID);
        this.FailEvent.eventval = "DUPLICATE";
      } 
    }
    ReqLogin.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

/*<log>
                <Code>1200</Code>
                <Description>Schema Version Mismatch</Description>*/

//---------------------------------------
ReqLogout.prototype = new Request();
function ReqLogout () {
// setup inheritence and call base constructor
  ReqLogout.BaseClass = ReqLogout.prototype;
  ReqLogout.prototype.constructor = ReqLogout;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.obj = arrServiceObjectInfo["Logout"];
  if( this.obj != null ) {
    this.SuccessEvent = this.obj.successevent;
    this.FailEvent = this.obj.failevent;
  }
 
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
	  this.req.userJID = topic_Get(tApp, "LoginUserJID");
    this.req.ServiceName = "xmpp";
    this.req.ObjectType = "Logout";
    this.req.OpType = "Operation";
    ReqLogout.BaseClass.sendRequest.call(this);
  }
}

//---------------------------------------
ReqSetServiceDomain.prototype = new Request();
function ReqSetServiceDomain (ReqEvent) {
// setup inheritence and call base constructor
  ReqSetServiceDomain.BaseClass = ReqSetServiceDomain.prototype;
  ReqSetServiceDomain.prototype.constructor = ReqSetServiceDomain;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.obj = arrServiceObjectInfo["ServiceDomain"];
  if( this.obj != null ) {
    this.SuccessEvent = this.obj.successevent;
    this.FailEvent = this.obj.failevent;
  }
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = "SetServiceDomain";
    this.req.OpType = "Operation";
    this.req.ServiceDomain = topic_Get(tApp, "ServiceDomain");
    this.req.AssetTemplateTitle = topic_Get(tApp, "AssetTemplateTitle");
    this.req.AssetPrivateKeyScheme = topic_Get(tApp, "AssetPrivateKeyScheme");
    this.req.AssetPrivateKeyData = topic_Get(tApp, "AssetPrivateKeyData");
    this.req.AssetPrivateData = topic_Get(tApp, "AssetPrivateData");
    this.req.ServiceVersion = topic_Get(tApp, "ServiceVersion");
    this.req.IPAddress = topic_Get(tApp, "ClientIPAddress");
    ReqSetServiceDomain.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var respServiceRoot = response.selectSingleNode("//ServiceRoot");
    if( respServiceRoot != null ) {
      var cacheServiceRoot = topic_GetNode(tCache, "//ServiceRoot");
      if( cacheServiceRoot == null ) {
        cacheServiceRoot = topic_Set(tCache, "ServiceRoot", null);
      }
      cacheServiceRoot.getParent().replaceNode(respServiceRoot.cloneNode(true), cacheServiceRoot);
    }
    
    var respAppServerAgentJID = response.selectSingleNode("//appServerAgentJID");
    if( respAppServerAgentJID != null ) {
      var AppServerAgentJID = respAppServerAgentJID.getValue();
      topic_Set(tApp, "AppServerAgentJID", AppServerAgentJID);
      var msIP = AppServerAgentJID.substring(AppServerAgentJID.indexOf("@") + 1, AppServerAgentJID.indexOf("/"));
      topic_Set(tApp, "MSServerIP", msIP);
    }
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSetServiceDomain.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // check for service version mismatch error - if so change fail eventval
    if( response.match(/ResponseXML/) ) {
      var ResponseXML = (response.replace(/\n/g, ' ')).replace(/.*ResponseXML:(.*)/, "$1");
      var responseDoc = new jsx3.xml.Document;
      responseDoc.loadXML(ResponseXML);
      var nodeResponseLogCode = responseDoc.selectSingleNode(".//log/Code");
      if( nodeResponseLogCode != null ) {
        this.FailEvent.eventval = nodeResponseLogCode.getValue();
      } 
    }
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSetServiceDomain.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqCreateConnection.prototype = new Request();
function ReqCreateConnection (SourceNID, DestNID, RelayNID, StartOffset, ProfileNID, profileNode, RxMediaIpAddress, RxMediaPort, ReqEvent) {
// setup inheritence and call base constructor
  ReqCreateConnection.BaseClass = ReqCreateConnection.prototype;
  ReqCreateConnection.prototype.constructor = ReqCreateConnection;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.SourceNID = SourceNID;
  this.DestNID = DestNID;
  this.RelayNID = RelayNID;
  this.StartOffset = StartOffset;
  this.ProfileNID = ProfileNID;
  this.profileNode = profileNode;
  this.RxMediaIpAddress = RxMediaIpAddress;
  this.RxMediaPort = RxMediaPort; 
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "CreateConnection";
    this.req.OpType = "Operation";

    this.req.SourceNID = this.SourceNID;
    this.req.DestNID = this.DestNID;
    this.req.RelayNID = this.RelayNID;
    this.req.StartOffset = this.StartOffset;
    this.req.ProfileNID = this.ProfileNID;
    if( this.profileNode == null ) {
      this.profileNode = topic_GetNIDNode(tCache, this.req.ProfileNID);
    }
    if( this.profileNode != null ) {
      this.req.RequestDataObjectArr = new Object;
      this.req.RequestDataObjectArr["profileXML"] = this.profileNode;
    }
    this.req.RxMediaIpAddress = this.RxMediaIpAddress;
    this.req.RxMediaPort = this.RxMediaPort; 
    ReqCreateConnection.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    var roomNIDNode = response.selectSingleNode("//roomNID");
    if( roomNIDNode != null ) {
      RespEventVal = roomNIDNode.getValue();
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqCreateConnection.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqCreateConnection.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetMediaRoom.prototype = new Request();
function ReqGetMediaRoom (RoomNID, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetMediaRoom.BaseClass = ReqGetMediaRoom.prototype;
  ReqGetMediaRoom.prototype.constructor = ReqGetMediaRoom;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.RoomNID = RoomNID;
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "GetMediaRoom";
    this.req.OpType = "Operation";

    this.req.RoomNID = this.RoomNID;
    
    ReqGetMediaRoom.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // check room nid
    var nodeRoom = response.selectSingleNode("//MediaRoom");
    if(nodeRoom != null){
      setObjectCacheNode(nodeRoom);
      // setup this req as the NID specific req to fire during cache sync
      arrServiceObjectInfo["MediaRoom"][this.RoomNID] = new Object;
      arrServiceObjectInfo["MediaRoom"][this.RoomNID].req = this;
    }
    else{
      var cacheMediaRoom = topic_GetNIDNode(tCache, this.RoomNID);
      if(cacheMediaRoom != null) {
        cacheMediaRoom.getParent().removeChild(cacheMediaRoom);
      }
      arrServiceObjectInfo["MediaRoom"][this.RoomNID] = null;
    }
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetMediaRoom.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetMediaRoom.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetAgentObject.prototype = new Request();
function ReqGetAgentObject (AgentJID, AgentReqObj, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetAgentObject.BaseClass = ReqGetAgentObject.prototype;
  ReqGetAgentObject.prototype.constructor = ReqGetAgentObject;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.AgentJID = AgentJID;
  this.agentObj = AgentReqObj;
  if( this.agentObj != null ) {
    if( this.agentObj.successevent != null) {
      this.SuccessEvent = {"action":this.agentObj.successevent.action, "topic":AgentJID,
                           "eventvar":this.agentObj.successevent.eventvar, 
                           "eventval":this.agentObj.successevent.eventval};
    }
    if( this.agentObj.failevent != null) {
      this.FailEvent = {"action":this.agentObj.failevent.action, "topic":AgentJID,
                        "eventvar":this.agentObj.failevent.eventvar,
                        "eventval":this.agentObj.failevent.eventval};
    }
  }
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function () {
      // check if already processing this req
      if (this.req != null) return false;
      // if not start a new req
      this.initRequest();
      // set req params
      this.req.ServiceName = this.agentObj.reqServiceName;
      this.req.ObjectType = this.agentObj.reqObjectType;
      this.req.OpType = "Operation";
      if (this.agentObj.reqParams != null) {
          for (var rpName in this.agentObj.reqParams) {
              this.req[rpName] = this.agentObj.reqParams[rpName];
          }
      }
      //set agent message parameters
      this.req.AgentMessageHeader = new Object;
      //    this.req.AgentMessageHeader.agentJID = this.AgentJID;
      //    this.req.AgentMessageHeader.fromagent = this.agentObj.reqFromAgentName;
      //    this.req.AgentMessageHeader.agent = this.agentObj.reqAgentName;  
      //    this.req.AgentMessageHeader.agentmessagetype = "Request";
      
      //ref. sotry #46274 new agent message header format
      this.req.AgentMessageHeader.from = topic_Get(tApp, "LoginUserJID");
      this.req.AgentMessageHeader.to = this.AgentJID;
      this.req.AgentMessageHeader.type = "Request";
      this.req.AgentMessageHeader.agent = this.agentObj.reqAgentName;
      this.req.AgentMessageHeader.fromagent = this.agentObj.reqFromAgentName;
      ReqGetAgentObject.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // put data from response into agent specific topic doc
    var agentObjtopicCacheRoot  = this.agentObj.cacheRoot;
    //To handle GetMediaRoomResponse data
    if( agentObjtopicCacheRoot == "MediaRoomListGetResponse" ) {
      var responseNode = response.selectSingleNode(".//ResponseXML");
    }
    else {
      var responseNode = response.selectSingleNode(agentObjtopicCacheRoot);
    }

    var TopicName = this.AgentJID;
    if(! topic_Exists(TopicName)) {
    } else {
      var agentObjtopicNode = topic_GetNode(TopicName, "//"+agentObjtopicCacheRoot);
      if( responseNode == null) {
        log.warn("Empty GET Agent Response: \n" + response.toString());
      } else {
        if(agentObjtopicNode == null) {
          agentObjtopicNode = topic_Set(TopicName, agentObjtopicCacheRoot, null);
        } else {
          agentObjtopicNode.removeChildren()
        }
        //set the StateImage Attribute in the DeviceStatus.
        // Its value is based on the State attrubute
        var deviceStatusState = responseNode.getAttribute("State");
        if( !jsx3.util.strEmpty(deviceStatusState) ){
          if( deviceStatusState == "OK" ) {
            responseNode.setAttribute("StateImage", "images/icons/icon_greenBall.png"); 
            responseNode.setAttribute("Status", "Ready");     
          }
          else if( deviceStatusState == "ERRORS" ) {
            responseNode.setAttribute("StateImage", "images/icons/icon_orangeBall.png");
            responseNode.setAttribute("Status", "Ready");
          }
          else if( deviceStatusState == "FAILED" ) {
            responseNode.setAttribute("StateImage", "images/icons/icon_redBall.png");
            responseNode.setAttribute("Status", "FAILED");
          }
        }
        agentObjtopicNode.appendChild(responseNode);
      }
    }
    //var agentObjtopicNode = topic_GetNode(TopicName, "//"+agentObjtopicCacheRoot);
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetAgentObject.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqGetAgentObject.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqSendCommandAgentObject.prototype = new Request();
function ReqSendCommandAgentObject (AgentJID, AgentReqDataNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqSendCommandAgentObject.BaseClass = ReqSendCommandAgentObject.prototype;
  ReqSendCommandAgentObject.prototype.constructor = ReqSendCommandAgentObject;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.AgentJID = AgentJID;
  this.AgentReqDataNode = AgentReqDataNode;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = "DeviceCommand";
    this.req.OpType = "Operation";
    if( this.AgentReqDataNode != null ) {
      this.req.RequestDataObjectArr = new Object;
      this.req.RequestDataObjectArr["DeviceCommandRequestData"] = this.AgentReqDataNode;
    }    
    //set agent message parameters
    this.req.AgentMessageHeader = new Object;
//    this.req.AgentMessageHeader.agentJID = this.AgentJID;
//    this.req.AgentMessageHeader.fromagent = "com.ipvs.common.service.DeviceAdminClientAgent";  
//    this.req.AgentMessageHeader.agent = "com.ipvs.common.service.DeviceAdminServerAgent";
//    this.req.AgentMessageHeader.agentmessagetype = "Request";

    //ref. sotry #46274 new agent message header format
    this.req.AgentMessageHeader.from = topic_Get(tApp, "LoginUserJID");
    this.req.AgentMessageHeader.to = this.AgentJID;
    this.req.AgentMessageHeader.type = "Request";
    this.req.AgentMessageHeader.fromagent = "com.ipvs.common.service.DeviceAdminClientAgent"; 
    this.req.AgentMessageHeader.agent = "com.ipvs.common.service.DeviceAdminServerAgent";
    ReqSendCommandAgentObject.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqSendCommandAgentObject.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqSendCommandAgentObject.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqSendConfiguarationAgentObject.prototype = new Request();
function ReqSendConfiguarationAgentObject (AgentJID, AgentReqDataNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqSendConfiguarationAgentObject.BaseClass = ReqSendConfiguarationAgentObject.prototype;
  ReqSendConfiguarationAgentObject.prototype.constructor = ReqSendConfiguarationAgentObject;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.AgentJID = AgentJID;
  this.AgentReqDataNode = AgentReqDataNode;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = "DeviceConfiguration";
    this.req.OpType = "Operation";
    if( this.AgentReqDataNode != null ) {
      this.req.RequestDataObjectArr = new Object;
      this.req.RequestDataObjectArr["DeviceConfigurationRequestData"] = this.AgentReqDataNode;
    }    
    //set agent message parameters
    this.req.AgentMessageHeader = new Object;
//    this.req.AgentMessageHeader.agentJID = this.AgentJID;
//    this.req.AgentMessageHeader.fromagent = "com.ipvs.common.service.DeviceAdminClientAgent";  
//    this.req.AgentMessageHeader.agent = "com.ipvs.common.service.DeviceAdminServerAgent";
//    this.req.AgentMessageHeader.agentmessagetype = "Request";

    //ref. sotry #46274 new agent message header format
    this.req.AgentMessageHeader.from = topic_Get(tApp, "LoginUserJID");
    this.req.AgentMessageHeader.to = this.AgentJID;
    this.req.AgentMessageHeader.type = "Request";
    this.req.AgentMessageHeader.fromagent = "com.ipvs.common.service.DeviceAdminClientAgent";
    this.req.AgentMessageHeader.agent = "com.ipvs.common.service.DeviceAdminServerAgent";
    ReqSendConfiguarationAgentObject.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqSendConfiguarationAgentObject.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqSendConfiguarationAgentObject.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqSendAgentEventMessageObject.prototype = new Request();
function ReqSendAgentEventMessageObject(AgentJID, AgentEventNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqSendAgentEventMessageObject.BaseClass = ReqSendConfiguarationAgentObject.prototype;
  ReqSendAgentEventMessageObject.prototype.constructor = ReqSendConfiguarationAgentObject;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.AgentJID = AgentJID;
  this.AgentEventNode = AgentEventNode;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = "AgentEvent";
    this.req.OpType = "Operation";
    if( this.AgentEventNode != null ) {
      this.req.AgentMessageDataObjectArr = new Object;
      this.req.AgentMessageDataObjectArr["agentmessagedata"] = this.AgentEventNode;
    }    
    //set agent message parameters
    this.req.AgentMessageHeader = new Object;
//    this.req.AgentMessageHeader.agentJID = this.AgentJID;
//    this.req.AgentMessageHeader.fromagent = "";  
//    this.req.AgentMessageHeader.agent = "com.ipvs.mediaroomservice.impl.DestStreamAgent";
//    this.req.AgentMessageHeader.agentmessagetype = "AgentEvent";

    //ref. sotry #46274 new agent message header format
    this.req.AgentMessageHeader.from = topic_Get(tApp, "LoginUserJID");
    this.req.AgentMessageHeader.to = this.AgentJID;
    this.req.AgentMessageHeader.type = "Event";
    this.req.AgentMessageHeader.fromagent = "";
    this.req.AgentMessageHeader.agent = "com.ipvs.mediaroomservice.impl.DestStreamAgent";
    ReqSendAgentEventMessageObject.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSendAgentEventMessageObject.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqSendAgentEventMessageObject.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqSaveRestoreDB.prototype = new Request();
function ReqSaveRestoreDB (ArchiverName, ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqSaveRestoreDB.BaseClass = ReqSaveRestoreDB.prototype;
  ReqSaveRestoreDB.prototype.constructor = ReqSaveRestoreDB;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ArchiverName = ArchiverName;
  this.ObjectType = ObjectType;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Operation";
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    
    ReqSaveRestoreDB.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqSaveRestoreDB.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    ReqSaveRestoreDB.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
//AddExport, AddImport, GetExportImport
//ClearQueue, Suspend, Resume, Delete, GetQueueStatus
ReqExportImportMedia.prototype = new Request();
function ReqExportImportMedia (ObjectType, ObjNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqExportImportMedia.BaseClass = ReqExportImportMedia.prototype;
  ReqExportImportMedia.prototype.constructor = ReqExportImportMedia;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.obj = arrServiceObjectInfo["ExportImportFileListItem"];
  this.ObjectType = ObjectType;
  this.ReqEvent = ReqEvent;
  this.ObjNode = ObjNode;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "assetadminservice";
    this.req.ObjectType = this.ObjectType;
    this.req.RequestName = this.ObjectType + "Request";
    this.req.OpType = "Operation";
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }   
        
    ReqExportImportMedia.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    if (this.ObjectType == "GetExportImportFile") {
      this.obj.listreqstate = newServerDate();
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }  
    ReqExportImportMedia.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    ReqExportImportMedia.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//UpdateExportImportFile
/*
 @EIFLIObj -- object
 EIFLIObj.Priority;
 EIFLIObj.ExternalFileName;
 EIFLIObj.ExternalJID;
 EIFLIObj.ExternalFolderPath;
*/
ReqUpdateExportImportMedia.prototype = new Request();
function ReqUpdateExportImportMedia (EIFLIObj, ReqEvent) {
// setup inheritence and call base constructor
  ReqUpdateExportImportMedia.BaseClass = ReqUpdateExportImportMedia.prototype;
  ReqUpdateExportImportMedia.prototype.constructor = ReqUpdateExportImportMedia;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  if( jsx3.util.strEmpty(EIFLIObj) ) {
    EIFLIObj = new Object;
    EIFLIObj.Priority = "";
    EIFLIObj.ExternalFileName = "";
    EIFLIObj.ExternalJID = "";
    EIFLIObj.ExternalFolderPath = "";
  }
  this.Priority = EIFLIObj.Priority;
  this.ExternalFileName = EIFLIObj.ExternalFileName;
  this.ExternalJID = EIFLIObj.ExternalJID;
  this.ExternalFolderPath = EIFLIObj.ExternalFolderPath;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "assetadminservice";
    this.req.ObjectType = "UpdateExportImportFile";
    this.req.RequestName = "UpdateExportImportFileRequest";
    this.req.OpType = "Operation";
    this.req.Priority = this.Priority;
    this.req.ExternalFileName = this.ExternalFileName;
    this.req.ExternalJID = this.ExternalJID;
    this.req.ExternalFolderPath = this.ExternalFolderPath;  
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }       
    ReqUpdateExportImportMedia.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqUpdateExportImportMedia.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    ReqUpdateExportImportMedia.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqFileTransfer.prototype = new Request();
function ReqFileTransfer (ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqFileTransfer.BaseClass = ReqFileTransfer.prototype;
  ReqFileTransfer.prototype.constructor = ReqFileTransfer;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ObjectType = ObjectType;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Operation";
    this.req.DeviceType = this.reqParams["DeviceType"];
    this.req.LocalFileName = this.reqParams["LocalFileName"];
    
    ReqFileTransfer.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqFileTransfer.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    ReqFileTransfer.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqSendFileXferAgentObject.prototype = new Request();
function ReqSendFileXferAgentObject (AgentJID, ObjectType, FileXferID, ReqXferDataNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqSendFileXferAgentObject.BaseClass = ReqSendFileXferAgentObject.prototype;
  ReqSendFileXferAgentObject.prototype.constructor = ReqSendFileXferAgentObject;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.AgentJID = AgentJID;
  this.ObjectType = ObjectType;
  this.FileXferID = FileXferID;
  this.ReqXferDataNode = ReqXferDataNode;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Operation";
    this.req.FileXferID = this.FileXferID;
    if( this.ReqXferDataNode != null ) {
      this.req.RequestDataObjectArr = new Object;
      this.req.RequestDataObjectArr["FileXfer"] = this.ReqXferDataNode;
    }    
    //set agent message parameters
    this.req.AgentMessageHeader = new Object;
//    this.req.AgentMessageHeader.agentJID = this.AgentJID;
//    this.req.AgentMessageHeader.agent = "com.ipvs.systemservice.impl.FileTransferAgent";
//    this.req.AgentMessageHeader.agentmessagetype = "Request";

    //ref. sotry #46274 new agent message header format
    this.req.AgentMessageHeader.from = topic_Get(tApp, "LoginUserJID");
    this.req.AgentMessageHeader.to = this.AgentJID;
    this.req.AgentMessageHeader.type = "Request";
    this.req.AgentMessageHeader.agent = "com.ipvs.systemservice.impl.FileTransferAgent";
    ReqSendFileXferAgentObject.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqSendFileXferAgentObject.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqSendFileXferAgentObject.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetSetServiceConfig.prototype = new Request();
function ReqGetSetServiceConfig (ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetSetServiceConfig.BaseClass = ReqGetSetServiceConfig.prototype;
  ReqGetSetServiceConfig.prototype.constructor = ReqGetSetServiceConfig;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ObjectType = ObjectType;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Operation";
   
    ReqGetSetServiceConfig.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqGetSetServiceConfig.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    ReqGetSetServiceConfig.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetServerTime.prototype = new Request();
function ReqGetServerTime () {
// setup inheritence and call base constructor
  ReqGetServerTime.BaseClass = ReqGetServerTime.prototype;
  ReqGetServerTime.prototype.constructor = ReqGetServerTime;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.obj = arrServiceObjectInfo["ServerTime"];
  if( this.obj != null ) {
    this.SuccessEvent = this.obj.successevent;
    this.FailEvent = this.obj.failevent;
  }
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "systemservice";
    this.req.ObjectType = "GetServerTime";
    this.req.OpType = "Operation";
    ReqGetServerTime.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var respServerTime = response.selectSingleNode("//serverTime");
    if( respServerTime != null ) {
      var serverTime = respServerTime.getValue();
      topic_Set(tApp, "ServerTime", serverTime);
      
      var localTime = new Date().getTime();
      topic_Set(tApp, "LocalTime", localTime);
      
      var deltaTime = parseInt(localTime) - parseInt(serverTime);
      topic_Set(tApp, "DeltaServerLocalTime", deltaTime);
      
    }
    ReqGetServerTime.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    var cacheServerTime = topic_GetNode(tApp, "//ServerTime");
    if( cacheServerTime != null ) {
      topic_Set(tApp, "ServerTime", "");
      topic_Set(tApp, "DeltaServerLocalTime", "");
      var localTime = new Date().getTime();
      topic_Set(tApp, "LocalTime", localTime);
    }
    ReqGetServerTime.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetCurrentTimeStamp.prototype = new Request();
function ReqGetCurrentTimeStamp (StreamNID, StreamAction, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetCurrentTimeStamp.BaseClass = ReqGetCurrentTimeStamp.prototype;
  ReqGetCurrentTimeStamp.prototype.constructor = ReqGetCurrentTimeStamp;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.StreamNID = StreamNID;
  this.StreamAction = StreamAction;
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "GetCurrentTimeStamp";
    this.req.OpType = "Operation";

    this.req.StreamNID = this.StreamNID;
    this.req.StreamAction = this.StreamAction;
    
    ReqGetCurrentTimeStamp.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetCurrentTimeStamp.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqAddStream.prototype = new Request();
function ReqAddStream (SourceNID, DestNID, RelayNID, Description, ProfileNID, profileNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqAddStream.BaseClass = ReqAddStream.prototype;
  ReqAddStream.prototype.constructor = ReqAddStream;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.SourceNID = SourceNID;
  this.DestNID = DestNID;
  this.RelayNID = RelayNID;
  this.ProfileNID = ProfileNID;
  this.Description = Description;
  this.profileNode = profileNode;  
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "AddStream";
    this.req.OpType = "Operation";

    this.req.SourceNID = this.SourceNID;
    this.req.DestNID = this.DestNID;
    this.req.RelayNID = this.RelayNID;
    this.req.Description = this.Description;
    this.req.ProfileNID = this.ProfileNID;
    if( this.profileNode == null ) {
      this.profileNode = topic_GetNIDNode(tCache, this.req.ProfileNID);
    }
    if( this.profileNode != null ) {
      this.req.RequestDataObjectArr = new Object;
      this.req.RequestDataObjectArr["profileXML"] = this.profileNode;
    }
    ReqAddStream.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    var streamNIDNode = response.selectSingleNode("//streamNID");
    if( streamNIDNode != null ) {
      RespEventVal = streamNIDNode.getValue();
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqAddStream.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqAddStream.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//---------------------------------------
ReqAddStreamAction.prototype = new Request();
function ReqAddStreamAction (StreamNID, ProfileNID, profileNode, UpdateAction, ReqEvent) {
// setup inheritence and call base constructor
  ReqAddStreamAction.BaseClass = ReqAddStreamAction.prototype;
  ReqAddStreamAction.prototype.constructor = ReqAddStreamAction;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.StreamNID = StreamNID;
  this.ProfileNID = ProfileNID;
  this.profileNode = profileNode; 
  this.UpdateAction = UpdateAction;  
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "AddStreamAction";
    this.req.OpType = "Operation";

    this.req.StreamNID = this.StreamNID;
    this.req.ProfileNID = this.ProfileNID;
    this.req.ProfileNID = this.ProfileNID;
    if( this.profileNode == null ) {
      this.profileNode = topic_GetNIDNode(tCache, this.req.ProfileNID);
    }
    if( this.profileNode != null ) {
      this.req.RequestDataObjectArr = new Object;
      this.req.RequestDataObjectArr["profileXML"] = this.profileNode;
    }
    this.req.UpdateAction = this.UpdateAction;
    
    ReqAddStreamAction.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqAddStreamAction.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqAddStreamAction.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqAddTrackAction.prototype = new Request();
function ReqAddTrackAction (StreamNID, ActionData, UpdateAction, ReqEvent) {
// setup inheritence and call base constructor
  ReqAddTrackAction.BaseClass = ReqAddTrackAction.prototype;
  ReqAddTrackAction.prototype.constructor = ReqAddTrackAction;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.StreamNID = StreamNID;
  this.ActionData = ActionData;
  this.UpdateAction = UpdateAction;  
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "AddTrackAction";
    this.req.OpType = "Operation";

    this.req.StreamNID = this.StreamNID;
    this.req.ActionData = this.ActionData;
    this.req.UpdateAction = this.UpdateAction;
    
    ReqAddTrackAction.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqAddTrackAction.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqAddTrackAction.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqDeleteMediaRoom.prototype = new Request();
function ReqDeleteMediaRoom (RoomNID, ReqEvent) {
// setup inheritence and call base constructor
  ReqDeleteMediaRoom.BaseClass = ReqDeleteMediaRoom.prototype;
  ReqDeleteMediaRoom.prototype.constructor = ReqDeleteMediaRoom;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.RoomNID = RoomNID;
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "DeleteMediaRoom";
    this.req.OpType = "Operation";

    this.req.RoomNID = this.RoomNID;
    
    ReqDeleteMediaRoom.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // removing room from cache
    var cacheMediaRoom = topic_GetNIDNode(tCache, this.RoomNID);
    if(cacheMediaRoom != null) {
      cacheMediaRoom.getParent().removeChild(cacheMediaRoom);
    }
    arrServiceObjectInfo["MediaRoom"][this.RoomNID] = null;
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqDeleteMediaRoom.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqDeleteMediaRoom.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//---------------------------------------
ReqAddDestFile.prototype = new Request();
function ReqAddDestFile (ClipType, MediaGroupFileResourceNID, FileTitle, FileDescription, 
                         nodeGroups, nodeMetaData, nodePermissions, ParentMediaDirNID, 
                         MediaStorePortResourceNID, ReqEvent) {
// setup inheritence and call base constructor
  ReqAddDestFile.BaseClass = ReqAddDestFile.prototype;
  ReqAddDestFile.prototype.constructor = ReqAddDestFile;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");

  this.ClipType = ClipType;
  this.MediaGroupFileResourceNID = MediaGroupFileResourceNID;
  this.FileTitle = FileTitle;
  this.FileDescription = FileDescription;  
  this.nodeGroups = nodeGroups;
  this.nodeMetaData = nodeMetaData;
  this.nodePermissions = nodePermissions;
  this.ParentMediaDirNID = ParentMediaDirNID;
  this.MediaStorePortResourceNID = MediaStorePortResourceNID;  
  
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "AddDestFile";
    this.req.OpType = "Operation";

    this.req.ClipType = this.ClipType;
    this.req.MediaGroupFileResourceNID = this.MediaGroupFileResourceNID;
    this.req.FileTitle = this.FileTitle;
    this.req.FileDescription = this.FileDescription;    
    this.req.ParentMediaDirNID = this.ParentMediaDirNID;
    this.req.MediaStorePortResourceNID = this.MediaStorePortResourceNID;     
    
    this.req.RequestDataObjectArr = new Object;
    
    if( this.nodeGroups != null ) {
      this.req.RequestDataObjectArr["Groups"] = this.nodeGroups;
    }
    if( this.nodeMetaData != null ) {
      this.req.RequestDataObjectArr["MetaData"] = this.nodeMetaData;
    }
    if( this.nodePermissions != null ) {
      this.req.RequestDataObjectArr["Permissions"] = this.nodePermissions;
    }    
    ReqAddDestFile.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    var objectNIDNode = response.selectSingleNode("//ObjectNID");
    if( objectNIDNode != null ) {
      RespEventVal = objectNIDNode.getValue();
    }
    var nodeObject = response.selectSingleNode("//MediaStreamFileResource");
    if(nodeObject != null){
      setObjectCacheNode(nodeObject);
    }
    else{
      var cacheObject = topic_GetNIDNode(tCache, objectNIDNode);
      if(cacheObject != null) {
        cacheObject.getParent().removeChild(cacheObject);
      }
    }
    // do events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqAddDestFile.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqAddDestFile.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//---------------------------------------
ReqDeleteDestFile.prototype = new Request();
function ReqDeleteDestFile (FileNID, ReqEvent) {
// setup inheritence and call base constructor
  ReqDeleteDestFile.BaseClass = ReqDeleteDestFile.prototype;
  ReqDeleteDestFile.prototype.constructor = ReqDeleteDestFile;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.FileNID = FileNID;
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "DeleteDestFile";
    this.req.OpType = "Operation";

    this.req.FileNID = this.FileNID;
    
    ReqDeleteDestFile.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqDeleteDestFile.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqDeleteDestFile.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//---------------------------------------
ReqUpdateFileMetaData.prototype = new Request();
function ReqUpdateFileMetaData (MediaNID, nodeMetaData, ReqEvent) {
// setup inheritence and call base constructor
  ReqUpdateFileMetaData.BaseClass = ReqUpdateFileMetaData.prototype;
  ReqUpdateFileMetaData.prototype.constructor = ReqUpdateFileMetaData;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.MediaNID = MediaNID;
  this.nodeMetaData = nodeMetaData;  
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "UpdateDestFileMetaData";
    this.req.OpType = "Operation";

    this.req.MediaNID = this.MediaNID;
    this.req.RequestDataObjectArr = new Object;
    if( this.nodeMetaData != null ) {
      this.req.RequestDataObjectArr["MetaData"] = this.nodeMetaData;
    }
    ReqUpdateFileMetaData.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqUpdateFileMetaData.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqUpdateFileMetaData.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//---------------------------------------
//To get new bookmark for adding
ReqGetNewBookmark.prototype = new Request();
function ReqGetNewBookmark (StreamNID, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetNewBookmark.BaseClass = ReqGetNewBookmark.prototype;
  ReqGetNewBookmark.prototype.constructor = ReqGetNewBookmark;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.StreamNID = StreamNID;
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "GetNewBookmark";
    this.req.OpType = "Operation";

    this.req.StreamNID = this.StreamNID;
    
    ReqGetNewBookmark.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetNewBookmark.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
//For Add/Edit/Delete bookmarks
ReqHandleBookmark.prototype = new Request();
function ReqHandleBookmark (addUpdateMode, bookMarkNID, nodeBookmark, ReqEvent) {
// setup inheritence and call base constructor
  ReqHandleBookmark.BaseClass = ReqHandleBookmark.prototype;
  ReqHandleBookmark.prototype.constructor = ReqHandleBookmark;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");

  this.BookmarkNID = bookMarkNID;
  this.ObjectType = addUpdateMode;
  this.nodeBookmark = nodeBookmark;
  this.ReqEvent = ReqEvent;

 // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Operation";
    if( this.BookmarkNID != null ) {
      this.req.BookmarkNID = this.BookmarkNID;
    }

    if( this.ObjectType != "DeleteBookmark" ) {
      this.req.RequestDataObjectArr = new Object;
    }
    if( this.nodeBookmark != null ) {
      this.req.RequestDataObjectArr["Bookmark"] = this.nodeBookmark;
    }
    ReqHandleBookmark.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqHandleBookmark.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqHandleBookmark.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetSrcMediaFileTrack.prototype = new Request();
function ReqGetSrcMediaFileTrack (MediaFileNID, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetSrcMediaFileTrack.BaseClass = ReqGetSrcMediaFileTrack.prototype;
  ReqGetSrcMediaFileTrack.prototype.constructor = ReqGetSrcMediaFileTrack;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.MediaFileNID = MediaFileNID;
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "SrcMediaFileTrack";
    this.req.OpType = "Get";

    this.req.MediaFileNID = this.MediaFileNID;
    ReqGetSrcMediaFileTrack.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetSrcMediaFileTrack.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetSrcMediaFileTrack.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//---------------------------------------
ReqDeleteObjectWhere.prototype = new Request();
function ReqDeleteObjectWhere (ObjectType, ObjectWhereXPath, ReqEvent) {
// setup inheritence and call base constructor
  ReqDeleteObjectWhere.BaseClass = ReqDeleteObjectWhere.prototype;
  ReqDeleteObjectWhere.prototype.constructor = ReqDeleteObjectWhere;
  if( ObjectType != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectType = ObjectType;
    this.ObjectWhereXPath = ObjectWhereXPath;
    var ObjectWhereAssistXMLDoc = new jsx3.xml.Document();
    this.ObjectWhereAssistXMLNode = ObjectWhereAssistXMLDoc.createDocumentElement(this.ObjectType).cloneNode(true);
    this.ReqEvent = ReqEvent;
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
  }
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Delete";
    this.req.Op_Where_WhereAssistXML = this.ObjectWhereAssistXMLNode;
    this.req.Op_Where_WhereXPath = "//" + this.ObjectType + this.ObjectWhereXPath;
    ReqDeleteObjectWhere.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    var objectNIDNode = response.selectSingleNode("//ObjectNID");
    if( objectNIDNode != null ) {
      RespEventVal = objectNIDNode.getValue();
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    if( this.obj != null ) {
      utilObjectListUpdate("DELETE", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqDeleteObjectWhere.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // no return data to process - just fire req/fail event  if any
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqDeleteObjectWhere.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqDeleteObjectNID.prototype = new Request();
function ReqDeleteObjectNID (ObjectNID, ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqDeleteObjectNID.BaseClass = ReqDeleteObjectNID.prototype;
  ReqDeleteObjectNID.prototype.constructor = ReqDeleteObjectNID;
  if( ObjectNID != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectNID = ObjectNID;
    this.ReqEvent = ReqEvent;
  }
  if( ObjectType != null ) {
    this.ObjectType = ObjectType;
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
  }
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Delete";
    this.req.Op_Where_NID = this.ObjectNID;
    ReqDeleteObjectNID.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var objectNIDNode = response.selectSingleNode("//ObjectNID");
    if( objectNIDNode == null ) {
      objectNIDNode = response.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID");
      objectNIDNode.setValue(this.ObjectNID);
      response.appendChild(objectNIDNode);
    }
    else {
      objectNIDNode = objectNIDNode.cloneNode(true);
    }
    // var topicDataListNode = topic_Set(this.TopicName, "ObjectNIDList", null);
    // topicDataListNode.appendChild(objectNIDNode);
    var RespObjectNID = objectNIDNode.getValue();
    if( RespObjectNID != this.ObjectNID ) {
      handleFatalError( "InternalError: DeleteObject: NID Mismatch: " + "\n" +
            "Requested Object NID: " + this.ObjectNID + "\n" +
            "Response Object NID: " + RespObjectNID );
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = this.ObjectNID;
    }
    if( this.obj != null ) {
      utilObjectListUpdate("DELETE", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqDeleteObjectNID.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // no return data to process - just fire req/fail event  if any
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = this.ObjectNID;
    }
    ReqDeleteObjectNID.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqSetObject.prototype = new Request();
function ReqSetObject (SetMode, ObjectType, objectDataNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqSetObject.BaseClass = ReqSetObject.prototype;
  ReqSetObject.prototype.constructor = ReqSetObject;
  if( SetMode != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.SetMode = SetMode;
    this.ObjectType = ObjectType;
    this.objectDataNode = objectDataNode;
    this.ReqEvent = ReqEvent;
    this.ObjectNID = "";
    
    if( this.ObjectType == null ) {
      this.ObjectType = this.objectDataNode.getNodeName(); 
    } 
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
    var objNID = this.objectDataNode.getAttribute("NID");
    if( objNID != null ) {
      this.ObjectNID = objNID;
    }
  }
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.OpType = this.SetMode;
    this.req.ObjectType = this.ObjectType;
    if( this.SetMode == "Replace" ) {
      this.req.Op_Where_NID = this.ObjectNID;
      if( this.reqParams != null ) {
        for(var rpName in this.reqParams) {
          this.req[rpName] = this.reqParams[rpName];
        }
      }
    }
    this.req.RequestDataObjectArr = new Object;
    this.req.RequestDataObjectArr["OperationData"] = this.objectDataNode;
    ReqSetObject.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    var objectNIDNode = response.selectSingleNode("//ObjectNID");
    if( objectNIDNode!= null ) {
      objectNIDNode = objectNIDNode.cloneNode(true);
    }
    if( objectNIDNode != null ) {
      RespEventVal = objectNIDNode.getValue();
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    if( this.obj != null ) {
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqSetObject.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // no return data to process - just fire req/fail event  if any
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqSetObject.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}  

//---------------------------------------
ReqGetObjectWhere.prototype = new Request();
function ReqGetObjectWhere (ObjectType, ObjectWhereXPath, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetObjectWhere.BaseClass = ReqGetObjectWhere.prototype;
  ReqGetObjectWhere.prototype.constructor = ReqGetObjectWhere;
  if( ObjectType != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectType = ObjectType ;
    var ObjectWhereAssistXMLDoc = new jsx3.xml.Document();
    this.ObjectWhereAssistXMLNode = ObjectWhereAssistXMLDoc.createDocumentElement(this.ObjectType).cloneNode(true);
    this.ObjectWhereAssistXMLNode.setValue("");
    this.ObjectWhereXPath = ObjectWhereXPath;
    this.ReqEvent = ReqEvent;
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
  }
  
  // extend sendrequest
  this.sendRequest = function() {
      // check if already processing this req
      if (this.req != null) return false;
      // if not start a new req
      this.initRequest();
      // set req params
      this.req.ServiceName = this.ServiceName;
      this.req.ServiceName = "assetadminservice";
      this.req.ObjectType = this.ObjectType;
      this.req.OpType = "Get";
      this.req.Op_Where_WhereAssistXML = this.ObjectWhereAssistXMLNode;
      if(this.ObjectWhereXPath !=null){
        this.req.Op_Where_WhereXPath = "//" + this.ObjectType + this.ObjectWhereXPath;
      }
      else{
        this.req.Op_Where_WhereXPath = "";
      }
      ReqGetObjectWhere.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    var objectNIDNode = response.selectSingleNode("//ObjectNID");
    if( objectNIDNode != null ) {
      RespEventVal = objectNIDNode.getValue();
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    if( this.obj != null ) {
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqGetObjectWhere.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // no return data to process - just fire req/fail event  if any
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqGetObjectWhere.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetObjectNID.prototype = new Request();
function ReqGetObjectNID (ObjectNID, ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetObjectNID.BaseClass = ReqGetObjectNID.prototype;
  ReqGetObjectNID.prototype.constructor = ReqGetObjectNID;
  if( ObjectNID != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectNID = ObjectNID;
    this.ReqEvent = ReqEvent;
  }
  if( ObjectType != null ) {
    this.ObjectType = ObjectType;
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
  }
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Get";
    this.req.Op_Where_NID = this.ObjectNID;
    ReqGetObjectNID.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var objectNIDNode = response.selectSingleNode("//ObjectNID");
    var RespObjectNID = null;
    if( !jsx3.util.strEmpty(objectNIDNode) ) {
      RespObjectNID = objectNIDNode.getValue();
    }
    if( RespObjectNID != this.ObjectNID ) {
      handleFatalError( "InternalError: GetObject: NID Mismatch: " + "\n" +
            "Requested Object NID: " + this.ObjectNID + "\n" +
            "Response Object NID: " + RespObjectNID );
    }
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = this.ObjectNID;
    }
    if( this.obj != null ) {
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqGetObjectNID.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // no return data to process - just fire req/fail event  if any
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = this.ObjectNID;
    }
    ReqGetObjectNID.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
/*****************************************
*            Session     API             *
******************************************/
//------------ SessionGetRequest --------------
ReqSessionGetRequest.prototype = new Request();
function ReqSessionGetRequest(ObjectType, ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionGetRequest.BaseClass = ReqSessionGetRequest.prototype;
  ReqSessionGetRequest.prototype.constructor = ReqSessionGetRequest;
  var ThisClass = this;  
  Request.prototype.constructor.call(ThisClass, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;
  this.ObjectType = ObjectType;
  if (this.ObjectType == null) {
    this.ObjectType = "Get";
  }

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if (this.req != null) {
      // kill the old request
      ReqSessionGetRequest.BaseClass.endRequest.call(this);
      // dont return create the new request this done so that when the user changes dir quickly
      // he selected one and while getting response he selects other dir then kill the old request and send the new one
      // old response will be invalid reponse.
      //return false;
    }
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = this.ObjectType + "Request";
    this.req.iq_type = "set"; //or may be "get"
    this.req.iq_from = topic_Get(tApp, "LoginUserJID"); //sunuser1@localhost/sunpc1
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");
    
    if (this.reqParams != null) {
      for (var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    if (this.requestDataObjectArr != null) {
      for (var rdName in this.requestDataObjectArr) {
        if (jsx3.util.strEmpty(this.req.RequestDataObjectArr)) {
          this.req.RequestDataObjectArr = new Object;
        }
        this.req.RequestDataObjectArr[rdName] = this.requestDataObjectArr[rdName];
      }    
    }
    
    if(this.SimulateResponse){
      this.req.SimulateResponse = this.SimulateResponse;
      this.req.SimulateResponseXML = this.SimulateResponseXML;
    }
    ReqSessionGetRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//------------ Session Start/Stop Request --------------
ReqSessionStartStopRequest.prototype = new Request();
function ReqSessionStartStopRequest(ReqAction, SessionNode, ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionStartStopRequest.BaseClass = ReqSessionStartStopRequest.prototype;
  ReqSessionStartStopRequest.prototype.constructor = ReqSessionStartStopRequest;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;
  this.ReqAction = ReqAction;
  this.SessionNode = SessionNode;

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if(this.req != null) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = this.ReqAction + "Request";
    this.req.iq_type = "set"; //or may be "get"
    this.req.iq_from = topic_Get(tApp, "LoginUserJID"); //sunuser1@localhost/sunpc1
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");

    if(this.ReqAction == "Start") {
      this.req.RequestDataObjectArr = new Object;
      if(this.SessionNode != null) {
        this.req.RequestDataObjectArr["Session"] = this.SessionNode;
      }
    }
    if(this.reqParams != null) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    
    ReqSessionStartStopRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if(this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionStartStopRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if(this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionStartStopRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//------------------ Session Join -------------------
ReqSessionJoinRequest.prototype = new Request();
function ReqSessionJoinRequest(sessionID, ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionJoinRequest.BaseClass = ReqSessionJoinRequest.prototype;
  ReqSessionJoinRequest.prototype.constructor = ReqSessionJoinRequest;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if (this.req != null) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = "JoinRequest";
    //can be "get" also
    this.req.iq_type = "set"; 
    this.req.iq_from = topic_Get(tApp, "LoginUserJID"); 
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");
    //pass in sessionID
    this.req.session_id = sessionID;

    if (this.reqParams != null) {
      for (var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqSessionGetRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//--- Session Delete Connection Request ---
ReqSessionDeleteConnRequest.prototype = new Request();
function ReqSessionDeleteConnRequest(sessionID,connectionID, ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionDeleteConnRequest.BaseClass = ReqSessionDeleteConnRequest.prototype;
  ReqSessionDeleteConnRequest.prototype.constructor = ReqSessionDeleteConnRequest;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if (this.req != null) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = "DeleteConnRequest";
    //can be "get" also
    this.req.iq_type = "set";
    this.req.iq_from = topic_Get(tApp, "LoginUserJID");
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");
    //pass in sessionID & connectionID
    this.req.session_id = sessionID;
    this.req.conn_id = connectionID;

    if (this.reqParams != null) {
      for (var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqSessionGetRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}
//--- Session PVR(Pause,Resume,PauseRecord,ResumeRecord,Catchup,SkipBack,SkipFwd) Request ---
ReqSessionPvrRequest.prototype = new Request();
function ReqSessionPvrRequest(sessionID, pvrAction,skipOffset,ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionPvrRequest.BaseClass = ReqSessionPvrRequest.prototype;
  ReqSessionPvrRequest.prototype.constructor = ReqSessionPvrRequest;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if (this.req != null) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = pvrAction + "Request"; 
    //set sessionIQ wrappper
    //can be "get" also
    this.req.iq_type = "set";
    this.req.iq_from = topic_Get(tApp, "LoginUserJID");
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");
    //pass in sessionID
    this.req.session_id = sessionID;
    //set for SkipBack and SkipFwd
    this.req.skipOffset = skipOffset;

    if (this.reqParams != null) {
      for (var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqSessionPvrRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionPvrRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionPvrRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}


//--- Session resumefrombookmark Request ---
ReqSessionResumeFromBookmarkRequest.prototype = new Request();
function ReqSessionResumeFromBookmarkRequest(sessionID, pvrAction,actionBookmarkId,ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionResumeFromBookmarkRequest.BaseClass = ReqSessionResumeFromBookmarkRequest.prototype;
  ReqSessionResumeFromBookmarkRequest.prototype.constructor = ReqSessionResumeFromBookmarkRequest;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if (this.req != null) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = pvrAction + "Request"; 
    //set sessionIQ wrappper
    //can be "get" also
    this.req.iq_type = "set";
    this.req.iq_from = topic_Get(tApp, "LoginUserJID");
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");
    //pass in sessionID
    this.req.session_id = sessionID;
    //set for SkipBack and SkipFwd
    this.req.bookmark_id = actionBookmarkId;

    if (this.reqParams != null) {
      for (var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqSessionResumeFromBookmarkRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionResumeFromBookmarkRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionResumeFromBookmarkRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
//---------------------------------------
ReqGetObjectList.prototype = new Request();
function ReqGetObjectList (ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetObjectList.BaseClass = ReqGetObjectList.prototype;
  ReqGetObjectList.prototype.constructor = ReqGetObjectList;
  if( ObjectType != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectType = ObjectType;
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
  }
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Get";
    this.req.OpSpec = "List";
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqGetObjectList.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    if( this.obj != null ) {
      this.obj.listreqstate = newServerDate();
      if( this.obj.nidlist != null ) {
        var topicDataListNode = topic_GetNode(tApp, ".//" + this.obj.nidlist);
        if(topicDataListNode != null) {
          topicDataListNode.removeChildren();
        }
      }
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
      utilObjectListCacheSync(response, this.obj.nidlist, this.ObjectType);
    }
    ReqGetObjectList.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    if( this.obj != null ) {
      this.obj.listreqstate = newServerDate();
      if( this.obj.nidlist != null ) {
        var topicDataListNode = topic_GetNode(tApp, ".//" + this.obj.nidlist);
        if(topicDataListNode != null) {
          topicDataListNode.removeChildren();
        }
      }
      utilObjectListUpdate("ADD", null, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqGetObjectList.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetObjectListRangeWhere.prototype = new Request();
function ReqGetObjectListRangeWhere (objInfo, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetObjectListRangeWhere.BaseClass = ReqGetObjectListRangeWhere.prototype;
  ReqGetObjectListRangeWhere.prototype.constructor = ReqGetObjectListRangeWhere;
  if( objInfo != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectType = null;
    this.obj = objInfo;
    this.ServiceName = this.obj.reqservice;
    this.SuccessEvent = this.obj.successevent;
    this.FailEvent = this.obj.failevent;
  }
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Get";
    this.req.OpSpec = "List";
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqGetObjectListRangeWhere.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    if (this.obj != null) {
      this.obj.listreqstate = newServerDate();
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }
    ReqGetObjectListRangeWhere.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    if (this.obj != null) {
      //TBD
    }
    ReqGetObjectListRangeWhere.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetPaginationList.prototype = new Request();
function ReqGetPaginationList(ReqEvent) {
// setup inheritence and call base constructor
  ReqGetPaginationList.BaseClass = ReqGetPaginationList.prototype;
  ReqGetPaginationList.prototype.constructor = ReqGetPaginationList;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;
  
    // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.RequestName = this.RequestName;
    this.req.OpType = "Get";
    this.req.OpSpec = "List";
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqGetPaginationList.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetPaginationList.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetPaginationList.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetListReadQuery.prototype = new Request();
function ReqGetListReadQuery (ReqEvent) {
// setup inheritence and call base constructor
  ReqGetListReadQuery.BaseClass = ReqGetListReadQuery.prototype;
  ReqGetListReadQuery.prototype.constructor = ReqGetListReadQuery;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) {
      // log the request
      log.error("ENDING/KILLING ReqGetListReadQuery RequestName: ReadQueryRequest Request iq_id: "+ this.req.iq_id + " \n" );    
      // end/kill the old request
      ReqGetListReadQuery.BaseClass.endRequest.call(this);
      // dont return create the new request this done so that when the user changes dir quickly
      // he selected one and while getting response he selects other dir then kill the old request and send the new one
      // old response will be invalid reponse.
      //return false;
    }
    // log the request
    log.warn("STARTING ReqGetListReadQuery RequestName: ReadQueryRequest \n "); 
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = null;
    this.req.OpType = "ReadQuery";
    this.req.OpSpec = "";
    if( this.reqParams != null ) {
      for(var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    
    //for the Simulation
    if(this.SimulateResponse){
      this.req.SimulateResponse = this.SimulateResponse;
      this.req.SimulateResponseXML = this.SimulateResponseXML;
    } 
        
    ReqGetListReadQuery.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetListReadQuery.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqGetListReadQuery.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}


//function to get the MediaStreamFiles Pagination
// its defined here becuase Admin project js file is loaded first and then common js files
cmpReq_GetListReadQuery.prototype = new ReqGetListReadQuery();
function cmpReq_GetListReadQuery (InstanceID, ReadQueryFile, CacheListArr, ReqEvent, ItemByNID) {
// setup inheritence and call base constructor
  cmpReq_GetListReadQuery.BaseClass = cmpReq_GetListReadQuery.prototype;
  cmpReq_GetListReadQuery.prototype.constructor = cmpReq_GetListReadQuery;
  ReqGetListReadQuery.prototype.constructor.call(this, ReqEvent);
  
  this.ReadQueryFile = ReadQueryFile;
  this.CacheListArr = CacheListArr;
  this.ReadQuery = null;
  this.reqParams = null;
  this.InstancID = InstanceID;
  this.ItemByNID = ItemByNID;
  if( jsx3.util.strEmpty(this.ReadQueryFile) ) return;
  var queryDataDoc = PNameSpace.loadInclude(this.ReadQueryFile, "ReadQueryFile", "xml", true);
  if( !jsx3.util.strEmpty(queryDataDoc) ) {
    var query = queryDataDoc.getValue();
    this.ReadQuery = query.toString().replace(/[\n\r]/g, " ");
    
  }
  
  this.ObjectType = null;
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI" ) {
    this.ServiceName = "assetadminservice";
  }
  else {
    this.ServiceName = "mediaroomservice";
  }
        
  this.sendRequest = function() {
    var InstanceID = this.InstancID;
    
    var thisReadQuery = eval(this.ReadQuery);
    thisReadQuery = thisReadQuery.replace(/_SORTXPATH_/g, this.reqParams["Op_Select_ListSpec_SortByXPathList_SortByXpath"]);
    thisReadQuery = thisReadQuery.replace(/_SORTORDER_/g, this.reqParams["Op_Select_ListSpec_SortOrder"]);
    
    thisReadQuery = thisReadQuery.replace("_STARTFROM_", this.reqParams["Op_Select_ListSpec_StartFrom"]);
    thisReadQuery = thisReadQuery.replace("_COUNTTOFETCH_", this.reqParams["Op_Select_ListSpec_CountToFetch"]);
    
    
    for( var VarNameRef in this.CacheListArr ) {
      if( this.CacheListArr[VarNameRef].VarRef != null ) {
        var varName =  this.CacheListArr[VarNameRef].VarName;
        var varRef = this.CacheListArr[VarNameRef].VarRef;
        var varValue = topic_Get(InstanceID, varRef);
        thisReadQuery = thisReadQuery.replace(new RegExp(varName,"g"), varValue); 
      }
    }
    //using escape only in 3.6 not in 3.5 to workaround for middleware issue
    this.reqParams["Op_ReadQueryRequest_Query"] =  escape(thisReadQuery);
            
    cmpReq_GetListReadQuery.BaseClass.sendRequest.call(this);  
  }
  
  this.onSuccess = function(req, ErrCode, response) {
   if(ErrCode == "SUCCESS" && response != null) {
     //check request is for list or for one item
     if( jsx3.util.strEmpty(this.ItemByNID) ) {
       //set the variable values
       this.reqParams["ObjectCount"] = response.selectSingleNode(".//ResponseXML/result/GetListObjectResponseData/ObjectCount").getValue();
       this.reqParams["Op_Select_ListSpec_StartFrom"] = response.selectSingleNode(".//ResponseXML/result/GetListObjectResponseData/ObjectLastWhereEndCount").getValue();
       this.reqParams["Op_Select_ListSpec_CountToFetch"] = response.selectSingleNode(".//ResponseXML/result/GetListObjectResponseData/CountToFetch").getValue();
     }

     var responseVal = response.selectSingleNode(".//ResponseXML/result/data");
     
     cmpReq_GetListReadQuery.BaseClass.onSuccess.call(this, req, ErrCode, responseVal);
   }
   
  }
} // end cmpReq_GetListReadQuery

//function to get the procedures Pagination
cmpReq_GetAdminPaginationList.prototype = new ReqGetPaginationList();
function cmpReq_GetAdminPaginationList(InstanceID, ReqName, ObjectType, WhereXPath, SelectXML, CacheListArr, ReqEvent) {
// setup inheritence and call base constructor
  cmpReq_GetAdminPaginationList.BaseClass = cmpReq_GetAdminPaginationList.prototype;
  cmpReq_GetAdminPaginationList.prototype.constructor = cmpReq_GetAdminPaginationList;
  ReqGetPaginationList.prototype.constructor.call(this, ReqEvent);
  
  this.InstancID = InstanceID;
  this.SelectXML = escape(SelectXML);
  this.WhereXPath = WhereXPath;
  this.CacheListArr = CacheListArr;  
  this.reqParams = null;  
  this.ObjectType = ObjectType;
  this.ServiceName = "assetadminservice";
  this.RequestName = ReqName;
        
  this.sendRequest = function() {
    var InstanceID = this.InstancID;
    var whereXPath = this.WhereXPath;

    for( var VarNameRef in this.CacheListArr ) {
      if( this.CacheListArr[VarNameRef].VarRef != null ) {
        var varName =  this.CacheListArr[VarNameRef].VarName;
        var varRef = this.CacheListArr[VarNameRef].VarRef;
        var varValue = topic_Get(InstanceID, varRef);
        whereXPath = whereXPath.replace(varName, varValue); 
      }
    }

    //HACK:mediaroom service handlers are 0 based to fetch list
    //Middleware should fix to 1 based
    this.reqParams["Op_Select_ListSpec_StartFrom"] = this.reqParams["Op_Select_ListSpec_StartFrom"] - 1;
    this.reqParams["Op_Where_WhereXPath"] =  whereXPath;
    this.reqParams["Op_Select_ItemSpec_SelectXML"] =  this.SelectXML;
    
    cmpReq_GetAdminPaginationList.BaseClass.sendRequest.call(this);  
  }
  
  this.onSuccess = function(req, ErrCode, response) {
   if(ErrCode == "SUCCESS" && response != null) {
     //set the variable values
     this.reqParams["ObjectCount"] = response.getAttribute("ObjectCount");
     this.reqParams["Op_Select_ListSpec_StartFrom"] = response.getAttribute("ListStartFrom");
     //HACK:mediaroom service handlers are 0 based to fetch list
     //Middleware should fix to 1 based
     this.reqParams["Op_Select_ListSpec_StartFrom"] = parseInt(this.reqParams["Op_Select_ListSpec_StartFrom"]) + 1;

     var responseDoc = new jsx3.xml.CDF.Document.newDocument();

     var respValDataNodes = response.selectNodes("//ResponseObjectNIDList/*");
     for(var it = respValDataNodes.iterator(); it.hasNext(); ) {
       var respValData = it.next();
       responseDoc.appendChild(respValData);
     }

     cmpReq_GetAdminPaginationList.BaseClass.onSuccess.call(this, req, ErrCode, responseDoc);
   }
   
  }
} // end cmpReq_GetAdminPaginationList

//---------------------------------------
//To Repair/Upgrade MediaStreamFile
ReqHandleMediaStreamFile.prototype = new Request();
function ReqHandleMediaStreamFile(MediaFileNID, ObjectType, ReqEvent) {
// setup inheritence and call base constructor
  ReqHandleMediaStreamFile.BaseClass = ReqHandleMediaStreamFile.prototype;
  ReqHandleMediaStreamFile.prototype.constructor = ReqHandleMediaStreamFile;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.MediaFileNID = MediaFileNID;
  this.ReqEvent = ReqEvent;
  this.ObjectType = ObjectType;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "assetadminservice";
    this.req.ObjectType = this.ObjectType;
    this.req.RequestName = this.ObjectType + "Request";
    this.req.OpType = "Operation";
    this.req.MediaStreamFileResourceNID = this.MediaFileNID;

     ReqHandleMediaStreamFile.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqHandleMediaStreamFile.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqHandleMediaStreamFile.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
//To Empty(start/stop) dir/Clean Orphan directory
ReqHandleDir.prototype = new Request();
function ReqHandleDir(MediaDirNID, UpgradeMediaFiles, ObjectType, ReqAction, ReqEvent) {
// setup inheritence and call base constructor
  ReqHandleDir.BaseClass = ReqHandleDir.prototype;
  ReqHandleDir.prototype.constructor = ReqHandleDir;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.MediaStreamDirNID = MediaDirNID;
  this.UpgradeMediaFiles = UpgradeMediaFiles;
  this.ObjectType = ObjectType;
  this.ActionRequest = ReqAction;
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "assetadminservice";
    this.req.ObjectType = this.ObjectType;
    this.req.RequestName = this.ObjectType + "Request";
    this.req.OpType = "Operation";
    this.req.MediaStreamDirNID = this.MediaStreamDirNID;
    this.req.UpgradeMediaFiles = this.UpgradeMediaFiles;
    if( this.ActionRequest != null ) {
      this.req.ActionRequest = this.ActionRequest;
    }

    ReqHandleDir.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqHandleDir.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqHandleDir.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
//To change user password
ReqChangePassword.prototype = new Request();
function ReqChangePassword(ReqEvent) {
// setup inheritence and call base constructor
  ReqChangePassword.BaseClass = ReqChangePassword.prototype;
  ReqChangePassword.prototype.constructor = ReqChangePassword;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.ObjectType = "ChangePassword";
  this.ReqEvent = ReqEvent;
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "assetadminservice";
    this.req.ObjectType = this.ObjectType;
    this.req.RequestName = this.ObjectType + "Request";
    this.req.OpType = "Operation";

    ReqChangePassword.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    // process events
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }  
    ReqChangePassword.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // clear out the cache value
  this.onFail = function(req, ErrCode, response) {
    ReqChangePassword.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
ReqGetAppServerAgentJID.prototype = new Request();
function ReqGetAppServerAgentJID (ObjectType, ObjectWhereXPath, ReqEvent) {
// setup inheritence and call base constructor
  ReqGetAppServerAgentJID.BaseClass = ReqGetAppServerAgentJID.prototype;
  ReqGetAppServerAgentJID.prototype.constructor = ReqGetAppServerAgentJID;
  if( ObjectType != null ) {
    Request.prototype.constructor.call(this, "ServiceAppletRequest");
    this.ObjectType = ObjectType ;
    var ObjectWhereAssistXMLDoc = new jsx3.xml.Document();
    this.ObjectWhereAssistXMLNode = ObjectWhereAssistXMLDoc.createDocumentElement(this.ObjectType).cloneNode(true);
    this.ObjectWhereAssistXMLNode.setValue("");
    this.ObjectWhereXPath = ObjectWhereXPath;
    this.ReqEvent = ReqEvent;
    this.obj = arrServiceObjectInfo[ObjectType];
    if( this.obj != null ) {
      this.ServiceName = this.obj.reqservice;
      this.SuccessEvent = this.obj.successevent;
      this.FailEvent = this.obj.failevent;
    }
  }
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = this.ServiceName;
    this.req.ObjectType = this.ObjectType;
    this.req.OpType = "Get";
    this.req.Op_Where_WhereAssistXML = this.ObjectWhereAssistXMLNode;
    this.req.Op_Where_WhereXPath = "//" + this.ObjectType + this.ObjectWhereXPath;
    ReqGetAppServerAgentJID.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    if( this.obj != null ) {
      this.obj.listreqstate = newServerDate();
      if( this.obj.nidlist != null ) {
        var topicDataListNode = topic_GetNode(tApp, ".//" + this.obj.nidlist);
        if(topicDataListNode != null) {
          topicDataListNode.removeChildren();
        }
      }
      utilObjectListUpdate("ADD", response, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
      utilObjectListCacheSync(response, this.obj.nidlist, this.ObjectType);
    }
    
    ReqGetAppServerAgentJID.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // no return data to process - just fire req/fail event  if any
  this.onFail = function(req, ErrCode, response) {   
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    if( this.obj != null ) {
      this.obj.listreqstate = newServerDate();
      if( this.obj.nidlist != null ) {
        var topicDataListNode = topic_GetNode(tApp, ".//" + this.obj.nidlist);
        if(topicDataListNode != null) {
          topicDataListNode.removeChildren();
        }
      }
      utilObjectListUpdate("ADD", null, this.obj.nidlist, this.obj.grouptreelist, this.obj.grouptagxpath);
    }    
    ReqGetAppServerAgentJID.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}


//---------------------------------------
ReqRecycleBin.prototype = new Request();
function ReqRecycleBin (ActionRequest, ReqEvent) {
// setup inheritence and call base constructor
  ReqRecycleBin.BaseClass = ReqRecycleBin.prototype;
  ReqRecycleBin.prototype.constructor = ReqRecycleBin;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  this.obj = arrServiceObjectInfo["EmptyRecycleBin"];
  if( this.obj != null ) {
    this.SuccessEvent = this.obj.successevent;
    this.FailEvent = this.obj.failevent;
    this.ActionRequest = ActionRequest;
    this.ReqEvent = ReqEvent;
  }
  
   // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "assetadminservice";
    this.req.ObjectType = "EmptyRecycleBin";
    this.req.OpType = "Operation";
    this.req.ActionRequest = this.ActionRequest;
    ReqRecycleBin.BaseClass.sendRequest.call(this);
  }
  
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    ReqRecycleBin.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    ReqRecycleBin.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}


//---------------------------------------
ReqCreateCollaboration.prototype = new Request();
function ReqCreateCollaboration (objectDataNode, ReqEvent) {
// setup inheritence and call base constructor
  ReqCreateCollaboration.BaseClass = ReqCreateConnection.prototype;
  ReqCreateCollaboration.prototype.constructor = ReqCreateCollaboration;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");
  
  this.objectDataNode = objectDataNode; 
  this.ReqEvent = ReqEvent;
  
  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "mediaroomservice";
    this.req.ObjectType = "CreateCollaboration";
    this.req.OpType = "Operation";
    
    this.req.RequestDataObjectArr = new Object;
    this.req.RequestDataObjectArr["CreateCollaborationRequestData"] = this.objectDataNode; 
    ReqCreateCollaboration.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqCreateCollaboration.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    var RespEventVal = "DATE";
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = RespEventVal;
    }
    ReqCreateCollaboration.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}


//------------ Session GetBookamrkRequest --------------
ReqSessionGetBookmarkRequest.prototype = new Request();
function ReqSessionGetBookmarkRequest(ReqEvent) {
  // setup inheritence and call base constructor
  ReqSessionGetBookmarkRequest.BaseClass = ReqSessionGetBookmarkRequest.prototype;
  ReqSessionGetBookmarkRequest.prototype.constructor = ReqSessionGetBookmarkRequest;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");

  this.ReqEvent = ReqEvent;

  // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if (this.req != null) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = "GetBookmarkRequest";
    this.req.iq_type = "get"; //or may be "get"
    this.req.iq_from = topic_Get(tApp, "LoginUserJID"); //sunuser1@localhost/sunpc1
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");
    
    if (this.reqParams != null) {
      for (var rpName in this.reqParams) {
        this.req[rpName] = this.reqParams[rpName];
      }
    }
    ReqSessionGetBookmarkRequest.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetBookmarkRequest.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  // treat fail the same as success with zero records in the response list
  this.onFail = function(req, ErrCode, response) {
    if (this.ReqEvent != null && this.ReqEvent.eventval == null) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionGetBookmarkRequest.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//For Windowing
ReqSessionHandleWindowing.prototype = new Request();
function ReqSessionHandleWindowing (nodeDisplayConfig,ObjectType,ReqEvent) {
// setup inheritence and call base constructor
  ReqSessionHandleWindowing.BaseClass = ReqSessionHandleWindowing.prototype;
  ReqSessionHandleWindowing.prototype.constructor = ReqSessionHandleWindowing;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");

  this.ObjectType = ObjectType;
  this.nodeDisplayConfig = nodeDisplayConfig;
  this.ReqEvent = ReqEvent;

 // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = this.ObjectType;
    this.req.iq_type = "set"; //or may be "get"
    this.req.iq_from = topic_Get(tApp, "LoginUserJID"); //sunuser1@localhost/sunpc1
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");

    this.req.RequestDataObjectArr = new Object;

    if( this.nodeDisplayConfig != null ) {
      this.req.RequestDataObjectArr[this.ObjectType] = this.nodeDisplayConfig;
    }
    if(this.SimulateResponse){
      this.req.SimulateResponse = this.SimulateResponse;
      this.req.SimulateResponseXML = this.SimulateResponseXML;
    }    
    ReqSessionHandleWindowing.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionHandleWindowing.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionHandleWindowing.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//For Add/Edit/Delete Session Bookmarks
ReqSessionHandleBookmark.prototype = new Request();
function ReqSessionHandleBookmark (addUpdateMode, nodeBookmark, ReqEvent) {
// setup inheritence and call base constructor
  ReqSessionHandleBookmark.BaseClass = ReqSessionHandleBookmark.prototype;
  ReqSessionHandleBookmark.prototype.constructor = ReqSessionHandleBookmark;
  Request.prototype.constructor.call(this, "ServiceAppletRequest");

  this.ObjectType = addUpdateMode;
  this.nodeBookmark = nodeBookmark;
  this.ReqEvent = ReqEvent;

 // extend sendrequest
  this.sendRequest = function() {
    // check if already processing this req
    if( this.req != null ) return false;
    // if not start a new req
    this.initRequest();
    // set req params
    this.req.ServiceName = "sessionservice";
    this.req.RequestName = this.ObjectType;
    this.req.iq_type = "set"; //or may be "get"
    this.req.iq_from = topic_Get(tApp, "LoginUserJID"); //sunuser1@localhost/sunpc1
    //get this.req.iq_to from AppServerAgentJID i.e.dmsnew@localhost/dmsnew
    this.req.iq_to = topic_Get(tApp, "AppServerAgentJID");

    this.req.RequestDataObjectArr = new Object;

    if( this.nodeBookmark != null ) {
      this.req.RequestDataObjectArr[this.ObjectType] = this.nodeBookmark;
    }
    ReqSessionHandleBookmark.BaseClass.sendRequest.call(this);
  }
  // redefine onsuccess
  this.onSuccess = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionHandleBookmark.BaseClass.onSuccess.call(this, req, ErrCode, response);
  }
  // redefine onfail
  this.onFail = function(req, ErrCode, response) {
    if( this.ReqEvent != null && this.ReqEvent.eventval == null ) {
      this.ReqEvent.eventval = "DATE";
    }
    ReqSessionHandleBookmark.BaseClass.onFail.call(this, req, ErrCode, response);
  }
}

//---------------------------------------
function utilObjectListCacheSync(response, ObjectListNode, ObjectType) {
  var cacheRootNode = getObjectCacheListNode(ObjectType);
  if ( response != null ) {
    var cloneResponseString = response.toString();
    var cloneResponseNode = new jsx3.xml.Document;
    cloneResponseNode.loadXML(cloneResponseString);
   
    var cacheRootNIDIter = cacheRootNode.selectNodeIterator(".//"+ObjectType);
    while( cacheRootNIDIter.hasNext() ) {
      var elementCacheNode = cacheRootNIDIter.next();
      objectNID = elementCacheNode.getAttribute("NID");
      var matchResult = objectNID.search(/DEFAULT/);
      if(matchResult != -1 ) {
      } else {
        var responseObjNIDXPath = "[ObjectNID='" + objectNID + "']/ObjectNID";
        var responseObjNIDNode = cloneResponseNode.selectSingleNode("//*"+responseObjNIDXPath );
        if( responseObjNIDNode == null) {
          cacheRootNode.removeChild(elementCacheNode);
        }
      }  
    }
  }
}

//---------------------------------------
function utilObjectListUpdate(AddDel, response, ObjectListNode, ObjectGroupTreeNode, ObjectGroupTagXPath) {
  if( ObjectListNode == null ) return;
  var topicDataListNode = topic_Set(tApp, ObjectListNode, null);
  if( response != null ) {
    var responseNIDIter = response.selectNodeIterator(".//ObjectNID");
    while( responseNIDIter.hasNext() ) {
      var objNIDNode = responseNIDIter.next().cloneNode(true);
      var objNID = objNIDNode.getValue();
      if( AddDel == "ADD" ) {
        if( topicDataListNode.selectSingleNode(".//ObjectNID[.='" + objNID + "']") == null ) {
          topicDataListNode.appendChild(objNIDNode);
        }
      }
      else if( AddDel == "DELETE" ) {
        var listNIDNode = topicDataListNode.selectSingleNode(".//ObjectNID[.='" + objNID + "']");
        if( listNIDNode != null ) {
          topicDataListNode.removeChild(listNIDNode);
        }
      }
    }
  }
  if( ObjectGroupTreeNode != null ) {
    var objectGroupTree = utilObjectNIDListToGroupTree(topicDataListNode, ObjectGroupTagXPath, null);
    var topicDataTreeNode = topic_Set(tApp, ObjectGroupTreeNode, null);
    topicDataTreeNode.removeChildren();
    topicDataTreeNode.appendChild(objectGroupTree);
  }
}
//---------------------------------------    
function utilObjectNIDListToGroupTree(listNode, ObjectGroupTagXPath, hmode) {
  // helper funtion
  function utilGroupTreeAddGroup(pNode, GroupName, hmode) {
    var GroupNameTail = null;
    if( hmode != null ) {
      GroupNameTail = GroupName.replace(/.*\/(.*)$/, "$1");
      GroupName = GroupName.replace(/^(.*)\/.*$/, "$1");
    }    
    var gNode = pNode.selectSingleNode("./Group[@Name='" + GroupName + "']");
    if( gNode == null ) {
      gNode = pNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "Group");
      gNode.setAttribute("Name", GroupName);
      fcNode = pNode.getFirstChild();
      if( fcNode == null ) {
        pNode.appendChild(gNode);
      }
      else {
        pNode.insertBefore(gNode, fcNode);
      }   
    }
    pNode = gNode;
    if( GroupNameTail != null ) {
      pNode = utilGroupTreeAddGroup(pNode, GroupNameTail, hmode);
    }
    return pNode;  
  }
  
  // create tree node 
  var treeNode = new jsx3.xml.Document().createDocumentElement("ObjectNIDTree");
  // create top level unassigned entry 
  var treeUNode = utilGroupTreeAddGroup(treeNode, "_unassigned_", null);
  // for each NID in list - get obj from cache
  var objectNIDNodeIter = listNode.selectNodeIterator(".//ObjectNID");
  while( objectNIDNodeIter.hasNext() ) {
    var objectNIDNode = objectNIDNodeIter.next().cloneNode(true);
    var ObjectNID = objectNIDNode.getValue();
    var objectNode = topic_GetNIDNode(tCache, ObjectNID); 
    if( objectNode == null ) continue;
    // set parent = unassigned
    var parentNode = treeUNode;
    if( ObjectGroupTagXPath != null ) {
      // for each obj - iter over tags
      var objectTagNodeIter = objectNode.selectNodeIterator(ObjectGroupTagXPath);
      while( objectTagNodeIter.hasNext() ) {
        // for each tag - get grouptype(tagname) 
        var objectTagNode = objectTagNodeIter.next().cloneNode(true);
        var TagName = objectTagNode.getAttribute("Name");
        if( TagName != null && TagName != "" ) {
          //-check/set under toplevel - set as parent
          parentNode = utilGroupTreeAddGroup(treeNode, TagName, null);
          // create unassigned under parent - set as parent    
          var parentUNode = utilGroupTreeAddGroup(parentNode, "_unassigned_", null);
          // get group(tagval)
          var TagVal = objectTagNode.getAttribute("Value");
          if( TagVal != null && TagVal != "") {
            //-check/set under parent - set as parent
            parentNode = utilGroupTreeAddGroup(parentNode, TagVal, hmode);
          }
          else {
            parentNode = parentUNode;
          }
        }
        // put NID under parent
        // cloning objectNIDNode because it is being appended to diff tags. Otherwise only
        // 1 of the tags will show the objectNIDNode
        parentNode.appendChild(objectNIDNode.cloneNode(true));
      }
    }
    // if no group tags - put under unassigned
    if( parentNode == treeUNode ) {
      // put NID under parent
      parentNode.appendChild(objectNIDNode.cloneNode(true));
    }
  }
  return treeNode;
}
//---------------------------------------
function utilRuleGetOutboundDoc(RuleName) {
  try {
    var objService = PNameSpace.loadResource(RuleName + "_xml");
    objService.setOperation("");
    objService.caller = this;
    objService.requestHandle = this;
    var ruleDoc = objService.getServiceMessage();
    return ruleDoc;
  }
  catch(e) {
    handleFatalError( "Internal Error: Outbound Map Rule: " + RuleName + "\n" );
    return null;
  }    
}

//---------------------------------------
function utilRuleSetInboundDoc(RuleName, ruleDoc) {
  try {
    var objService = PNameSpace.loadResource(RuleName + "_xml");
	  objService.setOperation("");
	  objService.setInboundDocument(ruleDoc);
	  objService.doInboundMap();
	}
	catch(e) {
	  handleFatalError( "Internal Error: Inbound Map Rule: " + RuleName + "\n" +
	                          "Input Document: " + ruleDoc );
	}
}

function getServiceVersion(){
  var serviceVersion = topic_Get(tApp, "ServiceVersion");
  return serviceVersion;
}
