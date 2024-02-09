package com.ipvsserv.nxd.migration.xslt;

import java.io.File;
import java.io.StringReader;
import java.io.StringWriter;
import java.util.ArrayList;
import java.util.Hashtable;
import java.util.Iterator;
import java.util.List;

import org.dom4j.Document;
import org.dom4j.Element;
import org.xmldb.api.base.Collection;
import org.xmldb.api.base.CompiledExpression;
import org.xmldb.api.base.Resource;
import org.xmldb.api.base.ResourceIterator;
import org.xmldb.api.base.ResourceSet;
import org.xmldb.api.base.XMLDBException;
import org.xmldb.api.modules.XMLResource;
import org.xmldb.api.modules.XQueryService;

import com.ipvsserv.common.config.IpvsServGlobals;
import com.ipvsserv.common.exception.DBConnectTimedoutException;
import com.ipvsserv.common.exception.DBExecutionException;
import com.ipvsserv.common.exception.DBNotConnectedException;
import com.ipvsserv.common.log.MRLog;
import com.ipvsserv.common.util.CompUtils;
import com.ipvsserv.common.util.Dom4jUtils;
import com.ipvsserv.nxd.api.xmldb.sedna.SednaXmlDBModule;
import com.ipvsserv.nxd.migration.ScriptVariables;
import com.ipvsserv.nxd.migration.tag.MigrationTagImpl;
import com.ipvsserv.nxd.xslt.XsltTransformUtils;
import com.ipvsserv.server.IpvsServer;

public class ApplyXSLToLiveDBTag extends MigrationTagImpl {
    private File tempFolder;
    private String dbMigrationFolder = "";
    private String ALL_OBJECTS = "all";
    private Hashtable<String, List<String>> objectXsls = null;
    private SednaXmlDBModule dbModule = null;
    public ApplyXSLToLiveDBTag() {
        super("ApplyXSLToLiveDBTag");
    }

    public ApplyXSLToLiveDBTag(String moduleName) {
        super(moduleName);
    }

    @Override
    public void doMigration(ScriptVariables variables) throws Exception {
        dbMigrationFolder = root.getScriptGlobals().getDBMigrationFolder();
        //useXSLLib = Dom4jUtils.value(element, "useXSLLib", "xalan");

        /*
        <xsllist>
            <allXslFromFolder folder=".." />
            <allXslFromFolder folder="xsl" />
            <xsl file="C:/etc/migration/1.0/update_object_version.xsl" includesObjects="" excludesObjects=""/>
        </xsllist>  
        */
        Element xsllistEL = Dom4jUtils.element(element, "xsllist");
        if (xsllistEL == null) {
            MRLog.error("No xsl to apply");
            return;
        }
        objectXsls = new Hashtable<String, List<String>>();

        //get object wise xsl from folder
        List<?> allXslFromFolderList = Dom4jUtils.getList(xsllistEL, "allXslFromFolder");
        for (Iterator<?> iterator = allXslFromFolderList.iterator(); iterator.hasNext();) {
            Element el = (Element) iterator.next();
            String folder = Dom4jUtils.getAttributeValue(el, "folder", "");
            String excludeList = Dom4jUtils.getAttributeValue(el, "excludesFiles", "");
            String includeList = Dom4jUtils.getAttributeValue(el, "includesFiles", "");
            excludeList = getList(excludeList);
            includeList = getList(includeList);
            findObjectXsl_directory(folder, excludeList, includeList);
        }

        //get object wise xsl from file
        List<?> xslList = Dom4jUtils.getList(xsllistEL, "xsl");
        for (Iterator<?> iterator = xslList.iterator(); iterator.hasNext();) {
            Element el = (Element) iterator.next();
            String folder = Dom4jUtils.getAttributeValue(el, "file", "");
            String[] includeList = Dom4jUtils.getAttributeValue(el, "includesObjects", "").split(",");
            findObjectXsl_file(folder, includeList);
        }

        try {
            for (String object : objectXsls.keySet()) {
                System.out.println("object " + object);
                List<String> objectXls = objectXsls.get(object);
                if (objectXls != null) {
                    for (String xsl : objectXls) {
                        System.out.println("object " + object + " " + xsl);
                    }
                }
                applyXSLToAll();
            }
        } catch (Exception ex) {
            MRLog.error(ex);
        } finally {
            log("End  execute task : " + getName());
        }
    }

    private void applyXSLToAll() throws DBNotConnectedException, DBConnectTimedoutException, DBExecutionException, InterruptedException {
        dbModule = (SednaXmlDBModule) IpvsServer.getInstance().getModule(SednaXmlDBModule.class);
        //SednaXmlDBModule dbModule =root.getMigrationModule().getDBModule();
        Collection rootCollection = dbModule.checkOut();
        try {
            String[] childCollectionNames = rootCollection.listChildCollections();
            for (int i = 0; i < childCollectionNames.length; i++) {
                Collection collection = rootCollection.getChildCollection(childCollectionNames[i]);
                String domainName = "";
                domainName = IpvsServGlobals.getDomainDBSchema().getDomainName(collection.getName());
                if (domainName != null) {
                    String[] resourceNames = getResourceList(collection); //collection.listResources();
                    for (int k = 0; k < resourceNames.length; k++) {
                        Resource resource = collection.getResource(resourceNames[k]);
                        try {
                            Document doc = Dom4jUtils.stringToDocument(resource.toString());
                            String objectName = doc.getRootElement().getName();
                            String xml = doc.getRootElement().asXML();
                            List<String> xslFiles = objectXsls.get(ALL_OBJECTS);
                            StringReader srcFile = null;
                            StringWriter destFile = new StringWriter();
                            srcFile = new StringReader(xml);
                            destFile.write(xml);
                            Hashtable<String, String> parameters = new Hashtable<String, String>();
                            parameters = root.getScriptGlobals().getList();
                            if (xslFiles != null && xslFiles.size() > 0) {
                                for (String xsl : xslFiles) {
                                    destFile = new StringWriter();
                                    XsltTransformUtils.transform_Xalan(xsl, srcFile, destFile, parameters);
                                    srcFile = new StringReader(destFile.toString());
                                }

                            }
                            xslFiles = objectXsls.get(objectName);
                            if (xslFiles != null && xslFiles.size() > 0) {
                                for (String xsl : xslFiles) {
                                    destFile = new StringWriter();
                                    XsltTransformUtils.transform_Xalan(xsl, srcFile, destFile, parameters);
                                    srcFile = new StringReader(destFile.toString());
                                }
                            }
                            doc = Dom4jUtils.stringToDocument(destFile.toString());
                            System.out.println("after transform " + doc.asXML());
                            XMLResource document = (XMLResource) collection.createResource(resource.getId(), "XMLResource");
                            document.setContent(xml);
                            collection.storeResource(document);
                            // NID= doc.getRootElement().getName();
                        } catch (Exception ex) {
                            ex.printStackTrace();
                        }
                    }
                }
            }
        } catch (Exception ex) {
            ex.printStackTrace();
        } finally {
            try {
                MRLog.debug("closing rootCollection");
                dbModule.checkIn(rootCollection, false);
            } catch (Exception ex) {
                ex.printStackTrace();
            }
        }
    }

    private String[] getResourceList(Collection collection) throws Exception {
       
        String name = "getResourceList-" + collection.getName();
        String query = getObjectListXquery(collection, collection.getName());
        long t0 = System.currentTimeMillis();
        long tCon = 0;
        tCon = System.currentTimeMillis();
        try {
            if (objectXsls.containsKey(ALL_OBJECTS)) {
                String[] resourceNames = collection.listResources();
                return resourceNames;
            }
            XQueryService service = (XQueryService) dbModule.getServiceManager().getXQueryService(collection);
            ArrayList<String> listNodes = new ArrayList<String>();
            CompiledExpression compiled = service.compile(query);
            ResourceSet result = service.execute(compiled);
            ResourceIterator iterator = result.getIterator();
            while (iterator.hasMoreResources()) {
                Resource resource = iterator.nextResource();
                listNodes.add(CompUtils.removeXmlProlog((String) resource.getContent()));
            }
            String[] resourceNames = new String[listNodes.size()];
            listNodes.toArray(resourceNames);
            return resourceNames;
        } catch (XMLDBException ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } catch (Exception ex) {
            MRLog.error(name + " " + query + " " + ex.getMessage());
            MRLog.error(ex);
            throw new DBExecutionException(name + " " + query + " " + ex.getMessage());
        } finally {
            if (MRLog.isDbsummaryLogEnable()) {
                long t1 = System.currentTimeMillis();
                MRLog.debug("DB: " + (t1 - tCon) + "  GETCON:" + (tCon - t0) + " ms." + name + "");
            }
        }
    }

    private String getObjectListXquery(Collection collection, String collectionName) {
        StringBuffer buf = new StringBuffer();
        buf.append("for $b in collection(\"collectionName\")/*");
        if (!objectXsls.containsKey(ALL_OBJECTS) && objectXsls.size() > 0) {
            StringBuffer buf2 = new StringBuffer();
            boolean badded = false;
            for (String objectname : objectXsls.keySet()) {
                if (badded) {
                    buf2.append(" or ");
                }
                if (!objectname.trim().equals("")) {
                    buf2.append(" ( name()='" + objectname + "' ) ");
                    badded = true;
                }
            }
            if (badded) {
                // buf.a
            }
        }
        buf.append("");
        buf.append("return document-uri(root($b)) ");
        return buf.toString();
    }

    public void transform(String xsl, File srcXml, String useXSLLib) throws Exception {
        try {
            File destFile = File.createTempFile(srcXml.getName(), ".xml", tempFolder);
            MRLog.debug(getName() + " transform XALAN " + xsl + " " + srcXml.getAbsolutePath());
            if (useXSLLib.equalsIgnoreCase("XALAN")) {
                XsltTransformUtils.transform_Xalan(xsl, srcXml, destFile);
            } else {
                XsltTransformUtils.transform_xsltproc(xsl, srcXml, destFile);
            }
            CompUtils.copyfile(destFile, srcXml);
            boolean temp = destFile.delete();
            MRLog.debug("transform  " + xsl + " " + srcXml.getAbsolutePath());

        } catch (Exception ex) {
            srcXml.renameTo(srcXml.getAbsoluteFile());
            MRLog.error(xsl + " " + srcXml.getAbsolutePath(), ex);
            throw ex;
        }

    }

    private void addXslToList(String object, String filePath) {
        List<String> objectXls = objectXsls.get(object);
        if (objectXls == null) {
            objectXls = new ArrayList<String>();
        }
        MRLog.debug(" xsl list " + object + " = " + filePath);
        objectXls.add(filePath);
        objectXsls.put(object, objectXls);
    }

    private String getList(String includes) {
        if (includes.trim().equals("")) {
            return "";
        }
        StringBuffer buf = new StringBuffer(",");
        includes = includes.replace('\n', ' ').trim();
        for (int i = 0; i < includes.length(); i++) {
            char c = includes.charAt(i);
            if (c != ' ') {
                buf.append(c);
            }
        }
        buf.append(",");
        return buf.toString();
    }

    private String parseFileName(String fileName) {
        if (fileName.startsWith("ipvsobject_")) {
            String[] objects = fileName.split("_");
            if (objects.length > 1) {
                String object = objects[1];
                int ipos = object.indexOf('.');
                if (ipos != -1) {
                    object = object.substring(0, ipos);
                }
                return object;
            }
        }
        return ALL_OBJECTS;
    }

    private void findObjectXsl_directory(String folder, String exludeList, String includeList) {
        File directory = new File(dbMigrationFolder + File.separator + folder);
        if (!directory.exists()) {
            directory = new File(folder);
            if (!directory.exists()) {
                MRLog.error("Directory does not exits '" + folder + "' ");
                return;
            }
        }
        File files[] = directory.listFiles();
        for (int i = 0; i < files.length; i++) {
            File xslFile = files[i];
            if (xslFile.isDirectory()) {
                continue;
            }
            String fileName = xslFile.getName();
            if (!fileName.toLowerCase().endsWith(".xsl")) {
                continue;
            }
            boolean bUse = false;
            if (!includeList.equals("") && includeList.indexOf(fileName) != -1) {
                bUse = true;
            } else if (exludeList.indexOf(fileName) == -1) {
                bUse = true;
            }
            if (!bUse) {
                MRLog.debug("Neglect file '" + xslFile.getAbsolutePath() + "'");
                continue;
            }
            String objectName = parseFileName(fileName);
            addXslToList(objectName, xslFile.getAbsolutePath());
        }
    }

    private void findObjectXsl_file(String folder, String[] includesObjectList) {
        File xslFile = new File(dbMigrationFolder + File.separator + folder);
        if (!xslFile.exists()) {
            xslFile = new File(folder);
            if (!xslFile.exists()) {
                MRLog.error("File does not exits '" + folder + "' ");
                return;
            }
        }

        String fileName = xslFile.getName();
        if (includesObjectList != null && includesObjectList.length > 0) {
            for (int i = 0; i < includesObjectList.length; i++) {
                String objectName = includesObjectList[i].trim();
                if (!objectName.equals("")) {
                    addXslToList(objectName, xslFile.getAbsolutePath());
                }
            }
        } else {
            String objectName = parseFileName(fileName);
            addXslToList(objectName, xslFile.getAbsolutePath());
        }
    }
    /* private String getObjectListXquery2(String collectionName){
         StringBuffer buf = new StringBuffer();
         buf.append("let $objects := for $b in collection(\"collectionName\")/*");
         if(!objectXsls.containsKey(ALL_OBJECTS) && objectXsls.size()>0){
             StringBuffer buf2 = new StringBuffer();
             boolean badded=false;
             for (String objectname : objectXsls.keySet()) {
                 if(badded){
                     buf2.append(" or ");
                 }
                 if(!objectname.trim().equals("")){
                     buf2.append(" ( name()='"+objectname+"' ) ");
                     badded=true;
                 }
             }
             if(badded){
                // buf.a
             }
         }
         buf.append("");
         buf.append("return <object name='{$b/name()}'  docuri='{ document-uri(root($b)) }'>");
         buf.append("{$b/@NID} </object>");
         buf.append("return <objects> {$objects} /objects>");
         return buf.toString();
     } */
}
