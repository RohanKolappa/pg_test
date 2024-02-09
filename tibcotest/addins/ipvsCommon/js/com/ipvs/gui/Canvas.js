jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.CanvasInterface");

jsx3.Class.defineClass("com.ipvs.gui.Canvas", jsx3.gui.Template.Block, [jsx3.gui.Form,jsx3.xml.Cacheable,jsx3.xml.CDF,com.ipvs.gui.CanvasInterface], function(Canvas, Canvas_prototype) {

  /** @package */
  Canvas.DEFAULT_WIDTH = 210;
  Canvas.DEFAULT_HEIGHT = 180;
  
  // Selection Model for button panel
  Canvas.SELECTION_UNSELECTABLE = 0;
  Canvas.SELECTION_SINGLE = 1;
  Canvas.SELECTION_MULTI = 2;

 /**
   * The instance initializer.
   *
   * @param strName {String}  the JSX name
   * @param vntLeft {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntTop {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntWidth {int}
   * @param vntHeight {int}
   */
  Canvas_prototype.init = function(strName, vntLeft, vntTop, vntWidth, vntHeight) {
     jsx3.log("Canvas_prototype:init");
    this.jsxsuper(strName, vntLeft, vntTop, vntWidth, vntHeight);
  };
  
  /**
  * Get an iterator for the windows that will be painted
  * Returns all the windows in the given canvas. 
  * @return {jsx3.util.Iterator}
  */
  Canvas_prototype.getIterableWindows = function() {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectSingleNode(".//record/Canvas").selectNodeIterator(".//Window"):
    (new jsx3.util.List()).iterator();    
  };


 /**
  * Selects the window with given id
  * @param strRECORDID {string}. hte id of the window to be selected and processed.
  */
  Canvas_prototype.selectWindow = function(strRECORDID) {
    var objCDF = this.getXML();
    var windowNode = objCDF.selectSingleNode(".//record/Canvas/Window[@id ='"+strRECORDID+"']");
    if(!this.isWindowSelectable(strRECORDID))return;
    if(this.isWindowSelected(strRECORDID))return;
    if(this.getSelectionModel() == Canvas.SELECTION_SINGLE){
      this.deselectAllWindows();
    }
    if (!this.isWindowSelected(strRECORDID)) {
       windowNode.setAttribute("jsxselected","1");    
    }  
    this.repaint();
  };


 /**
  * The XML Template used by the Template class to render this Canvas Block
  *
  * @return {String}
  */
  Canvas_prototype.getTemplateXML = function() {
    jsx3.log("getTemplateXML");
    var returnVal = ['',
      '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
      ' <model>',
      ' </model>',
      ' <template dom="static" recalc="true">',
      '   <inlinebox u:id="canvasBlock" style="position:{$position};left:{$left};top:{$top};width:{$width};height:{$height};margin:{$margin};">',
      '   <for-each select="this.getIterableRecords()">',
      '     <var id="canvasid">this.getId()</var>',
      '     <var id="recordtext">this.getText($$target)</var>',
      '     <var id="canvas_target">$$target.selectSingleNode(".//Canvas")</var>',
      '     <var id="blkwidth">this.getCanvasBlkWidth(canvas_target)</var>', 
      '     <var id="blkheight">this.getCanvasBlkHeight(canvas_target)</var>',
      '     <var id="canvaswidth">this.getCanvasWidth(canvas_target)+ \'px\'</var>', 
      '     <var id="canvasheight">this.getCanvasHeight(canvas_target)+ \'px\'</var>',
      '     <var id="txtcanvasheight">this.getCanvasBlkHeight()-18</var>',
      '     <div id ="{canvasid}" style="position:relative;float:left;width:{blkwidth};height:{blkheight};margin: 8px 8px 8px 8px" class="canvasOuter" >',
      '       <inlinebox  u:\protected="true" style="overflow:hidden;position:absolute;left:4px;top:4px;width:{canvaswidth};height:{canvasheight};" class="canvasInner" >',
      '        <for-each select="this.getIterableWindows()">',
                 this.getWindowTemplateXML(), 
      '        </for-each>', 
      '       </inlinebox>',    
//      '       <span style="position:absolute;left:4px;top:{txtcanvasheight};font-size:12px;color:#555;">',
//      '         <text>{recordtext}</text>',
//      '       </span>',
      '     </div>',
      '   </for-each>', 
      '   </inlinebox>',
      ' </template>',
      '</transform>'].join("");
    // form template string
    return returnVal;
  };

  /**
  * The XML Template used by the Template class to render this Window Block in each Canvas
  *
  * @return {String}
  */
  Canvas_prototype.getWindowTemplateXML = function() {
   jsx3.log("getwinTemplate");
    var returnVal = ['',
      '<var id="canvas">this.getXML().selectSingleNode(".//record/Canvas")</var>',
      '<var id="window_target">$$target</var>',     
      '<var id="windowid">this.getId()+ \'_\'+ $$target.getAttribute(\'id\')</var>',
      '<var id="windowtext">$$target.getAttribute(\'id\')</var>',
      '<var id="winwidth">this.getCanvasWindowWidth($$target,canvas)+ \'px\'</var>', 
      '<var id="winheight">this.getCanvasWindowHeight($$target,canvas)+ \'px\'</var>',
      '<var id="winleft">this.getCanvasWindowLeft($$target,canvas)+ \'px\'</var>', 
      '<var id="wintop">this.getCanvasWindowTop($$target,canvas)+ \'px\'</var>',
      '<var id="winzindex">this.getCanvasWindowZIndex($$target)</var>',
//      '<var id="winvisibility">this.getCanvasWindowVisibility($$target)</var>',
//      '<var id="winmute">this.getCanvasWindowMute($$target)</var>',
      '<var id="winclass">this.getCurrentCanvasWindowClass($$target)</var>', 
      '<var id="wintransparency">\'alpha(opacity=\'+ this.getCanvasWindowTransparency($$target)+ \')\'</var>', 
//      '<inlinebox u:\protected="true" id="{windowid}" style="position:absolute;width:{winwidth};height:{winheight};left:{winleft};top:{wintop};opacity:0.4;filter:{wintransparency};z-index:{winzindex};overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis" class="{winclass}" onclick="{onClick_Window}">',
      '<inlinebox u:\protected="true" id="{windowid}" style="position:absolute;width:{winwidth};height:{winheight};left:{winleft};top:{wintop};z-index:{winzindex};overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis" class="{winclass}" onclick="{onClick_Window}">',
      '<text>{windowtext}</text>',
//      '<div u:id="mute" style="position:relative;height:10px;" >',
//      '<img src="{winmute}" width="10" height="10"/>',
//      '</div>',
      '</inlinebox>'].join("");   
        // form template string
        return returnVal;
  };

});