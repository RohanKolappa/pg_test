package com.ipvsserv.plugin;

import java.io.File;
import java.io.FilenameFilter;
import java.net.MalformedURLException;
import java.net.URL;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.starter.IpvsServClassLoader;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PluginClassLoader extends IpvsServClassLoader {
    
    public PluginClassLoader() {
        super(new URL[] {}, findParentClassLoader());
    }
    
    public void addDirectory(File directory, boolean developmentMode) {
        try {
            // Add classes directory to classpath.
            File classesDir = new File(directory, "classes");
            if (classesDir.exists()) {
                addURL(classesDir.toURI().toURL());
            }
            
            // Add i18n directory to classpath.
            File databaseDir = new File(directory, "database");
            if (databaseDir.exists()) {
                addURL(databaseDir.toURI().toURL());
            }
            
            // Add i18n directory to classpath.
            File i18nDir = new File(directory, "i18n");
            if (i18nDir.exists()) {
                addURL(i18nDir.toURI().toURL());
            }
            
            // Add web directory to classpath.
            File webDir = new File(directory, "web");
            if (webDir.exists()) {
                addURL(webDir.toURI().toURL());
            }
            
            // Add lib directory to classpath.
            File libDir = new File(directory, "lib");
            File[] jars = libDir.listFiles(new FilenameFilter() {
                public boolean accept(File dir, String name) {
                    return name.endsWith(".jar") || name.endsWith(".zip");
                }
            });
            if (jars != null) {
                for (int i = 0; i < jars.length; i++) {
                    if (jars[i] != null && jars[i].isFile()) {
                        if (developmentMode) {
                            // Do not add plugin-pluginName.jar to classpath.
                            if (!jars[i].getName().equals("plugin-" + directory.getName() + ".jar")) {
                                addURL(jars[i].toURI().toURL());
                            }
                        } else {
                            addURL(jars[i].toURI().toURL());
                        }
                    }
                }
            }
        } catch (MalformedURLException mue) {
            MRLog.error(mue);
        }
    }
    
    public void addURLFile(URL file) {
        addURL(file);
    }
    
    private static ClassLoader findParentClassLoader() {
        ClassLoader parent = IpvsServer.class.getClassLoader();
        if (parent == null) {
            parent = PluginClassLoader.class.getClassLoader();
        }
        if (parent == null) {
            parent = ClassLoader.getSystemClassLoader();
        }
        return parent;
    }
}
