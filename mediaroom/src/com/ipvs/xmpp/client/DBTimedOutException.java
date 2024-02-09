package com.ipvs.xmpp.client;

public class DBTimedOutException extends Exception {
    private static final long serialVersionUID = -995425672692655477L;
    public DBTimedOutException(String error) {
        super(error);
    }
}