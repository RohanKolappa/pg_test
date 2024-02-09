/**
 * @author ZHEN
 */
Ext.define('TabletGui.controller.Setup', {
  extend: 'Ext.app.Controller',
  alias: 'controller.setup',
  /**
   * initialize Setup controller 
   **/
  init:function(){
		var me =  this;
  	this.getApplication().on("MSG_SETUP", function(args,e) {
	    // login logic
	    console.log('controller.Setup: subscribe MSG_SETUP ');
	    me.appContainer = args.appContianer;
	    me.loginPanel = args.loginPanel;
	    me.mainPanel = args.mainPanel;	
  	});
  },
    
  /**
   * config of Setup controller 
   **/
  config:{
  	refs:{
			//form references
			settingsFormPanel:'#settingsFormPanle',
  		//set up screen 
  		nextButton:'button[text=Next]',
  		//server IP field
  		serverIpField:'textfield[name=serverip]',
  		//clientid
  		clientIdField:'textfield[name=clientid]'
  	},
  	control:{
  		nextButton:{
  			tap:'doNext'
  		},
  		clientIdField:{
  			keyup:'clickGo'
  		}
  	}
  },

  /**
   * launch function of Setup controller
   **/
	launch:function(){
		var ipaddress = location.host;
		console.log('controller.setup:launch: server ip: '+ipaddress);
		if(ipaddress){
			var re = new RegExp("^(([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])\.){3}([0-9]|[1-9][0-9]|1[0-9]{2}|2[0-4][0-9]|25[0-5])$");
			if(re.test(ipaddress)){
				this.getServerIpField().setValue(ipaddress);
				this.getClientIdField().focus();
			}
		}
		this.store = Ext.data.StoreManager.lookup('settingsStoreId');
	},
	
	/**
	 * store serverIP and clientId to local storage and move to login page 
	 **/
	doNext:function(){	
		console.log('store settings to HTML5 localstorage ... ');
		var settingFormPanel = this.getSettingsFormPanel();
		//validation settings form without load store
		var values = settingFormPanel.getValues(),
    settings = Ext.create('TabletGui.model.Setup', {
                        		serverip:values.serverip,
                        		clientid:values.clientid
                         }),
		errs = settings.validate(),
		msg = '';		
		if (!errs.isValid()) {
		  errs.each(function (err) {
		    //msg += err.getField()+' : '+err.getMessage() + '<br/>';
		    if (err.getField().indexOf('serverip')>-1)  { msg += 'Server IP ';}
		    msg += err.getMessage() + '<br/>';
			}); 
			Ext.Msg.alert('Validation Error', msg);
		}
		else{
			var recordIndex = this.store.findBy(function(record,id){
				return (record.data.serverip==values.serverip && record.data.clientid==values.clientid);
			});
			var record = this.store.getAt(recordIndex);
			if(!record ){
				//add record if not in local storage
				this.store.add({'serverip':values.serverip,'clientid':values.clientid});
				this.settingsUuid = this.store.last().data.localSettingsId;
	 		}
			else {
				this.settingsUuid = record.data.localSettingsId;
			}
			//froward to login screen
			this.appContainer.setActiveItem('#loginscreen');
		}
	},   
    
	/**
	 * click virtual keyboard GO 
	 **/
	clickGo:function(objField,e){
		if(e.event.keyCode == 13){
			this.doNext();
		}
	}
	
});