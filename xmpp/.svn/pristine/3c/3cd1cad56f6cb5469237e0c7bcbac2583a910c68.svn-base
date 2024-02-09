package com.ipvsserv.nxd.dbtask.test;

import java.util.ArrayList;
import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.server.IpvsServModule;
import com.ipvsserv.server.ProcessingDoneEventListener;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTestActions implements ProcessingDoneEventListener {
    private Map<String, IpvsServModule> ldbTestActionsByName = new LinkedHashMap<String, IpvsServModule>();
    private Map<String, Boolean> processingList = new LinkedHashMap<String, Boolean>();
    private String name = null;
    private int priorityLevels = 10;
    private ClassLoader loader;
    private boolean shutdown = false;
    private Map<String, String> params = null;
    private boolean completBeforeStartNext;
    private int priority;
    private boolean enable;
    private String xpath;
    private List<ProcessingDoneEventListener> listeners = new ArrayList<ProcessingDoneEventListener>();
    private int timeout = 100 * 60 * 1000;
    private int waitTime = 5 * 1000;
    
    public DBTestActions(ClassLoader loader, String name, Map<String, String> params) {
        this.loader = loader;
        this.params = params;
        this.name = name;
        completBeforeStartNext = CompUtils.booleanValue(params.get("completBeforeStartNext"), false);
        priority = CompUtils.intValue(params.get("priority"), priorityLevels);
        enable = CompUtils.booleanValue(params.get("enable"), false);
        xpath = "//DBTestActions[@name='" + name + "']";
        
    }
    
    public Map<String, String> getParams() {
        return params;
    }
    
    public String getName() {
        return name;
    }
    
    public boolean isCompletBeforeStartNext() {
        return completBeforeStartNext;
    }
    
    public int getPriority() {
        return priority;
    }
    
    public boolean isEnable() {
        return enable;
    }
    
    public String getXpath() {
        return xpath;
    }
    
    public boolean isShutdown() {
        return shutdown;
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
    
    public void start() {
        try {
            Map<String, Map<String, String>> testActions = TestGlobals.getModulesXpath("DBTestActionsSequence.priorityLevels", xpath, "DBTestAction");
            loadModules(testActions);
            initModules(testActions);
            startModules(completBeforeStartNext);
            if (completBeforeStartNext) {
                waitForResult();
            }
            stopModules();
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    private void waitForResult() {
        long timeWaiting = System.currentTimeMillis();
        boolean timedout = false;
        synchronized (this) {
            while (true) {
                try {
                    if (System.currentTimeMillis() - timeWaiting > timeout) {
                        timedout = true;
                        break;
                    }
                    wait(waitTime);
                    if (processingList.size() == 0) {
                        break;
                    }
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
            if (timedout) {
                processingList.clear();
            }
        }
    }
    
    public void stop() {
        stopModules();
    }
    
    private boolean isProcessginDone() {
        synchronized (processingList) {
            if (processingList.size() == 0) {
                return true;
            }
        }
        return false;
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
    
    //////////////////////////
    private void loadModules(Map<String, Map<String, String>> testActions) {
        for (int i = 1; i < priorityLevels; i++) {
            loadModules(testActions, i);
        }
    }
    
    private void loadModules(Map<String, Map<String, String>> testActions, int priority) {
        //dbTestActionParams.keySet().iterator()
        for (String moduleName : testActions.keySet()) {
            Map<String, String> params = testActions.get(moduleName);
            String moduleClass = params.get("class");
            int modulePriority = CompUtils.intValue(params.get("priority"), 10);
            String name = params.get("name");
            boolean moduleEnabled = CompUtils.booleanValue(params.get("enable"), false);
            boolean completBeforeStartNext = CompUtils.booleanValue(params.get("completBeforeStartNext"), false);
            if (priority == modulePriority) {
                //MRLog.info(moduleName + " loading " + moduleClass + " modulePriority " + modulePriority + " " + moduleEnabled);
                loadModule(moduleClass, moduleName, modulePriority, moduleEnabled, completBeforeStartNext);
            }
        }
    }
    
    private void loadModule(String module, String ipvsModuleName, int modulePriority, boolean moduleEnabled, boolean completBeforeStartNext) {
        try {
            Class modClass = loader.loadClass(module);
            IpvsServModule mod = (IpvsServModule) modClass.newInstance();
            //todo: support same class with multiple dbTestActions
            mod.setIpvsModuleName(ipvsModuleName);
            mod.setModulePriority(modulePriority);
            mod.setModuleEnabled(moduleEnabled);
            mod.setCompletBeforeStartNext(completBeforeStartNext);
            this.ldbTestActionsByName.put(ipvsModuleName, mod);
        } catch (Exception ex) {
            MRLog.error("loadModule ", ex);
        }
    }
    
    ////////////////////////
    
    @Override
    public void processingDoneEvent(Object event) {
        synchronized (processingList) {
            IpvsServModule module = (IpvsServModule) event;
            processingList.remove(module.getIpvsModuleName());
        }
        if (processingList.size() == 0) {
            synchronized (this) {
                if (processingList.size() == 0) notifyAll();
            }
        }
    }
    
    /////////////////////////initModule/////////////////////////
    private void initModules(Map<String, Map<String, String>> testActions) {
        for (int i = 1; i < priorityLevels; i++) {
            initModules(testActions, i);
        }
    }
    
    private void initModules(Map<String, Map<String, String>> testActions, int priority) {
        for (String moduleName : ldbTestActionsByName.keySet()) {
            IpvsServModule module = ldbTestActionsByName.get(moduleName);
            boolean isInitialized = false;
            try {
                if (priority == module.getModulePriority()) {
                    if (module.isModuleEnabled()) {
                        //MRLog.info("initialize " + moduleName + " " + module.getClass().getName());
                        //module.initialize( IpvsServGlobals.getModuleParams( module.getIpvsModuleName() ) );
                        module.initialize(testActions.get(moduleName));
                        isInitialized = true;
                    } else {
                        MRLog.info("skip initialize- not enabled. " + module.getClass().getName());
                    }
                }
            } catch (Exception ex) {
                
                // Remove the failed initialized module
                this.ldbTestActionsByName.remove(moduleName);
                if (isInitialized) {
                    try {
                        module.stop();
                        module.destroy();
                    } catch (Exception ex2) {
                        MRLog.error(ex2);
                    }
                }
                MRLog.error("loadModule ", ex);
            }
        }
    }
    
    /////////////////////////startModules/////////////////////////
    private void startModules(boolean completBeforeStartNext) {
        for (int i = 1; i < priorityLevels; i++) {
            startModules(i, completBeforeStartNext);
        }
    }
    
    private void startModules(int priority, boolean completBeforeStartNext) {
        for (String moduleName : ldbTestActionsByName.keySet()) {
            IpvsServModule module = ldbTestActionsByName.get(moduleName);
            boolean started = false;
            try {
                if (priority == module.getModulePriority()) {
                    if (module.isModuleEnabled()) {
                        //MRLog.info("start " + module.getClass().getName());
                        if (completBeforeStartNext) {
                            synchronized (processingList) {
                                processingList.put(module.getIpvsModuleName(), false);
                                module.addProcessingDoneEventListener(this);
                            }
                        }
                        module.start();
                        started = true;
                    } else {
                        MRLog.info("skip start- not enabled. " + module.getClass().getName());
                    }
                }
            } catch (Exception ex) {
                if (started && module != null) {
                    try {
                        if (completBeforeStartNext) {
                            synchronized (processingList) {
                                module.removeProcessingDoneEventListener(this);
                                processingList.remove(module.getIpvsModuleName());
                            }
                        }
                        module.stop();
                        module.destroy();
                    } catch (Exception ex2) {
                        MRLog.error(ex2);
                    }
                }
                MRLog.error("loadModule ", ex);
            }
        }
    }
    
    /////////////////////////startModules/////////////////////////
    private void stopModules() {
        shutdown = true;
        raiseProcessingDoneEvent();
        // Get all dbTestActions and stop and destroy them
        for (int i = priorityLevels; i > 0; i--) {
            stopModules(i);
        }
        //MRLog.debug("stopModules " + name);
    }
    
    private void stopModules(int priority) {
        for (String moduleName : ldbTestActionsByName.keySet()) {
            IpvsServModule module = ldbTestActionsByName.get(moduleName);
            //boolean stopped = false;
            try {
                if (priority == module.getModulePriority()) {
                    if (module.isModuleEnabled()) {
                        //MRLog.info("start " + module.getClass().getName());
                        try {
                            module.stop();
                        } catch (Exception ex) {
                            ex.printStackTrace();
                        }
                        try {
                            module.destroy();
                        } catch (Exception ex) {
                            ex.printStackTrace();
                        }
                        //stopped=true;
                    } else {
                        MRLog.info("skip stop- not enabled. " + module.getClass().getName());
                    }
                }
            } catch (Exception ex) {
                MRLog.error("stopModules", ex);
            }
        }
    }
}
