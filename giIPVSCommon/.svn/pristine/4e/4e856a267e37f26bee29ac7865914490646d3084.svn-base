/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This is MediaPlayer class
*/


jsx3.lang.Class.defineClass("com.ipvs.gui.MediaPlayer", null, null, function (MediaPlayer, MediaPlayer_prototype) {
    
    // create Logger for this class
    MediaPlayer_prototype._LOG = jsx3.util.Logger.getLogger(MediaPlayer.jsxclass.getName());

    //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ properties ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
    /**
     * Static constant varibles for player modes
     */
     MediaPlayer.WINDOWLESS_MODE_PLAYER = 1;
     MediaPlayer.WINDOW_MODE_PLAYER = 0;

     /**
     * {ActiveX object} media player state 
     */
     MediaPlayer.LOWEST_FPS = 1;
     MediaPlayer.DEFAULT_FPS = 15;
     MediaPlayer.IPVSPlayerState_PAUSED = 1;
     MediaPlayer.IPVSPlayerState_PLAYING = 2;

     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ setter, getter ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     /**
     * Sets the transparency/opacity property of a player Window
     * @param blkTransparency {int | String} the transparency as a non-negative integer.
     */
     MediaPlayer_prototype.setBlkTransparency = function(blkTransparency) {
       this._blkTransparency = blkTransparency;
     };

     /** 
     * Returns the transparency/opacity property of a Window on this Canvas
     * @return {int | String} _blkTransparency.
     */
     MediaPlayer_prototype.getBlkTransparency = function() {
       return this._blkTransparency;
     };

     /**
     * Method to set media player type
     * @param value {int} 0:normal,1:windowless; Default value 1.
     */
     MediaPlayer_prototype.setPlayerType = function(value) {
       this._PlayerType = value;
     }

     /**
     * Method to get media player type
     * @return {int}
     */
     MediaPlayer_prototype.getPlayerType = function() {
       return this._PlayerType;
     }

     /**
     * Method playerSetFocus  
     */
     MediaPlayer_prototype.setFocus = function() {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player setFocus: uniqueID: " + this.IPVideoPlayer.uniqueID);
         this.IPVideoPlayer.focus();
       }
       else {
         this._LOG.warn("setFocus:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerSetFps  
     * @param Fps(frame per second) {int} 
     */
     MediaPlayer_prototype.setFps = function(Fps) {
       if (this.IPVideoPlayer) {
         if (this.IPVideoPlayer.PlayState == MediaPlayer.IPVSPlayerState_PLAYING) {
           this._LOG.info("Player SetFps: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID + ": Fps: " + Fps);
           this.IPVideoPlayer.Fps = Fps;
         }
       }
       else {
         this._LOG.warn("setFps:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerSetPriority:set player priority [true|false]:[normal|low] 
     * @param priority {boolean} 
     */
     MediaPlayer_prototype.setPriority = function(priority) {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player setPriority: uniqueID: " + this.IPVideoPlayer.uniqueID + ": Priority: " + priority);
         this.IPVideoPlayer.Priority = priority;
       }
       else {
         this._LOG.warn("setPriority:Player not loaded.");
         return;
       }
     }

     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ player functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     /**
     * Method player start
     * @param StreamURL {String} 
     */
     MediaPlayer_prototype.start = function(StreamURL) {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player Start: uniqueID: " + this.IPVideoPlayer.uniqueID + ": StreamURL: " + StreamURL + " :playerId:" + this._PlayerID);
         // UseHTTP
         var useHTTP = this.clsApp.clsPreferences.getLocalPreferences("UseHTTP", 0);
         var useMMS = 0;
         // Parse Options 
         // ??? - should we be getting all this as part of URL or should some of this come from Profile?
         if (StreamURL.match(/^v2d/)) {
           // Host
           if (useHTTP == 1) {
             useMMS = this.clsApp.clsPreferences.getLocalPreferences("UseMMS", 0);
           }
           var StreamURLHostPart = StreamURL.replace(/\/[^\/]*$/, "");
           if(useMMS == 1) {
             var mmsIPAddress = this.clsApp.clsServiceAgent.getLoginParam(com.ipvs.agent.ServiceAgent.LOGIN_SERVER);
             StreamURLHostPart = "v2d://" + mmsIPAddress;
           }

           // Port
           var ServerPort = this.clsApp.clsPreferences.getLocalPreferences("ServerPort", 6060);
           if (StreamURL.match(/port=/)) {
             ServerPort = parseInt(StreamURL.replace(/^.*port=(\d+).*$/, "$1"));
           }

           // V2D set values on player ctrl
           this.IPVideoPlayer.Filename = StreamURLHostPart;
           this.IPVideoPlayer.ServerPort = ServerPort;

           //V2D authentication (?authscheme=TOKEN?authdata=47f10529-f6b7-44ee-9011-c08b942122e2")
           if (StreamURL.match(/authscheme=/) && StreamURL.match(/authdata=/)) {
             var authScheme = StreamURL.replace(/^.*authscheme=(\w+).*$/, "$1");
             if (authScheme == "TOKEN") {
               var authdata = StreamURL.replace(/^.*authdata=(\S+).*$/, "$1");
               this.IPVideoPlayer.SetAuthParameters(authScheme, authdata)
             }
           }
         }
         else if (StreamURL.match(/^http/)) {
           this.IPVideoPlayer.Filename = StreamURL;
         }
         else if (StreamURL.match(/^udp/) || StreamURL.match(/^igmp/)) {
           this.IPVideoPlayer.Filename = StreamURL.replace("://", "://@");
         }
         //common player parameters used for all types of StreamURL
         // Bandwidth
         var BandwidthKbps = this.clsApp.clsPreferences.getLocalPreferences("BandwidthKbps", 10240);
         if (StreamURL.match(/bandwidth=/)) {
           var Bandwidthbps = parseInt(StreamURL.replace(/^.*bandwidth=(\d+).*$/, "$1"));
           //"bandwidth in URL is in bps - need to convert to Kbps for player API"
           BandwidthKbps = Bandwidthbps / 1024;
         }
         // AVOption
         var AVOption = this.clsApp.clsPreferences.getLocalPreferences("AVOption", 3);
         if (StreamURL.match(/avoption=/)) {
           AVOption = StreamURL.replace(/^.*avoption=(\w+).*$/, "$1");
           switch (AVOption) {
             case "Video":
               AVOption = 1;
               break;
             case "Audio":
               AVOption = 2;
               break;
             case "Both":
               AVOption = 3;
               break;
             default:
               AVOption = 3;
               break;
           }
         }
         // Fps
         var Fps = this.clsApp.clsPreferences.getLocalPreferences("Fps", MediaPlayer.DEFAULT_FPS);
         if (StreamURL.match(/fps=/)) {
           Fps = StreamURL.replace(/^.*fps=(\d+).*$/, "$1");
         }
         // Multicast
         var Multicast = this.clsApp.clsPreferences.getLocalPreferences("Multicast", 0);
         if (StreamURL.match(/multicast=/)) {
           Multicast = StreamURL.replace(/^.*multicast=(\w+).*$/, "$1");
           switch (Multicast) {
             case "false":
               Multicast = 0;
               break;
             case "true":
               Multicast = 1;
               break;
             default:
               Multicast = 0;
               break;
           }
         }
         if (Multicast == 0) {
           this.IPVideoPlayer.useHTTP = useHTTP;
         } else {
           this.IPVideoPlayer.useHTTP = 0;
         }
         // SRD
         var SliceReduction = this.clsApp.clsPreferences.getLocalPreferences("SliceReduction", 1);
         // Enable Kbm
         var EnableKbm = this.clsApp.clsPreferences.getLocalPreferences("EnableKbm", 1);
         if (StreamURL.match(/enablekbm=/)) {
           EnableKbm = StreamURL.replace(/^.*enablekbm=(\w+).*$/, "$1");
           switch (EnableKbm) {
             case "false":
               EnableKbm = 0;
               break;
             case "true":
               EnableKbm = 1;
               break;
             default:
               EnableKbm = 0;
               break;
           }
         }
         
         // Local KBMS State
         var LocalKBMSState = this.clsApp.clsPreferences.getLocalPreferences("LocalKBMSState", 0);
         this.IPVideoPlayer.MaxBandwidth = BandwidthKbps;
         this.IPVideoPlayer.AVMode = AVOption;
         this.IPVideoPlayer.Fps = Fps;
         this.IPVideoPlayer.Multicast = Multicast;
         // Optional  
         this.IPVideoPlayer.SliceReduction = SliceReduction;
         // not used by activeX control
         this.IPVideoPlayer.EnableKbm = EnableKbm;
         this.IPVideoPlayer.LocalKBMSState = LocalKBMSState;

         //set toolbar to preserve in togglescreen::
         this.IPVideoPlayer.Settings = "<ParameterList><toolbar>Preserve</toolbar><doubleclick>Enable</doubleclick></ParameterList>"; //Disable

         // palyer priority
         var priority = this.clsApp.clsPreferences.getLocalPreferences("Priority", "false");
         this._LOG.info("Player play with priority: " + priority + " :uniqueID: " + this.IPVideoPlayer.uniqueID + ": fps: " + Fps);
         priority = (priority == "true") ? 1 : 0;
         this.IPVideoPlayer.Priority = priority;
         // start playback
         this.IPVideoPlayer.Play();
         //stroe run time fps to a instance varible
         if (!this.IPVideoPlayerRunTimeFPS) this.IPVideoPlayerRunTimeFPS = Fps;
         
       }
       else {
         this._LOG.warn("start:Player not loaded.");
         return;
       }
     }

     /**
     * Method player stop
     */
     MediaPlayer_prototype.stop = function playerStop() {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player Stop: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
         this.IPVideoPlayer.Stop();
       }
       else {
         this._LOG.info("stop:Player not loaded.");
         return;
       }
     }

     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ player PVR functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~
     /**
     * Method player pause
     */
     MediaPlayer_prototype.pause = function() {
       if (this.IPVideoPlayer) {
         if (this.IPVideoPlayer.PlayState != MediaPlayer.IPVSPlayerState_PAUSED) {
           this._LOG.info("Player pause: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
           this.IPVideoPlayer.Pause();
         }
       }
       else {
         this._LOG.warn("pause:Player not loaded.");
         return;
       }
     }

     /**
     * Method player resume
     */
     MediaPlayer_prototype.resume = function() {
       if (this.IPVideoPlayer) {
         //from bookmark playFrom
         if (this.IPVideoPlayer.PlayState == MediaPlayer.IPVSPlayerState_PLAYING) {
           this.IPVideoPlayer.Pause();
           this.IPVideoPlayer.Resume();
         }
         if (this.IPVideoPlayer.PlayState == MediaPlayer.IPVSPlayerState_PAUSED) {
           this.IPVideoPlayer.Resume();
         }
         this._LOG.info("Player resume: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
       }
       else {
         this._LOG.warn("resume:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerSkipBackward
     * @param backwardSeconds {int} (millisecond)
     */
     MediaPlayer_prototype.skipBackward = function(backwardSeconds) {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player skipBackward: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
         this.IPVideoPlayer.SkipBack(backwardSeconds);
       }
       else {
         this._LOG.warn("skipBackward:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerSkipForward
     * @param forwardSeconds {int} (millisecond)
     */
     MediaPlayer_prototype.skipForward = function(forwardSeconds) {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player skipForward: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
         this.IPVideoPlayer.SkipFwd(forwardSeconds);
       }
       else {
         this._LOG.warn("skipForward:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerCatchUp  
     * (RTP requires this for synchronizing clock with streming server,
     *  V2D has no timestamp, so it's simple)
     */
     MediaPlayer_prototype.catchUp = function() {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player catchup: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
         this.IPVideoPlayer.CatchUp();
       }
       else {
         this._LOG.warn("catchup:Player not loaded.");
         return;
       }
     }
    
     /**
     * Method playerMute to set Player mute ON or OFF
     * @param muteType {int} 
     */
     MediaPlayer_prototype.mute = function(muteType) {
       if (this.IPVideoPlayer) {
         this.IPVideoPlayer.Mute = muteType;
       }
       else {
         this._LOG.warn("mute:Player not loaded.");
         return;
       }
     }

     /**
     * Method restoreFPS: restore FPS to  
     */
     MediaPlayer_prototype.restoreFPS = function() {
       if (!this.IPVideoPlayer) return false;
       this.IPVideoPlayer.Fps = this.IPVideoPlayerRunTimeFPS;
       return true;
     }


     //~~~~~~~~~~~~~~~~~~~~~~~~ player windowing control functions ~~~~~~~~~~~~~~~~~~~~~~
     /**
     * Method playerSetLocalKBMSState  
     * @param lKBMSState {int} 
     */
     MediaPlayer_prototype.setLocalKBMSState = function(lKBMSState) {
       if (this.IPVideoPlayer) {
         this._LOG.info("Player setLocalKBMSState:lKBMSState:" + lKBMSState + " uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
         this.IPVideoPlayer.LocalKBMSState = lKBMSState;
       }
       else {
         this._LOG.warn("setLocalKBMSState:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerGetKBMSState to check KBMSState is success or not
     */
     MediaPlayer_prototype.getKBMSState = function() {
       if (this.IPVideoPlayer) {
         return this.IPVideoPlayer.IsKBMSSuccess;
       }
       else {
         this._LOG.warn("getKBMSState:Player not loaded.");
         return 0;
       }
     }

     /**
     * Method playerSetNativeSize 
     */
     MediaPlayer_prototype.setNativeSize = function() {
       if (this.IPVideoPlayer) {
         this.getNativeSize();
         this._LOG.info("setNativeSize:Player(" + this.IPVideoPlayer.uniqueID + "):native width: " + this.nativeSize.width + ":nativeSize.height: " + this.nativeSize.height);
         if (this.nativeSize.width <= 0 || this.nativeSize.height <= 0) {
           //Fix bug 30225. Not able to come out of full screen mode for blank video player;
           //Analysis: The player is going to "0" size in this case. Before going to 1:1 mode,
           //validate the nativeSize.width and height. If any of two <= '0', then do not resize the container.
           return;
         }
         //set the outer DIV size to actual video value
         this.IPVideoPlayer.style.width = this.nativeSize.width;
         this.IPVideoPlayer.style.height = this.nativeSize.height;
         //      this.IPVideoPlayer.ShowNativeVideoResolution();
       }
       else {
         this._LOG.warn("setNativeSize:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerGetNativeSize 
     * @return {Object}
     */
     MediaPlayer_prototype.getNativeSize = function() {
       if (this.IPVideoPlayer) {
         this.nativeSize.width = this.IPVideoPlayer.GetNativeWidth();
         this.nativeSize.height = this.IPVideoPlayer.GetNativeHeight();
         if (isNaN(this.nativeSize.width) || (this.nativeSize.width <= 0) || (this.nativeSize.width == '')) {
           this.nativeSize.width = 0;
         }
         if (isNaN(this.nativeSize.height) || (this.nativeSize.height <= 0) || (this.nativeSize.height == '')) {
           this.nativeSize.height = 0;
         }
         return this.nativeSize;
       }
       else {
         this._LOG.warn("getNativeSize:Player not loaded.");
         return null;
       }
     }

     /**
     * Method playerToggleDisplay 
     * @return {boolean}
     */
     MediaPlayer_prototype.toggleDisplay = function() {
       if (this.IPVideoPlayer) {
         try {
           this._LOG.info("Player toggleDisplay: uniqueID: " + this.IPVideoPlayer.uniqueID + " :playerId:" + this._PlayerID);
           this.IPVideoPlayer.ToggleDisplay();
         }
         catch (er) {
           return false;
         }
         return true;
       }
       else {
         this._LOG.warn("toggleDisplay:Player not loaded.");
         return false;
       }
     }

     /**
     * Method playerSetScaleToZoom 
     * @param zoomSize {int}
     */
     MediaPlayer_prototype.setScaleToZoom = function (zoomSize) {
       if (this.IPVideoPlayer) {
         this.getNativeSize();
         this._LOG.info("setScaleToZoom:Player(" + this.IPVideoPlayer.uniqueID + "):native width: " + this.nativeSize.width + ":nativeSize.height: " + this.nativeSize.height);
         if (this.nativeSize.width <= 0 || this.nativeSize.height <= 0) {
           return;
         }
         if (jsx3.util.strEmpty(zoomSize)) zoomSize = 100;
         //set the outer DIV size to actual video value
         this.IPVideoPlayer.style.width = (this.nativeSize.width * zoomSize) / 100;
         this.IPVideoPlayer.style.height = (this.nativeSize.height * zoomSize) / 100;
         // set player 'border'
         this.IPVideoPlayer.style.border = this.clsApp.clsPreferences.getLocalPreferences("Border", 0);
       }
       else {
         this._LOG.warn("setScaleToZoom:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerSetScaleToWindow 
     */
     MediaPlayer_prototype.setScaleToWindow = function() {
       if (this.IPVideoPlayer) {
         this._LOG.info("setScaleToWindow:Player(" + this.IPVideoPlayer.uniqueID + ")");
         //set the outer DIV size to actual video value
         this.IPVideoPlayer.style.width = "100%";
         this.IPVideoPlayer.style.height = "100%";
         // set player 'border'
         this.IPVideoPlayer.style.border = this.clsApp.clsPreferences.getLocalPreferences("Border", 0);
       }
       else {
         this._LOG.warn("setScaleToWindow:Player not loaded.");
         return;
       }
     }

     /**
     * Method playerSnapShot 
     * @return {boolean}
     */
     MediaPlayer_prototype.snapShot = function(path) {
       if (this.IPVideoPlayer) {
         try {
           this._LOG.info("Player snapShot: uniqueID: " + this.IPVideoPlayer.uniqueID);
           this.IPVideoPlayer.Snapshot(path, "");
         }
         catch (er) {
           return false;
         }
         return true;
       }
       else {
         this._LOG.warn("snapShot:Player not loaded.");
         return false;
       }
     }

     /**
     * zoom:Change player zoom by given dimentions:width,height;
     * @param objPlayerCmd {JSON}
     * @return {boolean} true:success;false:fail
     */
     MediaPlayer_prototype.zoom = function(currentZoomX, currentZoomY) {
       if (!this.IPVideoPlayer) { return false; }
       //get s pairs(2 dimensions) of zoom settings
       var lastZoomX = this.blkPlayer.getWidth();
       var lastZoomY = this.blkPlayer.getHeight();
       //dimensions before zooming
       var blkWinHtmlElement = this.getRendered();
       var blkWinWidth = blkWinHtmlElement.clientWidth;
       var blkWinHeight = blkWinHtmlElement.clientHeight;
       var blkWinScrollLeft = blkWinHtmlElement.scrollLeft;
       var blkWinScrollTop = blkWinHtmlElement.scrollTop;
       // calulate current center offset
       var offX = parseInt(blkWinScrollLeft) + (blkWinWidth / 2);
       var offY = parseInt(blkWinScrollTop) + (blkWinHeight / 2);
       // calulate new scrollLeft/scrollTop
       blkWinScrollLeft = (offX * parseInt(currentZoomX) / parseInt(lastZoomX)) - (blkWinWidth / 2);
       blkWinScrollTop = (offY * parseInt(currentZoomY) / parseInt(lastZoomY)) - (blkWinHeight / 2);
       this.blkPlayer.setWidth(currentZoomX, true);
       this.blkPlayer.setHeight(currentZoomY, true);
       blkWinHtmlElement.scrollLeft = blkWinScrollLeft;
       blkWinHtmlElement.scrollTop = blkWinScrollTop;
       return { zoomX: currentZoomX, zoomY: currentZoomY, scrollLeft: blkWinScrollLeft, scrollTop: blkWinScrollTop };
     }

     /**
     * pan:Change player zoom by given dimentions:width,height;
     * @param objPlayerCmd {JSON}
     * @return {boolean} true:success;false:fail
     */
     MediaPlayer_prototype.pan = function(blkWinScrollLeft, blkWinScrollTop) {
       if (!this.IPVideoPlayer) { return false; }
       //dimensions before zooming
       var blkWinHtmlElement = this.getRendered();
       blkWinHtmlElement.scrollLeft = blkWinScrollLeft;
       blkWinHtmlElement.scrollTop = blkWinScrollTop;
       return true;
     }

     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~ player call back functions ~~~~~~~~~~~~~~~~~~~~~~~~~~
     /**
     * Method ChangeDisplayMode 
     * @return {boolean}
     */
     MediaPlayer_prototype.changeDisplayMode = function() {
       if (this.IPVideoPlayer) {
         return true;
       }
       else {
         this._LOG.warn("changeDisplayMode:Player not loaded.");
         return false;
       }
     }
     /**
     * Instance call back function doOnNativeSizeChange(bOnNativeSizeWidth)
     * @param lWidth {int}
     * @param lHeight {int}
     */
     MediaPlayer_prototype.doOnNativeSizeChange = function(lWidth, lHeight) {
       // update the displayconfig with native size info
       this.clsApp.clsDisplayAgent.setDisplayConfigWinProp(this.winId, "ConnectionContent", "hres", lWidth);
       this.clsApp.clsDisplayAgent.setDisplayConfigWinProp(this.winId, "ConnectionContent", "vres", lHeight);
       // log the event
       this._LOG.debug("::doOnNativeSizeChange:winId=" + this.winId + ":hres=" + lWidth + ":vres=" + lHeight);
       // publish window event
       this.publish({ subject: com.ipvs.gui.MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + this.winId, context: { objNATIVEMODE: { hres: lWidth, vres: lHeight}} });
     }

     /**
     * Instance call back function doFullScreen(bFullScreen)
     * @param bFullScreen {boolean}
     */
     MediaPlayer_prototype.doFullScreen = function(bFullScreen) {
       // log the event
       this._LOG.debug("::doFullScreen:winId=" + this.winId + ":bFullScreen=" + bFullScreen);
       // publish window event
       this.publish({ subject: com.ipvs.gui.MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + this.winId, context: { objFULLSCREEN: { bFullScreen: bFullScreen}} });
     }

     /**
     * Instance call back function doKBMSEnable(bKBMSEnabled)
     * @param bKBMSEnabled {boolean}
     */
     MediaPlayer_prototype.doKBMSEnable = function(bKBMSEnabled) {
       this.clsApp.clsDisplayAgent.setDisplayConfigWinProp(this.winId, "Properties", "remoteKbmCtrl", bKBMSEnabled);
       // log the event
       this._LOG.debug("::doKBMSEnable:winId=" + this.winId + ":doKBMSEnable=" + bKBMSEnabled);
       // publish window event
       this.publish({ subject: com.ipvs.gui.MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + this.winId, context: { objREMOTEKBM: { remoteKBM: bKBMSEnabled}} });
     }

     /**
     * Instance call back function doRFBEnabled(doRFBEnabled)
     * @param doRFBEnabled {boolean}
     */
     MediaPlayer_prototype.doRFBEnabled = function(doRFBEnabled) {
       this.clsApp.clsDisplayAgent.setDisplayConfigWinProp(this.winId, "ConnectionContent", "srcKbmCtrl", doRFBEnabled);
       // log the event
       this._LOG.debug("::doRFBEnabled:winId=" + this.winId + ":doRFBEnabled=" + doRFBEnabled);
       // publish window event
       this.publish({ subject: com.ipvs.gui.MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + this.winId, context: { objSRCKBM: { srcKBM: doRFBEnabled}} });
     }

     //~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~ utility functions ~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~

     /**
     * get SDP decode string from encode escape one
     */
     MediaPlayer_prototype.getDecodeUnescapeStr = function(escapeStr) {
       var reg1 = /\n\S=/;
       //replace %0A with the new line
       var newStr = escapeStr.replace(reg1, escapeStr);
       //replace all the + with space and VIDEOPORT_TOKEN with 0 and AUDIOPORT_TOKEN with 0
       var newStr = newStr.replace(/\+/g, " ").replace(/VIDEOPORT_TOKEN/g, "0").replace(/AUDIOPORT_TOKEN/g, "0");
       var unescapeStr = unescape(newStr);
       return unescapeStr;
     }

     /**
     * startCriticalTask:Change player fps to lowest ;
     * @return {boolean} true:success;false:fail
     */
     MediaPlayer_prototype.startCriticalTask = function() {
       if (!this.IPVideoPlayer) { return false; }
       this.setPriority(false);
       this.IPVideoPlayer.Fps = MediaPlayer.LOWEST_FPS;
       this._LOG.info("startCriticalTask:drop player FPS to " + MediaPlayer.LOWEST_FPS);
       return true;
     }

     /**
     * stopCriticalTask:use only after startCriticalTask ;
     * @return {boolean} true:success;false:fail
     */
     MediaPlayer_prototype.stopCriticalTask = function() {
       if (!this.IPVideoPlayer) { return false; }
       jsx3.sleep(function() {
         this._LOG.info("stopCriticalTask:this.IPVideoPlayerRunTimeFPS=" + this.IPVideoPlayerRunTimeFPS);
         this.IPVideoPlayer.Fps = this.IPVideoPlayerRunTimeFPS;
       }, null, this);
       return true;
     }

     /**
     * Method player destroy to break cycle between DOM oject and Javascript
     * @param IPVideoPlayer {ActiveX Object}  
     */
     MediaPlayer_prototype.destroy = function() {
       if (this.IPVideoPlayer) {
         var attributes = this.IPVideoPlayer.attributes, i, l, n;
         this._LOG.trace("MediaPlayer_prototype.destroy:" + attributes.length + " player attributes.");
         if (attributes) {
           l = attributes.length;
           for (i = 0; i < l; i += 1) {
             n = attributes[i].name;
             if (typeof this.IPVideoPlayer[n] === 'function') {
               this.IPVideoPlayer[n] = null;
             }
           }
         }
         this.IPVideoPlayer = null;
       }
     }

});     // end class




  