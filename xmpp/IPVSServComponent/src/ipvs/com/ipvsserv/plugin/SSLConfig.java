package com.ipvsserv.plugin;

import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.IOException;
import java.net.InetAddress;
import java.net.ServerSocket;
import java.security.KeyStore;
import java.security.cert.X509Certificate;
import java.util.List;

import javax.net.ssl.KeyManagerFactory;
import javax.net.ssl.SSLContext;
import javax.net.ssl.SSLServerSocketFactory;
import javax.net.ssl.TrustManagerFactory;

import com.ipvsserv.common.certificate.CertificateEventListener;
import com.ipvsserv.common.certificate.CertificateManager;
import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class SSLConfig {
    
    private static SSLServerSocketFactory s2sFactory;
    private static SSLServerSocketFactory c2sFactory;
    
    private static String storeType;
    private static SSLContext s2sContext;
    private static SSLContext c2sContext;
    
    private static KeyStore keyStore;
    private static String keyStoreLocation;
    private static String keypass;
    
    private static KeyStore s2sTrustStore;
    private static String s2sTrustStoreLocation;
    private static String s2sTrustpass;
    
    private static KeyStore c2sTrustStore;
    private static String c2sTrustStoreLocation;
    private static String c2sTrustpass;
    
    private SSLConfig() {}
    
    static {
        String algorithm = IpvsServGlobals.getProperty("xmpp.socket.ssl.algorithm", "TLS");
        storeType = IpvsServGlobals.getProperty("xmpp.socket.ssl.storeType", "jks");
        
        // Get the keystore location. The default location is security/keystore
        keyStoreLocation = IpvsServGlobals.getProperty("xmpp.socket.ssl.keystore", "resources" + File.separator + "security" + File.separator + "keystore");
        keyStoreLocation = IpvsServGlobals.getHomeDirectory() + File.separator + keyStoreLocation;
        
        // Get the keystore password. The default password is "changeit".
        keypass = IpvsServGlobals.getProperty("xmpp.socket.ssl.keypass", "changeit");
        keypass = keypass.trim();
        
        // Get the truststore location for c2s connections
        c2sTrustStoreLocation = IpvsServGlobals.getProperty("xmpp.socket.ssl.client.truststore", "resources" + File.separator + "security" + File.separator + "client.truststore");
        c2sTrustStoreLocation = IpvsServGlobals.getHomeDirectory() + File.separator + c2sTrustStoreLocation;
        
        c2sTrustpass = IpvsServGlobals.getProperty("xmpp.socket.ssl.client.trustpass", "changeit");
        c2sTrustpass = c2sTrustpass.trim();
        
        // Get the truststore location for s2s connections
        s2sTrustStoreLocation = IpvsServGlobals.getProperty("xmpp.socket.ssl.truststore", "resources" + File.separator + "security" + File.separator + "truststore");
        s2sTrustStoreLocation = IpvsServGlobals.getHomeDirectory() + File.separator + s2sTrustStoreLocation;
        
        // Get the truststore passwprd; default is "changeit".
        s2sTrustpass = IpvsServGlobals.getProperty("xmpp.socket.ssl.trustpass", "changeit");
        s2sTrustpass = s2sTrustpass.trim();
        
        // Load s2s keystore and trusstore
        try {
            keyStore = KeyStore.getInstance(storeType);
            keyStore.load(new FileInputStream(keyStoreLocation), keypass.toCharArray());
            
            s2sTrustStore = KeyStore.getInstance(storeType);
            s2sTrustStore.load(new FileInputStream(s2sTrustStoreLocation), s2sTrustpass.toCharArray());
            
        } catch (Exception ex) {
            MRLog.error("SSLConfig startup problem.\n" + "  storeType: [" + storeType + "]\n" + "  keyStoreLocation: [" + keyStoreLocation + "]\n" + "  keypass: [" + keypass + "]\n" + "  s2sTrustStoreLocation: [" + s2sTrustStoreLocation + "]\n" + "  s2sTrustpass: [" + s2sTrustpass + "]\n");
            keyStore = null;
            s2sTrustStore = null;
            s2sFactory = null;
        }
        // Load c2s trusstore
        try {
            if (s2sTrustStoreLocation.equals(c2sTrustStoreLocation)) {
                c2sTrustStore = s2sTrustStore;
                c2sTrustpass = s2sTrustpass;
            } else {
                c2sTrustStore = KeyStore.getInstance(storeType);
                c2sTrustStore.load(new FileInputStream(c2sTrustStoreLocation), c2sTrustpass.toCharArray());
            }
        } catch (Exception ex) {
            try {
                c2sTrustStore = KeyStore.getInstance(storeType);
                c2sTrustStore.load(null, c2sTrustpass.toCharArray());
            } catch (Exception ex2) {
                MRLog.error("SSLConfig startup problem.\n" + "  storeType: [" + storeType + "]\n" + "  c2sTrustStoreLocation: [" + c2sTrustStoreLocation + "]\n" + "  c2sTrustPass: [" + c2sTrustpass + "]", ex2);
                c2sTrustStore = null;
                c2sFactory = null;
            }
        }
        resetFactory();
        
        // Reset ssl factoty when certificates are modified
        CertificateManager.addListener(new CertificateEventListener() {
            // Reset ssl factory since keystores have changed
            public void certificateCreated(KeyStore keyStore, String alias, X509Certificate cert) {
                resetFactory();
            }
            
            public void certificateDeleted(KeyStore keyStore, String alias) {
                resetFactory();
            }
            
            public void certificateSigned(KeyStore keyStore, String alias, List<X509Certificate> certificates) {
                resetFactory();
            }
        });
    }
    
    private static void resetFactory() {
        try {
            String algorithm = IpvsServGlobals.getProperty("xmpp.socket.ssl.algorithm", "TLS");
            
            s2sContext = SSLContext.getInstance(algorithm);
            c2sContext = SSLContext.getInstance(algorithm);
            
            KeyManagerFactory keyFactory = KeyManagerFactory.getInstance(KeyManagerFactory.getDefaultAlgorithm());
            keyFactory.init(keyStore, SSLConfig.getKeyPassword().toCharArray());
            
            TrustManagerFactory s2sTrustFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
            s2sTrustFactory.init(s2sTrustStore);
            
            s2sContext.init(keyFactory.getKeyManagers(), s2sTrustFactory.getTrustManagers(), new java.security.SecureRandom());
            
            s2sFactory = s2sContext.getServerSocketFactory();
            
            if (s2sTrustStore == c2sTrustStore) {
                c2sContext = s2sContext;
                c2sFactory = s2sFactory;
            } else {
                TrustManagerFactory c2sTrustFactory = TrustManagerFactory.getInstance(TrustManagerFactory.getDefaultAlgorithm());
                c2sTrustFactory.init(c2sTrustStore);
                
                c2sContext.init(keyFactory.getKeyManagers(), c2sTrustFactory.getTrustManagers(), new java.security.SecureRandom());
                
                c2sFactory = c2sContext.getServerSocketFactory();
            }
            
        } catch (Exception ex) {
            MRLog.error("SSLConfig factory setup problem.\n" + "  storeType: [" + storeType + "]\n" + "  keyStoreLocation: [" + keyStoreLocation + "]\n" + "  keypass: [" + keypass + "]\n" + "  s2sTrustStoreLocation: [" + s2sTrustStoreLocation + "]\n" + "  s2sTrustpass: [" + s2sTrustpass + "]" + "  c2sTrustStoreLocation: [" + c2sTrustStoreLocation + "]\n" + "  c2sTrustpass: [" + c2sTrustpass + "]", ex);
            keyStore = null;
            s2sTrustStore = null;
            c2sTrustStore = null;
            s2sFactory = null;
            c2sFactory = null;
        }
    }
    
    public static String getKeyPassword() {
        return keypass;
    }
    
    public static String gets2sTrustPassword() {
        return s2sTrustpass;
    }
    
    public static String getc2sTrustPassword() {
        return c2sTrustpass;
    }
    
    public static String[] getDefaultCipherSuites() {
        String[] suites;
        if (s2sFactory == null) {
            suites = new String[] {};
        } else {
            suites = s2sFactory.getDefaultCipherSuites();
        }
        return suites;
    }
    
    public static String[] getSupportedCipherSuites() {
        String[] suites;
        if (s2sFactory == null) {
            suites = new String[] {};
        } else {
            suites = s2sFactory.getSupportedCipherSuites();
        }
        return suites;
    }
    
    public static KeyStore getKeyStore() throws IOException {
        if (keyStore == null) {
            throw new IOException();
        }
        return keyStore;
    }
    
    public static KeyStore gets2sTrustStore() throws IOException {
        if (s2sTrustStore == null) {
            throw new IOException();
        }
        return s2sTrustStore;
    }
    
    public static KeyStore getc2sTrustStore() throws IOException {
        if (c2sTrustStore == null) {
            throw new IOException();
        }
        return c2sTrustStore;
    }
    
    public static KeyStore initializeKeyStore() {
        try {
            keyStore = KeyStore.getInstance(storeType);
            keyStore.load(null, keypass.toCharArray());
        } catch (Exception ex) {
            MRLog.error("Unable to initialize keystore: ", ex);
        }
        return keyStore;
    }
    
    public static void saveStores() throws IOException {
        try {
            keyStore.store(new FileOutputStream(keyStoreLocation), keypass.toCharArray());
            s2sTrustStore.store(new FileOutputStream(s2sTrustStoreLocation), s2sTrustpass.toCharArray());
            if (c2sTrustStore != s2sTrustStore) {
                c2sTrustStore.store(new FileOutputStream(c2sTrustStoreLocation), c2sTrustpass.toCharArray());
            }
        } catch (IOException ex) {
            throw ex;
        } catch (Exception ex) {
            throw new IOException(ex.getMessage());
        }
    }
    
    public static ServerSocket createServerSocket(int port, InetAddress ifAddress) throws IOException {
        if (s2sFactory == null) {
            throw new IOException();
        } else {
            return s2sFactory.createServerSocket(port, -1, ifAddress);
        }
    }
    
    public static ServerSocket createc2sServerSocket(int port, InetAddress ifAddress) throws IOException {
        if (c2sFactory == null) {
            throw new IOException();
        } else {
            return c2sFactory.createServerSocket(port, -1, ifAddress);
        }
    }
    
    public static String getKeystoreLocation() {
        return keyStoreLocation;
    }
    
    public static String gets2sTruststoreLocation() {
        return s2sTrustStoreLocation;
    }
    
    public static String getc2sTruststoreLocation() {
        return c2sTrustStoreLocation;
    }
    
    public static String getStoreType() {
        return storeType;
    }
    
    public static SSLContext getSSLContext() {
        return s2sContext;
    }
    
    public static SSLContext getc2sSSLContext() {
        return c2sContext;
    }
    
    public static SSLServerSocketFactory getServerSocketFactory() {
        return s2sFactory;
    }
    
    public static SSLServerSocketFactory getc2sServerSocketFactory() {
        return c2sFactory;
    }
}
