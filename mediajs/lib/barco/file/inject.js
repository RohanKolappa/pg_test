exports.inject = function(req, resp, queryObject, mimeType, filePath, callback){
	
	var url = require('url');
	var fs = require('fs');
	var path = require('path');
	var crypto = require('crypto');
	var utils = require('../utils/common');
	var utility = require('../utils/fileUtils');
	var logger = require('../utils/logger');
	var hlsenable= "false";
					
			var uid = crypto.randomBytes(20).toString('hex');  //Unique ID generation
			logger.info(" Inside inject.js ");
			var imageType;
			var type = "MediaImage";
			
			if(mimeType.indexOf('png') > -1){
				imageType = "PNG";
			}else if(mimeType.indexOf('bmp') > -1 || mimeType.indexOf('bitmap') > -1){
				imageType = "BMP";
			}else if(mimeType.indexOf('gif') > -1){
				imageType = "GIF";
			}else if(mimeType.indexOf('tiff') > -1){
				imageType = "TIF";
			}else if(mimeType.indexOf('peg') > -1 || mimeType.indexOf('jpg') > -1){
				imageType = "JPG";
			}else if(mimeType === "video/mp4"){
				imageType = "MP4";
				type = "MediaClip";
			}
			var newFilePath = filePath;
//			var newFileName = uid+'.'+imageType.toLowerCase();
//
//			var newFilePath = path.dirname(filePath) + '/' + newFileName;
//			logger.info("Uploaded file name :: " + newFileName + " Uploaded file path :: "+newFilePath);
//			fs.rename(filePath, newFilePath, function(err){
//			if(err){
//			        logger.info("Error to rename file :: " + filePath);
//					var error = new Error("Internal Server Error")
//					error.http_code=500;
//					utils.handleError(error, resp);
//					return;
//				}
//			});
//			
			
			var protocol = "http";
			if(req.headers['x-forwarded-proto'] === "https") {
				protocol = "https";
			  }
				
			var dirNID = "";
			var copySnapshot = "true";
			//META DATA 
			var tag = "";
			var temp = req.url.split("&");
			for(var i=0;i<temp.length;i++)
			{
				var str = temp[i].split("=");
				if(str[0].indexOf("mediaID")>-1 || str[0].indexOf("timestamp")>-1 || str[0].indexOf("apiKey")>-1 || str[0].indexOf("signature")>-1 || str[0].indexOf("title")>-1 || str[0].indexOf("format")>-1 || str[0].indexOf("dirID")>-1 || str[0].indexOf("sparam")>-1 || str[0].indexOf("options")>-1){
					continue;
				}else if(str[0] !='' && str[1] !=''){
					tag += "<Tag Name=\'"+str[0]+"\'>"+decodeURIComponent(str[1])+"</Tag>";
				}
			}
			var metaData = "<MetaData>"+tag+"</MetaData>";
						
			if(queryObject.options!=null && queryObject.options=="donotcreatehls"){
				hlsenable= "false";
				callToAddFileRequest(type,imageType,queryObject.title,dirNID,queryObject.dirID,newFilePath,copySnapshot,queryObject.mediaID,metaData,resp,hlsenable, protocol);
			}else{
				var paramString=";hls=true";
				utility.verifyHash(queryObject,paramString, function(response){
					if(response=="error"){						
							var error = new Error("Internal Server Error")
							error.http_code=500;
							utils.handleError(error, resp);
							return;
					}
				   callToAddFileRequest(type,imageType,queryObject.title,dirNID,queryObject.dirID,newFilePath,copySnapshot,queryObject.mediaID,metaData,resp,response, protocol);
				});
			}	
					
			
};

//sends the PUT request to AddFileRequest.
function callToAddFileRequest(type,imageType,fileTitle,dirNID,dirID,newFilePath,copySnapshot,mediaID,metaData,resp, hlsenable, protocol){

	var param = {
		hostname: 'localhost',
		port: 10080,
		path: '/deviceinterface/File',
		method: 'PUT',
		headers: {
	          'X-Forwarded-Proto': protocol
	      }
	};
	var http = require ('http');
	var fs = require ('fs');
	var logger = require('../utils/logger');
	var utils = require('../utils/common');
	var utility = require('../utils/fileUtils');
	var request = http.request(param);//TODO set headers
	
	utility.sendAddFileRequest(type, imageType, fileTitle, dirID, newFilePath, copySnapshot, "", "", "", metaData, "", "", "false", request, hlsenable)
	
	/*
		var xml_data = '<AddFileRequestData><AddFile type=\'' + type + '\' imageType=\'' + imageType + '\'' + ' title=\'' + fileTitle + '\' byDirNID=\'' + dirNID + '\'' + ' byDirID=\'' + dirID + '\'' + ' filePath=\'' + newFilePath + '\'' + ' copySnapshot=\'' + copySnapshot + '\'' + ' queueID=\'' + mediaID + '\'>' + metaData + '</AddFile></AddFileRequestData>';	
		logger.info("Request to be sent :::: " +xml_data);
		request.write(xml_data);
		request.end();
	*/
	request.on("error", function (response){
		logger.info("Error calling AddFileRequest: "+response.statusCode);
		utils.handleError(response.statusCode, response);
	});
	request.on("response", function (response){
		logger.info("Response Received: "+response.statusCode);
		var xmlResponse = "";
		if(response.statusCode == '500'){
				utility.handleResponseFromAddfileRequest("","", resp,"");
			}
		else{
		response.on('data', function (chunk) {	
				xmlResponse += chunk;
			});

			response.on('end', function () {
				logger.info('Response From AddFile Recieved: xmlResponse: '+xmlResponse);
				var xpath = require('xpath');
				var dom = require('xmldom').DOMParser;
				var doc = new dom().parseFromString(xmlResponse)
				var fileNID = xpath.select1("/File/@NID",doc).value
				logger.info('Response From AddFile Recieved: fileNID :: '+fileNID);
				var url = xpath.select1("/File/@url",doc).value
				logger.info('Response From AddFile Recieved: url :: '+url);
				utility.handleResponseFromAddfileRequest(fileNID, "", resp, url);
			});
			response.on('error', function(error){
				logger.info("Error in response from addfile request: "+error);
				utility.handleResponseFromAddfileRequest("","", resp,"");
			});
		}		
	});	
}
