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
public class DBTestActionDeleteCollection extends DBTestAction {
    private static int collectionType = 1;
    
    public DBTestActionDeleteCollection() {
        super("DBTestActionDeleteCollection");
    }
    
    public DBTestActionDeleteCollection(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        String collectionName = "";
        try {
            collectionName = collectionPrefix + "" + DBTestActionShared.getInstance().decrementAndGetCollection();
            t0 = System.currentTimeMillis();
            db.deleteCollection(getThreadName(), ipvsDomain, collectionType, collectionName);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "c='" + collectionName + "'");
        }
    }
    
}
