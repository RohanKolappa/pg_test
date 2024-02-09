package com.ipvs.proxy.impl;

import java.io.InputStream;
import java.util.Properties;

public class ProxyExceptionCodes {
    public static final int NOERROR = 0;
    public static final int STREAM_ID_NOT_FOUND = 100;
    public static final int RECORDING_SESSION_NOT_FOUND = 100;
    public static final int ID_NOT_FOUND = 101;
    public static final int OTHER = 104;
    public static final int REQUEST_IN_PROCESS = 105;
    public static final int RECORD_STARTED = 106;
    public static final int RECORD_PAUSED = 107;
    public static final int ABORT_RECORDING = 108;
    public static final int RECORDING_SESSION_IN_PROGRESS = 109;
    public static final int UNSUPPORTED_REQUEST = 110;
    public static final int MEDIASTORE_ERROR = 111;
    public static final int USER_LOGGED_OFF = 112;
    public static final int RECORDING_ERROR = 113;
    public static final String OTHER_DESCRIPTION = "Error encountered while processing the request";
    
    public static String getErrorDescription(int errorCode) {
    	 Properties props = new Properties();
         try {
        	 InputStream url = ProxyExceptionCodes.class.getResourceAsStream("/com/ipvs/proxy/impl/message.properties");
        	 props.load(url);
             String message = props.getProperty(String.valueOf(errorCode));
             if(message==null)
             {
             	   return OTHER_DESCRIPTION;
             	   
             }
             return message;
         }catch(Exception e) {
        	 e.printStackTrace();
        	 return OTHER_DESCRIPTION;
         }

    }
    
}
