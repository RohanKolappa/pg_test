exports.downloadFile = function(req, resp){
	var logger = require('../utils/logger');
	var url = require('url');
	var utils = require('../utils/common');
	var parsedURL = url.parse(req.url,true);
	var queryObject = parsedURL.query;

	utils.authenticateURL(queryObject, resp, function(error,respose){
		if (error) {
			utils.handleError(error, resp);
			return;
		}
		var mediaID = queryObject.mediaID;
		var dirID = queryObject.dirID;
		var folderName = queryObject.format;
		var persistence = queryObject.persistence;
		var uuid = queryObject.uuid;
		var view = queryObject.type;
		
		var fs = require('fs');
		var path = require('path');
		var stat = "";
		var fileParam = "";
		var filePath = "";
		var contentType = "";
		
		if(view == 'export'){
			var fileFolder = "";
			if(persistence == "temporary") {
				fileFolder = dirID+"\/dms\/mediafile\/cutlist\/"+uuid+"\/";
			} else {
				//fileFolder = dirID+"\/DSS\/data\/portal\/site\/default\/blob\/"+mediaID+"\/"+folderName+"\/";
				fileFolder = getdirInformation(dirID)+"\/"+mediaID+"\/"+folderName+"\/";
				logger.info("fileFolder: "+fileFolder);
			}
  

			var fileName = fs.readdirSync(fileFolder);
			logger.info("fileName: "+fileName);
			var fileString = "\/"+fileName;
			filePath = path.join(fileFolder,fileString);
		
			fileParam = "attachment; filename="+fileName;
			contentType = "application/mkd";
			stat = fs.statSync(filePath);
		}else if(view == 'snapshot'){
			var mediaFileName = getMediaFileNamefromSSIndex(getdirInformation(dirID)+"\/"+mediaID);
			var ImageType = getImageTypefromDBXML(getdirInformation(dirID)+"\/"+mediaID);
								
			logger.info("mediaFileName :: "+mediaFileName);
			var fileString = "\/"+mediaFileName;
			filePath = path.join(getdirInformation(dirID)+"\/"+mediaID,fileString);
			logger.info("filePath :: "+filePath);
			
			fileParam = "attachment; filename="+mediaFileName;
			contentType = getContentType(ImageType, function(err){
				if(err){
					utils.handleError(error, resp);
				}				
			});
			stat = fs.statSync(filePath);
		}else if (view == "dbbackup") {
			var fileFolder = "\/data\/saved_databases\/";
			var fileName = fs.readdirSync(fileFolder);
			var fileString = "\/snapshot.tgz";
			filePath = path.join(fileFolder,fileString);
			logger.info("DB Back up location: "+filePath);
			if (fs.existsSync(filePath)) {
				fileParam = "attachment; filename="+fileName;
				contentType = "application/octet-stream";
				stat = fs.statSync(filePath);
			}
			else{
				logger.info("ERROR:File rename :: " + filePath);
				var error = new Error("Internal Server Error");
				error.http_code=500;
				utils.handleError(error, resp);
			}
			
		}else if (view == "dbdownload") {
			var fileFolder = "\/data\/existdbserver\/upload\/";
			var fileName = fs.readdirSync(fileFolder);
			logger.info("DB Download location: "+fileName[0]);
			var fileString = "\/"+fileName[0];
			filePath = path.join(fileFolder,fileString);
		
			fileParam = "attachment; filename="+fileName;
			contentType = "application/octet-stream";
			stat = fs.statSync(filePath);
		}else if (view == "reports") {
			var workareaID= queryObject.workAreaID;
			var fileFolder = "\/data\/reports\/WORKAREA_"+workareaID;
			var fileName = queryObject.fileName; 
			filePath = path.join(fileFolder,"downloadToPC",fileName);	
			
			logger.info("fileName: "+fileName);				
			fileParam = "attachment; filename="+fileName;
			contentType = "application/octet-stream";
			stat = fs.statSync(filePath);
		}
		else if(view == ""){ //In case if querystring 'view' is blank
			contentType = getContentType("", function(err){
				if(err){
					utils.handleError(error, resp);
				}				
			});
			filePath = path.join(getdirInformation(dirID)+"\/"+mediaID+"\/tn","tn.jpg");
			logger.info("In case 'view' is balnk, filePath :: "+filePath);
			fileParam = "attachment; filename=tn.jpg";
			stat = fs.statSync(filePath);
		}
		downloadFile(fs, resp,contentType,stat,fileParam,filePath, persistence, view, fileFolder, logger);	
	});
};

function downloadFile(fs, resp, contentType, stat, fileParam, filePath, persistence, view, fileFolder, logger){
	resp.writeHead(
				200,
				{
					'Content-Type' : contentType,
					'Content-Length' : stat.size,
					'Content-Disposition' : fileParam
				});
		var readStream = fs.createReadStream(filePath);
		readStream.on('data', function(data) {
			var flushed = resp.write(data);
			// Pause the read stream when the write stream gets
			// saturated
			if (!flushed)
				readStream.pause();
		});
		resp.on('drain', function() {
			// Resume the read stream when the write stream gets
			// hungry
			readStream.resume();
		});
		readStream.on('end', function() {
			// clean up temp file after serving is done.
			if(persistence == "temporary" || view == "reports") {
				logger.info('Clean up temporary file after serving: ' + filePath);
				cleanUpTemporaryMediaDir(fileFolder);
			}
			resp.end();
		});
}
function cleanUpTemporaryMediaDir(fileFolder) {
	var fs = require('fs');
	var files = [];
    if( fs.existsSync(fileFolder) ) {
        files = fs.readdirSync(fileFolder);
        files.forEach(function(file,index){
            var curPath = fileFolder + "/" + file;
            if(fs.statSync(curPath).isDirectory()) { // recurse
            	cleanUpTemporaryMediaDir(curPath);
            } else { // delete file
                fs.unlinkSync(curPath);
            }
        });
        fs.rmdirSync(fileFolder);
    }
};

function getContentType(imageType, callback){
	var logger = require('../utils/logger');
	var contentType = "";
	if(imageType == "PNG"){
		imageType = "image/png";
	}else if(imageType == "BMP"){
		imageType = "image/bmp";
	}else if(imageType == "GIF"){
		imageType = "image/gif";
	}else if(imageType == "TIF"){
		imageType = "image/tiff";
	}else if(imageType == "JPG"){
		imageType = "image/jpeg";
	}else if(imageType == ""){
		imageType = "image/jpeg";
	}else{
		logger.info("File content type not supported");
		var error = new Error("Internal Server Error")
		error.http_code=500;
		callback(error);		
	}
	contentType = imageType;
	return contentType;
}

function getMediaFileNamefromSSIndex(path){
	var fs = require('fs');
	var logger = require('../utils/logger');
	var ssindex = path+'\/SSINDEX';
	
	fs.exists(ssindex, function(exists){
	if (!exists){	
					logger.info('SSINDEX does not exists');
					var error = new Error("Internal Server Error")
					error.http_code=500;
					callback(error);		
		}		
	 });
	
	var fileData = fs.readFileSync(ssindex, 'ascii');
	//logger.info("SSINDEX File: "+ssindex);     //Prints whole SSINDEX file
	var fileName = ((fileData.toString()).split(' '))[1];
	logger.info("fileName :: "+fileName);
	return fileName;
}
function getImageTypefromDBXML(path){
	var fs = require('fs');
	var xmlFile = path+'\/db.xml';
	var xpath = require('xpath');
	var xmlDom = require('xmldom');
	var logger = require('../utils/logger');
	
	fs.exists(xmlFile, function(exists){
	if (!exists){	
					logger.info('db.xml does not exists');
					var error = new Error("Internal Server Error")
					error.http_code=500;
					callback(error);		
		}		
	 });	
	
		var fileData = fs.readFileSync(xmlFile, 'ascii');
		//logger.info("XML File: "+xmlFile);     //Prints whole db.xml file
		var dom = xmlDom.DOMParser;
		var doc = new dom().parseFromString(fileData);   
		var dirNodeXPath = "//ImageType";		
		var node = xpath.select(dirNodeXPath, doc);
		var imgType = node[0].firstChild.data;
		logger.info('Image type: ' + imgType);
		return imgType;
}

function getdirInformation(dirID){
		var fs = require('fs');
		var xmlFile = '\/etc\/config\/Device.xml';
		var xpath = require('xpath');
		var xmlDom = require('xmldom');
		var logger = require('../utils/logger');
		
		fs.exists(xmlFile, function(exists){
		if (!exists){	
					logger.info('Device.xml does not exists');
					var error = new Error("Internal Server Error")
					error.http_code=500;
					callback(error);		
		}		
	 });	

			var fileData = fs.readFileSync(xmlFile, 'ascii');
			//logger.info("XML File: "+xmlFile);     //Prints whole Device.xml file
			var dom = xmlDom.DOMParser;
			var doc = new dom().parseFromString(fileData);    
			var dirNodeXPath = "//MediaStore[Name='"+dirID+"']/RootDirPath";
			var node = xpath.select(dirNodeXPath, doc);
			var path = node[0].firstChild.data;
			var fileFolder1="";
			if(path.length > 0){
					var parentElementXpath = "//MediaStore[Name='"+dirID+"']/RootDirPath/../..";
					var parentNode = xpath.select(parentElementXpath, doc);
				var elementName = parentNode[0].localName;
				if(elementName == 'NASEntry'){
					var nasListIndexXpath = "//MediaStore[Name='"+dirID+"']/RootDirPath/../../@ListIndex";
					var listIndexNASNode = xpath.select(nasListIndexXpath, doc);
					var nodeCountXpath = "count(//MediaStore[Name='"+dirID+"']/RootDirPath/../../@ListIndex)";
					var count = xpath.select(nodeCountXpath, doc);
					var nasListIndex = ((listIndexNASNode[count-1].toString()).split('="'))[1].replace('"','');
					var nasPath = path+"\/"+nasListIndex+"\/blob\/";
					logger.info('NAS file path:  ' + nasPath);
					return nasPath;
					//fileFolder1 = nasPath+"\/"+mediaID+"\/"+folderName+"\/";
				}else{
					var defaultPath = path+"\/default\/blob\/";
					logger.info('Default file path: ' + defaultPath);
					return defaultPath;
			
				}
			}	 
};
