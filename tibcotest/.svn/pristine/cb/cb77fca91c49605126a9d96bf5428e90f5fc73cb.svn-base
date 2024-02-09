//a patch function for MediaPlayerIE.setFps
/*
patch_MediaPlayerIE_setFps = function (aop,Fps) {
	if (this.IPVSPlayerAvailability) {
		if(!Fps) Fps = null;
        this._LOG.info("Player SetFps: uniqueID: " + this.IPVideoPlayer.uniqueID + ": Fps: " + Fps);
        this.IPVideoPlayer.Fps = Fps;
    }
    else {
      this._LOG.info("Player not loaded.");
      return;
    }
}

 */

  /**
  * hot patch function to get player size log; using setFps to trigger
  * other candidate target functions can be: 
  * setScaleToWindow,setNativeSize,getNativeSize,toggleDisplay,	remoteKBMSState	,
  * mute, skipForward,catchUp,skipBackward,resume,pause,stop,start,load,destroy
  *
  */
  getPlayerSize= function() {
    if (this.IPVideoPlayer) {
	  this._LOG.warn("AOP LOG:: Player uniqueID= "+this.IPVideoPlayer.uniqueID+":jsxid= "+this._PlayerID);
      var playerSizeObj = new Object();
      //get the outer DIV size to actual video value
      var height = this.BlkJSX.getHeight();		
	  var width = this.BlkJSX.getWidth();
	  this._LOG.warn("AOP LOG:: this.BlkJSX.getHeight()="+width+"::this.BlkJSX.getWidth()="+height);
  	 
  	  var height3 = this.BlkJSX.getRendered().offsetHeight;		
	  var width3 = this.BlkJSX.getRendered().offsetWidth;	
	  this._LOG.warn("AOP LOG:: this.BlkJSX.getRendered().offsetWidth="+width3+"::this.BlkJSX.getRendered().offsetHeigh="+height3);

    }
	
  }

  /*
  *	patch function to track caller function of jsx3.gui.Block.paint()
  * need set 'tgtmethod' name="paint" in aopconfig.xml
  */
  getBlockPaintCallerFnc= function() {
	if(this.getName()=="blkVideoBlock") {	//this.getName()=="blkVideoBlockContainer"||this.getName()=="videoObjectContainer"
		var callerFunc = arguments.callee.caller.toString();
		callerFuncName = (callerFunc.substring(callerFunc.indexOf("function") + 8, callerFunc.indexOf("(")));
		jsx3.util.Logger.getLogger("jsx3.gui.Block").warn(" --------------- AOP Block.paint() caller function log -------------------");
		jsx3.util.Logger.getLogger("jsx3.gui.Painted").warn("AOP LOF::this.getName()= "+this.getName()+":: caller fnc name: " + callerFuncName +"::function:: "+callerFunc);
	}
  }

  /*
  *	patch function to track caller function of jsx3.gui.Block.repaint()
  * need set 'tgtmethod' name="repaint" in aopconfig.xml
  */
  getBlockRepaintCallerFnc= function() {
	var callerFunc = arguments.callee.caller.toString();
	callerFuncName = (callerFunc.substring(callerFunc.indexOf("function") + 8, callerFunc.indexOf("(")));
	jsx3.util.Logger.getLogger("jsx3.gui.Block").warn(" --------------- AOP Block.repaint() caller function log -------------------");
	jsx3.util.Logger.getLogger("jsx3.gui.Painted").warn("AOP LOG::this.getName()= "+this.getName()+":: caller fnc name: " + callerFuncName +"::function:: "+callerFunc);
  }