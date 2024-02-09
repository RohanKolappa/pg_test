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
public class DBTestActionDeleteDocument extends DBTestAction {
    private static int docType = 1;
    
    public DBTestActionDeleteDocument() {
        super("DBTestActionDeleteDocument");
    }
    
    public DBTestActionDeleteDocument(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        String docId = "";
        String collectionName = "";
        try {
            DBTestActionShared shared = DBTestActionShared.getInstance();
            int col = shared.getRandomCollectionWithDoc();
            if (col == -1) {
                MRLog.debug("execute " + getThreadName() + " " + col);
                return;
            }
            collectionName = collectionPrefix + "" + col;
            docId = documentPrefix + "" + shared.decrementAndGetDocuments(col);
            t0 = System.currentTimeMillis();
            db.deleteDocument(getThreadName(), ipvsDomain, docType, docId, collectionName);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "d='" + docId + "','" + collectionName + "'");
        }
    }
}
