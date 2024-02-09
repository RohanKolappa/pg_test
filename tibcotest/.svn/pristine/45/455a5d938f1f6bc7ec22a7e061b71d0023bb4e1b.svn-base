jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.BlockPanel");

jsx3.Class.defineClass('com.ipvs.gui.CanvasEditorWindowPanel', com.ipvs.gui.BlockPanel, [jsx3.gui.Form, jsx3.xml.Cacheable, jsx3.xml.CDF], function (CanvasEditorWindowPanel, CanvasEditorWindowPanel_prototype) {

  // create Logger for this class
  CanvasEditorWindowPanel_prototype._LOG = jsx3.util.Logger.getLogger("com.ipvs.gui.CanvasEditorWindowPanel");

  //Event Variables 
  CanvasEditorWindowPanel.WindowFullScreen = "jsxwinfullscreen";
  CanvasEditorWindowPanel.WindowOneToOne = "jsxwinonetoone";
  CanvasEditorWindowPanel.WindowMute = "jsxwinmute";
  CanvasEditorWindowPanel.WindowVisible = "jsxwinvisible";
  CanvasEditorWindowPanel.WindowKbm = "jsxwinkbm";
  CanvasEditorWindowPanel.DeleteBlock = "jsxdeleteblock";

  /**
  * {String} EndPoint Fields
  */
  CanvasEditorWindowPanel.BUTTON_DELETEUP = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnDeleteUp.png');
  CanvasEditorWindowPanel.BUTTON_DELETEOVER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnDeleteOver.png');
  CanvasEditorWindowPanel.FORM_FIELD_VIEW = new Object;
  CanvasEditorWindowPanel.FORM_FIELD_VIEW["DeleteBlock"] = { ATTRIBUTES: { VAL: null,
    IMG: 'jsximgWinDelete',
    LBL: null
  },
    VALUES: { DEFAULT: { LBL: null,
      IMG: 'jsxaddin://user!ipvsCommon/images/buttons/btnDeleteUp.png'
    }
    }
  };

  /**
  * Method to return a handle to the Form Field View Array
  */
  CanvasEditorWindowPanel_prototype.getFormFieldViewArr = function () {
    return CanvasEditorWindowPanel.FORM_FIELD_VIEW;
  };

  /**
  * Returns the  image of this panel button based on the current state.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getCurrentWindowFullScreenImage = function (bOver, bDown, record) {
    var maximize = record.getAttribute("maximize");
    var strImg = null;
    if (maximize == "false") {
      if (bOver)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcFullscreenOver.png');
      else if (bDown)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcFullscreenDown.png');
    } else {
      strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcFullscreenOn.png');
    }

    strImg = strImg || jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcFullscreenOff.png');

    return strImg;
  };

  /**
  * Returns the  image of this panel button based on the current state.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getCurrentWindowOneToOneImage = function (bOver, bDown, record) {
    var scale = record.getAttribute("scale");
    var strImg = null;
    if (scale == "fitWindow") {
      if (bOver)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpc1to1Over.png');
      else if (bDown)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpc1to1Down.png');
    } else {
      strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpc1to1On.png');
    }

    strImg = strImg || jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpc1to1Off.png');

    return strImg;
  };

  /**
  * Returns the  image of this panel button based on the current state.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getCurrentWindowMuteImage = function (bOver, bDown, record) {
    var mute = record.getAttribute("mute");
    var strImg = null;
    if (mute == "false") {
      if (bOver)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcSoundOver.png');
      else if (bDown)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcSoundDown.png');
    } else {
      strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcSoundOn.png');
    }

    strImg = strImg || jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcSoundOff.png');

    return strImg;
  };

  /**
  * Returns the  image of this panel button based on the current state.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getCurrentWindowVisibleImage = function (bOver, bDown, record) {
    var visibility = record.getAttribute("visibility");
    var strImg = null;
    if (visibility == "false") {
      if (bOver)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcVisibleOver.png');
      else if (bDown)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcVisibleDown.png.png');
    } else {
      strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcVisibleOff.png');
    }

    strImg = strImg || jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcVisibleOn.png');

    return strImg;
  };

  /**
  * Returns the  image of this panel button based on the current state.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getCurrentWindowKbmImage = function (bOver, bDown, record) {
    //this._LOG.error("getCurrentWindowKbmImageRecord:" + record + "\n");
    var remoteKbmCtrl = record.getAttribute("remoteKbmCtrl");
    var srcKbmCtrl = record.getAttribute("srcKbmCtrl");
    var ediCanvasType = record.getAttribute("jsxCanvasEditType");
 
    var strImg = null;

    if (jsx3.util.strEmpty(srcKbmCtrl) || srcKbmCtrl == "false") {
      bOver = false;
      bDown = false;
    }

    if (remoteKbmCtrl == "false") {
      if (bOver)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcKbmOver.png');
      else if (bDown)
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcKbmDown.png');
    } else {
      if (ediCanvasType != "AdminEditCanavsConfig" && (jsx3.util.strEmpty(srcKbmCtrl) || srcKbmCtrl == "false")) {   
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcKbmOnX.png');
      }
      else {
        strImg = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcKbmOn.png');
      }
    }
    
    if (ediCanvasType != "AdminEditCanavsConfig" && (jsx3.util.strEmpty(srcKbmCtrl) || srcKbmCtrl == "false")) {
      strImg = strImg || jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcKbmOffX.png');
    }
    else {
      strImg = strImg || jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnEpcKbmOff.png');
    }
    

    return strImg;
  };

  /**
  * Returns the Img of the delete connection/session button of this panel based on the current state.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getCurrentDeleteImg = function (bOver, record) {
    if (bOver) {
      strPath = CanvasEditorWindowPanel.BUTTON_DELETEOVER;
    }
    else {
      strPath = CanvasEditorWindowPanel.BUTTON_DELETEUP;
    }
    return strPath;
  };

  CanvasEditorWindowPanel_prototype.onClick_WinFullScreen = function (objEvent, objGUI) {
    //    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? CanvasEditorWindowPanel.WindowFullScreen : jsx3.gui.Interactive.EXECUTE;
    this._LOG.info("CanvasEditorWindowPanel_prototype.onClick_WinFullScreen record:" + record + "\n");
    this.doEvent(event, { objEVENT: objEvent });
  };

  CanvasEditorWindowPanel_prototype.onClick_WindowOneToOne = function (objEvent, objGUI) {
    //    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? CanvasEditorWindowPanel.WindowOneToOne : jsx3.gui.Interactive.EXECUTE;
    this._LOG.info("CanvasEditorWindowPanel_prototype.onClick_WindowOneToOne record:" + record + "\n");
    this.doEvent(event, { objEVENT: objEvent });
  };

  CanvasEditorWindowPanel_prototype.onClick_WinMute = function (objEvent, objGUI) {
    //    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? CanvasEditorWindowPanel.WindowMute : jsx3.gui.Interactive.EXECUTE;
    this._LOG.info("CanvasEditorWindowPanel_prototype.onClick_WinMute record:" + record + "\n");
    this.doEvent(event, { objEVENT: objEvent });
  };

  CanvasEditorWindowPanel_prototype.onClick_WinVisible = function (objEvent, objGUI) {
    //    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? CanvasEditorWindowPanel.WindowVisible : jsx3.gui.Interactive.EXECUTE;
    this._LOG.info("CanvasEditorWindowPanel_prototype.onClick_WinVisible record:" + record + "\n");
    this.doEvent(event, { objEVENT: objEvent });
  };

  CanvasEditorWindowPanel_prototype.onClick_WinKbm = function (objEvent, objGUI) {
    //    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    //get the canvas edit type
    var ediCanvasType = record.getAttribute("jsxCanvasEditType");
    // if AdminEditCanavsConfig then kbm is allowed other wise it will get enabled based on the srckbmCtrl value
    if (ediCanvasType != "AdminEditCanavsConfig") {
      //this._LOG.error("onClick_WinKbm:" + record + "\n");
      var srcKbmCtrl = record.getAttribute("srcKbmCtrl");
      if (jsx3.util.strEmpty(srcKbmCtrl) || srcKbmCtrl == "false") {
        return;
      }    
    }

    record && this.setValue(id);
    // do SELECT/EXECUTE event
    var event = record ? CanvasEditorWindowPanel.WindowKbm : jsx3.gui.Interactive.EXECUTE;
    this._LOG.info("CanvasEditorWindowPanel_prototype.onClick_WinKbm record:" + record + "\n");
    this.doEvent(event, { objEVENT: objEvent });
  };

  CanvasEditorWindowPanel_prototype.onMouseOver_WinFullScreen = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowFullScreenImage(true, false, record);
    this.setTip("Toggle Fullscreen");
  };

  CanvasEditorWindowPanel_prototype.onMouseOut_WinFullScreen = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowFullScreenImage(false, false, record);
    this.setTip("");
  };

  CanvasEditorWindowPanel_prototype.onMouseOver_WindowOneToOne = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowOneToOneImage(true, false, record);
    this.setTip("Native Resolution");
  };

  CanvasEditorWindowPanel_prototype.onMouseOut_WindowOneToOne = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowOneToOneImage(false, false, record);
    this.setTip("");
  };

  CanvasEditorWindowPanel_prototype.onMouseOver_WinMute = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowMuteImage(true, false, record);
    this.setTip("Mute/Unmute");
  };

  CanvasEditorWindowPanel_prototype.onMouseOut_WinMute = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowMuteImage(false, false, record);
    this.setTip("");
  };

  CanvasEditorWindowPanel_prototype.onMouseOver_WinVisible = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowVisibleImage(true, false, record);
    this.setTip("Show/Hide Player");
  };

  CanvasEditorWindowPanel_prototype.onMouseOut_WinVisible = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowVisibleImage(false, false, record);
    this.setTip("");
  };

  CanvasEditorWindowPanel_prototype.onMouseOver_WinKbm = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowKbmImage(true, false, record);
    this.setTip("Enable/Disable Remote KBM");
  };

  CanvasEditorWindowPanel_prototype.onMouseOut_WinKbm = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentWindowKbmImage(false, false, record);
    this.setTip("");
  };

  CanvasEditorWindowPanel_prototype.onMouseOver_WinDelete = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteImg(true, record);
    this.setTip("Delete Window");
  };

  CanvasEditorWindowPanel_prototype.onMouseOut_WinDelete = function (objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteImg(false, record);
    this.setTip("");
  };

  /**
  * Returns the Text string of this panel button.
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getWinText = function (record) {
    var text = this.getText(record);
    var textWidth = 10;
    if (!jsx3.util.strEmpty(textWidth)) {
      if (text.length > textWidth) {
        var i = 1;
        var returnText = '';
        while (returnText.length < (textWidth) && i < text.length) {
          returnText = text.substr(0, i) + '...';
          i++;
        }
        text = returnText;
      }
    }
    return text;
  };

  /**
  * The XML Template used by the Template class to render this CanvasEditorWindowPanel
  *
  * @return {String}
  */
  CanvasEditorWindowPanel_prototype.getTemplateXML = function () {
    // form template string
    var returnVal = ['',
      '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
      '  <model>',
      '  </model>',
      ' <template dom="static">',
      ' <inlinebox u:id="canvaswindowpanel" style="position:absolute;left:0px;top:0px;width:205px;height:200px;overflow:auto;cursor:default;display:block;overflow-x:hidden;">',
      ' <for-each select="this.getIterableRecords()">',
      '   <var id="winid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
      '   <var id="blocktext">this.getWinText($$target)</var>',
      '   <var id="btnwinfullscreen">this.getCurrentWindowFullScreenImage(false,false,$$target)</var>',
      '   <var id="btnwinonetoone">this.getCurrentWindowOneToOneImage(false,false,$$target)</var>',
      '   <var id="btnwinmute">this.getCurrentWindowMuteImage(false,false,$$target)</var>',
      '   <var id="btnwinvisible">this.getCurrentWindowVisibleImage(false,false,$$target)</var>',
      '   <var id="btnwinkbm">this.getCurrentWindowKbmImage(false,false,$$target)</var>',
      '   <var id="btnwindelete">this.getCurrentFieldImgURL("DeleteBlock", $$target)</var>',
      '   <var id="cssclass">this.getCurrentClass(false,$$target)</var>',
        '   <div u:id="windowblock" id="{winid}" class="{cssclass}" style="margin:0;padding:0;padding-right:18px;position:relative;width:205px;height:25px;display:block;text-align:right;font-family:Arial, Helvetica, sans-serif;" onclick="{onClick}" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onmouseup="{onMouseUp}">',
        '     <span u:id="wintext" style="position:absolute; left:3px; top:3px; font-size:12px; width:60px; color:white; text-align:left; overflow:hidden; font-weight:normal; white-space:nowrap;">',
        '        <text>{blocktext}</text>',
        '     </span>',
        '    <span style="position:absolute; left:70px; top:3px; font-size:12px; text-align:left; ">',
        '     <img src="{btnwinfullscreen}" id="{winid}" onclick="{onClick_WinFullScreen}" onmouseover="{onMouseOver_WinFullScreen}" onmouseout="{onMouseOut_WinFullScreen}" width="16" height="16"/>',
        '   </span>',
        '    <span style="position:absolute; left:88px; top:3px; font-size:12px; text-align:left; ">',
        '     <img src="{btnwinonetoone}"  id="{winid}" onclick="{onClick_WindowOneToOne}" onmouseover="{onMouseOver_WindowOneToOne}" onmouseout="{onMouseOut_WindowOneToOne}" width="16" height="16"/>',
        '   </span>',
        '    <span style="position:absolute; left:106px; top:3px; font-size:12px; text-align:left; ">',
        '     <img src="{btnwinmute}" id="{winid}" onclick="{onClick_WinMute}" onmouseover="{onMouseOver_WinMute}" onmouseout="{onMouseOut_WinMute}" width="16" height="16"/>',
        '   </span>',
        '    <span style="position:absolute; left:124px; top:3px; font-size:12px; text-align:left; ">',
        '     <img src="{btnwinvisible}" id="{winid}" onclick="{onClick_WinVisible}"  onmouseover="{onMouseOver_WinVisible}" onmouseout="{onMouseOut_WinVisible}" width="16" height="16"/>',
        '   </span>',
        '    <span style="position:absolute; left:144px; top:3px; font-size:12px; text-align:left; ">',
        '     <img src="{btnwinkbm}" id="{winid}" onclick="{onClick_WinKbm}"  onmouseover="{onMouseOver_WinKbm}" onmouseout="{onMouseOut_WinKbm}" width="16" height="16"/>',
        '   </span>',
        '    <span style="position:absolute; left:162px; top:3px; font-size:12px; text-align:left; ">',
        '     <img src="{btnwindelete}"  id="{winid}" onclick="{onClick_delete}" width="16" height="16" onmouseover="{onMouseOver_WinDelete}" onmouseout="{onMouseOut_WinDelete}"/>',
        '   </span>',
        '   </div>',
      '   </for-each>',
      '   </inlinebox>',
      '  </template>',
      '</transform>'].join("");
    jsx3.log("getTemplateXML:Html String:" + returnVal);
    return returnVal;

  };


});