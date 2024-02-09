exports.uploadFile = function(req, resp, next){
	var fs = require('fs');
	var path = require('path');
	var url = require('url');
	var utils = require('../utils/common');
	var parsedURL = url.parse(req.url,true);
	var queryObject = parsedURL.query;
	var logger = require('../utils/logger');
	var utility = require('../utils/fileUtils');
		
	utils.authenticateURL(queryObject, resp, function(error,respose){
		if (error) {
			utils.handleError(error, resp);
			return;
		}
	
	logger.info('Inside Upload.js');

	//var form = new formidable.IncomingForm();
	var uploaded_file;
	var type = queryObject.type;
	var uploadPath;
	var importMimes = ["video/x-matroska","application/xml", "application/octet-stream","application/zip"];
	var injectMimes =["image/x-png", "image/png", "image/bmp","image/x-ms-bmp","image/gif","image/tiff", "image/jpeg","video/mp4" ];
	var dbMime = "application/x-tar";
	
	if(type == 'upgrade'){
		uploadPath = "/data/mediaroom/upgrade/download";
	}else if(type == 'dbrestore'){
		uploadPath = "/data/saved_databases";
	}else{
		uploadPath = "/data/mediadir/inject";
	}
	var fstream;
	var filePath;
	
	
	
    req.pipe(req.busboy);
    req.busboy.on('file', function (fieldname, file, filename, encoding, mimetype) {	
    		//var originalFileName = filename;
    		var uuid = queryObject.uuid;
    		if(typeof uuid === 'undefined' || uuid === '')
    			uuid = createUUID();
    			
    		if (importMimes.indexOf(mimetype) > -1 || (typeof mimetype !== 'undefined' && mimetype.indexOf('image') > -1) || mimetype === "video/mp4" || dbMime === mimetype || type === "dbrestore" || mimetype === "application/x-zip-compressed"){
			    filename = uuid+path.extname(filename)
			    var uploadUUIDPath = uploadPath+'/'+uuid;
			    	
			    fs.mkdir(uploadUUIDPath, function(err) {
			    	if(err) {
			    		logger.info("ERROR: Cannot create File path :: " + uploadUUIDPath);
						utils.handleError(err, resp);
						return;
			    	}
			    	filePath = path.join(uploadUUIDPath,filename);
				    
					fstream = fs.createWriteStream(filePath);
	
					fstream.on('error', function(error) {
						logger.info("Error creating fstream" + error);
						utils.handleError(error, resp);
						return;
					});
					fstream.on('end', function() {
						//logger.info("End");
					});
					fstream.on('close', function () {    
					    logger.info("Upload.js: File Uploaded " + filePath);
					    
					    var mmm = require('mmmagic'),
						Magic = mmm.Magic;

						var magic = new Magic(mmm.MAGIC_MIME_TYPE);
						var mimeType;
						magic.detectFile(filePath, function(err, result) {
							if (err) {
								logger.info("ERROR:File rename :: " + filePath);

								var error = new Error("Can not detect mime type");
								error.http_code=500;
								utils.handleError(error, resp);
							}
							logger.info("Mime Type is: "+result);
							mimetype = result;
							
						    if (importMimes.indexOf(mimetype) > -1){
						    	
								callToAddFileRequest(filePath,queryObject.dirID,queryObject.mediaID,queryObject.groupnid, uuid, mimetype, resp);
						    }
						    else if((typeof mimetype !== 'undefined' && mimetype.indexOf('image') > -1) || mimetype === "video/mp4"){
						    	//else if(injectMimes.indexOf(mimetype) > -1){
						    	logger.info("Detected an image/video file for ingestion!");
						    	var inj = require('../file/inject');
						    	inj.inject(req, resp, queryObject, mimetype,filePath, function(err) {
								if(err){
									utils.handleError(error, resp);
								}
						    	});
							}
							else if(type === 'upgrade' && dbMime === mimetype){
							var renamePath = path.join(uploadPath,"image.tar");
							fs.rename(filePath, renamePath, function(err){
								if(err){
									logger.info("ERROR:File rename :: " + filePath);
									var error = new Error("Internal Server Error");
									error.http_code=500;
									utils.handleError(error, resp);
								}
							});
							logger.info("File Uploaded Successfully at : "+renamePath);
							utility.handleResponseMessage("", resp, "");
							}
							else if(type == "dbrestore"){
							var renamePath = path.join(uploadPath,"importdb.tgz");
							fs.rename(filePath, renamePath, function(err){
								if(err){
									logger.info("ERROR:File rename :: " + filePath);
									var error = new Error("Internal Server Error")
									error.http_code=500;
									utils.handleError(error, resp);
								}
							});
							logger.info("File Upload Successfully at : "+renamePath);
							utility.handleResponseMessage("", resp, "");
								}
								else{
									//file.resume(); //ignore the file stream
									
									logger.info("ERROR: Unsupported Mime Type: "+mimetype);
									utility.handleNotSupportedFile(mimetype, resp);
									fs.unlink(filePath, function(err){
										if (err) logger.error("ERROR: Can not remove file: ",filePath,"::",err)
										logger.info("Removed file", filePath)
									})
								}
							});

					
					});
					
					file.pipe(fstream);
			    });
			}    
			else{
				file.resume(); //ignore the file stream
				logger.info("ERROR: Unsupported Mime Type: "+mimetype);
				utility.handleNotSupportedFile(mimetype, resp);
			}	
		});
	});
	
};

function callToAddFileRequest(filePath, dirID, queueID, groupID, uuid, mimetype, resp){                                                                                                            
	var javaHttp = require ('http');
	var fs = require ('fs');
	var logger = require('../utils/logger');
	var xpath = require('xpath');
	var dom = require('xmldom').DOMParser;
	var spawn = require("child_process").spawn
	var execFile = require("child_process").execFile
	var xml2js = require('xml2js');
	var utility = require('../utils/fileUtils');
	var imageType = (mimetype == 'video/x-matroska')// || mimetype == "application/octet-stream" || mimetype == "application/zip")
	? 'MKD'
			: 'GRP';
	var options = {
			host: 'localhost',
			path: '/deviceinterface/File',
			//since we are listening on a custom port, we need to specify it by hand
			port: '10080',
			//In case of huge files and/or slow connection, ensure that the connection is persisted by server until next request
			headers: {'Content-Type': 'text/xml',				  
				'Connection':'keep-alive'
			},
			method: 'PUT'
	};	
	javaCallback = function(response) {		
		var xmlResponse='';

		logger.info('STATUS: ' + response.statusCode);		
		if(response.statusCode == '500'){
			logger.info("ERROR:AddFileResponse: 500: ")
			utility.handleResponseFromAddfileRequest("",filePath, resp, "");
		}
		else{
			response.on('data', function (chunk) {		
				xmlResponse += chunk;
			});

			response.on('end', function () {
				logger.info('Response From AddFile Recieved: ');
				if(response.statusCode == '500'){
					logger.info("ERROR:AddFileResponse: 500: ")
					utility.handleResponseFromAddfileRequest("",filePath, resp, "");
				}
				else{
					var doc = new dom().parseFromString(xmlResponse)
					var fileNID = xpath.select1("/File/@NID",doc).value
					//logger.info('Response From AddFile Recieved: fileNID: '+fileNID);
					utility.handleResponseFromAddfileRequest(fileNID, filePath, resp, "");
				}
				
			});
			response.on('error', function(error){
				logger.info("ERROR:AddFileResponse: "+error);
				utility.handleResponseFromAddfileRequest("",filePath, resp, "");
				
			});
		}
	}//.bind( {filepath: filePath} )

	var request = javaHttp.request(options, javaCallback);	
	if(imageType == 'MKD'){
		logger.info("Upload.js: Sending MKD Info");		
		var xml='';
		var parser = new xml2js.Parser();
		var child = spawn('/home/onair/TV/bin/mediaconverter', ['-i','metadata', '-f', filePath]);
		child.stdout.on("data", function (data){
			xml += data;
		})
		child.stderr.on("data", function (data){
			logger.error("ERROR::"+ JSON.stringify(data))
		})

		child.on("exit", function(code){		
			//logger.error("Upload.js: xml is :"+ xml)
			var doc = new dom().parseFromString(xml)		
			var type = xpath.select("//MediaStoreFile/Type/text()", doc).toString();
			var groupTitle="";
			var groupNID="";
			var isOldMediaWithGroup="false" // true only if 4.0 or older media with group info
				//var typeNodes = xpath.select("//Type", doc);
				//var type=typeNodes[0].firstChild.data;
				if(groupID != ""){
					var oldGroupFileNode = xpath.select("//MediaStreamFileResource[Info/MediaStoreFileInfo/Type='MediaGroup']", doc);			
					if(oldGroupFileNode.length > 0){ //send group info for older media (4.0 and older)
						groupTitle= xpath.select("//MediaStreamFileResource/Info[MediaStoreFileInfo/Type='MediaGroup']/Title/text()", doc).toString();
						groupNID = xpath.select1("//MediaStreamFileResource[Info/MediaStoreFileInfo/Type='MediaGroup']/@NID", doc).value;
						isOldMediaWithGroup = "true";
						logger.info("This is 4.0 or older media with group: groupTitle: "+groupTitle+"::groupNID: "+groupNID);
					}
				}
			//var nodes = xpath.select('//Title', doc);
			var title = xpath.select("//MediaStoreFile/Title/text()", doc).toString();
			var metadataTag = xpath.select("//MediaStoreFile/MetaData", doc)[0].toString();
			var copySnapshot = "false";
			utility.sendAddFileRequest(type, imageType, title, dirID,filePath, copySnapshot, queueID, groupID, uuid, metadataTag,groupTitle,groupNID,isOldMediaWithGroup,request)
		})
	}
	else{ // for .grp file upload
		logger.info("Upload.js: Sending group Info");		
		var groupFile = fs.readFileSync(filePath, 'ascii');	
		var doc = new dom().parseFromString(groupFile)		
		var type = xpath.select("//MediaStoreFile/Type/text()", doc).toString();	
		//var nodes = xpath.select('//Title', doc);
		var title = xpath.select("//MediaStoreFile/Title/text()", doc).toString();
		var metadataTag = xpath.select("//MediaStoreFile/MetaData", doc)[0].toString();
		var copySnapshot = "false";		
		utility.sendAddFileRequest(type, imageType, title, dirID, filePath, copySnapshot, queueID, groupID, uuid, metadataTag,"","","false", request)
	}

}
function createUUID() {   
    var s = [];
    var hexDigits = "0123456789abcdef";
    for (var i = 0; i < 36; i++) {
        s[i] = hexDigits.substr(Math.floor(Math.random() * 0x10), 1);
    }
    s[14] = "4";  // bits 12-15 of the time_hi_and_version field to 0010
    s[19] = hexDigits.substr((s[19] & 0x3) | 0x8, 1);  // bits 6-7 of the clock_seq_hi_and_reserved to 01
    s[8] = s[13] = s[18] = s[23] = "-";

    var uuid = s.join("");
    return uuid;
}