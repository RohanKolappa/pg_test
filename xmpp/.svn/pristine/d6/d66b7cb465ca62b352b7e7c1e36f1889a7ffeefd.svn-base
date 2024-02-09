package com.ipvsserv.starter;

import java.io.File;
import java.io.FilenameFilter;
import java.lang.reflect.Method;
import java.net.MalformedURLException;
import java.net.URL;
import java.net.URLClassLoader;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.plugin.PluginClassLoader;
import com.ipvsserv.server.IpvsServer;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public class IpvsServClassLoader extends URLClassLoader {
    public String getName(){
        return "IpvsServClassLoader";
    }
    public IpvsServClassLoader() {
        super(new URL[] {}, findParentClassLoader());
    }    
    public IpvsServClassLoader(ClassLoader parent, File libDir) throws MalformedURLException {//, File xmldbjar) throws MalformedURLException {
        super(new URL[] { libDir.toURI().toURL() }, parent);
        loadIpvsLib(libDir);
    }
    public IpvsServClassLoader(ClassLoader parent, File libDir, File mrlibDir) throws MalformedURLException {//, File xmldbjar) throws MalformedURLException {
        super(new URL[] { libDir.toURI().toURL(), mrlibDir.toURI().toURL() }, parent);
        loadIpvsLib(mrlibDir);
        loadIpvsLib(libDir);
    }
    public IpvsServClassLoader(ClassLoader parent, File libDir, String[] mrlibDirs) throws MalformedURLException {//, File xmldbjar) throws MalformedURLException {
        super(new URL[] { libDir.toURI().toURL() }, parent);
        for (int i = 0; i < mrlibDirs.length; i++) {
            try{
                loadIpvsLib(new File(mrlibDirs[i]));
            }catch(Exception ex){
                MRLog.error(ex);
            }
        }
        loadIpvsLib(libDir);
    }
    
    public void loadIpvsLib(File libDir, boolean recursive) throws MalformedURLException {//, File xmldbjar) throws MalformedURLException {
        loadIpvsLib(libDir);
        if(recursive){
            File[] List = libDir.listFiles();
            for (File file : List) {
                if(file.isDirectory()){
                    loadIpvsLib(file);
                }
            }
        }
    }   
    
    public void loadIpvsLib(File libDir) throws MalformedURLException {//, File xmldbjar) throws MalformedURLException {
        if(libDir !=null){
            MRLog.debug("class loader contructor  libDir " + libDir.toURI().toURL() +" " );
        }
        File[] jars = libDir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                boolean accept = false;
                String smallName = name.toLowerCase();
                if (smallName.endsWith(".jar")) {
                    accept = true;
                } else if (smallName.endsWith(".zip")) {
                    accept = true;
                }
                return accept;
            }
        });
        // addURL(xmldbjar.toURL());
        // Do nothing if no jar or zip files were found
        if (jars == null) {
            return;
        }
       
        for (int i = 0; i < jars.length; i++) {
            if (jars[i].isFile()) {
                if (jars[i].getName().equalsIgnoreCase("xmldb.jar")) {
                   // MRLog.info("XMLDBjar? IPVSClassLoader load jars[i].toURL() " + jars[i].toURI().toURL().toString());
                } else if (jars[i].getName().startsWith("ipvsstarter_")) {
                } else if (jars[i].getName().startsWith("smack-1.4.jar")) {
                } else if (jars[i].getName().startsWith("smackx-1.4.jar")) {
                   // MRLog.info("skip? IPVSClassLoader load jars[i].toURL() " + jars[i].toURI().toURL().toString());
                } else {
                    //MRLog.info("IPVSClassLoader load jars[i].toURL() " + jars[i].toURI().toURL().toString());
                    //System.out.println("add jar "+ jars[i].getAbsolutePath());
                    addURL(jars[i].toURI().toURL());
                }
                
            }
        }
    }
    public IpvsServClassLoader(URL[] urls, ClassLoader findParentClassLoader) {
        super(new URL[] {}, findParentClassLoader());
    }
    public void addURL(URL url) {
        super.addURL(url);
        URLClassLoader sysloader = (URLClassLoader) ClassLoader.getSystemClassLoader();
        Class<?> sysclass = URLClassLoader.class;
        try {
           Class<?>[] argClasses = {URL.class };
           Method method = sysclass.getDeclaredMethod("addURL", argClasses);
           method.setAccessible(true);
           method.invoke(sysloader, new Object[]{url});
        } catch (Throwable t) {
           t.printStackTrace();
           //throw new Exception("Error, could not add URL to system classloader");
        }//end try catch
    }
    // used for plugin loading 
    public void addDirectory(File directory, boolean developmentMode) {
        try {
            if(directory !=null){
                MRLog.debug("class loader for addDirectory " + directory.toURI().toURL() +" " );
            }
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
                        //System.out.println("jars[i].toURL()" + jars[i].toURL());
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
            //System.out.println("class loader for plugin " + directory.toURL() +" " );
            /*try{
                if(directory.getName().indexOf("mediaroom") != -1){
                Class<?>  c = this.findClass("org.jivesoftware.smack.XMPPException");
                //System.out.println(directory.getName()+ " c " + (c==null) +" " );
                }
            }catch(Exception ex){
                ex.printStackTrace();
            }*/
        } catch (MalformedURLException mue) {
            MRLog.error(mue);
        }
    }
   
    public Class<?> loadClass(String name, boolean resolve ) throws ClassNotFoundException{
		return super.loadClass(name, resolve);
    } 

    public Package[] getPackages() {
    	 return super.getPackages();
    } 
    
    public void addDirectory(String folderPath ) {
        try {
            File directory = new File(folderPath);
            // Add classes directory to classpath.
            if (directory.exists()) {
                addURL(directory.toURI().toURL());
            }
            if(directory.isFile()){
                return;
            }
            if(directory !=null){
                MRLog.debug("class loader for addDirectory only " + directory.toURI().toURL() +" " );
            }            
            // Add directory to classpath.
            File[] jars = directory.listFiles(new FilenameFilter() {
                public boolean accept(File dir, String name) {
                    return name.endsWith(".jar") || name.endsWith(".zip");
                }
            });
            if (jars != null) {
                for (int i = 0; i < jars.length; i++) {
                    if (jars[i] != null && jars[i].isFile()) {
                    	//System.out.println("addURL " + jars[i].getAbsolutePath());
                        addURL(jars[i].toURI().toURL());
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
