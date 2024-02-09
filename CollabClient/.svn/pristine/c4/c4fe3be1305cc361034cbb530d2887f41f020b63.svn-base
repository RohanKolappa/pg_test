/* 
 * Wraps the player.js within an Ext.Component. To use this component you must
 * include the additional JavaScript file: player.js
 */

Ext.define('TabCollabClient.player.TouchBarcoPlayer', {
    extend: 'Ext.Container',
    xtype: 'barcoplayer',
    config: {
        id: 'barcoSDKPlayer',
        baseCls: 'barcoplayer',
        player: null,
        width: '100%',
        height: '100%',
        playerOptions: {showRecordingControls:true},
        playerWidth:0,
        playerHeight:0,
        listeners: [
            {
                fn: 'onPainted',
                event: 'painted'
            }
        ]
    },

    onPainted: function (cmp) {
        var parentDivSize = cmp.getSize();
        this.playerWidth = parentDivSize.width;
        this.playerHeight = parentDivSize.height;
    },

    //constructor
    constructor: function () {
        this.callParent(arguments);
    },

    //initialize
    initialize: function () {
        this.callParent();
    },

    //start player with a mediaURL
    doPlayAction: function (mediaURL) {
        var confObj = this.getPlayerOptions();
        if (!this.player) {
            this.loadPlayer();
        }
        jQuery.extend(confObj, { playerURL: mediaURL });
        this.player.player('conf', confObj);
        this.player.player('authenticate', confObj);
        this.player.player('play', mediaURL);
    },

    //load player
    loadPlayer: function () {
        console.log('loading player....');
        if (!this.player) {
            var barcoplayerDiv = jQuery('div.barcoplayer');
            barcoplayerDiv.empty();
            this.player = barcoplayerDiv.player({width:this.playerWidth,height:this.height});
            this.setPlayer(this.player);
        }
    },

    //logout
    doLogout: function () {
        if (this.player) {
            this.player.player('stop');
            this.player.player('destroy');
        }
    },

    // @private
    playerEventSubscriptions: function () {
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
    displayEventMsg: function (obj1, obj2) {
        setTimeout(function () {
            console.log('barcoplayer type: ' + obj1.type + '  ||  id: ' + obj2.id + '  ||  message: ' + obj2.message + '  ||  playerID: ' + obj2.playerId);
        }, 0);
    },

    //destroy
    destroy: function () {
        if (this.player) {
            this.player.player('destroy');
            this.setPlayer(null);
        }
        self.callParent();
    }
});

