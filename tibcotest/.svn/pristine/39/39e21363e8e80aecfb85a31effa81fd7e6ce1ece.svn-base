/*
 * Copyright (c) IP Video Systems Inc.
 * Use, modification, and distribution subject to terms of license.
 */


/**
 * This class provides a block-based, object-oriented approach to creating media player objects
 */
jsx3.Class.defineClass("com.ipvs.gui.MediaPlayer", null, null, function(MediaPlayer, MediaPlayer_prototype) {

  // create Logger for this class
  MediaPlayer_prototype._LOG = jsx3.util.Logger.getLogger("com.ipvs.gui.MediaPlayer");
  
  /**
  * Method getPlayerInstance
  * @param instanceID {String} instance id
  * @param blkJSX {jsx3.gui.Model} 
  * @return {ActiveX Object} 
  */
  MediaPlayer.getPlayerInstance = function(instanceID, blkJSX) {
    var playerInstance = null;
    if(jsx3.CLASS_LOADER.IE){
      playerInstance = new com.ipvs.gui.MediaPlayerIE(instanceID, blkJSX);
    }
    if (jsx3.CLASS_LOADER.FX) {
      playerInstance = new com.ipvs.gui.MediaPlayerFX(instanceID, blkJSX);
    }
    if (jsx3.CLASS_LOADER.SAF) {
     //for afari
    }
    return playerInstance;
  }
  
});
