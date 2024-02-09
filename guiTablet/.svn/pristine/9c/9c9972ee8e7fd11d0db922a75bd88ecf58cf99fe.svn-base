/**
 * @author ZHEN
 */
Ext.define('TabletGui.controller.Login', {
  extend: 'Ext.app.Controller',
  alias: 'controller.login',

  /**
   * , itialize pub/sub
   **/
  init:function(){
		var me = this; 
		this.getApplication().on("MSG_LOGIN", function(args,e) {
  		// login logic
  		console.log('Login: subscribe MSG_LOGIN ');
  		me.appContainer = args.appContianer;
  		me.setupPanel = args.setupPanel;
  		me.mainPanel = args.mainPanel;	
		});
  },
     
  /**
   * launch function 
   **/
  launch:function(){
  	this.setupController = this.getApplication().getController('Setup');
  	this.settingsStore = this.setupController.store;
   	this.loginStore = Ext.data.StoreManager.lookup('loginStoreId'),
  	this.notificationHandler = this.getApplication().getController('NotificationHandler');
  	this.loginCount = 0;
  	this.uuid = new Ext.data.identifier.Uuid();
  },
    
  /**
   * config function 
   **/
  config: {
  	refs:{
  		//form fields 'Username' & 'Password' 
  		loginFormPanel:'#loginFormPanle',
  		//password field 
  		passwordfield:'passwordfield',  		
  		//button 'login' and 'Settings' ???
  		loginButton:' button[text=Sign In]',
  		settingButton:'button[iconCls=settings]'
  	},
  	
  	control:{
  		loginButton:{
  			tap:'doLogin'
  		},
  		settingButton:{
  			tap:'doSetup'
  		}, 
  		passwordfield:{
  			keyup:'clickGo'
  		}
  	},
  	httpHeader:{},
  	userData: {ipaddress:'',clientid:'',username:'',password:'',urlBase:''}
  },
    
  /**
   * Login button in login page 
   **/
	doLogin:function(){	
		//get log form panel
		var loginFormPanel = this.getLoginFormPanel();
		var values = loginFormPanel.getValues();
		var	loginModel = Ext.create('TabletGui.model.Login', {
                        	username:values.username,
                        	password:values.password
                        }),
			errs = loginModel.validate(),
			msg = '';
		
		if (!errs.isValid()) {
		  errs.each(function (err) {
		    msg += err.getMessage() + '<br/>';
			}); 
			Ext.Msg.alert('Validation Error', msg);
		}
		else{
			//disable the login button util response back
			this.getLoginButton().disable();			
			//initialize subsciption ids
			this.srcSubId = "Src_" + this.uuid.generate();
			this.destSubId = "Dst_" + this.uuid.generate();
			this.rcdSubId = "Rec_" + this.uuid.generate();			
			//mask fieldset to indicate click login 
			loginFormPanel.down('fieldset').setMasked(true);
			//get IP address from HTML5 localstorage
			var settingRecord = this.settingsStore.findRecord('localSettingsId',this.setupController.settingsUuid).getData();
      //{"serverip":"10.1.3.2","clientid":"asdfad","id":"ext-record-4"}
			var ipAddress = settingRecord.serverip,
				clientid = settingRecord.clientid,
				username = values.username,
			    password = values.password,
		    	urlBaseStr = 'http://'+ipAddress+'/controller';
			this.setUserData({ipaddress:ipAddress,clientid:clientid,username:username,password:password,urlBase:urlBaseStr});   
			var base64Encord = 'Basic '+ Ext.create('TabletGui.util.Base64').encode(username+':'+password),
				barcoResource = clientid+'_'+username,	
				header = {
          "Content-Type": "application/json",
          'Authorization': base64Encord,
          //if Accept header then reture */*, bydefault get JSON response back, 
          //if fill then following application/vnd.barco.nimbus.Device+json except serviceLogin
          'Accept':'application/json',
          'X-Barco-resource':barcoResource
			  };			
			this.setHttpHeader({httpHeader:header});   
			console.log(' Login: doLogin base64encordStr: '+base64Encord+' X-Barco-resource: '+barcoResource);
			//this should use POST					
			var urlStr = urlBaseStr+'/Service/Login';
			var failureCB = this.loginFailure, successCB = this.loginSuccess;
			var options = {httpHeader:header,urlStr:urlStr,successCallBack:successCB,failureCallBack:failureCB,jsonData:{"ServiceLogin": {"clientPrivateKey":"" ,"@clientVersion": SERVICE_VERSION, "clientTemplateTitle": clientid}}};
			//send login request
			this.sendLoginRequest(options);
		}
	},//end of doLogin
	
	/**
	 * login success call back 
	 **/
	loginSuccess: function(response){ 
    console.log('Login: service login success.');    
    //[1] unmask login fieldset
    var loginFormPanel = this.getLoginFormPanel();
    loginFormPanel.down('fieldset').setMasked(false);
    this.getLoginButton().enable();  	
  	//[2] load src ports first, load dest when src reponse back
  	var mainPanelController = this.getApplication().getController('MainPanel');
  	mainPanelController.loadSrcList();  	
  	//[3] load recording list as iPad re-login has a problem if load while rcdPanel activate
  	mainPanelController.loadRecordingList();          
		//[4] froward to mainscreen
		this.appContainer.setActiveItem('#mainscreen');		
		//[5] start from live tab
		mainPanelController.getTabpanel().setActiveItem('#livepanel');	
		//[6] send Long pulling notification request (create notification channel) 
		//i.e. http://10.51.51.73/controller/Service/NotificationChannel?token=RTAwQT...
		var headersObj = response.getAllResponseHeaders();
		var notificationUrl = headersObj['x-barco-notification-channel'];
		//notificationUrl for debug change expired time
		//notificationUrl += '&expiryinterval=300000';
		var options = {};
		var header = this.getHttpHeader().httpHeader;
		Ext.defer(function () {
			console.log('Login:doLogin:defer createLongPollingChannel');
			//reset aborted flag  
		  this.notificationHandler.setLongpollingAborted(false); 
			//workaround for notification problem 
      this.notificationHandler.createLongPollingChannel(notificationUrl,header,options);
    },100,this);
	},
	
	/**
	 * login fail call back 
	 **/
	loginFailure: function(e){
  	// if refresh,close Tab/Browser window relogin has 'device in use' exception 
  	console.log('Login: service login fails with error, status:'+ e.status);
    if(e.status == '409'){
    	//parse user from e.request.header
    	var lastLoginRecord = null, resourceHeader = e.request.headers['X-Barco-resource'];
    	if(resourceHeader){
    		var user = resourceHeader.substring(resourceHeader.indexOf('_')+1);
    		lastLoginRecord = this.loginStore.findRecord('username',user);
    	}
    	else{
    		//'User Already Loggedin', check localstroage to see wheter logged in before without nice logout
        lastLoginRecord = this.loginStore.last();	
    	}
    	if(!lastLoginRecord){
    		// this user already login from other machine/browser, kick him out (FIFO)
    		//send log out request first and call back login if logout success
    		var thisObj = this;
    		Ext.Msg.confirm("Confirmation", "This user is logged in on different device, would you like to end that session and log in here?", function(btnId){
    			if(btnId==='yes'){
    				thisObj._kickoutAndLogin('MainPanel:login here even if the user already login on different device');		
    			}
    			else {
    				//release fields to change login user
    				var loginFormPanel = thisObj.getLoginFormPanel();
    				loginFormPanel.setValues({username:'',password:''});
    	      loginFormPanel.down('fieldset').setMasked(false);
    	      thisObj.getLoginButton().enable();
    			}
    		});    		
    	}
    	else if (lastLoginRecord.getData()) {
    		//logout and login when the logout success
    		this._kickoutAndLogin('MainPanel:re-login after window being updated(refresh/close)');
    	}
    }
      /* uncomment when reliability put in
      else if ((e.status == 502 || e.status == 503) && this.loginCount == 0 ){
      	//'Proxy Error' || 'Service Unavailable', Jetty server down
        	// keep on log in till success or try max 30 times
      	
      	var thisObj =this, timerId = 0;
      	var keepLogin = function(){
      		if(thisObj.loginCount >= 30 || thisObj.appContainer.getActiveItem() == '#mainscreen'){
      			clearInterval(timerId);
      			thisObj.loginCount = 0;
      		}
      		else{
      			thisObj.doLogin();
      			thisObj.loginCount++;	
      		}
      	}
      	timerId = setInterval(keepLogin,100);
      
      }
      */
    else{
    	//unmask login fieldset
    	this.getLoginFormPanel().down('fieldset').setMasked(false);
    	this.getLoginButton().enable();
    	var msg = '';
    	if (e.responseText && e.responseText.indexOf('Authentication failed')>=0){
    		msg = 'The username and password did not match, please try again.';
    	}
    	else if(e.status == '401'){
    		//"java.lang.NullPointerException"
    		msg = 'Unauthorized';
    	}				
    	else if(e.status == '500' && e.responseText.indexOf('<Code>1026</Code>')>=0){
    		//"<Code>1026</Code><Description>dddd</Description>"
    		msg = 'Invalid client name';
    		
    		//remove this invalid client from settings store
    		var clientId =  e.responseText.substring(e.responseText.indexOf('<Description>')+'<Description>'.length,e.responseText.indexOf('</Description>'));
    		if(clientId){
    			var index = this.settingsStore.findExact('clientid',clientId);
    			if(index>=0) {
    				this.settingsStore.removeAt(index);
    				this.appContainer.setActiveItem('#settingsscreen');
    				var clientIdField = this.setupPanel.down('textfield[name=clientid]');
    				clientIdField.setValue('');
    				clientIdField.focus();
    			}
    		}
    	}
    	else if (e.status == '502' || e.status == '503'){
    		msg = 'Server not available';
    	}
    	else{
    		msg = e.statusText? e.statusText:e.responseText;
    		if(msg && msg.indexOf('</Description>')>=0){
    			msg = msg.substring(msg.indexOf('<Description>')+'<Description>'.length,msg.indexOf('</Description>'));
    		}
    		else{
    			//given a wrong IP,remove the record from setting store. (e.status: 0;statusText:"", responseText:"")
    			var obsolateRecord = this.settingsStore.findRecord('localSettingsId',this.setupController.settingsUuid);
    			this.settingsStore.remove(obsolateRecord);	
    		}
    	}	
    	Ext.Msg.alert('Login Error:', msg);
    }
	},
	
	/**
	 * helper function, kickout and then login 
	 **/
	_kickoutAndLogin:function(msg){
		var url = this.getUserData().urlBase,
			header = this.getHttpHeader().httpHeader;
			
		this.sendKickoutRequest({
  		url:url, httpHeader:header,
			successCallBack:function(){
    	  	this.doLogin();
    	  	console.log(msg);
    	  	//console.log('MainPanel:re-login after window being updated');
  		}.bind(this)   
    });
	},
	 
	/**
	 * send login request 
	**/
	sendLoginRequest:function(options){
		Ext.Ajax.request({
		  url: options.urlStr,
			method:'POST',
			scope:this,
			//type:'jsonp',
		  headers:options.httpHeader,
			//clientPrivateKey:clientid's password (optional empty for now)
			// clientTemplateTitle:webtablet (This is an element, so @ not needed)				    
			//jsonData:{"ServiceLogin": {"clientPrivateKey":"" ,"@clientVersion": SERVICE_VERSION, "clientTemplateTitle": clientid}},
			jsonData:options.jsonData,
			success: function(resp){ 
  			// urlStr, base64Encord, barcoResource
  			var pid = 'PID_'+new Date().getTime(),
  				username = this.getUserData().username;
  			var record = this.loginStore.findRecord('username',username);
  			if(!record){
  				//keep only one record per user per local storage
  				this.loginStore.add({'username':username,'password':pid});	
      		var currentRecord = this.loginStore.findRecord('username',username);
  				//data.localLoginId 
  				this.loginUuid = currentRecord.internalId;	
  			}
  			//success call back
    		options.successCallBack.call(this,resp);
  		},
  		
  		failure: function(e){
  			options.failureCallBack.call(this,e);
  		}           
		}); 
	},
	
	/**
	 * Send logout request
	 **/
	sendLogoutRequest:function(options){
		Ext.Ajax.request({
			url: options.url+'/Service/Logout',
			method:'POST',
    	headers:options.httpHeader,
    	scope:this,
    	success: function(){ 
      	options.successCallBack();
				this.srcSubId = null;
				this.destSubId = null;
				this.rcdSubId = null;
    	},
  		failure: function(e){
  			var msg = '<h30><i>'+e.statusText+'<b><br>';	
  			Ext.Msg.alert('Logout Error', msg);
  		}
		}); 
	},
	
	/**
	 * Send kickout request, duplicate login, kick the first one out
	 **/
	sendKickoutRequest:function(options){
		Ext.Ajax.request({
			url: options.url+'/Service/Kickout',
			method:'POST',
    	headers:options.httpHeader,
    	scope:this,
    	success: function(){ 
  			this.srcSubId = null;
  			this.destSubId = null;
  			this.rcdSubId = null;
    		options.successCallBack();
  		},
  		failure: function(e){
  			var msg = '<h30><i>'+e.statusText+'<b><br>';	
  			Ext.Msg.alert('Kickout Error', msg);
  		}
		}); 
	},
	
	/**
	 * Setup button in main page
	 **/
	doSetup:function(){
		this.appContainer.setActiveItem('#settingsscreen');
		this.settingsUuid = this.setupController.settingsUuid;
		var values = {serverip:'',clientid:''};
		if (this.setupController.settingsUuid){
			values = this.settingsStore.findRecord('localSettingsId',this.setupController.settingsUuid);
		}
		//to get model instance, need setRecord to form panel
		this.setupPanel.down('formpanel').setRecord(values);
	},
    
	/**
	 * helper function, click virtual keyboard GO 
	 **/
	clickGo:function(objField,e){
		if(e.event.keyCode == 13){
			this.doLogin();
		}
	}
});