/**
 * @author ZHEN
 */
Ext.define('TabletGui.model.DisplayConfig', {
  extend: 'Ext.data.Model',
  alias: 'model.displayconfigmodel',

  config: {
	 	fields: [
    			{name:'nid', mapping:'@NID'},
    			{name:'title', mapping:'@title'},
    			{name:'canvas', mapping:'Canvas'},        			
    			{name:'stateImg', 
    				type:'string',
    				convert:function(v,rec){
    					return '<img src="./app/resource/image/icon_dstmonitor.png">' ;
    				}
    			}        			
    		]
  }
});