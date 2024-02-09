/**
 * @author ZHEN
 */


Ext.define('TabletGui.view.DstPortList', {
  extend: 'Ext.dataview.List',
  alias: 'widget.dstportlist',
	requires: ['TabletGui.store.DstPortList'],    

  config: {
	  inline: true,
		style: 'background:none',
		store:{ xclass : 'TabletGui.store.DstPortList'},
		itemTpl:[
             '<div class="display-item-container">',
                '<div class="display-item-title">{title}</div>',
                	'<div class="display-item-thumb-container">',
                     '<div class="display-item-thumb" id="{nid}">',
                         '<div class="display-item-source"></div>',
                         '</div>',
                    '</div>',
                	'<div id="delImgCont" class="display-item-delete-hidden"><img id="imgDelete" src="app/resource/image/icon_deleteItem.png"/></div>',
                '</div>'
                ].join(''),
                        
    title:'Destination Port List'

        
  }/*,
    
    onLoadCallback:function(){
    	console.log('DstPortList onLoad success call back');
    	//can be injected later
    },
    
    onLoad:function(){
    	
    	var me = this,
    		store = me.getStore();

    		me.callParent(arguments);
    		
    		if(store.getCount()===0 && store.isLoaded()){
    			
    			me.setMasked({
    				xtype:'loadmask',
    				indicator:false,
    				message:'Loading destination ports ...'
    			});
    			me.getScrollable().getScroller().setDisabled(true);
    			
    		}
    		else{
				me.onLoadCallback();
    		}
    		
    		//console.log('onLoad TabletGui.view.DstPortList');
    }
  
    */
});

/*
 
 Properties: Object
	@aspect: ""
	@border: ""
	@fps: 15
	@hpos: 600
	@hres: 222
	@layoutId: "layoutId1"
	@layoutPos: ""
	@maximize: false
	@mute: false
	@remoteKbmCtrl: false
	@scale: "fitWindow"
	@scrollHPos: 0
	@scrollVPos: 0
	@transparency: 0
	@visibility: true
	@volume: "50%"
	@vpos: 400
	@vres: 267
	@zIndex: 100
	 
 * 
 * */