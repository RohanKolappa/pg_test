/**
 * @author ZHEN
 */

Ext.define('TabletGui.store.SourcePortList', {
  extend: 'Ext.data.Store',
  alias: 'store.sourceportlist',
  requires: ['TabletGui.model.SourcePortList'],
     
	config: {
    model: 'TabletGui.model.SourcePortList',
    //model:'sourceportmodel',
    storeId: 'sourcePortStoreId',
    autoLoad:false,
        //autoDestroy:true,
		filters:[{property:'presence',value:'available'},{property:'portType',value:'StreamSrc'}],

		/*
     	proxy:{
        	type:'ajax',
        	url: '/tabletFilter/app/data/getSrcPort.json',
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