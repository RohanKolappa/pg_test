/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

jsx3.require("jsx3.gui.Template","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.BlockPanel");
jsx3.Class.defineClass('com.ipvs.gui.SessionDstPanel', com.ipvs.gui.BlockPanel, [jsx3.xml.Cacheable,jsx3.xml.CDF], function(SessionDstPanel, SessionDstPanel_prototype) {


  var Event = jsx3.gui.Event;
  var Interactive = jsx3.gui.Interactive;
 
  
  /**
   * {String} EndPoint Fields
   */
  SessionDstPanel.FORM_FIELD_VIEW = new Object;

  // Port Stream Type
  SessionDstPanel.FORM_FIELD_VIEW["DstType"] = {ATTRIBUTES : {VAL : null,
                                                            IMG : 'jsxdevtype',
                                                            LBL : null}, 
                                                  VALUES : {MULTI  :{LBL:'MULTI',
                                                                     IMG:'jsxaddin://user!ipvsCommon/images/icons/computer.png'},
                                                            DEFAULT:{LBL:null,
                                                                     IMG:'jsxaddin://user!ipvsCommon/images/icons/pc_screen.png'}}};

  SessionDstPanel.FORM_FIELD_VIEW["DeleteDst"] = {ATTRIBUTES : {VAL : null,
                                                              IMG : 'jsxendpoint',
                                                              LBL : null}, 
                                                   VALUES  : {DEFAULT :{LBL:null,
                                                                   IMG:'jsxaddin://user!ipvsCommon/images/buttons/btnRemoveUp.png'}}};

 /**
  * Method to return a handle to the Form Field View Array
  */
  SessionDstPanel_prototype.getFormFieldViewArr = function() {
    return SessionDstPanel.FORM_FIELD_VIEW;
  };

  SessionDstPanel_prototype.getDstText = function (record) {
    var returnText = this.getEllipsisText(this.getText(record), 20);
    //this._LOG.info(" srcName:" + text + " returnText:" + returnText);
    return returnText;
  }

 /**
   * The XML Template used by the Template class to render this SessionDstPanel
   *
   * @return {String}
   */
  SessionDstPanel_prototype.getTemplateXML = function() {
      // form template string
      return ['',
      '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
      '  <model>',
      '  </model>',
      '  <template dom="static">',
      ' <inlinebox u:id="SessionDstPanel" style="overflow:auto;overflow-x:scroll;overflow-y:hidden;width:100%;height:104px;white-space:nowrap;" onscroll="{onScroll}">',
      '   <div style="width:1000000px;">',
      '     <for-each select="this.getIterableRecords()">',
      '       <var id="dstid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
      '       <var id="dsttext">this.getDstText($$target)</var>',
      '       <var id="buttonddstclose">this.getCurrentFieldImgURL("DeleteDst", $$target)</var>',
      '       <var id="imgdsttype">this.getCurrentFieldImgURL("DstType", $$target)</var>',
      '       <var id="displayval">this.getDisplayVal($$target)</var>', 
      '       <var id="cssclass">this.getCurrentClass(false,$$target)</var>',
      '       <span u:id="blockdst" id="{dstid}" class="{cssclass}" style="float:left;margin:8px 8px 8px 8px;padding:0;position:relative;width:', this.getBlockWidth(), 'px;height:', this.getBlockHeight(), 'px;display:{displayval};font-family:Arial, Helvetica, sans-serif;" onclick="{onClick}" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onmouseup="{onMouseUp}">',      
//      '       <span u:id="imgdst" style="position:absolute;height:16px;left:75px;top:5px;overflow:hidden;">',
//      '         <img src="{buttonddstclose}" id="{dstid}" onclick="{onClick_delete}" onmouseover="{onMouseOver_delete}" onmouseout="{onMouseOut_delete}" width="16px" height="16px"/>',
//      '       </span>', 
      '         <span u:id="imgdst" style="position:absolute;width:80px;height:60px;left:15px;top:5px;overflow:hidden;">',
      '           <img unselectable="on" class="jsx30image" src="{imgdsttype}" width="30px" height="30px"/>',
      '         </span>',   
      '         <span u:id="dsttitle" style="position:absolute; left:15px; top:43px; font-size:12px;color:#ffffff;font-weight:bold;">',
      '           <text>{dsttext}</text>',
      '         </span>',
      '       </span>',
      '     </for-each>',
      '   </div>',
      ' </inlinebox>',
      '</template>',
      '</transform>'].join("");
  };




});