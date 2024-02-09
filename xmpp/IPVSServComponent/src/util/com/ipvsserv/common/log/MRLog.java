package com.ipvsserv.common.log;

import java.io.File;
import java.net.URL;

import org.apache.log4j.Level;
import org.apache.log4j.Logger;
import org.apache.log4j.xml.DOMConfigurator;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.util.CompUtils;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class MRLog {
    private static Logger log = null;
    //private static Properties enableLogs = new Properties();
    private static String XML_FILE = "/com/ipvs/serv/common/log/log4j.xml";
    private static boolean bLogToFile = false;
    private static boolean bLogToConsole = false;
    private static long lasttime = System.currentTimeMillis();
    
    private static boolean jettyLogEnable;
    private static boolean logToFile;//todo: use this 
    private static boolean subscriptionLogEnable;
    private static boolean presenceLogEnable;
    private static boolean dbcacheLogEnable;
    private static boolean connectionhandlerLogEnable;
    private static boolean packetsummaryLogEnable;
    private static boolean packetdetailsLogEnable;
    private static boolean dbsummaryLogEnable;
    private static boolean notificationsummaryLogEnable;
    private static boolean dbqueryLogEnable;
    private static boolean dbqueryresultsLogEnable;
    private static boolean connectionpoolLogEnable;
    private static boolean ipvsxmlpacketsLogEnable;
    private static boolean interComponentLogEnable;
    private static boolean dbadminLogEnable;
       
    
    public static void setLogger_category(String logger_category) {
        try {
            MRLog.log = Logger.getLogger(logger_category);
            MRLog.debug("Logger used is  " + logger_category);
           
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    /*
    <logs>
        <enable>
            <jetty>false</jetty>
            <logToFile>true</logToFile>
            <subscription>false</subscription>
            <dbcache>false</dbcache>
            <presence>false</presence>
            <connectionhandler>false</connectionhandler>
            <packetsummary>true</packetsummary>
            <packetdetails>false</packetdetails>
            <dbsummary>true</dbsummary>
            <notificationsummary>false</notificationsummary>
        </enable>
    </logs>      
     */
    public static void setLogLevels() {
        try {
            bLogToConsole=false;
            if(System.getProperty("LogToConsole") !=null){
                bLogToConsole = CompUtils.booleanValue(System.getProperty("LogToConsole","false"), false);
            }
            jettyLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.jetty");
            logToFile = IpvsServGlobals.getBooleanProperty("logs.enable.logToFile");
            subscriptionLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.subscription");
            dbcacheLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.dbcache");
            presenceLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.presence");
            connectionhandlerLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.connectionhandler");
            packetsummaryLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.packetsummary");
            packetdetailsLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.packetdetails");
            dbsummaryLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.dbsummary");
            notificationsummaryLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.notificationsummary");
            dbqueryLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.dbquery");
            dbqueryresultsLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.dbqueryresults");
            connectionpoolLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.connectionpool");
            ipvsxmlpacketsLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.ipvsxmlpackets");
            interComponentLogEnable = IpvsServGlobals.getBooleanProperty("logs.enable.intercomponent");
            dbadminLogEnable= IpvsServGlobals.getBooleanProperty("logs.enable.dbadmin");
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    
    public static boolean isDbadminLogEnable() {
        return dbadminLogEnable;
    }

    public static boolean isInterComponentLogEnable() {
        return interComponentLogEnable;
    }

    public static void setInterComponentLogEnable(boolean interComponentLogEnable) {
        MRLog.interComponentLogEnable = interComponentLogEnable;
    }

    public static boolean isIpvsxmlpacketsLogEnable() {
        return ipvsxmlpacketsLogEnable;
    }
    
    public static boolean isConnectionpoolLogEnable() {
        return connectionpoolLogEnable;
    }
    
    public static boolean isDbqueryresultsLogEnable() {
        return dbqueryresultsLogEnable;
    }
    
    public static void setDbqueryresultsLogEnable(boolean dbqueryresultsLogEnable) {
        MRLog.dbqueryresultsLogEnable = dbqueryresultsLogEnable;
    }
    
    public static boolean isDbqueryLogEnable() {
        return dbqueryLogEnable;
    }
    
    public static void setDbqueryLogEnable(boolean dbqueryLogEnable) {
        MRLog.dbqueryLogEnable = dbqueryLogEnable;
    }
    
    public static boolean isJettyLogEnable() {
        return jettyLogEnable;
    }
    
    public static void setJettyLogEnable(boolean jettyLogEnable) {
        MRLog.jettyLogEnable = jettyLogEnable;
    }
    
    public static boolean isSubscriptionLogEnable() {
        return subscriptionLogEnable;
    }
    
    public static void setSubscriptionLogEnable(boolean subscriptionLogEnable) {
        MRLog.subscriptionLogEnable = subscriptionLogEnable;
    }
    
    public static boolean isPresenceLogEnable() {
        return presenceLogEnable;
    }
    
    public static void setPresenceLogEnable(boolean presenceLogEnable) {
        MRLog.presenceLogEnable = presenceLogEnable;
    }
    
    public static boolean isDbcacheLogEnable() {
        return dbcacheLogEnable;
    }
    
    public static void setDbcacheLogEnable(boolean dbcacheLogEnable) {
        MRLog.dbcacheLogEnable = dbcacheLogEnable;
    }
    
    public static boolean isConnectionhandlerLogEnable() {
        return connectionhandlerLogEnable;
    }
    
    public static void setConnectionhandlerLogEnable(boolean connectionhandlerLogEnable) {
        MRLog.connectionhandlerLogEnable = connectionhandlerLogEnable;
    }
    
    public static boolean isPacketsummaryLogEnable() {
        return packetsummaryLogEnable;
    }
    
    public static void setPacketsummaryLogEnable(boolean packetsummaryLogEnable) {
        MRLog.packetsummaryLogEnable = packetsummaryLogEnable;
    }
    
    public static boolean isPacketdetailsLogEnable() {
        return packetdetailsLogEnable;
    }
    
    public static void setPacketdetailsLogEnable(boolean packetdetailsLogEnable) {
        MRLog.packetdetailsLogEnable = packetdetailsLogEnable;
    }
    
    public static boolean isDbsummaryLogEnable() {
        return dbsummaryLogEnable;
    }
    
    public static void setDbsummaryLogEnable(boolean dbsummaryLogEnable) {
        MRLog.dbsummaryLogEnable = dbsummaryLogEnable;
    }
    
    public static boolean isNotificationsummaryLogEnable() {
        return notificationsummaryLogEnable;
    }
    
    public static void setNotificationsummaryLogEnable(boolean notificationsummaryLogEnable) {
        MRLog.notificationsummaryLogEnable = notificationsummaryLogEnable;
    }
    
    //    public static void loadLog4j(String log4jConfigPath) {
    //        try {
    //            DOMConfigurator.configure(log4jConfigPath);
    //            log = Logger.getLogger("FILE_DETAILS");
    //        } catch (Exception ex) {
    //            MRLog.debug("failed to log4jConfigPath " + log4jConfigPath);
    //            ex.printStackTrace();
    //            if (MRLog.log == null) {
    //                try {
    //                    URL url = MRLog.class.getResource(XML_FILE);
    //                    DOMConfigurator.configure(url);
    //                    log = Logger.getLogger("FILE_DETAILS");
    //                } catch (Exception ex2) {
    //                    ex2.printStackTrace();
    //                }
    //            }
    //        }
    //    }
    public static void loadLog4j() {
        try {
            if (MRLog.log == null) {
                String log4jConfigFilePath = System.getProperty("log4jConfigFilePath");
                if (log4jConfigFilePath == null || log4jConfigFilePath.trim().equals("")) {
                    MRLog.debug("log4jConfigFilePath does not exists " + log4jConfigFilePath + " usiung default " + XML_FILE);
                    log4jConfigFilePath = XML_FILE;
                    URL url = MRLog.class.getResource(log4jConfigFilePath);
                    DOMConfigurator.configure(url);
                } else {
                    File file = new File(log4jConfigFilePath);
                    if (!file.exists()) {
                        MRLog.debug("log4jConfigFilePath does not exists " + log4jConfigFilePath);
                    } else {
                        DOMConfigurator.configure(log4jConfigFilePath);
                    }
                }
                log = Logger.getLogger("FILE_DETAILS");
            }
        } catch (Exception ex2) {
            ex2.printStackTrace();
        }
    }
    
    public static Logger getLogger_category(String logger_category) {
        Logger logger = Logger.getLogger(logger_category);
        return logger;
    }
    
    public static void setLogToConsole() {
        MRLog.bLogToConsole = true;
        MRLog.bLogToFile = false;
    }
    
    public static boolean isLogToConsole() {
        return MRLog.bLogToConsole;
    }
    
    public static void setLogToFile(boolean logToFile) {
        MRLog.bLogToFile = logToFile;
    }
    
    public static boolean isLogToFile() {
        if (MRLog.isLogToConsole()) {
            return false;
        }
        if (!MRLog.bLogToFile) {
            return false;
        }
        return true;
    }
    
    public static void setLogToFile() {
        if (MRLog.isLogToConsole()) {
            return;
        }
        MRLog.bLogToFile = true;
    }
    
    public static String getThreadName() {
        try {
            return Thread.currentThread().getName() + ":";
        } catch (Exception ex) {
            return "";
        }
    }
    
    public static long getDiffTime() {
        
        long curtime = System.currentTimeMillis();
        long diffTime = curtime - MRLog.lasttime;
        
        MRLog.lasttime = System.currentTimeMillis();
        return diffTime;
    }
    
    //todo: log levels : get it from Log4j
    public static boolean isDebugEnabled() {
        if (log.getLevel().isGreaterOrEqual(Level.DEBUG)){
            return true;
        }
        return true;
    }
    
    public static boolean isErrorEnabled() {
        if (log.getLevel().isGreaterOrEqual(Level.ERROR)) {
            return true;
        }
        return true;
    }
    
    public static boolean isInfoEnabled() {
        if (log.getLevel().isGreaterOrEqual(Level.INFO)){
            return true;
        }
        return true;
    }
    
    public static boolean isWarnEnabled() {
        if (log.getLevel().isGreaterOrEqual(Level.WARN)) {
            return true;
        }
        return true;
    }
    
    public static void logTossConsole(String msg) {
        if (MRLog.bLogToConsole) {
            System.out.println(MRLog.getThreadName() + " " + msg);
        }
    }
    
    public static void debug(String msg) {
        if (MRLog.bLogToConsole) {
            System.out.println(MRLog.getThreadName() + " " + msg);
        }
        log.debug(MRLog.getThreadName() + " " + msg);
    }
    
    public static void debug2(String msg) {
        if (MRLog.bLogToConsole){
            System.out.println(msg);
        }
        log.debug(msg);
    }
    
    public static void error(String msg) {
        if (MRLog.bLogToConsole) {
            System.out.println(MRLog.getThreadName() + " " + msg);
        }
        
        log.error(MRLog.getThreadName() + " " + msg);
    }
    
    public static void info(String msg) {
        if (MRLog.bLogToConsole) {
            System.out.println(MRLog.getThreadName() + " " + msg);
        }
        log.info(MRLog.getThreadName() + " " + msg);
    }
    
    public static void warn(String msg) {
        if (MRLog.bLogToConsole) {
            System.out.println(MRLog.getThreadName() + " " + msg);
        }
        log.warn(MRLog.getThreadName() + " " + msg);
    }
    
    public static void fatal(String msg) {
        if (MRLog.bLogToConsole) {
            System.out.println(MRLog.getThreadName() + " " + msg);
        }
        log.fatal(MRLog.getThreadName() + " " + msg);
    }
    
    public static void error(Exception ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("ERROR " + MRLog.getThreadName());
                ex.printStackTrace();
            }
            if (log == null) {
                ex.printStackTrace();
                return;
            }
            log.error(MRLog.getThreadName(), ex);
        }
    }
    
    public static void error(Throwable ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("ERROR " + MRLog.getThreadName());
                ex.printStackTrace();
            }
            if (log == null) {
                return;
            }
            log.error(MRLog.getThreadName(), ex);
            
        }
    }
    
    public static void warn(String msg, Exception ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("WARN " + MRLog.getThreadName() +" " + msg);
                ex.printStackTrace();
            }
            if (log == null) {
                return;
            }
            log.error(MRLog.getThreadName() + " " + msg, ex);
        }
    }
    
    public static void error(String msg, Exception ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("ERROR " + MRLog.getThreadName() +" " + msg);
                ex.printStackTrace();
            }
            if (log == null) {
                return;
            }
            log.error(MRLog.getThreadName() + " " + msg, ex);
        }
    }
    
    public static void error(String msg, Throwable ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("ERROR " + MRLog.getThreadName() +" " + msg);
                ex.printStackTrace();
            }
            if (log == null) {
                return;
            }
            log.error(MRLog.getThreadName() + " " + msg, ex);
        }
    }
    
    public static void debug(String msg, Throwable ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("DEBUG " + MRLog.getThreadName() +" " + msg);
                ex.printStackTrace();
            }
            if (log == null) {
                return;
            }
            log.debug(MRLog.getThreadName() + " " + msg, ex);
        }
    }
    
    public static void warn(String msg, Throwable ex) {
        if (ex != null) {
            if (MRLog.bLogToConsole) {
                System.out.println("WARN " + MRLog.getThreadName() +" " + msg);
                ex.printStackTrace();
            }
            if (log == null) {
                return;
            }
            log.warn(MRLog.getThreadName() + " " + msg, ex);
        }
    }
    
}
