/**
 * @author ZHEN
 */

Ext.define('TabletGui.store.RecordingList', {
  extend: 'Ext.data.Store',
  alias: 'store.recordinglist',
  requires: ['TabletGui.model.RecordingList'],
     
	config: {
    model: 'TabletGui.model.RecordingList',
    //model:'sourceportmodel',
    storeId: 'recordingStoreId',
    autoLoad:false,
    //autoDestroy:true,
		filters:[{property:'fileType',value:'MediaClip'},{property:'state',value:'Ready'}],
		//filters:[{property:'presence',value:'available'}],
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
        rootProperty:'Resource.File'
      }	
	  }     
  }
});