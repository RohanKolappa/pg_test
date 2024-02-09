package com.ipvsserv.nxd.mdc.iq;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.config.PropertyEventListener;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.mdc.sub.SubscriptionsMDCI;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.xmpp.mdc.DomainManager;
import com.ipvsserv.xmpp.mdc.IpvsDomain;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DomainManagerImpl implements PropertyEventListener, DomainManager {
    private ConcurrentHashMap<String, DomainMDC> domains = new ConcurrentHashMap<String, DomainMDC>();
    private static DomainManager domainManager = null;
    private String parentIpvsModuleName = "";
   // private DBCache dbCache = null;
    
    //public DBCache getDBCache() {
    //    return dbCache;
   // }
    
    public DomainManagerImpl() {}
    
    public synchronized List<String> getDomainNames() {
        List<String> list = new ArrayList<String>();
        try {
            list.addAll(domains.keySet());
        } catch (Exception ex) {}
        return list;
    }
    
    /* (non-Javadoc)
     * @see com.ipvsserv.nxd.mdc.domain.DomainManagerMDCI#getDomain(java.lang.String)
     */
    public synchronized IpvsDomain getDomain(String domainName) {
        return domains.get(domainName);
    }
    
    public synchronized IpvsDomain addDomain(String domainName) {
        //DomainMDC domain = new DomainMDC(domainName, getSubscriptionsMDCI(), getDBCacheI());
        DomainMDC domain = new DomainMDC(domainName, getSubscriptionsMDCI());
        setXmlConfig(domain);
        domains.putIfAbsent(domainName, domain);
        return domains.get(domainName);
    }
    
    public synchronized IpvsDomain removeDomain(String domainName) {
        if (!domains.containsKey(domainName)) {
            return domains.remove(domainName);
        }
        return null;
    }
    
    private synchronized SubscriptionsMDCI getSubscriptionsMDCI() {
        SubscriptionsMDCI subs = null;
        String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".impl.subscription", "com.ipvsserv.nxd.mdc.domain.SubscriptionsMDCI");
        if (!className.equals("")) {
            MRLog.debug(" SubscriptionsMDCI CLASS using " + className);
            try {
                subs = (SubscriptionsMDCI) Class.forName(className).newInstance();
                setXmlConfig(subs);
            } catch (InstantiationException ex) {
                MRLog.error(ex);
            } catch (IllegalAccessException ex) {
                MRLog.error(ex);
            } catch (ClassNotFoundException ex) {
                MRLog.error(ex);
            }
        }
        if (subs == null) {
            MRLog.debug(className + " not found ");
        }
        return subs;
    }
    /*
    private synchronized DBCache getDBCacheI() {
        if (dbCache == null) {
            String className = IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".impl.dbCache", "com.ipvsserv.nxd.mdc.db.DBCache");
            if (!className.equals("")) {
                MRLog.debug(" DBCacheI CLASS using " + className);
                try {
                    
                    dbCache = (DBCache) Class.forName(className).newInstance();
                    setXmlConfig(dbCache);
                    dbCache.init();
                    
                } catch (InstantiationException ex) {
                    MRLog.error(ex);
                } catch (IllegalAccessException ex) {
                    MRLog.error(ex);
                } catch (ClassNotFoundException ex) {
                    MRLog.error(ex);
                }
            }
            if (dbCache == null) {
                MRLog.debug(className + " not found ");
            }
        }
        return dbCache;
    }*/
    
    public void setParentIpvsModuleName(String parentIpvsModuleName) {
        this.parentIpvsModuleName = parentIpvsModuleName;
    }
    
    public String getParentIpvsModuleName() {
        return parentIpvsModuleName;
    }
    
    private void setXmlConfig(DomainMDC domain) {
        domain.setSubscriptionsSaveEnabled(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".subscription.saveToDB", false));
        domain.setAddUpdateMergeEnable(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".updatewithmerge.enable", false));
        domain.setAddUpdateAssert(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".updatewithmerge.assert", false));
    }
    
    private void setXmlConfig(SubscriptionsMDCI subs) {
        subs.enableSubscriptions(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".subscription.enable", true));
        subs.setSaveSubscriptionsToDB(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".subscription.saveToDB", false));
        subs.setSaveSubscriptionsInterval(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".subscription.saveIntervalMinutes", 2 * 60 * 60));
        subs.enableSubscriptionsLog(MRLog.isSubscriptionLogEnable());
    }
    
   /* private void setXmlConfig(DBCache dbCache) {
        dbCache.enableDBCache(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".dbCache.enable", true));
        dbCache.setMaxCount(IpvsServGlobals.getXMLProperty(parentIpvsModuleName + ".dbCache.maxLimit", 100));
        dbCache.enableDBCacheLog(MRLog.isDbcacheLogEnable());
    }*/
    
    @Override
    public void propertyDeleted(String property, Map<String, Object> params) {
        xmlPropertySet(property, params);
    }
    
    @Override
    public void propertySet(String property, Map<String, Object> params) {
        xmlPropertySet(property, params);
        
    }
    
    @Override
    public void xmlPropertyDeleted(String property, Map<String, Object> params) {
        xmlPropertySet(property, params);
        
    }
    
    @Override
    public void xmlPropertySet(String property, Map<String, Object> params) {
        List<String> list = domainManager.getDomainNames();
        for (Iterator<String> i = list.iterator(); i.hasNext();) {
            String domainName = i.next();
            DomainMDC domain = (DomainMDC) domainManager.getDomain(domainName);
            if (domain != null) {
                setXmlConfig(domain);
                setXmlConfig(domain.getSubs());
                //setXmlConfig(domain.getDbCache());
            }
        }
        
    }
    
    @Override
    public void saveSubscriptions() {
        List<String> list = getDomainNames();
        for (Iterator<String> i = list.iterator(); i.hasNext();) {
            String domainName = i.next();
            DomainMDC domain = (DomainMDC) getDomain(domainName);
            if (domain != null) {
                domain.getSubs().saveSubscriptions(new RequestMDCParams(domainName));
            }
        }
        
    }
    
    @Override
    public void clear() {
        //if (dbCache != null) {
        //    dbCache.clear();
        //}
        List<String> list = getDomainNames();
        for (Iterator<String> i = list.iterator(); i.hasNext();) {
            String domainName = i.next();
            try {
                DomainMDC domain = (DomainMDC) removeDomain(domainName);
                if (domain != null) {
                    domain.clear();
                    domain.getSubs().saveSubscriptions(new RequestMDCParams(domainName));
                }
                domain = null;
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
    }
    
}
