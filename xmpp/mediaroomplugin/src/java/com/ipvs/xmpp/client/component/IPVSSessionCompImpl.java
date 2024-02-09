package com.ipvs.xmpp.client.component;

import java.util.Hashtable;
import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import org.dom4j.Element;
import org.jivesoftware.smack.PacketCollector;
import org.jivesoftware.smack.PacketListener;
import org.jivesoftware.smack.filter.PacketFilter;
import org.jivesoftware.smack.filter.ToContainsFilter;
import org.jivesoftware.smack.packet.Packet;
import org.xmpp.packet.JID;
import org.xmpp.packet.Presence;

import com.ipvs.common.MRLog;
import com.ipvs.common.XMPPI;
import com.ipvs.common.db.MRXMLDBUtils;
import com.ipvs.config.IPVSConst;
import com.ipvs.mediaroomservice.impl.AppServerAgent;
import com.ipvs.utils.Utils;
import com.ipvs.xmpp.client.params.DBParams;
import com.ipvs.xmpp.client.smack.IPVSServiceDiscoveryManager;
import com.ipvs.xmpp.client.smack.IPVSSession;
import com.ipvs.xmpp.client.smack.PresenseHandler;
import com.ipvs.xmpp.client.smack.ft.FileTransferManager;
import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.threadmodel.ChangeThreadName;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.xmpp.session.InternalJID;
import com.ipvsserv.xmpp.session.InternalJIDManager;
import com.ipvsserv.xmpp.session.XmppSession;
import com.ipvsserv.xmpp.session.XmppSessionEventListener;
import com.ipvsserv.xmpp.session.XmppSessionImpl;

public class IPVSSessionCompImpl extends IPVSSession implements XmppSessionEventListener, InternalJID, Runnable {
    private XmppSession connection;
    private PacketCollector collector;
    private PacketReaderImpl packetReader = null;

    ////////////////////////// IPVSSession component login///////////////////////////////
    private String ipvsModuleName = "";//"test";
    private String secretKey = "";//"test";
    private String host = "localhost";
    private int countInComp = 20;
    private String serviceID = "";
    private String serviceDesc = "";
    private int liveComp = 0;
    ////////////////////////// XmppSessionEventListener ///////////////////////////////
    private boolean isStarted = false;
    private boolean isStandAlone = false;
    ////////////////////////// InternalJID ///////////////////////////////
    private InternalJIDManager manager = InternalJIDManager.getInstance();
    private JID jid = null;
    private ThreadPoolExecutor threadPool = null;

    ////////////////////////// login /logout ///////////////////////////////
    public String getIpvsModuleName() {
        return ipvsModuleName;
    }
  
    public void setIpvsModuleName(String ipvsModuleName) {
        this.ipvsModuleName = ipvsModuleName;
    }
    
    public IPVSSessionCompImpl(){
        super();
        initialize();
    }
    
    private void initialize(){
        setIpvsModuleName(IPVSConst.getInstance().getMediaroomPluginName());
        secretKey = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.secret", "").trim();
        host = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.xmppServer", "").trim();
        port = Utils.getIntValue(IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.xmppServerPort", "-1").trim(), -1);

        serviceID = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.component.serviceId", "").trim();
        serviceDesc = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.component.serviceDesc", "").trim();
        countInComp = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.component.registerComponents", 20);

        connection = new XmppSessionImpl(serviceID, serviceDesc, secretKey, host, port, countInComp);
    }
    ////////////////////////// login /logout ///////////////////////////////
    @Override
    public void login(String serverip, int pport, String userJID, String password, String resource) throws Exception {
        login();
    }
    public void login() throws Exception {
        boolean runningWithDBComponent = IpvsServGlobals.getBooleanProperty(getIpvsModuleName() + ".runningWithDBComponent", false);
        if(!isStandAlone){
            while ( !IpvsServer.getInstance().isStarted()) {
                try {
                    Thread.sleep(100);
                } catch (Exception e) {
                }
            }
            if(runningWithDBComponent) {
                //wait for db to start before login 
                while ( !IpvsServer.getInstance().isDBReady()) {
                    try {
                        Thread.sleep(100);
                    } catch (Exception e) {
                    }
                }
            }
        }        
        //long to = System.currentTimeMillis();
        packetReader = new PacketReaderImpl(connection);
        initialize();
        connection.addXmppSessionEventListener(this);
        connection.login();

        try {
            int processInThreadCorePoolSize = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.thread.corePoolSize", 40);
            int processInThreadMaximumPoolSize = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.thread.maxPoolSize", 100);
            int processInThreadKeepAliveTime = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".mscomponent.thread.keepAlivetimeSec", 60);

            // Create a pool of threads that will process requests received by this component. If more
            // threads are required then the command will be executed on the SocketReadThread process
            threadPool = new ThreadPoolExecutor(processInThreadCorePoolSize, processInThreadMaximumPoolSize, processInThreadKeepAliveTime, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(), new ThreadPoolExecutor.CallerRunsPolicy());

            final ThreadFactory originalThreadFactory = threadPool.getThreadFactory();
            ThreadFactory newThreadFactory = new ThreadFactory() {
                private final AtomicInteger threadId = new AtomicInteger(0);

                public Thread newThread(Runnable runnable) {
                    Thread t = originalThreadFactory.newThread(new ChangeThreadName(runnable, "MRPlgn_" + threadId.incrementAndGet()));
                    t.setDaemon(true);
                    return t;
                }
            };
            threadPool.setThreadFactory(newThreadFactory);

        } catch (Exception ex) {
            ex.printStackTrace();
        }

        this.server = host;
        this.user = serviceID;

        this.password = secretKey;
        this.resource = "";
        createPendingList();
        
        //wait until login complete.
        synchronized (this) {
            while (!bConnectionSuccess) {
                try {
                    wait(5 * 1000);
                } catch (Exception e) {
                }
            }
        }        
        
        //servername = host;
        myjid = connection.getAddress().toString();
        servername = myjid.substring(myjid.indexOf(".") + 1);
        pubsubjid = IPVSConst.getInstance().getIpvsDbComponentJIDPrefix() + "." + servername;
        serviceName = servername;
        AppServerAgent.setTestAppServerAgentJID(myjid);

        jid = new JID(getXMPPUserJid());
        manager.addInternalJID(this);
        iPVSServiceDiscoveryManager = new IPVSServiceDiscoveryManager(this);
        PacketFilter filter = new ToContainsFilter(myjid);
        collector = packetReader.createPacketCollector(filter);
        presenseHandler = new PresenseHandler();
        presenseHandler.init(this);
        fxfer = new FileTransferManager(this);
        start_thread();
        loginComplete();
        waitForDBToReady();
        System.out.println("login done");
    }
    
    private void waitForDBToReady() {
        String query = "doc('service.version')/Service";
        String response = null;
        Map<String, String> nidList = new Hashtable<String, String>();
        MRLog.getInstance(MRLog.CONSOLE).addInfo("START : waitForDBToReady ");
        //File file = new File(IpvsFileStatus.MS_STATUS_INITIALIZING_FILE);
        boolean isReady = false;
        while (true) {
            isReady = false;
            response = null;
            try {
                MRLog.getInstance(MRLog.CONSOLE).addInfo("LOOP START: waitForDBToReady  ");
                DBParams dbParams = new DBParams();
                dbParams.setAction(DBParams.READ_QUERY);
                dbParams.setDBCallTimeOut(MRXMLDBUtils.TIME_OUT);
                dbParams.setServiceDomain("default");
                dbParams.setSubscribeJID(getXMPPUserJid());
                dbParams.setNodeID("");

                //new 
                dbParams.setQuery(query);
                dbParams.setQueryXPathList(nidList);
                dbParams.setQueryNIDList(nidList);
                Element responseEL = ipvsdb_getData(dbParams);
                if (responseEL != null ) {
                    isReady = true;
                    break;
                }
                MRLog.getInstance(MRLog.CONSOLE).addInfo("LOOP NEXT: waitForDBToReady : DB not ready yet. response='" + response + "'");
            } catch (Exception ex) {
                isReady = false;
                String error = "";
                if(ex != null){
                    error = ex.getMessage();
                    //ex.printStackTrace();
                }
                MRLog.getInstance(MRLog.CONSOLE).addInfo("LOOP NEXT: waitForDBToReady : DB not ready yet. Error response='" + response + "' " + error);
            }
            if (!isReady) {
                //todo: not to get stuck forever...
                //if (!file.exists()) {
                //    MRLog.getInstance(MRLog.CONSOLE).addInfo("FORCE END: waitForDBToReady : Interrupted ? status file does not exist='" + file.getAbsolutePath() + "'");
                //    break;
                //}
                try {
                    Thread.sleep(1000);
                } catch (Exception e) {
                }
            }
        }
        MRLog.println("DONE : "+isReady+" waitForDBToReady  response='" + response + "'") ;
        MRLog.getInstance(MRLog.CONSOLE).addInfo("DONE : "+isReady+" waitForDBToReady  response='" + response + "'", MRLog.INFO, MRLog.NOTIFY);
    }
    
    private void start_thread() {
        if (thread == null) {
            thread = new Thread(this, "IPVSPacketCollector");
            thread.start();
        }
    }

    @Override
    public void logout() throws Exception {
        try {
            connection.logout();
            //connection.removeConnectionCreationListener(this);
            connection = null;
        } catch (Exception e) {
            System.out.println("Ignoring the Security Exception " + e.getMessage());
        }
        try {
        	fxfer.cleanup(this);
        } catch (Exception e) {
        }        
        fxfer=null;        
        try {
            threadPool.shutdown();
            threadPool = null;
        } catch (Exception e) {
            System.out.println("Ignoring the threadPool shutdown Exception " + e.getMessage());
        }
       
    }

    @Override
    public void loginComplete() {
        isStarted = true;
    }

    @Override
    public void logoutComplete() {
        jid = null;
        manager.removeInternalJID(this);
        isStarted = false;

    }

    public void sendPacket(Packet packet) {
        /*if (liveComp <=0) {
            MRLog.println("Login not complete ....");
        }*/        
        try {
            if (packet.getFrom() == null || packet.getFrom().equals("")) {
                packet.setFrom(getXMPPUserJid());
            }
        } catch (Exception e) {
            e.printStackTrace();
        }
        //System.out.println("OUT packet " + packet.getPacketID());
        if (packet.getTo() == null) {
            try {
                connection.sendPacket("", packet.toXML());
            }catch(Exception ex){
                ex.printStackTrace();
            }
            return;
        }
        //if internal JID -- just hand over the packet
        InternalJID internalJID = manager.getInternalJID(packet.getTo());
        if (internalJID != null) {
            try {
                internalJID.processPacket(packet.getTo(), packet.toXML());
            } catch (Exception e) {
                e.printStackTrace();
            }
            //System.out.println("FOUND internalJID send  '" + packet.getTo() + "' .");
            return;
        } else {
            // System.out.println("NOT FOUND internalJID send Xmpp  '" + packet.getTo() + "'.");
        }
        try {
            connection.sendPacket(packet.getTo(), packet.toXML());
        }catch(Exception ex){
            System.out.println("send failed Xmpp  '" + packet.getTo() + "'." +  packet.toXML());
            ex.printStackTrace();
        }
    }

    @Override
    public void start() {
        isStarted=true;
    }

    @Override
    public void stop() {
        isStarted=false;
        try {
            thread.interrupt();
        } catch (Exception ex) {
            try {
                if (thread != null) {
                    MRLog.println("IPVSSession thread: Error on interuption. isInterrupted? " + thread.isInterrupted() + " isAlive? " + thread.isAlive() + " bstopThread " + bstopThread);
                }
            } catch (Exception ex2) {
            }
        }
    }

    @Override
    public void run() {
        while (!bstopThread) {
            if (!processMessages()) {
                try {
                    Thread.sleep(1);
                } catch (InterruptedException e) {
                    break;
                }
            }
        }
    }

    public boolean processMessages() {
        boolean result = false;
        if (connection == null) {
            return result;
        }

        if (collector == null) {
            MRLog.println("Collector not yet set!!");
            return result;
        }
        
        Packet packet = collector.pollResult();
        if (packet == null) {
            return result;
        }
        /*if(packet instanceof Message){//Presence){
           System.out.println("component xmpp packet processMessages from " + packet.getFrom() + " to "+ packet.toXML()); //packet.toXML());
        }*/
        return processMessages(packet);
    }

    ///////////////InternalJID implements /////////////////////////
    @Override
    public JID getAddress() {
        try {
            if (jid == null) {
                jid = new JID(getXMPPUserJid());
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return jid;
    }

    @Override
    public boolean isStarted() {
        return isStarted;
    }

    @Override
    public void sendPacket(org.xmpp.packet.Packet packet) throws Exception {
        InternalJID internalJID = manager.getInternalJID(packet.getTo());
        if (packet != null) {
            if (internalJID != null) {
                internalJID.processPacket(packet);
                return;
            } 
            connection.sendPacket(packet);
        }
    }

    ///////////////InternalJID implements /////////////////////////

    public void addPacketListener(PacketListener packetListener, PacketFilter packetFilter) {
        packetReader.addPacketListener(packetListener, packetFilter);
    }

    public PacketCollector createPacketCollector(PacketFilter packetFilter) {
        return packetReader.createPacketCollector(packetFilter);
    }

    //////////////////// XmppSessionEventListener ///////////////////////
    public void loginComplete(XmppSession comp, int ID) {
        liveComp++;
        if (ID + 1 == countInComp) {
            bConnectionSuccess = true;
            synchronized (this) {
                try {
                    notify();
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        }
    }
    public void logoutComplete(XmppSession comp, int ID) {
        liveComp--;
        MRLog.println("*** logoutComplete  " + ID +" liveComp "+ liveComp);
        if(liveComp <= 0){
            bConnectionSuccess = false;
        }
    }

    public void loginComplete(XmppSession comp) {
        MRLog.println("*** loginComplete "  );
        bConnectionSuccess = true;
    }

    public void logoutComplete(XmppSession comp) {
        MRLog.println("*** logoutComplete "  );
        bConnectionSuccess = false;
    }
    
    public void connectionLost(XmppSession comp, int ID) {
        liveComp--;
        MRLog.println("*** connectionLost " + ID +" liveComp "+ liveComp);
        if(liveComp <= 0){
            bConnectionSuccess = false;
        }
    }
    //////////////////// XmppSessionEventListener ///////////////////////

    ////////////////////processPacket ///////////////////////
    @Override
    public void processPacket(org.xmpp.packet.Packet packet, XmppSession xmppsession, int i) {
        try {
            processPacket(packet);
        } catch (Exception e) {
            e.printStackTrace();
        }
    }

    @Override
    public void processPacket(String to, String packet) throws Exception {
        packetReader.convertPacket(packet);
    }

    @Override
    public void processPacket(org.xmpp.packet.Packet packet) throws Exception {
        if (packet != null) {
            packetReader.convertPacket(packet);
        }
    }
    //////////////////

    @Override
    public boolean isUserPresenceAvailable(String touser) {
        if(touser ==null || touser.equals("")){
            return false;
        }
        if(touser.equals(myjid)){
            return true;
        }
        try {
            boolean present = presenseHandler.isUserPresence(touser);
            if (present) {
                return true;
            }
        } catch (Exception ex) {
            MRLog.println(touser + " isUserPresenceAvailable exception using roster.");
            ex.printStackTrace();
        }
        try {
            bsLocalIPHandler.isFileTransferToLocalIP(touser);
            return true;
        } catch (Exception ex) {
            MRLog.println(touser + " isUserPresenceAvailable exception using isFileTransferToLocalIP.");
            ex.printStackTrace();
        }
        return false;        
        
    }

    @Override
    public String getIPVSConnectionType() {
        return IPVSSession.COMPONENT_CONNECTION;
    }

    @Override
    public void subscribeUserPresence(String touser) throws Exception {
        if(touser ==null || touser.equals("")){
            return ;
        }
        if(touser.equals(myjid)){
            return ;
        }        
        JID jid = getJID(touser);
        if(jid==null){
            return;
        }
        if (!presenseHandler.isUserPresence(touser)) {
            Presence subscription = new Presence(Presence.Type.subscribe);
            subscription.setTo(myjid);
            subscription.setFrom(touser);
            sendPacket(subscription);
        }
    }

    @Override
    public void unSubscribeUserPresence(String touser) throws Exception {
        if(touser ==null || touser.equals("")){
            return ;
        }
        if(touser.equals(myjid)){
            return ;
        }    
        JID jid = getJID(touser);
        if(jid==null) {
            return;
        }
        if (presenseHandler.isUserPresence(touser)) {
            Presence subscription = new Presence(Presence.Type.unsubscribe);
            subscription.setTo(myjid);
            subscription.setFrom(touser);
            sendPacket(subscription);
        }
        
    }    
    public JID getJID(String user){
        try{
            return new JID(user);
        }catch(Exception ex){
        }
        return null;
    }

	@Override
	public boolean isConnected() {
		return ( ! connection.isConnectionLostToServer() );
	}

	@Override
	public void removePacketListener(PacketListener packetListener) {
		 packetReader.removePacketListener(packetListener);
	}

    @Override
    public String getUser() {
        return myjid;
    }

    @Override
    public String getServiceName() {
        if(servername==null) {
            servername = myjid.substring(myjid.indexOf(".") + 1);
        }
        return servername;
    }
    
    public String getLoginType() {
        return XMPPI.LOGIN_TYPE_COMP;
    }

	@Override
	public void addDiscoFeature(String name) {
		iPVSServiceDiscoveryManager.addFeature(name);
	}   
    
}
