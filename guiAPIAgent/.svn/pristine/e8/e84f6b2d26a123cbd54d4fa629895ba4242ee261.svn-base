/*
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 */

/**
 * Mixin interface that provides methods for accessing and manipulating a document in Session schema.
 * <p/>
 * This interface is modeled on the lines of the CDF interface provided/used by GI
 */
SessionInfo = function (strSession) {
  this.cls = "SessionInfo";
  BarcoUtil.logger(bDEBUG, "", this.cls, "", "constructor", "");
  this.init(strSession);
}
jQuery.extend(SessionInfo.prototype, {
    /**
    * {String}
    * @final @jsxobf-final
    */
    ELEM_ROOT: '<SessionInfo id="" name=""/>',

    DIR_IDPREFIX: "Dir_",
    GRPFILE_IDPREFIX: "GrpFile_",
    ENTITY_IDPREFIX: "Entity_",
    INSTANCE_IDPREFIX: "Instance_",
    CONNGRP_IDPREFIX: "ConnGrp_",
    CONN_IDPREFIX: "Conn_",

    DIRID_DEFAULTREC: "Dir_DefaultRec",
    GRPFILEID_DEFAULTREC: "GrpFile_DefaultRec",

    GRPFILETYPE_SYNCREC: "SyncRec",
    GRPFILETYPE_SYNCPLY: "SyncPly",

    ENTITYTYPE_USER: "User",
    ENTITYTYPE_SRCPORT: "SrcPort",
    ENTITYTYPE_DSTPORT: "DstPort",
    ENTITYTYPE_RELAYPORT: "RelayPort",
    ENTITYTYPE_STOREPORT: "StorePort",
    ENTITYTYPE_PLYFILE: "PlyFile",
    ENTITYTYPE_RECFILE: "RecFile",
    ENTITYTYPE_PVRFILE: "PVRFile",

    ENTITYSTREAMTYPE_V2D: "V2D",
    ENTITYSTREAMTYPE_RTP: "RTP",
    ENTITYSTREAMTYPE_UDP: "UDP",
    ENTITYSTREAMTYPE_MPEGTS: "MPEGTS",
    ENTITYSTREAMTYPE_MULTI: "MULTI",

    //STREAM is deprecated not in use new values View and Playback
    CONNTYPE_STREAM: "STREAM",

    CONNTYPE_VIEW: "VIEW",
    CONNTYPE_PLAYBACK: "PLAYBACK",
    CONNTYPE_RECORD: "RECORD",
    CONNTYPE_PVR: "PVR",
    CONNTYPE_PARTIAL: "PARTIAL",

    SESSIONTYPE_LIVE: "LIVE",
    SESSIONTYPE_PLAYBACK: "PLAYBACK",
    SESSIONTYPE_MIXED: "MIXED",

    /** @private @jsxobf-clobber */
    //Session._LOG = jsx3.util.Logger.getLogger(Session.jsxclass.getName());
    init: function (objSession) {
        BarcoUtil.logger(bDEBUG, "", this.cls, "init", "call", "");
        //console.log("strSession>>>" + strSession);
        //var jsonSession = JSON.parse(strSession);
        if (objSession.View) {
            this.objSession = objSession.View.Session;
        }
        else if (objSession.Playback) {
            this.objSession = objSession.Playback.Session;
        }
        else if (objSession.Session) {
            //Session created by startURL
            this.objSession = objSession.Session;
        }
        else if (objSession.Pause) {
            this.objSession = objSession.Pause;
        }
        else {
            return null;
        }
    },
    /**
    * Gets the Session Name
    * @return {String}
    */
    getName: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getName", "call", "");
        return this.objSession['@name'];
    },
    /*
    * Sets the Session Name
    * @param strName {String}
    */
    setName: function (strName) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setName", "call", strName);
        this.objSession['@name'] = strName;
    },
    /*
    * Gets the ID
    * @return {String}
    */
    getID: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getID", "call", "");
        return this.objSession['@id'];
    },
    /*
    * Sets the ID
    * @param ID {String}
    */
    setID: function (ID) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setID", "call", ID);
        this.objSession['@id'] = ID;
    },
    /*
    * Gets the state
    * @return {String}
    */
    getState: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getState", "call", "");
        return this.objSession['@state'];
    },
    /*
    * Sets the state
    * @param state {String}
    */
    setState: function (state) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setState", "call", "");
        this.objSession['@state'] = state;
    },
    /*
    * Gets the timelineId
    * @return {String}
    */
    getTimelineId: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineId", "call", "");
        return this.objSession['@timelineId'];
    },
    /*
    * Sets the timelineId
    * @param ID {String}
    */
    setTimelineId: function (ID) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setTimelineId", "call", ID);
        this.objSession["@timelineId"] = ID;
    },
    /*
    * Gets the recordTimelineId
    * @return {String}
    */
    getRecordTimelineId: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getRecordTimelineId", "call", "");
        return this.objSession['@recordTimelineId'];
    },
    /*
    * Sets the recordTimelineId
    * @param ID {String}
    */
    setRecordTimelineId: function (ID) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setRecordTimelineId", "call", ID);
        this.objSession['@recordTimelineId'] = ID;
    },
    /*
    * Gets the sessionConfigNID
    * @return {String}
    */
    getsessionConfigNID: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getsessionConfigNID", "call", "");
        return this.objSession.Properties['@sessionConfigNID'];
    },
    /*
    * Sets the sessionConfigNID
    * @param sessionConfigNID {String}
    */
    setsessionConfigNID: function (sessionConfigNID) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setsessionConfigNID", "call", "");
        this.objSession.Properties['@sessionConfigNID'] = sessionConfigNID;
    },
    /*
    * Gets the session type for task 19599
    */
    getSessionType: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getSessionType", "call", "");
        return this.objSession['@type'];
    },
    /*
    * Sets the session type
    */
    setSessionType: function (type) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "setSessionType", "call", "");
        return this.objSession['@type'] = type;
    },

    /*
    * Get last Action from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineRequestedAction: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineRequestedAction", "call", timelineId);
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get the attr from the Last element
		return nodeTimeline ? nodeTimeline.Last['@requestedAction'] : "";
    },

    /*
    * Get time scale from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineTimeScale: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineTimeScale", "call", timelineId);
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get the attr from the Last element
		return nodeTimeline ? nodeTimeline.Last['@TimeScale'] : "";
    },

    /*
    * Get start offset from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineStartOffset: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineStartOffset", "call", timelineId);
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get the attr from the Start element
		return nodeTimeline ? nodeTimeline.Start['@Offset'] : "";
    },

    /*
    * Get start wc from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineStartWC: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineStartWC", "call", timelineId);
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get the attr from the Start element
        return nodeTimeline ? nodeTimeline.Start['@WC'] : "";
    },

    /*
    * Get last offset from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineLastOffset: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineLastOffset", "call", timelineId);
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get the attr from the Last element
        return nodeTimeline ? nodeTimeline.Last['@Offset'] : "";
    },

    /*
    * Get last wc from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineLastWC: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineLastWC", "call", timelineId);
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get the attr from the Last element
		return nodeTimeline ? nodeTimeline.Last['@WC'] : "";
    },

    /*
    * Get if timeshifted from the timeline specified
    * @param {String} timelineId
    * @return {String}
    */
    getTimelineIsTimeshifted: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimelineIsTimeshifted", "call", timelineId);
        var bIsTimeshifted = false;
        // get the timeline node
        var nodeTimeline = this.getTimeline(timelineId);
        // get start info
        if( nodeTimeline ) {
			var startOffset = nodeTimeline.Start['@Offset'];
			var startWC = nodeTimeline.Start['@WC'];
			// get last info
			var lastOffset = nodeTimeline.Last['@Offset'];
			var lastWC = nodeTimeline.Last['@WC'];
			// if the lastwc - lastoffset != startwc - startOffset - then time shifted     
			if ((lastWC - lastOffset) != (startWC - startOffset)) {
				bIsTimeshifted = true;
			}
		}
        return bIsTimeshifted;
    },

    /*
    * gets an timeline from the Session
    * @param {String} timelineId
    * @return {Entity} nodeTimeline
    */
    getTimeline: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getTimeline", "call", timelineId);
        //var timeLineList = this.objSession.TimelineList.Timeline;
        var timeLineList = this.objSession.TimelineList.Timeline ? this.objSession.TimelineList.Timeline : this.objSession.Session.TimelineList.Timeline;
        var objTimeLine = null;
        //Check if it has multiple timelines which is array
        if ((timeLineList) instanceof Array) {
            jQuery.each(timeLineList, function (key, timelineNode) {
                if (timelineNode['@id'] == timelineId) {
                    objTimeLine = timelineNode;
                    return false;
                }
            })
        }
        else {
            if (timeLineList['@id'] == timelineId) objTimeLine = timeLineList;
        }
        return objTimeLine;
    },

    /*
    * Adds an timeline to the Session
    * @param {Object} objTimeline
    * @return {String} timelineId
    */
    addTimeline: function (objTimeline) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "addTimeline", "call", "");
        // create a new timeline node
        //var nodeTimeline = this._getNewTimelineNode();
        //if (!nodeTimeline) return null;

        //// generate a new TimelineId - if needed
        //var timelineId = objTimeline.timelineId;
        //if (!timelineId) {
        //  timelineId = objTimeline.type + '_' + getUUID();
        //  objTimeline.timelineId = timelineId;
        //}

        //// set the values in the new node 
        //nodeTimeline = this._updateTimelineNode(nodeTimeline, objTimeline);

        // append it to the list in the current Session xml in the topic
        if( (this.objSession.TimelineList.Timeline) instanceof Array) {
			this.objSession.TimelineList.Timeline.push(objTimeline);
		}
		else {
			this.objSession.TimelineList.Timeline = objTimeline;
		}
        // return the id of the new timeline
        return objTimeline['@Id'];
    },
    /*
    * Deletes an Timeline from the Session.
    * Typically not called directly. (called from add/updateConnGroup).
    * @param {String} timelineId
    * @return {Boolean}
    */
    delTimeline: function (timelineId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "delTimeline", "call", timelineId);
        // get the timeline Node
        var nodeTimeline = this.getTimeline(timelineId);
        if (!nodeTimeline) return false;

        // delete it from its parent node
        //this.objSession.TimelineList.Timeline.splice(jQuery.inArray(nodeTimeline, this.objSession.TimelineList.Timeline), 1);
		if( (this.objSession.TimelineList.Timeline) instanceof Array) {
			this.objSession.TimelineList.Timeline.splice(jQuery.inArray(nodeTimeline, this.objSession.TimelineList.Timeline), 1);
		}
		else {
			this.objSession.TimelineList.Timeline = null;
			delete this.objSession.TimelineList.Timeline;
		}
		
        // return success
        return true;
    },
    /*
    * Updates an timeline in the Session
    * @param {String} timelineId
    * @param {Object} objTimeline
    * @return {Boolean}
    */
    updateTimeline: function (timelineId, objTimeline) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "updateTimeline", "call", timelineId);
        // get the timeline Node
        var nodeTimeline = this.getTimeline(timelineId);
        if (!nodeTimeline) return false;

        // update timeline node
        //nodeTimeline = this._updateTimelineNode(nodeTimeline, objTimeline);
        this.delTimeline(timelineId);
        // append it to the list in the current Session xml in the topic
        if( (this.objSession.TimelineList.Timeline) instanceof Array) {
			    this.objSession.TimelineList.Timeline.push(objTimeline);
		    }
		    else {
			    this.objSession.TimelineList.Timeline = objTimeline;
		    }
		    return true;
    },
    /*
    * Helper function to update timeline
    * @param nodeTimeline {jsx3.xml.Entity}
    * @param objTimeline {Object}
    * @return {String} timelineId
    */
    _updateTimelineNode: function (nodeTimeline, objTimeline) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_updateTimelineNode", "call", "");
        if (objTimeline.sessionId) nodeTimeline['@sessionId'] = objTimeline.sessionId;
        if (objTimeline.connGroupId) nodeTimeline['@connGroupId'] = objTimeline.connGroupId;
        if (objTimeline.connId) nodeTimeline['@connId'] = objTimeline.connId;
        if (objTimeline.timelineId) nodeTimeline['@id'] = objTimeline.timelineId;

        if (objTimeline.startOffset) nodeTimeline.Start['@Offset'] = objTimeline.startOffset;
        if (objTimeline.startWC) nodeTimeline.Start['@WC'] = objTimeline.startWC;

        var nodeLast = nodeTimeline.selectSingleNode(".//Last");
        if (objTimeline.lastOffset) nodeTimeline.Last['@Offset'] = objTimeline.lastOffset;
        if (objTimeline.lastWC) nodeTimeline.Last['@WC'] = objTimeline.lastWC;
        if (objTimeline.timeScale) nodeTimeline.Last['@TimeScale'] = objTimeline.timeScale;
        if (objTimeline.lastAction) nodeTimeline.Last['@requestedAction'] = objTimeline.lastAction;

        // return timelinenode
        return nodeTimeline;
    },

    /*
    * Updates a connection state in the Session
    * @param {String} connId
    * @param {Object} objConnState
    * @return {Boolean}
    */
    updateConnState: function (connId, objConnState) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "updateConnState", "call", connId);
        var objConn = this.getConnection(connId);
        if (!objConn) return false;
        // update the state and streamURL attr in the connection
        objConn['@state'] = objConnState['@value'];
        //if state is error the update code and description
        if (objConnState['@errorCode']) {
            objConn['@errorCode'] = objConnState['@errorCode'];
            objConn['@errorData'] = objConnState['@errorData'];
        }

        this.deleteConn(connId);
		if( (this.objSession.ConnectionList.Connection) instanceof Array) {
			this.objSession.ConnectionList.Connection.push(objConn);
		}
		else {
			this.objSession.ConnectionList.Connection = objConn;
		}
        // update the timeline node
        if (objConnState.Timeline) {
			var connTimelineId = objConnState.Timeline['@id'];
			if (!this.updateTimeline(connTimelineId, objConnState.Timeline)) {
				// if not exist - add
				this.addTimeline(objConnState.Timeline);
			}
		}
		return true;
    },

    /*
    * Deletes a connection from the Session.
    * Typically not called directly. (called from add/updateConnGroup).
    * @param {String} timelineId
    * @return {Boolean}
    */
    deleteConn: function (connId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "deleteConn", "call", connId);
        // get the timeline Node
        var objConn = this.getConnection(connId);
        if (!objConn) return null;
        // delete it from its parent node
		if( (this.objSession.ConnectionList.Connection) instanceof Array) {
			this.objSession.ConnectionList.Connection.splice(jQuery.inArray(objConn, this.objSession.ConnectionList.Connection), 1);
		}
		else {
			this.objSession.ConnectionList.Connection = null;
			delete this.objSession.ConnectionList.Connection;
		}

        // return success
        return true;
    },
    /*
    * gets connection node from the Session
    * @param {String} timelineId
    * @return {Entity} nodeTimeline
    */
    getConnection: function (connId) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getConnection", "call", connId);
        var connlist = this.objSession.ConnectionList.Connection;
        var objConn = null;
        //Check if it has multiple connections which is array
        if ((connlist) instanceof Array) {
            jQuery.each(connlist, function (key, connNode) {
                if (connNode['@id'] == connId) {
                    objConn = connNode;
                    return false;
                }
            })
        }
        else {
            if (connlist['@id'] == connId) objConn = connlist;
        }
        return objConn;
    },
	/*
    * Gets the error details of an connection
    * @ param nodeConn
    * @return {objError} JSON
    */
    getConnError: function (nodeConn) {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getConnError", "call", "");
        if (nodeConn['@errorCode'] != null && nodeConn['@errorCode'] != "") {
            var objError = new Object();
            objError.errorCode = nodeConn['@errorCode'];
            objError.errorData = nodeConn['@errorData'];
            return objError;
        }
        return null;
    },
	getPVRSessionId: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getPVRSessionId", "call", "");
        var sessionId = null;
        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            // look for PVR in the first src port entity
            var entityList = this.objSession.EntityList.Entity;
            jQuery.each(entityList, function (key, entityNode) {
                if (entityNode['@type'] == "SrcPort" && (entityNode.PVR['@enable'])) {
                    sessionId = entityNode.PVR['@sessionId'];
					return false;
                }
            })
        }
        return sessionId;
    },

    getPVRDirId: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getPVRDirId", "call", "");
        var pvrDirId = null;
        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            // look for PVR in the first src port entity
            var entityList = this.objSession.EntityList.Entity;
            jQuery.each(entityList, function (key, entityNode) {
                if (entityNode['@type'] == "SrcPort" && (entityNode.PVR['@enable'])) {
                    pvrDirId = entityNode.PVR['@dirId'];
					return false;
                }
            })
        }
        return pvrDirId;
    },
    getPVRStartTC: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getPVRStartTC", "call", "");
        var startTC = null;
        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            // look for PVR in the first src port entity
            var entityList = this.objSession.EntityList.Entity;
            jQuery.each(entityList, function (key, entityNode) {
                if (entityNode['@type'] == "SrcPort" && (entityNode.PVR['@enable'])) {
                    if (entityNode.PVR['@sessionId']) startTC = entityNode.PVR['@startTC'];
					return false;
                }
            })
        }
        return startTC;
    },
    getPVRConnId: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getPVRConnId", "call", "");
        var connId = null;
        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            // look for PVR in the first src port entity
            var connlist = this.objSession.ConnectionList.Connection;
            //Check if it has multiple connections which is array
			if( ! ((connlist) instanceof Array) ) {
				connlist = new Array();
				connlist.push(this.objSession.ConnectionList.Connection);
			}
			jQuery.each(connlist, function (key, connNode) {
				if (connNode['@type'] == "PVR") {
					connId = connNode['@id'];
					return false;
				}
			});
        }
        return connId;
    },
	
    /*
    * Internal utility function to get new timeline nodes
    * @return {jsx3.xml.Entity} nodeTimeline
    */
    _getNewTimelineNode: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_getNewTimelineNode", "call", "");
        // get a template from the cache OR model/objectXml
        var nodeSession = Session._getSessionObject();

        // get the default timeline node
        return nodeSession.TimelineList.Timeline;
    },

    //-------SessionCtrlInfo---------
	// Parse thru the seesion structure - normalize the different session types/variations
	// and collect the info needed to drive a SessionCtrl bar. 
    getSessionCtrlInfo: function() {
		BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlInfo", "call", "");
		var objSessionCtrlInfo = {};
		objSessionCtrlInfo.sessionId = this.getID();
		// set the modes for other function
		objSessionCtrlInfo.timeMode = this.getSessionCtrlTimeMode();
		objSessionCtrlInfo.timeInfo = this.getSessionCtrlTimeInfo();
		objSessionCtrlInfo.pvrInfo = this.getSessionCtrlPvrInfo();
		objSessionCtrlInfo.playlength = this.getSessionCtrlTimelength();
		objSessionCtrlInfo.state = this.getSessionCtrlState();
		objSessionCtrlInfo.connId = this.getSessionCtrlConnId();

		objSessionCtrlInfo.action = this.getSessionCtrlActionState();
		objSessionCtrlInfo.isTimeshifted = this.isSessionCtrlTimeShifted();
		objSessionCtrlInfo.timelineId = this.getTimelineId();

		objSessionCtrlInfo.recState = this.getSessionCtrlRecordState();
		objSessionCtrlInfo.recErrorData = this.getSessionCtrlRecordErrorData();
		objSessionCtrlInfo.recConnId = this.getSessionCtrlRecordConnId();
		objSessionCtrlInfo.recAction = this.getSessionCtrlRecordActionState();
		objSessionCtrlInfo.recTimelineId = this.getRecordTimelineId();

		objSessionCtrlInfo.canBookmark = this.isSessionCtrlBookmarkAvailable();
		objSessionCtrlInfo.bookmarkFileNID = this.getSessionCtrlBookmarkFileNID();
		
		objSessionCtrlInfo.connErrorInfo = this.getSessionCtrlError();

		return objSessionCtrlInfo;
	},
	
	getSessionCtrlTimeMode: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlTimeMode", "call", "");
        var timeMode = "NONE";
        // set time mode based on session type
        var sessionType = this.getSessionType();
        if (sessionType == "LIVE") {
            // live by default
            timeMode = "LIVE";
            // look for PVR in the first src port entity
			if( this._isSessionCtrlPersistentPVRAvailable() ) {
				timeMode = "PERSISTENT_PVR";
			}
			else if( this._isSessionCtrlLivePVRAvailable() ) {
				timeMode = "PVR";
			}
        }
        else if (sessionType == "PLAYBACK") {
            timeMode = "PLAYBACK";
        }
        else { // MIXED
            timeMode = "LIVE";
        }
        return timeMode;
    },
    /*
    * Gets the SessionCtrl Time Info
    * @return {String}
    */
    getSessionCtrlTimeInfo: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlTimeInfo", "call", "");
        // get the id of the main SessionCtrl timeline
        var timelineId = this.getTimelineId();
        // get the action from it
        var objTimeInfo = new Object();
        objTimeInfo.timeScale = this.getTimelineTimeScale(timelineId);
        objTimeInfo.startOffset = this.getTimelineStartOffset(timelineId);
        objTimeInfo.startWC = this.getTimelineStartWC(timelineId);
        objTimeInfo.lastOffset = this.getTimelineLastOffset(timelineId);
        objTimeInfo.lastWC = this.getTimelineLastWC(timelineId);
        return objTimeInfo;
    },

    /*
    * Gets the SessionCtrlPVR Info
    * @return {String}
    */
    getSessionCtrlPvrInfo: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlPvrInfo", "call", "");
        var objPvrInfo = new Object();
        objPvrInfo.canPVR = this._isSessionCtrlPVRAvailable();
        objPvrInfo.timeLength = this.getSessionCtrlTimelength();
        objPvrInfo.sessionId = this.getPVRSessionId();
        objPvrInfo.dirId = this.getPVRDirId();
        objPvrInfo.startTC = this.getPVRStartTC();
        objPvrInfo.connId = this.getPVRConnId();
        objPvrInfo.pvrState = this._getSessionCtrlPVRState();

        return objPvrInfo;
    },

    _isSessionCtrlPVRAvailable: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_isSessionCtrlPVRAvailable", "call", "");
        var bSessionCtrlPVRAvailable = false;

        var sessionType = this.getSessionType();
		// if session is live - find the first pvr connection
		if (sessionType == "LIVE") {
			// in case of LIVE there are 2 cases:
            //1: PersistentPVR with sessionId
            //2: No persistent PVR and PVR Connection is there
			bSessionCtrlPVRAvailable = this._isSessionCtrlPersistentPVRAvailable() ||
									this._isSessionCtrlLivePVRAvailable();
        // if session is playback - pvr available by default
        }
		else if (sessionType == "PLAYBACK") {
            bSessionCtrlPVRAvailable = true;
        }
        else { // MIXED
            bSessionCtrlPVRAvailable = true;
        }

        return bSessionCtrlPVRAvailable;
    },
	_isSessionCtrlPersistentPVRAvailable: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_isSessionCtrlPersistentPVRAvailable", "call", "");
        var bSessionCtrlPersistentPVRAvailable = false;

        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            // look for PVR in the first src port entity
            var entityList = this.objSession.EntityList.Entity;
            if ((entityList) instanceof Array) {
                jQuery.each(entityList, function (key, entityNode) {
                    if (entityNode['@type'] == "SrcPort" && (entityNode.PVR['@enable'])) {
                        if (entityNode.PVR['@sessionId']) {
                            bSessionCtrlPersistentPVRAvailable = true;
							return false;
                        }
                    }
                })
            }
            else {
                if (entityList['@type'] == "SrcPort" && (entityList.PVR['@enable'])) {
                    if (entityList.PVR['@sessionId']) {
                        bSessionCtrlPersistentPVRAvailable = true;
                    }
                }
            }
		}
        return bSessionCtrlPersistentPVRAvailable;
    },
	_isSessionCtrlLivePVRAvailable: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_isSessionCtrlLivePVRAvailable", "call", "");
        var bSessionCtrlLivePVRAvailable = false;

        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            //this 2nd case
            var connList = this.objSession.ConnectionList.Connection;
            if ((connList) instanceof Array) {
                jQuery.each(connList, function (key, connNode) {
                    if (connNode['@type'] == "PVR") {
                        if (connNode['@state'] == "STREAMING") {
                            bSessionCtrlLivePVRAvailable = true;
						}
						return false;
                    }
                })
            }
        }
        return bSessionCtrlLivePVRAvailable;
    },
	_getSessionCtrlPVRState: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_getSessionCtrlPVRState", "call", "");
        var sessionCtrlPVRState = "NONE";
        var pvrConnNode = this.getConnection(this.getPVRConnId());
        if (pvrConnNode) {
            sessionCtrlPVRState = pvrConnNode['@state'];
        }
        return sessionCtrlPVRState;
    },
	
    getSessionCtrlTimelength: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlTimelength", "call", "");
        // use default for live and mixed sessions
        var defaultPlaylength = 4 * 3600000;
        var playlength = defaultPlaylength;
        // if playback session
        var sessionType = this.getSessionType();
        if (sessionType == "PLAYBACK") {
            // look for playlength in first plyFile entity
            var entityList = this.objSession.EntityList.Entity;
            if ((entityList) instanceof Array) {
                jQuery.each(entityList, function (key, entityNode) {
                    if (entityNode['@type'] == "PlyFile") {
                        playlength = entityNode.File['@playLength'];
                        return false;
                    }
                })
            }
        }
        else if (sessionType == "LIVE") {
            var entityList = this.objSession.EntityList.Entity;
            if ((entityList) instanceof Array) {
                jQuery.each(entityList, function (key, entityNode) {
                    if (entityNode['@type'] == "SrcPort") {
                        playlength = entityNode.PVR['@trimLength'];
                        return false;
                    }
                })
            }
        }
        if (playlength == null) playlength = defaultPlaylength;
        return playlength;
    },

    /*
    * Gets the main SessionCtrl state
    * @return {String}
    */
    getSessionCtrlState: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlState", "call", "");
        var sessionCtrlState = "NONE";
        // get the main SessionCtrl conn node
        var nodeConn = this._getSessionCtrlConn();
        // use it's state as the SessionCtrl state
        if (nodeConn) {
            sessionCtrlState = nodeConn['@state'];
            if (sessionCtrlState == 'WAITING_FOR_STREAMSTART' && this.getSessionCtrlActionState() != 'Start') {
                sessionCtrlState = 'STREAMING';
            }
        }
        return sessionCtrlState;
    },

    /*
    * Gets the main SessionCtrl Src entityId from Connection
    * @return {String}
    */
    getSessionCtrlSrcEntityId: function () {
      BarcoUtil.logger(bDEBUG+1, "", this.cls, "getSessionCtrlSrcEntityId", "call", "");
      var entityId = null;
      // get the main SessionCtrl conn node
      var nodeConn = this._getSessionCtrlConn();
      // use it's state as the SessionCtrl state
      if (nodeConn) {
        entityId = nodeConn['@srcEntityId'];
      }
      return entityId;
    },

  
    /*
    * Gets the main Session Entity name by entityId
    * @entityId {String}
    * @return {String}
    */
    getEntityNameById: function (entityId) {
      BarcoUtil.logger(bDEBUG+1, "", this.cls, "getEntityNameById", "call", entityId);
      var entityName = "";
      if (entityId) {
        var entityList = this.objSession.EntityList.Entity;
        jQuery.each(entityList, function (key, entityNode) {
          if (entityNode['@id'] == entityId) {
            entityName = (entityNode.File) ? entityNode.File['@resourceTitle'] : entityNode.Port['@resourceTitle'];
            return false;
          }
        })
      }
      return entityName;
    },

	  /*
    * Gets the main SessionCtrl connection
    * @return connId {String}
    */
    getSessionCtrlConnId: function () {
      BarcoUtil.logger(bDEBUG+1, "", this.cls, "getSessionCtrlConnId", "call", "");
        // get the SessionCtrl conn node
        var nodeConn = this._getSessionCtrlConn();
        // if found - return it's id
        if (nodeConn) {
            return nodeConn['@id'];
        }
        return null;
    },

    /*
    * Private method - gets the main SessionCtrl connection
    * @return nodeConn {Entity}
    */
    _getSessionCtrlConn: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_getSessionCtrlConn", "call", "");
        // depending on session type find the first live or playback connection
        var sessionCtrlConn = null;
        var sessionType = this.getSessionType();
        var connlist = this.objSession.ConnectionList.Connection;
        var connType = "VIEW";
        if (sessionType == "PLAYBACK") {
            connType = "PLAYBACK";
        }
        //Check if it has multiple connections which is array
        if ((connlist) instanceof Array) {
          jQuery.each(connlist, function (key, connNode) {
            if (connNode['@type'] == connType) {
              sessionCtrlConn = connNode;
              return false;
            }
          })
        }
        else {
          if (connlist['@type'] == connType) {
            sessionCtrlConn = connlist;
          }
        }
        return sessionCtrlConn;
    },
    /*
    * Gets the SessionCtrl Action state
    * @return {String}
    */
    getSessionCtrlActionState: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getSessionCtrlActionState", "call", "");
        // get the id of the main SessionCtrl timeline
        var timelineId = this.getTimelineId();
        // get the action from it
        var actionState = this.getTimelineRequestedAction(timelineId);
        // normalize all Pause/Resume variations (PauseAt, ResumeFrom etc..)
        actionState.match("/^Pause.*$/") && (actionState = "Pause");
        actionState.match("/^Resume.*$/") && (actionState = "Resume");

        return actionState;
    },
    /*
    * Gets the SessionCtrl Timeshift state
    * @return {Boolean}
    */
    isSessionCtrlTimeShifted: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "isSessionCtrlTimeShifted", "call", "");
        // get the id of the main SessionCtrl timeline
        var timelineId = this.getTimelineId();
        // check if it is timeshifted it
        return this.getTimelineIsTimeshifted(timelineId);
    },
 
    /*
    * Gets the SessionCtrl Record state
    * @return {String}
    */
    getSessionCtrlRecordState: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "getSessionCtrlRecordState", "call", "");
        var sessionCtrlRecordState = "NONE";
        // find the first record connection
        // use it's state as the SessionCtrl state
        var nodeConn = this._getSessionCtrlRecordConn();
        if (nodeConn) {
            sessionCtrlRecordState = nodeConn['@state']; ;
            // *HACK* convert WAITING_FOR_STREAMSTART + PVRaction = STREAMING
            if (sessionCtrlRecordState == 'WAITING_FOR_STREAMSTART' && this.getSessionCtrlRecordActionState() != 'Start') {
                sessionCtrlRecordState = 'STREAMING';
            }
        }
        return sessionCtrlRecordState;
    },
    /*
    * Gets the SessionCtrl Record error
    * @return connId {String}
    */
    getSessionCtrlRecordErrorData: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlRecordError", "call", "");
        // get the SessionCtrl conn node
        var nodeConn = this._getSessionCtrlRecordConn();
        // if found - return it's errorData
        if (nodeConn && nodeConn['@errorData']) {
            return nodeConn['@errorData'];
        }
        return null;
    },
    /*
    * Gets the SessionCtrl Record connection
    * @return connId {String}
    */
    getSessionCtrlRecordConnId: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlRecordConnId", "call", "");
        // get the SessionCtrl conn node
        var nodeConn = this._getSessionCtrlRecordConn();
        // if found - return it's id
        if (nodeConn) {
            return nodeConn['@id'];
        }
        return null;
    },
    /*
    * Private method - gets the main SessionCtrl connection
    * @return nodeConn {Entity}
    */
    _getSessionCtrlRecordConn: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_getSessionCtrlRecordConn", "call", "");
        // depending on session type find the first live or playback connection
        var recConn = null;
        var connlist = this.objSession.ConnectionList.Connection;
        //Check if it has multiple connections which is array
        jQuery.each(connlist, function (key, connNode) {
            if (connNode['@type'] == "RECORD") {
                recConn = connNode;
                return false;
            }
        })
        return recConn;
    },
    /*
    * Gets the SessionCtrl Record Action state
    * @return {String}
    */
    getSessionCtrlRecordActionState: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlRecordActionState", "call", "");
        // get the id of the record SessionCtrl timeline
        var timelineId = this.getRecordTimelineId();
        // get the action from it
        return this.getTimelineRequestedAction(timelineId);
    },
    /*
    * Gets the isSessionCtrlBookmarkAvailable
    * @return {Boolean}
    */
    isSessionCtrlBookmarkAvailable: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "isSessionCtrlBookmarkAvailable", "call", "");
        var bSessionCtrlBookmarkAvailable = false;
        var sessionType = this.getSessionType();
        // if session is live - find the first pvr connection
        if (sessionType == "LIVE") {
            var connState = this.getSessionCtrlRecordState();
            if (connState == "STREAMING" || (connState == "WAITING_FOR_STREAMSTART" && this.getSessionCtrlRecordActionState() == "Pause")) {
                bSessionCtrlBookmarkAvailable = true;
            }
        }
        // if session is playback - pvr available by default
        else if (sessionType == "PLAYBACK") {
            bSessionCtrlBookmarkAvailable = true;
        }
        else { // MIXED
            bSessionCtrlBookmarkAvailable = true;
        }

        return bSessionCtrlBookmarkAvailable;
    },
    /*
    * Gets the SessionCtrl bookmark fileNID connection
    * @return connId {String}
    */
    getSessionCtrlBookmarkFileNID: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlBookmarkFileNID", "call", "");
        var fileNID = "";
        var sessionType = this.getSessionType();
        var entityType = (sessionType == "PLAYBACK") ? "PlyFile" : "RecFile";
        var entityList = this.objSession.EntityList.Entity;
        jQuery.each(entityList, function (key, entityNode) {
            if (entityNode['@type'] == entityType) {
                fileNID = entityNode.File['@resourceNID'];
				        return false;
            }
        })
        return fileNID;
    },
    /*
    * Gets the main SessionCtrl Error details
    * @return {objSessionCtrlError} JSON/null
    */
    getSessionCtrlError: function () {
        BarcoUtil.logger(bDEBUG, "", this.cls, "getSessionCtrlError", "call", "");
        var objSessionCtrlError = null;
        // get the main SessionCtrl conn node
        var nodeConn = this._getSessionCtrlConn();
        // use it's error details as the SessionCtrl error
        if (nodeConn) {
            objSessionCtrlError = this.getConnError(nodeConn);

        }
        return objSessionCtrlError;
    },
    
    //-------End  SessionCtrl--------

    ///*
    // * Internal utility function to get new empty Session
    // * @return {jsx3.xml.Entity} nodeSession
    // */
    //_getNewSession:function() {
    //  // get the template from the cache OR model/objectXml
    //  var nodeSession = Session._getSessionObject();

    //  // clean out id,
    //  nodeSession.setAttribute("id", "");
    //  // clean out timelineId,
    //  nodeSession.setAttribute("timelineId", "");
    //  // clean out recordTimelineId,
    //  nodeSession.setAttribute("recordTimelineId", "");
    //  // clean out all TimelineList
    //  var nodeTimelineList = nodeSession.selectSingleNode(".//TimelineList");
    //  nodeTimelineList.removeChildren();
    //  // clean out all dirs
    //  var nodeDirList = nodeSession.selectSingleNode(".//DirList");
    //  nodeDirList.removeChildren();
    //  // clean out all grpfiles
    //  var nodeGrpFileList = nodeSession.selectSingleNode(".//GrpFileList");
    //  nodeGrpFileList.removeChildren();
    //  // clean out all entities
    //  var nodeEntityList = nodeSession.selectSingleNode(".//EntityList");
    //  nodeEntityList.removeChildren();
    //  // clean out all profiles
    //  var nodeProfileList = nodeSession.selectSingleNode(".//ProfileList");
    //  nodeProfileList.removeChildren();
    //  // clean out all connections
    //  var nodeConnList = nodeSession.selectSingleNode(".//ConnectionList");
    //  nodeConnList.removeChildren();

    //  return nodeSession;
    //}

    ///*
    //* GSets the SessionInfo date created
    //* @param strName {String}
    //*/
    //getDateCreated: function() {
    //  this.selectSingleNode(".//Properties/DateCreated").getValue();
    //},
    ///*
    //* Sets the SessionInfo date created
    //* @param strName {String}
    //*/
    //setDateCreated: function() {
    //  var dateCreated = new Date().getTime();
    //  this.selectSingleNode(".//Properties/DateCreated").setValue(dateCreated);
    //},
    ///*
    //* Updates the SessionInfo Type based on the src entity types of the connections
    //* @return {String}
    //*/
    //_updateSessionType: function() {
    //  // get the current SessionInfo type to start with
    //  var sessionType = this.getSessionType();

    //  // iterate over all the connections
    //  var iterConn = this.getConnIter();
    //  while(iterConn.hasNext()) {
    //    var nodeConn = iterConn.next();
    //    // check the src entity type
    //    var srcEntityId = this.getConnSrcEntityId(nodeConn);
    //    if(jsx3.util.strEmpty(srcEntityId)) continue;
    //    var srcEntityType = this.getEntityType(srcEntityId);
    //    // if it is a live port - set to LIVE / MIXED
    //    if(srcEntityType == SessionInfo.ENTITYTYPE_SRCPORT) {
    //      if(jsx3.util.strEmpty(sessionType)) {
    //        sessionType = SessionInfo.SESSIONTYPE_LIVE;
    //      }
    //      else if(sessionType == SessionInfo.SESSIONTYPE_PLAYBACK) {
    //        sessionType = SessionInfo.SESSIONTYPE_MIXED;
    //        // once mixed - no need to look anymore
    //        break;
    //      }
    //    }
    //      // if it is a file - set to PLAYBACK / MIXED
    //    else if(srcEntityType == SessionInfo.ENTITYTYPE_PLYFILE) {
    //      if(jsx3.util.strEmpty(sessionType)) {
    //        sessionType = SessionInfo.SESSIONTYPE_PLAYBACK;
    //      }
    //      else if(sessionType == SessionInfo.SESSIONTYPE_LIVE) {
    //        sessionType = SessionInfo.SESSIONTYPE_MIXED;
    //        // once mixed - no need to look anymore
    //        break;
    //      }
    //    }
    //      // others - do nothing for now - TBD
    //    else {
    //    }
    //  }

    //  // finally set the new SessionInfo type
    //  this.setSessionType(sessionType);

    //  return sessionType;
    //},

    ///*
    //* Gets the DefaultRec Dir Id
    //* @return objDir {Object}
    //*/
    //getDefaultRecDir: function() {
    //  return this.getDirObj(SessionInfo.DIRID_DEFAULTREC);
    //},
    ///*
    //* Sets the DefaultRec Dir - adds a dir node if not exist already
    //* @param objDir {Object}
    //*/
    //setDefaultRecDir: function(objDir) {
    //  // try to update the default rec dir
    //  if(!this.updateDir(SessionInfo.DIRID_DEFAULTREC, objDir)) {
    //    // if not exist - add it
    //    objDir.id = SessionInfo.DIRID_DEFAULTREC;
    //    this.addDir(objDir);
    //    // update any entities that have blank dirid
    //    var objFile = new Object();
    //    objFile.dirId = objDir.id;
    //    var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
    //    while(iterRecFileEntity.hasNext()) {
    //      var nodeEntity = iterRecFileEntity.next();
    //      this._updateEntityNode(nodeEntity, objFile);
    //    }
    //  }
    //},
    ///*
    //* Clears the DefaultRec Dir - deletes the dir node and clears refs in entities
    //* @return
    //*/
    //clrDefaultRecDir: function() {
    //  if(this.getDirObj(SessionInfo.DIRID_DEFAULTREC)) {
    //    // if exist - delete it
    //    this.delDir(SessionInfo.DIRID_DEFAULTREC);
    //    // update any entities that have this dirid
    //    var objFile = new Object();
    //    objFile.dirId = "";
    //    var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
    //    while(iterRecFileEntity.hasNext()) {
    //      var nodeEntity = iterRecFileEntity.next();
    //      this._updateEntityNode(nodeEntity, objFile);
    //    }
    //  }
    //},
    ///*
    //* Adds a Dir to the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} dirTitle
    //* @param {String} dirNID
    //* @return {String} dirId
    //*/
    //addDir: function(objDir) {
    //  // get a new dir node
    //  var nodeDir = this._getNewDirNode();
    //  // setup a new dir id - if needed
    //  if(!objDir.id) {
    //    objDir.id = SessionInfo.DIR_IDPREFIX + getUUID();
    //  }

    //  // setup values in this dir - if err return
    //  if(!this._updateDirNode(nodeDir, objDir)) {
    //    return null;
    //  }

    //  // if success - append the new node to the list  
    //  var nodeDirList = this.selectSingleNode(".//DirList");
    //  nodeDirList.appendChild(nodeDir);

    //  // return the new dir id 
    //  return objDir.id;
    //},
    ///*
    //* Updates a Dir to the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} dirId
    //* @param {Object} objDir
    //* @return {String} dirId
    //*/
    //updateDir: function(dirId, objDir) {
    //  // get the dir Node
    //  var nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    //  if(!nodeDir) return false;

    //  // setup values in this dir - if err return
    //  if(!this._updateDirNode(nodeDir, objDir)) {
    //    return null;
    //  }

    //  // if success - append the new node to the list  
    //  var nodeDirList = this.selectSingleNode(".//DirList");
    //  nodeDirList.appendChild(nodeDir);

    //  // return the new dir id 
    //  return dirId;
    //},
    ///*
    //* Deletes a Dir from the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} dirId
    //* @return {Boolean}
    //*/
    //delDir: function(dirId) {
    //  // get the dir Node
    //  var nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    //  if(!nodeDir) return false;

    //  // delete it from its parent node
    //  nodeDir.getParent().removeChild(nodeDir);

    //  // return success
    //  return true;
    //},
    ///*
    //* GCs a Dir from the SessionInfo if not used anymore.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} dirId
    //* @return {Boolean}
    //*/
    //_gcDir: function(dirId) {
    //  // check for use of the dir Id
    //  var nodeDir = this.selectSingleNode(".//*[@dirId='" + dirId + "']");
    //  if(nodeDir) return false;

    //  // check if there are any files related to this dir
    //  var nodeEntityFile = this.selectSingleNode(".//Entity/File[@dirId='" + dirId + "']");
    //  // if none left - delete the dir
    //  if(!nodeEntityFile) {
    //    nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    //    nodeDir.getParent().removeChild(nodeDir);
    //    return true;
    //  }
    //  // return false to indicate it still exists
    //  return false;
    //},
    ///*
    //* Helper function to update dir
    //* @param nodeDir {jsx3.xml.Entity}
    //* @param objDir {Object}
    //* @return {String} dirId
    //*/
    //_updateDirNode: function(nodeDir, objDir) {
    //  if(objDir.dirTitle) {
    //    nodeDir.setAttribute("resourceTitle", objDir.dirTitle);
    //  }
    //  if(objDir.dirNID) {
    //    nodeDir.setAttribute("resourceNID", objDir.dirNID);
    //  }
    //  if(objDir.id) {
    //    nodeDir.setAttribute("id", objDir.id);
    //  }
    //  // return id
    //  return nodeDir.getAttribute("id");
    //},
    ///*
    //* Helper function to check dir existence
    //* @param dirTitle {String} and/or dirNID {String}
    //* @return {String} dirId
    //*/
    //getDir: function(dirTitle, dirNID, dirId) {
    //  // form xpath to look for entity node
    //  var whereXPath = "";
    //  if(dirTitle) {
    //    whereXPath += "./@resourceTitle='" + dirTitle + "'";
    //  }
    //  if(dirNID) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@resourceNID='" + dirNID + "'";
    //  }
    //  if(dirId) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@id='" + dirId + "'";
    //  }
    //  // get the dir node
    //  var nodeDir = this.selectSingleNode(".//Dir[" + whereXPath + "]");
    //  // if not found return null
    //  if(!nodeDir) {
    //    return null;
    //  }
    //  // else return id
    //  return nodeDir.getAttribute("id");
    //},
    ///*
    //* Gets a Dir obj from the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} dirId
    //* @return {Object}
    //*/
    //getDirObj: function(dirId) {
    //  // get the dir Node
    //  var nodeDir = this.selectSingleNode(".//Dir[@id='" + dirId + "']");
    //  if(!nodeDir) return null;

    //  // get info from dir node
    //  var objDir = new Object();
    //  objDir.id = nodeDir.getAttribute("id");
    //  objDir.dirNID = nodeDir.getAttribute("resourceNID");
    //  objDir.dirTitle = nodeDir.getAttribute("resourceTitle");

    //  return objDir;
    //},
    ///*
    //* Gets the DefaultRec grpFile Id
    //* @return objGrpFile {Object}
    //*/
    //getDefaultRecGrpFile: function() {
    //  return this.getGrpFileObj(SessionInfo.GRPFILEID_DEFAULTREC);
    //},
    ///*
    //* Sets the DefaultRec grpFile - adds a grpFile node if not exist already
    //* @param objGrpFile {Object}
    //*/
    //setDefaultRecGrpFile: function(objGrpFile) {
    //  // try to update the default rec grpfile
    //  if(!this.updateGrpFile(SessionInfo.GRPFILEID_DEFAULTREC, objGrpFile)) {
    //    objGrpFile.id = SessionInfo.GRPFILEID_DEFAULTREC;
    //    objGrpFile.grpFileType = SessionInfo.GRPFILETYPE_SYNCREC;
    //    this.addGrpFile(objGrpFile);
    //    // update any entities that have blank grpfileid
    //    var objFile = new Object();
    //    objFile.grpFileId = objGrpFile.id;
    //    var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
    //    while(iterRecFileEntity.hasNext()) {
    //      var nodeEntity = iterRecFileEntity.next();
    //      this._updateEntityNode(nodeEntity, objFile);
    //    }
    //  }
    //},
    ///*
    //* Updates the DefaultRec grpFile 
    //* @param objGrpFile {Object}
    //*/
    //updateDefaultRecGrpFile: function(objGrpFile) {
    //  this.updateGrpFile(SessionInfo.GRPFILEID_DEFAULTREC, objGrpFile);
    //},
    ///*
    //* Clears the DefaultRec grpFile - deletes the grpFile node and clears refs in entities
    //* @return
    //*/
    //clrDefaultRecGrpFile: function() {
    //  if(this.getGrpFileObj(SessionInfo.GRPFILEID_DEFAULTREC)) {
    //    // if exist - delete it
    //    this.delGrpFile(SessionInfo.GRPFILEID_DEFAULTREC);
    //    // update any entities that have this grpfileid
    //    var objFile = new Object();
    //    objFile.grpFileId = "";
    //    var iterRecFileEntity = this.getEntityIterByType(SessionInfo.ENTITYTYPE_RECFILE);
    //    while(iterRecFileEntity.hasNext()) {
    //      var nodeEntity = iterRecFileEntity.next();
    //      this._updateEntityNode(nodeEntity, objFile);
    //    }
    //  }
    //},


    ///*
    //* Updates a GrpFile to the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {Object} objGrpFile
    //* @return {String} grpFileId
    //*/
    //updateGrpFile: function(grpFileId, objGrpFile) {
    //  // get the GrpFile node
    //  var nodeGrpFile = this.selectSingleNode(".//GrpFile[@id='" + grpFileId + "']");
    //  if(!nodeGrpFile) return false;

    //  // setup values in this grpFile - if err return
    //  if(!this._updateGrpFileNode(nodeGrpFile, objGrpFile)) {
    //    return null;
    //  }

    //  // return the new grpFile id 
    //  return grpFileId;
    //},
    ///*
    //* Deletes a GrpFile from the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} grpFileId
    //* @return {Boolean}
    //*/
    //delGrpFile: function(grpFileId) {
    //  // get the GrpFile node
    //  var nodeGrpFile = this.selectSingleNode(".//GrpFile[@id='" + grpFileId + "']");
    //  if(!nodeGrpFile) return false;

    //  // delete it from its parent node
    //  nodeGrpFile.getParent().removeChild(nodeGrpFile);

    //  // return success
    //  return true;
    //},
    ///*
    //* GCs a GrpFile from the SessionInfo if not used anymore.
    //* Typically not called directly. (called from add/updateEntity/deleteEntity).
    //* @param {String} grpFileId
    //* @return {Boolean}
    //*/
    //_gcGrpFile: function(grpFileId) {
    //  // check for use of the grpFile Id
    //  var nodeGrpFile = this.selectSingleNode(".//*[@id='" + grpFileId + "']");
    //  if(!nodeGrpFile) return false;

    //  // check if there are any files related to this grpfile
    //  var nodeEntityFile = this.selectSingleNode(".//Entity/File[@grpFileId='" + grpFileId + "']");
    //  // if none left - delete the grpfile
    //  if(!nodeEntityFile) {
    //    nodeGrpFile.getParent().removeChild(nodeGrpFile);
    //    return true;
    //  }
    //  // return false to indicate it still exists
    //  return false;
    //},
    ///*
    //* Helper function to update grpFile
    //* @param nodeGrpFile {jsx3.xml.Entity}
    //* @param objGrpFile {Object}
    //* @return {String} grpFileId
    //*/
    //_updateGrpFileNode: function(nodeGrpFile, objGrpFile) {
    //  if(objGrpFile.grpFileTitle) {
    //    nodeGrpFile.setAttribute("resourceTitle", objGrpFile.grpFileTitle);
    //  }
    //  if(objGrpFile.grpFileNID) {
    //    nodeGrpFile.setAttribute("resourceNID", objGrpFile.grpFileNID);
    //  }
    //  if(objGrpFile.grpFileType) {
    //    nodeGrpFile.setAttribute("type", objGrpFile.grpFileType);
    //  }
    //  if(objGrpFile.id) {
    //    nodeGrpFile.setAttribute("id", objGrpFile.id);
    //  }
    //  // return id
    //  return nodeGrpFile.getAttribute("id");
    //},
    ///*
    //* Helper function to check grpFile existence
    //* @param grpFileTitle {String} and/or grpFileNID {String}
    //* @return {String} grpFileId
    //*/
    //getGrpFile: function(grpFileTitle, grpFileNID, grpFileId) {
    //  // form xpath to look for entity node
    //  var whereXPath = "";
    //  if(grpFileTitle) {
    //    whereXPath += "./@resourceTitle='" + grpFileTitle + "'";
    //  }
    //  if(grpFileNID) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@resourceNID='" + grpFileNID + "'";
    //  }
    //  if(grpFileId) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@id='" + grpFileId + "'";
    //  }
    //  // get the grpFile node
    //  var nodeGrpFile = this.selectSingleNode(".//GrpFile[" + whereXPath + "]");
    //  // if not found return null
    //  if(!nodeGrpFile) {
    //    return null;
    //  }
    //  // else return id
    //  return nodeGrpFile.getAttribute("id");
    //},

    ///*
    //* Helper function to get the grpFileId from the entity
    //* @param entityId {String}
    //* @return {String} grpFileId
    //*/
    //getGrpFileId: function(entityId) {
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  var grpFileId = nodeEntity.selectSingleNode(".//File").getAttribute("grpFileId");
    //  if(!grpFileId) return null;
    //  return grpFileId;
    //},

    ///*
    //* Gets a GrpFile obj from the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {String} grpFileId
    //* @return {Object}
    //*/
    //getGrpFileObj: function(grpFileId) {
    //  // get the GrpFile node
    //  var nodeGrpFile = this.selectSingleNode(".//GrpFile[@id='" + grpFileId + "']");
    //  if(!nodeGrpFile) return null;

    //  // get info from grpFile node
    //  var objGrpFile = new Object();
    //  objGrpFile.id = nodeGrpFile.getAttribute("id");
    //  objGrpFile.grpFileType = nodeGrpFile.getAttribute("type");
    //  objGrpFile.grpFileNID = nodeGrpFile.getAttribute("resourceNID");
    //  objGrpFile.grpFileTitle = nodeGrpFile.getAttribute("resourceTitle");

    //  return objGrpFile;
    //},
    ///*
    //* Internal utility function to get new grpFile nodes
    //* @return {jsx3.xml.Entity} nodeGrpFile
    //*/
    //_getNewGrpFileNode: function(grpFileType) {
    //  // get a template from the cache OR model/objectXml
    //  var nodeSession = this._getSessionObject();

    //  // get a grpFile node
    //  var nodeGrpFile = nodeSession.selectSingleNode(".//GrpFile").cloneNode(true);
    //  // set the grpFileType
    //  nodeGrpFile.setAttribute("type", grpFileType);

    //  return nodeGrpFile;
    //},
    ///*
    //* Adds a GrpFile to the SessionInfo.
    //* Typically not called directly. (called from add/updateEntity).
    //* @param {Object} objGrpFile
    //* @return {String} grpFileId
    //*/
    //addGrpFile: function(objGrpFile) {
    //  // get a new grpFile node
    //  var nodeGrpFile = this._getNewGrpFileNode(objGrpFile.grpFileType);
    //  // setup a new grpFile id - if needed
    //  if(!objGrpFile.id) {
    //    objGrpFile.id = SessionInfo.GRPFILE_IDPREFIX + getUUID();
    //  }

    //  // setup values in this grpFile - if err return
    //  if(!this._updateGrpFileNode(nodeGrpFile, objGrpFile)) {
    //    return null;
    //  }

    //  // if success - append the new node to the list  
    //  var nodeGrpFileList = this.selectSingleNode(".//GrpFileList");
    //  nodeGrpFileList.appendChild(nodeGrpFile);

    //  // return the new grpFile id 
    //  return objGrpFile.id;
    //},
    ///*
    //* Internal utility function to get new dir nodes
    //* @return {jsx3.xml.Entity} nodeEntity
    //*/
    //_getNewDirNode: function() {
    //  // get a template from the cache OR model/objectXml
    //  var nodeSession = this._getSessionObject();

    //  // get the default dir node
    //  var nodeDir = nodeSession.selectSingleNode(".//Dir").cloneNode(true);

    //  return nodeDir;
    //},

    ///*
    //* Adds an entity to the SessionInfo
    //* @param {Object} objEntity
    //* @return {String} entityId
    //*/
    //addEntity: function(objEntity) {
    //  // create a new entity node based on type
    //  var nodeEntity = SessionInfo._getNewEntityNode(objEntity.type);
    //  if(!nodeEntity) return null;

    //  // generate a new entityId - if needed
    //  var entityId = objEntity.id;
    //  if(!entityId) {
    //    entityId = objEntity.type + '_' + getUUID();
    //    objEntity.id = entityId;
    //  }

    //  // set the values in the new node 
    //  this._updateEntityNode(nodeEntity, objEntity);

    //  // append it to the list in the current Session xml in the topic
    //  var nodeEntityList = this.selectSingleNode(".//EntityList");
    //  nodeEntityList.appendChild(nodeEntity);

    //  // return the id of the new entity
    //  return entityId;
    //},
    ///*
    //* Deletes an Entity from the SessionInfo.
    //* Typically not called directly. (called from add/updateConnGroup).
    //* @param {String} entityId
    //* @return {Boolean}
    //*/
    //delEntity: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return false;

    //  // find the entity type.
    //  var entityType = this.getEntityType(entityId);
    //  var grpFileId = null;
    //  //  If its PlyFile then get the grpFileId
    //  if(entityType == SessionInfo.ENTITYTYPE_PLYFILE) {
    //    grpFileId = this.getGrpFileId(entityId);
    //  }

    //  // delete it from its parent node
    //  nodeEntity.getParent().removeChild(nodeEntity);

    //  // If its PlyFile and the grpFileId is not "" or null then do gcGrpFile
    //  if((entityType == SessionInfo.ENTITYTYPE_PLYFILE) && (!jsx3.util.strEmpty(grpFileId))) {
    //    this._gcGrpFile(grpFileId);
    //  }

    //  // return success
    //  return true;
    //},
    ///*
    //* Updates an entity in the SessionInfo
    //* @param {String} entityId
    //* @param {Object} objEntity
    //* @return {Boolean}
    //*/
    //updateEntity: function(entityId, objEntity) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return false;

    //  // update entity node
    //  return this._updateEntityNode(nodeEntity, objEntity);
    //},
    ///*
    //* GCs an Entity from the SessionInfo if not used anymore.
    //* Typically not called directly. (called from add/updateConn).
    //* @param {String} entityId
    //* @return {Boolean}
    //*/
    //_gcEntity: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return true;

    //  // check if there are any conns using this entity
    //  var nodeConn = this.selectSingleNode(".//Connection[@srcEntityId='" + entityId + " or " + "@dstEntityId='" + entityId + "']");
    //  // if none left - delete the entity
    //  if(!nodeConn) {
    //    nodeEntity.getParent().removeChild(nodeEntity);
    //    return true;
    //  }
    //  // return false to indicate it still exists
    //  return false;
    //},
    ///*
    //* Checks the entity in the Connection List.
    //* If entityId is used in any of the connections then returns true else false
    //* @param (String) entityId
    //*/
    //checkEntityInConnectionList: function(entityId) {
    //  // check if entity is peresnet in any of the connection
    //  if(this.selectSingleNode(".//Connection[@dstEntityId='" + entityId + "'" +
    //                                           " or @srcEntityId='" + entityId + "']")) {
    //    return true;
    //  }
    //  return false;
    //},
    ///*
    //* Internal method to updates an entity Node in the SessionInfo
    //* @param {jsx3.xml.Entity} nodeEntity
    //* @param {Object} objEntity
    //* @return {Boolean}
    //*/
    //_updateEntityNode: function(nodeEntity, objEntity) {
    //  // set the values in the node - skip anything unspecified
    //  if(objEntity.id) nodeEntity.setAttribute("id", objEntity.id);
    //  if(objEntity.name) nodeEntity.setAttribute("name", objEntity.name);
    //  if(objEntity.streamType) nodeEntity.setAttribute("streamType", objEntity.streamType);
    //  if(objEntity.presence) nodeEntity.setAttribute("presence", objEntity.presence);

    //  var nodeEntityUser = nodeEntity.selectSingleNode("./User");
    //  if(nodeEntityUser) {
    //    if(objEntity.userNID) nodeEntityUser.setAttribute("userNID", objEntity.userNID);
    //    if(objEntity.userJID) nodeEntityUser.setAttribute("userJID", objEntity.userJID);
    //    if(objEntity.userTitle) {
    //      nodeEntityUser.setAttribute("username", objEntity.userTitle);
    //      if(!objEntity.name) nodeEntity.setAttribute("name", objEntity.userTitle);
    //    }
    //  }
    //  var nodeEntityPort = nodeEntity.selectSingleNode("./Port");
    //  if(nodeEntityPort) {
    //    if(objEntity.portNID) nodeEntityPort.setAttribute("resourceNID", objEntity.portNID);
    //    if(objEntity.portTitle) {
    //      nodeEntityPort.setAttribute("resourceTitle", objEntity.portTitle);
    //      if(!objEntity.name) nodeEntity.setAttribute("name", objEntity.portTitle);
    //    }
    //    if(objEntity.maxInstanceCount) {
    //      var nodeInstanceList = nodeEntityPort.selectSingleNode("./InstanceList");
    //      if(nodeInstanceList) nodeInstanceList.setAttribute("maxInstanceCount", objEntity.maxInstanceCount);
    //    }
    //  }

    //  var nodeEntityPVR = nodeEntity.selectSingleNode("./PVR");
    //  if (nodeEntityPVR && !jsx3.util.strEmpty(objEntity.PVREnable)) {
    //    nodeEntityPVR.setAttribute("enable", objEntity.PVREnable);
    //  }
    //  var nodeEntityFile = nodeEntity.selectSingleNode("./File");
    //  if(nodeEntityFile) {
    //    // update file
    //    if(objEntity.fileNID) nodeEntityFile.setAttribute("resourceNID", objEntity.fileNID);
    //    if(objEntity.fileTitle) {
    //      nodeEntityFile.setAttribute("resourceTitle", objEntity.fileTitle);
    //      if(!objEntity.name) nodeEntity.setAttribute("name", objEntity.fileTitle);
    //    }
    //    // update dir
    //    if(objEntity.dirId || objEntity.dirTitle || objEntity.dirNID) {
    //      // save the current val of dirId
    //      var oldDirId = nodeEntityFile.getAttribute("dirId");
    //      // check and reuse dir if already exists
    //      objEntity.dirId = this.getDir(objEntity.dirTitle, objEntity.dirNID, objEntity.dirId);
    //      if(!objEntity.dirId) {
    //        // if not exist - add it to dir list
    //        var objDir = new Object;
    //        objDir.id = objEntity.dirId;
    //        objDir.dirTitle = objEntity.dirTitle;
    //        objDir.dirNID = objEntity.dirNID;

    //        objEntity.dirId = this.addDir(objDir);
    //      }
    //      nodeEntityFile.setAttribute("dirId", objEntity.dirId);
    //      // finally GC the old dir if needed
    //      if(!jsx3.util.strEmpty(oldDirId)) {
    //        this._gcDir(oldDirId);
    //      }
    //    }
    //    // update grp file
    //    if(objEntity.grpFileId || objEntity.grpFileTitle || objEntity.grpFileNID) {
    //      // save the current val of grpFileId
    //      var oldGrpFileId = nodeEntityFile.getAttribute("grpFileId");
    //      // check and reuse grpfile if already exists
    //      objEntity.grpFileId = this.getGrpFile(objEntity.grpFileTitle, objEntity.grpFileNID, objEntity.grpFileId);
    //      if(!objEntity.grpFileId) {
    //        // if not exist - add it to grpfile list - only if either name or title is specified
    //        if(objEntity.grpFileTitle || objEntity.grpFileNID) {
    //          if(!objEntity.grpFileType) {
    //            if(nodeEntity.getAttribute("type") == SessionInfo.ENTITYTYPE_RECFILE) {
    //              objEntity.grpFileType = SessionInfo.GRPFILETYPE_SYNCREC;
    //            }
    //            else {
    //              objEntity.grpFileType = SessionInfo.GRPFILETYPE_SYNCPLY;
    //            }
    //          }
    //          objEntity.grpFileId = this.addGrpFile(objEntity.grpFileType, objEntity.grpFileTitle, objEntity.grpFileNID, objEntity.grpFileId);
    //        }
    //        //TBD - log error here : grpfileId specified for non existent grp file
    //      }
    //      nodeEntityFile.setAttribute("grpFileId", objEntity.grpFileId);
    //      // finally GC the old grpfile if needed
    //      if(!jsx3.util.strEmpty(oldGrpFileId)) {
    //        this._gcGrpFile(oldGrpFileId);
    //      }
    //    }
    //    // update clipId
    //    if(objEntity.clipId) {
    //      if(jsx3.util.strEmpty(objEntity.clipId)) {
    //        objEntity.clipId = SessionInfo.ENTITYTYPE_RECFILE + "_" + getUUID();
    //      }
    //      nodeEntityFile.setAttribute("clipId", objEntity.clipId);
    //    }
    //    // update the recSessionConfigNID if its there()
    //    // generally it will be there if plyfile is added as an entity
    //    if(objEntity.recSessionConfigNID) {
    //      if(!jsx3.util.strEmpty(objEntity.recSessionConfigNID)) {
    //        nodeEntityFile.setAttribute("recSessionConfigNID", objEntity.recSessionConfigNID);
    //      }
    //    }
    //  }

    //  // return success
    //  return true;
    //},
    ///*
    //* Adds an Instance to an Entity.
    //* Checks to see if maxInstanceCount is exceeded before adding instance
    //* (TBD: Currently uses external function to get new Instance node)
    //* @param {String} entityId
    //* @param {String} instanceId (optional)
    //* @return {String} instanceId
    //*/
    //addEntityInstance: function(entityId, instanceId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  // generate a new InstanceId - if needed
    //  if(!instanceId) instanceId = entityId + "::" + SessionInfo.INSTANCE_IDPREFIX + getUUID();

    //  // check maxInstanceCount
    //  var maxInstanceCount = nodeEntity.selectSingleNode("./*/InstanceList/@maxInstanceCount").getValue();
    //  if(!jsx3.util.strEmpty(maxInstanceCount)) {
    //    var currInstanceCount = nodeEntity.selectNodes(".//Instance").size();
    //    if(currInstanceCount >= maxInstanceCount) {
    //      return null;
    //    }
    //  }
    //  // create a new entity Instance node 
    //  var nodeInstance = SessionInfo._getNewEntityInstanceNode();
    //  if(!nodeInstance) return null;

    //  // set the value in the new node
    //  nodeInstance.selectSingleNode("./@id").setValue(instanceId ? instanceId : "");

    //  // append it to the instance list in the entity node
    //  var nodeInstanceList = nodeEntity.selectSingleNode(".//InstanceList");
    //  nodeInstanceList.appendChild(nodeInstance);

    //  // return the id of the new instance
    //  return instanceId;
    //},
    ///*
    //* Gets an Iterator over the Entities
    //* 
    //* @return {jsx3.util.Iterator} Entity Nodes
    //*/
    //getEntityIter: function() {
    //  return this.selectNodeIterator(".//Entity");
    //},
    ///*
    //* Gets an Iterator over the Entities of the specified type
    //* @param entityType {String}
    //* @return {jsx3.util.Iterator} Entity Nodes
    //*/
    //getEntityIterByType: function(entityType) {
    //  var whereXPath = "";
    //  if(entityType) {
    //    whereXPath = "@type='" + entityType + "'";
    //  }
    //  return this.selectNodeIterator(".//Entity[" + whereXPath + "]");
    //},

    ///*
    //* Helper function to check entity existence
    //* @param entityType {String} and/or entityNID {String}
    //* @return {String} entityId
    //*/
    //// 
    //getEntity: function(entityType, entityNID, entityId) {
    //  // form xpath to look for entity node
    //  var whereXPath = "";
    //  if(entityType) {
    //    whereXPath += "./@type='" + entityType + "'";
    //  }
    //  if(entityType == SessionInfo.ENTITYTYPE_USER) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./User/@resourceNID='" + entityNID + "'";
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_SRCPORT ||
    //            entityType == SessionInfo.ENTITYTYPE_DSTPORT ||
    //            entityType == SessionInfo.ENTITYTYPE_RELAYPORT ||
    //            entityType == SessionInfo.ENTITYTYPE_STOREPORT) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./Port/@resourceNID='" + entityNID + "'";
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_PLYFILE ||
    //            entityType == SessionInfo.ENTITYTYPE_RECFILE ||
    //            entityType == SessionInfo.ENTITYTYPE_PVRFILE) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./File/@resourceNID='" + entityNID + "'";
    //  }
    //  if(entityId) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@id='" + entityId + "'";
    //  }
    //  // get the entity node
    //  var nodeEntity = this.selectSingleNode(".//Entity[" + whereXPath + "]");
    //  // if not found return null
    //  if(!nodeEntity) {
    //    return null;
    //  }
    //  // else return id
    //  return nodeEntity;
    //},

    ///*
    //* Gets the id of an entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityId: function(nodeEntity) {
    //  return nodeEntity.getAttribute("id");
    //},
    ///*
    //* Gets the name of an entity
    //* @ param entityId
    //* @return {String}
    //*/
    //getEntityName: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  return nodeEntity.getAttribute("name");
    //},
    ///*
    //* Gets the entity by name
    //* @ param entityId
    //* @return {entity node}
    //*/
    //getEntityByName: function(entityName) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@name='" + entityName + "']");
    //  if(!nodeEntity) return null;

    //  return nodeEntity;
    //},
    ///*
    //* Gets the entity by NID
    //* @ param entityId
    //* @return {entity node}
    //*/
    //getEntityByNID: function(portNID) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[Port/@resourceNID='" + portNID + "']");
    //  if(!nodeEntity) return null;

    //  return nodeEntity;
    //},
    ///*
    //* Gets the entity by type
    //* @ param entityType
    //* @return {entity node}
    //*/
    //getEntityByType: function(entityType) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@type='" + entityType + "'] ");
    //  if(!nodeEntity) return null;

    //  return nodeEntity;
    //},
    ///*
    //* Gets the presence of an entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPresence: function(nodeEntity) {
    //  return nodeEntity.getAttribute("presence");
    //},
    ///*
    //* Gets the type of an entity
    //* @ param entityId
    //* @return {String}
    //*/
    //getEntityType: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  return nodeEntity.getAttribute("type");
    //},
    ///*
    //* Gets the streamType of an entity
    //* @ param entityId
    //* @return {String}
    //*/
    //getEntityStreamType: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  return nodeEntity.getAttribute("streamType");
    //},
    ///*
    //* Gets the pvrEnable of an entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPVREnable: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./PVR/@enable"), null);
    //},

    ///*
    //* Gets the sessionId from the pvr entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPVRSessionId: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./PVR/@sessionId"), null);
    //},

    ///*
    //* Gets the trimlength from the pvr entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPVRTrimLength: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./PVR/@trimLength"), null);
    //},

    ///*
    //* Gets the startTC from the pvr entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPVRStartTC: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./PVR/@startTC"), null);
    //},

    ///*
    //* Gets the connId from the pvr entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPVRConnId: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./PVR/@connId"), null);
    //},

    ///*
    //* Gets the dirId from the pvr entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPVRDirId: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./PVR/@dirId"), null);
    //},

    ///*
    //* Gets the playlength of an entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityPlaylength: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./File/@playLength"), null);
    //},

    ///*
    //* Gets the mediaGroupId of an entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityMediaGroupId: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./File/@mediaGroupId"), null);
    //},

    ///*
    //* Gets the mediaGroupFileNID
    //* @ param mediaGroupId
    //* @return {String}
    //*/
    //getEntityMediaGroupFileNID: function(mediaGroupId) {
    //  var grpFileNode = this.selectSingleNode(".//GrpFile[@mediaGroupId='" + mediaGroupId + "']");
    //  if(jsx3.util.strEmpty(grpFileNode)) return null;
    //  return getNodeValue(grpFileNode.selectSingleNode("//GrpFile/@resourceNID"), null);
    //},

    ///*
    //* Gets the fileNID of an entity
    //* @ param nodeEntity
    //* @return {String}
    //*/
    //getEntityFileNID: function(nodeEntity) {
    //  return getNodeValue(nodeEntity.selectSingleNode("./File/@resourceNID"), null);
    //},
    ///*
    //* Gets an entity object for an entityId
    //* @ param entityId
    //* @return {Object} objEntity
    //*/
    //getEntityObj: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  // get the values from the entity Node
    //  var objEntity = new Object();
    //  objEntity.id = getNodeValue(nodeEntity.selectSingleNode("./@id"), null);
    //  objEntity.name = getNodeValue(nodeEntity.selectSingleNode("./@name"), null);
    //  objEntity.type = getNodeValue(nodeEntity.selectSingleNode("./@type"), null);
    //  objEntity.streamType = getNodeValue(nodeEntity.selectSingleNode("./@streamType"), null);
    //  objEntity.presence = getNodeValue(nodeEntity.selectSingleNode("./@presence"), null);

    //  objEntity.userNID = getNodeValue(nodeEntity.selectSingleNode("./User/@resourceNID"), null);
    //  objEntity.userTitle = getNodeValue(nodeEntity.selectSingleNode("./User/@resourceTitle"), null);

    //  objEntity.portNID = getNodeValue(nodeEntity.selectSingleNode("./Port/@resourceNID"), null);
    //  objEntity.portTitle = getNodeValue(nodeEntity.selectSingleNode("./Port/@resourceTitle"), null);
    //  objEntity.maxInstanceCount = getNodeValue(nodeEntity.selectSingleNode("./Port/InstanceList/@maxInstanceCount"), null);

    //  objEntity.PVREnable = getNodeValue(nodeEntity.selectSingleNode("./PVR/@enable"), null);
    //  objEntity.plyEntityId = getNodeValue(nodeEntity.selectSingleNode("./PVR/@plyEntityId"), null);
    //  objEntity.pvrConnId = getNodeValue(nodeEntity.selectSingleNode("./PVR/@pvrConnId"), null);
    //  objEntity.recEntityId = getNodeValue(nodeEntity.selectSingleNode("./PVR/@recEntityId"), null);
    //  objEntity.trimLength = getNodeValue(nodeEntity.selectSingleNode("./PVR/@trimLength"), null);

    //  objEntity.fileNID = getNodeValue(nodeEntity.selectSingleNode("./File/@resourceNID"), null);
    //  objEntity.fileTitle = getNodeValue(nodeEntity.selectSingleNode("./File/@resourceTitle"), null);
    //  objEntity.playLength = getNodeValue(nodeEntity.selectSingleNode("./File/@playLength"), null);
    //  objEntity.dateCreated = getNodeValue(nodeEntity.selectSingleNode("./File/@dateCreated"), null);
    //  var objDir = this.getDirObj(getNodeValue(nodeEntity.selectSingleNode("./File/@dirId"), ""));
    //  if(objDir) {
    //    objEntity.dirNID = objDir.dirNID;
    //    objEntity.dirTitle = objDir.dirTitle;
    //  }
    //  var objGrpFile = this.getGrpFileObj(getNodeValue(nodeEntity.selectSingleNode("./File/@grpFileId"), ""));
    //  if(objGrpFile) {
    //    objEntity.grpFileNID = objGrpFile.grpFileNID;
    //    objEntity.grpFileTitle = objGrpFile.grpFileTitle;
    //  }
    //  // return the entity object
    //  return objEntity;
    //},
    ///*
    //* Gets the effective NID for an entityId
    //* Typically this should not be used.
    //* @ param entityId
    //* @return {String} NID
    //*/
    //getEntityNID: function(entityId) {
    //  // get the entity Node
    //  var nodeEntity = this.selectSingleNode(".//Entity[@id='" + entityId + "']");
    //  if(!nodeEntity) return null;

    //  // get entity Type
    //  var entityType = nodeEntity.selectSingleNode("./@type").getValue();

    //  // get NID - based on type
    //  var entityNID = null;
    //  if(entityType == SessionInfo.ENTITYTYPE_USER) {
    //    entityNID = nodeEntity.selectSingleNode("./User/@resourceNID").getValue();
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_SRCPORT ||
    //            entityType == SessionInfo.ENTITYTYPE_DSTPORT ||
    //            entityType == SessionInfo.ENTITYTYPE_RELAYPORT ||
    //            entityType == SessionInfo.ENTITYTYPE_STOREPORT) {
    //    entityNID = nodeEntity.selectSingleNode("./Port/@resourceNID").getValue();
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_PLYFILE ||
    //            entityType == SessionInfo.ENTITYTYPE_RECFILE ||
    //            entityType == SessionInfo.ENTITYTYPE_PVRFILE) {
    //    entityNID = nodeEntity.selectSingleNode("./File/@resourceNID").getValue();
    //  }
    //  // return NID
    //  return entityNID;
    //},
    ///*
    //* Internal utility function to get new entity instance nodes
    //* @ param entityType
    //* @return {jsx3.xml.Entity} nodeEntityInstance
    //*/
    //_getNewEntityInstanceNode: function() {
    //  // get a template from the cache OR model/objectXml
    //  var nodeSession = this._getSessionObject();

    //  // get an entity instance node 
    //  var nodeEntityInstance = null;
    //  var nodeInstanceList = nodeSession.selectSingleNode(".//InstanceList");
    //  if(nodeInstanceList) {
    //    nodeEntityInstance = nodeInstanceList.selectSingleNode(".//Instance").cloneNode(true);
    //  }

    //  return nodeEntityInstance;
    //},
    ///*
    //* Internal utility function to get new entity nodes
    //* @ param entityType
    //* @return {jsx3.xml.Entity} nodeEntity
    //*/
    //_getNewEntityNode: function(entityType) {
    //  // get a template from the cache OR model/objectXml
    //  var nodeSession = this._getSessionObject();

    //  // get an entity node
    //  var nodeEntity = nodeSession.selectSingleNode(".//Entity").cloneNode(true);
    //  // if instance support - remove all instances
    //  var nodeInstanceList = nodeEntity.selectSingleNode(".//InstanceList");
    //  if(nodeInstanceList) {
    //    nodeInstanceList.removeChildren();
    //  }
    //  // set the entityType
    //  nodeEntity.setAttribute("type", entityType);
    //  // modify the node contents as needed based on entityType
    //  var nodeEntityUser = nodeEntity.selectSingleNode(".//User");
    //  var nodeEntityPort = nodeEntity.selectSingleNode(".//Port");
    //  var nodeEntityFile = nodeEntity.selectSingleNode(".//File");
    //  var nodeEntityPVR = nodeEntity.selectSingleNode(".//PVR");
    //  var nodeEntityChat = nodeEntity.selectSingleNode(".//Chat");
    //  var nodeEntityAuthCtrl = nodeEntity.selectSingleNode(".//AuthCtrl");
    //  var nodeEntityMediaCtrl = nodeEntity.selectSingleNode(".//MediaCtrl");

    //  if(entityType == SessionInfo.ENTITYTYPE_USER) {
    //    nodeEntity.removeChild(nodeEntityPort);
    //    nodeEntity.removeChild(nodeEntityFile);
    //    nodeEntity.removeChild(nodeEntityPVR);
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_SRCPORT) {
    //    nodeEntity.removeChild(nodeEntityFile);
    //    nodeEntity.removeChild(nodeEntityChat);
    //    nodeEntity.removeChild(nodeEntityAuthCtrl);
    //    nodeEntity.removeChild(nodeEntityMediaCtrl);
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_DSTPORT) {
    //    nodeEntity.removeChild(nodeEntityFile);
    //    nodeEntity.removeChild(nodeEntityPVR);
    //    nodeEntity.removeChild(nodeEntityChat);
    //    nodeEntity.removeChild(nodeEntityAuthCtrl);
    //    nodeEntity.removeChild(nodeEntityMediaCtrl);
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_RELAYPORT ||
    //           entityType == SessionInfo.ENTITYTYPE_STOREPORT) {
    //    nodeEntity.removeChild(nodeEntityUser);
    //    nodeEntity.removeChild(nodeEntityFile);
    //    nodeEntity.removeChild(nodeEntityPVR);
    //    nodeEntity.removeChild(nodeEntityChat);
    //    nodeEntity.removeChild(nodeEntityAuthCtrl);
    //    nodeEntity.removeChild(nodeEntityMediaCtrl);
    //  }
    //  else if(entityType == SessionInfo.ENTITYTYPE_PLYFILE ||
    //            entityType == SessionInfo.ENTITYTYPE_RECFILE ||
    //            entityType == SessionInfo.ENTITYTYPE_PVRFILE) {
    //    nodeEntity.removeChild(nodeEntityUser);
    //    nodeEntity.removeChild(nodeEntityPVR);
    //    nodeEntity.removeChild(nodeEntityChat);
    //    nodeEntity.removeChild(nodeEntityAuthCtrl);
    //    nodeEntity.removeChild(nodeEntityMediaCtrl);
    //  }

    //  return nodeEntity;
    //},
    ///*
    //* Adds an entity to the SessionInfo
    //* @param {Object} objEntity
    //* @return {String} entityId
    //*/
    //addEntity: function(objEntity) {
    //  // create a new entity node based on type
    //  var nodeEntity = this._getNewEntityNode(objEntity.type);
    //  if(!nodeEntity) return null;

    //  // generate a new entityId - if needed
    //  var entityId = objEntity.id;
    //  if(!entityId) {
    //    entityId = objEntity.type + '_' + getUUID();
    //    objEntity.id = entityId;
    //  }

    //  // set the values in the new node 
    //  this._updateEntityNode(nodeEntity, objEntity);

    //  // append it to the list in the current Session xml in the topic
    //  var nodeEntityList = this.selectSingleNode(".//EntityList");
    //  nodeEntityList.appendChild(nodeEntity);

    //  // return the id of the new entity
    //  return entityId;
    //},
    ///*
    //* function to get GrpFileList
    //* @return  nodeGrpFileList
    //*/
    //getGrpFileIter: function() {
    //  return this.selectNodeIterator(".//GrpFile");;
    //},
    ///*
    //* function to set GrpFileList
    //* @param {nodeListIter} GrpFileListIter
    //*/
    //setGrpFileList: function(GrpFileListIter) {
    //  var nodeGrpFileList = this.selectSingleNode(".//GrpFileList");
    //  while(GrpFileListIter.hasNext()) {
    //    var grpFileNode = GrpFileListIter.next();
    //    nodeGrpFileList.appendChild(grpFileNode);
    //  }
    //},

    ///*
    //* function to get getGrpFileNumber
    //* @return  grpFileNumber
    //*/
    //getGrpFiles: function() {
    //  var nodeGrpFileList = this.selectNodes(".//GrpFileList/GrpFile");
    //  var grpFileNumber = nodeGrpFileList == null ? 0 : nodeGrpFileList.getLength();
    //  return grpFileNumber;
    //},

    ///*
    //* function to get DirList
    //* @return  nodeDirList
    //*/
    //getDirIter: function() {
    //  return this.selectNodeIterator(".//Dir");
    //},
    ///*
    //* function to set DirList
    //* @param {nodeListIter} DirListIter
    //*/
    //setDirList: function(DirListIter) {
    //  var nodeDirList = this.selectSingleNode(".//DirList");
    //  while(DirListIter.hasNext()) {
    //    var dirNode = DirListIter.next();
    //    nodeDirList.appendChild(dirNode);
    //  }
    //},

    ///*
    //* function to set EntityList
    //* @param {nodeList} EntityListIter
    //*/
    //setEntityList: function(EntityListIter) {
    //  var nodeEntityList = this.selectSingleNode(".//EntityList");
    //  while(EntityListIter.hasNext()) {
    //    var entityNode = EntityListIter.next();
    //    nodeEntityList.appendChild(entityNode);
    //  }
    //},

    ///******************************** CONNECTIONS **************************/

    ///*
    //* function to set ConnList
    //* @param {nodeList} ConnListIter
    //*/
    //setConnList: function(ConnListIter) {
    //  var nodeConnList = this.selectSingleNode(".//ConnectionList");
    //  while(ConnListIter.hasNext()) {
    //    var connNode = ConnListIter.next();
    //    nodeConnList.appendChild(connNode);
    //  }
    //},

    ///*
    //* Adds a connection grp to the SessionInfo.
    //* Typically only one of src or dst Entity Id is provided. (the other is added via updateConnGroup).
    //* @param {String} srcEntityId
    //* @param {String} dstEntityId
    //* @return {String} connGrpId
    //*/
    //addConnGroup: function(srcEntityId, dstEntityId) {
    //  // setup a new conn grp id
    //  var connGrpId = SessionInfo.CONNGRP_IDPREFIX + getUUID();

    //  // add a new conn node - with this grp id
    //  if(!this._addConn(connGrpId, srcEntityId, dstEntityId)) {
    //    return null;
    //  }
    //  // check/update the SessionInfo type
    //  this._updateSessionType();

    //  // return the new conngrpid
    //  return connGrpId;
    //},
    ///*
    //* Deletes a connection grp from the SessionInfo.
    //* Deletes all connections that are part of this connGroup. Also cleans up assoc Entities
    //* @param {String} connGrpId
    //* @return {Boolean}
    //*/
    //delConnGroup: function(connGrpId) {
    //  // delete the RECORD connection
    //  this.updateConnGroupDelRec(connGrpId);
    //  // go thru all the conns in the same grp
    //  var iterConn = this.getConnIterByGrpId(connGrpId);
    //  while(iterConn.hasNext()) {
    //    var nodeConn = iterConn.next();
    //    var connId = nodeConn.getAttribute("id");
    //    // delete each conn
    //    if(!this._delConn(connId)) {
    //      return false;
    //    }
    //  }
    //  // check/update the SessionInfo type
    //  this._updateSessionType();

    //  return true;
    //},
    ///*
    //* Updates the name of a connection grp in the SessionInfo.
    //* Renames all connections that are part of this connGroup. 
    //* @param {String} connGrpId
    //* @param {String} name
    //* @return {Boolean}
    //*/
    //updateConnGroupName: function(connGrpId, name) {
    //  // go thru all the conns in the same grp
    //  var iterConn = this.getConnIterByGrpId(connGrpId);
    //  while(iterConn.hasNext()) {
    //    var nodeConn = iterConn.next();
    //    nodeConn.setAttribute("name", name);
    //  }
    //  return true;
    //},
    ///*
    //* Updates a the src of a connection grp in the SessionInfo.
    //* Used to switch src for all connections of an existing connGroup.
    //* @param {String} connGrpId
    //* @param {String} srcEntityId
    //* @return {Boolean}
    //*/
    //updateConnGroupSrc: function(connGrpId, srcEntityId) {
    //  // go thru all the conns in the same grp
    //  var iterConn = this.getConnIterByGrpId(connGrpId);
    //  while(iterConn.hasNext()) {
    //    var nodeConn = iterConn.next();
    //    // replace src in this conn - if err return
    //    if(!this._updateConnNode(nodeConn, srcEntityId, null)) {
    //      return false;
    //    }
    //  }
    //  // check/update the SessionInfo type
    //  this._updateSessionType();

    //  return true;
    //},
    ///*
    //* Adds a conn dest to a connection grp in the SessionInfo.
    //* Used to add dests (one connection for each dest) to an existing connGroup.
    //* @param {String} connGrpId
    //* @param {String} dstEntityId
    //* @return {Boolean}
    //*/
    //updateConnGroupAddDst: function(connGrpId, dstEntityId) {
    //  var nodeConn;
    //  // check if duplicate - if so return
    //  if(this.selectSingleNode(".//Connection[@gid='" + connGrpId + "'" +
    //                                           " and @dstEntityId='" + dstEntityId + "']")) {
    //    return false;
    //  }
    //  // next look for a conn in the grp with a blank dest id
    //  nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "'" +
    //                                                      " and @dstEntityId='']");
    //  // if found - 
    //  if(nodeConn != null) {
    //    // put this dest in it
    //    if(!this._updateConnNode(nodeConn, null, dstEntityId)) {
    //      return false;
    //    }
    //  }
    //    // if not found - 
    //  else {
    //    // get the source entity for this conn grp from an existing conn
    //    var srcEntityId = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "']").getAttribute("srcEntityId");
    //    // add a new conn node - with this src id and dst id
    //    if(!this._addConn(connGrpId, srcEntityId, dstEntityId)) {
    //      return false;
    //    }
    //  }
    //  return true;
    //},
    ///*
    //* Deletes a conn dest from a connection grp in the SessionInfo.
    //* Used to del dests (one connection for each dest) from an existing connGroup.
    //* @param {String} connGrpId
    //* @param {String} dstEntityId
    //* @return {Boolean}
    //*/
    //updateConnGroupDelDst: function(connGrpId, dstEntityId) {
    //  // get the conn for this dst entity in this conn grp
    //  var nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "'" +
    //                                           " and @dstEntityId='" + dstEntityId + "']");
    //  if(!nodeConn) return false;
    //  var connId = nodeConn.getAttribute("id");
    //  // check the number of conn in the conn grp
    //  var connCount = (this.selectNodes(".//Connection[@gid='" + connGrpId + "']")).size();
    //  // if more than one - delte the whole connection
    //  // otherwise - just remove the dst - to preserve src info for this conngrp
    //  if(connCount > 1) {
    //    this._delConn(connId);
    //  }
    //  else {
    //    this._delConnDst(connId);
    //    nodeConn.setAttribute("type", SessionInfo.CONNTYPE_PARTIAL);
    //  }
    //  // return success
    //  return true;
    //},
    ///*
    //* Adds a Recording connection (and the required dstEntity) to the SessionInfo.
    //* Uses the srcEntity name to derive filename if recName is not gvien
    //* @param {String} connGrpId
    //* @param {Object} objFile (optional)
    //* @return {String} connId
    //*/
    //updateConnGroupAddRec: function(connGrpId, objFile) {
    //  // get the source entity for this conn grp from an existing conn
    //  var srcEntityId = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "']").getAttribute("srcEntityId");
    //  if(jsx3.util.strEmpty(srcEntityId)) {
    //    return null;
    //  }
    //  // setup to add a recfile entity
    //  var objEntity = new Object();
    //  objEntity.type = SessionInfo.ENTITYTYPE_RECFILE;

    //  if(objFile && !jsx3.util.strEmpty(objFile.dirNID)) {
    //    objEntity.dirNID = objFile.dirNID;
    //  }
    //  // use the DefaultRec dir if none specified
    //  if(objFile && (!(objEntity.dirId || objFile.dirName || objFile.dirNID))) {
    //    var objDir = this.getDefaultRecDir();
    //    if(objDir) {
    //      objEntity.dirId = objDir.id;
    //    }
    //  }
    //  // use the DefaultRec group file if none specified
    //  if(objFile && (!(objEntity.grpFileId || objFile.grpFileName || objFile.grpFileNID))) {
    //    var objGrpFile = this.getDefaultRecGrpFile();
    //    if(!objGrpFile) {
    //      var newObjGrpFile = new Object();
    //      newObjGrpFile.grpFileTitle = this.getName();;
    //      this.setDefaultRecGrpFile(newObjGrpFile);
    //      objGrpFile = this.getDefaultRecGrpFile();
    //    }
    //    objEntity.grpFileId = objGrpFile.id;
    //  }

    //  // set the fileName and entityName - use srcname as default
    //  var srcEntityName = this.getEntityName(srcEntityId);
    //  objEntity.fileTitle = (objFile && objFile.fileTitle) ? objFile.fileTitle : (objEntity.type + '_' + srcEntityName);
    //  objEntity.name = objEntity.fileTitle;
    //  // set the clipId
    //  objEntity.clipId = SessionInfo.ENTITYTYPE_RECFILE + "_" + getUUID();
    //  // set the streamtype - based on the src
    //  objEntity.streamType = this.getEntityStreamType(srcEntityId);

    //  // add to entity list
    //  var dstEntityId = this.addEntity(objEntity);
    //  if(!dstEntityId) return null;

    //  //    // add a new conn node - with this src id and dst id
    //  //    var connId = this._addConn(connGrpId, srcEntityId, dstEntityId);
    //  //    if( ! connId ) return null;
    //  //    
    //  //    // return the new conn id 
    //  //    return connId;

    //  // update the connection
    //  if(this.updateConnGroupAddDst(connGrpId, dstEntityId)) {
    //    return true;
    //  }
    //  return false;
    //},
    ///*
    //* Deletes a Recording connection (and the required dstEntity) from the SessionInfo.
    //* Deletes all the Record Connections and there respective Dest entity 
    //* @param {String} connGrpId
    //* @param {String} connId (optional)
    //* @return {Boolean}
    //*/
    //updateConnGroupDelRec: function(connGrpId, connId) {
    //  // get a recording conn for this conn grp from an existing conn
    //  var iterConn;
    //  if(connId) {
    //    iterConn = this.selectNodeIterator(".//Connection[@id='" + connId + "']");
    //  }
    //  else {
    //    iterConn = this.selectNodeIterator(".//Connection[@gid='" + connGrpId + "' and @type='" + SessionInfo.CONNTYPE_RECORD + "']");
    //  }
    //  if(!iterConn.hasNext()) return false;

    //  // get the number of connections for this connGrpId
    //  var connCount = (this.selectNodes(".//Connection[@gid='" + connGrpId + "']")).size();

    //  // if more than 1 means there are other destinations also
    //  if(connCount > 1) {
    //    // delete the entity and the connection
    //    while(iterConn.hasNext()) {
    //      var nodeConn = iterConn.next();
    //      // delete the connection
    //      connId = nodeConn.getAttribute("id");
    //      var dstEntityId = nodeConn.getAttribute("dstEntityId");
    //      this._delConn(connId);
    //      // delete RecFile entity from the entitylist
    //      this.delEntity(dstEntityId);
    //    }
    //  }
    //    // if only 1 connection with type=RECORD then delete the entity and clear the dstentityid and set type=PARTIAL
    //  else {
    //    var nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "' and @type='" + SessionInfo.CONNTYPE_RECORD + "']");
    //    var connId = nodeConn.getAttribute("id");
    //    var dstEntityId = nodeConn.getAttribute("dstEntityId");
    //    this._delConnDst(connId);
    //    // delete RecFile entity from the entitylist
    //    this.delEntity(dstEntityId);
    //    nodeConn.setAttribute("type", SessionInfo.CONNTYPE_PARTIAL);
    //  }


    //  // return success
    //  return true;
    //},
    ///*
    //* Updates the title/NID info of the file/dir entity of a Recording connection from the SessionInfo.
    //* @param {String} connGrpId
    //* @param {String} connId (optional)
    //* @param {String} fileTitle (optional)
    //* @param {String} dirTitle (optional)
    //* @param {String} dirNID (optional)
    //* @return {Boolean}
    //*/
    //updateConnGroupRecInfo: function(connGrpId, connId, objFile) {
    //  // get a recording conn for this conn grp from an existing conn
    //  var nodeConn;
    //  if(connId) {
    //    nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    //  }
    //  else {
    //    nodeConn = this.selectSingleNode(".//Connection[@gid='" + connGrpId + "' and @type='" + SessionInfo.CONNTYPE_RECORD + "']");
    //  }
    //  if(!nodeConn) return false;

    //  // get the dest entity ID and update it
    //  var dstEntityId = nodeConn.getAttribute("dstEntityId");
    //  if(jsx3.util.strEmpty(dstEntityId)) return false;

    //  // use the file/dir info to update the dest entity    
    //  return this.updateEntity(dstEntityId, objFile);
    //},

    ///*
    //* Deletes a connection from the SessionInfo.
    //* Deletes Src and Dst Entities also - if not in use anymore
    //* Typically not called directly. (called from add/updateConnGroup).
    //* @param {String} connId
    //* @return {Boolean}
    //*/
    //_delConn: function(connId) {
    //  // get the connection node
    //  var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    //  if(!nodeConn) return false;

    //  // del the src / dst entity for this conn
    //  this._delConnSrc(connId);
    //  this._delConnDst(connId);

    //  // delete it from its parent node
    //  nodeConn.getParent().removeChild(nodeConn);

    //  // return success
    //  return true;
    //},
    ///*
    //* Deletes the src entity from a connection from the SessionInfo.
    //* Deletes Src Entities also - if not in use anymore
    //* Typically not called directly. (called from add/updateConnGroup).
    //* @param {String} connId
    //* @return {Boolean}
    //*/
    //_delConnSrc: function(connId) {
    //  // get the connection node
    //  var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    //  if(!nodeConn) return false;

    //  // get the src entity for this conn
    //  var srcEntityId = nodeConn.getAttribute("srcEntityId");

    //  // clear it 
    //  nodeConn.setAttribute("srcEntityId", "");

    //  // TBD: gc the entity

    //  // return success
    //  return true;
    //},
    ///*
    //* Deletes the dst entity from a connection from the SessionInfo.
    //* Deletes Dst Entities also - if not in use anymore
    //* Typically not called directly. (called from add/updateConnGroup).
    //* @param {String} connId
    //* @return {Boolean}
    //*/
    //_delConnDst: function(connId) {
    //  // get the connection node
    //  var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    //  if(!nodeConn) return false;

    //  // get the dst entity for this conn
    //  var dstEntityId = nodeConn.getAttribute("dstEntityId");

    //  // clear it 
    //  nodeConn.setAttribute("dstEntityId", "");

    //  // TBD: gc the entity

    //  // return success
    //  return true;
    //},
    ///*
    //* Updates a connection in the SessionInfo.
    //* Typically not called directly.
    //* @param {String} connId
    //* @param {String} srcEntityId
    //* @param {String} dstEntityId
    //* @return {Boolean}
    //*/
    //_updateConn: function(connId, srcEntityId, dstEntityId) {
    //  // get the connection node
    //  var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    //  if(!nodeConn) return false;
    //  // setup values in this conn - if err return
    //  if(!this._updateConnNode(nodeConn, srcEntityId, dstEntityId)) {
    //    return false;
    //  }
    //  // return success
    //  return true;
    //},
    ///** @private @jsxobf-clobber
    //* Sets values in a connection node
    //* Not called directly. (called from add/updateConnection).
    //* @param {Node} nodeConn
    //* @param {String} srcEntityId
    //* @param {String} dstEntityId
    //* @return {Boolean}
    //*/
    //_updateConnNode: function(nodeConn, srcEntityId, dstEntityId) {
    //  var srcEntityIdFlag = false;
    //  if(!jsx3.util.strEmpty(srcEntityId)) {
    //    srcEntityIdFlag = true;
    //  }
    //  else {
    //    srcEntityId = nodeConn.getAttribute("srcEntityId");
    //    if(!jsx3.util.strEmpty(srcEntityId)) {
    //      srcEntityIdFlag = true;
    //    }
    //  }

    //  var dstEntityIdFlag = false;
    //  if(!jsx3.util.strEmpty(dstEntityId)) {
    //    dstEntityIdFlag = true;
    //  }
    //  else {
    //    dstEntityId = nodeConn.getAttribute("dstEntityId");
    //    if(!jsx3.util.strEmpty(dstEntityId)) {
    //      dstEntityIdFlag = true;
    //    }
    //  }

    //  // if both src and dst available - check for stream type compatibility
    //  if((!jsx3.util.strEmpty(srcEntityId)) &&
    //      (!jsx3.util.strEmpty(dstEntityId))) {
    //    if(!this._isCompatibleStreamType(srcEntityId, dstEntityId)) {
    //      return false;
    //    }
    //  }

    //  // if src specified - 
    //  if(srcEntityIdFlag) {
    //    // set srcEntityId
    //    nodeConn.setAttribute("srcEntityId", srcEntityId);
    //    // set connName if needed - based on srcEntityName
    //    var connName = nodeConn.getAttribute("name");
    //    if(jsx3.util.strEmpty(connName)) {
    //      var srcEntityName = this.getEntityName(srcEntityId);
    //      if(!jsx3.util.strEmpty(srcEntityName)) {
    //        //connName = SessionInfo.CONN_IDPREFIX + srcEntityName;
    //        connName = srcEntityName;
    //      }
    //      else {
    //        connName = nodeConn.getAttribute("id");
    //      }
    //      nodeConn.setAttribute("name", connName);
    //    }
    //    // set the conn streamtype
    //    var srcEntityStreamType = this.getEntityStreamType(srcEntityId);
    //    nodeConn.setAttribute("streamType", srcEntityStreamType);
    //  }

    //  // if dst specified - 
    //  if(dstEntityIdFlag) {
    //    // set dstEntityId
    //    nodeConn.setAttribute("dstEntityId", dstEntityId);
    //  }

    //  // set the connection type when both src and dest entity are present
    //  if(srcEntityIdFlag && dstEntityIdFlag) {
    //    var connType = "";
    //    // set the connection type : if the src entity type of plyfile
    //    var srcEntityType = this.getEntityType(srcEntityId);
    //    var dstEntityType = this.getEntityType(dstEntityId);

    //    if(srcEntityType == SessionInfo.ENTITYTYPE_PLYFILE) {
    //      connType = SessionInfo.CONNTYPE_PLAYBACK;
    //    }
    //    else if(dstEntityType == SessionInfo.ENTITYTYPE_DSTPORT) {
    //      connType = SessionInfo.CONNTYPE_VIEW;
    //    }
    //    else if(dstEntityType == SessionInfo.ENTITYTYPE_RECFILE) {
    //      connType = SessionInfo.CONNTYPE_RECORD;
    //    }
    //    else if(dstEntityType == SessionInfo.ENTITYTYPE_PVRFILE) {
    //      connType = SessionInfo.CONNTYPE_PVR;
    //    }

    //    nodeConn.setAttribute("type", connType);
    //  }

    //  // return success
    //  return true;
    //},
    ///*
    //* Validates streamType compatability between src and dst entityID
    //* Rules for passing:
    //* 1)dst is of type file OR
    //* 2)destination port stream type is same as source OR 
    //* 2)destination port stream type is "MULTI" but source is not "UDP"
    //* @param {String} srcEntityId
    //* @param {String} dstEntityId
    //* @return {String} connGrpId
    //*/
    //_isCompatibleStreamType: function(srcEntityId, dstEntityId) {
    //  var srcEntityStreamType = this.getEntityStreamType(srcEntityId);
    //  var dstEntityStreamType = this.getEntityStreamType(dstEntityId);
    //  var dstEntityType = this.getEntityType(dstEntityId);
    //  var dstEntityStreamTypeSplit = dstEntityStreamType.split(" ");
    //  if((dstEntityType == SessionInfo.ENTITYTYPE_RECFILE) ||
    //      (dstEntityStreamTypeSplit[0] == srcEntityStreamType) ||
    //      (dstEntityStreamTypeSplit[1] == srcEntityStreamType) ||
    //      (dstEntityStreamTypeSplit[2] == srcEntityStreamType) ||
    //      ((dstEntityStreamType == SessionInfo.ENTITYSTREAMTYPE_MULTI) &&
    //       (srcEntityStreamType != SessionInfo.ENTITYSTREAMTYPE_UDP))) {
    //    return true;
    //  }
    //  else {
    //    return false;
    //  }
    //},
    ///*
    //* Validates destEntityID is being used in any connection
    //* Rules for passing:
    //* 1)destination type is "MULTI" 
    //* 2) destination not in use in any connection
    //* @param {String} dstEntityId
    //*/
    //isDestEntityAvailable: function(dstEntityId) {
    //  var dstEntityStreamType = this.getEntityStreamType(dstEntityId);
    //  if((dstEntityStreamType == SessionInfo.ENTITYSTREAMTYPE_MULTI)) {
    //    return true;
    //  }

    //  var nodeConn = this.selectSingleNode(".//Connection[@dstEntityId='" + dstEntityId + "']");
    //  if(!jsx3.util.strEmpty(nodeConn)) return false;

    //  return true;

    //},
    ///*
    //* Validates srcEntityID is being used in any connection
    //* Rules for passing:
    //* 1) source not in use in any connection
    //* @param {String} srcEntityId
    //*/
    //isSrcEntityAvailable: function(srcEntityId) {
    //  var srcEntityStreamType = this.getEntityStreamType(srcEntityId);

    //  var nodeConn = this.selectSingleNode(".//Connection[@srcEntityId='" + srcEntityId + "']");
    //  if(!jsx3.util.strEmpty(nodeConn)) return false;

    //  return true;

    //},
    ///*
    //* Gets an Iterator over the Connections
    //* 
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnIter: function() {
    //  return this.selectNodeIterator(".//Connection");
    //},
    ///****** FUNCTION DEPRECATED *******/
    ///*
    //* Gets an Iterator over the Connections of type=STREAM
    //* 
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnStreamIter: function() {
    //  return this.selectNodeIterator(".//Connection[@type='STREAM']");
    //},

    ///*
    //* Gets an Iterator over the Connections of type=VIEW
    //* 
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnViewIter: function() {
    //  return this.selectNodeIterator(".//Connection[@type='VIEW' or @type='STREAM' or @type='PLAYBACK' or @type='PARTIAL' or @type='RECORD']");
    //},
    ///*
    //* Gets an Iterator over the Connections of the specified connGrpId
    //* @param connGrpId {String}
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnIterByGrpId: function(connGrpId) {
    //  return this.selectNodeIterator(".//Connection[@gid='" + connGrpId + "']");
    //},
    ///*
    //* Gets an Iterator over the Connections of the specified dstEntityId
    //* @param dstEntityId {String}
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnIterByDstEntityId: function(dstEntityId) {
    //  return this.selectNodeIterator(".//Connection[@dstEntityId='" + dstEntityId + "']");
    //},
    ///*
    //* Gets the Connection of the specified connGrpId
    //* @param connGrpId {String}
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnByGrpId: function(connGrpId) {
    //  return this.selectSingleNode(".//Connection[@gid='" + connGrpId + "']");
    //},
    ///*
    //* Gets the Record Connection of the specified connGrpId
    //* @param connGrpId {String}
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getRecordConnByGrpId: function(connGrpId) {
    //  return this.selectSingleNode(".//Connection[@gid='" + connGrpId + "' and @type='RECORD']");
    //},
    ///*
    //* Gets an Iterator over the Connections of the specified type
    //* @param connType {String|List} of CONNTYPEs
    //* @return {jsx3.util.Iterator} Connection Nodes
    //*/
    //getConnIterByType: function(connType) {
    //  var whereXPath = "";
    //  if(connType instanceof jsx3.util.List) {
    //    var iterConnType = connType.iterator();
    //    while(iterConnType.hasNext()) {
    //      if(!jsx3.util.strEmpty(whereXPath)) {
    //        whereXPath += " or ";
    //      }
    //      whereXPath += "@type='" + iterConnType.next() + "'";
    //    }
    //  }
    //  else {
    //    whereXPath = "@type='" + connType + "'";
    //  }
    //  return this.selectNodeIterator(".//Connection[" + whereXPath + "]");
    //},
    ///*
    //* Gets the id of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnId: function(nodeConn) {
    //  return nodeConn.getAttribute("id");
    //},

    ///*
    //* Gets the sessionid of an pvr connection
    //* @ param pvrnodeConn
    //* @return {String}
    //*/
    //getShareId: function(nodeConn) {
    //  return nodeConn.getAttribute("sessionId");
    //},

    ///*
    //* Gets the sharedid of an pvr connection
    //* @ param pvrnodeConn
    //* @return {String}
    //*/
    //getTrimLength: function(nodeConn) {
    //  return nodeConn.getAttribute("trimLength");
    //},

    ///*
    //* Gets the sharedid of an pvr connection
    //* @ param pvrnodeConn
    //* @return {String}
    //*/
    //getStartTC: function(nodeConn) {
    //  return nodeConn.getAttribute("startTC");
    //},

    ///*
    //* Gets the gid of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnGrpId: function(nodeConn) {
    //  return nodeConn.getAttribute("gid");
    //},
    ///*
    //* Gets the name of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnName: function(nodeConn) {
    //  return nodeConn.getAttribute("name");
    //},
    ///*
    //* Gets the type of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnType: function(nodeConn) {
    //  return nodeConn.getAttribute("type");
    //},
    ///*
    //* Gets the srcEntityId of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnSrcEntityId: function(nodeConn) {
    //  return nodeConn.getAttribute("srcEntityId");
    //},
    ///*
    //* Gets the dstEntityId of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnDstEntityId: function(nodeConn) {
    //  return nodeConn.getAttribute("dstEntityId");
    //},
    ///*
    //* Gets the state of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnState: function(nodeConn) {
    //  return nodeConn.getAttribute("state");
    //},

    ///*
    //* Gets connection by connId
    //* @ param connId
    //* @return {connection node}
    //*/
    //getConnById: function(connId) {
    //  // get the connection Node
    //  var nodeConn = this.selectSingleNode(".//Connection[@id='" + connId + "']");
    //  if(!nodeConn) return null;

    //  return nodeConn;
    //},
    ///*
    //* Gets connection by srcEntityId, dstEntityId, windowId
    //* @ param srcEntityId, dstEntityId, windowId
    //* @return {connection node}
    //*/
    //getConn: function(srcEntityId, dstEntityId, windowId) {
    //  var whereXPath = "";
    //  if(srcEntityId) {
    //    whereXPath += "./@srcEntityId='" + srcEntityId + "'";
    //  }

    //  if(dstEntityId) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@dstEntityId='" + dstEntityId + "'";
    //  }

    //  if(windowId) {
    //    if(!jsx3.util.strEmpty(whereXPath)) whereXPath += " and ";
    //    whereXPath += "./@windowId='" + windowId + "'";
    //  }
    //  // get the connection node
    //  var nodeConn = this.selectSingleNode(".//Connection[" + whereXPath + "]");
    //  // if not found return null
    //  if(!nodeConn) {
    //    return null;
    //  }
    //  return nodeConn;
    //},
    ///*
    //* Gets the canvasId of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnCanvasId: function(nodeConn) {
    //  return nodeConn.getAttribute("canvasId");
    //},
    ///*
    //* Gets the windowId of an connection
    //* @ param nodeConn
    //* @return {String}
    //*/
    //getConnWindowId: function(nodeConn) {
    //  return nodeConn.getAttribute("windowId");
    //},
    ///*
    //* Internal utility function to get new entity connection nodes
    //* @ param connType (optional)
    //* @return {jsx3.xml.Entity} nodeConn
    //*/
    //_getNewConnectionNode: function(connType) {
    //  // get a template from the cache OR model/objectXml
    //  var nodeSession = this._getSessionObject();

    //  // set PARTIAL by default
    //  if(!connType) {
    //    connType = SessionInfo.CONNTYPE_PARTIAL;
    //  }
    //  // get a connection node
    //  var nodeConn = nodeSession.selectSingleNode(".//Connection").cloneNode(true);
    //  // clean out any connsegs
    //  nodeConn.removeChildren();
    //  // set the connType
    //  nodeConn.setAttribute("type", connType);

    //  return nodeConn;
    //},
    ///*
    //* Adds a connection to the SessionInfo.
    //* Typically not called directly. (called from add/updateConnGroup).
    //* @param {String} connGrpId
    //* @param {String} srcEntityId
    //* @param {String} dstEntityId
    //* @return {String} connId
    //*/
    //_addConn: function(connGrpId, srcEntityId, dstEntityId) {
    //  // get a new connection node
    //  var nodeConn = this._getNewConnectionNode();
    //  // setup a new conn id
    //  var connId = SessionInfo.CONN_IDPREFIX + getUUID();
    //  nodeConn.setAttribute("id", connId);
    //  // set the conn grp id
    //  nodeConn.setAttribute("gid", connGrpId);
    //  // setup values in this conn - if err return
    //  if(!this._updateConnNode(nodeConn, srcEntityId, dstEntityId)) {
    //    return null;
    //  }

    //  // if success - append the new node to the list  
    //  var nodeConnList = this.selectSingleNode(".//ConnectionList");
    //  nodeConnList.appendChild(nodeConn);

    //  // return the new conn id 
    //  return connId;
    //},
    /*
    * Internal utility function to get get a template SessionConfig
    * Looks in the cache first - if not found inits it from model/objectXml
    * @return {jsx3.xml.Entity} nodeSessionConfig
    */
    _getSessionObject: function () {
        BarcoUtil.logger(bDEBUG+1, "", this.cls, "_getSessionObject", "call", "");
        // get template from cache
        var nodeSession = { "Session": {
            "Groups": { "Tag": { "@Name": "", "@Value": ""} },
            "TimelineList": { "Timeline": {
                "Last": { "@requestedAction": "", "@TimeScale": "", "@WC": "", "@Offset": "", "@TC": "" },
                "Start": { "@parentTimelineId": "", "@WC": "", "@Offset": "", "@TC": "" },
                "@id": ""
            }
            },
            "@name": "",
            "@recordTimelineId": "",
            "MetaData": { "Tag": { "@Name": ""} },
            "@type": "",
            "@timelineId": "",
            "GrpFileList": { "GrpFile": [{
                "@syncEntityId": "",
                "MetaData": { "Tag": { "@Name": ""} },
                "@type": "SyncPly",
                "@dateCreated": "",
                "@recSessionConfigId": "",
                "@resourceTitle": "",
                "@id": "",
                "@recSessionId": "",
                "@resourceNID": ""
            },
                                  {
                                      "MetaData": { "Tag": { "@Name": ""} },
                                      "@type": "SyncRec",
                                      "@resourceTitle": "",
                                      "@id": "",
                                      "@resourceNID": ""
                                  }]
            },
            "@state": "",
            "@id": "",
            "ConnectionList": { "Connection": {
                "@streamURL": "",
                "@errorCode": "",
                "@dstEntityId": "",
                "@srcEntityId": "",
                "@errorData": "",
                "@timelineId": "",
                "@state": "",
                "@id": "",
                "ConnSeg": {
                    "@profileTitle": "",
                    "@errorCode": "",
                    "@errorData": "",
                    "@profileNID": "",
                    "@dstResourceNID": "",
                    "@state": "",
                    "@srcResourceNID": "",
                    "@srcResourceTitle": "",
                    "@dstResourceTitle": ""
                }
            }
            },
            "EntityList": { "Entity": [{
                "PVR": { "@trimLength": "", "@ffTracks": "", "@connId": "", "@enable": "" },
                "@errorCode": "",
                "@streamType": "",
                "@name": "",
                "Port": { "@trickPlay": "", "@type": "", "@resourceTitle": "", "@resourceNID": "" },
                "@errorData": "",
                "@type": "SrcPort",
                "@gc": "",
                "@presence": "",
                "@state": "",
                "@id": ""
            },
                               {
                                   "@errorCode": "",
                                   "@streamType": "",
                                   "@name": "",
                                   "Port": { "@trickPlay": "", "@type": "", "@resourceTitle": "", "@resourceNID": "" },
                                   "@errorData": "",
                                   "@type": "DstPort",
                                   "@gc": "",
                                   "@presence": "",
                                   "@state": "",
                                   "@id": ""
                               },
                               {
                                   "@errorCode": "",
                                   "@streamType": "",
                                   "@name": "",
                                   "@errorData": "",
                                   "@type": "PlyFile",
                                   "File": { "@playLength": "", "@type": "", "@startOffset": "", "@grpFileId": "", "@dirId": "", "@resourceTitle": "", "@resourceNID": "" },
                                   "@gc": "",
                                   "@presence": "",
                                   "@state": "",
                                   "@id": ""
                               },
                               {
                                   "@errorCode": "",
                                   "@streamType": "",
                                   "@name": "",
                                   "@errorData": "",
                                   "@type": "RecFile",
                                   "File": { "@action": "", "@key": "",
                                       "MetaData": { "Tag": { "@Name": ""} },
                                       "@type": "",
                                       "@grpFileId": "",
                                       "@dirId": "",
                                       "@resourceTitle": "",
                                       "@keyMetaDataKeyName": "",
                                       "@resourceNID": ""
                                   },
                                   "@gc": "",
                                   "@presence": "",
                                   "@state": "",
                                   "@id": ""
                               }
                               ]
            },
            "Properties": { "@dateCreated": "", "@ownerUserJID": "", "@sessionConfigNID": "" }
        }
        }
        return nodeSession;
    }


});