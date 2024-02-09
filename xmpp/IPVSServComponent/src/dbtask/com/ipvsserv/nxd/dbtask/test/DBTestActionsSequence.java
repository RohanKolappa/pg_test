package com.ipvsserv.nxd.dbtask.test;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.server.ProcessingDoneEventListener;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTestActionsSequence implements ProcessingDoneEventListener {
    private Map<String, DBTestActions> ldbTestActionsByName = new LinkedHashMap<String, DBTestActions>();
    private Map<String, Boolean> processingList = new LinkedHashMap<String, Boolean>();
    private int priorityLevels = 10;
    private ClassLoader loader;
    private boolean shutdown = false;
    private Element actionsSequence = null;
    private int timeout = 100 * 60 * 1000;
    private int waitTime = 5 * 1000;
    
    public DBTestActionsSequence(ClassLoader loader) {
        this.loader = loader;
    }
    
    public void start() {
        actionsSequence = TestGlobals.getElementXpath("//DBTestActionsSequence");
        priorityLevels = TestGlobals.getIntProperty("DBTestActionsSequence.priorityLevels", 10);
        List<?> dbTestActionsEL = Dom4jUtils.getList(actionsSequence, "DBTestActions");
        for (Iterator iterDBTestActions = dbTestActionsEL.iterator(); iterDBTestActions.hasNext();) {
            Element EL = (Element) iterDBTestActions.next();
            Map<String, String> params = new LinkedHashMap<String, String>();
            boolean completBeforeStartNext = CompUtils.booleanValue(EL.attributeValue("completBeforeStartNext"), false);
            String actionsName = EL.attributeValue("name");
            int priority = CompUtils.intValue(EL.attributeValue("priority"), priorityLevels);
            boolean enable = CompUtils.booleanValue(EL.attributeValue("enable"), false);
            String xpath = "//DBTestActions[@name='" + actionsName + "']";
            params.put("name", actionsName);
            params.put("completBeforeStartNext", completBeforeStartNext + "");
            params.put("priority", priority + "");
            params.put("enable", enable + "");
            params.put("xpath", xpath);
            if (enable) {
                //MRLog.debug(actionsName + " enable priority " + priority);
                //ldbTestActionsSequence.put(name, params);
                DBTestActions dbTestActions = new DBTestActions(loader, actionsName, params);
                ldbTestActionsByName.put(actionsName, dbTestActions);
            }
        }
        for (int i = 1; i < priorityLevels; i++) {
            for (String key : ldbTestActionsByName.keySet()) {
                //MRLog.debug("process DBTestActions " + key + " " + ldbTestActionsByName.size());
                DBTestActions dbTestActions = ldbTestActionsByName.get(key);
                if (dbTestActions == null) continue;
                if (dbTestActions.getPriority() == i) {
                    //MRLog.debug("process DBTestActions start " + key + " " + ldbTestActionsByName.size());
                    if (dbTestActions.isCompletBeforeStartNext()) {
                        dbTestActions.addProcessingDoneEventListener(this);
                        synchronized (processingList) {
                            processingList.put(dbTestActions.getName(), false);
                        }
                    }
                    dbTestActions.start();
                    if (dbTestActions.isCompletBeforeStartNext()) {
                        waitForResult();
                        dbTestActions.stop();
                        //ldbTestActionsByName.remove(key);
                    }
                }
            }
        }
        
        waitForResult();
        stop();
    }
    
    private void waitForResult() {
        long timeWaiting = System.currentTimeMillis();
        boolean timedout = false;
        while (!isProcessginDone()) {
            synchronized (this) {
                try {
                    if (System.currentTimeMillis() - timeWaiting > timeout) {
                        //MRLog.debug("timted out DBTestActions started waiting to finish " + processingList.size());
                        timedout = true;
                        break;
                    }
                    //MRLog.debug("DBTestActions waiting to finish " + processingList.size());
                    //Thread.sleep(5*1000);
                    wait(waitTime);
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
        }
        if (timedout) {
            synchronized (processingList) {
                processingList.clear();
            }
        }
    }
    
    private boolean isProcessginDone() {
        synchronized (processingList) {
            if (processingList.size() == 0) return true;
        }
        
        //MRLog.debug("DBTestActionsSequence started waiting to finish " + processingList.size());
        return false;
    }
    
    public void stop() {
        shutdown = true;
        for (String key : ldbTestActionsByName.keySet()) {
            DBTestActions dbTestActions = ldbTestActionsByName.get(key);
            if (dbTestActions == null) continue;
            dbTestActions.stop();
            //ldbTestActionsByName.remove(key);
        }
        ldbTestActionsByName.clear();
    }
    
    public boolean isShutdown() {
        return shutdown;
    }
    
    @Override
    public void processingDoneEvent(Object event) {
        synchronized (processingList) {
            DBTestActions module = (DBTestActions) event;
            processingList.remove(module.getName());
            //MRLog.debug("2 DBTestActionsSequence processingDoneEvent " + processingList.size() + " " + module.getName());
        }
        if (isProcessginDone()) {
            synchronized (this) {
                notifyAll();
            }
        }
        
    }
    
}
