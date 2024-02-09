/**
 * @author ZHEN
 */
Ext.define('TabletGui.model.DstPortList', {
  extend: 'Ext.data.Model',
  alias: 'model.dstportmodel',

  config: {
	 	fields: [
          {name:'nid', mapping:'@NID'},
    			{name:'title', mapping:'@title'},
    			{name:'presence', mapping:'@presence'},
    			{name:'portType', mapping:'@portType'},
    			{name:'streamType', mapping:'@streamType'},
    			{name:'state', mapping:'@state'},
    			{name:'hasDisplayConfig',
    			 type:'boolean',
    			 mapping:'FeatureList',
    			 convert:function(v,rec){
    				 if(v.Feature){
    					 for(var i=0; i<v.Feature.length;i++){
        				 var obj =v.Feature[i]; 
        				 if((obj['@name']=="IPVSClientDisplayConfig")||(obj['@name']=="XP200DisplayConfig")){
     							 return true;
     						 }
        			 }	
    					}
    					return false;
    				}
    			},
    			{	name:'stateImg', 
    				type:'string',
    				mapping:'@state',
    				convert:function(v,rec){
    					return '<img src="./app/resource/image/icon_dstmonitor.png">' ;
    				}
    			}        			
    		]
  }
});