package com.ipvsserv.nxd.xslt;

import java.io.File;
import java.io.IOException;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.mdc.NIDSchemeMDC;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.adminapi.DBAdminParams;
import com.ipvsserv.nxd.api.xmldb.exist.ExistDBUtils;

public class FileSystemApplyXSLUtils {
    
    private static boolean isSaveToCol (File directory, String collectionName) {
        //import /export and attach detach uses old collection name
        if (IpvsServGlobals.getDomainDBSchema().getDomainName(collectionName) != null || IpvsServGlobals.getDomainDBSchema().getDomainName(directory.getName()) != null) {
            //MRLog.debug("OLD applyXslOnDirectory isSaveToCol " + collectionName +" " +  directory.getAbsolutePath());
            return true;
        }
        
        if(ExistDBUtils.getDomainNameFromFolderName(collectionName) != null || ExistDBUtils.getDomainNameFromFolderName(directory.getName()) != null) {
            //MRLog.debug("New applyXslOnDirectory isSaveToCol " + collectionName +" " +  directory.getAbsolutePath());
            return true;
        }
        //MRLog.error("NONE ???? applyXslOnDirectory isSaveToCol " + collectionName +" " +  directory.getAbsolutePath());
        return  false;
    }
    
    public static void applyXslOnDirectory(File directory, String collectionName, DBAdminParams params, Hashtable<String, ActionsListData> xslMappings, boolean applyServiceVersion, Hashtable<String, String> parameters) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, IOException {
        boolean isRootCollection = collectionName == null || collectionName.trim().equals("");
        boolean saveToCol = isSaveToCol(directory, collectionName);
              
        File[] dirs = directory.listFiles();

        if (saveToCol) {
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (file.isDirectory()) {
                    int total = getFileCount(file);
                    String objectType = file.getName();
                    ActionsListData actionListData = xslMappings.get("objectType." + objectType);
                    if (actionListData != null) {
                        applyActionList(file, objectType, actionListData, saveToCol, total, parameters);
                    } else {
                        actionListData = xslMappings.get("objectType.*");
                        if (actionListData != null) {
                            applyActionList(file, objectType, actionListData, saveToCol, total, parameters);
                        } else if (applyServiceVersion) {
                            actionListData = xslMappings.get("apply_serviceVersion.*");
                            if (actionListData != null) {
                                applyActionList(file, objectType, actionListData, saveToCol, total, parameters);
                            } else {
                                MRLog.debug("NO service version tag ..........");
                            }
                        }
                    }
                } else {
                    Document doc = Dom4jUtils.getDocument(file);
                    int total = 1;
                    String objectType = doc.getRootElement().getName();
                    ActionsListData actionListData = xslMappings.get("objectType." + objectType);
                    if (actionListData != null) {
                        applyActionList(file, objectType, actionListData, saveToCol, total, parameters);
                    } else {
                        actionListData = xslMappings.get("objectType.*");
                        if (actionListData != null) {
                            applyActionList(file, objectType, actionListData, saveToCol, total, parameters);
                        } else if (applyServiceVersion) {
                            actionListData = xslMappings.get("apply_serviceVersion.*");
                            if (actionListData != null) {
                                applyActionList(file, objectType, actionListData, saveToCol, total, parameters);
                            } else {
                                MRLog.debug("NO service version tag ..........");
                            }
                        }
                    }

                }
            }
            return;
        } else {
            for (int i = 0; i < dirs.length; i++) {
                File file = dirs[i];
                if (!file.isDirectory()) {
                    if (isRootCollection) {
                        String docuri = file.getName();
                        if (docuri.equalsIgnoreCase("$db_security_data")) {
                        } else if (docuri.equalsIgnoreCase("service.version")) {
                            ActionsListData actionListData = xslMappings.get("non_objectType.service.version");
                            if (actionListData != null) {
                                applyActionList_non_objecttype(file, actionListData, parameters);
                            }
                        }
                    }
                }
            }
        }
        for (int i = 0; i < dirs.length; i++) {
            File dir = dirs[i];
            if (dir.isDirectory()) {
                String collectionNameChild = dir.getName();
                applyXslOnDirectory(dir, collectionNameChild, params, xslMappings, applyServiceVersion, parameters);

            }
        }
    }

    public static void applyActionList(File file, String objectType, ActionsListData actionListData, boolean saveToCol, int total, Hashtable<String, String> parameters) {
        int index = 0;
        //first delete if it has to be deleted
        boolean isDelete = false;
        String deleteXPath = "";
        for (ActionData actionData : actionListData.actions) {
            if (actionData.isdoc_delete) {
                isDelete = true;
                deleteXPath = actionData.xpath;
            }
        }
        
        if (isDelete) {
            try {               
            	MRLog.debug("Deteting files  from the directory :="+file.getAbsolutePath() + " Object:=" + objectType +  "  and XPath :="+deleteXPath);        
                if (file.isDirectory()) { 
                	MRLog.debug("Deteting all files  " + objectType + "(" + total + ")  " + file.getName());
                    CompUtils.deleteDirectory(file);
                } else {
                	try {
                		if(!deleteXPath.isEmpty() ) {
	                		Document doc = Dom4jUtils.loadXMLFile(file.getAbsolutePath());
	                		Object obj = Dom4jUtils.selectSingleNodeXpath(doc, deleteXPath);
	                		if (obj == null) {
	                			return;
	                		}
                		}
                		file.delete();
                		MRLog.debug("Deteted all files " + objectType + "  " +file.getName());
                	}catch(Exception exp)
                	{
                		MRLog.error("Deleting file failed");
                	}
                    return;
                }
            } catch (Exception e) {
				e.printStackTrace();
			} finally {
                

            }
        } else {
            //apply xsl and save
            if (file.isDirectory()) {
                File[] childirs = file.listFiles();
                for (int k = 0; k < childirs.length; k++) {
                    File child = childirs[k];
                    index++;
                    applyActionList_file(index, child, objectType, actionListData, saveToCol, total, parameters);
                }
            } else {
                index++;
                applyActionList_file(index, file, objectType, actionListData, saveToCol, total, parameters);
            }
        }
    }

    public static void applyActionList_file(int index, File file, String objectType, ActionsListData actionListData, boolean saveToCol, int total, Hashtable<String, String> parameters) {
        if (file.isDirectory()) {
            MRLog.debug("isDirectory ? " + file.getAbsolutePath());
            return;
        }
        String counter_state="(" + index + "/" + total + ") " ;
        if(total==1){
            counter_state=" ";
        }
        String nodeID = "";
        Document doc = null;
        try {
            doc = Dom4jUtils.loadXMLFile(file.getAbsolutePath());
            nodeID = Dom4jUtils.getAttributeValue(doc.getRootElement(), "NID", "");
        } catch (Exception ex) {
            MRLog.debug("ERROR ApplyXSL" + objectType + counter_state +  nodeID + " file: " + file.getName());
            MRLog.error(file.getAbsolutePath(), ex);
            return;
        }

        for (ActionData actionData : actionListData.actions) {
            try {

                if (actionData.xpath != null && !actionData.xpath.equals("")) {
                    Object obj = Dom4jUtils.selectSingleNodeXpath(doc, actionData.xpath);
                    if (obj == null) {
                        continue;
                    }
                }
                if ("newdoc".equalsIgnoreCase(actionData.resultType)) {
                    try {
                        String xmlFile = xsl_transform(doc, actionData.xsl, parameters);
                        //save new document
                        Document newDoc = Dom4jUtils.stringToDocument(xmlFile);
                        String nodeID_after = Dom4jUtils.getAttributeValue(newDoc.getRootElement(), "NID", "");
                        File file_save = null;
                        if (saveToCol) {
                            NIDSchemeMDC node = new NIDSchemeMDC();
                            node.parse(nodeID_after);
                            if(!"".equals(actionData.newDocDir)) {
                                 File parentDir = file.getParentFile().getParentFile();
                             	 File file_save_folder = new File(parentDir,actionData.newDocDir);
 	                             file_save_folder.mkdirs();
 	                             MRLog.debug("save new file in dir :="+actionData.newDocDir);
 	                             file_save = new File(file_save_folder, node.getDocUri());  
                            } else {
	                            File file_save_folder = new File(file.getParentFile(), newDoc.getRootElement().getName());
	                            file_save_folder.mkdirs();
	                            file_save = new File(file_save_folder, node.getDocUri());
                            }
                            
                            MRLog.debug("new file save to..." + file_save.getAbsolutePath());
                            Dom4jUtils.saveDocument(file_save.getAbsolutePath(), newDoc);
                            MRLog.debug("ApplyXSL " + objectType + counter_state + nodeID + " XSL: " + actionData.xsl + " file: " + file.getName());
                        } else {
                            MRLog.debug("new file but does not know where to save...");
                        }
                    } catch (Exception ex) {
                        MRLog.debug("Error NEW_DOC ApplyXSL " + objectType + counter_state + nodeID + " XSL: " + actionData.xsl + " file: " + file.getName());
                    }
                } else {
                    try {
                        String xmlFile = xsl_transform(doc, actionData.xsl, parameters);
                        doc = Dom4jUtils.stringToDocument(xmlFile);
                        MRLog.debug("ApplyXSL  " + objectType + counter_state + nodeID + " XSL: " + actionData.xsl + " file: " + file.getName());
                    } catch (Exception ex) {
                        MRLog.debug("Error  ApplyXSL " + objectType + "(" + index + "/" + total + ")  " + nodeID + " XSL: " + actionData.xsl + " file: " + file.getName());
                    }
                }
            } catch (Exception ex) {
                MRLog.debug("Error ApplyXSL " + objectType + counter_state + nodeID + " XSL: " + actionData.xsl + " file: " + file.getName());
            }
        }

        try {
            Dom4jUtils.saveDocument(file.getAbsolutePath(), doc);
        } catch (Exception ex) {
            MRLog.debug("ERROR Save " + objectType + counter_state + nodeID + " file: " + file.getName());
            MRLog.error(file.getAbsolutePath(), ex);
        }
    }

    public static void applyActionList_non_objecttype(File child, ActionsListData actionListData, Hashtable<String, String> parameters) {
        //first delete if it has to be deleted
        boolean isDelete = false;
        for (ActionData actionData : actionListData.actions) {
            if (actionData.isdoc_delete) {
                isDelete = true;
            }
        }

        if (isDelete) {
            child.delete();
            MRLog.debug("Detete non_objectType   " + child.getName());
            return;
        }

        String nodeID = child.getName();
        Document doc = null;
        try {
            doc = Dom4jUtils.loadXMLFile(child.getAbsolutePath());
        } catch (Exception ex) {
            MRLog.debug("ERROR ApplyXSL non_objectType " + nodeID + " file: " + child.getName());
            MRLog.error(child.getAbsolutePath(), ex);
            return;
        }

        for (ActionData actionData : actionListData.actions) {
            try {
                if (actionData.xpath != null && !actionData.xpath.equals("")) {
                    Object obj = Dom4jUtils.selectSingleNodeXpath(doc, actionData.xpath);
                    if (obj == null) {
                        continue;
                    }
                }
                if ("newdoc".equalsIgnoreCase(actionData.resultType)) {
                    //donothing
                } else {
                    try {
                        String xmlFile = xsl_transform(doc, actionData.xsl, parameters);
                        doc = Dom4jUtils.stringToDocument(xmlFile);
                        MRLog.debug("ApplyXSL  non_objectType XSL: " + actionData.xsl + " file: " + child.getName());
                    } catch (Exception ex) {
                        MRLog.debug("ERROR ApplyXSL  non_objectType XSL: " + actionData.xsl + " file: " + child.getName());
                    }
                }
            } catch (Exception ex) {
                MRLog.debug("ERROR ApplyXSL  non_objectType XSL: " + actionData.xsl + " file: " + child.getName());
            }
        }

        try {
            Dom4jUtils.saveDocument(child.getAbsolutePath(), doc);
        } catch (Exception ex) {
            MRLog.debug("ERROR Save  non_objectType  file: " + child.getName());
            MRLog.error(child.getAbsolutePath(), ex);
        }

    }

    public static int getFileCount(File directory) throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, IOException {
        int total = 0;
        File[] dirs = directory.listFiles();
        for (int i = 0; i < dirs.length; i++) {
            File file = dirs[i];
            if (!file.isDirectory()) {
                total++;
            } else {
                getFileCount(file);
            }
        }
        return total;
    }

    public static String xsl_transform(Document doc, String filePath, Hashtable<String, String> parameters) throws Exception {
        StringWriter destFile = new StringWriter();
        File file = new File(filePath);
        String xml = doc.getRootElement().asXML();
        StringReader srcFile = new StringReader(xml);
        parameters.put("devname", doc.getRootElement().getName());
        XsltTransformUtils.transform_Xalan(file.getAbsolutePath(), srcFile, destFile, parameters);
        return destFile.toString();
    }

    public static class ActionsListData {
        public Element mapEL = null;
        public String type = null;
        public String type_name = null;
        public String name = null;
        public List<ActionData> actions = new ArrayList<ActionData>();

        @SuppressWarnings("unchecked")
        public ActionsListData(Element mapEL, String migrationFolder) throws Exception {
            this.mapEL = mapEL;
            type = Dom4jUtils.getAttributeValue(mapEL, "type", "objectType");
            type_name = Dom4jUtils.getAttributeValue(mapEL, type, "");
            name = type + "." + type_name;
            List<Element> actionsELList = (List<Element>) Dom4jUtils.getList(mapEL);
            for (Element actionsEL : actionsELList) {
                actions.add(new ActionData(actionsEL, migrationFolder, name));
            }
        }

    }

    public static class ActionData {
        Element actionsEL = null;
        String name = null;
        String xsl = null;
        String xpath = null;
        String resultType = null;
        String newDocDir = null;
        boolean isdoc_delete = false;

        //private StreamSource xslStreamSource=null;
        // <xsl_transform xsl="../../shared/xsl/_updateServiceVersionXml.xsl" />
        //<doc_delete/>
        public ActionData(Element actionsEL, String migrationFolder, String nameParent) throws Exception {
            this.actionsEL = actionsEL;
            name = actionsEL.getName();
            if (name.equals("xsl_transform")) {
                xpath = Dom4jUtils.getAttributeValue(actionsEL, "xpath", "");
                resultType = Dom4jUtils.getAttributeValue(actionsEL, "resultType", "");
                newDocDir = Dom4jUtils.getAttributeValue(actionsEL, "newDocDir", "");
                String fileName = Dom4jUtils.getAttributeValue(actionsEL, "xsl", "").trim();
                boolean relativePath = CompUtils.booleanValue(Dom4jUtils.getAttributeValue(actionsEL, "relativePath", ""), true);
                File file = new File(fileName);
                if (relativePath) {
                    String filePath = migrationFolder + File.separator + fileName;
                    file = new File(filePath);
                    if (MRLog.isDbadminLogEnable()) {
                        MRLog.debug("DB_ADMIN: XSL File Mapping ApplyXslFileSystem " + nameParent + "  xsl: " + filePath);
                    }
                }
                if (!file.exists()) {
                    throw new Exception("XSL File does not exists  '" + fileName + "'");
                }
                xsl = file.getAbsolutePath();
            } else if (name.equals("doc_delete")) {
                MRLog.debug("DB_ADMIN: ApplyXslFileSystem " + nameParent + " doc_delete  ");
                isdoc_delete = true;
                xpath = Dom4jUtils.getAttributeValue(actionsEL, "xpath", "");
            }
        }

    }

}
