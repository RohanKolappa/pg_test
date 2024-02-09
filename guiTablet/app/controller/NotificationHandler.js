/**
 * @author ZHEN
 */
Ext.define('TabletGui.controller.NotificationHandler', {
  extend: 'Ext.app.Controller',
  alias: 'controller.notificationhandler',

  init:function(){
  },
  config:{
  	refs:{
    
  	},
  	control:{

  	},
  	//set to 5 minutes as Jetty multiple continuation may raise up to minutes level 
  	urlStr:'',
  	httpHeader:'',
  	options:{},
  	longpollingAborted:false
  },
       
  /**
   * Create long polling notification channel;
   * send a long polling request,either get notification or expired
   * in both cases, recreate channel again (send a new request);
   **/
	createLongPollingChannel:function(urlStr,httpHeader,options){	
		var me = this;
		if(!urlStr || urlStr == '') {
			urlStr = this.getUrlStr();
		}
		else{
			this.setUrlStr(urlStr);
		};
		if(!httpHeader || httpHeader == '') {
			httpHeader = this.getHttpHeader(); 
		}
		else{
			this.setHttpHeader(httpHeader);
		};
		if(!options || options == {}) {
			options = this.getOptions(); 
		}
		else{
			this.setOptions(options);
		};
		//options.successCBFnc ? options.successCBFnc :			
		var successFnc = function(notificationResp){
		    	//handle notification

		    	/* there are 2 case get responseText = '': logout and browser timeout; logout no need create long polling again,expire will be handled in failure;
				if(notificationResp.responseText == ''){
					//this is a hack, MDW should return a decent empty response
			    	console.log('NotificationHandler:createLongPollingChannel success, make new request at ***** '+new Date().getTime()+' ,notificationResp.responseText:'+notificationResp.responseText );
		    		this.createLongPollingChannel();
				}
				*/
			if(notificationResp.responseText){
	    	//this is a hack, MDW should return a decent empty response
	    	console.log('NotificationHandler:createLongPollingChannel success, make new request at ***** '+new Date().getTime()+' ,notificationResp.responseText:'+notificationResp.responseText );
	    	var noficationObj = Ext.decode(notificationResp.responseText);
	    	if(((notificationResp.responseText.indexOf('KickedOut')<0) && (notificationResp.responseText.indexOf('LoggedOut')<0)) ||
	    	   (noficationObj.NotificationList=="")){
	    		// Send long polling request when 
	    	   	// [1] have a notification but not KickedOut or LoggedOut; {"NotificationList": {"KickedOut": ""}}, {"NotificationList": {"LoggedOut": ""}}
	    		// [2] notification expired
	    		this.createLongPollingChannel();
	    	}
	    	//handle notifications if not empty, otherwise expired got: "{"NotificationList": ""}"
    		if(noficationObj.NotificationList!=""){
    			me.handleNotification(noficationObj);
    		}
    	}
    	else {
    		//if no responseText then publish error
  			this.getApplication().fireEvent("NotificationError", {responseText:"Empty Notification"});
    	}
		};
		//options.failureCBFnc ? options.failureCBFnc :	 
		var failureFnc = function(e){
			console.log('NotificationHandler:createLongPollingChannel:longpolling failure: requestId: '+e.requestId+', status: '+e.status+', responseText: '+e.responseText+';'+', e.timedout: '+e.timedout+', e.aborted: '+e.aborted);
			this.getApplication().fireEvent("NotificationError", e);
		};
		/*
		var failureFnc = function(e){
			if(!this.getLongpollingAborted()){ 
				console.log('NotificationHandler:createLongPollingChannel fail, make new request at ***** '+new Date().getTime());
				//retry to create long pulling channel if failure because of timeout
				if(e.status == 0) {
					//most failure case, status:0 which is notification expired response (there is no notification during this period)
					console.log('NotificationHandler:createLongPollingChannel:longpolling expired: requestId: '+e.requestId+', e.timedout: '+e.timedout+', status: '+e.status+', responseText: '+e.responseText+';');
					this.createLongPollingChannel();
				}
				else {					
					this.getApplication().fireEvent("NotificationError", e);
				}					
			}
			else{
				console.log('NotificationHandler: Long pulling notification aborted, status:'+e.statusText);
			}
		};
		*/
		//var time1 = new Date().getTime();    	   
		this.longpollingRequestRef = Ext.Ajax.request({
          												 url:urlStr,
          												 method:'GET',
          												 type:'jsonp',
          												 headers:httpHeader,
          												 //timeout:expireTime,
          												 callback:function(options,success,response){
          													 //var time2 = new Date().getTime();     	
          												   //console.log(' >>>>> Long polling request callback time= '+(time2-time1));
          												   if(success){
          												     successFnc.call(me,response);
          												   }
          												   else {
          												   	 failureFnc.call(me,response);
          												   }          												    	
          												 }          													
          											 });   		
	},
	
	/**
	 * Handl notification, can be an array, multi-notification or single one;
	 * When notification back (long polling response) process the notification;
	 **/
	handleNotification:function(nitificationResp){
		//handle notification list
		if(nitificationResp.NotificationList){
			if(Array.isArray(nitificationResp.NotificationList.Notification)){
				//many Notifications return in an array, if single no array return
				Ext.iterate(nitificationResp.NotificationList.Notification,function(notificationNode){
					this.publishSingleNotification(notificationNode);	
			  },this);	
			}
			else{
				//single notification, special cases: {"NotificationList": {"KickedOut": ""}} {"NotificationList": {"LoggedOut": ""}}
				var notificationSingleObj = nitificationResp.NotificationList.Notification ? nitificationResp.NotificationList.Notification : nitificationResp.NotificationList;
				this.publishSingleNotification(notificationSingleObj);				
			}			
		}
	},
	
	/**
	 * Publish notification(s); 
	 **/
	publishSingleNotification:function(JsonObj){
		var keyArray = Object.keys(JsonObj), 
			propertyName = keyArray.shift();
		this.getApplication().fireEvent(propertyName,JsonObj[propertyName]);
		console.log('NotificationHandler:publishSingleNotification:handle '+propertyName+' notification.');
	}
	  
});