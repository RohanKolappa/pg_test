package com.ipvsserv.nxd.bulkupload;

import java.util.ArrayList;
import java.util.Enumeration;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmldb.api.DatabaseManager;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.Database;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.XMLResource;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.xmldb.sedna.SednaXmlDBModule;
import com.ipvsserv.nxd.bulkupload.template.BulkLoadGroupTemplateIterator;
import com.ipvsserv.nxd.bulkupload.template.BulkLoadTemplateIterator;
import com.ipvsserv.nxd.bulkupload.template.TemplateDefination;

/**
 * @author pravin reddy
 * Email: pravin@ipvideosys.com
 * IP Video Systems , CA, USA
 * http://www.ipvideosys.com 
 * @since 1.0.0
 */
public class BulkUploadService {
    private SednaXmlDBModule xmlDBModule = null;
   /* public static void main(String[] args) {
        try {
             
            String propsFile = "bulkupload.xml";
            if (args.length > 0) {
                propsFile = args[0].trim();
            }
            String templateDefinationXmlPath = "templateDefination.xml";
            if (args.length > 1) {
                templateDefinationXmlPath = args[1].trim();
            }
            BulkUploadService importDocuments = new BulkUploadService();
            importDocuments.createXmls(propsFile, templateDefinationXmlPath, null);
        } catch (Exception ex) {
            printUsage();
            MRLog.error(ex);
        }
        //MRLog.debug("done");
    }*/
    
    private static void printUsage() {}
    
    private Document doc = null;
    private Collection rootCollection = null;
    private Hashtable<String, String> domains = null;
    private List<String> uploadObjectsedNIDS = new ArrayList<String>();
    private TemplateDefination templateDefination = null;
    
    public void createXmls(String xmlFilePath, TemplateDefination templateDefination, SednaXmlDBModule xmlDBModule) {
         this.xmlDBModule=xmlDBModule;
         this.templateDefination = templateDefination ;
        try {
            doc = Dom4jUtils.loadXMLFile(xmlFilePath);
            if(xmlDBModule==null){
                login();
            }else{
                rootCollection = xmlDBModule.checkOut();
            }
            getDomains();
            List<?> list = Dom4jUtils.getList(doc.getRootElement(), "bulkload");
            for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
                Element bulkloadEL = (Element) iterator.next();
                processBulkload(bulkloadEL);
            }
        } catch (Exception ex) {
            MRLog.debug(" doc " + doc.asXML());
            MRLog.error(ex);
            printUsage();
        } finally {
            try {
                if(xmlDBModule==null){
                    rootCollection.close();
                }else{
                    xmlDBModule.checkIn(rootCollection, false);
                }
            } catch (XMLDBException ex) {
                MRLog.error(ex);
            }
        }
    }
    
    private void processBulkload(Element bulkloadEL) throws Exception {
        String ipvsdomain = Dom4jUtils.getAttributeValue(bulkloadEL, "ipvsdomain", "");
        String bulkloadId = Dom4jUtils.getAttributeValue(bulkloadEL, "bulkloadId", "");
        String collectionName = domains.get(ipvsdomain);
        if (collectionName == null) {
            throw new Exception("domain does not exists");
        }
        Collection col = rootCollection.getChildCollection(collectionName);
        List<?> list = Dom4jUtils.getList(bulkloadEL, "loadObjects");
        int i = 0;
        for (Iterator<?> iterator = list.iterator(); iterator.hasNext();) {
            Element loadObjectsEL = (Element) iterator.next();
            String type = Dom4jUtils.getAttributeValue(loadObjectsEL, "type", "");
            MRLog.debug("\n" + i++ + " processBulkload- " + type);
            if (type.equals("template")){
                processBulkLoadT(loadObjectsEL, col, ipvsdomain, bulkloadId);
            }
            else if (type.equals("groupTemplate")) {
                processBulkLoadGroupT(loadObjectsEL, col, ipvsdomain, bulkloadId);
            }
        }
    }
    
    //////////////////////////////////////////////////////////////////////////////////
    private void processBulkLoadGroupT(Element loadObjectsEL, Collection col, String ipvsdomain, String bulkloadId) throws Exception {
        BulkLoadGroupTemplateIterator iterator = new BulkLoadGroupTemplateIterator(templateDefination, loadObjectsEL);
        iterator.setIpvsdomain(ipvsdomain);
        if (iterator.isEnable() && iterator.hasNext()) {
            while (iterator.hasNext()) {
                MRLog.debug("\nSTART############################# " + iterator.getPosition() + " processBulkLoadGroupT " + iterator.getloadObjectsId() + " #############################");
                Hashtable<String, Element> tobeSavedMembers = iterator.next();
                for (Enumeration<Element> e = tobeSavedMembers.elements(); e.hasMoreElements();) {
                    Element element = e.nextElement();
                    //save to db
                    saveToDB(element, col);
                }
                MRLog.debug("END############################# " + iterator.getPosition() + " processBulkLoadGroupT " + iterator.getloadObjectsId() + " #############################");
            }
        }
        
        /*//<loadObjects type="objecttemplate" loadObjectsId='bulkload1.4' templateId='location1' count='5' enable='true'  />       
        boolean enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "enable", "true"), true);
        String loadObjectsId = Dom4jUtils.getAttributeValue(loadObjectsEL, "loadObjectsId", "");
        //String type = Dom4jUtils.getAttributeValue(loadObjectsEL, "type", "");
        int count = CompUtils.intValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "count", "0"), 0);
        String groupTemplateId = Dom4jUtils.getAttributeValue(loadObjectsEL, "groupTemplateId", "");
        
        //check enabled
        if (!enable) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  objecttemplate not enabled  '" + groupTemplateId + "' ");
            return;
        }
        
        //validate groupTemplate defined 
        groupTemplate groupTemplate = templateDefination.getgroupTemplate(groupTemplateId);
        if (groupTemplate == null) {
            throw new Exception(" processBulkLoadGroup='" + loadObjectsId + "'  objecttemplate not found  '" + groupTemplateId + "' ");
        }
        
        //process
        for (int i = 0; i < count; i++) {
            MRLog.debug("\nSTART############################# " + i + " processBulkLoadGroupT " + groupTemplateId + " #############################");
            Hashtable<String, Element> tobeSavedMembers = new Hashtable<String, Element>();
            for (String groupTemplateMemberId : groupTemplate.getgroupTemplateKeys()) {
                groupTemplateMember objecttemplateMember = groupTemplate.getgroupTemplateMember(groupTemplateMemberId);
                template template = templateDefination.gettemplate(objecttemplateMember.gettemplateId());
                if (template == null) {
                    throw new Exception(" processBulkLoadGroup='" + loadObjectsId + "'  objecttemplate not found  '" + objecttemplateMember.gettemplateId() + "' ");
                }
                String parentNID = ipvsdomain + "." + template.getListName();
                
                //prcess for each objecttemplate
                Document tobeSavedDoc = template.getXml();
                if (tobeSavedDoc == null) {
                    throw new Exception(" processBulkLoadGroup='" + loadObjectsId + "'  tobeSavedDoc is not null '" + objecttemplateMember.gettemplateId() + "' ");
                } else {
                    BulkUploadUtils.updateValue(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), i);
                    IpvsDBUtils.applyNID(tobeSavedDoc.getRootElement(), parentNID, i);
                    BulkUploadUtils.updateValue(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), i);
                    tobeSavedMembers.put(objecttemplateMember.getgroupTemplateMemberId(), tobeSavedDoc.getRootElement());
                }
            }
            //update references
            BulkUploadUtils.updateValueGroup(tobeSavedMembers, groupTemplate);
            //save to all xmls to db
            for (Enumeration<Element> e = tobeSavedMembers.elements(); e.hasMoreElements();) {
                Element element = e.nextElement();
                //save to db
                saveToDB(element, col);
            }
            MRLog.debug("END############################# " + i + " processBulkLoadGroupT " + groupTemplateId + " #############################");
        }*/
    }
    
    ////////////////////////////////////
    
    ////////////////////////////////////////////////////////////////////////////////////////////////
    private void processBulkLoadT(Element loadObjectsEL, Collection col, String ipvsdomain, String bulkloadId) throws Exception {
        BulkLoadTemplateIterator iterator = new BulkLoadTemplateIterator(templateDefination, loadObjectsEL);
        iterator.setIpvsdomain(ipvsdomain);
        if (iterator.isEnable() && iterator.hasNext()) {
            while (iterator.hasNext()) {
                MRLog.debug("\nSTART############################# " + iterator.getPosition() + " processBulkLoadGroupT " + iterator.getloadObjectsId() + " #############################");
               
                Element element = iterator.next();
                saveToDB(element, col);
                MRLog.debug("END############################# " + iterator.getPosition() + " processBulkLoadGroupT " + iterator.getloadObjectsId() + " #############################");
            }
        }
        
        /*//<loadObjects type="objecttemplate" loadObjectsId='bulkload1.1' templateId='userlist1' count='5'  enable='true' />
        boolean enable = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "enable", "true"), true);
        String loadObjectsId = Dom4jUtils.getAttributeValue(loadObjectsEL, "loadObjectsId", "");
        //String type = Dom4jUtils.getAttributeValue(loadObjectsEL, "type", "");
        int count = CompUtils.intValue(Dom4jUtils.getAttributeValue(loadObjectsEL, "count", "0"), 0);
        String templateId = Dom4jUtils.getAttributeValue(loadObjectsEL, "templateId", "");
        //if not enable skip
        if (!enable) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  is not enabled '" + templateId + "' ");
            return;
        }
        //validate
        template objecttemplate = templateDefination.gettemplate(templateId);
        if (objecttemplate == null) {
            MRLog.debug("SKIP.  " + loadObjectsId + "  objecttemplate not found  '" + templateId + "' ");
            return;
        }
        String parentNID = ipvsdomain + "." + objecttemplate.getListName();
        
        for (int i = 0; i < count; i++) {
            MRLog.debug("\nSTART############################# " + i + " processBulkLoadT " + templateId + " #############################");
            Document tobeSavedDoc = objecttemplate.getXml();
            if (tobeSavedDoc == null) {
                MRLog.debug("SKIP.  " + loadObjectsId + "  tobeSavedDoc is not null '" + templateId + "' ");
            } else {
                BulkUploadUtils.updateValue(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), i);
                IpvsDBUtils.applyNID(tobeSavedDoc.getRootElement(), parentNID, i);
                BulkUploadUtils.updateValue(tobeSavedDoc.getRootElement(), templateDefination.getUpdateXmlDefinations(), i);
                saveToDB(tobeSavedDoc.getRootElement(), col);
            }
            MRLog.debug("END############################# " + i + " processBulkLoadT#############################");
        }*/
    }
    
    ////////////////////////////////////////////////////////////////////////////////// ///////////////////////////////////////// 
    
    ///////////////////////////////////////////////////////////////////////////////////////////
    //////////////////////////////////////////////////////////////////////////////////  
    private void login() throws Exception {
        String databaseurl = Dom4jUtils.valueXpath(doc, "//database/databaseurl/text()", null);
        String user = Dom4jUtils.valueXpath(doc, "//database/user/text()", null);
        String pwd = Dom4jUtils.valueXpath(doc, "//database/pwd/text()", null);
        Database dbDriver = (Database) Class.forName("net.cfoster.sedna.DatabaseImpl").newInstance();
        DatabaseManager.registerDatabase(dbDriver);
        rootCollection = DatabaseManager.getCollection(databaseurl, user, pwd);
    }
    
    private void getDomains() throws Exception {
        String DB_COL_PREFIX = "IPVSc_";
        domains = new Hashtable<String, String>();
        String[] arrCol = rootCollection.listChildCollections();
        for (int i = 0; i < arrCol.length; i++) {
            String docName = arrCol[i];
            if (docName.startsWith(DB_COL_PREFIX)) {
                String serviceDomain = docName.substring(docName.indexOf(DB_COL_PREFIX) + DB_COL_PREFIX.length());
                MRLog.debug(serviceDomain + " " + docName);
                domains.put(serviceDomain, docName);
            }
        }
    }
    
    private void saveToDB(Element element, Collection col) throws Exception {
        String NID = "";
        try {
            NID = Dom4jUtils.getAttributeValue(element, "NID", "");
            NIDSchemeMDC node = new NIDSchemeMDC();
            node.parse(NID);
            String docUri = node.getDocUri();
            //MRLog.debug("/////////////////////////////saveToDB/////////////////////////////////////////////");
            MRLog.debug("saveToDB " + NID + " " );//+ element.asXML());//Dom4jUtils.elementToString(element));
            //System.out.println("saveToDB " + NID );//+ " \n " + element.asXML());//Dom4jUtils.elementToString(element));
            //MRLog.debug("saveToDB " + NID +"");
            //System.out.println(" saveToDB " + NID +"");
            //MRLog.debug("//////////////////////////////saveToDB////////////////////////////////////////////");
            uploadObjectsedNIDS.add(NID); // may be useful to revert in case of failure
            XMLResource resource = (XMLResource) col.createResource(docUri, XMLResource.RESOURCE_TYPE);
            resource.setContent(element.asXML());//Dom4jUtils.elementToString(element));
            col.storeResource(resource);
            
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
}
