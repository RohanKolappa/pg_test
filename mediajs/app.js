/**
 * Module dependencies.
 */

var express = require('express')  
  , app = express()
  , upload = require('./lib/barco/file/upload')
  , download = require('./lib/barco/file/download')
  , thumbnail = require('./lib/barco/file/thumbnail')
  , common = require('./lib/barco/utils/common')
  , expressWinston = require('express-winston')
  , busboy = require('connect-busboy')
  , winston = require('winston')
  , gc_thumbnail = require('./lib/barco/file/gc_thumbnail'); 
  
//config
app.use(busboy());
app.use(expressWinston.logger({
	transports: [
	             new winston.transports.File({
	            	 json: true,
	            	 colorize: true,
	            	 filename: '/var/log/mediajs/messages'//,
	            	/* maxsize: 100000,
		             maxFiles: 2	*/	            
	             })//,
	            /* new winston.transports.DailyRotateFile({
	            	  filename: '/var/log/mediajs/messages',
	            	  datePattern: '.dd-MM-yyyy',
	            	})*/
	             ],
	             exitOnError: false
}));

app.use(expressWinston.errorLogger({
	transports: [
	             new winston.transports.Console({
	            	 json: true,
	            	 colorize: true,
	            	 filename: '/var/log/mediajs/nohup.log'
	             })
	             ],exitOnError: false
}));

app.route('/mediajs').get(function(req, res){
	res.send('Welcome to Nodejs Server');
});



app.route('/mediajs/file/upload').all(upload.uploadFile); // in case of direct connection (testing)
app.route('/mediajs/file/download').all(download.downloadFile);
app.route('/mediajs/thumbnail')	
.get(thumbnail.getFile)
.post(thumbnail.cacheFile);

app.route('/mediajs/db/upload').all(upload.uploadFile); 
app.route('/mediajs/db/download').all(download.downloadFile);
app.route('/mediajs/reports/download').all(download.downloadFile);
app.route('/mediajs/upgrade/upload').all(upload.uploadFile); 
app.route('/mediajs/dbrestore/upload').all(upload.uploadFile);

// This is the cleanup activity which the server initializes
// TODO: Move to an init routine/ cleanup thread. Re-write the logic in an event-emitter architecture.
var cleanupLive = setInterval(function() {
	var logger = require('./lib/barco/utils/logger');
	gc_thumbnail.cleanup_live_tn_cache(function(err) {
		if(err) { logger.error('Cleanup Live Thumbnail Cache, failed!');}
	});
}, 3600000);

var cleanupRecordingBySize = setInterval(function() {
	var logger = require('./lib/barco/utils/logger');
	gc_thumbnail.cleanup_recording_tn_cache_by_size(function(err) {
		if(err) { logger.error('Cleanup Recording Thumbnail Cache by size, failed!');}
	});
}, 3600000);

var cleanupRecordingByTime = setInterval(function() {
	var logger = require('./lib/barco/utils/logger');
	gc_thumbnail.cleanup_recording_tn_cache_by_time(function(err) {
		if(err) { logger.error('Cleanup Recording Thumbnail Cache by time, failed!');}
	});
}, 86400000);

app.listen(10089);
/*app.on('connection', function(socket) {
	  console.log("A new connection was made by a client.");
	  socket.setTimeout(10 * 1000); 
	  // 30 second timeout. Change this as you see fit.
	})*/
console.log('Express app started on port 10089');

