/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This is a device discovery agent which will maintain the list of discovered devices on the network
*/

jsx3.Class.defineClass('com.ipvs.agent.DeviceDiscoveryAgent', null, [jsx3.util.EventDispatcher], function (DeviceDiscoveryAgent, DeviceDiscoveryAgent_prototype) {

  DeviceDiscoveryAgent.STR_CDFRECORD = '<record jsxid="" jsxstatus="" jsxmacaddress="" jsxtype="" jsxipaddress="" jsxtext=""/>';
  DeviceDiscoveryAgent.STR_CDF = '<data jsxid="jsxroot"></data>';

  //DISCOVERYDEVICEUPDATE: will update the list listener
  DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE = "OnDiscoveryDeviceUpdate";
  DeviceDiscoveryAgent.DISCOVERYDEVICEUPDATE = "DiscoveryDeviceUpdate";
  DeviceDiscoveryAgent.DISCOVERYADDDEVICE = "DiscoveryAddDevice";
  DeviceDiscoveryAgent.DISCOVERYDELETEDEVICE = "DiscoveryDeleteDevice";

  DeviceDiscoveryAgent.ON_DISCOVERY_READY = "OnDiscoveryReady";
  DeviceDiscoveryAgent.DISCOVERYREADYSTATE = "DiscoveryReadyState";

  DeviceDiscoveryAgent.DISCOVERYREADYSTATE_NONE = "NONE";
  DeviceDiscoveryAgent.DISCOVERYREADYSTATE_SUCCESS = "VALID";

  DeviceDiscoveryAgent.ON_DEVICE_GETDATA = "OnDeviceGetData";
  DeviceDiscoveryAgent.DEVICEDATAUPDATE = "DeviceDataUpdate";
  DeviceDiscoveryAgent.DEVICEDATAUPDATE_SHOW = "SHOW";
  DeviceDiscoveryAgent.DEVICEDATAUPDATE_HIDE = "HIDE";
  DeviceDiscoveryAgent._DeviceTypeXMLFile = "JSXAPPS/IPVS_AdminUI/xml/selections/DeviceType.xml";

  // create Logger for this class
  DeviceDiscoveryAgent_prototype._LOG = jsx3.util.Logger.getLogger(DeviceDiscoveryAgent.jsxclass.getName());

  /**
  * The instance initializer.
  * @param eventApp {Object}
  */
  DeviceDiscoveryAgent_prototype.init = function (eventApp) {

    this._LOG.debug("com.ipvs.agent.DeviceDiscoveryAgent.init");

    // Display Config Document
    this.DOC_CDF = new jsx3.xml.Document().loadXML(DeviceDiscoveryAgent.STR_CDF);

    // list (assoc. array) of New devices
    this.arrDeviceAgentJIDs = {};
    this._linklocalStateParams = [];
    this._setStateParam(DeviceDiscoveryAgent.DISCOVERYREADYSTATE, DeviceDiscoveryAgent.DISCOVERYREADYSTATE_NONE);

    if (!jsx3.util.strEmpty(eventApp)) {
      // setup event subscriptions
      this.eventWrap(eventApp);
    }
  };

  /** @private @jsxobf-clobber */
  DeviceDiscoveryAgent_prototype._setStateParam = function (paramName, paramValue) {
    this._linklocalStateParams[paramName] = paramValue;
  };

  /**
  * Returns the login state.
  * @param paramName {String}
  * @return {String} login state.
  */
  DeviceDiscoveryAgent_prototype.getStateParam = function (paramName) {
    return this._linklocalStateParams[paramName];
  };

  /**
  * The restart LL Service method.
  * 
  */
  DeviceDiscoveryAgent_prototype._resetServiceVariables = function () {
    this._setStateParam(DeviceDiscoveryAgent.DISCOVERYREADYSTATE, DeviceDiscoveryAgent.DISCOVERYREADYSTATE_NONE);
    this.DOC_CDF = new jsx3.xml.Document().loadXML(DeviceDiscoveryAgent.STR_CDF);
    for (var deviceAgentJID in this.arrDeviceAgentJIDs) {
      delete this.arrDeviceAgentJIDs[deviceAgentJID];
    }
    this.arrDeviceAgentJIDs = {};
  }

  /**
  * The restart LL Service method.
  * 
  */
  DeviceDiscoveryAgent_prototype.restartDiscoveryService = function () {
    this._publishShowSpinner();
    this._resetServiceVariables();
    this._publishDeviceUpdate();
    this.startDiscoveryService();

  }


  /**
  * The start LL Service method.
  * 
  */
  DeviceDiscoveryAgent_prototype.startDiscoveryService = function () {
    var numAgentJID = 0;
    for (var deviceAgentJID in this.arrDeviceAgentJIDs) {
      numAgentJID++
    }
    // if discovery is started then publish data ready.
    if (this.getStateParam(DeviceDiscoveryAgent.DISCOVERYREADYSTATE) == DeviceDiscoveryAgent.DISCOVERYREADYSTATE_SUCCESS && numAgentJID != 0) {
      this._publishShowSpinner();
      this._publishDeviceUpdate();
      this._publishHideSpinner();
      return;
    }
    // start the service
    this._LOG.debug("com.ipvs.agent.DeviceDiscoveryAgent.StartDiscoveryService");

    var urlStr = 'ipvs:///StartDiscoveryService?ServiceName=Discovery' +
                 '&OpType=Operation' +
                 '&ObjectType=StartDiscoveryService';

    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(urlStr);
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        this._LOG.info("StartDiscoveryService Success");
        this._setStateParam(DeviceDiscoveryAgent.DISCOVERYREADYSTATE, DeviceDiscoveryAgent.DISCOVERYREADYSTATE_SUCCESS);
        // publish to update the list
        this._publishDeviceUpdate();
        // publish to hide the spinner
        this._publishHideSpinner();
      }
      else {
        this._LOG.error("StartDiscoveryService error. Error code:" + respStatus);
      }
      delete serviceAppletReq;
    }

    req.subscribe("*", this, onRequestEvent);
    this._LOG.info("Sending StartDiscoveryService request...");
    req.send();
  };

  /**
  * The stop LL Service method.
  * 
  */
  DeviceDiscoveryAgent_prototype.stopDiscoveryService = function () {
    this._resetServiceVariables();

    this._LOG.debug("com.ipvs.agent.DeviceDiscoveryAgent.StopDiscoveryService");

    var urlStr = 'ipvs:///StopDiscoveryService?ServiceName=Discovery' +
                 '&OpType=Operation' +
                 '&ObjectType=StopDiscoveryService';

    var uri = jsx3.net.URIResolver.DEFAULT.resolveURI(urlStr);
    var serviceAppletReq = new com.ipvs.api.Request("ServiceAppletRequest");
    var req = serviceAppletReq.open("GET", uri, true);
    var onRequestEvent = function (objEvent) {
      var Request = jsx3.net.Request;
      var req = objEvent.target;
      req.unsubscribe("*", this, onRequestEvent);
      var respStatus = req.getStatus();
      if (respStatus == Request.STATUS_OK) {
        this._LOG.info("StopDiscoveryService Success");
      }
      else {
        this._LOG.error("StopDiscoveryService error. Error code:" + respStatus);
      }
      delete serviceAppletReq;
    }

    req.subscribe("*", this, onRequestEvent);
    this._LOG.info("Sending StopDiscoveryService request...");
    req.send();

  };

  /**
  * gets the device config.
  * 
  */
  DeviceDiscoveryAgent_prototype._getDisplayDeviceType = function (deviceType) {
    var returnDeviceType = "";
    if (jsx3.util.strEmpty(deviceType)) return returnDeviceType;

    var deviceNode = loadXMLFile(DeviceDiscoveryAgent._DeviceTypeXMLFile);
    var recordNode = deviceNode.selectSingleNode(".//record[@jsxid='" + deviceType + "']");
    returnDeviceType = recordNode.getAttribute("jsxtext");
    return returnDeviceType;
  }

  /**
  * gets the device reoutetable.
  * 
  */
  DeviceDiscoveryAgent_prototype.getDeviceAgentJIDRouteTableNode = function (deviceAgentJID) {
    var devicedataNode = this.arrDeviceAgentJIDs[deviceAgentJID];
    var routeTableNode = devicedataNode.selectSingleNode(".//RouteTableConfig");
    return routeTableNode;
  }

  /**
  * sets the device node.
  * 
  */
  DeviceDiscoveryAgent_prototype.setDeviceNode = function (deviceAgentJID, deviceNode) {
    if (this.arrDeviceAgentJIDs[deviceAgentJID]) {
      this.arrDeviceAgentJIDs[deviceAgentJID] = deviceNode;
    }
  }

  /**
  * publish to refresh view list with filter.
  * 
  */
  DeviceDiscoveryAgent_prototype.applyFilterForDiscoveryList = function () {
    this._publishDeviceUpdate();
  }

  /**
  * publishes data update to the listener
  * 
  */
  DeviceDiscoveryAgent_prototype._publishDeviceMessage = function (agentJID, deviceMessage) {
    this.publish({ subject: DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE,
      stateVar: agentJID,
      stateVal: deviceMessage
    });
  }

  /**
  * publishes data update to the listener
  * 
  */
  DeviceDiscoveryAgent_prototype._publishDeviceUpdate = function () {
    this.publish({ subject: DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE,
      stateVar: DeviceDiscoveryAgent.DISCOVERYDEVICEUPDATE,
      stateVal: (newServerDate()).getTime()
    });
  }

  /**
  * publishes Add data update to the listener
  * 
  */
  DeviceDiscoveryAgent_prototype._publishAddDeviceUpdate = function (cdfRecord) {
    this.publish({ subject: DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE,
      stateVar: DeviceDiscoveryAgent.DISCOVERYADDDEVICE,
      stateVal: cdfRecord
    });
  }

  /**
  * publishes to show spinner
  * 
  */
  DeviceDiscoveryAgent_prototype._publishShowSpinner = function () {
    this.publish({ subject: DeviceDiscoveryAgent.ON_DEVICE_GETDATA,
      stateVar: DeviceDiscoveryAgent.DEVICEDATAUPDATE,
      stateVal: DeviceDiscoveryAgent.DEVICEDATAUPDATE_SHOW
    });
  }

  /**
  * publishes to hide spinner
  * 
  */
  DeviceDiscoveryAgent_prototype._publishHideSpinner = function () {
    this.publish({ subject: DeviceDiscoveryAgent.ON_DEVICE_GETDATA,
      stateVar: DeviceDiscoveryAgent.DEVICEDATAUPDATE,
      stateVal: DeviceDiscoveryAgent.DEVICEDATAUPDATE_HIDE
    });
  }

  /**
  * return the cdf doc
  * 
  */
  DeviceDiscoveryAgent_prototype.getCDFDoc = function () {
    return this.DOC_CDF.cloneNode(true);
  }

  /**
  * return the number of records
  * 
  */
  DeviceDiscoveryAgent_prototype.getTotalCount = function () {
    var objXML = this.DOC_CDF.cloneNode(true);
    var recLength = objXML.selectNodes(".//record").getLength();
    if (jsx3.util.strEmpty(recLength) || recLength == "undefined") {
      recLength = 0;
    }
    return recLength.toString();
  }

  /**
  * return the cdf doc of specified device type
  * 
  */
  DeviceDiscoveryAgent_prototype.getSpecificDeviceTypeCDFDoc = function (deviceType) {
    var objXML = this.DOC_CDF.cloneNode(true);
    if (this.getStateParam(DeviceDiscoveryAgent.DISCOVERYREADYSTATE) == DeviceDiscoveryAgent.DISCOVERYREADYSTATE_SUCCESS) {
      var recordIter = objXML.selectNodeIterator(".//record[@jsxorgtype !='" + deviceType + "']");
      while (recordIter.hasNext()) {
        var recordNode = recordIter.next();
        recordNode.getParent().removeChild(recordNode);
      }
    }
    else {
      objXML = new jsx3.xml.Document().loadXML(DeviceDiscoveryAgent.STR_CDF);
    }
    return objXML;
  }

  /**
  * return the number of specific device records
  * 
  */
  DeviceDiscoveryAgent_prototype.getSpecificDeviceTotalCount = function (deviceType) {
    var objXML = this.DOC_CDF.cloneNode(true);
    var recLength = objXML.selectNodes(".//record[@jsxorgtype ='" + deviceType + "']").getLength();
    if (jsx3.util.strEmpty(recLength) || recLength == "undefined") {
      recLength = 0;
    }
    return recLength.toString();
  }

  /**
  * function to delete a specific agentjid from the list
  * 
  */
  DeviceDiscoveryAgent_prototype._deleteDeviceAgentJID = function (deviceAgentJID) {
    // remove the array
    delete this.arrDeviceAgentJIDs[deviceAgentJID];
    // remove from the cdf doc
    var recordNode = this.DOC_CDF.selectSingleNode(".//record[@jsxid='" + deviceAgentJID + "']");
    if (!jsx3.util.strEmpty(recordNode)) {
      this.DOC_CDF.removeChild(recordNode);
      // publish to update the list
      this._publishDeleteDeviceUpdate(deviceAgentJID);
    }
  }

  /**
  * publishes delete data update to the listener
  * 
  */
  DeviceDiscoveryAgent_prototype._publishDeleteDeviceUpdate = function (recordId) {
    this.publish({ subject: DeviceDiscoveryAgent.ON_DISCOVERY_DEVICE_UPDATE,
      stateVar: DeviceDiscoveryAgent.DISCOVERYDELETEDEVICE,
      stateVal: recordId
    });
  }

  /**
  * process the presence.
  * 
  */
  DeviceDiscoveryAgent_prototype._processPresence = function (presenceObj) {
    var deviceAgentJID = presenceObj.name;
    var isRegistered = presenceObj.registered;
    var isAvailable = presenceObj.available;

    var devicveMessage = (isRegistered == "No") ? "UnRegistered" : "Registered";
    if (isAvailable == "true") {
      var recordNode = new jsx3.xml.Document().loadXML(DeviceDiscoveryAgent.STR_CDFRECORD);
      recordNode.setAttribute("jsxid", deviceAgentJID);
      recordNode.setAttribute("jsxstatus", devicveMessage);
      recordNode.setAttribute("jsxmacaddress", presenceObj.macaddress);
      recordNode.setAttribute("jsxtype", this._getDisplayDeviceType(presenceObj.type + "-Device"));
      recordNode.setAttribute("jsxorgtype", presenceObj.type + "-Device");
      recordNode.setAttribute("jsxipaddress", presenceObj.ipaddress);
      recordNode.setAttribute("jsxport", presenceObj.port);
      recordNode.setAttribute("jsxtext", presenceObj.name);
      if (!this.arrDeviceAgentJIDs[deviceAgentJID]) {
        this.arrDeviceAgentJIDs[deviceAgentJID] = "NONE";
        if (this.getStateParam(DeviceDiscoveryAgent.DISCOVERYREADYSTATE) == DeviceDiscoveryAgent.DISCOVERYREADYSTATE_SUCCESS) {
          //save the response in the array
          this.arrDeviceAgentJIDs[deviceAgentJID] = "DONE";
          //append the record to the CDF doc
          this.DOC_CDF.appendChild(recordNode);
        }
      }
      else {
        //update the record in CDF doc
        var oldRecordNode = this.DOC_CDF.selectSingleNode(".//record[@jsxid='" + deviceAgentJID + "']");
        if (!jsx3.util.strEmpty(oldRecordNode)) {
          this.DOC_CDF.removeChild(oldRecordNode);
        }
        this.DOC_CDF.appendChild(recordNode);
      }
      // publish to update the list
      this._publishAddDeviceUpdate(recordNode.cloneNode(true));
      // publish the agentjid and device message
      // if the add process is on the status page and if its the same agentjid then it will show registering failed
      this._publishDeviceMessage(deviceAgentJID, devicveMessage);
    }
    // if sttaus is unavail means device has regisstered with ms
    else if (isAvailable != "true") {
      // remove the array and list cdfdoc
      this._deleteDeviceAgentJID(deviceAgentJID);
    }
  }

  /**
  * Envent listener sections
  * @param eventApp {Object}
  */
  DeviceDiscoveryAgent_prototype.eventWrap = function (eventApp) {

    //EventListener for new device presence notification
    eventApp.subscribe("DISCOVERYEVENT", null, this,
                      function (EventVar, EventVal) {
                        var presenceObj = EventVal;
                        //process this presence
                        this._processPresence(presenceObj);

                      });

  }

  /**
  * Unsubscribes all events
  */
  DeviceDiscoveryAgent_prototype.unSubscribeAll = function () {
    this.unsubscribeAllFromAll();
  }

  /** @private @jsxobf-clobber */
  DeviceDiscoveryAgent_prototype._topicPublish = function (stateVar, stateVal) {
    topic_Publish(this.tName, stateVar, stateVal);
  }

});                                                  //END OF CLASS