/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* API class provides async methods for making requests.
*/
jsx3.Class.defineClass("com.ipvs.api.API", null, null, function(API, API_prototype) {
  
  /** @private @jsxobf-clobber */
  API._LOG = jsx3.util.Logger.getLogger(API.jsxclass.getName());
  
  /**
   * Login method will send asynchronous request for XMPP login.
   * <p/>
   * The arguments and the order of those arguments for this method are:;
   * <p/>
   * @param UserName {String} 
   * @param Password {String}
   * @param LoginDomain {String} 
   * @param LoginResource {String}
   * @param LoginServer {String} 
   * @param LoginPort {String}
   * @return objResponse (asyncRV).
   */
  API.Login = jsx3.$Y(function(cb) {    
    var urlStr = 'ipvs:///LoginRequest?ServiceName=xmpp' + 
                 '&OpType=Operation' +  
                 '&User=' + escape(cb.args()[0]) + 
                 '&Password=' + escape(cb.args()[1]) + 
                 '&Domain=' + escape(cb.args()[2]) + 
                 '&Resource=' + escape(cb.args()[3]) + 
                 '&Server=' + escape(cb.args()[4]) + 
                 '&Port=' + escape(cb.args()[5]) + 
                 '&ObjectType=Login';
    var LoginRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending Login request... ::RequestURL:" + urlStr);
    LoginRequest.async_send(urlStr).when(function(objResponse) {
      LoginRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * SetServiceDomain method will send asynchronous request to set ServiceDomain.
   * <p/>
   * The arguments and the order of those arguments for this method are:;
   * <p/>
   * @param ServiceDomain {String} 
   * @param AssetTemplateTitle {String}
   * @param AssetPrivateKeyScheme {String} 
   * @param AssetPrivateKeyData {String}
   * @param AssetPrivateData {String} 
   * @param ServiceVersion {String}
   * @param SoftwareVersion {String} 
   * @param IPAddress {String}
   * @return objResponse (asyncRV).
   */
  API.SetServiceDomain = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///SetServiceDomain?ServiceName=systemservice' + 
                 '&OpType=Operation' +  
                 '&ServiceDomain=' + escape(cb.args()[0]) + 
                 '&AssetTemplateTitle=' + escape(cb.args()[1]) + 
                 '&AssetPrivateKeyScheme=' + escape(cb.args()[2]) + 
                 '&AssetPrivateKeyData=' + escape(cb.args()[3]) + 
                 '&AssetPrivateData=' + escape(cb.args()[4]) + 
                 '&ServiceVersion=' + escape(cb.args()[5]) + 
                 '&SoftwareVersion=' + escape(cb.args()[6]) + 
                 '&IPAddress=' + escape(cb.args()[7]) + 
                 '&AppName=' + escape(cb.args()[8]) + 
                 '&ObjectType=SetServiceDomain';
    var SetServiceDomainRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending SetServiceDomain request... ::RequestURL:" + urlStr);
    SetServiceDomainRequest.async_send(urlStr).when(function(objResponse) {
      SetServiceDomainRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetServiceClient method will send asynchronous request to get service client info.
   * @return objResponse (asyncRV).
   */
  API.GetServiceClient = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///getServiceClient?ServiceName=sessionservice' + 
                 '&OpType=Operation' +
                 '&ObjectList_SelectFormat=EXTENDED' +
                 '&RequestName=GetServiceClientRequest' +
                 '&iq_type=set ' +
                 '&iq_from=' + escape(cb.args()[0]) +
                 '&iq_to=' + escape(cb.args()[1]) +
                 '&ObjectType=GetServiceClient';
                 
    var GetServiceClientRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetServiceClient request... ::RequestURL:" + urlStr);
    GetServiceClientRequest.async_send(urlStr).when(function(objResponse) {
      GetServiceClientRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetServerTime method will send asynchronous request to get ServerTime.
   * @return objResponse (asyncRV).
   */
  API.GetServerTime = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///getSrerverTime?ServiceName=systemservice' + 
                 '&OpType=Operation' +
                 '&ObjectType=GetServerTime';
    var GetServerTimeRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetServerTime request... ::RequestURL:" + urlStr);
    GetServerTimeRequest.async_send(urlStr).when(function(objResponse) {
      GetServerTimeRequest.endRequest();
      cb.done(objResponse);
    });
  });
  
  /**
   * Logout method will send asynchronous request for Logout.
   * <p/>
   * The arguments and the order of those arguments for this method are:;
   * <p/>
   * @param UserJID {String}
   * @return objResponse (asyncRV).
   */ 
  API.Logout = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///LogoutRequest?ServiceName=xmpp' + 
                 '&OpType=Operation' +  
                 '&userJID=' + escape(cb.args()[0]) +
                 '&ObjectType=Logout';
    var LogoutRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending Logout request... ::RequestURL:" + urlStr);
    LogoutRequest.async_send(urlStr).when(function(objResponse) {
      LogoutRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * SetUserPassword method will send asynchronous request to change user password.
   * <p/>
   * The arguments and the order of those arguments for this method are:;
   * <p/>
   * @param OldPassword {String}
   * @param NewPassword {String}
   * @return objResponse (asyncRV).
   */ 
  API.SetUserPassword = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///ChangePassword?ServiceName=sessionservice' + 
                 '&RequestName=SetUserPasswordRequest' +
                 '&iq_type=set' +
                 '&oldPassword=' + escape(cb.args()[0]) + 
                 '&newPassword=' + escape(cb.args()[1]);
    var SetUserPasswordRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending SetUserPassword request... ::RequestURL:" + urlStr);
    SetUserPasswordRequest.async_send(urlStr).when(function(objResponse) {
      SetUserPasswordRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetPortTags method will send asynchronous request to get port tags.
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.GetPortTags = jsx3.$Y(function(cb) {
    var urlStr = API.GetTagsURL(cb.args()[0], "GetPortTags");
    var GetPortTagsRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetPortTags request... ::RequestURL:" + urlStr);
    GetPortTagsRequest.async_send(urlStr).when(function(objResponse) {
      GetPortTagsRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * GetFileTags method will send asynchronous request to get file tags.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.GetFileTags = jsx3.$Y(function (cb) {
    var urlStr = API.GetTagsURL(cb.args()[0], "GetFileTags");
    var GetFileTagsRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetFileTags request... ::RequestURL:" + urlStr);
    GetFileTagsRequest.async_send(urlStr).when(function (objResponse) {
      GetFileTagsRequest.endRequest();
      cb.done(objResponse);
    });
  });
  
  /**
   * GetTagsURL method will form the url for GetPortTags/GetDirTags/GetFileTags request. .
   * @param objReqParams {Object}
   * @param reqName {String}
   * @return urlStr (String).
   */
  API.GetTagsURL = function(objReqParams, reqName) {
    var urlStr = 'ipvs:///' + reqName + '?ServiceName=sessionservice' + 
                 '&RequestName=' + reqName + 'Request' +
                 '&iq_type=get' +
                 '&ObjectList_StartFrom=' + API._getParamVal(objReqParams, "StartFrom") + 
                 '&ObjectList_CountToFetch=' + API._getParamVal(objReqParams, "CountToFetch") +
                 '&ObjectList_SortBy=' + API._getParamVal(objReqParams, "SortBy") + 
                 '&ObjectList_SortOrder=' + API._getParamVal(objReqParams, "SortOrder") + 
                 '&ObjectList_SelectFormat=' + API._getParamVal(objReqParams, "SelectFormat") +
                 '&ObjectList_WhereNameTextSubStr=' + API._getParamVal(objReqParams, "WhereNameTextSubStr") +
                 '&ObjectList_WhereValueTextSubStr=' + API._getParamVal(objReqParams, "WhereValueTextSubStr") +
                 '&ObjectList_WherePermissions=' + API._getParamVal(objReqParams, "WherePermissions");
    return urlStr;
  }

  /**
   * GetPort method will send asynchronous request to get ports list .
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.GetPort = jsx3.$Y(function(cb) {
    var urlStr = API.GetPortURL(cb.args()[0]);
    var GetPortRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetPort request... ::RequestURL:" + urlStr);
    GetPortRequest.async_send(urlStr).when(function(objResponse) {
      GetPortRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetPortURL method will form the url for GetPort request. .
   * @param objReqParams {Object}
   * @return urlStr (string).
   */
  API.GetPortURL = function(objReqParams) {
    var urlStr = 'ipvs:///GetPort?ServiceName=sessionservice' + 
                 '&RequestName=GetPortRequest' +
                 '&iq_type=get' +
                 '&ObjectList_Type=' + API._getParamVal(objReqParams, "Type") + 
                 '&ObjectList_StartFrom=' + API._getParamVal(objReqParams, "StartFrom") + 
                 '&ObjectList_CountToFetch=' + API._getParamVal(objReqParams, "CountToFetch") +
                 '&ObjectList_SortBy=' + API._getParamVal(objReqParams, "SortBy") + 
                 '&ObjectList_SortOrder=' + API._getParamVal(objReqParams, "SortOrder") + 
                 '&ObjectList_SelectFormat=' + API._getParamVal(objReqParams, "SelectFormat") +
                 '&ObjectList_WhereNID=' + API._getParamVal(objReqParams, "WhereNID") +
                 '&ObjectList_WhereTextSubStr=' + API._getParamVal(objReqParams, "WhereTextSubStr") + 
                 '&ObjectList_WhereDateRange=' + API._getParamVal(objReqParams, "WhereDateRange") + 
                 '&ObjectList_WhereTagExpr=' + API._getParamVal(objReqParams, "WhereTagExpr") +
                 '&ObjectList_WhereStreamType=' + API._getParamVal(objReqParams, "WhereStreamType") +
                 '&ObjectList_WherePermissions=' + API._getParamVal(objReqParams, "WherePermissions") +
                 '&ObjectList_WhereStreamType=' + API._getParamVal(objReqParams, "WhereStreamType") +  
                 '&ObjectList_SubscriptionID=' + API._getParamVal(objReqParams, "SubscriptionID");
    return urlStr;
  }
  
  /**
   * GetDir method will send asynchronous request to get dir list .
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.GetDir = jsx3.$Y(function(cb) {
    var urlStr = API.GetDirURL(cb.args()[0]);
    var GetDirRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetDir request... ::RequestURL:" + urlStr);
    GetDirRequest.async_send(urlStr).when(function(objResponse) {
      GetDirRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetDirURL method will form the url for GetDir request. .
   * @param objReqParams {Object}
   * @return urlStr (string).
   */
  API.GetDirURL = function(objReqParams) {
    var urlStr = 'ipvs:///GetDir?ServiceName=sessionservice' + 
                 '&RequestName=GetDirRequest' +
                 '&iq_type=get' +
                 '&ObjectList_StartFrom=' + API._getParamVal(objReqParams, "StartFrom") + 
                 '&ObjectList_CountToFetch=' + API._getParamVal(objReqParams, "CountToFetch") +
                 '&ObjectList_SortBy=' + API._getParamVal(objReqParams, "SortBy") + 
                 '&ObjectList_SortOrder=' + API._getParamVal(objReqParams, "SortOrder") + 
                 '&ObjectList_SelectFormat=' + API._getParamVal(objReqParams, "SelectFormat") +
                 '&ObjectList_WhereNID=' + API._getParamVal(objReqParams, "WhereNID") +
                 '&ObjectList_WhereTextSubStr=' + API._getParamVal(objReqParams, "WhereTextSubStr") + 
                 '&ObjectList_WhereDevice=' + API._getParamVal(objReqParams, "WhereDevice") + 
                 '&ObjectList_WhereTagExpr=' + API._getParamVal(objReqParams, "WhereTagExpr") +
                 '&ObjectList_WhereParentNID=' + API._getParamVal(objReqParams, "WhereParentDirNID") +
                 '&ObjectList_WherePermissions=' + API._getParamVal(objReqParams, "WherePermissions") +                 
                 '&ObjectList_SubscriptionID=' + API._getParamVal(objReqParams, "SubscriptionID");
    return urlStr;
  }

  /**
   * GetFile method will send asynchronous request to get list files from a directory.
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.GetFile = jsx3.$Y(function(cb) {
    var urlStr = API.GetFileURL(cb.args()[0]);
    var GetFileRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetFile request... ::RequestURL:" + urlStr);
    GetFileRequest.async_send(urlStr).when(function(objResponse) {
      GetFileRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetFileURL method will form the url for GetFile request. .
   * @param objReqParams {Object}
   * @return urlStr (string).
   */
  API.GetFileURL = function(objReqParams) {
    var urlStr = 'ipvs:///GetFile?ServiceName=sessionservice' + 
                 '&RequestName=GetFileRequest' +
                 '&iq_type=get' +
                 '&ObjectList_StartFrom=' + API._getParamVal(objReqParams, "StartFrom") + 
                 '&ObjectList_CountToFetch=' + API._getParamVal(objReqParams, "CountToFetch") +
                 '&ObjectList_SortBy=' + API._getParamVal(objReqParams, "SortBy") + 
                 '&ObjectList_SortByMetaData=' + API._getParamVal(objReqParams, "SortByMetaData") + 
                 '&ObjectList_SortOrder=' + API._getParamVal(objReqParams, "SortOrder") + 
                 '&ObjectList_SelectFormat=' + API._getParamVal(objReqParams, "SelectFormat") +
                 '&ObjectList_WhereNID=' + API._getParamVal(objReqParams, "WhereNID") +
                 '&ObjectList_WhereParentNID=' + API._getParamVal(objReqParams, "WhereParentDirNID") +  
                 '&ObjectList_WhereState=' + API._getParamVal(objReqParams, "WhereState") +
                 '&ObjectList_WhereType=' + API._getParamVal(objReqParams, "WhereType") + 
                 '&ObjectList_WhereTextSubStr=' + API._getParamVal(objReqParams, "WhereTextSubStr") + 
                 '&ObjectList_SearchTextSubStrIn=' + API._getParamVal(objReqParams, "SearchTextSubStrIn") + 
                 '&ObjectList_WhereDateRange=' + API._getParamVal(objReqParams, "WhereDateRange") +
                 '&ObjectList_WhereTagExpr=' + API._getParamVal(objReqParams, "WhereTagExpr") +
                 '&ObjectList_WherePermissions=' + API._getParamVal(objReqParams, "WherePermissions") +  
                 '&ObjectList_WhereMetaDataExpr=' + API._getParamVal(objReqParams, "WhereMetaDataExpr") +                               
                 '&ObjectList_SubscriptionID=' + API._getParamVal(objReqParams, "SubscriptionID");
    return urlStr;
  }

  /**
   * AuditEvent method will send asynchronous request to set event for audit log.
   * <p/>
   * The arguments and the order of those arguments for this method are:;
   * <p/>
   * @param Type {String}
   * @param Date {String}
   * @param Time {String}
   * @param Level {String}
   * @param Name {String}
   * @param Description {Etity}
   * @return objResponse (asyncRV).
   */
  API.AuditEvent = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///AuditEvent?ServiceName=sessionservice' + 
                 '&RequestName=AuditEventRequest' +
                 '&iq_type=set' +
                 '&type=' + cb.args()[0] + 
                 '&date=' + cb.args()[1] + 
                 '&time=' + cb.args()[2] +  
                 '&level=' + cb.args()[3] + 
                 '&name=' + cb.args()[4];

    var AuditEventRequest = new com.ipvs.api.Request();
    AuditEventRequest.RequestDataObjectArr["Description"] = cb.args()[5];
    API._LOG.info("API:Sending AuditEvent request... ::RequestURL:" + urlStr);
    AuditEventRequest.async_send(urlStr).when(function(objResponse) {
      AuditEventRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * SetFileMetaData method will send asynchronous request to set/update MetaData for a file.
   * <p/>
   * The arguments and the order of those arguments for this method are:;
   * <p/>
   * @param ObjectList_WhereNID {String}
   * @param MetaData {Entity}
   * @param ObjectList_WhereTextSubStr {String}
   * @return objResponse (asyncRV).
   */ 
  API.SetFileMetaData = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///SetFileMetaData?ServiceName=sessionservice' + 
                 '&RequestName=SetFileMetaDataRequest' +
                 '&iq_type=set' +
                 '&ObjectList_WhereNID=' + cb.args()[0] + 
                 '&ObjectList_WhereTextSubStr=' + cb.args()[2];
    var SetFileMetaDataRequest = new com.ipvs.api.Request();
    SetFileMetaDataRequest.RequestDataObjectArr["MetaData"] = cb.args()[1];
    API._LOG.info("API:Sending SetFileMetaData request... ::RequestURL:" + urlStr);
    SetFileMetaDataRequest.async_send(urlStr).when(function(objResponse) {
      SetFileMetaDataRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * ViewOrPlayback method will form the url for ViewOrPlayback request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetViewOrPlaybackURL = function(objReqParams) {
  
  var urlStr = 'ipvs:///' + API._getParamVal(objReqParams, "ReqType") + '?ServiceName=sessionservice' +
                 '&RequestName=' + API._getParamVal(objReqParams, "ReqType") + 'Request' +
                 '&iq_type=get' +
                 '&ObjectList_sessionId=' + API._getParamVal(objReqParams, "SessionId") +
                 '&ObjectList_connId=' + API._getParamVal(objReqParams, "ConnId") +
                 '&ObjectList_connName=' + API._getParamVal(objReqParams, "ConnName") +
                 ( API._getParamVal(objReqParams, "ReqType") == "View"
                  ? '&ObjectList_srcPortNID=' + API._getParamVal(objReqParams, "SrcPortNID") +
                    '&ObjectList_srcPortTitle=' + API._getParamVal(objReqParams, "SrcPortTitle") +
                    '&ObjectList_pvrEnable=' + API._getParamVal(objReqParams, "pvrEnable")
                  : '&ObjectList_fileNID=' + API._getParamVal(objReqParams, "FileNID") ) +
                 '&ObjectList_dstEntityId=' + API._getParamVal(objReqParams, "DstEntityId") +
                 '&ObjectList_dstPortNID=' + API._getParamVal(objReqParams, "DstPortNID") +
                 '&ObjectList_dstPortTitle=' + API._getParamVal(objReqParams, "DstPortTitle") +
                 '&ObjectList_canvasId=' + API._getParamVal(objReqParams, "CanvasId") +
                 '&ObjectList_windowId=' + API._getParamVal(objReqParams, "WindowId") +
                 '&ObjectList_restartSession=' + API._getParamVal(objReqParams, "RestartSession");
  
  
    return urlStr;
  }

  /**
  * ViewOrPlayback method will send asynchronous request to View(Start) a session.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.ViewOrPlayback = jsx3.$Y(function(cb) {
    var urlStr = API.GetViewOrPlaybackURL(cb.args()[0]);
    var GetViewOrPlaybackRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending ViewOrPlayback request... ::RequestURL:" + urlStr);
    GetViewOrPlaybackRequest.async_send(urlStr).when(function(objResponse) {
      GetViewOrPlaybackRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /*
  <Record sessionId="" sessionName="" id="" name="">
    <SrcPort id="" portNID="" portTitle="tx1"/>
    <DstFile id="" fileTitle="">
      <MetaData>
        <Tag Name="ID">1299792256</Tag>
        <Tag Name="TestCase">BasicRecording</Tag>
      </MetaData>
    </DstFile>
  </Record>
*/
  /**
  * Record method will form the url for Record request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetRecordURL = function(objReqParams) {
  
    var urlStr = 'ipvs:///RecordRequest?ServiceName=sessionservice' +
                 '&RequestName=RecordRequest' +
                 '&iq_type=set' +
                 '&ObjectList_sessionId=' + API._getParamVal(objReqParams, "SessionId") +
                 '&ObjectList_connId=' + API._getParamVal(objReqParams, "ConnId") +
                 '&ObjectList_connName=' + API._getParamVal(objReqParams, "ConnName") +
                 '&ObjectList_srcPortNID=' + API._getParamVal(objReqParams, "SrcPortNID") +
                 '&ObjectList_srcPortTitle=' + API._getParamVal(objReqParams, "SrcPortTitle") +
                 '&ObjectList_dstFileTitle=' + API._getParamVal(objReqParams, "DstFileTitle") +
                 '&ObjectList_srcEntityId=' + API._getParamVal(objReqParams, "SrcEntityId") +
                 '&ObjectList_resetTimeline=' + API._getParamVal(objReqParams, "resetTimeline");
  
    return urlStr;
  }    
    
  /**
  * Record method will send asynchronous request to View(Start) a session.
  * @param objReqParams {Object}
  * @param MetaData {Entity}
  * @return objResponse (asyncRV).
  */
  API.Record = jsx3.$Y(function(cb) {
    var urlStr = API.GetRecordURL(cb.args()[0]);
    var GetRecordRequest = new com.ipvs.api.Request();
    GetRecordRequest.RequestDataObjectArr["MetaData"] = cb.args()[1];
    API._LOG.info("API:Sending Record request... ::RequestURL:" + urlStr);
    GetRecordRequest.async_send(urlStr).when(function(objResponse) {
      GetRecordRequest.endRequest();
      cb.done(objResponse);
    });
  });
  

  /**
  * GetDelConnAgentEventMessageURL method will form the url for DelConnAgentEventMessage request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetDelConnAgentEventMessageURL = function(objReqParams) {
    var urlStr = 'ipvs:///AgentEvent?ServiceName=stemservice' +
                 '&RequestName=ViewRequest' +
                 '&iq_type=get';
    //TBD:refer to clsRequest Line 643
    return urlStr;
  }

  /**
  * DelConnAgentEventMessage method will send asynchronous request to DelConn for UMC (??).
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.DelConnAgentEventMessage = jsx3.$Y(function(cb) {
    var urlStr = API.GetDelConnAgentEventMessageURL(cb.args()[0]);
    var DelConnAgentEventMessageRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending DelConnAgentEventMessage request... ::RequestURL:" + urlStr);
    DelConnAgentEventMessageRequest.async_send(urlStr).when(function(objResponse) {
      DelConnAgentEventMessageRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * GetStartSessionURL method will form the url for StartSession request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetStartSessionURL = function(objReqParams) {
    var urlStr = 'ipvs:///Start?ServiceName=sessionservice' +
                 '&RequestName=StartRequest' +
                 '&iq_type=get' +
                 '&SessionID=' + API._getParamVal(objReqParams, "id") +
                 '&SessionConfigID=' + API._getParamVal(objReqParams, "sessionConfigId");
    return urlStr;
  }

  /**
  * StartSessionRequest method will send asynchronous request to Start a session.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.StartSession = jsx3.$Y(function(cb) {
    var urlStr = API.GetStartSessionURL(cb.args()[0]);
    var StartSessionRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending StartSession request... ::RequestURL:" + urlStr);
    StartSessionRequest.async_send(urlStr).when(function(objResponse) {
      StartSessionRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * GetStopSessionURL method will form the url for StopSession request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetStopSessionURL = function(objReqParams) {
    var urlStr = 'ipvs:///Stop?ServiceName=sessionservice' +
                 '&RequestName=StopRequest' +
                 '&iq_type=get' +
                 '&session_id=' + API._getParamVal(objReqParams, "sessionId");
    return urlStr;
  }

  /**
  * StopSession method will send asynchronous request to Stop a session.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.StopSession = jsx3.$Y(function(cb) {
    var urlStr = API.GetStopSessionURL(cb.args()[0]);
    var StopSessionRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending StopSession request... ::RequestURL:" + urlStr);
    StopSessionRequest.async_send(urlStr).when(function(objResponse) {
      StopSessionRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * GetSessionDeleteConnURL method will form the url for SessionDeleteConn request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetSessionDeleteConnURL = function(objReqParams) {
    var urlStr = 'ipvs:///DeleteConn?ServiceName=sessionservice' +
                 '&RequestName=DeleteConnRequest' +
                 '&iq_type=get' +
                 '&ObjectList_sessionId=' + API._getParamVal(objReqParams, "SessionId") +
                 '&ObjectList_connId=' + API._getParamVal(objReqParams, "ConnId");
    return urlStr;
  }

  /**
  * SessionDeleteConn method will send asynchronous request to delete a connection.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.SessionDeleteConn = jsx3.$Y(function(cb) {
    var urlStr = API.GetSessionDeleteConnURL(cb.args()[0]);
    var SessionDeleteConnRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending SessionDeleteConn request... ::RequestURL:" + urlStr);
    SessionDeleteConnRequest.async_send(urlStr).when(function(objResponse) {
      SessionDeleteConnRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * GetSessionPvrURL method will form the url for SessionPvr request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetSessionPvrURL = function(objReqParams) {
    var urlStr = 'ipvs:///'+objReqParams.action+'?ServiceName=sessionservice' +
                 '&RequestName=' + objReqParams.action + 'Request' +
                 '&iq_type=set' +
                 //'&iq_from=' + API._getParamVal(objReqParams, "iqFrom") +
                // '&iq_to=' + API._getParamVal(objReqParams, "iqTo") +
                 '&session_id=' + API._getParamVal(objReqParams, "sessionId") +
                 '&skipOffset=' + API._getParamVal(objReqParams, "skipOffset");
    return urlStr;
  }

  /**
  * SessionDeleteConn method will send asynchronous request to delete a connection.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.SessionPvr = jsx3.$Y(function(cb) {
    var urlStr = API.GetSessionPvrURL(cb.args()[0]);
    var SessionPvrRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending SessionPvr request... ::RequestURL:" + urlStr);
    SessionPvrRequest.async_send(urlStr).when(function(objResponse) {
      SessionPvrRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * GetResumeFromBookmarkURL method will form the url for ResumeFromBookmark request.
  * @param objReqParams {Object}
  * @return urlStr (string).
  */
  API.GetResumeFromBookmarkURL = function(objReqParams) {
    var urlStr = 'ipvs:///ResumeFromBookmark?ServiceName=sessionservice' +
                 '&RequestName=ResumeFromBookmarkRequest' +
                 '&iq_type=set' +
                 //'&iq_from=' + API._getParamVal(objReqParams, "iqFrom") +
                 //'&iq_to=' + API._getParamVal(objReqParams, "iqTo") +
                 '&session_id=' + API._getParamVal(objReqParams, "sessionId") +
                 '&bookmark_id=' + API._getParamVal(objReqParams, "bookmarkId");
    return urlStr;
  }

  /**
  * ResumeFromBookmarkRequest method will send asynchronous request to ResumeFromBookmark.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.ResumeFromBookmark = jsx3.$Y(function(cb) {
    var urlStr = API.GetResumeFromBookmarkURL(cb.args()[0]);
    var ResumeFromBookmarkRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending ResumeFromBookmark request... ::RequestURL:" + urlStr);
    ResumeFromBookmarkRequest.async_send(urlStr).when(function(objResponse) {
      ResumeFromBookmarkRequest.endRequest();
      cb.done(objResponse);
    });
  });
  
  /**
   * GetSession method will send asynchronous request to get sessions list.
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.GetSession = jsx3.$Y(function(cb) {
    var urlStr = API.GetSessionURL(cb.args()[0]);
    var GetSessionRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetSession request... ::RequestURL:" + urlStr);
    GetSessionRequest.async_send(urlStr).when(function(objResponse) {
      GetSessionRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * NewBookmark method will send asynchronous request to get new bookmark.
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.NewBookmark = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///NewBookmark?ServiceName=sessionservice' + 
                 '&RequestName=NewBookmarkRequest' +
                 '&iq_type=get' +
                 '&ObjectList_WhereFileNID=' + API._getParamVal(cb.args()[0], "WhereFileNID") + 
                 '&ObjectList_WhereSessionInfo=' + API._getParamVal(cb.args()[0], "WhereSessionInfo");
    var NewBookmarkRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending NewBookmark request... ::RequestURL:" + urlStr);
    NewBookmarkRequest.async_send(urlStr).when(function(objResponse) {
      NewBookmarkRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * AddBookmarkRequest method will send asynchronous request to AddBookmark.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.AddBookmark = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///' + cb.args()[0] + '?ServiceName=sessionservice' +
                 '&RequestName=' + cb.args()[0] + 'Request'+
                 '&iq_type=get';
    var AddBookmarkRequest = new com.ipvs.api.Request();
    AddBookmarkRequest.RequestDataObjectArr["AddBookmark"] = cb.args()[1];
    API._LOG.info("API:Sending AddBookmarkRequest request... ::RequestURL:" + urlStr);
    AddBookmarkRequest.async_send(urlStr).when(function(objResponse) {
      AddBookmarkRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * DeleteBookmarkRequest method will send asynchronous request to DeleteBookmark.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.DeleteBookmark = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///' + cb.args()[0] + '?ServiceName=sessionservice' +
                 '&RequestName=' + cb.args()[0] + 'Request'+
                 '&iq_type=get';
    var DeleteBookmarkRequest = new com.ipvs.api.Request();
    DeleteBookmarkRequest.RequestDataObjectArr["DeleteBookmark"] = cb.args()[1];
    API._LOG.info("API:Sending DeleteBookmarkRequest request... ::RequestURL:" + urlStr);
    DeleteBookmarkRequest.async_send(urlStr).when(function(objResponse) {
      DeleteBookmarkRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
  * UpdateBookmarkRequest method will send asynchronous request to UpdateBookmark.
  * @param objReqParams {Object}
  * @return objResponse (asyncRV).
  */
  API.UpdateBookmark = jsx3.$Y(function(cb) {
    var urlStr = 'ipvs:///' + cb.args()[0] + '?ServiceName=sessionservice' +
                 '&RequestName=' + cb.args()[0] + 'Request'+
                 '&iq_type=set';
    var UpdateBookmarkRequest = new com.ipvs.api.Request();
    UpdateBookmarkRequest.RequestDataObjectArr["UpdateBookmark"] = cb.args()[1];
    API._LOG.info("API:Sending UpdateBookmarkRequest request... ::RequestURL:" + urlStr);
    UpdateBookmarkRequest.async_send(urlStr).when(function(objResponse) {
      UpdateBookmarkRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetSessionURL method will form the url for GetSession request. .
   * @param objReqParams {Object}
   * @return urlStr (string).
   */
  API.GetSessionURL = function(objReqParams) {
    var urlStr = 'ipvs:///GetSession?ServiceName=sessionservice' + 
                 '&RequestName=GetSessionRequest' +
                 '&iq_type=get' +
                 '&ObjectList_StartFrom=' + API._getParamVal(objReqParams, "StartFrom") + 
                 '&ObjectList_CountToFetch=' + API._getParamVal(objReqParams, "CountToFetch") +
                 '&ObjectList_SortBy=' + API._getParamVal(objReqParams, "SortBy") + 
                 '&ObjectList_SortOrder=' + API._getParamVal(objReqParams, "SortOrder") + 
                 '&ObjectList_SelectFormat=' + API._getParamVal(objReqParams, "SelectFormat") +
                 '&ObjectList_WhereID=' + API._getParamVal(objReqParams, "WhereID") + 
                 '&ObjectList_WhereTextSubStr=' + API._getParamVal(objReqParams, "WhereTextSubStr") + 
                 '&ObjectList_WhereTagExpr=' + API._getParamVal(objReqParams, "WhereTagExpr") +                                
                 '&ObjectList_SubscriptionID=' + API._getParamVal(objReqParams, "SubscriptionID");
    return urlStr;
  }

  /**
   * GetBookmark method will send asynchronous request to get bookmarks list for a File/Session.
   * @param objReqParams {Object}
   * @return objResponse (asyncRV).
   */
  API.GetBookmark = jsx3.$Y(function(cb) {
    var urlStr = API.GetBookmarkURL(cb.args()[0]);
    var GetBookmarkRequest = new com.ipvs.api.Request();
    API._LOG.info("API:Sending GetBookmark request... ::RequestURL:" + urlStr);
    GetBookmarkRequest.async_send(urlStr).when(function(objResponse) {
      GetBookmarkRequest.endRequest();
      cb.done(objResponse);
    });
  });

  /**
   * GetBookmarkURL method will form the url for GetBookmark request. .
   * @param objReqParams {Object}
   * @return urlStr (string).
   */
  API.GetBookmarkURL = function(objReqParams) {
    var urlStr = 'ipvs:///GetBookmark?ServiceName=sessionservice' + 
                 '&RequestName=GetBookmarkRequest' +
                 '&iq_type=get' +
                 '&ObjectList_StartFrom=' + API._getParamVal(objReqParams, "StartFrom") + 
                 '&ObjectList_CountToFetch=' + API._getParamVal(objReqParams, "CountToFetch") +
                 '&ObjectList_SortBy=' + API._getParamVal(objReqParams, "SortBy") + 
                 '&ObjectList_SortOrder=' + API._getParamVal(objReqParams, "SortOrder") + 
                 '&ObjectList_SelectFormat=' + API._getParamVal(objReqParams, "SelectFormat") +
                 '&ObjectList_WhereNID=' + API._getParamVal(objReqParams, "WhereNID") +
                 '&ObjectList_WhereTextSubStr=' + API._getParamVal(objReqParams, "WhereTextSubStr") + 
                 '&ObjectList_WhereDateRange=' + API._getParamVal(objReqParams, "WhereDateRange") + 
                 '&ObjectList_WhereTCRange=' + API._getParamVal(objReqParams, "WhereTCRange") +  
                 '&ObjectList_WhereFileNID=' + API._getParamVal(objReqParams, "WhereFileNID") + 
                 '&ObjectList_WhereSessionInfo=' + API._getParamVal(objReqParams, "WhereSessionInfo") +                               
                 '&ObjectList_SubscriptionID=' + API._getParamVal(objReqParams, "SubscriptionID");
    return urlStr;
  }

//  /**
//  * GetStartSessionURL method will form the url for StartSession request.
//  * @param objReqParams {Object}
//  * @return urlStr (string).
//  */
//  API.GetStartSessionURL = function(objReqParams) {
//    var urlStr = 'ipvs:///Start?ServiceName=sessionservice' +
//                 '&RequestName=' + objReqParams.action + 'Request' +
//                 '&iq_type=set' +
//                 '&iq_from=' + API._getParamVal(objReqParams, "iqFrom") +
//                 '&iq_to=' + API._getParamVal(objReqParams, "iqTo") +
//                 '&session_id=' + API._getParamVal(objReqParams, "sessionId") +
//                 '&bookmark_id=' + API._getParamVal(objReqParams, "bookmarkId");
//    return urlStr;
//  }

//  /**
//  * StartSession method will send asynchronous request to StartSession.
//  * @param objReqParams {Object}
//  * @return objResponse (asyncRV).
//  */
//  API.ResumeFromBookmark = jsx3.$Y(function(cb) {
//    var urlStr = API.GetStartSessionURL(cb.args()[0]);
//    var ResumeFromBookmarkRequest = new com.ipvs.api.Request();
//    API._LOG.info("API:Sending ResumeFromBookmark request... ::RequestURL:" + urlStr);
//    ResumeFromBookmarkRequest.async_send(urlStr).when(function(objResponse) {
//      ResumeFromBookmarkRequest.endRequest();
//      cb.done(objResponse);
//    });
//  });
  API._getParamVal = function(obj, paramName) {
    return (jsx3.util.strEmpty(obj[paramName]) ? "" : escape(obj[paramName]));
    }

});

/*
  "VIEW":
  ObjectList_canvasId	""	String
	ObjectList_connId	"Conn_C4CB7CA7-8FC0-0001-89724D6D1E40A320"	String
	ObjectList_connName	"tx19"	String
	ObjectList_dstEntityId	""	String
	ObjectList_dstPortNID	"default.mediastreamdstportresourcelist.msdprl_51bf105d-47e2-4beb-a71c-74d8b0ba2e74"	String
	ObjectList_dstPortTitle	"sunpc3"	String
	ObjectList_restartSession	""	String
	ObjectList_sessionId	"Session_C4CB7C9F-BCD0-0001-C62686301000EDB0"	String
	ObjectList_srcEntityId	""	String
	ObjectList_srcPortNID	"default.mediastreamsrcportresourcelist.mssprl_b2d1bf67-d0a0-4b26-8576-971f25ac3326"	String
	ObjectList_srcPortTitle	"tx19"	String
	ObjectList_windowId	""	String      
*/     