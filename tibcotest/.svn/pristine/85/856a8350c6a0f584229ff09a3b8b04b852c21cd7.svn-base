var NODE_DEPTH = "-1";

function getRequestName(ServiceName, ObjectType, RequestOpType, RequestOpSpec) {
  var RequestName = "";
  if( ServiceName == "xmpp" ||
      ServiceName == "mediaroomservice" ) {
    if(RequestOpType != "Operation") {
      RequestName += RequestOpType;
    }
    RequestName += ObjectType;
    if(RequestOpSpec != "NONE") {
      RequestName += RequestOpSpec;
    }
    RequestName += "Request";
  }
  else if(ServiceName == "assetadminservice") {
    RequestName = "AssetManagementRequest";
  }
  else if(ServiceName == "policyadminservice") {
    RequestName = "PolicyManagementRequest";
  }
  return RequestName;
}

function getRequestXML (ServiceName, RequestName, ObjectType, RequestOpType, RequestOpSpec, requestData) {
// set request data in requestdoc for rules to use
  // set objecttype
  topic_Set(tRequest, "ObjectType", ObjectType);
  // set request op related data
  if(RequestOpType == "Get") {
    // if get-list - requestdata must be start/count array
    if(RequestOpSpec == "List") {
      topic_Set(tRequest, "Op_Select_ListSpec_StartFrom", requestData[0]);
      topic_Set(tRequest, "Op_Select_ListSpec_CountToFetch", requestData[1]);
      topic_Set(tRequest, "Op_Select_ItemSpec_NodeDepth", NODE_DEPTH);
    }
    // if get-NID - requestdata must be NID
    else if(RequestOpSpec == "NID") {
      if(requestData == "NONE") return null;
      topic_Set(tRequest, "Op_Where_NID", requestData);
    }
    else {
      return null;
    }
  }
  else if(RequestOpType == "Add") {
    var opDataNode = topic_Set(tRequest, "Op_Nodes", null);
    // if add - requestdata must be adddatanode
    if(requestData == "NONE") {
      return null;
    }
    else {
      requestData = requestData.cloneNode(true);
    }
    var requestOperationDataTagNode = opDataNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "OperationData");
    requestOperationDataTagNode.appendChild(requestData);
    opDataNode.appendChild(requestOperationDataTagNode);
    
  }
  else if(RequestOpType == "Delete") {
    // if delete-NID - requestdata must be NID
    if(RequestOpSpec == "NID") {
      if(requestData == "NONE") return null;
      topic_Set(tRequest, "Op_Where_NID", requestData);
    }
    else {
      return null;
    }
  }
  else if(RequestOpType == "Replace") {
    // if replace-NID - requestdata must be NID + replacedata
    if(RequestOpSpec == "NID") {
      if(requestData == "NONE") return null;
      var replaceNIDNode = requestData.selectSingleNode(".//ReplaceNID");
      var ReplaceNID = replaceNIDNode.getValue();
      topic_Set(tRequest, "Op_Where_NID", ReplaceNID);
      var replaceOpDataNode = replaceNIDNode.getNextSibling().cloneNode(true);
      var opDataNode = topic_Set(tRequest, "Op_Nodes", null);
      var requestOperationDataTagNode = opDataNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "OperationData");
      requestOperationDataTagNode.appendChild(replaceOpDataNode);
      opDataNode.appendChild(requestOperationDataTagNode);
    }
    else {
      return null;
    }
  }
  else if(RequestOpType == "Operation") {
    var opDataNode = topic_Set(tRequest, "Op_Nodes", null);
    // if add - requestdata must be adddatanode
    if(requestData != "NONE") {
      var requestDataNodeIter = requestData.selectNodeIterator("./*");
      while( requestDataNodeIter.hasNext() ) {
        var requestDataNode = requestDataNodeIter.next().cloneNode(true);
        opDataNode.appendChild(requestDataNode);
      }
    }
  }
  else { 
    // add code for partial-get-replace-etc.. ops here
  }
// select request data rule
  var requestDataRule = null;  
  if(ServiceName == "xmpp" || ServiceName == "mediaroomservice") {
    // select requestdatarule based on requestname
    requestDataRule = PNameSpace.loadResource(RequestName + "Data" + "_xml");
  }
  else if( (ServiceName == "assetadminservice") || (ServiceName == "policyadminservice") ) {
    // select assetadmin requestdatarule based on op-type
    requestDataRule = PNameSpace.loadResource(RequestOpType + RequestName + "Data" + "_xml");
  }

  if(requestDataRule == null) {
    return null;
  }
// generate requestdata
  requestDataRule.setOperation("");
  var requestDataDoc = requestDataRule.getServiceMessage();
  // do additonal processing for opnode if needed
  var opNode = topic_GetNode(tRequest, "//Op_Nodes");
  if( opNode != null ) {
    var opNodeIter = opNode.selectNodeIterator("./*");
    while( opNodeIter.hasNext() ) {
      var opDataNode = opNodeIter.next().cloneNode(true);
      var OpDataNodeName = opDataNode.getNodeName();
      var requestOpDataNode = requestDataDoc.selectSingleNode("//" + OpDataNodeName);
      if( requestOpDataNode ) {
        requestOpDataNode.getParent().replaceNode(opDataNode, requestOpDataNode);
      }
    }
  }
// get requestheader
  var requestHeaderRule = PNameSpace.loadResource("Request_xml");
  requestHeaderRule.setOperation("");
  var requestDoc = requestHeaderRule.getServiceMessage();
// put requestdatanode in the requestheader datanode
  var requestDataDoc = requestDoc.selectSingleNode("//data");
  var requestDataRootNode = requestDataDoc.getRootNode().cloneNode(true);
  requestDataNode.appendChild(requestDataRootNode);
// convert to XML, cleanup and return
  var RequestXML = requestDoc.toString();
  
  delete requestDataDoc;
  delete requestDoc;
  
  return RequestXML;
}
    
/*
  <Request servicename="mediaroom">
    <MediaRoomRequestData ObjectType="Source/StreamProfile/CreateConnection/AddStreamAction/MediaRoomNID/MediaRoom">
      <RequestInfo RequestOpType="Get/Add/Operation" RequestOpSpec="List/NONE">
        <OperationData RequestOpData="GetSourceListRequestData/GetStreamProfileListRequestData/CreateConnectionRequestData/AddStreamActionRequestData/GetMediaRoomNIDRequestData/GetMediaRoomRequestData""/>
      </RequestInfo>
    </MediaRoomRequestData>
  </Request>

  <Request servicename="assetadmin">
    <AssetManagementRequestData ObjectType="AssetResource/V2DMediaStreamProfile/MPEGTSMediaStreamProfile/RTPMediaStreamProfile/V2D-Tx-Device/V2D-Rx-Device/DMS-120-Device/DMS-1000-Device/IPVSClient-Device">
      <RequestInfo RequestOpType="Get/Add" RequestOpSpec="List/NID/NONE">
        <OperationData RequestOpData="NONE/ObjectXML"/>
      </RequestInfo>
    </AssetManagementRequestData>
  </Request>
*/