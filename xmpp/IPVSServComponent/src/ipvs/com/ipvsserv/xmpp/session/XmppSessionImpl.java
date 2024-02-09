package com.ipvsserv.xmpp.session;

import java.util.ArrayList;
import java.util.Collection;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.concurrent.ConcurrentLinkedQueue;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import org.jivesoftware.whack.ExternalComponentManager;
import org.xmpp.component.Component;
import org.xmpp.component.ComponentException;
import org.xmpp.component.ComponentManager;
import org.xmpp.packet.JID;
import org.xmpp.packet.Packet;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class XmppSessionImpl extends XmppSession {
    ///////////////////// ScheduledExecutorService //////////
    private long initialDelay = 0;
    private long delay = 5;
    private TimeUnit unit = TimeUnit.SECONDS;
    private ScheduledExecutorService executor = null;
    private boolean executed = false;
    private Runnable stateMonitor = new StateMonitor();
    
    /////////////// xmpp component /////////////////    
    private String serviceID = null;
    
    private String serviceDescription = null;
    private String secretKey = null;
    private String host = "localhost";
    private int port = 5275;
    private int numberOfConnections;
    
    //////////////// start /stop //////////////
    private int state = 0;
    private boolean started = false;
    private boolean shutdownInProgress;
    private boolean verifingState = false;
    private boolean connectionLostToServer = true;
    private int componentsConnected = 0;
    
    //routers
    private int currentPos = 0;
    private ExternalComponentManager compManager = null;
    private ComponentManager componentManager = null;
    private JID jid;
    
    //routers sessionListeners collectors
    private Hashtable<String, ComponentImpl> routers = new Hashtable<String, ComponentImpl>();
    private Collection<XmppSessionEventListener> sessionListeners = new ConcurrentLinkedQueue<XmppSessionEventListener>();
    private InternalJIDManager manager = InternalJIDManager.getInstance();
    
    ///////////////////// ScheduledExecutorService //////////
    public XmppSessionImpl(String serviceName, String extCompDescription, String secretKey, String host, int port, int countInComp) {
        this.numberOfConnections = countInComp;
        this.serviceID = serviceName;
        this.serviceDescription = extCompDescription;
        this.secretKey = secretKey;
        this.host = host;
        this.port = port;
    }
    
    @Override
    public boolean isStarted() {
        return started;
    }
    
    @Override
    public boolean isConnectionLostToServer() {
        return connectionLostToServer;
    }
    
    @Override
    public boolean isShutdownInProgress() {
        return shutdownInProgress;
    }
    
    //////////////////XmppSessionEventListenerI
    @Override
    public void addXmppSessionEventListener(XmppSessionEventListener xmppSessionEventListener) {
        this.sessionListeners.add(xmppSessionEventListener);
    }
    
    @Override
    public void removeXmppSessionEventListener(XmppSessionEventListener xmppSessionEventListener) {
        sessionListeners.remove(xmppSessionEventListener);
    }
    
    //////////////////PacketCollector
    
    ////////// login parameters ///////////
    @Override
    public String getServiceID() {
        return serviceID;
    }
    
    @Override
    public String getServiceDescription() {
        return serviceDescription;
    }
    
    @Override
    public String getSecretKey() {
        return secretKey;
    }
    
    @Override
    public String getHost() {
        return host;
    }
    
    @Override
    public int getPort() {
        return port;
    }
    
    @Override
    public int getNumberOfConnections() {
        return numberOfConnections;
    }
    
    @Override
    public String getServiceDomain() {
        return serviceID + "." + host;
    }
    
    @Override
    public JID getAddress() {
        if (jid == null) {
            jid = new JID(null, getServiceDomain(), null);
        }
        return jid;
    }
    
    ////////// login parameters ///////////
    
    @Override
    public void logout() throws Exception {
        shutdownInProgress = true;
        stopMonitor();
        stateMonitor = null;
        started = false;
        connectionLostToServer = true;
        destroyComponents();
        try {
            sessionListeners.clear();
        } catch (Exception ex) {}
        try {
            sessionListeners = new ArrayList<XmppSessionEventListener>();
        } catch (Exception ex) {}
    }
    
    @Override
    public void login() throws Exception {
        compManager = (ExternalComponentManager) IpvsServer.getInstance().getManager();
        if(compManager ==null){
            compManager = new ExternalComponentManager(host, port);
            //compManager = manager;
            IpvsServer.getInstance().setManager(compManager);
            compManager.setSecretKey(serviceID, secretKey);
            compManager.setMultipleAllowed(serviceID, true);
        }
        //start the thread to connect to xmpp 
        try {
            startMonitor();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void sendPacket(Packet packet) throws Exception {
        routelocal(packet);
    }
    @Override
    public void sendPacket(String to, String packet) throws Exception {
         routelocal(to, packet);
    }
    
    ////////////////////
    private void routelocal(Packet packet) throws Exception {
        if (MRLog.isPacketdetailsLogEnable()) {
            MRLog.debug("*** route " + packet.toXML());
        }
        //if internal JID -- just hand over the packet
        InternalJID internalJID = manager.getInternalJID(packet.getTo());
        if (internalJID != null) {
            internalJID.processPacket(packet);
            if (MRLog.isInterComponentLogEnable()) {
                MRLog.debug("routelocal  send internalJID '" + packet.getTo() +"' .");
            }
            return;
        }else{
            //System.out.println("NOT FOUND send XmppSession '" + packet.getTo() +"'.");
        }
        //if not internal JID -- route the packet to openfire
        ComponentImpl component = getNextComponent();
        if (component == null) {
            if (MRLog.isPacketdetailsLogEnable()) {
                MRLog.debug("*** route router " + currentPos + " " + packet + " " + routers.size());
            }
        }
        componentManager.sendPacket(component, packet);
    }
   
    private void routelocal(String to, String packet) throws Exception {
        if (MRLog.isPacketdetailsLogEnable()) {
            MRLog.debug("*** route string " + packet);
        }
        //if internal JID -- just hand over the packet
        InternalJID internalJID = manager.getInternalJID(to);
        if (internalJID != null) {
            internalJID.processPacket(to, packet);
            if (MRLog.isInterComponentLogEnable()) {
                MRLog.debug("routelocal  send internalJID '" + to +"' .");
            }
            return;
        }else{
            //System.out.println("NOT FOUND send XmppSession '" + packet.getTo() +"'.");
        }
        //if not internal JID -- route the packet to openfire
        ComponentImpl component = getNextComponent();
        if (component == null) {
            if (MRLog.isPacketdetailsLogEnable()) {
                MRLog.debug("*** route router " + currentPos + " " + packet + " " + routers.size());
            }
        }
        componentManager.sendPacket(component, to, packet);
    }
    //    public IQ query(IQ packet, long timeout) throws Exception {
    ////        if (MRLog.isPacketdetailsLogEnable()) MRLog.debug("*** route " + packet.toXML());
    ////        ComponentImpl component = getNextComponent();
    ////        if (component == null) {
    ////            if (MRLog.isPacketdetailsLogEnable()) MRLog.debug("*** route router " + currentPos + " " + packet + " " + routers.size());
    ////        }
    ////        return componentManager.query(component, packet, timeout);
    //        return null;
    //    }
    //    
    //    public void query(IQ packet, PacketListener listener) throws Exception {
    ////        if (MRLog.isPacketdetailsLogEnable()) MRLog.debug("*** route " + packet.toXML());
    ////        ComponentImpl component = getNextComponent();
    ////        if (component == null) {
    ////            if (MRLog.isPacketdetailsLogEnable()) MRLog.debug("*** route router " + currentPos + " " + packet + " " + routers.size());
    ////        }
    ////        componentManager.query(component, packet, listener);
    //    }
    long to=System.currentTimeMillis();
    private void startComponents() {
        try {
            if (started) {
                return;
            }
            while (routers.size() < 20) {
                ComponentImpl recCopm = new ComponentImpl(routers.size());
                compManager.addComponent(serviceID + "." + routers.size(), recCopm);
                //add to routers
                //routers.put(routers.size() + "", recCopm);
                componentsConnected = routers.size();
            }
            MRLog.debug("------------ connect done. added IPVSComponent = " + numberOfConnections + " " + componentsConnected);
            if (routers.size() == numberOfConnections) {
                started = true;
                connectionLostToServer = false;
            }
        } catch (ComponentException ex) {
            MRLog.error((System.currentTimeMillis()-to) + " XmppSession startComponent '" + serviceID +"' " + ex.getMessage());
            to=System.currentTimeMillis();
        } catch (Exception ex) {
            MRLog.error("XmppSession startComponent '" + serviceID +"' " + ex.getMessage());
        }
    }
    
    private void destroyComponents() throws Exception {
        try {
            int iTotalComp = routers.size();
            for (int i = iTotalComp; i > -1; i--) {
                try {
                    Component comp = routers.remove(i + "");
                    if (comp != null) {
                        MRLog.debug("shutdown comp " + serviceID + "." + i);
                        compManager.removeComponent(serviceID + "." + i);
                        comp.shutdown();
                    }
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        try{
            manager.removeInternalJID(this);
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        connectionLostToServer = true;
        // hack to allow safe stopping
        MRLog.info("XmppSession stopped " + serviceID);
    }
    
    //////////////////// 
    public void processPacket(Packet packet) {
        processPacketComponent(-1, packet);
    }
    
    private void processPacketComponent(int ID, Packet packet) {
        try {
            for (Iterator<XmppSessionEventListener> iterator = sessionListeners.iterator(); iterator.hasNext();) {
                XmppSessionEventListener sessionListener = iterator.next();
                sessionListener.processPacket(packet, this, ID);
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void initializingComponent(int ID, JID jid, ComponentManager componentManager) throws ComponentException {
        this.jid = jid;
        this.componentManager = componentManager;
    }
    
    private void startComponent(int ID, ComponentImpl recCopm) {
        try {
            MRLog.debug("startComponent " + serviceID + " : " + ID);
            routers.put(ID + "", recCopm);
            manager.addInternalJID(this);
            //if (routers.size() == numberOfConnections) {
            //    manager.addInternalJID(this);
            //}
            
            connectionLostToServer = false;
            for (Iterator<XmppSessionEventListener> iterator = sessionListeners.iterator(); iterator.hasNext();) {
                XmppSessionEventListener sessionListener = iterator.next();
                sessionListener.loginComplete(this, ID);
                if (routers.size() == numberOfConnections) {
                    sessionListener.loginComplete(this);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void shutdownComponent(int ID, ComponentImpl recCopm ) {
        try {
            MRLog.debug("shutdownComponent " + serviceID + " : " + ID);
            routers.remove(ID + "");
            connectionLostToServer = true;
            if (routers.size() == 0) {
                manager.removeInternalJID(this);
            }
            for (Iterator<XmppSessionEventListener> iterator = sessionListeners.iterator(); iterator.hasNext();) {
                XmppSessionEventListener sessionListener = iterator.next();
                sessionListener.logoutComplete(this, ID);
                sessionListener.connectionLost(this, ID);
                if (routers.size() == 0) {
                    sessionListener.logoutComplete(this);
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    // from    PacketRouterMCImpl //////////
    private synchronized ComponentImpl getNextComponent() {
        if (routers.size() == 0) {
            return null;
        }
        currentPos++;
        if (currentPos < 0 || currentPos >= routers.size()) {
            currentPos = 0;
        }
        if(routers.get(currentPos + "")==null){
        	//todo: bug is there randomly some componenta are null
        	MRLog.error("components is NULL?? " + currentPos );
            for (ComponentImpl component : routers.values()) {
                if(component !=null){
                    currentPos = component.getCompID();
                    return component;
                }
            }
        }
        return routers.get(currentPos + "");
    }
    
    //////////////// ComponentImpl ///////////////////
    class ComponentImpl implements Component {
        private int ID;
        
        public int getCompID() {
            return ID;
        }
 
        public ComponentImpl(int ID) {
            this.ID = ID;
        }
        
        @Override
        public String getDescription() {
            return serviceDescription;
        }
        
        @Override
        public String getName() {
            return serviceID;
        }
        
        @Override
        public void initialize(JID jid, ComponentManager componentManager) throws ComponentException {
            initializingComponent(ID, jid, componentManager);
        }
        
        @Override
        public void shutdown() {
            shutdownComponent(ID, this);
        }
        
        @Override
        public void start() {
            startComponent(ID, this);
        }
        
        @Override
        public void processPacket(Packet packet) {
            //System.out.println("processPacket '" + packet.toXML() +"' .");
            processPacketComponent(ID, packet);
        }
    }
    
    /////////////////////////////////////// ensure the state = HEALTHY completes ///////////////////////////
    
    public int getLastKnownState() throws Exception {
        if (started) {
            state = IpvsServBasicModule.HEALTHY;
        } else {
            state = IpvsServBasicModule.UNHEALTHY;
        }
        //the other thing that openfire is diconnected not known here
        return state;
    }
    
    private void verifyState() throws Exception {
        if (getLastKnownState() == IpvsServBasicModule.UNHEALTHY) {
            if (verifingState == true) {
                return;
            }
            synchronized (this) {
                if (verifingState == true) {
                    return;
                }
                verifingState = true;
            }
            try {
                startComponents();
            } catch (Exception ex) {
                throw ex;
            } finally {
                verifingState = false;
            }
        }
    }
    
    private void startMonitor() {
        startMonitor(stateMonitor, initialDelay, delay, unit);
    }
    
    private void startMonitor(Runnable command, long initialDelay, long delay, TimeUnit unit) {
        this.stateMonitor = command;
        this.initialDelay = initialDelay;
        this.delay = delay;
        this.unit = unit;
        executor = new ScheduledThreadPoolExecutor(1);
        
        // See if we're in development mode. If so, check for new plugins once every 5 seconds.
        // Otherwise, default to every 20 seconds.
        if (Boolean.getBoolean("developmentMode")) {
            executor.scheduleWithFixedDelay(stateMonitor, initialDelay, delay, TimeUnit.SECONDS);
        } else {
            executor.scheduleWithFixedDelay(stateMonitor, initialDelay, delay, TimeUnit.SECONDS);
        }
    }
    
    private void stopMonitor() {
        if (executor != null) {
            executor.shutdown();
        }
        executor = null;
    }
    
    private class StateMonitor implements Runnable {
        boolean running = false;
        boolean firstRun = true;
        
        public boolean isExecuted() {
            return executed;
        }
        
        @Override
        public void run() {
            // If the task is already running, return.
            synchronized (this) {
                if (running) {
                    return;
                }
                running = true;
            }
            try {
                running = true;
                if (!shutdownInProgress) {
                    try {
                        verifyState();
                    } catch (Exception ex) {
                        MRLog.debug(ex.getMessage());
                    }
                    // Set that at least one iteration was done. That means that "all available" plugins
                    // have been loaded by now.
                    if (!IpvsServer.getInstance().isSetupMode()) {
                        executed = true;
                    }
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            // Finished running task.
            synchronized (this) {
                running = false;
            }
            // Process finished, so set firstRun to false (setting it multiple times doesn't hurt).
            firstRun = false;
        }
    }
    //////////////////// new ///////////////////

    @Override
    public void processPacket(String to, String packet) throws Exception {
        processPacket(IPVSPacketParser.getInstance().parse(packet));
    }
}
