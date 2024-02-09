jsx3.require("jsx3.gui.Template","jsx3.gui.Form","jsx3.xml.Cacheable","jsx3.xml.CDF","com.ipvs.gui.CanvasInterface");

jsx3.Class.defineClass('com.ipvs.gui.CanvasPanel', jsx3.gui.Template.Block, [jsx3.gui.Form,jsx3.xml.Cacheable,jsx3.xml.CDF,com.ipvs.gui.CanvasInterface], function(CanvasPanel, CanvasPanel_prototype) {

  /** @package */
  CanvasPanel.DEFAULT_WIDTH = 300;
  CanvasPanel.DEFAULT_HEIGHT = 18;
  
   /**
   * The instance initializer.
   *
   * @param strName {String}  the JSX name
   * @param vntLeft {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntTop {int|String} either a number (i.e, 12, 30, etc) or a number and a unit value (i.e., "25%", "36pt", etc); if a number is passed, pixels will be the assumed unit when painted to screen
   * @param vntWidth {int}
   * @param vntHeight {int}
   */
  CanvasPanel_prototype.init = function(strName, vntLeft, vntTop, vntWidth, vntHeight) {
    this.jsxsuper(strName, vntLeft, vntTop, vntWidth, vntHeight);
  };

  /**
  * Get an iterator for the canvas that will be painted
  * Returns all the canvases in the given record.
  * @return {jsx3.util.Iterator}
  */
  CanvasPanel_prototype.getIterableCanvas = function() {
    var objCDF = this.getXML();
    return objCDF ? objCDF.selectNodeIterator(".//record/Canvas"):
                  (new jsx3.util.List()).iterator();    
  };

  /**
  * Get an iterator for the windows that will be painted
  * Returns all the windows in the given canvas. 
  *
  * @return {jsx3.util.Iterator}
  */
  CanvasPanel_prototype.getIterableWindows = function(canvas) {
    jsx3.log("getIterableWindows: canvas = " + canvas);
    return canvas ? canvas.selectNodeIterator(".//Window") : 
                    (new jsx3.util.List()).iterator();
  };
  

  /**
  * The XML Template used by the Template class to render this Canvas Panel
  *
  * @return {String}
  */
  CanvasPanel_prototype.getTemplateXML = function() {
    jsx3.log("getTemplateXML");
    var returnVal = ['',
      '<transform xmlns="http://gi.tibco.com/transform/" xmlns:u="http://gi.tibco.com/transform/user" version="1.0">',
      '  <model>',
      '  </model>',
      '  <template dom="static" recalc="true">',
      '    <inlinebox u:id="canvaspanel" style="position:{$position};left:{$left};top:{$top};width:{$width};height:{$height};margin:{$margin};">',
    //  panel
      '      <inlinebox style="position:absolute;left:0px;top:0px;width:100%;height:100%;overflow:auto;">',
      '        <for-each select="this.getIterableRecords()">',
                  this.getCanvasTemplateXML(),
      '        </for-each>',
      '      </inlinebox>',
      '    </inlinebox>',
      '  </template>',
      '</transform>'].join("");
   jsx3.log("getTemplateXML:FINALHTML:"+returnVal);
    // form template string
    return returnVal;
  };

  /**
  * The XML Template used by the Template class to render this Canvas Block in each Panel
  *
  * @return {String}
  */
 CanvasPanel_prototype.getCanvasTemplateXML = function() {
    var returnVal = ['',
      '<var id="record_target">$$target</var>', 
      '<var id="canvas_target">record_target.selectSingleNode(".//Canvas")</var>', 
      '<var id="recordid">this.getId()+ \'_\'+record_target.getAttribute(\'jsxid\')</var>',
      '<var id="recordtext">this.getText(record_target)</var>',
      '<var id="canvasclass">this.getCurrentCanvasClass(record_target)</var>',  
      '<var id="canvaswidth">this.getCanvasWidth(canvas_target)+ \'px\'</var>', 
      '<var id="canvasheight">this.getCanvasHeight(canvas_target)+ \'px\'</var>', 
      '<inlinebox u:\protected="true" id="{recordid}" style="position:relative;float:left;width:',this.getCanvasBlkWidth(),'px;height:',this.getCanvasBlkHeight(),'px;margin: 8px 8px;" class="{canvasclass}" onclick="{onClick}">',
      '   <inlinebox  u:\protected="true" style="overflow:hidden;position:absolute;left:4px;top:4px;width:{canvaswidth};height:{canvasheight};" class="canvasInner" >',
      '        <for-each select="this.getIterableWindows(canvas_target)">',
      '           <var id="window_target">$$target</var>',     
      '           <var id="windowid">window_target.getAttribute(\'id\')</var>',
      '           <var id="windowtext">window_target.getAttribute(\'id\')</var>',
      '           <var id="winwidth">this.getCanvasWindowWidth(window_target,canvas_target)+ \'px\'</var>', 
      '           <var id="winheight">this.getCanvasWindowHeight(window_target,canvas_target)+ \'px\'</var>',
      '           <var id="winleft">this.getCanvasWindowLeft(window_target,canvas_target)+ \'px\'</var>', 
      '           <var id="wintop">this.getCanvasWindowTop(window_target,canvas_target)+ \'px\'</var>',
      '           <var id="winzindex">this.getCanvasWindowZIndex(window_target)</var>',
      '           <var id="wintransparency">\'alpha(opacity=\'+ this.getCanvasWindowTransparency(window_target)+ \')\'</var>',
//      '           <inlinebox u:\protected="true" id="{windowid}" style="position:absolute;width:{winwidth};height:{winheight};left:{winleft};top:{wintop};opacity:0.4;filter:{wintransparency};z-index:{winzindex};overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis" class="window">',
      '           <inlinebox u:\protected="true" id="{windowid}" style="position:absolute;width:{winwidth};height:{winheight};left:{winleft};top:{wintop};z-index:{winzindex};overflow:hidden;font-weight:normal;white-space:nowrap;text-overflow:ellipsis" class="window">',
      '             <text>{windowtext}</text>',
      '           </inlinebox>',
      '        </for-each>', 
      '   </inlinebox>',    
      '    <span style="position:absolute;left:4px;top:',this.getCanvasBlkHeight()-18,'px;font-size:12px;color:#555;">',
      '     <text>{recordtext}</text>',
      '   </span>',
      ' </inlinebox>'].join("");   
    // form template string
    return returnVal;
  };

}); 























































