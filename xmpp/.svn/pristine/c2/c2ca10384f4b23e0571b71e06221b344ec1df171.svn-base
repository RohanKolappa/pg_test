package com.ipvsserv.nxd.dbtask.test.dbaction;

import java.util.Hashtable;
import java.util.Map;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.nxd.dbtask.test.DBTestAction;
import com.ipvsserv.nxd.dbtask.test.DBTestActionShared;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTestActionXquery extends DBTestAction {
    private String xQuery = null;
    private Hashtable<String, String> replacelist = new Hashtable<String, String>();
    
    public DBTestActionXquery() {
        super("DBTestActionXquery");
    }
    
    public DBTestActionXquery(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            String xQueryFolderPath = getPropertyDeep("xQueryFolderPath", null);
            String xQueryFilePath = getPropertyXmlFile("xQueryFilePath", xQueryFolderPath, null);
            xQuery = CompUtils.readFile(xQueryFilePath);
            if (xQuery != null) xQuery = xQuery.replace('\n', ' ');
            replacelist = getReplaceList();
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        String collectionName = "";
        String xQueryLocal = xQuery;
        StringBuffer bufNodes = new StringBuffer();
        try {
            if (xQuery == null) return;
            xQueryLocal = replace(xQueryLocal, replacelist, loop);
            String colNumber = "0";
            try {
                colNumber = DBTestActionShared.getInstance().getRandomCollectionWithDoc() + "";
            } catch (Exception ex) {
                MRLog.error(ex.getMessage());
            }
            collectionName = collectionPrefix + "" + colNumber;
            t0 = System.currentTimeMillis();
            db.execute(getThreadName(), ipvsDomain, collectionName, xQueryLocal, bufNodes, 0, -1);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "resp bytes: " + bufNodes.length() + " " + xQueryLocal);
        }
    }
    
}
