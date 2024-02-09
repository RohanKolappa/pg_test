package com.barco.utils.mr;

import java.util.Hashtable;

public interface MRRequestI {
	public abstract String getNID();
	public abstract int getLogLevel();
	public MRRequestCache getCache();
}
