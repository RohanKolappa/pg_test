/**
 * @author ZHEN
 */
Ext.define('TabletGui.controller.Main', {
  extend: 'Ext.app.Controller',
  alias: 'controller.main', 
  statics:{
  	log:null
  },
  
  init:function(){
 		log = log4javascript.getDefaultLogger();
  },
    
  launch:function(){
		//set Ajax timeout to 3 minutes
		Ext.Ajax.setTimeout(180000);
		//initialize server version based on service.version on server
		Ext.Ajax.request({
      url: 'service.version',
      method: 'GET',
      callback: function(options, success, response) {    
        if(success && response.responseXML){
        	var serviceNode  = response.responseXML.firstChild;
         	SERVICE_VERSION  = serviceNode.getAttribute('Ver');
        }
        else{
        	msg = "Can't read service.version from MS server <br/>";
  				Ext.Msg.alert('Read Service.version Error', msg);               		
        }
      }
    });  
       
        /*
        Ext.Ajax.on('beforerequest', function(conn,response,options){
        	console.log('before reqid =:'+conn.id+' ,response: '+response);
        });
        
        Ext.Ajax.on('requestcomplete', function(conn,response,options){
        	console.log('after req =: '+conn.id+' ,response.responseText: '+response.responseText+' ,response.text: '+response.text+' ,response.statusText: '+response.statusText+' ,response.status: '+response.status);
        	if (response.responseText && response.status === 0) {
    			response.status = response.responseText ? 200 : 404;
    		}
    		console.log('after convert req =: '+conn.id+' ,response.responseText: '+response.responseText+' ,response.text: '+response.text+' ,response.statusText: '+response.statusText+' ,response.status: '+response.status);
        });
        
        Ext.Ajax.on('requestexception', function(conn,response,options){
        	 console.log('requestexception req =: '+conn.id+' ,response.responseText: '+response.responseText+' ,response.text: '+response.text+' ,response.statusText: '+response.statusText+' ,response.status: '+response.status);
        });
        */
  },
  config:{
  	refs:{
  		//top level container and second level pages
  		//main contianer
  		appContianer:{
  			selector:'#appcardctn',
  			fullscreen:true
  		},
  		//setup panel
  		setupPanel:{
  			selector:'#settingsscreen'
  		},
  		//login panel
  		loginPanel:{
  			selector:'#loginscreen'
  		},
  		//main panel			
  		mainPanel:{
  			selector:'#mainscreen'
  		}		
		},
		//linkage between view and control
		control:{
			setupPanel:{
				activate:function(){
					console.log('Main: setupPanel being activated  ');
					this.getApplication().fireEvent("MSG_SETUP",{appContianer:this.getAppContianer(),loginPanel:this.getLoginPanel(),mainPanel:this.getMainPanel()});						
				}
			},
			loginPanel:{
				activate:function(){
					console.log('Main: loginPanel being activated  ');
					this.getApplication().fireEvent("MSG_LOGIN",{appContianer:this.getAppContianer(),setupPanel:this.getSetupPanel(),mainPanel:this.getMainPanel()});						
				}
			},
			mainPanel:{
				activate:function(){
					console.log('Main: mainPanel being activated  ');
					this.getApplication().fireEvent("MSG_MAINPANEL",{appContianer:this.getAppContianer(),setupPanel:this.getSetupPanel(),loginPanel:this.getLoginPanel()});						
				}
			}						   
		}    	
  }
});