package com.ipvsserv.nxd.mdc.sub;

import java.util.List;

import org.xmpp.packet.IQ;

import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.nxd.mdc.util.RequestMDCParams;
import com.ipvsserv.xmpp.mdc.ComponentEngine;
import com.ipvsserv.xmpp.mdc.DBApi;
import com.ipvsserv.xmpp.mdc.NotificationsRouter;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface SubscriptionsMDCI {
    
    // //////////// document /////////////////////
    public abstract boolean addDocumentSub(NIDSchemeMDC node, String jid, RequestMDCParams params) throws Exception;
    
    public abstract boolean deleteDocumentSub(NIDSchemeMDC node, RequestMDCParams params);
    
    // /////////////////////////////////// subscribe / unsubscribe
    public abstract boolean subscribe(NIDSchemeMDC node, String jid, String subscriptionId, String topic, RequestMDCParams params) throws Exception;
    
    public abstract boolean deleteSubscriptionsBasedOnSubscriptionId(String subscriptionId) throws Exception;
    	
    public abstract boolean unsubscribe(NIDSchemeMDC node, String jid, String subscriptionId, RequestMDCParams params);

    public abstract void loadSubscriptions(RequestMDCParams params);
    
    public abstract void saveSubscriptions(RequestMDCParams params);
    
    /////////////////////////////////////// subscription list
    public abstract String getSubscribers(NIDSchemeMDC node, RequestMDCParams params, String selectFormat);
    
    public abstract List<PubsubSubscription> getSubscriptions(NIDSchemeMDC node, RequestMDCParams params);
    
    // /////////////////////////////////// create bare minimum schema and
    // document list based on col node
    // /////////////////////////////////////
    public abstract boolean ensureSubscriptionDoc(String ID, RequestMDCParams params, String xmlSchema);
    
    // //////////////////// notifications //////////////////////
    public abstract List<PubsubSubscription> getDeleteSubscription(NIDSchemeMDC node, RequestMDCParams params);
    
    public abstract void sendDeleteDomainNotification(ComponentEngine component, NIDSchemeMDC node, List<PubsubSubscription> subscriptions, IQ iq, String nodeID, RequestMDCParams params, NotificationsRouter notificationsRouter) ;
    
    public abstract void sendCreateDomainNotification(ComponentEngine component, NIDSchemeMDC node, List<PubsubSubscription> subscriptions, IQ iq, String nodeID, RequestMDCParams params, NotificationsRouter notificationsRouter) ;
    
    public abstract void sendDeleteNotification(ComponentEngine component, NIDSchemeMDC node, List<PubsubSubscription> subscriptions, IQ iq, String nodeID, RequestMDCParams params, NotificationsRouter notificationsRouter) ;

    public abstract void sendAddNotification(ComponentEngine component, NIDSchemeMDC node, IQ iq, String nodeID, String xml, RequestMDCParams params, NotificationsRouter notificationsRouter) ;

    public abstract void sendUpdateNotification(ComponentEngine component, NIDSchemeMDC node, IQ iq, String nodeID, String xml, RequestMDCParams params, NotificationsRouter notificationsRouter) ;

    //////////////////////
    public abstract void setSaveSubscriptionsInterval(int interval);
    
    public abstract void setSaveSubscriptionsToDB(boolean enable);
    
    public abstract void enableSubscriptionsLog(boolean enable);
    
    public abstract void enableSubscriptions(boolean enable);
    
    ////////// add impl for xml db //////////
    public DBApi getDBApi();
    
    public void setDBApi(DBApi dbApi);
    ////////// add impl for xml db //////////
    
}
