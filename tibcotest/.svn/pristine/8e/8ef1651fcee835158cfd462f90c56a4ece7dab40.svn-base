var arrAgentObjectInfo = null;
var arrAgentTopicList = null;
var arrReqInterval = null;

function commonAgentCache_Init() {
  arrReqInterval = new Object();
  arrAgentTopicList = new Object();
  arrAgentObjectInfo = new Object();
  
  arrAgentObjectInfo["DeviceStatus_Full"] = {cacheRoot:"DeviceStatus",
                                             reqName:"ReqGetAgentObject",
                                             reqServiceName:"systemservice",
                                             reqFromAgentName:"com.ipvs.common.service.DeviceAdminClientAgent",
                                             reqAgentName:"com.ipvs.common.service.DeviceAdminServerAgent",
                                             reqObjectType:"GetDeviceStatus",         
					     // Shanker 01/15/09 Request the entire status
                                             reqParams:{"RequestXPath":""},
                                             //successeventvar:"DeviceStatus_Full",
                                             successevent:{action:"TOPICEVENT",
                                                           eventvar:"DeviceStatus_Full",
                                                           eventval:"DATE"},
                                             reqstate:null};
                                             
  arrAgentObjectInfo["SysMonDeviceStatus"] = {cacheRoot:"DeviceStatus",
                                             reqName:"ReqGetAgentObject",
                                             reqServiceName:"systemservice",
                                             reqFromAgentName:"com.ipvs.common.service.DeviceAdminClientAgent",
                                             reqAgentName:"com.ipvs.common.service.DeviceAdminServerAgent",
                                             reqObjectType:"GetDeviceStatus",         
					     // Shanker 01/15/09 Request the entire status
                                             reqParams:{"RequestXPath":""},
                                             //successeventvar:"DeviceStatus_Full",
                                             successevent:{action:"TOPICEVENT",
                                                           eventvar:"SysMonDeviceStatus",
                                                           eventval:"DATE"},
                                             reqstate:null};                                             
                                             
  arrAgentObjectInfo["MediaRoomList"] = {cacheRoot:"MediaRoomListGetResponse",
                                         reqNidlist:"ProgramNIDList", 
                                         reqGrouptreelist:null, 
                                         reqGrouptagxpath:null,
                                         reqName:"ReqGetAgentObject",
                                         reqServiceName:"mediaroomservice",
                                         reqAgentJID:"AppServerAgentJID",
                                         reqFromAgentName:"com.ipvs.client.MRClientAgent",
                                         reqAgentName:"com.ipvs.mediaroomservice.impl.AppServerAgent",
                                         reqObjectType:"GetMediaRoomList",         
                                         reqParams:null,
                                         successevent:{action:"TOPICEVENT",
                                                       eventvar:"MediaRoomList",
                                                       eventval:"DATE"},
                                         reqstate:null}; 
                                         
}     

function commonAgentCache_Final() {

  arrAgentObjectInfo = null;
  
  //delete all the tpoics from arrAgentTopicList
  
  for( var AgentObj in arrAgentTopicList ) {
    for( var AgentJID in arrAgentTopicList[AgentObj] ) {
      topic_Destroy(arrAgentTopicList[AgentObj][AgentJID]);
      delete arrAgentTopicList[AgentObj][AgentJID];
    }
  }
  arrAgentTopicList = null;
  
  // clear arrReqInterval for all id's
  for( var i=0; i<arrReqInterval.length; i++) {
    var intervalID = arrReqInterval[i].IntervalID;
    clearInterval(intervalID);
    arrReqInterval[i].IntervalID = null;
    arrReqInterval[i] = null;    
  }
  arrReqInterval = null;  
}   

//-------------------
function setAgentReqInterval(Interval, AgentJID, AgentObj, objStatus) {
  // check if new interval is to be started
  var intervalID = null;
  for( var i=0; i<arrReqInterval.length; i++) {
    if( (arrReqInterval[i].AgentJID == AgentJID) &&
        (arrReqInterval[i].AgentObj == AgentObj) &&
        (arrReqInterval[i].Interval <= Interval)  ) {
      intervalID = arrReqInterval[i].IntervalID;
      break;
    }
  }
  if( intervalID == null ) {
    // if Interval is 0 zero then using setTimeout otherwise setInterval. 
    // SetTimeout will issue one time request
    if( Interval == "0") {
      intervalID = setTimeout("sendAgentObjRequest("+"\""+AgentJID+"\""+","+"\""+AgentObj+"\""+","+"\""+objStatus+"\""+")", Interval);
    } else {
      intervalID = setInterval("sendAgentObjRequest("+"\""+AgentJID+"\""+","+"\""+AgentObj+"\""+","+"\""+objStatus+"\""+")", Interval);
    }
  }
  var ReqIntervalID = jsx3.xml.CDF.getKey();
  arrReqInterval[ReqIntervalID] = new Object;
  arrReqInterval[ReqIntervalID].AgentJID = AgentJID;
  arrReqInterval[ReqIntervalID].AgentObj = AgentObj;
  arrReqInterval[ReqIntervalID].Interval = Interval;
  arrReqInterval[ReqIntervalID].IntervalID = intervalID;
  
  return ReqIntervalID;
}

//-------------------
function clearAgentReqInterval(ReqIntervalID) {
  try {
    var intervalID = arrReqInterval[ReqIntervalID].IntervalID;
     
    if( intervalID != null ) {
      clearInterval(intervalID);
      delete arrReqInterval[ReqIntervalID];
    }
  } catch(e) {
  }
}

//-------------------
function clearAgentTopic(AgentJID, AgentObj) {
 try{
   topic_Destroy(arrAgentTopicList[AgentObj][AgentJID]);
   delete arrAgentTopicList[AgentObj][AgentJID];
 } catch(e) {
 }
}

//-------------------
function clearAgentAllReqInterval(AgentJID, AgentObj) {
  for( var ReqIntervalObj in arrReqInterval ) {
    try {
      var agentJID = arrReqInterval[ReqIntervalObj].AgentJID;
      var agentObj = arrReqInterval[ReqIntervalObj].AgentObj;
      if( (AgentJID == agentJID) && (AgentObj ==agentObj) ) {
        intervalID = arrReqInterval[ReqIntervalObj].IntervalID;
        clearInterval(intervalID);
        arrReqInterval[ReqIntervalObj].IntervalID = null;
        delete arrReqInterval[ReqIntervalObj];
      }
    } catch(e) {
    } 
  }
}

function clearAgentObjRequest(AgentJID, AgentObj) {
  clearAgentTopic(AgentJID, AgentObj);
  clearAgentAllReqInterval(AgentJID, AgentObj);

  try {
    arrAgentObjectInfo[AgentObj][AgentJID].req = null;
    delete arrAgentObjectInfo[AgentObj][AgentJID];
  } catch(e) {
  }
}

function sendAgentObjRequest(AgentJID, AgentObj, objStatus) {
  var agentObj = arrAgentObjectInfo[AgentObj];
  if(agentObj[AgentJID] == null) {
    agentObj[AgentJID] = new Object;
    agentObj[AgentJID].req = eval("new "+agentObj.reqName+"('"+AgentJID+"', agentObj)");
  }
  
  if(arrAgentTopicList[AgentObj] == null) {
    arrAgentTopicList[AgentObj] = new Object;
  }
  if( arrAgentTopicList[AgentObj][AgentJID] == null) {
    arrAgentTopicList[AgentObj][AgentJID] = topic_Create(AgentJID);
  }
  //checking asset status, if "ready" then send request
  // this will prevent from sending request to offline devices/assets
  
  if(objStatus == "Ready") {
    var req = arrAgentObjectInfo[AgentObj][AgentJID].req;
    req.sendRequest(this);
  }
  else {
    log.error(" 'sendAgentObjRequest' Asset not READY AgentJID: " + AgentJID);
  }
}

                          
