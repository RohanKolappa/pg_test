package com.ipvsserv.plugin.http;

import java.io.File;
import java.security.KeyStore;
import java.security.cert.X509Certificate;
import java.util.List;
import java.util.Map;

import javax.net.ssl.SSLContext;

import org.mortbay.jetty.Connector;
import org.mortbay.jetty.Handler;
import org.mortbay.jetty.Server;
import org.mortbay.jetty.handler.ContextHandlerCollection;
import org.mortbay.jetty.handler.DefaultHandler;
import org.mortbay.jetty.nio.SelectChannelConnector;
import org.mortbay.jetty.security.SslSelectChannelConnector;
import org.mortbay.jetty.servlet.Context;
import org.mortbay.jetty.webapp.WebAppContext;

import com.ipvsserv.common.certificate.CertificateEventListener;
import com.ipvsserv.common.certificate.CertificateManager;
import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.plugin.SSLConfig;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class JettyModule extends IpvsServBasicModule {
    public final static String secret = CompUtils.randomString(64);
    protected int adminPort;
    protected int adminSecurePort;
    protected Server jettyServer;
    protected ContextHandlerCollection contexts;
    protected CertificateEventListener certificateListener;
    protected boolean restartNeeded = false;
    protected boolean sslEnabled = false;
    
    protected File pluginDir;
    
    /////////////////////////////////////////////
    public JettyModule() {
        this("Jetty server");
    }
    
    public JettyModule(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            File pluginsDir = new File(IpvsServGlobals.getPluginsDir());
            pluginDir = new File(pluginsDir, "admin");
            contexts = new ContextHandlerCollection();
            // Configure Jetty logging to a more reasonable default.
            System.setProperty("org.mortbay.log.class", "com.ipvsserv.common.log.JettyLog");
            // JSP 2.0 uses commons-logging, so also override that implementation.
            System.setProperty("org.apache.commons.logging.LogFactory", "com.ipvsserv.common.log.CommonsLogFactory");
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void start() throws Exception {
        try {
            //super.startMonitor();
            // Ask the state monitor to update the state immediately.
            stateMonitor.run();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void stop() throws Exception {
        try {
            destroy();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void destroy() throws Exception {
        MRLog.debug("----------------------stopHTTPServer  start");
        synchronized (objectSync) {
            started = false;
        }
        // Remove listener for certificate events
        if (certificateListener != null) {
            CertificateManager.removeListener(certificateListener);
        }
        
        //noinspection ConstantConditions
        try {
            if (jettyServer != null && jettyServer.isRunning()) {
                jettyServer.stop();
            }
        } catch (Exception ex) {
            MRLog.error("Error stopping admin console server", ex);
        }
        jettyServer = null;
        MRLog.debug("----------------------stopHTTPServer  done");
    }
    
    @Override
    public void verifyState() throws Exception {
        if (!isStarted()) {
            MRLog.debug("----------------------startHTTPServer  start");
            MRLog.debug("----------------------startHTTPServer " + getPluginDir());
            try {
                createWebAppContext();
                startup();
            } catch (Exception ex) {
                MRLog.error("Error startHTTPServer ", ex);
            }
            MRLog.debug("----------------------startHTTPServer  done");
        }
    }
    
    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }
    
    //////////////////
    
    public File getPluginDir() {
        return pluginDir;
    }
    
    public void setPluginDir(File pluginDir) {
        this.pluginDir = pluginDir;
    }
    
    public void startup() {
        synchronized (objectSync) {
            if (started) return;
            started = true;
        }
        
        restartNeeded = false;
        
        // Add listener for certificate events
        certificateListener = new CertificateListener();
        CertificateManager.addListener(certificateListener);
        
        adminPort = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + ".port", 9190);
        adminSecurePort = IpvsServGlobals.getXMLProperty(getIpvsModuleName() + "securePort", 9191);
        jettyServer = new Server();
        // Do not send Jetty info in HTTP headers
        jettyServer.setSendServerVersion(false);
        
        // Create connector for http traffic if it's enabled.
        if (adminPort > 0) {
            Connector httpConnector = new SelectChannelConnector();
            // Listen on a specific network interface if it has been set.
            String bindInterface = getBindInterface();
            httpConnector.setHost(bindInterface);
            httpConnector.setPort(adminPort);
            jettyServer.addConnector(httpConnector);
        }
        
        // Create a connector for https traffic if it's enabled.
        sslEnabled = false;
        try {
            MRLog.debug(" Using RSA certificates ");
            
            if (adminSecurePort > 0 && CertificateManager.isRSACertificate(SSLConfig.getKeyStore(), "*")) {
                if (!CertificateManager.isRSACertificate(SSLConfig.getKeyStore(), IpvsServer.getInstance().getServerInfo().getXMPPDomain())) {
                    MRLog.debug("Admin console: Using RSA certificates but they are not valid for the hosted domain");
                }
                
                JiveSslConnector httpsConnector = new JiveSslConnector();
                String bindInterface = getBindInterface();
                httpsConnector.setHost(bindInterface);
                httpsConnector.setPort(adminSecurePort);
                
                httpsConnector.setTrustPassword(SSLConfig.gets2sTrustPassword());
                httpsConnector.setTruststoreType(SSLConfig.getStoreType());
                httpsConnector.setTruststore(SSLConfig.gets2sTruststoreLocation());
                httpsConnector.setNeedClientAuth(false);
                httpsConnector.setWantClientAuth(false);
                
                httpsConnector.setKeyPassword(SSLConfig.getKeyPassword());
                httpsConnector.setKeystoreType(SSLConfig.getStoreType());
                httpsConnector.setKeystore(SSLConfig.getKeystoreLocation());
                jettyServer.addConnector(httpsConnector);
                
                sslEnabled = true;
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
        // Make sure that at least one connector was registered.
        if (jettyServer.getConnectors() == null || jettyServer.getConnectors().length == 0) {
            jettyServer = null;
            // Log warning.
            MRLog.debug("ERROR admin.console.warning adminServer.getConnectors() == null || adminServer.getConnectors().length == 0");
            return;
        }
        
        jettyServer.setHandlers(new Handler[] { contexts, new DefaultHandler() });
        try {
            jettyServer.start();
        } catch (Exception ex) {
            MRLog.error("Could not start admin conosle server", ex);
        }
        
        // Log the ports that the admin server is listening on.
        logAdminConsolePorts();
    }
    
    public boolean isRestartNeeded() {
        return restartNeeded;
    }
    
    public String getBindInterface() {
        String interfaceName = IpvsServGlobals.getXMLProperty("network.interface");
        String bindInterface = null;
        if (interfaceName != null) {
            if (interfaceName.trim().length() > 0) {
                bindInterface = interfaceName;
            }
        }
        return bindInterface;
    }
    
    public int getAdminUnsecurePort() {
        return adminPort;
    }
    
    public int getAdminSecurePort() {
        if (!sslEnabled) {
            return 0;
        }
        return adminSecurePort;
    }
    
    public ContextHandlerCollection getContexts() {
        return contexts;
    }
    
    public void restart() {
        try {
            jettyServer.stop();
            jettyServer.start();
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        MRLog.debug("restart admin UI done");
    }
    
    public void createWebAppContext() {
        Context context;
        // Add web-app. Check to see if we're in development mode. If so, we don't
        // add the normal web-app location, but the web-app in the project directory.
        if (Boolean.getBoolean("developmentMode")) {
            context = new WebAppContext(contexts, pluginDir.getParentFile().getParentFile().getParentFile().getParent() + File.separator + "src" + File.separator + "web", "/");
        } else {
            String path = pluginDir.getAbsoluteFile() + File.separator + "webapp";
            MRLog.debug("WebAppContext path " + path);
            context = new WebAppContext(contexts, path, "/");
        }
        context.setWelcomeFiles(new String[] { "index.jsp" });
    }
    
    private void logAdminConsolePorts() {
        // Log what ports the admin console is running on.
        String listening = "listening ";//LocaleUtils.getLocalizedString("admin.console.listening");
        boolean isPlainStarted = false;
        boolean isSecureStarted = false;
        for (Connector connector : jettyServer.getConnectors()) {
            if (connector.getPort() == adminPort) {
                isPlainStarted = true;
            } else if (connector.getPort() == adminSecurePort) {
                isSecureStarted = true;
            }
        }
        
        if (isPlainStarted && isSecureStarted) {
            MRLog.debug(listening + ":" + System.getProperty("line.separator") + "  http://" + IpvsServer.getInstance().getServerInfo().getXMPPDomain() + ":" + adminPort + System.getProperty("line.separator") + "  https://" + IpvsServer.getInstance().getServerInfo().getXMPPDomain() + ":" + adminSecurePort);
        } else if (isSecureStarted) {
            MRLog.debug(listening + " https://" + IpvsServer.getInstance().getServerInfo().getXMPPDomain() + ":" + adminSecurePort);
        } else if (isPlainStarted) {
            MRLog.debug(listening + " http://" + IpvsServer.getInstance().getServerInfo().getXMPPDomain() + ":" + adminPort);
        }
    }
    
    private class CertificateListener implements CertificateEventListener {
        
        public void certificateCreated(KeyStore keyStore, String alias, X509Certificate cert) {
            // If new certificate is RSA then (re)start the HTTPS service
            if ("RSA".equals(cert.getPublicKey().getAlgorithm())) {
                restartNeeded = true;
            }
        }
        
        public void certificateDeleted(KeyStore keyStore, String alias) {
            restartNeeded = true;
        }
        
        public void certificateSigned(KeyStore keyStore, String alias, List<X509Certificate> certificates) {
            // If new certificate is RSA then (re)start the HTTPS service
            if ("RSA".equals(certificates.get(0).getPublicKey().getAlgorithm())) {
                restartNeeded = true;
            }
        }
    }
    
    private class JiveSslConnector extends SslSelectChannelConnector {
        
        @Override
        protected SSLContext createSSLContext() throws Exception {
            return SSLConfig.getSSLContext();
        }
    }
    
}
