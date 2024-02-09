package com.ipvsserv.admin;

import java.io.File;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.plugin.Plugin;
import com.ipvsserv.plugin.PluginManager;
import com.ipvsserv.plugin.http.JettyModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class AdminConsolePlugin implements Plugin {
    private JettyModule server = null;
    
    public AdminConsolePlugin() {
        super();
    }
    
    public void initializePlugin(PluginManager manager, File pluginDir) {
        //server = IpvsServer.getInstance().getJettyModule();
        server = (JettyModule) IpvsServer.getInstance().getModule(JettyModule.class);
        if(server !=null) {
            if (!server.isStarted()) {
                server.setPluginDir(pluginDir);
                try {
                    server.createWebAppContext();
                    server.startup();
                } catch (Exception ex) {
                    MRLog.error("Error initializePlugin admin UI console server", ex);
                }
            }
        }
        MRLog.debug("initializePlugin admin UI done");
    }
    
    public void destroyPlugin() {
        try {
            if(server !=null) {
                server.destroy();
            }
        } catch (Exception ex) {
            MRLog.error("Error destroyPlugin admin UI console server", ex);
        }
        MRLog.debug("destroyPlugin admin UI done");
    }
    
}
