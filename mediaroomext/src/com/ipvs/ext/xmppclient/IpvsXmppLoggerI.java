package com.ipvs.ext.xmppclient;

public interface IpvsXmppLoggerI {
	public static int DEBUG = 5;
	public static int INFO = 4;
	public static int WARN = 3;
	public static int ERROR = 2;
	public static int FATAL = 1;
	public void log(String s, int level, int ref);
}
