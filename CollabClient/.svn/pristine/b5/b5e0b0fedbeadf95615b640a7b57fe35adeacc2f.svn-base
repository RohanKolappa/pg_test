/* ============================================================================
 * MainMenu jQuery plugin
 * Copyright (c) Barco Inc.
 * Use, modification, and distribution subject to terms of license.
 * ========================================================================= */


var MainMenu = Class.extend({
	_defaults:{
   
	},

	init: function (options, elem) {
    //initialize instance variables
    this.playerMenuBtn = null;
    this.controllerMenuBtn = null;
    this.sessionMenuBtn = null; 
    this.mainMenuPanel = null;
		this.options = jQuery.extend({},this._defaults,options);
    this.elem = elem;
    this.$elem = jQuery(elem);
    this._build();
    this._setFields();
    this.log(bINFO, "init", "return", "Done");
	},
  
	_build:function(){
    var html =   
        '   <div id="main-menu">'+
        '      <a href="#">'+
        '         <div id="player-menu" class="btn-main">'+
		'			 <i class="fa fa-play"></i>'+
        '            <span>&nbsp;&nbsp;Media Player</span>'+
        '         </div>'+
        '      </a>'+
        '      <a href="#">'+
        '         <div id="controller-menu" class="btn-main">'+
		'			 <i class="fa fa-cog"></i>'+
        '            <span>&nbsp;&nbsp;Endpoint Controller</span>'+
        '         </div>'+
        '      </a>'+
        '   </div>'
    jQuery(html).appendTo(this.$elem);
    this.log(bINFO, "_build", "return", "Done");
	},
  
	conf:function(options){
    this.options = jQuery.exentd({},this.options,options);
  },

	destroy:function(){
    this._resetFields();
    this.$elem.removeData('edp_mainMenu');
  },
  
	_setFields:function(){
    this.playerMenuBtn = this.$elem.find('#player-menu');
    this.controllerMenuBtn = this.$elem.find('#controller-menu');
    this.sessionMenuBtn = this.$elem.find('#session-menu'); 
    this.mainMenuPanel = this.$elem.find('#main-menu-container'); 
    this._attachEvents();
  },
	 
  _resetFields:function(){
    this.playerMenuBtn = null;
    this.controllerMenuBtn = null;
    this.sessionMenuBtn = null; 
    this.mainMenuPanel =  null;
    this._detachEvents();
  },

  _attachEvents:function(){
    this.playerMenuBtn.on('click',jQuery.proxy(this._onClickPlayer,this));
    this.controllerMenuBtn.on('click',jQuery.proxy(this._onClickController,this));    
    this.sessionMenuBtn.on('click',jQuery.proxy(this._onClickSession,this));    
  },
  
  _detachEvents:function(){
    this.playerMenuBtn.off('click',jQuery.proxy(this._onClickPlayer,this));
    this.controllerMenuBtn.off('click',jQuery.proxy(this._onClickController,this));
    this.sessionMenuBtn.off('click',jQuery.proxy(this._onClickSession,this));    
  },
  
  _onClickPlayer:function(){
    this.mainMenuPanel.hide();
    jQuery.publish(MainMenu.EVENT.MSG,MainMenu.EVENT.ON_MSG_SELECT_PLAYER);
  },
  
  _onClickController:function(){
    this.mainMenuPanel.hide();
    jQuery.publish(MainMenu.EVENT.MSG,MainMenu.EVENT.ON_MSG_SELECT_CONTROLLER);
  },
  
  _onClickSession:function(){
    this.mainMenuPanel.hide();
    jQuery.publish(MainMenu.EVENT.MSG,MainMenu.EVENT.ON_MSG_SELECT_SESSION);
  },
  
  show:function(){
    this.$elem.removeClass('hidden');
  },
  
  hide:function(){
    this.$elem.addClass('hidden');
  }  
});

MainMenu.EVENT = {
  MSG:'EDP.MENU_SELECTION',
  ON_MSG_SELECT_MAINMENU: "EDP.SELECT_MAINMENU",
  ON_MSG_SELECT_PLAYER: "EDP.SELECT_PLAYER",
  ON_MSG_SELECT_SESSION: "EDP.SELECT_SESSION", 
  ON_MSG_SELECT_CONTROLLER: "EDP.SELECT_CONTROLLER"
};
// expose Objects as jQuery plugins
jQuery.plugin('edp_mainMenu', MainMenu);
