package com.ipvs.proxy.impl;

public class ProxyException extends Exception {
    private static final long serialVersionUID = -5141663424246925758L;
    int code = 0;
	String data = null;
	
	public ProxyException(int code , String data) {
		this.code = code;
		this.data = data;
	}
	
	@Override
	public String toString() {
	
		return "Code: " + code + " " + "Data: " + data; 
	}
	
	public int getCode() {
		return code;
	}
	
	public String getData() {
		return data;
	}

}
