/**
 * @author ZHEN
 */
Ext.Loader.setConfig({
  enabled: true
});

Ext.Loader.setPath({
  'TabletGui.util': 'util'
});


Ext.application({
	name: 'TabletGui',
	requirs:['TabletGui.util.Base64','TabletGui.util.PostRequestPayload'],
	
	viewport: {
    autoMaximize: true
  },

  views: ['SrcPortList','DstPortList','AppContainer','RecordingList','MainPage','SettingsPage','LoginPage'],
    
	controllers:['Main','Setup','Login','MainPanel','NotificationHandler','DisplayConfig'],
	
	models: ['Setup','Login','SourcePortList','DstPortList','RecordingList','DisplayConfig'],
	
  stores: ['Setup','Login','SourcePortList','DstPortList','RecordingList','DisplayConfig'],
    
  launch: function() {
    SERVICE_VERSION = null;
    //launch AppContainer conditionally
    var store = Ext.data.StoreManager.lookup('settingsStoreId');
		itemIndex = store ? (store.last()? 1 : 0) : 0; 
    Ext.create('TabletGui.view.AppContainer',{fullscreen: true,activeItem:itemIndex});
  }

});