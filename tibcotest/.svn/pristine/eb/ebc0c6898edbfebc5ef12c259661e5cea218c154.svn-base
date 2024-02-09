jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.BlockPanel");

jsx3.Class.defineClass('com.ipvs.gui.SessionSrcPanel', com.ipvs.gui.BlockPanel, [jsx3.gui.Form, jsx3.xml.Cacheable, jsx3.xml.CDF], function (SessionSrcPanel, SessionSrcPanel_prototype) {

  // create Logger for this class
  SessionSrcPanel_prototype._LOG = jsx3.util.Logger.getLogger(SessionSrcPanel.jsxclass.getName());

  //MouseOver change images
  SessionSrcPanel.BUTTON_DELETEUP = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnDeleteUp.png');
  SessionSrcPanel.BUTTON_DELETEOVER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnDeleteOver.png');
  SessionSrcPanel.BUTTON_SPACER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/spacer.gif');

  //Event Variables  
  SessionSrcPanel.DeleteConnection = "jsxdeleteConnection";


  /**
  * {String} Src Fields
  */
  SessionSrcPanel.FORM_FIELD_VIEW = new Object;
  // Port Presence 
  SessionSrcPanel.FORM_FIELD_VIEW['Presence'] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxdevpresence',
    LBL: 'jsxpresence'
  },
    VALUES: { available: { LBL: 'available',
      IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_greenBall.png'
    },
      unavailable: { LBL: 'unavailable',
        IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_grayBall.png'
      },
      DEFAULT: { LBL: null,
        IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_lightgrayBall.png'
      }
    }
  };

  SessionSrcPanel.FORM_FIELD_VIEW["State"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxtstate',
    LBL: null
  },
    VALUES: { DEFAULT: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/icons/icon_network_off.png'
    }
    }
  };


  SessionSrcPanel.FORM_FIELD_VIEW["DeleteConnection"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsxstate',
    LBL: null
  },
    VALUES: { DEFAULT: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/buttons/btnDeleteUp.png'
    }
    }
  };

  SessionSrcPanel.FORM_FIELD_VIEW["ErrorConnection"] = { ATTRIBUTES: { VAL: null,
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
  SessionSrcPanel_prototype.getFormFieldViewArr = function () {
    return SessionSrcPanel.FORM_FIELD_VIEW;
  };

  /**
  * Returns the jsxdestinationid of this DestinationBlk.
  * @return {String}
  */
  SessionSrcPanel_prototype.getDestinationId = function (destinationNode) {
    return destinationNode ? destinationNode.getAttribute('jsxdestinationid') || this.jsxdestinationid : this.jsxdestinationid;
  };

  /**
  * Returns the Text of the label of this SessionBlk.
  * @return {String}
  */
  SessionSrcPanel_prototype.getDestinationText = function (destinationNode) {
    var destName = this.jsxdestinationname;
    if (destinationNode) {
      destName = destinationNode.getAttribute('jsxdestinationname');
      var windowId = destinationNode.getAttribute('jsxwindowid');
      if (!jsx3.util.strEmpty(windowId)) {
        destName = destName + " [" + windowId + "]";
        destName = this.getEllipsisText(destName, 33);
      }
    }


    return destName;
  };

  /**
  * Sets the Text of the label of this DestinationBlk.
  * @param strLabel {String}
  * @return {com.ipvs.gui.SessionSrcPanel} this object
  */
  SessionSrcPanel_prototype.setDestinationText = function (strLabel, destinationNode) {
    if (destinationNode) {
      destinationNode.setAttribute('jsxdestinationname', strLabel);
      return destinationNode;
    }
    else {
      this.jsxdestinationname = strLabel;
      return this;
    }
  };


  /**
  * Returns the CDF Destination Node with jsxdestinationid equals strRecordId.
  * @param strRecordId {String}
  * @return {String}
  */
  SessionSrcPanel_prototype.getDestinationNode = function (strRecordId) {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectSingleNode(".//record/destination[@jsxconnectionid ='" + strRecordId + "']") : null;
  };

  /**
  * Returns the Destination Object with jsxid equals strRecordId.
  * @param strRecordId {String}
  * @return SessionObj
  */
  SessionSrcPanel_prototype.getConnection = function (strRecordId) {
    var destNode = this.getDestinationNode(strRecordId);
    if (!destNode) return;
    var destObj = new Object();
    destObj.jsxconnectionid = destNode.getAttribute("jsxconnectionid");
    destObj.jsxdestinationname = destNode.getAttribute("jsxdestinationname");
    return destObj;
  };


  /**
  * Get an iterator for the destinations that will be painted
  * Returns all the destinations in the given record.
  *
  * @return {jsx3.util.Iterator}
  */
  SessionSrcPanel_prototype.getIterableDestinations = function (record) {
    return record ? record.selectNodeIterator(".//destination") :
    (new jsx3.util.List()).iterator();
  };


  /**
  * Returns the Img of the delete connection/session button of this panel based on the current state.
  * @return {String}
  */
  SessionSrcPanel_prototype.getCurrentDeleteImg = function (bOver, record) {
    if (bOver) {
      strPath = SessionSrcPanel.BUTTON_DELETEOVER;
    }
    else {
      strPath = SessionSrcPanel.BUTTON_DELETEUP;
    }
    return strPath;
  };



  /**
  * Returns the SpyGlassMessage string for Errored Connection in this block panel.
  * @return {String}
  */
  SessionSrcPanel_prototype.getErrorMessage = function (record) {
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


  //--------------------OnClick------------------
  SessionSrcPanel_prototype.onClick_deleteConnection = function (objEvent, objGUI) {
    objEvent.cancelBubble();
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getDestinationNode ? this.getDestinationNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? SessionSrcPanel.DeleteConnection : jsx3.gui.Interactive.EXECUTE;
    this.doEvent(event, { objEVENT: objEvent });
  };

  //---------------------- On MouseOver and MouseOut--------------------------


  SessionSrcPanel_prototype.onMouseOver_deleteSessionConnection = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteImg(true, record);
    this.setTip("Delete");
  };

  SessionSrcPanel_prototype.onMouseOut_deleteSessionConnection = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteImg(false, record);
    this.setTip("");
  };


  SessionSrcPanel_prototype.onMouseOver_errorConnection = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getDestinationNode ? this.getDestinationNode(strCdfId) : null;
    var message = this.getErrorMessage(record);
    if (!message) return;
    this.setTip(message);
  };

  SessionSrcPanel_prototype.getSrcText = function (record) {
    var returnText = this.getEllipsisText(this.getText(record), 33);
    //this._LOG.info(" srcName:" + text + " returnText:" + returnText);
    return returnText;
  }
  /**
  * The XML Template used by the Template class to render this Src Block Panel
  *
  * @return {String}
  */
  SessionSrcPanel_prototype.getTemplateXML = function () {
    // form template string
    return ['',
    '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
    '  <model>',
    '  </model>',
    '  <template dom="static">',
    ' <inlinebox u:id="srcpanel" style="position:absolute;left:0px;top:0px;overflow:auto;cursor:default;display:block;overflow-x:hidden;" onscroll="{onScroll}">',
    ' <div style="height:100%;">',
    ' <for-each select="this.getIterableRecords()">',
    '   <var id="srcid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '   <var id="buttonid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
    '   <var id="buttonsrcpresence">this.getCurrentFieldImgURL("Presence", $$target)</var>',
    '   <var id="cssclass">this.getCurrentClass(false,$$target)</var>',
    '   <var id="displayval">this.getDisplayVal($$target)</var>',
    '   <var id="srctext">this.getSrcText($$target)</var>',
    '   <var id="src_target">$$target</var>',
    '   <div u:id="srcblk" id="{srcid}" class="{cssclass}" style="margin:0;padding:0px 0px 6px 0px;position:relative;width:298px;display:{displayval};font-family:Arial, Helvetica, sans-serif;overflow-x:hidden;" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onclick="{onClick}">',
    //target Blocks
    '     <div u:id="targetblk" id="{buttonid}" style="position:relative;width:280px;height:26px;overflow:hidden;text-align:right;display:{displayval};" >',
    '        <span u:id="imgMediaPlayerSrcPresence" style="position:absolute;width:18px;height:24px;left:0px;top:5px;overflow:hidden;">',
    '            <img unselectable="on" class="jsx30image" src="{buttonsrcpresence}" width="16" height="16"/>',
    '        </span>',
    '        <span u:id="titleSrc" style="position:absolute; left:26px; top:5px; font-size:12px; font-weight:bold;">',
    '        <text>{srctext}</text>',
    '        </span>',
    '     </div>',
    //Session Container
    '     <div class="mediaPlayerSrcExpand" style="position:relative;width:280px;display:block;">',
    '        <for-each select="this.getIterableDestinations(src_target)">',
                 this.getDestinationTemplateXML(),
    '        </for-each>',
    '      </div>',
    '    </div>',
    '   </for-each>',
    '   </div>',
    '  </inlinebox>',
    '  </template>',
    '</transform>'].join("");
  };

  /**
  * The nested XML Template for each destination Block in this Src Block Panel
  * Override this for different Internal blocks
  * @return {String}
  */
  SessionSrcPanel_prototype.getDestinationTemplateXML = function () {
    return ['',
    '<var id="buttonCloseConnectionid">this.getId() + \'_\' + $$target.getAttribute(\'jsxconnectionid\')</var>',
    '<var id="buttonCloseConnectionImg">this.getCurrentFieldImgURL("DeleteConnection", $$target)</var>',
    '<var id="buttonConnectionState">this.getCurrentFieldImgURL("State", $$target)</var>',
    '<var id="destinationid">this.getId() + \'_\'+ this.getDestinationId($$target)</var>',
    '<var id="destinationtext">this.getDestinationText($$target)</var>',
    '<var id="buttonconnectionerror">this.getCurrentFieldImgURL("ErrorConnection", $$target)</var>',
    '<var id="displayval">this.getDisplayVal($$target)</var>',
    '<var id="destination_target">$$target</var>',
    //Session block
    '   <div u:id="destinationblk" id="{destinationid}" style="position:relative;width:278px;height:20px;overflow:hidden;text-align:right;display:{displayval};">',
    '       <span style="position:absolute;width:12px;height:12px;left:13px;top:3px;cursor:pointer;">',
    '            <div u:id="connectionError" style="position:relative;height:9px;" >',
    '             <img src="{buttonconnectionerror}" id="{buttonCloseConnectionid}" width="12" height="12" onmouseover="{onMouseOver_errorConnection}"/>',
    '            </div>',
    '       </span>',
    '       <span style="position:absolute;width:20px;height:20px;left:23px;top:3px;cursor:pointer;">',
    '            <div u:id="connectionState" style="position:relative;height:9px;" >',
    '             <img src="{buttonConnectionState}" id="{buttonCloseConnectionid}" width="16" height="16"/>',
    '            </div>',
    '       </span>',
    '       <span style="position:absolute;text-align:left;width:180px;left:50px;top:3px;font-size:11px;overflow:hidden;font-weight:normal;white-space:nowrap;">',
    '       <text>{destinationtext}</text>',
    '       </span>',
    //Close session
    '       <span style="position:absolute;width:16px;height:16px;left:255px;top:3px;cursor:pointer;">',
    '           <div u:id="sessionclose" style="position:relative;height:16px;" >',
    '           <img src="{buttonCloseConnectionImg}" id="{buttonCloseConnectionid}" width="16" height="16" onclick="{onClick_deleteConnection}" onmouseover="{onMouseOver_deleteSessionConnection}" onmouseout="{onMouseOut_deleteSessionConnection}"/>',
    '         </div>',
    '       </span>',
    '   </div>'].join("");
  }



});