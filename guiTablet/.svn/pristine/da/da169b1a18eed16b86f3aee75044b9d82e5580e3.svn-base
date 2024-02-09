/**
 * @author ZHEN
 */
Ext.define('TabletGui.controller.MainPanel', {
	extend: 'Ext.app.Controller',
	alias: 'controller.mainpanel',

	/**
	 * Controller initialize function is called before almost everything else
	 * 1.Controller#init;2.Proifle#launch;3.Application#launch;4.Controller#launch;
	 **/
  init:function(){
		this.getApplication().on("MSG_MAINPANEL", function(args,e) {
			// login logic
			console.log('MainPanel:init: subscribe MSG_MAINPANEL');
			this.appContainer = args.appContianer;
			this.setupPanel = args.setupPanel;
			this.loginPanel = args.loginPanel;
			//put current user to first record of SelectField 	
			var loginFormPanel =  this.loginPanel.down('formpanel');	
			var values = loginFormPanel.getValues();
			var selectOptions = this.getSelectFld().getOptions();
			if(selectOptions.length>2){
				selectOptions.shift();
			};
			selectOptions.unshift({text:values.username,value:values.username});
			this.getSelectFld().updateOptions(selectOptions).setValue(values.username);		
			//set page size for recording
			this.pageSize = 200;
			//	this.getApplication().on("MSG_LOADPORTS", function(){me.loadSrcDestList.call(me)});
		}, this);
  },
    
	/**
	 * Initial application environments
	 **/
	launch:function(){
	  //SUBSCRIBE NOTIFICATIONS 
		this.getApplication().on("KickedOut",  function(args){this.kickOutHandler.call(this,args)}, this);
		//this.getApplication().on("LoggedOut",  function(args){this.loggedOutHandler.call(this,args)}, this);		
	  this.getApplication().on("ConnState",  function(args){this.connStateHandler.call(this,args)}, this);
		this.getApplication().on("PortConnectionUpdate", function(args){this.portConnectionUpdateHandler.call(this,args)}, this);
		this.getApplication().on("PortConnectionStateUpdate", function(args){this.portConnectionStateUpdateHandler.call(this,args)}, this);
		this.getApplication().on("PortUpdate", function(args){this.portUpdateHandler.call(this,args)}, this);
		this.getApplication().on("NotificationError", function(args){this.doLogoutOnError.call(this,args)}, this);
		this.getApplication().on("PortDisplayConfigUpdate", function(args){this.portDisplayConfigUpdateHandler.call(this,args)}, this);
		
		//get request payload
		this.viewReqPayload = Ext.create('TabletGui.util.PostRequestPayload').getViewReq();
		this.playbackReqPayload = Ext.create('TabletGui.util.PostRequestPayload').getPlayBackReq();
		
		//src/deset list
		this.dstlist = this.getDestlist();
		//this.dstlist.onLoadCallback = this.destOnloadCallBack.bind(this);
		this.srclist = this.getSourcelist();
		
		//layout class
		//this.layoutCssClass = {0:'layout1x1',1:'layout1x1',2:'layout2x1',3:'layout2x2',4:'layout2x2',5:'layout3x2',6:'layout3x2',7:'layout4x2',8:'layout4x2'};
		this.displayConfigStore = Ext.data.StoreManager.lookup('displayconfigStoreId');	
		this.displayConfigController = this.getApplication().getController('DisplayConfig');
		this.loginController = this.getApplication().getController('Login');	
		this.notificationHandlerController = this.getApplication().getController('NotificationHandler');	
		
		//init display status array 
		this.srcDisplayStatusArr = {};
		this.recDisplayStatusArr = {};
		this.destDisplayStatusArr = {};
		
		//thumbnail manage array
		this.srcTnTimerArr = [];
		this.destTnTimerArr = [];
  },

	/**
	 * Define, mapping controls to CSS selector based targets
	 **/
	config: {
    refs:{
			//port panel and list 
			mainpanel:'#mainpanel',
			tabpanel:'#tabpanel',
			recordingpanel:'#recordingpanel',	
			recordinglist:'recordinglist',		
			livepanel:'#livepanel',
			sourcelist:'srcportlist',
			destpanel:'#destpanel',
			destlist:'dstportlist',
			selectFld:'#mainscreen > toolbar > selectfield'
    },
	  control:{
			selectFld: {
		    change: 'onSelectChange'
		  },
		  mainpanel:{
		    listeners: {
		    	painted:{
					  fn : function(img) {
					    console.log('MainPanel:config: main panel painted... ');             
					  },
					  scope: this
					}
				}
		  },
		  //click Recording tab
		  recordingpanel:{
				activate:function(){
					console.log('MainPanel:config: recordingPanel being activated  ');
				}
			},
	    //add list listener
	    recordinglist:{
	     	select:'onRecordingListTap'
	    },	        
	    sourcelist:{
	     	itemtap:'onSrcListTap'
	    },
	    destlist:{
	     	itemtap:'onDstListTap'
	    }   		
	  },
		//Session manager data
		sessionData:{},
		selSrcItem:"NONE",
		selSrcType:"NONE",
		selDstItem:"NONE",
		selDstWin:"NONE",
		//add src, dest port, recording file title
		selSrcTitle:"NONE",
		selDstTitle:"NONE",
		selSrcStreamType:"NONE",
		selDstStreamType:"NONE"
  },//end of config

  /**
   * Tap recording list
   **/	
  onRecordingListTap:function(dv,record){
  	console.log('MainPanel:onRecordingListTap:title: '+record.getData().title);
		this.getSourcelist().deselectAll();
		//set as selected file
		this.setSelSrcItem(record.getData().nid);
		this.setSelSrcTitle(record.getData().title);
		this.setSelSrcType("RECORDING");
		if(this.getSelDstItem() == "NONE") return;
		this.handleSrcDestSelectionForPlayback();   
  },
	
  /**
   * Tap source port list
   **/
	onSrcListTap:function(list, idx, target, record, evt){
		console.log('MainPanel:onSrcListTap:title: '+record.getData().title);
		//set as selected source
		this.getRecordinglist().deselectAll();
		this.setSelSrcItem(record.getData().nid);
		this.setSelSrcTitle(record.getData().title);
		this.setSelSrcStreamType(record.getData().streamType);
  	this.setSelSrcType("LIVE");
  	//check if destination is already selected or not
  	if(this.getSelDstItem() == "NONE") return;
  	this.handleSrcDestSelectionForView();    	     
  },

	/**
	 * Tap dest port list
	 **/
	onDstListTap:function(list, idx, target, record, evt){
		console.log('MainPanel:onDstListTap:title: '+record.getData().title);
  	var dstWinId = null;
  	//<img> alt has assigned to winId in displayConfig 
  	var itemParentNode = evt.target.parentNode;
  	if(itemParentNode.className.search("display-window") != -1) {
  		dstWinId = itemParentNode.id;
  		if(itemParentNode.parentNode.childElementCount == 1) {
  			this.isSingleWindow = true;
  		}
  		else {
  			this.isSingleWindow = false;
  		}  			
  	}
  	//check tap is on img btn to stop session
  	if(evt.target.nodeName.toUpperCase() === 'IMG' && evt.target.id == "imgDelete") {
  		dstWinId = this.getSelDstWin();
  	}  	
  	if(dstWinId == "" || dstWinId == null) {
  		list.deselectAll();
  		return false;
  	}
  	
  	var selDstSessionId = null;
  	var sessionData = this.getSessionData();
  	Ext.iterate(sessionData,function(key, sdata){
  		//check dst is already connected
  		if(sdata.PortConnectionInfo['@dstNID'] == record.data.nid){
  		  if(sdata.PortConnectionInfo['@windowId'] == dstWinId || this.isSingleWindow) {
  			  selDstSessionId = key;
  			  return false;
  			}  				  
  		}
  	}, this); 

		if(evt.target.nodeName.toUpperCase() === 'IMG' && evt.target.id == "imgDelete" && selDstSessionId != null) {
			console.log('MainPanel:onDstListTap:clicking imgDelete:selDstSessionId '+selDstSessionId);
			this.sendStopRequest(selDstSessionId);
		}
		else {
			//set as selected destination
			this.setSelDstItem(record.getData().nid);
			this.setSelDstTitle(record.getData().title);
			this.setSelDstStreamType(record.getData().streamType);
			this.setSelDstWin(dstWinId);
			//check if Src is already selected or not
			if(this.getSelSrcItem() == "NONE") {
				//If Src is not selected and session exists for this Dst then display img for stop session
				//User can select img for stop session or can select src to switch
				if(selDstSessionId != null) {
					console.log('MainPanel:onDstListTap:no selected src but has an exist session: '+selDstSessionId); 
					var element = (!target.dom ? target.innerElement : target);
					this.currentDeleteButton = element.down('.display-item-delete-hidden', true);
					this.currentDeleteButton.className="display-item-delete";
					//add a listener to the body, so we can hide the button if the user taps anywhere but the button.
					Ext.getBody().on('tap', this.onBodyTap, this);
				}
			}
			else {
				if (this.getSelSrcType() == "LIVE") {
					this.handleSrcDestSelectionForView();
				}
				else {
					this.handleSrcDestSelectionForPlayback();
				}
			}
		}
	},
 
	/**
	 * Helper function,hide 'x'/delete button on tapping 
	 * **/
	onBodyTap: function(e) {
		if (this.currentDeleteButton) {
			this.currentDeleteButton.className="display-item-delete-hidden";
		}
		this.currentDeleteButton = null;
		//remove the listener
		Ext.getBody().un('tap', this.onBodyTap, this);
	},

	/**
	 * Connect playback source to dest port 
	 **/
	handleSrcDestSelectionForPlayback:function() {
		var selDstSessionId = null;
		var sessionData = this.getSessionData();
		var isSrcDstAlreadyConnected = false;
		Ext.iterate(sessionData,function(key, sdata){
			//check dst is already connected
			if(sdata.PortConnectionInfo['@dstNID'] == this.getSelDstItem() && (sdata.PortConnectionInfo['@windowId'] == this.getSelDstWin() || this.isSingleWindow)){
				//check if selected src and dst already connected
				if(sdata.PortConnectionInfo['@srcNID'] == this.getSelSrcItem()) {
					isSrcDstAlreadyConnected = true;
					console.log('MainPanel:handleSrcDestSelectionForPlayback:isSrcDstAlreadyConnected ('+isSrcDstAlreadyConnected+'), fileNID: '+this.getSelSrcItem()+', dest:'+this.getSelDstItem()+', windowId:'+this.getSelDstWin());
					return false;
				}
				//switch src for the selected dst
				selDstSessionId = sdata["@id"];
			}
		}, this); 

		if(!isSrcDstAlreadyConnected) {
			var params = this.playbackReqPayload;
			params.Playback.SrcFile['@fileNID'] = this.getSelSrcItem();
    	params.Playback.SrcFile['@fileTitle'] = this.getSelSrcTitle();
			params.Playback.DstPort['@portNID'] = this.getSelDstItem();
    	params.Playback.DstPort['@portTitle'] = this.getSelDstTitle();
			params.Playback['@windowId'] = this.getSelDstWin();	        
			params.Playback['@sessionId'] = (selDstSessionId == null) ? this.getSessionIdForDstWin(this.getSelDstItem(), this.getSelDstWin()) : selDstSessionId;
			params.Playback['@restartSession'] = true;
			console.log('MainPanel:handleSrcDestSelectionForPlayback: fileNID: '+this.getSelSrcItem()+', dest:'+this.getSelDstItem()+', sessionId:'+params.Playback['@sessionId']);
			//send playback request
			this.sendPlaybackRequest(params);
		}
		else {
			Ext.defer(function () {
				this.getRecordinglist().deselectAll();
				this.getDestlist().deselectAll();
			},500,this);
		}
		this.setSelSrcItem("NONE");
		this.setSelDstItem("NONE");
		this.setSelSrcTitle("NONE");
		this.setSelDstTitle("NONE");	
		this.setSelSrcStreamType("NONE");
	  this.setSelDstStreamType("NONE");
		this.setSelDstWin("NONE");
	},
	
	/**
	 * Helper function, get sessionId by winId
	 **/
	getSessionIdForDstWin :function(dstNID, winId){
  	var sessioId = "Session" + dstNID.substring(dstNID.indexOf("_"));
		if(winId.indexOf("No_") != 0) {
			sessioId = sessioId + "_" + winId;
		}
		return sessioId;
	},
	
	/**
	 * Connect live source to dest port 
	 **/
	handleSrcDestSelectionForView:function() {
  	var selDstSessionId = null;
  	var sessionData = this.getSessionData();
  	var isSrcDstAlreadyConnected = false;
  	Ext.iterate(sessionData,function(key, sdata){
  		//check dst is already connected
  		if(sdata.PortConnectionInfo['@dstNID'] == this.getSelDstItem() && (sdata.PortConnectionInfo['@windowId'] == this.getSelDstWin() || this.isSingleWindow)){
  			//check if selected src and dst already connected
  			if(sdata.PortConnectionInfo['@srcNID'] == this.getSelSrcItem()) {
  				isSrcDstAlreadyConnected = true;
  				console.log('MainPanel:handleSrcDestSelectionForView:isSrcDstAlreadyConnected('+isSrcDstAlreadyConnected+'), src:'+this.getSelSrcItem()+', dest:'+this.getSelDstItem()+', windowId:'+this.getSelDstWin());
  				return false;
  			}
  			//switch src for the selected dst
  			selDstSessionId = sdata["@id"];
  		}
  	}, this);
  	
  	var srcStreamType = this.getSelSrcStreamType();
  	var	dstStreamType = this.getSelDstStreamType();  		
  	var streamTypeMatch = dstStreamType.indexOf(srcStreamType)>=0 ? true:false;
  	console.log('MainPanel:handleSrcDestSelectionForView: dstStreamType = '+dstStreamType+', srcStreamType = '+srcStreamType+'. ');
  	
  	!streamTypeMatch && Ext.Msg.alert('Session Error:', srcStreamType+' stream types can only connect to '+srcStreamType+' compatible destinations.');
  	
  	if(!isSrcDstAlreadyConnected && streamTypeMatch) {
  		var params = this.viewReqPayload;
  		params.View.SrcPort['@portNID'] = this.getSelSrcItem();
  		params.View.SrcPort['@portTitle'] = this.getSelSrcTitle();
  		params.View.DstPort['@portNID'] = this.getSelDstItem();
  		params.View.DstPort['@portTitle'] = this.getSelDstTitle();
  		params.View['@windowId'] = this.getSelDstWin();	      
  		params.View['@sessionId'] = (selDstSessionId == null) ? this.getSessionIdForDstWin(this.getSelDstItem(), this.getSelDstWin()) : selDstSessionId;
  		params.View['@restartSession'] = true;  		
  		console.log('MainPanel:handleSrcDestSelectionForView:sessionId:'+params.View['@sessionId']+', windowId:'+params.View['@windowId']+', src'+this.getSelSrcItem()+', dest:'+this.getSelDstItem());
  		//send view request
  		this.sendViewRequest(params);
  	}
  	else {
  		Ext.defer(function () {
  			this.getSourcelist().deselectAll();
  			this.getDestlist().deselectAll();
  		},500,this);
  	}  
  	this.setSelSrcItem("NONE");
  	this.setSelDstItem("NONE");
  	this.setSelSrcTitle("NONE");
  	this.setSelDstTitle("NONE"); 
  	this.setSelSrcStreamType("NONE");
  	this.setSelDstStreamType("NONE");
    this.setSelDstWin("NONE");
	},

	/**
	 * Send STOP session request
	 **/
	sendStopRequest:function(sessionId){
		if(sessionId){
			var	userData = this.userData;
			var ipAddress = userData.ipaddress,
				clientid = userData.clientid,
				username = userData.username,
			  	password = userData.password,
			  	urlStr = userData.urlBase+'/Session';
			    
		  var jsondata = '{"StopSession": {"@sessionId":'+sessionId+' }}'				
			//view request
			Ext.Ajax.request({
				url: urlStr,
				method:'POST',
				headers:this.httpHeader,
				jsonData:jsondata,
     		success: function(response){ 
    			console.log('MainPanel:sendStopRequest success.');		            
    			//remove the session from the sessionData array
    			delete this.getSessionData()[sessionId];
    			this.setSelSrcItem("NONE");
    			this.setSelDstItem("NONE");
    			this.setSelSrcTitle("NONE");
    			this.setSelDstTitle("NONE");          			
    			this.setSelDstWin("NONE");
     			this.setSelSrcStreamType("NONE");
     			this.setSelDstStreamType("NONE");
        	this.getDestlist().deselectAll();
     		}.bind(this),
       
     		failure: function(e){
     			console.log('MainPanel:sendStopRequest fails with error', e);
     			var msg = '<h30><i>'+e.statusText+'<b><br>';	
     			Ext.Msg.alert('Stop Session Error', msg);
     			this.setSelSrcItem("NONE");
     			this.setSelDstItem("NONE");
     			this.setSelSrcTitle("NONE");
     			this.setSelDstTitle("NONE");
     			this.setSelSrcStreamType("NONE");
     			this.setSelDstStreamType("NONE");
     			this.setSelDstWin("NONE");
     			this.getDestlist().deselectAll();
	     	}.bind(this)
		  });
		}
		else{
			console.log('MainPanel:sendStopRequest fails as no session data.');
		}
	},
	
	/**
	 * Send VIEW session request
	 **/
	sendViewRequest:function(params){
		if(params){
			var userData = this.userData,
				viewReq = this.viewReqPayload,
				ipAddress = userData.ipaddress,
				clientid = userData.clientid,
				username = userData.username,
			  	password = userData.password,
			  	urlStr = userData.urlBase+'/Session';
			var t1 = new Date().getTime();
			//view request
			Ext.Ajax.request({
				url: urlStr,
				method:'POST',
				headers:this.httpHeader,
				jsonData:params,
				success: function(response){ 
    			var t2 = new Date().getTime();
    			var respData = Ext.JSON.decode( response.responseText);
    			if(respData.View.Session.ConnectionList.Connection['@state'].search("ERROR")>=0){
    				//Session Errors: nested inside response          				
    				var errMsg = respData.View.Session.ConnectionList.Connection['@errorData'];
    				Ext.Msg.alert('Session Error',errMsg);
    			}
    			else{
    				console.log('MainPanel:sendViewRequest success, session id:' + respData.View.Session['@id']+', request urlStr: '+ urlStr +', request takes: '+(t2-t1)+' ms.');
    				this.handleSessionResponse(respData.View.Session);
    			}
    			//reset viewReq to initial status for next session
    			viewReq.View.SrcPort['@portNID'] = '';
    			viewReq.View.DstPort['@portNID'] = '';
    			viewReq.View.SrcPort['@portTitle'] = '';
      		viewReq.View.DstPort['@portTitle'] = '';          			
    			viewReq.View['@windowId'] = '';
			
    			Ext.defer(function () {
    				this.getSourcelist().deselectAll();
    				this.getDestlist().deselectAll();
    			},500,this);
				}.bind(this),
				
	  		failure: function(e){
	  			var t2 = new Date().getTime();
        	console.log('MainPanel:sendViewRequest fails, it takes '+(t2-t1)+'ms, request urlStr: '+urlStr+', error:'+e.statusText);
	  			//reset viewReq to initial status for next session
	  			viewReq.View.SrcPort['@portNID'] = '';
	  			viewReq.View.DstPort['@portNID'] = '';
        	viewReq.View.SrcPort['@portTitle'] = '';
          viewReq.View.DstPort['@portTitle'] = ''; 		  			
	  			viewReq.View['@windowId'] = '';
	  			Ext.defer(function () {
	  				this.getSourcelist().deselectAll();
	  				this.getDestlist().deselectAll();;	
	  			},500,this);		  					
	  			//Data Connection Errors: http 4xx and 5xx (Authorization,Server error) 
	  			var msg = '<h30><i>'+e.statusText+'<b><br>';	
	  			Ext.Msg.alert('View Error', msg);					
	  		}.bind(this)
			});
		}
	},
	
	/**
	 * Send PLAYBACK request
	 **/
  sendPlaybackRequest:function(params){
		if(params){
			var	userData = this.userData,
			    playbackReq = this.playbackReqPayload,
			    ipAddress = userData.ipaddress,
				clientid = userData.clientid,
				username = userData.username,
				password = userData.password,
				urlStr = userData.urlBase+'/Session';
			  	
			//view request
			Ext.Ajax.request({
				url: urlStr,
				method:'POST',
				headers:this.httpHeader,
				jsonData:params,
				success: function(response){ 
			  	//Session Errors: nested inside response
			  	var respData = Ext.JSON.decode( response.responseText);
			  	if(respData.Playback.Session.ConnectionList.Connection['@state'].search("ERROR")>=0){
			  		var errMsg = respData.Playback.Session.ConnectionList.Connection['@errorData'];
			  		Ext.Msg.alert('Playback Session Error',errMsg);
			  	}
			  	else{
			  		console.log('MainPanel:sendPlaybackRequest success.');	
			  		this.handleSessionResponse(respData.Playback.Session);
			  	}
  				//reset playbackReq to initial status for next session
  				playbackReq.Playback.SrcFile['@fileNID'] = '';
  				playbackReq.Playback.DstPort['@portNID'] = '';
  				playbackReq.Playback.SrcFile['@fileTitle'] = '';
  				playbackReq.Playback.DstPort['@portTitle'] = ''; 					
  				playbackReq.Playback['@windowId'] = '';  			
  				Ext.defer(function () {
  	      	this.getRecordinglist().deselectAll();
  	      	this.getDestlist().deselectAll();
  				}, 500, this);
		  	}.bind(this),
				
		  	failure: function(e){
		  		//reset playbackReq to initial status for next session
		  		playbackReq.Playback.SrcFile['@fileNID'] = '';
		  		playbackReq.Playback.DstPort['@portNID'] = '';
		  		playbackReq.Playback.SrcFile['@fileTitle'] = '';
		  		playbackReq.Playback.DstPort['@portTitle'] = ''; 				  		
		  		playbackReq.Playback['@windowId'] = '';
		  		Ext.defer(function () {
		  			this.getRecordinglist().deselectAll();
		  			this.getDestlist().deselectAll();;	
		  		},500,this);		  					
		  		//Data Connection Errors: http 4xx and 5xx (Authorization,Server error) 
		  		var msg = '<h30><i>'+e.statusText+'<b><br>';	
		  		Ext.Msg.alert('Playback Error', msg);					
		  	}.bind(this)
			});
		}
	},
	
	/**
	 * Handle session response from VIEW or PLAYBACK
	 **/
	handleSessionResponse: function(sessionInfo) {
  	var portSessionInfo = {
  			"@id": "",
  			"@name": "",
  			"PortConnectionInfo": {
				"@canvasId": "",
  			"@dstNID": "",
  			"@dstTitle": "",
  			"@errorCode": "",
  			"@errorData": "",
  			"@id": "",
  			"@name": "",
  			"@srcNID": "",
  			"@srcTitle": "",
  			"@state": "",
  			"@type": "",
  			"@windowId": ""
			}                      	
		}	    
  	portSessionInfo["@id"] = sessionInfo['@id'];
    portSessionInfo["@name"] = sessionInfo['@name'];    
    portSessionInfo.PortConnectionInfo["@canvasId"] = sessionInfo.ConnectionList.Connection['@canvasId'];
    portSessionInfo.PortConnectionInfo["@id"] = sessionInfo.ConnectionList.Connection['@id'];
    portSessionInfo.PortConnectionInfo["@name"] = sessionInfo.ConnectionList.Connection['@name'];    
    portSessionInfo.PortConnectionInfo["@state"] = sessionInfo.ConnectionList.Connection['@state'];
    portSessionInfo.PortConnectionInfo["@type"] = sessionInfo.ConnectionList.Connection['@type'];
    portSessionInfo.PortConnectionInfo["@windowId"] = sessionInfo.ConnectionList.Connection['@windowId'];    
    var arrEntity = sessionInfo.EntityList.Entity;
    Ext.iterate(sessionInfo.EntityList.Entity, function(entity){
    	if(entity['@id'] == sessionInfo.ConnectionList.Connection['@dstEntityId']) {
    		portSessionInfo.PortConnectionInfo["@dstNID"] = entity.Port["@resourceNID"];
    		portSessionInfo.PortConnectionInfo["@dstTitle"] = entity.Port["@resourceTitle"];
    	}
    	else if(entity['@id'] == sessionInfo.ConnectionList.Connection['@srcEntityId']){
    		var srcEntity = (entity["@type"] == "PlyFile") ? entity.File : entity.Port;
    		portSessionInfo.PortConnectionInfo["@srcNID"] = srcEntity["@resourceNID"];
    		portSessionInfo.PortConnectionInfo["@srcTitle"] = srcEntity["@resourceTitle"];
    	}
    }, this);  
    this.getSessionData()[sessionInfo['@id']] = portSessionInfo;
	},
	
	/**
	 * Drop down select in mainpage
	 **/
  onSelectChange: function(selectfield, newValue, oldValue, options) {
   	//selectfield.getOptions() return [{text:'Logout',value:'Logout'},{text:'Settings',value:'Settings'}]
		//newValue:'Settings',oldValue:'Logout'
		switch (newValue) {
			case 'Logout':
				console.log('MainPanel:select Logout: '+newValue);
				this.doLogout();
				selectfield.blur();
				selectfield.setValue(null);
				//froward to mainscreen
				this.appContainer.setActiveItem('#loginscreen');
				var loginFormPanel = this.loginPanel.down('formpanel');
				loginFormPanel.setValues({password:''});//loginFormPanel.setValues({username:'',password:''});
			break;
			case 'Settings':
				console.log('MainPanel:select Settings: '+newValue);
				//logout in order to re-login 
		    this.doLogout();
				this.doSetup();
				selectfield.blur();
				selectfield.setValue(null);
			break;
			default:				
		}
	},

	/**
	 * Logout function from drop down select
	 **/
	doLogout:function(){
		//clear local data
		//logout in order to re-login 
		this.loginController.sendLogoutRequest({
			url:this.userData.urlBase,
			httpHeader:this.httpHeader,
			successCallBack:function(){
    	  this._onLogoutSuccess();
    	  console.log('MainPanel:doLogout:success');
  		}.bind(this)   
		});
		//reset this.userData to null
		this.userData = null;
	},
	
	/**
	 * Cleanup UI on logout success
	 */	
	_onLogoutSuccess:function(e) {
		//remove the user from local storage for both logout and kickout 
		if(!(e && e.status==0 && e.statusText=="" && e.responseText=="")){
			//exclude F5 refresh
    	var record = this.loginController.loginStore.findRecord('username',this.loginController.getUserData().username);
		  record && this.loginController.loginStore.remove(record);
		}
  	this.getSourcelist().getStore().removeAll();
  	this.getSourcelist().getStore().sync();
  	this.getDestlist().getStore().removeAll();
  	this.getDestlist().getStore().sync();
  	this.getRecordinglist().getStore().removeAll();
  	this.getRecordinglist().getStore().sync();
  	this.setSessionData({});
  	//clean display status array 
  	this.srcDisplayStatusArr = {};
  	this.recDisplayStatusArr = {};
  	this.destDisplayStatusArr = {};	
  	//clean tn url timer
  	this.clearUpTnUrlTimer(this.srcTnTimerArr);
  	this.srcTnTimerArr = [];
  	this.clearUpTnUrlTimer(this.destTnTimerArr);
  	this.destTnTimerArr = [];
  	//clean long polling Ajax request immediately 
  	//this.notificationHandlerController.setLongpollingAborted(true);
  	//Ext.Ajax.abort(this.notificationHandlerController.longpollingRequestRef);
  },
	
  /**
   * Logout on any communication errors like Server Down/Timeout etc..
   */
  doLogoutOnError:function(e){
    this._onLogoutSuccess(e);
   	console.log('MainPanel:doLogoutOnError');
    //froward to mainscreen
    this.appContainer.setActiveItem('#loginscreen');
    var loginFormPanel = this.loginPanel.down('formpanel');
    loginFormPanel.setValues({password:''});
    //reset this.userData to null
    this.userData = null;
    var msg = '';
    if(e.kickOut){
  	  Ext.Msg.alert('','<h30><i> Session Closed <b><br> This user has started a session on another device. ');
    }
    else{
      if(e.timedout){
  	    msg = '<h30><i> Session Closed <b><br> Request timed out.';
      }
      else if (e.aborted){
  	    msg = '<h30><i> Session Closed <b><br> Request stopped.';
      }
      else if (e.responseText.indexOf('Invalid token')>=0){
  	    msg = '<h30><i> Session Closed <b><br> This user may have started a session on another device or the connection to the server was lost.';
      }
      else if (e.status == 502 || e.status == 503 ){
      	//'Proxy Error' || 'Service Unavailable'
      	msg = '<h30><i> Session Closed <b><br> The server may be down temporarily – please try again. ';
      }
      else{
     		msg = e.responseText;
     		console.log('MainPanel:doLogoutOnError, e.status:'+e.status+' ,requestId: '+e.requestId+' ,e.responseText: '+e.responseText+', e.statusText:'+e.statusText );
      }	
      Ext.Msg.alert('Server Connection Error', msg);
    }
  },

	/**
	 * Clear thumbnail URL timer by nid
	 **/
	clearUpTnUrlTimer:function(tnUrlTimerArr,nid){
  	//clean thumbnail url timer
  	if((tnUrlTimerArr instanceof Array) && (tnUrlTimerArr.length>0) ){
  		if(typeof nid !== 'undefined'){
  			//clean single tn url timer by nid
  			for(var j=0; j<tnUrlTimerArr.length;j++ ){
  				var obj1 = tnUrlTimerArr[j];
  				if(obj1.nid==nid){
  					var tnSpliceArr = tnUrlTimerArr.splice(j,1);
  					clearInterval(tnSpliceArr[0].timerId);
  					console.log(' MainPanel:clearUpTnUrlTimer: timerId: '+tnSpliceArr[0].timerId+', -- nid: '+nid);
  					break;	
  				}
  			};
  		}
  		else{
  			//clean all tn url timer
  			console.log('MainPanel:clear all tnUrlTimer. ');
  			for(var i=0; i<tnUrlTimerArr.length;i++ ){
  				clearInterval(tnUrlTimerArr[i].timerId);
  			};
  		}
  	}
	},
	
	/**
	 * Setup from main page drop down select
	 **/
	doSetup:function(){		
		console.log('MainPanel:doSetup.');
		this.appContainer.setActiveItem('#settingsscreen');		
		var store = Ext.data.StoreManager.lookup('settingsStoreId');
		if(store){
			values = store.last();
			//to get model instance, need setRecord to form panel
			this.setupPanel.down('formpanel').setRecord(store.last());
		}		
	},

	/**
	 * Load recording list
	 **/
	loadRecordingList:function(){
		var rcdlist = this.getRecordinglist();
		var rcdListStore = rcdlist.getStore();
		rcdListStore.setPageSize(this.pageSize);
		//startFrom=1&countToFetch=25&
		var urlStr1 = this.userData.urlBase+'/Resource/File?whereType=MediaClip&whereState=Ready&sortBy=dateCreated&sortOrder=descending&subscriptionID='+this.loginController.rcdSubId +'&selectFormat=BASICURL&wherePermissions=PLAYBACK';
		rcdListStore.getProxy().setUrl(urlStr1);
		rcdListStore.getProxy().setHeaders(this.httpHeader);	
		rcdlist.setStore(rcdListStore);
		rcdListStore.load();
		console.log('MainPanel:loadRecordingList: get recordings files by: '+urlStr1);
	},

	/**
	 * Load source port list
	 **/
	loadSrcList:function(whereClause){
		if(!this.userData){
			//get login userdate/httpHeader after login success
			this.userData = this.loginController.getUserData();	
			this.httpHeader = this.loginController.getHttpHeader().httpHeader;
		}
		//dynamic put url and headers, call  with subscriptionID to get notification
		var srcListStore = this.srclist.getStore();
		//keep same selectFormat as the dummy request, add whereState and remove subscriptionID
		var urlStr1 = this.userData.urlBase+'/Resource/Port?type=SrcPort&subscriptionID='+this.loginController.srcSubId+'&subscriptionScope=ALLNODES&whereState=Ready&selectFormat=BASICCONNECTIONINFO&wherePermissions=VIEWFROM';
		srcListStore.getProxy().setUrl(urlStr1);	
		console.log('MainPanel:loadSrcList: get all src ports by: '+urlStr1);
		srcListStore.getProxy().setHeaders(this.httpHeader);
		srcListStore.filter(Ext.create('Ext.util.Filter', {
                                    	filterFn: function(item) {
                                    		return item.get('streamType') != 'RFB';  
                                    	}
									   							})
											 );
		this.srclist.setStore(srcListStore);
		srcListStore.load({
			callback:function(records,operation,success){
				this.loadDestList();
				//process ports to handle connections
				var srcRecords = srcListStore.getRange();
		  	Ext.iterate(srcRecords,function(record){
		  		//add timer to get thumbnail url repeatedly
		  		var tnUrl = record.raw['@thumbnailURL'],nid = record.raw['@NID'];
		  		var streamType = record.data.streamType;
		  		if(tnUrl && streamType !='RFB' && streamType !='RTP-PCM'){
    		  	//fix mobile Safari on iOS6 image update issue 
  	  		  var strArr = tnUrl.split('/mediaportal/');
  	  		 	tnUrl = strArr[0]+record.data.tnurlport+'/mediaportal/'+strArr[1];
    		  	this.setSrcTnUrlRepeatedly(tnUrl,nid);	
		  		}
		  		//update port connection status
		  		this.processPortConnectionStatus(record.raw);
		  	}, this);
		  },
			scope:this
		});
	},		
	
	/**
	 * Helper function, poll source port thumbnail URL each 4 seconds 
	 **/
	setSrcTnUrlRepeatedly:function(tnUrl,nid){
		var imgEl = Ext.DomQuery.selectNode('img[id="img_'+nid +'"]', Ext.get('livepanel').dom);
		if(imgEl!==null){
			var timerId = setInterval(
					function(){
						imgEl.src = tnUrl;
					},4000);
			console.log('MainPanel:setSrcTnUrlRepeatedly timer for '+nid);
			this.srcTnTimerArr.push({"nid":nid,"timerId":timerId}); 
		}
	},
	
	/**
	 * Load dest port list
	 **/
	loadDestList:function(whereClause){	
		//get destination port request
		var dstListStore = this.dstlist.getStore();
		//keep same selectFormat as the dummy request, add whereState and remove subscriptionID
		var urlStr1 = this.userData.urlBase+'/Resource/Port?type=DstPort&subscriptionID='+this.loginController.destSubId+'&subscriptionScope=ALLNODES&whereState=Ready&selectFormat=BASICCONNECTIONINFO&wherePermissions=STREAMTO';
		dstListStore.getProxy().setUrl(urlStr1);	
		console.log('MainPanel:loadDestList: get all dest ports by: '+urlStr1);
		dstListStore.getProxy().setHeaders(this.httpHeader);
		dstListStore.filter(Ext.create('Ext.util.Filter', {
                              		    filterFn: function(item) {
                            	        	return item.get('streamType') != 'RFB';  
                                    	}
                          		    })
          						 );
		this.dstlist.setStore(dstListStore);
		dstListStore.load({
			callback:function(records,operation,success){
				if(success){
					//UNCOMMENT the following to start Display config
					this.processPortDisplayConfig();  		 			
				}
			},
			scope:this
		});
		
	},

	/**
	 * Changes Src list Item style based on connection state
	 * @recId {string}
	 * @state {string}
	 **/
	updateSrcOnConnState:function(recId, state) {
		var srcListItemDom = Ext.DomQuery.selectNode('div[id="'+ recId +'"]', Ext.get('livepanel').dom);
		if(srcListItemDom) {
			var clsName = "live-item-thumb";
			if(state == "STREAMING"){
				clsName="live-item-thumb connected";
			}
			if(srcListItemDom.className != clsName) {
				srcListItemDom.className=clsName;
				//keep current source status 
				this.srcDisplayStatusArr[recId] = srcListItemDom.outerHTML;
			}
			console.log('MainPanel:updateSrcOnConnState: id:'+recId+', state:'+state+', CSS:'+clsName);
		}
	},

	/**
	 * Changes Recording list Item style based on connection state
	 * @recId {string}
	 * @state {string}
	 **/
	updateRecOnConnState:function(recId, state) {
  	var recListItemDom = Ext.DomQuery.selectNode('div[id="'+ recId +'"]', Ext.get('recordingpanel').dom);
  	if(recListItemDom) {
  		var clsName = "rec-item-thumb";
  		if(state == "STREAMING"){
  			clsName="rec-item-thumb connected";
  		}
  		if(recListItemDom.className != clsName) {
  			recListItemDom.className=clsName;
  			//keep current recording status 
  			this.recDisplayStatusArr[recId] = recListItemDom.innerHTML;
  		}
  		console.log('MainPanel:updateRecOnConnState: id:'+recId+', state:'+state+', CSS:'+clsName);
  	}
	},

	/**
	 * Changes Window/Dest style based on connection state
	 * @dstId {string}
	 * @winId {string}
	 * @state {string}
	 * @srcTitle {string}
	 **/
	updateDstOnConnState:function(dstId, winId, state, srcId) {
	  if(dstId == "" || dstId == null) return;
		if(winId == "") winId = dstId;		
		var dstListItemDom = Ext.DomQuery.selectNode('div[id="'+ dstId+'"]', Ext.get('destpanel').dom);
		if(dstListItemDom) {
  		var dstWinDom = Ext.DomQuery.selectNode('div[id="'+ winId +'"]', dstListItemDom);
  		if(!dstWinDom && dstListItemDom.childElementCount == 1) {
  			dstWinDom = dstListItemDom.children[0];
  			if(dstWinDom.className.search("display-window") == -1){
  				dstWinDom = null;
  			}  			
  		}
  		if(dstWinDom) {
  			var clsName = "display-window";
  			var srcName = "";
  			var thumbnailSrc = null;
  			if(state == "STREAMING"){
  				clsName="display-window connected";
  				thumbnailSrc = this.getSelSrcTnUrl(srcId);
  			}
  			else if(state.indexOf('_ERROR')>=0){
  				thumbnailSrc = 'http://'+location.host+"/clients/tablet/app/resource/image/icon_error.png";
  			}
  			else {
    			if (Ext.os.is.Windows) {
    			  thumbnailSrc = "data:image/gif;base64,R0lGODlhAQABAAD/ACwAAAAAAQABAAACADs%3D"; 
    			}
    			else{
  	  			// iPad, iPod, iPhone, android etc.
  	  			thumbnailSrc = "data:image/gif;base64,AN_ENCODED_IMAGE_DATA_STRING"; 
    			}
  			} 
  			
  			if(dstWinDom.className != clsName) {
  				Ext.Function.defer(function(){dstWinDom.className=clsName;},0,this);
  			}
  			
  			if(dstWinDom.firstElementChild.src != thumbnailSrc) {
  				dstWinDom.firstElementChild.src = thumbnailSrc;
  				this.forceElRepaint(dstWinDom);
  				if(state == "STREAMING") {
  					//start tn timer
  					this.setDestTnUrlRepeatedly(thumbnailSrc,dstId,winId,dstWinDom);
  				}
  				else {
    				//stop dest thumbnail request timer by destNID and winId
  					var compbine_nid = dstId+'_'+winId;
  					this.clearUpTnUrlTimer(this.destTnTimerArr,compbine_nid);
  				}  				
  			}
  			console.log('MainPanel:updateDstOnConnState: destId: '+ dstId +', winId: '+winId+',srcId: '+srcId+', state:'+state+', CSS:'+clsName);
  		}
  		//keep current destination status as well as DOM ref.
  		this.destDisplayStatusArr[dstId] = dstListItemDom.innerHTML;	   
  	}
	},
	
	/**
	 * Helper function, get source URL by source NID 
	 **/
	getSelSrcTnUrl:function(srcNID) {
		var srcRec = this.getSourcelist().getStore().findRecord('nid', srcNID);
    if(srcRec == null) {
    	srcRec = this.getRecordinglist().getStore().findRecord('nid', srcNID);
    }
    var thumbnailURL = null;
    if(srcRec != null){
    	thumbnailURL =  srcRec.raw['@thumbnailURL'] ? srcRec.raw['@thumbnailURL'] : srcRec.data['thumbnailurl'];
    	if(thumbnailURL && thumbnailURL.indexOf('mediaportal') >=0){
    		//"http://10.51.51.19/mediaportal/thumbnail?sourceNID=default.mediastreamioportresourcelist.msioprl_b6893ce5-c1da-4276-b88c-3a6e54e089eb&apiKey=5535362444744999678&timestamp=1373329031603&signature=7344f46a3c9ccefa50a75c62ccc16a87"
    		var strArr = thumbnailURL.split('/mediaportal/');
    		thumbnailURL = strArr[0]+srcRec.data.tnurlport+'/mediaportal/'+strArr[1];	
    	}
    }
    return thumbnailURL;
	},
		
	/**
	 * Helper fucntion, poll dest port thumbnail each 4 seconds  
	 **/
	setDestTnUrlRepeatedly:function(tnUrl,nid,winId,dstWinDom){
		var imgEl = Ext.DomQuery.selectNode('img[id="img_'+nid +'"]', dstWinDom);
		if(imgEl && tnUrl){
			if(tnUrl.indexOf('mediaportal')>=0){
    		var timerId = setInterval(
    				function(){
    					imgEl.src = tnUrl;
    				},4000);
    		var destId = 	nid+'_'+winId;
    		this.destTnTimerArr.push({"nid":destId,"timerId":timerId}); 
    		console.log('MainPanel:setDestTnUrlRepeatedly: nid: '+ nid +', winId: '+winId+',tnUrl: '+tnUrl+', timerId:'+timerId);
			}
			else {
				imgEl.src = tnUrl;
				console.log('MainPanel:setDestTnUrl with a static img: nid: '+ nid +', winId: '+winId+',tnUrl: '+tnUrl);
			}
		}
	},	  
	
	/**
	 * Notification handler function to process port session info
	 * @portSessionInfo
	 **/
	processPortSessionInfo:function(portSessionInfo) {
  	var portConnectionInfo = portSessionInfo.PortConnectionInfo;
  	if(portConnectionInfo['@dstNID'] == "" || portConnectionInfo['@dstNID'] == null || portConnectionInfo['@type'] == "PVR") return;
  	var srcState = portConnectionInfo['@state'];
  	var dstState = portConnectionInfo['@state'];
  	this.getSessionData()[portSessionInfo['@id']] = portSessionInfo;    
  	var dstDisplayStr = portConnectionInfo['@srcNID'];
  	console.log('MainPanel:processPortSessionInfo: Session id:' + portSessionInfo['@id'] + ' State:' + portConnectionInfo['@state']);
  	if(portConnectionInfo['@state'] != "STREAMING") {
  		//check if the same src has connected to other dst
  		var sessionData = this.getSessionData();
  		Ext.iterate(sessionData,function(key, sdata){
  			if(sdata.PortConnectionInfo['@srcNID'] == portConnectionInfo['@srcNID']) {
  				if(sdata.PortConnectionInfo['@state'] == "STREAMING") {
  					srcState = sdata.PortConnectionInfo['@state'];
  					return false;
  				}
  			}
  		}, this);
    
  		if(portConnectionInfo['@errorCode'] != null && portConnectionInfo['@errorCode'] != "") {
  			dstDisplayStr = "_ERROR_";
  		}  
  	}
  	this.updateSrcOnConnState(portConnectionInfo['@srcNID'], srcState);
  	this.updateRecOnConnState(portConnectionInfo['@srcNID'], srcState);
  	this.updateDstOnConnState(portConnectionInfo['@dstNID'], portConnectionInfo['@windowId'], dstState, dstDisplayStr);
	},

  /**
   * Notification handler function to process port connection status
   * @portNode
   **/
  processPortConnectionStatus:function(portNode){
		if(portNode.ConnectionStatus) {
  	  var portSessionInfo = portNode.ConnectionStatus.PortSessionInfo;	
  	  console.log('MainPanel:processPortConnectionStatus:calling processPortSessionInfo');
  		if(Array.isArray(portSessionInfo)){
  			//many Notifications return in an array, if single no array return
  			Ext.iterate(portSessionInfo,function(sessionInfo){
  				this.processPortSessionInfo(sessionInfo);	
  		  }, this);	
  		}
  		else{
  			//single notification
  			this.processPortSessionInfo(portSessionInfo);				
  		}
		}
	},

	/**
	 * Notification handler function to process port connection state 
	 **/
	connStateHandler:function(notifyNode){
		var connId = notifyNode['@connId'],
			sessionId = notifyNode['@sessionId'],
			streamURL = notifyNode['@streamURL'],	
			value = notifyNode['@value'],
			timelineNode = notifyNode.Timeline;
		console.log('MainPanel:connStateHandler:connId: '+connId+',sessionId: '+sessionId+', streamURL:'+streamURL);			
		/*
		 	@connId: "2587e41e-76aa-4f93-b186-540c07388312"
			@sessionId: "8cbfb559-ecc4-41e1-995a-79731f27a88b"
			@streamURL: ""
			@value: "WAITING_FOR_STREAMSTART"
			Timeline: Object
		 * */
		
		/*
		 	timelineNode : Object
				@id: "591f3570-2ecb-4d13-9dca-c5ca87bce9b9"
				@ssInstanceID: 2017
				Last: Object
					@Offset: 204
					@TC: 1366154235866
					@TimeScale: 1
					@WC: 1366154235866
					@requestedAction: "Start"
				Start: Object
					@Offset: ""
					@TC: 1366154235866
					@WC: 1366154235866
					@parentTimelineId: ""
		 * */
	},
	
	/**
	 * Notification handler function to process port connection update
	 **/
	portConnectionUpdateHandler:function(notifyNode){
		console.log('portConnectionUpdateHandler notification ');
  	var portSessionInfo = notifyNode.Port.ConnectionStatus.PortSessionInfo;
  	var portConnectionInfo = portSessionInfo.PortConnectionInfo;
  	if(portConnectionInfo['@dstNID'] == "" || portConnectionInfo['@dstNID'] == null || portConnectionInfo['@type'] == "PVR") return;
  	var srcState = portConnectionInfo['@state'];
  	var dstState = portConnectionInfo['@state'];
  	var dstDisplayStr = portConnectionInfo['@srcNID'];
  	var connId =  portConnectionInfo['@id'];
  	console.log('MainPanel:portConnectionUpdateHandler:: Session id:' + portSessionInfo['@id'] + ' State:' + portConnectionInfo['@state'] + ' Operation:' + notifyNode.Port['@operation']+' ConnId: '+connId);

  	if(notifyNode.Port['@operation'] == "Delete") {
  		if(this.getSessionData()[portSessionInfo['@id']]) {
  			delete this.getSessionData()[portSessionInfo['@id']];
  			//this.sendStopRequest(portSessionInfo['@id']); 
  		}
  		srcState = "None";
  		dstState = "None";
  	}
  	else {
  		this.getSessionData()[portSessionInfo['@id']] = portSessionInfo;
  	}
  	
  	if(srcState != "STREAMING"){
  		//check if the same src has connected to other dst
  		var sessionData = this.getSessionData();
  		Ext.iterate(sessionData,function(key, sdata){
  			if(sdata.PortConnectionInfo['@srcNID'] == portConnectionInfo['@srcNID'] && sdata.PortConnectionInfo['@state'] == "STREAMING") {
  				srcState = sdata.PortConnectionInfo['@state'];
  				return false;
  			}
  		}, this);
  		
  		if(portConnectionInfo['@errorCode'] != null && portConnectionInfo['@errorCode'] != "") {
  			dstDisplayStr = "_ERROR_";
  		}  
  	}    
  	this.updateSrcOnConnState(portConnectionInfo['@srcNID'], srcState);
  	this.updateRecOnConnState(portConnectionInfo['@srcNID'], srcState);
  	this.updateDstOnConnState(portConnectionInfo['@dstNID'], portConnectionInfo['@windowId'], dstState, dstDisplayStr);
	/*
	 Port: Object
		@NID: "default.mediastreamsrcportresourcelist.mssprl_3784b717-f4bf-443e-a0fe-dafa46687f39"
		@operation: "Add"
		@subscriptionID: 123456
		ConnectionStatus: Object
		PortSessionInfo: Object
			@id: "8cbfb559-ecc4-41e1-995a-79731f27a88b"
			@name: "sunuser3 Apr 16 22:47"
			PortConnectionInfo: Object
			@canvasId: "8cbfb559-ecc4-41e1-995a-79731f27a88b"
			@dstNID: "default.mediastreamioportresourcelist.msioprl_9c678440-74c3-4b3f-a6cc-8be526390ab0"
			@dstTitle: "xp200514844_CHANNEL2"
			@errorCode: ""
			@errorData: ""
			@id: "2587e41e-76aa-4f93-b186-540c07388312"
			@name: "Connection_2587e41e-76aa-4f93-b186-540c07388312"
			@srcNID: "default.mediastreamsrcportresourcelist.mssprl_3784b717-f4bf-443e-a0fe-dafa46687f39"
			@srcTitle: "tx515012"
			@state: ""
			@type: "VIEW"
			@windowId: "2587e41e-76aa-4f93-b186-540c07388312"
	 */		
	},
	
	/**
	 * Notification handler function to process port connection state update 
	 **/
	portConnectionStateUpdateHandler:function(notifyNode){
		var portNode = notifyNode.Port,
			portNID = portNode['@NID'],
			connStateNode = portNode.ConnectionState,
			connId = connStateNode['@connId'],
			sessionId = connStateNode['@sessionId'],
			value = connStateNode['@value'];
			//timelineNode = 	connStateNode.Timeline;	
		console.log('MainPanel:portConnectionStateUpdateHandler:: Session id:' + sessionId +' ConnectionId: ' +connId+ ' State:' + value);
    var sessionData = this.getSessionData()[sessionId];
    if(sessionData) {
    	sessionData.PortConnectionInfo['@state'] = value;
    	this.updateSrcOnConnState(sessionData.PortConnectionInfo['@srcNID'], value);
    	this.updateRecOnConnState(sessionData.PortConnectionInfo['@srcNID'], value);
    	var dstDisplayStr = sessionData.PortConnectionInfo['@srcNID'];
    	if( connStateNode['@errorCode'] != null && connStateNode['@errorCode'] != "") {
    		dstDisplayStr = "_ERROR_";
    		console.log('MainPanel:portConnectionStateUpdateHandler:: Session id:' + sessionId +' ConnectionId: ' +connId+' errorCode:' + connStateNode['@errorCode']);
    	}
    	this.updateDstOnConnState(sessionData.PortConnectionInfo['@dstNID'], sessionData.PortConnectionInfo['@windowId'], value, dstDisplayStr);
    }
		/*
		 notifyNode: Object
		Port: Object
		@NID: "default.mediastreamioportresourcelist.msioprl_9c678440-74c3-4b3f-a6cc-8be526390ab0"
		@subscriptionID: "1366236552948,1366239448284,1366239171274,1366238794615,1366241126465,1366239944790,1366238147687,1366240110821"
		ConnectionState: Object
			@connId: "0de3c1b5-68f4-4051-af32-6b716bcc90bf"
			@sessionId: "8c6076a2-2478-414e-ae3a-cae171cf5f8f"
			@value: "STREAMING"
			Timeline: Object
				@id: "4ad59b01-26a3-41cb-b93d-2ca02da7f3f6"
				@ssInstanceID: 2009
				Last: Object
					@Offset: 213
					@TC: 1366242324327
					@TimeScale: 1
					@WC: 1366242324327
					@requestedAction: "Start"
				Start: Object
					@Offset: ""
					@TC: 1366242507117
					@WC: 1366242507117
					@parentTimelineId: ""
		 * */
	},
	
    /**
     * Notification handler function to process add new port 
     **/
	portAddHandler:function(nid,portType){
    //for add port update notification only have '@NID', '@portType' and '@operation'
		var urlStr = "";
		if(portType=="StreamSrc"){
			urlStr = this.userData.urlBase+'/Resource/Port?type=SrcPort&subscriptionID='+this.loginController.srcSubId+'&subscriptionScope=ALLNODES&selectFormat=BASICCONNECTIONINFO&wherePermissions=VIEWFROM&whereNID='+nid;
		}
		else{
			urlStr = this.userData.urlBase+'/Resource/Port?type=DstPort&subscriptionID='+this.loginController.destSubId+'&subscriptionScope=ALLNODES&selectFormat=BASICCONNECTIONINFO&wherePermissions=STREAMTO&whereNID='+nid;
		}
		Ext.Ajax.request({
			url:urlStr,
			method:'GET',
    	headers:this.httpHeader,
    	scope:this,
    	success: function(response){ 
    		console.log('Get port by NID request success, nid=' + nid);
    		var respData = Ext.JSON.decode(response.responseText),
    			portNode = !respData.Resource.Port[0]?respData.Resource.Port:respData.Resource.Port[0],
    			presence = portNode['@presence'],
    			state = portNode['@state'],
    			title = portNode['@title'];
    		if(presence == "available" && state =="Ready"){
    			if(portType=="StreamSrc") {
        			//1)If src: get thumbnail (if have otherwise give local default img) , add to store/list
    				var tnUrl = portNode['@thumbnailURL'] ? portNode['@thumbnailURL'] : "./app/resource/image/src-thumb-video-online.png"; 
    				this.sourcePortStore.add({nid:nid,title:title,presence:presence,portType:portType,streamType:portNode['@streamType'], state:state, '@thumbnailURL':tnUrl});
    				this.sourcePortStore.sync();
    				//this.getSourcelist().refresh();
    				this.setSrcTnUrlRepeatedly(tnUrl, nid);
        		}
        		else {
        			//2)If dest: get display config, add to store/list
        			var hasDisplayConfig = this._getDisplayConfigByFeaturelist( portNode['FeatureList']);
        			this.dstPortStore.add({nid:nid,title:title,presence:presence,portType:portType,streamType:portNode['@streamType'],state:state,hasDisplayConfig:hasDisplayConfig});
        			this.dstPortStore.sync();
        			//process ConnectionStatus for new add in dest port (normally don't have);
        			this.processPortDisplayConfigByNid(nid,hasDisplayConfig,portNode);
        		}	
      		}
    		},
    		failure: function(e){
    			console.log('Get port by NID request fail, nid=' + nid);
    			var msg = '<h30><i>'+e.statusText+'<b><br>';	
    			Ext.Msg.alert('Get Port Error', msg);
    		}
		}); 
	},
	 
	/**
	 * Notification handler function to process port update 
	 **/
	portUpdateHandler:function(notifyNode){
		var portNode = notifyNode.Port,
			//basic common part for all ports
			nid = portNode['@NID'],
			portType = portNode['@portType'],	
			presence = portNode['@presence'],
			operation = portNode['@operation'],	
			title = portNode['@title'],	
			state = portNode['@state'];
		
		if(!this.sourcePortStore) this.sourcePortStore =  this.getSourcelist().getStore();
		if(!this.dstPortStore) this.dstPortStore =  this.getDestlist().getStore();			
		if(operation=="Add") {
			this.portAddHandler(nid,portType);
			return;
		}
		console.log('MainPanel:portUpdateHandler notification: presence:'+presence+', state:'+state+', portType:'+portType+', title:'+title+', nid:'+nid);
		if(presence == "unavailable" && state =="Offline"){
			//remove from the list
			if(portType == 'StreamSrc'){
				var record = this.sourcePortStore.findRecord('nid',nid);
				record && this.sourcePortStore.remove(record);
				delete this.srcDisplayStatusArr[nid];				
				//stop src port thumbnail request timer by src NID
				this.clearUpTnUrlTimer(this.srcTnTimerArr,nid);
			}
			else if(portType == 'StreamDst'){
				var record = this.dstPortStore.findRecord('nid',nid);
				record && this.dstPortStore.remove(record);
				delete this.destDisplayStatusArr[nid];				
				//stop dest port thumbnail url request timer, by destNID combin with winId
				var destWinArr = this.displayConfigController.getDestWindowArr();
				var arrLength = destWinArr.length;
				for(var j=0; j<arrLength;j++ ){
					if(destWinArr[j] && (destWinArr[j].nid == nid)){
			    	var objArr = destWinArr.splice(j,1);
			        	nid = objArr[0].nid,
			        	winId = objArr[0].windowid,
			        	combine_id = nid+'_'+winId;
			      this.clearUpTnUrlTimer(this.destTnTimerArr,combine_id);
				  }
				}
				//manage dest port window array after the dest port offline
				this.displayConfigController.setDestWindowArr(destWinArr);
			}				
		}//offline		
		  			
		if(presence == "available" && state =="Ready"){
			//add in the list
			if(portType == 'StreamSrc'){
				if(this.sourcePortStore.getCount()>0){
					var record = this.sourcePortStore.findRecord('nid',nid);
					record && this.sourcePortStore.remove(record);
				}
				//update the record
				this.sourcePortStore.add({nid:nid,title:title,presence:presence,portType:portType,streamType:portNode['@streamType'], state:state, '@thumbnailURL':portNode['@thumbnailURL']});
				if(portNode['@thumbnailURL']){
					this.setSrcTnUrlRepeatedly(portNode['@thumbnailURL'], nid);	
				}				
				//bug#50609 if port type is changed from Dst to Src then remove from Dst store
				if(this.dstPortStore.getCount()>0){
					var record = this.dstPortStore.findRecord('nid',nid);
					record && this.dstPortStore.remove(record);
				}
			}
			else if(portType == 'StreamDst'){
				if(this.dstPortStore.getCount()>0){
					var record = this.dstPortStore.findRecord('nid',nid);
					record && this.dstPortStore.remove(record);
				}
				var hasDisplayConfig = this._getDisplayConfigByFeaturelist( portNode['FeatureList']);
    		//update store
    		this.dstPortStore.add({nid:nid,title:title,presence:presence,portType:portType,streamType:portNode['@streamType'],state:state,hasDisplayConfig:hasDisplayConfig});
				//bug#50609 if port type is changed from Src to Dst then remove from Src store
				if(this.sourcePortStore.getCount()>0){
					var record = this.sourcePortStore.findRecord('nid',nid);
					record && this.sourcePortStore.remove(record);
				}
			}			
		}//Ready			
		//restore previous status		
		this.updateDisplayStatus(portType);
								
		/*
		//extra properties may different vary port type
		featureListNode = portNode.FeatureList,
		streamURL = portNode.StreamURL,
		groupsNode = portNode.Groups,
		observerInfoList = portNode.ObserverInfoList,
		streamType = portNode.streamType,	
		pVRInfoNode = portNode.PVRInfo,
		title = portSessionInfoNode['@title'],	
		state = portSessionInfoNode['@state'],
		subscriptionID = portSessionInfoNode['@subscriptionID']
		*/
	},
	
	/**
	 * Check whether has displayConfig from a given destination port's feature list 
	 * return boolean;
	 **/
	_getDisplayConfigByFeaturelist:function(featurelist){
		//update notification
		var returnFlag = false;
		if(featurelist.Feature){
			for(var i=0; i<featurelist.Feature.length;i++){
				var obj =featurelist.Feature[i]; 
				if((obj['@name']=="IPVSClientDisplayConfig")||(obj['@name']=="XP200DisplayConfig")){
					returnFlag = true;
					break;
				}
			}	
		}
		return returnFlag;
  },
	
	/**
	 * Notification handler function to update display status 
	 **/
	updateDisplayStatus:function(portType){
		if(portType == 'StreamSrc'){
			var srcStore = this.srclist.getStore();
			var srcRecords = srcStore.getRange();
			Ext.iterate(srcRecords,function(record){
				var nid = record.data.nid,
					srcOuterHTML = this.srcDisplayStatusArr[nid];
				console.log('MainPanel:updateDisplayStatus:source port: nid:'+nid);					
				if(srcOuterHTML) {
					//back to previous status
					var srcDOM = Ext.DomQuery.selectNode('div[id="'+ nid +'"]', Ext.get('livepanel').dom);//this.srcDOMArr[nid];
					srcDOM.outerHTML = srcOuterHTML;
				}
				else{
					//refresh new one
				}	 
			}, this);
		}
		else if(portType == 'StreamDst'){
			var destStore = this.dstlist.getStore();
			var destRecords = destStore.getRange();
			Ext.iterate(destRecords,function(record){
				var nid = record.data.nid,
					hasDisplayConfig = record.data.hasDisplayConfig,
					portInfo = record.raw,
					dstInnerHTML = this.destDisplayStatusArr[nid],
 					destDOM =  Ext.DomQuery.selectNode('div[id="'+ nid+'"]', Ext.get('destpanel').dom);//this.destDOMArr[nid]; 	
 				
				if(dstInnerHTML) {
					console.log('MainPanel:updateDisplayStatus:dest port: nid:'+nid);
					//back to previous status
					destDOM.innerHTML = dstInnerHTML;
				}
				else{
					console.log('MainPanel:updateDisplayStatus:get dest port displayConfig: nid:'+nid);
					//refresh new one
					this.processPortDisplayConfigByNid(nid,hasDisplayConfig,portInfo);
				}					 
			}, this);
		}	
	},
	
  /**
	 * Notification handler function to process port display config update 
	 **/
	portDisplayConfigUpdateHandler:function(notifyNode){
	  var displayConfigNode = notifyNode.PortDisplayConfig;
	  var displayConfigObj = [];
	  displayConfigObj.push({data:{nid:displayConfigNode['@NID'], title:'', canvas:displayConfigNode.Canvas}});
	  this.portDisplayConfigCallback(displayConfigObj);	  
	  var sessionData = this.getSessionData();
		Ext.iterate(sessionData,function(key, sdata){
			//check dst is already connected
			if(sdata.PortConnectionInfo['@dstNID'] == displayConfigNode['@NID']){
			  this.processPortSessionInfo(sdata);
			}
		}, this); 
	},
	
	/**
	 * Notification handler function to process port display config 
	 **/
	processPortDisplayConfig:function(){
		var destStore = this.dstlist.getStore();
		var destRecords = destStore.getRange();
		Ext.iterate(destRecords,function(record){
			var nid = record.data.nid,
				hasDisplayConfig = record.data.hasDisplayConfig,
				portInfo = record.raw;
			this.processPortDisplayConfigByNid(nid,hasDisplayConfig,portInfo);
		}, this);
		
		/* localhost test
		//processPortDisplayConfig 
		this.displayConfigStore.getProxy().setUrl('/story_48526/data/processPortDisplayConfig.json');
		this.displayConfigStore.getProxy().setHeaders(this.httpHeader);		
		this.displayConfigStore.load({
			callback:function(records,operation,success){
				if(success){
					this.portDisplayConfigCallback(records,operation);	
				}
				else{
					console.log('MainPanel processPortDisplayConfig fail.');
				}
			},
			scope:this});
			*/
	},
	
	/**
	 * Request function to get port dispaly config by nid 
	 **/
	processPortDisplayConfigByNid:function(nid,hasDisplayConfig,portInfo){
	  if(hasDisplayConfig){
		  this.displayConfigStore.getProxy().setUrl(this.userData.urlBase+'/Resource/Port/DisplayConfig?byNID='+nid + '&subscriptionID=DisplyConfig_'+this.loginController.uuid.generate());
			this.displayConfigStore.getProxy().setHeaders(this.httpHeader);		
			this.displayConfigStore.load({
				callback:function(records,operation,success){
					if(success){
						console.log('MainPanel:processPortDisplayConfigByNid success:nid:'+nid);
						this.portDisplayConfigCallback(records,operation);	
					}
					else{
						console.log('MainPanel:processPortDisplayConfigByNid:fail:add a display-window for dest nid:'+nid);
						this._addDisplayWindowByNid(nid);
					}
					portInfo &&	this.processPortConnectionStatus(portInfo);
				},
			scope:this});
		}
		else{
		  console.log('MainPanel:processPortDisplayConfigByNid:no displayConfig:add a display-window for dest nid:'+nid);
			this._addDisplayWindowByNid(nid); 
			portInfo && this.processPortConnectionStatus(portInfo);
		}
	},	
	
	/**
	 * Add 'display-window' for dest port DIV 
	 **/
	_addDisplayWindowByNid:function(nid){
		//insert a 'display-window' to any destination where don't have display config for display status changing(i.e. media client)  
		var dstTplArr =[];
		var styleCSS = 'position:absolute;top:0;left:0;height:100%;width:100%;zIndex:1';
		dstTplArr.push(' <div class=\"display-window\" id=\"No_'+nid+'\" style=\"'+styleCSS+'\">');
		dstTplArr.push(' <img src=" " style="width:100%; height:100%; !important" id=\"img_'+nid+'\">');
		dstTplArr.push(' </div>');
		dstListItemDom =  Ext.DomQuery.selectNode('div[id="'+ nid+'"]', Ext.get('destpanel').dom);	
		dstListItemDom.innerHTML = dstTplArr.join('');
	},
	
	/**
	 * Notification handler function to process kickout(while have duplicate login) 
	 **/
	kickOutHandler:function(notifyNode){
		this.doLogoutOnError({kickOut:true})
	},	

	//destOnloadCallBack:function(){
		//console.log('MainPanel controller destOnloadCallBack');
		//this.processPortDisplayConfig();
	//},
	
	/**
	 * Helper function, process display config response call back 
	 **/
	portDisplayConfigCallback:function(records){
		//var records = Ext.data.StoreManager.lookup('displayconfigStoreId').getRange();
		var recordObj = records[0];
		var nid = recordObj.data.nid,
			  title = recordObj.data.title,
			  dstListItemDom =  Ext.DomQuery.selectNode('div[id="'+ nid+'"]', Ext.get('destpanel').dom);	
		this.displayConfigController.applyDisplayCofig(recordObj,dstListItemDom);
	},
	  
	/**
	 * Helper function to force element in DOM repaint  
	 **/
	forceElRepaint:function(el){
		var tempEl = el.ownerDocument.createTextNode('');
	  el.appendChild(tempEl);
	  Ext.Function.defer(function(){el.removeChild(tempEl)},0);
	}      
});