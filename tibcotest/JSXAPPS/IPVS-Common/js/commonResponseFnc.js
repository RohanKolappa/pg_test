/*
  common response handler
  takes the response and updates the required cache/request-specific topic locations
*/
function OLD_handleResponse(TopicName) {
  var responseDoc = topic_GetNode(TopicName, "ResponseNode");
// check the state in the response - if 0 - simple pass the raw data thru to the caller
  var ResponseHeaderState = responseDoc.selectSingleNode("//header/state").getValue();
  topic_Set(TopicName, "ResponseHeaderState", ResponseHeaderState);
  if( ResponseHeaderState == 0 ) {
    var responseNode = responseDoc.selectSingleNode("//Response").cloneNode(true);
    var badResponseNode = topic_Set(TopicName, "BadResponse");
	  badResponseNode.appendChild(responseNode);
    return true;
  }
// get the original requestname - and handle the response
  var ResponseRequestName = topic_Get(TopicName, "RequestName");
  switch (ResponseRequestName) {
    case "LoginRequest":
      var UserJID = responseDoc.selectSingleNode("//header/userJID").getValue();
      topic_Set(TopicName, "UserJID", UserJID);
      break;
    case "SetServiceDomainRequest":
    case "CreateConnectionRequest":
    case "AddStreamActionRequest":
    case "DeleteMediaRoomRequest":
	    var responseNode = responseDoc.selectSingleNode("//data/*").cloneNode(true);
	    handleGenericResponse(TopicName, responseNode);
	    break;
    case "GetSourceListRequest":
      var elementParentCacheNode = getObjectCacheListNode("Source");
      var responseNode = responseDoc.selectSingleNode("//data//GetResponse");
      handleGetResponse(TopicName, responseNode, elementParentCacheNode);
      break;
    case "GetStreamProfileListRequest":
      var elementParentCacheNode = getObjectCacheListNode("StreamProfile");
      var responseNode = responseDoc.selectSingleNode("//data//GetResponse");
      handleGetResponse(TopicName, responseNode, elementParentCacheNode);
      break;
	case "PolicyManagementRequest":
    case "AssetManagementRequest":
      // get parent cachenode based on objecttype
      var ObjectType = responseDoc.selectSingleNode("//data//ObjectType[1]").getValue();
	  var elementParentCacheNode = getObjectCacheListNode(ObjectType);
      if(! elementParentCacheNode) {
        return false;
      }
      // process response based on responsetype
      if( responseNode = responseDoc.selectSingleNode("//data//GetResponse") ) {
        handleGetResponse(TopicName, responseNode, elementParentCacheNode);
      }
      else if( responseNode = responseDoc.selectSingleNode("//data//AddResponse") ) {
        handleAddResponse(TopicName, responseNode, elementParentCacheNode);
      }
      else if( responseNode = responseDoc.selectSingleNode("//data//DeleteResponse") ) {
        handleDeleteResponse(TopicName, responseNode, elementParentCacheNode);
      }
      else if( responseNode = responseDoc.selectSingleNode("//data//ReplaceResponse") ) {
        handleUpdateResponse(TopicName, responseNode, elementParentCacheNode);
      }
      break;
    default:
      break;
  }
  return true;
}
/*
  copy elements returned to the request-specific topic cache
*/
function handleGenericResponse(TopicName, responseNode) {
// for each element in response - add it to requestspecific topiccache
  var topicResponseDataNode = topic_Set(TopicName, "GenericResponseData", null);
  var elementIter = responseNode.selectNodeIterator("./*");
  while(elementIter.hasNext()) {
    var elementNode = elementIter.next().cloneNode(true);
    topicResponseDataNode.appendChild(elementNode);
  }
}
/*
  update the cache with the elements returned
  push the list of NIDs in the request-specific topic cache
  push any non-NID type return elements in topiccache as well
*/
function handleGetResponse(TopicName, responseNode, elementParentCacheNode) {
  if(responseNode == null) {
    cmpMask_hide();
  }
// create root element for NIDlist in request-specific topicdoc
  var objectNIDListNode = topic_Set(TopicName, "ResponseObjectNIDList", null);
// for each element in response - add it to cache and add its NID to requestspecific topiccache  
  var elementIter = responseNode.selectNodeIterator(".//ResponseXML/*");
  while(elementIter.hasNext()) {
    var elementNode = elementIter.next().cloneNode(true);
    var ElementNID = elementNode.getAttribute("NID");
   if( ! ElementNID ) {
      var topicRootNode = topic_GetRootNode(TopicName);
      topicRootNode.appendChild(elementNode);
    }
    else {
      var elementCacheNode = topic_GetNIDNode(tCache, ElementNID);
      if(elementCacheNode) {
        elementParentCacheNode.replaceNode(elementNode, elementCacheNode);
      }
      else {
        elementParentCacheNode.appendChild(elementNode);
      }
      var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
      objectNIDListNode.appendChild(objectNIDNode);
    }
  }
}
/*
  ideally the add response should not touch the cache
  instead update notifications from the DB should make that happen
  for now we cheat and treat the Add request as if it was followed by an automatic Get request
*/
function handleAddResponse(TopicName, responseNode, elementParentCacheNode) {
  handleGetResponse(TopicName, responseNode, elementParentCacheNode);
}
/*
  delete the elements from the cache
  push the list of deleted NIDs in the request-specific topic
*/
function handleDeleteResponse(TopicName, responseNode, elementParentCacheNode) {
// create root element for NIDlist in request-specific topicdoc
  var objectNIDListNode = topic_Set(TopicName, "ResponseObjectNIDList", null);
// for each element in response - add it to cahce and add its NID to requestspecific topiccache  
  var elementIter = responseNode.selectNodeIterator(".//ResponseXML/*");
  while(elementIter.hasNext()) {
    var elementNode = elementIter.next().cloneNode(true);
    var ElementNID = elementNode.getAttribute("NID");
    if( ! ElementNID ) {
      var topicRootNode = topic_GetRootNode(TopicName);
      topicRootNode.appendChild(elementNode);
    }
    else {
      var elementCacheNode = topic_GetNIDNode(tCache, ElementNID);
      if(! elementCacheNode) {
        return false;
      }
      elementParentCacheNode.removeChild(elementCacheNode);
      var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
      objectNIDListNode.appendChild(objectNIDNode);
    }
  }
}
/*
  update the cache with the elements returned
  push the list of NIDs in the request-specific topic cache
*/
function handleUpdateResponse(TopicName, responseNode, elementParentCacheNode) {
// create root element for NIDlist in request-specific topicdoc
  var objectNIDListNode = topic_Set(TopicName, "ResponseObjectNIDList", null);
// for each element in response - add it to cahce and add its NID to requestspecific topiccache  
  var elementIter = responseNode.selectNodeIterator(".//ResponseXML/*");
  while(elementIter.hasNext()) {
    var elementNode = elementIter.next().cloneNode(true);
    var ElementNID = elementNode.getAttribute("NID");
    if( ! ElementNID ) {
      var topicRootNode = topic_GetRootNode(TopicName);
      topicRootNode.appendChild(elementNode);
    }
    else {
      var elementCacheNode = topic_GetNIDNode(tCache, ElementNID);
      if(! elementCacheNode) {
        return false;
      }
      elementParentCacheNode.replaceNode(elementNode, elementCacheNode);
      var objectNIDNode = objectNIDListNode.createNode(jsx3.xml.Entity.TYPEELEMENT, "ObjectNID").setValue(ElementNID);
      objectNIDListNode.appendChild(objectNIDNode);
    }
  }
}
