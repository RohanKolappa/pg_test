package com.ipvs.ext.xmppclient;

public class IpvsXmppLog {
	
	static IpvsXmppLoggerI logger;

	public static void setLogger(IpvsXmppLoggerI logger) {
		IpvsXmppLog.logger = logger;
	}
	public static void log(String s, int level, int ref) {
		if(IpvsXmppLog.logger != null)
			IpvsXmppLog.logger.log(s, level, ref);
		else if(level <= ref)
			System.out.println(s);
	}

}
