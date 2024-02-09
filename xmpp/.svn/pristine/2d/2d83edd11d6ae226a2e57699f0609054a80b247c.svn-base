package com.ipvsserv.nxd.mdc.comp;

import java.util.concurrent.ThreadPoolExecutor;

import org.xmpp.packet.IQ;
import org.xmpp.packet.Message;
import org.xmpp.packet.Packet;
import org.xmpp.packet.Presence;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.mdc.db.DBApiFactoryMDC;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.xmpp.mdc.ComponentEngine;
import com.ipvsserv.xmpp.mdc.DBApi;
import com.ipvsserv.xmpp.mdc.DomainManager;
import com.ipvsserv.xmpp.mdc.IQHandler;
import com.ipvsserv.xmpp.mdc.NotificationsRouter;
import com.ipvsserv.xmpp.mdc.PresenceHandler;
import com.ipvsserv.xmpp.mdc.PresenceListener;
import com.ipvsserv.xmpp.session.XmppSession;
import com.ipvsserv.xmpp.session.XmppSessionEventListener;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public abstract class PubSubEngineBase implements XmppSessionEventListener, ComponentEngine, PresenceListener, IQHandler {
    protected NotificationsRouter notificationsRouter = null;
    protected PresenceHandler presenceHandler = null;
    protected ThreadPoolExecutor threadPool;
    protected DomainManager domainManager = null;
    protected String parentIpvsModuleName;
    protected boolean isProcessInThreadDisable=false;
    protected boolean isDBInititated = false;
    protected DBApi dbApi = null;
    protected String ID;
    protected boolean bConnectionLost = false;
    private IpvsFileStatus openfireConnectionStatus=null;
    protected int maxRequestLimit = 0;
    public PubSubEngineBase(String ID, String componentId) {
        this.ID = ID;
        this.parentIpvsModuleName = componentId;
        openfireConnectionStatus = new IpvsFileStatus(IpvsFileStatus.STATUS_OPENFIRE_FAILED_FILE);
        openfireConnectionStatus.init(true);
        isProcessInThreadDisable = IpvsServGlobals.getBooleanProperty(componentId+".thread.disable", false);
        maxRequestLimit = IpvsServGlobals.getIntProperty(componentId+".maxRequestLimit", 60);
    }
    
    ///////////////new ///////////////////////
    protected XmppSession xmppSession = null;
    
    @Override
    public XmppSession getXmppSession() {
        return xmppSession;
    }
    
    @Override
    public void setXmppSession(XmppSession xmppSession) {
        this.xmppSession = xmppSession;
    }
    
    ///////////////new ///////////////////////
    @Override
    public void shutdown() {
        try {
            //connectionLostToServer = true;
            //MRLog.debug("#################################PubSubEngineSednaMultiCompImpl shutdown called");
            shutdownEngine();
            openfireConnectionStatus.removeStatusFile();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void start() {
        try {
            //MRLog.debug("#################################PubSubEngineSednaMultiCompImpl start called");
            startEngine();
            //connectionLostToServer = false;
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    ///////////////////////// ComponentEngineI //////////////////
    @Override
    public PresenceHandler getPresenceHandler() {
        return presenceHandler;
    }
    
    @Override
    public void setPresenceHandler(PresenceHandler presenceHandler) {
        this.presenceHandler = presenceHandler;
    }
    
    @Override
    public NotificationsRouter getNotificationsRouter() {
        return notificationsRouter;
    }
    
    @Override
    public void setNotificationsRouter(NotificationsRouter notificationsRouter) {
        this.notificationsRouter = notificationsRouter;
    }
    
    @Override
    public DomainManager getDomainManager() {
        return domainManager;
    }
    
    @Override
    public ThreadPoolExecutor getThreadPoolExecutor() {
        return threadPool;
    }
    
    @Override
    public void setDomainManager(DomainManager domainManager) {
        this.domainManager = domainManager;
    }
    
    @Override
    public void setThreadPoolExecutor(ThreadPoolExecutor executor) {
        threadPool = executor;
        
    }
    
    protected abstract void shutdownEngine() throws Exception;
    
    protected abstract void startEngine() throws Exception;
    
    //RecvPacketEventListenerI
    
    @Override
    public DBApi getDBApi() {
        if (dbApi == null) {
            try {
                dbApi = DBApiFactoryMDC.getDBApi(getParentIpvsModuleName());
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        return dbApi;
    }
    
    @Override
    public void setDBApi(DBApi dbApi) {
    //TODO: do something here
    }
    
    @Override
    public void setParentIpvsModuleName(String parentIpvsModuleName) {
        this.parentIpvsModuleName = parentIpvsModuleName;
    }
    
    @Override
    public String getParentIpvsModuleName() {
        return parentIpvsModuleName;
    }
    
    @Override
    public void run() {
    //todo: find out
    //for now do nothing
    
    }
    
    @Override
    public void save() throws Exception {}
    
    ///////////////////////// component //////////////////
    @Override
    public void processPacket(Packet packet) {
        try {
            processPacketLocal(packet);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    @Override
    public void process(Presence presence) {
        presenceHandler.process(presence);
    }
    
    @Override
    public void process(Message message) {
    //donothing.
    }
    
    ///////////////////////// component //////////////////
    
    private void processPacketLocal(Packet packet) {
        try {
            if (packet instanceof IQ) {
                try {
                    process((IQ) packet);
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            } else if (packet instanceof Presence) {
                process((Presence) packet);
            } else {
                process((Message) packet);
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    ///////////////////////// XmppSessionEventListenerI //////////////////
    protected boolean isConnectionLost() {
        synchronized (this) {
            return bConnectionLost;
        }
    }
    
    private void setConnectionLost(boolean connectionLost) {
        synchronized (this) {
            //MRLog.debug("setConnectionLost " + connectionLost);
            if(connectionLost){
                openfireConnectionStatus.createStatusFile();
                this.bConnectionLost = true;
            }else{
                openfireConnectionStatus.removeStatusFile();
                this.bConnectionLost = false;
            }
        }
    }
    
    //protected Collection<PacketCollector> collectors = new ConcurrentLinkedQueue<PacketCollector>();
    /*
    @Override
    public PacketCollector createPacketCollector(PacketFilter packetFilter) {
        PacketCollector collector = new PacketCollector(this, packetFilter);
        collectors.add(collector);
        // Add the collector to the list of active collector.
        return collector;
    }
    
    @Override
    public void cancelPacketCollector(PacketCollector packetCollector) {
        collectors.remove(packetCollector);
    }
    */
    @Override
    public void processPacket(Packet packet, XmppSession comp, int ID) {
        processPacketLocal(packet);
        
        /*// todo: test 
        for (Iterator<PacketCollector> iterator = collectors.iterator(); iterator.hasNext();) {
            PacketCollector packetCollector = iterator.next();
            packetCollector.processPacket(packet);
        }*/
    }
    
    @Override
    public void connectionLost(XmppSession comp, int ID) {
        setConnectionLost(true);
    }
    
    @Override
    public void loginComplete(XmppSession comp) {
        try {
            synchronized (this) {
                setConnectionLost(false);
                notify();
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }
    
    @Override
    public void loginComplete(XmppSession comp, int ID) {}
    
    @Override
    public void logoutComplete(XmppSession comp) {}
    
    @Override
    public void logoutComplete(XmppSession comp, int ID) {
        setConnectionLost(true);
    }
    
    @Override
    public int getMaxRequestLimit() {
        return maxRequestLimit;
    }    
}
