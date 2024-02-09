/**
 * @author ZHEN
 */
Ext.define('TabletGui.store.DstPortList', {
  extend: 'Ext.data.Store',
  alias: 'store.dstportlist',
  requires: ['TabletGui.model.DstPortList'],
     
	config: {
    model: 'TabletGui.model.DstPortList',
    storeId: 'dstPortStoreId',
    autoLoad:false,
		filters:[{property:'presence',value:'available'},{property:'portType',value:'StreamDst'}],
/*				
     	proxy:{
        	type:'ajax',
        	url: '/tabletFilter/app/data/getDstPort.json',
			reader:{
        		type:'json',
        		noCache: false,
        		rootProperty:'Resource.Port'
        	}	
	     }		
		
*/        
      
    proxy:{
     	type:'ajax',
      url:'',
			withCredentials:true,
			headers:{},
			reader:{
        type:'json',
        noCache: false,
        rootProperty:'Resource.Port'
      }	
	  }
  }
  
});
