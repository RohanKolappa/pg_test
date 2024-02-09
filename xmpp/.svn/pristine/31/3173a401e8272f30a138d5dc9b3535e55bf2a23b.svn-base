package com.ipvsserv.plugin;

import java.io.BufferedInputStream;
import java.io.BufferedOutputStream;
import java.io.File;
import java.io.FileFilter;
import java.io.FileInputStream;
import java.io.FileOutputStream;
import java.io.FilenameFilter;
import java.io.IOException;
import java.io.InputStream;
import java.io.OutputStream;
import java.util.ArrayList;
import java.util.Arrays;
import java.util.Collection;
import java.util.Collections;
import java.util.Comparator;
import java.util.Enumeration;
import java.util.HashMap;
import java.util.HashSet;
import java.util.List;
import java.util.Map;
import java.util.Set;
import java.util.StringTokenizer;
import java.util.concurrent.ConcurrentHashMap;
import java.util.concurrent.CopyOnWriteArraySet;
import java.util.jar.JarEntry;
import java.util.jar.JarFile;
import java.util.jar.JarOutputStream;
import java.util.jar.Pack200;
import java.util.zip.ZipFile;

import org.dom4j.Attribute;
import org.dom4j.Document;
import org.dom4j.Element;
import org.dom4j.io.SAXReader;

import com.ipvsserv.admin.AdminConsole;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.Version;
import com.ipvsserv.plugin.PluginLicence.License;
import com.ipvsserv.plugin.http.PluginServlet;
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
public class PluginManagerWithWatchImpl extends IpvsServBasicModule implements PluginManager {
    
    private File pluginDirectory;
    private Map<String, Plugin> plugins;
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
    public PluginManagerWithWatchImpl() {
        super("PluginManager");
    }
    
    public PluginManagerWithWatchImpl(String moduleName) {
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
        if (in == null || pluginFilename == null || pluginFilename.length() < 1) {
            MRLog.error("Error installing plugin: Input stream was null or pluginFilename was null or had no length.");
            return false;
        }
        try {
            byte[] b = new byte[1024];
            int len;
            // If pluginFilename is a path instead of a simple file name, we only want the file name
            int index = pluginFilename.lastIndexOf(File.separator);
            if (index != -1) {
                pluginFilename = pluginFilename.substring(index + 1);
            }
            // Absolute path to the plugin file
            String absolutePath = pluginDirectory + File.separator + pluginFilename;
            // Save input stream contents to a temp file
            OutputStream out = new FileOutputStream(absolutePath + ".part");
            while ((len = in.read(b)) != -1) {
                //write byte to file
                out.write(b, 0, len);
            }
            out.close();
            // Delete old .jar (if it exists)
            new File(absolutePath).delete();
            // Rename temp file to .jar
            new File(absolutePath + ".part").renameTo(new File(absolutePath));
            // Ask the plugin monitor to update the plugin immediately.
            stateMonitor.run();
        } catch (IOException e) {
            MRLog.error("Error installing new version of plugin: " + pluginFilename, e);
            return false;
        }
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
            if (pluginConfig.exists()) {
                SAXReader saxReader = new SAXReader();
                saxReader.setEncoding("UTF-8");
                Document pluginXML = saxReader.read(pluginConfig);
                
                // See if the plugin specifies a version of Openfire
                // required to run.
                Element minServerVersion = (Element) pluginXML.selectSingleNode("/plugin/minServerVersion");
                if (minServerVersion != null) {
                    String requiredVersion = minServerVersion.getTextTrim();
                    Version version = IpvsServer.getInstance().getServerInfo().getVersion();
                    String hasVersion = version.getMajor() + "." + version.getMinor() + "." + version.getMicro();
                    if (hasVersion.compareTo(requiredVersion) < 0) {
                        String msg = "Ignoring plugin " + pluginDir.getName() + ": requires " + "server version " + requiredVersion;
                        MRLog.warn(msg);
                        return;
                    }
                }
                
                IpvsServClassLoader pluginLoader;
                
                // Check to see if this is a child plugin of another plugin. If it is, we
                // re-use the parent plugin's class loader so that the plugins can interact.
                Element parentPluginNode = (Element) pluginXML.selectSingleNode("/plugin/parentPlugin");
                
                String pluginName = pluginDir.getName();
                String webRootKey = pluginName + ".webRoot";
                String classesDirKey = pluginName + ".classes";
                String webRoot = System.getProperty(webRootKey);
                String classesDir = System.getProperty(classesDirKey);
                
                if (webRoot != null) {
                    final File compilationClassesDir = new File(pluginDir, "classes");
                    if (!compilationClassesDir.exists()) {
                        compilationClassesDir.mkdir();
                    }
                    compilationClassesDir.deleteOnExit();
                }
                
                if (parentPluginNode != null) {
                    String parentPlugin = parentPluginNode.getTextTrim();
                    // See if the parent is already loaded.
                    if (plugins.containsKey(parentPlugin)) {
                        pluginLoader = classloaders.get(getPlugin(parentPlugin));
                        pluginLoader.addDirectory(pluginDir, classesDir != null);
                        
                    } else {
                        // See if the parent plugin exists but just hasn't been loaded yet.
                        // This can only be the case if this plugin name is alphabetically before
                        // the parent.
                        if (pluginName.compareTo(parentPlugin) < 0) {
                            // See if the parent exists.
                            File file = new File(pluginDir.getParentFile(), parentPlugin + ".jar");
                            if (file.exists()) {
                                // Silently return. The child plugin will get loaded up on the next
                                // plugin load run after the parent.
                                return;
                            } else {
                                file = new File(pluginDir.getParentFile(), parentPlugin + ".war");
                                if (file.exists()) {
                                    // Silently return. The child plugin will get loaded up on the next
                                    // plugin load run after the parent.
                                    return;
                                } else {
                                    String msg = "Ignoring plugin " + pluginName + ": parent plugin " + parentPlugin + " not present.";
                                    MRLog.warn(msg);
                                    return;
                                }
                            }
                        } else {
                            String msg = "Ignoring plugin " + pluginName + ": parent plugin " + parentPlugin + " not present.";
                            MRLog.warn(msg);
                            return;
                        }
                    }
                }
                // This is not a child plugin, so create a new class loader.
                else {
                    MRLog.debug("PluginClassLoader  '" + pluginDir.getAbsolutePath() + "'   classesDir'" + classesDir + "' ");
                    pluginLoader = new IpvsServClassLoader();
                    pluginLoader.addDirectory(pluginDir, classesDir != null);
                }
                
                // Check to see if development mode is turned on for the plugin. If it is,
                // configure dev mode.
                
                PluginDevEnvironment dev = null;
                if (webRoot != null || classesDir != null) {
                    dev = new PluginDevEnvironment();
                    
                    MRLog.info("Plugin " + pluginName + " is running in development mode.");
                    if (webRoot != null) {
                        File webRootDir = new File(webRoot);
                        if (!webRootDir.exists()) {
                            // Ok, let's try it relative from this plugin dir?
                            webRootDir = new File(pluginDir, webRoot);
                        }
                        
                        if (webRootDir.exists()) {
                            dev.setWebRoot(webRootDir);
                        }
                    }
                    
                    if (classesDir != null) {
                        File classes = new File(classesDir);
                        if (!classes.exists()) {
                            // ok, let's try it relative from this plugin dir?
                            classes = new File(pluginDir, classesDir);
                        }
                        
                        if (classes.exists()) {
                            dev.setClassesDir(classes);
                            pluginLoader.addURLFile(classes.getAbsoluteFile().toURI().toURL());
                        }
                    }
                }
                
                String className = pluginXML.selectSingleNode("/plugin/class").getText().trim();
                plugin = (Plugin) pluginLoader.loadClass(className).newInstance();
                if (parentPluginNode != null) {
                    String parentPlugin = parentPluginNode.getTextTrim();
                    // See if the parent is already loaded.
                    if (plugins.containsKey(parentPlugin)) {
                        pluginLoader = classloaders.get(getPlugin(parentPlugin));
                        classloaders.put(plugin, pluginLoader);
                    }
                }
                
                plugins.put(pluginName, plugin);
                pluginDirs.put(plugin, pluginDir);
                
                // If this is a child plugin, register it as such.
                if (parentPluginNode != null) {
                    String parentPlugin = parentPluginNode.getTextTrim();
                    List<String> childrenPlugins = parentPluginMap.get(plugins.get(parentPlugin));
                    if (childrenPlugins == null) {
                        childrenPlugins = new ArrayList<String>();
                        parentPluginMap.put(plugins.get(parentPlugin), childrenPlugins);
                    }
                    childrenPlugins.add(pluginName);
                    // Also register child to parent relationship.
                    childPluginMap.put(plugin, parentPlugin);
                } else {
                    // Only register the class loader in the case of this not being
                    // a child plugin.
                    classloaders.put(plugin, pluginLoader);
                }
                
                //                // Check the plugin's database schema (if it requires one).
                //                if (!DbConnectionManager.getSchemaManager().checkPluginSchema(plugin)) {
                //                    // The schema was not there and auto-upgrade failed.
                //                    MRLog.error(pluginName + " - " +
                //                            LocaleUtils.getLocalizedString("upgrade.database.failure"));
                //                }
                
                // Load any JSP's defined by the plugin.
                File webXML = new File(pluginDir, "web" + File.separator + "WEB-INF" + File.separator + "web.xml");
                if (webXML.exists()) {
                    PluginServlet.registerServlets(this, plugin, webXML);
                }
                // Load any custom-defined servlets.
                File customWebXML = new File(pluginDir, "web" + File.separator + "WEB-INF" + File.separator + "web-custom.xml");
                if (customWebXML.exists()) {
                    PluginServlet.registerServlets(this, plugin, customWebXML);
                }
                
                if (dev != null) {
                    pluginDevelopment.put(plugin, dev);
                }
                
                // Configure caches of the plugin
                configureCaches(pluginDir, pluginName);
                
                // Init the plugin.
                ClassLoader oldLoader = Thread.currentThread().getContextClassLoader();
                Thread.currentThread().setContextClassLoader(pluginLoader);
                plugin.initializePlugin(this, pluginDir);
                Thread.currentThread().setContextClassLoader(oldLoader);
                
                // If there a <adminconsole> section defined, register it.
                Element adminElement = (Element) pluginXML.selectSingleNode("/plugin/adminconsole");
                if (adminElement != null) {
                    Element appName = (Element) adminElement.selectSingleNode("/plugin/adminconsole/global/appname");
                    if (appName != null) {
                        // Set the plugin name so that the proper i18n String can be loaded.
                        appName.addAttribute("plugin", pluginName);
                    }
                    // If global images are specified, override their URL.
                    Element imageEl = (Element) adminElement.selectSingleNode("/plugin/adminconsole/global/logo-image");
                    if (imageEl != null) {
                        imageEl.setText("plugins/" + pluginName + "/" + imageEl.getText());
                        // Set the plugin name so that the proper i18n String can be loaded.
                        imageEl.addAttribute("plugin", pluginName);
                    }
                    imageEl = (Element) adminElement.selectSingleNode("/plugin/adminconsole/global/login-image");
                    if (imageEl != null) {
                        imageEl.setText("plugins/" + pluginName + "/" + imageEl.getText());
                        // Set the plugin name so that the proper i18n String can be loaded.
                        imageEl.addAttribute("plugin", pluginName);
                    }
                    // Modify all the URL's in the XML so that they are passed through
                    // the plugin servlet correctly.
                    List urls = adminElement.selectNodes("//@url");
                    for (Object url : urls) {
                        Attribute attr = (Attribute) url;
                        attr.setValue("plugins/" + pluginName + "/" + attr.getValue());
                    }
                    // In order to internationalize the names and descriptions in the model,
                    // we add a "plugin" attribute to each tab, sidebar, and item so that
                    // the the renderer knows where to load the i18n Strings from.
                    String[] elementNames = new String[] { "tab", "sidebar", "item" };
                    for (String elementName : elementNames) {
                        List values = adminElement.selectNodes("//" + elementName);
                        for (Object value : values) {
                            Element element = (Element) value;
                            // Make sure there's a name or description. Otherwise, no need to
                            // override i18n settings.
                            if (element.attribute("name") != null || element.attribute("value") != null) {
                                element.addAttribute("plugin", pluginName);
                            }
                        }
                    }
                    
                    AdminConsole.addModel(pluginName, adminElement);
                }
                firePluginCreatedEvent(pluginName, plugin);
            } else {
                MRLog.warn("Plugin " + pluginDir + " could not be loaded: no plugin.xml file found");
            }
        } catch (Throwable e) {
            MRLog.error("Error loading plugin: " + pluginDir, e);
        }
    }
    
    private void configureCaches(File pluginDir, String pluginName) {
    //        File cacheConfig = new File(pluginDir, "cache-config.xml");
    //        if (cacheConfig.exists()) {
    //            PluginCacheConfigurator configurator = new PluginCacheConfigurator();
    //            try {
    //                configurator.setInputStream(new BufferedInputStream(new FileInputStream(cacheConfig)));
    //                configurator.configure(pluginName);
    //            }
    //            catch (Exception ex) {
    //                MRLog.error(ex);
    //            }
    //        }
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
            // Remove from dev mode if it exists.
            pluginDevelopment.remove(plugin);
            
            // See if any child plugins are defined.
            if (parentPluginMap.containsKey(plugin)) {
                String[] childPlugins = parentPluginMap.get(plugin).toArray(new String[parentPluginMap.get(plugin).size()]);
                parentPluginMap.remove(plugin);
                for (String childPlugin : childPlugins) {
                    MRLog.debug("Unloading child plugin: " + childPlugin);
                    childPluginMap.remove(plugins.get(childPlugin));
                    unloadPlugin(childPlugin);
                }
            }
            
            File webXML = new File(pluginDirectory, pluginName + File.separator + "web" + File.separator + "WEB-INF" + File.separator + "web.xml");
            if (webXML.exists()) {
                AdminConsole.removeModel(pluginName);
                PluginServlet.unregisterServlets(webXML);
            }
            File customWebXML = new File(pluginDirectory, pluginName + File.separator + "web" + File.separator + "WEB-INF" + File.separator + "web-custom.xml");
            if (customWebXML.exists()) {
                PluginServlet.unregisterServlets(customWebXML);
            }
            
            // Wrap destroying the plugin in a try/catch block. Otherwise, an exception raised
            // in the destroy plugin process will disrupt the whole unloading process. It's still
            // possible that classloader destruction won't work in the case that destroying the plugin
            // fails. In that case, Openfire may need to be restarted to fully cleanup the plugin
            // resources.
            try {
                plugin.destroyPlugin();
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        
        // Remove references to the plugin so it can be unloaded from memory
        // If plugin still fails to be removed then we will add references back
        // Anyway, for a few seconds admins may not see the plugin in the admin console
        // and in a subsequent refresh it will appear if failed to be removed
        plugins.remove(pluginName);
        File pluginFile = pluginDirs.remove(plugin);
        IpvsServClassLoader pluginLoader = classloaders.remove(plugin);
        
        // Try to remove the folder where the plugin was exploded. If this works then
        // the plugin was successfully removed. Otherwise, some objects created by the
        // plugin are still in memory.
        File dir = new File(pluginDirectory, pluginName);
        // Give the plugin 2 seconds to unload.
        try {
            Thread.sleep(2000);
            // Ask the system to clean up references.
            System.gc();
            int count = 0;
            while (!deleteDir(dir) && count < 5) {
                MRLog.warn("Error unloading plugin " + pluginName + ". " + "Will attempt again momentarily.");
                Thread.sleep(8000);
                count++;
                // Ask the system to clean up references.
                System.gc();
            }
        } catch (InterruptedException ex) {
            MRLog.error(ex);
        }
        
        if (plugin != null && !dir.exists()) {
            // Unregister plugin caches
            //            PluginCacheRegistry.getInstance().unregisterCaches(pluginName);
            
            // See if this is a child plugin. If it is, we should unload
            // the parent plugin as well.
            if (childPluginMap.containsKey(plugin)) {
                String parentPluginName = childPluginMap.get(plugin);
                Plugin parentPlugin = plugins.get(parentPluginName);
                List<String> childrenPlugins = parentPluginMap.get(parentPlugin);
                
                childrenPlugins.remove(pluginName);
                childPluginMap.remove(plugin);
                
                // When the parent plugin implements PluginListener, its pluginDestroyed() method
                // isn't called if it dies first before its child. Athough the parent will die anyway,
                // it's proper if the parent "gets informed first" about the dying child when the
                // child is the one being killed first.
                if (parentPlugin instanceof PluginListener) {
                    PluginListener listener;
                    listener = (PluginListener) parentPlugin;
                    listener.pluginDestroyed(pluginName, plugin);
                }
                unloadPlugin(parentPluginName);
            }
            firePluginDestroyedEvent(pluginName, plugin);
        } else if (plugin != null) {
            // Restore references since we failed to remove the plugin
            plugins.put(pluginName, plugin);
            pluginDirs.put(plugin, pluginFile);
            classloaders.put(plugin, pluginLoader);
        }
    }
    
    private void firePluginDestroyedEvent(String name, Plugin plugin) {
        for (PluginListener listener : pluginListeners) {
            listener.pluginDestroyed(name, plugin);
        }
    }
    
    public Class loadClass(Plugin plugin, String className) throws ClassNotFoundException, IllegalAccessException, InstantiationException {
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
    
    private boolean deleteDir(File dir) {
        if (dir.isDirectory()) {
            String[] childDirs = dir.list();
            // Always try to delete JAR files first since that's what will
            // be under contention. We do this by always sorting the lib directory
            // first.
            List<String> children = new ArrayList<String>(Arrays.asList(childDirs));
            Collections.sort(children, new Comparator<String>() {
                public int compare(String o1, String o2) {
                    if (o1.equals("lib")) {
                        return -1;
                    }
                    if (o2.equals("lib")) {
                        return 1;
                    } else {
                        return o1.compareTo(o2);
                    }
                }
            });
            for (String file : children) {
                boolean success = deleteDir(new File(dir, file));
                if (!success) {
                    MRLog.debug("PluginManager: Plugin removal: could not delete: " + new File(dir, file));
                    return false;
                }
            }
        }
        boolean deleted = !dir.exists() || dir.delete();
        if (deleted) {
            // Remove the JAR/WAR file that created the plugin folder
            pluginFiles.remove(dir.getName());
        }
        return deleted;
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
        // If the task is already running, return.
        try {
            // Look for extra plugin directories specified as a system property.
            String pluginDirs = System.getProperty("pluginDirs");
            if (pluginDirs != null) {
                StringTokenizer st = new StringTokenizer(pluginDirs, ", ");
                while (st.hasMoreTokens()) {
                    String dir = st.nextToken();
                    if (!devPlugins.contains(dir)) {
                        loadPlugin(new File(dir));
                        devPlugins.add(dir);
                    }
                }
            }
            
            File[] jars = pluginDirectory.listFiles(new FileFilter() {
                public boolean accept(File pathname) {
                    String fileName = pathname.getName().toLowerCase();
                    return (fileName.endsWith(".jar") || fileName.endsWith(".war"));
                }
            });
            
            if (jars == null) {
                return;
            }
            
            for (File jarFile : jars) {
                String pluginName = jarFile.getName().substring(0, jarFile.getName().length() - 4).toLowerCase();
                // See if the JAR has already been exploded.
                File dir = new File(pluginDirectory, pluginName);
                // Store the JAR/WAR file that created the plugin folder
                pluginFiles.put(pluginName, jarFile);
                // If the JAR hasn't been exploded, do so.
                if (!dir.exists()) {
                    unzipPlugin(pluginName, jarFile, dir);
                }
                // See if the JAR is newer than the directory. If so, the plugin
                // needs to be unloaded and then reloaded.
                else if (jarFile.lastModified() > dir.lastModified()) {
                    // If this is the first time that the monitor process is running, then
                    // plugins won't be loaded yet. Therefore, just delete the directory.
                    if (firstRun) {
                        int count = 0;
                        // Attempt to delete the folder for up to 5 seconds.
                        while (!deleteDir(dir) && count < 5) {
                            Thread.sleep(1000);
                        }
                    } else {
                        unloadPlugin(pluginName);
                    }
                    // If the delete operation was a success, unzip the plugin.
                    if (!dir.exists()) {
                        unzipPlugin(pluginName, jarFile, dir);
                    }
                }
            }
            
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
            
            // Turn the list of JAR/WAR files into a set so that we can do lookups.
            Set<String> jarSet = new HashSet<String>();
            for (File file : jars) {
                jarSet.add(file.getName().toLowerCase());
            }
            
            // See if any currently running plugins need to be unloaded
            // due to the JAR file being deleted (ignore admin plugin).
            // Build a list of plugins to delete first so that the plugins
            // keyset isn't modified as we're iterating through it.
            List<String> toDelete = new ArrayList<String>();
            for (File pluginDir : dirs) {
                String pluginName = pluginDir.getName();
                if (pluginName.equals("admin")) {
                    continue;
                }
                if (!jarSet.contains(pluginName + ".jar")) {
                    if (!jarSet.contains(pluginName + ".war")) {
                        toDelete.add(pluginName);
                    }
                }
            }
            for (String pluginName : toDelete) {
                unloadPlugin(pluginName);
            }
            
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
        // Process finished, so set firstRun to false (setting it multiple times doesn't hurt).
        firstRun = false;
    }
    
    private void unzipPlugin(String pluginName, File file, File dir) {
        try {
            ZipFile zipFile = new JarFile(file);
            // Ensure that this JAR is a plugin.
            if (zipFile.getEntry("plugin.xml") == null) {
                return;
            }
            dir.mkdir();
            // Set the date of the JAR file to the newly created folder
            dir.setLastModified(file.lastModified());
            MRLog.debug("PluginManager: Extracting plugin: " + pluginName);
            for (Enumeration e = zipFile.entries(); e.hasMoreElements();) {
                JarEntry entry = (JarEntry) e.nextElement();
                File entryFile = new File(dir, entry.getName());
                // Ignore any manifest.mf entries.
                if (entry.getName().toLowerCase().endsWith("manifest.mf")) {
                    continue;
                }
                if (!entry.isDirectory()) {
                    entryFile.getParentFile().mkdirs();
                    FileOutputStream out = new FileOutputStream(entryFile);
                    InputStream zin = zipFile.getInputStream(entry);
                    byte[] b = new byte[512];
                    int len;
                    while ((len = zin.read(b)) != -1) {
                        out.write(b, 0, len);
                    }
                    out.flush();
                    out.close();
                    zin.close();
                }
            }
            zipFile.close();
            
            // The lib directory of the plugin may contain Pack200 versions of the JAR
            // file. If so, unpack them.
            unpackArchives(new File(dir, "lib"));
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void unpackArchives(File libDir) {
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
                // Call the unpacker
                unpacker.unpack(in, out);
                
                in.close();
                out.close();
                packedFile.delete();
            } catch (Exception e) {
                MRLog.error(e);
            }
        }
    }
    
}
