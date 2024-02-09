/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */

jsx3.require("jsx3.gui.ImageButton");

/**
 * Mixin interface that provides methods for accessing and manipulating an Image Button.
 * <p/>
 * This interface is modeled on the lines of the Image Button provided/used by GI
 */
jsx3.Class.defineInterface("com.ipvs.gui.BlockButton", null, function(BlockButton, BlockButton_prototype) {

  /** @private @jsxobf-clobber */
  BlockButton_prototype._LOG = jsx3.util.Logger.getLogger(BlockButton.jsxclass.getName());

  var Event = jsx3.gui.Event;
  var Interactive = jsx3.gui.Interactive;
  var Form = jsx3.gui.Form;
  var ImageButton = jsx3.gui.ImageButton;


  /** {String} @private */
  BlockButton_prototype.jsxtext = null;

  /** {String} @private */
  BlockButton_prototype.jsxoffclass = null;

  /** {String} @private */
  BlockButton_prototype.jsxoverclass = null;

  /** {String} @private */
  BlockButton_prototype.jsxdownclass = null;

  /** {String} @private */
  BlockButton_prototype.jsxonclass = null;

  /** {String} @private */
  BlockButton_prototype.jsxdisabledclass = null;

  /** {int} @private */
  BlockButton_prototype.jsxtype = ImageButton.TYPE_NORMAL;

  /** {int} @private */
  BlockButton_prototype.jsxstate = ImageButton.STATE_OFF;

  /**
  * {int} Defaults for the form FieldTypes.
  */
  BlockButton.FORM_FIELD_FMT_IMG = 'IMG';
  BlockButton.FORM_FIELD_FMT_LBL = 'LBL';

  /**
  * Sample entry for adding a form field
  *  
  */



  /*--------------------------- Form Fields Related code -------------------------------*/


  /**
  * Returns the value of a field on an instance of this button
  * read from a local instance property or a CDF record attribute
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockButton_prototype.getField = function(fName, record) {
    var fVal = null;
    var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES.VAL;
    if (attName) {
      fVal = record ? record.getAttribute(attName) : this[attName];
    }
    if (!fVal) fVal = 'DEFAULT';
    return fVal;
  };

  /**
  * Sets the value of a field on this button.
  * to a local instance attribute or a CDF record attribute
  * @param fName {String} - eg: type
  * @param fVal {String} - eg: v2d
  * @return {com.ipvs.gui.BlockButton} this object or record
  */
  BlockButton_prototype.setField = function(fName, fVal, record) {
    var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES.VAL;
    if (attName) {
      if (record) {
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
  * Returns the imageURI or lblText of a field on this button.
  * If a record node is passed then it looks for the value in the record
  * If not found, or no record specified - it returns the value from the instance array
  * @param fName {String} - eg: Type
  * @param fVal {String} - eg: v2d
  * @param fFmt {String} - eg: IMG/LBL
  * @param record (Entity) - eg: jsxfield_Type='v2d' jsxfield_img_Type='URL'
  * @return {String}
  */
  BlockButton_prototype.getFieldView = function(fName, fVal, fFmt, record) {
    var fView = null;
    if (record) {
      var attName = this.getFormFieldViewArr()[fName].ATTRIBUTES[fFmt];
      if (attName) fView = record.getAttribute(attName);
    }
    if (!fView) fView = this.getFormFieldViewArr()[fName].VALUES[fVal][fFmt];
    return fView;
  };

  /**
  * Sets the URI/Txt of the image/lbl of a field on this button.
  * If a record node is passed then it sets it in the record attribute else in instance array
  * @param fName {String} - eg: type
  * @param fVal {String} - eg: v2d
  * @param strImage {String}
  * @param fFmt {String} - eg: IMG/LBL
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setFieldView = function(fName, fVal, fFmt, fView, record) {
    if (record) {
      var attName = this.getFormFieldViewArr()[fName][fFmt + '_ATTRIBUTE'];
      if (attName) record.setAttribute(attName, fView);
      return record;
    }
    else {
      this.getFormFieldViewArr()[fName].VALUES[fVal][fFmt] = fView;
      return this;
    }
  };

  /**
  * Returns the currently applicable URI/Txt of the image/lbl of a field on this button.
  * The current value of the attribute comes from either an instance property or an attribute of the CDF record passed in
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockButton_prototype.getCurrentFieldImgURL = function(fName, record) {
    var strPath = this.getFieldView(fName, this.getField(fName, record), BlockButton.FORM_FIELD_FMT_IMG, record);
    return strPath ? this.getUriResolver().resolveURI(strPath) : "";
  };

  /**
  * Returns the currently applicable text value of a field on this button.
  * @param fName {String} - eg: type
  * @return {String}
  */
  BlockButton_prototype.getCurrentFieldLblTxt = function(fName, record) {
    var strTxt = this.getFieldView(fName, this.getField(fName, record), BlockButton.FORM_FIELD_FMT_LBL, record);
    return strTxt ? strTxt : "";
  };


  /*--------------------------- Image Button Related code -------------------------------*/
  /**
  * Returns the Text of the label of this button.
  * @return {String}
  */
  BlockButton_prototype.getText = function(record) {
    return record ? record.getAttribute('jsxtext') || this.jsxtext : this.jsxtext;
  };

  /**
  * Sets the Text of the label of this button.
  * @param strLabel {String}
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setText = function(strLabel, record) {
    if (record) {
      record.setAttribute('jsxtext', strLabel);
      return record;
    }
    else {
      this.jsxtext = strLabel;
      return this;
    }
  };

  /**
  * Returns the css of the image button when OFF.
  * @return {String}
  */
  BlockButton_prototype.getOffClass = function(record) {
    return record ? record.getAttribute('jsxoffclass') || this.jsxoffclass : this.jsxoffclass;
  };

  /**
  * Sets the css of this button. This is the default css that is applied if the button
  * is off or if the button is in a state for which no css is specified.
  * @param strImage {String}
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setOffClass = function(strClass, record) {
    if (record) {
      record.setAttribute('jsxoffclass', strImage);
      return record;
    }
    else {
      this.jsxoffclass = strClass;
      return this;
    }
  };

  /**
  * Returns the css of the image button when Over.
  * @return {String}
  */
  BlockButton_prototype.getOverClass = function(record) {
    return record ? record.getAttribute('jsxoverclass') || this.jsxoverclass : this.jsxoverclass;
  };

  /**
  * Sets the css of this button. This is the css that is applied when the mouse moves
  * over the panel button.
  * @param strImage {String}
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setOverClass = function(strClass, record) {
    if (record) {
      record.setAttribute('jsxoverclass', strClass);
      return record;
    }
    else {
      this.jsxoverclass = strClass;
      return this;
    }
  };

  /**
  * Returns the css of the image button when Down.
  * @return {String}
  */
  BlockButton_prototype.getDownClass = function(record) {
    return record ? record.getAttribute('jsxdownclass') || this.jsxdownclass : this.jsxdownclass;
  };

  /**
  * Sets the css of this button. This is the css that is applied when the mouse is down
  * over the panel button.
  * @param strImage {String}
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setDownClass = function(strClass, record) {
    if (record) {
      record.setAttribute('jsxdownclass', strClass);
      return record;
    }
    else {
      this.jsxdownclass = strClass;
      return this;
    }
  };
  /**
  * Returns the css of the image button when On.
  * @return {String}
  */
  BlockButton_prototype.getOnClass = function(record) {
    return record ? record.getAttribute('jsxonclass') || this.jsxonclass : this.jsxonclass;
  };

  /**
  * Sets the css of this button. This is the css that is applied when this panel button is
  * selected.
  * @param strImage {String}
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setOnClass = function(strClass, record) {
    if (record) {
      record.setAttribute('jsxonclass', strClass);
      return record;
    }
    else {
      this.jsxonclass = strClass;
      return this;
    }
  };

  /**
  * Returns the css of the image button when Disabled.
  * @return {String}
  */
  BlockButton_prototype.getDisabledClass = function(record) {
    return record ? record.getAttribute('jsxdisabledclass') || this.jsxdisabledclass : this.jsxdisabledclass;
  };

  /**
  * Sets the css of this button. This is the css that is applied when this panel
  * button is disabled.
  * @param strImage {String}
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setDisabledClass = function(strClass, record) {
    if (record) {
      record.setAttribute('jsxdisabledclass', strClass);
      return record;
    }
    else {
      this.jsxdisabledclass = strClass;
      return this;
    }
  };

  /**
  * Returns the current state of this panel button.
  * @return {int} <code>ImageButton.STATE_OFF</code> or <code>ImageButton.STATE_ON</code>
  * @see #STATE_OFF
  * @see #STATE_ON
  */
  BlockButton_prototype.getState = function(record) {
    return record ? record.getAttribute('jsxselected') || this.jsxstate : this.jsxstate;
  };
  /**
  * Sets the current state of this panel button and updates the displayed image accordingly.
  * @param intState {int} <code>ImageButton.STATE_OFF</code> or <code>ImageButton.STATE_ON</code>
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setState = function(intState, record) {
    if (record) {
      record.setAttribute('jsxselected', intState);
      return record;
    }
    else {
      this.jsxstate = intState;
      return this;
    }
  };

  /**
  * Returns the type of this panel button.
  * @return {int} <code>ImageButton.TYPE_NORMAL</code> or <code>ImageButton.TYPE_TOGGLE</code>
  * @see #TYPE_NORMAL
  * @see #TYPE_TOGGLE
  */
  BlockButton_prototype.getType = function(record) {
    return record ? record.getAttribute('jsxtype') || this.jsxtype : this.jsxtype;
  };
  /**
  * Sets the type of this panel button.
  * @param intType {int} <code>ImageButton.TYPE_NORMAL</code> or <code>ImageButton.TYPE_TOGGLE</code>
  * @return {com.ipvs.gui.BlockButton} this object
  */
  BlockButton_prototype.setType = function(intType, record) {
    if (record) {
      record.setAttribute('jsxtype', intType);
      return record;
    }
    else {
      this.jsxtype = intType;
      return this;
    }
  };

  /**
  * Returns the  css of this panel button based on the current state.
  * @return {String}
  */
  BlockButton_prototype.getCurrentClass = function(bOver, bDown, record) {
    jsx3.log("BlockButton_prototype.getCurrentClass");
    var strClass = null;
    var strRecordId = record.getAttribute("jsxid");
    if (!this.isRecordSelectable(strRecordId)) {
      strClass = this.getDisabledClass(record);
    }
    else if (this.getEnabled() == Form.STATEENABLED) {
      if (bOver)
        strClass = this.getOverClass(record);
      else if (bDown)
        strClass = this.getDownClass(record);

      if (this.getState(record) == ImageButton.STATE_ON) {
        strClass = this.getOnClass(record);
      }
    } else {
      strClass = this.getDisabledClass(record);
    }

    strClass = strClass || this.getOffClass(record);
    return strClass;
  };

  /**
  * Returns the Text string of this panel button.
  * @return {String}
  */
  BlockButton_prototype.getCurrentLabelText = function(record) {
    var text = this.getText(record);
    return this.getEllipsisText(text, 17);
  };

  /**
  * Returns the Text string of this panel button in the ellipsis format.
  * @return {String}
  */
  BlockButton_prototype.getEllipsisText = function(text, textWidth) {
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

  // events
  BlockButton_prototype.onMouseOver = function(objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.className = this.getCurrentClass(true, false, record);
  };

  BlockButton_prototype.onMouseOut = function(objEvent, objGUI) {
    var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
    var record = this.getRecordNode(strCdfId);
    objGUI.className = this.getCurrentClass(false, false, record);
  };

  BlockButton_prototype.onMouseDown = function(objEvent, objGUI) {
    if (objEvent.leftButton()) {
      var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
      var record = this.getRecordNode(strCdfId);
      objGUI.className = this.getCurrentClass(false, true, record);
    }
  };

  BlockButton_prototype.onMouseUp = function(objEvent, objGUI) {
    if (objEvent.rightButton()) {
      this.jsxsupermix(objEvent, objGUI);
    }
    else if (objEvent.leftButton()) {
      var strCdfId = objGUI.getAttribute("id").substr(this.getId().length + 1);
      var record = this.getRecordNode(strCdfId);
      objGUI.className = this.getCurrentClass(false, false, record);
    }
  };

  BlockButton_prototype.onClick = function(objEvent, objGUI) {
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length + 1);

    // get the CDF record node for this - if any
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    if (!this.isRecordSelectable(id)) return;      
    record && this.setValue(id);

    // set state if toggle button
    if (this.getType(record) == ImageButton.TYPE_TOGGLE) {
      var newState = this.getState(record) == ImageButton.STATE_OFF ? ImageButton.STATE_ON : ImageButton.STATE_OFF;
      var bCancel = this.doEvent(Interactive.TOGGLE, { objEVENT: objEvent, intSTATE: newState, _gipp: 1 });
      if (bCancel !== false) {
        this.setState(newState, record);
        objGUI.className = this.getCurrentClass(false, false, record);
      }
    }
    else {
      // Deselect all records and set state if only Single Selection is allowed
      if (this.getSelectionModel() == com.ipvs.gui.BlockButtonPanel.SELECTION_BUTTON) {
        var recordIter = this.getXML().selectNodeIterator(".//record[@jsxid !='" + id + "']");
        while (recordIter.hasNext()) {
          var recordNode = recordIter.next();
          this.setState(ImageButton.STATE_OFF, recordNode);
        }
      }
      this.setState(ImageButton.STATE_ON, record);
    }
    // do SELECT/EXECUTE event
    var event = record ? Interactive.SELECT : Interactive.EXECUTE;
    this.doEvent(event, { objEVENT: objEvent });
  };

});
 