var tCache = "IPVSDataCache";
var tCacheDoc = null;

// for async notification handling
var arrObjListInfo = null;
  
// info for all objects in the system should sit here
var arrServiceObjectInfo = null;
var syncID = null;

function commonCache_Init() {
  arrServiceObjectInfo = new Object();
  arrObjListInfo = new Object();

  arrObjListInfo["userlist"] = "User";
  arrObjListInfo["assetresourcelist"] = "AssetResource";
  arrObjListInfo["devicelist"] = "Device";
  arrObjListInfo["mediastreamdirresourcelist"] = "MediaStreamDirResource";
  arrObjListInfo["mediastreamfileresourcelist"] = "MediaStreamFileResource";
  arrObjListInfo["bookmarklist"] = "Bookmark";
  arrObjListInfo["mediastoreportresourcelist"] = "MediaStorePortResource";
  arrObjListInfo["mediastreamsrcportresourcelist"] = "MediaStreamSrcPortResource";
  arrObjListInfo["mediastreamdstportresourcelist"] = "MediaStreamDstPortResource";
  arrObjListInfo["mediastreamrelayportresourcelist"] = "MediaStreamRelayPortResource";
  arrObjListInfo["mediastreamioportresourcelist"] = "MediaStreamIOPortResource";
  arrObjListInfo["mediastreamprofilelist"] = "MediaStreamProfile";
  arrObjListInfo["mediaroomconfiglist"] = "MediaRoomConfig";
  arrObjListInfo["mediaroomlist"] = "MediaRoom";
  arrObjListInfo["collaborationconfiglist"] = "CollaborationConfig";
  arrObjListInfo["serviceresourcelist"] = "ServiceResource";
  arrObjListInfo["sessionconfiglist"] = "SessionConfig";
  arrObjListInfo["networkaddressrulelist"] = "NetworkAddressRule";
  arrObjListInfo["grouptagdeflist"] = "GroupTagDef";
  arrObjListInfo["filterlist"] = "Filter";
  arrObjListInfo["tagobjectfilterlist"] = "TagObjectFilter";
  arrObjListInfo["objectfilterlist"] = "ObjectFilter";
  arrObjListInfo["policyadminpolicyrulelist"] = "PolicyAdminPolicyRule";
  arrObjListInfo["useradminpolicyrulelist"] = "UserAdminPolicyRule";
  arrObjListInfo["assetadminpolicyrulelist"] = "AssetAdminPolicyRule";
  arrObjListInfo["mediastoreadminpolicyrulelist"] = "MediaStoreAdminPolicyRule";
  arrObjListInfo["mediaroompolicyrulelist"] = "MediaRoomPolicyRule";
  arrObjListInfo["mediastreamfilterlist"] = "MediaStreamFilter";
  arrObjListInfo["exportimportfilelist"] = "ExportImportFileListItem";
  arrObjListInfo["displayprofilelist"] = "DisplayProfile";
  arrObjListInfo["reportlist"] = "Report";

  arrServiceObjectInfo["Login"] = {cacheRoot:null,
                                    nidlist:null, 
                                    grouptreelist:null, 
                                    grouptagxpath:null, 
                                    successevent:{action:"APPEVENT",
                                                  eventvar:"LoginState",
                                                  eventval:"VALID"},
                                    failevent:{action:"APPEVENT",
                                               eventvar:"LoginState",
                                               eventval:"ERROR"},
                                    listreqstate:null};
                                    
  arrServiceObjectInfo["Logout"] = {cacheRoot:null,
                                    nidlist:null, 
                                    grouptreelist:null, 
                                    grouptagxpath:null, 
                                    successevent:{action:"APPEVENT",
                                                  eventvar:"LogoutState",
                                                  eventval:"VALID"},
                                    failevent:{action:"APPEVENT",
                                               eventvar:"LogoutState",
                                               eventval:"ERROR"},
                                    listreqstate:null};
                                    
  arrServiceObjectInfo["ServiceDomain"] = {cacheRoot:null,
                                            nidlist:null, 
                                            grouptreelist:null, 
                                            grouptagxpath:null, 
                                            successevent:{action:"APPEVENT",
                                                          eventvar:"ServiceState",
                                                          eventval:"VALID"},
                                            failevent:{action:"APPEVENT",
                                                       eventvar:"ServiceState",
                                                       eventval:"ERROR"},
                                            listreqstate:null};
                                            
  arrServiceObjectInfo["ServerTime"] = {cacheRoot:null,
                                            nidlist:null, 
                                            grouptreelist:null, 
                                            grouptagxpath:null, 
                                            successevent:{action:"APPEVENT",
                                                          eventvar:"ServerTimeState",
                                                          eventval:"DATE"},
                                            failevent:{action:"APPEVENT",
                                                       eventvar:"ServerTimeState",
                                                       eventval:"ERROR"},
                                            listreqstate:null}; 

  arrServiceObjectInfo["EmptyRecycleBin"] = {cacheRoot:null,
                                              nidlist:null, 
                                              grouptreelist:null, 
                                              grouptagxpath:null, 
                                              successevent:{action:"APPEVENT",
                                                            eventvar:"EmptyRecycleBinState",
                                                            eventval:"DATE"},
                                              failevent:{action:"APPEVENT",
                                                         eventvar:"EmptyRecycleBinState",
                                                         eventval:"ERROR"},
                                              listreqstate:null};                                            
                                                                                       
  arrServiceObjectInfo["User"] = {cacheRoot:"UserList",
                                  nidlist:"UserNIDList", 
                                  grouptreelist:"UserGroupTree", 
                                  grouptagxpath:".//Groups/Tag", 
                                  successevent:{action:"APPEVENT",
                                                eventvar:"UserDataState",
                                                eventval:"DATE"},
                                  listreqstate:null};
                                  
  arrServiceObjectInfo["AssetResource"] = {cacheRoot:"AssetResourceList",
                                           nidlist:"AssetNIDList",
                                           grouptagxpath:".//Info/Groups/Tag",
                                           grouptreelist:"AssetGroupTree", 
                                           successevent:{action:"APPEVENT", 
                                                         eventvar:"AssetResourceDataState", 
                                                         eventval:"DATE"},
                                           listreqstate:null};
  arrServiceObjectInfo["Asset"] = arrServiceObjectInfo["AssetResource"];

  arrServiceObjectInfo["ServiceResource"] = {cacheRoot:"ServiceResourceList",
                                             nidlist:"ServiceNIDList",
                                             grouptagxpath:null,
                                             grouptreelist:null,
                                             reqservice:"assetadminservice", 
                                             successevent:{action:"APPEVENT", 
                                                           eventvar:"ServiceResourceDataState", 
                                                           eventval:"DATE"},
                                             failevent:{action:"APPEVENT", 
                                                           eventvar:"ServiceResourceDataState", 
                                                           eventval:"DATE"},              
                                             listreqstate:null};  

  arrServiceObjectInfo["Device"] = {cacheRoot:"DeviceList",
                                    nidlist:"DeviceNIDList", 
                                    grouptreelist:null, 
                                    grouptagxpath:null,
                                    successevent:{action:"APPEVENT", 
                                                  eventvar:"DeviceDataState", 
                                                  eventval:"DATE"},
                                    listreqstate:null};
  
  /*arrServiceObjectInfo["V2D-Tx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-Rx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-XP200-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-XP200S-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-XP220-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-XP220S-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-XP100-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2D-XPi-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-120-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-1000-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["IPVSClient-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["IPVS-Client-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-100-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-200-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-1200-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-1500-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-1600-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-1700-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["DMS-25-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["MS-25-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["MS-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["UDP-Tx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["UDP-Rx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["RTP-Tx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["RTP-Rx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["MPEGTS-Tx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["MPEGTS-Rx-Device"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["ORB-500-Device"] = arrServiceObjectInfo["Device"];*/
  
  // These have to be inside the respective device node TBD
  arrServiceObjectInfo["V2DEncoderInstance"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2DStreamTxInstance"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["V2DStreamRxInstance"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["StreamWriterInstance"] = arrServiceObjectInfo["Device"];
  arrServiceObjectInfo["StreamReaderInstance"] = arrServiceObjectInfo["Device"];


  arrServiceObjectInfo["MediaStreamFileResource"] = { cacheRoot:"MediaStreamFileResourceList",
                                                      nidlist:"MediaSFRNIDList", 
                                                      nidlisttotaldbcount:null,
                                                      nidlistcurrentfrom:null,
                                                      nidlistnextfrom:0,
                                                      nidlistdefaultfetchcount:10,
                                                      listreqarr:{NID:{nidlisttotaldbcount:null,
                                                                      nidlistcurrentfrom:null,
                                                                      xpathquery:null,
                                                                      nidlistnextfrom:0,
                                                                      nidlistdefaultfetchcount: 1,
                                                                      listreqstate:null}},
                                                      listreqitembyNIDfnc:null,
                                                      listnidrefcount:[],
                                                      grouptreelist:"MediaSFRGroupTree", 
                                                      grouptagxpath:".//Info/Groups/Tag", 
                                                      successevent:null,
                                                      listreqstate:null};

  arrServiceObjectInfo["DisplayProfile"] = {cacheRoot:"DisplayProfileList",
                                            nidlist:"DisplayProfileNIDList", 
                                            grouptreelist:"DisplayProfileGroupTree", 
                                            grouptagxpath:".//Groups/Tag", 
                                            successevent:{action:"APPEVENT",
                                                          eventvar:"DisplayProfileDataState",
                                                          eventval:"DATE"},
                                            listreqstate:null};
                                                                                        
  arrServiceObjectInfo["SrcMediaFile"] = arrServiceObjectInfo["MediaStreamFileResource"];

  arrServiceObjectInfo["Bookmark"] = {cacheRoot:"BookmarkList",
                                    nidlist:"BookmarkNIDList", 
                                    grouptreelist:null, 
                                    grouptagxpath:null,
                                    successevent:null,
                                    listreqstate:null};

  arrServiceObjectInfo["ExportImportFileListItem"] = {cacheRoot:"ExportImportFileList",
                                                      nidlist:"ExportImportFileNIDList", 
                                                      grouptreelist:null, 
                                                      grouptagxpath:null,
                                                      successevent:null,
                                                      listreqstate:null};

  arrServiceObjectInfo["MediaStreamDirResource"] = { cacheRoot:"MediaStreamDirResourceList",
                                                      nidlist:"MediaSDRNIDList", 
                                                      grouptreelist:"MediaSDRGroupTree", 
                                                      grouptagxpath:".//Info/Groups/Tag", 
                                                      successevent:{action:"APPEVENT",
                                                                    eventvar:"MediaSDRDataState",
                                                                    eventval:"DATE"},
                                                      listreqstate:null};
  arrServiceObjectInfo["SrcMediaDir"] = arrServiceObjectInfo["MediaStreamDirResource"];

  arrServiceObjectInfo["MediaStreamSrcPortResource"] = {cacheRoot:"MediaStreamSrcPortResourceList",
                                                        nidlist:"SourcePortNIDList", 
                                                        grouptreelist:"SourcePortGroupTree", 
                                                        grouptagxpath:".//Info/Groups/Tag", 
                                                        successevent:{action:"APPEVENT",
                                                                      eventvar:"MediaStreamSrcPortResourceDataState", 
                                                                      eventval:"DATE"},
                                                        listreqstate:null};
  arrServiceObjectInfo["Source"] = arrServiceObjectInfo["MediaStreamSrcPortResource"];
  arrServiceObjectInfo["SourcePort"] = arrServiceObjectInfo["MediaStreamSrcPortResource"];
                                  
  arrServiceObjectInfo["MediaStreamDstPortResource"] = {cacheRoot:"MediaStreamDstPortResourceList",
                                                        nidlist:"DstPortNIDList", 
                                                        grouptreelist:"DstPortGroupTree", 
                                                        grouptagxpath:".//Info/Groups/Tag", 
                                                        successevent:{action:"APPEVENT",
                                                                      eventvar:"MediaStreamDstPortResourceDataState", 
                                                                      eventval:"DATE"},
                                                        listreqstate:null};  
  arrServiceObjectInfo["DestinationPort"] = arrServiceObjectInfo["MediaStreamDstPortResource"];
  arrServiceObjectInfo["Destination"] = arrServiceObjectInfo["MediaStreamDstPortResource"];

  arrServiceObjectInfo["MediaStreamRelayPortResource"] = {cacheRoot:"MediaStreamRelayPortResourceList",
                                                        nidlist:"MediaStreamRelayPortResourceNIDList", 
                                                        grouptreelist:"MediaStreamRelayPortResourceGroupTree", 
                                                        grouptagxpath:".//Info/Groups/Tag", 
                                                        successevent:{action:"APPEVENT",
                                                                      eventvar:"MediaStreamRelayPortResourceDataState", 
                                                                      eventval:"DATE"},
                                                        listreqstate:null};
  arrServiceObjectInfo["RelayPort"] = arrServiceObjectInfo["MediaStreamRelayPortResource"];
  

  arrServiceObjectInfo["MediaStreamIOPortResource"] = {cacheRoot:"MediaStreamIOPortResourceList",
                                                        nidlist:"MediaStreamIOPortResourceNIDList", 
                                                        grouptreelist:"MediaStreamIOPortResourceGroupTree", 
                                                        grouptagxpath:".//Info/Groups/Tag", 
                                                        successevent:{action:"APPEVENT",
                                                                      eventvar:"MediaStreamIOPortResourceDataState", 
                                                                      eventval:"DATE"},
                                                        listreqstate:null};

  arrServiceObjectInfo["MediaStorePortResource"] = {cacheRoot:"MediaStorePortResourceList",
                                                        nidlist:"MediaStorePortResourceNIDList", 
                                                        grouptreelist:"MediaStorePortResourceGroupTree", 
                                                        grouptagxpath:".//Info/Groups/Tag", 
                                                        successevent:{action:"APPEVENT",
                                                                      eventvar:"MediaStorePortResourceDataState", 
                                                                      eventval:"DATE"},
                                                        listreqstate:null};

  arrServiceObjectInfo["MediaStreamProfile"] = {cacheRoot:"MediaStreamProfileList",
                                                nidlist:"ProfileNIDList", 
                                                grouptreelist:"ProfileGroupTree", 
                                                grouptagxpath:".//Groups/Tag", 
                                                successevent:{action:"APPEVENT",
                                                              eventvar:"MediaStreamProfileDataState",
                                                              eventval:"DATE"},
                                                listreqstate:null};
  arrServiceObjectInfo["V2DMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];
  arrServiceObjectInfo["MPEGTSMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];
  arrServiceObjectInfo["RTPMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];
  arrServiceObjectInfo["UDPMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];
  arrServiceObjectInfo["RTP-RAWMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];
  arrServiceObjectInfo["RTP-PCMMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];
  arrServiceObjectInfo["RFBMediaStreamProfile"] = arrServiceObjectInfo["MediaStreamProfile"];

  arrServiceObjectInfo["RelayProfile"] = {cacheRoot:"RelayList"};
  
  arrServiceObjectInfo["CollaborationConfig"] = {cacheRoot:"CollaborationConfigList",
                                                 nidlist:"CollaborationConfigNIDList", 
                                                 grouptreelist:"CollaborationConfigGroupTree", 
                                                 grouptagxpath:".//collaborationGroup/Tag", 
                                                 successevent:{action:"APPEVENT",
                                                               eventvar:"CollaborationConfigDataState",
                                                               eventval:"DATE"},
                                                 listreqstate:null};
                                                 
  arrServiceObjectInfo["SessionConfig"] = {cacheRoot:"SessionConfigList",
                                                 nidlist:"SessionConfigNIDList", 
                                                 grouptreelist:"SessionConfigGroupTree", 
                                                 grouptagxpath:".//Groups/Tag", 
                                                 successevent:{action:"APPEVENT",
                                                               eventvar:"SessionConfigDataState",
                                                               eventval:"DATE"},
                                                 listreqstate:null};

  arrServiceObjectInfo["MediaRoom"] = {cacheRoot:"MediaRoomList",
                                       nidlist:"ProgramNIDList", 
                                       grouptreelist:null, 
                                       grouptagxpath:null, 
                                       successevent:{action:"APPEVENT",
                                                     eventvar:"ProgramDataState",
                                                     eventval:"DATE"},
                                       listreqstate:null};
                                     
  arrServiceObjectInfo["GroupTagDef"] = {cacheRoot:"GroupTagDefList",
                                         nidlist:"GroupTagNIDList", 
                                         grouptreelist:null, 
                                         grouptagxpath:null, 
                                         successevent:{action:"APPEVENT",
                                                       eventvar:"GroupTagDataState",
                                                       eventval:"DATE"},
                                         listreqstate:null};
                                         
  arrServiceObjectInfo["TagObjectFilter"] = {cacheRoot:"TagObjectFilterList",
                                             nidlist:"TagObjectFilterNIDList", 
                                             grouptreelist:null, 
                                             grouptagxpath:null, 
                                             successevent:{action:"APPEVENT",
                                                           eventvar:"TagObjectFilterDataState",
                                                           eventval:"DATE"},
                                             listreqstate:null};

  arrServiceObjectInfo["ObjectFilter"] = {cacheRoot:"ObjectFilterList",
                                           nidlist:"ObjectFilterNIDList", 
                                           grouptreelist:null, 
                                           grouptagxpath:null, 
                                           successevent:{action:"APPEVENT",
                                                         eventvar:"ObjectFilterDataState",
                                                         eventval:"DATE"},
                                           listreqstate:null};
                                                                                          
  arrServiceObjectInfo["UserAdminPolicyRule"] = {cacheRoot:"UserAdminPolicyRuleList",
                                                 nidlist:"UserAdminNIDList", 
                                                 grouptreelist:null, 
                                                 grouptagxpath:null, 
                                                 successevent:{action:"APPEVENT",
                                                               eventvar:"UserAdminDataState",
                                                               eventval:"DATE"},
                                                 listreqstate:null};
                                                 
  arrServiceObjectInfo["AssetAdminPolicyRule"] = {cacheRoot:"AssetAdminPolicyRuleList",
                                                  nidlist:"AssetAdminNIDList", 
                                                  grouptreelist:null, 
                                                  grouptagxpath:null, 
                                                  successevent:{action:"APPEVENT",
                                                                eventvar:"AssetAdminDataState",
                                                                eventval:"DATE"},
                                                  listreqstate:null};

  arrServiceObjectInfo["MediaStoreAdminPolicyRule"] = {cacheRoot:"MediaStoreAdminPolicyRuleList",
                                                        nidlist:"MediaStoreAdminNIDList", 
                                                        grouptreelist:null, 
                                                        grouptagxpath:null, 
                                                        successevent:{action:"APPEVENT",
                                                                      eventvar:"MediaStoreAdminDataState",
                                                                      eventval:"DATE"},
                                                        listreqstate:null};
                                                             
  arrServiceObjectInfo["PolicyAdminPolicyRule"] = {cacheRoot:"PolicyAdminPolicyRuleList",
                                                    nidlist:"PolicyAdminPolicyNIDList", 
                                                    grouptreelist:null, 
                                                    grouptagxpath:null, 
                                                    successevent:{action:"APPEVENT",
                                                                  eventvar:"PolicyAdminPolicyDataState",
                                                                  eventval:"DATE"},
                                                    listreqstate:null};
                                                                                                    
  arrServiceObjectInfo["MediaRoomPolicyRule"] = {cacheRoot:"MediaRoomPolicyRuleList",
                                                 nidlist:"CollabNIDList", 
                                                 grouptreelist:null, 
                                                 grouptagxpath:null, 
                                                 successevent:{action:"APPEVENT",
                                                               eventvar:"CollabDataState",
                                                               eventval:"DATE"},
                                                 listreqstate:null};

  arrServiceObjectInfo["NetworkAddressRule"] = {cacheRoot:"NetworkAddressRuleList",
                                                   nidlist:"NetworkAddressNIDList", 
                                                   grouptreelist:null, 
                                                   grouptagxpath:null, 
                                                   successevent:{action:"APPEVENT",
                                                                 eventvar:"NetworkAddressDataState",
                                                                 eventval:"DATE"},
                                                   listreqstate:null};
                                                                                                                                                   
  arrServiceObjectInfo["MediaStreamFilter"] = {cacheRoot:"MediaStreamFilterList",
                                               nidlist:"MediaStreamFilterNIDList", 
                                               grouptreelist:null, 
                                               grouptagxpath:null, 
                                               successevent:{action:"APPEVENT",
                                                             eventvar:"MediaStreamFilterDataState",
                                                             eventval:"DATE"},
                                               listreqstate:null};

  syncID = 0;
  tCache = tCache + jsx3.xml.CDF.getKey();
  tCacheDoc = topic_Create(tCache);
}

function commonCache_Final() {
  clearInterval(syncID);
  syncID = 0;
}

function cmpReqObj_Get(eventvar) {
  for( var objType in arrServiceObjectInfo ){
    var obj = arrServiceObjectInfo[objType];
    if( obj.successevent != null ) {
      if( obj.successevent.eventvar == eventvar ) {
        return obj;
      }
    }
  }
  for( var objType in arrAgentObjectInfo ){
    var obj = arrAgentObjectInfo[objType];
    if( obj.successevent != null ) {
      if( obj.successevent.eventvar == eventvar ) {
        return obj;
      }
    }
  }    
  return null;
}

//updates Startfrom and CountToFetch parameters in req
function updateReqObjPaginationParam(objInfo, ObjectCount, ListStartFrom, ListCount) {
  if (objInfo == null) return;
  objInfo.nidlisttotaldbcount = ObjectCount;
  objInfo.nidlistcurrentfrom = ListStartFrom;
  objInfo.nidlistnextfrom = parseInt(ListStartFrom) + ListCount;
  if (objInfo.nidlistnextfrom > objInfo.nidlisttotaldbcount) {
    objInfo.nidlistnextfrom = objInfo.nidlisttotaldbcount;
  }
}

function getObjectCacheListNode(ObjectType){
  if( ObjectType == null || ObjectType == "" ) {
    return topic_GetRootNode(tCache);
  }
  // lookup node xpath in map table - find node and return it
  var ObjectCacheListNodeName = null;
  var ObjectCacheListNode = arrServiceObjectInfo[ObjectType];
  if( ObjectCacheListNode != null ) {
    ObjectCacheListNodeName = ObjectCacheListNode.cacheRoot;
  }
  if( ObjectCacheListNodeName == null ) {
    log.warn("Internal Warning: Using unregistered Object Type: " + ObjectType + "\n");
    ObjectCacheListNodeName = ObjectType + "_CACHELIST";
  }
  var ObjectCacheListNodeXPath = "//" + ObjectCacheListNodeName;
  var objectCacheListNode = topic_GetNode(tCache, ObjectCacheListNodeXPath);
  // if it does not exist - make one up
  if( ! objectCacheListNode ) {
    objectCacheListNode = topic_Set(tCache, ObjectCacheListNodeName, null);
  }
  return objectCacheListNode
}

function setObjectCacheNode(objectNode) {
  objectNode = objectNode.cloneNode(true);
  var ObjectNID = objectNode.getAttribute("NID");
  if( ObjectNID != null ) {
    var objectCacheNode = topic_GetNIDNode(tCache, ObjectNID);
    if(objectCacheNode) {
      objectCacheNode.getParent().replaceNode(objectNode, objectCacheNode);
    }
    else {
      var ObjectNodeName = objectNode.getNodeName();
      var objectParentCacheNode = getObjectCacheListNode(ObjectNodeName);
      objectParentCacheNode.appendChild(objectNode);
    }
  }
  else {
    var rootCacheNode = topic_GetRootNode(tCache);
    rootCacheNode.appendChild(objectNode);
  }
}

function setObjectCacheDefaultXML(ObjectType, DefaultsXMLFile, ObjectNID) {
  var objectNode = loadXMLFile(DefaultsXMLFile); 
  if( objectNode != null ) {
    var serviceVersion = topic_Get(tApp, "ServiceVersion");
    objectNode.setAttribute("serviceVer", serviceVersion);
    return setObjectCacheDefaultNode(ObjectType, objectNode, ObjectNID);
  }
}

function setObjectCacheDefaultNode(ObjectType, objectNode, ObjectNID) {
  objectNode = objectNode.cloneNode(true);
  var objectListNode = getObjectCacheListNode(ObjectType);
  if(ObjectNID == null) {
    var ObjectNID = objectNode.getAttribute("NID");
    if(ObjectNID == null) ObjectNID = "DEFAULTNID";
  }
  else {
    objectNode.setAttribute("NID", ObjectNID);
  }
  var objectCacheNode = objectListNode.selectSingleNode(".//*[@NID='" + ObjectNID + "']");
  if(objectCacheNode) {
    objectCacheNode = objectListNode.replaceNode(objectNode, objectCacheNode);
  }
  else {
    objectCacheNode = objectListNode.appendChild(objectNode);
  }
  return objectCacheNode;
}

function getObjectCacheDefaultNode(ObjectType, ObjectNID) {
  if(ObjectNID == null) ObjectNID = "DEFAULTNID";
  var objectListNode = getObjectCacheListNode(ObjectType);
  var objectCacheNode = objectListNode.selectSingleNode(".//*[@NID='" + ObjectNID + "']");
  return objectCacheNode;
}


//function to handle reference count
function handleRefCountForCacheNID(ObjectType, NodeNID, AddDel) {
  if( NodeNID == null ) return;
  var objCacheInfo = arrServiceObjectInfo[ObjectType];
  var NIDCountRef = objCacheInfo.listnidrefcount[NodeNID];
  if( AddDel == "ADD" ) {
    if( NIDCountRef == null ) {
      NIDCountRef = 1;
    }
    else {
      NIDCountRef = parseInt(NIDCountRef) + 1;
    }
  }
  else if( AddDel == "DELETE" ) {
    if( NIDCountRef != null && NIDCountRef > 0 ) {
      NIDCountRef = parseInt(NIDCountRef) - 1;
    }
  }  
  objCacheInfo.listnidrefcount[NodeNID] = NIDCountRef;
}

//Removes bookmarks which are not required in cache
function trimBookmarkCacheNode(mediaFileNID) {
  var ObjectType = "MediaStreamFileResource";
  var objCacheInfo = arrServiceObjectInfo[ObjectType];
  var MediaFileNIDCountRef = objCacheInfo.listnidrefcount[mediaFileNID];
  if( MediaFileNIDCountRef == null || MediaFileNIDCountRef == 0) {
    var mediaBookmarkListNode = topic_GetNode(tCache, "//BookmarkList");
    if( mediaBookmarkListNode == null ) return;
    // for each bookmark
    var nodeBookmarkIter = mediaBookmarkListNode.selectNodeIterator("./*[.//MediaStreamFileResourceNID='" + mediaFileNID + "']");
    while( nodeBookmarkIter.hasNext() ) {
      var nodeBookmark = nodeBookmarkIter.next();
      mediaBookmarkListNode.removeChild(nodeBookmark);
    }
  }
}

// function to trim the MediaStreamFile
// remove bookmarks and gaplist from the cache node
function trimMediaFileCacheNode(mediaFileNID){
  var ObjectType = "MediaStreamFileResource";
  var objCacheInfo = arrServiceObjectInfo[ObjectType];
  var MediaFileNIDCountRef = objCacheInfo.listnidrefcount[mediaFileNID];
  if( MediaFileNIDCountRef == null || MediaFileNIDCountRef == 0) {
    var FileNode = topic_GetNIDNode(tCache, mediaFileNID);
    if( FileNode == null ) return;
    // get bookmark track node
    var bookmarkTrackNode = FileNode.selectSingleNode(".//BookmarkTrack");
    if (bookmarkTrackNode != null) {
      // delete all its children
      bookmarkTrackNode.removeChildren();
    }
    // get gaplist node
    var gapListNode = FileNode.selectSingleNode(".//MediaTrackIndex");
    if (gapListNode != null) {
      // delete all its children
      gapListNode.removeChildren();
    }
  }
}

//function to handle dialog reference
function getDialogDataNode(tDlg, ObjectType){
  if( ObjectType == null || ObjectType == "" ) {
    return topic_GetRootNode(tDlg);
  }
  var dialogDataNodeXPath = "//" + ObjectType;
  var dialogDataNode = topic_GetNode(tDlg, dialogDataNodeXPath);
  // if it does not exist - make one up
  if( ! dialogDataNode ) {
    ObjectType = ObjectType.replace("record/","");
    dialogDataNode = topic_Set(tDlg, ObjectType, null);
  }
  return dialogDataNode
}

function setDialogDataNode(tDlg, objectNode) {
  objectNode = objectNode.cloneNode(true);
  var ObjectNodeName = "DialogData";
  var objectParentNode = getDialogDataNode(tDlg, ObjectNodeName);
  objectParentNode.appendChild(objectNode);
}