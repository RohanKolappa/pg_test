/*
 * Watch event handler (observer pattern)
 * If changes made to a file are to be watched, getInstance() of Watchfile and call emit method on it when file changes
 */
var util = require("util")
  , logger = require('./logger')
  , EventEmitter = process.EventEmitter
  , instance;

function WatchFile() {
  EventEmitter.call(this);
}

util.inherits(WatchFile, EventEmitter);
WatchFile.prototype.upload = function(file) {
    var statusMessage = 'uploaded: ' + file;  
    instance.emit(file, statusMessage);    
     
}
module.exports = {
  // Singleton watchfile instance
  getInstance: function() {
    return instance || (instance = new WatchFile());
  }
};