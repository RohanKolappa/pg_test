package com.ipvsserv.plugin;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface PluginListener {
    void pluginCreated(String pluginName, Plugin plugin);
    
    void pluginDestroyed(String pluginName, Plugin plugin);
}
