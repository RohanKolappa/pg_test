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
public class DBTestActionRefreshDBStat extends DBTestAction {
    
    public DBTestActionRefreshDBStat() {
        super("DBTestActionRefreshDBStat");
    }
    
    public DBTestActionRefreshDBStat(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        try {
            DBTestActionShared shared = DBTestActionShared.getInstance();
            t0 = System.currentTimeMillis();
            shared.init(db.getChildCollectionResourceCount(getThreadName(), "", "startswith", collectionPrefix), collectionPrefix);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "");
            
        }
    }
    
}
