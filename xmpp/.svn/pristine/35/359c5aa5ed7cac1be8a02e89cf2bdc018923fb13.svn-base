package com.ipvsserv.nxd.dbtask.test.dbaction;

import java.util.Hashtable;
import java.util.Map;

import org.dom4j.Document;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.dbtask.test.DBTestAction;
import com.ipvsserv.nxd.dbtask.test.DBTestActionShared;
import com.ipvsserv.nxd.dbtask.test.TestGlobals;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class DBTestActionUpdateDocument extends DBTestAction {
    private static int docType = 1;
    private static String indexName = "";
    private String xmlFilePath;
    private Document document = null;
    private String parentNID;
    private Hashtable<String, String> replacelist = new Hashtable<String, String>();
    private boolean readAndUpdate = false;
    
    public DBTestActionUpdateDocument() {
        super("DBTestActionUpdateDocument");
    }
    
    public DBTestActionUpdateDocument(String moduleName) {
        super(moduleName);
    }
    
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            String listName = getProperty("listName", TestGlobals.getProperty(dbActionID + ".listName", ""));
            if (!listName.equals("")) {
                parentNID = ipvsDomain + "." + listName;
            } else {
                parentNID = getProperty("parentNID", TestGlobals.getProperty(dbActionID + ".parentNID", ""));
            }
            readAndUpdate = CompUtils.booleanValue(getProperty("readAndUpdate", "true"), true);
            if (!readAndUpdate) {
                String xmlFolderPath = getPropertyDeep("xmlFolderPath", null);
                xmlFilePath = getPropertyXmlFile("xmlFilePath", xmlFolderPath, null);
                document = Dom4jUtils.loadXMLFile(xmlFilePath);
                replacelist = getReplaceList();
            }
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
        
    }
    
    @Override
    public void execute(int loop) throws Exception {
        long t0 = System.currentTimeMillis();
        String docId = "";
        String collectionName = "";
        try {
            DBTestActionShared shared = DBTestActionShared.getInstance();
            int col = shared.getRandomCollectionWithDoc();
            collectionName = collectionPrefix + "" + col;
            docId = documentPrefix + "" + DBTestActionShared.getInstance().getRandomDocument(col);
            t0 = System.currentTimeMillis();
            String xml = db.getDocument(getThreadName(), ipvsDomain, docType, docId, collectionName);
            xml = replace(xml, replacelist, loop);
            String NID = "";
            db.updateDocument(getThreadName(), ipvsDomain, 1, docId, collectionName, xml, indexName, NID);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "d='" + docId + "','" + collectionName + "'");
        }
    }
    
}
