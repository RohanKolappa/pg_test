package com.ipvsserv.nxd.dbtask.test.dbaction;

import java.util.Hashtable;
import java.util.Map;
import java.util.Properties;

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
public class DBTestActionDeleteIndex extends DBTestAction {
    private String indexFile = null;
    private Hashtable<String, String> replacelist = new Hashtable<String, String>();
    private Properties props = new Properties();
    
    public DBTestActionDeleteIndex() {
        super("DBTestActionCreateCollection");
    }
    
    public DBTestActionDeleteIndex(String moduleName) {
        super(moduleName);
    }
    
    @Override
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            String xQueryFolderPath = getPropertyDeep("indexFolderPath", null);
            String xQueryFilePath = getPropertyXmlFile("indexFilePath", xQueryFolderPath, null);
            indexFile = CompUtils.readFile(xQueryFilePath);
            props = CompUtils.loadProperties(props, xQueryFilePath);
            replacelist = getReplaceList();
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        String collectionName = "";
        String xQueryLocal = indexFile;
        StringBuffer bufNodes = new StringBuffer();
        try {
            if (indexFile == null) return;
            xQueryLocal = replace(xQueryLocal, replacelist, loop);
            collectionName = collectionPrefix + "" + DBTestActionShared.getInstance().getRandomCollection();
            t0 = System.currentTimeMillis();
            String path = props.getProperty("path", "");
            String docId = "path";
            if (path.equals("")) {
                path = props.getProperty("qname", "");
                docId = "qname";
            }
            String indexName = collectionName;
            db.addIndex(getThreadName(), indexName, docId, collectionName, path, xQueryLocal);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "i='" + xQueryLocal + "'");
        }
    }
    
}
