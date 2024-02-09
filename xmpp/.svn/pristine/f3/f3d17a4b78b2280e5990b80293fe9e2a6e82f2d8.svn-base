package com.ipvsserv.plugin;

import java.io.File;
import java.io.InputStream;
import java.util.Collection;

import com.ipvsserv.plugin.PluginLicence.License;
import com.ipvsserv.starter.IpvsServClassLoader;

public interface PluginManager {
    
    public abstract boolean installPlugin(InputStream in, String pluginFilename);
    
    public abstract boolean isPluginDownloaded(String pluginFilename);
    
    public abstract Collection<Plugin> getPlugins();
    
    public abstract Plugin getPlugin(String name);
    
    public abstract File getPluginDirectory(Plugin plugin);
    
    public abstract File getPluginFile(String name);
    
    public abstract void unloadPlugin(String pluginName);
    
    public abstract Class loadClass(Plugin plugin, String className) throws ClassNotFoundException, IllegalAccessException, InstantiationException;
    
    public abstract PluginDevEnvironment getDevEnvironment(Plugin plugin);
    
    public abstract String getName(Plugin plugin);
    
    public abstract String getDescription(Plugin plugin);
    
    public abstract String getAuthor(Plugin plugin);
    
    public abstract String getVersion(Plugin plugin);
    
    public abstract String getMinServerVersion(Plugin plugin);
    
    public abstract String getDatabaseKey(Plugin plugin);
    
    public abstract int getDatabaseVersion(Plugin plugin);
    
    public abstract License getLicense(Plugin plugin);
    
    public abstract IpvsServClassLoader getPluginClassloader(Plugin plugin);
    
    public abstract void addPluginListener(PluginListener listener);
    
    public abstract void removePluginListener(PluginListener listener);
    
    public abstract void addPluginManagerListener(PluginManagerListener listener);
    
    public abstract void removePluginManagerListener(PluginManagerListener listener);
    
    public abstract void pluginMonitor();
    
}
