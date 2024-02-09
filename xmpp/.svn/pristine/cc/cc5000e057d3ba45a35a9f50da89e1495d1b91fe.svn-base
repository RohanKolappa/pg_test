package com.ipvsserv.xmpp.mdc;

import java.util.concurrent.ThreadPoolExecutor;

import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.Packet;
import org.xmpp.packet.Presence;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.xmpp.session.XmppSession;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface ComponentEngine {
    public void shutdown();
    
    public void start();
    
    //////////////XmppSession //////////////
    public XmppSession getXmppSession();
    
    public void setXmppSession(XmppSession xmppSession);
    
    ////////////// notification //////////////
    public NotificationsRouter getNotificationsRouter();
    
    public void setNotificationsRouter(NotificationsRouter notificationsRouter);
    
    ////////////// PresenceHandlerI //////////////  
    public PresenceHandler getPresenceHandler();
    
    public void setPresenceHandler(PresenceHandler presenceHandler);
    
    ////////////// PresenceHandlerI //////////////  
    public DomainManager getDomainManager();
    
    public void setDomainManager(DomainManager domainManager);
    
    ////////////// ThreadPoolExecutor //////////////  
    public ThreadPoolExecutor getThreadPoolExecutor();
    
    public void setThreadPoolExecutor(ThreadPoolExecutor executor);
    
    //////////////process Packet //////////////
    public void processPacket(Packet packet);
    
    public void process(Message message);
    
    public void process(Presence presence);
    
    public boolean process(IQ iq);
    
    ////////// DBApi //////////
    public DBApi getDBApi();
    
    public void setDBApi(DBApi dbApi);
    
    ////////// IpvsModuleName //////////
    public void setParentIpvsModuleName(String parentIpvsModuleName);
    
    public String getParentIpvsModuleName();
    
    public void save() throws Exception;
    
    public void dbInititated() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException;
    
    public int getMaxRequestLimit();
}
