jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF");

jsx3.Class.defineClass('com.ipvs.gui.BlockPanel', jsx3.gui.Template.Block, [jsx3.gui.Form,jsx3.xml.Cacheable,jsx3.xml.CDF], function(BlockPanel, BlockPanel_prototype) {

  var Event = jsx3.gui.Event;
  var Interactive = jsx3.gui.Interactive;
  var Form = jsx3.gui.Form;

  BlockPanel.DeleteBlock = "jsxdeleteblock";
  
  BlockPanel.BUTTON_DELETEBLOCKUP = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnRemoveUp.png');
  BlockPanel.BUTTON_DELETEBLOCKOVER = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/btnRemoveOver.png');  

  // Selection Model for Block panel
  BlockPanel.SELECTION_UNSELECTABLE = 0;
  BlockPanel.SELECTION_BLOCK = 1;
  BlockPanel.SELECTION_MULTI_BLOCK = 2;    
  
  //Selection Type for Block panel
  BlockPanel.TYPE_NORMAL = 0;  
  BlockPanel.TYPE_TOGGLE = 1;
  
  /**
  * {int} Defaults for the form FieldTypes.
  */
  BlockPanel.FORM_FIELD_FMT_IMG = 'IMG';
  BlockPanel.FORM_FIELD_FMT_LBL = 'LBL';

  /**
  * Block Form Fields
  * Classes that derive from this class will define their own version of this and return a handle to it in the getFormFieldViewArr method.
  */
  BlockPanel.FORM_FIELD_VIEW = new Object; 

  /**
  * Method to return a handle to the Form Field View Array
  */
  BlockPanel_prototype.getFormFieldViewArr = function() {
    return BlockPanel.FORM_FIELD_VIEW;
  };

  /**
  * Returns the Text of the label of this Block.
  * @return {String}
  */
  BlockPanel_prototype.getText = function(record) { 
    return record ? record.getAttribute('jsxtext') || this.jsxtext : this.jsxtext; 
  };

  /**
  * Returns the Text of the label of this Block.
  * @return {String}
  */
  BlockPanel_prototype.onAfterPaint = function() {
    var scrollTop = this.getVScrollPosition();
    var scrollLeft = this.getHScrollPosition();
    this.jsxsuper();
    if (!jsx3.util.strEmpty(scrollTop))
      this.getRendered().setAttribute("scrollTop", scrollTop);
    if (!jsx3.util.strEmpty(scrollLeft))
      this.getRendered().setAttribute("scrollLeft", scrollLeft);
  };
  
  /**
  * Returns the Text string of this block in the ellipsis format.
  * @return {String}
  */
  BlockPanel_prototype.getEllipsisText = function (text, textWidth) {
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
  * Sets the Text of the label of this Block.
  * @param strLabel {String}
  * @return {com.ipvs.gui.BlockPanel} this object
  */
  BlockPanel_prototype.setText = function(strLabel,record) { 
    if(record) {
      record.setAttribute('jsxtext', strLabel); 
      return record;
    }
    else {
      this.jsxtext = strLabel; 
      return this; 
    } 
  };
  /**
   * Returns whether the block has to be shown or hide.
   * @return block or none
   */
  BlockPanel_prototype.getDisplayVal = function(record) {
    var visibleVal = "block";
    if( record ){
      var visible = record.getAttribute('jsxVisible');
      if( visible == "0"){
        visibleVal = "none";
      }
    }
    return visibleVal;
  }
  
  /**
  * Returns the type of this panel block.
  * @return {int} <code>BlockPanel.TYPE_NORMAL</code> or <code>BlockPanel.TYPE_TOGGLE</code>
  * @see #TYPE_NORMAL
  * @see #TYPE_TOGGLE
  */
  BlockPanel_prototype.getType = function(record) {
    return record ? record.getAttribute('jsxtype') || this.jsxtype : this.jsxtype;    
  };
  
  /**
  * Sets the type of this panel block.
  * @param intType {int} <code>BlockPanel.TYPE_NORMAL</code> or <code>BlockPanel.TYPE_TOGGLE</code>
  * @return {com.ipvs.gui.BlockPanel} this object
  */
  BlockPanel_prototype.setType = function(intType, record) { 
    if( record ) {
      record.setAttribute('jsxtype', intType); 
      return record;
    }
    else {
      this.jsxtype = intType; 
      return this; 
    } 
  };
  
  /**
  * Returns the CSS On Class of this block.
  * @return {String}
  */
  BlockPanel_prototype.getClassOn = function() { 
    return this.jsxclasson; 
  };
  
  /**
  * Sets the the CSS On Class of this block.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.BlockPannel} this object
  */
  BlockPanel_prototype.setClassOn = function(vntClassName, bUpdateView) { 
    this.jsxclasson = vntClassName; 
    return this;  
  };  
  
  /**
  * Returns the CSS Off Class of this block.
  * @return {String}
  */
  BlockPanel_prototype.getClassOff = function() { 
    return this.jsxclassoff; 
  };
  
  /**
  * Sets the the CSS Off Class of this block.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.BlockPannel} this object
  */
  BlockPanel_prototype.setClassOff = function(vntClassName, bUpdateView) { 
    this.jsxclassoff = vntClassName; 
    return this;  
  };  
  
  /**
  * Returns the CSS On Class of this block.
  * @return {String}
  */
  BlockPanel_prototype.getClassOver = function() { 
    return this.jsxclassover; 
  };
  
  /**
  * Sets the the CSS Over Class of this block.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.BlockPannel} this object
  */
  BlockPanel_prototype.setClassOver = function(vntClassName, bUpdateView) { 
    this.jsxclassover = vntClassName; 
    return this;  
    };  

  /**
  * Returns the tooltip text to display when the object is hovered over.
  * @return {String}
  */
  BlockPanel_prototype.getTip = function() {
    return this.jsxtip;
  };

  /**
   * Sets the tooltip text to display when the object is hovered over; updates the model and the view
   * immediately. Note that the tip text is rendered natively by the browser and that the behavior may vary
   * between browsers. Some browsers may honor line breaks in the text and some may have a maximum length that
   * then show before truncating the tip. For more consistent rendering across browsers use the <code>SPYGLASS</code>
   * event instead.
   *
   * @param strTip {String} the tip text.
   * @return {jsx3.gui.BlockPanel} this object.
   */
   BlockPanel_prototype.setTip = function(strTip) {
    this.jsxtip = strTip;
    var objGUI;
    if(objGUI = this.getRendered()) objGUI.title = strTip;
    return this;
  };

  /**
  * Sets whether is object supports programmatic drag, meanining it will allow any contained item to be dragged/dropped.
  * Implementing classes can decide whether to consult this value or ignore it.
  * @param bDrag {int} <code>jsx3.Boolean.TRUE</code> or <code>jsx3.Boolean.FALSE</code>
  * @return {jsx3.gui.Interactive} this object
  */
  BlockPanel_prototype.setCanDrag = function(bDrag,record) {
    if(record) {
      record.setAttribute('jsxdrag', bDrag); 
      return record;
    }
    else {
      this.jsxdrag = bDrag; 
      return this; 
    }   
  };

  /**
   * Returns whether is object supports programmatic drag, meanining it will allow any contained item to be
   * dragged and dropped on another container supporting drop.
   * @return {int} <code>jsx3.Boolean.TRUE</code> or <code>jsx3.Boolean.FALSE</code>
   */
  BlockPanel_prototype.getCanDrag = function(record) {
    return record ? record.getAttribute('jsxdrag') || this.jsxdrag : this.jsxdrag;
  };
  
  /**
  * Returns if record can be selected in this Block panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  BlockPanel_prototype.isRecordSelectable = function(strRecordId) {
    var record = this.getRecord(strRecordId);
    return record && (record['jsxunselectable'] == null || record['jsxunselectable'] != "1");
  };
  
  /**
  * Returns if record is selected in this Block panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  BlockPanel_prototype.isRecordSelected = function(strRecordId) {
    var record = this.getRecord(strRecordId);
    return record && record['jsxselected'] == "1";
  }; 
 
  /**
  * Returns the Selection Model for this Block panel
  * @return {int}
  */
  BlockPanel_prototype.getSelectionModel = function() {
    return (this.jsxselectionmodel != null) ? ((this.jsxselectionmodel > BlockPanel.SELECTION_MULTI_BLOCK) ? BlockPanel.SELECTION_UNSELECTABLE : this.jsxselectionmodel) :BlockPanel.SELECTION_BLOCK ;
  };
  
  /**
  * Sets the Selection Model for this Block panel 
  * @param intType {int}
  * @return {com.ipvs.gui.BlockPanel} this object
  */
  BlockPanel_prototype.setSelectionModel = function(intType) {
    this.jsxselectionmodel = intType;
    return this;
  }; 
  
  /**
  * Get an iterator for the records that will be painted
  * Returns all the records in the attached CDF doc.  Override this to return something other.
  *
  * @return {jsx3.util.Iterator}
  */
  BlockPanel_prototype.getIterableRecords = function() {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectNodeIterator("//record"):
    (new jsx3.util.List()).iterator();
  };
  
  /**
  * Deselects all selected CDF records.
  */
  BlockPanel_prototype.deselectAllRecords = function() {
    var objCDF = this.getXML();
    var recordIterator = objCDF.selectNodeIterator(".//record[@jsxselected ='1']");
    while(recordIterator.hasNext()){
      var record = recordIterator.next();
      record.removeAttribute("jsxselected");
    }   
  };
  
  /**
  * Returns the width property of a Block on this Panel
  * @return {int|String} width.
  */
  BlockPanel_prototype.getBlockWidth = function() {
    return this.jsxblockwidth;
  };

  /**
  * Sets the width property of a Block on this Panel
  * @param vntWidth {int | String} the width as non-negative integer or non-negative percentage. For example: 45%, 12.
  * @param bUpdateView {boolean} if <code>true</code>, the view of this object is immediately updated, obviating the need to call <code>repaint()</code>.
  * @return {com.ipvs.gui.BlockPanel} this object.
  */
  BlockPanel_prototype.setBlockWidth = function(vntWidth, bUpdateView) {
    //update the model
    this.jsxblockwidth = vntWidth;
    //    //update the boxprofile
    //    if (bUpdateView) {
    //      this.syncBoxProfile({width:vntWidth}, true);
    //    } else {
    //      this.setBoxDirty();
    //    }
    return this;
  };
  
  /**
  * Returns the height property of a Block on this Panel
  * @return {int | String} height.
  */
  BlockPanel_prototype.getBlockHeight = function() {
    return this.jsxblockheight;
  };

  /**
  * Sets the height property of a Block on this Panel
  * @param vntHeight {int | String} the height as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @param bUpdateView {boolean} if <code>true</code>, the view of this object is immediately updated, obviating the need to call <code>repaint()</code>.
  * @return {com.ipvs.gui.BlockPanel} this object.
  */
  BlockPanel_prototype.setBlockHeight = function(vntHeight,bUpdateView) {
    //update the model
    this.jsxblockheight = vntHeight;
  //    //update the boxprofile
  //    if (bUpdateView) {
  //      this.syncBoxProfile({height:vntHeight}, true);
  //    } else {
  //      this.setBoxDirty();
  //    }
    return this;
  };
 
  /**
  * Returns the Css of the block of this panel based on the current state.
  * @return {String}
  */
  BlockPanel_prototype.getCurrentClass = function(bOver,record) {
    var strClass = null;  
    if(!record)return;
    var strRecordId = record.getAttribute("jsxid");
    if(!this.isRecordSelectable(strRecordId))return "endpointOffline";     
    if(this.isRecordSelected(strRecordId)){
      strClass =this.getClassOn();
    } 
    else{
      if (bOver)
        strClass = this.getClassOver();
      else
        strClass = this.getClassOff();
    }   
    return strClass ? strClass : this.jsxclassoff ;
  };
    
  /**
  * Returns the Img of the delete connection/session button of this panel based on the current state.
  * @return {String}
  */
  BlockPanel_prototype.getCurrentDeleteBlockImg = function(bOver,record) {
    if(bOver){     
      strPath = BlockPanel.BUTTON_DELETEBLOCKOVER;
    }
    else{
      strPath = BlockPanel.BUTTON_DELETEBLOCKUP;
    }
    return strPath;
  };
 

  /*--------------------------- Form Fields Related code -------------------------------*/
    
  /**
  * Returns the value of a field on an instance of this block
  * read from a local instance property or a CDF record attribute
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockPanel_prototype.getField = function(fName, record) {
    var fVal = null;
    var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES.VAL;
    if( attName ) {
      fVal = record ? record.getAttribute(attName) : this[attName]; 
    }
    if( ! fVal ) fVal = 'DEFAULT';
    return fVal;
  };
  
  /**
  * Sets the value of a field on this block.
  * to a local instance attribute or a CDF record attribute
  * @param fName {String} - eg: type
  * @param fVal {String} - eg: v2d
  * @return {com.ipvs.gui.BlockPanel} this object or record
  */
  BlockPanel_prototype.setField = function(fName, fVal, record) {
    var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES.VAL;
    if( attName ) {
      if( record ) {
        record.setAttribute(attName, fVal); 
        return record;
      }
      else {
        this[attName] = fVal; 
        return this; 
      }
    } 
  };

  /**
  * Returns the imageURI or lblText of a field on this block.
  * If a record node is passed then it looks for the value in the record
  * If not found, or no record specified - it returns the value from the instance array
  * @param fName {String} - eg: Type
  * @param fVal {String} - eg: v2d
  * @param fFmt {String} - eg: IMG/LBL
  * @param record (Entity) - eg: jsxfield_Type='v2d' jsxfield_img_Type='URL'
  * @return {String}
  */
  BlockPanel_prototype.getFieldView = function(fName, fVal, fFmt, record) {
    var fView = null;
    if( record ) {
      var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES[fFmt];
      if( attName ) fView = record.getAttribute(attName);
    }
    if( ! fView ) fView = this.getFormFieldViewArr()[fName].VALUES[fVal][fFmt];
    return fView; 
  };
  
  /**
  * Sets the URI/Txt of the image/lbl of a field on this block.
  * If a record node is passed then it sets it in the record attribute else in instance array
  * @param fName {String} - eg: type
  * @param fVal {String} - eg: v2d
  * @param strImage {String}
  * @param fFmt {String} - eg: IMG/LBL
  * @return {com.ipvs.gui.BlockPanel} this object
  */
  BlockPanel_prototype.setFieldView = function(fName, fVal, fFmt, fView, record) {
    if( record ) {
      var attName = this.getFormFieldViewArr()[fName][fFmt+'_ATTRIBUTE'];
      if( attName ) record.setAttribute(attName, fView); 
      return record;
    }
    else {
      this.getFormFieldViewArr()[fName].VALUES[fVal][fFmt] = fView; 
      return this; 
    }
  };

  /**
  * Returns the currently applicable URI/Txt of the image/lbl of a field on this block.
  * The current value of the attribute comes from either an instance property or an attribute of the CDF record passed in
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockPanel_prototype.getCurrentFieldImgURL = function(fName, record) {
    var strPath = this.getFieldView(fName, this.getField(fName, record), BlockPanel.FORM_FIELD_FMT_IMG, record);
    return strPath ? this.getUriResolver().resolveURI(strPath) : "";
  };
  
  /**
  * Returns the currently applicable text value of a field on this block.
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockPanel_prototype.getCurrentFieldLblTxt = function(fName, record) {
    var strTxt = this.getFieldView(fName, this.getField(fName, record), BlockPanel.FORM_FIELD_FMT_LBL, record);
    return strTxt ? strTxt : "";
  }
    
  /*--------------------------- Form Fields Related code -------------------------------*/
    
  /**
  * Returns the value of a field on an instance of this block
  * read from a local instance property or a CDF record attribute
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockPanel_prototype.getField = function(fName, record) {
    var fVal = null;
    var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES.VAL;
    if( attName ) {
      fVal = record ? record.getAttribute(attName) : this[attName]; 
    }
    if( ! fVal ) fVal = 'DEFAULT';
    return fVal;
  };
  
  /**
  * Sets the value of a field on this block.
  * to a local instance attribute or a CDF record attribute
  * @param fName {String} - eg: type
  * @param fVal {String} - eg: v2d
  * @return {com.ipvs.gui.BlockPanel} this object or record
  */
  BlockPanel_prototype.setField = function(fName, fVal, record) {
    var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES.VAL;
    if( attName ) {
      if( record ) {
        record.setAttribute(attName, fVal); 
        return record;
      }
      else {
        this[attName] = fVal; 
        return this; 
      }
    } 
  };

  /**
  * Returns the imageURI or lblText of a field on this block.
  * If a record node is passed then it looks for the value in the record
  * If not found, or no record specified - it returns the value from the instance array
  * @param fName {String} - eg: Type
  * @param fVal {String} - eg: v2d
  * @param fFmt {String} - eg: IMG/LBL
  * @param record (Entity) - eg: jsxfield_Type='v2d' jsxfield_img_Type='URL'
  * @return {String}
  */
  BlockPanel_prototype.getFieldView = function(fName, fVal, fFmt, record) {
    var fView = null;
    if( record ) {
      var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES[fFmt];
      if( attName ) fView = record.getAttribute(attName);
    }
    if( ! fView ) fView = this.getFormFieldViewArr()[fName].VALUES[fVal][fFmt];
    return fView; 
  };
  
  /**
  * Sets the URI/Txt of the image/lbl of a field on this block.
  * If a record node is passed then it sets it in the record attribute else in instance array
  * @param fName {String} - eg: type
  * @param fVal {String} - eg: v2d
  * @param strImage {String}
  * @param fFmt {String} - eg: IMG/LBL
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockPanel_prototype.setFieldView = function(fName, fVal, fFmt, fView, record) {
    if( record ) {
      var attName = this.getFormFieldViewArr()[fName][fFmt+'_ATTRIBUTE'];
      if( attName ) record.setAttribute(attName, fView); 
      return record;
    }
    else {
      this.getFormFieldViewArr()[fName].VALUES[fVal][fFmt] = fView; 
      return this; 
    }
  };

  /**
  * Returns the currently applicable URI/Txt of the image/lbl of a field on this block.
  * The current value of the attribute comes from either an instance property or an attribute of the CDF record passed in
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockPanel_prototype.getCurrentFieldImgURL = function(fName, record) {
    var strPath = this.getFieldView(fName, this.getField(fName, record), BlockPanel.FORM_FIELD_FMT_IMG, record);
    return strPath ? this.getUriResolver().resolveURI(strPath) : "";
  };
  
  /**
  * Returns the currently applicable text value of a field on this block.
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockPanel_prototype.getCurrentFieldLblTxt = function(fName, record) {
    var strTxt = this.getFieldView(fName, this.getField(fName, record), BlockPanel.FORM_FIELD_FMT_LBL, record);
    return strTxt ? strTxt : "";
  };

  /**
  * Returns the Horizontal Scroll position of the block..
  * @return {String}
  */
  BlockPanel_prototype.getHScrollPosition = function() {
    return this.jsxhscrollposition;
  };

  /**
  * Sets the  Horizontal Scroll position of the block.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.BlockPanel} this object
  */
  BlockPanel_prototype.saveHScrollPosition = function(vntHScrollPosition) {
    if (jsx3.util.strEmpty(vntHScrollPosition))
      vntHScrollPosition = 0;
    this.jsxhscrollposition = vntHScrollPosition;
    return this;
  };

  /**
  * Returns the Vertical Scroll position of the block.
  * @return {String}
  */
  BlockPanel_prototype.getVScrollPosition = function() {
    return this.jsxvscrollposition;
  };

  /**
  * Sets the  Vertical Scroll position of the block.
  * @param vntVScrollPosition {int | String} the vertical position is a string. For example:"200".
  * @return {com.ipvs.gui.BlockPanel} this object
  */
  BlockPanel_prototype.saveVScrollPosition = function(vntVScrollPosition) {
    if (jsx3.util.strEmpty(vntVScrollPosition))
      vntVScrollPosition = 0;
    this.jsxvscrollposition = vntVScrollPosition;
    return this;
  };

  BlockPanel_prototype.onScroll = function(objEvent, objGUI) {
    objEvent.cancelBubble();
    var scrollLeft = objGUI.getAttribute("scrollLeft");
    this.saveHScrollPosition(scrollLeft);
    var scrollTop = objGUI.getAttribute("scrollTop");
    this.saveVScrollPosition(scrollTop);
  };
  
/*--------------------- OnMouse Events-------------------------*/
  
  BlockPanel_prototype.onMouseOver = function(objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId); 
    objGUI.className = this.getCurrentClass(true,record);
    if (this.getCanDrop() == 1)
      this.doDrop(objEvent, objGUI, jsx3.EventHelp.ONDROP);      
  };

  BlockPanel_prototype.onMouseOut = function(objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);  
    objGUI.className = this.getCurrentClass(false,record);
    if (this.getCanDrop() == 1)
      this.doDrop(objEvent, objGUI, jsx3.EventHelp.ONDROP);      
  };

  BlockPanel_prototype.onMouseDown = function(objEvent, objGUI) {
    if (objEvent.leftButton()){
      var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
      var record = this.getRecordNode(strCdfId);       
      objGUI.className = this.getCurrentClass(true, record);
      if(this.getCanDrag() == 1){
        this.doBeginDrag(objEvent, objGUI);
      }        
    }
  };

  BlockPanel_prototype.onMouseUp = function(objEvent, objGUI) {
    if (objEvent.rightButton()) {
      this.jsxsupermix(objEvent, objGUI);
    }
    else if (objEvent.leftButton()) {
      var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
      var record = this.getRecordNode(strCdfId);    
    }
  };

  BlockPanel_prototype.onMouseOver_delete= function(objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteBlockImg(true, record);
    this.setTip("Delete");     
  };

  BlockPanel_prototype.onMouseOut_delete= function(objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.src = this.getCurrentDeleteBlockImg(false, record);
    this.setTip(""); 
  };
    
  /*------------------------ OnClick Events ------------------------*/  

  BlockPanel_prototype.onClick = function(objEvent, objGUI) {
    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length+1);
    var selected = this.isRecordSelected(id);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null; 
    if(!this.isRecordSelectable(id))return;     
          
    if(selected){
      if (this.getType(record) == BlockPanel.TYPE_NORMAL) {
        return;
      }   
      if (this.getType(record) == BlockPanel.TYPE_TOGGLE) {
        record.removeAttribute("jsxselected"); 
      }   
    }
    if(this.getSelectionModel() == com.ipvs.gui.BlockPanel.SELECTION_BLOCK){
      this.deselectAllRecords();
    }
    if (!selected) {
       record.setAttribute("jsxselected","1");  
    }
    this.removeEvent(Interactive.SCROLL);
    this.repaint();
     // do SELECT/EXECUTE event
    record && this.setValue(id);
    this.setEvent(this.onScroll, Interactive.SCROLL);
    var event = record ? Interactive.SELECT : Interactive.EXECUTE;
    this.doEvent(event, {objEVENT:objEvent});    
  };
    
  BlockPanel_prototype.onClick_delete = function(objEvent, objGUI) {
    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length+1);
    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    record && this.setValue(id);     
      // do SELECT/EXECUTE event
    var event = record ? BlockPanel.DeleteBlock : Interactive.EXECUTE;
    this.doEvent(event, {objEVENT:objEvent});  
  };
      
  /**
   * The XML Template used by the Template class to render this Block Panel
   *
   * @return {String}
   */
  BlockPanel_prototype.getTemplateXML = function() {
      // form template string
      return ['',
      '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
      '  <model>',
      '  </model>',
      '  <template dom="static">', 
      ' <inlinebox u:id="buttonpanel" style="position:{$position};left:{$left};top:{$top};width:{$width};height:{$height};margin:{$margin};">',
      ' <for-each select="this.getIterableRecords()">',
      '   <var id="blockid">this.getId() + \'_\' + $$target.getAttribute(\'jsxid\')</var>',
      '   <var id="blockdragid">$$target.getAttribute(\'jsxid\')</var>',   
      '   <var id="blocktext">this.getText($$target)</var>',
      '   <var id="cssclass">this.getCurrentClass(false,$$target)</var>',
      '   <div u:id="block" id="{blockid}" JSXDragId="{blockdragid}" JSXDragType="JSX_GENERIC" style="margin:0;padding:0;position:relative;width:', this.getBlockWidth(), 'px;height:', this.getBlockHeight(), 'px;display:block;font-family:Arial, Helvetica, sans-serif;" onclick="{onClick}" onmouseover="{onMouseOver}" onmouseout="{onMouseOut}" onmousedown="{onMouseDown}" onmouseup="{onMouseUp}" class="{cssclass}">',      
      '        <span u:id="blocktext" style="position:absolute; left:36px; top:2px; font-size:12px; font-weight:bold;">',
      '           <text>{blocktext}</text>',
      '        </span>',    
      '    </div>',
      '   </for-each>',
      '   </inlinebox>',
      '  </template>',
      '</transform>'].join("");
  };

});