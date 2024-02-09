/**
 * @author ZHEN
 */

Ext.define('TabletGui.model.RecordingList', {
  extend: 'Ext.data.Model',
  alias: 'model.recordingmodel',

  config: {
  	fields: [
      			{name:'title', mapping:'@title'},
      			{name:'nid', mapping:'@NID'},
      			{name:'fileType', mapping:'@fileType'},
      			{name:'playlength', mapping:'@playlength'},
      			{name:'size', mapping:'@size'},
      			{name:'state', mapping:'@state'},
      			{name:'tnurlport', type:'string', defaultValue:':8880'},		
      			{name:'thumbnailurl',
      			 type:'string',
      			 convert:function(v,rec){
      				 var thumbnailUrl = rec.raw['@thumbnailURL'];
							 if(thumbnailUrl) {
								 //fix mobile Safari on iOS6 image update issue 
								 var strArr = thumbnailUrl.split('/mediaportal/');
								 thumbnailUrl = strArr[0]+rec.data.tnurlport+'/mediaportal/'+strArr[1];
								 return thumbnailUrl;
							 }
							 else{
								 return '_';
							 }
      			 }
      			},
      			{	name:'stateImg', 
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