if (window.com && com.ipvs.cmp)
  delete com.ipvs.api.Session;

jsx3.require("jsx3.xml.Document");
/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Mixin interface that provides methods for accessing and manipulating a document in Session schema.
 * <p/>
 * This interface is modeled on the lines of the CDF interface provided/used by GI
 */
 jsx3.Class.defineInterface("com.ipvs.api.Session", com.ipvs.api.SessionInfo,function(Session, Session_prototype) {
  
  /** @private @jsxobf-clobber */
  Session._LOG = jsx3.util.Logger.getLogger(Session.jsxclass.getName());

  /**
   * Gets the Session Name
   * @return {String}
   */
  Session_prototype.getName = function() {
    return this.getAttribute("name");
  }
  /*
   * Sets the Session Name
   * @param strName {String}
   */
  Session_prototype.setName = function(strName) {
    this.setAttribute("name", strName);
  }
  /*
   * Gets the ID
   * @return {String}
   */
  Session_prototype.getID = function() {
    return this.getAttribute("id");
  }   
  /*
   * Sets the ID
   * @param ID {String}
   */
  Session_prototype.setID = function(ID) {
    this.setAttribute("id", ID);
  }  
  /*
   * Gets the timelineId
   * @return {String}
   */
  Session_prototype.getTimelineId = function() {
    return this.getAttribute("timelineId");
  }   
  /*
   * Sets the timelineId
   * @param ID {String}
   */
  Session_prototype.setTimelineId = function(ID) {
    this.setAttribute("timelineId", ID);
  }   
  /*
   * Gets the recordTimelineId
   * @return {String}
   */
  Session_prototype.getRecordTimelineId = function() {
    return this.getAttribute("recordTimelineId");
  }   
  /*
   * Sets the recordTimelineId
   * @param ID {String}
   */
  Session_prototype.setRecordTimelineId = function(ID) {
    this.setAttribute("recordTimelineId", ID);
  }   
  /*
   * Gets the sessionConfigNID
   * @return {String}
   */
  Session_prototype.getsessionConfigNID = function() {
    return this.selectSingleNode("./Properties/@sessionConfigNID").getValue();
  }   
  /*
   * Sets the sessionConfigNID
   * @param sessionConfigNID {String}
   */
  Session_prototype.setsessionConfigNID = function(sessionConfigNID) {
    this.selectSingleNode("./Properties/@sessionConfigNID").setValue(sessionConfigNID);
  }
  /*
  * Gets the session type for task 19599
  */
  Session_prototype.getSessionTypeAttribute = function() {
    return this.getAttribute("type");
  }

  /*
  * Get last Action from the timeline specified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineRequestedAction = function (timelineId) {
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get the attr from the Last element
    var nodeLast = nodeTimeline.selectSingleNode(".//Last");
    return nodeLast.getAttribute("requestedAction");
  }

  /*
  * Get time scale from the timeline specified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineTimeScale = function (timelineId) {
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get the attr from the Last element
    var nodeLast = nodeTimeline.selectSingleNode(".//Last");
    return nodeLast.getAttribute("TimeScale");
  }

  /*
  * Get start offset from the timeline specified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineStartOffset = function (timelineId) {
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get the attr from the Start element
    var nodeStart = nodeTimeline.selectSingleNode(".//Start");
    return nodeStart.getAttribute("Offset");
  }

  /*
  * Get start wc from the timeline specified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineStartWC = function (timelineId) {
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get the attr from the Start element
    var nodeStart = nodeTimeline.selectSingleNode(".//Start");
    return nodeStart.getAttribute("WC");
  }

  /*
  * Get last offset from the timeline specified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineLastOffset = function (timelineId) {
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get the attr from the Last element
    var nodeLast = nodeTimeline.selectSingleNode(".//Last");
    return nodeLast.getAttribute("Offset");
  }

  /*
  * Get last wc from the timeline specified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineLastWC = function (timelineId) {
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get the attr from the Last element
    var nodeLast = nodeTimeline.selectSingleNode(".//Last");
    return nodeLast.getAttribute("WC");
  }

  /*
  * Get if timeshifted from the timeline speficified
  * @param {String} timelineId
  * @return {String}
  */
  Session_prototype.getTimelineIsTimeshifted = function (timelineId) {
    var bIsTimeshifted = false;
    // get the timeline node
    var nodeTimeline = this.getTimeline(timelineId);
    // get start info
    var nodeStart = nodeTimeline.selectSingleNode(".//Start");
    var startOffset = nodeStart.getAttribute("Offset");
    var startWC = nodeStart.getAttribute("WC");
    // get last info
    var nodeLast = nodeTimeline.selectSingleNode(".//Last");
    var lastOffset = nodeLast.getAttribute("Offset");
    var lastWC = nodeLast.getAttribute("WC");
    // if the lastwc - lastoffset != startwc - startOffset - then time shifted     
    if ((lastWC - lastOffset) != (startWC - startOffset)) {
      bIsTimeshifted = true;
    }
    return bIsTimeshifted;
  }

  /*
  * gets an timeline from the Session
  * @param {String} timelineId
  * @return {Entity} nodeTimeline
  */
  Session_prototype.getTimeline = function (timelineId) {
    return this.selectSingleNode(".//Timeline[@id='" + timelineId + "']");
  }   

  /*
   * Internal utility function to get new empty Session
   * @return {jsx3.xml.Entity} nodeSession
   */
  Session._getNewSession = function() {
    // get the template from the cache OR model/objectXml
    var nodeSession = Session._getSessionObjectXML();
    
    // clean out id,
    nodeSession.setAttribute("id", "");
    // clean out timelineId,
    nodeSession.setAttribute("timelineId", "");
    // clean out recordTimelineId,
    nodeSession.setAttribute("recordTimelineId", "");        
    // clean out all TimelineList
    var nodeTimelineList = nodeSession.selectSingleNode(".//TimelineList");
    nodeTimelineList.removeChildren();    
    // clean out all dirs
    var nodeDirList = nodeSession.selectSingleNode(".//DirList");
    nodeDirList.removeChildren();
    // clean out all grpfiles
    var nodeGrpFileList = nodeSession.selectSingleNode(".//GrpFileList");
    nodeGrpFileList.removeChildren();
    // clean out all entities
    var nodeEntityList = nodeSession.selectSingleNode(".//EntityList");
    nodeEntityList.removeChildren();
    // clean out all profiles
    var nodeProfileList = nodeSession.selectSingleNode(".//ProfileList");
    nodeProfileList.removeChildren();
    // clean out all connections
    var nodeConnList = nodeSession.selectSingleNode(".//ConnectionList");
    nodeConnList.removeChildren();

    return nodeSession;
  }
  
  /*
   * Internal utility function to get get a template Session
   * Looks in the cache first - if not found inits it from model/objectXml
   * @return {jsx3.xml.Entity} nodeSession
   */
  Session._getSessionObjectXML = function() {
    // get template from cache
    var ObjectID = "DEFAULT_SESSION_ID";    
    var nodeSession = getObjectCacheDefaultNode("Session", ObjectID);
    if( nodeSession == null ) {
      var objectNode = loadXMLFile("JSXAPPS/IPVS-Common/model/objectXml/Session.xml"); 
      if( objectNode != null ) {
        var serviceVersion = topic_Get(tApp, "ServiceVersion");
        objectNode.setAttribute("serviceVer", serviceVersion);
        //objectNode.setAttribute("ID", ObjectID);
        nodeSession = objectNode.cloneNode(true);
      }
    }
    return nodeSession;
  }
  
});

/**
 * A subclass of <code>jsx3.xml.Document</code> that implements the Session interface. This class simply exposes the Session
 * convenience methods on an XML document.
 */
jsx3.Class.defineClass("com.ipvs.api.Session.Document", jsx3.xml.Document, [com.ipvs.api.Session], function(Document, Document_prototype) {
  /**
   * Creates a new XML document that represents an empty Session document. The XML source of the new document
   * is based on the Session.xml under model/ObjectXML
   *
   * @return {jsx3.xml.Session.Document} the newly created document.
   */
  Document.newDocument = function() {
    var nodeXML = com.ipvs.api.Session._getNewSession();
    var objXML = new Document(nodeXML);
    return objXML;
  };
  /**
   * @param objXML {jsx3.xml.Document}
   * @return {com.ipvs.api.Session.Document}
   */
  Document.wrap = function(objXML) {
    return new Document(objXML.getNativeDocument());
  };
  
  Document_prototype.cloneDocument = function() {
    return Document.wrap(this.jsxsuper());
  };

});