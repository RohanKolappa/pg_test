package com.ipvsserv.common.config.watch;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public interface IResourceListener {
    
    public void onStart(Object monitoredResource);
    
    public void onStop(Object notMonitoredResource);
    
    public void onAdd(Object newResource);
    
    public void onChange(Object changedResource);
    
    public void onDelete(Object deletedResource);
}
