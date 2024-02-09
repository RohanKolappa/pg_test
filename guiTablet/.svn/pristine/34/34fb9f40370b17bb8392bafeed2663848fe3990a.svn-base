/**
 * @author ZHEN
 */
Ext.define('TabletGui.store.DisplayConfig', {
  extend: 'Ext.data.Store',
  alias: 'store.displayconfig',
  requires: ['TabletGui.model.DisplayConfig'],
     
	config: {
    model: 'TabletGui.model.DisplayConfig',
    storeId: 'displayconfigStoreId',
    autoLoad:false,
		proxy:{
      type:'ajax',
      url:'',
			withCredentials:true,
			headers:{},
			reader:{
    		type:'json',
    		noCache: false,
    		rootProperty:'Resource.PortDisplayConfig'//.Canvas.Window
    	}	
	   }
   }  
});