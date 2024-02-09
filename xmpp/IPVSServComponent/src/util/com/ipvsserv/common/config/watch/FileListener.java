package com.ipvsserv.common.config.watch;

import java.io.File;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class FileListener extends BaseListener implements IFileListener {
    
    public FileListener() {
        super();
    }
    
    public void onStart(Object monitoredResource) {
        // On startup
        if (monitoredResource instanceof File) {
            File resource = (File) monitoredResource;
            if (resource.isDirectory()) {
                //MRLog.debug("Start to monitor " + resource.getAbsolutePath());
                /*File[] files = resource.listFiles();
                for (int i = 0; i < files.length; i++) {
                    File f = (File) files[i];
                    onAdd(f);
                }*/
            }
        }
    }
    
    public void onStop(Object notMonitoredResource) {

    }
    
    public void onAdd(Object newResource) {
        if (newResource instanceof File) {
            File file = (File) newResource;
            if (file.isFile()) {
                //MRLog.debug(file.getAbsolutePath() + " is added");
            }
        }
    }
    
    public void onChange(Object changedResource) {
        if (changedResource instanceof File) {
            File file = (File) changedResource;
            if (file.isFile()) {
                // MRLog.debug(file.getAbsolutePath() + " is changed");
            }
            
        }
    }
    
    public void onDelete(Object deletedResource) {
        if (deletedResource instanceof String) {
            String deletedFile = (String) deletedResource;
            // MRLog.debug(deletedFile + " is deleted");
        }
    }
}
