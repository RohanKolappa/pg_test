/**
 * @author ZHEN
 */

Ext.define('TabletGui.model.SourcePortList', {
  extend: 'Ext.data.Model',
  alias: 'model.sourceportmodel',

  config: {
  	fields: [
  	        {name:'nid', mapping:'@NID'},
      			{name:'title', mapping:'@title'},
      			{name:'presence', mapping:'@presence'},
      			{name:'portType', mapping:'@portType'},
      			{name:'streamType', mapping:'@streamType'},
      			{name:'state', mapping:'@state'},
      			{name:'tnurlport', type:'string', defaultValue:':8880'},		
      			{name:'thumbnailurl',
      			 type:'string',
      			 convert:function(v,rec){
      				 var thumbnailUrl = rec.raw['@thumbnailURL'];
      				 var streamType = rec.raw['@streamType'];
      				 if(streamType=='RFB'){
      					 return "./app/resource/image/src-thumb-kbm-online.png";
      				 }
      				 else if(streamType=='RTP-PCM'){
      					 return "./app/resource/image/src-thumb-audio-online.png";
      				 }
      				 else{
      					 if(thumbnailUrl) {
  								 //fix mobile Safari on iOS6 image update issue 
  								 var strArr = thumbnailUrl.split('/mediaportal/');
  								 thumbnailUrl = strArr[0]+rec.data.tnurlport+'/mediaportal/'+strArr[1];
  								 return thumbnailUrl;
  							 }
  							 else{
  								 return "./app/resource/image/src-thumb-video-online.png";
  							 }	
      				 }						
      			 }
      			},
      			{ name:'stateImg', 
      				type:'string',
      				mapping:'@state',
      				convert:function(v,rec){
      					return '<img src="./app/resource/image/icon_greenBall.png">' ;
      				}
      			}
      		]
      		/*
      		fields: [
      			'title' ,'presence','portType'
      		]
      		*/
  }
});