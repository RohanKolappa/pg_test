package com.ipvsserv.starter;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.InputStream;
import java.util.jar.JarOutputStream;
import java.util.jar.Pack200;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

//-DLogToConsole=true
//-DtestDBXmlPath=testDB_read.xml
//-DipvsAppName=ipvscomponent or existdbtest or sednativedbtest or dbtask -DdbTaskName=[restore or backup ] -DdbTaskFolder=/data/dbtask/backup -DipvscomponentHome=C:/pravin3/extcomp/xmpp/IPVSServComponent/home  -Dipvscomponent.lib.dir=C:/pravin3/extcomp/xmpp/IPVSServComponent/out

//-DLogToConsole=true -DipvsAppName=ipvscomponent -DipvscomponentHome=C:/pravin/branch/xmpp/IPVSServComponent/home/ipvscomponent  -Dipvscomponent.lib.dir=C:/pravin/branch/xmpp/IPVSServComponent/out
//-DipvsAppName=existdbtask -DipvscomponentHome=C:/pravin3/extcomp/xmpp/IPVSServComponent/home  -Dipvscomponent.lib.dir=C:/pravin3/extcomp/xmpp/IPVSServComponent/out
//-DipvsAppName=existdbtest -DipvscomponentHome=C:/java/IPVSServComponent/home  -Dipvscomponent.lib.dir=C:/java/IPVSServComponent/out
//-DipvsAppName=sednanativedbtest -DipvscomponentHome=C:/java/IPVSServComponent/home  -Dipvscomponent.lib.dir=C:/java/IPVSServComponent/out
public class ServerStarter {
    
    private static final String DEFAULT_LIB_DIR = "/home/onair/ipvscomponent/lib";
    private static final String DEFAULT_HOME = "/home/onair/ipvscomponent";
    private static final String IPVS_MAIN_CLASSNAME = "com.ipvsserv.server.IpvsServer";
    private static final String DEFAULT_ADMIN_LIB_DIR = "../plugins/admin/webapp/WEB-INF/lib";
    private static final String DEFAULT_XMLDBJAR = "xmldblb/sedna/xdmldb.jar";
    
    public static void main(String[] args) {
        new ServerStarter().start(args);
    }
    
    private void printUsage() {
        try {
            StringBuffer buf = new StringBuffer();
            buf.append("\njvm arguments ");
            buf.append("\n-DipvsAppName = ipvscomponent or ipvsdbtask");
            buf.append("\n-DdbTaskName=restore or backup");
            buf.append("\n-Ddbtaskfolder=/data/dbtask/backup");
            buf.append("\n-DipvscomponentHome = ipvscomponent home path ");
            buf.append("\n-Dipvscomponent.lib.dir = ipvscomponent lib path ");
            buf.append("\n-DipvsConfigFilePath= conf/ipvscomponent.xml path ");
            buf.append("\njava -jar /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar [optional argument ipvscomponent home path. not needed if DipvscomponentHome provided this]");
            buf.append("\n................... e.g. .....................");
            buf.append("\n/usr/bin/java -server -Xmx128m -DipvsAppName=ipvscomponent -DipvscomponentHome=/home/onair/ipvscomponent -Dipvscomponent.lib.dir=/home/onair/ipvscomponent/lib -classpath /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar -jar /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar \"/home/onair/ipvscomponent\" &");
            buf.append("\n.................... .....................");
            buf.append("\n/usr/bin/java -server -Xmx128m -DipvsAppName=ipvsdbtask -DdbTaskName=restore -DdbTaskFolder=/data/dbtask/backup  -DipvscomponentHome=/home/onair/ipvscomponent -Dipvscomponent.lib.dir=/home/onair/ipvscomponent/lib -classpath /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar -jar /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar \"/home/onair/ipvscomponent\" &");
            buf.append("\n.................... .....................");
            buf.append("\n/usr/bin/java -server -Xmx128m -DipvsAppName=ipvsdbtask -DdbTaskName=backup -DdbTaskFolder=/data/dbtask/backup -DipvscomponentHome=/home/onair/ipvscomponent -Dipvscomponent.lib.dir=/home/onair/ipvscomponent/lib -classpath /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar -jar /home/onair/ipvscomponent/lib/ipvscomponentstartup.jar \"/home/onair/ipvscomponent\" &");
            MRLog.error(buf.toString());
            MRLog.loadLog4j();
            MRLog.warn(buf.toString());
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void start(String[] args) {
        // Setup the classpath using JiveClassLoader
        try {
            long ss = System.currentTimeMillis();
            ProcessArguments argList = new ProcessArguments(args);
            
            // Load up the bootstrap container
            final ClassLoader parent = findParentClassLoader();
            //must suply
            String ipvsAppName = System.getProperty("ipvsAppName");
            String dbTaskName = null;
            String dbTaskFolder = null;
            if (ipvsAppName == null || ipvsAppName.equals("")) {
                MRLog.info("ABORTING appName is not provided ");
                printUsage();
                return;
            } else {
                if (ipvsAppName.trim().equalsIgnoreCase("ipvsdbtask")) {
                    dbTaskName = System.getProperty("dbTaskName");
                    dbTaskFolder = System.getProperty("dbTaskFolder");
                    if (dbTaskName == null || dbTaskFolder == null) {
                        MRLog.info("ABORTING dbTask  or dbtaskfolder are not provided ");
                        printUsage();
                        return;
                    }
                }
                
            }
            StringBuffer buf = new StringBuffer();
            
            ////////////////home
            String ipvscomponentHome = System.getProperty("ipvscomponentHome");
            if (ipvscomponentHome == null || ipvscomponentHome.equals("")) {
                buf.append("Lib directory argument -ipvscomponentHome='' is empty " + ipvscomponentHome + " does not exist. Using default " + DEFAULT_HOME);
                ipvscomponentHome = DEFAULT_HOME;
            }
            ipvscomponentHome = ipvscomponentHome.replace('\\', File.separatorChar).replace('/', File.separatorChar);
            System.setProperty("ipvscomponentHome", ipvscomponentHome);
            
            //////lib dir 
            String libDirString = System.getProperty("ipvscomponent.lib.dir");
            if (libDirString == null || libDirString.equals("")) {
                buf.append("\nLib directory argument -DipvscomponentHome='' is empty " + libDirString + " does not exist. Using default " + DEFAULT_LIB_DIR);
                libDirString = DEFAULT_LIB_DIR;
                System.setProperty("ipvscomponent.lib.dir", libDirString);
            }
            libDirString = libDirString.replace('\\', File.separatorChar).replace('/', File.separatorChar);
            System.setProperty("ipvscomponent.lib.dir", libDirString);
            
             
            /////////////////  config 
            if (System.getProperty("ipvsConfigFilePath") == null || System.getProperty("ipvsConfigFilePath").equals("")) {
                System.setProperty("ipvsConfigFilePath", ipvscomponentHome + File.separator + "conf" + File.separator + ipvsAppName + File.separator + "ipvscomponent.xml");
            }
            if (System.getProperty("log4jConfigFilePath") == null || System.getProperty("log4jConfigFilePath").equals("")) {
                System.setProperty("log4jConfigFilePath", ipvscomponentHome + File.separator + "conf" + File.separator + ipvsAppName + File.separator + "log4j.xml");
            }
            if (System.getProperty("pluginDirs") == null || System.getProperty("pluginDirs").equals("")) {
                System.setProperty("pluginDirs", ipvscomponentHome + File.separatorChar + "plugins");
            }
            String ipvsConfigFilePath = System.getProperty("ipvsConfigFilePath").replace('\\', File.separatorChar).replace('/', File.separatorChar);
            String log4jConfigFilePath = System.getProperty("log4jConfigFilePath").replace('\\', File.separatorChar).replace('/', File.separatorChar);
            String pluginDirs = System.getProperty("pluginDirs").replace('\\', File.separatorChar).replace('/', File.separatorChar);
            System.setProperty("ipvsConfigFilePath", ipvsConfigFilePath);
            System.setProperty("log4jConfigFilePath", log4jConfigFilePath);
            System.setProperty("pluginDirs", pluginDirs);
            /////////////// config 
            
            MRLog.loadLog4j();
            if (buf.length() > 0) {
                MRLog.warn(buf.toString());
            }
            
            // throw errors from here 
            IpvsServGlobals.setHomeDirectory(ipvscomponentHome.toString());
            IpvsServGlobals.setConfigPath(ipvsConfigFilePath);
            IpvsServGlobals.setPluginsDir(pluginDirs);
            IpvsServGlobals.setIpvsAppName(ipvsAppName.toLowerCase());
            IpvsServGlobals.setDBTaskName(dbTaskName);
            IpvsServGlobals.setDBTaskFolder(dbTaskFolder);
            IpvsServGlobals.setProcessArguments(argList);
            //IpvsServGlobals.setKeyStoreLocationPath(keyStoreLocationPath);
            
            File libDir = new File(libDirString);
            if (!libDir.exists()) {
                MRLog.warn("ABORT. Lib directory does not exist. '" + libDirString + "' ");
                return;
            }
            // Unpack any pack files.
            unpackArchives(libDir, true);
            
            String adminLibDirString = ipvscomponentHome + "/plugins/admin/webapp/WEB-INF/lib";
            File adminLibDir = new File(adminLibDirString);
            if (adminLibDir.exists()) {
                unpackArchives(adminLibDir, false);
            } else {
                MRLog.warn("Admin Lib Directory '" + adminLibDirString + "' does not exist. Web admin console may not work.");
            }
            MRLog.debug("IpvsServClassLoader( '" + parent.getClass().getName() + "' , '" + libDir.getAbsolutePath() + "')");

            String[] libDirs = IpvsServGlobals.getValues("library.lib");
            IpvsServClassLoader loader = new IpvsServClassLoader(parent, libDir, libDirs);//, xmldbjarfile);
            
            Thread.currentThread().setContextClassLoader(loader);
            Class<?> containerClass = loader.loadClass(IPVS_MAIN_CLASSNAME);
            containerClass.newInstance();
 
        } catch (Exception ex) {
            ex.printStackTrace();
            MRLog.error(ex);
        }
    }
    
    private ClassLoader findParentClassLoader() {
       ClassLoader parent = Thread.currentThread().getContextClassLoader();
        if (parent == null) {
            parent = this.getClass().getClassLoader();
            if (parent == null) {
                parent = ClassLoader.getSystemClassLoader();
            }
        }
        return parent;
    }
    
    private void unpackArchives(File libDir, boolean printStatus) {
        // Get a list of all packed files in the lib directory.
        File[] packedFiles = libDir.listFiles(new FilenameFilter() {
            public boolean accept(File dir, String name) {
                return name.endsWith(".pack");
            }
        });
        
        if (packedFiles == null) {
            // Do nothing since no .pack files were found
            return;
        }
        
        // Unpack each.
        boolean unpacked = false;
        for (File packedFile : packedFiles) {
            try {
                String jarName = packedFile.getName().substring(0, packedFile.getName().length() - ".pack".length());
                // Delete JAR file with same name if it exists (could be due to upgrade
                // from old Openfire release).
                File jarFile = new File(libDir, jarName);
                if (jarFile.exists()) {
                    jarFile.delete();
                }
                
                InputStream in = new BufferedInputStream(new FileInputStream(packedFile));
                JarOutputStream out = new JarOutputStream(new BufferedOutputStream(new FileOutputStream(new File(libDir, jarName))));
                Pack200.Unpacker unpacker = Pack200.newUnpacker();
                // Print something so the user knows something is happening.
                if (printStatus) {
                    System.out.print(".");
                }
                // Call the unpacker
                unpacker.unpack(in, out);
                
                in.close();
                out.close();
                packedFile.delete();
                unpacked = true;
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        // Print newline if unpacking happened.
        if (unpacked && printStatus) {
            System.out.println();
        }
    }
}
