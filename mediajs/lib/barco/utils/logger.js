var winston = require('winston');

  //
  // Configure the logger 
  //

var config = {
		levels: {
			notify: 0,
			verbose: 1,
			info: 2,
			data: 3,
			warn: 4,
			debug: 5,
			error: 6
		},
		colors: {
			silly: 'magenta',
			verbose: 'cyan',
			info: 'green',
			data: 'grey',
			warn: 'yellow',
			debug: 'blue',
			error: 'red'
		}
};

var logger = module.exports = new (winston.Logger)({
	transports: [
	             //new winston.transports.Console(), use when need to log on console
	             new winston.transports.File({
	            	 handleExceptions: true,
	            	 json: true,
	            	 filename: '/var/log/mediajs/nohup.out',
	            	 colorize: true//,
	            	/*timestamp: 'true',
	            	maxsize: 100000,
	            	maxFiles: 2*/
	            	
	             })//,
	         /*    new winston.transports.DailyRotateFile({
	            	  filename: '/var/log/mediajs/nohup.log',
	            	  datePattern: '.dd-MM-yyyy',
	            	  //timestamp: timeFormatFn
	            	})*/
	             ],
	             levels: config.levels,
	             colors: config.colors,
	             exitOnError: false
});
winston.addColors(config.colors);
logger.setLevels(config.levels);
