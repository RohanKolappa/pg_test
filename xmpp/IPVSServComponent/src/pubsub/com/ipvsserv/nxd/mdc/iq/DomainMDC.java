package com.ipvsserv.nxd.mdc.iq;

import org.xmpp.packet.Presence;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.mdc.sub.SubscriptionsMDCI;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.xmpp.mdc.IpvsDomain;
import com.ipvsserv.xmpp.mdc.PresenceHandler;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DomainMDC implements IpvsDomain {
    private String serviceDomain = "";
    private SubscriptionsMDCI subs = null;
   // private DBCache dbCache = null;
    private boolean isSubscriptionsSaveEnabled;
    private boolean isAddUpdateAssert;
    private boolean isAddUpdateMerge;
    
    //private Hashtable<String, Integer> countToFetchList = new Hashtable<String, Integer>();
    
    //public DomainMDC(String serviceDomain, SubscriptionsMDCI subs, DBCache dbCache) {
    //    this.serviceDomain = serviceDomain;
    //    this.subs = subs;
     //   //this.dbCache = dbCache;
        
    //}
    public DomainMDC(String serviceDomain, SubscriptionsMDCI subs) {
        this.serviceDomain = serviceDomain;
        this.subs = subs;
        //this.dbCache = dbCache;
        
    }
    /* (non-Javadoc)
     * @see com.ipvsserv.nxd.mdc.iq.IpvsDomainI#getServiceDomain()
     */
    public String getServiceDomain() {
        return serviceDomain;
    }
    
    public void setServiceDomain(String serviceDomain) {
        this.serviceDomain = serviceDomain;
    }
    
    public SubscriptionsMDCI getSubs() {
        return subs;
    }
    
   /* public DBCache getDbCache() {
        return dbCache;
    }
    */
    public void loadSubscriptions() {
        RequestMDCParams params = new RequestMDCParams(serviceDomain);
        if (isSubscriptionsSaveEnabled) {
            subs.loadSubscriptions(params);
        }
    }
    
    public boolean unsubscribePresence(PresenceHandler presenceHandler, Presence presence, String jid, String domainName) {
        long enterTime = System.currentTimeMillis();
        long sendTimeDiff = 0;
        String ID = presence.getID();
        try {
            sendTimeDiff = System.currentTimeMillis() - enterTime;
            // String action="unsubscribe";
            //added code to subscribe presence
            if (presenceHandler.isUserInPresenceSubList(jid)) {
                presenceHandler.presenceSubscriptionNotRequired(jid);
            }
            subs.unsubscribe(null, jid, null, new RequestMDCParams(domainName));
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            if (MRLog.isPacketsummaryLogEnable()) MRLog.debug("ID: " + ID + " SENT:" + sendTimeDiff + " " + "END " + (System.currentTimeMillis() - enterTime) + " unsubscribe jid='" + jid + "'");
        }
        return true;
    }
    
    public void setSubscriptionsSaveEnabled(boolean enable) {
        isSubscriptionsSaveEnabled = enable;
    }
    
    public boolean isSubscriptionsSaveEnabled() {
        return isSubscriptionsSaveEnabled;
    }
    
    public void setAddUpdateMergeEnable(boolean enable) {
        isAddUpdateMerge = enable;
    }
    
    public boolean isAddUpdateMergeEnable() {
        return isAddUpdateMerge;
    }
    
    public void setAddUpdateAssert(boolean enable) {
        isAddUpdateAssert = enable;
    }
    
    public boolean isAddUpdateAssert() {
        return isAddUpdateAssert;
    }
    
    /*
     public void setCountToFetchProperties(Hashtable<String, Integer> countToFetchList) {
         this.countToFetchList = countToFetchList;
     }*/

    public int getCountToFetch_ByListName(String listName) {
        //todo:
        return 50000;
        /*if (countToFetchList.containsKey(listName)) {
            return countToFetchList.get(listName);
        }
        return countToFetchList.get("default");*/
    }
    
    /* (non-Javadoc)
     * @see com.ipvsserv.nxd.mdc.iq.IpvsDomainI#clear()
     */
    public void clear() {

    }
}
