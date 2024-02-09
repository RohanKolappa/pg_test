
jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.BlockButton");

/**
 * An object-oriented interface onto a Block containg a left-right,top-down sequential list of buttons
 * Each button represents a record in the attached CDF document and is decorated based on the following attributes in that record
 */

jsx3.Class.defineClass('com.ipvs.gui.BlockButtonPanel', jsx3.gui.Template.Block, [jsx3.gui.Form,jsx3.xml.Cacheable,jsx3.xml.CDF,com.ipvs.gui.BlockButton], function(BlockButtonPanel, BlockButtonPanel_prototype) {

  // create Logger for this class
  BlockButtonPanel_prototype._LOG = jsx3.util.Logger.getLogger(BlockButtonPanel.jsxclass.getName());

  /**
   * Button Form Fields
   * Classes that derive from this class will define their own version of this and return a handle to it in the getFormFieldViewArr method.
   */
  BlockButtonPanel.FORM_FIELD_VIEW = new Object;
  
  // Selection Model for button panel
  BlockButtonPanel.SELECTION_UNSELECTABLE = 0;
  BlockButtonPanel.SELECTION_BUTTON = 1;
  BlockButtonPanel.SELECTION_MULTI_BUTTON = 2;
    
  /**
   * The instance initializer.
   *
   * @param strName {String}  the JSX name
   * @param vntLeft {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntTop {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntWidth {int}
   * @param vntHeight {int}
   */
  BlockButtonPanel_prototype.init = function(strName, vntLeft, vntTop, vntWidth, vntHeight) {
    this._LOG.debug("BlockButtonPanel.init::(strName)" + strName + "(vntLeft)" + vntLeft + "(vntTop)" + vntTop + "(vntWidth)" + vntWidth + "(vntHeight)" + vntHeight);
    this.jsxsuper(strName, vntLeft, vntTop, vntWidth, vntHeight);
  };
  
  /**
   * Method to return a handle to the Form Field View Array
   * Must be overridden by any class that derives from this class
   */
  BlockButtonPanel_prototype.getFormFieldViewArr = function() {
    return BlockButtonPanel.FORM_FIELD_VIEW;
  };
      
  /**
   * Returns the width property of a Button on this Panel
   * @return {int|String} width.
   */
  BlockButtonPanel_prototype.getButtonWidth = function() {
    return this.jsxbuttonwidth;
  };

  /**
   * Sets the width property of a Button on this Panel
   * @param vntWidth {int | String} the width as non-negative integer or non-negative percentage. For example: 45%, 12.
   * @param bUpdateView {boolean} if <code>true</code>, the view of this object is immediately updated, obviating the need to call <code>repaint()</code>.
   * @return {com.ipvs.gui.BlockButtonPanel} this object.
   */
  BlockButtonPanel_prototype.setButtonWidth = function(vntWidth, bUpdateView) {
    //update the model
    this.jsxbuttonwidth = vntWidth;

//    //update the boxprofile
//    if (bUpdateView) {
//      this.syncBoxProfile({width:vntWidth}, true);
//    } else {
//      this.setBoxDirty();
//    }

    return this;
  };
  
  /**
   * Returns the height property of a Button on this Panel
   * @return {int | String} height.
   */
  BlockButtonPanel_prototype.getButtonHeight = function() {
    return this.jsxbuttonheight;
  };

  /**
   * Sets the height property of a Button on this Panel
   * @param vntHeight {int | String} the height as a non-negative integer or non-negative percentage. For example: 45%, 12.
   * @param bUpdateView {boolean} if <code>true</code>, the view of this object is immediately updated, obviating the need to call <code>repaint()</code>.
   * @return {com.ipvs.gui.ButtonPane} this object.
   */
  BlockButtonPanel_prototype.setButtonHeight = function(vntHeight,bUpdateView) {
    //update the model
    this.jsxbuttonheight = vntHeight;

//    //update the boxprofile
//    if (bUpdateView) {
//      this.syncBoxProfile({height:vntHeight}, true);
//    } else {
//      this.setBoxDirty();
//    }

    return this;
  };
  

  /**
   * Returns the width of the label of this button.
   * @return {String}
   */
  BlockButtonPanel_prototype.getLabelWidth = function() { 
    return this.jsxtextwidth; 
  };
  
  /**
   * Sets the Text width of the label of this button.
   * @param vntWidth {int | String} the width as non-negative integer or non-negative percentage. For example: 45%, 12.
   * @return {com.ipvs.gui.BlockButton} this object
   */
  BlockButtonPanel_prototype.setLabelWidth = function(vntWidth, bUpdateView) { 
    this.jsxtextwidth = vntWidth; 
    return this;  
  };  
  
  /**
   * Returns the font size of the label of this button.
   * @return {String}
   */
  BlockButtonPanel_prototype.getLabelFontSize = function() { 
    return this.jsxtextfontsize; 
  };
  
  /**
   * Sets the font size of the label of this button.
   * @param vntSize {int | String} the size as non-negative integer. For example:12.
   * @return {com.ipvs.gui.BlockButton} this object
   */
  BlockButtonPanel_prototype.setLabelFontSize = function(vntSize, bUpdateView) { 
    this.jsxtextfontsize = vntSize; 
    return this;  
  };  
  
  /**
   * Returns the top position of the label of this button.
   * @return {String}
   */
  BlockButtonPanel_prototype.getLabelTopPosition = function() { 
    return this.jsxtexttop; 
  };
  
  /**
   * Sets the top position of the label of this button.
   * @param vntPosition {int | String} the position as non-negative integer or non-negative percentage. For example: 45%, 12.
   * @return {com.ipvs.gui.BlockButton} this object
   */
  BlockButtonPanel_prototype.setLabelTopPosition = function(vntPosition, bUpdateView) { 
    this.jsxtexttop = vntPosition; 
    return this;  
  };  

  /**
   * Returns the left position of the label of this button.
   * @return {String}
   */
  BlockButtonPanel_prototype.getLabelLeftPosition = function() { 
    return this.jsxtextleft; 
  };
  
  /**
   * Sets the left position of the label of this button.
   * @param vntPosition {int | String} the position as non-negative integer or non-negative percentage. For example: 45%, 12.
   * @return {com.ipvs.gui.BlockButton} this object
   */
  BlockButtonPanel_prototype.setLabelLeftPosition = function(vntPosition, bUpdateView) { 
    this.jsxtextleft = vntPosition; 
    return this;  
  };  

  /**
  * Returns if record can be selected in this port button panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  BlockButtonPanel_prototype.isRecordSelectable = function(strRecordId) {
    var record = this.getRecord(strRecordId);
    return record && (record['jsxunselectable'] == null || record['jsxunselectable'] != "1");
  };
  /**
  * Returns if record is selected in this port button panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  BlockButtonPanel_prototype.isRecordSelected = function(strRecordId) {
    var record = this.getRecord(strRecordId);
    return record && record['jsxselected'] == "1";
  }; 
 
  /**
  * Returns the Selection Model for this port button panel
  * @return {int}
  */
  BlockButtonPanel_prototype.getSelectionModel = function() {
    return (this.jsxselectionmodel != null) ? ((this.jsxselectionmodel > BlockButtonPanel.SELECTION_MULTI_BUTTON) ? BlockButtonPanel.SELECTION_UNSELECTABLE : this.jsxselectionmodel) :BlockButtonPanel.SELECTION_BUTTON ;
  };
  
  /**
  * Sets the Selection Model for this port button panel 
  * @param intType {int}
  * @return {com.ipvs.gui.PortButtonPanel} this object
  */
  BlockButtonPanel_prototype.setSelectionModel = function(intType) {
    this.jsxselectionmodel = intType;
    return this;
  }; 
   
  /**
   * Get an iterator for the records that will be painted
   * Returns all the records in the attached CDF doc.  Override this to return something other.
   *
   * @return {jsx3.util.Iterator}
   */
  BlockButtonPanel_prototype.getIterableRecords = function() {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectNodeIterator("//record") :
    (new jsx3.util.List()).iterator();
  };

   /**
   * The XML Template used by the Template class to render this Button Panel
   *
   * @return {String}
   */
  BlockButtonPanel_prototype.getTemplateXML = function() {
    var returnVal = ['',
      '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
      '  <model>',
      '  </model>',
      '  <template dom="static">',
      '    <inlinebox u:id="buttonpanel" style="position:{$position};left:{$left};top:{$top};width:{$width};height:{$height};margin:{$margin};">',
    // button panel
      '      <div style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:auto;">',
      '        <for-each select="this.getIterableRecords()">',
    // button
                  this.getButtonTemplateXML(),
      '        </for-each>',
      '      </div>',
      '    </inlinebox>',
      '  </template>',
      '</transform>'].join("");
    this._LOG.debug("BlockButtonPanel.getTemplateXML::(returnVal):" + returnVal.toString());
    // form template string
    return returnVal;
  };
  
   /**
   * The nested XML Template for each Button in this Button Panel
   * Override this for different buttons
   * @return {String}
   */
  BlockButtonPanel_prototype.getButtonTemplateXML = function() {
    var returnVal = ['',
      '<var id="buttonid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
      '<var id="buttonimgclass">this.getCurrentClass(false, false, $$target)</var>',
      '<inlinebox u:id="button" id="{buttonid}" style="position:relative;float:left;margin:2px 2px 2px 2px;cursor:pointer;" onclick="{onClick}" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onmouseup="{onMouseUp}" class="{buttonimgclass}">',
      '  <div style="position:relative;left:0px;top:0px;width:', this.getButtonWidth(), 'px;height:', this.getButtonHeight(), 'px;overflow:hidden;">',
    // button fields
           this.getButtonFieldsTemplateXML(),
      '  </div>',
      '</inlinebox>'].join("");
    this._LOG.debug("BlockButtonPanel.getButtonTemplateXML::(returnVal):" + returnVal.toString());
    return returnVal;
  }
   
   /**
   * The nested XML Template for the Form Fields on each Button in this Button Panel
   * Override this for fields of different buttons
   * @return {String}
   */
  BlockButtonPanel_prototype.getButtonFieldsTemplateXML = function() {
    var returnVal = ['',
      '<var id="buttontext">this.getCurrentLabelText($$target)</var>',
      '<div u:id="buttonfields" style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:hidden;">',
      '  <span u:id="title" style="position:absolute;width:', this.getLabelWidth(), 'px;left:', this.getLabelLeftPosition(), 'px;top:', this.getLabelTopPosition(), 'px;font-size:', this.getLabelFontSize(), 'px;overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis;">',
      '    <text>{buttontext}</text>',
      '  </span>',
      '</div>'].join("");
    this._LOG.debug("BlockButtonPanel.getButtonFieldsTemplateXML::(returnVal):" + returnVal.toString());
    return returnVal;
  }

});
