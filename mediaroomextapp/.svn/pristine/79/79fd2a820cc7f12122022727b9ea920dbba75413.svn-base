package com.barco.utils.mr;

import java.io.PrintStream;
import java.io.PrintWriter;
import java.io.StringWriter;
import java.text.SimpleDateFormat;
import java.util.Date;

import org.apache.log4j.Logger;


/**
 * Implements the common logging class to be used by all services
 * @author miyer
 *
 */
public class MRLog {

	public static final int ERROR = 0;
	public static final int NOTIFY = 1;
	public static final int INFO = 2;
	public static final int DEBUG = 3;
	public static final int OK = 200;
	public static final int UPDATE = 201;
	public static final int BEGINMETHOD = 100;
	public static final int ENDMETHOD = 101;
	

	//public static final Logger logger = Logger.getLogger(MRLog.class);
	private static  Logger logger = null;;

	//applet must use this  LOGGER_CATEGORY
	public static final String CONSOLE="CONSOLE";
	//linux server can use DAEMON or CONSOLE_DAEMON. Default is DAEMON
	public static final String DAEMON="DAEMON";
	public static final String CONSOLE_DAEMON="CONSOLE_DAEMON";
	//in case want to use files 
	public static final String FILE_DETAIL="FILE_DETAIL";
	public static final String FILE_SUMMARY="FILE_SUMMARY";
	//set LOGGER_CATEGORY to DAEMON as default
	private static String LOGGER_CATEGORY = CONSOLE;
	//public static boolean enableSysLog = true;
	int _logLevel;
	StringBuffer _log;
	Date _date;
	
	private static MRLog mrlog = null;
	public static MRLog getInstance(String logger_category) {

        if (mrlog == null) {
            setLogger_category(logger_category);
            mrlog = new MRLog();
            //redirect system out and err to log4j
            tieSystemOutAndErrToLog(logger_category);
            //testConsoleToLog4j();
        }
        return mrlog;
    }

    public static MRLog getInstance(String logger_category, boolean bforceNewInstance) {
        if (mrlog == null || bforceNewInstance) {
            setLogger_category(logger_category);
            mrlog = new MRLog();
            //redirect system out and err to log4j
            tieSystemOutAndErrToLog(logger_category);
            //testConsoleToLog4j();
        }
        return mrlog;
    }	
	private MRLog() {
		_date = new Date();
		_logLevel = MRLog.ERROR;
		_log = new StringBuffer();
		
		if(logger == null) {
			 setLogger_category(getLogger_category());
		}
	}
    
	///////////// add code to proxy exceptions START//////////
    public static void tieSystemOutAndErrToLog(String logger_category) {
        //System.out.println(logger_category + " isComponentLogin  " + IPVSConst.getInstance().isComponentLogin());
        //if (DAEMON.equals(logger_category) && IPVSConst.getInstance().isComponentLogin()) {
        if (DAEMON.equals(logger_category)) {
            //redirect system out and err to log4j
            System.setOut(createLoggingProxy(System.out));
            System.setErr(createLoggingProxy(System.err));
            //testConsoleToLog4j();
        }
    }

    public static PrintStream createLoggingProxy(final PrintStream realPrintStream) {
        return new PrintStream(realPrintStream) {
            public void print(final String string) {
                realPrintStream.print(string);
                //todo:levels
                //logger.debug(string);
                mrlog.addInfo(string, MRLog.ERROR, MRLog.ERROR);
            }
        };
    }

    /*
     private static void testConsoleToLog4j() {
        System.out.println("TESTING tieSystemOutAndErrToLog "  );
        try {
           throw new Exception("RAISE ERROR TESTING tieSystemOutAndErrToLog");
       } catch (Exception ex) {
           ex.printStackTrace();
       }
       System.out.println("DONE TESTING tieSystemOutAndErrToLog "  );

    }*/
    ///////////// add code to proxy exceptions END//////////

    private void appendLog(String str) {
        // TBD : Ensure that flush log is getting called
        // this._log.append(str);
    }

	/////////////// log4j methods
	public static String getLogger_category() {
		return LOGGER_CATEGORY;
	}
	//this will be used for internal looger
	public static void setLogger_category(String logger_category) {
		try{
			LOGGER_CATEGORY = logger_category;
			if(!logger_category.trim().equalsIgnoreCase(CONSOLE))
				logger = Logger.getLogger(getLogger_category());
			System.out.println("Logger used is " + getLogger_category());
			//logger.debug("<info time=" + System.currentTimeMillis() + ">" + getLogger_category() + "</info>");			
		}catch(Exception ex){
		}
	}
	//get logger for outside use from here. this looger in not used for internal purposes
	public static Logger getLogger(String logger_category ) {
		Logger llogger =null;
		try{
			if(!logger_category.trim().equalsIgnoreCase(CONSOLE))
				llogger = Logger.getLogger(logger_category);
		}catch(Exception ex){
		}
		return llogger;
	}
	//get internal logger for outside use.
	public static Logger getLogger() {
		try{
			if(logger == null){
				if(!getLogger_category().trim().equalsIgnoreCase(CONSOLE))
					logger = Logger.getLogger(getLogger_category());
			}
		}catch(Exception ex){
		}
		return logger;
	}
	//outside logger can be set for internal use
	public static void setLogger(Logger plogger) {
		try{
			logger = plogger;
		}catch(Exception ex){
		}
	}

	/**
	 * Set the global log level for the log instance
	 * @param logLevel
	 */
	public void setLogLevel(int logLevel) {
		_logLevel = logLevel;
	}
	public int getLogLevel() {
		return _logLevel ;
	}
	public boolean isLogLevelInfo() {
		if( _logLevel ==INFO) 
			return true;
		return false;
	}
	public boolean isLogLevelDebug() {
		if( _logLevel ==DEBUG) 
			return true;
		return false;
	}
	/**
	 * Flush the log for the current requestID in the request
	 * @param request the user session
	 * @return the flushed log string
	 */
	public String flush(String requestNID) {
		// TBD flush only the entries that match the request id
		String response = _log.toString();
		_log = new StringBuffer();
		return response;
	}

	public void addLog(MRRequestI request, int level, int code, String data) {
		if( (level <= _logLevel) || (level <= request.getLogLevel()) )
			this.appendLog(request.getNID() + ":" + _date.getTime() + ":" + code + ":" + data + "\n");
	}

	private static long lasttime = System.currentTimeMillis();
	public static long getDiffTime() {

		long curtime = System.currentTimeMillis();
		long diffTime = curtime - lasttime;

		lasttime = System.currentTimeMillis();
		return diffTime;
	}
	public void addSystemOut(int level, int code, String data, String systemOutAndLog) {
		// systemOutAndLog="12" Log=systemOutAndLog="1" , System.out=systemOutAndLog="2"
		// to restrict debug messages appending to _log  
		// ... danger of out of memory, since the _log grows.
		//... May be we need server log file to see what is happening at server.
		if(level <= _logLevel) {
			if(systemOutAndLog.indexOf("1") !=-1)
				this.appendLog("0" +  ":" + _date.getTime() + ":" + code + ":" + data + "\n");
			if(systemOutAndLog.indexOf("2")  !=-1){
				long curtime = System.currentTimeMillis();
				long diffTime = curtime-lasttime;
				System.out.println("<info time=" + System.currentTimeMillis() + ">time spent:"+diffTime +": "  + data + "</info>");				
				lasttime = System.currentTimeMillis();
			}
		}
	}	

	public void addLog(int level, int code, String data) {
		if(level <= _logLevel)
			this.appendLog("0" +  ":" + _date.getTime() + ":" + code + ":" + data + "\n");
	}
	public static boolean isDaemonLoggerOn() {
		if(ApplicationContext.isAppletContext())
			return false;
		if(LOGGER_CATEGORY.equals(DAEMON) || LOGGER_CATEGORY.equals(CONSOLE_DAEMON)){
			return true;
		}
		return false;
	}	
	public void addInfoDaemon(String data) {
		if(!isDaemonLoggerOn())
			return;
		//int code = MRLog.OK;
		//int level = MRLog.INFO;
		if( logger!=null)
			logger.debug(data);
		//else
		  //  System.out.println("<info time=" + System.currentTimeMillis() + ">" + data + "</info>");
		//addLog(level, code, data);
		//if(level <= _logLevel)
		//	_log.append("0" +  ":" + _date.getTime() + ":" + code + ":" + data + "\n");
	}
	
	public void addInfo(String data) {
		this.addInfo(data, MRLog.OK, MRLog.INFO);
	}
	
	public static String stackToString(Exception e) {
        try {
            StringWriter sw = new StringWriter();
            PrintWriter pw = new PrintWriter(sw);
            e.printStackTrace(pw);
            return   sw.toString()  ;
        } catch (Exception e2) {
            return "";
        }
    }
	
	public void addInfo(String data, int code, int level) {
		if(level > this._logLevel)
			return;		
		if( logger!=null)
			logger.debug("<info time=" + System.currentTimeMillis() + ">" + data + "</info>");
		else
		    System.out.println("<info time='" + System.currentTimeMillis() + "'>" + data + "</info>");

		addLog(level, code, data);
	}

	public String getMethodName() {
		return "methodName"; // TBD
	}

	public void addTraceBegin(MRRequestI request) {
		addLog(request, MRLog.DEBUG, MRLog.BEGINMETHOD, getMethodName());
	}

	public void addTraceEnd(MRRequestI request) {
		addLog(request, MRLog.DEBUG, MRLog.ENDMETHOD, getMethodName());
	}






	///////////////////// modified by pravin for new api in branch////////////////////////////////////
	public void addSystemOutNewLine() {
		System.out.println( "\n");
	}

	
	/// add date to System.out.println
    private static long start=System.currentTimeMillis();
    private static SimpleDateFormat format = new SimpleDateFormat("MM/d/yyyy HH:mm:ss:SSS z");
    public static void println(String msg){
        long diff = System.currentTimeMillis() - start;
        start=System.currentTimeMillis() ;
        System.out.println( format.format(new Date()) + "("+diff+") " + msg );
    }	
}
