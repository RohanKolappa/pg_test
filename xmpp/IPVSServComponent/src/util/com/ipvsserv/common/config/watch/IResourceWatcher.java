package com.ipvsserv.common.config.watch;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
  */
public interface IResourceWatcher {
    
    public void start();
    
    public void addListener(IResourceListener listener);
    
    public void removeListener(IResourceListener listener);
    
    public void stop();
}
