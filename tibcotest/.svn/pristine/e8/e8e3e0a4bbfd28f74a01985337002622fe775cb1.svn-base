var cmpReqObjectArr = null;

// setup static requests and their params
function cmpReq_Create() {
  cmpReqObjectArr = new Object;
  
//  // login
//  arrServiceObjectInfo["Login"].reqservice = "xmpp";
//  arrServiceObjectInfo["Login"].req = new ReqLogin();
//  
//  // logout
//  arrServiceObjectInfo["Logout"].reqservice = "xmpp";
//  arrServiceObjectInfo["Logout"].req = new ReqLogout(); 
//  
//  // get server time
//  arrServiceObjectInfo["ServerTime"].reqservice = "systemservice";
//  arrServiceObjectInfo["ServerTime"].req = new ReqGetServerTime();
//
  // set service domain
  // done from cmpBlkLogin
//  arrServiceObjectInfo["ServiceDomain"].reqservice = "xmpp";
//  arrServiceObjectInfo["ServiceDomain"].req = new ReqSetServiceDomain();
  
  //------------- Media List -------------
  // Live tree -- source port list data req 
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI" ) {
    arrServiceObjectInfo["MediaStreamSrcPortResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamSrcPortResource"].req = new ReqGetObjectList("MediaStreamSrcPortResource");
  }
  else {
    arrServiceObjectInfo["MediaStreamSrcPortResource"].reqservice = "mediaroomservice";
    arrServiceObjectInfo["MediaStreamSrcPortResource"].req = new ReqGetObjectList("Source");
    var reqParams_live = new Object;
    reqParams_live["DestNID"] = topic_Get(tApp, "DestNID");
    arrServiceObjectInfo["MediaStreamSrcPortResource"].req.reqParams = reqParams_live;
  }
  // destination port resource
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI" ) {
    arrServiceObjectInfo["MediaStreamDstPortResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamDstPortResource"].req = new ReqGetObjectList("MediaStreamDstPortResource");
  }
  else if(PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI") {
    arrServiceObjectInfo["MediaStreamDstPortResource"].reqservice = "mediaroomservice";
    arrServiceObjectInfo["MediaStreamDstPortResource"].req = new ReqGetObjectList("Destination");    
  }
    
  // Recorded tree -- directory list
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI" ) {
    arrServiceObjectInfo["MediaStreamDirResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamDirResource"].req = new ReqGetObjectList("MediaStreamDirResource"); 
  }
  else {
    arrServiceObjectInfo["MediaStreamDirResource"].reqservice = "mediaroomservice";
    arrServiceObjectInfo["MediaStreamDirResource"].req = new ReqGetObjectList("SrcMediaDir");
    var reqParams_record = new Object;
    reqParams_record["DestNID"] = topic_Get(tApp, "DestNID");
    reqParams_record["Title"] = "";
    arrServiceObjectInfo["MediaStreamDirResource"].req.reqParams = reqParams_record;
  }
  // Recorded tree -- file list
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI" ) {
    arrServiceObjectInfo["MediaStreamFileResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamFileResource"].listreqitembyNIDfnc = cmpReq_GetMediaFileReq; 
    }
  else {
    arrServiceObjectInfo["MediaStreamFileResource"].reqservice = "mediaroomservice";
    arrServiceObjectInfo["MediaStreamFileResource"].listreqitembyNIDfnc = cmpReq_GenerateGetSrcMediaFileReq;
  }
  
  // IO ports
  if( (PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI") ) {
    arrServiceObjectInfo["MediaStreamIOPortResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamIOPortResource"].req = new ReqGetObjectList("MediaStreamIOPortResource");
  }
  
  //Realy ports
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {    
    arrServiceObjectInfo["MediaStreamRelayPortResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamRelayPortResource"].req = new ReqGetObjectList("MediaStreamRelayPortResource");
  }
  
  //Media Store ports
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["MediaStorePortResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStorePortResource"].req = new ReqGetObjectList("MediaStorePortResource");
  }
  
  //Group Tag
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["GroupTagDef"].reqservice = "policyadminservice";
    arrServiceObjectInfo["GroupTagDef"].req = new ReqGetObjectList("GroupTagDef");
  }
  
  //Filter  --TagObject
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["TagObjectFilter"].reqservice = "policyadminservice";
    arrServiceObjectInfo["TagObjectFilter"].req = new ReqGetObjectList("TagObjectFilter");
  }

  //Filter  --Object  
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["ObjectFilter"].reqservice = "policyadminservice";
    arrServiceObjectInfo["ObjectFilter"].req = new ReqGetObjectList("ObjectFilter"); 
  }

  //Filter  --MediaStream
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["MediaStreamFilter"].reqservice = "policyadminservice";
    arrServiceObjectInfo["MediaStreamFilter"].req = new ReqGetObjectList("MediaStreamFilter");
  }
  
  //PolicyRule --UserAdmin
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["UserAdminPolicyRule"].reqservice = "policyadminservice";
    arrServiceObjectInfo["UserAdminPolicyRule"].req = new ReqGetObjectList("UserAdminPolicyRule");
  }

  //PolicyRule --AssetAdmin  
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["AssetAdminPolicyRule"].reqservice = "policyadminservice";
    arrServiceObjectInfo["AssetAdminPolicyRule"].req = new ReqGetObjectList("AssetAdminPolicyRule");
  }

  //PolicyRule --MediaStoreAdmin  
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["MediaStoreAdminPolicyRule"].reqservice = "policyadminservice";
    arrServiceObjectInfo["MediaStoreAdminPolicyRule"].req = new ReqGetObjectList("MediaStoreAdminPolicyRule");
  }

  //PolicyRule --PolicyAdmin  
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["PolicyAdminPolicyRule"].reqservice = "policyadminservice";
    arrServiceObjectInfo["PolicyAdminPolicyRule"].req = new ReqGetObjectList("PolicyAdminPolicyRule"); 
  }

  //PolicyRule --MediaRoom
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["MediaRoomPolicyRule"].reqservice = "policyadminservice";
    arrServiceObjectInfo["MediaRoomPolicyRule"].req = new ReqGetObjectList("MediaRoomPolicyRule"); 
  }

  //NetworkAddressRule
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["NetworkAddressRule"].reqservice = "policyadminservice";
    arrServiceObjectInfo["NetworkAddressRule"].req = new ReqGetObjectList("NetworkAddressRule");
  }
    
  // Profile
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["MediaStreamProfile"].reqservice = "assetadminservice";
    arrServiceObjectInfo["MediaStreamProfile"].req = new ReqGetObjectList("MediaStreamProfile");
  }

  // Asset Resource
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["AssetResource"].reqservice = "assetadminservice";
    arrServiceObjectInfo["AssetResource"].req = new ReqGetObjectList("AssetResource");
  }
  
  // Device
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
     arrServiceObjectInfo["Device"].reqservice = "assetadminservice";
    //arrServiceObjectInfo["Device"].req = new ReqGetObjectList("Device");
  }
  
  // User
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["User"].reqservice = "assetadminservice";
    arrServiceObjectInfo["User"].req = new ReqGetObjectList("User");
  }

  // DisplayProfile
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["DisplayProfile"].reqservice = "assetadminservice";
    arrServiceObjectInfo["DisplayProfile"].req = new ReqGetObjectList("DisplayProfile");
  }

  // Session Objects -- mediarooms
  if((PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI") || (PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI")) {
    arrServiceObjectInfo["MediaRoom"].reqservice = "mediaroomservice";
    arrServiceObjectInfo["MediaRoom"].req = new ReqGetObjectList("MediaRoom");
  }
  
  // Session Objects -- collab config
  if(PNameSpace.getEnv("NAMESPACE") == "IPVSClientUI") {
    arrServiceObjectInfo["SessionConfig"].reqservice = "assetadminservice";
    arrServiceObjectInfo["SessionConfig"].req = new ReqGetObjectList("SessionConfig");
  }

  // -- bookmarks
  arrServiceObjectInfo["Bookmark"].reqservice = "mediaroomservice";
  arrServiceObjectInfo["Bookmark"].listreqitembyNIDfnc = cmpReq_GenerateGetBookmarkReq;

}

// destroy static requests
function cmpReq_Destroy() {
  cmpReqObjectArr = null;
}

// dynamic request functions
// will be called with the appropriate "Instance" doc reference

//this function will be called to get a list of media files for a media dir incrementally
function cmpReq_GetSrcMediaFileList(ParentMediaDirNID) {
  // get a handle on the object info
  var objCacheInfo = arrServiceObjectInfo["SrcMediaFile"];
  if( objCacheInfo.listreqarr == null ) {
    return false;
  }
  var objListReqInfo = objCacheInfo.listreqarr[ParentMediaDirNID];
  if( objListReqInfo == null ) {
    return false;
  }
  // setup request range -  based on what is in cache right now
  var TotalDBCount = objListReqInfo.nidlisttotaldbcount;
  var CountToFetch = objListReqInfo.nidlistdefaultfetchcount;
  var StartFrom = objListReqInfo.nidlistnextfrom;
  if( StartFrom == null ) {
     StartFrom = 0;
  }
  // if next start gt total db count
  if( TotalDBCount != null && StartFrom >= TotalDBCount ) {
    // fire final event here and retun
    topic_Publish(tApp, "MediaSFRDataState", newServerDate());
    return true;
  }
  // create a new list request
  var reqGetSrcMediaFileList = new ReqGetObjectListRangeWhere(objListReqInfo,
                                  { action: "FNCCALL", context: this,
                                    fnc: function(respCode, respVal) {
                                      // get object count info from response and set them in objInfo
                                      var ObjectCount = respVal.getAttribute("ObjectCount");
                                      var ListStartFrom = respVal.getAttribute("ListStartFrom");
                                      var ListCount = respVal.getChildNodes().size();
                                      updateReqObjPaginationParam(objListReqInfo, ObjectCount, ListStartFrom, ListCount);
                                      // call recursively to get the rest of the list iteratively
                                      cmpReq_GetSrcMediaFileList(ParentMediaDirNID);
                                      // fire an event to signal partial data state change --- tbd fix obj
                                    }
                                  }); 

  reqGetSrcMediaFileList.ObjectType = "SrcMediaFile";
  reqGetSrcMediaFileList.ServiceName = "mediaroomservice";
  //in case of error we need to resend the same request
  //reqGetSrcMediaFileList.RetryTimeout = 10000; //10 sec
  var reqParams = new Object;
  reqParams["DestNID"] = topic_Get(tApp, "DestNID");
  reqParams["Op_Where_WhereXPath"] = "Info/Properties/ParentMediaDirNID[.='" + ParentMediaDirNID + "']"; 
  reqParams["Op_Select_ListSpec_StartFrom"] = StartFrom;
  reqParams["Op_Select_ListSpec_CountToFetch"] = CountToFetch;
  reqGetSrcMediaFileList.reqParams = reqParams;

  reqGetSrcMediaFileList.sendRequest(this);
  return true;
}

//function to get the MediaFiles using session API in Pagination
cmpReq_GetSessionAPIPaginationList.prototype = new ReqSessionGetRequest();
function cmpReq_GetSessionAPIPaginationList(InstanceID, ObjectType, CacheListArr, ReqEvent, ItemByNID) {
  // setup inheritence and call base constructor
  cmpReq_GetSessionAPIPaginationList.BaseClass = cmpReq_GetSessionAPIPaginationList.prototype;
  cmpReq_GetSessionAPIPaginationList.prototype.constructor = cmpReq_GetSessionAPIPaginationList;
  ReqSessionGetRequest.prototype.constructor.call(this, ObjectType, ReqEvent);

  this.InstancID = InstanceID;
  this.CacheListArr = CacheListArr;
  this.reqParams = null;
  this.ObjectType = ObjectType;
  this.ItemByNID = ItemByNID;

  this.sendRequest = function () {
    var InstanceID = this.InstancID;
    var ObjectCount = parseInt(this.reqParams["ObjectCount"]);
    var objStartFrom = this.reqParams["ObjectList_StartFrom"];
    if (jsx3.util.strEmpty(objStartFrom)) {
      objStartFrom = this.reqParams["Op_Select_ListSpec_StartFrom"];
    }
    var StartFrom = parseInt(objStartFrom);
    var objCountToFetch = this.reqParams["ObjectList_CountToFetch"];
    if (jsx3.util.strEmpty(objCountToFetch)) {
      objCountToFetch = this.reqParams["Op_Select_ListSpec_CountToFetch"];
      this.reqParams["ObjectList_CountToFetch"] = this.reqParams["Op_Select_ListSpec_CountToFetch"];
    }
    var CountToFetch = parseInt(objCountToFetch);
    // UI is sending wrong StartFrom when auto refresh. 
    if (StartFrom > ObjectCount) {
      StartFrom = ObjectCount - CountToFetch + 1;
      if (StartFrom < 1) {
        StartFrom = 1;
      }
      this.reqParams["ObjectList_StartFrom"] = StartFrom;
    }

    if (this.reqParams["ObjectList_SortOrder"] == "") {
    }
    else if (jsx3.util.strEmpty(this.reqParams["ObjectList_SortOrder"])) {
      this.reqParams["ObjectList_SortOrder"] = this.reqParams["Op_Select_ListSpec_SortOrder"];
    }

    if (this.reqParams["ObjectList_SortBy"] == "") {
    }
    else if (jsx3.util.strEmpty(this.reqParams["ObjectList_SortBy"])) {
      this.reqParams["ObjectList_SortBy"] = this.reqParams["Op_Select_ListSpec_SortByXPathList_SortByXpath"];
    }

    for (var VarNameRef in this.CacheListArr) {
      if (this.CacheListArr[VarNameRef].VarRef != null) {
        var varName = this.CacheListArr[VarNameRef].VarName;
        var varRef = this.CacheListArr[VarNameRef].VarRef;
        var varValue = topic_Get(InstanceID, varRef);
        this.reqParams[varName] = varValue;
      }
    }

    cmpReq_GetSessionAPIPaginationList.BaseClass.sendRequest.call(this);
  }

  this.onSuccess = function(req, ErrCode, response) {
    if(ErrCode == "SUCCESS" && response != null) {
      //check request is for list or for one item
     if( jsx3.util.strEmpty(this.ItemByNID) ) {
        //set the variable values
        this.reqParams["ObjectCount"] = response.getAttribute("total");
        this.reqParams["ObjectList_StartFrom"] = response.getAttribute("from");
     }

      cmpReq_GetSessionAPIPaginationList.BaseClass.onSuccess.call(this, req, ErrCode, response);
    }

  }
} // end 

//function to get the procedures Pagination
cmpReq_GetPaginationList.prototype = new ReqGetPaginationList();
function cmpReq_GetPaginationList(InstanceID, ReqName, WhereXPath, SelectXML, CacheListArr, ReqEvent) {
// setup inheritence and call base constructor
  cmpReq_GetPaginationList.BaseClass = cmpReq_GetPaginationList.prototype;
  cmpReq_GetPaginationList.prototype.constructor = cmpReq_GetPaginationList;
  ReqGetPaginationList.prototype.constructor.call(this, ReqEvent);
  
  this.InstancID = InstanceID;
  this.SelectXML = escape(SelectXML);
  this.WhereXPath = WhereXPath;
  this.CacheListArr = CacheListArr;  
  this.reqParams = null;  
  this.ObjectType = null;
  this.ServiceName = "mediaroomservice";
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
    this.reqParams["DestNID"] = topic_Get(tApp, "DestNID");
    //HACK:mediaroom service handlers are 0 based to fetch list
    //Middleware should fix to 1 based
    this.reqParams["Op_Select_ListSpec_StartFrom"] = this.reqParams["Op_Select_ListSpec_StartFrom"] - 1;
    this.reqParams["Op_Where_WhereXPath"] =  whereXPath;
    this.reqParams["Op_Select_ItemSpec_SelectXML"] =  this.SelectXML;
    
    cmpReq_GetPaginationList.BaseClass.sendRequest.call(this);  
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

     cmpReq_GetPaginationList.BaseClass.onSuccess.call(this, req, ErrCode, responseDoc);
   }
   
  }
} // end cmpReq_GetPaginationList

//this function will be called to get a request that can get one media file
// the mode parameter decides what type of event to fire (ie: why we are getting this node)
function cmpReq_GenerateGetSrcMediaFileReq(MediaFileNID, Mode) {
  // if delete - then just fire a delete event
  if( Mode == "DELETE" ) {
    topic_Publish(tApp, "MediaSFRDataDeleteNode", MediaFileNID);
    return null;
  }
  // if add/update send request to get new node
  // get a handle on the object info
  var objCacheInfo = arrServiceObjectInfo["SrcMediaFile"];
  if (objCacheInfo.listreqarr == null) {
    return false;
  }
  var objListReqInfo = objCacheInfo.listreqarr["NID"];
  if (objListReqInfo == null) {
    return false;
  }
  var CountToFetch = 1;
  var StartFrom = 0;
  // create a new request
  var reqGetSrcMediaFile = new ReqGetObjectListRangeWhere(objListReqInfo,
                                  { action: "FNCCALL", context: this,
                                    fnc: function(respCode, respVal) {
                                      if (Mode == "ADD") {
                                        topic_Publish(tApp, "MediaSFRDataAddNode", MediaFileNID);
                                      }
                                      else if (Mode == "UPDATE") {
                                        topic_Publish(tApp, "MediaSFRDataUpdateNode", MediaFileNID);
                                      }
                                      else if( Mode == "PLAY" ) {
                                        topic_Publish(tApp, "MediaSFRPlayNode", MediaFileNID);
                                      }
                                    }
                                  });
  reqGetSrcMediaFile.ObjectType = "SrcMediaFile";
  reqGetSrcMediaFile.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["DestNID"] = topic_Get(tApp, "DestNID");
  reqParams["Op_Where_WhereXPath"] = "(//MediaStreamFileResource[@NID=('" + MediaFileNID + "')])";
  reqParams["Op_Select_ListSpec_StartFrom"] = StartFrom;
  reqParams["Op_Select_ListSpec_CountToFetch"] = CountToFetch;
  reqGetSrcMediaFile.reqParams = reqParams;

  //reqGetSrcMediaFile.sendRequest(this);
  return reqGetSrcMediaFile;
}

function cmpReq_GetStreamProfile(tInstance) {
  var reqGetStreamProfileList = new ReqGetObjectList("StreamProfile", 
                                      {action:"FNCCALL", context:this, 
                                       fnc:function(respCode, respVal){
                                            // start by clearing out the profile related data
                                            topic_Set(tInstance, "ProfileNID", "");
                                            var profileXMLNode = topic_GetNode(tInstance, "//ProfileXML");
                                            if( profileXMLNode != null ) {
                                              profileXMLNode.removeChildren();
                                            }
                                            // now check for success and proceed
                                            if( respCode == "SUCCESS" && respVal != null) {
                                              var StreamProfileSelectedNID = "";
                                              var StreamProfileSelectedPriority = 0;
                                              var streamProfileNIDListIter = respVal.selectNodeIterator(".//ObjectNID");
                                              while( streamProfileNIDListIter.hasNext() ) {
                                                var ProfileNID = streamProfileNIDListIter.next().getValue();
                                                var profileNode = topic_GetNIDNode(tCache, ProfileNID);
                                                var ProfilePriority = profileNode.selectSingleNode("//Priority").getValue();
                                                if( ProfilePriority >= StreamProfileSelectedPriority ) {
                                                  StreamProfileSelectedPriority = ProfilePriority;
                                                  StreamProfileSelectedNID = ProfileNID;
                                                }
                                              }
                                              if( StreamProfileSelectedNID != "" ) {
                                                topic_Set(tInstance, "ProfileNID", StreamProfileSelectedNID);
                                                var profileNode = topic_GetNIDNode(tCache, StreamProfileSelectedNID);
                                                if( profileNode != null ) {
                                                  var profileXMLNode = topic_GetNode(tInstance, "//ProfileXML");
                                                  if( profileXMLNode == null ) {
                                                    profileXMLNode = topic_Set(tInstance, "ProfileXML", null);
                                                  }
                                                  else {
                                                    profileXMLNode.removeChildren();
                                                  }
                                                  profileXMLNode.appendChild(profileNode.cloneNode(true));
                                                }
                                              }
                                            }
                                            else {
                                              //publishing Error code to the connection state msg tooltip
                                              cmpReq_HandleErrorCode(tInstance, respCode);
                                            }
                                            // finally check if we ended up with a valid profile nid or not
                                            var ProfileNID = topic_Get(tInstance, "ProfileNID");
                                            if( ProfileNID !="" ) {
                                              topic_Publish(tInstance, "ConnectionReqState", "GETSTREAMPROFILE_SUCCESS");
                                            }
                                            else {
                                              topic_Publish(tInstance, "ConnectionReqState", "GETSTREAMPROFILE_FAIL");
                                            }
                                            return true;
                                          }
                                      });
  reqGetStreamProfileList.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["SourceNID"] = topic_Get(tInstance, "SourceNID");
  reqParams["DestNID"] = topic_Get(tInstance, "DestNID");
  reqGetStreamProfileList.reqParams = reqParams;
  reqGetStreamProfileList.sendRequest(this);
  return true;
}
//--------------------
function cmpReq_GetRelayProfile(tInstance) {
  var reqGetRelayList = new ReqGetObjectList("Relay", 
                              {action:"FNCCALL", context:this, 
                               fnc:function(respCode, respVal){
                                    // check that this is a response to the current request
                                    var ConnectionReqTopicName = topic_Get(tInstance, "ConnectionReqTopicName");
                                    if( reqGetRelayList.req.TopicName != ConnectionReqTopicName ) {
                                      return true;
                                    }
                                    // clear out relayNID and profileNID/profileXML to start
                                    topic_Set(tInstance, "RelayNID", "");
                                    topic_Set(tInstance, "ProfileNID", "");
                                    var profileXMLNode = topic_GetNode(tInstance, "//ProfileXML");
                                    if( profileXMLNode != null ) {
                                      profileXMLNode.removeChildren();
                                    }
                                    // now look at the response and get new values
                                    if( respCode == "SUCCESS" && respVal != null) {
                                      // get the relay node and nid from the first relayprofile
                                      var relayProfileNode = respVal.selectSingleNode(".//RelayProfile[1]");
                                      if( relayProfileNode != null ) {
                                        // get the relay node and nid from this relayprofile
                                        var relayNode = relayProfileNode.selectSingleNode(".//Relay/*");
                                        if( relayNode != null ) {
                                          setObjectCacheNode(relayNode.cloneNode(true));
                                          var RelayNID = relayNode.getAttribute("NID");
                                          topic_Set(tInstance, "RelayNID", RelayNID);
                                        }
                                        // get the profile node and nid from the same relayprofile
                                        var profileNode = relayProfileNode.selectSingleNode(".//Profile/*");
                                        if( profileNode != null ) {
                                          setObjectCacheNode(profileNode.cloneNode(true));
                                          var ProfileNID = profileNode.getAttribute("NID");
                                          topic_Set(tInstance, "ProfileNID", ProfileNID);
                                          // stash profile xml as well 
                                          var profileXMLNode = topic_GetNode(tInstance, "//ProfileXML");
                                          if( profileXMLNode == null ) {
                                            profileXMLNode = topic_Set(tInstance, "ProfileXML", null);
                                          }
                                          else {
                                            profileXMLNode.removeChildren();
                                          }
                                          profileXMLNode.appendChild(profileNode.cloneNode(true));
                                        }
                                      }
                                    }
                                    else {
                                      //publishing Error code to the connection state msg tooltip
                                      cmpReq_HandleErrorCode(tInstance, respCode);
                                    }
                                    // finally check if we ended up with at least a valid profile nid or not
                                    //var ProfileNID = topic_Get(tInstance, "ProfileNID");
                                    //we are not checking for NID becuase logic has changed. If the device is configured with the 
                                    // profile then it wont have the NID .
                                    if( profileNode != null ) {
                                      topic_Publish(tInstance, "ConnectionReqState", "GETRELAYPROFILE_SUCCESS");
                                    }
                                    else {
                                      topic_Publish(tInstance, "ConnectionReqState", "GETRELAYPROFILE_FAIL");
                                    }
                                    return true;
                                   }
                            });
  reqGetRelayList.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["DestNID"] = topic_Get(tInstance, "DestNID");
  reqParams["SourceNID"] = topic_Get(tInstance, "SourceNID");
  reqGetRelayList.reqParams = reqParams;
  
  topic_Set(tInstance, "ConnectionReqTopicName", "");
  reqGetRelayList.sendRequest(this);
  topic_Set(tInstance, "ConnectionReqTopicName", reqGetRelayList.req.TopicName);
}


//--------------------
function cmpReq_CreateConnection(tInstance) {
  var DestNID = topic_Get(tInstance, "DestNID");
  var SourceNID = topic_Get(tInstance, "SourceNID");
  var ProfileNID = topic_Get(tInstance, "ProfileNID");
  var RelayNID = topic_Get(tInstance, "RelayNID");
  //var StartOffset = topic_Get(tInstance, "StartOffset"); //TBD
  var StartOffset = null;
  var profileNode = topic_GetNode(tInstance, "//ProfileXML/*");
  //var profileNode = topic_GetNode(tInstance, "//ProfileXML/*");
  var RxMediaIpAddress = topic_Get(tInstance, "RxMediaIpAddress");
  var RxMediaPort = topic_Get(tInstance, "RxMediaPort");
  var reqCreateConnection = new ReqCreateConnection(SourceNID, DestNID, RelayNID, StartOffset, ProfileNID, profileNode, RxMediaIpAddress, RxMediaPort, 
                                    {action:"FNCCALL", context:this,
                                     fnc:function(respCode, respVal){
                                          // check that this is a response to the current request
                                          var ConnectionReqTopicName = topic_Get(tInstance, "ConnectionReqTopicName");
                                          if( reqCreateConnection.req.TopicName != ConnectionReqTopicName ) {
                                            return true;
                                          }
                                          // check respCode and proceed
                                          if( respCode == "SUCCESS" && respVal != null) {
                                            // get streamNID, roomNID
                                            var StreamNID = respVal.selectSingleNode("//streamNID").getValue();
                                            topic_Set(tInstance, "StreamNID", StreamNID);
                                            var RoomNID = respVal.selectSingleNode("//roomNID").getValue();
                                            topic_Set(tInstance, "RoomNID", RoomNID);
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                          }
                                          // finally check if we ended up with a valid room nid or not
                                          var RoomNID = topic_Get(tInstance, "RoomNID");
                                          if( RoomNID != "" && RoomNID !="PENDING" ) {
                                            topic_Publish(tInstance, "ConnectionReqState", "CREATECONNECTION_SUCCESS");
                                          }
                                          else {
                                            topic_Publish(tInstance, "ConnectionReqState", "CREATECONNECTION_FAIL");
                                          } 
                                          return true;
                                        }
                                    });
  topic_Set(tInstance, "ConnectionReqTopicName", "");
  reqCreateConnection.sendRequest(this);
  topic_Set(tInstance, "ConnectionReqTopicName", reqCreateConnection.req.TopicName);
}
//--------------------
function cmpReq_GetMediaRoom(tInstance) {
  var RoomNID = topic_Get(tInstance, "RoomNID");
  var reqGetMediaRoom = new ReqGetMediaRoom(RoomNID,
                                  { action:"FNCCALL", context:this,
                                    fnc:function(respCode, respVal) {
                                          // start off by clearing the room XML node
                                          var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                          if( roomXMLNode != null ) {
                                            roomXMLNode.removeChildren();
                                          }
                                          // then check on respCode and proceed
                                          if( respCode == "SUCCESS" && respVal != null) {
                                            var roomNode = respVal.selectSingleNode("//MediaRoom");
                                            if(roomNode != null){
                                              var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                              if( roomXMLNode == null ) {
                                                roomXMLNode = topic_Set(tInstance, "MediaRoomXML", null);
                                              }
                                              else {
                                                roomXMLNode.removeChildren();
                                              }
                                              roomXMLNode.appendChild(roomNode.cloneNode(true));
                                            }
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                          }
                                          // finally check if we ended up with a valid room XML node or not
                                          var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                          if( roomXMLNode.selectSingleNode("./*") != null ) {
                                            topic_Publish(tInstance, "ConnectionReqState", "GETMEDIAROOM_SUCCESS");
                                          }
                                          else {
                                            topic_Publish(tInstance, "ConnectionReqState", "GETMEDIAROOM_FAIL");
                                          }              
                                          return true;
                                        }
                                    });
  reqGetMediaRoom.sendRequest(this);
}

//--------------------
function cmpReq_AddStream(tInstance, streamMode) {
  // get source nid from the stream
  var StreamNID = topic_Get(tInstance, "StreamNID");
  var nodeStream = topic_GetNode(tInstance, "//MediaRoom//Stream[@NID='" + StreamNID + "']");
  if( nodeStream == null ) return;
  var SourceNID = nodeStream.selectSingleNode(".//data/mediaSourceNID").getValue();
  if( streamMode == "PVR" ) {
    var DestNID = topic_Get(tInstance, "PVRDestFileNID");
    var Description = "This is the PVR Stream";
  }
  else {
    var DestNID = topic_Get(tInstance, "DestFileNID");
    var Description = "This is the Record Stream";
  }
  // no relay - use of SPACE is a hack - to work around nillable node in output
  var RelayNID = "";
  // reuse original stream profile
  var ProfileNID = topic_Get(tInstance, "ProfileNID");
  var profileNode = topic_GetNode(tInstance, "//ProfileXML/*");
  // add stream just like any other
  var reqAddStream = new ReqAddStream(SourceNID, DestNID, RelayNID, Description, ProfileNID, profileNode,
                                  { action:"FNCCALL", context:this,
                                    fnc:function(respCode, respVal) {
                                          // start by clearing out the stream NID
                                          if( streamMode == "PVR" ) {
                                            topic_Set(tInstance, "PVRStreamNID", "");
                                          }
                                          else {
                                            topic_Set(tInstance, "RecordStreamNID", "");
                                          }
                                          if( respCode == "SUCCESS" && respVal != null) {
                                            // get record streamNID
                                            var StreamNID = respVal.selectSingleNode("//streamNID").getValue();
                                            if( streamMode == "PVR" ) {
                                              topic_Set(tInstance, "PVRStreamNID", StreamNID);
                                            }
                                            else {
                                              topic_Set(tInstance, "RecordStreamNID", StreamNID);
                                            }
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                          }
                                            
                                          // finally check if we ended up with a valid fileNID or not
                                          if( streamMode == "PVR" ) {
                                            var StreamNID = topic_Get(tInstance, "PVRStreamNID");
                                            if( StreamNID != null && StreamNID != "" ) {
                                              topic_Publish(tInstance, "AddPVRStreamState", "SUCCESS");
                                            }
                                            else {
                                              topic_Publish(tInstance, "AddPVRStreamState", "FAIL");
                                            }
                                          }
                                          else {
                                            var StreamNID = topic_Get(tInstance, "RecordStreamNID");
                                            if( StreamNID != null && StreamNID != "" ) {
                                              topic_Publish(tInstance, "AddRecordStreamState", "SUCCESS");
                                            }
                                            else {
                                              topic_Publish(tInstance, "AddRecordStreamState", "FAIL");
                                            }
                                          }
                                          return true;
                                        }
                                    });
  reqAddStream.sendRequest(this);
}
//--------------------
function cmpReq_GetCurrentTimeStamp(tInstance) {
  //????Make sure that we need to use StreamNID/RecordSreamNID
  //use StreamNID to get timecode from live stream
  var StreamNID = topic_Get(tInstance, "StreamNID");
  if( StreamNID == null ) return;
  var reqGetCurrentTimeStamp = new ReqGetCurrentTimeStamp(StreamNID, "GetCurrentTimeStamp",
                                                          { action:"FNCCALL", context:this,
                                                            fnc:function(respCode, respVal) {
                                                                  if( respCode == "SUCCESS" && respVal != null ) {
                                                                    var BookmarkAtTC = respVal.selectSingleNode("//StreamCurrentTimeStamp").getValue();
                                                                    topic_Set(tInstance, "BookmarkAtTC", BookmarkAtTC);
                                                                    topic_Publish(tInstance, "BookmarkDlgState", "ADD");
                                                                  }
                                                                  else {
                                                                    //publishing Error code to the connection state msg tooltip
                                                                    cmpReq_HandleErrorCode(tInstance, respCode);
                                                                  }
                                                                  return true;
                                                                }
                                                           });
  reqGetCurrentTimeStamp.sendRequest(this);
}
//--------------------
function cmpReq_AddStreamAction(tInstance, StreamNID, UpdateAction) {
  var ProfileNID = topic_Get(tInstance, "ProfileNID");
  var profileNode = topic_GetNode(tInstance, "//ProfileXML/*");
  if(profileNode ==  null){
    var newDoc = new jsx3.xml.Document();
    var profileXML = "<Profile> </Profile>";
    newDoc.loadXML(profileXML);  
    profileNode = newDoc;
  }
  var reqAddStreamAction = new ReqAddStreamAction(StreamNID, ProfileNID, profileNode, UpdateAction,
                                    {action:"FNCCALL", context:this,
                                     fnc:function(respCode, respVal){
                                           // TBD - need to put in code that handles these responses
                                           // decide success failure based on respCode
                                           if( respCode == "SUCCESS" && respVal != null) {
                                             topic_Publish(tInstance, "AddStreamActionState", StreamNID+":"+UpdateAction+"_SUCCESS");
                                           }
                                           else {
                                             //publishing Error code to the connection state msg tooltip
                                             cmpReq_HandleErrorCode(tInstance, respCode);
                                             if( respCode == "1023" ) {
                                               var InstanceStreamNID = topic_Get(tInstance,  "StreamNID");
                                               if( InstanceStreamNID == StreamNID ) {
                                                topic_Set(tInstance, "ConnectionXAuthState", "DENIED");
                                                topic_Publish(tInstance, "ConnectionReqState", "PLAYER_ERROR");
                                                return true;
                                               }
                                             }
                                             topic_Publish(tInstance, "AddStreamActionState", StreamNID+":"+UpdateAction+"_FAIL");
                                           }
                                           return true;
                                         }
                                     });
  reqAddStreamAction.sendRequest(this);
}
//-------------------- Session PVR API ----------------
function cmpReq_DeleteMediaRoom(tInstance) {
  var RoomNID = topic_Get(tInstance, "RoomNID");
  var reqDeleteMediaRoom = new ReqDeleteMediaRoom(RoomNID,
                                    {action:"FNCCALL", context:this,
                                     fnc:function(respCode, respVal){
                                          // decide success failure based on respCode
                                          if( respCode == "SUCCESS" && respVal != null ) {
                                            topic_Publish(tInstance, "ConnectionReqState", "DELETEMEDIAROOM_SUCCESS");
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                            topic_Publish(tInstance, "ConnectionReqState", "DELETEMEDIAROOM_FAIL");
                                          }
                                          return true;
                                        }
                                    });
  reqDeleteMediaRoom.sendRequest(this);
}
//------------
function cmpReq_GetDestDir(tInstance, streamMode) {
  var ObjectType = "";
  if( streamMode == "PVR" ) {
    ObjectType = "PVRDestMediaDir";
    var DestNID = topic_Get(tInstance, "PVRDestFileNID");
  }else{
    ObjectType = "DestMediaDir";
    var DestNID = topic_Get(tInstance, "DestFileNID");
  }
  
  var reqGetDestDirList = new ReqGetObjectList(ObjectType, 
                                    {action:"FNCCALL", context:this, 
                                     fnc:function(respCode, respVal){
                                          // start off by clearing the dest dir XML node
                                          if( streamMode == "PVR" ) {
                                            var destDirXMLNode = topic_GetNode(tInstance, "//PVRDestDirXML");
                                          }
                                          else {
                                            var destDirXMLNode = topic_GetNode(tInstance, "//DestDirXML");
                                          }
                                          if( destDirXMLNode != null ) {
                                            destDirXMLNode.removeChildren();
                                          }
                                          // then check on respCode and proceed
                                          if( respCode == "SUCCESS" && respVal != null) {
                                            var DestDirSelectedNID = "";
                                            var destDirNode = null;
                                            var destDirNIDListIter = respVal.selectNodeIterator(".//ObjectNID");
                                            // this will select the first dir of the list
                                            while( destDirNIDListIter.hasNext() ) {
                                              var DestDirNID = destDirNIDListIter.next().getValue();
                                              destDirNode = topic_GetNIDNode(tCache, DestDirNID);
                                              if( destDirNode != null ) {
                                                DestDirSelectedNID = DestDirNID;
                                                break;
                                              }
                                            }
                                            if( DestDirSelectedNID != "" ) {                                        
                                              if( streamMode == "PVR" ) {
                                                var destDirXMLNode = topic_Set(tInstance, "PVRDestDirXML", null);
                                                destDirXMLNode.appendChild(destDirNode.cloneNode(true));
                                              }
                                              else {
                                                var destDirXMLNode = topic_Set(tInstance, "DestDirXML", null);
                                                destDirXMLNode.appendChild(destDirNode.cloneNode(true));
                                              }
                                            }
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                          }
                                           
                                          // finally check if we ended up with a valid dir XML node or not
                                          if( streamMode == "PVR" ) {
                                            var destDirXMLNode = topic_GetNode(tInstance, "//PVRDestDirXML");
                                            if( destDirXMLNode != null && destDirXMLNode.selectSingleNode("./*") != null ) {
                                              topic_Publish(tInstance, "DestPVRDirState", "SUCCESS");
                                            }
                                            else {
                                              topic_Publish(tInstance, "DestPVRDirState", "FAIL");
                                            }
                                          }
                                          else {
                                            var destDirXMLNode = topic_GetNode(tInstance, "//DestDirXML");
                                            if( destDirXMLNode != null && destDirXMLNode.selectSingleNode("./*") != null ) {
                                              topic_Publish(tInstance, "DestDirState", "SUCCESS");
                                            }
                                            else {
                                              topic_Publish(tInstance, "DestDirState", "FAIL");
                                            }
                                          }          
                                          return true;
                                        }
                                    });

  var sortByXpath = "$VAR/min(//MediaStorePortResourceNIDList/MediaStorePortResourceNID/@priority)";
  var sortOrder = "ascending";

  reqGetDestDirList.ServiceName = "mediaroomservice";
  reqGetDestDirList.obj = null;
  var reqParams = new Object;
  reqParams["SourceNID"] = topic_Get(tInstance, "SourceNID");
  reqParams["Title"] = "";
  reqParams["Op_Select_ListSpec_SortByXPathList_SortByXpath"] = sortByXpath;
  reqParams["Op_Select_ListSpec_SortOrder"] = sortOrder;
  reqGetDestDirList.reqParams = reqParams;
  reqGetDestDirList.sendRequest(this);
  return true;
}
//------------
function cmpReq_AddDestFile(tInstance, streamMode, mediaStorePortResourceNID) {
  var MediaGroupFileResourceNID = topic_Get(tInstance, "MediaGroupFileResourceNID");
  if(MediaGroupFileResourceNID == null) {
    MediaGroupFileResourceNID = "";
  }
  var nodeDestDir = null;
  var nodeMetaData = null;
  var ClipType = null;
  if( streamMode == "PVR" ) {
    // forcing MediaGroupFileResourceNID = "" dont create files under the group id
    MediaGroupFileResourceNID = "";
    nodeDestDir = topic_GetNode(tInstance, "//PVRDestDirXML/*");
    nodeMetaData = topic_GetNode(tInstance, "//PVRDestFileMetaDataXML/*");
    if( nodeMetaData == null ) {
      var nodeMetaDataXML = topic_Set(tInstance, "PVRDestFileMetaDataXML", null);
      nodeMetaData = nodeMetaDataXML.createNode(jsx3.xml.Entity.TYPEELEMENT, "MetaData");
      nodeMetaData.setValue("");
      nodeMetaDataXML.appendChild(nodeMetaData.cloneNode(true));
    }
    ClipType = "PVRMediaClip"; // old value PVR
  }
  else if( streamMode == "RECORDGROUP" ) {
    nodeDestDir = topic_GetNode(tInstance, "//DestDirXML/*");
    nodeMetaData = topic_GetNode(tInstance, "//DestGroupFileMetaDataXML/*");
    if( nodeMetaData == null ) {
      var nodeMetaDataXML = topic_Set(tInstance, "DestGroupFileMetaDataXML", null);
      nodeMetaData = nodeMetaDataXML.createNode(jsx3.xml.Entity.TYPEELEMENT, "MetaData");
      nodeMetaData.setValue("");
      nodeMetaDataXML.appendChild(nodeMetaData.cloneNode(true));
    }    
    ClipType = "MediaGroup";
  }
  else {
    nodeDestDir = topic_GetNode(tInstance, "//DestDirXML/*");
    nodeMetaData = topic_GetNode(tInstance, "//DestFileMetaDataXML/*");
    ClipType = "MediaClip"; // old value RECORD
  }

  var FileTitle = topic_Get(tInstance, "FileTitle");
  if(FileTitle == null) {
    FileTitle = "";
  }
  var FileDescription = FileTitle;
  var ParentMediaDirNID = nodeDestDir.getAttribute("NID");
  
  var MediaStorePortResourceNID = mediaStorePortResourceNID;
  if(jsx3.util.strEmpty(mediaStorePortResourceNID)) {
    MediaStorePortResourceNID = nodeDestDir.selectSingleNode(".//MediaStorePortResourceNID").getValue();
  }
  
  var nodeGroups = nodeDestDir.selectSingleNode(".//Groups");
  var nodePermissions = null;
  
  var reqAddDestFile = new ReqAddDestFile(ClipType, MediaGroupFileResourceNID, FileTitle, FileDescription, 
                                          nodeGroups, nodeMetaData, nodePermissions, ParentMediaDirNID, 
                                          MediaStorePortResourceNID,  
                                          {action:"FNCCALL", context:this,
                                           fnc:function(respCode, respVal){
                                                // start off by clearing dest file NID
                                                if( streamMode == "PVR" ) {
                                                  topic_Set(tInstance, "PVRDestFileNID", "");
                                                }
                                                else {
                                                  topic_Set(tInstance, "DestFileNID", "");
                                                }
                                                if( respCode == "SUCCESS" && respVal != null) {
                                                  // get fileNID and stash it as destNID
                                                  var nodeDestFileNID = respVal.selectSingleNode("//ObjectNID");
                                                  var DestFileNID = nodeDestFileNID.getValue();
                                                  if( streamMode == "PVR" ) {
                                                    topic_Set(tInstance, "PVRDestFileNID", DestFileNID);
                                                  }
                                                  else {
                                                    topic_Set(tInstance, "DestFileNID", DestFileNID);
                                                  }
                                                }
                                                else {
                                                  //publishing Error code to the connection state msg tooltip
                                                  cmpReq_HandleErrorCode(tInstance, respCode);
                                                }
                                                  
                                                // finally check if we ended up with a valid fileNID or not
                                                if( streamMode == "PVR" ) {
                                                  var DestFileNID = topic_Get(tInstance, "PVRDestFileNID");
                                                  if( DestFileNID != null && DestFileNID != "" ) {
                                                    topic_Publish(tInstance, "DestPVRFileState", "SUCCESS");
                                                  }
                                                  else {
                                                    topic_Publish(tInstance, "DestPVRFileState", "FAIL");
                                                  }
                                                }
                                                else if( streamMode == "RECORDGROUP" ) {
                                                  var DestFileNID = topic_Get(tInstance, "DestFileNID");
                                                  if( DestFileNID != null && DestFileNID != "" ) {
                                                    topic_Publish(tInstance, "DestGroupFileState", "SUCCESS");
                                                  }
                                                  else {
                                                    topic_Publish(tInstance, "DestGroupFileState", "FAIL");
                                                  }
                                                }
                                                else {
                                                  var DestFileNID = topic_Get(tInstance, "DestFileNID");
                                                  if( DestFileNID != null && DestFileNID != "" ) {
                                                    topic_Publish(tInstance, "DestFileState", "SUCCESS");
                                                  }
                                                  else {
                                                    cmpReq_HandleErrorCode(tInstance, respCode);
                                                    topic_Publish(tInstance, "DestFileState", "FAIL");
                                                  }
                                                }
                                                return true;
                                              }
                                          });
  reqAddDestFile.sendRequest(this);
}
//------------
function cmpReq_DeleteDestFile(tInstance) {
  var FileNID = topic_Get(tInstance, "PVRDestFileNID");
  var reqDeleteDestFile = new ReqDeleteDestFile(FileNID,
                                    {action:"FNCCALL", context:this,
                                     fnc:function(respCode, respVal){
                                          // decide success failure based on respCode
                                          topic_Set(tInstance, "PVRDestFileNID", "");
                                          if( respCode == "SUCCESS" && respVal != null ) {
                                            topic_Publish(tInstance, "ConnectionReqState", "DELETEDESTFILE_SUCCESS");
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                            topic_Publish(tInstance, "ConnectionReqState", "DELETEDESTFILE_FAIL");
                                          }
                                          return true;
                                        }
                                    });
  reqDeleteDestFile.sendRequest(this);
}
//------------
function cmpReq_UpdateMetaData(tInstance, MediaNID, mediaMetaDataNode) {
  var reqUpdateMetaData = new ReqUpdateFileMetaData(MediaNID, mediaMetaDataNode,
                                    {action:"FNCCALL", context:this,
                                     fnc:function(respCode, respVal){
                                          if( respCode == "SUCCESS" && respVal != null) {
                                            topic_Publish( tInstance, "MetaDataState", (newServerDate()) );
                                          }
                                          else {
                                            //publishing Error code to the connection state msg tooltip
                                            cmpReq_HandleErrorCode(tInstance, respCode);
                                          }
                                          return true;
                                        }
                                    });
  reqUpdateMetaData.sendRequest(this);
}

//--------------------
//before adding a bookmark to get TC and Offset
function cmpReq_GetNewBookmark(tInstance) {
  //????Make sure that we need to use StreamNID/RecordSreamNID
  //use StreamNID to get timecode from live stream
  var StreamNID = topic_Get(tInstance, "StreamNID");
  if( StreamNID == null ) return;
  var reqGetNewBookmark = new ReqGetNewBookmark(StreamNID,
                                                { action:"FNCCALL", context:this,
                                                  fnc:function(respCode, respVal) {
                                                        if( respCode == "SUCCESS" && respVal != null ) {
                                                          var newBookmarkNode = respVal.selectSingleNode("//Bookmark");
                                                          var BookmarkNode = topic_GetNode(tInstance, "//BookmarkNode");
                                                          if( BookmarkNode != null ) {
                                                            BookmarkNode.removeChildren();
                                                          }
                                                          else {
                                                            BookmarkNode = topic_Set(tInstance, "BookmarkNode", null);
                                                          }
                                                          BookmarkNode.appendChild(newBookmarkNode);
                                                          topic_Publish(tInstance, "BookmarkDlgState", "ADD");
                                                        }
                                                        else {
                                                          //publishing Error code to the connection state msg tooltip
                                                          cmpReq_HandleErrorCode(tInstance, respCode);
                                                        }
                                                        return true;
                                                      }
                                                 });
  reqGetNewBookmark.sendRequest(this);
}

//---------------
//To Add/Edit/Delete bookmarks
function cmpReq_HandleBookmark(tInstance, BookmarkMode, BookmarkNID, BookmarkNode) {
  var MediaNID = topic_Get(tInstance, "NIDForBookmark");
  if( MediaNID ==null ) return;
  var reqHandleBookmark = new ReqHandleBookmark(BookmarkMode, BookmarkNID, BookmarkNode,
                              {action:"FNCCALL", context:this,
                               fnc:function(respCode, respVal){
                                    if( respCode == "SUCCESS" && respVal != null) {
                                      if( BookmarkMode == "DeleteBookmark" ) {
                                        var cacheBookmarkNode = topic_GetNIDNode(tCache, BookmarkNID);
                                        if(cacheBookmarkNode != null) {
                                          cacheBookmarkNode.getParent().removeChild(cacheBookmarkNode);
                                        }
                                      }
                                      topic_Publish( tInstance, "BookmarkState", (newServerDate()+1) );
                                    }
                                    else {
                                      //publishing Error code to the connection state msg tooltip
                                      cmpReq_HandleErrorCode(tInstance, respCode);
                                    }
                                    return true;
                                  }
                              });
  reqHandleBookmark.sendRequest(this);
}

function cmpReq_GenerateGetBookmarkReq(BookmarkNID, Mode) {
  // if delete - then just fire a delete event
  if( Mode == "DELETE" ) {
    topic_Publish(tApp, "BookmarkNodeState", BookmarkNID);
    return null;
  }
  var objListReqInfo = arrServiceObjectInfo["Bookmark"];
  var CountToFetch = 1;
  var StartFrom = 0;
  // create a new request
  var reqGetBookmark = new ReqGetObjectListRangeWhere(objListReqInfo,
                                  { action: "FNCCALL", context: this,
                                    fnc: function(respCode, respVal) {
                                           if( respCode == "SUCCESS" && respVal != null) {
                                             if( Mode == "ADD" ) {
                                               //When Add notification, the request will be sent automatically to get that bookmark.
                                               //Once Response comes back that will be added to cache even though it is not required.
                                               //Check MediaStreamNID of that bookmark is in use else trim that bookmark from cache.
                                               var BookmarkNode = topic_GetNIDNode(tCache, BookmarkNID);
                                               if( BookmarkNode != null ) {
                                                 var MediaFileNID = BookmarkNode.selectSingleNode(".//MediaStreamFileResourceNID").getValue();
                                                 trimBookmarkCacheNode(MediaFileNID);
                                               }
                                               topic_Publish(tApp, "BookmarkNodeState", BookmarkNID);
                                             }
                                             else if( Mode == "UPDATE" ) {
                                               topic_Publish(tApp, "BookmarkNodeState", BookmarkNID);
                                             }
                                           }
                                         }
                                  });
  reqGetBookmark.ObjectType = "Bookmark";
  reqGetBookmark.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["MediaStreamFileResourceNID"] = "";
  reqParams["Op_Where_WhereXPath"] = "//Bookmark[@NID='" + BookmarkNID + "']";
  reqParams["Op_Select_ListSpec_StartFrom"] = StartFrom;
  reqParams["Op_Select_ListSpec_CountToFetch"] = CountToFetch;
  reqGetBookmark.reqParams = reqParams;
  return reqGetBookmark;
}

//---------------
function cmpReq_GetSrcMediaFileTrack(tInstance, MediaFileNID) {
  var reqGetSrcMediaFileTrack = new ReqGetSrcMediaFileTrack(MediaFileNID,
                                         {action:"FNCCALL", context:this,
                                          fnc:function(respCode, respVal) {
                                                // start off by clearing the SrcMediaFileTrack XML node
                                                var SrcMediaFileTrackNode = topic_GetNode(tInstance, "//SrcMediaFileTrack");
                                                if( SrcMediaFileTrackNode != null ) {
                                                  SrcMediaFileTrackNode.removeChildren();
                                                }
                                                else {
                                                  SrcMediaFileTrackNode = topic_Set(tInstance, "SrcMediaFileTrack", null);
                                                }
                                                // then check on respCode and proceed
                                                if( respCode == "SUCCESS" && respVal != null) {
                                                  var elementIter = respVal.selectNodeIterator("//ResponseObjectNIDList/*");
                                                  while(elementIter.hasNext()) {
                                                    var SrcMediaFileTrackInfo = elementIter.next().cloneNode(true);
                                                    SrcMediaFileTrackNode.appendChild(SrcMediaFileTrackInfo);
                                                  }
                                                  //topic_Publish(tApp, "MediaSFRDataNodeState", MediaFileNID);
                                                  //send request for bookmarks
                                                  var NIDForBookmark = topic_Get(tInstance, "NIDForBookmark");
                                                  cmpReq_GetBookmarkList(tInstance, NIDForBookmark);
                                                }
                                                else {
                                                  //publishing Error code to the connection state msg tooltip
                                                  cmpReq_HandleErrorCode(tInstance, respCode);
                                                }
                                                return true;
                                              }
                                         });
  reqGetSrcMediaFileTrack.sendRequest(this);
}
//--------------------
function cmpReq_GetBookmarkList(tInstance, MediaStreamFileResourceNID) {
  var reqGetBookmarkList = new ReqGetObjectList("Bookmark", 
                              {action:"FNCCALL", context:this, 
                               fnc:function(respCode, respVal){
                                     if( respCode == "SUCCESS" && respVal != null) {
                                       topic_Publish(tApp, "MediaSFRDataNodeState", MediaStreamFileResourceNID);
                                     }
                                     else {
                                       //publishing Error code to the connection state msg tooltip
                                       cmpReq_HandleErrorCode(tInstance, respCode);
                                     }
                                     return true;
                                   }
                            });
  reqGetBookmarkList.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["MediaStreamFileResourceNID"] = MediaStreamFileResourceNID;
  reqParams["Op_Where_WhereXPath"] = "";
  reqParams["Op_Select_ListSpec_StartFrom"] = 0;
  reqParams["Op_Select_ListSpec_CountToFetch"] = "-1";
  reqGetBookmarkList.reqParams = reqParams;
  reqGetBookmarkList.sendRequest(this);
}
//-------------
//Handle response errorcode using CommonErrorCodeArray
function cmpReq_HandleErrorCode(tInstance, errorCode) {
  var ErrorCodeArr = CommonErrorCodeArr[errorCode];
  if( ErrorCodeArr != null ) {
    var codeErrorMsg = ErrorCodeArr.errorDesc;
    //var ErrorMsg = "Error: GetStreamProfileList request failed \n";
    var ErrorMsg = codeErrorMsg;
    topic_Publish(tInstance, "ERRORSTATUS", ErrorMsg);
  }
  return true;
}

//this function will be called to get a request that can get one media file
// the mode parameter decides what type of event to fire (ie: why we are getting this node)
function cmpReq_GetMediaFileReq(MediaFileNID, Mode) {
  // if delete - then just fire a delete event
  if( Mode == "DELETE" ) {
    topic_Publish(tApp, "MediaSFRDataDeleteNode", MediaFileNID);
    return null;
  }
  // if add/update send request to get new node
  // get a handle on the object info
  var objCacheInfo = arrServiceObjectInfo["MediaStreamFileResource"];
  if( objCacheInfo.listreqarr == null ) {
    return false;
  }
  var objListReqInfo = objCacheInfo.listreqarr["NID"];
  if( objListReqInfo == null ) {
    return false;
  }
  var CountToFetch = 1;
  var StartFrom = 0;
  // create a new request
  var reqGetMediaFile = new ReqGetObjectListRangeWhere(objListReqInfo,
                                  { action: "FNCCALL", context: this,
                                    fnc: function(respCode, respVal) {
                                      if( Mode == "ADD" ) {
                                        // trim bookmark and gaplist
                                        trimMediaFileCacheNode(MediaFileNID);
                                        topic_Publish(tApp, "MediaSFRDataAddNode", MediaFileNID);
                                      }
                                      else if( Mode == "UPDATE" ) {
                                        // get currently selected mediafile. 
                                        //If matches then dont trim otherwise trim the contents
                                        var currentlySelectedMediaFileNID = topic_Get(tApp, "MediaSFRSelected");
                                        if( currentlySelectedMediaFileNID != MediaFileNID ) {
                                          // trim bookmark and gaplist func defined in commoncachefnc.js
                                          trimMediaFileCacheNode(MediaFileNID);
                                        }
                                        topic_Publish(tApp, "MediaSFRDataUpdateNode", MediaFileNID);
                                      }
                                      else if( Mode == "PLAY" ) {
                                        // no trimming for play mode - we need it all
                                        topic_Publish(tApp, "MediaSFRDataNodeState", MediaFileNID);
                                      }
                                    }
                                  });
  reqGetMediaFile.ObjectType = "MediaStreamFileResource";
  if( PNameSpace.getEnv("NAMESPACE") == "IPVSAdminUI" ) {
    reqGetMediaFile.ServiceName = "assetadminservice";
  }
  else {
    reqGetMediaFile.ServiceName = "mediaroomservice";
  }
  var reqParams = new Object;
  reqParams["Op_Where_WhereXPath"] = "//MediaStreamFileResource[@NID='" + MediaFileNID + "']";
  reqParams["Op_Select_ListSpec_StartFrom"] = StartFrom;
  reqParams["Op_Select_ListSpec_CountToFetch"] = CountToFetch;
  reqGetMediaFile.reqParams = reqParams;

  //reqGetSrcMediaFile.sendRequest(this);
  return reqGetMediaFile;
}

//---------------
function cmpReq_SetCollaborationConfig(tInstance, ccNode, SetMode) {

  var reqSetCollaborationConfigObject = new ReqSetObject(SetMode, "SessionConfig", ccNode, 
                                                          {action:"FNCCALL", context:this, 
                                                             fnc:function(respCode, respVal) {
                                                                   if(respCode == "SUCCESS" && respVal != null) {
                                                                     //CollaborationList[tInstance].destroy();
                                                                     // if save was called when add then replace the node and nid
                                                                     if(SetMode == "Add") {
                                                                       var newCCNID = respVal.selectSingleNode(".//ObjectNID").getValue();
                                                                       delete this.sessionConfig;
                                                                       // get CC node from cache
                                                                       var ccNode = topic_GetNIDNode(tCache, newCCNID);
                                                                       if( ccNode ) {
                                                                        this.sessionConfig = new com.ipvs.api.SessionConfig.Document(ccNode.cloneNode(true));
                                                                       }
                                                                     }
                                                                     topic_Publish(tInstance, "CollaborationEventMsg", "\nSaving Session... Done");
                                                                   } else {
                                                                     //publishing Error code to the connection state msg tooltip
                                                                      var ErrorCodeArr = CommonErrorCodeArr[respCode];
                                                                      if( ErrorCodeArr != null ) {
                                                                        topic_Publish(tInstance, "CollaborationEventMsg", "\nError Saving Session: " + ErrorCodeArr.errorDesc);
                                                                      }                                                                   
                                                                   }
                                                                 }
                                                          }                                                 
                                                        );
  reqSetCollaborationConfigObject.sendRequest();
}

//------------
function cmpReq_CreateCollaboration(tInstance, ccNode) {

  var reqCreateCollaborationObject = new ReqCreateCollaboration(ccNode, 
                                                                {action:"FNCCALL", context:this, 
                                                                   fnc:function(respCode, respVal) {
                                                                         // start off by clearing the room XML node
                                                                         var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                                                         if( roomXMLNode != null ) {
                                                                           roomXMLNode.removeChildren();
                                                                         }
                                                                         else {
                                                                          roomXMLNode = topic_Set(tInstance, "MediaRoomXML", null);
                                                                         }
                                                                         // then check on respCode and proceed
                                                                         if( respCode == "SUCCESS" && respVal != null) {
                                                                           var collabStatusNode = respVal.selectSingleNode("//ConnectionStatusList");
                                                                           if(collabStatusNode != null){
                                                                             var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                                                             roomXMLNode.appendChild(collabStatusNode.cloneNode(true));
                                                                           }
                                                                           var roomNode = respVal.selectSingleNode("//MediaRoom");
                                                                           if(roomNode != null){
                                                                             var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                                                             roomXMLNode.appendChild(roomNode.cloneNode(true));
                                                                           }
                                                                         } else {
                                                                           //publishing Error code to the connection state msg tooltip
                                                                           cmpReq_HandleErrorCode(tInstance, respCode);
                                                                         }
                                                                         // finally check if we ended up with a valid room XML node or not
                                                                         var roomXMLNode = topic_GetNode(tInstance, "//MediaRoomXML");
                                                                         if( roomXMLNode.selectSingleNode("./*") != null ) {
                                                                           topic_Publish(tInstance, "ConnectionReqState", "CREATECOLLABORATION_SUCCESS");
                                                                         }
                                                                         else {
                                                                           topic_Publish(tInstance, "ConnectionReqState", "CREATECOLLABORATION_FAIL");
                                                                         }               
                                                                         return true;
                                                                       }
                                                                }                                                 
                                                              );
                                            
  reqCreateCollaborationObject.sendRequest();
}
//this function will be called to get a list of media files for a media dir incrementally
function cmpReq_GetCollabrationConfigList()
{
  var cmpReqCollaborationConfigObj = cmpReqObj_Get("SessionConfigDataState");
  if (cmpReqCollaborationConfigObj != null && cmpReqCollaborationConfigObj.listreqstate == null)
  {
    // if yes fire it off
    jsx3.sleep(function()
    {
      cmpReqCollaborationConfigObj.req.sendRequest();
    }, null, this);
  }
  // fire final event here and retun
  topic_Publish(tApp, "MediaCollaborationDataState", newServerDate());
  return true;
}
//for session start get collaboration config by name of NID
function cmpReq_GetCollabrationConfigByNIDOrName(ccNID, ccName,sessionId) {
  var whereClause = null;
  var collabNode = null;
  if (!jsx3.util.strEmpty(ccNID)) {
    whereClause = "//SessionConfig[@NID='" + ccNID + "']";
  }
  else {
    whereClause = "//SessionConfig[@name='" + ccName + "']"; //??????????
  }
  collabNode = topic_GetNode(tCache, whereClause); 
  if (collabNode == null) {
    var reqGetcollabConfig = new ReqGetObjectList("SessionConfig",
                              { action: "FNCCALL", context: this,
                                fnc: function(respCode, respVal) {
                                  if (respCode == "SUCCESS" && respVal != null) {
                                    cmpReq_GetCollabrationConfigByNIDOrName(ccNID, ccName, sessionId)
                                  }
                                  else {
                                    //publishing Error code to the connection state msg tooltip
                                    cmpReq_HandleErrorCode(tInstance, respCode);
                                  }
                                  return true;
                                }
                              });
    reqGetcollabConfig.sendRequest(this);
  }
  else {
        var respNode = new Object;
        var ccNIDNode = collabNode.selectSingleNode("./@NID");
        respNode.ccNID = ccNIDNode == null ? null : ccNIDNode.getValue();
        var ccNIDNameNode = collabNode.selectSingleNode("./collaborationName");
        respNode.ccName = ccNIDNameNode == null ? null : ccNIDNameNode.getValue();
        if (!jsx3.util.strEmpty(sessionId)) {
          respNode.sessionId = sessionId;
        }
        topic_Publish(tApp, "SessionStartCCNode", respNode);
  }
  
}

//this function will be called to get a list of media files for a collaboration NID incrementally
function cmpReq_GetPlaybackMediaFileList(tInstance, CollaborationNID) {
  // get a handle on the object info
  var objCacheInfo = arrServiceObjectInfo["SrcMediaFile"];
  if( objCacheInfo.listreqarr == null ) {
    return false;
  }
  var objListReqInfo = objCacheInfo.listreqarr[CollaborationNID];
  if( objListReqInfo == null ) {
    return false;
  }
  // setup request range -  based on what is in cache right now
  var TotalDBCount = objListReqInfo.nidlisttotaldbcount;
  var CountToFetch = objListReqInfo.nidlistdefaultfetchcount;
  var StartFrom = objListReqInfo.nidlistnextfrom;
  if( StartFrom == null ) {
     StartFrom = 0;
  }
  // if next start gt total db count
  if( TotalDBCount != null && StartFrom >= TotalDBCount ) {
    // fire final event here and retun
    topic_Publish(tInstance, "PlayBackMediaSFRDataState", newServerDate());
    return true;
  }
  // create a new list request
  var reqGetSrcMediaFileList = new ReqGetObjectListRangeWhere(objListReqInfo,
                                                              { action: "FNCCALL", context: this,
                                                                fnc: function(respCode, respVal) {
                                                                  // get object count info from response and set them in objInfo
                                                                  var ObjectCount = respVal.getAttribute("ObjectCount");
                                                                  var ListStartFrom = respVal.getAttribute("ListStartFrom");
                                                                  var ListCount = respVal.getChildNodes().size();
                                                                  updateReqObjPaginationParam(objListReqInfo, ObjectCount, ListStartFrom, ListCount);
                                                                  // call recursively to get the rest of the list iteratively
                                                                  cmpReq_GetPlaybackMediaFileList(tInstance, CollaborationNID);
                                                                  // fire an event to signal partial data state change --- tbd fix obj
                                                                }
                                                              }); 

  reqGetSrcMediaFileList.ObjectType = "SrcMediaFile";
  reqGetSrcMediaFileList.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["DestNID"] = "";
  reqParams["Op_Where_WhereXPath"] = ".//MediaStoreFileInfo/MetaData/Tag[@Name='CollaborationNID' and .='" + CollaborationNID + "']"; 
  reqParams["Op_Select_ListSpec_StartFrom"] = StartFrom;
  reqParams["Op_Select_ListSpec_CountToFetch"] = CountToFetch;
  reqGetSrcMediaFileList.reqParams = reqParams;

  reqGetSrcMediaFileList.sendRequest(this);
  return true;
}


//this function will be called to get a request that can get one media file
function cmpReq_GetSrcMediaFileReq(mediaInfo) {
  // get a handle on the object info
  var objCacheInfo = arrServiceObjectInfo["SrcMediaFile"];
  if (objCacheInfo.listreqarr == null) {
    return false;
  }
  var objListReqInfo = objCacheInfo.listreqarr["NID"];
  if (objListReqInfo == null) {
    return false;
  }
  
  var whereXPath = null;
  if (!jsx3.util.strEmpty(mediaInfo.NID)) {
    whereXPath = "//MediaStreamFileResource[@NID='" + mediaInfo.NID + "']";
  }
  else if (!jsx3.util.strEmpty(mediaInfo.Name)) {
    whereXPath = "//MediaStreamFileResource[.//Info[Title='" + mediaInfo.Name + "']]";
  } 
  
  var CountToFetch = 1;
  var StartFrom = 0;
  // create a new request
  var reqGetSrcMediaFile = new ReqGetObjectListRangeWhere(objListReqInfo,
                                  { action: "FNCCALL", context: this,
                                    fnc: function(respCode, respVal) {
                                        if(respCode == "SUCCESS" && respVal != null) {
                                          var mediaSFRNode = topic_GetNIDNode(tCache, mediaInfo.NID);
                                          if (jsx3.util.strEmpty(mediaSFRNode)) {
                                            log.error("Can't find " + mediaInfo.NID + " in cache");
                                            return;
                                          }
                                          if (mediaSFRNode.selectSingleNode(".//MediaStoreFileInfo/Type").getValue() == "MediaGroup") {
                                            var MediaGroupNID = mediaSFRNode.selectSingleNode(".//MediaStoreFileInfo/MediaGroupID").getValue();
                                              topic_Set(tApp, "StoredMediaInfo", mediaInfo);
                                              var whereClause = "//MediaStreamFileResource[.//MediaStoreFileInfo/MediaGroupID='"+ MediaGroupNID +"']";
                                              cmpReq_GetSrcMediaGroupFiles(mediaInfo,whereClause);
                                          }
                                          else{
                                            topic_Publish(tApp, "MediaListSelection", mediaInfo);
                                          }
                                        }
                                    }
                                  });
  reqGetSrcMediaFile.ObjectType = "SrcMediaFile";
  reqGetSrcMediaFile.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["DestNID"] = topic_Get(tApp, "DestNID");
  reqParams["Op_Where_WhereXPath"] = whereXPath; 
  reqParams["Op_Select_ListSpec_StartFrom"] = StartFrom;
  reqParams["Op_Select_ListSpec_CountToFetch"] = CountToFetch;
  reqGetSrcMediaFile.reqParams = reqParams;

  reqGetSrcMediaFile.sendRequest(this);
}

//this function will be called to get a list of media files for a media dir incrementally
function cmpReq_GetSrcMediaGroupFiles(mediaInfo,whereClause) {
  // get a handle on the object info
  var objCacheInfo = arrServiceObjectInfo["SrcMediaFile"];
  if (objCacheInfo.listreqarr == null) {
    return false;
  }
  // create a new list request
  var reqGetSrcMediaGroupFiles = new ReqGetObjectListRangeWhere(objCacheInfo,
                                  { action: "FNCCALL", context: this,
                                    fnc: function(respCode, respVal) {
                                      var tt = "ttt";
                                      topic_Publish(tApp, "MediaListSelection", mediaInfo);
                                    }
                                  });

  reqGetSrcMediaGroupFiles.ObjectType = "SrcMediaFile";
  reqGetSrcMediaGroupFiles.ServiceName = "mediaroomservice";
  var reqParams = new Object;
  reqParams["DestNID"] = topic_Get(tApp, "DestNID");
  reqParams["Op_Where_WhereXPath"] = whereClause;
  reqParams["Op_Select_ListSpec_StartFrom"] = 0;
  reqParams["Op_Select_ListSpec_CountToFetch"] = "-1";
  reqGetSrcMediaGroupFiles.reqParams = reqParams;

  reqGetSrcMediaGroupFiles.sendRequest(this);
  return true;
}

//function to get the Bookmarks using session API 
cmpReq_GetSessionBookmarkPaginationList.prototype = new ReqSessionGetBookmarkRequest();
function cmpReq_GetSessionBookmarkPaginationList(InstanceID, ReqEvent) {
  // setup inheritence and call base constructor
  cmpReq_GetSessionBookmarkPaginationList.BaseClass = cmpReq_GetSessionBookmarkPaginationList.prototype;
  cmpReq_GetSessionBookmarkPaginationList.prototype.constructor = cmpReq_GetSessionBookmarkPaginationList;
  ReqSessionGetBookmarkRequest.prototype.constructor.call(this, ReqEvent);

  this.InstancID = InstanceID;
  this.reqParams = null;

  this.sendRequest = function() {
    var InstanceID = this.InstancID;

    this.reqParams["ObjectList_WhereNID"] = topic_Get(this.InstancID, "whereNID");
    this.reqParams["ObjectList_WhereTextSubStr"] = topic_Get(this.InstancID, "whereTextSubStr");
    this.reqParams["ObjectList_WhereDateRange"] = topic_Get(this.InstancID, "whereDateRange");
    this.reqParams["whereTCRange"] = topic_Get(this.InstancID, "whereTCRange");
    this.reqParams["whereFileNID"] = topic_Get(this.InstancID, "whereFileNID");
    this.reqParams["whereSessionInfo"] = topic_Get(this.InstancID, "whereSessionInfo");
    
    cmpReq_GetSessionBookmarkPaginationList.BaseClass.sendRequest.call(this);
  }

  this.onSuccess = function(req, ErrCode, response) {
    if(ErrCode == "SUCCESS" && response != null) {
      //set the variable values
      this.reqParams["ObjectCount"] = response.getAttribute("total");
      this.reqParams["ObjectList_StartFrom"] = response.getAttribute("from");

      cmpReq_GetSessionBookmarkPaginationList.BaseClass.onSuccess.call(this, req, ErrCode, response);
    }

  }
} // end
