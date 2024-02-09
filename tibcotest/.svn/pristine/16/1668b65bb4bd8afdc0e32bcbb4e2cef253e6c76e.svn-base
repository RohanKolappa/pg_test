
//dynamic requests

function sendAgentCommandRequest(AgentJID, AgentCommandDataNode) {

  var reqSendCommandAgentObject = new ReqSendCommandAgentObject(AgentJID, AgentCommandDataNode,
                                                                 { action:"FNCCALL", context:this,
                                                                   fnc:function(respCode, respVal) {
                                                                         var deviceCommandNode = AgentCommandDataNode.selectSingleNode(".//SystemElementsCommand/*");
                                                                         var deviceCommand = null;
                                                                         if( !jsx3.util.strEmpty(deviceCommandNode) ) {
                                                                           deviceCommand = deviceCommandNode.getNodeName();
                                                                         }
                                                                         if(respCode == "SUCCESS" && respVal != null) {
                                                                           if( deviceCommand == "SaveToFlashCommand" ){
                                                                             topic_Set(AgentJID, "DeviceSaveToFlashCommand", "SUCCESS");
                                                                           }
                                                                          
                                                                         } else {
                                                                           var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                           var ErrorMsg = "Error: Device->" + deviceCommand + " request failed \n";
                                                                           ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                           ErrorMsg += "Error code: " + respCode;
                                                                           alert(ErrorMsg);
                                                                         }
                                                                       }
                                                                 }
                                                               );
  
  reqSendCommandAgentObject.sendRequest(this);

}  

function sendAgentConfigRequest(AgentJID, AgentCofigDataNode) {

  var reqSendConfigAgentObject = new ReqSendConfiguarationAgentObject(AgentJID, AgentCofigDataNode,
                                                                       { action:"FNCCALL", context:this,
                                                                         fnc:function(respCode, respVal) {
                                                                               if(respCode == "SUCCESS" && respVal != null) {
                                                                                 var resVal = respVal;
                                                                                 var resCode = respCode;
                                                                               } else {
                                                                                 var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                                                 var ErrorMsg = "Error: Device->Resource get data request failed \n";
                                                                                 ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                                                 ErrorMsg += "Error code: " + respCode;
                                                                                 alert(ErrorMsg);
                                                                               }
                                                                             }
                                                                       }
                                                                     );
  
  reqSendConfigAgentObject.sendRequest(this);

}  

function cmpReq_RecycleBin(ActionRequest) {
  var reqRecycleBin = new ReqRecycleBin(ActionRequest,
                                         { action:"FNCCALL", context:this,
                                           fnc:function(respCode, respVal) {
                                                 if(respCode == "SUCCESS" && respVal != null) {
                                                   var resVal = respVal;
                                                   var resCode = respCode;
                                                 } else {
                                                   var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                   var ErrorMsg = "Error: Device->Resource get data request failed \n";
                                                   ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                   ErrorMsg += "Error code: " + respCode;
                                                   alert(ErrorMsg);
                                                 }
                                               }
                                         }
                                       );

  reqRecycleBin.sendRequest(this);

}

function cmpReq_GetSuperUser() {
  var reqGetUser = new ReqSessionGetRequest("GetUser",
                                            { action: "FNCCALL", context: null,
                                              fnc: function (respCode, respVal) {
                                                if (respCode == "SUCCESS" && respVal != null) {
                                                  var ObjectCount = respVal.getAttribute("total");
                                                  if (ObjectCount == "0") {
                                                    topic_Publish(tApp, "DatabaseState", "NONE");
                                                  }
                                                  else {
                                                    topic_Publish(tApp, "DatabaseState", "VALID");
                                                  }
                                                }
                                                else {
                                                  topic_Set(tInstance, "GetCompleteUserStatus", "ERROR");
                                                  var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                  var ErrorMsg = "Error: GetUser request failed \n";
                                                  ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                  ErrorMsg += "Error code: " + respCode;
                                                  alert(ErrorMsg);
                                                  return false;
                                                }
                                              }
                                            }
                                            );
  reqGetUser.ServiceName = "SessionService";
  var reqParams = new Object;
  reqParams["ObjectList_WhereUsername"] = "superuser-default";
  reqParams["ObjectList_SelectFormat"] = "BASIC";
  reqGetUser.reqParams = reqParams;
  // send the request
  reqGetUser.sendRequest(this);

}

// function to get Complete Device set(Asset, Ports, Device)
function cmpReq_GetCompleteDeviceWithReadQuery(tInstance, assetNID){
  topic_Set(tInstance, "AssetNID", assetNID);
  if(assetNID == "DEFAULT_ASSETRESOURCE_NID") {
    topic_Set(tInstance, "GetCompleteDeviceStatus", "SUCCESS");
    topic_Publish(tInstance, "GetCompleteDeviceDone", assetNID);  
  }
  else{
    var reqAssetData = new cmpReq_GetListReadQuery(tInstance, "JSXAPPS/IPVS_AdminUI/query/GetCompleteAsset.xml",
                                                    [{VarName:"_ASSETNID_", VarRef:"AssetNID"}
                                                    ],
                                                    {action: "FNCCALL", context: null,
                                                      fnc: function(respCode, respVal) {
                                                          if (respCode == "SUCCESS" && respVal != null) {
                                                            var recordNode = respVal.selectSingleNode(".//record");
                                                            if(!jsx3.util.strEmpty(recordNode)){
                                                              setDialogDataNode(tInstance, recordNode);
                                                            }
                                                            topic_Set(tInstance, "GetCompleteDeviceStatus", "SUCCESS");
                                                          }
                                                          else{
                                                            topic_Set(tInstance, "GetCompleteDeviceStatus", "ERROR");
                                                            var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                            var ErrorMsg = "Error: Device->Asset get data request failed \n";
                                                            ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                            ErrorMsg += "Error code: " + respCode;
                                                            alert(ErrorMsg);
                                                            return false;
                                                          }
                                                          topic_Publish(tInstance, "GetCompleteDeviceDone", assetNID);  
                                                        }
                                                    }, "ItemByNID" 
                                                   );
      var reqParams = new Object;
      reqParams["Op_Select_ListSpec_SortOrder"] = "ascending";
      reqParams["ObjectCount"] = "";
      reqParams["Op_Select_ListSpec_StartFrom"] = "1";
      reqParams["Op_Select_ListSpec_CountToFetch"] = "5000";
      reqParams["Op_Select_ListSpec_SortByXPathList_SortByXpath"] = "";
      reqAssetData.reqParams = reqParams;                              
      reqAssetData.sendRequest();
  }
}

  // fnc to get dlg port resource data
  function getDialogPortResourceDataNode(tInstance, portResourceID){
    var resourceDataNode = "";
    if(!jsx3.util.strEmpty(topic_GetNIDNode(tInstance, portResourceID))){
      resourceDataNode = topic_GetNIDNode(tInstance, portResourceID).cloneNode(true);
    }
    return resourceDataNode;
  }

  function cmpReq_GetCompleteProfileWithReadQuery(tInstance, profileNID){
    topic_Set(tInstance, "ProfileNID", profileNID);
    if( profileNID == "DEFAULT_V2D-STREAMPROFILE_NID" || profileNID == "DEFAULT_MPEGTS-STREAMPROFILE_NID" || profileNID == "DEFAULT_RTP-STREAMPROFILE_NID" || profileNID == "DEFAULT_UDP-STREAMPROFILE_NID"){
      topic_Set(tInstance, "GetCompleteProfileStatus", "SUCCESS");
      topic_Publish(tInstance, "GetCompleteProfileDone", profileNID);  
    }
    else{
      var reqProfileData = new cmpReq_GetListReadQuery(tInstance, "JSXAPPS/IPVS_AdminUI/query/GetCompleteProfile.xml",
                                                      [{VarName:"_PROFILENID_", VarRef:"ProfileNID"}
                                                      ],
                                                      {action: "FNCCALL", context: null,
                                                        fnc: function(respCode, respVal) {
                                                            if (respCode == "SUCCESS" && respVal != null) {
                                                              var recordNode = respVal.selectSingleNode(".//record");
                                                              if(!jsx3.util.strEmpty(recordNode)){
                                                                setDialogDataNode(tInstance, recordNode);
                                                              }
                                                              topic_Set(tInstance, "GetCompleteProfileStatus", "SUCCESS");
                                                            }
                                                            else{
                                                              topic_Set(tInstance, "GetCompleteProfileStatus", "ERROR");
                                                              var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                              var ErrorMsg = "Error: Profile get data request failed \n";
                                                              ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                              ErrorMsg += "Error code: " + respCode;
                                                              alert(ErrorMsg);
                                                              return false;
                                                            }
                                                            topic_Publish(tInstance, "GetCompleteProfileDone", profileNID);  
                                                          }
                                                      }, "ItemByNID" 
                                                     );
        var reqParams = new Object;
        reqParams["Op_Select_ListSpec_SortOrder"] = "ascending";
        reqParams["ObjectCount"] = "";
        reqParams["Op_Select_ListSpec_StartFrom"] = "1";
        reqParams["Op_Select_ListSpec_CountToFetch"] = "5000";
        reqParams["Op_Select_ListSpec_SortByXPathList_SortByXpath"] = "";
        reqProfileData.reqParams = reqParams;                              
        reqProfileData.sendRequest();
    }
  }

  // fnc to get dlg stream profile data
  function getDialogStreamProfileDataNode(tInstance, profileNID){
    var profileDataNode = topic_GetNIDNode(tInstance, profileNID).cloneNode(true);
    return profileDataNode;
  }


  // function to get User by GetUser API Call
  function cmpReq_GetUser(tInstance, userNID, format) {
    if (userNID == "DEFAULT_USER_NID") {
      topic_Set(tInstance, "GetCompleteUserStatus", "SUCCESS");
      topic_Publish(tInstance, "GetCompleteUserDone", userNID);
    }
    else {
      var reqGetUser = new ReqSessionGetRequest("GetUser",
                                                  { action: "FNCCALL", context: null,
                                                    fnc: function (respCode, respVal) {
                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                        var userNode = respVal.selectSingleNode(".//User");
                                                        setDialogDataNode(tInstance, userNode);
                                                        topic_Set(tInstance, "GetCompleteUserStatus", "SUCCESS");
                                                      }
                                                      else {
                                                        topic_Set(tInstance, "GetCompleteUserStatus", "ERROR");
                                                        var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                        var ErrorMsg = "Error: GetUser request failed \n";
                                                        ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                        ErrorMsg += "Error code: " + respCode;
                                                        alert(ErrorMsg);
                                                        return false;
                                                      }
                                                      topic_Publish(tInstance, "GetCompleteUserDone", userNID);
                                                    }
                                                  }
                                                 );
      reqGetUser.ServiceName = "SessionService";
      var reqParams = new Object;
      reqParams["ObjectList_WhereNID"] = userNID;
      reqParams["ObjectList_SelectFormat"] = format;
      reqGetUser.reqParams = reqParams;
      // send the request
      reqGetUser.sendRequest(this);
    }
  }

  function cmpReq_GetDir(tInstance, mediaDirNID) {
    // helper function to get the dir with NID old way
    function getDirOldWay(DirNID) {
      var reqMediaDirData = new ReqGetObjectNID(DirNID, "MediaStreamDirResource",
                                                  { action: "FNCCALL", context: this,
                                                    fnc: function (respCode, respVal) {
                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                      } else {
                                                      }
                                                    }
                                                  }
                                                );
      reqMediaDirData.sendRequest();
    }

    topic_Set(tInstance, "MediaDirNID", mediaDirNID);
    if( mediaDirNID == "DEFAULT_MEDIASTREAMDIR_NID"){
      topic_Set(tInstance, "GetCompleteMediaDirStatus", "SUCCESS");
      topic_Publish(tInstance, "GetCompleteMediaDirDone", mediaDirNID);  
    }
    else{
      var reqGetDir = new ReqSessionGetRequest("GetDir",
                                                  { action: "FNCCALL", context: null,
                                                    fnc: function (respCode, respVal) {
                                                      if (respCode == "SUCCESS" && respVal != null) {
                                                        getDirOldWay(mediaDirNID);
                                                        var dirNode = respVal.selectSingleNode(".//Dir");
                                                        setDialogDataNode(tInstance, dirNode);
                                                        topic_Set(tInstance, "GetCompleteMediaDirStatus", "SUCCESS");
                                                      }
                                                      else {
                                                        topic_Set(tInstance, "GetCompleteMediaDirStatus", "ERROR");
                                                        var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                        var ErrorMsg = "Error: GetDir request failed \n";
                                                        ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                        ErrorMsg += "Error code: " + respCode;
                                                        alert(ErrorMsg);
                                                        return false;
                                                      }
                                                      topic_Publish(tInstance, "GetCompleteMediaDirDone", mediaDirNID);
                                                    }
                                                  }
                                                 );
        reqGetDir.ServiceName = "SessionService";
        var reqParams = new Object;
        reqParams["ObjectList_WhereNID"] = mediaDirNID;
        reqParams["ObjectList_SelectFormat"] = "EXTENDED";
        reqGetDir.reqParams = reqParams;
        // send the request
        reqGetDir.sendRequest(this);
    }
  }
 
// function to get Device by GetDevice API Call
function cmpReq_GetDevice(tInstance, assetNID, format){
  if(assetNID == "DEFAULT_ASSETRESOURCE_NID") {
    topic_Set(tInstance, "GetCompleteDeviceStatus", "SUCCESS");
    topic_Publish(tInstance, "GetCompleteDeviceDone", assetNID);  
  }
  else{      
    var reqGetDevice = new ReqSessionGetRequest("GetDevice",
                                                { action: "FNCCALL", context: null,
                                                  fnc: function(respCode, respVal) {
                                                    if (respCode == "SUCCESS" && respVal != null) { 
                                                      var deviceNode = respVal.selectSingleNode(".//Device");
                                                      setDialogDataNode(tInstance, deviceNode);
                                                      topic_Set(tInstance, "GetCompleteDeviceStatus", "SUCCESS");
                                                    }
                                                    else {
                                                      topic_Set(tInstance, "GetCompleteDeviceStatus", "ERROR");
                                                      var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                      var ErrorMsg = "Error: Device->Asset get data request failed \n";
                                                      ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                      ErrorMsg += "Error code: " + respCode;
                                                      alert(ErrorMsg);
                                                      return false;
                                                    }
                                                    topic_Publish(tInstance, "GetCompleteDeviceDone", assetNID);
                                                  }
                                                }
                                               );
    reqGetDevice.ServiceName = "SessionService";
    var reqParams = new Object;
    reqParams["ObjectList_WhereNID"] = assetNID;
    reqParams["ObjectList_SelectFormat"] = format;
    reqGetDevice.reqParams = reqParams;
    // send the request
    reqGetDevice.sendRequest(this); 
  }
  
 
                                            
} // end cmpReq_GetDevice

// function to get Device Config by GetDeviceConfig API Call
function cmpReq_GetDeviceConfig(tInstance, assetNID){
  var reqGetDevice = new ReqSessionGetRequest("GetDeviceConfig",
                                              { action: "FNCCALL", context: null,
                                                fnc: function(respCode, respVal) {
                                                  if (respCode == "SUCCESS" && respVal != null) { 
                                                    var deviceNode = respVal.selectSingleNode(".//Device");
                                                    setDialogDataNode(tInstance, deviceNode);
                                                    topic_Set(tInstance, "GetCompleteDeviceStatus", "SUCCESS");
                                                  }
                                                  else {
                                                    topic_Set(tInstance, "GetCompleteDeviceStatus", "ERROR");
                                                    var codeErrorMsg = CommonErrorCodeArr[respCode].errorDesc;
                                                    var ErrorMsg = "Error: Device->Asset get data request failed \n";
                                                    ErrorMsg += "Error description: " + codeErrorMsg + "\n";
                                                    ErrorMsg += "Error code: " + respCode;
                                                    alert(ErrorMsg);
                                                    return false;
                                                  }
                                                  topic_Publish(tInstance, "GetCompleteDeviceDone", assetNID);
                                                }
                                              }
                                             );
  reqGetDevice.ServiceName = "SessionService";
  var reqParams = new Object;
  reqParams["ObjectList_ByNID"] = assetNID;
  reqParams["ObjectList_ByTitle"] = "";
  reqParams["ObjectList_SubscriptionID"] = "";
  reqGetDevice.reqParams = reqParams;
  // send the request
  reqGetDevice.sendRequest(this); 
} // end cmpReq_GetDeviceConfig

  // function to get port by GetPort API Call
  function cmpReq_GetPort(tInstance, portNID, deviceUserJID){
    var urlStr = 'ipvs:///getPort?ServiceName=sessionservice' +
               '&OpType=Operation' +
               '&ObjectList_WhereNID='+portNID +
               '&ObjectList_SelectFormat=EXTENDED'+
               '&RequestName=GetPortRequest'+
               '&iq_type=set ' +
               '&iq_from='+deviceUserJID +
               '&iq_to='+topic_Get(tApp,"AppServerAgentJID");
               '&ObjectType=GetPort';

    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(urlStr);
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        var portNode = req.getResponseXML().selectSingleNode(".//Port");
        setDialogDataNode(tInstance, portNode);
        topic_Set(tInstance, "GetCompletePortStatus", "SUCCESS");
      }
      else {
        topic_Set(tInstance, "GetCompletePortStatus", "ERROR");
        var codeErrorMsg = CommonErrorCodeArr[respStatus].errorDesc;
        var ErrorMsg = "Error: Get Port data request failed \n";
        ErrorMsg += "Error description: " + codeErrorMsg + "\n";
        ErrorMsg += "Error code: " + respStatus;
        alert(ErrorMsg);
        return false;
      }
      topic_Publish(tInstance, "GetCompletePortDone", portNID);
      delete serviceAppletReq;
    }

    req.subscribe("*", this, onRequestEvent);
    req.send();
  }// end cmpReq_GetPort
