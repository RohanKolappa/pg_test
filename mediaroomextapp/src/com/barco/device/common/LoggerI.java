package com.barco.device.common;

public interface LoggerI {
	public static final int ERROR = 0;
	public static final int NOTIFY = 1;
	public static final int INFO = 2;
	public static final int DEBUG = 3;
	public abstract void log(String s, int level, int ref);
}
