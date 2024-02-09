package com.ipvsserv.common.log;

import java.io.PrintStream;

import org.apache.log4j.Appender;
import org.apache.log4j.Logger;
import org.apache.log4j.spi.ErrorHandler;
import org.apache.log4j.spi.LoggingEvent;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class Log4JErrorHandler implements ErrorHandler {
    
    final static String WARN_PREFIX = "log4j warning: ";
    final static String ERROR_PREFIX = "log4j error: ";
    
    boolean firstTime = true;
    
    static PrintStream output = System.err;
    
    public static void setOutput(PrintStream out) {
        Log4JErrorHandler.output = out;
    }
    
    public void setLogger(Logger logger) {}
    
    public void activateOptions() {}
    
    public void error(String message, Exception e, int errorCode) {
        error(message, e, errorCode, null);
    }
    
    public void error(String message, Exception e, int errorCode, LoggingEvent event) {
        if (firstTime) {
            Log4JErrorHandler.output.println(Log4JErrorHandler.ERROR_PREFIX + message);
            e.printStackTrace(Log4JErrorHandler.output);
            firstTime = false;
        }
    }
    
    public void error(String message) {
        if (firstTime) {
            Log4JErrorHandler.output.println(Log4JErrorHandler.ERROR_PREFIX + message);
            firstTime = false;
        }
    }
    
    public void setAppender(Appender appender) {}
    
    public void setBackupAppender(Appender appender) {}
}
