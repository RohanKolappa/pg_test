package com.ipvsserv.common.util;

import java.util.Collection;
import java.util.Date;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface IpvsServInfo {
    
    public Version getVersion();
    
    @Deprecated
    public String getName();
    
    @Deprecated
    public void setName(String serverName);
    
    public String getHostname();
    
    public String getXMPPDomain();
    
    public void setXMPPDomain(String domainName);
    
    public Date getLastStarted();
    
    public Collection<ServerPort> getServerPorts();
}
