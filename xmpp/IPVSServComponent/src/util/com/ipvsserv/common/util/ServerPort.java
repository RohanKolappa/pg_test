package com.ipvsserv.common.util;

import java.util.ArrayList;
import java.util.Collections;
import java.util.List;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class ServerPort {
    
    private int port;
    private List<String> names = new ArrayList<String>(1);
    private String address;
    private boolean secure;
    private String algorithm;
    private Type type;
    
    public ServerPort(int port, String name, String address, boolean isSecure, String algorithm, Type type) {
        this.port = port;
        this.names.add(name);
        this.address = address;
        this.secure = isSecure;
        this.algorithm = algorithm;
        this.type = type;
    }
    
    public int getPort() {
        return port;
    }
    
    public List<String> getDomainNames() {
        return Collections.unmodifiableList(names);
    }
    
    public String getIPAddress() {
        return address;
    }
    
    public boolean isSecure() {
        return secure;
    }
    
    public String getSecurityType() {
        return algorithm;
    }
    
    public boolean isServerPort() {
        return type == Type.server;
    }
    
    public boolean isClientPort() {
        return type == Type.client;
    }
    
    public boolean isComponentPort() {
        return type == Type.component;
    }
    
    public boolean isConnectionManagerPort() {
        return type == Type.connectionManager;
    }
    
    public Type getType() {
        return type;
    }
    
    public static enum Type {
        client,

        server,

        component,

        connectionManager
    }
}
