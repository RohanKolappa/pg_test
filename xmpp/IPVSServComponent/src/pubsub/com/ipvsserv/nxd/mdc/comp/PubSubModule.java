package com.ipvsserv.nxd.mdc.comp;

import java.util.Map;
import java.util.concurrent.LinkedBlockingQueue;
import java.util.concurrent.ThreadFactory;
import java.util.concurrent.ThreadPoolExecutor;
import java.util.concurrent.TimeUnit;
import java.util.concurrent.atomic.AtomicInteger;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.threadmodel.ChangeThreadName;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.mdc.iq.DomainManagerImpl;
import com.ipvsserv.nxd.mdc.notif.NotificationsRouterImpl;
import com.ipvsserv.nxd.mdc.pres.PresenceHandlerImpl;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.xmpp.mdc.DomainManager;
import com.ipvsserv.xmpp.mdc.NotificationsRouter;
import com.ipvsserv.xmpp.mdc.PresenceHandler;
import com.ipvsserv.xmpp.session.XmppSessionImpl;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class PubSubModule extends IpvsServBasicModule {
    private String secretKey = "";//"test";
    private String host = "localhost";
    private int port = 5275;
    private PubSubEngineImpl engine;
    private XmppSessionImpl xmppSession = null;
    
    public PubSubModule() {
        super("Multi PubSub Module");
    }
    
    public PubSubModule(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void start() throws Exception {
        
        int countInComp = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".pubsub.registerComponents", 20);
        String serviceID = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".pubsub.serviceId", "").trim();
        String serviceDesc = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".pubsub.serviceDesc", "").trim();
        
        xmppSession = new XmppSessionImpl(serviceID, serviceDesc, secretKey, host, port, countInComp);
        xmppSession.login();
        //todo : xmppSession : some event listener is needed to find out when login complete for mediaroom
        engine = new PubSubEngineImpl(serviceID, getIpvsModuleName());
        engine.setXmppSession(xmppSession);
        // set xmpp event listener
        xmppSession.addXmppSessionEventListener(engine);
        try {
            int processInThreadCorePoolSize = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".thread.corePoolSize", 40);
            int processInThreadMaximumPoolSize = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".thread.maxPoolSize", 100);
            int processInThreadKeepAliveTime = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".thread.keepAlivetimeSec", 60);
            
            // Create a pool of threads that will process requests received by this component. If more
            // threads are required then the command will be executed on the SocketReadThread process
            ThreadPoolExecutor threadPool = new ThreadPoolExecutor(processInThreadCorePoolSize, processInThreadMaximumPoolSize, processInThreadKeepAliveTime, TimeUnit.SECONDS, new LinkedBlockingQueue<Runnable>(), new ThreadPoolExecutor.CallerRunsPolicy());
            
            final ThreadFactory originalThreadFactory = threadPool.getThreadFactory();
            ThreadFactory newThreadFactory = new ThreadFactory() {
                private final AtomicInteger threadId = new AtomicInteger(0);
                
                public Thread newThread(Runnable runnable) {
                    Thread t = originalThreadFactory.newThread(new ChangeThreadName(runnable, "MDC_" + threadId.incrementAndGet()));
                    t.setDaemon(true);
                    return t;
                }
            };
            threadPool.setThreadFactory(newThreadFactory);
            
            //set thread pool on engine 
            engine.setThreadPoolExecutor(threadPool);
            
            // domain manager
            DomainManager domainManager = new DomainManagerImpl();
            domainManager.setParentIpvsModuleName(getIpvsModuleName());
            engine.setDomainManager(domainManager);
            
            //presence
            PresenceHandler presenceHandler = new PresenceHandlerImpl(xmppSession);
            presenceHandler.setEnableLog(MRLog.isPresenceLogEnable());
            presenceHandler.addPresneceListner(engine);
            engine.setPresenceHandler(presenceHandler);
            
            //notification
            NotificationsRouter notificationsRouter = new NotificationsRouterImpl(presenceHandler, xmppSession);
            engine.setNotificationsRouter(notificationsRouter);
            
            engine.start();
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void stop() throws Exception {
        destroy();
    }
    
    @Override
    public void destroy() throws Exception {
        engine.shutdown(); //todo: test new added
        xmppSession.logout();
        super.destroy();
        // hack to allow safe stopping
        MRLog.info("XMPP (DB Component) Module stopped");
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        host = CompUtils.nonEmpty(params.get("host"), host);
        port = CompUtils.intValue(params.get("port"), port);
        secretKey = CompUtils.nonEmpty(params.get("secretKey"), "");
    }
    
    @Override
    public void verifyState() throws Exception {

    }
    
    @Override
    public int getLastKnownState() throws Exception {
        if (xmppSession.isStarted() && !xmppSession.isConnectionLostToServer()) {
            state = HEALTHY;
        } else {
            state = UNHEALTHY;
        }
        //todo: the other thing: openfire diconnected not known here
        return state;
    }
    
}
