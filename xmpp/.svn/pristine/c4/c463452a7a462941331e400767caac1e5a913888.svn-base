package com.ipvsserv.plugin;

import java.io.File;
import java.io.FileFilter;
import java.io.InputStream;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.HashMap;
import java.util.HashSet;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArraySet;

import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;

import com.ipvsserv.admin.AdminConsole;
import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.plugin.PluginLicence.License;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;
import com.ipvsserv.starter.IpvsServClassLoader;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class PluginManagerImpl extends IpvsServBasicModule implements PluginManager {
    
    private File pluginDirectory;
    private Map<String, Plugin> plugins;
    //pravin: keep this hash table .. may be useful if want to run two version so same plugin.
    //also try to have differnt loaders for each plugin. for now only one for all supported.
    private Map<Plugin, IpvsServClassLoader> classloaders;
    private Map<Plugin, File> pluginDirs;
    /**
     * Keep track of plugin names and their unzipped files. This list is updated when plugin
     * is exploded and not when is loaded.
     */
    private Map<String, File> pluginFiles;
    private Map<Plugin, PluginDevEnvironment> pluginDevelopment;
    private Map<Plugin, List<String>> parentPluginMap;
    private Map<Plugin, String> childPluginMap;
    private Set<String> devPlugins;
    private Set<PluginListener> pluginListeners = new CopyOnWriteArraySet<PluginListener>();
    private Set<PluginManagerListener> pluginManagerListeners = new CopyOnWriteArraySet<PluginManagerListener>();
    private boolean firstRun;
    
    /////////////////// moudule /////////////////////////////////////////////////////////////////
    public PluginManagerImpl() {
        super("PluginManager");
    }
    
    public PluginManagerImpl(String moduleName) {
        super(moduleName);
        
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        String ipvscomponentHome = System.getProperty("ipvscomponentHome");
        File pluginDir = new File(ipvscomponentHome, "plugins");
        this.pluginDirectory = pluginDir;
        plugins = new ConcurrentHashMap<String, Plugin>();
        pluginDirs = new HashMap<Plugin, File>();
        pluginFiles = new HashMap<String, File>();
        classloaders = new HashMap<Plugin, IpvsServClassLoader>();
        pluginDevelopment = new HashMap<Plugin, PluginDevEnvironment>();
        parentPluginMap = new HashMap<Plugin, List<String>>();
        childPluginMap = new HashMap<Plugin, String>();
        devPlugins = new HashSet<String>();
    }
    
    @Override
    public void start() throws Exception {
        super.startMonitor();
    }
    
    @Override
    public void stop() throws Exception {
        destroy();
    }
    
    @Override
    public void destroy() throws Exception {
        super.destroy();
        // Shutdown all installed plugins.
        for (Plugin plugin : plugins.values()) {
            try {
                plugin.destroyPlugin();
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        plugins.clear();
        pluginDirs.clear();
        pluginFiles.clear();
        classloaders.clear();
        pluginDevelopment.clear();
        childPluginMap.clear();
        
    }
    
    @Override
    public int getLastKnownState() throws Exception {
        return HEALTHY;
    }
    
    @Override
    public void verifyState() throws Exception {
        pluginMonitor();
    }
    
    /////////////////// moudule ///////////////////////////////////////////////////    
    
    public boolean installPlugin(InputStream in, String pluginFilename) {
        //todo: delete
        return true;
    }
    
    public boolean isPluginDownloaded(String pluginFilename) {
        return new File(pluginDirectory + File.separator + pluginFilename).exists();
    }
    
    public Collection<Plugin> getPlugins() {
        return Collections.unmodifiableCollection(plugins.values());
    }
    
    public Plugin getPlugin(String name) {
        return plugins.get(name);
    }
    
    public File getPluginDirectory(Plugin plugin) {
        return pluginDirs.get(plugin);
    }
    
    public File getPluginFile(String name) {
        return pluginFiles.get(name);
    }
    private void loadPlugin(File pluginDir) {
        // Only load the admin plugin during setup mode.
         if (IpvsServer.getInstance().isSetupMode() && !(pluginDir.getName().equals("admin"))) {
            return;
        }
        MRLog.debug("PluginManager: Loading plugin " + pluginDir.getName());
        Plugin plugin;
        try {
            File pluginConfig = new File(pluginDir, "plugin.xml");
            //System.out.println("pluginConfig "+ pluginConfig.getPath());
            if (pluginConfig.exists()) {
                Document pluginXML = Dom4jUtils.getDocument(pluginConfig);
                Element root = pluginXML.getRootElement();
                IpvsServClassLoader pluginLoader= IpvsServer.getInstance().getIpvsClassLoader();//get IPVSServer Loader

                String pluginName = pluginDir.getName();
                String pluginElementName = Dom4jUtils.value(root, "name", "").trim();
                Element el = Dom4jUtils.element(root, pluginElementName);
                if(el !=null){
                    IpvsServGlobals.setElement(el);
                }
                String classesDir = Dom4jUtils.value(root, "classesDir", "").trim();
                pluginLoader.addDirectory(pluginDir, classesDir != null);

                List<String> libDir = Dom4jUtils.valuesXpath(root, "//library/lib");
                if(libDir !=null ){
                    for (Iterator<String> iterator = libDir.iterator(); iterator.hasNext();) {
                        String libFolder = iterator.next();
                        pluginLoader.addDirectory(libFolder);
                    }
                }
                
                if(!classesDir.equals("")){
                    File classes = new File(classesDir);
                    if (classes.exists()) {
                        pluginLoader.addURLFile(classes.getAbsoluteFile().toURI().toURL());
                    }
                }
                
                String className =Dom4jUtils.value(root, "class", "").trim(); 
                //pluginXML.selectSingleNode("/plugin/class").getText().trim();
                plugin = (Plugin) pluginLoader.loadClass(className).newInstance();
                            
                plugins.put(pluginName, plugin);
                pluginDirs.put(plugin, pluginDir);

                // Only register the class loader in the case of this not being
                // a child plugin.
                classloaders.put(plugin, pluginLoader);
 
                
                // Configure caches of the plugin
                configureCaches(pluginDir, pluginName);
                
                // Init the plugin.
                ClassLoader oldLoader = Thread.currentThread().getContextClassLoader();
                //if(pluginLoader==oldLoader ){
                //   System.out.println("pluginLoader==oldLoader " + (pluginLoader==oldLoader) +" " );
                //}
                Thread.currentThread().setContextClassLoader(pluginLoader);
                plugin.initializePlugin(this, pluginDir);
                Thread.currentThread().setContextClassLoader(oldLoader);
 
                firePluginCreatedEvent(pluginName, plugin);

                
            } else {
                MRLog.warn("Plugin " + pluginDir + " could not be loaded: no plugin.xml file found");
            }
        } catch (Throwable e) {
            MRLog.error("Error loading plugin: " + pluginDir, e);
        }
    }
 
    
    private void configureCaches(File pluginDir, String pluginName) {
 
    }
    
    private void firePluginCreatedEvent(String name, Plugin plugin) {
        for (PluginListener listener : pluginListeners) {
            listener.pluginCreated(name, plugin);
        }
    }
    
    private void firePluginsMonitored() {
        for (PluginManagerListener listener : pluginManagerListeners) {
            listener.pluginsMonitored();
        }
    }
    
    public void unloadPlugin(String pluginName) {
        MRLog.debug("PluginManager: Unloading plugin " + pluginName);
        
        Plugin plugin = plugins.get(pluginName);
        if (plugin != null) {
            try {
                plugin.destroyPlugin();
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        plugins.remove(pluginName);
        pluginDirs.remove(plugin);
        classloaders.remove(plugin);
        firePluginDestroyedEvent(pluginName, plugin);
        
    }
 
    private void firePluginDestroyedEvent(String name, Plugin plugin) {
        for (PluginListener listener : pluginListeners) {
            listener.pluginDestroyed(name, plugin);
        }
    }
    
    public Class<?> loadClass(Plugin plugin, String className) throws ClassNotFoundException, IllegalAccessException, InstantiationException {
        IpvsServClassLoader loader = classloaders.get(plugin);
        return loader.loadClass(className);
    }
    
    public PluginDevEnvironment getDevEnvironment(Plugin plugin) {
        return pluginDevelopment.get(plugin);
    }
    
    public String getName(Plugin plugin) {
        String name = getElementValue(plugin, "/plugin/name");
        String pluginName = pluginDirs.get(plugin).getName();
        if (name != null) {
            return AdminConsole.getAdminText(name, pluginName);
        } else {
            return pluginName;
        }
    }
    
    public String getDescription(Plugin plugin) {
        String pluginName = pluginDirs.get(plugin).getName();
        return AdminConsole.getAdminText(getElementValue(plugin, "/plugin/description"), pluginName);
    }
    
    public String getAuthor(Plugin plugin) {
        return getElementValue(plugin, "/plugin/author");
    }
    
    public String getVersion(Plugin plugin) {
        return getElementValue(plugin, "/plugin/version");
    }
    
    public String getMinServerVersion(Plugin plugin) {
        return getElementValue(plugin, "/plugin/minServerVersion");
    }
    
    public String getDatabaseKey(Plugin plugin) {
        return getElementValue(plugin, "/plugin/databaseKey");
    }
    
    public int getDatabaseVersion(Plugin plugin) {
        String versionString = getElementValue(plugin, "/plugin/databaseVersion");
        if (versionString != null) {
            try {
                return Integer.parseInt(versionString.trim());
            } catch (NumberFormatException nfe) {
                MRLog.error(nfe);
            }
        }
        return -1;
    }
    
    public License getLicense(Plugin plugin) {
        String licenseString = getElementValue(plugin, "/plugin/licenseType");
        if (licenseString != null) {
            try {
                // Attempt to load the get the license type. We lower-case and
                // trim the license type to give plugin author's a break. If the
                // license type is not recognized, we'll log the error and default
                // to "other".
                return License.valueOf(licenseString.toLowerCase().trim());
            } catch (IllegalArgumentException iae) {
                MRLog.error(iae);
            }
        }
        return License.other;
    }
    
    public IpvsServClassLoader getPluginClassloader(Plugin plugin) {
        return classloaders.get(plugin);
    }
    
    private String getElementValue(Plugin plugin, String xpath) {
        File pluginDir = pluginDirs.get(plugin);
        if (pluginDir == null) {
            return null;
        }
        try {
            File pluginConfig = new File(pluginDir, "plugin.xml");
            if (pluginConfig.exists()) {
                SAXReader saxReader = new SAXReader();
                saxReader.setEncoding("UTF-8");
                Document pluginXML = saxReader.read(pluginConfig);
                Element element = (Element) pluginXML.selectSingleNode(xpath);
                if (element != null) {
                    return element.getTextTrim();
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        return null;
    }
    
    
    
    public void addPluginListener(PluginListener listener) {
        pluginListeners.add(listener);
    }
    
    public void removePluginListener(PluginListener listener) {
        pluginListeners.remove(listener);
    }
    
    public void addPluginManagerListener(PluginManagerListener listener) {
        pluginManagerListeners.add(listener);
        if (isExecuted()) {
            firePluginsMonitored();
        }
    }
    
    public void removePluginManagerListener(PluginManagerListener listener) {
        pluginManagerListeners.remove(listener);
    }
    
    /////////////////////////////////////////////////// pulgin monitor ///////////////////////////////
    public void pluginMonitor() {
        if(firstRun)
            return;
        synchronized (this) {
            if(firstRun)
                return;
            firstRun=true;
        }
        // If the task is already running, return.
        try {
            File[] dirs = pluginDirectory.listFiles(new FileFilter() {
                public boolean accept(File pathname) {
                    return pathname.isDirectory();
                }
            });
            // Sort the list of directories so that the "admin" plugin is always
            // first in the list.
            Arrays.sort(dirs, new Comparator<File>() {
                public int compare(File file1, File file2) {
                    if (file1.getName().equals("admin")) {
                        return -1;
                    } else if (file2.getName().equals("admin")) {
                        return 1;
                    } else {
                        return file1.compareTo(file2);
                    }
                }
            });
            // Load all plugins that need to be loaded.
            for (File dirFile : dirs) {
                // If the plugin hasn't already been started, start it.
                if (dirFile.exists() && !plugins.containsKey(dirFile.getName())) {
                    loadPlugin(dirFile);
                }
            }
            // Trigger event that plugins have been monitored
            firePluginsMonitored();
        } catch (Throwable e) {
            MRLog.error(e);
        }
        
    }
 
   
    
}
