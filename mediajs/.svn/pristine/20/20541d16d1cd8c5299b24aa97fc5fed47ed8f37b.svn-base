
exports.cacheFile = function(req, resp) {
	var fs = require('fs')		
	, path = require('path')
	, url = require('url')
	, uploadEventEmitter = require('../utils/watchFile').getInstance()
	, utils = require('../utils/common')
	, parsedURL = url.parse(req.url, true)
	, queryObject = parsedURL.query
	, logger = require('../utils/logger')
	, utility = require('../utils/fileUtils')
	, touch = require("touch")
	, size = queryObject.size;
	// logger.info("Thumbnail.cacheFile")
	utils.authenticateURL(queryObject, resp, function(error,respose){ 
		if (error) { utils.handleError(error, resp); return;};
			var fstream
			, uploadPath ="/data/mediadir/inject/"
			,finalPath = new ThumbnailDirPath();			
		    req.pipe(req.busboy);
		    req.busboy.on('file', function (fieldname, file, filename, encoding, mimetype) {	   
				if ( mimetype.match(/image/).length > 0) {					
					getCachePath(queryObject,function(error, filePath){
					finalPath.path = filePath;
					fstream = fs.createWriteStream(path.join(uploadPath,filename));
					file.pipe(fstream);
					fstream.on('close', function () { 
						
							var TNFileName = (queryObject.type == 'recording')
							? 'tn.jpg'
									: filename;// .file_ext;
							try {
								fs.mkdirSync(filePath);
								touch(filePath+'/access', function(error){// creare
																			// touch
																			// file
																			// 'access'
									if(error) logger.error("ERROR: Can not access file: "+error);
								});
		
							} catch (e) {
								if(e.code === 'EEXIST'){					    
							    } else {
							    	logger.error("Thumbnail.cacheFile: ERROR:",e);
							    }
							}
							
							var targetPath = path.resolve(filePath +'/' + TNFileName);				
		
							// var targetPath = path.resolve(filePath+'/' + TNFileName);
							fs.rename(uploadPath+filename, targetPath, function (err) {
								if (err) throw err;
								// logger.info('FileUploaded: '+targetPath);
								uploadEventEmitter.upload(targetPath);
							});
						});
					});
				} else {
					logger.error("ERROR: Unsupported Media Type: " + mimetype);
					error = new Error("Unsupported Media Type" + mimetype);
					error.http_code = '415';
					utils.handleError(error, resp);
					return;
				}
			
			});
		    req.busboy.on('finish', function(){
		    	resp.send(200);
				if(queryObject.type == "live"){					
					fs.stat(finalPath.path+'/access', function(err,stats){
						if(err) {
							logger.error("ERROR: Can not stat access file: "+err);
							touch(finalPath.path+'/access', function(error){// creare
																		// touch
																		// file
																		// 'access'
								if(error) logger.error("ERROR: Can not access file: "+error);
							});
						}
						else{
							var date = new Date();			
							if(new Date().getTime() - stats.mtime.getTime() > 180000) {// if
																						// not
																						// accessed
																						// in
																						// last
																						// 3
																						// mins
								// call stopPostingThumbnails
								fs.exists(finalPath.path+ "/stop_tn_requested", function (exists) {
									if(!exists){						
										logger.info("STOP THUMBNAIL REQUESTED");
										callToStartThumbnailRequest(size, 'STOP', queryObject.mediaID, queryObject.type,resp)
										touch(finalPath.path+'/stop_tn_requested', function(error){// create
																								// stop_tn_requested
																								// file
											if(error) logger.error("ERROR: Can not access file : "+error);
										});
									}

								});
							}

						}
					});				
				}
		    });
	         req.busboy.on('error', function (error) {
	        	 logger.error("Error in uploading file: "  + error);
	                resp.writeHead(500, { 'Connection': 'close' });
	                resp.end("Error in uploading!");
	         });
	});

};
exports.getFile = function(req, resp) {
	var fs = require('fs'),
	path = require('path'),
	url = require('url'),
	utils = require('../utils/common'),
	utility = require('../utils/fileUtils'),
	touch = require("touch"),	
	uploadEventEmitter = require('../utils/watchFile').getInstance(),
	parsedURL = url.parse(req.url, true),
	queryObject = parsedURL.query,
	logger = require('../utils/logger'),
	sourceNID = queryObject.mediaID,
	defaultLiveTNPath = "/home/onair/modules/mediaportal/public/images/",
	imageFullPath,
	isDefaultPath=1,
	size = queryObject.size,
	type= queryObject.type;
	// logger.info('Thumbnail.getFile');
	utils.authenticateURL(queryObject, resp, function(error,respose){ 
		if (error) { utils.handleError(error, resp); return};
		// resp.setHeader('content-type', 'image/jpeg');
		getCachePath(queryObject,function(error, filePath){			
			utils.getTNFileName(queryObject, function(error,tnFileName){
				if(error) {utils.handleError(error,resp);return}
				else{				
					var defaultTNFileName = (tnFileName == 'tn_large')
					? 'livethumb_large.jpg'
							: 'livethumb_small.jpg';
					var defaultRecordTNFileName = 'recthumb_large.jpg'
						resp.setHeader('Content-type', 'image/jpeg');
					
					fs.exists(filePath, function (exists){
						if(exists){							
							if(utility.exists(filePath + "/stop_tn_requested")) {

								if(utility.unlink(filePath + "/stop_tn_requested")) {

									logger.info('Thumbnail.getFile: Removed Stop status file');
								}else{logger.error('could not remove stop_tn_requested');}
							}
							if(utility.exists(filePath+'/'+tnFileName+'.jpg')){
								// logger.info("Thumbnail:
								// "+filePath+'/'+tnFileName+'.jpg'+' exists')
								if(type == 'live'){
									checkAccessAndCallStartThumbnail(filePath+'/'+tnFileName+'.jpg',defaultLiveTNPath+defaultTNFileName, size, sourceNID, tnFileName, resp)
								}
								else{
									resp.sendfile(filePath+'/'+tnFileName+'.jpg');
								}
							}
							else if(tnFileName == 'tn_large'){
								if(utility.exists(filePath+'/'+'tn_small.jpg')){
									checkAccessAndCallStartThumbnail(filePath+'/'+'tn_small.jpg', defaultLiveTNPath+defaultTNFileName, size, sourceNID, tnFileName, resp)
								}
								else{ // serve default image
									resp.sendfile(defaultLiveTNPath+defaultTNFileName)
								}

							}
							else{ // serve default image
								// logger.info("Thumbnail: Serving default
								// image");
								resp.sendfile(defaultLiveTNPath+defaultTNFileName);							
								fs.createReadStream(defaultLiveTNPath+defaultTNFileName).pipe(fs.createWriteStream(filePath+'/'+tnFileName+'.jpg'));							
								callToStartThumbnailRequest(size, 'START', sourceNID, queryObject.type,resp);
							}
							touch(filePath+'/access', function(error){// create
																		// touch
																		// file
																		// 'access'
								if(error) logger.error("Thumbnail: ERROR: Can not access file: "+error);
							});
						}
						else{							
							if(type == "live"){	
								resp.sendfile(defaultLiveTNPath+defaultTNFileName);
								try {
									fs.mkdirSync(filePath);
									// logger.info("Thumbnail: Serving default
									// image");
									
									fs.createReadStream(defaultLiveTNPath+defaultTNFileName).pipe(fs.createWriteStream(filePath+'/'+tnFileName+'.jpg'));
									// logger.info("Thumbnail: No thumbnail
									// found sending start posting thumbnail")
									callToStartThumbnailRequest(size, 'START', sourceNID, queryObject.type,resp);
									touch(filePath+'/access', function(error){// creare
																				// touch
																				// file
																				// 'access'
										if(error) logger.error("ERROR: Can not access file: "+error);
									});


								} catch (e) {
									if(e.code === 'EEXIST'){
								    } else {
								    	logger.error("Thumbnail get: EXCEPTION: ",e)
								    }
								}								
							}
							else{
								// logger.info("Thumbnail: Polling for Recording
								// recording thumbnail")
								callToStartThumbnailRequest(size, 'START', sourceNID, queryObject.type,resp);				
								var timerEventEmitter = require('events').EventEmitter;
								var timeEvent = new timerEventEmitter();
								var isFileUploaded = false;			
								var listener = function (message) {
									logger.info("Thumbnail: "+message)
									resp.sendfile(filePath+'/'+tnFileName+'.jpg');
									isFileUploaded = true;			
									removeUploadFileListener(uploadEventEmitter, listener,filePath+'/'+tnFileName+'.jpg')
									// uploadEventEmitter.removeListener(filePath+'/'+tnFileName+'.jpg',listener);
									return;
								};
								uploadEventEmitter.on(filePath+'/'+tnFileName+'.jpg', listener);

								timeEvent.on(sourceNID, function(data){
									if(!isFileUploaded){
										logger.info("Thumbnail: Thumbnail not found serving default thumbnail")
										resp.sendfile(defaultLiveTNPath+defaultRecordTNFileName);										
									}
									timeEvent.removeAllListeners();
									uploadEventEmitter.removeListener(filePath+'/'+tnFileName+'.jpg',listener);
									return;
								});
								setInterval(function(){timeEvent.emit(sourceNID,'Download default image');},10000);
							}
						}						
					});
				}
			});
		});
	});
};
function callToStartThumbnailRequest(size, action, sourceNID, type,resp){                                                                                                            
	var http = require ('http');
	var fs = require ('fs');
	var logger = require('../utils/logger');
	var utility = require('../utils/common');
	var requestName = (action == 'START')
	? 'Start'
			: 'Stop'; 
	// logger.info("Thumbnail.callToStartThumbnailRequest")
	var requestData = "<Request/>";
	var options = {
			host: 'localhost',
			path: '/controller/Thumbnail?NID='+sourceNID +'&size='+size+'&action='+requestName+'&type='+type,
			// since we are listening on a custom port, we need to specify it by
			// hand
			port: '10088',
			// In case of huge files and/or slow connection, ensure that the
			// connection is persisted by server until next request
			headers: {'Content-Type': 'application/xml',				  
				'Accept' : 'text/html',
				'Connection': 'close'
			},
			method: 'POST'
	};	
	var request = http.request(options, function(response) {		
		var xmlResponse='';

		logger.info('STATUS: ' + response.statusCode);		
		if(response.statusCode == '200' || response.statusCode == '201' || response.statusCode == '202'){
			// logger.info("Thumbnail: Start posting thumbnail response
			// recieved: "+response.statusCode)
		}
		else{
			logger.error("Thumbnail: ERROR:Start posting thumbnail: "+response.statusCode)


		}
		response.on('end',function () {
			// request.connection.destroy();
			request.connection.end();
		});
	});
	request.shouldKeepAlive = false
	request.on('error', function(e) {
		logger.error("Thumbnail: ERROR:problem with request: " + e.message);
	});
	request.write(requestData);
	request.end();

}
function checkAccessAndCallStartThumbnail(imageFullPath,defaultPath, size, sourceNID, tnFileName, resp){
	var fs = require('fs');
	var logger = require('../utils/logger');

	fs.stat(imageFullPath, function(err,stats){
		if(err) {
			logger.error('Thumbnail: Error while getting stat: ',imageFullPath,err)
			resp.sendfile(defaultPath)
		}
		else{
			var date = new Date();			
			if(new Date().getTime() - stats.mtime.getTime() > 60000) {// if
																		// stale
																		// (more
																		// than
																		// 1
																		// minute)
																		// copy
																		// call
																		// startposting
																		// thumbnails
				// call startPostingThumbnails
				logger.info("Thumbnail.checkAccessAndCallStartThumbnail:: Thumbnail is too old, calling start posting thumbnail")
				resp.sendfile(defaultPath)
				fs.createReadStream(defaultPath).pipe(fs.createWriteStream(imageFullPath));

				callToStartThumbnailRequest(size, 'START', sourceNID, 'live',resp)

			}
			else{
				// logger.info('Sending found thumbnail')
				resp.sendfile(imageFullPath)
			}
		}
	});
}

function getCachePath(queryObject,callback){
	var sourceNID = queryObject.mediaID;
	var type = queryObject.type;
	if (sourceNID == '' || typeof sourceNID == 'undefined' || type == 'undefined' || type == '') {
		// logger.error("Invalid URL");
		var error = new Error("Invalid URL")
		error.http_code = 400;
		callback(error)
	} else if (sourceNID.match(/^[a-zA-Z0-9.\-_]+$/) == null) {
		// logger.error("Invalid SourceNID in URL");
		var error = new Error("Invalid SourceNID in URL")
		error.http_code = 400;
		callback(error)
	} 
	else{
		(type == 'live')
		? callback(null, "/data/tncache/live/" + sourceNID)
				: callback(null, "/data/tncache/recordings/" + sourceNID);
	}
}
function removeUploadFileListener(uploadEventEmitter, listener, filePath){
	uploadEventEmitter.removeListener(filePath,listener);
}

var ThumbnailDirPath = function  (){			
          };

          Object.defineProperty(ThumbnailDirPath.prototype, 'path', {
              get: function() {
                  return this._path;
              },
              set: function(path) {                  
              this._path = path;
              }
          });