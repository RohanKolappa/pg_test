/**
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license. 
 *
 * This  acts as the streaming server agent
 * UI blocks that will instantiate and manage the decoder port instances (ie Video Player Objects)
 * all dest agent events for that mediaroom will be parsed by this agent and sent
 * as simplified messages (ideally, similar to SS API) on the registered topic
 */

jsx3.Class.defineClass('com.ipvs.agent.SSAgent', null, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(SSAgent, SSAgent_prototype) {

  // create Logger for this class
  SSAgent_prototype._LOG = jsx3.util.Logger.getLogger(SSAgent.jsxclass.getName());

  /**
  * Instance varibales
  */
  SSAgent_prototype.arrDstObj = null;
  SSAgent_prototype.arrResourceInstanceList = null;
  SSAgent_prototype.SS = null;

  /**
  * The instance initializer.
  * @param eventApp {Object}
  */
  SSAgent_prototype.init = function() {
    this._LOG.debug("SSAgent.init");
    this.arrDstObj = new Object;
    this.arrResourceInstanceList = new Object;
    this.SS = { "input": {},
                "output": {}
              };
    // get a handle to App - and wait for app init          
    this.clsApp = com.ipvs.app.App.clsApp;
    if(! this.clsApp) return;
    if (this.clsApp.INITED) {
      this._onAppInit();
    }
    else {
      this.clsApp.subscribe(com.ipvs.app.App.APP_INIT, this, this._onAppInit);
    } 
  }
  SSAgent_prototype._onAppInit = function() {
    this.clsApp.clsNotification.subscribe(com.ipvs.api.Notification.MSG_SSCMD, this, this._handleSSEvent);
  };

  /**
  * The destroy method.
  */
  SSAgent_prototype.destroy = function () {
    this.arrDstObj = null;
    this.arrResourceInstanceList = null;
    this.SS = null;
  }


  /**
  * SS Event Handler
  * simulate what the Streaming server does and translate to Player commands
  * @param objSSCmd {Object}
  */
  SSAgent_prototype._handleSSEvent = function (objSSCmd) {
    // process cmd based on 'action'
    switch (objSSCmd.action) {
      case "add":
        // just stash the cmd by 'operation.id' in the SS object - will be used later when start comes
        this.SS[objSSCmd.operation][objSSCmd.id] = objSSCmd;
        break;
      case "delete":
        // find matching object from 'add+start' - may not exist - extra/precaution deletes are normal
        var objSS = this.SS[objSSCmd.operation][objSSCmd.id];
        if (objSS) {
          // check if this has already been 'start'ed - ie: has a 'queueid'
          if (objSS.queueid) {
            // if started - check if an object of the opposite 'operation' already exists with matching 'queueid'
            var xobjSS = this._getSSEntity(objSS);
            if (xobjSS) {
              //if found - put the 'Stop' Player cmd together
              var objPlayerCmd = this._getPlayerCmd("Stop", objSS, xobjSS);
              // and send to the proper Display Agent
              this._notifyPlayer(objPlayerCmd);
            }
            // done with check - in either case delete this object
            delete this.SS[objSSCmd.operation][objSSCmd.id];
          }
          else {
            // if not started - just delete the obj
            delete this.SS[objSSCmd.operation][objSSCmd.id];
          }
        }
        break;
      case "start":
        // find matching object from 'add'
        var objSS = this.SS[objSSCmd.operation][objSSCmd.id];
        if (objSS) {
          // assign the 'queueid' and 'action' to the matching object - just copy all props to be sure
          for (var i in objSSCmd) {
            objSS[i] = objSSCmd[i];
          }
          // check if an object of the opposite 'operation' already exists with matching 'queueid'
          var xobjSS = this._getSSEntity(objSS);
          if (xobjSS) {
            //if found - put the "Start" Player cmd together
            var objPlayerCmd = this._getPlayerCmd("Start", objSS, xobjSS);
            // and send to the proper Display Agent
            this._notifyPlayer(objPlayerCmd);
          }
        }
        break;
      default:
        break;
    }
  }

  /**
  * check if an object of the opposite 'operation' already exists with matching 'queueid'
  * @param objSS {SSEntity Object}
  * @return xobjSS {SSEntity Object}
  */
  SSAgent_prototype._getSSEntity = function (objSS) {
    // get the opposite 'operation'
    var xop = objSS.operation == 'input' ? 'output' : 'input';
    // look in SSEntity arr
    for (var xobjSSid in this.SS[xop]) {
      var xobjSS = this.SS[xop][xobjSSid];
      // check for matching 'queueid'
      if (xobjSS.queueid == objSS.queueid) {
        // if found - return it
        return xobjSS;
      }
    }
    // not found - return null
    return null;
  }

  /**
  * combine the info from the SS entities and create a player Cmd obj
  * @param action {String}
  * @param objSS {SSEntity Object}
  * @param xobjSS {SSEntity Object}
  * @return objPlayerCmd {PlayerCmd Object}
  */
  SSAgent_prototype._getPlayerCmd = function (action, objSS, xobjSS) {
    // put the SS entity info together
    var objPlayerCmd = new Object;
    for (var i in objSS) {
      objPlayerCmd[i] = objSS[i];
    }
    for (var i in xobjSS) {
      objPlayerCmd[i] = xobjSS[i];
    }
    // set the action
    objPlayerCmd.action = action;

    //get canvasId and winId from 'ioport' - if not already specified
    jsx3.util.strEmpty(objPlayerCmd.canvasId) && (objPlayerCmd.canvasId = objPlayerCmd.ioport.split('.')[0]);
    jsx3.util.strEmpty(objPlayerCmd.windowId) && (objPlayerCmd.windowId = objPlayerCmd.ioport.split('.')[1]);

    return objPlayerCmd;
  }

  /**
  * send PlayerCmd to the right display agent/player
  * @param objPlayerCmd {PlayerCmd Object}
  */
  SSAgent_prototype._notifyPlayer = function (objPlayerCmd) {
    // publish event to Player
    this._LOG.info("com.ipvs.agent.SSAgent: _notifyPlayer: action: " + objPlayerCmd.action +
                    ": canvasId: " + objPlayerCmd.canvasId + ": windowId: " + objPlayerCmd.windowId);
    this.publish({subject:"EVT_PLAYERACTION-" + objPlayerCmd.windowId, context:{ objPLAYERCMD: objPlayerCmd }});
  }
  
  /**
  * Get any missing SSCmd before window loaded
  * @param winId {string}
  */
  SSAgent_prototype.getPlayerAction = function(winId) {
    // find an input object with matching winId
    var op = 'input';
    for (var objSSid in this.SS[op]) {
      var objSS = this.SS[op][objSSid];
      if (objSS.windowId == winId) {
        // check if an object of the opposite 'operation' already exists with matching 'queueid'
        var xobjSS = this._getSSEntity(objSS);
        if (xobjSS) {
          //if found - put the "Start" Player cmd together
          var objPlayerCmd = this._getPlayerCmd("Start", objSS, xobjSS);
          // and send to the proper Display Agent
          this._LOG.info(":getPlayerAction: " + objPlayerCmd.action +
                    ": canvasId: " + objPlayerCmd.canvasId + ": windowId: " + objPlayerCmd.windowId);
          this._notifyPlayer(objPlayerCmd);
        }
        break;
      }
    }
  }

  /**
  * Add resource instance
  * @param portInstanceId {String}
  * @param streamType {String}
  */
  SSAgent_prototype.addResourceInstance = function (portInstanceId, streamType) {
    //make sure this port instance id exists
    if (this.arrResourceInstanceList[portInstanceId] == null) {
      this.arrResourceInstanceList[portInstanceId] = new Object;
    }
    // create a resource instance for thsi port id
    var resourceInstance = dsaResourceInstanceAdd(portInstanceId, streamType);
    var IpPort = dsaResourceInstanceGetRxMediaIpPort(resourceInstance);
    // store this instance in the array
    var instanceId = jsx3.xml.CDF.getKey();
    this.arrResourceInstanceList[portInstanceId][instanceId] = new Object;
    this.arrResourceInstanceList[portInstanceId][instanceId].resourceInstance = resourceInstance;
    this.arrResourceInstanceList[portInstanceId][instanceId].IpPort = IpPort;
    return instanceId;
  }


  /**
  * Delete resource instance
  * @param portInstanceId {String}
  * @param instanceId {String}
  */
  SSAgent_prototype.delResourceInstance = function (portInstanceId, instanceId) {
    if (instanceId != null) {
      if (this.arrResourceInstanceList[portInstanceId][instanceId] != null) {
        var resourceInstance = this.arrResourceInstanceList[portInstanceId][instanceId].resourceInstance;
        dsaResourceInstanceDelete(resourceInstance);
        this.arrResourceInstanceList[portInstanceId][instanceId] = null;
        return true;
      }
    }
    else {
      if (this.arrResourceInstanceList[portInstanceId] != null) {
        for (var tInstance in this.arrResourceInstanceList[portInstanceId]) {
          if (this.arrResourceInstanceList[portInstanceId][tInstance] != null) {
            var resourceInstance = this.arrResourceInstanceList[portInstanceId][tInstance].resourceInstance;
            dsaResourceInstanceDelete(resourceInstance);
            this.arrResourceInstanceList[portInstanceId][tInstance] = null;
          }
        }
        this.arrResourceInstanceList[portInstanceId] = null;
        return true;
      }
    }
    return false;
  }


  /**
  * Get resource instance IP port
  * @param portInstanceId {String}
  * @param instanceId {String}
  */
  SSAgent_prototype.getResourceInstanceIpPort = function (portInstanceId, instanceId) {
    var IpPort = this.arrResourceInstanceList[portInstanceId][instanceId].IpPort;
    return IpPort;
  }

  /**
  * Reset destination stream
  * @param tDst {String}
  */
  SSAgent_prototype.resetStreamDst = function (tDst) {
    if (this.arrDstObj[tDst] == null) throw new jsx3.Exception("DstAgent: Reset: PlayerTopic: " + tDst + ": Not found");
    var objDst = this.arrDstObj[tDst];
    // clear objs
    objDst.ResourceNID = null;
    objDst.MediaRoomNID = null;
    objDst.objMediaRoom = null;
    objDst.nodeMediaRoom = null;
    objDst.SessionID = null;
    objDst.nodeSession = null;
    // clear the lastAction
    objDst.lastAction = "NONE";
    // TBD - things like layout profiles etc..
    // that will come in the messages from the system
  }

  /**
  * Unregister destination stream
  * @param tDst {String}
  */
  SSAgent_prototype.unregisterStreamDst = function (tDst) {
    if (this.arrDstObj[tDst] == null) {
      //calling from cmpWinMediaPlayer_Destroy(), this.arrDstObj[tDst] already gone,avoid to throw exception
      log.warn("DstAgent: UnRegister: PlayerTopic: " + tDst + ": Not found");
      return;
    }
    /// initialize the objects and state vars
    this.resetStreamDst(tDst);
    //When closing MediaPlayer window remove current topic from this.arrDstObj
    delete this.arrDstObj[tDst];
    //remove the this.arrSSAgentInstance[tDst]
    delete this.arrSSAgentInstance[tDst]
  }

  /**
  * Register destination stream
  * @param tDst {String}
  * @param ResourceNID {String}
  */
  SSAgent_prototype.registerStreamDst = function (tDst, ResourceNID) {
    // check / create new entry in arr 
    if (this.arrDstObj[tDst] != null) throw new jsx3.Exception("DstAgent: Register: PlayerTopic: " + tDst + ": Already exists");
    this.arrDstObj[tDst] = new Object;
    // initialize the objects and state vars
    this.resetStreamDst(tDst);
    // - init dst info
    var objDst = this.arrDstObj[tDst];
    objDst.ResourceNID = ResourceNID;
    //initial arrSSAgentInstance 
    this.arrSSAgentInstance[tDst] = new Object;
  }

  /**
  * delete connection for a window
  * @param winId or ConnId {String}
  */
  SSAgent_prototype.sendSSCmdDeleConn = function (windowId) {
    var sendRequest = false;
    var Type = null;
    var Id = null;
    var xop = 'output';
    for (var objSSid in this.SS[xop]) {
      var xobjSS = this.SS[xop][objSSid];
      var ssWindowId = xobjSS.ioport.split('.')[1];
      if (ssWindowId == windowId) {
        Type = xobjSS.dsttype;
        Id = xobjSS.id;
        sendRequest = true;
        break;
      }
    }
    //send the request/message to delete/stop the connection
    if (sendRequest) {
      //  this._sendDelConnAgentEventMessage(windowId, Id, Type);
      //TBD: clean up systemservice request later
      var AgentJID = this.clsApp.clsServiceAgent.getLoginParams().LoginUserJID;
      var timeStamp = (newServerDate()).getTime();
      var agentEventXML = "<AgentEvent><header><eventLevel>200</eventLevel><eventAgentJID>" + AgentJID + "</eventAgentJID><eventWallclock>" + timeStamp + "</eventWallclock><eventname>SSEvent</eventname></header><data><SSEventData><action>SSEvent</action><SSEventActionData><SSNotification><Header><EventType>Stopped</EventType><Id>" + ssCmdId + "</Id><Type>" + ssCmdPortType + "</Type><Status>0</Status><WallClock>" + timeStamp + "</WallClock></Header><Data><QueueId>0</QueueId><Status>Stopped</Status><StopTimeStamp>" + timeStamp + "</StopTimeStamp><StopWallClock>" + timeStamp + "</StopWallClock></Data></SSNotification></SSEventActionData></SSEventData></data><log></log></AgentEvent>";
      var agentEventXMLDoc = new jsx3.xml.Document();
      agentEventXMLDoc.loadXML(agentEventXML);
      var agentEventNode = agentEventXMLDoc.selectSingleNode("/*").cloneNode(true);
      //attching the AgentJID and agentEventNode to DelConnAgentEventMessage request
      var reqParams = new Obejct;
      reqParams.AgentJID = AgentJID;
      reqParams.agentEventNode = agentEventNode;
      com.ipvs.api.API.DelConnAgentEventMessage(reqParams).when(jsx3.$F(function(objResponse) {
        var respStatus = objResponse.Status;
        var response = objResponse.Val;
        if (respStatus == jsx3.net.Request.STATUS_OK) {
          this._LOG.info("DelConnAgentEventMessage:respStatus:" + respStatus);
        }
        else {
          this._LOG.error(":sendSSCmdDeleConn : DelConnAgentEventMessage response status:" + respStatus);
        }
      }).bind(this));
    }
  }
  
});

