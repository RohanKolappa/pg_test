/**
 * @author ZHEN
 */


Ext.define('TabletGui.view.SrcPortList', {
    extend: 'Ext.List',
    alias: 'widget.srcportlist',
    requires: ['TabletGui.store.SourcePortList'],
    

    config: {
 		inline: true,
		cls: 'source-list-bg',
		store:{ xclass : 'TabletGui.store.SourcePortList'},
		itemTpl:[
                '<div class="source-item-container">',
                  	'<div class="live-item-thumb" id="{nid}">',
                		'<img src="{thumbnailurl}" style="width:100%; height:100%; !important" title={title} id=\"img_{nid}\" >',
                  	'</div>',
                    '<div>',
                   		 '<span class="source-item-title">{title}</span>',
                    '</div>',
                '</div>'
                ].join(''),	
        title:'Source Port Status List'
        
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