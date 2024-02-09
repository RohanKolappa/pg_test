if (window.com && com.ipvs.cmp)
  delete com.ipvs.api.Session;

jsx3.require("jsx3.xml.Document","com.ipvs.api.SessionInfo");
/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Mixin interface that provides methods for accessing and manipulating a document in Session schema.
 * <p/>
 * This interface is modeled on the lines of the CDF interface provided/used by GI
 */
  
 jsx3.Class.defineInterface("com.ipvs.api.Session", com.ipvs.api.SessionInfo, function(Session, Session_prototype) {
  
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
   * Gets the state
   * @return {String}
   */
  Session_prototype.getState = function() {
    return this.getAttribute("state");
  }   
  /*
   * Sets the state
   * @param state {String}
   */
  Session_prototype.setState = function(state) {
    this.setAttribute("state", state);
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
  Session_prototype.getSessionType = function() {
    return this.getAttribute("type");
  }
  /*
  * Sets the session type
  */
  Session_prototype.setSessionType = function(type) {
    return this.setAttribute("type", type);
  }  
  
  /*
   * Get last Action from the timeline specified
   * @param {String} timelineId
   * @return {String}
   */
  Session_prototype.getTimelineRequestedAction = function(timelineId) {
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
  Session_prototype.getTimelineTimeScale = function(timelineId) {
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
  Session_prototype.getTimelineStartOffset = function(timelineId) {
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
  Session_prototype.getTimelineStartWC = function(timelineId) {
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
  Session_prototype.getTimelineLastOffset = function(timelineId) {
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
  Session_prototype.getTimelineLastWC = function(timelineId) {
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
  Session_prototype.getTimelineIsTimeshifted = function(timelineId) {
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
    if( (lastWC - lastOffset) != (startWC - startOffset) ) {
      bIsTimeshifted = true;
    }
    return bIsTimeshifted;
  }
  
  /*
   * gets an timeline from the Session
   * @param {String} timelineId
   * @return {Entity} nodeTimeline
   */
  Session_prototype.getTimeline = function(timelineId) {
    return this.selectSingleNode(".//Timeline[@id='"+timelineId+"']");
  }   
  
  /*
   * Adds an timeline to the Session
   * @param {Object} objTimeline
   * @return {String} timelineId
   */
  Session_prototype.addTimeline = function(objTimeline) {
    // create a new timeline node
    var nodeTimeline = this._getNewTimelineNode();
    if( ! nodeTimeline ) return null;
    
    // generate a new TimelineId - if needed
    var timelineId = objTimeline.timelineId;
    if( ! timelineId ) {
      timelineId = objTimeline.type + '_' + getUUID();
      objTimeline.timelineId = timelineId;
    }
    
    // set the values in the new node 
    this._updateTimelineNode(nodeTimeline, objTimeline);
    
    // append it to the list in the current collab xml in the topic
    var nodeTimelineList = this.selectSingleNode(".//TimelineList");
    nodeTimelineList.appendChild(nodeTimeline);

    // return the id of the new timeline
    return timelineId;
  }
  /*
   * Deletes an Timeline from the Session.
   * Typically not called directly. (called from add/updateConnGroup).
   * @param {String} timelineId
   * @return {Boolean}
   */
  Session_prototype.delTimeline = function(timelineId) {
    // get the timeline Node
    var nodeTimeline = this.selectSingleNode(".//Timeline[@id='"+ timelineId +"']");
    if( ! nodeTimeline ) return false;
    
    // delete it from its parent node
    nodeTimeline.getParent().removeChild(nodeTimeline);
    
    // return success
    return true;
  }
  /*
   * Updates an timeline in the Session
   * @param {String} timelineId
   * @param {Object} objTimeline
   * @return {Boolean}
   */
  Session_prototype.updateTimeline = function(timelineId, objTimeline) {
    // get the timeline Node
    var nodeTimeline = this.selectSingleNode(".//Timeline[@id='"+ timelineId +"']");
    if( ! nodeTimeline ) return false;
    
    // update timeline node
    return this._updateTimelineNode(nodeTimeline, objTimeline);
  }
  /*
   * Helper function to update timeline
   * @param nodeTimeline {jsx3.xml.Entity}
   * @param objTimeline {Object}
   * @return {String} timelineId
   */
  Session_prototype._updateTimelineNode = function(nodeTimeline, objTimeline) {    
    if( objTimeline.sessionId ) nodeTimeline.setAttribute("sessionId", objTimeline.sessionId);
    if( objTimeline.connGroupId ) nodeTimeline.setAttribute("connGroupId", objTimeline.connGroupId);
    if( objTimeline.connId ) nodeTimeline.setAttribute("connId", objTimeline.connId);
    if( objTimeline.timelineId ) nodeTimeline.setAttribute("id", objTimeline.timelineId);

    var nodeStart = nodeTimeline.selectSingleNode(".//Start");
    if( objTimeline.startOffset ) nodeStart.setAttribute("Offset", objTimeline.startOffset);
    if( objTimeline.startWC ) nodeStart.setAttribute("WC", objTimeline.startWC);

    var nodeLast = nodeTimeline.selectSingleNode(".//Last");
    if( objTimeline.lastOffset ) nodeLast.setAttribute("Offset", objTimeline.lastOffset);
    if( objTimeline.lastWC ) nodeLast.setAttribute("WC", objTimeline.lastWC);
    if( objTimeline.timeScale ) nodeLast.setAttribute("TimeScale", objTimeline.timeScale);
    if( objTimeline.lastAction ) nodeLast.setAttribute("requestedAction", objTimeline.lastAction);
    
    // return id
    return nodeTimeline.getAttribute("id");
  }
  
  /*
   * Updates a connection state in the Session
   * @param {String} connId
   * @param {Object} objConnState
   * @return {Boolean}
   */
  Session_prototype.updateConnState = function(connId, objConnState) {
    // update the state and streamURL attr in the connection
    this.setConnState(connId, objConnState.state);
    //if state is error the update code and description
    if(!jsx3.util.strEmpty(objConnState.errorCode)) {
      this.setConnErrorDetails(connId, objConnState.errorCode, objConnState.errorData)
    }
    this.setConnStreamURL(connId, objConnState.streamURL);
    // update the timeline node
    if(!objConnState.objTimeline) return;
    var connTimelineId = objConnState.objTimeline.timelineId;   
    if( ! this.updateTimeline(connTimelineId, objConnState.objTimeline) ) {
      // if not exist - add
      this.addTimeline(objConnState.objTimeline);
    }
  }
  /*
  * Sets the state of an connection
  * @param {String} connId
   * @param {String} state
  */
  Session_prototype.setConnState = function(connId, state) {
    // get the connection Node
    var nodeConn = this.selectSingleNode(".//Connection[@id='"+ connId +"']");
    if( ! nodeConn ) return null;
    
    nodeConn.setAttribute("state", state);
  }
  /*
  * Sets the error details of an connection
  * @param {String} connId
  * @param {String} errorCode
  * @param {String} errorData
  */
  Session_prototype.setConnErrorDetails = function(connId, errorCode, errorData) {
    // get the connection Node
    var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    if(!nodeConn) return null;

    nodeConn.setAttribute("errorCode", errorCode);
    nodeConn.setAttribute("errorData", errorData);
  }
  /*
  * Sets the streamURL of an connection
  * @param {String} connId
   * @param {String} streamURL
  */
  Session_prototype.setConnStreamURL = function(connId, streamURL) {
    // get the connection Node
    var nodeConn = this.selectSingleNode(".//Connection[@id='"+ connId +"']");
    if( ! nodeConn ) return null;
    
    nodeConn.setAttribute("streamURL", streamURL);
  }
  /*
   * Internal utility function to get new timeline nodes
   * @return {jsx3.xml.Entity} nodeTimeline
   */
  Session_prototype._getNewTimelineNode = function() {
    // get a template from the cache OR model/objectXml
    var nodeSession = Session._getSessionObjectXML();
    
    // get the default dir node
    var nodeTimeline = nodeSession.selectSingleNode(".//Timeline").cloneNode(true);
    
    return nodeTimeline;
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
    var nodeSession = "";
    var objectNode = loadXMLFile("addins/model/objectXml/Session.xml");
    if(objectNode != null) {
      var serviceVersion = com.ipvs.app.App.clsApp.clsApplet.getServiceVersion();
      objectNode.setAttribute("serviceVer", serviceVersion);
      //objectNode.setAttribute("ID", ObjectID);
      nodeSession = objectNode.cloneNode(true);
    }
    return nodeSession;
  }

});

/**
 * A subclass of <code>jsx3.xml.Document</code> that implements the Session interface. This class simply exposes the Session
 * convenience methods on an XML document.
 */
jsx3.Class.defineClass("com.ipvs.api.Session.Document",jsx3.xml.Document,[com.ipvs.api.Session, com.ipvs.api.SessionInfo], function(Document, Document_prototype) {
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