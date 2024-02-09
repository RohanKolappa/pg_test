package com.ipvsserv.nxd.migration.tag.db;

import java.io.File;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.IpvsDBDriver;
import com.ipvsserv.nxd.migration.DBDocuments;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;
import com.ipvsserv.nxd.xslt.XsltTransformUtils;

public class UpdateDocsTag extends MigrationTagImpl {
    public UpdateDocsTag() {
        super("UpdateDocsTag");
    }

    public UpdateDocsTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        doMigrationNative(variables);
    }
 
    public void doMigrationNative(ScriptVariables variables) throws Exception {
        /*
        <ipvstag_UpdateDocs>
            <readDocs readFromType="xpath" readFrom='/User'>
                <el_remove select="/User/Roster/Item/Group"/>
                <el_rename select="/User/Groups" renameTo="Group" changeParent="/User/Roster/Item"/>
            </readDocs>     
        </ipvstag_UpdateDocs>  
        */
        DBDocuments dBDocuments = root.getMigrationModule().getDBDocuments();
        String query = "";
        //Element readDocsEL = element;//Dom4jUtils.element(element, "readDocs");
        String readFromType = Dom4jUtils.getAttributeValue(element, "readFromType", "xpath");
        String readFrom = Dom4jUtils.getAttributeValue(element, "readFrom", "/*");
        List<String> listDocuments = new ArrayList<String>();
        log(" start doMigration " + element.asXML());
        IpvsDBDriver db = root.getMigrationModule().getDBAdminApi().getIpvsDBDriver();//IpvsServer.getInstance().getDefaultIpvsDBDriver();
        try {
            List<String> list = dBDocuments.getDomainNames();//db.getDomainNames();
            for (Iterator<String> domianiterator = list.iterator(); domianiterator.hasNext();) {
                String domainName = domianiterator.next();
                root.getScriptGlobals().setServiceDomain(domainName);
                NIDSchemeMDC nodeDomain = new NIDSchemeMDC();
                nodeDomain.setDomain(domainName);
                String collectionName = nodeDomain.getCollectionName();
                //String indexName = nodeDomain.getIndexName();
                try {
                    try {
                        query = " for $b in collection('" + collectionName + "')" + readFrom + " return document-uri(root($b))";
                        /*if (readFromType.equalsIgnoreCase("xquery")) {
                            boolean relativePath = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "relativePath", ""), true);
                            File file = new File(readFrom);
                            if (relativePath) {
                                String filePath = root.getScriptGlobals().getDBMigrationFolder() + File.separator + readFrom;
                                file = new File(filePath);
                            }
                            if (!file.exists()) {
                                throw new Exception("File does not exists  '" + readFrom + "'");
                            }
                            query = CompUtils.readFile(file.getAbsolutePath());
                        }*/
                        query = root.getScriptGlobals().modifyQuery(query);
                        db.execute("migration", domainName, collectionName, query, listDocuments, 1, -1);
                      
                    } catch (Exception ex) {
                        MRLog.error(query, ex);
                    } finally {
                    }

                    int counter = 0;
                    int total = listDocuments.size();
                    Element delDoc = element.element("doc_delete");
                    if (delDoc != null) {
                        //[Sedna-discussion] ERROR SE3007 and then "Offset is out of range"
                        //http://www.mail-archive.com/sedna-discussion@lists.sourceforge.net/msg00302.html
                        //delete from db
                        //query = " update delete collection('" + collectionName + "')" + readFrom + "";
                        //query = root.getScriptGlobals().modifyQuery(query);
                        //db.executeXQueryToUpdate("migration", collectionName, query);    
                        
                        //TODO:not good for big db
                        //query = " update replace $b in collection('" + collectionName + "')" + readFrom + " with <a/>";
                        //query = root.getScriptGlobals().modifyQuery(query);
                        //db.executeXQueryToUpdate("migration", collectionName, query);    
                        
                        
                        for (Iterator<String> documents = listDocuments.iterator(); documents.hasNext();) {
                            String docUri =   documents.next().trim();
                            try {
                                counter++;
                                //if (counter > 1687) {
                                 //   System.out.println(" " + " doc_delete (" + counter + "/" + total + ") " + docUri + " " + readFrom);
                                //}
                                db.deleteDocument("migration_doc_delete", domainName, 1, docUri, collectionName);
                            } catch (Exception ex) {
                                MRLog.error(query, ex);
                            } finally {
                                log(" doc_delete (" + counter + "/" + total + ") " + docUri + " " + readFrom);
                            }
                        }
                    } else {

                        //loop on each document 
                        for (Iterator<String> documents = listDocuments.iterator(); documents.hasNext();) {
                            String nodeID = "";
                            counter++;
                            String docUri =  documents.next().trim();
                            String name = "";
                            try {
                                String xml = db.getDocument("migartion", domainName, 1, docUri, collectionName);
                                Document doc = Dom4jUtils.stringToDocument(xml);
                                nodeID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
                                NIDSchemeMDC node = new NIDSchemeMDC();
                                node.parse(nodeID);

                                boolean test_do_not_save = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "test_do_not_save", "true"), true);
                                //apply each tag on each document
                                List<?> actionELs = element.elements();
                                for (Iterator<?> iterator2 = actionELs.iterator(); iterator2.hasNext();) {
                                    Element actionEL = (Element) iterator2.next();
                                    name = actionEL.getName();
                                    if (name.equals("el_rename")) {
                                        log(" " + name + "(" + counter + "/" + total + ")  " + nodeID);
                                        el_rename(doc, actionEL);
                                    } else if (name.equals("el_remove")) {
                                        log(" " + name + "(" + counter + "/" + total + ")  " + nodeID);
                                        el_remove(doc, actionEL);
                                    } else if (name.equals("xsl_transform")) {
                                        String fileName = Dom4jUtils.getAttributeValue(actionEL, "xsl", "").trim();
                                        log(" " + name + "(" + counter + "/" + total + ")  " + nodeID + " XSL: " + fileName);
                                        xsl_transform(doc, actionEL);
                                    }
                                }
                                if (!test_do_not_save) {
                                    System.out.println("/////////////////// new xml //////////////////////////");
                                    System.out.println(doc.getRootElement().asXML());
                                    System.out.println("/////////////////// new xml //////////////////////////");
                                } else {
                                    //add the resource
                                    String nodeID2 = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
                                   
                                    node = new NIDSchemeMDC();
                                    node.parse(nodeID2);
                                    if (nodeID2.equals("") || node.getDocUri().equals("")) {
                                        MRLog.debug("Error in NID is invalid on returned document '" + nodeID2 + "' original NID=" + nodeID);
                                    } else {
                                        if(!node.getDocUri().equals(docUri)){
                                            MRLog.debug("Add document '" + nodeID2 + "' original NID=" + nodeID);
                                        }
                                        if(node.getDocUri().equals(docUri) || db.isDocumentExists("migartion", domainName, 1, node.getDocUri(), collectionName)) {
                                            //db.updateDocument("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML(), indexName, nodeID2);
                                            db.deleteDocument("migartion", domainName, 1,  node.getDocUri(), collectionName);
                                        } else {
                                            //db.addDocument("migartion", domainName, 1,  node.getDocUri(), collectionName, doc.getRootElement().asXML());//("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML(), indexName, nodeID2);
                                        }                                        
                                        db.addDocument("migartion", domainName, 1,  node.getDocUri(), collectionName, doc.getRootElement().asXML());//("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML(), indexName, nodeID2);
                                    }
                                }

                            } catch (Exception ex) {
                                MRLog.error(query, ex);
                            } finally {
                                log(" done " + name + " (" + counter + "/" + total + ") " + nodeID);
                            }
                        }
                    }
                } catch (Exception ex) {
                    MRLog.error(query, ex);
                } finally {
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

//    public void doMigrationXmlDB(ScriptVariables variables) throws Exception {
//        /*
//        <ipvstag_UpdateDocs>
//            <readDocs readFromType="xpath" readFrom='/User'>
//                <el_remove select="/User/Roster/Item/Group"/>
//                <el_rename select="/User/Groups" renameTo="Group" changeParent="/User/Roster/Item"/>
//            </readDocs>     
//        </ipvstag_UpdateDocs>  
//        */
//        String query = "";
//        //Element readDocsEL = element;//Dom4jUtils.element(element, "readDocs");
//        String readFromType = Dom4jUtils.getAttributeValue(element, "readFromType", "xpath");
//        String readFrom = Dom4jUtils.getAttributeValue(element, "readFrom", "/*");
//
//        try {
//            List<String> list = root.getMigrationModule().getDBAdminApi().getIpvsDBDriver().getDomainNames();
//            for (Iterator<String> domianiterator = list.iterator(); domianiterator.hasNext();) {
//                String serviceDomain = domianiterator.next();
//                root.getScriptGlobals().setServiceDomain(serviceDomain);
//                Collection rootCollection = root.getMigrationModule().getDBModule().checkOut();
//                try {
//                    Collection collection = rootCollection.getChildCollection(root.getScriptGlobals().getIpvs_collection());
//
//                    //decide query to read document-uri
//                    query = " for $b in collection('" + root.getScriptGlobals().getIpvs_collection() + "')" + readFrom + " return document-uri(root($b))";
//                    if (readFromType.equalsIgnoreCase("xquery")) {
//                        boolean relativePath = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "relativePath", ""), true);
//                        File file = new File(readFrom);
//                        if (relativePath) {
//                            String filePath = root.getScriptGlobals().getDBMigrationFolder() + File.separator + readFrom;
//                            file = new File(filePath);
//                        }
//                        if (!file.exists()) {
//                            throw new Exception("File does not exists  '" + readFrom + "'");
//                        }
//                        query = CompUtils.readFile(file.getAbsolutePath());
//                    }
//                    query = root.getScriptGlobals().modifyQuery(query);
//
//                    //// read list on document-uri s. 
//                    List<String> listDocuments = new ArrayList<String>();
//                    XQueryService service = (XQueryService) root.getMigrationModule().getDBModule().getServiceManager().getXQueryService(collection);
//                    CompiledExpression compiled = service.compile(query);
//                    ResourceSet result = service.execute(compiled);
//                    ResourceIterator iterator = result.getIterator();
//                    while (iterator.hasMoreResources()) {
//                        Resource resource = iterator.nextResource();
//                        listDocuments.add(resource.toString());
//                    }
//
//                    int counter = 0;
//                    int total = listDocuments.size();
//
//                    //loop on each document 
//                    for (Iterator<String> documents = listDocuments.iterator(); documents.hasNext();) {
//                        String nodeID = "";
//                        counter++;
//                        try {
//                            String docUri = (String) documents.next();
//                            Resource resource = collection.getResource(docUri);
//                            Document doc = Dom4jUtils.stringToDocument(resource.toString());
//                            nodeID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
//                            NIDSchemeMDC node = new NIDSchemeMDC();
//                            node.parse(nodeID);
//                            Element delDoc = element.element("doc_delete");
//                            boolean test_do_not_save = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "test_do_not_save", "true"), true);
//                            if (delDoc != null) {
//                                XMLResource document = (XMLResource) collection.createResource(node.getDocUri(), "XMLResource");
//                                collection.removeResource(document);
//                                log(" doc_delete (" + counter + "/" + total + ") " + nodeID);
//                            } else {
//                                //apply each tag on each document
//                                List<?> actionELs = element.elements();
//                                for (Iterator<?> iterator2 = actionELs.iterator(); iterator2.hasNext();) {
//                                    Element actionEL = (Element) iterator2.next();
//                                    String name = actionEL.getName();
//                                    if (name.equals("el_rename")) {
//                                        log(" " + name + "(" + counter + "/" + total + ")  " + nodeID);
//                                        el_rename(doc, actionEL);
//                                    } else if (name.equals("el_remove")) {
//                                        log(" " + name + "(" + counter + "/" + total + ")  " + nodeID);
//                                        el_remove(doc, actionEL);
//                                    } else if (name.equals("xsl_transform")) {
//                                        String fileName = Dom4jUtils.getAttributeValue(actionEL, "xsl", "").trim();
//                                        log(" " + name + "(" + counter + "/" + total + ")  " + nodeID + " XSL: " + fileName);
//                                        xsl_transform(doc, actionEL);
//                                    }
//                                }
//                                if (!test_do_not_save) {
//                                    System.out.println("/////////////////// new xml //////////////////////////");
//                                    System.out.println(doc.getRootElement().asXML());
//                                    System.out.println("/////////////////// new xml //////////////////////////");
//                                } else {
//                                    //add the resource
//                                    String nodeID2 = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
//                                    node = new NIDSchemeMDC();
//                                    node.parse(nodeID2);
//                                    if (nodeID2.equals("") || node.getDocUri().equals("")) {
//                                        MRLog.debug("Error in NID is invalid on returned document '" + nodeID2 + "' original NID=" + nodeID);
//                                    } else {
//                                        XMLResource document = (XMLResource) collection.createResource(node.getDocUri(), "XMLResource");
//                                        document.setContent(doc.getRootElement().asXML());
//                                        collection.storeResource(document);
//                                    }
//                                }
//                            }
//                        } catch (Exception ex) {
//                            MRLog.error(query, ex);
//                        } finally {
//                            log(" done (" + counter + "/" + total + ") " + nodeID);
//                        }
//                    }
//                } catch (Exception ex) {
//                    MRLog.error(query, ex);
//                } finally {
//                    try {
//                        root.getMigrationModule().getDBModule().checkIn(rootCollection, false);
//                    } catch (Exception ex) {
//                        MRLog.error(ex);
//                    }
//                }
//            }
//        } catch (Exception ex) {
//            MRLog.error(ex);
//        }
//    }

    private void el_rename(Document doc, Element actionEL) {
        try {
            //<el_rename select="/User/Groups" renameTo="Group" changeParent="/User/Roster/Item"/>

            if (actionEL != null) {
                String removeXpath = Dom4jUtils.getAttributeValue(actionEL, "select", "").trim();
                String renameTo = Dom4jUtils.getAttributeValue(actionEL, "renameTo", "").trim();
                String changeParent = Dom4jUtils.getAttributeValue(actionEL, "changeParent", "").trim();
                if (!removeXpath.equals("")) {
                    Element temp = Dom4jUtils.getElementXpath(doc, removeXpath);
                    if (temp != null) {
                        temp.setName(renameTo);
                        if (!changeParent.equals("")) {
                            Element parent = Dom4jUtils.getElementXpath(doc, changeParent);
                            if (parent == null) {
                                String[] parents = changeParent.split("/");
                                for (int i = 0; i < parents.length; i++) {
                                    Element childEL = null;
                                    String parentName = parents[i];
                                    if (parentName.equals("")) {
                                        throw new Exception("Please provide full path to parent '" + changeParent + "'");
                                    }
                                    if (i == 0) {
                                        childEL = Dom4jUtils.getElementXpath(doc, changeParent);
                                        if (childEL == null) {
                                            throw new Exception("Please provide correct full path to parent '" + changeParent + "'");
                                        }
                                    } else {
                                        childEL = Dom4jUtils.getElementXpath(parent, changeParent);
                                    }
                                    if (childEL == null) {
                                        childEL = Dom4jUtils.addElement(parent, changeParent);
                                    }
                                    parent = childEL;
                                }
                            }
                            if (parent == null) {
                                throw new Exception("Could not find parent '" + changeParent + "'");
                            }
                            Dom4jUtils.addElement(parent, (Element) temp.detach());
                        }
                    }
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void el_remove(Document doc, Element EL) {
        try {
            if (EL != null) {
                //<el_remove select="/User/Roster/Item/Group" />
                String removeXpath = Dom4jUtils.getAttributeValue(EL, "select", "").trim();
                if (!removeXpath.equals("")) {
                    Element temp = Dom4jUtils.getElementXpath(doc, removeXpath);
                    if (temp != null) {
                        temp.detach();
                    }
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        }
    }

    private void xsl_transform(Document doc, Element EL) {
        try {
            if (EL != null) {
                //<xsl_transform xsl="../device/upgrade.xsl"/>
                try {
                    String fileName = Dom4jUtils.getAttributeValue(EL, "xsl", "").trim();
                    boolean relativePath = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(EL, "relativePath", ""), true);
                    File file = new File(fileName);
                    if (relativePath) {
                        String filePath = root.getScriptGlobals().getDBMigrationFolder() + File.separator + fileName;
                        file = new File(filePath);
                    }
                    if (!file.exists()) {
                        throw new Exception("XSL File does not exists  '" + fileName + "'");
                    }

                    String xml = doc.getRootElement().asXML();
                    StringReader srcFile = new StringReader(xml);
                    StringWriter destFile = new StringWriter();
                    Hashtable<String, String> parameters = new Hashtable<String, String>();
                    parameters = root.getScriptGlobals().getList();
                    parameters.put("devname", doc.getRootElement().getName());
                    XsltTransformUtils.transform_Xalan(file.getAbsolutePath(), srcFile, destFile, parameters);
                    String xmlAfter = destFile.toString();
                    Document doc2 = Dom4jUtils.stringToDocument(xmlAfter);
                    doc.getRootElement().detach();
                    doc.add(doc2.getRootElement().detach());
                } catch (Exception ex) {
                    ex.printStackTrace();
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }

/*
  public void doMigrationNative(ScriptVariables variables) throws Exception {
        String query = "";
        String readFrom = Dom4jUtils.getAttributeValue(element, "readFrom", "/*");
        Hashtable<String, Element> listDocuments = new Hashtable<String, Element>();
        log(" start doMigration " + element.asXML());
        IpvsDBDriver db = root.getMigrationModule().getDBAdminApi().getIpvsDBDriver();
        DBDocuments dBDocuments = root.getMigrationModule().getDBDocuments();
        try {
            List<String> list = dBDocuments.getDomainNames();
            for (Iterator<String> domianiterator = list.iterator(); domianiterator.hasNext();) {
                String domainName = domianiterator.next();
                root.getScriptGlobals().setServiceDomain(domainName);
                String collectionName = root.getScriptGlobals().getIpvs_collection();
                try {
                    try {
                        String readFrom2 = readFrom.substring(1);
                        if (readFrom2.indexOf("parentNID") != -1) {
                            readFrom2 = "//" + readFrom2;
                        } else {
                            readFrom2 = "/document[@root='" + readFrom2 + "']";
                        }
                        query = "//collection[@name='" + collectionName + "']" + readFrom2;
                        query = root.getScriptGlobals().modifyQuery(query);
                        listDocuments = dBDocuments.getDocumentELList(collectionName, query);
                        log(listDocuments.size() + " query " + query);
                    } catch (Exception ex) {
                        MRLog.error(query, ex);
                    } finally {
                    }

                    int counter = 0;
                    int total = listDocuments.size();
                    Element delDoc = element.element("doc_delete");
                    if (delDoc != null) {
                        for (String docUri : listDocuments.keySet()) {
                            try {
                                counter++;
                                Element el = listDocuments.get(docUri);
                                if (el != null) {
                                    db.deleteDocument("migration_doc_delete", domainName, 1, docUri, collectionName);
                                    el.detach();
                                }
                            } catch (Exception ex) {
                                MRLog.error(query, ex);
                            } finally {
                                log(" doc_delete (" + counter + "/" + total + ") " + docUri + " " + readFrom);
                            }
                        }
                    } else {
                        for (String docUri : listDocuments.keySet()) {
                            String nodeID = "";
                            counter++;
                            String name = "";
                            try {
                                Element el = listDocuments.get(docUri);
                                if (el != null) {
                                    String xml = db.getDocument("migartion", domainName, 1, docUri, collectionName);
                                    Document doc = Dom4jUtils.stringToDocument(xml);
                                    nodeID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
                                    NIDSchemeMDC node = new NIDSchemeMDC();
                                    node.parse(nodeID);

                                    boolean test_do_not_save = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(element, "test_do_not_save", "true"), true);
                                    //apply each tag on each document
                                    List<?> actionELs = element.elements();
                                    for (Iterator<?> iterator2 = actionELs.iterator(); iterator2.hasNext();) {
                                        Element actionEL = (Element) iterator2.next();
                                        name = actionEL.getName();
                                        if (name.equals("xsl_transform")) {
                                            String fileName = Dom4jUtils.getAttributeValue(actionEL, "xsl", "").trim();
                                            log(" " + name + "(" + counter + "/" + total + ")  " + nodeID + " XSL: " + fileName);
                                            xsl_transform(doc, actionEL);
                                        }
                                    }
                                    if (!test_do_not_save) {
                                        System.out.println("/////////////////// new xml //////////////////////////");
                                        System.out.println(doc.getRootElement().asXML());
                                        System.out.println("/////////////////// new xml //////////////////////////");
                                    } else {
                                        //add the resource
                                        String nodeID2 = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
                                        node = new NIDSchemeMDC();
                                        node.parse(nodeID2);
                                        if (nodeID2.equals("") || node.getDocUri().equals("")) {
                                            MRLog.debug("Error in NID is invalid on returned document '" + nodeID2 + "' original NID=" + nodeID);
                                        } else {
                                            if (!node.getDocUri().equals(docUri)) {
                                                MRLog.debug("Add document '" + nodeID2 + "' original NID=" + nodeID);
                                            }
                                            if (node.getDocUri().equals(docUri)) {
                                                db.deleteDocument("migartion", domainName, 1, node.getDocUri(), collectionName);
                                                db.addDocument("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML());//("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML(), indexName, nodeID2);
                                            } else {
                                                if (dBDocuments.isDocumentExists("migartion", domainName, 1, node.getDocUri(), collectionName)) {
                                                    db.deleteDocument("migartion", domainName, 1, node.getDocUri(), collectionName);
                                                    db.addDocument("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML());//("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML(), indexName, nodeID2);
                                                } else {
                                                    dBDocuments.addDocument(node.getDocUri(), collectionName, doc.getRootElement().getName(), node.getNID(), node.getCollectionNodeID());
                                                    db.addDocument("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML());//("migartion", domainName, 1, node.getDocUri(), collectionName, doc.getRootElement().asXML(), indexName, nodeID2);
                                                }
                                            }
                                        }
                                    }
                                }
                            } catch (Exception ex) {
                                MRLog.error(query, ex);
                            } finally {
                                log(" done " + name + " (" + counter + "/" + total + ") " + nodeID);
                            }
                        }
                    }
                } catch (Exception ex) {
                    MRLog.error(query, ex);
                } finally {
                }
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        }
    }
 
  */

}
