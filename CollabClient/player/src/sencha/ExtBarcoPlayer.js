/* 
 * Wraps the player.js within an Ext.Component. To use this component you must
 * include the additional JavaScript file: player.js
 */

Ext.define('DeskCollabClient.player.ExtBarcoPlayer', {
    extend: 'Ext.container.Container',
    xtype: 'barcoplayer',
    cls: 'barcoplayer',
    id: 'barcoSDKPlayer',
    width: '100%',
    height: '100%',
    //after render listener
    listeners: {
        afterrender: function (cmp) {
            // After rendering the component will have a titleEl and descEl properties
            jQuery.log('ExtBarcoPlayer: afterrender ::' + this.getSize().width + '::' + this.getSize().height);
        },
        afterlayout: function (cmp) {
            //console.log('ExtBarcoPlayer:afterlayout ' + this.getSize().width + '::' + this.getSize().height);
            var pcmp = Ext.getCmp('playerobjctn');
            var pheight = pcmp.getSize().height;
            var pwidth = pcmp.getSize().width;
            jQuery.log('ExtBarcoPlayer:afterlayout pcmp.getWidth/pcmp.getHeight:' + pcmp.getWidth() + ':/:' + pcmp.getHeight() + ':pwidth/pheight:' + pwidth + ':/:' + pheight);
            jQuery.extend(this.options, { width: pwidth, height: pheight });
            //this.player && this.player.player('conf', this.options);
        }
    },

    //constructore
    constructor: function () {
        this.callParent(arguments);
        this.options = { playerPref: 'AUTO', showRecordingControls:true};
        jQuery.log('ExtBarcoPlayer:constructor ');
    },

    //initialize
    initialize: function () {
        this.callParent();
        this.playerEventSubscriptions();
        jQuery.log('ExtBarcoPlayer:initialize ');
    },

    //start play  
    doPlayAction: function (mediaURL, config) {
        if (config) {
            jQuery.extend(this.options, config);
        }
        if (!this.player) {
            this.loadPlayer();
        }
        else {
            this.player.player('conf', this.options);
        }
        this.player.player('play', mediaURL);
    },

    //stop session
    doStopSession: function () {
        if (this.player) { // && this.player.player('isPlaying')) {
            this.player.player('stop');
        }
    },

    //load player  
    loadPlayer: function () {
        jQuery.log('ExtBarcoPlayer:loading player ...');
        // var element = Ext.get('barco-embedded-player');
        var barcoplayerDiv = jQuery('div.barcoplayer');
        this.player = barcoplayerDiv.player(this.options);
        this.player.player('authenticate', this.options);
    },

    //logout  
    doLogout: function () {
        if (this.player) {
            this.doStopSession();
            this.player.player('destroy');
        }
    },


    // event subscriptions
    playerEventSubscriptions: function () {
        //        jQuery.subscribe("onSeek", this.displayEventMsg);
        //        jQuery.subscribe("onFullscreen", jQuery.proxy(this.doIPVFullscreen, this));
    },

    //do fullscreen for IPVPlayer
//    doIPVFullscreen: function (ojb1, obj2) {
//        var element = Ext.get('barcoSDKPlayer'); //barco-embedded-player
//        if (this.player) {
//            this.player.player('_setFullscreenStatus', { isFullScreen: obj2.isFullScreen });
//        }
//        jQuery.log('ExtBarcoPlayer:fullscreen status:' + obj2.isFullScreen);
//    },

    //call player.snapshot
    doSnapshot: function (path) {
        if (this.player) {
            this.player.player('snapshot', path);
        }
    },

    // @private
    displayEventMsg: function (obj1, obj2) {
        setTimeout(function () {
            jQuery.log('ExtBarcoPlayer:barcoplayer type: ' + obj1.type + '  ||  id: ' + obj2.id + '  ||  message: ' + obj2.message + '  ||  playerID: ' + obj2.playerId);
        }, 0);
    },

    //destroy
    destroy: function () {
        jQuery.log('ExtBarcoPlayer:destroy ...');
        if (this.player) {
            this.player = null;
            this.options = {};
        }
        this.callParent();
    }
});

