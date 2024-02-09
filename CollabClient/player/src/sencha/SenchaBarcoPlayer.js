/* 
 * Wraps the player.js within an Ext.Component. To use this component you must
 * include the additional JavaScript file: player.js
 */

Ext.define('MyApp.player.SenchaBarcoPlayer', {
  extend: 'Ext.Component',
  xtype: 'senchabarcoplayer'//,
/*
  doPlayAction: function(mode) {
    var self = this;
    var confObj = (mode=='touch')? self.getPlayerOptions() : self.playerOptions ;
    var lcControlURL = 'http://10.51.49.130/EP_HLS_DASH/hls/output.m3u8';
    jQuery.extend(confObj, {controlURL: lcControlURL});
    var jQPlayer = (mode=='touch')? self.getJQPlayer() : self.jQPlayer;
    jQPlayer.player('conf', confObj).player('play');
  },
  
  loadPlayer: function() {
    console.log('loading player ....');
    var self = this;
    var EmbeddedPlayer = (window.barco || {}).EmbeddedPlayer;
    //var jQPlayer = self.getJQPlayer();
    if (!this.jQPlayer) {
      self.renderPlayer();
    }

    // TEMPORARY UNTIL WE ADD A PLAY BUTTON
    setTimeout(function() {
      self.doPlayAction();
    }, 3000);
  },
  
  // @private
  playerEventSubscriptions: function() {
    jQuery.subscribe("onLoad", this.displayEventMsg);
    jQuery.subscribe("onUnload", this.displayEventMsg);
    jQuery.subscribe("onStart", this.displayEventMsg);
    jQuery.subscribe("onStop", this.displayEventMsg);
    jQuery.subscribe("onMute", this.displayEventMsg);
    jQuery.subscribe("onUnmute", this.displayEventMsg);
    jQuery.subscribe("onPause", this.displayEventMsg);
    jQuery.subscribe("onResume", this.displayEventMsg);
    jQuery.subscribe("onCatchup", this.displayEventMsg);
    jQuery.subscribe("onError", this.displayEventMsg);
    //sub 'onSeek' (after receiving SkipFwd/SkipBack response)  
    jQuery.subscribe("onSeek", this.displayEventMsg);
  },  
  
  // @private
  displayEventMsg: function(obj1, obj2) {
    setTimeout(function() {
      console.log('barcoplayer type: ' + obj1.type + '  ||  id: ' + obj2.id + '  ||  message: ' + obj2.message + '  ||  playerID: ' + obj2.playerId);
    }, 0);
  },

  destroy: function(mode) {
    var self = this;
    var jQPlayer = (mode=='touch') ? self.getJQPlayer() : self.jQPlayer;
    if ((window.barco || {}).EmbeddedPlayer) {
      // cleanup EmbeddedPlayer
    }
    self.callParent();
  }
*/
});

