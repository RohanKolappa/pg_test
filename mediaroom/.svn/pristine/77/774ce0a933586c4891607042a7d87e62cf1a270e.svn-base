package com.ipvs.common;

import java.util.Hashtable;

public class MRRequestCache {
	private Hashtable<String, Object> table;
	public MRRequestCache() {
		this.table = new Hashtable<String, Object>();
	}
	public void put(String key, Object value) {
		this.table.put(key, value);
	}
	public Object get(String key) {
		return this.table.get(key);
	}
	public boolean containsKey(String key) {
		return this.table.containsKey(key);
	}
}
