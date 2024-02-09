package com.ipvs.xmpp.client;

public class DBErrorException extends Exception {
    private static final long serialVersionUID = 4775534620854936584L;
    public DBErrorException(String error) {
        super(error);
    }
}