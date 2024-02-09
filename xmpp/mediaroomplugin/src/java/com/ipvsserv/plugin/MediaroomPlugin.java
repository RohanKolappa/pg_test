package com.ipvsserv.plugin;

import java.io.File;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvs.config.IPVSConst;
import com.ipvs.device.AppServerApp;
import com.ipvs.utils.Utils;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.Dom4jUtils;

public class MediaroomPlugin implements Plugin {
    public static void main2(String[] args) {
        //DMSApp dms = new DMSApp();        
        //dms.processCmdLine(args);
        // System.exit(1);
    }

    //private DeviceApp dms = null;
    AppServerApp dms = null;

    @Override
    public void destroyPlugin() {
        if (dms != null) {
            dms = null;
        }
    }

    private boolean debug = false;

    @Override
    public void initializePlugin(PluginManager manager, File pluginDirectory) {
        String[] args = null;
        File pluginxml = new File(pluginDirectory, "plugin.xml");
        Document doc = Dom4jUtils.getDocument(pluginxml);
        Element root = doc.getRootElement();
        //todo: make both jid works
        String pluginName = Dom4jUtils.valueXpath(root, "name", "");
        String ipvssessionimpl = Dom4jUtils.valueXpath(root, "ipvssessionimpl", "");
        boolean enableXMPPArgument = Utils.booleanValue(Dom4jUtils.valueXpath(root, "enablePlugin", ""), false);
        if(enableXMPPArgument){
            if (debug) {
                args = getArgumentsDebug(pluginDirectory);
            } else {
                args = getArguments(root, pluginDirectory);
            }
            IPVSConst.getInstance().setComponentLogin(true);
            IPVSConst.getInstance().setMediaroomPluginName(pluginName);
            IPVSConst.getInstance().setXmppClientClass(ipvssessionimpl);
    
            //String className_DeviceApp = Dom4jUtils.valueXpath(root, "//settings/class", "");
            //IpvsServClassLoader  classLoader= IpvsServer.getInstance().getClassLoader();
            //IPVSConst.getInstance().setClassLoader((ClassLoader)classLoader);
            //todo: get class name and load
            //if(className_DeviceApp.endsWith("AppServerApp")){
            dms = new AppServerApp();
            //}else{
            //dms = new DMSApp();        
            //}
            dms.processCmdLine(args);
        }else{
            System.out.println("Force setXmppEnable=false. SKIP managmentserver component Login. enableXMPPArgument=" + enableXMPPArgument);
            MRLog.debug("Force setXmppEnable=false. SKIP managmentserver component Login. enableXMPPArgument=" + enableXMPPArgument);
            com.ipvs.common.MRLog.getInstance(com.ipvs.common.MRLog.CONSOLE_DAEMON).addInfo("Force setXmppEnable=false. SKIP managmentserver component Login. enableXMPPArgument=" + enableXMPPArgument);
        }

    }

    private String[] getArguments(Element root, File pluginDirectory) {
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
            String login = Dom4jUtils.valueXpath(root, "//settings/login", "");
            String servicedomain = Dom4jUtils.valueXpath(root, "//settings/servicedomain", "");
            String config = Dom4jUtils.valueXpath(root, "//settings/config", "");

            //ArrayList<String> list = new ArrayList<String> ();
            System.out.println("login " + login + " servicedomain " + servicedomain + " config " + config);
            MRLog.debug("login " + login + " servicedomain " + servicedomain + " config " + config);
            // read from plugin.xml 
            //String[] args = { "login", login, "servicedomain", servicedomain};
            String[] args = { "login", login, "servicedomain", servicedomain , "config", config };
            //device class name from plugin.xml 
            //args = list.toArray(args);
            for (int i = 0; i < args.length; i++) {
                System.out.println("argument " + i + " " + args[i]);
                MRLog.debug("argument " + i + " " + args[i]);
            }
            return args;
        } catch (Exception ex) {
            ex.printStackTrace();
        }
        return null;
    }

    private String[] getArgumentsDebug(File pluginDirectory) {
        // read from plugin.xml 
        String[] args = { "login", "C:/pravin3/_test/txlogin.xml", "servicedomain", "C:/pravin3/_test/txdomain.xml", "config", "C:/pravin3/_test/Device.xml" };
        return args;
    }

}
