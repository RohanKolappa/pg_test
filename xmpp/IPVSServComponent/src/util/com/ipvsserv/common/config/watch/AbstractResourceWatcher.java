package com.ipvsserv.common.config.watch;

import java.util.Collection;
import java.util.Iterator;
import java.util.LinkedList;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public abstract class AbstractResourceWatcher extends IntervalThread implements IResourceWatcher {
    
    private Collection<IResourceListener> listeners = new LinkedList<IResourceListener>();
    
    public AbstractResourceWatcher(int intervalSeconds, String name) {
        
        //Get the common thread interval stuff set up.
        super(intervalSeconds, name);
    }
    
    public void removeAllListeners() {
        
        //Clear the list of all listeners
        listeners.clear();
    }
    
    public void addListener(IResourceListener listener) {
        
        //And add the listener
        listeners.add(listener);
    }
    
    public void removeListener(IResourceListener listener) {
        
        //Iterate through the listeners and remove the this listener
        listeners.remove(listener);
    }
    
    protected void resourceAdded(Object newResource) {
        
        //Iterate through the listeners and fire the onAdd method
        Iterator<IResourceListener> listIt = listeners.iterator();
        
        while (listIt.hasNext()) {
            (  listIt.next()).onAdd(newResource);
        }
    }
    
    protected void resourceChanged(Object changedResource) {
        
        //Iterate through the listeners and fire the onChange method
        Iterator<IResourceListener> listIt = listeners.iterator();
        
        while (listIt.hasNext()) {
            (  listIt.next()).onChange(changedResource);
        }
    }
    
    protected void resourceDeleted(Object deletedResource) {
        
        //Iterate through the listeners and fire the onDelete method
        Iterator<IResourceListener> listIt = listeners.iterator();
        
        while (listIt.hasNext()) {
            (  listIt.next()).onDelete(deletedResource);
        }
    }
    
    protected void monitoringStarted(Object monitoredResource) {
        
        //Iterate through the listeners and fire the onStart method
        Iterator<IResourceListener> listIt = listeners.iterator();
        
        while (listIt.hasNext()) {
            (  listIt.next()).onStart(monitoredResource);
        }
    }
    
    protected void monitoringStopped(Object notMonitoredResource) {
        
        //Iterate through the listeners and fire the onStop method
        Iterator<IResourceListener> listIt = listeners.iterator();
        
        while (listIt.hasNext()) {
            ( listIt.next()).onStop(notMonitoredResource);
        }
    }
    
    @Override
    protected abstract void doInterval();
}
