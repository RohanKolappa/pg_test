/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the Media Player Window
*/

jsx3.require("jsx3.gui.Block", "com.ipvs.gui.MediaPlayerIE", "com.ipvs.gui.MediaPlayerFX");

jsx3.lang.Class.defineClass("com.ipvs.gui.MediaPlayerWindow", jsx3.gui.Block, [jsx3.gui.Interactive], function (MediaPlayerWindow, MediaPlayerWindow_prototype) {

  // create Logger for this class
  MediaPlayerWindow_prototype._LOG = jsx3.util.Logger.getLogger(MediaPlayerWindow.jsxclass.getName());

  /**
  * Static constant varibles for player modes
  */
  MediaPlayerWindow.WINDOWLESS_MODE_PLAYER = 1;
  MediaPlayerWindow.WINDOW_MODE_PLAYER = 0;
  /**
  * Static pub/sub massage constant string
  */
  MediaPlayerWindow.MSG_ONNATIVESIZECHANGE = "OnNativeSizeChange@";
  MediaPlayerWindow.MSG_ONFULLSCREEN = "OnFullScreen";
  MediaPlayerWindow.MSG_ONKBMSENABLED = "OnKBMSEnabled";
  MediaPlayerWindow.MSG_ONRFBENABLED = "OnRFBEnabled";
  /**
  * {String} plalyer ID 
  */
  MediaPlayerWindow_prototype._playerId = null;
  MediaPlayerWindow_prototype._flagValue = null;

  /**
  * {int} player type 1:windowless player, 0:normal player;
  */
  MediaPlayerWindow_prototype._playerType = MediaPlayerWindow.WINDOWLESS_MODE_PLAYER;

  /**
  * {String} plalyerSize
  */
  MediaPlayerWindow_prototype._playerSize = null;

  /**
  * {ActiveX object} media player
  */
  MediaPlayerWindow_prototype.IPVideoPlayer = null;

  /**
  * for setting 1:1 scroll bar
  */
  MediaPlayerWindow_prototype.innerSpan = null;

  /**
  * Transparency
  */
  MediaPlayerWindow_prototype._blkTransparency = null;

  //-------------------------------------------------
  // properties required for mixin methods
  /**
  * {ActiveX object} media player state 
  */
  MediaPlayerWindow_prototype.IPVSPlayerState_PAUSED = 1;
  MediaPlayerWindow_prototype.IPVSPlayerState_PLAYING = 2;

  /**
  * native size of the media player
  */
  MediaPlayerWindow_prototype.nativeSize = new Object();
  //-------------------------------------------------

  /**
  * init
  */
  MediaPlayerWindow_prototype.init = function (strName, vntLeft, vntTop, vntWidth, vntHeight, playerType) {
    this._playerType = playerType;
    this.jsxsuper(strName, vntLeft, vntTop, vntWidth, vntHeight);
  }

  /**
  * Loads the Header into the current Block
  * Async method - does load and fires the callback when complete
  * @param {jsx3.$AsyncCB}
  */
  MediaPlayerWindow_prototype.loadHeader = jsx3.$Y(function (cb) {
    // load the serialized Header component as a child
    // set bRepaint to true to force paint
    this.blkJSXHeader = this.loadAndCache("jsxaddin://user!ipvsCommon/prototypes/MediaPlayerWindowHeader.xml", true);
    this.blkJSXHeader.repaint();
    return this.blkJSXHeader;
  });

  /**
  * returns a handle to the player from the current Block
  * @return IPVideoPlayer {ActiveX Object}
  */
  MediaPlayerWindow_prototype.getPlayer = function () {
    return this.IPVideoPlayer;
  };

  /**
  * Loads a player into the current Block
  * Async method - does load and calls the (self-recursive) check method - which fires the callback when complete
  * might be called multiple times - so async callback should happen in all cases
  * also need to make sure the checking for load will abort if the block is deleted during load
  * @param {jsx3.$AsyncCB}
  */
  MediaPlayerWindow_prototype.asyncLoadPlayer = jsx3.$Y(function (cb) {

    // check playerId to make sure we only load one player
    if (!this._playerId) {
      // cook up a Player ID based on it's ID
      this._playerId = "Player" + this.getId();
      // initialize  player size
      this._playerSize = new Object;
      var playerTextHtml = "";
      // mixin the player methods based on browser type
      if (jsx3.CLASS_LOADER.IE) {
        // if 'true', then do not transfer any methods already existing in obj 'this'
        com.ipvs.gui.MediaPlayerIE.jsxclass.mixin(this, true);
        //this.getClass().bless(com.ipvs.gui.MediaPlayerIE.jsxclass);
        // set the ActiveX Object HTML as it's text; call after mixin
        playerTextHtml = this.getPlayerTextHtml(this._playerId, this._playerType, this.getId());
      }
      if (jsx3.CLASS_LOADER.FX) {
        //for Firefox
        com.ipvs.gui.MediaPlayerFX.jsxclass.mixin(this, true);
        playerTextHtml = this.getPlayerTextHtml(this._playerId, this._playerType);
      }
      if (jsx3.CLASS_LOADER.SAF) {
        //for safari
      }
      // repaint to force rendering
      this.setText(playerTextHtml, true);
    }

    // function to check for load to complete
    var checkMediaPlayer = function() {
      // if window is still valid but player handle is not yet valid - ie: player not yet loaded
      if (this._playerId && !this.IPVideoPlayer) {
        // try to get a handle to the player HTML object thru the DHTML for this class
        var playerHTML = this.getRendered();
        if (playerHTML) {
          this.IPVideoPlayer = playerHTML.ownerDocument.getElementById(this._playerId);
          // if valid - load is complete - do first time load actions
          if (this.IPVideoPlayer) {
            // set default scale mode to match window
            this.setScaleToWindow();
          }
        }
        // if win is still valid but player not ready - setTimeout and repeat
        if (this._playerId && !this.IPVideoPlayer) {
          var thisObj = this;
          setTimeout(function () { checkMediaPlayer.call(thisObj); }, 100);
        }
      }
      // if handle is finally valid = load complete = fire callback
      if (this.IPVideoPlayer) {
        // callback the async caller
        cb.done(this.IPVideoPlayer);
      }
    }

    // call the load checker - do in sleep to increase chance of success on first try
    jsx3.sleep(function() {
      checkMediaPlayer.call(this);
    }, null, this);
  });

  /**
  * unload a player from the current Block
  * stops the player, derefs it and clears the DHTML
  */
  MediaPlayerWindow_prototype.unloadPlayer = function () {
    //in case player not loaded
    if (this.IPVideoPlayer) {
      // stop the player
      this.stop();
      // deref it from this class
      this.IPVideoPlayer = null;
    }
    //    // turn off resize subscription
    //    this.unsubscribe(jsx3.gui.Interactive.AFTER_RESIZE_VIEW, this);
    this._playerSize = null;
    // remove player object
    this.setText("");
    this.repaint();
    // reset ID - acts as a semaphore
    this._playerId = null;
  };

  //  // Resize the player if the block is resized
  //  MediaPlayerWindow_prototype.onAfterResizeView = function(objEvent, objJSX) {
  //    // get the current window size
  //    this._playerSize.width = this.getWidth();
  //    this._playerSize.height = this.getHeight();
  //    // resize player to match
  //    this.resize(this._playerSize);
  //  };

  // Temp placeholder for testing
  MediaPlayerWindow_prototype.doClick = function (objEvent, objJSX) {
    alert("Thanks for clicking!");
  };

  /**
  * Sets the transparency/opacity property of a player Window
  * @param blkTransparency {int | String} the transparency as a non-negative integer.
  */
  MediaPlayerWindow_prototype.setBlkTransparency = function (blkTransparency) {
    this._blkTransparency = blkTransparency;
  };

  /** 
  * Returns the transparency/opacity property of a Window on this Canvas
  * @return {int | String} _blkTransparency.
  */
  MediaPlayerWindow_prototype.getBlkTransparency = function () {
    return this._blkTransparency;
  };

  /** 
  * Get playerType
  * @return playerType {int}: 0--windowless player, 1--window mode player;
  */
  MediaPlayerWindow_prototype.getPlayerType = function () {
    return this._playerType;
  };

  /**
  * get SDP decode string from encode escape one
  */
  MediaPlayerWindow_prototype.getDecodeUnescapeStr = function (escapeStr) {
    var reg1 = /\n\S=/;
    //replace %0A with the new line
    var newStr = escapeStr.replace(reg1, escapeStr);
    //replace all the + with space and VIDEOPORT_TOKEN with 0 and AUDIOPORT_TOKEN with 0
    var newStr = newStr.replace(/\+/g, " ").replace(/VIDEOPORT_TOKEN/g, "0").replace(/AUDIOPORT_TOKEN/g, "0");
    var unescapeStr = unescape(newStr);
    return unescapeStr;
  }
});     // end class




  
