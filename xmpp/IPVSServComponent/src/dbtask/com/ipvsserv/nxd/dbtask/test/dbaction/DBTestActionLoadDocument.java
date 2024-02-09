package com.ipvsserv.nxd.dbtask.test.dbaction;

import java.util.Hashtable;
import java.util.Map;

import org.dom4j.Document;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.common.util.IpvsDBUtils;
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
public class DBTestActionLoadDocument extends DBTestAction {
    private static int docType = 1;
    private String xmlFilePath;
    private Document document = null;
    private String parentNID;
    private Hashtable<String, String> replacelist = null;
    
    public DBTestActionLoadDocument() {
        super("DBTestActionLoadDocument");
    }
    
    public DBTestActionLoadDocument(String moduleName) {
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
            String xmlFolderPath = getPropertyDeep("xmlFolderPath", null);
            xmlFilePath = getPropertyXmlFile("xmlFilePath", xmlFolderPath, null);
            document = Dom4jUtils.loadXMLFile(xmlFilePath);
            replacelist = getReplaceList();
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
            if (document == null) return;
            Document doc = null;
            synchronized (document) {
                doc = (Document) document.clone();
            }
            IpvsDBUtils.applyNID(doc.getRootElement(), parentNID, loop);
            DBTestActionShared shared = DBTestActionShared.getInstance();
            
            int col = shared.getRandomCollection();
            collectionName = collectionPrefix + "" + col;
            docId = documentPrefix + "" + shared.getAndIncrementDocuments(col);
            
            String xml = doc.getRootElement().asXML();
            xml = replace(xml, replacelist, loop);
            
            t0 = System.currentTimeMillis();
            db.addDocument(getThreadName(), ipvsDomain, docType, docId, collectionName, xml);
            //ID, domainName, 1, docId, collectionName, doc.getRootElement().asXML(), indexName, NID);
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "d='" + docId + "','" + collectionName + "'");
        }
    }
    
}
