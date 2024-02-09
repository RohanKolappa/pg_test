package com.ipvs.xmpp.client.component;

import java.util.Map;

import com.ipvs.config.IPVSConst;
import com.ipvs.device.AppServerApp;
import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.starter.ServerStarter;

//use below to debug
/*
-DipvsConfigFilePath=home/onair/mscomponent/conf/mscomponent/mscomponent.xml 
-Djava.endorsed.dirs=home/onair/ipvscomponent/xmldblib/sedna/endorsed 
-DipvsAppName=mscomponent 
-DipvscomponentHome=home/onair/ipvscomponent  
-Dipvscomponent.lib.dir=home/onair/ipvscomponent/lib   
-Djava.security.egd=file:/dev/./urandom  
-DipvsConfigFilePath=home/onair/mscomponent/conf/mscomponent/mscomponent.xml
-Dlog4jConfigFilePath=home/onair/mscomponent/conf/mscomponent/log4j.xml
 */
public class MSComponentModule  extends IpvsServBasicModule {//implements Plugin {
    //private DeviceApp dms = null;
    private AppServerApp dms = null;
    private boolean debug = false;
    private boolean runningInsert = false;
    private boolean done = false;
    private IpvsFileStatus initializingFile = null;
    
    public MSComponentModule() {
        super("MSComponent");
    }

    public static void main(String[] args) {
        //DMSApp dms = new DMSApp();        
        //dms.processCmdLine(args);
        // System.exit(1);
        ServerStarter.main(args);
    }
        
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
    }

    @Override
    public void start() throws Exception {
        initializingFile = new IpvsFileStatus(IpvsFileStatus.MS_STATUS_INITIALIZING_FILE);
        initializingFile.isStatusFileExists(true);
        super.startMonitor();
    }

    @Override
    public void stop() throws Exception {
        destroy();
    }

    @Override
    public void destroy() throws Exception {
        if (dms != null) {
            dms = null;
        }
        super.destroy();
        try {
            if(initializingFile !=null){
                initializingFile.removeStatusFile(true);
            }
        }catch(Exception ex){
        }
    }

    @Override
    public void verifyState() throws Exception {

        synchronized (this) {
            if (runningInsert) {
                return;
            }
            runningInsert = true;
        }
        try {

            done = startAppServerApp();
            if (done) {
                started=true;
                initialized = true;
                stopMonitor();
            }
            executed = true;
        } catch (Exception ex) {
            MRLog.error("MSComponentModule", ex);
        } finally {
            if(started) {
                try {
                    initializingFile.removeStatusFile(true);
                } catch (Exception ex) {
                    MRLog.debug("Error Starting AppSrerverApp ........", ex);
                }
                initializingFile=null;
                MRLog.debug("Started AppSrerverApp ........");
            }
            synchronized (this) {
                runningInsert = false;
            }
        }
    }
    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }
 

    private boolean startAppServerApp() throws ClassNotFoundException, InstantiationException, IllegalAccessException {
    	String devappImplClassName = IpvsServGlobals.getProperty(getName()+".appServerAppImpl", "");
    	String ipvssessionimpl = IpvsServGlobals.getProperty(getName()+".ipvssessionimpl", "");
        Class<?> c = Class.forName(devappImplClassName);
        dms =  (AppServerApp) c.newInstance();
        String[] args = null;
        if (debug) {
            args = getArgumentsDebug();
        } else {
            args = getArguments();
        }
        IPVSConst.getInstance().setComponentLogin(true);
        IPVSConst.getInstance().setMediaroomPluginName(getName());
        IPVSConst.getInstance().setXmppClientClass(ipvssessionimpl);
        dms.processCmdLine(args);
        return true;
    }

   

    private String[] getArguments() {
        /*
            <settings>
                <class>com.ipvs.device.DMSApp</class> 
                <login>/etc/config/LoginRequest.xml</login> 
                <servicedomain>/etc/config/SetServiceDomainRequest.xml</servicedomain>
                <config>/etc/config/Device.xml</config>
                <lib>/home/onair/modules/mediaroom/public</lib>
            </settings>       
        */
        try {
            String login = IpvsServGlobals.getProperty(getName()+".settings.login", ""); 
            String servicedomain = IpvsServGlobals.getProperty(getName()+".settings.servicedomain", ""); 
            String config = IpvsServGlobals.getProperty(getName()+".settings.config", ""); 
            String[] args = { "login", login, "servicedomain", servicedomain , "config", config };
            return args;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }

    private String[] getArgumentsDebug() {
        // read from plugin.xml 
        String[] args = { "login", "C:/pravin3/_test/txlogin.xml", "servicedomain", "C:/pravin3/_test/txdomain.xml", "config", "C:/pravin3/_test/Device.xml" };
        return args;
    }
     

}
