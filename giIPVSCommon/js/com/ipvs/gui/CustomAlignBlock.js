/*
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
*/

/**
* Class is extended from block to support custom align for a block.
*/
jsx3.require("jsx3.gui.Block");

jsx3.Class.defineClass("com.ipvs.gui.CustomAlignBlock", jsx3.gui.Block, null, function(CustomAlignBlock, CustomAlignBlock_prototype) {

  /**
  * setRight method will set align from right of the parent block.
  * @param vntRight {int} pixels from Right to align.
  * @param bUpdateView {boolean}.If it is true view will be updated immidiatly
  */
  CustomAlignBlock_prototype.setRight = function(vntRight, bUpdateView) {
    this.jsxright = vntRight;
    if(bUpdateView) {
      this.repaint();
    }
  }

  /**
  * setBottom method will set align from Bottom of the parent block.
  * @param vntBottom {int} pixels from Bottom to align.
  * @param bUpdateView {boolean}.If it is true view will be updated immidiatly
  */
  CustomAlignBlock_prototype.setBottom = function(vntBottom, bUpdateView) {
    this.jsxbottom = vntBottom;
    if(bUpdateView) {
      this.repaint();
    }
  }

  /**
  * reSetWidth method will recalculate and do the setLeft to maintain the Right align when Width is changed.
  * @param vntWidth {int} pixels from Bottom to align.
  */
  CustomAlignBlock_prototype.reSetWidth = function(vntWidth) {
    var currenLeft = this.getLeft();
    var currentWidth = this.getWidth();
    var vntLeft = parseInt(currenLeft) + parseInt(currentWidth) - parseInt(vntWidth);
    this.setDimensions(vntLeft, null, vntWidth, null, true);
  }

  /**
  * reSetHeight method will recalculate and do the setTop to maintain the Bottom align when Height is changed.
  * @param vntHeight {int} pixels from Bottom to align.
  */
  CustomAlignBlock_prototype.reSetHeight = function(vntHeight) {
    var currenTop = this.getTop();
    var currentHeight = this.getHeight();
    var vntTop = parseInt(currenTop) + parseInt(currentHeight) - parseInt(vntHeight);
    this.setDimensions(null, vntTop, null, vntHeight, true);
  }

  /**
  * Returns the Right property of this object.
  * @return {int|String} right.
  */
  CustomAlignBlock_prototype.getRight = function() {
    return this.jsxright;
  }

  /**
  * Returns the Bottom property of this object.
  * @return {int|String} bottom.
  */
  CustomAlignBlock_prototype.getBottom = function() {
    return this.jsxbottom;
  }

  /**
  * Overriding Block createBoxProfile.
  * This will recalculate Left and Top based on right/bottom properties.
  * @param objImplicit {object} implicit map comprised of one or more of the following: left, top, width, height, boxtype, tagname, parentheight, parentwidth
  * @return {jsx3.gui.Painted.Box}
  */
  //For Src version of GI
  if(CustomAlignBlock_prototype.createBoxProfile) {
    CustomAlignBlock_prototype.createBoxProfile = function(objImplicit) {
      if(this.getParent() && (objImplicit == null || ((isNaN(objImplicit.parentwidth) || isNaN(objImplicit.parentheight))))) {
        objImplicit = this.getParent().getClientDimensions(this);
      }
      if(objImplicit != null) {
        objImplicit = this._modifyBoxProfile(objImplicit);
      }
      return this.jsxsuper(objImplicit);
    }
  }
  //For Build
  else if(CustomAlignBlock_prototype.zd) {
    CustomAlignBlock_prototype.zd = function(objImplicit) {
      if(this.getParent() && (objImplicit == null || ((isNaN(objImplicit.parentwidth) || isNaN(objImplicit.parentheight))))) {
        objImplicit = this.getParent().Id(this);
      }
      if(objImplicit != null) {
        objImplicit = this._modifyBoxProfile(objImplicit);
      }
      return this.jsxsuper(objImplicit);
    }
  }
  //For Debug version of GI
  else {
    CustomAlignBlock_prototype.Vm = function(objImplicit) {
      if (this.getParent() && (objImplicit == null || ((isNaN(objImplicit.parentwidth) || isNaN(objImplicit.parentheight))))) {
        objImplicit = this.getParent().ng(this);
      }
      if (objImplicit != null) {
        objImplicit = this._modifyBoxProfile(objImplicit);
      }
      return this.jsxsuper(objImplicit);
    }
  }
  

  /**
  * Overriding Block updateBoxProfile.
  * This will recalculate Left and Top based on right/bottom properties.
  * @param objImplicit {object} implicit map comprised of one or more of the following: left, top, width, height, boxtype, tagname, parentheight, parentwidth
  * @param objGUI {object} native browser element representing the view for the dialog instance
  */
  //For Src version of GI
  if(CustomAlignBlock_prototype.updateBoxProfile) {
    CustomAlignBlock_prototype.updateBoxProfile = function(objImplicit, objGUI, objQueue) {
      objImplicit = this._modifyBoxProfile(objImplicit);
      this.jsxsuper(objImplicit, objGUI, objQueue);
    }
  }
  //For build
  else if(CustomAlignBlock_prototype.Hj) {
    CustomAlignBlock_prototype.Hj = function(objImplicit, objGUI, objQueue) {
      objImplicit = this._modifyBoxProfile(objImplicit);
      this.jsxsuper(objImplicit, objGUI, objQueue);
    }
  }
  //For Debug of version of GI
  else {
    CustomAlignBlock_prototype.Rc = function(objImplicit, objGUI, objQueue) {
      objImplicit = this._modifyBoxProfile(objImplicit);
      this.jsxsuper(objImplicit, objGUI, objQueue);
    }
  }

  /** @private @jsxobf-clobber */
  CustomAlignBlock_prototype._modifyBoxProfile = function(objImplicit) {
    if(!jsx3.util.strEmpty(objImplicit.parentwidth)) {
      var vntRight = this.getRight();
      var vntWidth = this.getWidth();
      if(!jsx3.util.strEmpty(vntRight)) {
        if((isNaN(vntWidth) && vntWidth.indexOf("%") == -1) || !isNaN(vntWidth)) {
          var vntLeft = objImplicit.parentwidth - parseInt(vntWidth) - parseInt(vntRight);
          if(vntLeft < 0) vntLeft = 0;
          objImplicit.left = vntLeft;
          this.setLeft(vntLeft)
        }
      }
    }

    if(!jsx3.util.strEmpty(objImplicit.parentheight)) {
      var vntBottom = this.getBottom();
      var vntHeight = this.getHeight();
      if(!jsx3.util.strEmpty(vntBottom)) {
        if((isNaN(vntHeight) && vntHeight.indexOf("%") == -1) || !isNaN(vntHeight)) {
          var vntTop = objImplicit.parentheight - parseInt(vntHeight) - parseInt(vntBottom);
          if(vntTop < 0) vntTop = 0;
          objImplicit.top = vntTop;
          this.setTop(vntTop);
        }
      }
    }
    return objImplicit;
  }
});