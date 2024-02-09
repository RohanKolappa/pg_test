package com.ipvsserv.nxd.adminapi.db;

import java.io.File;
import java.util.Map;

import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.adminapi.SaveFileQueue;
import com.ipvsserv.nxd.adminapi.SaveFileQueueCompletedListener;
import com.ipvsserv.nxd.adminapi.SaveParams;
import com.ipvsserv.nxd.api.IpvsDBDriver;

public class ExportQueryAction extends DBQueryAction implements SaveFileQueueCompletedListener {
    private boolean completed = false;

    @Override
    public Object execute(Map<String, String> params) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException, Exception {
        String dbquery = updateQuery(getQuery(), params);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        IpvsDBDriver db = dbadmin.getIpvsDBDriver();
        NIDSchemeMDC requestMDCParams = new NIDSchemeMDC();
        requestMDCParams.setDomain(exportDB.getDomainName());
        String collectionName = requestMDCParams.getCollectionNameSCol();

        File directory = new File(exportDB.getWorkarea_folder());
        directory = new File(directory, collectionName);
        directory.mkdirs();

        boolean isRootCollection = false;
        boolean saveToCol = false;
        DBAdminParams dbadminparams = new DBAdminParams();

        SaveFileQueue saveFileQueue = new SaveFileQueue();
        SaveParams saveParams = new SaveParams(directory, dbadminparams, isRootCollection, saveToCol);
        saveFileQueue.setSaveParams(saveParams);
        saveFileQueue.addSaveFileQueueCompletedListener(this);
        saveFileQueue.start();
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN:  ExportQuery " + directory.getAbsolutePath() + " " + dbquery);
        }
        db.execute("ExportQuery dbToDirectory", collectionName, dbquery, saveFileQueue);
        Thread.sleep(20);
        saveFileQueue.setStop(true);

        synchronized (this) {
            try {
                while (true) {
                    if (completed) {
                        break;
                    }
                    wait(5000);
                }
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
        //createZip(saveParams.getDirectory().getAbsolutePath(), exportDB.getZipFilePath());
        exportDB.invokeActions(this, queryEL);
        return saveParams.getDirectory().getAbsolutePath();
    }

    @Override
    public void saveQueueCompleted(SaveParams saveParams, int count) {
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN:   save file queue Completed");
        }
        
        synchronized (this) {
            try {
                completed = true;
                notify();
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }
    }

    @Override
    public void saveQueueFailed(SaveParams saveParams, int count) {
        if (MRLog.isDbadminLogEnable()) {
            MRLog.debug("DB_ADMIN:   save file queue failed");
        }

        synchronized (this) {
            try {
                completed = true;
                notify();
            } catch (Exception ex) {
                MRLog.error(ex);
            }
        }

    }
}
