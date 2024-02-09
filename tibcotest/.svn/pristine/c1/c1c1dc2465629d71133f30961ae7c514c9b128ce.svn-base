var tDSA = "DestStreamAgent";
var tDSADoc = null;

// HACK: This should live in a network section in device.xml 
// - current Device.xml does not have a network section
// Put it here as a global till that is fixed
var DeviceIpAddress = null;

var DefaultPortID = "Port1";
var DefaultDeviceIpAddress = "127.0.0.1";
var DefaultMPEGTSRxMediaPortRangeFrom = "2000";
var DefaultRTPRxMediaPortRangeFrom = "4000";

// destroy topic
function commonDSA_Destroy() {
  topic_Destroy(tDSA);
  tDSADoc = null;
}

function commonDSA_Init() {
  // create DSA specific topic/doc
  tDSADoc = topic_Create(tDSA);
  dsaRootNode = topic_GetRootNode(tDSA);
  var xmlLoadDir = null;
  var NAMESPACE = PNameSpace.getEnv("NAMESPACE");
  if (clientType == "WEB") {
    xmlLoadDir = giStartPath + "/JSXAPPS/IPVS-Common/model/objectXml/";
  }
  else {
    if (NAMESPACE == "STXClientUI") {
      xmlLoadDir = "../IPVS-Common/model/objectXml/";
    } else {
      xmlLoadDir = "JSXAPPS/IPVS-Common/model/objectXml/"; 
    }
  }
  
  // Init Device Instance List Node - this should get a handle to the instance list node in the Device struct
  // for now just load a file - enough to hold the stream and port instance list
  var deviceNode = loadXMLFile(xmlLoadDir+"IPVSClient-Device.xml");
  // init the Port ID
  deviceNode.selectSingleNode("//IPVSClient-Device//AVCodecElementsStatus/IPVSClientPortStatus/PortID").setValue(DefaultPortID);
  // Init the local IP Address and port ranges
  DeviceIpAddress = topic_Get(tApp, "ClientIPAddress");
  if( DeviceIpAddress == null ) {
    DeviceIpAddress = DefaultDeviceIpAddress;
  }
  deviceNode.selectSingleNode("//IPVSClient-Device//StreamingServiceConfig//MPEGTSStreamsRxConfig/RxMediaPortRange/From").setValue(DefaultMPEGTSRxMediaPortRangeFrom);
  deviceNode.selectSingleNode("//IPVSClient-Device//StreamingServiceConfig//RTPStreamsRxConfig/RxMediaPortRange/From").setValue(DefaultRTPRxMediaPortRangeFrom);
  // add the device node into the dsa topic
  dsaRootNode.appendChild(deviceNode);
  
  // Init (all) Resource Node(s) - this should get a handle to the instance list node 
  //                             - in the MediaStreamDstPortResource node at DestNID(s)
  // for now - just load a file - enough for one resource with an instance list node
  var resourceNode = loadXMLFile(xmlLoadDir+"MediaStreamDstPortResource.xml");
  // add the resource node into the dsa topic
  dsaRootNode.appendChild(resourceNode);
}

function commonDSA_Final() {
  topic_Destroy(tDSA);
  tDSADoc = null;
}

function dsaPortGet(DestNID) {
  // return the portID from the MediaStreamDstPortResource node at DestNID
  // for now just make one up
  var PortID = DefaultPortID;
  
  return PortID;
}
function dsaPortInstanceAdd(PortID, PortInstanceID) {
  // get the port instance list node
  var nodeInstanceInfo = topic_GetNode(tDSA, ".//IPVSClient-Device//AVCodecElementsStatus/IPVSClientPortStatus[PortID='" + PortID + "']/InstanceInfo");
  
  // get a template port instance node
  var nodePortInstance = nodeInstanceInfo.selectSingleNode(".//PortInstance[ID='']").cloneNode(true);
  
  // set a new instance ID if not provided - should not happen
  if( PortInstanceID == null ) PortInstanceID = jsx3.xml.CDF.getKey();
  nodePortInstance.selectSingleNode("./ID").setValue(PortInstanceID);
  
  nodeInstanceInfo.appendChild(nodePortInstance);
  
  return PortInstanceID;
}

function dsaPortInstanceDelete(PortInstanceID) {
  // get the matching port instance node
  var nodePortInstance = topic_GetNode(tDSA, ".//IPVSClient-Device//AVCodecElementsStatus/IPVSClientPortStatus/InstanceInfo/PortInstance[ID='" + PortInstanceID + "']");
  nodePortInstance.getParent().removeChild(nodePortInstance);
}

function dsaResourceInstanceAdd(PortInstanceID, StreamType) {
  if( StreamType == "" ) {
    StreamType = "V2D";
    log.warn("DSA: ResourceInstanceAdd: Null StreamType");
  }
  else {
    log.debug("DSA: ResourceInstanceAdd: StreamType: " + StreamType);
  }
  // use the portinstance id as the QID and derive SSInstanceIDs from it
  var QID = PortInstanceID;
  var Rx_SSInstanceID = QID + "_Rx";
  var Decode_SSInstanceID = QID + "_Decode"; 
  // get a template stream Rx and Decoder port instance node
  var nodeStreamInstanceList = topic_GetNode(tDSA, ".//IPVSClient-Device//StreamingServiceStatus/InstanceList");
  var cntStreamRxInstance = 0;
  var RxMediaRxMediaPortRangeFrom = 10000;
  var RxMediaPort = 10000;
  var nodeStreamRxInstance = null;
  var nodeStreamDecoderInstance = null;
  var deviceNode = topic_GetNode(tDSA, ".//IPVSClient-Device");
  if( StreamType == "V2D" ) {
    cntStreamRxInstance = nodeStreamInstanceList.selectNodes(".//V2DStreamRxInstance").size();
    RxMediaRxMediaPortRangeFrom = deviceNode.selectSingleNode("//IPVSClient-Device//StreamingServiceConfig//V2DStreamsRxConfig/RxMediaPortRange/From").getValue();
    RxMediaPort = parseInt(RxMediaRxMediaPortRangeFrom) +  2 * parseInt(cntStreamRxInstance);
    nodeStreamRxInstance = nodeStreamInstanceList.selectSingleNode(".//V2DStreamRxInstance[QID='']").cloneNode(true);
    nodeStreamDecoderInstance = topic_GetNode(tDSA, ".//V2DDecoderInstance[QID='']").cloneNode(true);
  }
  else if( StreamType == "MPEGTS" || StreamType == "UDP") {
    cntStreamRxInstance = nodeStreamInstanceList.selectNodes(".//MPEGTSStreamRxInstance").size();
    RxMediaRxMediaPortRangeFrom = deviceNode.selectSingleNode("//IPVSClient-Device//StreamingServiceConfig//MPEGTSStreamsRxConfig/RxMediaPortRange/From").getValue();
    RxMediaPort = parseInt(RxMediaRxMediaPortRangeFrom) +  parseInt(cntStreamRxInstance);
    nodeStreamRxInstance = nodeStreamInstanceList.selectSingleNode(".//MPEGTSStreamRxInstance[QID='']").cloneNode(true);
    nodeStreamDecoderInstance = topic_GetNode(tDSA, ".//MPEGDecoderInstance[QID='']").cloneNode(true);
  }
  else if( StreamType == "RTP" ) {
    cntStreamRxInstance = nodeStreamInstanceList.selectNodes(".//RTPStreamRxInstance").size();
    RxMediaRxMediaPortRangeFrom = deviceNode.selectSingleNode("//IPVSClient-Device//StreamingServiceConfig//RTPStreamsRxConfig/RxMediaPortRange/From").getValue();
    RxMediaPort = parseInt(RxMediaRxMediaPortRangeFrom) +  (4 * parseInt(cntStreamRxInstance));
    nodeStreamRxInstance = nodeStreamInstanceList.selectSingleNode(".//RTPStreamRxInstance[QID='']").cloneNode(true);
    nodeStreamDecoderInstance = topic_GetNode(tDSA, ".//MPEGDecoderInstance[QID='']").cloneNode(true);
  }
  if( nodeStreamRxInstance == null || nodeStreamDecoderInstance == null ) {
    log.error("DSA: ResourceInstanceAdd: NULL StreamInstance");
    return null;
  }
  // set the QID in them
  nodeStreamRxInstance.selectSingleNode(".//QID").setValue(QID);
  nodeStreamDecoderInstance.selectSingleNode(".//QID").setValue(QID);
  // set the SSInstanceID
  nodeStreamRxInstance.selectSingleNode(".//SSInstanceID").setValue(Rx_SSInstanceID);
  nodeStreamDecoderInstance.selectSingleNode(".//SSInstanceID").setValue(Decode_SSInstanceID);
  // setup the Rx IPAddress and Port
  nodeStreamRxInstance.selectSingleNode(".//RxMediaIpAddress").setValue(DeviceIpAddress);
  nodeStreamRxInstance.selectSingleNode(".//RxMediaPort").setValue(RxMediaPort);
  // add them to device stream instance list
  nodeStreamInstanceList.appendChild(nodeStreamRxInstance);
  nodeStreamInstanceList.appendChild(nodeStreamDecoderInstance);
  // now get a template resource instance
  var nodeResourceInstanceList = topic_GetNode(tDSA, ".//MediaStreamDstPortResource//InstanceList");
  // HACK: resourceinstances dont have an ID in them - use the resourceNID field for now.
  var nodeResourceInstance = nodeResourceInstanceList.selectSingleNode(".//Instance[ResourceNID='']").cloneNode(true);
  nodeResourceInstance.selectSingleNode(".//ResourceNID").setValue(PortInstanceID);
  // set the ss stream decoder instance in it
  nodeResourceInstance.selectSingleNode(".//DeviceStreamingServiceReceiveInstanceNID").setValue(Rx_SSInstanceID);
  nodeResourceInstance.selectSingleNode(".//DeviceStreamingServiceDecoderInstanceNID").setValue(Decode_SSInstanceID);
  // and add this instance to the resource instance list
  nodeResourceInstanceList.appendChild(nodeResourceInstance);
  
  return nodeResourceInstance;
}
/* StreamNID no longer lives in ResourceInstance
function dsaResourceInstanceBind(nodeResourceInstance, StreamNID) {
  log.debug("DSA: ResourceInstanceBind: StreamNID: " + StreamNID);
  // set the streamnid in the resource instance
  nodeResourceInstance.selectSingleNode(".//StreamNID").setValue(StreamNID);
}
*/
function dsaResourceInstanceGetRxMediaIpPort(nodeResourceInstance) {
  log.debug("DSA: ResourceInstanceDelete");
  // get the stream service Rx instance from the resource instance
  var Rx_SSInstanceID = nodeResourceInstance.selectSingleNode("./DeviceStreamingServiceReceiveInstanceNID").getValue();
  var nodeStreamRxInstance = topic_GetNode(tDSA, ".//IPVSClient-Device//StreamingServiceStatus/InstanceList/*[SSInstanceID='" + Rx_SSInstanceID + "']");
  // get the Rx IPAddress and Port
  var objRxMediaIpPort = new Object;
  objRxMediaIpPort.IpAddress = nodeStreamRxInstance.selectSingleNode(".//RxMediaIpAddress").getValue();
  objRxMediaIpPort.Port = nodeStreamRxInstance.selectSingleNode(".//RxMediaPort").getValue();
  
  return objRxMediaIpPort;
}
function dsaResourceInstanceSetPlayerURL(nodeResourceInstance, StreamURL) {
  log.debug("DSA: SetPlayerURL: StreamURL: " + StreamURL);
  // set the Player URL in the resource instance node
  nodeResourceInstance.selectSingleNode(".//MediaStreamURL").setValue(StreamURL);
}

function dsaResourceInstanceGetPlayerURL(nodeResourceInstance) {
  // ret the Player URL from the resource instance node
  var StreamURL = nodeResourceInstance.selectSingleNode(".//MediaStreamURL").getValue(); 
  log.debug("DSA: GetPlayerURL: StreamURL: " + StreamURL);
  
  return StreamURL;
}
function dsaResourceInstanceDelete(nodeResourceInstance) {
  log.debug("DSA: ResourceInstanceDelete");
  // get the stream service Rx instance from the resource instance
  var Rx_SSInstanceID = nodeResourceInstance.selectSingleNode("./DeviceStreamingServiceReceiveInstanceNID").getValue();
  var nodeStreamRxInstance = topic_GetNode(tDSA, ".//IPVSClient-Device//StreamingServiceStatus/InstanceList/*[SSInstanceID='" + Rx_SSInstanceID + "']");
  // get the stream service decoder instance from the resource instance
  var Decode_SSInstanceID = nodeResourceInstance.selectSingleNode("./DeviceStreamingServiceDecoderInstanceNID").getValue();
  var nodeStreamDecoderInstance = topic_GetNode(tDSA, ".//IPVSClient-Device//StreamingServiceStatus/InstanceList/*[SSInstanceID='" + Decode_SSInstanceID + "']");
  // delete the device stream instances
  nodeStreamRxInstance.getParent().removeChild(nodeStreamRxInstance);
  nodeStreamDecoderInstance.getParent().removeChild(nodeStreamDecoderInstance);
  // delete the resource instance
  nodeResourceInstance.getParent().removeChild(nodeResourceInstance);
}


