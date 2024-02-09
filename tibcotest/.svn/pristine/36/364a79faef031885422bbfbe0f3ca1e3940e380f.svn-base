jsx3.Class.defineInterface("com.ipvs.gui.CanvasInterface", null, function(CanvasInterface, CanvasInterface_prototype) {

// create Logger for this class
  CanvasInterface_prototype._LOG = jsx3.util.Logger.getLogger(CanvasInterface.jsxclass.getName());
  
  var Interactive = jsx3.gui.Interactive;
  var Event = jsx3.gui.Event;
 
  // Selection Model for button panel
  CanvasInterface_prototype.SELECTION_UNSELECTABLE = 0;
  CanvasInterface_prototype.SELECTION_SINGLE = 1;
  CanvasInterface_prototype.SELECTION_MULTI = 2;

  // Selection Type for Canvas Windows
  CanvasInterface.SELECTION_NORMAL = 0;
  CanvasInterface.SELECTION_TOGGLE = 1;
  
  CanvasInterface_prototype.CANVAS_WIDTH = 1920;
  CanvasInterface_prototype.CANVAS_HEIGHT = 1080;
  
  //Event Variables 
  CanvasInterface.SelectWindow = "jsxselectwindow";  
  /**
  * Returns the id of this Record.
  * @return {String}
  */
  CanvasInterface_prototype.getRecordId = function(record) { 
    return record ? record.getAttribute('jsxid') || this.jsxid : this.jsxid; 
  };
  
  /**
  * Returns the recordName of this Record.
  * @return {String}
  */
  CanvasInterface_prototype.getText = function(record) { 
    return record ? record.getAttribute('jsxtext') || this.jsxtext : this.jsxtext; 
  };
    
  /**
   * Sets the Text of the label of this Canvas.
   * @param strLabel {String}
   * @return {com.ipvs.gui.CanvasInterface} this object
   */
  CanvasInterface_prototype.setText = function(strLabel) { 
//    if( record ){
//      record.setAttribute('jsxtext', strLabel);
//      return record;
//    }
//    else {
//      this.jsxtext = strLabel;
//      return this;
//    }
    this.jsxtext = strLabel;
    return this;
  };
  
  CanvasInterface_prototype.getAspectRatio = function(canvasNode) { 
    if(!canvasNode)return;
    var canvasWidth = canvasNode.getAttribute("hres");
    var canvasHeight = canvasNode.getAttribute("vres");
    if( canvasWidth!= null && canvasHeight!= null){
      var aspectRatio = canvasWidth/canvasHeight;
    }
    else{
      var aspectRatio = CanvasInterface_prototype.CANVAS_WIDTH/CanvasInterface_prototype.CANVAS_HEIGHT;
    }
    return aspectRatio;
  };
  
  
      
 /**
  * Get an iterator for the records within the CDF
  * Returns all the records in the given CDF.
  * @return {jsx3.util.Iterator}
  */
  CanvasInterface_prototype.getIterableRecords = function() {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectNodeIterator(".//record"):
                  (new jsx3.util.List()).iterator();    
  };       
 

  /**
  * Returns the id of this WindowBlk.
  * @return {String}
  */
  CanvasInterface_prototype.getWindowId = function(windowNode) { 
    return windowNode ? windowNode.getAttribute('id') || this.id : this.id; 
  };
  
   /**
  * Returns the Connection Object with jsxconnectionid equals strRecordId.
  * @param strRecordId {String}
  * @return ConnectionObj
  */
  CanvasInterface_prototype.getWindow = function(strRecordId) {
    var objCDF = this.getXML();
    var windowNode = objCDF.selectSingleNode(".//record/Canvas/Window[@id ='"+strRecordId+"']");
    if(!windowNode)return;
    var windowObj = new Object();
    windowObj .id =  windowNode.getAttribute("id");
    windowObj .enable =  windowNode.getAttribute("enable");
    windowObj .content =  windowNode.getAttribute("content");        
    windowObj .zIndex=  windowNode.selectSingleNode(".//Properties").getAttribute("zIndex");
    windowObj .vpos =  windowNode.selectSingleNode(".//Properties").getAttribute("vpos");
    windowObj .hpos =  windowNode.selectSingleNode(".//Properties").getAttribute("hpos");
    windowObj .vres =  windowNode.selectSingleNode(".//Properties").getAttribute("vres");
    windowObj .hres =  windowNode.selectSingleNode(".//Properties").getAttribute("hres");
    windowObj .transparency = windowNode.selectSingleNode(".//Properties").getAttribute("transparency");
    return windowObj;    
  }

   /* Returns the height property of a given Canvas Outer Block
  * @return {int | String} jsxcanvasblkheight
  */
  CanvasInterface_prototype.getCanvasBlkHeight = function() {
    return this.jsxcanvasblkheight
  };

  /**
  * Sets the height property of a Canvas Outer Block
  * @param vntHeight {int | String} the height as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasBlkHeight = function(vntHeight) {
      this.jsxcanvasblkheight = vntHeight; 
      return this; 
  
  };

  /* Returns the width property of a given Canvas Outer Block
  * @return {int | String} jsxcanvasblkwidth.
  */
  CanvasInterface_prototype.getCanvasBlkWidth = function() {
    return this.jsxcanvasblkwidth;
  };

  /**
  * Sets the width property of a given Canvas Outer Block
  * @param vntWidth {int | String} the width as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasBlkWidth = function(vntWidth) {
      this.jsxcanvasblkwidth = vntWidth; 
      return this; 
  }; 
  
   /* Returns the height property of a given Canvas Inner Block
  * @return {int | String} jsxcanvasheight
  */
  CanvasInterface_prototype.getCanvasHeight = function(canvasNode) {
    if(!canvasNode)return;
    var aspectRatio = this.getAspectRatio(canvasNode);
    if(aspectRatio >=1){
     if(parseInt((this.getCanvasBlkWidth()-10)/aspectRatio)>(this.getCanvasBlkHeight()-30)){
        this.jsxcanvasheight = parseInt(this.getCanvasBlkHeight()-30);
      }
      else{
        this.jsxcanvasheight = parseInt((this.getCanvasBlkWidth()-10)/aspectRatio);
      }
    }
    else{
      this.jsxcanvasheight = parseInt(this.getCanvasBlkHeight()-30);
    }
    return this.jsxcanvasheight;
  };

  /**
  * Sets the height property of a Canvas Inner Block
  * @param vntHeight {int | String} the height as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasHeight = function(vntHeight) {
    this.jsxcanvasheight = vntHeight;
    return this; 
  };

  /* Returns the width property of a given Canvas Inner Block
  * @return {int | String} jsxcanvaswidth.
  */
  CanvasInterface_prototype.getCanvasWidth = function(canvasNode) {
    if(!canvasNode)return;
    var aspectRatio = this.getAspectRatio(canvasNode);
    if(aspectRatio >1){
      if(parseInt((this.getCanvasBlkWidth()-10)/aspectRatio)>(this.getCanvasBlkHeight()-30)){
        this.jsxcanvaswidth = parseInt((this.getCanvasBlkHeight()-30)* aspectRatio);
      }
      else{
        this.jsxcanvaswidth = parseInt(this.getCanvasBlkWidth()-10);
      }
    }
    else{
      this.jsxcanvaswidth = parseInt((this.getCanvasBlkHeight()-30)* aspectRatio);
    }
    return this.jsxcanvaswidth;
  };

  /**
  * Sets the width property of a Canvas Inner Block
  * @param vntWidth {int | String} the width as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWidth = function(vntWidth) {
    this.jsxcanvaswidth = vntWidth;
    return this; 
  };
  
  /* Returns the height property of a given Window within the Canvas
  * @return {int | String} jsxcanvaswinheight/vres.
  */
  CanvasInterface_prototype.getCanvasWindowHeight = function(windowNode,canvasNode) {
    var canvasHeight = this.getCanvasHeight(canvasNode);
    if(!canvasNode)return;
    var vRes = canvasNode.getAttribute("vres");
    if(!jsx3.util.strEmpty(vRes)){
      var displayRatioHeight = vRes/canvasHeight;
    }
    else{
      var displayRatioHeight = CanvasInterface_prototype.CANVAS_HEIGHT/canvasHeight;
    }
    var windowHeight = "";
    var displayHeight = windowHeight;
    if(windowNode){
      windowHeight = windowNode.selectSingleNode(".//Properties").getAttribute('vres');
    }
    else{
      windowHeight = this.jsxcanvaswinheight;
    }
    if( windowHeight < 0){
      windowHeight = 0;
      this._LOG.error("getCanvasWindowHeight: Encountered Negative windowHeight");
    }
    var ispercentage = windowHeight.substr(windowHeight.length-1,1);
    if( ispercentage =="%"){
      var splitResult =windowHeight.split("%");
      var percentage =splitResult[0];
      displayHeight = parseInt((percentage/100)*canvasHeight);
    }
    else{
      displayHeight = parseInt(windowHeight/displayRatioHeight);
    }
    return displayHeight;  
    //return windowNode ? windowNode.selectSingleNode(".//Properties").getAttribute('vres') ||  this.jsxwinheight :  this.jsxwinheight; 
  };

  /**
  * Sets the height property of a Window on this Canvas
  * @param vntHeight {int | String} the height as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowHeight = function(vntHeight,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswinheight',vntHeight); 
      return windowNode;
    }
    else {
      this.jsxcanvaswinheight = vntHeight; 
      return this; 
    } 
  };

  /* Returns the width property of a given Window within the Canvas
  * @return {int | String} jsxcanvaswinwidth/hres.
  */
  CanvasInterface_prototype.getCanvasWindowWidth = function(windowNode,canvasNode) {
    var canvasWidth = this.getCanvasWidth(canvasNode);
    if(!canvasNode)return;
    var hRes = canvasNode.getAttribute("hres");
    if(!jsx3.util.strEmpty(hRes)){
      var displayRatioWidth = hRes/canvasWidth;
    }
    else{
      var displayRatioWidth = CanvasInterface_prototype.CANVAS_WIDTH/canvasWidth;
    }
    var windowWidth = "";
    var displayWidth = windowWidth;
    if(windowNode){
      windowWidth = windowNode.selectSingleNode(".//Properties").getAttribute('hres');
    }
    else{
      windowWidth = this.jsxcanvaswinwidth;
    }
    if( windowWidth < 0){
      windowWidth = 0;
      this._LOG.error("getCanvasWindowWidth: Encountered Negative windowWidth");
    }
    var ispercentage = windowWidth.substr(windowWidth.length-1,1);
    if( ispercentage =="%"){
      var splitResult =windowWidth.split("%");
      var percentage =splitResult[0];
      displayWidth = parseInt((percentage/100)*canvasWidth);
    }
    else{
      displayWidth = parseInt(windowWidth/displayRatioWidth);
    }
    return displayWidth;
    //return windowNode ? windowNode.selectSingleNode(".//Properties").getAttribute('hres') ||  this.jsxwinwidth :  this.jsxwinwidth; 
  };

  /**
  * Sets the width property of a Window on this Canvas
  * @param vntWidth {int | String} the width as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowWidth = function(vntWidth,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswinwidth',vntWidth); 
      return windowNode;
    }
    else {
      this.jsxcanvaswinwidth = vntWidth; 
      return this; 
    } 
  };
  
  /* Returns the z-index property of a given Window within the Canvas
  * @return {int | String} jsxcanvaswinzindex/zIndex.
  */
  CanvasInterface_prototype.getCanvasWindowZIndex = function(windowNode) {
    return windowNode ?windowNode.selectSingleNode(".//Properties").getAttribute('zIndex') ||  this.jsxcanvaswinzindex :  this.jsxcanvaswinzindex; 
  };

  /**
  * Sets the zindex property of a Window on this Canvas
  * @param vntZIndex {int | String} the height as a integer For example: 1,2.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowZIndex = function(vntZIndex,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswinzindex',vntZIndex); 
      return windowNode;
    }
    else {
      this.jsxcanvaswinzindex = vntZIndex; 
      return this; 
    } 
  };

  /* Returns the left property of a Window on this Canvas
  * @return {int | String} jsxcanvaswinleft/hpos.
  */
  CanvasInterface_prototype.getCanvasWindowLeft = function(windowNode,canvasNode) {
    var canvasWidth = this.getCanvasWidth(canvasNode);
    if(!canvasNode)return;
    var hRes = canvasNode.getAttribute("hres");
    if(!jsx3.util.strEmpty(hRes)){
      var displayRatioWidth = hRes/canvasWidth;
    }
    else{
      var displayRatioWidth = CanvasInterface_prototype.CANVAS_WIDTH/canvasWidth;
    }
    var windowLeft = "";
    var displayLeft = windowLeft;
    if(windowNode){
      windowLeft = windowNode.selectSingleNode(".//Properties").getAttribute('hpos');
    }
    else {
      windowLeft = this.jsxcanvaswinleft;
    }
    if( windowLeft < 0){
      windowLeft = 0;
      this._LOG.error("getCanvasWindowLeft: Encountered Negative windowLeft");
    }  
    var ispercentage = windowLeft.substr(windowLeft.length-1,1);
    if( ispercentage =="%"){
      var splitResult =windowLeft.split("%");
      var percentage =splitResult[0];
      displayLeft = parseInt((percentage/100)*canvasWidth);
    }
    else{  
      displayLeft = parseInt(windowLeft/displayRatioWidth);
    }
    return displayLeft;  
   //return windowNode ? windowNode.selectSingleNode(".//Properties").getAttribute('hpos') ||  this.jsxwinleft :  this.jsxwinleft; 
  };

  /**
  * Sets the left property of a Window on this Canvas
  * @param vntLeft {int | String} the left as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowLeft = function(vntLeft,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswinleft',vntLeft); 
      return windowNode;
    }
    else {
      this.jsxcanvaswinleft = vntLeft; 
      return this; 
    } 
  };
  
  /*Returns the top property of a Window on this Canvas
  * @return {int | String} jsxcanvaswintop/vpos.
  */
  CanvasInterface_prototype.getCanvasWindowTop = function(windowNode,canvasNode) {
    var canvasHeight = this.getCanvasHeight(canvasNode);
    if(!canvasNode)return;
    var vRes = canvasNode.getAttribute("vres");
    if(!jsx3.util.strEmpty(vRes)){
      var displayRatioHeight = vRes/canvasHeight;
    }
    else{
      var displayRatioHeight = CanvasInterface_prototype.CANVAS_HEIGHT/canvasHeight;
    }
    var windowTop = "";
    var displayTop = windowTop;
    if(windowNode){
      windowTop = windowNode.selectSingleNode(".//Properties").getAttribute('vpos');
    }
    else {
      windowTop = this.jsxcanvaswintop;
    }
    if( windowTop < 0){
      windowTop = 0;
      this._LOG.error("getCanvasWindowTop: Encountered Negative windowTop");
    }  
    var ispercentage = windowTop.substr(windowTop.length-1,1);
    if( ispercentage =="%"){
      var splitResult =windowTop.split("%");
      var percentage =splitResult[0];
      displayTop = parseInt((percentage/100)*canvasHeight);
    }
    else{
      displayTop = parseInt(windowTop/displayRatioHeight);
    }
    return displayTop; 
    //return windowNode ? windowNode.selectSingleNode(".//Properties").getAttribute('vpos') ||  this.jsxwintop :  this.jsxwintop; 
  };

  /**
  * Sets the top property of a Window on this Canvas
  * @param vntTop {int | String} the top as a non-negative integer or non-negative percentage. For example: 45%, 12.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowTop = function(vntTop,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswintop',vntTop); 
      return windowNode;
    }
    else {
      this.jsxcanvaswintop = vntTop; 
      return this; 
    } 
  };
  
 /* Returns the transparency/opacity property of a Window on this Canvas
  * @return {int | String} jsxcanvaswintransparency/transparency.
  */
  CanvasInterface_prototype.getCanvasWindowTransparency = function(windowNode) {
    var transparency = this.jsxcanvaswintransparency;
    if (windowNode) {
      transparency = windowNode.selectSingleNode(".//Properties").getAttribute('transparency')
    }
    var opacity = parseInt(transparency) - 100;
    if (opacity < 0) {
      opacity = parseInt(opacity) * (-1);
    }
    return opacity;
  };

  /**
  * Sets the transparency/opacity property of a Window on this Canvas
  * @param vntTransparency {int | String} the transparency as a non-negative integer. For example: 40.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowTransparency = function(vntTransparency,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswintransparency',vntTransparency); 
      return windowNode;
    }
    else {
      this.jsxcanvaswintransparency = vntTransparency; 
      return this; 
    } 
  };
  
   /* Returns the transparency/opacity property of a Window on this Canvas
  * @return {int | String} jsxcanvaswintransparency/transparency.
  */
  CanvasInterface_prototype.getCanvasWindowVisibility = function(windowNode) {
    jsx3.log("getCanvasWindowVisibility");
    jsx3.log("getCanvasWindowVisibility: window =" + windowNode);
    var visibility = this.jsxcanvaswinvisibility;
    if (windowNode) {
      visibility = windowNode.selectSingleNode(".//Properties").getAttribute('visibility');
      if(visibility == "true"){
        visibility = "block";
      }
      else{
       visibility = "none";
      }
    }
    jsx3.log("getCanvasWindowVisibility: visibility =" + visibility);
    return visibility;
  };

  /**
  * Sets the transparency/opacity property of a Window on this Canvas
  * @param vntTransparency {int | String} the transparency as a non-negative integer. For example: 40.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowVisibility = function(vntVisibility,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswinvisibility',vntVisibility); 
      return windowNode;
    }
    else {
      this.jsxcanvaswinvisibility = vntVisibility; 
      return this; 
    } 
  };
 
   /* Returns the transparency/opacity property of a Window on this Canvas
  * @return {int | String} jsxcanvaswintransparency/transparency.
  */
  CanvasInterface_prototype.getCanvasWindowMute = function(windowNode) {
    var imgmute = this.jsxcanvaswinmute;
    if (windowNode) {
      var mute = windowNode.selectSingleNode(".//Properties").getAttribute('mute');
      if(mute == "true"){
        imgmute = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/minus.gif');
      }
      else{
       imgmute = jsx3.resolveURI('jsxaddin://user!ipvsCommon/images/buttons/plus.gif');;
      }
    }
    return imgmute;
  };

  /**
  * Sets the transparency/opacity property of a Window on this Canvas
  * @param vntTransparency {int | String} the transparency as a non-negative integer. For example: 40.
  * @return {com.ipvs.gui.CanvasInterface} this object.
  */
  CanvasInterface_prototype.setCanvasWindowMute = function(vntMute,windowNode) {
    if( windowNode ) {
      windowNode.setAttribute('jsxcanvaswinmute',vntMute); 
      return windowNode;
    }
    else {
      this.jsxcanvaswinmute = vntMute; 
      return this; 
    } 
  };
 
  /**
  * Returns the CSS On Class of this Canvas.
  * @return {String}
  */
  CanvasInterface_prototype.getCanvasClassOn = function() { 
    return this.jsxcanvasclasson; 
  };
  
  /**
  * Sets the the CSS On Class of this Canvas.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.CanvasInterface} this object
  */
  CanvasInterface_prototype.setCanvasClassOn = function(vntClassName, bUpdateView) { 
    this.jsxcanvasclasson = vntClassName; 
    return this;  
  };  
  
  /**
  * Returns the CSS Off Class of this Canvas.
  * @return {String}
  */
  CanvasInterface_prototype.getCanvasClassOff = function() { 
    return this.jsxcanvasclassoff; 
  };
  
  /**
  * Sets the the CSS Off Class of this Canvas.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.CanvasInterface} this object
  */
  CanvasInterface_prototype.setCanvasClassOff = function(vntClassName, bUpdateView) { 
    this.jsxcanvasclassoff = vntClassName; 
    return this;  
  };  
  
   /**
  * Returns the Css of the Canvas of this panel based on the current state.
  * @return {String}
  */
  CanvasInterface_prototype.getCurrentCanvasClass = function(record) {
    var strClass = null;  
    if(!record)return;
    var strRecordId = record.getAttribute("jsxid");
    if(this.isRecordSelected(strRecordId)){
      strClass =this.getCanvasClassOn();
    } 
    else{
      strClass =this.getCanvasClassOff();
    }   
    return strClass ? strClass : this.jsxcanvasclassoff;
  };
  
    /**
  * Returns the CSS On Class of this Canvas Window based on the current state.
  * @return {String}
  */
  CanvasInterface_prototype.getCanvasWindowClassOn = function() { 
    return this.jsxcanvaswindowclasson; 
  };
  
  /**
  * Sets the the CSS On Class of this Canvas Window based on the current state.
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.CanvasInterface} this object
  */
  CanvasInterface_prototype.setCanvasWindowClassOn = function(vntClassName, bUpdateView) { 
    this.jsxcanvaswindowclasson = vntClassName; 
    return this;  
  };  
  
  /**
  * Returns the CSS Off Class of this Canvas Window based on the current state.
  * @return {String}
  */
  CanvasInterface_prototype.getCanvasWindowClassOff = function() { 
    return this.jsxcanvaswindowclassoff; 
  };
  
  /**
  * Sets the the CSS Off Class of this Canvas Window based on the current state. 
  * @param vntClassName {int | String} the classname is a string. For example:" blockselect".
  * @return {com.ipvs.gui.CanvasInterface} this object
  */
  CanvasInterface_prototype.setCanvasWindowClassOff = function(vntClassName, bUpdateView) { 
    this.jsxcanvaswindowclassoff = vntClassName; 
    return this;  
  };  
  
   /**
  * Returns the Css of the Canvas Window of this panel based on the current state.
  * @return {String}
  */
  CanvasInterface_prototype.getCurrentCanvasWindowClass = function(window) {
    var strClass = null;  
    if(!window)return;
    var strRecordId = window.getAttribute("id");
    if(this.isWindowSelected(strRecordId)){
      strClass =this.getCanvasWindowClassOn();
    } 
    else{
      strClass =this.getCanvasWindowClassOff();
    }   
    return strClass ? strClass : this.jsxcanvaswindowclassoff;
  };
  
   /**
  * Returns the Selection Model for this Canvas panel
  * @return {int}
  */
  CanvasInterface_prototype.getSelectionModel = function() {
    return (this.jsxselectionmodel != null) ? ((this.jsxselectionmodel >   CanvasInterface_prototype.SELECTION_MULTI) ?   CanvasInterface_prototype.SELECTION_UNSELECTABLE  : this.jsxselectionmodel) :CanvasInterface_prototype.SELECTION_SINGLE ;
  };
  
  /**
  * Sets the Selection Model for this Canvas panel 
  * @param intType {int}
  * @return {com.ipvs.gui.CanvasInterface} this object
  */
  CanvasInterface_prototype.setSelectionModel = function(intType) {
    this.jsxselectionmodel = intType;
    return this;
  }; 
  
    /**
  * Returns the Selection Type for this Canvas panel
  * @return {int}
  */
  CanvasInterface_prototype.getSelectionType = function() {
    return (this.jsxselectiontype != null) ? this.jsxselectiontype : CanvasInterface_prototype.SELECTION_NORMAL ;
  };
  
  /**
  * Sets the Selection Type for this Canvas panel 
  * @param intType {int}
  * @return {com.ipvs.gui.CanvasInterface} this object
  */
  CanvasInterface_prototype.setSelectionType = function(intType) {
    this.jsxselectiontype = intType;
    return this;
  }; 

    /**
  * Returns if record can be selected in this Canvas panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  CanvasInterface_prototype.isRecordSelectable = function(strRecordId) {
    var record = this.getRecord(strRecordId);
    return record && (record['jsxunselectable'] == null || record['jsxunselectable'] != "1");
  };
  
  /**
  * Returns if record is selected in this Canvas panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  CanvasInterface_prototype.isRecordSelected = function(strRecordId) {
    var record = this.getRecord(strRecordId);
    return record && record['jsxselected'] == "1";
  }; 
 
  /**
  * Deselects all selected CDF records.
  */
  CanvasInterface_prototype.deselectAllRecords = function() {
    var objCDF = this.getXML();
    var recordIterator = objCDF.selectNodeIterator(".//record[@jsxselected ='1']");
    while(recordIterator.hasNext()){
      var record = recordIterator.next();
      record.removeAttribute("jsxselected");
    }   
  };
  
  /**
  * Returns if window can be selected in this Canvas panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  CanvasInterface_prototype.isWindowSelectable = function(strRecordId,record) {
    if(record){
      var windowNode = record.selectSingleNode(".//Canvas/Window[@id ='"+strRecordId+"']");
    }
    else{
      var objCDF = this.getXML();
      var windowNode = objCDF.selectSingleNode(".//record/Canvas/Window[@id ='"+strRecordId+"']");
    }
    return windowNode && ((windowNode.getAttribute('jsxunselectable'))== null || (windowNode.getAttribute('jsxunselectable')) != "1");
  };
  
  /**
  * Returns if window is selected in this Canvas panel
  * @param strRecordId {String}
  * @return {boolean}
  */
  CanvasInterface_prototype.isWindowSelected = function(strRecordId,record) {
    if(record){
      var windowNode = record.selectSingleNode(".//Canvas/Window[@id ='"+strRecordId+"']");
    }
    else{
      var objCDF = this.getXML();
      var windowNode = objCDF.selectSingleNode(".//record/Canvas/Window[@id ='"+strRecordId+"']");
    }
    return windowNode && windowNode.getAttribute('jsxselected');
  }; 
 
  /**
  * Deselects all selected windows.
  */
  CanvasInterface_prototype.deselectAllWindows = function(record) {
    if(record){
     var canvasNode = record.selectSingleNode(".//Canvas");
     var windowIterator =canvasNode.selectNodeIterator(".//Window[@jsxselected ='1']");
    }
    else{
     var objCDF = this.getXML();
     var windowIterator = objCDF.selectNodeIterator(".//record/Canvas/Window[@jsxselected ='1']");
    }
    while(windowIterator.hasNext()){
      var windowNode = windowIterator.next();
      windowNode.removeAttribute("jsxselected");
    }   
  };

  CanvasInterface_prototype.shiftKeySelection = function(selectedFirstId, selectedLastId) {
    var records = this.getXML().selectNodeIterator("//record");
    var bselection = false;
    while (records.hasNext()) {
      var record = records.next();
      var id = record.getAttribute("jsxid");
      if (id == selectedFirstId) {
        bselection = true;
      }
      else if (id == selectedLastId) {
        record.setAttribute("jsxselected", "1");
        bselection = false;
        break;
      }
      if (bselection) {
        record.setAttribute("jsxselected", "1");
      }
    }
  };

  /**
  * Returns the <b>jsxid(s)</b> for the selected record(s). Equivalent to <code>this.getValue()</code> except that the return value is always an Array.
  * @return {Array<String>} JavaScript array of stings
  */
  CanvasInterface_prototype.getSelectedIds = function() {
    var ids = [];
    var i = this.getXML().selectNodeIterator("//record[@jsxselected='1']");
    while (i.hasNext()) {
      var record = i.next();
      ids[ids.length] = record.getAttribute("jsxid");
    }
    return ids;
  };
  
  /**
  * Called by keydown/mousedown events. Persists the state of the ctrl,alt,and shift keys
  */
  CanvasInterface_prototype._getCtrlKeys = function() {
    return (this._jsxeventclone != null) ? this._jsxeventclone : {};
  };

  /**
  * Called by keydown/mousedown events. persists the state of the ctrl,alt,and shift keys
  */
  CanvasInterface_prototype._setCtrlKeys = function(objEvent) {
    this._jsxeventclone = { ctrl: jsx3.gui.isMouseEventModKey(objEvent), shift: objEvent.shiftKey(), alt: objEvent.altKey() };
  };
   
  /*------------------------ OnClick Events ------------------------*/  

  CanvasInterface_prototype.onClick = function(objEvent, objGUI) {
    objEvent.cancelBubble();
    this._setCtrlKeys(objEvent);
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length+1);
    var record = this.getRecordNode ? this.getRecordNode(id) : null;
    var recordId = record.getAttribute("jsxid");

    if (!this.isRecordSelectable(id)) return;
    //Returns true if the record is selected
    var selected = this.isRecordSelected(id);
    if (selected) {
      if (this.getSelectionModel() == CanvasInterface_prototype.SELECTION_MULTI) {
        // Removed selection based on Ctrl or Shift keys
        if (this._getCtrlKeys().ctrl == true) {
          record.removeAttribute("jsxselected");
        }
        else if (this._getCtrlKeys().shift == true) {
          var strRecordIds = this.getSelectedIds();
          for (var i = 0; i < strRecordIds.length; i++) {
            this.deselectAllRecords();
            this.shiftKeySelection(strRecordIds[i], id);
            break;
          } 
        }
        else {
          this.deselectAllRecords();
          record.setAttribute("jsxselected", "1");
        }
      }
    }
    if(this.getSelectionModel() ==  CanvasInterface_prototype.SELECTION_SINGLE){
      this.deselectAllRecords();
    }
    if (!selected) {
       record.setAttribute("jsxselected","1");
    }
    
    var strRecordIds = this.getSelectedIds();
    if (strRecordIds.length > 1) {
      // Multiple selection based on Ctrl or Shift keys
      if (this.getSelectionModel() == CanvasInterface_prototype.SELECTION_MULTI) {
        if (this._getCtrlKeys().ctrl == false && this._getCtrlKeys().shift == false) {
          this.deselectAllRecords();
          record.setAttribute("jsxselected", "1");
        }
        else if (this._getCtrlKeys().shift == true) {
          for (var i = 0; i < strRecordIds.length; i++) {
            this.shiftKeySelection(strRecordIds[i], strRecordIds[strRecordIds.length - 1]);
            break;
          }
        }
      }
    }

    this.repaint();
    // do SELECT/EXECUTE event
    record && this.setValue(recordId);
    var event = record ? Interactive.SELECT : Interactive.EXECUTE;
    this.doEvent(event, {objEVENT:objEvent});    
  }
  
   CanvasInterface_prototype.onClick_Window = function(objEvent, objGUI) {
    objEvent.cancelBubble(); 
    if (!objEvent.leftButton() && objEvent.isMouseEvent()) return;
    // get gui obj id
    var id = objGUI.getAttribute('id').substr(this.getId().length+1);
    var objCDF = this.getXML();
    var windowNode = objCDF.selectSingleNode(".//record/Canvas/Window[@id ='"+id+"']");
    var selected = this.isWindowSelected(id);
    if(!this.isWindowSelectable(id))return;
    if(selected){
      if (this.getSelectionType() == CanvasInterface.SELECTION_NORMAL) {
        return;
      }   
      if (this.getSelectionType() == CanvasInterface.SELECTION_TOGGLE) {
        windowNode.removeAttribute("jsxselected"); 
      }      
    }
    if(this.getSelectionModel() ==  CanvasInterface_prototype.SELECTION_SINGLE){
     this.deselectAllWindows();
    }
    if (!selected) {
       windowNode.setAttribute("jsxselected","1");  
    }  
    this.repaint();
    // do SELECT/EXECUTE event
    windowNode && this.setValue(id);
    var event = windowNode ?  CanvasInterface.SelectWindow : Interactive.EXECUTE;
    this.doEvent(event, {objEVENT:objEvent});    
  }

});