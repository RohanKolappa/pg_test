exports.handleResponseFromAddfileRequest= function(fileNID,filePath,resp,url){
	var utils = require('../utils/common')
	, logger = require('../utils/logger')
	, fs = require('fs');	
	if(fileNID ==""){
		logger.info("util.js: Error:"+filePath);
		var error = new Error("Internal Server Error")
		error.http_code=500;
		utils.handleError(error, resp);
		/*if(filePath != ""){		
			fs.unlink(filePath, function(err){
				if (err) logger.error("ERROR: Can not remove file: ",filePath,"::",err)
				else logger.info("Removed file: ",filePath)
			})
	   }*/	
	}
	else{
		resp.writeHead(200, {
			'content-type' : 'text/plain',
			'X-BARCO-mediaNID': fileNID ,
			'Location': url
		});
		resp.end(url + "\n\n Uploaded file successfully");			
	}
}
exports.sendAddFileRequest= function(type, imageType, title, dirID,filePath, ingestMedia, queueID, groupID, uuid, metadataTag,groupTitle,groupNID,isOldMediaWithGroup, request, hlsenable){
	var logger = require('../utils/logger');
	var xml_data = '<AddFileRequestData><AddFile type="'+type+'" imageType="'+imageType+'" title="'+title+'" byDirNID="" byDirID="'+dirID+'" filePath="'+filePath+'" ingestMedia="'+ingestMedia+'" queueID="'+queueID+'" mediaGroupId="'+groupID+'" uuid="'+uuid+'" mediaGroupTitle="'+groupTitle+'" mediaGroupNID="'+groupNID+'"  isOldMediaWithGroup="'+isOldMediaWithGroup+'" hlsenable ="'+hlsenable+'">'+metadataTag+'</AddFile></AddFileRequestData>';   
	logger.info("util.js: addfilerequest :::::: "+xml_data)
	request.write(xml_data);                                                                                                                    
	request.end();
	request.on('error', function(e) {
		logger.info("Error while file Upload: "+e);
	  });
}
exports.handleNotSupportedFile= function(fileType, resp){
	resp.writeHead(415, {
		'content-type' : 'text/plain'
	});
	resp.end("File type Not Supported: "+fileType);			
}



exports.verifyHash= function(queryObject,paramString, callback){
	// decode sparams and mark hlsenable =true/false accordingly
	var logger = require('../utils/logger');
	var fs = require('fs'), xml2js = require('xml2js');
	var crypto = require('crypto');
	var parser = new xml2js.Parser();
	var xmlFile = '\/var\/status\/APIKeyTable.xml';
	var apiKey = queryObject.apiKey;
	var sparam = queryObject.sparam;
	fs.exists(xmlFile, function(exists) {
	if (exists) {			
	fs.readFile(xmlFile, function(err, data) {
		parser.parseString(data, function(err, result) {
			var sharedSecret = "";
			var md5 = "";
			var xmlObj=eval(result);
			if(xmlObj.ApiKeyTable.ApiKey.length > 0){
				for (var i = 0; i < xmlObj.ApiKeyTable.ApiKey.length; i++) 
				{
					var key=xmlObj.ApiKeyTable.ApiKey[i].$.key;
					if(key == apiKey)
					{
						sharedSecret=xmlObj.ApiKeyTable.ApiKey[i].$.sharedSecret;
						break;
					}
				}
			}
			if(sharedSecret != '')
			{
				md5 = crypto.createHash('md5').update(sharedSecret+paramString).digest("hex");
				var date = new Date();
				var currentTime = (date.getTime());
				if(sparam == md5) 
				{
					hlsenable="true";
					callback(hlsenable);
				}else {											
					hlsenable="false";
					callback(hlsenable);
				}
			}else{
				callback("error");
			} 
		});
	});
	}else{
		callback("error");
	} 
});
}
exports.handleResponseMessage= function(fileName,resp,url){
	resp.writeHead(200, {
			'content-type' : 'text/plain',
			'X-BARCO-mediaNID': fileName ,
			'Location': url
		});
     resp.end(url + "\n\n Uploaded file successfully: ",fileName);			
	
}

var fs = require('fs');
var path = require('path');
 
if (typeof exports === 'undefined') var exports = {};
 
exports.exists = function(filename) {
	try {
		return (fs.lstatSync(filename) ? true : false);
	}
	catch (err) {
		return false;
	}
};
 
exports.create = function(filename,  contents) {
	try {
		if (!contents) contents = '';
		fs.writeFileSync(filename, contents);
		return true;
	}
	catch (err) {
		return false;
	}
};
 
exports.unlink = function(filename) {	
	logger = require('../utils/logger');
	try {
		 fs.unlinkSync(filename);
		 return true;
	}
	catch (err) {
		logger.error("ERROR in unlinking :"+filename+":: "+err)
		return false;
	}
};
exports.rename = function(tempPath, targetPath) {
	try{
		return fs.renameSync(tempPath, targetPath);
	}
	catch(err){
		return false;
	}
};
exports.mkdirSync = function(filePath) {
	try {
		return fs.mkdirSync(filePath);

		
	} catch (e) {
		return false;
	}
}