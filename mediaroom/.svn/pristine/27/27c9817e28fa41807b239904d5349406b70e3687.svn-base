package com.ipvs.config;

public class ApplicationContext {
	public static final String APPLET_CONTEXT= "applet";	
	public static final String SERVERF_CONTEXT = "server";	
	private  static String curContext = SERVERF_CONTEXT;
	public static void setApplicationContext(String pcurContext){
		curContext = pcurContext;
	}
	public static String getApplicationContext(){
		return curContext;
	}	
	public static boolean isAppletContext(){
		if(curContext.equalsIgnoreCase(APPLET_CONTEXT))
			return true;
		return false;
	}
	public static boolean isServerContext(){
		if(curContext.equalsIgnoreCase(SERVERF_CONTEXT))
			return true;
		return false;
	}

		
}
