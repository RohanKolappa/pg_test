/**
*
*  Base64 encode / decode
*  http://www.webtoolkit.info/
*
**/
Ext.define('TabletGui.util.PostRequestPayload', {
  extend: 'Ext.Class',
	//view request
	getViewReq:function(){
		return  {
				     "View": {
				    	  "@windowId": "",
				        "@srcEntityId": "",
				        "@sessionId": "",
				        "Profile": {
				          "@profileTitle": "",
				          "@profileNID": "",
				          "@id": ""
				        },
				        "@sessionName": "",
				        "SrcPort": {
				          "@portNID": "",
				          "@portTitle": "",
				          "@pvrEnable": "false",
				          "@trimLength": "3600000",
				          "@id": ""
				        },
				        "@name": "",
				        "@dstEntityId": "",
				        "@profileId": "",
				        "@restartSession": false,
				        "@id": "",
				        "@canvasId": "",
				        "DstPort": {
				            "@portNID": "",
				            "@portTitle": "",
				            "@id": ""
				        }
				    }
		}
  },
				
	getPlayBackReq:function(){
		return  {
						 "Playback": {
             	 "@windowId": "",
             	 "@dstEntityId": "",
             	 "GrpFile": {
                 "@fileNID": "",
                 "@fileTitle": "",
                 "@id": ""
             	 },
             "@srcEntityId": "",
             "@restartSession": "",
             "@sessionId": "",
             "SrcFile": {
                "@fileNID": "",
                "@fileTitle": "",
                "@grpFileId": "",
                "@id": ""
             },
             "@canvasId": "",
             "@id": "",
             "DstPort": {
                "@portNID": "",
                "@portTitle": "",
                "@id": ""
             }
          }
    }
//			return 	{
//				    "Playback": {
//				        "SrcFile": {
//                    "@fileNID":"",
//				            "@fileTitle": "",
//				            "@fileDate": "",
//				            "@id": ""
//				        },
//				        "DstPort": {
//                    "@portNID":"",
//				            "@portTitle": "",
//				            "@id": ""
//				        }
//				    }
//				}
  }
});



