package com.ipvsserv.nxd.mdc.pres;

import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ConcurrentHashMap;

import org.xmpp.packet.JID;
import org.xmpp.packet.Presence;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.xmpp.mdc.PresenceHandler;
import com.ipvsserv.xmpp.mdc.PresenceListener;
import com.ipvsserv.xmpp.session.XmppSessionImpl;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PresenceHandlerImpl implements PresenceHandler {
    private List<PresenceListener> presneceListners = Collections.synchronizedList(new ArrayList<PresenceListener>());
    private Map<String, Map<String, String>> barePresences = new ConcurrentHashMap<String, Map<String, String>>();
    private ConcurrentHashMap<String, String> watchSubList = new ConcurrentHashMap<String, String>();
    private boolean enableLog = false;
    private XmppSessionImpl xmppSession;
    
    public PresenceHandlerImpl(XmppSessionImpl xmppSession) {
        this.xmppSession = xmppSession;
    }
    
    public XmppSessionImpl getXmppSession() {
        return xmppSession;
    }
    
    public void setXmppSession(XmppSessionImpl router) {
        this.xmppSession = router;
    }
    
    public Map<String, Map<String, String>> getBarePresences() {
        return barePresences;
    }
    
    public void process(Presence presence) {
        try {
            if (enableLog) {
                MRLog.debug("PRESENSE: presence: " + presence.toXML());
            }
            if (presence.isAvailable()) {
                JID subscriber = presence.getFrom();
                Map<String, String> fullPresences = getBarePresences().get(subscriber.toBareJID());
                if (fullPresences == null) {
                    synchronized (subscriber.toBareJID().intern()) {
                        fullPresences = getBarePresences().get(subscriber.toBareJID());
                        if (fullPresences == null) {
                            fullPresences = new ConcurrentHashMap<String, String>();
                            getBarePresences().put(subscriber.toBareJID(), fullPresences);
                            if (enableLog) {
                                MRLog.debug("PRESENSE: availableEvent: " + subscriber.toString());
                            }
                        }
                    }
                }
                Presence.Show show = presence.getShow();
                fullPresences.put(subscriber.toString(), show == null ? "online" : show.name());
            } else if (presence.getType() == Presence.Type.unavailable) {
                JID subscriber = presence.getFrom();
                try {
                    Map<String, String> fullPresences = getBarePresences().get(subscriber.toBareJID());
                    if (fullPresences != null) {
                        fullPresences.remove(subscriber.toString());
                        if (enableLog) {
                            MRLog.debug("PRESENSE: unavailableEvent: " + subscriber.toString());
                        }
                        if (fullPresences.isEmpty()) {
                            getBarePresences().remove(subscriber.toBareJID());
                        }
                    }
                } catch (Exception ex) {}
                //for now we are only interrested in unavailable event
                for (Iterator<PresenceListener> iterator = presneceListners.iterator(); iterator.hasNext();) {
                    PresenceListener presneceListner = iterator.next();
                    presneceListner.unavailableEvent(presence, subscriber.toString());
                }
                
            } else   {
                if (enableLog) {
                    MRLog.debug("PRESENSE: Event: ? " + presence.toXML());
                }
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    public Collection<String> getShowPresences(JID subscriber) {
        try {
            Map<String, String> fullPresences = getBarePresences().get(subscriber.toBareJID());
            if (fullPresences == null) {
                // User is offline so return empty list
                return Collections.emptyList();
            }
            if (subscriber.getResource() == null) {
                // Subscriber used bared JID so return show value of all connected resources
                return fullPresences.values();
            } else {
                // Look for the show value using the full JID
                String show = fullPresences.get(subscriber.toString());
                if (show == null) {
                    // User at the specified resource is offline so return empty list
                    return Collections.emptyList();
                }
                // User is connected at specified resource so answer list with presence show value
                return Arrays.asList(show);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    public boolean isUserInPresenceSubList(String user) {
        try {
            synchronized (watchSubList) {
                if (watchSubList.containsKey(user)) {
                    return true;
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return false;
    }
    
    public boolean isUserPresenceAvailable(String user) {
        boolean available =false;
        try {
            if(IpvsServer.isComponentJID(user)){//if(InternalJIDManager.getInstance().isInternalJID(user)){
                available =true;
            } else {
                /*if (!isUserInPresenceSubList(user)) {
                    return false;
                }*/
                JID jid =getJID(user);
                if(jid!=null){
                    Map<String, String> fullPresences = getBarePresences().get(jid.toBareJID());
                    if (fullPresences != null && !fullPresences.isEmpty()) {
                        available= true;
                    }
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        if(!available ){
            if (enableLog) {
                MRLog.debug("PRESENSE: isUserPresenceAvailable : false ? " + user);
            }
        }
        return available;
    }
    
    public void presenceSubscriptionNotRequired(String user) {
        //keep subscribed
        //todo: add subscribe to all users in db.
        /*try {
            JID jid =getJID(user);
            if(jid==null){
                return;
            }
            Map<String, String> fullPresences = getBarePresences().get(jid.toBareJID());
            if (fullPresences != null) {
                if (!fullPresences.isEmpty()) {
                    // Unsubscribe from the user presence
                    Presence subscription = new Presence(Presence.Type.unsubscribe);
                    subscription.setTo(user);
                    subscription.setFrom(xmppSession.getAddress());
                    if (enableLog) {
                        MRLog.debug("PRESENSE: Sending presence unsubscription request " + subscription.toXML());
                    }
                    getXmppSession().sendPacket(subscription);
                }
            }
            getBarePresences().remove(jid.toBareJID());
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        try {
            
            //remove from list
            synchronized (watchSubList) {
                watchSubList.remove(user);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }*/
    }
    public JID getJID(String user){
        try{
            return new JID(user);
        }catch(Exception ex){
        }
        return null;
    }
    public void presenceSubscriptionRequired(String user) {
        try {
            if(IpvsServer.isComponentJID(user)){//if(InternalJIDManager.getInstance().isInternalJID(user)){
                //available =true;
            } else {
                //remove from list
                JID jid =getJID(user);
                if(jid==null){
                    return;
                }
                Map<String, String> fullPresences = getBarePresences().get(jid.toBareJID());
                if (fullPresences == null || fullPresences.isEmpty()) {
                    Presence subscription = new Presence(Presence.Type.subscribe);
                    subscription.setTo(user);
                    subscription.setFrom(xmppSession.getAddress());
                    if (enableLog) {
                        MRLog.debug("PRESENSE: Sending presence subscription request " + subscription.toXML());
                    }
                    getXmppSession().sendPacket(subscription);
                    
                    // Sending subscription requests based on received presences may generate
                    // that a sunscription request is sent to an offline user (since offline
                    // presences are not stored in the service's "barePresences"). However, this
                    // not optimal algorithm shouldn't bother the user since the user's server
                    // should reply when already subscribed to the user's presence instead of
                    // asking the user to accept the subscription request.
                }
            }
            
        } catch (Exception ex) {
            MRLog.error(user, ex);
        }
        try {
            //add to watch list
            synchronized (watchSubList) {
                watchSubList.putIfAbsent(user, System.currentTimeMillis() + "");
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    public void probePresences() {

    }
    
    public void addPresneceListner(PresenceListener presneceListner) {
        if (!presneceListners.contains(presneceListner)) {
            presneceListners.add(presneceListner);
        }
    }
    
    public void removePresneceListner(PresenceListener presneceListner) {
        if (presneceListners.contains(presneceListner)) {
            presneceListners.remove(presneceListner);
        }
    }
    
    @Override
    public void setEnableLog(boolean enable) {
        enableLog = enable;
    }
    
    @Override
    public void clear() {
        // TODO Auto-generated method stub
        //presneceListners.clear(); 
        barePresences.clear();
        //watchSubList.clear(); 
    }
    
}
