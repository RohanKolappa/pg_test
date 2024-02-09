
jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","com.ipvs.api.SessionConfig");

/**
 * An object-oriented interface onto a Block containg a left-right,top-down sequential list of dst windows
 * Each dst window represents a dst entity in the attached SessionConfig document and is decorated based on the window attributes in that record
 */

jsx3.Class.defineClass('com.ipvs.gui.WindowPanel', jsx3.gui.Template.Block, [jsx3.gui.Form,jsx3.xml.Cacheable,com.ipvs.api.SessionConfig], function(WindowPanel, WindowPanel_prototype) {

  var Event = jsx3.gui.Event;
  var Interactive = jsx3.gui.Interactive;
  var Form = jsx3.gui.Form;
    
  /**
   * {String} Default Values for the windows
   * @final @jsxobf-final
   */
  WindowPanel.BUTTON_IMG_DEFAULT = 'jsx:///JSXAPPS/IPVS_ClientUI/images/controller/sourceOffBG.png';
  
  WindowPanel.BUTTON_PRESENCE_DEFAULT = 'jsx:///JSXAPPS/IPVS_ClientUI/images/icons/icon_lightgrayBall.png';

  WindowPanel.BUTTON_TITLE_DEFAULT = 'Title';

  WindowPanel.BUTTON_STATE_DEFAULT = 'jsx:///JSXAPPS/IPVS_ClientUI/images/icons/icon_network_off.png';
  
  WindowPanel.BUTTON_TYPE_DEFAULT = 'jsx:///JSXAPPS/IPVS_ClientUI/images/icons/icon_v2d.png';
 
  /**
   * The instance initializer.
   *
   * @param strName {String}  the JSX name
   * @param vntLeft {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntTop {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntWidth {int}
   * @param vntHeight {int}
   */
  WindowPanel_prototype.init = function(strName, vntLeft, vntTop, vntWidth, vntHeight) {
    this.jsxsuper(strName, vntLeft, vntTop, vntWidth, vntHeight);
  };

// template xml
  WindowPanel_prototype.getTemplateXML = function() {
    return ['',
    '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
    '  <model>',
    '  </model>',
    '  <template dom="static">',
    '    <inlinebox style="position:{$position};left:{$left};top:{$top};width:{$width};height:{$height};margin:{$margin};">',
           // window panel
    '      <div u:id="windowpanel" style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:auto;">',
    '          <for-each select="this.getIterableRecords()">',
                  // window
    '            <var id="windowid">this.getId() + \'_\' + $$target.getAttribute(\'id\')</var>',
    '            <var id="windowtarget">$$target</var>',
    '            <var id="buttonimg">this.getCurrentImageURL($$target.getAttribute(\'jsxselected\'))</var>',
    '            <var id="buttondevpresence">$$target.getAttribute(\'jsxdevpresence\') || WindowPanel.BUTTON_PRESENCE_DEFAULT</var>',
    '            <var id="buttondevtitle">$$target.getAttribute(\'jsxtext\') || WindowPanel.BUTTON_TITLE_DEFAULT</var>',
    '            <var id="buttondevstate">$$target.getAttribute(\'jsxdevstatus\') || WindowPanel.BUTTON_STATE_DEFAULT</var>',
    '            <var id="buttondevtype">$$target.getAttribute(\'jsxdevtype\') || WindowPanel.BUTTON_TYPE_DEFAULT</var>',
    '            <span u:id="window" id="{windowid}" style="position:relative;width:130px;height:40px;cursor:pointer" onclick="{onClick}" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onmouseup="{onMouseUp}">',
    '                <div style="position:relative;height:40px">',
                        // button image
    '                  <img src="{buttonimg}" width="130" height="40"/>',
    '                  <span style="position:absolute;width:130px;height:40px;left:0px;top:0px;">',
                          // presence
    '                    <span u:id="presence" style="position:absolute;width:16px;height:16px;left:2px;top:3px;overflow:hidden;">',
    '                      <img src="{buttondevpresence}" width="16" height="16"/>',
    '                    </span>',
                          // title
    '                    <span u:id="title" style="position:absolute;width:106px;left:21px;top:3px;font-size:12px;overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis;">',
    '                      <text>{buttondevtitle}</text>',
    '                    </span>',
                          // state
    '                    <span u:id="state" style="position:absolute;width:16px;height:16px;left:3px;top:22px;overflow:hidden;">',
    '                      <img src="{buttondevstate}" width="16" height="16"/>',
    '                    </span>',
                          // type
    '                    <span u:id="type" style="position:absolute;width:35px;height:16px;left:22px;top:22px;overflow:hidden;">',
    '                      <img src="{buttondevtype}" width="35" height="16"/>',
    '                    </span>',
    '                  </span>',
    '                </div>',
    '              </span>',    
    '          </for-each>',
    '      </div>',
    '    </inlinebox>',
    '  </template>',
    '</transform>'].join("");
  };
  
  WindowPanel_prototype.getIterableRecords = function() {
    var objSessionConfig = this.getXML();
    return objSessionConfig ? objSessionConfig.selectNodeIterator("//Entity") :
    (new jsx3.util.List()).iterator();
  };
  
  /**
   * Returns the URI of the image of this panel window.
   * @return {String}
   */
  WindowPanel_prototype.getImage = function() { return this.jsximage; };

  /**
   * Sets the URI of the image of this panel window. This is the default image that is displayed if the window
   * is off or if the window is in a state for which no image URI is specified.
   * @param strImage {String}
   * @return {com.ipvs.gui.WindowPanel} this object
   */
  WindowPanel_prototype.setImage = function(strImage) { this.jsximage = strImage; return this; };

  /**
   * Returns the URI of the over image of this panel window.
   * @return {String}
   */
  WindowPanel_prototype.getOverImage = function() { return this.jsxoverimage; };

  /**
   * Sets the URI of the over image of this panel window. This is the image that is displayed when the mouse moves
   * over the panel window.
   * @param strImage {String}
   * @return {com.ipvs.gui.WindowPanel} this object
   */
  WindowPanel_prototype.setOverImage = function(strImage) { this.jsxoverimage = strImage; return this; };

  /**
   * Returns the URI of the down image of this panel window.
   * @return {String}
   */
  WindowPanel_prototype.getDownImage = function() { return this.jsxdownimage; };

  /**
   * Sets the URI of the down image of this panel window. This is the image that is displayed when the mouse is down
   * over the panel window.
   * @param strImage {String}
   * @return {com.ipvs.gui.WindowPanel} this object
   */
  WindowPanel_prototype.setDownImage = function(strImage) { this.jsxdownimage = strImage; return this; };

  /**
   * Returns the URI of the on image of this panel window.
   * @return {String}
   */
  WindowPanel_prototype.getOnImage = function() { return this.jsxonimage; };

  /**
   * Sets the URI of the on image of this panel window. This is the image that is displayed when this panel window is
   * selected.
   * @param strImage {String}
   * @return {com.ipvs.gui.WindowPanel} this object
   */
  WindowPanel_prototype.setOnImage = function(strImage) { this.jsxonimage = strImage; return this; };

  /**
   * Returns the URI of the disabled image of this panel window.
   * @return {String}
   */
  WindowPanel_prototype.getDisabledImage = function() { return this.jsxdisabledimage; };

  /**
   * Sets the URI of the disabled image of this panel window. This is the image that is displayed when this panel
   * window is disabled.
   * @param strImage {String}
   * @return {com.ipvs.gui.WindowPanel} this object
   */
  WindowPanel_prototype.setDisabledImage = function(strImage) { this.jsxdisabledimage = strImage; return this; };

  /**
   * Returns the URI of the image of this panel window based on the current state.
   * @return {String}
   */
  WindowPanel_prototype.getCurrentImageURL = function(bOn, bOver, bDown) {
    var strPath = null;
    if (this.getEnabled() == Form.STATEENABLED) {
      if (bOver)
        strPath = this.getOverImage();
      else if (bDown)
        strPath = this.getDownImage();
      if (bOn)
        strPath = strPath || this.getOnImage();
    } else {
      strPath = this.getDisabledImage();
    }

    strPath = strPath || this.getImage();
    return strPath ? this.getUriResolver().resolveURI(strPath) : WindowPanel.BUTTON_IMG_DEFAULT;
  };

  // events
 WindowPanel_prototype.onMouseOver = function(objEvent, objGUI) {
    objGUI.childNodes[0].childNodes[0].src = this.getCurrentImageURL(false, true, false);
  };

  WindowPanel_prototype.onMouseOut = function(objEvent, objGUI) {
    objGUI.childNodes[0].childNodes[0].src = this.getCurrentImageURL(false, false, false);
  };

  WindowPanel_prototype.onMouseDown = function(objEvent, objGUI) {
    if (objEvent.leftButton())
      objGUI.childNodes[0].childNodes[0].src = this.getCurrentImageURL(false, false, true);
  };

  WindowPanel_prototype.onMouseUp = function(objEvent, objGUI) {
    if (objEvent.rightButton())
      this.jsxsupermix(objEvent, objGUI);
    else if (objEvent.leftButton())
      objGUI.childNodes[0].childNodes[0].src = this.getCurrentImageURL(false, false, false);
  };
  
  WindowPanel_prototype.onClick = function(objEvent, objGUI) {
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get selected cdf record id
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length+1);
    // set that as the value
    this.setValue(strCdfId);
//    // set 'selected' in cdf
//    this.getRecordNode(strCdfId).setAttribute('jsxselected', '1');
    // do SELECT event
    this.doEvent(Interactive.SELECT, {objEVENT:objEvent});
  };

});
