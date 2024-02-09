var TopicRootXML = '<TopicCacheRoot></TopicCacheRoot>';

var cacheDocNamesArr = new Array();

function topic_Exists(topicName) {
  return PNameSpace.getCache().getDocument(topicName);
}

function topic_Create(topicName) {
  var newDoc = new jsx3.xml.Document();
  newDoc.loadXML(TopicRootXML);
  PNameSpace.Cache.setDocument(topicName, newDoc);
  cacheDocNamesArr.push(topicName);
  return PNameSpace.getCache().getDocument(topicName); 
}

function topic_Destroy(topicName) {
  PNameSpace.unsubscribeAll(topicName);
  cacheDocNamesArr.splice(cacheDocNamesArr.indexOf(topicName), 1);
  var topicCacheDoc = PNameSpace.getCache().getDocument(topicName);
  if(topicCacheDoc) {
    PNameSpace.getCache().clearById(topicName);
    return true;
  }
  return false;
}

function topic_Del(topicName, Var) {
  var topicCacheDoc = PNameSpace.getCache().getDocument(topicName); 
  if (!topicCacheDoc) {
    return null;
  }
  var topicNode = topicCacheDoc.selectSingleNode("//" + Var);
  if (topicNode) {
    topicNode.getParent().removeChild(topicNode);
    return true;
  }
  return false;
}

function topic_Set(topicName, Var, Val) {
  var topicCacheDoc = PNameSpace.getCache().getDocument(topicName);
  if( ! topicCacheDoc ) {
    return null;
  }
  var topicNode = topicCacheDoc.selectSingleNode("//" + Var);
  if( ! topicNode ) {
    var topicRootNode = topicCacheDoc.getRootNode();
    var topicNode = topicRootNode.createNode(jsx3.xml.Entity.TYPEELEMENT, Var);
    topicRootNode.appendChild(topicNode);
  }
  if(Val != null) {
    topicNode.setValue(Val);
  }
  return topicNode;
}

function topic_Get(topicName, Var) {
  var topicCacheDoc;
  var topicNode;
  if( topicCacheDoc = PNameSpace.getCache().getDocument(topicName) ) {
    if( topicNode = topicCacheDoc.selectSingleNode("//" + Var) ) {
      return topicNode.getValue();
    }  
  }
  return null;
}

function topic_GetNode(topicName, NodeXPath) {
  var topicCacheDoc;
  var topicNode;
  if( topicCacheDoc = PNameSpace.getCache().getDocument(topicName) ) {
    topicNode = topicCacheDoc.selectSingleNode(NodeXPath);
    return topicNode;
  }
  return null;
}

function topic_GetNodeIterator(topicName, NodeXPath) {
  var topicCacheDoc;
  var topicNodeIter;
  if( topicCacheDoc = PNameSpace.getCache().getDocument(topicName) ) {
    topicNodeIter = topicCacheDoc.selectNodeIterator(NodeXPath);
    return topicNodeIter;
  }
  return null;
}

function topic_GetNIDNode(topicName, NID) {
  var topicCacheDoc;
  var topicNode;
  if( topicCacheDoc = PNameSpace.getCache().getDocument(topicName) ) {
    topicNode = topicCacheDoc.selectSingleNode("//*[@NID='" + NID + "']");
    return topicNode;
  }
  return null;
}

function topic_GetRootNode(topicName) {
  var topicCacheDoc;
  var topicNode;
  if( topicCacheDoc = PNameSpace.getCache().getDocument(topicName) ) {
    topicNode = topicCacheDoc.getRootNode();
    return topicNode;
  }
  return null;
}

function topic_Subscribe(topicName, cmpEventHandler, cmpEventHandlerFnc) {
  if( ! cmpEventHandlerFnc ) {
    PNameSpace.subscribe(topicName, eval(cmpEventHandler));
  }
  else {
    PNameSpace.subscribe(topicName, cmpEventHandler, cmpEventHandlerFnc);
  }
  if(! topic_Exists(topicName) ) {
    return false;
  }
  else {
    return true;
  }
}

function topic_UnSubscribe(topicName, cmpEventHandler) {
  PNameSpace.unsubscribe(topicName, eval(cmpEventHandler));
  if(! topic_Exists(topicName) ) {
    return false;
  }
  else {
    return true;
  }
}

function topic_Publish(topicName, stateVar, stateVal) {
  log.debug("EVENT: PUB: Topic: " + topicName + " Var: " + stateVar + " Val: " + stateVal);
  topic_Set(topicName, stateVar, stateVal);
  var objTopic = new Object();
  objTopic.subject = topicName;
  objTopic.stateVar = stateVar;
  objTopic.stateVal = stateVal;
  PNameSpace.publish(objTopic);
  if(! topic_Exists(topicName) ) {
    return false;
  }
  else {
    return true;
  }
}


//end of Topic Functions
