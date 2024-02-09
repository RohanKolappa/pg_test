jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.BlockPanel");

jsx3.Class.defineClass('com.ipvs.gui.EndPointPanel', com.ipvs.gui.BlockPanel, [jsx3.gui.Form, jsx3.xml.Cacheable, jsx3.xml.CDF], function (EndPointPanel, EndPointPanel_prototype) {

  //MouseOver change images
  EndPointPanel.ICON_MINUS = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/minus.gif');
  EndPointPanel.ICON_PLUS = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/plus.gif');
  EndPointPanel.BUTTON_DELETEUP = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnDeleteUp.png');
  EndPointPanel.BUTTON_DELETEOVER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnDeleteOver.png');
  EndPointPanel.BUTTON_WINDOWINGUP = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnWindowingControllerOff.png');
  EndPointPanel.BUTTON_WINDOWINGOVER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnWindowingController.png');
  EndPointPanel.BUTTON_SPACER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/spacer.gif');

  //Event Variables 
  EndPointPanel.DeleteBlock = "jsxdeleteblock";
  EndPointPanel.Windowing = "jsxwindowing";
  EndPointPanel.SelectConnection = "jsxconnection";
  EndPointPanel.DeleteConnection = "jsxdeleteConnection";
  EndPointPanel.SessionInfo = "jsxsessionInfo";

  /**
  * {String} EndPoint Fields
  */
  EndPointPanel.FORM_FIELD_VIEW = new Object;
  EndPointPanel.FORM_FIELD_VIEW["EndPointType"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxdevimg',
    LBL: 'jsxmaxInstanceCount'
  },
    VALUES: { 1: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/icons/pc_screen.png'
    },
      16: { LBL: null,
        IMG: 'jsxaddin://user!ipvsCommon/images/icons/computer.png'
      },
      DEFAULT: { LBL: null,
        IMG: 'jsxaddin://user!ipvsCommon/images/icons/pc_screen.png'
      }
    }
  };

  EndPointPanel.FORM_FIELD_VIEW["Windowing"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxfeaturename',
    LBL: null
  },
    VALUES: { XP200DisplayConfig: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/buttons/btnWindowingControllerOff.png'
    },
      DEFAULT: { LBL: null,
        IMG: 'jsxaddin://user!ipvsCommon/images/buttons/spacer.gif'
      }
    }
  };

  EndPointPanel.FORM_FIELD_VIEW["DeleteBlock"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxendpoint',
    LBL: null
  },
    VALUES: { DEFAULT: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/buttons/btnRemoveUp.png'
    }
    }
  };

  EndPointPanel.FORM_FIELD_VIEW["SessionPlus"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxsession',
    LBL: null
  },
    VALUES: { DEFAULT: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/buttons/minus.gif'
    }
    }
  };

  EndPointPanel.FORM_FIELD_VIEW["DeleteConnection"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxconnection',
    LBL: null
  },
    VALUES: { DEFAULT: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/buttons/btnDeleteUp.png'
    }
    }
  };

  EndPointPanel.FORM_FIELD_VIEW["ErrorConnection"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxerrorimg',
    LBL: null
  },
    VALUES: { Error: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_error_small.png'
    },
      DEFAULT: { LBL: null,
        IMG: 'jsxaddin://user!ipvsCommon/images/buttons/spacer.gif'
      }
    }
  };

  /**
  * Method to return a handle to the Form Field View Array
  */
  EndPointPanel_prototype.getFormFieldViewArr = function () {
    return EndPointPanel.FORM_FIELD_VIEW;
  };

  /**
  * Returns the jsxsessionid of this SessionBlk.
  * @return {String}
  */
  EndPointPanel_prototype.getSessionId = function (sessionNode) {
    return sessionNode ? sessionNode.getAttribute('jsxsessionid') || this.jsxsessionid : this.jsxsessionid;
  };

  /**
  * Returns the Text of the label of this SessionBlk.
  * @return {String}
  */
  EndPointPanel_prototype.getSessionText = function (sessionNode) {
    return sessionNode ? sessionNode.getAttribute('jsxsessionname') || this.jsxsessionname : this.jsxsessionname;
  };

  /**
  * Sets the Text of the label of this SessionBlk.
  * @param strLabel {String}
  * @return {com.ipvs.gui.EndPointPanel} this object
  */
  EndPointPanel_prototype.setSesionText = function (strLabel, sessionNode) {
    if (sessionNode) {
      sessionNode.setAttribute('jsxsessionname', strLabel);
      return sessionNode;
    }
    else {
      this.jsxsessionname = strLabel;
      return this;
    }
  };

  /* Returns the height for a Connection Container based on number of connections in the session 
  * @return {int | String} 
  */
  EndPointPanel_prototype.getSessionHeight = function (sessionNode) {
    var height = 22;
    if (!sessionNode) return;
    var numberOfConnections = 0;
    var connIterator = this.getIterableConnections(sessionNode);
    while (connIterator.hasNext()) {
      numberOfConnections++;
      connIterator.next();
    }
    if (numberOfConnections >= 1) {
      height = height * numberOfConnections;
    }
    return height;
  };

  /**
  * Returns the jsxconnectionid of this ConnectionBlk.
  * @return {String}
  */
  EndPointPanel_prototype.getConnectionId = function (connectionNode) {
    return connectionNode ? connectionNode.getAttribute('jsxconnectionid') || this.jsxconnectionid : this.jsxconnectionid;
  };

  /**
  * Returns the Text of the label of this ConnectionBlk.
  * @return {String}
  */
  EndPointPanel_prototype.getConnectionText = function (connectionNode) {
    var connName = this.jsxconnectionname;
    if (connectionNode) {
      connName = connectionNode.getAttribute('jsxconnectionname');
      var windowId = connectionNode.getAttribute('jsxwindowid');
      if (!jsx3.util.strEmpty(windowId)) {
        connName = connName + " [" + windowId + "]";
      }
    }
    return this.getEllipsisText(connName, 27);
  };

  /**
  * Sets the Text of the label of this ConnectionBlk.
  * @param strLabel {String}
  * @return {com.ipvs.gui.EndPointPanel} this object
  */
  EndPointPanel.setConnectionText = function (strLabel, connectionNode) {
    if (connectionNode) {
      connectionNode.setAttribute('jsxconnectionname', strLabel);
      return connectionNode;
    }
    else {
      this.connectionNode = strLabel;
      return this;
    }
  };

  /**
  * Returns the CDF Session Node with jsxsessionid equals strRecordId.
  * @param strRecordId {String}
  * @return {String}
  */
  EndPointPanel_prototype.getSessionNode = function (strRecordId) {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectSingleNode(".//record/PortSessionInfo[@jsxsessionid ='" + strRecordId + "']") : null;
  };

  /**
  * Returns the Session Object with jsxid equals strRecordId.
  * @param strRecordId {String}
  * @return SessionObj
  */
  EndPointPanel_prototype.getSession = function (strRecordId) {
    var sessionNode = this.getSessionNode(strRecordId);
    if (!sessionNode) return;
    var sessionObj = new Object();
    sessionObj.jsxsessionid = sessionNode.getAttribute("jsxsessionid");
    sessionObj.jsxsessionname = sessionNode.getAttribute("jsxsessionname");
    return sessionObj;
  };

  /**
  * Returns the CDF Connection Node with jsxconnectionid equals strRecordId.
  * @param strRecordId {String}
  * @return {String}
  */
  EndPointPanel_prototype.getConnectionNode = function (strRecordId) {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectSingleNode(".//record/PortSessionInfo/PortConnectionInfo[@jsxconnectionid ='" + strRecordId + "']") : null;
  };

  /**
  * Returns the Connection Object with jsxconnectionid equals strRecordId.
  * @param strRecordId {String}
  * @return ConnectionObj
  */
  EndPointPanel_prototype.getConnection = function (strRecordId) {
    var connectionNode = this.getConnectionNode(strRecordId);
    if (!connectionNode) return;
    var connectionObj = new Object();
    connectionObj.jsxconnectionid = connectionNode.getAttribute("jsxconnectionid");
    connectionObj.jsxconnectionname = connectionNode.getAttribute("jsxconnectionname");
    return connectionObj;
  }

  /**
  * Get an iterator for the sessions that will be painted
  * Returns all the sessions in the given record.
  *
  * @return {jsx3.util.Iterator}
  */
  EndPointPanel_prototype.getIterableSessions = function (record) {
    return record ? record.selectNodeIterator(".//PortSessionInfo") :
    (new jsx3.util.List()).iterator();
  };

  /**
  * Get an iterator for the connections that will be painted
  * Returns all the connections in the given session. 
  *
  * @return {jsx3.util.Iterator}
  */
  EndPointPanel_prototype.getIterableConnections = function (record) {
    return record ? record.selectNodeIterator(".//PortConnectionInfo") :
    (new jsx3.util.List()).iterator();
  };

  /**
  * Returns the Img of the delete connection/session button of this panel based on the current state.
  * @return {String}
  */
  EndPointPanel_prototype.getCurrentDeleteImg = function (bOver, record) {
    if (bOver) {
      strPath = EndPointPanel.BUTTON_DELETEOVER;
    }
    else {
      strPath = EndPointPanel.BUTTON_DELETEUP;
    }
    return strPath;
  };

  /**
  * Returns the Img of the delete windowing button of this panel based on the current state.
  * @return {String}
  */
  EndPointPanel_prototype.getCurrentWindowingImg = function (bOver, record) {
    var windowing = record.getAttribute("jsxfeaturename");
    if (!windowing) return EndPointPanel.BUTTON_SPACER;
    if (bOver) {
      strPath = EndPointPanel.BUTTON_WINDOWINGOVER;
    }
    else {
      strPath = EndPointPanel.BUTTON_WINDOWINGUP;
    }
    return strPath;
  };

  /**
  * Returns the SpyGlassMessage string for Errored Connection in this block panel.
  * @return {String}
  */
  EndPointPanel_prototype.getErrorMessage = function (record) {
    if (!record) return;
    var conStatus = record.getAttribute("jsxconnectionstate");
    if ((conStatus == "SETUP_ERROR") ||
        (conStatus == "START_ERROR") ||
        (conStatus == "SOURCE_ERROR") ||
        (conStatus == "MEDIA_ERROR") ||
        (conStatus == "DEST_ERROR") ||
        (conStatus == "error")) {
      var errorCode = record.getAttribute("jsxerrorcode");
      var errorData = record.getAttribute("jsxerrordata");
      if (!jsx3.util.strEmpty(errorCode) && !jsx3.util.strEmpty(errorData)) {
        var message = "Error :" + conStatus + "\n" + "ErrorCode : " + errorCode + "\n" + "ErrorData :" + errorData;
      }
      else {
        var message = "Error :" + conStatus;
      }
      return message;
    }
    return;
  };

  /**
  * Returns the Css of the Connection of this panel based on the current state.
  * @return {String}
  */
  EndPointPanel_prototype.getCurrentConnectionClass = function (record) {
    var strClass = null;
    if (!record) return;
    var isSelected = record.getAttribute("jsxselected");
    if (isSelected == "1") {
      strClass = this.getConnectionClassOn();
    }
    return strClass;
  };

  /**
  * Returns the CSS Off Class of this Connection.
  * @return {String}
  */
  EndPointPanel_prototype.getConnectionClassOn = function () {
    return this.jsxconnectionclasson;
  };

  /**
  * Sets the the CSS Off Class of this Connection.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.BlockCanvas} this object
  */
  EndPointPanel_prototype.setConnectionClassOn = function (vntClassName, bUpdateView) {
    this.jsxconnectionclasson = vntClassName;
    return this;
  };

  EndPointPanel_prototype.deselectAllConnections = function (record) {
    if (record) {
      var connectionIterator = record.selectNodeIterator(".//PortSessionInfo/PortConnectionInfo[@jsxselected ='1']");
      while (connectionIterator.hasNext()) {
        var connectionNode = connectionIterator.next();
        connectionNode.removeAttribute("jsxselected");
      }
    }
    else {
      var recordIterator = this.getIterableRecords();
      while (recordIterator.hasNext()) {
        record = recordIterator.next();
        var connectionIterator = record.selectNodeIterator(".//PortSessionInfo/PortConnectionInfo[@jsxselected ='1']");
        while (connectionIterator.hasNext()) {
          var connectionNode = connectionIterator.next();
          connectionNode.removeAttribute("jsxselected");
        }
      }
    }
  }

  //--------------------OnClick------------------
  EndPointPanel_prototype.onClick_Windowing = function (objEvent, objGUI) {
    objEvent.cancelBubble();
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? EndPointPanel.Windowing : jsx3.gui.Interactive.EXECUTE;
    this.doEvent(event, { objEVENT: objEvent });
  };

  EndPointPanel_prototype.onClick_sessionPlus = function (objEvent, objGUI) {
    objEvent.cancelBubble();
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    var plus = objGUI.parentNode.parentNode.parentNode.parentNode.nextSibling.style.overflow;
    if (plus == "hidden") {
      objGUI.parentNode.parentNode.parentNode.parentNode.nextSibling.style.height = "22px";
      objGUI.parentNode.parentNode.parentNode.parentNode.nextSibling.style.overflow = "";
      objGUI.src = EndPointPanel.ICON_MINUS;
    }
    else if (plus == "") {
      objGUI.parentNode.parentNode.parentNode.parentNode.nextSibling.style.height = "2px";
      objGUI.parentNode.parentNode.parentNode.parentNode.nextSibling.style.overflow = "hidden";
      objGUI.src = EndPointPanel.ICON_PLUS;
    }
  };

  EndPointPanel_prototype.onClick_sessionInfo = function (objEvent, objGUI) {
    objEvent.cancelBubble();
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getSessionNode ? this.getSessionNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? EndPointPanel.SessionInfo : jsx3.gui.Interactive.EXECUTE;
    this.doEvent(event, { objEVENT: objEvent });
  };

  EndPointPanel_prototype.onClick_Connection = function (objEvent, objGUI) {
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getConnectionNode ? this.getConnectionNode(id) : null;
    var isUnSelectable = record.getAttribute("jsxunselectable");
    if (isUnSelectable == "1") return;
    var isSelected = record.getAttribute("jsxselected");
    if (isSelected == "1") return;
    this.deselectAllConnections();
    record.setAttribute("jsxselected", "1");
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? EndPointPanel.SelectConnection : jsx3.gui.Interactive.EXECUTE;
    this.doEvent(event, { objEVENT: objEvent });
  };

  EndPointPanel_prototype.onClick_deleteConnection = function (objEvent, objGUI) {
    objEvent.cancelBubble();
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getConnectionNode ? this.getConnectionNode(id) : null;
    if (!record) return;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? EndPointPanel.DeleteConnection : jsx3.gui.Interactive.EXECUTE;
    this.doEvent(event, { objEVENT: objEvent });
  };

  //---------------------- On MouseOver and MouseOut--------------------------

  EndPointPanel_prototype.onMouseOver_Windowing = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowingImg(true, record);
  };

  EndPointPanel_prototype.onMouseOut_Windowing = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowingImg(false, record);
  };

  EndPointPanel_prototype.onMouseOver_deleteSessionConnection = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteImg(true, record);
    this.setTip("Delete");
  };

  EndPointPanel_prototype.onMouseOut_deleteSessionConnection = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteImg(false, record);
    this.setTip("");
  };

  EndPointPanel_prototype.onMouseOver_sessionInfo = function (objEvent, objGUI) {
    objGUI.parentNode.style.textDecorationUnderline = true;
  };

  EndPointPanel_prototype.onMouseOut_sessionInfo = function (objEvent, objGUI) {
    objGUI.parentNode.style.textDecorationNone = true;
  };

  EndPointPanel_prototype.onMouseOver_errorConnection = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getConnectionNode ? this.getConnectionNode(strCdfId) : null;
    var message = this.getErrorMessage(record);
    if (!message) return;
    this.setTip(message);
  };

  /**
  * The XML Template used by the Template class to render this EndPoint Block Panel
  *
  * @return {String}
  */
  EndPointPanel_prototype.getTemplateXML = function () {
    // form template string
    return ['',
    '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
    '  <model>',
    '  </model>',
    '  <template dom="static" recalc="true">',
    ' <inlinebox u:id="endpointpanel" class="jsx30block.endpointContainer" style="position:absolute;left:0px;top:26px;overflow:auto;cursor:default;display:block;overflow-x:hidden;" onscroll="{onScroll}">',
    ' <div style="height:100%;">',
    ' <for-each select="this.getIterableRecords()">',
    '   <var id="endpointid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '   <var id="buttonid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '   <var id="buttoncloseid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '   <var id="buttondendpointclose">this.getCurrentFieldImgURL("DeleteBlock", $$target)</var>',
    '   <var id="buttonwindowingid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '   <var id="buttonwindowing">this.getCurrentFieldImgURL("Windowing", $$target)</var>',
    '   <var id="buttonendpointtype">this.getCurrentFieldImgURL("EndPointType", $$target)</var>',
    '   <var id="cssclass">this.getCurrentClass(false,$$target)</var>',
    '   <var id="endpointtext">this.getText($$target)</var>',
    '   <var id="endpoint_target">$$target</var>',
    '   <div u:id="endpointblk" id="{endpointid}" class="{cssclass}" style="margin:0;padding:0;position:relative;width:280px;display:block;font-family:Arial, Helvetica, sans-serif;" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}"  onclick="{onClick}">',
    //target Blocks
    '     <div u:id="targetblk" id="{buttonid}" style="position:relative;width:280px;height:30px;overflow:hidden;text-align:right;display:block;">',
    '        <span u:id="imgEndPoint" style="position:absolute;width:24px;height:24px;left:4px;top:3px;overflow:hidden;">',
    '            <img unselectable="on" class="jsx30image" src="{buttonendpointtype}" width="24" height="24"/>',
    '        </span>',
    '        <span u:id="titleEndPoint" style="position:absolute; left:36px; top:6px; font-size:12px; font-weight:bold;">',
    '        <text>{endpointtext}</text>',
    '        </span>',
    '         <span style="position:absolute;width:16px;height:16px;left:220px;top:6px;cursor:pointer;display:none;">',
    '            <div style="position:relative;height:16px;">',
    '                <img src="{buttonwindowing}" id="{buttonwindowingid}" onclick="{onClick_Windowing}" onmouseover="{onMouseOver_Windowing}" onmouseout="{onMouseOut_Windowing}" width="16" height="16"/>',
    '            </div>',
    '         </span>',
    '        <span style="position:absolute;width:16px;height:16px;left:246px;top:3px;cursor:pointer;">',
    //close EndPoint button
    '           <div u:id="closetarget" style="position:relative;height:16px;" >',
    '                <img src="{buttondendpointclose}" id="{buttoncloseid}" onclick="{onClick_delete}" onmouseover="{onMouseOver_delete}" onmouseout="{onMouseOut_delete}" width="16" height="16"/>',
    '           </div>',
    '        </span>',
    '     </div>',
    //Session Container
    '     <div class="endpointExpand" style="position:relative;width:280px;display:block;">',
    '        <for-each select="this.getIterableSessions(endpoint_target)">',
                 this.getSessionTemplateXML(),
    '        </for-each>',
    '      </div>',
    '    </div>',
    '   </for-each>',
    '   </div>',
    '   </inlinebox>',
    '  </template>',
    '</transform>'].join("");
  };

  /**
  * The nested XML Template for each Session Block in this EndPoint Block Panel
  * Override this for different Internal blocks
  * @return {String}
  */
  EndPointPanel_prototype.getSessionTemplateXML = function () {
    return ['',
    '<var id="buttonclosesessionid">this.getId() + \'_\' + $$target.getAttribute(\'jsxsessionid\')</var>',
    '<var id="buttonsessioninfoid">this.getId() + \'_\' + $$target.getAttribute(\'jsxsessionid\')</var>',
    '<var id="buttonsessionplus">this.getCurrentFieldImgURL("SessionPlus", $$target)</var>',
    '<var id="buttonsessionplusid">this.getId() + \'_\' + $$target.getAttribute(\'jsxsessionid\')</var>',
    '<var id="sessionid">this.getId() + \'_\'+ this.getSessionId($$target)</var>',
    '<var id="sessiontext">this.getSessionText($$target)</var>',
    '<var id="sessionheight">this.getSessionHeight($$target)+ \'px\'</var>',
    '<var id="session_target">$$target</var>',
    //Session block
    '   <inlinebox u:id="sessionblk" id="{sessionid}" style="position:relative;width:278px;height:20px;overflow:hidden;text-align:right;display:block;" >',
    '    <div style="margin:0;padding:0;position:relative;text-align:left;">',
    '       <span style="position:absolute;width:9px;height:9px;left:23px;top:6px;cursor:pointer;">',
    '            <div u:id="sessionplus" style="position:relative;height:9px;" >',
    '             <img src="{buttonsessionplus}" id="{buttonsessionplusid}" onclick="{onClick_sessionPlus}" width="9" height="9"/>',
    '            </div>',
    '       </span>',
    '       <span id="{buttonsessioninfoid}" style="position:absolute;left:38px;top:3px;text-align:left;font-size:11px;cursor:pointer;" ondblclick="{onClick_sessionInfo}" onmouseover="{onMouseOver_sessionInfo}" onmouseout="{onMouseOut_sessionInfo}">',
    '       <text>{sessiontext}</text>',
    '       </span>',
    '     </div>',
    '   </inlinebox>',
    //Connection Container
    '   <div class="endpointExpand" u:\protected="true" style="position:relative;width:278px;height:{sessionheight};display:block;">',
    '       <for-each select="this.getIterableConnections(session_target)">',
                this.getConnectionTemplateXML(),
    '       </for-each>',
    '   </div>'].join("");
  }

  /**
  * The nested XML Template for each Connection Block in this  Block Panel
  * Override this for different Internal blocks
  * @return {String}
  */
  EndPointPanel_prototype.getConnectionTemplateXML = function () {
    return ['',
    '<var id="buttoncloseconnectionid">this.getId() + \'_\' + $$target.getAttribute(\'jsxconnectionid\')</var>',
    '<var id="buttonconnectionerrorid">this.getId() + \'_\' + $$target.getAttribute(\'jsxconnectionid\')</var>',
    '<var id="buttonconnectionclose">this.getCurrentFieldImgURL("DeleteConnection", $$target)</var>',
    '<var id="connectionid">this.getConnectionId($$target)</var>',
    '<var id="connectiontext">this.getConnectionText($$target)</var>',
    '<var id="cssconnectionclass">this.getCurrentConnectionClass($$target)</var>',
    '<var id="buttonconnectionerror">this.getCurrentFieldImgURL("ErrorConnection", $$target)</var>',
    //Connection Block 
    '   <div id="{buttoncloseconnectionid}" style="position:relative;width:278px;height:22px;overflow:hidden;text-align:right;display:block;" class="{cssconnectionclass}" onclick="{onClick_Connection}">',
    '       <span style="position:absolute;width:12px;height:12px;left:35px;top:4px;overflow:hidden;display:block;">',
    '         <img unselectable="on" class="jsx30image" src="{buttonconnectionerror}" id="{buttonconnectionerrorid}" onmouseover="{onMouseOver_errorConnection}" width="12" height="12"/>',
    '       </span>',
    '       <span style="position:absolute;left:50px;top:4px;font-size:11px;text-align:left;width:170px;overflow:hidden;font-weight:normal;white-space:nowrap;">',
    '         <text>{connectiontext}</text>',
    '       </span>',
     '       <span style="position:absolute;width:16px;height:16px;left:235px;top:4px;cursor:pointer;">',
    '            <div style="position:relative;height:16px;" >',
    '                <img src="{buttonconnectionclose}" id="{buttoncloseconnectionid}" width="16" height="16" onclick="{onClick_deleteConnection}" onmouseover="{onMouseOver_deleteSessionConnection}" onmouseout="{onMouseOut_deleteSessionConnection}"/>',
    '            </div>',
    '       </span>',
    '   </div>'].join("");
  }

});