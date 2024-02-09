exports.handleError= function(errCode, resp){

	var logger = require('../utils/logger');
	logger.error('Common.js: errCode: '+errCode);
	var errorsString ='{"errors":[{"errorCode":"419" , "description":"Time expired" },{"errorCode":"500" , "description":"Internal Server Error" },{"errorCode":"401" , "description":"Authentication Failed" },{"errorCode":"400" , "description":"Bad Request" },{"errorCode":"415" , "description":"Unsupported Media Type" }]}';
	parsedErrors = JSON.parse(errorsString);
	var description;
	parsedErrors.errors.forEach(function(item,index){		
		if(item.errorCode == errCode.http_code){			
			description = item.description;			
			return;
		}			
	});
	resp.send(errCode.http_code, { error: description });
	//logger.error(errCode, resp);
};
exports.handle200 = function(code,resp){
	resp.send(code);
}
exports.authenticateURL= function(queryObject, resp, callback){
	var logger = require('../utils/logger');	
	var timestamp = queryObject.timestamp;		
	if(typeof timestamp == 'undefined'){
		/*
		 * This is Session based authentication,call to ms to authenticate the url
		 *
		 */		
		callToAuthenticateRequest(queryObject.userJID, queryObject.signature, resp, function(err, response){
			if(err){callback(err);}
			else{callback();}
		});
	}
	else{
		var apiKey = queryObject.apiKey;
		var signature = queryObject.signature;
		if(signature.indexOf(':') > 0){
			signature = signature.substring(0, signature.indexOf(':'));
		}
		
		var fs = require('fs'), xml2js = require('xml2js');
		var parser = new xml2js.Parser();	
		fs.exists('/var/status/APIKeyTable.xml', function(exists) {
			if (exists) {			
				fs.readFile('/var/status/APIKeyTable.xml', function(err, data) {
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
						var crypto = require('crypto');
					
						if(apiKey != '' && sharedSecret != '' && timestamp != '')
						{
							md5 = crypto.createHash('md5').update(apiKey+sharedSecret+timestamp).digest("hex");
							var date = new Date();
							var currentTime = (date.getTime());
							logger.info('timestamp is: '+timestamp+':: currentTime: '+currentTime+':: signature: '+signature+'::md5: '+md5)
							if((timestamp == 'INFINITE'|| timestamp >= currentTime)&& signature == md5) 
							{
	
								logger.info("Authentication Successful :: signature:" +signature);
								callback();
							}						
							else {
								logger.info('timestamp: '+timestamp+':: currentTime: '+currentTime+':: signature: '+signature+'::md5: '+md5);								
								var error = new Error("Authentication token timed out :: signature:" +signature)
								error.http_code=419;
								callback(error);//timeout
							}
						} else {					
							var error = new Error("Authentication failed :: signature: "+signature)
							error.http_code=401;
							callback(error); //authentication failed
						}
					});
				});
			}else {			
				var error = new Error("Internal Server Error")
				error.http_code=500;
				callback(error);
			}		
		});
	}
};

exports.getTNFileName = function(queryObject, callback){
	var size = queryObject.size,
	type = queryObject.type;
	if(type == "recording"){
		callback(null, "tn");
	}
	else if(size == "LARGE"){
		callback(null,"tn_large");
	}	
	else if(size == "NATIVE"){
		var Error = new Error("Unsupported Media Size");
		error.http_code= 415;
		callback(error);
	}
	else{
		callback(null,"tn_small");
	}	
};

function callToAuthenticateRequest(userJID, signature, resp, callback){                                                                                                            
	var http = require ('http');
	var fs = require ('fs');
	var logger = require('../utils/logger');	
	if(typeof userJID != 'undefined' && typeof signature != 'undefined' ){
		//logger.info("Call to AuthenticateURL Request")
		var requestData = "<Request/>";
		var options = {
				host: 'localhost',
				path: '/controller/AuthenticateURL?userJID='+userJID +'&signature='+signature,
				//since we are listening on a custom port, we need to specify it by hand
				port: '10088',
				//In case of huge files and/or slow connection, ensure that the connection is persisted by server until next request
				headers: {'Content-Type': 'application/xml',				  
					'Accept' : 'text/html',
					'Connection': 'close'
				},
				method: 'POST'
		};	
		var request = http.request(options, function(response) {		
			var xmlResponse='';

			//logger.info('Authenticate URl Returned STATUS: ' + response.statusCode);		
			if(response.statusCode == '200'){
				//logger.info("AuthenticateURL response authenticated: "+response.statusCode)
				callback(null)
			}
			else{
				logger.error("AuthenticateURL: ERROR:AuthenticateURL: "+response.statusCode)
				var error = new Error("Authentication failed")
				error.http_code=response.statusCode;
				callback(error); 

			}
			response.on('end',function () {
				//request.connection.destroy();
				request.connection.end();
			});
		});
		request.shouldKeepAlive = false
		request.on('error', function(e) {
			logger.error("AuthenticateURL: ERROR:problem with request: " + e.message);
		});
		request.write(requestData);
		request.end();
	}
	else{
		var error = new Error("Authentication failed: Invalid URL")
		error.http_code=401;
		callback(error); 
	}
}


exports.removeRecursive = function(path, cb) {
	var fs = require('fs');
	var utils = require('../utils/common');
	
    var self = this;

    fs.stat(path, function(err, stats) {
        if (err) {
            cb(err, stats);
            return;
        }
        if (stats.isFile()) {
            fs.unlink(path, function(err) {
                if (err) {
                    cb(err, null);
                } else {
                    cb(null, true);
                }
                return;
            });
        } else if (stats.isDirectory()) {
            // A folder may contain files
            // We need to delete the files first
            // When all are deleted we could delete the 
            // dir itself
            fs.readdir(path, function(err, files) {
                if (err) {
                    cb(err, null);
                    return;
                }
                var f_length = files.length;
                var f_delete_index = 0;

                // Check and keep track of deleted files
                // Delete the folder itself when the files are deleted

                var checkStatus = function() {
                    // We check the status
                    // and count till we r done
                    if (f_length === f_delete_index) {
                        fs.rmdir(path, function(err) {
                            if (err) {
                                cb(err, null);
                            } else {
                                cb(null, true);
                            }
                        });
                        return true;
                    }
                    return false;
                };
                if (!checkStatus()) {
                    for (var i = 0; i < f_length; i++) {
                        (function() {
                            var filePath = path + '/' + files[i];
                            utils.removeRecursive(filePath, function removeRecursiveCB(err, status) {
                                if (!err) {
                                    f_delete_index++;
                                    checkStatus();
                                } else {
                                    cb(err, null);
                                    return;
                                }
                            });

                        })()
                    }
                }
            });
        }
    });
};