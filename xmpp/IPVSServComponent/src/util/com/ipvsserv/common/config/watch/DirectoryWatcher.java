package com.ipvsserv.common.config.watch;

import java.io.File;
import java.util.HashMap;
import java.util.Iterator;
import java.util.Map;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DirectoryWatcher extends AbstractResourceWatcher {
    
    private Map<String, Long> currentFiles = new HashMap<String, Long>();
    
    private String directory;
    
    private Map<String, Long> prevFiles = new HashMap<String, Long>();
    
    public DirectoryWatcher(String directoryPath, int intervalSeconds) throws IllegalArgumentException {
        
        //Get the common thread interval stuff set up.
        super(intervalSeconds, directoryPath + " interval watcher.");
        
        //Check that it is indeed a directory.
        File theDirectory = new File(directoryPath);
        
        if (theDirectory != null && !theDirectory.isDirectory()) {
            
            //This is bad, so let the caller know
            String message = "The path " + directory + " does not represent a valid directory.";
            throw new IllegalArgumentException(message);
            
        }
        
        //Else all is well so set this directory and the interval
        this.directory = directoryPath;
        
    }
    
    public static void main(String[] args) {
        // Monitor c:/temp every 5 seconds
        DirectoryWatcher dw = new DirectoryWatcher("c:/temp/", 5);
        dw.addListener(new FileListener());
        dw.start();
    }
    
    @Override
    public void start() {
        
        //Since we're going to start monitoring, we want to take a snapshot of the
        //current directory to we have something to refer to when stuff changes.
        takeSnapshot();
        
        //And start the thread on the given interval
        super.start();
        
        //And notify the listeners that monitoring has started
        File theDirectory = new File(directory);
        monitoringStarted(theDirectory);
    }
    
    @Override
    public void stop() {
        
        //And start the thread on the given interval
        super.stop();
        
        //And notify the listeners that monitoring has started
        File theDirectory = new File(directory);
        monitoringStopped(theDirectory);
    }
    
    private void takeSnapshot() {
        
        //Set the last recorded snap shot to be the current list
        prevFiles.clear();
        prevFiles.putAll(currentFiles);
        
        //And get a new current state with all the files and directories
        currentFiles.clear();
        
        File theDirectory = new File(directory);
        File[] children = theDirectory.listFiles();
        
        //Store all the current files and their timestamps
        for (int i = 0; i < children.length; i++) {
            
            File file = children[i];
            currentFiles.put(file.getAbsolutePath(), new Long(file.lastModified()));
            
        }
        
    }
    
    @Override
    protected void doInterval() {
        
        //Take a snapshot of the current state of the dir for comparisons
        takeSnapshot();
        
        //Iterate through the map of current files and compare
        //them for differences etc...
        Iterator<String> currentIt = currentFiles.keySet().iterator();
        
        while (currentIt.hasNext()) {
            
            String fileName =   currentIt.next();
            Long lastModified =  currentFiles.get(fileName);
            
            //If this file did not exist before, but it does now, then
            //it's been added
            if (!prevFiles.containsKey(fileName)) {
                //DirectorySnapshot.addFile(fileName);
                resourceAdded(new File(fileName));
            }
            //If this file did exist before
            else if (prevFiles.containsKey(fileName)) {
                
                Long prevModified =   prevFiles.get(fileName);
                
                //If this file existed before and has been modified
                if (prevModified.compareTo(lastModified) != 0) {
                    // 27 June 2006
                    // Need to check if the file are removed and added
                    // during the interval
                    /* if (!DirectorySnapshot.containsFile(fileName)) {
                         resourceAdded(new File(fileName));
                     } else {*/
                    resourceChanged(new File(fileName));
                    //}
                }
            }
        }
        
        //Now we need to iterate through the list of previous files and
        //see if any that existed before don't exist anymore
        Iterator<String> prevIt = prevFiles.keySet().iterator();
        
        while (prevIt.hasNext()) {
            
            String fileName = prevIt.next();
            
            //If this file did exist before, but it does not now, then
            //it's been deleted
            if (!currentFiles.containsKey(fileName)) {
                //DirectorySnapshot.removeFile(fileName);
                resourceDeleted(fileName);
            }
        }
    }
}
