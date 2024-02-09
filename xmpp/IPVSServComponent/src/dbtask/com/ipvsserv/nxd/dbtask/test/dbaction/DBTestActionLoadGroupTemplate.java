package com.ipvsserv.nxd.dbtask.test.dbaction;

import java.io.File;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Map;

import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.nxd.bulkupload.template.BulkLoadGroupTemplateIterator;
import com.ipvsserv.nxd.bulkupload.template.TemplateDefination;
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
public class DBTestActionLoadGroupTemplate extends DBTestAction {
    private BulkLoadGroupTemplateIterator iterator = null;
    private TemplateDefination templateDefination = null;
    private String templateDefinationXmlPath = "";
    private static int docType = 1;
    private String parentNID;
    
    public DBTestActionLoadGroupTemplate() {
        super("DBTestActionLoadDocumentGroup");
    }
    
    public DBTestActionLoadGroupTemplate(String moduleName) {
        super(moduleName);
    }
    
    public void initialize(Map<String, String> params) throws Exception {
        super.initialize(params);
        try {
            templateDefinationXmlPath = getProperty("templateDefinationXmlPath", TestGlobals.getProperty(dbActionID + ".templateDefinationXmlPath", ""));
            Element loadObjectsEL = TestGlobals.getElement(dbActionID + ".loadGroupTemplate");
            templateDefination = new TemplateDefination();
            File file = new File(templateDefinationXmlPath);
            if (!file.exists()) {
                templateDefinationXmlPath = IpvsServGlobals.verifyAndGetTestXmlPath(templateDefinationXmlPath);
            }
            templateDefination.setTemplateDefinationXmlPath(templateDefinationXmlPath);
            iterator = new BulkLoadGroupTemplateIterator(templateDefination, loadObjectsEL);
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
            t0 = System.currentTimeMillis();
            if (iterator.isEnable() && iterator.hasNext()) {
                MRLog.debug("\nSTART############################# " + iterator.getPosition() + " processBulkLoadGroupT " + iterator.getloadObjectsId() + " #############################");
                DBTestActionShared shared = DBTestActionShared.getInstance();
                int col = shared.getRandomCollectionWithDoc();
                collectionName = collectionPrefix + "" + col;
                Hashtable<String, Element> tobeSavedMembers = iterator.next();
                for (Enumeration<Element> e = tobeSavedMembers.elements(); e.hasMoreElements();) {
                    Element element = e.nextElement();
                    //save to db
                    docId = documentPrefix + "" + DBTestActionShared.getInstance().getRandomDocument(col);
                    db.addDocument(getThreadName(), ipvsDomain, docType, docId, collectionName, element.asXML());
                }
                MRLog.debug("END############################# " + iterator.getPosition() + " processBulkLoadGroupT " + iterator.getloadObjectsId() + " #############################");
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            logExec(t0, loop, "c='" + collectionName + "'");
        }
    }
    
}
