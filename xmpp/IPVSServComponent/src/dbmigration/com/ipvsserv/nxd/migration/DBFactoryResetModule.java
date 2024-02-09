package com.ipvsserv.nxd.migration;

import java.io.File;
import java.util.Map;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.adminapi.DBAdmin;
import com.ipvsserv.nxd.adminapi.DBAdminApi;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.server.IpvsFileStatus;
import com.ipvsserv.server.IpvsServBasicModule;
import com.ipvsserv.server.IpvsServer;

public class DBFactoryResetModule extends IpvsServBasicModule {
    public DBFactoryResetModule() {
        super("DBFactoryReset");
    }

    public DBFactoryResetModule(String moduleName) {
        super(moduleName);
    }


    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        File dir = new File(IpvsServGlobals.getProperty("admindbapi.dbBackupArea", "/data/sedna_dbsnapshot/backup"));
        dir.mkdirs();

    }

    @Override
    public void start() throws Exception {
        super.startMonitor();
    }

    @Override
    public void stop() throws Exception {
        destroy();
    }

    @Override
    public void destroy() throws Exception {
        super.destroy();
    }

    private boolean runningInsert = false;

    private boolean done = false;

    @Override
    public void verifyState() throws Exception {

        synchronized (this) {
            if (runningInsert) {
                return;
            }
            runningInsert = true;
        }
        try {
            //check if database is connected
            DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
            int counter=0;
            long to=System.currentTimeMillis();
            while(!dbadmin.isDBModuleStarted()){
                Thread.sleep(1000);
                counter++;
                if(counter>20){
                    MRLog.debug("Factory DB: Check Database initialized: Wait to connect to database...(" +(System.currentTimeMillis()-to)+")");
                    counter=0;
                }
            }
            if(!done) {
                done = reset(false);
            }
            if (done) {
                started = true;
                initialized = true;
                IpvsServer.getInstance().setFactoryDBCheckDone(true);
                try {
                    destroy();
                    MRLog.debug("DBFactory is upto date ........");
                    return;
                } catch (Exception ex) {

                }
            }
             
            executed = true;

        } catch (Exception ex) {
            MRLog.error("DBFactoryReset", ex);
        } finally {
            synchronized (this) {
                runningInsert = false;
            }
        }
    }

    @Override
    public int getLastKnownState() throws Exception {
        return 0;
    }

    public synchronized boolean reset(boolean force) throws Exception {
        String factorydbPath = IpvsServGlobals.getProperty("admindbapi.factorydb", "/home/onair/templates/factorydb");
        File directory = new File(factorydbPath);
        boolean saveToColEnable = true;
        DBAdminParams params = new DBAdminParams();
        params.setSaveToColEnable(saveToColEnable);
        DBAdminApi dbadmin = DBAdmin.getDBAdminApi();
        boolean factoryDBneeded= dbadmin.factoryReset(directory, params, force);
        if (factoryDBneeded) {
            if(force){
                IpvsFileStatus.createDBHistory("FORCED FACTORY DATABASE RESTORE", "");
            }else{
                IpvsFileStatus.createDBHistory("FACTORY DATABASE RESTORE", "");
            }
        }
        //done=true;
        return true;
    }

}