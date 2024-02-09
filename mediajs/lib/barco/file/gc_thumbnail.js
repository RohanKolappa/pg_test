var fs = require('fs');
var utils = require('../utils/common');
var logger = require('../utils/logger');

exports.cleanup_live_tn_cache = function(callback) {

    logger.info('gc_thumbnail.js: cleanup_live_tn_cache');

    //Live thumbnail cache dir
    var fileFolder = "/data/tncache/live";

    //Expiry time for thumbnail dir in seconds (7 days).
    var expiry = 604800000;

    //Call to cleanup the directory
    cleanup_expired_dir(fileFolder, expiry, function(err, resp) {
        if (err) {
            callback(err);
        } else {
            callback();
        }
    });
};

exports.cleanup_recording_tn_cache_by_time = function(callback) {

    logger.info('gc_thumbnail.js: cleanup_recording_tn_cache_by_time');

    //Recording thumbnail cache dir
    var fileFolder = "/data/tncache/recordings";

    //Expiry time for thumbnail dir in seconds (30 days).
    var expiry = 2592000000;

    //Call to cleanup the directory
    cleanup_expired_dir(fileFolder, expiry, function(err, resp) {
        if (err) {
            callback(err);
        } else {
            callback();
        }
    });
};

exports.cleanup_recording_tn_cache_by_size = function(callback) {
    logger.info('gc_thumbnail.js: cleanup_recording_tn_cache_by_size');

    //Recording thumbnail cache dir
    var fileFolder = "/data/tncache/recordings";

    //Max reserved disk size for rec thumbnail dir 5GB 
    var max_dir_size_reserved = 5368709120;

    fs.exists(fileFolder, function(exists) {
        if (exists) {

            //Check the size used by the recording tn cache dir
            var exec = require("child_process").exec;
            var command = "du -as " + fileFolder + " | cut -f 1 | egrep ^[[:digit:]]+$";

            logger.info('gc_thumbnail.js: Check size used: (' + command + ')');
            var dirSize = 0;
            var child = exec(command, function(error, stdout, stderr) {
                if (error != null) {
                    var errStr = "gc_thumbnail.js: Error reading disk size for:" + fileFolder + " " + stderr;
                    var err1 = new Error(errStr + ' ' + error);
                    logger.error(errStr);
                    callback(err1);
                }

                dirSize = stdout;
                logger.info('gc_thumbnail.js: Dir Size used by ' + fileFolder + ' is: ' + dirSize);
                if (dirSize != 0 && dirSize >= max_dir_size_reserved) {
                    logger.info('gc_thumbnail.js: Disk space used by: ' + fileFolder + ':' + dirSize + ', exceeds max reserved cache size: ' + max_dir_size_reserved);

                    fs.readdir(fileFolder, function(err, files) {
                        if (err) {
                            var errStr = "gc_thumbnail.js: Error in reading the directory: " + fileFolder;
                            var err1 = new Error(errStr + ' ' + err);
                            logger.error(errStr);
                            callback(err1);
                        }

                        if (files.length > 0) {
                            //Reverse sort by least recently accessed
                            files.sort(function(a, b) {
                                return fs.statSync(fileFolder + "/" + a).mtime.getTime() -
                                    fs.statSync(fileFolder + "/" + b).mtime.getTime();
                            }).reverse();

                            //Assuming 50K as the max size of a tn directory, remove 20000 dir to free up 1GB of space.
                            var count = 20000;
                            files.forEach(function(file, index) {
                                var accessFolderPath = fileFolder + "/" + file;

                                if (count <= 0)
                                    return;

                                utils.removeRecursive(accessFolderPath, function(err, status) {
                                    if (!err)
                                        logger.info('gc_thumbnail.js: Successfully cleaned folder ' + accessFolderPath);
                                    else
                                        logger.error('gc_thumbnail.js: Cannot remove: ' + accessFolderPath);
                                });

                                count = count - 1;
                            });
                        }
                    });
                }
            });
        }
    });
    callback(null);
};


function cleanup_expired_dir(fileFolder, expiry, callback) {

    logger.info('gc_thumbnail.js: cleanup_expired_dir: ' + fileFolder);

    var files = [];
    fs.exists(fileFolder, function(exists) {
        if (exists) {
            fs.readdir(fileFolder, function(err, files) {
                if (err) {
                    var errStr = "gc_thumbnail.js: Error in reading the directory: " + fileFolder;
                    var err1 = new Error(errStr + ' ' + err);
                    logger.error(errStr);
                    callback(err1);
                }

                if (files.length > 0) {
                    files.forEach(function(file, index) {

                        var accessFolderPath = fileFolder + "/" + file;
                        var accessFilePath = accessFolderPath + "/access";
                        logger.info('gc_thumbnail.js: Attempting to remove ' + accessFolderPath);
                        fs.stat(accessFilePath, function(err, stats) {
                            if (err) {
                                logger.error('gc_thumbnail.js: Cannot stat: ' + accessFilePath);
                                utils.removeRecursive(accessFolderPath, function(err, status) {
                                    if (!err)
                                        logger.info('gc_thumbnail.js: Successfully cleaned ' + accessFolderPath);
                                    else
                                        logger.error('gc_thumbnail.js: Cannot remove: ' + accessFolderPath);
                                });
                                return; //to break loop
                            }

                            var accessTime = stats.mtime.getTime();

                            logger.info('gc_thumbnail.js: Compare folder last access time: ' + accessTime + ' with current time: ' + Date.now());

                            //Check if last access is older than expiry.
                            if ((Date.now() - accessTime) > expiry) {
                                logger.info('gc_thumbnail.js: Folder last access time: ' + accessTime + ' is older than cache expiry:' + expiry + ' secs');

                                utils.removeRecursive(accessFolderPath, function(err, status) {
                                    if (!err)
                                        logger.info('gc_thumbnail.js: Successfully cleaned folder ' + accessFolderPath);
                                    else
                                        logger.error('gc_thumbnail.js: Cannot remove: ' + accessFolderPath);
                                });
                            }

                        });
                    });
                }

            });
        }
    });
    callback(null);
};