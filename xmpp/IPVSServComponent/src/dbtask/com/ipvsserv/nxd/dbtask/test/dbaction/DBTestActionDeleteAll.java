package com.ipvsserv.nxd.dbtask.test.dbaction;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.dbtask.test.DBTestAction;
import com.ipvsserv.nxd.dbtask.test.DBTestActionShared;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTestActionDeleteAll extends DBTestAction {
    public DBTestActionDeleteAll() {
        super("DBTestActionDeleteAll");
    }
    
    public DBTestActionDeleteAll(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        try {
            t0 = System.currentTimeMillis();
            db.deleteDB();
            DBTestActionShared.getInstance().init();
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "");
        }
    }
    
}
