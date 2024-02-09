/**
 * @author ZHEN
 */


Ext.define('TabletGui.view.RecordingList', {
    extend: 'Ext.List',
    alias: 'widget.recordinglist',
    requires: ['TabletGui.store.RecordingList'],
    

    config: {
 		inline: true,
		cls: 'source-list-bg',
		store:{ xclass : 'TabletGui.store.RecordingList'},	
		itemTpl:'<div class="source-item-container">'+
	              	'<div class="rec-item-thumb" id="{nid}">'+
	              		'<img src="{thumbnailurl}" style="width:100%; height:100%; !important" >'+
	              	'</div>'+                        
	              	'<div>'+
	              		'<span class="source-item-title">{title}</span>'+
	              	'</div>'+
    	        '</div>',
        title:'Recording Flile List'
        
    }/*,
    
    onLoad:function(){
    	
    	var me = this,
    		store = me.getStore();

    		me.callParent(arguments);
    		
    		if(store.getCount()===0 && store.isLoaded()){
    			
    			me.setMasked({
    				xtype:'loadmask',
    				indicator:false,
    				message:'Loading source ports...'
    			});
    			me.getScrollable().getScroller().setDisabled(true);

    		}
    		else{
    			//store.filter('portType','StreamDst');
				console.log('onLoad TabletGui.view.SrcPortList');   
    		}
    		
			//TBR
			//console.log('It (onload SrcPortList) takes:'+new Date().getTime());	    		
    }
    */
  
    
});
