if (window.com && com.ipvs.cmp)
  delete com.ipvs.api.Collaboration;

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
jsx3.Class.defineInterface("com.ipvs.api.Collaboration", com.ipvs.api.Session, function (Collaboration, Collaboration_prototype) {

  /** @private @jsxobf-clobber */
  Collaboration._LOG = jsx3.util.Logger.getLogger(Collaboration.jsxclass.getName());

  Collaboration.DefaultTimeLength = 4 * 3600000;

  /*
  * Private method - gets the main collaboration connection
  * @return nodeConn {Entity}
  */
  Collaboration_prototype._getCollabConn = function () {
    // depending on session type find the first live or playback connection
    var iterConn = null;
    var sessionType = this.getSessionType();
    if (sessionType == "LIVE") {
      iterConn = this.getConnIterByType("VIEW");
    }
    else if (sessionType == "PLAYBACK") {
      iterConn = this.getConnIterByType("PLAYBACK");
    }
    else { // MIXED
      iterConn = this.getConnIter();
    }
    return iterConn.next();
  }

  /*
  * Gets the main collaboration connection
  * @return connId {String}
  */
  Collaboration_prototype.getCollabConnId = function () {
    // get the collab conn node
    var nodeConn = this._getCollabConn();
    // if found - return it's id
    if (nodeConn) {
      return this.getConnId(nodeConn);
    }
    return null;
  }

  /*
  * Private method - gets the collaboration PVR connection
  * @return nodeConn {Entity}
  */
  Collaboration_prototype._getCollabPVRConn = function () {
    var nodeConn = null;

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      var iterConn = this.getConnIterByType("PVR");
      nodeConn = iterConn.next();
    }

    return nodeConn;
  }
  /*
  * Gets the collaboration PVR connection
  * @return connId {String}
  */
  Collaboration_prototype.getCollabPVRConnId = function () {
    // get the collab conn node
    var nodeConn = this._getCollabPVRConn();
    // if found - return it's id
    if (nodeConn) {
      return this.getConnId(nodeConn);
    }
    return null;
  }
  /*
  * Private method - gets the collaboration Rec connection
  * @return nodeConn {Entity}
  */
  Collaboration_prototype._getCollabRecordConn = function () {
    var nodeConn = null;

    var sessionType = this.getSessionType();
    // if session is live - find the first record connection
    if (sessionType == "LIVE") {
      var iterConn = this.getConnIterByType("RECORD");
      nodeConn = iterConn.next();
    }

    return nodeConn;
  }
  /*
  * Gets the collaboration Record connection
  * @return connId {String}
  */
  Collaboration_prototype.getCollabRecordConnId = function () {
    // get the collab conn node
    var nodeConn = this._getCollabRecordConn();
    // if found - return it's id
    if (nodeConn) {
      return this.getConnId(nodeConn);
    }
    return null;
  }

  /*
  * checks if collaboration PVR sessionId is available
  * @return {Boolean}
  */
  Collaboration_prototype.getPVRSessionId = function () {
    var sessionId = null;

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && this.getEntityPVREnable(nodeEntity) == "true") {
        sessionId = this.getEntityPVRSessionId(nodeEntity);
      }
    }
    return sessionId;
  }

  /*
  * checks if collaboration PVR connid
  * @return {Boolean}
  */
  Collaboration_prototype.getPVRConnId = function () {
    var connId = null;
    // there are 2 cases
    //1: In case of Persistent PVR connId is in the PVR element of the entity
    //2: In case non persistent PVr we find PVR connection then get the cinnId

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && !jsx3.util.strEmpty(this.getEntityPVRSessionId(nodeEntity))) {
        connId = this.getEntityPVRConnId(nodeEntity);
      }
      else {
        connId = this.getCollabPVRConnId()
      }
    }
    return connId;
  }

  /*
  * checks if collaboration PVR dirId is available
  * @return {Boolean}
  */
  Collaboration_prototype.getPVRStartTC = function () {
    var startTC = null;

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && !jsx3.util.strEmpty(this.getEntityPVRSessionId(nodeEntity))) {
        startTC = this.getEntityPVRStartTC(nodeEntity);
      }
    }
    return startTC;
  }

  /*
  * checks if collaboration PVR startTC is available
  * @return {Boolean}
  */
  Collaboration_prototype.getPVRDirId = function () {
    var dirId = null;

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && this.getEntityPVREnable(nodeEntity) == "true") {
        dirId = this.getEntityPVRDirId(nodeEntity);
      }
    }
    return dirId;
  }

  /*
  * checks if collaboration PVR control is available
  * @return {Boolean}
  */
  Collaboration_prototype.isCollabPVRAvailable = function () {
    var bCollabPVRAvailable = false;

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      // in case of LIVE there are 2 cases:
      //1: PersistentPVR with sessionId
      //2: No persistent PVR and PVR Connection is there

      // first we check for the persistent PVR 
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && this.getEntityPVREnable(nodeEntity) == "true") {
        timeMode = "PVR";
        if (!jsx3.util.strEmpty(this.getEntityPVRSessionId(nodeEntity))) {
          bCollabPVRAvailable = true;
        }
      }
      //this 2nd case
      var iterConn = this.getConnIterByType("PVR");
      nodeConn = iterConn.next();
      if (nodeConn) {
        // if streaming then pvr is available
        var connState = this.getConnState(nodeConn);
        bCollabPVRAvailable = (connState == "STREAMING" ? true : false);
      }
    }
    // if session is playback - pvr available by default
    else if (sessionType == "PLAYBACK") {
      bCollabPVRAvailable = true;
    }
    else { // MIXED
      bCollabPVRAvailable = true;
    }

    return bCollabPVRAvailable;
  }

  /*
  * Gets the collaboration PVR state
  * @return {String}
  */
  Collaboration_prototype.getCollabPVRState = function () {
    var collabPVRState = "NONE";
    // there are 2 cases
    //1: In case of Persistent PVR state would be streaming
    //2: In case non persistent PVr we find PVR connection then get the state

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && !jsx3.util.strEmpty(this.getEntityPVRSessionId(nodeEntity))) {
        collabPVRState = 'STREAMING';
      }
      else {
        // find the first pvr connection
        var nodeConn = this._getCollabPVRConn();
        // use it's state as the collaboration state
        if (nodeConn) {
          collabPVRState = this.getConnState(nodeConn);
          // *HACK* convert WAITING_FOR_STREAMSTART + PVRaction = STREAMING
          if (collabPVRState == 'WAITING_FOR_STREAMSTART' && this.getCollabControlState() != 'Start') {
            collabPVRState = 'STREAMING';
          }
        }
      }
    }
    return collabPVRState;
  }

  /*
  * Gets the main collaboration state
  * @return {String}
  */
  Collaboration_prototype.getCollabState = function () {
    var collabState = "NONE";
    // get the main collab conn node
    var nodeConn = this._getCollabConn();
    // use it's state as the collaboration state
    if (nodeConn) {
      collabState = this.getConnState(nodeConn);
      // *HACK* convert WAITING_FOR_STREAMSTART + PVRaction = STREAMING
      if (collabState == 'WAITING_FOR_STREAMSTART' && this.getCollabControlState() != 'Start') {
        collabState = 'STREAMING';
      }
    }
    return collabState;
  }

  /*
  * Gets the main collaboration Error details
  * @return {objCollabError} JSON/null
  */
  Collaboration_prototype.getCollabError = function () {
    var objCollabError = null;
    // get the main collab conn node
    var nodeConn = this._getCollabConn();
    // use it's error details as the collaboration error
    if (nodeConn) {
      objCollabError = this.getConnError(nodeConn);

    }
    return objCollabError;
  }

  /*
  * Gets the collaboration record Error details
  * @return {objCollabRecordError} JSON/null
  */
  Collaboration_prototype.getCollabPVRError = function () {
    var objCollabPVRError = null;
    // get the pvr collab conn node
    var nodeConn = this._getCollabPVRConn();
    // use it's error details as the pvr error
    if (nodeConn) {
      objCollabPVRError = this.getConnError(nodeConn);

    }
    return objCollabPVRError;
  }

  /*
  * Gets the collaboration Control state
  * @return {String}
  */
  Collaboration_prototype.getCollabControlState = function () {
    // get the id of the main collab timeline
    var timelineId = this.getTimelineId();
    // get the action from it
    var controlState = this.getTimelineRequestedAction(timelineId);
    // normalize all Pause/Resume variations (PauseAt, ResumeFrom etc..)
    controlState.match("/^Pause.*$/") && (controlState = "Pause");
    controlState.match("/^Resume.*$/") && (controlState = "Resume");

    return controlState;
  }

  /*
  * Gets the collaboration Record state
  * @return {String}
  */
  Collaboration_prototype.getCollabRecordState = function () {
    var collabRecordState = "NONE";
    // find the first record connection
    var iterConn = this.getConnIterByType("RECORD");
    // use it's state as the collaboration state
    var nodeConn = iterConn.next();
    if (nodeConn) {
      collabRecordState = this.getConnState(nodeConn);
      // *HACK* convert WAITING_FOR_STREAMSTART + PVRaction = STREAMING
      if (collabRecordState == 'WAITING_FOR_STREAMSTART' && this.getCollabRecordControlState() != 'Start') {
        collabRecordState = 'STREAMING';
      }
    }
    return collabRecordState;
  }
  /*
  * Gets the collaboration Record Control state
  * @return {String}
  */
  Collaboration_prototype.getCollabRecordControlState = function () {
    // get the id of the record collab timeline
    var timelineId = this.getRecordTimelineId();
    // get the action from it
    return this.getTimelineRequestedAction(timelineId);
  }

  /*
  * Gets the collaboration record Error details
  * @return {objCollabRecordError} JSON/null
  */
  Collaboration_prototype.getCollabRecordError = function () {
    var objCollabRecordError = null;
    // get the main collab conn node
    var nodeConn = this._getCollabRecordConn();
    // use it's error details as the collaboration error
    if (nodeConn) {
      objCollabRecordError = this.getConnError(nodeConn);

    }
    return objCollabRecordError;
  }

  /*
  * Gets the collaboration Time mode
  * @return {String}
  */
  Collaboration_prototype.getCollabTimeMode = function () {
    var timeMode = "NONE";
    // set time mode based on session type
    var sessionType = this.getSessionType();
    if (sessionType == "LIVE") {
      // live by default
      timeMode = "LIVE";
      // look for PVR in the first src port entity
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity && this.getEntityPVREnable(nodeEntity) == "true") {
        timeMode = "PVR";
        if (!jsx3.util.strEmpty(this.getEntityPVRSessionId(nodeEntity))) {
          timeMode = "PERSISTENT_PVR";
        }
      }
    }
    else if (sessionType == "PLAYBACK") {
      timeMode = "PLAYBACK";
    }
    else { // MIXED
      timeMode = "LIVE";
    }
    return timeMode;
  }

  /*
  * Gets the collaboration Time Info
  * @return {String}
  */
  Collaboration_prototype.getCollabTimeInfo = function () {
    // get the id of the main collab timeline
    var timelineId = this.getTimelineId();
    // get the action from it
    var objTimeInfo = new Object();
    objTimeInfo.timeScale = this.getTimelineTimeScale(timelineId);
    objTimeInfo.startOffset = this.getTimelineStartOffset(timelineId);
    objTimeInfo.startWC = this.getTimelineStartWC(timelineId);
    objTimeInfo.lastOffset = this.getTimelineLastOffset(timelineId);
    objTimeInfo.lastWC = this.getTimelineLastWC(timelineId);
    return objTimeInfo;
  }

  /*
  * Gets the PersistentPVR Info
  * @return {String}
  */
  Collaboration_prototype.getPvrInfo = function () {
    var objPvrInfo = new Object();
    objPvrInfo.canPVR = this.isCollabPVRAvailable();
    objPvrInfo.timeLength = this.getCollabTimelength();
    objPvrInfo.sessionId = this.getPVRSessionId();
    objPvrInfo.dirId = this.getPVRDirId();
    objPvrInfo.startTC = this.getPVRStartTC();
    objPvrInfo.connId = this.getPVRConnId();
    objPvrInfo.pvrState = this.getCollabPVRState();

    return objPvrInfo;
  }

  /*
  * Gets the collaboration Record Time Info
  * @return {String}
  */
  Collaboration_prototype.getCollabRecordTimeInfo = function () {
    // get the id of the main collab timeline
    var timelineId = this.getRecordTimelineId();
    // get the action from it
    var objTimeInfo = new Object();
    objTimeInfo.timeScale = this.getTimelineTimeScale(timelineId);
    objTimeInfo.startOffset = this.getTimelineStartOffset(timelineId);
    objTimeInfo.startWC = this.getTimelineStartWC(timelineId);
    objTimeInfo.lastOffset = this.getTimelineLastOffset(timelineId);
    objTimeInfo.lastWC = this.getTimelineLastWC(timelineId);
    return objTimeInfo;
  }

  /*
  * Gets the collaboration Playlength
  * @return {String}
  */
  Collaboration_prototype.getCollabTimelength = function () {
    // use default for live and mixed sessions
    var playlength = Collaboration.DefaultTimeLength;
    // if playback session
    var sessionType = this.getSessionType();
    if (sessionType == "PLAYBACK") {
      // look for playlength in first plyFile entity
      var iterEntity = this.getEntityIterByType("PlyFile");
      nodeEntity = iterEntity.next();
      if (nodeEntity) {
        playlength = this.getEntityPlaylength(nodeEntity);
        // if it is empty - back to default
        if (jsx3.util.strEmpty(playlength)) playlength = Collaboration.DefaultTimeLength;
      }
    }
    else if (sessionType == "LIVE") {
      var iterEntity = this.getEntityIterByType("SrcPort");
      nodeEntity = iterEntity.next();
      if (nodeEntity) {
        playlength = this.getEntityPVRTrimLength(nodeEntity);
        // if it is empty - back to default
        if (jsx3.util.strEmpty(playlength)) playlength = Collaboration.DefaultTimeLength;
      }
    }
    return playlength;
  }

  /*
  * Gets the collaboration Timeshift state
  * @return {Boolean}
  */
  Collaboration_prototype.isCollabTimeShifted = function () {
    // get the id of the main collab timeline
    var timelineId = this.getTimelineId();
    // check if it is timeshifted it
    return this.getTimelineIsTimeshifted(timelineId);
  }

  /*
  * Gets the isCollabBookmarkAvailable
  * @return {Boolean}
  */
  Collaboration_prototype.isCollabBookmarkAvailable = function () {
    var bCollabBookmarkAvailable = false;

    var sessionType = this.getSessionType();
    // if session is live - find the first pvr connection
    if (sessionType == "LIVE") {
      var iterConn = this.getConnIterByType("RECORD");
      nodeConn = iterConn.next();
      if (nodeConn) {
        // if streaming then bookmarking is available
        var connState = this.getConnState(nodeConn);
        if (connState == "STREAMING" || (connState == "WAITING_FOR_STREAMSTART" && this.getCollabRecordControlState() == "Pause")) {
          bCollabBookmarkAvailable = true;
        }
      }
    }
    // if session is playback - pvr available by default
    else if (sessionType == "PLAYBACK") {
      bCollabBookmarkAvailable = true;
    }
    else { // MIXED
      bCollabBookmarkAvailable = true;
    }

    return bCollabBookmarkAvailable;
  }
  /*
  * Gets the collaboration bookmark fileNID connection
  * @return connId {String}
  */
  Collaboration_prototype.getCollabBookmarkFileNID = function () {
    var fileNID = "";
    var sessionType = this.getSessionType();
    var entityType = (sessionType == "PLAYBACK") ? "PlyFile" : "RecFile";
    var iterEntity = this.getEntityIterByType(entityType);
    var nodeEntity = iterEntity.next();
    if(!jsx3.util.strEmpty(nodeEntity)) {
      //check for groupid and get fileNID for that group if exists
      var groupId = this.getEntityMediaGroupId(nodeEntity);
      if(!jsx3.util.strEmpty(groupId)) {
        fileNID = this.getEntityMediaGroupFileNID(groupId);
      }
      else {
        fileNID = this.getEntityFileNID(nodeEntity);
      }
    }
    return fileNID;
  }

});

/**
 * A subclass of <code>jsx3.xml.Document</code> that implements the Collaboration interface. This class simply exposes the Collaboration
 * convenience methods on an XML document.
 */
jsx3.Class.defineClass("com.ipvs.api.Collaboration.Document",jsx3.xml.Document,[com.ipvs.api.Collaboration, com.ipvs.api.Session, com.ipvs.api.SessionInfo], function(Document, Document_prototype) {
  /**
   * @param objXML {jsx3.xml.Document}
   * @return {com.ipvs.api.Collaboration.Document}
   */
  Document.wrap = function(objXML) {
    return new Document(objXML.getNativeDocument());
  };
  
  Document_prototype.cloneDocument = function() {
    return Document.wrap(this.jsxsuper());
  };

});