package com.ipvsserv.nxd.dbtask.test.dbaction;

import java.util.Map;

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
public class DBTestActionCreateCollection extends DBTestAction {
    private static int collectionType = 1;
    
    public DBTestActionCreateCollection() {
        super("DBTestActionCreateCollection");
    }
    
    public DBTestActionCreateCollection(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            // collectionPrefix = getProperty("collectionPrefix", TestGlobals.getProperty("DBTestActionsSequence.collectionPrefix", "ipvsCol"));
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        String collectionName = "";
        try {
            Integer col = DBTestActionShared.getInstance().getAndIncrementCollection();
            collectionName = collectionPrefix + "" + col;
            t0 = System.currentTimeMillis();
            db.addCollection(getThreadName(), ipvsDomain, collectionType, collectionName);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "c='" + collectionName + "'");
            
        }
    }
    
}
