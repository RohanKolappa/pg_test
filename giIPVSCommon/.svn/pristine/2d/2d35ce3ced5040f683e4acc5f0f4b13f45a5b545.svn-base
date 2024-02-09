/**
* Copyright (c) IP Video Systems Inc.
* Use, modification, and distribution subject to terms of license.
* 
* This acts as the Media Player Window
*/

jsx3.require("jsx3.gui.Block", "com.ipvs.gui.MediaPlayerIE");

jsx3.lang.Class.defineClass("com.ipvs.gui.MediaPlayerWindow", jsx3.gui.Block, [jsx3.gui.Interactive, jsx3.util.EventDispatcher], function(MediaPlayerWindow, MediaPlayerWindow_prototype) {

  // create Logger for this class
  MediaPlayerWindow_prototype._LOG = jsx3.util.Logger.getLogger(MediaPlayerWindow.jsxclass.getName());

  //~~~~~~~~~~~~~~~~~ properties ~~~~~~~~~~~~~~~~~

  MediaPlayerWindow.MSG_CONTROLLERUPDATE = "WindowControllerUpdate";

  /**
  * {String} window / player ID 
  */
  MediaPlayerWindow_prototype.winId = null;
  MediaPlayerWindow_prototype._playerId = null;

  /**
  * {ActiveX object} media player
  */
  MediaPlayerWindow_prototype.IPVideoPlayer = null;

  /**
  * for setting 1:1 scroll bar
  */
  MediaPlayerWindow_prototype.blkPlayer = null;

  /**
  * init
  */
  MediaPlayerWindow_prototype.init = function(winId, vntLeft, vntTop, vntWidth, vntHeight, playerType) {
    // save the winId locally for callback later
    this.winId = winId;
    this._playerType = playerType;

    // call super class
    this.jsxsuper(winId, vntLeft, vntTop, vntWidth, vntHeight);

    this.clsApp = com.ipvs.app.App.clsApp;
    if(!this.clsApp) return;

    // set fixed window properties - black bg, scroll, abolute pos
    this.setBackgroundColor("#181818", true);
    this.setOverflow(jsx3.gui.Block.OVERFLOWSCROLL);
    this.setRelativePosition(jsx3.gui.Block.ABSOLUTE, true);

    //get all function of MediaPlayer in 'this'
    this.nativeSize = new Object;
    com.ipvs.gui.MediaPlayer.jsxclass.mixin(this, true);
  }

  //  MediaPlayerWindow_prototype.onAfterAttach = function() {
  //    this.jsxsuper();
  //    
  //    // load the player
  //    this.asyncLoadPlayer().when(
  //      jsx3.$F(function() {
  //        //subscribe to get any SSCmd event this for the blkwin; windowId  = blkWin.getName()
  //        this.clsApp.clsSSAgent.subscribe("EVT_PLAYERACTION-" + this.winId, this, this.onPlayerActionEvent);
  //        //add blkWin to SSAgent to trigger player START
  //        this.clsApp.clsSSAgent.getPlayerAction(this.winId);
  //      }).bind(this));
  //  }

  //~~~~~~~~~~~~~~~~~ load/unload functions ~~~~~~~~~~~~~~~~~
  /**
  * Loads the Header into the current Block
  * Async method - does load and fires the callback when complete
  * @param {jsx3.$AsyncCB}
  */
  MediaPlayerWindow_prototype.loadHeader = jsx3.$Y(function(cb) {
    // load the serialized Header component as a child
    // set bRepaint to true to force paint
    this.blkJSXHeader = this.loadAndCache("components/blkPlayerHeader.xml", true);
    this.blkJSXHeader.repaint();
    return this.blkJSXHeader;
  });

  /**
  * Loads a player into the current Block
  * Async method - does load and calls the (self-recursive) check method - which fires the callback when complete
  * might be called multiple times - so async callback should happen in all cases
  * also need to make sure the checking for load will abort if the block is deleted during load
  * @param {jsx3.$AsyncCB}
  */
  MediaPlayerWindow_prototype.asyncLoadPlayer = jsx3.$Y(function(cb) {

    // check playerId to make sure we only load one player
    if(!this._playerId) {
      // cook up a Player ID based on it's ID
      this._playerId = "Player_" + this.getId();

      // attach player blk as a child of this window
      this.blkPlayer = new jsx3.gui.Block(this._playerId, 0, 0, '100%', '100%');
      if(this.blkPlayer) {
        this.blkPlayer.setBackgroundColor("#181818", true);
        // load a player into the window - async fnc - do nothing on return
        com.ipvs.util.Common.asyncSetChild(this, this.blkPlayer).when(
          jsx3.$F(function() {
            this._LOG.debug(":asyncLoadPlayer:" + this.blkPlayer.getName() + "'s block load successful.");
            // mixin the player methods based on browser type
            if(jsx3.CLASS_LOADER.IE) {
              // if 'true', then do not transfer any methods already existing in obj 'this'
              com.ipvs.gui.MediaPlayerIE.jsxclass.mixin(this, true);
              // set the ActiveX Object HTML as it's text
              var playerTextHTML = this.getPlayerTextHTML(this._playerId, this._playerType, this.getId());
              // repaint to force rendering
              this.blkPlayer.setText(playerTextHTML, true);;
            }
            if(jsx3.CLASS_LOADER.FX) {
              //for Firefox
            }
            if(jsx3.CLASS_LOADER.SAF) {
              //for safari
            }
          }).bind(this));
      }
      else {
        this._LOG.error(":asyncLoadPlayer: " + this._playerId + ": FAILED");
      }
    }

    // function to check for load to complete
    var checkMediaPlayer = function() {
      // if window is still valid but player handle is not yet valid - ie: player not yet loaded
      if(this._playerId && !this.IPVideoPlayer) {
        // try to get a handle to the player HTML object thru the DHTML for this class
        var playerHTML = this.getRendered();
        if(playerHTML) {
          this.IPVideoPlayer = playerHTML.ownerDocument.getElementById(this._playerId);
          // if valid - load is complete - do first time load actions
          // set default scale mode to match window
          this.IPVideoPlayer && this.setScaleToWindow();
        }
        // if win is still valid but player not ready - setTimeout and repeat
        if(this._playerId && !this.IPVideoPlayer) {
          var thisObj = this;
          setTimeout(function() { checkMediaPlayer.call(thisObj); }, 100);
        }
      }
      // if handle is finally valid = load complete = fire callback
      if(this.IPVideoPlayer) {
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
  MediaPlayerWindow_prototype.unloadPlayer = function() {
    //in case player not loaded
    if(this.IPVideoPlayer) {
      // stop the player
      this.stop();
      // deref it from this class
      this.IPVideoPlayer = null;
    }
    // remove player object
    this.setText("");
    this.repaint();
    // reset ID - acts as a semaphore
    this._playerId = null;
  };

  /**
  * onPlayerActionEvent: trigger applyPlayerAction after getting SUB
  */
  MediaPlayerWindow_prototype.onPlayerActionEvent = function(EventVal) {
    //get winID form evt subject:eg: EVT_PLAYERACTION-Conn_C4CD1BF5-B840-0001-A1CB1CF0490011F5
    var winId = EventVal.subject.substring(EventVal.subject.indexOf('-') + 1);
    // get player cmd from evt context
    this.objPlayerCmd = EventVal.context.objPLAYERCMD;
    // act on the 'action'
    switch(this.objPlayerCmd.action) {
      case "Start":
        this._LOG.info(":onPlayerActionEvent: Start: URL: " + this.objPlayerCmd.streamURL + " : winId: " + this.objPlayerCmd.windowId + " : connId: " + this.objPlayerCmd.connId);
        // stash stream URL
        this.streamURL = this.objPlayerCmd.streamURL;
        // start the player
        this.start(this.objPlayerCmd.streamURL);
        break;
      case "Stop":
        this._LOG.info("::onPlayerActionEvent: Stop: winId: " + this.objPlayerCmd.windowId + " : connId: " + this.objPlayerCmd.connId);
        // stop the player
        this.stop();
        // clear the stashed URL
        this.streamURL = null;
        break;
      default:
        this._LOG.warn(":onPlayerActionEvent: Unknown Action: " + this.objPlayerCmd.action + ": winId: " + this.objPlayerCmd.windowId + ": canvasId:" + this.objPlayerCmd.canvasId);
        return false;
        break;
    }
    // publish window event for Action
    this.publish({ subject: MediaPlayerWindow.MSG_CONTROLLERUPDATE + '_' + this.winId, context: { objACTION: this.objPlayerCmd} });

    return true;
  }

});     // end class




  