package com.ipvsserv.common.config;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsServConstants {
    
    public static final int SYSTEM = 17;
    public static final int ROSTER = 18;
    public static final int OFFLINE = 19;
    public static final int MUC_ROOM = 23;
    public static final int SECURITY_AUDIT = 25;
    
    public static final long SECOND = 1000;
    public static final long MINUTE = 60 * SECOND;
    public static final long HOUR = 60 * MINUTE;
    public static final long DAY = 24 * HOUR;
    public static final long WEEK = 7 * DAY;
    
    public static final String XMPP_DATETIME_FORMAT = "yyyy-MM-dd'T'HH:mm:ss.SSS'Z'";
    public static final String XMPP_DELAY_DATETIME_FORMAT = "yyyyMMdd'T'HH:mm:ss";
}
