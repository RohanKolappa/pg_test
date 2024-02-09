package com.ipvsserv.nxd.migration.task;

import java.util.Iterator;
import java.util.LinkedHashMap;
import java.util.List;
import java.util.Map;

import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.migration.ServiceVersionMigrateImpl;
import com.ipvsserv.server.ProcessingDoneEventListener;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class MigrationTasksSequence implements ProcessingDoneEventListener {

    private Map<String, MigrationTasks> ldbTestActionsByName = new LinkedHashMap<String, MigrationTasks>();
    private Map<String, Boolean> processingList = new LinkedHashMap<String, Boolean>();
    private int priorityLevels = 10;
    private ClassLoader loader;
    private boolean shutdown = false;
    private Element actionsSequence = null;
    private int timeout = 100 * 60 * 1000;
    private int waitTime = 5 * 1000;
    private ServiceVersionMigrateImpl root = null;
    

    public MigrationTasksSequence(ServiceVersionMigrateImpl migrateServiceVersion, ClassLoader loader) {
        this.loader = loader;
        this.root = migrateServiceVersion;
    }

    private void log(String msg) {
        root.getMigrationModule().debug("TasksSequence " + msg);
    }

    public void start() {
        actionsSequence = root.getSettings().getElementXpath("//DBMigrationTaskSequence");
        priorityLevels = root.getSettings().getIntProperty("DBMigrationTaskSequence.priorityLevels", 10);
        List<?> dbTestActionsEL = Dom4jUtils.getList(actionsSequence, "DBMigrationTasks");
        for (Iterator<?> iterDBTestActions = dbTestActionsEL.iterator(); iterDBTestActions.hasNext();) {
            Element EL = (Element) iterDBTestActions.next();
            Map<String, String> params = new LinkedHashMap<String, String>();
            boolean completBeforeStartNext = CompUtils.booleanValue(EL.attributeValue("completBeforeStartNext"), true);
            String migrationTasksName = EL.attributeValue("name");
            int priority = CompUtils.intValue(EL.attributeValue("priority"), priorityLevels);
            boolean enable = CompUtils.booleanValue(EL.attributeValue("enable"), false);
            String xpath = "//DBMigrationTasks[@name='" + migrationTasksName + "']";
            params.put("name", migrationTasksName);
            params.put("completBeforeStartNext", completBeforeStartNext + "");
            params.put("priority", priority + "");
            params.put("enable", enable + "");
            params.put("xpath", xpath);
            //log( " read " + migrationTasksName);
            if (enable) {
                MigrationTasks migrationTasks = new MigrationTasks(root, loader, migrationTasksName, params);
                ldbTestActionsByName.put(migrationTasksName, migrationTasks);
            }
        }
        for (int i = 1; i < priorityLevels; i++) {
            for (String key : ldbTestActionsByName.keySet()) {
                MigrationTasks migrationTasks = ldbTestActionsByName.get(key);
                if (migrationTasks == null)
                    continue;
                if (migrationTasks.getPriority() == i) {
                    if (migrationTasks.isCompletBeforeStartNext()) {
                        migrationTasks.addProcessingDoneEventListener(this);
                        synchronized (processingList) {
                            processingList.put(migrationTasks.getName(), false);
                        }
                    }
                    //log( " starting " + migrationTasks.getName());
                    migrationTasks.start();
                    if (migrationTasks.isCompletBeforeStartNext()) {
                        waitForResult();
                        migrationTasks.stop();
                        //log( " done stopped " + migrationTasks.getName());
                    }
                }
            }
        }

        //log( " done but waitForResult " );        
        waitForResult();
        stop();
        //log(" done. DONE migration tasks sequence");
    }

    private void waitForResult() {
        long timeWaiting = System.currentTimeMillis();
        boolean timedout = false;
        while (!isProcessginDone()) {
            synchronized (this) {
                try {
                    if (System.currentTimeMillis() - timeWaiting > timeout) {
                        timedout = true;
                        //log(" TIMED_OUT:  MigrationTasksSequence still processing " + processingList.size() + ". Waiting again ....");
                        //break;
                    }
                    wait(waitTime);
                } catch (Exception ex) {
                    MRLog.error(ex);
                }
            }
        }
        if (timedout) {
            log(" timed outed " + processingList.size());
            synchronized (processingList) {
                //processingList.clear();
            }
        }

    }

    private boolean isProcessginDone() {
        synchronized (processingList) {
            if (processingList.size() == 0)
                return true;
        }
        return false;
    }

    public void stop() {
        shutdown = true;
        for (String key : ldbTestActionsByName.keySet()) {
            MigrationTasks dbTestActions = ldbTestActionsByName.get(key);
            if (dbTestActions == null)
                continue;
            dbTestActions.stop();
        }
        ldbTestActionsByName.clear();
    }

    public boolean isShutdown() {
        return shutdown;
    }

    @Override
    public void processingDoneEvent(Object event) {
        synchronized (processingList) {
            MigrationTasks module = (MigrationTasks) event;
            processingList.remove(module.getName());
        }
        if (isProcessginDone()) {
            synchronized (this) {
                notifyAll();
            }
        }

    }

}
