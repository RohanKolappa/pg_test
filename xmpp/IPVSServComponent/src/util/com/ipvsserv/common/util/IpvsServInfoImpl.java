package com.ipvsserv.common.util;

import java.util.Collection;
import java.util.Date;

import com.ipvsserv.common.config.IpvsServGlobals;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class IpvsServInfoImpl implements IpvsServInfo {
    
    private Date startDate;
    private String xmppDomain;
    private String hostname;
    private Version ver;
    
    //private ConnectionManager connectionManager;
    
    public IpvsServInfoImpl(String xmppDomain, String hostname, Version version, Date startDate) {
        this.xmppDomain = xmppDomain;
        this.hostname = hostname;
        this.ver = version;
        this.startDate = startDate;
        //this.connectionManager = connectionManager;
    }
    
    public Version getVersion() {
        return ver;
    }
    
    @Deprecated
    public String getName() {
        return getXMPPDomain();
    }
    
    @Deprecated
    public void setName(String serverName) {
        setXMPPDomain(serverName);
    }
    
    public String getHostname() {
        return hostname;
    }
    
    public String getXMPPDomain() {
        return xmppDomain;
    }
    
    public void setXMPPDomain(String domainName) {
        //todo: resolve this
        this.xmppDomain = domainName;
        if (domainName == null) {
            IpvsServGlobals.deleteProperty("xmpp.domain");
        } else {
            IpvsServGlobals.setProperty("xmpp.domain", domainName);
        }
    }
    
    public Date getLastStarted() {
        return startDate;
    }
    
    public Collection<ServerPort> getServerPorts() {
        /*if (connectionManager == null) {
            return Collections.emptyList();
        }
        else {
            return connectionManager.getPorts();
        }*/
        return null;
    }
}
