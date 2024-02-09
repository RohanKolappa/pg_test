package com.ipvsserv.server;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.List;
import java.util.Map;
import java.util.concurrent.ScheduledExecutorService;
import java.util.concurrent.ScheduledThreadPoolExecutor;
import java.util.concurrent.TimeUnit;

import com.ipvsserv.common.log.MRLog;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */

public abstract class IpvsServBasicModule implements IpvsServModule {
    protected String name;
    protected Map<String, String> params = null;
    
    public static final int HEALTHY = 1;
    public static final int UNHEALTHY = 2;
    
    //state
    protected int state = 0;
    protected boolean initialized = false;
    protected boolean started = false;
    
    protected boolean shutdownInProgress = false;
    
    protected boolean checkStateInProgress = false;
    protected Runnable stateMonitor = new StateMonitor();
    
    protected ScheduledExecutorService executor = null;
    protected String ipvsModuleName = "";
    protected int modulePriority = 10;
    protected boolean moduleEnabled = false;
    
    protected long initialDelay = 0;
    protected long delay = 5;
    protected TimeUnit unit = TimeUnit.SECONDS;
    protected boolean executed = false;
    protected Object objectSync = new Object();
    
    private List<ProcessingDoneEventListener> listeners = new ArrayList<ProcessingDoneEventListener>();
    protected boolean completBeforeStartNext = false;
    private String ipvsRootModuleName=null;
    private IpvsServModule parent=null;


    public IpvsServBasicModule(String moduleName) {
       this(moduleName, null, null);
    }
    public IpvsServBasicModule(String moduleName, IpvsServModule parent) {
        this(moduleName, null, parent);
    }
    public IpvsServBasicModule(String moduleName, String ipvsRootModuleName, IpvsServModule parent) {
        if (moduleName == null) {
            this.name = "No name assigned";
        } else {
            this.name = moduleName;
        }
        this.ipvsRootModuleName=ipvsRootModuleName;
        this.parent=parent;
        
    }
    ////////////////// implememented by child classes///////////////////
    public void initialize(Map<String, String> params) throws Exception {
        this.params = params;
        //initialized=false;
        shutdownInProgress = false;
    }
    
    public void stop() throws Exception {
        destroy();
    }
    
    public void destroy() throws Exception {
        if(shutdownInProgress)
            return ;
        shutdownInProgress = true;
        stopMonitor();
        stateMonitor = null;
        started = false;
        raiseProcessingDoneEvent();
    }
    
    public void raiseProcessingDoneEvent() {
        synchronized (listeners) {
            if (listeners != null && listeners.size() > 0) {
                for (Iterator<ProcessingDoneEventListener> iterator = listeners.iterator(); iterator.hasNext();) {
                    ProcessingDoneEventListener listener = iterator.next();
                    listener.processingDoneEvent(this);
                }
            }
        }
    }
    
    public abstract void start() throws Exception;
    
    public abstract int getLastKnownState() throws Exception;
    
    public abstract void verifyState() throws Exception;
    
    //////////////////implememented by child classes///////////////////
    public String getName() {
        return name;
    }
    
    public String getProperty(String key, String sdefault) {
        String value = params.get(key);
        if (value == null) {
            return sdefault;
        }
        return value;
    }
    
    public boolean isExecuted() {
        return executed;
    }
    
    public void setExecuted(boolean executed) {
        this.executed = executed;
    }
    
    public void addProcessingDoneEventListener(ProcessingDoneEventListener listner) {
        synchronized (listeners) {
            listeners.add(listner);
        }
    }
    
    public void removeProcessingDoneEventListener(ProcessingDoneEventListener listner) {
        synchronized (listeners) {
            listeners.remove(listner);
        }
    }
    
    public Runnable getStateMonitor() {
        return stateMonitor;
    }
    
    public void setStateMonitor(Runnable stateMonitor) {
        this.stateMonitor = stateMonitor;
    }
    
    public String getIpvsModuleName() {
        return ipvsModuleName;
    }
    
    public void setIpvsModuleName(String ipvsModuleName) {
        this.ipvsModuleName = ipvsModuleName;
    }
    
    public int getModulePriority() {
        return modulePriority;
    }
    
    public void setModulePriority(int modulePriority) {
        this.modulePriority = modulePriority;
    }
    
    public boolean isModuleEnabled() {
        return moduleEnabled;
    }
    
    public void setModuleEnabled(boolean moduleEnabled) {
        this.moduleEnabled = moduleEnabled;
    }
    
    public void setCompletBeforeStartNext(boolean completBeforeStartNext) {
        this.completBeforeStartNext = completBeforeStartNext;
    }
    
    public boolean isInitialized() {
        return initialized;
    }
    
    public boolean isStarted() {
        synchronized (objectSync) {
            return started;
        }
    }
    
    public boolean isShutdownInProgress() {
        return shutdownInProgress;
    }
    
    public void setStarted(boolean started) {
        this.started = started;
    }
    
    //////////////// StateMonitor ////////////////////////////////////////////////
    protected void startMonitor() {
        startMonitor(stateMonitor, initialDelay, delay, unit);
    }
    
    protected void restartMonitor() {
        stopMonitor();
        startMonitor(stateMonitor, initialDelay, delay, unit);
    }
    
    protected void startMonitor(Runnable command, long initialDelay, long delay, TimeUnit unit) {
        this.stateMonitor = command;
        this.initialDelay = initialDelay;
        this.delay = delay;
        this.unit = unit;
        executor = new ScheduledThreadPoolExecutor(1);
        
        // See if we're in development mode. If so, check for new plugins once every 5 seconds.
        // Otherwise, default to every 20 seconds.
        if (Boolean.getBoolean("developmentMode")) {
            executor.scheduleWithFixedDelay(stateMonitor, initialDelay, delay, TimeUnit.SECONDS);
        } else {
            executor.scheduleWithFixedDelay(stateMonitor, initialDelay, delay, TimeUnit.SECONDS);
        }
    }
    
    protected void stopMonitor() {
        if (executor != null) {
            executor.shutdown();
        }
        executor = null;
    }
    
    public class StateMonitor implements Runnable {
        boolean running = false;
        
        boolean firstRun = true;
        
        public boolean isExecuted() {
            return executed;
        }
        
        @Override
        public void run() {
            // If the task is already running, return.
            synchronized (this) {
                if (running) {
                    return;
                }
                running = true;
            }
            try {
                running = true;
                if (!shutdownInProgress) {
                    try {
                        verifyState();
                    } catch (Exception ex) {
                        MRLog.debug(ex.getMessage());
                    }
                    // Set that at least one iteration was done. That means that "all available" plugins
                    // have been loaded by now.
                    if (!IpvsServer.getInstance().isSetupMode()) {
                        executed = true;
                    }
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
            // Finished running task.
            synchronized (this) {
                running = false;
            }
            // Process finished, so set firstRun to false (setting it multiple times doesn't hurt).
            firstRun = false;
        }
    }
    //////////////// StateMonitor ////////////////////////////////////////////////
    public String getRootIpvsModuleName(){
        return ipvsRootModuleName;
    }
    public void setRootIpvsModuleName(String ipvsRootModuleName){
        this.ipvsRootModuleName=ipvsRootModuleName;
    }
    public void setParent(IpvsServModule parent){
        this.parent=parent;
    }
    public IpvsServModule getParent(){
        return parent;
    }

}
