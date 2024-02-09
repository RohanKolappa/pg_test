package com.ipvsserv.xmpp.mdc;

import java.util.List;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface DomainManager {
    
    //public abstract DBCache getDBCache();
    
    public abstract List<String> getDomainNames();
    
    public abstract IpvsDomain getDomain(String domainName);
    
    public abstract IpvsDomain addDomain(String domainName);
    
    public abstract IpvsDomain removeDomain(String domainName);
    
    public abstract void setParentIpvsModuleName(String parentIpvsModuleName);
    
    public abstract String getParentIpvsModuleName();
    
    public abstract void saveSubscriptions();
    
    public abstract void clear();
}
